#ifndef MQTTCLIENT_h
#define MQTTCLIENT_h

// #include "Arduino.h"
#include "Ether.h"
#include "Wireless.h"
#include "mqtt_client.h"

class MQTTClient {
    private:
        esp_mqtt_client_handle_t client;
        char* client_id;
        Ether ethernet;
        Wireless wifi;

    public:
        MQTTClient(char* client_id, Ether& ethernet, Wireless& wifi);

        char* getClientId();
        void setClientId(char* client_id);

        // Setup internet connection
        bool begin(char* ssid, char* password);
        /* 
          Connects to Broker
          Inputs:
            char* broker_ipAddr: Brokers IP Address in string format e.g. "192.168.2.1"

        */
        bool connect(char* broker_ipAddr, int broker_port);
        // Disconnects from Broker
        esp_err_t disconnect();
        // Starts Broker Comms
        esp_err_t start();
        // Adds a callback function to a topic event on the broker
        esp_err_t add_message_callback(esp_mqtt_event_id_t event, const esp_event_handler_t& event_handler, void *event_handler_arg, char* callback_name);
        // Publish a message to the broker topic
        int publish(const char *topic, const char *data, int len, int qos, int retain);
        // Subscribe to a topic on the broker
        int subscribe(const char *topic, int qos);
        // Unsubscribe from a topic on the broker
        int unsubscribe(const char *topic);
};

#endif