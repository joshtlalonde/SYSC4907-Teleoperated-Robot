
// #include "MQTTClient.h"
// #include "MQTTClient_Callbacks.h"
#include "Motor_Control.h"
#include "Encoder.h"
#include "Current_Sensor.h"
#include "Arm.h"

/** Verbose Option */
#define VERBOSE true

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

/** Message Send Delay */
#define SEND_DELAY 250 // milliseconds

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
Encoder encoderL(ENC_L_A, ENC_L_B);
Encoder encoderR(ENC_R_A, ENC_R_B);
Current_Sensor current_sensorL(CURR_L);
Current_Sensor current_sensorR(CURR_R);
Motor_Control motorL(encoderL, current_sensorL, 
                     1, 1000, 8, PWM_L, DIR_L, VERBOSE);
Motor_Control motorR(encoderR, current_sensorR,
                     1, 1000, 8, PWM_R, DIR_R, VERBOSE);

/** ARM Declarations */
Arm arm(mqttClient, motorL, motorR);

void setup()
{
  Serial.begin(115200);

  arm.mqtt_setup(SSID, PASSWORD,
                 MOSQUITTO_IP, MOSQUITTO_PORT);
}

unsigned long prevTime = 0;

void loop () {
  unsigned long currTime = millis();
  if (currTime - prevTime >= SEND_DELAY) {
    prevTime = currTime;
    arm.publish_encoder();
  }
}
