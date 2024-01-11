/*
	Author: albeb3
	Date: 11/01/2024
	Student ID: 3962994
	
	Description:
  		- When called, returns the coordinates of the last target set by the user.
*/
//====================================HEADERS==============================
#include "ros/ros.h"
/*		Inclusion of headers for using custom service and action message */
#include "assignment_2_2023/Last_target.h"
#include "assignment_2_2023/PlanningActionGoal.h"

//====================================GLOBAL VARIABLES=====================
/*		Declaration of global variables for the target position				*/				
float x=0,y=0;

//====================================FUNCTIONS============================
/*		Callback of sub subscriber receives PlanningActionGoal message from /reaching_goal/goal topic
		which fills the global variables for the target position
*/			
void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	x=msg->goal.target_pose.pose.position.x;
	y=msg->goal.target_pose.pose.position.y;
}

/*
		Service callback which fills the response message of the goal position with the values 
		of the variables coming from target_position
*/
bool send_position(assignment_2_2023::Last_target::Request &req, assignment_2_2023::Last_target::Response &res){
    res.goal_x = x;  // Assigns the x-coordinate of the goal
    res.goal_y = y;  // Assigns the y-coordinate of the goal
    return true; // Returns true to indicate that the service has been handled correctly
}

//====================================MAIN=================================
int main (int argc, char **argv){
		
		ros::init(argc,argv,"last_target_service");
		ros::NodeHandle n;
		
		ros::Subscriber sub = n.subscribe<assignment_2_2023::PlanningActionGoal>("/reaching_goal/goal", 1, target_position);
		ros::ServiceServer service=n.advertiseService("/last_target_service",send_position);
		ros::spin();
		
		return 0;
}
