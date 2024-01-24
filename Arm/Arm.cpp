
#include "Arm.h"
#include "ArduinoJson.h"
#include "MQTTClient.h"
#include "MQTTClient_Callbacks.h"

Arm::Arm(MQTTClient& mqtt_client, Motor_Control& motorL, 
         Motor_Control& motorR) : mqtt_client(mqtt_client), 
         motorL(motorL), motorR(motorR)
{
  this->mqtt_client = mqtt_client;
  this->motorL = motorL;
  this->motorR = motorR;

  this->newTargetFlag = false;
}

char* Arm::getClientId() {
  return this->mqtt_client.getClientId();
}

int Arm::getNewTargetFlag() {
  return this->newTargetFlag;
}

/** MOTOR_CONTROLLER FUNCTIONALITY **/

/** QUESTIONS:
 *    It may be better to have the while loop here.
 *    Instead of having the main loop control the loop.
 *    The reason is because it causes more "branches/jumps"
 *    in memory, which causes slower and less efficient code.
 *    Defining everything ones and staying within the function 
 *    once means less stack and heap movement.
 *    I am building it with the while loop, the only real advantage to 
 *    the other method is that it looks pretty...?
 *    
*/
void Arm::updateMotorPosition() {
  // if (this->verbose)
  Serial.printf("<ARM>: Updating Motor Position, Target_L: %d, Target_R: %d\n", 
                this->motorL.getEncoderTarget(), this->motorR.getEncoderTarget());

  /** Initialize Default values */
  // Encoder
  int currEncoderCount_L = 0;
  int currEncoderCount_R = 0;
  // Time
  long currTime = 0;
  long deltaT = 0;
  long prevTime = 0;
  // Error
  int err_L = 0;
  int err_R = 0;
  long dedt_L = 0;
  long dedt_R = 0;
  int prevErr_L = 0;
  int prevErr_R = 0;
  // Control Signal
  long u_L = 0;
  long u_R = 0;
  // Motor
  float motorPWM_L = 0;
  float motorPWM_R = 0;
  int motorDir_L = 0;
  int motorDir_R = 0;


  /** FIX: Assuming that CW counts up and CCW counts down.
   *      Should flip wires if this is not the case?
   * 
   * Description of the while loop statement:
   *  If the motor currentDirection is ClockWise AND the value of the EncoderCount is < the target ... keep going,
   *    OR
   *  If the motor currentDirection is CunterClockWise AND the value of the EncoderCount is > the target ... keep going,
   * 
   * The above describes one motor instance. On the left of the while loop you can see another AND (&&), this is due to both 
   * motor's needing to go through the PID at the same time. 
   * This is saying that both motors need to meet the requirements above before exiting the while loop.
   * 
   * QUESTION: Does that mean we will need to turn off one of the motors once they reach their target?
   *           Or would it be fine since the PID will just set the power to more or less 0?
   *           There could be the issue of fidgetting like we see often if not turning it off though...
   *           Until testing we will leave it without the extra check at the bottom
   * 
   * ISSUE: How do we get the current direction at all time, it cannot only be set when changing the motor speed (as it is now)
   *        We would need to come up with a way to constantly check the direction the motor is moving.
   *        This could be done by simply polling the encoder (in main) and seeing if the value has increased or decreased.
   */

  /** FIX: This is for debugging */
  this->motorL.setMotor(MOTOR_DIR_CW, 0); 
  this->motorR.setMotor(MOTOR_DIR_CCW, 0); 

  Serial.printf("this->motorL.getCurrentDir(): %d, this->motorL.getEncoderCount(): %d, this->motorL.getEncoderTarget(): %d, this->motorR.getCurrentDir(): %d, this->motorR.getEncoderCount(): %d, this->motorR.getEncoderTarget(): %d\n",
                 this->motorL.getCurrentDir(), this->motorL.getEncoderCount(), this->motorL.getEncoderTarget(),
                 this->motorR.getCurrentDir(), this->motorR.getEncoderCount(), this->motorR.getEncoderTarget());

  while (((this->motorL.getCurrentDir() == MOTOR_DIR_CW && this->motorL.getEncoderCount() < this->motorL.getEncoderTarget()) || 
         (this->motorL.getCurrentDir() == MOTOR_DIR_CCW && this->motorL.getEncoderCount() > this->motorL.getEncoderTarget())) &&
         ((this->motorR.getCurrentDir() == MOTOR_DIR_CW && this->motorR.getEncoderCount() < this->motorR.getEncoderTarget()) || 
         (this->motorR.getCurrentDir() == MOTOR_DIR_CCW && this->motorR.getEncoderCount() > this->motorR.getEncoderTarget())))
  {
    /** TODO: Check if the encoder count is valid
    * if (currentEncoderCount "< or >" max/min value) {}
    * 
    * Do this here or in motor controller?
    */
    // Get Current Encoder Count 
    currEncoderCount_L = this->motorL.getEncoderCount();
    currEncoderCount_R = this->motorR.getEncoderCount();

    /** Update Time */
    currTime = micros();
    // Get change in time
    deltaT = ((long) (currTime - prevTime)) / (long)( 1.0e6 ); 
    // Set Previous Time to Current Time
    prevTime = currTime;

    /** Update Error */
    err_L = currEncoderCount_L - this->motorL.getEncoderTarget(); 
    err_R = currEncoderCount_R - this->motorR.getEncoderTarget(); 
    // Calculate Derivative
    dedt_L = ((long) (abs(err_L) - abs(prevErr_L))) / (long)(deltaT); /** QUESTION: Is using abs() here a good idea??
                                                                        * This might be fucking with the u (control signal) */
    dedt_R = ((long) (abs(err_R) - abs(prevErr_R))) / (long)(deltaT);
    // Set Previous Error to Current Error
    prevErr_L = err_L;
    prevErr_R = err_R;

    /** Update Control Signal */
    u_L = (PID_KP * err_L) + (PID_KD * dedt_L);
    u_R = (PID_KP * err_R) + (PID_KD * dedt_R);

    /** Update Motor Speed */
    float motorPWM_L = fabs(u_L); /** TODO: Determine a way to make the u value more proportional to our range of 0 - 255 
                                   * If this method doesn't work let's change the precision of the getCount to divide by 4.
                                   */
    float motorPWM_R = fabs(u_R);
    // Check MAX Speed limit reach
    if (motorPWM_L > 10)
      motorPWM_L = 10; /** NOTE: We may have to increase the max speed. Its set slow here to ensure we dont damage anything */  
    if (motorPWM_R > 10)
      motorPWM_R = 10;
    // Set Motor Direction
    if (u_L > 0)
      motorDir_L = 1;
    else
      motorDir_L = -1;
    if (u_R > 0)
      motorDir_R = 1;
    else
      motorDir_R = -1;
    // Update Motor Speed and Direction
    this->motorL.setMotor(motorDir_L, motorPWM_L);
    this->motorR.setMotor(motorDir_R, motorPWM_R);
  }

  // Reset the newTargetFlag
  this->newTargetFlag = false;

  Serial.printf("<ARM>: Motor Position Updated, resetting newTargetFlag\n");
}

void Arm::setEncoderTarget(int targetL, int targetR) {
  // Set the Target Flag
  this->newTargetFlag = true;

  this->motorL.setEncoderTarget(targetL);
  this->motorR.setEncoderTarget(targetR);
}

/************************************/

/******** MQTT FUNCTIONALITY ********/

void Arm::mqtt_setup(char* ssid, char* password, 
                     char* mosqutto_ip, int mosquitto_port) 
{
  // Setup the broker
  if (!this->mqtt_client.begin(ssid, password)) {
    Serial.printf("<ARM>: Failed to initialize mqttClient\n");
    while(1); // TODO: RESTART?? Should go to clean exit or clean restart?
  }

  delay(2000);

  // Connect to Broker
  if (!this->mqtt_client.connect(mosqutto_ip, mosquitto_port)) {
    Serial.println("<ARM>: Failed to Connect to Broker, Exiting...");
    while(1); // TODO: RESTART???
  }

  // Start MQTT
  this->mqtt_client.start();

  // Attach Callback for Data messages
  /** FIX: Cannot use the on_data event because it will be called for every send it makes
   *       this will cause the esp to crash
   *       Is there really a way around this though?
   */
  this->mqtt_client.add_message_callback(MQTT_EVENT_DATA, on_data, (void*) this, "on_data");

  // Subscribe to Encoder Topic
  this->mqtt_client.subscribe("encoder/#", 1);
  // Subscribe to Current Topic
  this->mqtt_client.subscribe("current/#", 1);
}

void Arm::encoder_jsonify(char* encoder_val_str) {
  if (encoder_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  DynamicJsonDocument json(1024);
  json["left"] = this->motorL.getEncoderCount();
  json["right"] = this->motorR.getEncoderCount();

  serializeJson(json, encoder_val_str, 50);
}

void Arm::current_jsonify(char* current_val_str) {
  if (current_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  DynamicJsonDocument json(1024);
  json["left"] = this->motorL.getCurrentAmps();
  json["right"] = this->motorR.getCurrentAmps();

  serializeJson(json, current_val_str, 50);
}

bool Arm::publish_encoder() {
  char topic[250], encoder_val_str[250];
  this->encoder_jsonify(encoder_val_str);

  sprintf(topic, "encoder/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, encoder_val_str, 0, 1, 0) < 0) 
    return false;
  
  return true;
}

bool Arm::publish_current() {
  char topic[250], current_val_str[250];
  this->current_jsonify(current_val_str);

  sprintf(topic, "current/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, current_val_str, 0, 1, 0) < 0)
    return false;
  
  return true;
}

/************************************/