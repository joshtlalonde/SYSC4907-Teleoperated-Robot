#ifndef ARM_h
#define ARM_h

// #include "Arduino.h"
#include "MQTTClient.h"
#include "Motor_Control.h"

#define TARGET_OFFSET 50 // This is 2degrees (aim for lower...)
#define PID_KP 0.005
#define PID_KD 0

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

        /** 
         * Set the Target Value for both Encoders.
         * Functions main purpose is for when receiving MQTT.
         * 
         * NOTE: Raises the newTargetFlag when called
         * 
         * int targetL: Left Encoder's Target Value
         * int targetR: Right Encoder's Target Value
         */
        void setEncoderTarget(int targetL, int targetR);

        /**
         * Utilizes the PID_Encoder to move the Motors into position
         * 
         * TODO: We should move the PID_Encoder method to here
         * In here instead of updating one encoder at a time we will do
         * both at the same time. It could work something like:
         * encoder1.gerCount(); encoder2.getCount(); and so on.
         * Doing this will act as a multithreaded program and allow for both 
         * encoder calculations to happen at the same time
         */
        // void updateMotorPosition();
        // void updateMotorPosition(float* prevTime, int* prevErr_L, int* prevErr_R);

        /**
         * Runs the setup sequence for MQTT. This must be run in 
         * the setup() function of the main .ino file, if you want
         * MQTT functionality.
         * 
         * char* ssid: WiFi SSID to connect to (can be any available wifi), char* password, 
         *              char* mosqutto_ip, int mosquitto_port
         */
        void mqtt_setup(char* ssid, char* password, 
                        char* mosqutto_ip, int mosquitto_port);

        /**
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