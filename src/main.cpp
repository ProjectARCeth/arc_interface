#include <arpa/inet.h>
#include <iostream>
#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

#include <arc_msgs/State.h>
#include <ackermann_msgs/AckermannDrive.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64.h>

//Define Constants.
const int BUFLEN = 512;
const int MSG_SIZE = 5;
int MY_PORT;
int NI_PORT;
int QUEUE_LENGTH;
std::string FRONT_LEFT_TOPIC;
std::string FRONT_RIGHT_TOPIC;
std::string NOTSTOP_TOPIC;
std::string NOTSTOP_NI_TOPIC;
std::string REAR_LEFT_TOPIC;
std::string REAR_RIGHT_TOPIC;
std::string STELLGROESSEN_TOPIC;
std::string STEERING_CURRENT_TOPIC;
std::string VELOCITY_CURRENT_TOPIC;
//Network.
struct sockaddr_in si_me, si_other, si_NI;
char buffer_in[BUFLEN];
char buffer_out[BUFLEN];
int sock;
socklen_t slen;
//Subscriber and publisher.
ros::Publisher front_left_pub;
ros::Publisher front_right_pub;
ros::Publisher notstop_NI_pub;
ros::Publisher rear_left_pub;
ros::Publisher rear_right_pub;
ros::Publisher steering_current_pub;
ros::Subscriber notstop_sub;
ros::Subscriber stellgroessen_should_sub;
ros::Subscriber velocity_current_sub;
//Declaration of functions.
std::string convertCharArrayToString(char array[BUFLEN], int size);
std::string convertDoubleToString(double value);
void convertStringToCharArray(std::string line, char* buffer);
double getValueFromMsg(std::string msg);
void handleReceivedMsg(std::string msg);
void notstopCallback(const std_msgs::Bool::ConstPtr& msg);
void printErrorAndFinish(std::string reason);
void setUpNetwork();
void setUpRosInterface(ros::NodeHandle* node);
void stellgroessenCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);
void velocityCallback(const arc_msgs::State::ConstPtr& msg);

int main(int argc, char** argv){
  //Init ros.
  ros::init(argc, argv, "arc_interface");
  ros::NodeHandle node;
  //Get constants from yaml file.
  node.getParam("/general/MY_PORT", MY_PORT);
  node.getParam("/general/NI_PORT", NI_PORT);
  node.getParam("/general/QUEUE_LENGTH", QUEUE_LENGTH);
  node.getParam("/topic/WHEEL_FRONT_LEFT", FRONT_LEFT_TOPIC);
  node.getParam("/topic/WHEEL_FRONT_RIGHT", FRONT_RIGHT_TOPIC);
  node.getParam("/topic/NOTSTOP", NOTSTOP_TOPIC);
  node.getParam("/topic/NOTSTOP_NI_TOPIC", NOTSTOP_NI_TOPIC);
  node.getParam("/topic/WHEEL_REAR_LEFT", REAR_LEFT_TOPIC);
  node.getParam("/topic/WHEEL_REAR_RIGHT", REAR_RIGHT_TOPIC);
  node.getParam("/topic/STELLGROESSEN_SAFE", STELLGROESSEN_TOPIC);
  node.getParam("/topic/STATE_STEERING_ANGLE", STEERING_CURRENT_TOPIC);
  node.getParam("/topic/STATE", VELOCITY_CURRENT_TOPIC);
  //Initialise addresses.
  setUpNetwork();
  if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) printErrorAndFinish("socket");
  if(bind(sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) printErrorAndFinish("binding");
  //Initialise ros interface.
  setUpRosInterface(&node);
  //Listening for and sending data.
  while(ros::ok()){
    //Receiving.
    int recv_len;
    if ((recv_len = recvfrom(sock, buffer_in, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) 
           printErrorAndFinish("receiving");
    std::string buffer_in_string = convertCharArrayToString(buffer_in, recv_len);
    handleReceivedMsg(buffer_in_string);
    //Sending.
    ros::spinOnce();
  }
  return 0;
}

std::string convertCharArrayToString(char array[BUFLEN], int size){
  std::string str;
  for (int i = 0; i < size; ++i){
    str += array[i];
  }
  return str;
}

std::string convertDoubleToString(double value){
  std::ostringstream stream;
  stream << value;
  return stream.str();
}

void convertStringToCharArray(std::string line, char* buffer){
  for (int i=0; i<line.size(); ++i){
    buffer[i] = line[i];
  }
}

double getValueFromMsg(std::string msg){
  //Get value string (substracting ; and beginning behind : ).
  std::string value_string(msg, 3, msg.length()-2);
  //Convert to number.
  char buffer[BUFLEN];
  convertStringToCharArray(value_string, buffer);
  double value = atof(buffer);
  return value;
}

void handleReceivedMsg(std::string msg){
  //Get kind of msg and value.
  std::string kind(msg, 0, 1);
  double value = getValueFromMsg(msg);
  //Creating ros msg.
  std_msgs::Float64 ros_msg;
  ros_msg.data = value;
  //Answers.
  if(kind == "si") steering_current_pub.publish(ros_msg);
  else if(kind == "fl") front_left_pub.publish(ros_msg);
  else if(kind == "fr") front_right_pub.publish(ros_msg);
  else if(kind == "rl") rear_left_pub.publish(ros_msg);
  else if(kind == "rr") rear_right_pub.publish(ros_msg);
  else if(kind == "hn"){
    std_msgs::Bool ros_bool_msg;
    ros_bool_msg.data = true;
    notstop_NI_pub.publish(ros_bool_msg);
  }
  else std::cout<<"ARC INTERFACE: Cannot assign msg " << msg << std::endl;
 }

void notstopCallback(const std_msgs::Bool::ConstPtr& msg){
  //Send notstop iff true.
  if(msg->data == true){
  }
  std::string notstop_string = "hr:1;";
  if (sendto(sock, buffer_out, sizeof(buffer_out), 0, (struct sockaddr*) &si_NI, slen) == -1) 
        printErrorAndFinish("sending notstop");
}

void printErrorAndFinish(std::string reason){
  std::cout << "ARC_INTERFACE: Error due to " << reason << std::endl;
  //Finish ros and program.
  ros::shutdown();
  exit(0);
  ros::waitForShutdown();
}

void setUpNetwork(){
  //Computer (my) address.
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(MY_PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  //NI (destination) address.
  si_NI.sin_family = AF_INET;
  si_NI.sin_port = htons(NI_PORT);
  si_NI.sin_addr.s_addr = inet_addr("10.0.0.5");
  //Socket length.
  slen = sizeof(si_other);
}

void setUpRosInterface(ros::NodeHandle* node){
  //Publisher and subscriber
  front_left_pub = node->advertise<std_msgs::Float64>(FRONT_LEFT_TOPIC, QUEUE_LENGTH);
  front_right_pub = node->advertise<std_msgs::Float64>(FRONT_RIGHT_TOPIC, QUEUE_LENGTH);
  notstop_NI_pub = node->advertise<std_msgs::Bool>(NOTSTOP_NI_TOPIC, QUEUE_LENGTH);
  rear_left_pub = node->advertise<std_msgs::Float64>(REAR_LEFT_TOPIC, QUEUE_LENGTH);
  rear_right_pub = node->advertise<std_msgs::Float64>(REAR_RIGHT_TOPIC, QUEUE_LENGTH);
  steering_current_pub = node->advertise<std_msgs::Float64>(STEERING_CURRENT_TOPIC, QUEUE_LENGTH);
  notstop_sub = node->subscribe(NOTSTOP_TOPIC, QUEUE_LENGTH, notstopCallback);
  stellgroessen_should_sub = node->subscribe(STELLGROESSEN_TOPIC, QUEUE_LENGTH, stellgroessenCallback);
  velocity_current_sub = node->subscribe(VELOCITY_CURRENT_TOPIC, QUEUE_LENGTH, velocityCallback);
  //Sending initialised comment.
  std::cout << std::endl << "ARC INTERFACE: Ros core initialised "<< std::endl;
}

void stellgroessenCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){
  //Getting stellgroessen.
  double vel_should = msg->speed;
  double steering_should = msg->steering_angle;
  //Sending to NI.
  std::string vel_string = "vs:" + convertDoubleToString(vel_should) + ";";
  convertStringToCharArray(vel_string, buffer_out);
  if (sendto(sock, buffer_out, sizeof(buffer_out), 0, (struct sockaddr*) &si_NI, slen) == -1) 
        printErrorAndFinish("sending velocity_should"); 
  std::string steering_string = "vs:" + convertDoubleToString(steering_should) + ";";
  convertStringToCharArray(steering_string, buffer_out);
  if (sendto(sock, buffer_out, sizeof(buffer_out), 0, (struct sockaddr*) &si_NI, slen) == -1) 
        printErrorAndFinish("sending steering_should");  
}

void velocityCallback(const arc_msgs::State::ConstPtr& msg){
  //Calculating velocity.
  double v_x = msg->pose_diff.twist.linear.x;
  double v_y = msg->pose_diff.twist.linear.y;
  double v_z = msg->pose_diff.twist.linear.z;
  double vel = sqrt(v_x*v_x + v_y*v_y + v_z*v_z);
  //Sending to NI.
  std::string vel_string = "vi:" + convertDoubleToString(vel) + ";";
  convertStringToCharArray(vel_string, buffer_out);
  if (sendto(sock, buffer_out, sizeof(buffer_out), 0, (struct sockaddr*) &si_NI, slen) == -1) 
        printErrorAndFinish("sending velocity_state");
  std::cout << "sent data: " << vel_string << std::endl;
}

//ToDo: Sizeof oder len vom Buffer_out in sendto ?


  // double i = 0;
  // while(1){
  //    std::cout<<("Waiting for data...")<<std::endl;
  //    fflush(stdout);
  //    //Receiving.
  //    i = i +1;
  //    convertStringToCharArray("vi:"+convertDoubleToString(i));
  //    if(i >= 5) i = 0;
  //    int recv_len;
  //    if ((recv_len = recvfrom(sock, buffer_in, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) 
  //       printErrorAndFinish("receiving");
  //    //Sending.
  //    if (sendto(sock, buffer_out, recv_len, 0, (struct sockaddr*) &si_NI, slen) == -1) 
  //       printErrorAndFinish("sending");
  //    //Printing data.
  //    std::cout << "Received data: " << buffer_in << std::endl;
  //    std::cout << "Sended data: " << buffer_out << std::endl;
  // } 