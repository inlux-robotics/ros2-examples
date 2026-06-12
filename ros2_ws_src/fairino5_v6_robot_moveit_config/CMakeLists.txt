cmake_minimum_required(VERSION 3.8)
project(fairino5_v6_robot_moveit_config)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(trajectory_msgs REQUIRED)

# =============================================================================
# EXECUTABLE TARGETS DEFINITION
# =============================================================================

# PROGRAM 1: move_single_joint
add_executable(move_single_joint_node src/move_single_joint.cpp)
ament_target_dependencies(move_single_joint_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)

# PROGRAM 2: move_base
add_executable(move_base_node src/move_base.cpp)
ament_target_dependencies(move_base_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)


# PROGRAM 3: move_pick_place
add_executable(move_pick_place_node src/move_pick_place.cpp)
ament_target_dependencies(move_pick_place_node
  rclcpp
  moveit_ros_planning_interface
  trajectory_msgs
)

# =============================================================================
# INSTALLATION RULES
# =============================================================================

install(TARGETS 
  move_single_joint_node 
  move_base_node
  move_pick_place_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config launch DESTINATION share/${PROJECT_NAME})

ament_package()