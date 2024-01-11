/*
	Author: albeb3
	Date: 11/01/2024
	Student ID: 3962994
	
	Description:
  		- Allows users to set a target (x, y) or cancel it.
  		- Utilizes action server feedback/status to determine target achievement.
  		- Publishes robot position and velocity as a custom message (x, y, vel_x, vel_z) based on the topic `/odom`.
*/
//====================================HEADERS==============================================
#include <ros/ros.h>
/*		Inclusion of headers for using custom message and action message 				*/
#include <actionlib/client/simple_action_client.h>
#include <assignment_2_2023/PlanningAction.h>
#include <assignment_2_2023/Posvel.h>
/*		Inclusion of headers for using ros messages 									*/
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
/*		Inclusion of headers for using input from keyboard 								*/
#include <iostream>
/*		Inclusion of headers for using thread  											*/
#include <thread>
//====================================GLOBAL VARIABLES=======================================
/*		Declaration of global variables for robot position and robot speed				*/
float robot_x = 0.0, robot_y = 0.0, robot_vel_x = 0.0, robot_vel_y = 0.0;
/*		Declaration of global variable for managing the condition block					*/
bool  busy=false;
/*		Creating a ROS publisher for position and speed of robot						*/
ros::Publisher pos_publisher;
//====================================FUNCTIONS===============================================
/*		Callback of sub subscriber receives Odometry message from /odom topic
		which fills the global variables for the robot position and robot speed
*/
void odomCallback(const nav_msgs::Odometry::ConstPtr &msg) {
    robot_x = msg->pose.pose.position.x;
    robot_y = msg->pose.pose.position.y;
    robot_vel_x = msg->twist.twist.linear.x;
    robot_vel_y = msg->twist.twist.linear.y;
}

/*		Function to handle user input for sending goals to a ROS action server
*/
void userInput() {
	// Creating a ROS action client for the PlanningAction
    actionlib::SimpleActionClient<assignment_2_2023::PlanningAction> client("/reaching_goal", true);
	ROS_INFO("Waiting for action server to start...");
    client.waitForServer();
    ROS_INFO("Action server started!");
    float goal_x, goal_y;
	char input=' ';
	// Main loop for user interaction    	
    while (ros::ok() && !ros::isShuttingDown()) {
		// Action has finished, get and print the status
		if(client.waitForResult(ros::Duration(0.1)) && busy){ 
			actionlib::SimpleClientGoalState state = client.getState();//receive the status
   	 		ROS_INFO("Action finished: %s \n",state.toString().c_str());
			busy=false;
		}
		// Insertion of the goal coordinates
		else{
			if(!busy){
        		ROS_INFO("INSERT THE GOAL COORDINATE (x,y) \n");
        		ROS_INFO("Insert 'x' coordinate, then press ENTER: ");
        		if (!(std::cin >> goal_x)) {
            		ROS_INFO("Invalid input for 'x' coordinate");
            		std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
           			continue;
        		}
				ROS_INFO("Insert 'y' coordinate, then press ENTER: ");
        		if (!(std::cin >> goal_y)) {
            		ROS_INFO("Invalid input for 'y' coordinate");
            		std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            		continue;
        		}
        		ROS_INFO("Sending the goal");
				assignment_2_2023::PlanningGoal goal;
        		goal.target_pose.pose.position.x = goal_x;
        		goal.target_pose.pose.position.y = goal_y;
        		client.sendGoal(goal);
        		busy=true;	
        	}
        	// Options when a goal is in progress to allow the user to cancel or check the goal
        	else if(busy){
        		ROS_INFO("->Press 'x' to cancel the goal, then press Enter:");
        		ROS_INFO("->Press 'c' to check if the goal has been reached, then press Enter:");
        		std::cin >> input;
        		if (input=='x'){
        			client.cancelGoal();
        			ROS_INFO("Goal cancelled!");
        			busy=false;
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
           			continue;	
        		}
        		//condition for restart the loop
        		else if(busy && input=='c') {
        			ROS_INFO("wait for the result try again");
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		}
        		else {
        			ROS_INFO("Invalid input!");
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		}
        	}
		}
	}
}

//====================================MAIN=================================
int main(int argc, char **argv) {
	
    ros::init(argc, argv, "goal_sender_node");
    ros::NodeHandle nh;
    
	ros::Rate rate(1);
	rate.sleep();
	
    pos_publisher = nh.advertise<assignment_2_2023::Posvel>("/posvel", 1000);
    ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("/odom", 1, odomCallback);

    std::thread input_thread(userInput);
   

    while (ros::ok()) {
        assignment_2_2023::Posvel pos_msg;
        pos_msg.x = robot_x;
        pos_msg.y = robot_y;
        pos_msg.vel_x = robot_vel_x;
        pos_msg.vel_y = robot_vel_y;
        pos_publisher.publish(pos_msg);

        rate.sleep();
        ros::spinOnce();
    }

    input_thread.join();
    return 0;
}

