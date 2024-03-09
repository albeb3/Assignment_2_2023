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
#include <assignment_2_2023/Dimarray.h>
/*		Inclusion of headers for using ros messages 									*/
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_srvs/Empty.h>
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
string controllo;

regions_ = {
    "right": 0,
    "fright": 0,
    "front": 0,
    "fleft": 0,
    "left": 0,
}




ros::Publisher pos_publisher;

actionlib::SimpleActionClient<assignment_2_2023::PlanningAction>* client_ptr;

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
void Callback(const actionlib_msgs::GoalStatus::ConstPtr &msg) {
    controllo=msg->status;
     
}

/*		Function to handle user input for sending goals to a ROS action server
*/
void userInput(ros::NodeHandle& nh) {
	 // Attendi che il server di azione sia disponibile prima di procedere
	// Creazione del client dell'azione dopo l'inizializzazione di ROS
    actionlib::SimpleActionClient<assignment_2_2023::PlanningAction> client("/reaching_goal", true);
    client_ptr = &client; // Salva il puntatore globale al client dell'azione
    ROS_INFO("Waiting for action server to start...");
    client.waitForServer();
    ROS_INFO("Action server started!"); 
   
	
    float goal_x, goal_y;
	char input=' ';
	// Main loop for user interaction    	
    while (ros::ok() && !ros::isShuttingDown()) {
		// Action has finished, get and print the status
		if(controllo== "SUCCEEDED"){ 
			//actionlib::SimpleClientGoalState state = client.getState();//receive the status
   	 		ROS_INFO("reached");
   	 		
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
        		ROS_INFO("->Press 'a' to change the window size for the averaging, then press Enter:");
        		std::cin >> input;
        		if (input=='x'){
        			client.cancelGoal();
        			ROS_INFO("Goal cancelled!");
        			busy=false;
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            		ROS_INFO("->Would you like to RESET the environment? (y/n)");
                    std::cin >> input;
                    if (input=='y'){
                        // Crea un oggetto di tipo ServiceClient per il servizio /gazebo/reset_world
                        ros::ServiceClient reset_client = nh.serviceClient<std_srvs::Empty>("/gazebo/reset_world");

                        // Crea un oggetto di tipo Empty per rappresentare la richiesta del servizio
                        std_srvs::Empty reset_srv;

                        if (reset_client.waitForExistence()) {
                            if (reset_client.call(reset_srv)) {
                                ROS_INFO("Environment reset successfully!");
                            } else {
                                ROS_ERROR("Failed to reset environment");
                            }
                        } else {
                            ROS_ERROR("Service '/gazebo/reset_world' not available");
                        }

                        continue;
                    } else if (input=='n') {
                        continue;
                    }
        		//condition for restart the loop
        		else if(busy && input=='c') {
        			ROS_INFO("wait for the result try again");
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        		}
        		else if(busy && input=='a'){
        			int dimensione;
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        			pos_publisher = nh.advertise<assignment_2_2023::Dimarray>("/dimarray", 1000);
        			if (!(std::cin >> dimensione)) {
            		ROS_INFO("Invalid input for 'y' coordinate");
            		std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            		
        			}
        			assignment_2_2023::Dimarray dimarray;
        			dimarray.dim = dimensione;
        			pos_publisher.publish(dimarray);
        			continue;
        		}
        		else {
        			ROS_INFO("Invalid input!");
        			std::cin.clear();
            		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');}
        		}
        	}
		}
	}
}



void clbk_laser(const LaserMessage& msg) {
    regions_ = {
        {"right",  *std::min_element(msg.ranges.begin(), msg.ranges.begin() + 144)},
        {"fright", *std::min_element(msg.ranges.begin() + 144, msg.ranges.begin() + 288)},
        {"front",  *std::min_element(msg.ranges.begin() + 288, msg.ranges.begin() + 432)},
        {"fleft",  *std::min_element(msg.ranges.begin() + 432, msg.ranges.begin() + 576)},
        {"left",   *std::min_element(msg.ranges.begin() + 576, msg.ranges.begin() + 714)}
    };
}

bool cancel_goal(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res) {
    if (client_ptr) { // Verifica che il puntatore al client dell'azione sia valido
        client_ptr->cancelGoal();
        ROS_INFO("Goal cancelled!");
        busy=false;
        return true;
    } else {
        ROS_ERROR("Action client not initialized!");
        return false;
    }
}




bool distance(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res) {
    // Verifica se la regione 'left' esiste nel dizionario
    regions=regions_
    
    ROS_INFO("The distance from the left obstacle is: %f",regions['left'] );
    
    return true;
}

//====================================MAIN=================================
int main(int argc, char **argv) {
	
    ros::init(argc, argv, "goal_sender_node");
    ros::NodeHandle nh;
  
    

    
    
	ros::Rate rate(1);
	rate.sleep();
	
    pos_publisher = nh.advertise<assignment_2_2023::Posvel>("/posvel", 1000);
    ros::Subscriber sub_2 = nh.subscribe<sensor_msgs::LaserScan>("/scan", clbk_laser);
    ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("/odom", 1, odomCallback);
    ros::Subscriber sub_1 = nh.subscribe<actionlib_msgs::GoalStatus>("/reaching_goal", 1, Callback);
	ros::ServiceServer service=nh.advertiseService("/cancel_goal",cancel_goal);
	ros::ServiceServer service1=nh.advertiseService("/distance_obstacle",distance);
    std::thread input_thread(userInput, std::ref(nh));
   
	
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

