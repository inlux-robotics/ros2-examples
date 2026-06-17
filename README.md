# ROS 2 MoveIt Examples for Fairino 5 V6

This repository contains practical examples and configurations for controlling the Fairino 5 V6 robot manipulator using ROS 2 and MoveIt 2. 

The examples demonstrate how to compute valid mathematical trajectories through MoveIt 2 and inject the resulting raw trajectory frames directly into the `ros2_control` hardware driver bus.

---

## Prerequisites

* **OS:** Ubuntu 22.04 LTS
* **ROS 2 Distribution:** Humble Hawksbill (Desktop-Full installation)
* **MoveIt 2**

Robot Drivers: The fairino5_controller or a valid simulation stack must be running and subscribing to /fairino5_controller/joint_trajectory.


## Repository Structure

```bash

📦 ros2-moveit-integration
┣ 📂 config                  # MoveIt configuration profiles, kinematics solvers, and joint constraints
┣ 📂 launch                  # Orchestration launch targets and automated execution pipelines
┃ ┣ 📜 move_base.launch.py
┃ ┣ 📜 move_single_joint.launch.py
┃ ┗ 📜 move_pick_place.launch.py
┣ 📂 media                   # Reference validation clips and hardware execution records
┗ 📂 src                     # Native C++ motion planning source nodes
  ┣ 📜 move_base.cpp         # Joint space rotation profile benchmarks (fairino_move_base)
  ┣ 📜 move_single_joint.cpp # Deterministic single-joint displacement bounds (fairino_single_joint)
  ┗ 📜 move_pick_place.cpp   # Cartesian pick-and-place sequence interpolation (fairino_pick_place)

```
              
Installation & Building

1. Clone this repository into the src directory of your ROS 2 workspace:

```bash
mkdir -p ~/inlux_ws/src
cd ~/inlux_ws/src
git clone [https://github.com/inlux-robotics/ros2-examples.git](https://github.com/inlux-robotics/ros2-examples.git)
```


2. Install dependencies:

```bash
cd ~/inlux_ws
rosdep install --from-paths src --ignore-src -r -y
```


3. Build the workspace:

```bash
colcon build --symlink-install
```


4. Source the setup files:

source install/setup.bash

Running the Examples
Ensure your Fairino 5 simulation or hardware controller interface is actively running before executing any of the following nodes.

```bash

  ros2 launch fairino_mtc_demo mtc_demo_env.launch.py
```

1. Base Motion Profile (move_base.cpp)
Rotates the base joint (j1) to 1.5 rad, keeping the remaining joints in the default home posture.

```bash
  ros2 launch fairino5_v6_robot_moveit_config move_base.launch.py

```


[![Watch the video](https://img.youtube.com/vi/cKOAUhq4eBk/0.jpg)](https://youtu.be/cKOAUhq4eBk)


2. Single Joint Safe Rotation (test_move.cpp)
Executes a safe rotation of 0.785 rad (45°) on the first joint.

```bash
  ros2 launch fairino5_v6_robot_moveit_config move_single_joint.launch.py

```

[![Watch Single Joint Motion Video](https://img.youtube.com/vi/nVjCMH9TRiQ/0.jpg)](https://youtu.be/nVjCMH9TRiQ)
    

3. Pick and Place Sequence (mover_pick_place.cpp)
Runs a full Cartesian pick‑and‑place routine, including grasping and joint‑space transitions.

```bash
  ros2 launch fairino5_v6_robot_moveit_config move_pick_place.launch.py

```

[![Watch Pick and Place Video](https://img.youtube.com/vi/1sjY4zWZriE/0.jpg)](https://youtu.be/1sjY4zWZriE)

The resulting JointTrajectory message is published directly to:
/fairino5_controller/joint_trajectory

```bash
 ros2 topic echo /fairino5_controller/joint_trajectory

```

<a href="ros2_ws_src/media/joint_trajectory.png" target="_blank">
  <img src="ros2_ws_src/media/joint_trajectory.png" width="450" alt="Robots">
</a>


Contact & Support
Email: support@inluxrobotics.com
