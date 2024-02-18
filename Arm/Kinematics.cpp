
#include "Kinematics.h"

Kinematics::Kinematics(bool verbose)
{
    /* Position Setup */
    // Inital postion is {x: 22.5, y: 76.32}
    this->x = 22.5;
    this->y = 76.32;

    /* Force Setup */
    // TODO
    // this->torque ...?

    this->verbose = verbose;
}

double Kinematics::getX() {
    return this->x;
}

double Kinematics::getY() {
    return this->y;
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