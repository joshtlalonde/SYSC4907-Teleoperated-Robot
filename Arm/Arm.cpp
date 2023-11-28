
#include "Arm.h"
#include "ArduinoJson.h"

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