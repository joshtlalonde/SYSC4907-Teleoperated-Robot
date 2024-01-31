
#include "Arm.h"
#include "ArduinoJson.h"
#include "MQTTClient.h"

Arm::Arm(MQTTClient& mqtt_client)
        : mqtt_client(mqtt_client) 
{
  this->mqtt_client = mqtt_client;

  // Set Default vals
  this->encoder_left;
  this->encoder_right;
  this->current_left;
  this->current_right;
}

float Arm::getEncoderLeft() {
  return this->encoder_left;
}

void Arm::setEncoderLeft(float value) {
  this->encoder_left = value;
}

float Arm::getEncoderRight() {
  return this->encoder_right;
}

void Arm::setEncoderRight(float value) {
  this->encoder_right = value;
}

float Arm::getCurrentLeft() {
  return this->current_left;
}

void Arm::setCurrentLeft(float value) {
  this->current_left = value;
}

float Arm::getCurrentRight() {
  return this->current_right;
}

void Arm::setCurrentRight(float value) {
  this->current_right = value;
}

char* Arm::getClientId() {
  return this->mqtt_client.getClientId();
}

int Arm::getNewTargetFlag() {
  return this->newTargetFlag;
}

/** MOTOR_CONTROLLER FUNCTIONALITY **/

void Arm::setEncoderTarget(int targetL, int targetR) {
  // Set the Target Flag
  this->newTargetFlag = true;

  this->motorL.setTarget(targetL);
  this->motorR.setTarget(targetR);
}

/************************************/

/******** MQTT FUNCTIONALITY ********/

void Arm::mqtt_setup(char* ssid, char* password, 
                     char* mosqutto_ip, int mosquitto_port) 
{
  // Setup the broker
  if (!this->mqtt_client.begin(ssid, password)) {
    Serial.printf("<ARM>: Failed to initialize mqttClient\n");
    while(1); // TODO: RESTART?? Should go to clean exit or clean restart?
  }

  delay(2000);

  // Connect to Broker
  if (!this->mqtt_client.connect(mosqutto_ip, mosquitto_port)) {
    Serial.println("<ARM>: Failed to Connect to Broker, Exiting...");
    while(1); // TODO: RESTART???
  }

  // Start MQTT
  this->mqtt_client.start();

  // Attach Callback for Data messages
  /** FIX: Cannot use the on_data event because it will be called for every send it makes
   *       this will cause the esp to crash
   *       Is there really a way around this though?
   */
  this->mqtt_client.add_message_callback(MQTT_EVENT_DATA, on_data, (void*) this, "on_data");

  // Subscribe to Encoder Topic
  this->mqtt_client.subscribe("encoder/#", 1);
  // Subscribe to Current Topic
  this->mqtt_client.subscribe("current/#", 1);
}

void Arm::encoder_jsonify(char* encoder_val_str) {
  if (encoder_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  DynamicJsonDocument json(1024);
  json["left"] = this->encoder_left;
  json["right"] = this->encoder_right;

  serializeJson(json, encoder_val_str, 50);
}

void Arm::current_jsonify(char* current_val_str) {
  if (current_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  DynamicJsonDocument json(1024);
  json["left"] = this->current_left;
  json["right"] = this->current_right;

  serializeJson(json, current_val_str, 50);
}

bool Arm::publish_encoder(MQTTClient mqttClient) {
  char topic[250], encoder_val_str[250];
  this->encoder_jsonify(encoder_val_str);

  sprintf(topic, "encoder/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, encoder_val_str, 0, 0, 0) < 0) 
    return false;
  
  return true;
}

bool Arm::publish_current(MQTTClient mqttClient) {
  char topic[250], current_val_str[250];
  this->current_jsonify(current_val_str);

  sprintf(topic, "current/%s", this->getClientId());
  mqttClient.publish(topic, current_val_str, 0, 1, 0);
  return true;
}