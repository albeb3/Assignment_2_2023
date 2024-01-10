#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <assignment_2_2023/PlanningAction.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <assignment_2_2023/Posvel.h>






void subCallback(const assignment_2_2023::Posvel::ConstPtr& msg){
	ROS_INFO("pos subscriber@[%f %f %f %f]", msg->x, msg->y, msg->vel_x, msg->vel_y);
	
	
}

int main(int argc, char** argv){
	ros::init(argc, argv, "subscriber");
	ros::NodeHandle nh;

	ros::Subscriber sub = nh.subscribe("/posvel", 1, subCallback);


	ros::spin();

	return 0;
}
