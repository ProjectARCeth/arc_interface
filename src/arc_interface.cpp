#include <vector>
#include <string>
#include <math.h>

#include "ros/ros.h"
#include "ackermann_msgs/AckermannDrive.h"
#include "../include/arc_interface/arc_interface.hpp"

interface::interface(const ros::NodeHandle &nh) : nh_(nh)
{
	std::cout<<"I'm open"<<std::endl;
	guard_bool_sub_= nh_.subscribe("/not_stop", 10, &interface::guardBoolCallback, this);
	ackermann_sub_= nh_.subscribe("/stellgroessen_safe", 10, &interface::CASCallback, this);
	ackermann_sub_= nh_.subscribe("/ni_out", 10, &interface::infoNiCallback, this); 
	emergency_stop_sub_= nh_.subscribe("state",10, &interface::stop_Callback, this); //type not known yet
	// dspace_v_left_sub_= nh_.subscribe("/dspace_out", &interface::velocity_left_tkp_Callback, this);
	// dspace_v_right_sub_= nh_.subscribe("/dspace_out", &interface::velocity_right_Callback, this);
	ackermann_to_ni_pub_ = nh_.advertise<ackermann_msgs::AckermannDrive>("/ni_in", 10);
	right_wheel_pub_ = nh_.advertise<std_msgs::Bool>("vcu_stop", 10);
	steering_pub_ = nh_.advertise<std_msgs::Float64>("steer_angle", 10);
	left_wheel_pub_ = nh_.advertise<std_msgs::Float64>("v_left", 10);
	right_wheel_pub_ = nh_.advertise<std_msgs::Float64>("v_right", 10);
}

void interface::guardBoolCallback(const std_msgs::Bool::ConstPtr& msg){
	// std::cout<< "I hear the safe bool"<<std::endl;
  	guard_bool_ = msg->data;
}

void interface::stop_Callback(const arc_msgs::State::ConstPtr& msg){
	stop_ = msg->stop;
}

void interface::CASCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){ 
	// std::cout<< "I have the input controls"<<std::endl;
 	control_input_ = *msg;
 	ROS_INFO("I heard: [%f]", control_input_.steering_angle);
}

void interface::infoNiCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){ 
 	sensors_ni_= *msg;
}

// //Callback for dspace

// void interface::velocity_left_dspace_Callback(const std_msgs::Float64::ConstPtr& msg){
// 	v_left_dspace_ = *msg; //
// }

// void interface::velocity_right_dspace_Callback(const std_msgs::Float64::ConstPtr& msg){
// 	v_right_dspace_ = *msg; //
// }

// void interface::steering_angle_dspace_Callback(const std_msgs::Float64::ConstPtr& msg){
// 	steering_angle_dspace_ = *msg; //
// }


void interface::kill()
{
	ros::Rate loop_rate(10);
	if(stop_==0)
	{
		ackermann_to_ni_pub_.publish(control_input_);
		loop_rate.sleep();
	}
	else
	{
		ros::shutdown();
	}
}

// void interface::safe() {
// 	while (ros::ok())
// 	{

// 	}
// }



