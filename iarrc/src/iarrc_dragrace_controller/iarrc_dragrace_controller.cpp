#include <ros/ros.h>
#include <ros/subscriber.h>
#include <ros/publisher.h>
#include <iarrc_msgs/iarrc_speed.h>
#include <std_msgs/Bool.h>
#include <string.h>


ros::Publisher speed_publisher;

ros::NodeHandle *nh;
ros::NodeHandle *nhp;

int drive_speed;
std::string race_end_topic;

ros::Subscriber stoplight_subscriber;
ros::Subscriber race_end_subscriber;

void stoplightCB(std_msgs::Bool);
void wallDetectCB(std_msgs::Bool);
void racecar_set_speed(int);

int main(int argc, char** argv)
{
	ros::init(argc, argv, "iarrc_dragrace_controller");

	nh = new ros::NodeHandle();
	nhp = new ros::NodeHandle("~");

	std::string stoplight_topic;
    	nhp->param(std::string("stoplight_topic"), stoplight_topic, std::string("/light_change"));
    	stoplight_subscriber = nh->subscribe(stoplight_topic, 1, stoplightCB);

	nhp->param(std::string("race_end_topic"), race_end_topic, std::string("/race_end"));

	nhp->param(std::string("drive_speed"), drive_speed, 14);

	std::string speed_topic;
    	nhp->param(std::string("speed_topic"), speed_topic, std::string("/speed"));
    	speed_publisher = nh->advertise<iarrc_msgs::iarrc_speed>(speed_topic, 1);
	racecar_set_speed(0);

	ROS_INFO("IARRC dragrace_controller node ready.");
	ros::spin();

	delete nh;
	delete nhp;
	ROS_INFO("Shutting down IARRC dragrace_controller node.");

    	return 0;
}

void stoplightCB(std_msgs::Bool)
{
	ROS_INFO("Drag Race Controller got stoplight.");
	racecar_set_speed(14);
	//racecar_set_speed(0);
	stoplight_subscriber.shutdown();
	system("rosnode kill stoplight_watcher &\n");
	system("roslaunch iarrc_launch lane_detection.launch &\n");
	system("roslaunch iarrc_launch race_end_detector.launch &\n");
	race_end_subscriber = nh->subscribe(race_end_topic, 1, wallDetectCB);
}

void wallDetectCB(std_msgs::Bool)
{
	ROS_INFO("Drag Race Controller got wall.");
	racecar_set_speed(0);
	race_end_subscriber.shutdown();
	system("rosnode kill iarrc_lane_detection &\n");
	system("rosnode kill drag_race_end_detector &\n");
	exit(0);
}

void racecar_set_speed(int speed)
{
	iarrc_msgs::iarrc_speed sp_cmd;
	sp_cmd.speed = speed;
	//sp_cmd.speed = 14;
	speed_publisher.publish(sp_cmd);
}


