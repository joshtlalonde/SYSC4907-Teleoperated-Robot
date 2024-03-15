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

        double getX();
        double getY();
        double getTorqueL();
        double getTorqueR();

        /** 
         * Gets the (x, y) coordinates of the ARM for the 
         * virtual environment
         * 
         * int theta1: Left Encoder's Current Angle Value
         * int theta2: Right Encoder's Current Angle Value
         * double &x: Return value for X position
         * double &y: Return value for Y position
         */
        bool getPosition(double theta1, double theta2, double &x, double &y);

        /** 
         * Gets the joint torque matrix, utilizes Jacobian multiplied by force
         * 
         * int theta1: Left Encoder's Current Angle Value
         * int theta2: Right Encoder's Current Angle Value
         * float forceX: Force Value in X relayed from virtual env
         * float forceY: Force Value in Y relayed from virtual env
         * double &torqueL: Return value for torque on left motor
         * double &torqueR: Return value for torque on right motor
         */
        bool getTorque(double y, double theta1, double theta2, float forceX, float forceY, double &torqueL, double &torqueR);

        /** 
         * Calculates the Current to apply to the motors based on Torque values
         * 
         * int theta1: Left Encoder's Current Angle Value
         * int theta2: Right Encoder's Current Angle Value
         * float forceX: Force Value in X relayed from virtual env
         * float forceY: Force Value in Y relayed from virtual env
         * float &currentL: Return value for current on left motor
         * float &currentR: Return value for current on right motor
         */
        bool getArmatureCurrent(double y, double theta1, double theta2, float forceX, float forceY, float &currentL, float &currentR);
};

#endif