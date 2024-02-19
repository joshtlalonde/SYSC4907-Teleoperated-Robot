#ifndef KINEMATICS_h
#define KINEMATICS_h

#include "Kinematic_Equations.h"

#define TORQUE_CONSTANT 1.0 //TODO: To be defined... This is the constant value that converts torque to current?

class Kinematics {
    private:
        double x;
        double y;
        float force; // Currently this is unused, but kept updated through "updateTorque()"
        double torque[2][2];
        bool verbose;
    public:
        Kinematics(bool verbose);

        double getX();
        double getY();
        void getTorque(double torque[2][2]);

        /** 
         * Updates the (x, y) coordinates of the ARM for the 
         * virtual environment
         * 
         * int encoderL: Left Encoder's Current Value
         * int encoderR: Right Encoder's Current Value
         */
        int updatePosition(int encoderL, int encoderR);

        /** 
         * Updates the joint torque matrix, utilizes Jacobian multiplied by force
         * 
         * int encoderL: Left Encoder's Current Value
         * int encoderR: Right Encoder's Current Value
         * float force: Force Magnitude relayed from virtual env to apply as torque
         */
        int updateTorque(int encoderL, int encoderR, float force);

        /** 
         * Calculates the Current to apply to the motors based on current Torque values
         * 
         * float &currentL: Return value for the current to the left motor
         * float &currentR: Return value for the current to the right motor
         */
        int torqueCurrent(float &currentL, float &currentR);
};

#endif