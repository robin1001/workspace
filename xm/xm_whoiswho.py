#!/usr/bin/env python
#encoding:utf8
'''
Created on 2013-8-27

@author: robin1001 & Luke 
'''
import roslib
roslib.load_manifest('XM_WhoIsWho')
roslib.load_manifest('XM_msgs')
import rospy
import threading
from math import *
import time
import actionlib

from std_msgs.msg import String 
from XM_msgs.srv import *
from geometry_msgs.msg import PoseStamped
from move_base_msgs.msg import *
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import tf

class State:
    READY = 0x01
    RECORD = 0x02
    RECOGNIZE = 0x03
    LEAVE = 0x04

class Person:
    x, y, radius = 0, 0, 0
	
class Mypose:
    x, y, theta = 0, 0, 0
    frame_id = ''
	
class WhoIsWho:
    def __init__(self):
	self.speech_dic = {}
	self.sr_data = ''
	self.face_data = None
	self.new_person = 0
	self.old_person = 0
	#varible to calculate the theta
	self.old_person_std = 2
	self.new_person_std = 1
	self.theta = 0.0
	self.person_tag = False #got a person in front of robot or not
	self.pose_tag = False #at the center pose or not
	self.listener = tf.TransformListener()
	self.state = State.READY#State.READY	
	self.lock = threading.Lock()
	#state 0:no goal set 	state 1:goal running 	state 2:goal reached
        self.tts_pub = rospy.Publisher('speech', String)
        self.vel_pub = rospy.Publisher('cmd_vel',Twist)
        self.sr_sub = rospy.Subscriber('sr_data', String, self.sr_callback)
	self.load_speech_cmd()
	self.load_params()
	self.wait_service()
	self.person_p = []
	
    def load_params(self):
	self.start_pose = Mypose()
	self.start_pose.x = rospy.get_param('~start_x')
	self.start_pose.y = rospy.get_param('~start_y')
	self.start_pose.frame_id = 'map'
	self.center_pose = Mypose()
	self.center_pose.x = rospy.get_param('~center_x')
	self.center_pose.y = rospy.get_param('~center_y')
	self.center_pose.frame_id = 'map'
	self.end_pose = Mypose()
	self.end_pose.x = rospy.get_param('~end_x')
	self.end_pose.y = rospy.get_param('~end_y')
	self.end_pose.frame_id = 'map'
	self.pose_tol = rospy.get_param('~tolerence')
	self.rotate_vel = rospy.get_param('~velocity')
	self.dis = rospy.get_param('~distance')
	self.out_judge = rospy.get_param('~out')
	self.new_person_std = rospy.get_param('~new_person')
	self.old_person_std = rospy.get_param('~old_person')
	
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

    def _pub_tts(self, tts_key, name = ''):
        msg = self.speech_dic[tts_key]
        print 'state:'+ msg
	self.tts_pub.publish(String(msg+name))
	time.sleep(3.0)
	
    def wait_service(self):
        print 'wait for service:kinect, face...'
        rospy.wait_for_service('kinect_srv')
        rospy.wait_for_service('face_srv')
        self.kinect_request = rospy.ServiceProxy('kinect_srv', XM_Kinect)
        self.face_request = rospy.ServiceProxy('face_srv', XM_FaceDetect)
        print 'all service ready'

    def sr_callback(self, data):
	print 'new recognize speech arrived:',  data.data
	with self.lock:
	    self.sr_data = data.data
	    	    
    def set_goal(self,data):
	client = actionlib.SimpleActionClient('move_base',MoveBaseAction)
	client.wait_for_server()
	goal = MoveBaseGoal()
	goal.target_pose.header.frame_id = data.frame_id
	goal.target_pose.header.stamp = rospy.Time.now()
    
	goal.target_pose.pose.position.x = data.x
	goal.target_pose.pose.position.y = data.y
	goal.target_pose.pose.orientation.w = cos(data.theta/2)
	goal.target_pose.pose.orientation.z = sin(data.theta/2)

	client.send_goal(goal)
	client.wait_for_result()
	return client.get_state()
    
    def check_pose(self, person):
	print 'check pose start'
	for member in self.person_p:
	    #print member.x , member.y 
	    if member < self.pose_tol + person and member > person - self.pose_tol:
		return False
	return True
	
    def set_pose(self, pose):
	#find a correct place to go 
	goal = Mypose()
	goal.theta = pose.theta
	goal.frame_id = pose.frame_id
	far = (pose.x**2+pose.y**2)**0.5
	scale = (far - self.dis)/far
	goal.x = pose.x * scale
	goal.y = pose.y * scale
	return goal
	
    def get_theta(self):
	tag = False
	while not tag:
	    tag = True
	    try:
		self.listener.waitForTransform('map','base_link',rospy.Time.now(),rospy.Duration(2.0))
		#tf.Exception: Unable to lookup transform, cache is empty, when looking up transform from frame [/base_link] to frame [/map]这尼玛。。。。非得要类变量才不抛异常
		local_pose, global_pose = self.listener.lookupTransform('map','base_link',rospy.Time(0))
	    except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
		tag = False
	return 2*acos(global_pose[3])
	
    def get_person(self,z,x):
	self.stop()
	pose_ = Mypose()
	pose_.x = z
	pose_.y = x
	dis = (pose_.x**2+pose_.y**2)**0.5
	pose_.frame_id = 'base_link'
	pose_.theta = acos(pose_.x/dis)
	if pose_.y < 0.0 :
	    pose_.theta = - pose_.theta
	#kinect find the person outside the field
	if self.out_judge < dis :
	    pose_tag = True
	    return 
	theta = self.get_theta()+pose_.theta
	if (self.check_pose(theta)):
	    print 'new avaliable pose arrived'
	    self.stop()
	    print theta
	    if self.set_goal(self.set_pose(pose_)) == 3 :
		self.pose_tag = False
		self.person_tag = True
		self.person_p.append(theta)
	    return theta
	else:
	    self.rotate()
	    return theta
	    
    def stop(self):
	twist = Twist()
	self.vel_pub.publish(twist)
	
    def rotate(self):
	twist = Twist()
	twist.angular.z = self.rotate_vel
	self.vel_pub.publish(twist)
	time.sleep(2.0)
	
    def start(self):
        print 'who is who start...'  
        sum_timer = time.time()
        num = 0
        #test the time limit
	while not rospy.is_shutdown():
	    with self.lock:
		if self.state == State.READY:
		    while self.set_goal(self.start_pose)!=3:
			pass
		    self.state = State.RECORD
		
		elif self.state == State.RECORD:
		    if num == 2:
			time.sleep(3.0)
			self.state = State.RECOGNIZE
			#navigate to the center of other room
			#self.set_goal(self.center_pose)
			
		    #if start with'我是' and end with '请记录我'
		    start_str = self.speech_dic['RECORD_START_STR']
		    end_str = self.speech_dic['RECORD_END_STR']			
		    if self.sr_data.startswith(start_str) and self.sr_data.endswith(end_str):
			self._pub_tts('RECORD_RESPOND')
			#name
			name = self.sr_data[len(start_str) :(len(self.sr_data) - len(end_str)) ]
			print 'record name:' + name
			#record face service, respond detail in xm_winserver.py
			respond = self.face_request('record', name)
			while not respond.back_type == 0x00:
			    respond = self.face_request('record', name)#record success
			    self._pub_tts('RECOGNIZE_FACEME')
			    time.sleep(3)
			self._pub_tts('RECORD_SUCCESS')
			self._pub_tts('RECORD_ACK',name)
			num = num + 1
			
		elif self.state == State.RECOGNIZE:
		    if not self.pose_tag:
			self.set_goal(self.center_pose)
			self.pose_tag = True
			self.person_tag = False
		    #get kinect and face's data
		    #rotate
		    theta = self.center_pose.theta
		    respond_kinect = self.kinect_request(1,0)
		    respond_face = self.face_request('recognize','')
		    if not respond_kinect.has_person and respond_face.back_type == 0x01:
			self.rotate()
		    #detect person
		    if respond_kinect.has_person == True:
			#deal with kinect and map axis transform
			theta = self.get_person(respond_kinect.z,respond_kinect.x)
			
		    if respond_kinect.has_person == False: 
			if respond_face.back_type == 0x02:
			    self.stop()
			    theta = self.get_theta()
			    if (self.check_pose(theta)):
				self._pub_tts('RECOGNIZE_SUCCESS',respond_face.name)
				print 'RECOGNIZE_SUCCESS', respond_face.name
				self.person_p.append(theta)
			    self.rotate()
			    
			elif respond_face.back_type == 0x03:
			    #stop and move the kinect to detect the person
			    self.stop()
			    for i in range(1,2+1):
				move = self.kinect_request(2,i)
				respond_kinect_ = self.kinect_request(1,0)
				if respond_kinect_.has_person == True:
				    theta = self.get_person(respond_kinect_.z,respond_kinect_.x)
				    break
				else:
				    move_back = self.kinect_request(2,0)
				self.rotate()
			#arrive the person's place
		    if self.person_tag :
			respond_ = self.face_request('recognize','')
			if respond_.back_type == 0x01:
			    self._pub_tts('RECOGNIZE_FACEME')
			    print 'RECOGNIZE_FACEME'
			    time.sleep(1.0)
			    for i in range(1,2+1):
				move = self.kinect_request(2,i)
				respond_ = self.face_request('recognize','')
				if respond_.back_type != 0x01:
				    break
			#recorded guest
			if respond_.back_type == 0x02:
			    self._pub_tts('RECOGNIZE_SUCCESS',respond_.name)
			    self.new_person = self.new_person + 1
			    print 'RECOGNIZE_SUCCESS'
			elif respond_.back_type ==0x03 :
			    self._pub_tts('RECOGNIZE_FAILED')
			    self.old_person = self.old_person + 1
			    print 'RECOGNIZE_FAILED'
			else :
			    #if we got more than one face in the camera , we want to ignore it
			    print 'too much person'
			move_back = self.kinect_request(2,0)
		    #back to center of the room
		    self.center_pose.theta = theta
		    print 'theta: ',self.center_pose.theta
		    #rotate angle more than 2 leave
			
		elif self.state == State.LEAVE:
		    while self.set_goal(self.end_pose)!=3 :
			pass
		    print 'Yes I can!!!'
		    exit()
		    
		self.sr_data = ''
		#calculate the time to decide when to leave
		'''if self.new_person == self.new_person_std and self.old_person == self.old_person_std:
		    self.state = State.LEAVE
		    print 'all person checked '''
		if time.time() - sum_timer > 7*60 : 
		    self.state = State.LEAVE
		    print 'time out '	
		    
if __name__ == '__main__':
    rospy.init_node('xm_whoiswho')
    WhoIsWho().start()
    


