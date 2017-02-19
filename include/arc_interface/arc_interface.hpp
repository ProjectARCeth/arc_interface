#pragma once

#include <vector>
#include <string>
#include <math.h>
#include "ros/ros.h"
#include "ackermann_msgs/AckermannDrive.h"
#include "std_msgs/Bool.h"

class interface {
 public:
 	interface(const ros::NodeHandle &nh);
  	void guardBoolCallback(const std_msgs::Bool::ConstPtr& msg);
  	void infoNiCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);
 	void ackCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);
 	void kill();
 private:
	ros::NodeHandle nh_; 	
	ros::Subscriber ackermann_sub_;
	ros::Subscriber guard_bool_sub_;
	ros::Subscriber sensors_ni_sub_;
	ros::Publisher ackermann_to_ni_pub_;
	ackermann_msgs::AckermannDrive control_input_;
	ackermann_msgs::AckermannDrive sensors_ni_;
	bool guard_bool_;
 };
