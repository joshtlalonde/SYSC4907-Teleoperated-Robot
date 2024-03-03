#ifndef MQTTCLIENT_CALLBACKS_h
#define MQTTCLIENT_CALLBACKS_h

#include "Arduino.h"
#include "mqtt_client.h"
#include "Arm.h"

/**
  These functions are used for handling different types of messages received from the MQTT Broker
*/

/** 
  Handles a CONNECTED message from the Broker, this message is broadcasted when a new connection is made
*/
void on_connect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

/** 
  Handles a DISCONNECTED message from the Broker, this message is broadcasted when a disconnection occurs
*/
void on_disconnect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

/** 
  Handles a PUBLISHED message from the Broker, this message is broadcasted when a publised message occurs.
  This does not handle the data from that published message though
*/
void on_publish(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

/** 
  Handles a SUBSCRIBED message from the Broker, this message is broadcasted when new subscribed occurs
*/
void on_subscribe(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

/** 
  Handles a DATA message from the Broker, this is the only way to handle data received from published topics
*/
void on_data(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

#endif