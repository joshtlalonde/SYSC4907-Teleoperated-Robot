
#include "Current_Sensor.h"

Current_Sensor::Current_Sensor(int pin, float voltage_offset)
{
    // Initialize Values
    this->pin = pin;
    this->current = 0; 
    this->target = 0;
    this->voltage_offset = voltage_offset;

    // PID setup
    prevError = 0;

    /* Set Pin Defintions */
    pinMode(pin, INPUT);
}

float Current_Sensor::getCurrent() {
    const int avgSamples = 10; // Number of samples to average the reading over
    const float sensitivity = 155.0 / 150.0; //155mA per 150mV 
    int sensorValue = 0; 
    float Vref = 1500 ; // Output voltage with no current: ~ 1500mV or 1.5V

    // read the analog in value:
    for (int i = 0; i < avgSamples; i++)
    {
        sensorValue += analogRead(this->pin);

        // wait 2 milliseconds before the next loop
        // for the analog-to-digital converter to settle
        // after the last reading:
        delay(2);
    }

    sensorValue = sensorValue / avgSamples;

    // The on-board ADC is 13-bits -> 2^13 = 8192 -> 3.3V / 8192 ~= 0.402mV
    // The voltage is in millivolts
    float voltage = 0.402 * sensorValue - this->voltage_offset; // 428 is offset to get to approx. 1500mV (Gain??)

    // This will calculate the actual current (in mA)
    // Using the Vref and sensitivity settings you configure
    float current = (voltage - Vref) * sensitivity;

    return current;
}

float Current_Sensor::getTarget() {
    return this->target;
}

void Current_Sensor::setTarget(float target) {
    this->target = target;
}

float Current_Sensor::getPrevTime() {
    return this->prevTime;
}

void Current_Sensor::setPrevTime(float time) {
    this->prevTime = time;
}

float Current_Sensor::getPrevError() {
    return this->prevError;
}

void Current_Sensor::setPrevError(float error) {
    this->prevError = error;
}




