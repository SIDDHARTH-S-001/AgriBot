#!/usr/bin/env python

import rospy
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint

rospy.init_node('joint_angle_publisher')

pub = rospy.Publisher('/arm_controller/command', JointTrajectory, queue_size=10)
rate = rospy.Rate(10)
traj = JointTrajectory()
traj.header.stamp = rospy.Time.now()
traj.header.frame_id = ''

joint_names = ['base_twist', 'shoulder', 'elbow', 'wrist']

while not rospy.is_shutdown():
    
    traj.joint_names = joint_names
    
    point = JointTrajectoryPoint()
    point.positions = [0.0, 1.0, 0.5, 0.0, 0.0]  # Set the joint angles here
    point.time_from_start = rospy.Duration(1.0)  # Set the time duration here
    
    traj.points.append(point)
    rospy.loginfo(traj)
    
    pub.publish(traj)
    rate.sleep()
