# Research track- Assignment 2

This repository contains a set of ROS nodes designed to accomplish specific tasks within a robot simulation environment. The implementation fulfills the following requirements:

## Task Description

### Node 1: Action Client Node
- **Functionality:**
  - Allows users to set a target (x, y) or cancel it.
  - Utilizes action server feedback/status to determine target achievement.
  - Publishes robot position and velocity as a custom message (x, y, vel_x, vel_z) based on the topic `/odom`.

### Node 2: Service Node (Retrieve Last Target Coordinates)
- **Functionality:**
  - When called, returns the coordinates of the last target set by the user.

### Node 3: Service Node (Robot's Distance & Average Speed)
- **Functionality:**
  - Subscribes to robot position and velocity through a custom message.
  - Computes the distance from the target and the robot's average speed.
  - Parameterized with the averaging window size for speed calculations.

### Launch File
- Orchestrates the execution of all three nodes.
- Configures the parameter for the third node to determine the averaging window size for speed calculation.
---
To run the project, follow these steps:

1. **Clone this repository to your system.**
    ```bash
    git clone https://github.com/albeb3/Assignment_2_2023.git
    ```

2. **Install the necessary dependencies by running `catkin_make`.**

3. **Launch the main node using the provided launch file:**
    ```bash
    roslaunch assignment_2_2023 assignment2.launch
    ```

4. **To use the service, execute:**
    ```bash
    rosservice call /dist_vel_service
	rosservice call /last_target_service
    ```

---

### Node 1: Action Client Node (`client.cpp`)
- **Language and Files:**
  - Implemented in C++.
  - Located in the `src` folder named `client.cpp`.
  - Utilizes a message file named `Posvel.h`, containing fields `x`, `y`, `vel_x`, and `vel_y`.
- **Message Structure:**
  - Created a custom message file (`Posvel.h`) with `float64` fields (`x`, `y`, `vel_x`, and `vel_y`).
  - The decision to use `vel_y` instead of `vel_z` was made for coherence with the third node's requirement for calculating average speed, ensuring consistency within the system's data structure.
- **Initialization:**
  - Initializes a ROS node, sets up a publisher to send information using the custom message, and subscribes to the `/odom` topic via the `odomCallback` function to gather robot position and velocity data.
  - Utilizes `std::thread` functionality to enable continuous transmission by the publisher while allowing user interaction without blocking the execution.
- **Functionality:**
  - **Target Handling:**
    - Initializes a `busy` variable as `false`.
    - Prompts the user to input target/goal coordinates initially.
    - Changes `busy` to `true` after acquiring the target coordinates.
  - **User Interaction:**
    - Based on the `busy` state, prompts the user to either cancel the goal or check if the goal has been reached.
  - **Goal Transmission:**
    - After populating the goal field of the action message with the target coordinates, the client sends this data using the `sendGoal()` functionality to the server.
  - **Goal Status Check:**
    - Implements code to request the server's goal status using the `getState` functionality when the user checks if the goal is reached.

---

### Node 2: Service Node (`last_target.cpp`)
- **Language and Files:**
  - Implemented in C++.
  - Located in the `src` folder named `last_target.cpp`.
  - Utilizes a service file (`.srv`) containing a response with two `float32` fields: `goal_x` and `goal_y`, without a request field.
- **Service Structure:**
  - Created a service file defining the service structure without a request but containing `goal_x` and `goal_y` as response fields of type `float32`.
- **Initialization:**
  - Initializes the `last_target_service` node.
  - Sets up a subscriber to listen to the `/reaching_goal/goal` topic, extracting goal coordinates using the `target_position` callback function.
- **Service Server:**
  - Establishes a `ServiceServer` named `/last_target_service` to respond to service requests.
  - Implements the `send_position` function, retrieving and returning the last goal's coordinates upon service request.
- **Observations:**
  - The `target_position` callback captures goal coordinates upon receiving messages from the `/reaching_goal/goal` topic.
  - The `send_position` function responds to service requests on `/last_target_service` by providing the last received goal's coordinates.

---

### Node 3: Service Node (`dist_vel.cpp`)
- **File Structure:**
  - Implemented in C++.
  - Located in `src` folder as `dist_vel.cpp`.
  - Utilizes a custom `.srv` file named `Dist_vel` located in the `srv` folder.
- **Service Structure:**
  - The `Dist_vel` service file defines the service structure without a request but containing the distance from the goal and the robot's average speed as response fields of type `float32`.
- **Initialization:**
  - Initializes two subscribers:
    - Subscribes to `/reaching_goal/goal` to obtain target coordinates via the `target_position` callback.
    - Subscribes to `/posvel` to retrieve robot's coordinates and velocity using `dataCallback`.
- **Functionality:**
  - **`target_position` Callback:**
    - Extracts and stores the goal's coordinates in `goal_x` and `goal_y` variables.
  - **`dataCallback` Function:**
    - Gathers robot's coordinates (position_x, position_y) and velocity (vel_x, vel_y).
    - Utilizes two functions inside this callback:
      - Calculates the distance between the robot and the goal.
      - Computes the average speed based on a vector with a dimension set by `get:param` from the launch file.
  - **Average Speed Calculation:**
    - Utilizes a vector to store the velocity values obtained from the callback.
    - Implements counters for managing vector updates with the modulo operator.
    - Calculates the average speed by summing up values in the array and dividing by the total number of values.
  - **ServiceServer:**
    - Establishes `/dist_vel_service` as a `ServiceServer` to retrieve average speed and distance from the goal.
    - The `send_info` function populates the fields of the `Dist_vel` service message with velocity and distance values obtained from `dataCallback`.

    
![rosrun rqt_graph rqt_graph](https://github.com/albeb3/Assignment_2_2023/blob/main/nodiassignment%20RT.jpg)

