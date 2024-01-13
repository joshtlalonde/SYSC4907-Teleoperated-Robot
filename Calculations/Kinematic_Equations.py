import math
import unittest
import numpy as np

ARM_LENGTH_A = 90 # First Link Length (mm)
ARM_LENGTH_B = 70 # Second Link Length (mm)
ARM_WIDTH = 45 # Distance Between Shafts (mm)

# TODO: HANDLE THE ERROR STATES
# Such as acos must be -1 <= value <= 1
# These should also be tested in unit tests

def forward_kinematics(theta1, theta2, previous_y):
    """
    forward_kinematics:
    Calculate the end effector position based on the given parameters from
    the position encoders on the shaft.

    Parameters:
    - theta1 from position encoder
    - theta2 from position encoder
    - previous_y from global variable

    Returns:
    - Tuple (float, float): A tuple containing the x and y coordinates of the end effector.
    - Starting position is (22.5, 76.4437)
    print(forward_kinematics(1.0148, 1.0148, 76.32))  # x=22.5, y = 76.4437
    """

    # Calculate R1 and R2 using given equations
    Lx = -(ARM_LENGTH_A * math.cos(theta1))
    Ly = (ARM_LENGTH_A * math.sin(theta1))
    Rx = (ARM_LENGTH_A * math.cos(theta2)) + ARM_WIDTH
    Ry = (ARM_LENGTH_A * math.sin(theta2))

    u1 = (Ry - Ly) / (Lx - Rx)
    u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
    u3 = u1**2 + 1
    u4 = (2 * u1 * u2) - (2 * u1 * Lx) - (2 * Ly)
    u5 = u2**2 - (2 * u2 * Lx) + Lx**2 + Ly**2 - ARM_LENGTH_B**2

    # Calculate yP using quadratic equation
    discriminant = (u4**2) - (4 * u3 * u5)

    if discriminant < 0 and discriminant > -1:
        discriminant = 0
    elif discriminant < 0:
        raise ValueError(f"No solution found for ({theta1}rad, {theta2}rad)")

    yP1 = (-u4 + math.sqrt(discriminant)) / (2 * u3)
    yP2 = (-u4 - math.sqrt(discriminant)) / (2 * u3)

    # Choose the solution that is closer to the previous y value
    ypos_endeff = yP1 if abs(yP1 - previous_y) < abs(yP2 - previous_y) else yP2

    # Calculate x based on y
    xpos_endeff = (ypos_endeff * u1) + u2

    return {'x': xpos_endeff, 'y': ypos_endeff}

def inverse_kinematics(xp: float, yp: float):
    """
    Inverse Kinematics Solver:
    Calculate the joint angles (theta1 and theta2) for a robotic arm's end effector based on specified x, y coordinates.

    Parameters:
    - xp: X-coordinate of the desired end effector position.
    - yp: Y-coordinate of the desired end effector position.

    Returns:
    - Tuple (float, float): A tuple containing the calculated theta1 and theta2 joint angles.

    This function implements an inverse kinematics solution for a two-joint robotic arm, determining the joint angles 
    required to position the end effector at the specified (xp, yp) coordinates. The starting position is assumed to be 
    (1.0148, 1.0148).

    The calculations involve determining c1 and c2 using geometric relationships, calculating alpha1 and alpha2 angles 
    using inverse trigonometric functions, and deriving beta1 and beta2 angles based on arm lengths. The final joint angles 
    (theta1 and theta2) are then computed and returned.

    Note: Ensure input coordinates (xp, yp) are compatible with the arm's workspace for accurate results.
    """

# Rest of the function remains unchanged


    # Calculate c1 and c2 using Equations 19 and 20
    c1 = math.sqrt(xp**2 + yp**2)
    c2 = math.sqrt(c1**2 - (2 * xp * ARM_WIDTH) + ARM_WIDTH**2)

    # Calculate alpha1 and alpha2 using Equations 21 and 22
    alpha1 = math.acos(xp / c1)
    alpha2 = math.acos((-xp + ARM_WIDTH) / c2)

    # Calculate beta1 and beta2 using Equation 18
    beta1 = math.acos((ARM_LENGTH_B**2 - ARM_LENGTH_A**2 - c1**2) / (-2 * ARM_LENGTH_A * c1))
    beta2 = math.acos((ARM_LENGTH_B**2 - ARM_LENGTH_A**2 - c2**2) / (-2 * ARM_LENGTH_A * c2))

    # Combine alpha and beta values based on proximity to previous values
    theta1 = math.pi - alpha1 - beta1
    theta2 = math.pi - alpha2 - beta2

    return {'left': theta1, 'right': theta2}

def jacobian(theta1, theta2, current_y):
    """
    Calculates the Jacobian matrix for a robotic arm with two revolute joints based on given shaft angles.

    Parameters:
    - theta1: Shaft angle from the first position encoder.
    - theta2: Shaft angle from the second position encoder.
    - current_y: The current y-position of the end effector.

    Returns:
    - numpy.ndarray: A 2x2 matrix representing the Jacobian matrix, which relates changes in joint angles to changes 
    in the end effector position.

    The function first computes the angles theta3 and theta4 necessary for Jacobian calculation based on the 
    provided shaft angles and the current y-position of the end effector. It then constructs the Jacobian matrix 
    using these angles and returns the result as a numpy array.

    Note: Ensure the parameters are in radians for accurate calculations.
    """

    # Define the matrix J with the given expressions
    Ly = (ARM_LENGTH_A * math.sin(theta1))
    Ry = (ARM_LENGTH_A * math.sin(theta2))
    theta3 = math.pi/2 + math.acos((current_y-Ly)/ARM_LENGTH_B)
    theta4 = math.pi/2 + math.acos((current_y-Ry)/ARM_LENGTH_B) 
    J = np.array([
        [ARM_LENGTH_A * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), ARM_LENGTH_A * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))],
        [-ARM_LENGTH_A * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -ARM_LENGTH_A * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
    ])

    return J

def transpose_jacobian(theta1, theta2, current_y):
    """
    Transpose Jacobian Matrix Calculator:
    Calculates the transposed Jacobian matrix for a two-joint robotic arm based on given joint angles and end effector position.

    Parameters:
    - theta1: Shaft angle from the first position encoder.
    - theta2: Shaft angle from the second position encoder.
    - current_y: The current y-position of the end effector.

    Returns:
    - numpy.ndarray: A 2x2 matrix representing the transposed Jacobian matrix.

    This function computes the transposed Jacobian matrix (J_T) for a two-joint robotic arm, which relates changes in end effector 
    position to changes in joint angles. The input parameters include the current joint angles (theta1 and theta2) and the 
    current y-position of the end effector.

    The matrix J_T is constructed using trigonometric expressions derived from the arm's geometry. The resulting transposed 
    Jacobian matrix is then returned as a numpy array.

    Note: Ensure the input joint angles and end effector position are compatible for accurate calculations.
    """

    # Define the matrix J_T with the given expressions
    Ly = (ARM_LENGTH_A * math.cos(theta1)) 
    Ry = (ARM_LENGTH_A * math.cos(theta2))
    theta3 = math.pi/2 + math.acos((current_y-Ly)/ARM_LENGTH_B)
    theta4 = math.pi/2 + math.acos((current_y-Ry)/ARM_LENGTH_B)
    J_T = np.array([
        [ARM_LENGTH_A * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -ARM_LENGTH_A * ((np.sin(theta1 - theta3) * np.cos(theta4))/np.sin(theta3 - theta4))],
        [ARM_LENGTH_A * ((np.sin(theta4 - theta2) * np.sin(theta3))/np.sin(theta3 - theta4)), -ARM_LENGTH_A * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
    ])

    return J_T

class TestKinematic_Equations(unittest.TestCase):
    """
    The TestKinematicMethods class contains unit tests for the methods in the Kinematics class.
    """
    def test_forward_kinematics(self):
        ret = forward_kinematics(1.0148, 1.0148, 76.32)
        self.assertAlmostEqual(ret['x'], 22.5, 3)
        self.assertAlmostEqual(ret['y'], 76.4437, 3) 

    def test_inverse_kinematics(self):
        ret = inverse_kinematics(22.5, 76.4)
        self.assertAlmostEqual(ret['left'], 1.0148, 4)
        self.assertAlmostEqual(ret['right'], 1.0148, 4)

    # def test_jacobian(self):
    #     self.assertAlmostEqual(jacobian(1.0148, 1.0148, 76.32), {}, 3)

    # def test_transpose_jacobian(self):
        # self.assertAlmostEqual(transpose_jacobian(1.0148, 1.0148, 76.32), {}, 3)

if __name__ == '__main__':
    unittest.main()

    # angle = {'left': 0, 'right': 0}
    # angle = inverse_kinematics(16.0, 51.0)
    # print(angle)
