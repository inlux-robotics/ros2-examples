import os
from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description():
    # Cargamos el robot oficial
    moveit_config = (
        MoveItConfigsBuilder("fairino5_v6_robot", package_name="fairino5_v6_moveit2_config")
        .to_moveit_configs()
    )
    
    moveit_params = moveit_config.to_dict()
    
    # TRUCO MAESTRO: Inyectamos a la fuerza la asignación del controlador en los parámetros del nodo
    moveit_params["moveit_managed_controllers"] = ["fairino5_controller"]
    moveit_params["moveit_simple_controller_manager"] = {
        "controller_names": ["fairino5_controller"],
        "fairino5_controller": {
            "type": "joint_trajectory_controller/JointTrajectoryController",
            "joints": ["j1", "j2", "j3", "j4", "j5", "j6"]
        }
    }

    run_move_node = Node(
        package="fairino5_v6_robot_moveit_config",
        executable="test_move_node",
        output="screen",
        parameters=[moveit_params],
    )

    return LaunchDescription([run_move_node])
