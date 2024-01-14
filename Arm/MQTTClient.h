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

        /*
         * Setup internet connection. 
         * Attempts Ethernet connection first, if it doesn't connect it will attempt WiFi Connection
         *
         * char* ssid: String of the name of the WiFi network to connect to
         * char* password: String of the password of the WiFi network to connect to
         */
        bool begin(char* ssid, char* password);

        /* 
         * Connects to Broker
         *
         * char* broker_ipAddr: Brokers IP Address in string format e.g. "192.168.2.1"
        */
        bool connect(char* broker_ipAddr, int broker_port);

        /*
         * Disconnects from Broker
         */
        esp_err_t disconnect();

        /*
         * Starts Broker Comms
         */
        esp_err_t start();

        /* 
         * Adds a callback function to a topic event on the broker
         *
         * esp_mqtt_event_id_t event: MQTT Event to set this callback to (Ex: MQTT_EVENT_CONNECTED)
         * const esp_event_handler_t& event_handler: Function to run when the event occurs (Ex: on_connect)
         * void *event_handler_arg: The object to pass to the function (Ex: this->client_id)
         * char* callback_name: String for the name of the callback function, can be referenced later
         */
        esp_err_t add_message_callback(esp_mqtt_event_id_t event, const esp_event_handler_t& event_handler, void *event_handler_arg, char* callback_name);
        
        /* 
         * Publish a message to the broker topic
         * 
         * const char *topic: String of the topic name to publish to (Ex: encoder/*)
         * const char *data: String of the data to send over the topic. Should be in JSON format
         * int len: Length of the String being sent
         * int qos: QoS level of the MQTT message
         * int retain: Retain value (TODO reason found on site)
         */
        int publish(const char *topic, const char *data, int len, int qos, int retain);

        /* 
         * Subscribe to a topic on the broker
         *
         * const char *topic: String of the Topic to subscribe to (Ex: encoder/*)
         * int qos: QoS level to send the Subscripton MQTT message on
         */ 
        int subscribe(const char *topic, int qos);
        /*
         * Unsubscribe from a topic on the broker
         *
         * const char *topic: String of the Topic to subscribe to (Ex: encoder/*)
         */
        int unsubscribe(const char *topic);
};

#endif