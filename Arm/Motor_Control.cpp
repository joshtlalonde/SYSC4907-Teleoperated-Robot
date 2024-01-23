#include "Arduino.h"
#include "Motor_Control.h"

#define PID_KP 1
#define PID_KD 0.75

Motor_Control::Motor_Control(Encoder& encoder, Current_Sensor& currentSensor, 
                             int pwmChannel, int frequency, int resolution, 
                             int pwmPin, int dirPin, bool verbose) 
                             : encoder(encoder), currentSensor(currentSensor)
{
    this->encoder = encoder;
    this->currentSensor = currentSensor;

    // Current Direction is STOPPED
    this->currentDir = 0;
    // Current PWM is STOPPED
    this->currentPWM = 0;

    /* Set Pin Defintions */
    this->pwmPin = pwmPin;
    this->dirPin = dirPin;
    // Set pinMode to OUTPUT
    pinMode(pwmPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    /* PWM Configuration */
    // Configuration of channel with the chosen frequency and resolution
    ledcSetup(pwmChannel, frequency, resolution);
    // Assigns the PWM channel to PWM_R pin
    ledcAttachPin(pwmPin, pwmChannel);
    // Create the selected output voltage
    // ledcWrite(pwmChannel, 127); // 1.65 V

    this->verbose = verbose;
}

void Motor_Control::setMotor(int direction, int pwmVal)
{
    if (this->verbose)
        Serial.printf("<MOTOR_CONTROL>: Setting Motor Direction to %s and PWM to %d\n", 
                      direction == 1 ? "CW" : "CCW", pwmVal);
    
    if(direction == 1){   // Spins in CW direction
        // digitalWrite(this->dirPin, LOW);
        digitalWrite(this->dirPin, HIGH); 
        // ledcWrite(this->pwmChannel, pwmVal); 
        analogWrite(this->pwmPin, pwmVal); 
    }
    else if(direction == -1){   // Spins in CCW direction
        // digitalWrite(this->dirPin, HIGH);
        digitalWrite(this->dirPin, LOW);
        analogWrite(this->pwmPin, pwmVal); 
    }
    else{
        analogWrite(this->pwmPin, 0); //Do not spin motor
    }  
}

void Motor_Control::PID_Encoder(int target){
    // PID Constants
    float kp = PID_KP; 
    float kd = PID_KD;

    int err = 0;
    int currEncoderCount = 0;

    /** TODO: Need to figure out which values to use here to hit the target and the stop */
    while (abs(this->encoder.getCount()) > abs(target) + 250 || abs(this->encoder.getCount()) < abs(target) - 250) {
        // Get Current Encoder Count 
        currEncoderCount = this->encoder.getCount();
        /** TODO: Check if the encoder count is valid
        * if (currentEncoderCount "< or >" max/min value) {}
        */

        if (this->verbose)
            Serial.printf("<MOTOR_CONTROL>: Updating PID_Encoder Target\n \tcurrEncoderCount: %d, newTarget: %d\n", 
                          currEncoderCount, target);

        // Get Current Time
        long currTime = micros();
        // Get change in time
        float deltaT = ((float) (currTime - this->encoder.getPrevTime())) / (float)( 1.0e6 ); 
        // Set Previous Time to Current Time
        this->encoder.setPrevTime(currTime);

        // Get Error 
        err = currEncoderCount - target; 
        // derivative
        float dedt = ((float) (abs(err) - abs(this->encoder.getPrevError()))) / (float)(deltaT);
        // Set Previous Error to Current Error
        this->encoder.setPrevError(err);

        // Control Signal 
        float u = (kp * err) + (kd * dedt);

        // Update Motor Speed
        float motorPWM = fabs(u); /** TODO: Determine a way to make the u value more propostanal to our range of 0 - 255 
                                    * If this method doesn't work let's change the precision of the getCount to divide by 4.
                                    */
        if(motorPWM > 10) { 
            /** NOTE: We may have to increase the max speed. 
            * Its set slow here to ensure we dont damage anything
            */
            motorPWM = 10;
        }

        // Set Motor Direction
        int motorDir = 1;
        if (u < 0) {
            motorDir = -1;
        }

        Serial.printf("err: %d\n", err);

        // Update Motor Speed and Direction
        this->setMotor(motorDir, motorPWM);
    }

    this->setMotor(0, 0);
}

/** TODO: Still to be implemented */
void Motor_Control::PID_Current(float target) {
    // Call get current function in currentSensor class
    float current = this->currentSensor.getCurrent(); 

    // PID Constants
    float kp = 1; 

    // error 
    int e = current - target; 
  
    // Control Signal 
    float u = kp * e; 

    // Set motor power
    // motor power
    float pwr = fabs(u);
    if (pwr > 255) {
        pwr = 255;
    }
 }
