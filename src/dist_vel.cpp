/*
	Author: albeb3
	Date: 11/01/2024
	Student ID: 3962994
	
	Description:
  		- Subscribes to robot position and velocity through a custom message.
  		- Computes the distance from the target and the robot's average speed.
  		- Parameterized with the averaging window size for speed calculations.
*/
//====================================HEADERS=========================================
#include <ros/ros.h>

/*		Inclusion of headers for using custom message, service and action message 				*/
#include <assignment_2_2023/PlanningAction.h>
#include "assignment_2_2023/Dist_vel.h"
#include <assignment_2_2023/Posvel.h> 

/*		Inclusion of header for using mathematical functions									*/
#include <math.h>

//====================================GLOBAL VARIABLES=========================================
/*		Declaration of global variable for the array and its dimension taken as a parameter		*/
int max_values=0;
float *velocity_values=nullptr;

/*		Declaration of global variables for the target position									*/
float goal_x=0.0,goal_y=0.0;

/*		Declaration of global variables for the robot position and the robot speed				*/
float position_x=0.0, position_y=0.0, vel_x=0.0, vel_y=0.0;

/*		Declaration of global variables 
		for the robot distance from the target and the robot average speed						*/
float ave_speed=0.0,dist=0.0;

/*		Declaration of global variables for the program											*/
int count = 0,ind = 0;

//====================================FUNCTIONS=================================================
/*
		Service Function to Send Distance and Average Velocity Information
*/
bool send_info(assignment_2_2023::Dist_vel::Request &req, assignment_2_2023::Dist_vel::Response &res) {
    res.distance = dist; 
    res.average_velocity = ave_speed;  
    return true;  
}

/*		
		Auxiliary function to calculate the distance between two points
*/
float distance(float position_x, float position_y, float goal_x, float goal_y){
    return sqrt(pow((goal_x - position_x), 2.0) + pow((goal_y - position_y), 2.0));
}

/*		
		Auxiliary function to calculate the average speed of a robot based on an array of dim elements
*/
float average_speed(float v_x, float v_y){
    float sum = 0.0;
    velocity_values[ind] = sqrt(pow(vel_x, 2.0) + pow(vel_y, 2.0));
    ind = (ind + 1) % max_values;
    for (int i = 0; i < max_values; ++i) {sum += velocity_values[i];}
    if (count < max_values) {return sum / (ind + 1);} 
    else {return sum / max_values;}
}

/*		Callback of sub subscriber receives PlanningActionGoal message from /reaching_goal/goal topic
		which fills the global variables for the target position
*/
void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	goal_x=msg->goal.target_pose.pose.position.x;
	goal_y=msg->goal.target_pose.pose.position.y;
}

/*
		Callback Function for Monitoring Robot Position and Velocity Information
*/
void dataCallback(const assignment_2_2023::Posvel::ConstPtr& data){
    position_x = data->x; 
    position_y = data->y; 
    vel_x = data->vel_x; 
    vel_y = data->vel_y; 
    dist = distance(position_x, position_y, goal_x, goal_y);
    ave_speed = average_speed(vel_x, vel_y);
    count++; 
}

//====================================MAIN========================================================
int main(int argc, char **argv){
    
    ros::init(argc, argv, "dist_vel_service");
    ros::NodeHandle nh;
    
	ros::Subscriber sub = nh.subscribe("/posvel", 1, dataCallback);
	ros::Subscriber sub_goal = nh.subscribe<assignment_2_2023::PlanningActionGoal>("/reaching_goal/goal", 1, target_position);
	
	ros::ServiceServer service=nh.advertiseService("/dist_vel_service",send_info);
	
	if (ros::param::get("/dist_vel_service/max_values", max_values)) {
    	
    	if (max_values > 0) {
        	velocity_values = new float[max_values];
    	} 
    	else {
        	ROS_ERROR("Invalid value for 'averaging_window'. It should be a positive integer.");
        	return 1; 
    	}
    }
    else {
        	ROS_ERROR("Failed to retrieve 'averaging_window' parameter.");
        	return 1; 
    	}
    
    ros::spin();
	delete[] velocity_values;
    return 0;
}

