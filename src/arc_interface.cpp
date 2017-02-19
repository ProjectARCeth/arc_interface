#include <vector>
#include <string>
#include <math.h>

#include "ros/ros.h"
#include "ackermann_msgs/AckermannDrive.h"
#include "../include/arc_interface/arc_interface.hpp"

interface::interface(const ros::NodeHandle &nh) : nh_(nh)
{
	std::cout<<"I'm open"<<std::endl;
	std::cout<<"CONNECTING WITH EMERGENCY.."<<std::endl;
	guard_bool_sub_= nh_.subscribe("/not_stop", 10, &interface::guardBoolCallback, this);
	std::cout<<"CHECK"<<std::endl;
	std::cout<<"GETTING CONTROL INPUTS.."<<std::endl;
	ackermann_sub_= nh_.subscribe("/stellgroessen_safe", 10, &interface::ackCallback, this);
	std::cout<<"CHECK"<<std::endl;	
	std::cout<<"CONNECTING WITH .."<<std::endl;	
	ackermann_sub_= nh_.subscribe("/ni_out", 10, &interface::infoNiCallback, this); //type not known yet

	// ackermann_to_ni_pub_ = nh_.advertise<ackermann_msgs::AckermannDrive>("/ni_in", 10);
}

void interface::guardBoolCallback(const std_msgs::Bool::ConstPtr& msg){
	// std::cout<< "I hear the safe bool"<<std::endl;
  	guard_bool_ = msg->data;
  
}

void interface::ackCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){ 
	// std::cout<< "I have the input controls"<<std::endl;
 	control_input_ = *msg;
 	ROS_INFO("I heard: [%f]", control_input_.steering_angle);
}

void interface::infoNiCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){ 
 	sensors_ni_= *msg;
}

void interface::kill()
{
	ros::Rate loop_rate(10);
	while(guard_bool_==0)
	{
		ackermann_to_ni_pub_.publish(control_input_);
		loop_rate.sleep();
	}
}

// void interface::safe() {
// 	while (ros::ok())
// 	{

// 	}
// }



