cmake_minimum_required(VERSION 2.8.3)
project(arc_interface)

find_package(catkin_simple 0.1.0 REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  catkin_simple
  REQUIRED
)

catkin_simple(ALL_DEPS_REQUIRED)

cs_add_library(${PROJECT_NAME}
	src/tcp_client.cpp
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(udp_client src/main.cpp)
target_link_libraries(udp_client ${catkin_LIBRARIES} ${PROJECT_NAME})

cs_install()
cs_export()

