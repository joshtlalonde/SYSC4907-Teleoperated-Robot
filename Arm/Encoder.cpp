#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder(Encoders& encoderObj, 
                 int pinA, int pinB) 
                 : encoderObj(encoderObj)
{
    this->encoderObj = encoderObj;

    // Initialize Previous Time
    this->prevTime = 0;
    // Initialize Previous Error
    this->prevError = 0;

    /* Set Pin Defintions */
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
}

int Encoder::getCount() {
    return this->encoderObj.getEncoderCount();
}

float Encoder::getPrevTime() {
    return this->prevTime;
}

void Encoder::setPrevTime(float time) {
    this->prevTime = time;
}

int Encoder::getPrevError() {
    return this->prevError;
}

void Encoder::setPrevError(int error) {
    this->prevError = error;
}
