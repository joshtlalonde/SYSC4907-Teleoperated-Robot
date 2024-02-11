#ifndef ENCODER_h
#define ENCODER_h

#include <ESP32Encoder.h>

//Create a Motor Control Class
class Encoder {
    private:
        // Encoders encoderObj;
        ESP32Encoder encoderObj;
        int pinA; 
        int pinB;
        float prevTime;
        int prevError;
        
    public: 
        Encoder(int pinA, int pinB);

        int getCount();
        float getPrevTime();
        void setPrevTime(float time);
        int getPrevError();
        void setPrevError(int error);
};

#endif