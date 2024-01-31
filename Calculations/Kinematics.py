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
        self.prev_position = self.position
        # Initial Theta Offset Value
        self.init_theta = 1.0148

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
        self.mqttClient.subscribe(topic='encoder/#', qos=2)
        self.mqttClient.subscribe(topic='current/#', qos=2)
        self.mqttClient.subscribe(topic='force/#', qos=2)

    def updatePosition(self, x: float, y: float):
        """
        Update (x,y) Position of the Arm
        
        Parameters:
            x: x coordinate of the Arm
            y: y coordinate of the Arm
        """

        # Store the current position as the previous position
        self.prev_position = self.position.copy()
        # Update the position
        #self.position =  Kinematic_Equations.forward_kinematics(self.encoderToAngle(self.encoder['left']), self.encoderToAngle(self.encoder['right']), self.prev_y)  # Call the forward kinematics
        self.position['x'] = x
        self.position['y'] = y


    def updateTheta(self, left: float, right: float):
        """
        Update angle from the starting position of the motors
        
        Parameters:
            left: Angle for the left motor
            right: Angle for the right motor
        """
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
        return (encoder * (math.pi/4096)) + self.init_theta # add the intial shaft offset angle
    

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
        return (((angle - self.init_theta) * 4096) / math.pi) # subtarct the inital angle value
    
class TestKinematics(unittest.TestCase):
    def test_encoderToAngle(self): 
        self.assertAlmostEqual((200 * (math.pi/4096)) + 1.0148, 1.1682, 4)

    def test_angleToEncoder(self):
        self.assertAlmostEqual((((1.1682 - 1.0148) * 4096) / math.pi), 200, 0)

    def test_updatePosition(self):
        kin = Kinematics()
        self.assertEqual(kin.position, {'x': 22.5, 'y': 76.32})
        kin.updatePosition(1.5, 2.0)

        self.assertEqual(kin.position, {'x': 1.5, 'y': 2.0})
        self.assertEqual(kin.prev_position, {'x': 22.5, 'y': 76.32})

if __name__ == '__main__':
    # unittest.main()

    print("Starting Kinematics")

    kin = Kinematics()

    time.sleep(1)

    # enc = {'left': 0, 'right': 0}
    
    # # coords (15, 50)    
    # angle = {'left': 0.973742872260748, 'right': 1.2204646692234953}
    # enc['left'] = kin.angleToEncoder(angle=angle['left'])
    # enc['right'] = kin.angleToEncoder(angle['right'])
    # # {'left': -53.5301720379992, 'right': 268.1450391656766}

    # print(enc)

    # # coords (16, 51)
    # angle = {'left': 0.9849743122743927, 'right': 1.197242811692461}
    # enc['left'] = kin.angleToEncoder(angle=angle['left'])
    # enc['right'] = kin.angleToEncoder(angle['right'])
    # # {'left': -38.886650942633246, 'right': 237.86844415950037}

    # print(enc)

    while 1:
        pass

