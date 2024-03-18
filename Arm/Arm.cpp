
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
  this->newEncoderTargetFlag = false;
  this->newCurrentTargetFlag = false;
  this->encoderPIDPrevT = 0;
  this->armatureCurrentPIDPrevT = 0;

  this->verbose = verbose;
}

char* Arm::getClientId() {
  return this->mqtt_client.getClientId();
}

bool Arm::getNewEncoderTargetFlag() {
  return this->newEncoderTargetFlag;
}

bool Arm::getNewCurrentTargetFlag() {
  return this->newCurrentTargetFlag;
}

/** MOTOR_CONTROLLER FUNCTIONALITY **/

void Arm::setEncoderTarget(int targetL, int targetR) {
  // Set the Target Flag
  this->newEncoderTargetFlag = true;

  this->motorL.setEncoderTarget(targetL);
  this->motorR.setEncoderTarget(targetR);
}

/** TODO: There will be two different types of this function: 
 *        1. Update target based on force values
 *        2. Update target based on current value from other arm
*/
void Arm::setCurrentTarget(float forceX, float forceY) {
  // Set the Target Flag
  this->newCurrentTargetFlag = true;

  // Get current Encoder count
  double theta1 = this->motorL.getEncoderTheta();
  double theta2 = this->motorR.getEncoderTheta();

  // Get current Position
  double x, y;
  this->kinematics.getPosition(theta1, theta2, x, y);

  // Serial.printf("\t theta1: %f, theta2: %f, x: %f, y: %f, forceX: %f, forceY: %f\n", theta1, theta2, x, y, forceX, forceY);
    
  // Get Current Sensor Values from Torque
  float currentL, currentR;
  if (this->kinematics.getArmatureCurrent(y, theta1, theta2, forceX, forceY, currentL, currentR) == false) {
      Serial.printf("<ARM>: Failed to Calculate Armature Torque Current with: theta1: %f, theta2: %f, forceX: %f, forceY: %f", 
                    theta1, theta2, forceX, forceY);
      return;
  }

  // Serial.printf("\t currentL: %f, currentR: %f\n", currentL, currentR);
  
  // Set Current Sensor target
  if (currentL != NULL || currentR != NULL) {
      this->motorL.setCurrentTarget(currentL);
      this->motorR.setCurrentTarget(currentR);
  } else {
    this->newCurrentTargetFlag = false;
  }
}

void Arm::dual_PID() {
  // Get current encoder value
  int64_t currentEncoderCount_L = this->motorL.getEncoderCount();
  int64_t currentEncoderCount_R = this->motorR.getEncoderCount();
  if (currentEncoderCount_L == NULL || currentEncoderCount_R == NULL) {
    Serial.printf("<ARM>: ERROR Current Encoder Count is NULL\n");
    return;
  }
  // Get Encoder Target
  int encoderTarget_L = this->motorL.getEncoderTarget(); 
  int encoderTarget_R = this->motorR.getEncoderTarget();
  if (encoderTarget_L == NULL || encoderTarget_R == NULL) {
    Serial.printf("<ARM>: ERROR Target Encoder Count is NULL\n");
    return;
  }
  // Get Previous Encoder Error
  int64_t prevEncoderError_L = this->motorL.getPrevEncoderError();
  int64_t prevEncoderError_R = this->motorR.getPrevEncoderError();
  if (prevEncoderError_L == NULL || prevEncoderError_R == NULL) {
    Serial.printf("<ARM>: ERROR Previous Encoder Count Error is NULL\n");
    return;
  }

  // time difference
  unsigned long currT = micros();
  float deltaT = ((float) (currT - this->encoderPIDPrevT)) / ( 1.0e6 );
  this->encoderPIDPrevT = currT;
  // Serial.printf("\t currT: %ld, deltaT: %f, prevT: %ld\n", currT, deltaT, prevT);  

  // error 
  int64_t e_L = currentEncoderCount_L - encoderTarget_L;
  int64_t e_R = currentEncoderCount_R - encoderTarget_R; 

  // derivative
  float dedt_L = (e_L - prevEncoderError_L) / (deltaT);
  float dedt_R = (e_R - prevEncoderError_R) / (deltaT);

  // Control Signal 
  float u_L = ENCODER_PID_KP*e_L + ENCODER_PID_KD*dedt_L;
  float u_R = ENCODER_PID_KP*e_R + ENCODER_PID_KD*dedt_R; 

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
  // Serial.printf("Time (ms), EncoderCount_L, EncoderCount_R, Target_L, Target_R, PWM_L, PWM_R, u_L, u_R, e_L, e_R\n");
  // double currentEncoderCount_mag = sqrt(pow(currentEncoderCount_L, 2) + pow(currentEncoderCount_R, 2));
  // double encoderTarget_mag = sqrt(pow(encoderTarget_L, 2) + pow(encoderTarget_R, 2));
  // double x, y;
  // this->kinematics.getPosition(this->motorL.getEncoderTheta(), this->motorR.getEncoderTheta(), x, y);

  // double position_mag = sqrt(pow(x, 2) + pow(y, 2));

  // Serial.printf("%ld, %f\n",//, %f, %f, %f, %f, %lld, %lld\n", 
  //               millis(), position_mag); 

  // Serial.printf("%ld, %f, %f, %lld, %lld, %d, %d, %f, %f\n",//, %f, %f, %f, %f, %lld, %lld\n", 
  //               millis(), currentEncoderCount_mag, encoderTarget_mag, currentEncoderCount_L, currentEncoderCount_R, 
  //               encoderTarget_L, encoderTarget_R, pwr_L, pwr_R);//, 
                //pwr_L, pwr_R, u_L, u_R, e_L, e_R);

  // Serial.printf("%ld, %lld, %lld, %d, %d\n",//, %f, %f, %f, %f, %lld, %lld\n", 
  //               millis(), currentEncoderCount_L, currentEncoderCount_R, 
  //               encoderTarget_L, encoderTarget_R);//, 
  //               //pwr_L, pwr_R, u_L, u_R, e_L, e_R);

  if ((e_L < ENCODER_TARGET_OFFSET && e_L > -ENCODER_TARGET_OFFSET) && 
      (e_R < ENCODER_TARGET_OFFSET && e_R > -ENCODER_TARGET_OFFSET)) { // Accounts for rounding errors
    // Shutoff motors
    this->motorL.setMotor(dir_L, 0, this->kinematics.getY());
    this->motorR.setMotor(dir_R, 0, this->kinematics.getY());
    // Reset Flag
    this->newEncoderTargetFlag = false;
  } else {
    if ((e_L > CUTOFF_ENCODER_ERROR || e_L < -CUTOFF_ENCODER_ERROR) || 
        (e_R > CUTOFF_ENCODER_ERROR || e_R < -CUTOFF_ENCODER_ERROR)) {
      Serial.printf("<ARM>: OUTSIDE OF CUTOFF ERROR, MOTORS SHUTTING DOWN!!!\n");
      // Shutoff motors
      this->motorL.setMotor(dir_L, 0, this->kinematics.getY());
      this->motorR.setMotor(dir_R, 0, this->kinematics.getY());
    } else {
      // Set Motors
      this->motorL.setMotor(dir_L, pwr_L, this->kinematics.getY());
      this->motorR.setMotor(dir_R, pwr_R, this->kinematics.getY());
    }
  }

  // Set Previous Error
  this->motorL.setPrevEncoderError(e_L); 
  this->motorR.setPrevEncoderError(e_R);
}

void Arm::dual_Current_PID() {
  // Get current Armature Current value
  float currentArmatureCurrent_L = this->motorL.getCurrentAmps();
  float currentArmatureCurrent_R = this->motorR.getCurrentAmps();
  if (currentArmatureCurrent_L == NULL || currentArmatureCurrent_R == NULL) {
    Serial.printf("<ARM>: ERROR Current Armature Current is NULL\n");
    return;
  }
  // Get Armature Current Target
  float armatureCurrentTarget_L = this->motorL.getCurrentTarget(); 
  float armatureCurrentTarget_R = this->motorR.getCurrentTarget();
  if (armatureCurrentTarget_L == NULL || armatureCurrentTarget_R == NULL) {
    Serial.printf("<ARM>: ERROR Target Armature Current is NULL\n");
    return;
  }
  // Get Previous Armature Current Error
  float prevArmatureCurrentError_L = this->motorL.getPrevArmatureCurrentError();
  float prevArmatureCurrentError_R = this->motorR.getPrevArmatureCurrentError();
  if (prevArmatureCurrentError_L == NULL || prevArmatureCurrentError_R == NULL) {
    Serial.printf("<ARM>: ERROR Previous Armature Current Error is NULL\n");
    return;
  }

  // Time difference calculation
  unsigned long currT = micros();
  float deltaT = ((float) (currT - this->armatureCurrentPIDPrevT)) / ( 1.0e6 );
  this->armatureCurrentPIDPrevT = currT;

  // error 
  float e_L = currentArmatureCurrent_L - armatureCurrentTarget_L;
  float e_R = currentArmatureCurrent_R - armatureCurrentTarget_R; 

  // derivative
  float dedt_L = (e_L - prevArmatureCurrentError_L) / (deltaT);
  float dedt_R = (e_R - prevArmatureCurrentError_R) / (deltaT);

  // Control Signal 
  float u_L = ARMATURE_CURRENT_PID_KP*e_L + ARMATURE_CURRENT_PID_KD*dedt_L;
  float u_R = ARMATURE_CURRENT_PID_KP*e_R + ARMATURE_CURRENT_PID_KD*dedt_R; 

  // Set Left motor power
  float pwr_L = fabs(u_L);
  if (pwr_L > MAX_PWM) {
    pwr_L = MAX_PWM;
  }
  else if (pwr_L < MIN_PWM && (e_L > 10 || e_L < -10)) {
    pwr_L = MIN_PWM;
  }

  // Set Right motor power
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
  // if (u_L > 0) {
  //   u_L = u_L * 0.05; // Decrease by 5%
  // }

  // Right motor direction
  int dir_R = -1;
  if(u_R < 0){
    dir_R = 1;
  }
  // if (u_R > 0) {
  //   u_R = u_R * 0.05; // Decrease by 5%
  // }

  // double currentArmatureCurrent_mag = sqrt(pow(currentArmatureCurrent_L, 2) + pow(currentArmatureCurrent_R, 2));
  // double armatureCurrentTarget_mag = sqrt(pow(armatureCurrentTarget_L, 2) + pow(armatureCurrentTarget_R, 2));

  // Serial.printf("%ld, %f, %f, %f, %f, %f, %f\n",//, %f, %f, %f, %f, %f, %f\n", 
  //                 millis(), currentArmatureCurrent_mag, armatureCurrentTarget_mag,
  //                 currentArmatureCurrent_L, currentArmatureCurrent_R, armatureCurrentTarget_L, armatureCurrentTarget_R); 

  // Serial.printf("\t pwr: %f, dir: %d\n", pwr, dir); 
  // Serial.printf("Time (ms), current_L, current_R, Target_L, Target_R, PWM_L, PWM_R, u_L, u_R, e_L, e_R\n");
  // Serial.printf("%ld, %f, %f, %f, %f\n",//, %f, %f, %f, %f, %f, %f\n", 
  //               millis(), currentArmatureCurrent_L, currentArmatureCurrent_R, 
  //               armatureCurrentTarget_L, armatureCurrentTarget_R);//, 
  //               //pwr_L, pwr_R, u_L, u_R, e_L, e_R);

  if ((armatureCurrentTarget_L == ARMATURE_CURRENT_TARGET_OFFSET) && 
      (armatureCurrentTarget_R == ARMATURE_CURRENT_TARGET_OFFSET)) {
    // Shutoff motors
    this->motorL.setMotor(dir_L, 0, this->kinematics.getY());
    this->motorR.setMotor(dir_R, 0, this->kinematics.getY());
    // Reset Flag
    this->newCurrentTargetFlag = false;
  } else {
    if ((e_L > CUTOFF_ARMARTURE_CURRENT_ERROR || e_L < -CUTOFF_ARMARTURE_CURRENT_ERROR) || 
        (e_R > CUTOFF_ARMARTURE_CURRENT_ERROR || e_R < -CUTOFF_ARMARTURE_CURRENT_ERROR)) {
      Serial.printf("<ARM>: OUTSIDE OF CUTOFF ERROR, MOTORS SHUTTING DOWN!!!\n");
      // Shutoff motors
      this->motorL.setMotor(dir_L, 0, this->kinematics.getY());
      this->motorR.setMotor(dir_R, 0, this->kinematics.getY());
    } else {
      // Set Motors
      this->motorL.setMotor(dir_L, pwr_L, this->kinematics.getY());
      this->motorR.setMotor(dir_R, pwr_R, this->kinematics.getY());
    }
  }

  // Set Previous Error
  this->motorL.setPrevArmatureCurrentError(e_L); 
  this->motorR.setPrevArmatureCurrentError(e_R);
}

/************************************/

/******** MQTT FUNCTIONALITY ********/

void Arm::mqtt_setup(char* ssid, char* password, 
                     char* mosqutto_ip, int mosquitto_port) 
{
  int attempt_reconnect = 0;
  bool ret = false;

  // Setup the broker
  while (this->mqtt_client.begin(ssid, password) == false) { 
    attempt_reconnect++;
    Serial.printf("<ARM>: Failed to initialize mqttClient\n");
    Serial.printf("\t Attempting reconnect %d\n", attempt_reconnect);
    delay(1000);
  }

  delay(2000);

  // Connect to Broker
  if (!this->mqtt_client.connect(mosqutto_ip, mosquitto_port)) {
    Serial.println("<ARM>: Failed to Connect to Broker, Exiting...");
    // while(1); // TODO: RESTART???
  }

  // Start MQTT
  this->mqtt_client.start();
  
  // Adding Callback Functions
  this->mqtt_client.add_message_callback(MQTT_EVENT_CONNECTED, on_connect, (void*) this, "on_connect");
  this->mqtt_client.add_message_callback(MQTT_EVENT_DISCONNECTED, on_disconnect, (void*) this, "on_disconnect");
  this->mqtt_client.add_message_callback(MQTT_EVENT_PUBLISHED, on_publish, (void*) this, "on_publish");
  this->mqtt_client.add_message_callback(MQTT_EVENT_SUBSCRIBED, on_subscribe, (void*) this, "on_subscribe");
  // Attach Callback for Data messages
  this->mqtt_client.add_message_callback(MQTT_EVENT_DATA, on_data, (void*) this, "on_data");

  // Subscribe to Encoder Topic
  this->mqtt_client.subscribe("encoder/#", 1);
  // Subscribe to Current Topic
  // this->mqtt_client.subscribe("current/#", 1); // FIXME: The arm does not need current data?
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

bool Arm::publish_current() {
  char topic[250], current_val_str[250];
  this->current_jsonify(current_val_str);

  sprintf(topic, "current/%s", this->mqtt_client.getClientId());
  if (this->mqtt_client.publish(topic, current_val_str, 0, 0, 0) < 0) 
    return false;
  
  return true;
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