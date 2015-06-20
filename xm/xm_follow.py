#!/usr/bin/env python
#encoding:utf8
'''
Created on 2013-9-21

@author: robin1001
'''
import roslib
roslib.load_manifest('XM_Follow')
roslib.load_manifest('XM_msgs')
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from XM_msgs.srv import *

import threading
import math
import time

class State:
    IDLE = 0x01
    RECORD = 0x02
    WAIT_FOLLOW = 0x03
    FOLLOW = 0x04
    WAIT_10 = 0x05
    HOLD_ON = 0x06
    LOST = 0x07
    
def sgn(data):
    if data>0 :
	return 1
    else :
	return -1

class XM_Follow:
    def __init__(self):
	self.speech_dic = {}
	self.sr_data = ''
	self.state = State.IDLE	
	self.lock = threading.Lock()
        self.tts_pub = rospy.Publisher('speech', String)
        self.sr_sub = rospy.Subscriber('sr_data', String, self.sr_callback)
        self.twist_pub = rospy.Publisher('cmd_vel', Twist)
	self.load_speech_cmd()
	self.load_params()
	self.wait_service()
	
    def load_speech_cmd(self):
        path = rospy.get_param('~speech_file')	
	print 'loading conversation file:%s' % path
	#parse conversation cmd
	with open(path, 'r') as fid:
	    for line in fid.readlines():
                line = line[:-1]#remove '\n'
		print line
		if line.startswith('#') or line == '':
                    continue
		words = line.split(':')
		self.speech_dic[words[0]] = words[1]

    def load_params(self):
        self.STD_DIST = rospy.get_param('~std_dist')
        self.STD_ERROR = rospy.get_param('~std_error')
        self.x_base_vel = rospy.get_param('~x_base_vel')
        self.y_base_vel = rospy.get_param('~y_base_vel')
        self.rotate_vel = rospy.get_param('~rotate_vel')
        self.move_ratio = rospy.get_param('~move_ratio')
        self.timeout = rospy.get_param('~timeout')
        print 'load params ok'
        
    def wait_service(self):
        print 'wait for [follow] service'
        rospy.wait_for_service('follow_srv')
        self.follow_request = rospy.ServiceProxy('follow_srv', Follow)
        print 'all service ready'
        print ''

    def sr_callback(self, data):
        msg = data.data
	print 'new recognize speech arrived:', msg
	with self.lock:
	    self.sr_data = msg

    def _pub_tts(self, tts_key):
        msg = self.speech_dic[tts_key]
        print 'state:'+ msg
	self.tts_pub.publish(String(msg))
	time.sleep(3)
	
    def handle_follow(self, point):
        x, y, z = point
        #行走策略
        twist = Twist()
        if math.fabs(x) > self.STD_ERROR:
            '''旋转'''
            angle = math.atan2(x, z)
            twist.angular.z =  sgn(angle) * self.rotate_vel
            '''左右'''
            vy = sgn(x) * self.y_base_vel
            twist.linear.y = vy
  
        error = z - self.STD_DIST 
        if math.fabs(error) > self.STD_ERROR:#前后
            vx  = sgn(error) * self.x_base_vel * self.move_ratio
            twist.linear.x = vx
        print twist
        self.twist_pub.publish(twist)
        
    def start(self):
        print 'follow start...'
        
	while not rospy.is_shutdown():
	    with self.lock:
		if self.state == State.IDLE:
		    if self.sr_data == self.speech_dic['CMD_RECORD']:
			self._pub_tts('CMD_RECORD_RESPOND')
			self.state = State.RECORD
			
		elif self.state == State.RECORD:
		    respond = self.follow_request(0x01)
		    if respond.status == True:#记录成功
			self._pub_tts('CMD_RECORD_SUCCESS')
			self.state = State.WAIT_FOLLOW
		    else:
			print 'record failed'

		elif self.state == State.WAIT_FOLLOW:
		    if self.sr_data == self.speech_dic['CMD_FOLLOW']:
			self._pub_tts('CMD_FOLLOW_RESPOND')
			self.s = time.time()
			self.state = State.FOLLOW
			
		elif self.state == State.FOLLOW:
		    if self.sr_data == self.speech_dic['CMD_WAIT_TENS']:
			for i in range(100):
			    self.twist_pub.publish(Twist())
			self._pub_tts('CMD_WAIT_TENS_RESPOND')
			self.state = State.WAIT_10
		    elif self.sr_data == self.speech_dic['CMD_HOLD_ON']:
			for i in range(100):
			    self.twist_pub.publish(Twist())
			self._pub_tts('CMD_HOLD_ON_RESPOND')
			self.state = State.HOLD_ON
		    elif self.sr_data == '':#即没有任何指令
			respond = self.follow_request(0x02)
			if respond.status:#返回位置成功
			    print respond
			    self.handle_follow((respond.x, respond.y, respond.z))
			    #refresh the player exist time
			    self.s = time.time()
			else:#没人
			    self.twist_pub.publish(Twist())
			    #lost player
			    if time.time() - self.s > self.timeout:
				self.state = State.LOST
				self._pub_tts('CMD_LOST_PLAYER')
				print 'lost player'
			time.sleep(0.1)
				
		elif self.state == State.WAIT_10:
		    t = time.time()
		    while time.time() - t < 10:
			self.twist_pub.publish(Twist())
			time.sleep(0.1)
		    self._pub_tts('CMD_TENS_OVER')
		    t = time.time()
		    while time.time() - t < 10:
			twist = Twist()
			twist.linear.x = 0.1
			self.twist_pub.publish(twist)
			respond = self.follow_request(0x02)
			if respond.status:#返回位置成功
			    self.state = State.FOLLOW
			    break
			time.sleep(0.01)
		    self.state = State.FOLLOW
		
		elif self.state == State.HOLD_ON:
		    #等待人消失后进入
		    time.sleep(8)
		    while True:
			respond = self.follow_request(0x02)
			if respond.status:#成功找到player
			    if respond.x > 0:
				self._pub_tts('CMD_PLAYER_LEFT')
			    else:
				self._pub_tts('CMD_PLAYER_RIGHT')
			    break
		    self.state = State.FOLLOW
		    
		elif self.state == State.LOST:
		    respond = self.follow_request(0x02)
		    #重新找到player
		    if respond.status:
			self.twist_pub.publish(Twist())
			self.state = State.FOLLOW
			self._pub_tts('CMD_REFIND_PLAYER')
			print 'refind player'
		    twist = Twist()
		    twist.angular.z = 0.2
		    self.twist_pub.publish(twist)
		    
		self.sr_data = ''
		#end lock
            
if __name__ == '__main__':
    rospy.init_node('xm_follow')
    XM_Follow().start()
    


