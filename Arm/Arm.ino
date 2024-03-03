
// #include "MQTTClient.h"
// #include "MQTTClient_Callbacks.h"
#include "Motor_Control.h"
#include "Encoder.h"
#include "Current_Sensor.h"
#include "Kinematics.h"
#include "Arm.h"

/** Verbose Option */
#define VERBOSE false

/** PIN Definitions */
// Encoder Pins
#define ENC_L_A 7
#define ENC_L_B 8
#define ENC_R_A 9
#define ENC_R_B 10
// Current Pins
#define CURR_L 16
#define CURR_R 17
// Motor Controller Pins
#define DIR_L 3
#define PWM_L 4
#define DIR_R 5
#define PWM_R 6
// Initial Angle Values of Arm
#define INIT_THETA_L 2.1268 
#define INIT_THETA_R 1.0148

/** Message Send Delay */
<<<<<<< HEAD
#define SEND_DELAY 250 // milliseconds
/** Dual PID Sample Rate */
#define PID_SAMPLE_RATE 1 // milliseconds
=======
#define ENCODER_RATE 250 // milliseconds
/** Dual PID Sample Rate */
#define PID_RATE 1 // microseconds
/** Position Sample Rate */
#define POSITION_RATE 250 // milliseconds
>>>>>>> 82bb521c3d3a0123d7dcd132e0fb84d84f1665f7

// The media access control (ethernet hardware) address for the ethernet shield:
byte ETHERNET_MAC[] = { 0x40, 0x8D, 0x5C, 0xE7, 0xA5, 0x98 };  
// The IP address for the ethernet shield:
IPAddress ETHERNET_IP(192, 168, 2, 147); 
// The IP address of the mosquitto instance
char* MOSQUITTO_IP = "LAPTOP-HL4N9U5Q";//"192.168.2.18"; //"172.17.32.215";
// The port of the Mosquitto instance
int MOSQUITTO_PORT = 8883;//1883;

// Wifi Network name
char* SSID = "BrokerNet";
//// Wifi Network password
char* PASSWORD = "CapstoneBroker";

/** MQTT Declarations */
Ether ether(ETHERNET_IP, ETHERNET_MAC);
Wireless wifi;
MQTTClient mqttClient("trainee", ether, wifi);

/** Motor Controller Declarations */
Encoder encoderL(ENC_L_A, ENC_L_B, INIT_THETA_L);
Encoder encoderR(ENC_R_A, ENC_R_B, INIT_THETA_R);
Current_Sensor current_sensorL(CURR_L);
Current_Sensor current_sensorR(CURR_R);
Motor_Control motorL(encoderL, current_sensorL, 
                     1, 1000, 8, PWM_L, DIR_L, VERBOSE);
Motor_Control motorR(encoderR, current_sensorR,
                     1, 1000, 8, PWM_R, DIR_R, VERBOSE);
Kinematics kinematics(VERBOSE);

/** ARM Declarations */
Arm arm(mqttClient, motorL, motorR, kinematics, VERBOSE);

void setup()
{
  Serial.begin(115200);

  arm.mqtt_setup(SSID, PASSWORD,
                 MOSQUITTO_IP, MOSQUITTO_PORT);
}

<<<<<<< HEAD
unsigned long prevTimeMQTT = 0;
unsigned long prevTimePID = 0;

void loop () {
  unsigned long currTimeMQTT = millis();
  unsigned long currTimePID = micros();

  if (currTimeMQTT - prevTimeMQTT >= SEND_DELAY) {
    prevTimeMQTT = currTimeMQTT;
    arm.publish_encoder();
  }

  if (currTimePID - prevTimePID >= PID_SAMPLE_RATE) {
=======
unsigned long prevTimeEncoder = 0;
unsigned long prevTimePID = 0;
unsigned long prevTimePosition = 0;

void loop () {
  unsigned long currTimeEncoder = millis();
  unsigned long currTimePID = micros();
  unsigned long currTimePosition = millis();

  if (currTimeEncoder - prevTimeEncoder >= ENCODER_RATE) {
    prevTimeEncoder = currTimeEncoder;
    arm.publish_encoder();
  }

  if (currTimePID - prevTimePID >= PID_RATE) {
>>>>>>> 82bb521c3d3a0123d7dcd132e0fb84d84f1665f7
    prevTimePID = currTimePID;
    if (arm.getNewTargetFlag()) {
      arm.dual_PID();
    }
  }
<<<<<<< HEAD
=======

  if (currTimePosition - prevTimePosition >= POSITION_RATE) {
    prevTimePosition = currTimePosition;
    arm.publish_position();
  }
>>>>>>> 82bb521c3d3a0123d7dcd132e0fb84d84f1665f7
}
