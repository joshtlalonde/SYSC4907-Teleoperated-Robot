import os
import time
import json
import math
import unittest
from MQTT_Client import MQTT_Client
#import paho.mqtt.client as mqtt

class Kinematics:
    def __init__(self, broker_hostname: str = "LAPTOP-HL4N9U5Q", cert_path: str = os.path.dirname(os.path.realpath(__file__)) + r"\certs\ca-root-cert.crt"):
        # Inital postion is {x: 22.5, y: 76.32}
        self.position = {'x': 22.5, 'y': 76.32} 
        # Initial encoder is {left: 1.0148, right: 1.0148} in radians
        self.theta = {'left': 1.0148, 'right': 1.0148} 
        # Initial previous position is {'x': 22.5, 'y': 76.45} 
        self.prev_position = {'x': 22.5, 'y': 76.45} 

        # Set Client ID
        self.client_id = "kin"

        # Initialize MQTT
        self.mqttClient = MQTT_Client(cert_path=cert_path, client_id=self.client_id)
        # Connect Clients
        self.mqttClient.connect(broker_hostname=broker_hostname)
        # Setup the userdata passed to the callbacks function
        self.mqttClient.client.user_data_set(self) 
        # ISSUE: There is a massive issue here. We need to be able to pass in the Kinematics data 
        #   to the MQTT_Callbacks so that we can update the position and encoder/current information. 
        #   But how do we do this? There is circular errors. I think we should try and make the code work 
        #   without the Callbacks first and see if that works at least
        # Subscribe to Encoder, Current, and Force Topics
        self.mqttClient.subscribe(topic='encoder/#', qos=1)
        self.mqttClient.subscribe(topic='current/#', qos=1)
        self.mqttClient.subscribe(topic='force/#', qos=1)

    # Update (x,y) Position of the Arm
    # Input:
    #   x: x coordinate of the Arm
    #   y: y coordinate of the Arm
    def updatePosition(self, x: float, y: float):
        # Store the current position as the previous position
        self.prev_position = self.position 
        # Update the position
        #self.position =  Kinematic_Equations.forward_kinematics(self.encoderToAngle(self.encoder['left']), self.encoderToAngle(self.encoder['right']), self.prev_y)  # Call the forward kinematics
        self.position['x'] = x
        self.position['y'] = y

    # Update angle from the starting position of the motors
    # Input:
    #   left: Angle for the left motor
    #   right: Angle for the right motor
    def updateTheta(self, left: float, right: float):
        self.theta['left'] = left
        self.theta['right'] = right

    def encoderToAngle(self, encoder: int):
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
    

    def angleToEncoder(self, angle: float):
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


# class TestKinematicMethods(unittest.TestCase):
#     """
#     The TestKinematicMethods class contains unit tests for the methods in the Kinematics class.
#     """
#     def test_forward_kinematics(self):
#         self.assertAlmostEqual(Kinematic_Equations.forward_kinematics(1.0148, 1.0148, 76.32), {22.5, 76.4437}, 3)

#     def test_inverse_kinematics(self):
#         self.assertAlmostEqual(Kinematic_Equations.inverse_kinematics(22.5, 76.4),{1.0148,1.0148}, 4 )

#     def test_jacobian(self):
#         self.assertAlmostEqual(Kinematic_Equations.jacobian(1.0148, 1.0148, 76.32), {}, 3)

#     def test_transpose_jacobian(self):
#         self.assertAlmostEqual(Kinematic_Equations.transpose_jacobian(1.0148, 1.0148, 76.32), {}, 3)

#     def test_encoderToAngle(self):
#         self.assertAlmostEqual(Kinematic_Equations.encoderToAngle(200),0.1533,4)

#     def test_angleToEncoder(self):
#         self.assertAlmostEqual(Kinematic_Equations.angleToEncoder(0.1533),200,4)

        
# class TestArmPy(unittest.TestCase):
#         """
#         The TestArmPy class contains unit tests for the methods in the Arm_Py class.
#         """
#         def test_getPostion(self):
#             self.assertAlmostEqual(Arm_Py.getPosition(), {22.5,76.32}, 3)

if __name__ == '__main__':
    print("Starting Kinematics")

    kin = Kinematics()

    time.sleep(1)

    while 1:
        pass

