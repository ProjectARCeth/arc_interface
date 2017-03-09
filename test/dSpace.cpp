#include <arpa/inet.h>
#include <iostream>
#include <ros/ros.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

//Define Constants.
const int BUFLEN = 512;
//Network.
struct sockaddr_in si_me, si_other, si_NI;
char buffer_in[BUFLEN];
int sock;
socklen_t slen;
//Declaration of functions.
void printErrorAndFinish(std::string str);
void setUpNetwork();

int main(int argc, char** argv){
  //Init ros.
  ros::init(argc, argv, "arc_interface");
  ros::NodeHandle node;
  //Initialise addresses.
  setUpNetwork();
  if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) printErrorAndFinish("socket");
  if(bind(sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) printErrorAndFinish("binding");
  //Listening for and sending data.
  while(ros::ok()){
    //Receiving.
    int recv_len;
    if ((recv_len = recvfrom(sock, buffer_in, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) 
           printErrorAndFinish("receiving");
    //Edit msg.
    std::ostringstream oss;
    oss << buffer_in;
    std::string intAsString(oss.str());
    //Hexadezimalzahl.
    int hexa; 
    std::stringstream sstr(intAsString);
    sstr >> hexa;
    //Print msg.
    std::cout << "New msg: " << oss << std::endl;
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
  si_me.sin_port = htons(5000);
  si_me.sin_addr.s_addr = inet_addr("192.168.140.2"); //htonl("INADDR_ANY");
  //NI (destination) address.
  si_NI.sin_family = AF_INET;
  si_NI.sin_port = htons(5000);
  si_NI.sin_addr.s_addr = inet_addr("192.168.140.3");
  //Socket length.
  slen = sizeof(si_other);
}