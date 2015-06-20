#!/usr/bin/env python
#encoding:utf8
'''
Created on 2013-8-17

@author: robin1001
'''
import roslib; roslib.load_manifest('XM_WinServer'); roslib.load_manifest('XM_msgs');
import rospy
from std_msgs.msg import String 
from XM_msgs.srv import *
from geometry_msgs.msg import Twist

import socket
import threading
import struct
import sys
'''
deal with all the conmunication to windows
0x01.speech recognize data

kinect and face sdk using c/s request/respond mode
''' 
class XM_WinServer:
    #WIN_IP = '127.0.0.1'
    WIN_IP = '10.12.1.94'
    SELF_PORT = 10000 
    TTS_PORT = 10001
    FACE_PORT = 10002
    KINECT_PORT = 10003 
    FOLLOW_PORT = 10004 
    def __init__(self):
    	#init params
    	XM_WinServer.WIN_IP = rospy.get_param("~win_ip")
    	XM_WinServer.SELF_PORT = rospy.get_param("~self_port")
    	XM_WinServer.TTS_PORT = rospy.get_param("~tts_port")
    	XM_WinServer.FACE_PORT = rospy.get_param("~face_port")
    	XM_WinServer.KINECT_PORT = rospy.get_param("~kinect_port")
    	XM_WinServer.FOLLOW_PORT = rospy.get_param("~follow_port")

        self.tts_sub = rospy.Subscriber('speech', String, self.speech_callback)
	self.twist_pub = rospy.Publisher('cmd_vel', Twist)
        self.kinect_pub = rospy.Publisher('kinect_data', String)
        self.sr_pub = rospy.Publisher('sr_data', String)
        self.face_srv = rospy.Service('face_srv',XM_FaceDetect, self.face_srv_handle)
        self.kinect_srv = rospy.Service('kinect_srv', XM_Kinect, self.kinect_srv_handle)
        self.follow_srv = rospy.Service('follow_srv', Follow, self.follow_srv_handle) 
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind(('', XM_WinServer.SELF_PORT))
    
    def start(self):
        print 'xm_winserver start...'
        print 'tts server info %s:%d' % (XM_WinServer.WIN_IP, XM_WinServer.TTS_PORT)
        print 'face server info %s:%d' % (XM_WinServer.WIN_IP, XM_WinServer.FACE_PORT)
        print 'kinect server info %s:%d' % (XM_WinServer.WIN_IP, XM_WinServer.KINECT_PORT)
        print 'follow server info %s:%d' % (XM_WinServer.WIN_IP, XM_WinServer.FOLLOW_PORT)
        #start win server
        t=threading.Thread(target = self.start_winserver)
        t.setDaemon(True)
        t.start()
        #start ros loop
        rospy.spin()
        
    def start_winserver(self):
        self.server.listen(10)
        while not rospy.is_shutdown():
            sock, addr = self.server.accept()
            t = threading.Thread(target=self.handle_connect, args=(sock, addr))
            t.setDaemon(True)
            t.start()
                       
    def handle_connect(self, sock, addr):
        cmd, length = struct.unpack('!bi', self.recv_len(sock, 5))
        data = self.recv_len(sock, length)
        if cmd == 0x01:
	    msg = data.decode('gbk').encode('utf8')
            print 'new speech recognize result arrived:', msg
            while self.sr_pub.get_num_connections() == 0:
                pass
            self.sr_pub.publish(String(msg))
            
	elif cmd == 0x02:
	    '''
	    用于非限定性项目，底盘和机械臂的交互
	    1. 0x01:机械臂			2. 0x02:底盘
		0x01:空间坐标				0x00:stop
		0x02:机械臂夹持				0x01:forward
		0x03:松开				0x02:back
							0x03:turn left
							0x04:turn right
	    '''
	    device, action = struct.unpack('!2b', data[0:2])
	    twist = Twist()
	    if device == 0x01:
		pass
	    elif device == 0x02:
		if action == 0x00:
		    pass
		elif action == 0x01 or action == 0x02:
		    twist.linear.x = 0.2 if action == 0x01 else -0.2
		elif action == 0x03 or action == 0x04:
		    twist.angular.z = 0.1 if action == 0x03 else -0.1
	    self.twist_pub.publish(twist)

    def speech_callback(self, data):
	speech = data.data.decode('utf8').encode('gbk')
        print 'new speech cmd arrived:', data.data
        self._send(XM_WinServer.WIN_IP, XM_WinServer.TTS_PORT, speech)
    
    def face_srv_handle(self, request):
        print 'new face request arrived:', request.command
        if request.command == 'record':
            name = request.name.decode('utf8').encode('gbk')
            data = struct.pack('!bi', 0x01, len(name))
            res = self._send_wait(XM_WinServer.WIN_IP, XM_WinServer.FACE_PORT, data + name)
            response = XM_FaceDetectResponse()
            response.back_type, = struct.unpack('!b', res[0])
            response.status = "ok" 
            return response
            
        elif request.command == 'recognize':
            data = struct.pack('!b', 0x02)
            res = self._send_wait(XM_WinServer.WIN_IP, XM_WinServer.FACE_PORT, data)
            response = XM_FaceDetectResponse()
            response.status = "ok"
            '''
	    0x01 no person
            0x02 people record
            0x03 people not record
            '''
            response.back_type, = struct.unpack('!b', res[0])
            #1:4 length
            response.name = res[5:].decode('gbk').encode('utf8')
            print response.name
            return response

    def kinect_srv_handle(self, request):	
        print 'new kinect request arrived'
	#目前仅一种获取有无人的指令，发不发（直接connect）命令都行，为了接口统一，发一个0x01
	if request.cmd_num == 0x01:
	    data = struct.pack('!b', 0x01)
	    res = self._send_wait(XM_WinServer.WIN_IP, XM_WinServer.KINECT_PORT, data)
	    #无人0 有人其他 1
	    response = XM_KinectResponse()
	    response.has_person, = struct.unpack('!b', res[0])
	    if response.has_person:
		response.x, response.y, response.z = struct.unpack('3f', res[1:])
	    return response
	else:
	    data = struct.pack('!bb',0x02,request.cmd)
	    res = self._send_wait(XM_WinServer.WIN_IP, XM_WinServer.KINECT_PORT, data)
	    response = XM_KinectResponse()
	    response_tag, = struct.unpack('!b',res[0])
	    response.status, =struct.unpack('!b',res[1])
	    return response

    def follow_srv_handle(self, request):
        print 'new follow request arrived', request.cmd_type
        data = struct.pack('!b', request.cmd_type)
        res = self._send_wait(XM_WinServer.WIN_IP, XM_WinServer.FOLLOW_PORT, data)
        response = FollowResponse()
        cmd_type, response.status = struct.unpack('!2b', res[0:2])
        #回复类型为0x02（获取位置）， 并且获取位置成功
        if cmd_type == 0x02 and response.status == True:
            response.x, response.y, response.z = struct.unpack('3f', res[2:])
        return response
        
    def recv_len(self, sock, length):
        data = []
        left = length
        while left != 0:
            tmp = sock.recv(left)
            data.append(tmp)
            left -= len(tmp)
        return ''.join(data)    
    
    
    def _send_helper(self, ip, port, data):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sock.connect((ip, port))
            data_len = struct.pack('!i', len(data))
            sock.sendall(data_len + data)
        except socket.error, ex:
            print socket.error, ex
        return sock

    def _send(self, ip, port, data):
        self._send_helper(ip, port, data).close()
	
    #send and wait return data
    def _send_wait(self, ip, port, data):
    	sock = self._send_helper(ip, port, data)
    	length, = struct.unpack('!i', self.recv_len(sock, 4))
        data = self.recv_len(sock, length)
        sock.close()
        return data
        
if __name__ == '__main__':
    rospy.init_node('xm_winserver')
    XM_WinServer().start()
    