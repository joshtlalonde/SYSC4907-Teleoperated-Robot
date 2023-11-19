#include "MQTTClient_Callbacks.h"

void on_connect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    // esp_mqtt_client_config_t context = (esp_mqtt_client_config_t) event->user_context;
    
    if (event->event_id == MQTT_EVENT_CONNECTED) {
        Serial.println("<MQTTClient_Callbacks>: MQTT_EVENT_CONNECTED");
        Serial.printf("<MQTTClient_Callbacks>: Client %s connected\n", client_id);
        // Serial.println(context.host);
        // Serial.println(client_id);
    }
}

void on_disconnect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_DISCONNECTED) {
        Serial.println("<MQTTClient_Callbacks>: MQTT_EVENT_DISCONNECTED");
        Serial.printf("<MQTTClient_Callbacks>: Client %s disconnected\n", client_id);
        
    }
}

void on_publish(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_PUBLISHED) {
        Serial.println("<MQTTClient_Callbacks>: MQTT_EVENT_PUBLISHED");
        Serial.printf("<MQTTClient_Callbacks>: (%d) Client %s published to topic (%.*s) message: %.*s\r\n", 
                      event->msg_id, client_id, event->topic_len, event->topic, event->data_len, event->data); 
    }
}

void on_subscribe(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_SUBSCRIBED) {
        Serial.println("<MQTTClient_Callbacks>: MQTT_EVENT_SUBSCRIBED");
        Serial.printf("<MQTTClient_Callbacks>: (%d) Client '%s' subscribed to a topic\n", 
                      event->msg_id, client_id);
    }
}
