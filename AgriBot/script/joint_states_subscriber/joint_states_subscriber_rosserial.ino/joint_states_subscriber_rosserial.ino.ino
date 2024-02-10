#include <ros.h>
#include <sensor_msgs/JointState.h>
#include <cstring.h>

ros::NodeHandle nh;

void jointStateCallback(const sensor_msgs::JointState& msg) {
  int num_joints = msg.name_length;  // Get number of joints from name_length property
  
  Serial.println("Received joint state message");
  for (int i = 0; i < num_joints; i++) {
    Serial.print("Joint ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(msg.name[i]);
    Serial.print("Position: ");
    Serial.println(msg.position[i]);
  }
}

ros::Subscriber<sensor_msgs::JointState> sub("/joint_states", jointStateCallback);

void setup() {
  Serial.begin(115200);
  nh.initNode();
  nh.subscribe(sub);
}

void loop() {
  nh.spinOnce();
  delay(10);
}
