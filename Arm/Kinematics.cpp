
#include "Kinematics.h"

Kinematics::Kinematics(bool verbose)
{
    /* Position Setup */
    // Inital postion is {x: 22.5, y: 76.32}
    this->x = 22.5;
    this->y = 76.32;

    /* Force Setup */
    this->force = 0;
    this->torque[2][2];

    this->verbose = verbose;
}

double Kinematics::getX() {
    return this->x;
}

double Kinematics::getY() {
    return this->y;
}

void Kinematics::getTorque(double torque[2][2]) {
    torque = this->torque;
}

int Kinematics::updatePosition(int encoderL, int encoderR) {
    double prevX = this->x;
    double prevY = this->y;

    // Get Angles (rad) from encoder values
    double theta1 = encoderToAngle(encoderL);
    double theta2 = encoderToAngle(encoderR);

    // Get Position from forward_kinematics
    if (forward_kinematics(theta1, theta2, this->x, this->y) != 0) {
        // Reset to previous position
        this->x = prevX;
        this->y = prevY;

        if (this->verbose)
            Serial.printf("<Kinematics>: No solution found encoder_L: %lf and encoder_R: %lf \n\t Previous Position: (%d, %d)", theta1, theta2, prevX, prevY);

        return -1;
    }

    return 0;
}

int Kinematics::updateTorque(int encoderL, int encoderR, float force) {
    double J[2][2];
    this->force = force;

    // Get Angles (rad) from encoder values
    double theta1 = encoderToAngle(encoderL);
    double theta2 = encoderToAngle(encoderR);

    if (jacobian(theta1, theta2, this->y, J)) {
        if (this->verbose)
            Serial.printf("<Kinematics>: Failed to Calculate Torque Joints with: theta1: %lf, theta2: %lf, current_y: %lf", theta1, theta2, this->y);
            return -1;
    }

    this->torque[0][0] = J[0][0] * force;
    this->torque[0][1] = J[0][1] * force;
    this->torque[1][0] = J[1][0] * force;
    this->torque[1][1] = J[1][1] * force;

    return 0;
}

/** TODO: Unsure how to calculate current based off of torque here? @Kade-MacWilliams */
int Kinematics::torqueCurrent(float &currentL, float &currentR) {
    currentL = this->torque[0][1] * TORQUE_CONSTANT; // Which values from the torque matrix should be applied?
    currentR = this->torque[1][1] * TORQUE_CONSTANT;
}
