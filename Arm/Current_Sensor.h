#ifndef CURRENT_SENSOR_h
#define CURRENT_SENSOR_h

//Create a Motor Control Class
class Current_Sensor {
    private:
        int pinA;
        float current;
        
    public: 
        Current_Sensor(int pin);

        float getCurrent();
        void setCurrent(float current);
};

#endif