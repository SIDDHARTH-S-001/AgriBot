#!/usr/bin/env python3
import rospy
import tf
from sensor_msgs.msg import JointState

def joint_states_callback(data):
    # Get the index of the desired link in the joint names list
    try:
        link_index = data.index("gripper_connect")
    except ValueError:
        rospy.logerr("Link not found in joint states message")
        return

    # Create a listener to receive the transform between the link and the world frame
    listener = tf.TransformListener()
    rate = rospy.Rate(10.0) # 10 Hz
    # while not rospy.is_shutdown():
    try:
        # Wait for the transform to be available
        listener.waitForTransform("base_link", "gripper_connect", rospy.Time(), rospy.Duration(4.0))
        (trans, rot) = listener.lookupTransform("base_link", "gripper_connect", rospy.Time(0))
        print('Target Pose is')
        # rospy.loginfo("Translation: %s, Rotation: %s", str(trans), str(rot))
        print("Translation:", str(trans))
        print("Rotation:", str(rot))
    except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException) as e:
        rospy.logerr("Error getting transform: %s", str(e))
    rate.sleep()

def main():
    rospy.init_node("link_pose_node")
    links = ['base_link', 'base_plate', 'bicep', 'forearm', 'holder_link', 'bearing', 'gripper_connect']
    rospy.Subscriber("joint_states", JointState, joint_states_callback(links))
    rospy.spin()

if __name__ == "__main__":
    main()
