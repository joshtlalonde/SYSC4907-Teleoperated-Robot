#ifndef MOTOR_CONTROL_h
#define MOTOR_CONTROL_h

#include "Encoder.h"
#include "Current_Sensor.h"

#define MOTOR_DIR_CW 1
#define MOTOR_DIR_CCW -1

//Create a Motor Control Class
class Motor_Control {
    private:
        Encoder encoder; 
        Current_Sensor currentSensor; // Currently not in use
        int currentDir; 
        int currentPWM; 
        int encoderTarget;
        int pwmPin;
        int dirPin;
        bool verbose;
  
    public: 
        Motor_Control(Encoder& encoder, Current_Sensor& currentSensor, 
                      int pwmChannel, int frequency, int resolution, 
                      int pwmPin, int dirPin, bool verbose);

        int getCurrentDir();
        int getCurrentPWM();
        int getEncoderCount();
        int getCurrentAmps();

        /** 
         * Updates the Target Value of the encoder.
         * This will be a value that the PID will attempt
         * to reach
         * 
         * int target: target value to set encoder to reach.
         */
        void setEncoderTarget(int target);
        int getEncoderTarget();

        /** 
         * Updates the motor speed and direction
         * 
         * int dir: Direction to spin, 1 = MOTOR_DIR_CW / -1 = MOTOR_DIR_CCW
         * int pwmVal: Speed of the motor, 0 = stop / 255 = full speed
         */
        void setMotor(int dir, int pwmVal); 
        // void PID_Encoder(int target);
        // void PID_Current(float target);
};

#endif