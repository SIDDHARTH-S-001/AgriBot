import random
import numpy as np
import moveit_commander
import sys
import rospy

# Initialize moveit_commander and move_group
moveit_commander.roscpp_initialize(sys.argv)
robot = moveit_commander.RobotCommander()
arm_group = moveit_commander.MoveGroupCommander('arm')

# Set workspace bounds
# workspace_bounds = np.array([[0.0, 0.6], [-0.3, 0.3], [0.0, 1.0]])

# Function to generate random target with z position greater than 0
def get_random_target():
    while not rospy.is_shutdown():
        # Generate random target within workspace bounds
        target_pose = arm_group.get_random_pose()
        target_pos = target_pose.pose.position
        target_xyz = np.array([target_pos.x, target_pos.y, target_pos.z])
        
        # Check if z position is greater than 0
        if target_xyz[2] > 0.05:
            arm_group.set_pose_target(target_pose)
            arm_group.plan()
            arm_group.go(wait = True)
            # return target_pose
        
        else:
            print('Robot will hit the ground')
            continue

    rospy.sleep(1)
    moveit_commander.roscpp_shutdown()

if __name__ == '__main__':
    try:
        rospy.init_node('random')
        get_random_target()
    except rospy.ROSInterruptException:
        pass

        
