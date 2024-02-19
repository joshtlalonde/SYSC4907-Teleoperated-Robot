#include "Arduino.h"
#include "Current_Sensor.h"

Current_Sensor::Current_Sensor(int pin)
{
    // Initialize Current Value
    /** TODO: Read this value from analog pin? */
    this->current = 0; 

    /* Set Pin Defintions */
    pinMode(pin, INPUT);
}

float Current_Sensor::getCurrent() {
    /** TODO: Read this value from analog pin? */
    return this->current;
}

void Current_Sensor::setCurrent(float current) {
    this->current = current;
}

float Current_Sensor::getTarget() {
    return this->target;
}

void Current_Sensor::setTarget(float target) {
    this->target = target;
}