
#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"
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

Ether ether(ETHERNET_IP, ETHERNET_MAC);
Wireless wifi;
MQTTClient mqttClient("trainee", ether, wifi);
Arm arm(mqttClient);

void setup_menu() {
  char inputString[100];
  char inChar;
  int index = 0;
  // TODO: Input IP Address


  // Input ClientID
  Serial.println("Select ClientID: ");
  Serial.printf("\t[1]: 'trainee'\n");
  Serial.printf("\t[2]: 'trainer'\n");
  while (inChar != '\n') {
    while(Serial.available()) {
      inChar = (char)Serial.read();
      inputString[index] = inChar;
      index++;
    }
  }
  
  // Get Input 
  if (strcmp(inputString, "1\n") == 0) {
    Serial.println("ClientID set to 'trainee'");
    mqttClient.setClientId("trainee");
  } else if (strcmp(inputString, "2\n") == 0) {
    Serial.println("ClientID set to 'trainer'");
    mqttClient.setClientId("trainer");
  } else {
    Serial.println("Incorrect Selection, please select '1' or '2'");
    setup_menu();
  }
}

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
