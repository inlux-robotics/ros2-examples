import os
from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("fairino5_v6_robot", package_name="fairino5_v6_moveit2_config")
        .to_moveit_configs()
    )
    moveit_params = moveit_config.to_dict()

    # Lanzamos el segundo nodo ejecutable
    run_move_node = Node(
        package="fairino5_v6_robot_moveit_config",
        executable="mover_base_node",
        output="screen",
        parameters=[moveit_params],
    )

    return LaunchDescription([run_move_node])
