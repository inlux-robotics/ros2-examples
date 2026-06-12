#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <memory>
#include <vector>

// Función auxiliar para ahorrarnos repetir código por cada punto
void enviar_trayectoria(
  const std::vector<double>& joints, 
  moveit::planning_interface::MoveGroupInterface& move_group,
  rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr publisher,
  const rclcpp::Logger& logger,
  std::shared_ptr<rclcpp::Node> node)
{
  move_group.setJointValueTarget(joints);
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  
  if (move_group.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
    auto msg = plan.trajectory_.joint_trajectory;
    msg.header.stamp = rclcpp::Time(0, 0, node->get_clock()->get_clock_type());
    
    for (size_t i = 0; i < msg.points.size(); ++i) {
        msg.points[i].time_from_start = rclcpp::Duration::from_seconds(i * 0.1); 
    }
    
    publisher->publish(msg);
    RCLCPP_INFO(logger, "¡Trayectoria enviada con éxito!");
  } else {
    RCLCPP_ERROR(logger, "Error al planificar el punto.");
  }
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto const node = std::make_shared<rclcpp::Node>("fairino_3_puntos", node_options);
  auto const logger = rclcpp::get_logger("fairino_3_puntos");

  auto trajectory_publisher = node->create_publisher<trajectory_msgs::msg::JointTrajectory>(
    "/fairino5_controller/joint_trajectory", 10);

  RCLCPP_INFO(logger, "Inicializando secuencia de 3 puntos...");
  moveit::planning_interface::MoveGroupInterface move_group_interface(node, "fairino5_v6_group");

  // DEFINICIÓN DE LOS 3 PUNTOS (j1, j2, j3, j4, j5, j6)
  std::vector<double> punto_A = {0.5, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};
  std::vector<double> punto_B = {-0.5, -1.570796, 1.570796, -1.570796, -1.570796, 0.4};
  std::vector<double> punto_C = {0.0, -1.570796, 1.570796, -1.570796, -1.570796, 0.0};

  // --- EJECUCIÓN SECUENCIAL ---
  
  RCLCPP_INFO(logger, ">> Ejecutando PUNTO A...");
  enviar_trayectoria(punto_A, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4)); // Tiempo para que el simulador termine el movimiento

  RCLCPP_INFO(logger, ">> Ejecutando PUNTO B...");
  enviar_trayectoria(punto_B, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  RCLCPP_INFO(logger, ">> Ejecutando PUNTO C (Home)...");
  enviar_trayectoria(punto_C, move_group_interface, trajectory_publisher, logger, node);
  rclcpp::sleep_for(std::chrono::seconds(4));

  RCLCPP_INFO(logger, "¡Secuencia de 3 puntos completada!");
  rclcpp::shutdown();
  return 0;
}
