#include <Arduino.h>
#include "Encoder.h"

Encoder::Encoder(int pinA, int pinB)
{
    // Initialize Previous Time
    this->prevTime = 0;
    // Initialize Previous Error
    this->prevError = 0;

    /* Set Pin Defintions */
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

    // Setup ESPEncoder
    this->encoderObj.attachFullQuad(pinA, pinB);
}

int Encoder::getCount() {
    return this->encoderObj.getCount();
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
