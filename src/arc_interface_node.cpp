#include "../include/arc_interface/arc_interface.hpp"
#include "ackermann_msgs/AckermannDrive.h"


int main(int argc, char** argv){
	ros::init(argc, argv, "arc_interface_node");
	ros::NodeHandle nh;
	interface object(nh);

	ros::spin();
	return 0;
}