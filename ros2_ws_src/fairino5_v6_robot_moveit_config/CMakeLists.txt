cmake_minimum_required(VERSION 3.8)
project(fairino5_v6_robot_moveit_config)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(trajectory_msgs REQUIRED)


# PROGRAM 1: test_move.cpp
add_executable(test_move_node src/test_move.cpp)
ament_target_dependencies(test_move_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)

# PROGRAM 2: mover_base.cpp
add_executable(mover_base_node src/mover_base.cpp)
ament_target_dependencies(mover_base_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)

#3 pick and place
add_executable(mover_pick_place_node src/mover_pick_place.cpp)
ament_target_dependencies(mover_pick_place_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)

install(TARGETS 
  test_move_node 
  mover_base_node
mover_pick_place_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config launch DESTINATION share/${PROJECT_NAME})

ament_package()
