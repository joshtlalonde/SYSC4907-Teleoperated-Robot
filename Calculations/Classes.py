import os
import time
import json
import math
import numpy as np
import unittest
#from MQTT_Client import MQTT_Client
#import paho.mqtt.client as mqtt

class Arm_Py:
    """
    The Arm_Py class represents a physical robotic arm with methods to set and get
    its position, joint angles, and encoder values.
    """
    def __init__(self, position: tuple , encoder1: float, encoder2: float, prev_y: float ):
        self.position = position # inital postion is {22.5, 76.32}
        self.encoder1 = encoder1 # 1.0148 inital angle
        self.encoder2 = encoder2 #1.0148 inital angle
        self.prev_y = prev_y #76.45 Initialize the previous y value

    def setPosition(self):
        # This method will get the position
        self.prev_y = self.postion[0] # Store the current y value as the previous y value
        self.position = Kinematics.forward_kinematics(self.encoderToAngle(self.encoder1), self.encoderToAngle(self.encoder2), self.prev_y)  # Call the forward kinematics

    def getPosition(self):
        # This method will set the position
        return self.position

    def getTheta1(self, encoder):
        return Kinematics.encoderToAngle(encoder)

    def setTheta1(self, encoder1: float):
        self.theta1 = Kinematics.encoderToAngle(encoder1)

    def getTheta2(self):
        return self.theta2

    def setTheta2(self, encoder2: float):
        self.theta2 = Kinematics.encoderToAngle(encoder2)

    def setEncoders(self, encoder1, encoder2):
        self.encoder1 = encoder 

    def setEncoder2(self, theta2: float):
        self.encoder2 =  Kinematics.angleToEncoder(theta2) 
    
    def getEncoder1(self):
        return self.encoder1

    def getEncoder2(self):
        return self.encoder2
    
    def encoderToAngle(encoder: int):
        """
        Encoder to Shaft Angle Converter:
        Converts an encoder value to the corresponding shaft angle for a rotary system.

        Parameters:
        - encoder: Integer representing the encoder value.

        Returns:
        - float: The calculated shaft angle in radians.

        This function transforms an encoder value into its corresponding shaft angle, considering a conversion factor of 8192 
        encoder units to 2*pi radians. The initial shaft offset of 1.0148 radians is added to the result.

        Note: Ensure the encoder value is within the appropriate range for accurate angle conversion.
        """
        return (encoder * (math.pi/4096)) + 1.0148 # add the intial shaft offset
    

    def angleToEncoder(angle: float):
        """
        Shaft Angle to Encoder Value Converter:
        Converts a shaft angle to the corresponding encoder value for a rotary system.

        Parameters:
        - angle: The shaft angle in radians.

        Returns:
        - int: The calculated encoder value.

        This function converts a given shaft angle to its corresponding encoder value, using a conversion factor of 4096 encoder 
        units per radian. The initial offset of 1.0148 radians is subtracted from the result.

        Note: Ensure the input shaft angle is within the appropriate range for accurate encoder value calculation.
        """
        return ((angle * 4096) / math.pi) - 1.0148 # subtarct the inital values
    
    def setManuallyPosition(self, x: float, y: float):
        self.position = x,y

class Kinematics:
    """
    The Kinematics class represents a two-joint robotic arm with methods for 
    forward kinematics, inverse kinematics, Jacobian matrix calculation, transposed Jacobian matrix 
    calculation, and encoder-to-angle and angle-to-encoder conversions.
    
    """

    def __init__(self,  a: int, b: int, c: int, arm: Arm_Py):
        # Your initialization code here
        self.a = a
        self.b = b
        self.c = c
        self.arm = arm

    def forward_kinematics(self, theta1, theta2, previous_y, arm: Arm_Py):
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
        Lx = -(self.a * math.cos(theta1))
        Ly = (self.a * math.sin(theta1))
        Rx = (self.a * math.cos(theta2)) + self.w
        Ry = (self.a * math.sin(theta2))

        u1 = (Ry - Ly) / (Lx - Rx)
        u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
        u3 = u1**2 + 1
        u4 = (2 * u1 * u2) - (2 * u1 * Lx) - (2 * Ly)
        u5 = u2**2 - (2 * u2 * Lx) + Lx**2 + Ly**2 - self.b**2

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

        return (xpos_endeff, ypos_endeff)

    def inverse_kinematics(self, xp: float, yp: float, Arm_Py: Arm_Py):
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
        c2 = math.sqrt(c1**2 - (2 * xp * self.w) + self.w**2)

        # Calculate alpha1 and alpha2 using Equations 21 and 22
        alpha1 = math.acos(xp / c1)
        alpha2 = math.acos((-xp + self.w) / c2)

        # Calculate beta1 and beta2 using Equation 18
        beta1 = math.acos((self.b**2 - self.a**2 - c1**2) / (-2 * self.a * c1))
        beta2 = math.acos((self.b**2 - self.a**2 - c2**2) / (-2 * self.a * c2))

        # Combine alpha and beta values based on proximity to previous values
        theta1 = math.pi - alpha1 - beta1
        theta2 = math.pi - alpha2 - beta2

        return (theta1, theta2)
    
    def jacobian(self, theta1, theta2, current_y):
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
        Ly = (self.a * math.sin(theta1))
        Ry = (self.a * math.sin(theta2))
        theta3 = math.pi/2 + math.acos((current_y-Ly)/self.b)
        theta4 = math.pi/2 + math.acos((current_y-Ry)/self.b) 
        J = np.array([
            [self.a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), self.a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))],
            [-self.a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -self.a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
        ])

        return J

    def transpose_jacobian(self, theta1, theta2, current_y):
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
        Ly = (self.a * math.cos(theta1)) 
        Ry = (self.a * math.cos(theta2))
        theta3 = math.pi/2 + math.acos((current_y-Ly)/self.b)
        theta4 = math.pi/2 + math.acos((current_y-Ry)/self.b)
        J_T = np.array([
            [self.a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -self.a * ((np.sin(theta1 - theta3) * np.cos(theta4))/np.sin(theta3 - theta4))],
            [self.a * ((np.sin(theta4 - theta2) * np.sin(theta3))/np.sin(theta3 - theta4)), -self.a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
        ])

        return J_T


    



class TestKinematicMethods(unittest.TestCase):
    """
    The TestKinematicMethods class contains unit tests for the methods in the Kinematics class.
    """
    def test_forward_kinematics(self):
        self.assertAlmostEqual(Kinematics.forward_kinematics(1.0148, 1.0148, 76.32), {22.5, 76.4437}, 3)

    def test_inverse_kinematics(self):
        self.assertAlmostEqual(Kinematics.inverse_kinematics(22.5, 76.4),{1.0148,1.0148}, 4 )

    def test_jacobian(self):
        self.assertAlmostEqual(Kinematics.jacobian(1.0148, 1.0148, 76.32), {}, 3)

    def test_transpose_jacobian(self):
        self.assertAlmostEqual(Kinematics.transpose_jacobian(1.0148, 1.0148, 76.32), {}, 3)

    def test_encoderToAngle(self):
        self.assertAlmostEqual(Kinematics.encoderToAngle(200),0.1533,4)

    def test_angleToEncoder(self):
        self.assertAlmostEqual(Kinematics.angleToEncoder(0.1533),200,4)

        
class TestArmPy(unittest.TestCase):
        """
        The TestArmPy class contains unit tests for the methods in the Arm_Py class.
        """
        def test_getPostion(self):
            self.assertAlmostEqual(Arm_Py.getPosition(), {22.5,76.32}, 3)

if __name__ == '__main__':
    print("hello all")

