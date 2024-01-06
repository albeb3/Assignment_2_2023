#include "ros/ros.h"
#include <stdlib.h>
#include <stdio.h>
#include "assignment_2_2023/Last_target.h"
#include "assignment_2_2023/PlanningActionGoal.h"
float x=0,y=0;
void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	x=msg->goal.target_pose.pose.position.x;
	y=msg->goal.target_pose.pose.position.y;
	
}
bool send_position(assignment_2_2023::Last_target::Request &req, assignment_2_2023::Last_target::Response &res) {
    // Implementazione per ottenere le coordinate dell'ultimo goal inviato
    res.goal_x = x;  // Assegna la coordinata x del goal
    res.goal_y = y;  // Assegna la coordinata y del goal

    return true;  // Restituisce true per indicare che il servizio è stato gestito correttamente
}



int main (int argc, char **argv){
		ros::init(argc,argv,"last_target_service");
		ros::NodeHandle n;
		
		ros::Subscriber sub = n.subscribe<assignment_2_2023::PlanningActionGoal>("/reaching_goal/goal", 1, target_position);
		ros::ServiceServer service=n.advertiseService("/last_target_service",send_position);
		ros::spin();
		return 0;
}
