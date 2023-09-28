#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/robot_trajectory/robot_trajectory.h>
#include <moveit/trajectory_processing/iterative_time_parameterization.h>
#include <moveit_msgs/OrientationConstraint.h>

// 先把txt文件中的数据读下来，存入数组中，再根据数组定义多个目标位置，规划出多条轨迹，然后实现多轨迹连续规划，多轨迹的拼接过程中速度不为零

int main(int argc, char **argv)
{
    ros::init(argc, argv, "exo_stand_continue_trajectory");
    ros::NodeHandle node_handle; 
    ros::AsyncSpinner spinner(1);
    spinner.start();

    moveit::planning_interface::MoveGroupInterface leg("maxon");

    leg.setGoalJointTolerance(0.001);

    double accScale = 0.5;
    double velScale = 0.5;
    leg.setMaxAccelerationScalingFactor(accScale);
    leg.setMaxVelocityScalingFactor(velScale);


    // 控制机械臂先回到初始化位置
    leg.setNamedTarget("start");
    leg.move();
    sleep(1);

    // 获取机器人的起始位置
    moveit::core::RobotStatePtr start_state(leg.getCurrentState());
    const robot_state::JointModelGroup *joint_model_group = start_state->getJointModelGroup(leg.getName());

    // std::vector<double> joint_group_positions;
    // start_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

    // double targetPose[6] = {-0.9526, 1.1956, -1.2026, 1.1956,  -0.9926, 0.6056};

    double targetPose[6] = {-1.0226, 1.1656, -1.2826, 1.3656,  -1.1726, 1.1656};
    std::vector<double> left_joint_positions1(2);
    std::vector<double> left_joint_positions2(2);
    std::vector<double> left_joint_positions3(2);

    left_joint_positions1[0] = targetPose[0];
    left_joint_positions1[1] = targetPose[1];

    left_joint_positions2[0] = targetPose[2];
    left_joint_positions2[1] = targetPose[3];

    left_joint_positions3[0] = targetPose[4];
    left_joint_positions3[1] = targetPose[5];


    //设置第一个目标点
    leg.setJointValueTarget(left_joint_positions1);

    // 计算第一条轨迹
    moveit::planning_interface::MoveGroupInterface::Plan plan1;
    moveit::planning_interface::MoveItErrorCode success = leg.plan(plan1);

    joint_model_group = start_state->getJointModelGroup(leg.getName());    
    start_state->setJointGroupPositions(joint_model_group, left_joint_positions1);
    leg.setStartState(*start_state);

    //设置第二个目标点
    leg.setJointValueTarget(left_joint_positions2);

    // 计算第二条轨迹
    moveit::planning_interface::MoveGroupInterface::Plan plan2;
    success = leg.plan(plan2);

    joint_model_group = start_state->getJointModelGroup(leg.getName());    
    start_state->setJointGroupPositions(joint_model_group, left_joint_positions2);
    leg.setStartState(*start_state);

    // 设计第三个目标点
    leg.setJointValueTarget(left_joint_positions3);

    // 计算第三条轨迹  
    moveit::planning_interface::MoveGroupInterface::Plan plan3;
    success = leg.plan(plan3);

    joint_model_group = start_state->getJointModelGroup(leg.getName());
    start_state->setJointGroupPositions(joint_model_group, left_joint_positions3);
    leg.setStartState(*start_state);

    

    //连接三条轨迹
    moveit_msgs::RobotTrajectory trajectory;

    trajectory.joint_trajectory.joint_names = plan1.trajectory_.joint_trajectory.joint_names;
    trajectory.joint_trajectory.points = plan1.trajectory_.joint_trajectory.points;

    // 添加plan2轨迹点
    for (size_t j = 1; j < plan2.trajectory_.joint_trajectory.points.size(); j++)
    {
        trajectory.joint_trajectory.points.push_back(plan2.trajectory_.joint_trajectory.points[j]);
    }

    // 添加plan3轨迹点
    for (size_t j = 1; j < plan3.trajectory_.joint_trajectory.points.size(); j++)
    {
        trajectory.joint_trajectory.points.push_back(plan3.trajectory_.joint_trajectory.points[j]);
    }

    // 重规划
    moveit::planning_interface::MoveGroupInterface::Plan joinedPlan;
    robot_trajectory::RobotTrajectory rt(leg.getCurrentState()->getRobotModel(), "maxon");
    rt.setRobotTrajectoryMsg(*leg.getCurrentState(), trajectory);
    trajectory_processing::IterativeParabolicTimeParameterization iptp;
    iptp.computeTimeStamps(rt, velScale, accScale);

    rt.getRobotTrajectoryMsg(trajectory);
    joinedPlan.trajectory_ = trajectory;

    if (!leg.execute(joinedPlan))
    {
        ROS_ERROR("Failed to execute plan");
        return false;
    }

    sleep(1);

    ROS_INFO("Finished");

    // 控制机械臂先回到初始化位置
    leg.setNamedTarget("start");
    leg.move();
    sleep(1);

    ros::shutdown(); 

    return 0;
}
