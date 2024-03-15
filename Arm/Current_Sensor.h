#ifndef CURRENT_SENSOR_h
#define CURRENT_SENSOR_h

#include "Arduino.h"

//Create a Motor Control Class
class Current_Sensor {
    private:
        int pin;
        float current;
        float target;
        float prevTime;
        float prevError;
        float voltage_offset;
        
    public: 
        Current_Sensor(int pin, float voltage_offset);

        float getTarget();
        void setTarget(float target);
        float getPrevTime();
        void setPrevTime(float time);
        float getPrevError();
        void setPrevError(float error);

        /*  SparkFun ACS712 and ACS723 Demo
            Created by George Beckstein for SparkFun
            4/30/2017
            Updated by SFE
            6/14/2018

            Uses an Arduino to set up the ACS712 and ACS723 Current Sensors
            See the tutorial at: https://learn.sparkfun.com/tutorials/current-sensor-breakout-acs723-hookup-guide

            Parts you may need:
            - 100 Ohm, 1/2W or greater resistor OR two 220 Ohm 1/4 resistors in parallel
            - ACS712 Breakout with on-board amplifier or ACS723 Current Sensor (Low Current)

            Optional equipment:
            - Oscilloscope
            - Multimeter (or two)
            - A power supply with current limiting/constant current would be handy to calibrate the device without using resistors
        */
        float getCurrent();
};

#endif