
#include "Arm.h"
#include "ArduinoJson.h"
#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"

Arm::Arm(MQTTClient& mqtt_client, Motor_Control& motorL, 
         Motor_Control& motorR) : mqtt_client(mqtt_client), 
         motorL(motorL), motorR(motorR)
{
  this->mqtt_client = mqtt_client;
  this->motorL = motorL;
  this->motorR = motorR;

  this->newTargetFlag = false;
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
  json["left"] = this->motorL.getEncoderCount();
  json["right"] = this->motorR.getEncoderCount();

  serializeJson(json, encoder_val_str, 50);
}

void Arm::current_jsonify(char* current_val_str) {
  if (current_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  DynamicJsonDocument json(1024);
  json["left"] = this->motorL.getCurrentAmps();
  json["right"] = this->motorR.getCurrentAmps();

  serializeJson(json, current_val_str, 50);
}

bool Arm::publish_encoder() {
  char topic[250], encoder_val_str[250];
  this->encoder_jsonify(encoder_val_str);

  sprintf(topic, "encoder/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, encoder_val_str, 0, 0, 0) < 0) 
    return false;
  
  return true;
}

bool Arm::publish_current() {
  char topic[250], current_val_str[250];
  this->current_jsonify(current_val_str);

  sprintf(topic, "current/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, current_val_str, 0, 1, 0) < 0)
    return false;
  
  return true;
}

/************************************/