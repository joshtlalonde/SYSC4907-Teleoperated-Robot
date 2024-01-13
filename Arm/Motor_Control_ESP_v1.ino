#include <QuadratureEncoder.h>
// must also have enableInterrupt.h library


// define the pins for the position encoders 
#define ENCA_L 2 
#define ENCB_L 3 
#define ENCA_R 4
#define ENCB_R 5

// Define pins for PWM and DIR of Each Motor 
#define PWM_L 6
#define DIR_L 7
#define PWM_R 8
#define DIR_R 9

// Setting PWM properties
int pwmChannel1 = 0; // Selects channel 0
int pwmChannel2 = 0; // Selects channel 0
int frequence = 1000; // PWM frequency of 1 KHz
int resolution = 8; // 8-bit resolution, 256 possible values
int pwmPin1 = 6; // Sets PWM pin
int pwmPin2 = 8; // Sets PWM pin

//Setup Pins 
void setup() {
  Serial.begin(9600);
  pinMode(ENCA_L,INPUT);
  pinMode(ENCB_L,INPUT);
  pinMode(ENCA_R,INPUT);
  pinMode(ENCB_R,INPUT);

  pinMode(curL, INPUT); 
  pinMode(curR, INPUT);

  pinMode(PWM_L,OUTPUT);
  pinMode(DIR_L,OUTPUT);
  pinMode(PWM_R,OUTPUT);
  pinMode(DIR_R,OUTPUT);\

  //Configure PWM1

  // Configuration of channel 0 with the chosen frequency and resolution
    ledcSetup(pwmChannel1, frequence, resolution);

    // Assigns the PWM channel to pin 6
    ledcAttachPin(pwmPin1, pwmChannel1);

    // Create the selected output voltage
    ledcWrite(pwmChannel1, 127); // 1.65 V

  //Configure PWM2

    // Configuration of channel 0 with the chosen frequency and resolution
    ledcSetup(pwmChannel2, frequence, resolution);

    // Assigns the PWM channel to pin 8
    ledcAttachPin(pwmPin2, pwmChannel2);

    // Create the selected output voltage
    ledcWrite(pwmChannel2, 127); // 1.65 V


}

// Create Two Encoder Objects (Left and Right)
Encoders leftEncoder(2,3); 
Encoders rightEncoder(4,5); 

//Create a Motor Control Class
class Motor_Control {
  public: 
  //int target; Don't Need
  //float tar_current; Don't Need
  int pwm; 
  int dir; 
  Encoders* Encoder; //Is this ok? 
  currentSensor* currentSensor; 
  void PID_Encoder(int target);
  void PID_Current(float tar_current);
  void setMotor(int dir, int pwmVal, int pwm, int dir1); 
}

void setMotor(
int dir, int pwmVal, int pwm, int dir1){
  analogWrite(pwm,pwmVal); 
  if(dir == 1){   //Spins in CW direction
    digitalWrite(dir1, LOW); 
  }
  else if(dir == -1){   //Spins in CCW direction
digitalWrite(dir1, HIGH);
  }
  else{
    analogWrite(PWM, 0); //Do not spin motor
  }  
}

void PID_Encoder(int target){
  //Get current Encoder Count 
  currentLeftEncoderCount = Encoder.getEncoderCount();
  // time difference
    long currT = micros();
    float deltaT = ((float) (currT - prevT))/( 1.0e6 );
    prevT = currT;

    //PID Constants
    float kp = 1; 
    float kd = 0.75;

    // error 
    int e = currentLeftEncoderCount - target; 

    // derivative
    float dedt = (e-eprev)/(deltaT);

    // Control Signal 
    float u = kp*e + kd*dedt;

    //Set motor power
     // motor power
    float pwr = fabs(u);
    if( pwr > 10 ){ //Note: We may have to increase the max speed. Its set slow here to ensure we dont damage anything
      pwr = 10;
    }

    // motor direction
    int dir = 1;
    if(u<0){
      dir = -1;
    }
    // signal the motor
    setMotor(dir,pwr,PWM,DIR);
 }
 void PID_Current (int tar_current){
  
  // Call get current function in currentSensor class
  float current = currentSensor.getCurrent(); 

  //PID Constants
    float kp = 1; 

  // error 
    int e = current - tar_Current; 
  
  // Control Signal 
    float u = kp*e; 

  //Set motor power
     // motor power
    float pwr = fabs(u);
    if( pwr > 255 ){
      pwr = 255;
    }
 }

Motor_Control leftMotor;
leftMotor.Encoder = leftEncoder(2,3); 
leftMotor.target = 0; 
leftMotor.pwm = 0; 
leftMotor.dir = 0; 

Motor_Control rightMotor;
rightMotor.Encoder = rightEncoder(4,5); 
rightMotor.target = 0; 
rightMotor.pwm = 0; 
rightMotor.dir = 0; 


void loop() {
  leftMotor.PID_Encoder(leftMotor.target);
  rightMotor.PID_Encoder(rightMotor.target);


}


