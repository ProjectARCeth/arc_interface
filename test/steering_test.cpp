#include <arpa/inet.h>
#include <iostream>
#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

#include <ackermann_msgs/AckermannDrive.h>
#include <std_msgs/Float64.h>

//Define Constants.
const int BUFLEN = 512;
int MY_PORT = 8010;
int NI_PORT = 8001;
std::string STELLGROESSEN_TOPIC = "/stellgroessen_safe";
std::string STEERING_CURRENT_TOPIC = "/state_steering_angle";
//Network.
struct sockaddr_in si_me, si_other, si_NI;
char buffer_in[BUFLEN];
char buffer_out[BUFLEN];
int sock;
socklen_t slen;
//Subscriber and publisher.
ros::Publisher steering_current_pub;
ros::Subscriber stellgroessen_should_sub;
//Declaration of functions.
std::string convertCharArrayToString(char array[BUFLEN], int size);
std::string convertDoubleToString(double value);
void convertStringToCharArray(std::string line, char* buffer);
double getValueFromMsg(std::string msg);
void handleReceivedMsg(std::string msg);
void printErrorAndFinish(std::string reason);
void setUpNetwork();
void setUpRosInterface(ros::NodeHandle* node);
void stellgroessenCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg);

int main(int argc, char** argv){
  //Init ros.
  ros::init(argc, argv, "steering_test");
  ros::NodeHandle node;
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
    std::cout << buffer_in << std::endl;
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
  //Get value string (beginning behind : ).
  std::string value_string(msg, 3, msg.length()-1);
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
  else std::cout<<"ARC INTERFACE: Cannot assign msg " << msg << std::endl;
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
  steering_current_pub = node->advertise<std_msgs::Float64>(STEERING_CURRENT_TOPIC, 10);
  stellgroessen_should_sub = node->subscribe(STELLGROESSEN_TOPIC, 10, stellgroessenCallback);
  //Sending initialised comment.
  std::cout << std::endl << "ARC INTERFACE: Ros core initialised "<< std::endl;
}

void stellgroessenCallback(const ackermann_msgs::AckermannDrive::ConstPtr& msg){
  //Getting stellgroessen.
  double vel_should = msg->speed;
  double steering_should = msg->steering_angle;
  //Sending to NI.
  std::string steering_string = "ss:" + convertDoubleToString(steering_should);
  convertStringToCharArray(steering_string, buffer_out);
  if (sendto(sock, buffer_out, sizeof(buffer_out), 0, (struct sockaddr*) &si_NI, slen) == -1) 
        printErrorAndFinish("sending steering_should");  
}