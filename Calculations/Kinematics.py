import os
import time
import json
import math
import unittest
import Kinematic_Equations
from MQTT_Client import MQTT_Client
#import paho.mqtt.client as mqtt

class Kinematics:
    def __init__(self, broker_hostname: str = "LAPTOP-HL4N9U5Q", cert_path: str = os.path.dirname(os.path.realpath(__file__)) + r"\certs\ca-root-cert.crt", verbose = False):
        # Inital postion is {x: 22.5, y: 76.32}
        self.position = {'x': 22.5, 'y': 76.32} 
        # Initial encoder is {left: 1.0148, right: 1.0148} in radians
        self.theta = {'left': 1.0148, 'right': 1.0148} 
        # Initial previous position is {'x': 22.5, 'y': 76.45} 
        self.prev_position = self.position

        # Set Client ID
        self.client_id = "kin"
        # Initialize Incoming Encoder Message Flag
        self.encoder_flag = False

        # Set the Verbose Level
        self.verbose = verbose


        # Initialize MQTT
        self.mqttClient = MQTT_Client(cert_path=cert_path, client_id=self.client_id, verbose=verbose)
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

    # Update (x,y) Position of the Arm
    # Input:
    #   x: x coordinate of the Arm
    #   y: y coordinate of the Arm
    def updatePosition(self, x: float, y: float):
        # Store the current position as the previous position
        self.prev_position = self.position.copy()
        # Update the position
        #self.position =  Kinematic_Equations.forward_kinematics(self.encoderToAngle(self.encoder['left']), self.encoderToAngle(self.encoder['right']), self.prev_y)  # Call the forward kinematics
        self.position['x'] = x
        self.position['y'] = y

    # Update angle from the starting position of the motors
    # Input:
    #   left: Encoder value for the left motor
    #   right: Angle for the right motor
    def updateTheta(self, encoderLeft: float, encoderRight: float):
        self.theta['left'] = Kinematic_Equations.encoderToAngle(encoderLeft)
        self.theta['right'] = Kinematic_Equations.encoderToAngle(encoderRight)
    
class TestKinematics(unittest.TestCase):
    def test_updatePosition(self):
        kin = Kinematics()
        self.assertEqual(kin.position, {'x': 22.5, 'y': 76.32})
        kin.updatePosition(1.5, 2.0)

        self.assertEqual(kin.position, {'x': 1.5, 'y': 2.0})
        self.assertEqual(kin.prev_position, {'x': 22.5, 'y': 76.32})

if __name__ == '__main__':
    # unittest.main()

    print("Starting Kinematics")

    kin = Kinematics(verbose=True)

    time.sleep(1)
    
    try:
        while True:
            # print(kin.position)

            if kin.encoder_flag:
                # Get Position from Angles
                try:
                    new_position = Kinematic_Equations.forward_kinematics(kin.theta["left"], 
                                                                        kin.theta["right"])
                except ValueError:
                    print(f"No solution for encoder_L: {kin.theta['left']} and encoder_R: {kin.theta['right']} \n\t Previous Position: {kin.position}")
                    time.sleep(0.01)

                    # Forward 0 to position value = no change in position on simulation
                    kin.mqttClient.publish(topic='position', 
                                payload=json.dumps({'x': 0, 'y': 0}), 
                                qos=0)

                    kin.encoder_flag = False
                    continue

                kin.updatePosition(new_position['x'], 
                                   new_position['y'])
                
                # print(new_position['x'], new_position['y'])

                # Get Change in position
                change_position = {}
                change_position['x'] = kin.prev_position['x'] - kin.position['x']
                change_position['y'] = kin.prev_position['y'] - kin.position['y']
                
                # Forward new position to Virtual Environment
                kin.mqttClient.publish(topic='position', 
                                payload=json.dumps(change_position), 
                                qos=0)

                kin.encoder_flag = False
    except KeyboardInterrupt:
        print("Keyboard Interrupt Detected: Exitting...")