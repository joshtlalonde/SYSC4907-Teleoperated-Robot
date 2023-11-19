
#include "MQTTClient.h"

// the media access control (ethernet hardware) address for the shield:
byte ETHERNET_MAC[] = { 0x40, 0x8D, 0x5C, 0xE7, 0xA5, 0x98 };  
//the IP address for the shield:
IPAddress ETHERNET_IP(192, 168, 2, 147); 
// The IP address of the mosquitto instance
// IPAddress MOSQUITTO_IP(192, 168, 2, 12); 
char* MOSQUITTO_IP = "192.168.2.18";
// The port of the Mosquitto instance
int MOSQUITTO_PORT = 8883;//1883;

// Wifi Network name
char* SSID = "BrokerNet";
//// Wifi Network password
char* PASSWORD = "CapstoneBroker";

Ether ether(ETHERNET_IP, ETHERNET_MAC);
Wireless wifi;
MQTTClient mqttClient("Arm1_Test", ether, wifi);

volatile int encoder_val = 0;
volatile int current_val = 0;

void on_encoder(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;

    char topic[50], event_topic[50];
    sprintf(topic, "encoder/%s", client_id);
    sprintf(event_topic, "%.*s", event->topic_len, event->topic);
    
    if (event->event_id == MQTT_EVENT_DATA) {
        if (strcmp(event_topic, topic) == 0) {
            Serial.println("<ARM>: MQTT_EVENT_DATA");
            
            Serial.printf("<ARM>: Recieved message on topic=%.*s with data: %.*s\n", 
                          event->topic_len, event->topic, event->data_len, event->data);

            encoder_val = atoi(event->data);
        }
    }
}

void on_current(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    char* client_id = (char*) handler_args;

    char topic[50], event_topic[50];
    sprintf(topic, "current/%s", client_id);
    sprintf(event_topic, "%.*s", event->topic_len, event->topic);
    
    if (event->event_id == MQTT_EVENT_DATA) {
          if (strcmp(event_topic, topic) == 0) {
            Serial.println("<ARM>: MQTT_EVENT_DATA");
            
            Serial.printf("<ARM>: Recieved message on topic=%.*s with data: %.*s\n", 
                          event->topic_len, event->topic, event->data_len, event->data);

            current_val = atoi(event->data);
          }
    }
}

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

  // Attach Callback for Encoder Data
  mqttClient.add_message_callback(MQTT_EVENT_DATA, on_encoder, mqttClient.getClientId(), "on_encoder");
  // Attach Callback for Current Data
  mqttClient.add_message_callback(MQTT_EVENT_DATA, on_current, mqttClient.getClientId(), "on_current");

  // Subscribe to Encoder Topic
  mqttClient.subscribe("encoder/Arm1_Test", 1);
  // Subscribe to Current Topic
  mqttClient.subscribe("current/Arm1_Test", 1);
}

void loop () {
  Serial.printf("<ARM>: encoder_val = %d\n", encoder_val);
  Serial.printf("<ARM>: current_val = %d\n", current_val);
  delay(5000);
}
