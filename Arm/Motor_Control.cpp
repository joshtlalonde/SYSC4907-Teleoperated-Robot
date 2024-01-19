// #include "Motor_Control.h"
// #include <QuadratureEncoder.h>
// // must also have enableInterrupt.h library

// Motor_Control::Motor_Control(Encoders& encoder, 
//                              int pwmChannel, int frequency, 
//                              int resolution, int pwmPin) 
//                              : encoder(encoder)
// {
//     this->encoder = encoder;
//     // Current Direction is STOPPED
//     this->currentDir = 0;
//     // Current PWM is STOPPED
//     this->currentPWM = 0;
// }

// void Motor_Control::setMotor(int dir, int pwmVal, 
//                              int pwm, int dir1)
// {
//     analogWrite(pwm,pwmVal); 
//     if(dir == 1){   // Spins in CW direction
//         digitalWrite(dir1, LOW); 
//     }
//     else if(dir == -1){   // Spins in CCW direction
//         digitalWrite(dir1, HIGH);
//     }
//     else{
//         analogWrite(PWM, 0); //Do not spin motor
//     }  
// }

// void PID_Encoder(int target){
//     // Get current Encoder Count 
//     currentLeftEncoderCount = Encoder.getEncoderCount();
//     // time difference
//     long currT = micros();
//     float deltaT = ((float) (currT - prevT))/( 1.0e6 );
//     prevT = currT;

//     // PID Constants
//     float kp = 1; 
//     float kd = 0.75;

//     // error 
//     int e = currentLeftEncoderCount - target; 

//     // derivative
//     float dedt = (e-eprev)/(deltaT);

//     // Control Signal 
//     float u = kp*e + kd*dedt;

//     // Set motor power
//     // motor power
//     float pwr = fabs(u);
//     if(pwr > 10) { 
//     // Note: We may have to increase the max speed. 
//     // Its set slow here to ensure we dont damage anything
//         pwr = 10;
//     }

//     // motor direction
//     int dir = 1;
//     if(u < 0){
//         dir = -1;
//     }
//     // signal the motor
//     setMotor(dir,pwr,PWM,DIR);
// }

// void PID_Current (int tar_current) {
//     // Call get current function in currentSensor class
//     float current = currentSensor.getCurrent(); 

//     // PID Constants
//     float kp = 1; 

//     // error 
//     int e = current - tar_Current; 
  
//     // Control Signal 
//     float u = kp*e; 

//     // Set motor power
//     // motor power
//     float pwr = fabs(u);
//     if(pwr > 255) {
//         pwr = 255;
//     }
//  }
