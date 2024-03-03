
#include "Arm.h"

Arm::Arm(MQTTClient& mqtt_client, Motor_Control& motorL, 
         Motor_Control& motorR, Kinematics& kinematics, bool verbose) : mqtt_client(mqtt_client), 
         motorL(motorL), motorR(motorR), kinematics(kinematics)
{
  this->mqtt_client = mqtt_client;
  this->motorL = motorL;
  this->motorR = motorR;
  this->kinematics = kinematics;

  /* PID Setup */
  this->newTargetFlag = false;
  this->PIDPrevT = 0;

  this->verbose = verbose;
}

char* Arm::getClientId() {
  return this->mqtt_client.getClientId();
}

int Arm::getNewTargetFlag() {
  return this->newTargetFlag;
}

/** MOTOR_CONTROLLER FUNCTIONALITY **/

void Arm::setEncoderTarget(int targetL, int targetR) {
  // Set the Target Flag
  this->newTargetFlag = true;

  this->motorL.setEncoderTarget(targetL);
  this->motorR.setEncoderTarget(targetR);
}

/** TODO: There will be two different types of this function: 
 *        1. Update target based on force values
 *        2. Update target based on current value from other arm
*/
void Arm::setCurrentTarget(float forceX, float forceY) {
  // Set the Target Flag
  // this->newTargetFlag = true;

  int encoderL = this->motorL.getEncoderCount();
  int encoderR = this->motorR.getEncoderCount();
    
  // Get Current Sensor Values from Torque
  float currentL, currentR;
  if (this->kinematics.getArmatureCurrent(encoderL, encoderR, forceX, forceY, currentL, currentR)) {
    if (this->verbose)
        Serial.printf("<ARM>: Failed to Calculate Armature Torque Current with: encoderL: %d, encoderR: %d, forceX: %f, forceY: %f", encoderL, encoderR, forceX, forceY);
        return;
  }
  
  // Set Current Sensor target
  this->motorL.setCurrentTarget(currentL);
  this->motorR.setCurrentTarget(currentR);
}

void Arm::dual_PID() {
  // unsigned long currTime = millis();
  // if (currTime - prevTime >= delayTime) {
  //   this->PIDPrevT = currTime;
  // }

  // Serial.printf("<ARM>: PID Target {Left: %d, Right: %d}\n", 
  //               this->motorL.getTarget(), this->motorR.getTarget());

  // Get current encoder value
  int64_t currentLeftEncoderCount = this->motorL.getEncoderCount();
  int64_t currentRightEncoderCount = this->motorR.getEncoderCount();
  

  // Serial.printf("currentLeftEncoderCount: %lld, target: %lld, kp: %f, kd: %f\n", currentLeftEncoderCount, target, kp, kd);
  
  // time difference
  unsigned long currT = micros();
  float deltaT = ((float) (currT - this->PIDPrevT)) / ( 1.0e6 );
  this->PIDPrevT = currT;
  // Serial.printf("\t currT: %ld, deltaT: %f, prevT: %ld\n", currT, deltaT, prevT);  

  // error 
  int64_t e_L = currentLeftEncoderCount - this->motorL.getEncoderTarget(); 
  int64_t e_R = currentRightEncoderCount - this->motorR.getEncoderTarget(); 

  // derivative
  float dedt_L = (e_L - this->motorL.getPrevEncoderError()) / (deltaT);
  float dedt_R = (e_R - this->motorR.getPrevEncoderError()) / (deltaT);

  // Previous Error
  this->motorL.setPrevEncoderError(e_L); 
  this->motorR.setPrevEncoderError(e_R);

  // Control Signal 
  float u_L = PID_KP*e_L + PID_KD*dedt_L;
  float u_R = PID_KP*e_R + PID_KD*dedt_R; 

  //Set Left motor power
  float pwr_L = fabs(u_L);
  if (pwr_L > MAX_PWM) {
    pwr_L = MAX_PWM;
  }
  else if (pwr_L < MIN_PWM && (e_L > 10 || e_L < -10)) {
    pwr_L = MIN_PWM;
  }

  //Set Right motor power
  float pwr_R = fabs(u_R);
  if (pwr_R > MAX_PWM) {
    pwr_R = MAX_PWM;
  }
  else if (pwr_R < MIN_PWM && (e_R > 10 || e_R < -10)) {
    pwr_R = MIN_PWM;
  }

  // Left motor direction
  int dir_L = -1;
  if(u_L < 0){
    dir_L = 1;
  }

  //Right motor direction
  int dir_R = -1;
  if(u_R < 0){
    dir_R = 1;
  }

  // Serial.printf("\t pwr: %f, dir: %d\n", pwr, dir); 
  Serial.printf("Time (ms), EncoderCount_L, EncoderCount_R, Target_L, Target_R, PWM_L, PWM_R, u_L, u_R, e_L, e_R\n");
  Serial.printf("%ld, %lld, %lld, %d, %d, %f, %f, %f, %f, %lld, %lld\n", 
                millis(), currentLeftEncoderCount, currentRightEncoderCount, 
                this->motorL.getEncoderTarget(), this->motorR.getEncoderTarget(), 
                pwr_L, pwr_R, u_L, u_R, e_L, e_R);

  if ((e_L < TARGET_OFFSET && e_L > -TARGET_OFFSET) && (e_R < TARGET_OFFSET && e_R > -TARGET_OFFSET)) { // Accounts for rounding errors
    this->motorL.setMotor(dir_L, 0, this->kinematics.getX(), this->kinematics.getY());
    this->motorR.setMotor(dir_R, 0, this->kinematics.getX(), this->kinematics.getY());
    this->newTargetFlag = false;
  } else {
      // Set Left Motor Speed/Direction
    this->motorL.setMotor(dir_L, pwr_L, this->kinematics.getX(), this->kinematics.getY());
    // Set Left Motor Speed/Direction
    this->motorR.setMotor(dir_R, pwr_R, this->kinematics.getX(), this->kinematics.getY());
  }
}

void Arm::dual_Current_PID() {
  Serial.println("TODO: Setup the PID for current updates???");
}

/************************************/

/******** MQTT FUNCTIONALITY ********/

void Arm::mqtt_setup(char* ssid, char* password, 
                     char* mosqutto_ip, int mosquitto_port) 
{
  // Setup the broker
  if (!this->mqtt_client.begin(ssid, password)) {
    Serial.printf("<ARM>: Failed to initialize mqttClient\n");
    // while(1); // TODO: RESTART?? Should go to clean exit or clean restart?
  }

  delay(2000);

  // Connect to Broker
  if (!this->mqtt_client.connect(mosqutto_ip, mosquitto_port)) {
    Serial.println("<ARM>: Failed to Connect to Broker, Exiting...");
    // while(1); // TODO: RESTART???
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
  // Subscribe to Position Topic
  // this->mqtt_client.subscribe("position/#", 1); // FIXME: The arm does not need position data?
  // Subscribe to Force Topic
  this->mqtt_client.subscribe("force/#", 1);
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

bool Arm::publish_encoder() {
  char topic[250], encoder_val_str[250];
  this->encoder_jsonify(encoder_val_str);

  sprintf(topic, "encoder/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, encoder_val_str, 0, 0, 0) < 0) 
    return false;
  
  return true;
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

void Arm::position_jsonify(char* position_val_str) {
  if (position_val_str == NULL) {
    Serial.println("<ARM> Current_val_str must not be NULL");
  }

  // Save Previous Position
  double prevX = this->kinematics.getX();
  double prevY = this->kinematics.getY();
  // Update Position using forward_kinematics
  double x, y;
  this->kinematics.getPosition(this->motorL.getEncoderTheta(), this->motorR.getEncoderTheta(), x, y);

  DynamicJsonDocument json(1024);
  json["x"] = prevX - x; /** QUESTION: Should this be prev-final or final-prev ??? */
  json["y"] = prevY - y;

  serializeJson(json, position_val_str, 50);
}

bool Arm::publish_position() {
  char topic[250], position_val_str[250];
  this->position_jsonify(position_val_str);

  sprintf(topic, "position/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, position_val_str, 0, 0, 0) < 0) 
    return false;
  
  return true;
}


/************************************/