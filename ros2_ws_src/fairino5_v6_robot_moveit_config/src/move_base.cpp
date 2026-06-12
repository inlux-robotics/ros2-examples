#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <memory>
#include <vector>

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto const node = std::make_shared<rclcpp::Node>("fairino_move_base", node_options);
  auto const logger = rclcpp::get_logger("fairino_move_base");

  auto trajectory_publisher = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
      "/fairino5_controller/joint_trajectory", 10);

  RCLCPP_INFO(logger, "Starting base motion profile execution...");
  moveit::planning_interface::MoveGroupInterface move_group_interface(node, "fairino5_v6_group");

  // TARGET CONFIGURATION: Shift base joint (j1 = 1.5 rad), keep default home posture for the arm
  std::vector<double> target_joint_group_positions = {1.5, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  move_group_interface.setJointValueTarget(target_joint_group_positions);

  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  bool success = (move_group_interface.plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (success)
  {
    RCLCPP_INFO(logger, "Motion profile computed successfully. Injecting to hardware bus...");
    auto trajectory_msg = my_plan.trajectory_.joint_trajectory;
    trajectory_msg.header.stamp = rclcpp::Time(0, 0, node->get_clock()->get_clock_type());

    for (size_t i = 0; i < trajectory_msg.points.size(); ++i)
    {
      trajectory_msg.points[i].time_from_start = rclcpp::Duration::from_seconds(i * 0.1);
    }

    trajectory_publisher->publish(trajectory_msg);
    RCLCPP_INFO(logger, "New trajectory sequence successfully injected.");
    rclcpp::sleep_for(std::chrono::seconds(2));
  }
  else
  {
    RCLCPP_ERROR(logger, "Failed to calculate motion plan for base joint rotation.");
  }

  rclcpp::shutdown();
  return 0;
}