#ifndef KINEMATICS_h
#define KINEMATICS_h

#include "Kinematic_Equations.h"

#define KT 1.0/232.65; // Motor velocity constant

class Kinematics {
    private:
        double x;
        double y;
        double torqueL;
        double torqueR;
        bool verbose;
    public:
        Kinematics(bool verbose);

        /** 
         * Gets the (x, y) coordinates of the ARM for the 
         * virtual environment
         * 
         * int encoderL: Left Encoder's Value
         * int encoderR: Right Encoder's Value
         * double &x: Return value for X position
         * double &y: Return value for Y position
         */
        bool getPosition(int encoderL, int encoderR, double &x, double &y);

        /** 
         * Gets the joint torque matrix, utilizes Jacobian multiplied by force
         * 
         * int encoderL: Left Encoder's Current Value
         * int encoderR: Right Encoder's Current Value
         * float forceX: Force Value in X relayed from virtual env
         * float forceY: Force Value in Y relayed from virtual env
         * double &torqueL: Return value for torque on left motor
         * double &torqueR: Return value for torque on right motor
         */
        bool getTorque(int encoderL, int encoderR, float forceX, float forceY, double &torqueL, double &torqueR);

        /** 
         * Calculates the Current to apply to the motors based on Torque values
         * 
         * int encoderL: Left Encoder's Current Value
         * int encoderR: Right Encoder's Current Value
         * float forceX: Force Value in X relayed from virtual env
         * float forceY: Force Value in Y relayed from virtual env
         * float &currentL: Return value for current on left motor
         * float &currentR: Return value for current on right motor
         */
        bool getArmatureCurrent(int encoderL, int encoderR, float forceX, float forceY, float &currentL, float &currentR);
};

#endif