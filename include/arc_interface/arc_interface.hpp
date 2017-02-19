#pragma once

#include <vector>
#include <string>
#include <math.h>
#include "ros/ros.h"
#include "ackermann_msgs/AckermannDrive.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Float64.h"
#include "arc_msgs/State.h"

class interface {
 public:
 	interface(const ros::NodeHandle &nh);
  	void guardBoolCallback(const std_msgs::Bool::ConstPtr& msg);
  	void infoNiCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);
 	void CASCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);
 	void stop_Callback(const arc_msgs::State::ConstPtr& msg);

 	// Methods for dspace Info (Black Box for types)
 	// void velocity_left_dspace_Callback(const std_msgs::Float64::ConstPtr& msg);
	// void velocity_right_dspace_Callback(const std_msgs::Float64::ConstPtr& msg); 
	// void steering_angle_dspace_Callback(const std_msgs::Float64::ConstPtr& msg); 



 	void kill();

 private:
	ros::NodeHandle nh_; 

	//Publisher
	ros::Publisher ackermann_to_ni_pub_;
	ros::Publisher steering_angle_pub_;
	ros::Publisher steering_pub_;
	ros::Publisher right_wheel_pub_;
	ros::Publisher left_wheel_pub_;

	// ros::Publisher left_wheel_dspace_sub;

	//Subscriber for CAS
	ros::Subscriber ackermann_sub_;
	ros::Subscriber guard_bool_sub_;
	//Subscriber for NI
	ros::Subscriber sensors_ni_sub_;
	//Subscriber for Dspace
	ros::Subscriber dspace_left_wheel_sub_; //velocity left from Dspace
	ros::Subscriber dspace_v_right_sub_; // velocity right from Dspace
	//Subscriber for arc_msgs/State
	ros::Subscriber emergency_stop_sub_;

	//Msgs for CAS
	ackermann_msgs::AckermannDrive control_input_;
	bool guard_bool_;

	//Msgs for arc_msgs
	bool stop_;

	//Msgs from NI
	ackermann_msgs::AckermannDrive sensors_ni_; //temporary ackermann_msgs = type of NI

	//Msgs from Dspace
	std_msgs::Float64 v_right_dspace_; //Suppose that the type of the single right velocity is Float64
	std_msgs::Float64 v_left_dspace_;
	std_msgs::Float64 steering_angle_dspace_;


 };
