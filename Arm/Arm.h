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

        void encoder_jsonify(char* encoder_val_str);
        void current_jsonify(char* current_val_str);
};

#endif