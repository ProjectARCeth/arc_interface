cmake_minimum_required(VERSION 2.8.3)
project(arc_interface)

find_package(catkin_simple 0.1.0 REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  ackermann_msgs
  catkin_simple
  REQUIRED
)


catkin_simple(ALL_DEPS_REQUIRED)

cs_add_library(${PROJECT_NAME}
	src/arc_interface.cpp
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

## Declare a C++ executable
 add_executable(arc_interface_node src/arc_interface_node.cpp)

## Add cmake target dependencies of the executable
## same as for the library above
 #add_dependencies(arc_interface_node
#	arc_interface_generate_messages_cpp)
 
## Specify libraries to link a library or executable target against
 target_link_libraries(arc_interface_node
   ${catkin_LIBRARIES}
${PROJECT_NAME}
 )

cs_install()
cs_export()

