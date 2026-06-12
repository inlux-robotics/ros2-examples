#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <memory>

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto const node = std::make_shared<rclcpp::Node>("fairino_direct_move", node_options);
  auto const logger = rclcpp::get_logger("fairino_direct_move");

  // Creamos un cliente directo para el topic de los motores del brazo real
  auto trajectory_publisher = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
    "/fairino5_controller/joint_trajectory", 10);

  RCLCPP_INFO(logger, "Inicializando interfaz de planificación...");
  moveit::planning_interface::MoveGroupInterface move_group_interface(node, "fairino5_v6_group");

  // Posición segura (Tu postura real + variación en j6)
std::vector<double> target_joint_group_positions = {0.785, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  move_group_interface.setJointValueTarget(target_joint_group_positions);

  RCLCPP_INFO(logger, "Calculando trayectoria matemática con MoveIt2...");
  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  bool success = (move_group_interface.plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (success) {
    RCLCPP_INFO(logger, "¡Planificación con ÉXITO! Puenteando MoveIt y enviando directamente al robot real...");
    
    // Extraemos la trayectoria calculada y la disparamos directo al driver de Fairino
// Extraemos la trayectoria calculada y la disparamos directo al driver de Fairino
    auto trajectory_msg = my_plan.trajectory_.joint_trajectory;
    
    // Forzamos a que empiece YA (tiempo cero absoluto)
    trajectory_msg.header.stamp = rclcpp::Time(0, 0, node->get_clock()->get_clock_type());
    
    // Reseteamos las marcas de tiempo de los puntos intermedios para que la simulación los devore en tiempo real
    for (size_t i = 0; i < trajectory_msg.points.size(); ++i) {
        trajectory_msg.points[i].time_from_start = rclcpp::Duration::from_seconds(i * 0.1); 
    }    
    // Publicamos en el bus de los motores
    trajectory_publisher->publish(trajectory_msg);
    
    RCLCPP_INFO(logger, "¡Bomba enviada al bus de ros2_control! Revisa el brazo físico.");
    rclcpp::sleep_for(std::chrono::seconds(2)); // Damos tiempo a que se vacíe el buffer
  } else {
    RCLCPP_ERROR(logger, "El planificador no pudo calcular la trayectoria.");
  }

  rclcpp::shutdown();
  return 0;
}
