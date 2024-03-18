
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
// PWR_L --> Black: White, Grey: White
// PWR_R --> Black: Blue, Grey: White
// GRN_L --> Black: Brown, Grey: Black
// GRN_L --> Black: Black, Grey: Black
// Encoder Pins
#define ENC_L_A 7 // Black: Orange, Grey: Red
#define ENC_L_B 8 // Black: Yellow, Grey: Orange
#define ENC_R_A 9 // Black: White, Grey: Green
#define ENC_R_B 10 // Black: Purple, Grey: Blue
// Current Pins
#define CURR_L 11
#define CURR_VOLT_OFFSET_L 428
#define CURR_R 12
#define CURR_VOLT_OFFSET_R 432
// Motor Controller Pins
#define DIR_L 3 // Black: , Grey:
#define PWM_L 4 // Black: , Grey:
#define DIR_R 5 // Black: , Grey:
#define PWM_R 6 // Black: , Grey:
// Motor Controller PWM
#define CHANNEL_L 1
#define CHANNEL_R 2
#define FREQUENCY 30000 // 30khz
#define RESOLUTION 10 // 0-1027
// Initial Angle Values of Arm
#define INIT_THETA_L 2.1268 
#define INIT_THETA_R 1.0148

/** Message Send Delay */
#define ENCODER_RATE 250 // milliseconds
/** Dual PID Sample Rate */
#define PID_RATE 100 // microseconds
/** Position Sample Rate */
#define POSITION_RATE 250 // milliseconds
/** Current Sensor Sample Rate */
#define CURRENT_SENSOR_RATE 250 // milliseconds

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
MQTTClient mqttClient("trainee", ether, wifi, VERBOSE);

/** Motor Controller Declarations */
Encoder encoderL(ENC_L_A, ENC_L_B, INIT_THETA_L);
Encoder encoderR(ENC_R_A, ENC_R_B, INIT_THETA_R);
Current_Sensor current_sensorL(CURR_L, CURR_VOLT_OFFSET_L);
Current_Sensor current_sensorR(CURR_R, CURR_VOLT_OFFSET_R);
Motor_Control motorL(encoderL, current_sensorL, 
                     CHANNEL_L, FREQUENCY, RESOLUTION, PWM_L, DIR_L, VERBOSE);
Motor_Control motorR(encoderR, current_sensorR,
                     CHANNEL_R, FREQUENCY, RESOLUTION, PWM_R, DIR_R, VERBOSE);
Kinematics kinematics(VERBOSE);

/** ARM Declarations */
Arm arm(mqttClient, motorL, motorR, kinematics, VERBOSE);

void setup()
{
  Serial.begin(115200);

  arm.mqtt_setup(SSID, PASSWORD,
                 MOSQUITTO_IP, MOSQUITTO_PORT);

  // Serial.printf("Time (ms), encoderCount_mag, target_mag, currentEncoderCount_L, currentEncoderCount_R, encoderCountTarget_L, encoderCountTarget_R, pwr_L, pwr_R\n");
  // Serial.printf("Time (ms), currentArmatureCurrent_L, currentArmatureCurrent_R, armatureCurrentTarget_L, armatureCurrentTarget_R\n");
  // Serial.printf("Time (ms), position_mag\n");
  Serial.printf("Time (ms), currentArmatureCurrent_mag, armatureCurrentTarget_mag, currentArmatureCurrent_L, currentArmatureCurrent_R, armatureCurrentTarget_L, armatureCurrentTarget_R\n");
}

unsigned long prevTimeEncoder = 0;
unsigned long prevTimePID = 0;
unsigned long prevTimePosition = 0;
unsigned long prevTimeCurrentSensor = 0;
unsigned long prevCurrentTimePID = 0;

void loop () {
  unsigned long currTimeEncoder = millis();
  unsigned long currTimePID = micros();
  unsigned long currTimePosition = millis();
  unsigned long currTimeCurrentSensor = millis();
  unsigned long currTimeCurrentPID = micros();

  if (currTimeEncoder - prevTimeEncoder >= ENCODER_RATE) {
    prevTimeEncoder = currTimeEncoder;
    if (arm.publish_encoder() == false) {
      if (VERBOSE) {
        Serial.printf("<ARM>: Failed to Publish Encoder Message\n");
      }
    }
  }

  if (currTimePID - prevTimePID >= PID_RATE) {
    prevTimePID = currTimePID;
    if (arm.getNewEncoderTargetFlag()) {
        arm.dual_PID();
      }
  }

  if (currTimePosition - prevTimePosition >= POSITION_RATE) {
    prevTimePosition = currTimePosition;
    if (arm.publish_position() == false)  {
      if (VERBOSE) {
        Serial.printf("<ARM>: Failed to Publish Position Message\n");
      }
    }
  }

  if (currTimeCurrentSensor - prevTimeCurrentSensor >= CURRENT_SENSOR_RATE) {
    prevTimeCurrentSensor = currTimeCurrentSensor;
    if (arm.publish_current() == false)  {
      if (VERBOSE) {
        Serial.printf("<ARM>: Failed to Publish Current Sensor Message\n");
      }
    }
  }

  if (currTimeCurrentPID - prevCurrentTimePID >= PID_RATE) {
    prevCurrentTimePID = currTimeCurrentPID;
    if (arm.getNewCurrentTargetFlag()) {
      arm.dual_Current_PID();
    }
  }
}
