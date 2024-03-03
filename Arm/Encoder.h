#ifndef ENCODER_h
#define ENCODER_h

#include <ESP32Encoder.h>

//Create a Motor Control Class
class Encoder {
    private:
        // Encoders encoderObj;
        ESP32Encoder encoderObj;
        float prevTime;
        int prevError;
        int target;
        double init_theta;
        
    public: 
        Encoder(int pinA, int pinB, double init_theta);

        int getCount();
        float getPrevTime();
        void setPrevTime(float time);
        int getPrevError();
        void setPrevError(int64_t error);
        int getTarget();
        void setTarget(int target);

        double encoderToAngle(int encoder);
        int angleToEncoder(double angle);
};

#endif