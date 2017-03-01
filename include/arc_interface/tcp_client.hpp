#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <arpa/inet.h>  
#include <iostream> 
#include <netdb.h>  
#include <signal.h>
#include <sstream> 
#include <stdio.h> 
#include <stdio.h>   
#include <string.h>  
#include <string>    
#include <sys/socket.h>
#include <unistd.h>   

#include <stdlib.h>
#include <ctype.h>

#include <ros/ros.h>
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"

class tcp_client{
private:
  ros::NodeHandle node_;
  ros::Subscriber sub_;
  ros::Publisher pub_;

  std::string msg_type_;
  int direction_;
  std::string topic_;

  std::vector<double> values_;
  std::string header_;
  std::string header2_;

public:
  tcp_client(int dir,std::string top,ros::NodeHandle node,std::string msg_type);
  bool send_data(std::string data);
  std::string receive_bytes(int);
  void CallbackF64(const std_msgs::Float64::ConstPtr& msg);
  void CallbackString(const std_msgs::String::ConstPtr& msg);

  void parser(std::string s);
  void publish();

  static int sock;
  static std::string address;
  static int port;
  static struct sockaddr_in server;
};

#endif
