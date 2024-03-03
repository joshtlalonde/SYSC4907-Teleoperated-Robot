#include "Arduino.h"
#include "Motor_Control.h"

Motor_Control::Motor_Control(Encoder& encoder, Current_Sensor& currentSensor, 
                             int pwmChannel, int frequency, int resolution, 
                             int pwmPin, int dirPin, bool verbose) 
                             : encoder(encoder), currentSensor(currentSensor)
{
    this->encoder = encoder;
    this->currentSensor = currentSensor;

    // Current Encoder Target is 0
    this->encoderTarget = 0;

    /* Set Pin Defintions */
    this->pwmPin = pwmPin;
    this->dirPin = dirPin;
    // Set pinMode to OUTPUT
    pinMode(pwmPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    /* PWM Configuration */
    // // Configuration of channel with the chosen frequency and resolution
    // ledcSetup(pwmChannel, frequency, resolution);
    // // Assigns the PWM channel to PWM_R pin
    // ledcAttachPin(pwmPin, pwmChannel);

    this->verbose = verbose;
}

int Motor_Control::getEncoderCount() {
    return this->encoder.getCount();
}

double Motor_Control::getEncoderTheta() {
    return this->encoder.encoderToAngle(this->encoder.getCount());
}

int Motor_Control::getCurrentAmps() {
    return this->currentSensor.getCurrent();
}

int64_t Motor_Control::getPrevEncoderError() {
    return this->encoder.getPrevError();
}

void Motor_Control::setPrevEncoderError(int64_t err) {
    this->encoder.setPrevError(err);
}

void Motor_Control::setEncoderTarget(int target) {
    this->encoderTarget = target;
}

int Motor_Control::getEncoderTarget() {
    return this->encoder.getTarget();
}

void Motor_Control::setCurrentTarget(float target) {
    this->currentSensor.setTarget(target);
}

float Motor_Control::getCurrentTarget() {
    return this->currentSensor.getTarget();
}

void Motor_Control::setMotor(int direction, int pwmVal, double x, double y) {
    if (this->verbose)
        Serial.printf("<MOTOR_CONTROL>: Setting Motor Direction to %s and PWM to %d\n", 
                      direction == 1 ? "CW" : "CCW", pwmVal);

    if (y < 0) {
        Serial.printf("<MOTOR_CONTROL>: Outside of Range, shutting off motors\n");
        analogWrite(this->pwmPin, 0); // For now turn off
        
        // if (direction == MOTOR_DIR_CW) {   // Spins in CCW direction
        //     digitalWrite(this->dirPin, LOW); 
        // }
        // else if (direction == MOTOR_DIR_CCW) {   // Spins in CW direction
        //     digitalWrite(this->dirPin, HIGH);
        // }
        // else {
        //     analogWrite(this->pwmPin, 0); //Do not spin motor
        //     return; // Exit now
        // }

        return;
    }
    
    if (direction == MOTOR_DIR_CW) {   // Spins in CW direction
        digitalWrite(this->dirPin, HIGH); 
    }
    else if (direction == MOTOR_DIR_CCW) {   // Spins in CCW direction
        digitalWrite(this->dirPin, LOW);
    }
    else {
        analogWrite(this->pwmPin, 0); //Do not spin motor
        return; // Exit now
    }

    if (pwmVal > 255) {
        analogWrite(this->pwmPin, 255); 
    } else {
        analogWrite(this->pwmPin, pwmVal); 
    }
}

// void Motor_Control::PID_Encoder(int target) {

// }

// /** TODO: Still to be implemented */
// void Motor_Control::PID_Current(float target) {
//     // Call get current function in currentSensor class
//     float current = this->currentSensor.getCurrent(); 

//     // PID Constants
//     float kp = 1; 

//     // error 
//     int e = current - target; 
  
//     // Control Signal 
//     float u = kp * e; 

//     // Set motor power
//     // motor power
//     float pwr = fabs(u);
//     if (pwr > 255) {
//         pwr = 255;
//     }
//  }
