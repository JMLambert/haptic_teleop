/**
 ** Not So Simple ROS Subscriber Node
 **/
#include <ros/ros.h>
#include <sensor_msgs/JointState.h> 


// MoveIt!
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>

#include <moveit/kinematic_constraints/utils.h>
#include <eigen_conversions/eigen_msg.h>


class SelfCollisionChk {

public:
	ros::Subscriber collsub;
	std::vector<double> joint_values_right;
	std::vector<double> joint_values_left;

    //Constructor for the vectors
   SelfCollisionChk():joint_values_right(6, 666.0), joint_values_left(6, 666.0)
	{
	   ros::NodeHandle node;
	   collsub = node.subscribe("joint_states", 1, &SelfCollisionChk::selfcollisionCallback, this);
	}


   void selfcollisionCallback(const sensor_msgs::JointState& msg)
   {

	    //std::vector<double> joint_values;
	    //std::vector<double> joint_values_right(6);
	    //std::vector<double> joint_values_left(6);

	    //joint_values = msg.position;

	    //this->joint_values_right[0] = msg.position[5];
	    //this->joint_values_right[1] = msg.position[1];
	    //this->joint_values_right[2] = msg.position[4];
	    //this->joint_values_right[3] = msg.position[3];
	    //this->joint_values_right[4] = msg.position[7];
	    //this->joint_values_right[5] = msg.position[10];

	    this->joint_values_right[0] = msg.position[0];
	    this->joint_values_right[1] = msg.position[1];
	    this->joint_values_right[2] = msg.position[2];
	    this->joint_values_right[3] = msg.position[3];
	    this->joint_values_right[4] = msg.position[4];
	    this->joint_values_right[5] = msg.position[5];

	    this->joint_values_left[0] = msg.position[6];
	    this->joint_values_left[1] = msg.position[7];
	    this->joint_values_left[2] = msg.position[8];
	    this->joint_values_left[3] = msg.position[9];
	    this->joint_values_left[4] = msg.position[10];
	    this->joint_values_left[5] = msg.position[11];



	  //ROS_INFO("Test 1cb : Current robot state is %f, %f, %f, %f, %f, %f", joint_values_right[0], joint_values_right[1], joint_values_right[2], joint_values_right[3], joint_values_right[4], joint_values_right[5]);
   }
};

int main(int argc, char **argv)
{

	ros::init(argc, argv, "collision_subscriber");

	SelfCollisionChk selfColl;

	/* Load the robot model */
	  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");

	  /* Get a shared pointer to the model */
	  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();

	  /* Construct a planning scene - NOTE: this is for illustration purposes only.
	     The recommended way to construct a planning scene is to use the planning_scene_monitor
	     to construct it for you.*/
	  planning_scene::PlanningScene planning_scene(kinematic_model);

	  /* The planning scene contains a RobotState *representation of the robot configuration
	   We can get a reference to it.*/
	  robot_state::RobotState& current_state = planning_scene.getCurrentStateNonConst();

	std::vector<double> joint_values;

	ros::Rate loop_rate(50);
	int count = 0;
	while (ros::ok())
		{

		robot_state::JointStateGroup* joint_state_group_r = current_state.getJointStateGroup("right_comau");
		joint_state_group_r->setVariableValues(selfColl.joint_values_right);
		robot_state::JointStateGroup* joint_state_group_l = current_state.getJointStateGroup("left_comau");
		joint_state_group_l->setVariableValues(selfColl.joint_values_left);

		collision_detection::CollisionRequest collision_request;
		collision_request.distance = true;
		collision_request.contacts = true;
		collision_detection::CollisionResult collision_result;


		planning_scene.checkSelfCollision(collision_request, collision_result);
		collision_detection::Contact& cont_info = collision_result.contacts.begin()->second[0];


		ROS_INFO_STREAM("Test 1 : Current state is " << (collision_result.collision ? "in" : "not in") << " self collision");

		ROS_INFO("Test 2 : Current robot 1 state is %f, %f, %f, %f, %f, %f", selfColl.joint_values_right[0], selfColl.joint_values_right[1], selfColl.joint_values_right[2], selfColl.joint_values_right[3], selfColl.joint_values_right[4], selfColl.joint_values_right[5]);
		ROS_INFO("Test 2 : Current robot 2 state is %f, %f, %f, %f, %f, %f", selfColl.joint_values_left[0], selfColl.joint_values_left[1], selfColl.joint_values_left[2], selfColl.joint_values_left[3], selfColl.joint_values_left[4], selfColl.joint_values_left[5]);
		ROS_INFO("Test 3 : Distance to collision is %f", collision_result.distance);
		

		if (collision_result.collision==true)
		{
			ROS_INFO("Test 4 : Contact between %s and %s", collision_result.contacts.begin()->first.first.c_str(), collision_result.contacts.begin()->first.second.c_str());
			ROS_INFO("Test 5 : Contact normal %f, %f, %f", cont_info.normal[0], cont_info.normal[1], cont_info.normal[2]);
			ROS_INFO("Test 6 : Contact position %f, %f, %f", cont_info.pos[0], cont_info.pos[1], cont_info.pos[2]);
			ROS_INFO("Test 7 : Contact depth %f", cont_info.depth);
		}


		ros::spinOnce();

	  	loop_rate.sleep();
	  	++count;
		}
    
    return 0;
}