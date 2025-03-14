#include "MQTTClient_Callbacks.h"
#include "ArduinoJson.h"

void on_connect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    Arm* arm_client = (Arm*) handler_args;
    
    if (event->event_id == MQTT_EVENT_CONNECTED) {
        if (arm_client->verbose) {
            Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_CONNECTED ");
            Serial.printf("Client %s connected\n", arm_client->getClientId());
        }
    }
}

void on_disconnect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    Arm* arm_client = (Arm*) handler_args;
    
    if (event->event_id == MQTT_EVENT_DISCONNECTED) {
        if (arm_client->verbose) {
            Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_DISCONNECTED ");
            Serial.printf("Client %s disconnected... Auto-reconnecting\n", arm_client->getClientId());
        }
    }
}

void on_publish(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    Arm* arm_client = (Arm*) handler_args;
    
    if (event->event_id == MQTT_EVENT_PUBLISHED) {
        if (arm_client->verbose) {
            Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_PUBLISHED ");
            Serial.printf("(%d) Client %s published to topic (%.*s) message: %.*s\r\n", 
                        event->msg_id, arm_client->getClientId(), event->topic_len, event->topic, event->data_len, event->data); 
        }
    }
}

void on_subscribe(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    // char* client_id = (char*) handler_args;
    Arm* arm_client = (Arm*) handler_args;
    
    if (event->event_id == MQTT_EVENT_SUBSCRIBED) {
        if (arm_client->verbose) {
            Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_SUBSCRIBED ");
            Serial.printf("(%d) Client '%s' subscribed to a topic\n", 
                        event->msg_id, arm_client->getClientId());
        }
    }
}

void on_data(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    Arm* arm_client = (Arm*) handler_args;

    // Convert Event information into an expected string type
    char eventTopic[100], eventData[100];
    sprintf(eventTopic, "%.*s", event->topic_len, event->topic);
    sprintf(eventData, "%.*s", event->data_len, event->data);

    // Make sure it is a DATA message and not from yourself
    if (event->event_id == MQTT_EVENT_DATA && !strstr(eventTopic, arm_client->getClientId())) {
        if (arm_client->verbose) {
            Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_DATA: "); // TODO: REMOVE to save processing power
            Serial.printf("Recieved message on topic=%s with data: %s\n", 
                          eventTopic, eventData);
        }

        // Initialize Temp memory to hold JSON data
        DynamicJsonDocument json(1024);
        // Convert JSON to object
        deserializeJson(json, eventData);

        // Check which type of DATA message
        if (strstr(eventTopic, "encoder")) {
            arm_client->setEncoderTarget(json["left"], json["right"]);
        } 
        else if (strstr(eventTopic, "current")) {
            /** TODO: Set flag and new target */
        //   arm_client->setCurrentTarget(json["left"], json["right"]);
        }
        else if (strstr(eventTopic, "position")) {
            /** TODO: Setup Position */
            Serial.println("\tTODO: Setup Position Response???");
        } 
        else if (strstr(eventTopic, "force")) {
            arm_client->setCurrentTarget(json["x"], json["y"]);
        }
    }
}
