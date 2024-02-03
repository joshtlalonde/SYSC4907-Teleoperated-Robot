#include "MQTTClient_Callbacks.h"
#include "ArduinoJson.h"

void on_connect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    // esp_mqtt_client_config_t context = (esp_mqtt_client_config_t) event->user_context;
    
    if (event->event_id == MQTT_EVENT_CONNECTED) {
        Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_CONNECTED ");
        Serial.printf("Client %s connected\n", client_id);
        // Serial.println(context.host);
        // Serial.println(client_id);
    }
}

void on_disconnect(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_DISCONNECTED) {
        Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_DISCONNECTED ");
        Serial.printf("Client %s disconnected... Auto-reconnecting\n", client_id);
    }
}

void on_publish(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_PUBLISHED) {
        Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_PUBLISHED ");
        Serial.printf("(%d) Client %s published to topic (%.*s) message: %.*s\r\n", 
                      event->msg_id, client_id, event->topic_len, event->topic, event->data_len, event->data); 
    }
}

void on_subscribe(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;
    
    if (event->event_id == MQTT_EVENT_SUBSCRIBED) {
        Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_SUBSCRIBED ");
        Serial.printf("(%d) Client '%s' subscribed to a topic\n", 
                      event->msg_id, client_id);
    }
}

// void on_encoder(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
//     esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
//     esp_mqtt_client_handle_t client = event->client;
//     char* client_id = (char*) handler_args;

//     // Convert Event information into a expected string type
//     char LEncoderTopic[50], REncoderTopic[50], eventTopic[50], eventData[50];
//     // sprintf(LEncoderTopic, "%s/left", client_id);
//     // sprintf(REncoderTopic, "%s/right", client_id);
//     sprintf(eventTopic, "%.*s", event->topic_len, event->topic);
//     sprintf(eventData, "%.*s", event->data_len, event->data);

//     if (event->event_id == MQTT_EVENT_DATA && !strstr(eventTopic, client_id) && strstr(eventTopic, "encoder")) {
//       // TODO: Fix so that enc vals are objects
//         if (strstr(eventTopic, "left")) {
//             // Received Encoder Left Topic
//             Serial.print("<ARM>: MQTT_EVENT_DATA: ");
//             Serial.printf("Recieved message on topic=%s with data: %s\n", 
//                           eventTopic, eventData);

//             // Assign value received
//             Lencoder_val = atoi(eventData);
//         }
//         else if (strstr(eventTopic, "right")) {
//             // Received Encoder Right Topic
//             Serial.print("<ARM>: MQTT_EVENT_DATA: ");
//             Serial.printf("Recieved message on topic=%s with data: %s\n", 
//                           eventTopic, eventData);

//             // Assign value received
//             Rencoder_val = atoi(eventData);
//         }
//     }
// }

// void on_current(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
//     esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
//     esp_mqtt_client_handle_t client = event->client;
//     arm_client_t* client = (arm_client_t*) handler_args;

//     // Convert Event information into an expected string type
//     char eventTopic[50], eventData[50];
//     sprintf(eventTopic, "%.*s", event->topic_len, event->topic);
//     sprintf(eventData, "%.*s", event->data_len, event->data);

//     if (event->event_id == MQTT_EVENT_DATA && !strstr(eventTopic, client->client_id) && strstr(eventTopic, "current")) {
//         // Received Current Left Topic from someone else
//         Serial.print("<ARM>: MQTT_EVENT_DATA: ");
//         Serial.printf("Recieved message on topic=%s with data: %s\n", 
//                       eventTopic, eventData);

//         // Assign value received
//         Lcurrent_val = atoi(eventData);
//     }
// }

void on_data(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    // esp_mqtt_client_handle_t client = event->client;
    Arm* arm_client = (Arm*) handler_args;

    // Convert Event information into an expected string type
    char eventTopic[50], eventData[50];
    sprintf(eventTopic, "%.*s", event->topic_len, event->topic);
    sprintf(eventData, "%.*s", event->data_len, event->data);

    // Serial.printf("Received message on %s: %s\n", eventTopic, eventData);

    // Make sure it is a DATA message and not from yourself
    if (event->event_id == MQTT_EVENT_DATA && !strstr(eventTopic, arm_client->getClientId())) {
        Serial.print("<MQTTClient_Callbacks>: MQTT_EVENT_DATA: "); // TODO: REMOVE to save processing power
        Serial.printf("Recieved message on topic=%s with data: %s\n", 
                      eventTopic, eventData);

        // Initialize Temp memory to hold JSON data
        DynamicJsonDocument json(1024);
        // Convert JSON to object
        deserializeJson(json, eventData);

        // Check which type of DATA message
        if (strstr(eventTopic, "encoder")) {
            /** TODO: Set flag and new target */
        //   arm_client->setEncoderLeft(json["left"]);
        //   arm_client->setEncoderRight(json["right"]);
            arm_client->setEncoderTarget(json["left"], json["right"]);
        } 
        else if (strstr(eventTopic, "current")) {
            /** TODO: Set flag and new target */
        //   arm_client->setCurrentLeft(json["left"]);
        //   arm_client->setCurrentRight(json["right"]);
        }
     }
}
