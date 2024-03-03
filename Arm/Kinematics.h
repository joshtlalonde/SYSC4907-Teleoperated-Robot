#ifndef KINEMATICS_h
#define KINEMATICS_h

#include "Kinematic_Equations.h"

class Kinematics {
    private:
        double x;
        double y;
        bool verbose;
    public:
        Kinematics(bool verbose);

        double getX();
        void setX(double x);
        double getY();
        void setY(double y);

        /** 
         * Updates the (x, y) coordinates of the ARM for the 
         * virtual environment
         * 
         * int encoderL: Left Encoder's Current Value
         * int encoderR: Right Encoder's Current Value
         */
        int updatePosition(int encoderL, int encoderR);

        /** TODO: Torque? */
        // updateTorqueCurrent() ...?
};

#endif