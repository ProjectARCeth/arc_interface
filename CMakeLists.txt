cmake_minimum_required(VERSION 2.8.3)
project(arc_interface)

find_package(catkin_simple 0.1.0 REQUIRED COMPONENTS
  roscpp
  std_msgs
  arc_msgs
  catkin_simple REQUIRED
)

catkin_simple(ALL_DEPS_REQUIRED)

add_executable(udp_client src/main.cpp)
target_link_libraries(udp_client ${catkin_LIBRARIES})

add_executable(conversion_test test/conversion_test.cpp)
target_link_libraries(conversion_test ${catkin_LIBRARIES})

add_executable(dSpace test/dSpace.cpp)
target_link_libraries(dSpace ${catkin_LIBRARIES})

add_executable(usb_NI test/usb_NI.cpp)
target_link_libraries(usb_NI ${catkin_LIBRARIES})

cs_install()
cs_export()

