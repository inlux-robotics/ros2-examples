#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <memory>
#include <vector>

// Helper function to handle planning and sending trajectories to the robot control bus
void planAndExecute(
  const std::vector<double>& joint_targets, 
  moveit::planning_interface::MoveGroupInterface& move_group,
  rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr publisher,
  const rclcpp::Logger& logger,
  std::shared_ptr<rclcpp::Node> node)
{
  // Set the target joint positions for the current step
  move_group.setJointValueTarget(joint_targets);
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  
  // Plan the mathematical trajectory to avoid sudden jerks or collisions
  if (move_group.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
    auto trajectory_msg = plan.trajectory_.joint_trajectory;
    
    // Critical Patch: Force start time to absolute zero to prevent virtual machine clock desync
    trajectory_msg.header.stamp = rclcpp::Time(0, 0, node->get_clock()->get_clock_type());
    
    // Smoothly space out target execution points in real-time increments
    for (size_t i = 0; i < trajectory_msg.points.size(); ++i) {
        trajectory_msg.points[i].time_from_start = rclcpp::Duration::from_seconds(i * 0.1); 
    }
    
    // Directly bypass the faulty execution manager and inject trajectory to the hardware controller
    publisher->publish(trajectory_msg);
    RCLCPP_INFO(logger, "Trajectory sent successfully to the control bus.");
  } else {
    RCLCPP_ERROR(logger, "MoveIt planning failed for this target position.");
  }
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto const node = std::make_shared<rclcpp::Node>("fairino_pick_place", node_options);
  auto const logger = rclcpp::get_logger("fairino_pick_place");

  // Create publisher to talk directly with the robot actuator controller
  auto trajectory_publisher = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
    "/fairino5_controller/joint_trajectory", 10);

  RCLCPP_INFO(logger, "Initializing Pick and Place simulation sequence...");
  moveit::planning_interface::MoveGroupInterface move_group_interface(node, "fairino5_v6_group");

  // ==========================================================================================
  // JOINT POSITIONS CONFIGURATION: {j1, j2, j3, j4, j5, j6} in Radians
  // ==========================================================================================
  
  // 1. Safe Home Position (Starting point above the workspace)
  std::vector<double> home_pose   = {0.0, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  
  // 2. Pick Pose (Base stays at 0.0, arm extends forward/down to grab the object)
  std::vector<double> pick_pose   = {0.0, -1.20,     1.80,     -1.570796, -1.570796, 0.0};
  
  // 3. Transit Pose (Base rotates 90 degrees/1.57 rad, arm lifts to clear obstacles)
  std::vector<double> transit_pose = {1.57, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  
  // 4. Place Pose (Base stays at 1.57, arm lowers to release the object at destination)
  std::vector<double> place_pose  = {1.57, -1.20,     1.80,     -1.570796, -1.570796, 0.0};

  // ==========================================================================================
  // SEQUENTIAL EXECUTION LOOPS
  // ==========================================================================================
  
  // STEP 1: Move to initial Home
  RCLCPP_INFO(logger, "Executing STEP 1: Moving to HOME position...");
  planAndExecute(home_pose, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4)); // Wait for simulation to finish moving

  // STEP 2: Go down to Pick the object
  RCLCPP_INFO(logger, "Executing STEP 2: Lowering arm to PICK object...");
  planAndExecute(pick_pose, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  // STEP 3: Return to height and rotate to the drop-off zone
  RCLCPP_INFO(logger, "Executing STEP 3: Lifting and rotating to TRANSIT position...");
  planAndExecute(transit_pose, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  // STEP 4: Lower arm to Place the object
  RCLCPP_INFO(logger, "Executing STEP 4: Lowering arm to PLACE object...");
  planAndExecute(place_pose, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  // STEP 5: Return to Safe Home to finish cycle
  RCLCPP_INFO(logger, "Executing STEP 5: Returning to SAFE HOME...");
  planAndExecute(home_pose, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  RCLCPP_INFO(logger, "Pick and Place routine successfully completed!");
  rclcpp::shutdown();
  return 0;
}
