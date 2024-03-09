/**
* \file last_target.cpp
* \brief Service Node
* \author Alberto Bono 3962994
* \date 04/03/2024
*
* \details
* Subscribes to:<BR>
*		/reaching_goal/goal

* Server:<BR>
*		/last_target_service
*
* Description:<BR>
* When called, returns the coordinates of the last target set by the user.
**/


#include "ros/ros.h"
#include "assignment_2_2023/Last_target.h"
#include "assignment_2_2023/PlanningActionGoal.h"
			
float x=0,y=0; ///<  variables for the target position

/**		
* \brief Callback for the reaching_goal/goal subscriber
* \param msg(assignment_2_2023::PlanningActionGoal)
* \result x,y (target position)
*/			
void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	x=msg->goal.target_pose.pose.position.x;
	y=msg->goal.target_pose.pose.position.y;
}

/**		
* \brief Service callback: fills the response message of the goal position with the values of the variables coming from target_position
* \param req(assignment_2_2023::Last_target::Request), res(assignment_2_2023::Last_target::Response)
*/	
bool send_position(assignment_2_2023::Last_target::Request &req, assignment_2_2023::Last_target::Response &res){
    res.goal_x = x;  // Assigns the x-coordinate of the goal
    res.goal_y = y;  // Assigns the y-coordinate of the goal
    return true; // Returns true to indicate that the service has been handled correctly
}


int main (int argc, char **argv){
		
		ros::init(argc,argv,"last_target_service");
		ros::NodeHandle n;
		
		ros::Subscriber sub = n.subscribe<assignment_2_2023::PlanningActionGoal>("/reaching_goal/goal", 1, target_position);///< subscriber for the target position from /reaching_goal/goal
		ros::ServiceServer service=n.advertiseService("/last_target_service",send_position); ///< server /last_target_service 
		ros::spin();
		
		return 0;
}
