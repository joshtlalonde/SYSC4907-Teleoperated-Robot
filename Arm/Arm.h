#ifndef ARM_h
#define ARM_h

#include "Arduino.h"
#include "ArduinoJson.h"

#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"
#include "Motor_Control.h"
#include "Kinematics.h"

#define ENCODER_TARGET_OFFSET 1 // This is the error that we want to be within for encoder value
#define ARMATURE_CURRENT_TARGET_OFFSET 1
#define ENCODER_PID_KP 0.225
#define ENCODER_PID_KD 0 
#define ARMATURE_CURRENT_PID_KP 0.225
#define ARMATURE_CURRENT_PID_KD 0 
#define MIN_PWM 29
#define MAX_PWM 1000

// TEMP ARM CLASS
class Arm {
    private:
        MQTTClient mqtt_client;
        Motor_Control motorL;
        Motor_Control motorR;
        Kinematics kinematics;
        bool newEncoderTargetFlag;
        bool newCurrentTargetFlag;
        unsigned long encoderPIDPrevT;
        unsigned long armatureCurrentPIDPrevT;
        bool verbose;
    public:
        Arm(MQTTClient& mqtt_client, Motor_Control& motorL, 
            Motor_Control& motorR, Kinematics& kinematics, bool verbose);

        char* getClientId();
        bool getNewEncoderTargetFlag();
        bool getNewCurrentTargetFlag();

        /** 
         * Set the Target Value for both Encoders.
         * Functions main purpose is for when receiving MQTT encoder updates.
         * 
         * NOTE: Raises the newTargetFlag when called
         * 
         * int targetL: Left Encoder's Target Value
         * int targetR: Right Encoder's Target Value
         */
        void setEncoderTarget(int targetL, int targetR);

        /** 
         * Set the Target Value for both Current Sensors.
         * Functions main purpose is for when receiving MQTT force updates.
         * 
         * float forceX and forceY: Applied force to Jacobian to calculate torque
         *                       Converts torque to Current
         */
        void setCurrentTarget(float forceX, float forceY);

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
        void dual_PID();
        // void updateMotorPosition(float* prevTime, int* prevErr_L, int* prevErr_R);

        void dual_Current_PID();

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

        /**
         * Used to JSONify the current current value. Allows you to be able to send the current value of MQTT
         * The value is written to the current_val_str
         * 
         * Input: current_val_str = pointer to a string, will have the JSON value written to it
         */
        void current_jsonify(char* current_val_str);

        /**
         * Used to JSONify the current CHANGE in position value. Allows for the update of (x, y)
         * that can be sent to the virtual environment 
         * 
         * Input: position_val_str = pointer to a string, will have the JSON value written to it
         */
        void position_jsonify(char* position_val_str);

        // Publish to the Encoder Topic
        bool publish_encoder();

        // Publish to the Current Topic
        bool publish_current();

        // Publish to Position Topic
        bool publish_position();
};

#endif