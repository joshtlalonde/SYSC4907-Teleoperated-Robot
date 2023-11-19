#ifndef MQTTCLIENT_CALLBACKS_h
#define MQTTCLIENT_CALLBACKS_h

#include "Arduino.h"
#include "mqtt_client.h"

void on_connect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
void on_disconnect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
void on_publish(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
void on_subscribe(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

#endif