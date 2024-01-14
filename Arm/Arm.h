#ifndef ARM_h
#define ARM_h

// #include "Arduino.h"
#include "MQTTClient.h"

// TEMP ARM CLASS
class Arm {
    private:
        MQTTClient mqtt_client;
        float encoder_left;
        float encoder_right;
        float current_left;
        float current_right;

    public:
        Arm(MQTTClient& mqtt_client);

        float getEncoderLeft();
        void setEncoderLeft(float value);
        float getEncoderRight();
        void setEncoderRight(float value);
        float getCurrentLeft();
        void setCurrentLeft(float value);
        float getCurrentRight();
        void setCurrentRight(float value);

        char* getClientId();

        /*
         * Used to JSONify the current encoder value. Allows you to be able to send the encoder value of MQTT
         * The value is written to the encoder_val_str
         * 
         * Input: encoder_val_str = pointer to a string, will have the JSON value written to it
         */
        void encoder_jsonify(char* encoder_val_str);

        /*
         * Used to JSONify the current current value. Allows you to be able to send the current value of MQTT
         * The value is written to the encoder_val_str
         * 
         * Input: encoder_val_str = pointer to a string, will have the JSON value written to it
         */
        void current_jsonify(char* current_val_str);

        // Publish to the Encoder Topic
        bool publish_encoder(MQTTClient mqttClient);

        // Publish to the Current Topic
        bool publish_current(MQTTClient mqttClient);
};

#endif