#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <assignment_2_2023/PlanningAction.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <assignment_2_2023/PosGoal.h>
#include <iostream>
#include <thread>
#include <actionlib/client/simple_action_client.h>
//ros::Publisher pos_publisher;
assignment_2_2023::PosGoal pos_goal;

const int ROWS=2;
const int COLS=5;
float pos_goals[ROWS][COLS];

float goal_x=0.0;
float goal_y=0.0;
int i=0;
int j=0;
void posgoalCallback(const assignment_2_2023::PosGoal::ConstPtr &msg) {
    if(i<COLS){
    	pos_goals[0][i] = msg->goal_x;
    	pos_goals[1][i] = msg->goal_y;
    	ROS_INFO("pos subscriber@[%f %f ]", pos_goals[0][i], pos_goals[1][i]);
    	
    	
    	
    	
    	
    	
    	
    	i++;   
    	}
}

void SendingCoordinate() {
	actionlib::SimpleActionClient<assignment_2_2023::PlanningAction> client("/reaching_goal", true);
	ROS_INFO("Waiting for action server to start...");
    client.waitForServer();
    ROS_INFO("Action server started!");
    
    while (ros::ok() && !ros::isShuttingDown()) {
    	ros::Rate rate(0.5);
		actionlib::SimpleClientGoalState state = client.getState();//receive the status
		assignment_2_2023::PlanningGoal goal;
		ROS_INFO("Action finished: %s \n",state.toString().c_str());
    	if(strcmp(state.toString().c_str(), "LOST") == 0){
    
	
		goal.target_pose.pose.position.x =pos_goals[0][0];
    	goal.target_pose.pose.position.y =pos_goals[1][0];
    	client.sendGoal(goal);
    	j++;
		}
	
		else if (strcmp(state.toString().c_str(), "SUCCEEDED") == 0){
	
		goal.target_pose.pose.position.x =pos_goals[0][j];
    	goal.target_pose.pose.position.y =pos_goals[1][j];
    	client.sendGoal(goal);
    	j++;
    	}
	

	
	rate.sleep();
	}
	
}



int main(int argc, char **argv) {
    ros::init(argc, argv, "reach_goals_node");
    ros::NodeHandle nh;
	ros::Subscriber sub = nh.subscribe<assignment_2_2023::PosGoal>("/posgoal", 1, posgoalCallback);
	//pos_publisher = nh.advertise<assignment_2_2023::Posgoal>("/posgoal", 1000);
	
	std::thread input_thread(SendingCoordinate);
    ros::Rate rate(1);
	while(ros::ok()){
		ros::spinOnce();
		}
	
	
	
	ros::spin();
    return 0;
}

