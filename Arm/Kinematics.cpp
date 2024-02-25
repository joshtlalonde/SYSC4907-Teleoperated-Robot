
#include "Kinematics.h"

Kinematics::Kinematics(bool verbose)
{
    /* Position Setup */
    // Inital postion is {x: 22.5, y: 76.32}
    this->x = 22.5; // Are these even needed????
    this->y = 76.32;

    /* Torque Setup */
    this->torqueL = 0;
    this->torqueR = 0;

    this->verbose = verbose;
}

bool Kinematics::getPosition(int encoderL, int encoderR, double &x, double &y) {
    double prevX = this->x;
    double prevY = this->y;

    // Get Angles (rad) from encoder values
    double theta1 = encoderToAngle(encoderL);
    double theta2 = encoderToAngle(encoderR);

    // Get Position from forward_kinematics
    if (forward_kinematics(theta1, theta2, x, y) != 0) {
        // Reset to previous position
        this->x = prevX;
        this->y = prevY;
        x = prevX;
        y = prevY;

        if (this->verbose)
            Serial.printf("<Kinematics>: No solution found encoder_L: %lf and encoder_R: %lf \n\t Previous Position: (%d, %d)", theta1, theta2, prevX, prevY);

        return false;
    }

    // Update x and y in class
    this->x = x;
    this->y = y;

    return true;
}

/** TODO: Update to Use Fx and Fy */
bool Kinematics::getTorque(int encoderL, int encoderR, float forceX, float forceY, double &torqueL, double &torqueR) {
    double J_T[2][2];

    // Get Angles (rad) from encoder values
    double theta1 = encoderToAngle(encoderL);
    double theta2 = encoderToAngle(encoderR);

    if (transpose_jacobian(theta1, theta2, this->y, J_T)) {
        if (this->verbose)
            Serial.printf("<Kinematics>: Failed to Calculate Torque Joints with: theta1: %lf, theta2: %lf, current_y: %lf", theta1, theta2, this->y);
        return false;
    }

    this->torqueL = (J_T[0][0] * forceX) + (J_T[0][1] * forceY); // should be Fx/Fy
    this->torqueR = (J_T[1][0] * forceX) + (J_T[1][1] * forceY); // should be Fx/Fy

    return true;
}

/** TODO: Unsure how to calculate current based off of torque here? @Kade-MacWilliams */
bool Kinematics::getArmatureCurrent(int encoderL, int encoderR, float forceX, float forceY, float &currentL, float &currentR) {
    double torqueL;
    double torqueR;

    if (this->getTorque(encoderL, encoderR, forceX, forceY, torqueL, torqueR) != 0) 
        return false;

    currentL = this->torqueL * KT;
    currentR = this->torqueR * KT;

    return true;
}
