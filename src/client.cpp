#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <assignment_2_2023/PlanningAction.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <assignment_2_2023/Posvel.h>
#include <iostream>
#include <thread>

float robot_x = 0.0, robot_y = 0.0, robot_vel_x = 0.0, robot_vel_y = 0.0;

bool cancel_requested = false;
bool busy=false;
std::string feedback_status;

ros::Publisher pos_publisher;

void odomCallback(const nav_msgs::Odometry::ConstPtr &msg) {
    robot_x = msg->pose.pose.position.x;
    robot_y = msg->pose.pose.position.y;
    robot_vel_x = msg->twist.twist.linear.x;
    robot_vel_y = msg->twist.twist.linear.y;
}


void userInput() {
    	actionlib::SimpleActionClient<assignment_2_2023::PlanningAction> client("/reaching_goal", true);
		ROS_INFO("Waiting for action server to start...");
        client.waitForServer();
        
        ROS_INFO("Action server started!");
        float goal_x, goal_y;
		char input=' ';
    	
    while (ros::ok() && !ros::isShuttingDown()) {
    	
        if( cancel_requested){
        	ROS_INFO("Do you want delete the goal?");
        	ROS_INFO("Press y or n"); 
        	std::cin >> input;
        	if (input=='y'){
        		cancel_requested = false;
        		client.cancelGoal();
        		ROS_INFO("Goal cancelled!");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
        	
        	else if (input == 'n'){
        		cancel_requested = false;
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		}
        	else {
        		ROS_INFO("Invalid Input");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
		}
		
		
		else if(client.waitForResult(ros::Duration(0.1)) && busy){ 
			actionlib::SimpleClientGoalState state = client.getState();//receive the status
   	 		ROS_INFO("Action finished: %s \n",state.toString().c_str());
			
			busy=false;
			}
		
		else{	
			if(!busy){
        		ROS_INFO("Insert the goal coordinate (x,y) \n");
        		}	
        	
        	else if(busy){
        		ROS_INFO("->Press 'x' to cancel the goal, then press Enter:");
        		ROS_INFO("->Press 'c' to check if the goal has been reached, then press Enter:");
        		std::cin >> input;
        		}
        	
        	if (input=='x'){
        		cancel_requested = true;
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
           		continue;	
        	}
        	
        	else if(!busy){
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
				busy=true;
				
        		assignment_2_2023::PlanningGoal goal;
        		goal.target_pose.pose.position.x = goal_x;
        		goal.target_pose.pose.position.y = goal_y;
        		client.sendGoal(goal);
        		
        	}
        	
        	else if(busy && input=='c') {
        		ROS_INFO("wait for the result try again");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
        	else  {
        		ROS_INFO("Invalid input!");
        		std::cin.clear();
            	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	}
        }
        
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "goal_sender_node");
    ros::NodeHandle nh;

    pos_publisher = nh.advertise<assignment_2_2023::Posvel>("/posvel", 1000);
    ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("/odom", 1, odomCallback);

    std::thread input_thread(userInput);
    ros::Rate rate(1);

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

