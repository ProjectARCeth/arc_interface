#include <vector>
#include <string>
#include <math.h>

#include "ros/ros.h"
#include "ackermann_msgs/AckermannDrive.h"
// #include "nav_msgs/Odometry.h"
// #include "location_monitor/LandmarkDistance.h"

// using std::vector;
// using std::string;
// using location_monitor::LandmarkDistance;

// class Landmark {
//  public:
//  	Landmark(string name, double x, double y)
//  	: name(name), x(x), y(y) {}
//  	string name;
//  	double x;
//  	double y;
//  };

// class LandmarkMonitor {
//  public:
//  	LandmarkMonitor(const ros::Publisher& landmarks_pub)
//  			: landmarks_(), landmarks_pub_(landmarks_pub) {
// 		InitLandmarks();	
// 	}

	void OdomCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){
		double x = msg->steering_angle;
		double y = msg->steering_angle;

		ROS_INFO("angle: %f", x);	
	}

//  private:
//   	vector<Landmark> landmarks_;
//   	ros::Publisher landmarks_pub_;

// 	LandmarkDistance FindClosest(double x, double y){
// 		LandmarkDistance result;
// 		result.distance = -1;

// 	for(size_t i=0; i < landmarks_.size(); ++i){
// 		const Landmark& landmark = landmarks_[i];
// 		double xd = landmark.x - x;
// 		double yd = landmark.y - y;
// 		double distance = sqrt(xd*xd + yd*yd);

// 		if (result.distance < 0 || distance < result.distance){
// 			result.name = landmark.name;
// 			result.distance = distance;
// 		}
// 	}
// 	return result;
//    }

//     void InitLandmarks() {
//    	landmarks_.push_back(Landmark("Cube", 0.31, -0.99));
//    	landmarks_.push_back(Landmark("Dumpster", 0.11, -2.42));
//    	landmarks_.push_back(Landmark("Cylnder", -1.14, -2.88));
//    	landmarks_.push_back(Landmark("Barrier", -2.59, -0.83));
//    	landmarks_.push_back(Landmark("Bookshelf", -0.09, 0.53));
//    }
// };




int main(int argc, char** argv){
	ros::init(argc, argv, "arc_interface");
	ros::NodeHandle nh;
	// ros::Publisher landmarks_pub = nh.advertise<LandmarkDistance>("closest_landmark", 10);
	// LandmarkMonitor monitor(landmarks_pub);
	ros::Subscriber sub = nh.subscribe("/stellgroessen_safe", 10, OdomCallback);
	ros::spin();
	return 0;
}