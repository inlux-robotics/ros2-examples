import os
from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description():
    # Load the official robot MoveIt configuration package
    moveit_config = (
        MoveItConfigsBuilder("fairino5_v6_robot", package_name="fairino5_v6_moveit2_config")
        .to_moveit_configs()
    )
    
    moveit_params = moveit_config.to_dict()
    
    # MASTER PATCH: Forcefully inject the controller manager specifications into the node parameters
    moveit_params["moveit_managed_controllers"] = ["fairino5_controller"]
    moveit_params["moveit_simple_controller_manager"] = {
        "controller_names": ["fairino5_controller"],
        "fairino5_controller": {
            "type": "joint_trajectory_controller/JointTrajectoryController",
            "joints": ["j1", "j2", "j3", "j4", "j5", "j6"]
        }
    }

    # Define the execution node mapping to the English refactored base movement executable
    run_move_node = Node(
        package="fairino5_v6_robot_moveit_config",
        executable="move_base_node",
        output="screen",
        parameters=[moveit_params],
    )

    return LaunchDescription([run_move_node])