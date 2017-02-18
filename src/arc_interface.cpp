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
	ackermann_sub_= nh_.subscribe("/stellgroessen_safe", 10, &interface::ackCallback, this);
	ackermann_pub_ = nh_.advertise<ackermann_msgs::AckermannDrive>("/ni", 10);
}

void interface::guardBoolCallback(const std_msgs::Bool::ConstPtr& msg){
  	guard_bool_ = msg->data;
}

void interface::ackCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){ 
 	control_input_ = *msg;
}

