#ifndef MOTOR_CONTROL_h
#define MOTOR_CONTROL_h

//Create a Motor Control Class
class Motor_Control {
    private:
        Encoder encoder; 
        Current_Sensor currentSensor; // Currently not in use
        int currentDir; 
        int currentPWM; 
        int pwmPin;
        int dirPin;
        bool verbose;
  
    public: 
        Motor_Control(Encoder& encoder, Current_Sensor& currentSensor, 
                      int pwmChannel, int frequency, int resolution, 
                      int pwmPin, int dirPin, bool verbose);

        void PID_Encoder(int target);
        void PID_Current(float target);
        void setMotor(int dir, int pwmVal); 
};

#endif