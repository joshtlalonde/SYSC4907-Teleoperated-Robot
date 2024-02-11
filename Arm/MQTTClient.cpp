#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"
#include <fstream>

const char* MAC_ADDR_TRAINEE = "60:55:f9:d9:d1:34";
const char* MAC_ADDR_TRAINER = "60:55:f9:d9:d9:44";

/****** TODO: Change all of the esp_err_t to bool and have them handle the err within the function *****/

// Constructor definition
MQTTClient::MQTTClient(char* client_id, Ether& ethernet, Wireless& wifi)
                      : ethernet(ethernet), wifi(wifi) 
{
    this->client = NULL;
    this->client_id = client_id;
    // this->client_id = NULL;
    this->ethernet = ethernet;
    this->wifi = wifi;
}

char* MQTTClient::getClientId() {
    return client_id;
}

void MQTTClient::setClientId(char* client_id) {
    this->client_id = client_id;
}

bool MQTTClient::begin(char* ssid, char* password) {
	Serial.println("<MQTTClient>: Connecting to internet...");

	// Attempt to connect through ethernet first
	if (this->ethernet.begin()) {
		Serial.println("<MQTTClient>: Connected through Ethernet");
        // Must turn on WiFi module to work
        WiFi.begin();
		return true;
	}

	// If ethernet does not work connect through wifi
	if (this->wifi.begin(ssid, password)) {
		Serial.println("<MQTTClient>: Connected through Wifi");
		return true;
	}

	Serial.println("<MQTTClient>: FAILED to connect to internet");
	return false;
}

const char* CA_ROOT_CERT = "-----BEGIN CERTIFICATE-----\n\
MIIDqTCCApECFEShFwlSY0ZRsbknfESNroARPO5qMA0GCSqGSIb3DQEBCwUAMIGQ\n\
MQswCQYDVQQGEwJDQTEQMA4GA1UECAwHT250YXJpbzEPMA0GA1UEBwwGT3R0YXdh\n\
MRwwGgYDVQQKDBNDYXJsZXRvbiBVbml2ZXJzaXR5MREwDwYDVQQLDAhTWVNDNDkw\n\
NzEtMCsGCSqGSIb3DQEJARYeam9zaHRsYWxvbmRlQGNtYWlsLmNhcmxldG9uLmNh\n\
MB4XDTIzMTAxNjE0NDIyOFoXDTI0MTAxNTE0NDIyOFowgZAxCzAJBgNVBAYTAkNB\n\
MRAwDgYDVQQIDAdPbnRhcmlvMQ8wDQYDVQQHDAZPdHRhd2ExHDAaBgNVBAoME0Nh\n\
cmxldG9uIFVuaXZlcnNpdHkxETAPBgNVBAsMCFNZU0M0OTA3MS0wKwYJKoZIhvcN\n\
AQkBFh5qb3NodGxhbG9uZGVAY21haWwuY2FybGV0b24uY2EwggEiMA0GCSqGSIb3\n\
DQEBAQUAA4IBDwAwggEKAoIBAQC87DADBiBgpq3rj6JtYvP42qe65eKCAzVl8XPH\n\
YELU14jAmjEbrqK5u+MKu5XFA0VH1aiW7KreB94Rrk7u8kwL0TBpzH6nsvGQ8xOb\n\
qKa5bjC/bnjH84SfuteP+N7ys1Am9/4tvtkWfSSgGFDwsArB26JR2dqsUHVlIu2V\n\
EUF87w0eov/IHFc1sLHlSXCr+r5BmyDY5hIKUPMooCEZuNAtOwemPPj9Uf0GCPUk\n\
TXCD3ruFfQZjnRWS6VefQgOK3exuBdJ5C75wC8Mi85V1cA2FpEnkDyghoeMi3Z5W\n\
aVLxLQ+9ZrbOfjDfP5U5GeoFESElS+huf6GSS6fSbnBZ7eUHAgMBAAEwDQYJKoZI\n\
hvcNAQELBQADggEBABlsUSFzGouX9MJIz3eKcJSDI8ofrNE7jBZiKkc2TrrdqS13\n\
baEPVPdbil6KwJTb/fYJOculKsa4jZ6O3O9l4OmcE6GH3+lYuxfQHY4Vj/2eWjz6\n\
1tHsw3PpytoGrCIaVGzw6Ngq5o9NG+sxHlGT/8d7IxeRpICzkuqwShCGIhjTFpuo\n\
5UFtfD7bziq+eq862F+UX+EelA5sm3VlywcYDs86R4kB6fpAbOlNDBDUSCp8+T6A\n\
KYHJBEI3d1FqFcdGTPMIcJr69Mspw0PAtZbsbtLexoHPagfOQYXQ+Mb3BTcBMmYr\n\
6YKIpJlY4RDKzDencpJlyO9KUaU2dpCB7q7ZaXc=\n\
-----END CERTIFICATE-----\n";

bool MQTTClient::connect(char* broker_hostname, int broker_port) {
    Serial.printf("<MQTTClient>: Initialize Connection to Broker at %s:%d...", broker_hostname, broker_port);

    // Update clientID based on MAC Address
    char mac[50];
    wifi.getMacAddr(mac);
    if (strcmp(mac, MAC_ADDR_TRAINER) == 0) {
      this->client_id = "trainer";
    } 
    else if (strcmp(mac, MAC_ADDR_TRAINEE) == 0) {
      this->client_id = "trainee";
    }
    else {
      Serial.println( " ERROR: MAC Address of ESP32 is not known!");
    }

    // Set Configuration of Broker
    const esp_mqtt_client_config_t mqtt_cfg = {
      .host = broker_hostname,
      .port = broker_port,
      .cert_pem = CA_ROOT_CERT,
      .transport = MQTT_TRANSPORT_OVER_SSL,
      .skip_cert_common_name_check = true
    };
    this->client = esp_mqtt_client_init(&mqtt_cfg);
    if (this->client == NULL) {
        Serial.printf(" Error Initializing Broker at %s:%d for %s\n", broker_hostname, broker_port, client_id);
        return false;
    }

    Serial.printf(" Initialized Connection\n", broker_hostname, broker_port);

    // Adding Callback Functions
    add_message_callback(MQTT_EVENT_CONNECTED, on_connect, this->client_id, "on_connect");
    add_message_callback(MQTT_EVENT_DISCONNECTED, on_disconnect, this->client_id, "on_disconnect");
    add_message_callback(MQTT_EVENT_PUBLISHED, on_publish, this->client_id, "on_publish");
    add_message_callback(MQTT_EVENT_SUBSCRIBED, on_subscribe, this->client_id, "on_subscribe");

    return true; 
}

esp_err_t MQTTClient::disconnect() {
    Serial.printf("<MQTTClient>: client_id=%s is disconnecting from MQTT Broker\n", client_id);
    return esp_mqtt_client_disconnect(client);
}

esp_err_t MQTTClient::start() {
  Serial.printf("<MQTTClient>: Starting MQTT Client with client_id '%s'\n", client_id);

  // Start Connection
	return esp_mqtt_client_start(this->client);
}

esp_err_t MQTTClient::add_message_callback(esp_mqtt_event_id_t event, const esp_event_handler_t& event_handler, void *event_handler_arg, char* callback_name) {
    Serial.printf("<MQTTClient>: Adding message callback: %s\n", callback_name);
    return esp_mqtt_client_register_event(this->client, event, event_handler, event_handler_arg);
}

int MQTTClient::publish(const char *topic, const char *data, int len, int qos, int retain) {
    Serial.printf("<MQTTClient>: Publishing Message: client_id=%s, topic=%s, data=%s, len=%d, qos=%d, retain=%d\n", client_id, topic, data, len, qos, retain);
    return esp_mqtt_client_publish(this->client, topic, data, len, qos, retain);
}

int MQTTClient::subscribe(const char *topic, int qos) {
    Serial.printf("<MQTTClient>: Subscribing to Topic: client_id=%s, topic=%s, qos=%d\n", client_id, topic, qos);
    return esp_mqtt_client_subscribe(this->client, topic, qos);
}

int MQTTClient::unsubscribe(const char *topic) {
    Serial.printf("<MQTTClient>: Unsubscribing from Topic: client_id=%s, topic=%s\n", client_id, topic);
    return esp_mqtt_client_unsubscribe(this->client, topic);
}