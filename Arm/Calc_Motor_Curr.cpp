#include <cmath>
#include <iostream>
#include <vector>
#include <array>

const double ARM_LENGTH_A = 90; // First Link Length (mm)
const double ARM_LENGTH_A_Meters = 0.09; // First Link Length (m)
const double ARM_LENGTH_B = 70; // Second Link Length (mm)
const double ARM_WIDTH = 45; // Distance Between Shafts (mm)
const double KT[2] = {1/232.65, 1/232.65}; // Motor velocity constant

/**
 * Calculate the transpose Jacobian matrix for a given configuration of the arm.
 * 
 * @param theta1 Angle of the first joint (radians).
 * @param theta2 Angle of the second joint (radians).
 * @param current_y Current height (y-coordinate) of the end-effector (mm).
 * @return Transpose Jacobian matrix as a 2x2 array.
 */
std::array<std::array<double, 2>, 2> get_transpose_jacobian(double theta1, double theta2, double current_y) {
    double Ly = ARM_LENGTH_A * std::sin(theta1);
    std::cout << "Height of the Left joint is: " << Ly << std::endl;
    double Ry = ARM_LENGTH_A * std::sin(theta2);
    std::cout << "Height of the Right joint is: " << Ry << std::endl;
    double theta3 = M_PI / 2 - std::acos((current_y - Ly) / ARM_LENGTH_B);
    if (theta3 > -0.003 && theta3 < 0) {
        theta3 = 0;
    }
    double theta4 = M_PI / 2 + std::acos((current_y - Ry) / ARM_LENGTH_B);
    std::cout << "The value for theta3 is: " << theta3 << std::endl;
    std::cout << "The value for theta4 is: " << theta4 << std::endl;
    std::array<std::array<double, 2>, 2> J_T = {{
        {ARM_LENGTH_A_Meters * ((std::sin(theta1 - theta3) * std::sin(theta4)) / std::sin(theta3 - theta4)), -ARM_LENGTH_A_Meters * ((std::sin(theta1 - theta3) * std::cos(theta4)) / std::sin(theta3 - theta4))},
        {ARM_LENGTH_A_Meters * ((std::sin(theta4 - theta2) * std::sin(theta3)) / std::sin(theta3 - theta4)), -ARM_LENGTH_A_Meters * ((std::sin(theta4 - theta2) * std::cos(theta3)) / std::sin(theta3 - theta4))}
    }};
    return J_T;
}

void test_transpose_jacobian() {
    std::array<std::array<double, 2>, 2> result;

    // Test case 1: Same angles, different y
    result = get_transpose_jacobian(2.1268, 2.1268, 76.3);
    std::cout << "Test case 1: " << result[0][0] << " " << result[0][1] << " " << result[1][0] << " " << result[1][1] << std::endl;

    // Test case 2: Different angles, same y
    result = get_transpose_jacobian(2.1268, 1.0148, 130);
    std::cout << "Test case 2: " << result[0][0] << " " << result[0][1] << " " << result[1][0] << " " << result[1][1] << std::endl;

    // Test case 3: Different angles, different y
    result = get_transpose_jacobian(1.8, 1.6, 130);
    std::cout << "Test case 3: " << result[0][0] << " " << result[0][1] << " " << result[1][0] << " " << result[1][1] << std::endl;

    // Test case 4: Test with high y value
    result = get_transpose_jacobian(1.8, 1.6,150);
    std::cout << "Test case 4: " << result[0][0] << " " << result[0][1] << " " << result[1][0] << " " << result[1][1] << std::endl;
}

/**
 * Calculate the armature currents for the left and right motors to apply
 * a given force to the endeffector.
 * 
 * @param theta1 Angle of the first joint (radians).
 * @param theta2 Angle of the second joint (radians).
 * @param current_y Current height (y-coordinate) of the end-effector (mm).
 * @return A pair containing the left motor current and the right motor current in .
 */
std::pair<double, double> get_armature_current(double theta1, double theta2, double current_y) {
    std::array<std::array<double, 2>, 2> JT = get_transpose_jacobian(theta1, theta2, current_y);
    double F[2] = {0.1, 0.1}; // note random values in now - Change to get_force(Fx,Fy)
    double torque[2] = {JT[0][0]*F[0] + JT[0][1]*F[1], JT[1][0]*F[0] + JT[1][1]*F[1]}; // Matrix multiplication 
    double Ial = torque[0] * KT[0];
    double Iar = torque[1] * KT[1];
    return std::make_pair(Ial, Iar);
}

void test_get_armature_current() {
    std::pair<double, double> result;

    // Test case 1: Same angles, different y
    result = get_armature_current(2.1268, 2.1268, 76.3);
    std::cout << "Test case 1: left motor current = " << result.first << ", right motor current = " << result.second << std::endl;

    // Test case 2: Different angles, same y
    result = get_armature_current(2.1268, 1.0148, 130);
    std::cout << "Test case 2: left motor current = " << result.first << ", right motor current = " << result.second << std::endl;

    // Test case 3: Different angles, different y
    result = get_armature_current(1.8, 1.6, 130);
    std::cout << "Test case 3: left motor current = " << result.first << ", right motor current = " << result.second << std::endl;

    // Test case 4: Test with high y value
    result = get_armature_current(1.8, 1.6,150);
    std::cout << "Test case 4: left motor current = " << result.first << ", right motor current = " << result.second << std::endl;
}

int main() {
    test_transpose_jacobian();
    test_get_armature_current();
    return 0;
}

