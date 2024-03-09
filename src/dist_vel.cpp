/**
* \file dist_vel.cpp
* \brief Service Node
* \author Alberto Bono 3962994
* \date 04/03/2024
*
* \details
* Subscribes to:<BR>
*		/posvel
*		/reaching_goal/goal
*		
* Server:<BR>
*		/dist_vel_service
*
* Description:<BR>
* Subscribes to robot position and velocity through a custom message.
* Computes the distance from the target and the robot's average speed.
* Parameterized with the averaging window size for speed calculations.
**/

#include <ros/ros.h>
#include <assignment_2_2023/PlanningAction.h>
#include "assignment_2_2023/Dist_vel.h"
#include <assignment_2_2023/Posvel.h> 
#include <assignment_2_2023/Dimarray.h>
#include <math.h>

int max_values=0;///< variables for the dimension of the array for handling the average speed and its dimension taken as a parameter
float *velocity_values=nullptr;///< variables for the array for handling the average speed 								*/
float goal_x=0.0,goal_y=0.0;///< variables for the target position	
float position_x=0.0, position_y=0.0, vel_x=0.0, vel_y=0.0;///< variables for the robot position and the robot speed
float ave_speed=0.0,dist=0.0; ///< variables for the robot  distance from the target and the robot average speed
int count = 0,ind = 0;///< variables for manage the code


/**		
* \brief Service Function to Send Distance and Average Velocity Information
* \param req(assignment_2_2023::Dist_vel::Request), res(assignment_2_2023::Dist_vel::Response)
*/	
bool send_info(assignment_2_2023::Dist_vel::Request &req, assignment_2_2023::Dist_vel::Response &res) {
    res.distance = dist; 
    res.average_velocity = ave_speed;  
    return true;  
}


/**		
* \brief Auxiliary function to calculate the distance between two points
* \param position_x position_y goal_x goal_y
* \result distance between target and position of the robot
*/		
float distance(float position_x, float position_y, float goal_x, float goal_y){
    return sqrt(pow((goal_x - position_x), 2.0) + pow((goal_y - position_y), 2.0));
}

/**		
* \brief Auxiliary function to calculate the average speed of a robot based on an array of dim elements
* \param v_x, v_y
* \result average speed
*/	
float average_speed(float v_x, float v_y){
    float sum = 0.0;
    velocity_values[ind] = sqrt(pow(vel_x, 2.0) + pow(vel_y, 2.0));
    ind = (ind + 1) % max_values;
    for (int i = 0; i < max_values; ++i) {sum += velocity_values[i];}
    if (count < max_values) {return sum / (ind + 1);} 
    else {return sum / max_values;}
}

/**		
* \brief Callback of sub subscriber receives PlanningActionGoal message from /reaching_goal/goal topic which fills the global variables for the target position
* \param msg(assignment_2_2023::PlanningActionGoal)
*/	
void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	goal_x=msg->goal.target_pose.pose.position.x;
	goal_y=msg->goal.target_pose.pose.position.y;
}

/**		
* \brief Callback  for Monitoring Robot Position and Velocity Information
* \param data(assignment_2_2023::Posvel)
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

/**		
* \brief Main function that manage the speed array dimension from parameter sent from launch file
*/
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

