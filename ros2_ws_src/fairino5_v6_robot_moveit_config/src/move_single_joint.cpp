#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <memory>
#include <vector>

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto const node = std::make_shared<rclcpp::Node>("fairino_single_joint", node_options);
  auto const logger = rclcpp::get_logger("fairino_single_joint");

  // Create a direct publisher to the robot joint trajectory controller topic
  auto trajectory_publisher = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
      "/fairino5_controller/joint_trajectory", 10);

  RCLCPP_INFO(logger, "Initializing planning interface...");
  moveit::planning_interface::MoveGroupInterface move_group_interface(node, "fairino5_v6_group");

  // Safe position target configuration (j1 rotated 45 degrees, rest at default)
  std::vector<double> target_joint_group_positions = {0.785, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  move_group_interface.setJointValueTarget(target_joint_group_positions);

  RCLCPP_INFO(logger, "Computing mathematical trajectory via MoveIt2...");
  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  bool success = (move_group_interface.plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (success)
  {
    RCLCPP_INFO(logger, "Planning SUCCESSFUL! Bypassing MoveIt execution manager...");

    // Extract calculated trajectory and inject it directly into the Fairino driver bus
    auto trajectory_msg = my_plan.trajectory_.joint_trajectory;

    // Force immediate execution by resetting the header timestamp to absolute zero
    trajectory_msg.header.stamp = rclcpp::Time(0, 0, node->get_clock()->get_clock_type());

    // Respace intermediate waypoint timestamps for real-time processing by the simulator
    for (size_t i = 0; i < trajectory_msg.points.size(); ++i)
    {
      trajectory_msg.points[i].time_from_start = rclcpp::Duration::from_seconds(i * 0.1);
    }

    // Publish directly to the hardware controller topic
    trajectory_publisher->publish(trajectory_msg);

    RCLCPP_INFO(logger, "Trajectory frame injected into ros2_control bus.");
    rclcpp::sleep_for(std::chrono::seconds(2)); // Allow buffer to clear completely
  }
  else
  {
    RCLCPP_ERROR(logger, "The motion planner was unable to compute a valid trajectory.");
  }

  rclcpp::shutdown();
  return 0;
}