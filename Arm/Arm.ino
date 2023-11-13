#include <SPI.h>
#include "mqtt_client.h"
#include "EthernetENC.h"
#include "WiFi.h"

// the media access control (ethernet hardware) address for the shield:
byte Ethernet_MAC[] = { 0x40, 0x8D, 0x5C, 0xE7, 0xA5, 0x98 };  
//the IP address for the shield:
IPAddress Ethernet_IP(169, 254, 119, 79); 
// The IP address of the mosquitto instance
IPAddress Mosquitto_IP(192, 168, 2, 12); 
// The port of the Mosquitto instance
int Mosquitto_Port = 1883;

EthernetClient client;

void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    // return ESP_OK;
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin();

  Ethernet.init(34);
  Ethernet.begin(Ethernet_MAC);
  // Ethernet.begin(Ethernet_MAC, Ethernet_IP);
  // if (Ethernet.begin(Ethernet_MAC) == 0) {
  //   Serial.println("Failed to configure Ethernet using DHCP");
  //   // try to congifure using IP address instead of DHCP:
  //   Ethernet.begin(Ethernet_MAC, Ethernet_IP);
  // }

  delay(2000);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5100) {
    Serial.println("W5100 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5200) {
    Serial.println("W5200 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5500) {
    Serial.println("W5500 Ethernet controller detected.");
  }
  else {
    Serial.println(Ethernet.hardwareStatus());
  }

    if (Ethernet.linkStatus() == Unknown) {
    Serial.println("Link status unknown. Link status detection is only available with W5200 and W5500.");
  }
  else if (Ethernet.linkStatus() == LinkON) {
    Serial.println("Link status: On");
  }
  else if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Link status: Off");
  }
  else {
    Serial.println(Ethernet.linkStatus());
  }

	// put your setup code here, to run once:
	const esp_mqtt_client_config_t mqtt_cfg = {
		.uri = "mqtt://LAPTOP-HL4N9U5Q:1883" // Try testing using just the IP and Port... The mqtt:// might be trying to make an external connection
    // .uri = "mqtt://mqtt.eclipseprojects.io"
	};
  Serial.println("Init MQTT Client");
	esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

	Serial.println("Register MQTT Client Handler");
	esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, mqtt_client);
  Serial.println("Start MQTT Client");
	esp_err_t ret = esp_mqtt_client_start(mqtt_client);
  ESP_LOGI ("TEST", "Client connect. Error = %d %s", err, esp_err_to_name (err));
	Serial.printf("ret: %u", ret);
}

void loop () {
  // if (client.available()) {
  //   char c = client.read();
  //   Serial.print(c);
  // }

  // if (!client.connected()) {
  //   Serial.println();
  //   Serial.println("disconnecting.");
  //   client.stop();
  //   for(;;)
  //     ;
  // }
}
