#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# python正运动学fk规划，提供关节角度，实现外骨骼机器人的运动

import rospy, sys
import moveit_commander

class MoveItFkDemo:
    def __init__(self):
        # 初始化move_group的API
        moveit_commander.roscpp_initialize(sys.argv)

        # 初始化ROS节点
        rospy.init_node('exo_stand_fk', anonymous=True)
 
        # 初始化需要使用 leg 控制的机械臂中的 maxon
        leg = moveit_commander.MoveGroupCommander('maxon')
        
        # 设置机械臂运动的允许误差值：单位rad
        leg.set_goal_joint_tolerance(0.001)

        # 设置允许的最大速度和加速度
        leg.set_max_acceleration_scaling_factor(0.5)
        leg.set_max_velocity_scaling_factor(0.5)

        # 控制机械臂先回到初始化位置
        leg.set_named_target('start')

        leg.go()
        rospy.sleep(1)
         
        # 设置机械臂的目标位置，目前只有左腿，可拓展到双腿     使用六轴的位置数据进行描述（单位：弧度）
        leg_joint_positions = [-1.20252, 1.19553]

        leg.set_joint_value_target(leg_joint_positions)
                 
        # 控制机械臂完成运动
        leg.go()
        rospy.sleep(1)

        # 控制机械臂先回到初始化位置
        leg.set_named_target('start')

        leg.go()
        rospy.sleep(1)
        
        # 关闭并退出moveit
        moveit_commander.roscpp_shutdown()
        moveit_commander.os._exit(0)

if __name__ == "__main__":
    try:
        MoveItFkDemo()
    except rospy.ROSInterruptException:
        pass
