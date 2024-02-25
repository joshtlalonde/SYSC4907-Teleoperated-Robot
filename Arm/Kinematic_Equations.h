 #ifndef KINEMATIC_EQUATIONS_h
#define KINEMATIC_EQUATIONS_h

#include "Arduino.h"

#define ARM_LENGTH_A 90 // First Link Length (mm)
#define ARM_LENGTH_B 70 // Second Link Length (mm)
#define ARM_WIDTH 45 // Distance Between Shafts (mm)
        
#define INIT_THETA 1.0148 // Initial Theta Offset Value

/**
  These functions are used for handling different types of messages received from the MQTT Broker
*/

/**
  Converts the encoder value to an angle in radians

  int encoder: encoder value to convert
  returns:
    Angle in radians
*/
float encoderToAngle(int encoder);

/**
  Converts the angle in radians to an encoder value

  float angle: angle to convert
  returns:
    Encoder value
*/
int angleToEncoder(double angle);

/**
  Determines the position of the endpoint based on the current angles of the arm

  float theta1: Left shaft angle value of the arm
  float theta2: Right shaft angle value of the arm
  double &x: Return value of the x position after forward kinematics
  double &y: Return value of the y position after forward kinematics

  returns:
    0 if passed
    -1 if fail
*/
int forward_kinematics(double theta1, double theta2, double &x, double &y);

/**
  Determines the angles of the arm based on the current position of the endpoint

  float x: x positon value of the endpoint
  float y: y positon value of the endpoint
  float &theta1: Return left shaft angle value
  float &theta2: Return right shaft angle value of the arm with respect to the base of the arm

  returns:
    0 if passed
    -1 if fail
*/
int inverse_kinematics(double x, double y, double &theta1, double &theta2);

/**
  Calculates the Jacobian matrix for a robotic arm with two revolute joints based on given shaft angles.

  float theta1: Left angle value of the arm with respect to the base of the arm
  float theta2: Right angle value of the arm with respect to the base of the arm
  double current_y: Current y position of the endpoint
  double J[2][2]: Returned Jacobian Matrix

  returns:
    0 if passed
    -1 if fail
*/
int jacobian(double theta1, double theta2, double current_y, double J[2][2]);

/**
  Calculates the transposed Jacobian matrix for a two-joint robotic arm based on given joint angles and end effector position.

  float theta1: Left angle value of the arm with respect to the base of the arm
  float theta2: Right angle value of the arm with respect to the base of the arm
  double current_y: Current y position of the endpoint
  double J[2][2]: Returned Jacobian Matrix

  returns:
    0 if passed
    -1 if fail
*/
int transpose_jacobian(double theta1, double theta2, double current_y, double J_T[2][2]);

#endif