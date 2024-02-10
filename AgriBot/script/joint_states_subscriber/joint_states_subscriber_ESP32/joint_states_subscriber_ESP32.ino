#include <WiFi.h>
#include <ros.h>
#include <sensor_msgs/JointState.h>

WiFiClient client;
ros::NodeHandle nh;

const char* ssid = "Micromax HS2";
const char* password = "Micromax";

const char* server = "176.16.206.207"; // rosMaster
const int port = 11311;  // rosPort

void jointStateCallback(const sensor_msgs::JointState& msg);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  nh.initNode(&client, server, port);

  ros::Subscriber<sensor_msgs::JointState> sub("joint_states", &jointStateCallback);
  nh.subscribe(sub);
  nh.spinOnce();

  // Subscribe to the joint_states topic
 // subscribeToJointStates();

}

void loop() {
  // put your main code here, to run repeatedly:
  nh.spinOnce();
}

void jointStateCallback(const sensor_msgs::JointState& msg) {
  // Print the joint positions received
  for (int i = 0; i < msg.position.size(); i++) {
    Serial.print("Joint ");
    Serial.print(i);
    Serial.print(" position: ");
    Serial.println(msg.position[i]);
  }
}
