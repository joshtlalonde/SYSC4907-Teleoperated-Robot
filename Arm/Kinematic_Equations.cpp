
#include "Kinematic_Equations.h"

float encoderToAngle(int encoder) {
    return (encoder * (PI / 4096)) + INIT_THETA; // add the intial shaft offset angle
}


int angleToEncoder(float angle) {
    return (((angle - INIT_THETA) * 4096) / PI); // subtarct the inital angle value
}

int forward_kinematics(float theta1, float theta2, double &x, double &y) {
    /* Calculate R1 and R2 using given equations */
    double Lx = (ARM_LENGTH_A * cos(theta1));
    double Ly =(ARM_LENGTH_A * sin(theta1));
    double Rx = (ARM_LENGTH_A * cos(theta2)) + ARM_WIDTH;
    double Ry = (ARM_LENGTH_A * sin(theta2));

    /* Helper equations */
    double u1 = (Ry - Ly) / (Lx - Rx);
    double u2 = (pow(Lx, 2) + pow(Ly, 2) - pow(Rx, 2) - pow(Ry, 2)) / (2 * (Lx - Rx));
    double u3 = pow(u1, 2) + 1;
    double u4 = (2 * u1 * u2) - (2 * u1 * Lx) - (2 * Ly);
    double u5 = pow(u2, 2) - (2 * u2 * Lx) + pow(Lx, 2) + pow(Ly, 2) - pow(ARM_LENGTH_B, 2);

    /* Calculate yP using quadratic equation */
    double discriminant = pow(u4, 2) - (4 * u3 * u5);

    if (discriminant < 0 && discriminant > -1)
        discriminant = 0;
    else if (discriminant < 0)
        Serial.printf("No solution found for (%frad, %frad)", theta1, theta2);
        return -1;

    double yP1 = (-u4 + sqrt(discriminant)) / (2 * u3);
    double yP2 = (-u4 - sqrt(discriminant)) / (2 * u3);

    /* Get max ypos since there are two possible ones */
    double ypos_endeff = max(yP1, yP2);
    // if yP1>=0 and yP2>=0:
    //    ypos_endeff = max(yP1, yP2)
    // elif yP1 >=0 or yP1 >=0:
    //     if yP1 > 0:
    //         ypos_endeff = yP2
    //     else:
    //         ypos_endeff = yP1
    // else:
    //     ypos_endeff = min(yP1,yP2)

    /* Calculate x based on y */
    double xpos_endeff = (ypos_endeff * u1) + u2;

    return 0;
}

int inverse_kinematics(float x, float y, float &theta1, float &theta2) {
    /* Calculate c1 and c2 using Equations 19 and 20 */
    double c1 = sqrt(pow(x, 2) + pow(y, 2));
    double c2 = sqrt(pow(c1, 2) - (2 * x * ARM_WIDTH) + pow(ARM_WIDTH, 2));

    /* Calculate alpha1 and alpha2 using Equations 21 and 22 */
    double alpha1 = acos(x / c1);
    double alpha2 = acos((-x + ARM_WIDTH) / c2);

    /* Calculate beta1 and beta2 using Equation 18 */
    double beta1 = acos((pow(ARM_LENGTH_B, 2) - pow(ARM_LENGTH_A, 2) - pow(c1, 2)) / (-2 * ARM_LENGTH_A * c1));
    double beta2 = acos((pow(ARM_LENGTH_B, 2) - pow(ARM_LENGTH_A, 2) - pow(c2, 2)) / (-2 * ARM_LENGTH_A * c2));

    /* Combine alpha and beta values based on proximity to previous values */
    theta1 = alpha1 + beta1;
    theta2 = PI - alpha2 - beta2;

    return 0;
}

int jacobian(float theta1, float theta2, double current_y, double J[2][2]) {
    /* Define the matrix J with the given expressions */
    double Ly = (ARM_LENGTH_A * sin(theta1));
    double Ry = (ARM_LENGTH_A * sin(theta2));
    double theta3 = PI/2 + acos((current_y - Ly) / ARM_LENGTH_B);
    double theta4 = PI/2 + acos((current_y - Ry) / ARM_LENGTH_B);

    J[0][0] = ARM_LENGTH_A * ((sin(theta1 - theta3) * sin(theta4)) / sin(theta3 - theta4));
    J[0][1] = ARM_LENGTH_A * ((sin(theta4 - theta2) * cos(theta3)) / sin(theta3 - theta4));
    J[1][0] = -ARM_LENGTH_A * ((sin(theta1 - theta3) * sin(theta4)) / sin(theta3 - theta4));
    J[1][1] = -ARM_LENGTH_A * ((sin(theta4 - theta2) * cos(theta3)) / sin(theta3 - theta4));

    return 0;
}

int transpose_jacobian(float theta1, float theta2, double current_y, double J_T[2][2]) {
    /* Define the matrix J_T with the given expressions */
    double Ly = (ARM_LENGTH_A * cos(theta1));
    double Ry = (ARM_LENGTH_A * cos(theta2));
    double theta3 = PI/2 + acos((current_y - Ly) / ARM_LENGTH_B);
    double theta4 = PI/2 + acos((current_y - Ry) / ARM_LENGTH_B);

    J_T[0][0] = ARM_LENGTH_A * ((sin(theta1 - theta3) * sin(theta4)) / sin(theta3 - theta4));
    J_T[0][1] = -ARM_LENGTH_A * ((sin(theta1 - theta3) * cos(theta4)) / sin(theta3 - theta4));
    J_T[1][0] = ARM_LENGTH_A * ((sin(theta4 - theta2) * sin(theta3)) / sin(theta3 - theta4));
    J_T[1][1] = -ARM_LENGTH_A * ((sin(theta4 - theta2) * cos(theta3)) / sin(theta3 - theta4));

    return 0;
}

int torque_current(float force, double &J_T) {
    Serial.println("torque_current: TODO");
}