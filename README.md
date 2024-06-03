# ros1_canopen_maxon_control——2023.7.28
## 引言
大四上，总耗时5个月，从0到1，独立完成的一个小项目。在学习过程中，也走了一些弯路，踩了很多坑。于是决定开源代码，希望可以帮助相关领域志同道合的小伙伴，节约开发的时间成本，把自己的精力放到控制算法的开发上。
## 1）功能介绍
在ros1-noetic中，使用了ros_control和ros_canopen官方提供的功能包，完成了can通讯驱动maxon电机（**EPOS4**），实现了ppm（位置模式）、pvm（速度模式）、csp（循环同步位置控制）三种模式的控制。

配套机器人仿真环境代码：https://github.com/hhz0328/ros1_exoskeleton_maxon_simulation

实物机器人展示视频（转载自本人B站）：https://www.bilibili.com/video/BV1HN4y1r7b5/?spm_id_from=333.999.0.0 

**效果展示：**

<div align="center">
<table>
<tr>
<td>

![](https://github.com/hhz0328/ros1_canopen_maxon_control/blob/noetic/demo.gif)  

</td>
</tr>
</table>
</div>

## 2）环境配置
（1）**用到的功能包**：ros_control、ros_canopen

（2）**编译过程**：将以上的功能包放入到 **/catkin_ws/src** ，再在 **/catkin_ws** 中打开终端用 **catkin_make** 命令进行编译
## 3）使用
- **（1）单个电机的ppm位置模式控制：**
```
  $ roscore
  $ sudo ip link set can0 type can bitrate 1000000
  $ sudo ip link set can0 up
  $ roslaunch maxon_epos4_ros_canopen maxon_epos4_canopen_motor_1dof_ppm.launch
  $ rosservice call /driver/init
  $ rostopic pub /canopen_motor/base_link1_joint_position_controller/command std_msgs/Float64 -- 1000
```
- **（2）双电机的pvm速度模式控制：**
```
  $ roscore
  $ sudo ip link set can0 type can bitrate 1000000
  $ sudo ip link set can0 up
  $ roslaunch maxon_epos4_ros_canopen maxon_epos4_canopen_motor_2dof_pvm.launch
  $ rosservice call /driver/init
  $ rosservice call /maxon/driver/set_object base_link1_joint '!!str6081' '!!str 1000' false
```
- **（3）多电机的csp循环同步位置模式控制（经过测试，可以稳定驱动4个电机，想要更快更多，可以自己打补丁，篇幅有限不多加赘述）：**
```
  $ roscore
  $ sudo ip link set can0 type can bitrate 1000000
  $ sudo ip link set can0 up
  $ roslaunch maxon_epos4_ros_canopen maxon_epos4_canopen_motor_4dof_csp.launch
  $ rosservice call /driver/init
  $ roslaunch exo_stand_moveit_config demo.launch rviz_tutorial:=true
```

## GOOD LUCK
坚持开源，共同进步！对代码有疑问，可以通过主页邮箱留言，也可以issue提问

你的星星，是我前进的动力。🖼
