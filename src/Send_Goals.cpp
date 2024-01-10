#include <ros/ros.h>
#include <assignment_2_2023/PosGoal.h>
#include <iostream>
#include <actionlib/client/simple_action_client.h>
#include <assignment_2_2023/PlanningAction.h>
int dim=5;

ros::Publisher pos_publisher;



int main(int argc, char **argv) {
    ros::init(argc, argv, "goal_sender_node");
    ros::NodeHandle nh;
    assignment_2_2023::PosGoal pos_goal;
	actionlib::SimpleActionClient<assignment_2_2023::PlanningAction> client("/reaching_goal", true);
	actionlib::SimpleClientGoalState state = client.getState();//receive the status
	pos_publisher = nh.advertise<assignment_2_2023::PosGoal>("/posgoal", 1000);
	float goal_x, goal_y;
	char input=' ';
	ROS_INFO("Waiting for action server to start...");
	client.waitForServer();
	ROS_INFO("Action server started!");
	ROS_INFO("Insert the goal coordinate (x,y) of 5 goals \n");
    while (ros::ok() && !ros::isShuttingDown()) {
    	ros::Rate rate(1);
		ROS_INFO("STATE: %s \n",state.toString().c_str());
        for(int i=1;i<=dim;i++){
        	ROS_INFO("Insert 'x' coordinate of %i, then press ENTER: ", i);
        	if (!(std::cin >> goal_x)) {
            	ROS_INFO("Invalid input for 'x' coordinate");
            	std::cin.clear();
           		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
           		continue;
        	}

        	ROS_INFO("Insert 'y' coordinate of %i, then press ENTER: ", i);
        	if (!(std::cin >> goal_y)) {
            	ROS_INFO("Invalid input for 'y' coordinate");
            	std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            	continue;
        	}
        	ROS_INFO("Sending the goal %i", i);	
			pos_goal.goal_x = goal_x;
        	pos_goal.goal_y = goal_y;
        	pos_publisher.publish(pos_goal);
        	}
        	
    	if (strcmp(state.toString().c_str(), "ACTIVE") == 0 ){
			ROS_INFO("Do you want delete the goal?");
        	ROS_INFO("Press y or n"); 
        	std::cin >> input;
        	if (input=='y'){
        		
        		client.cancelGoal();
        		ROS_INFO("Goal cancelled!");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
        	
        	else if (input == 'n'){
        		
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		}
        	else {
        		ROS_INFO("Invalid Input");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
		
    	}
    	rate.sleep();
	}
   
   
   
    return 0;
}

