#ifndef CURRENT_SENSOR_h
#define CURRENT_SENSOR_h

#include "Arduino.h"

//Create a Motor Control Class
class Current_Sensor {
    private:
        float current;
        float target;
        
    public: 
        Current_Sensor(int pin);

        float getCurrent();
        void setCurrent(float current);
        float getTarget();
        void setTarget(float target);
};

#endif