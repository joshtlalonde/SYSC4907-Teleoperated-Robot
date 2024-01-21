
#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"
#include "Arm.h"

// The media access control (ethernet hardware) address for the ethernet shield:
byte ETHERNET_MAC[] = { 0x40, 0x8D, 0x5C, 0xE7, 0xA5, 0x98 };  
// The IP address for the ethernet shield:
IPAddress ETHERNET_IP(192, 168, 2, 147); 
// The IP address of the mosquitto instance
char* MOSQUITTO_IP = "192.168.2.18"; //"172.17.49.245";
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

void setup()
{
  Serial.begin(115200);

  // Setup the broker
  if (!mqttClient.begin(SSID, PASSWORD)) {
    Serial.printf("<ARM>: Failed to initialize mqttClient\n");
    while(1); // TODO: SHUTDOWN?? Should go to clean exit or clean restart?
  }

  delay(2000);

  // Connect to Broker
  if (!mqttClient.connect(MOSQUITTO_IP, MOSQUITTO_PORT)) {
    Serial.println("Failed to Connect to Broker, Exiting...");
    while(1); // TODO: SHUTDOWN???
  }

  // Start MQTT
  mqttClient.start();

  // Attach Callback for Data messages
  /* TODO: Cannot use the on_data event because it will be called for every send it makes
   *       this will cause the esp to crash
   *       Is there really a way around this though?
   */
  mqttClient.add_message_callback(MQTT_EVENT_DATA, on_data, (void*) &arm, "on_data");

  // Subscribe to Encoder Topic
  mqttClient.subscribe("encoder/#", 1);
  // Subscribe to Current Topic
  mqttClient.subscribe("current/#", 1);
}

void loop () {
/*
  Serial.printf("<ARM>: Encoder --> Left = %f, Right = %f\n", arm.getEncoderLeft(), arm.getEncoderRight());
  Serial.printf("<ARM>: Current --> Left = %f, Right = %f\n", arm.getCurrentLeft(), arm.getCurrentRight());
  delay(1000);
  
  // Simulating Values constantly updating
  arm.setEncoderLeft(arm.getEncoderLeft() + 1);
  arm.setEncoderRight(arm.getEncoderRight() + 10);
  arm.setCurrentLeft(arm.getCurrentLeft() + 25);
  arm.setCurrentRight(arm.getCurrentRight() + 50);

  // Publish the JSONify version of the encoder/current values
  char topic[50], encoder_val_str[50], current_val_str[50];
  arm.encoder_jsonify(encoder_val_str);
  arm.current_jsonify(current_val_str);

  sprintf(topic, "encoder/%s", arm.getClientId());
  mqttClient.publish(topic, encoder_val_str, 0, 1, 0);

  sprintf(topic, "current/%s", arm.getClientId());
  mqttClient.publish(topic, current_val_str, 0, 1, 0);

  delay(1000);
*/

// Simulating Values constantly updating
  // arm.setEncoderLeft(-20); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(-20); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(-10); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(0); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(0); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(10); arm.setEncoderRight(30);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(20); arm.setEncoderRight(30);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(30); arm.setEncoderRight(30);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(20); arm.setEncoderRight(20);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(10); arm.setEncoderRight(10);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(0); arm.setEncoderRight(0);
  // delay(100);
  // arm.publish_encoder(mqttClient);
  // arm.setEncoderLeft(-10); arm.setEncoderRight(10);
  // delay(100);
  // arm.publish_encoder(mqttClient);

  if (mqttClient.getClientId() == "trainee") {
    arm.setEncoderLeft(arm.getEncoderLeft() + 1); arm.setEncoderRight(arm.getEncoderRight() + 1);
    arm.publish_encoder(mqttClient);
    delay(2000);
  } 
  else if (mqttClient.getClientId() == "trainer") {
    arm.setEncoderLeft(arm.getEncoderLeft() + 100); arm.setEncoderRight(arm.getEncoderRight() + 100);
    arm.publish_encoder(mqttClient);
    delay(1000);
  }

}
