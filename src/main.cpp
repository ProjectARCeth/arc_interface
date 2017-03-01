#include "arc_interface/tcp_client.hpp"

#include <iostream>  
#include <stdio.h>   
#include <string.h>  
#include <string>    
#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <netdb.h>   
#include <sstream>

#include <signal.h> 
#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>
#include <ctype.h>

#include <ros/ros.h>
//Constants.
std::string NI_HOSTNAME;
int NI_PORT;
//Declaration of functions.
bool conn(std::string address , int port);

int main(int argc , char  **argv){
  //Init ros.
  ros::init(argc, argv, "tcp_server");
  ros::NodeHandle node;
  //Host and port.
  node.getParam("/general/NI_HOSTNAME", NI_HOSTNAME);
  node.getParam("/general/NI_PORT", NI_PORT);
  //Connect to host.
  int tries=0;
  int numberOfTries=100;
  bool isConnected=false;
  while(tries<numberOfTries && isConnected==false){
    tries++;
    isConnected=conn(NI_HOSTNAME, NI_PORT);
    if(isConnected==false){
      ROS_WARN("INTERFACE: connection failed for the %d time",tries);
      ROS_WARN("INTERFACE: Trying again in 1 sec");
      if(tries>=numberOfTries){
        ROS_ERROR("INTERFACE: Too much tries");
        return 0;
      }
    sleep(1);
    }
  }
  //ROS --> MyRIO---------------------------------------------------------------
  tcp_client wheel_pub(1,"wheel",node,"String");                          
  //MyRIO --> ROS---------------------------------------------------------------
  tcp_client loop_sub(2,"loop",node,"int");
    
  int count = 0;
  ros::Rate loop_rate(100);
  while(ros::ok()){
    std::string receivedString= loop_sub.receive_bytes(1024);
    loop_sub.parser(receivedString);
    ros::spinOnce();
    loop_rate.sleep();
    count++;
   }
  return 0;
}

bool conn(std::string address , int port){
    //Create socket.
    if(tcp_client::sock == -1){
        tcp_client::sock = socket(PF_INET , SOCK_STREAM , 0);
        if (tcp_client::sock == -1){
            perror("Could not create socket");
            return false;
            exit(1);
        }
    }
    else {}
    //Śetup address structure (IP to machineIP).
    if(inet_addr(address.c_str()) == -1){
        std::cout << "Inet address" << std::endl;
        struct hostent *he;
        struct in_addr **addr_list;
        //Resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL){
            herror("gethostbyname");
            std::cout<<"Failed to resolve hostname\n";
            return false;
        }
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only.
        addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++){
            tcp_client::server.sin_addr = *addr_list[i];
            std::cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<std::endl;
            break;
        }
    }
    else tcp_client::server.sin_addr.s_addr = inet_addr(address.c_str());
    //Resolve the port.
    tcp_client::server.sin_family = AF_INET;
    tcp_client::server.sin_port = htons(port);
    //Connect to remote server.
    std::cout << "Server address: " << NI_HOSTNAME << std::endl;
    std::cout << "Server port: " << NI_PORT << std::endl;
    std::cout << "socket_number: " << tcp_client::sock << std::endl;
    std::cout << "server_adress: " << tcp_client::server.sin_addr.s_addr << std::endl;
    std::cout << "sin_family: " << tcp_client::server.sin_family << std::endl;
    std::cout << "sin_port: " << tcp_client::server.sin_port << std::endl;
    std::cout << "server_length: " << sizeof(tcp_client::server) << std::endl;
    //Create server struct.
    struct sockaddr* server_in;
    if(connect(tcp_client::sock, (struct sockaddr *)&tcp_client::server, sizeof(tcp_client::server)) < 0){
        perror("connect failed. Error");
        return false;
    }
    std::cout<<"Connected\n";
    tcp_client::address = address;
    tcp_client::port = port;
    return true;
}