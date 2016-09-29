#define NUMBER_FUNCTIONS 10

// Import required libraries
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include <aREST.h>

// Status
int status = WL_IDLE_STATUS;

// Clients
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Create aREST instance
aREST rest = aREST(client);

// WiFi parameters
char ssid[] = "wifi-name";
char password[] = "wifi-pass";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Define motor pins
int motorOnePlus = 6;
int motorOneMinus = 7;
int motorOneEnable = 5;

int motorTwoPlus = 8;
int motorTwoMinus = 9;
int motorTwoEnable = 4;

// Distance sensor
int distanceSensorPin = A0;

// Unique ID to identify the device
char* device_id = "40ep12";

// Declare functions to be exposed to the API
int stop(String command);
int forward(String command);
int left(String command);
int right(String command);
int backward(String command);
int distance(String command);
void callback(char* topic, byte* payload, unsigned int length);

void setup()
{

  Serial.begin(1152000);
    
  // Set pins
  pinMode(motorOnePlus, OUTPUT);
  pinMode(motorOneMinus, OUTPUT);
  pinMode(motorOneEnable, OUTPUT);
   
  pinMode(motorTwoPlus, OUTPUT);
  pinMode(motorTwoMinus, OUTPUT);
  pinMode(motorTwoEnable, OUTPUT);

  // Give name and ID to device
  rest.set_id(device_id);
  rest.set_name("robot");

  // Function to be exposed
  rest.function("forward", forward);
  rest.function("stop", stop);
  rest.function("right", right);
  rest.function("left", left);
  rest.function("backward", backward);
  rest.function("distance", measureDistance);

  // Set callback
  client.setCallback(callback);

  // Connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);

    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("WiFi connected");
   
}

void loop()
{
  
  // Connect to the cloud
  rest.handle(client);
   
}

// Function to control the motor
void setMotorOne(boolean forward, int motor_speed){
   digitalWrite(motorOnePlus, forward);
   digitalWrite(motorOneMinus, !forward);
   analogWrite(motorOneEnable, motor_speed);
}

// Function to control the motor
void setMotorTwo(boolean forward, int motor_speed){
   digitalWrite(motorTwoPlus, forward);
   digitalWrite(motorTwoMinus, !forward);
   analogWrite(motorTwoEnable, motor_speed);
}

// Functions to control the robot
int stop(String command) {

  setMotorOne(true, 0);
  setMotorTwo(true, 0);
  
}

int forward(String command) {

  setMotorOne(true, 1000);
  setMotorTwo(true, 1000);
  
}

int backward(String command) {

  setMotorOne(false, 1000);
  setMotorTwo(false, 1000);
  
}

int left(String command) {

  setMotorOne(false, 700);
  setMotorTwo(true, 700);
  
}

int right(String command) {

  setMotorOne(true, 700);
  setMotorTwo(false, 700);
  
}

int measureDistance(String command) {

  // Measure distance
  unsigned int Distance = 0;
  unsigned long DistanceMeasured = pulseIn(distanceSensorPin, LOW);

  // Compute distance
  if (DistanceMeasured == 50000 ) {        
    Serial.print("Invalid");    
  }
  else {
    Distance = DistanceMeasured/50;
  }
   
  return Distance;
  
}

// Handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {

  // Handle
  rest.handle_callback(client, topic, payload, length);

}
