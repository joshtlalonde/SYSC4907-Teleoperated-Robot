import json
import Kinematic_Equations
from Kinematics import Kinematics
from MQTT_Client import MQTT_Client
import paho.mqtt.client as mqtt

def on_connect(client: mqtt.Client, userdata: MQTT_Client, flags, rc):
    if rc == 0:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connected to MQTT Broker: {rc}")
    elif rc == 1:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connection refused - incorrect protocol version: {rc}")
    elif rc == 2:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connection refused - invalid client identifier: {rc}")
    elif rc == 3:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connection refused - server unavailable: {rc}")
    elif rc == 4:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connection refused - bad username or password: {rc}")
    elif rc == 5:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Connection refused - not authorised: {rc}")
        # Attempt to reconnect
        client.reconnect()

def on_disconnect(client: mqtt.Client, userdata: MQTT_Client, rc):
    if rc == 0:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Disconnected from MQTT Broker: {rc}")
    else:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Unexpected Disconnection from MQTT Broker: {rc}")
        # Attempt to reconnect
        client.reconnect()

def on_publish(client: mqtt.Client, userdata: MQTT_Client, mid):
    if userdata.verbose:
        print(f"<MQTT_Callbacks>: Published to topic with message id: {mid}")

def on_subscribe(client: mqtt.Client, userdata: MQTT_Client, mid, granted_qos):
    if userdata.verbose:
        print(f"<MQTT_Callbacks>: Subscribed to topic with message id: {mid} and qos: {granted_qos}")

def on_unsubscribe(client: mqtt.Client, userdata: MQTT_Client, mid):
    if userdata.verbose:
        print(f"<MQTT_Callbacks>: Unsubscribed to topic with message id: {mid}")

def on_message(client: mqtt.Client, userdata: MQTT_Client, message: mqtt.MQTTMessage):
    if userdata.verbose:
        print(f"<MQTT_Callbacks>: Received message on topic {message.topic}: {message.payload}")

'''
    This is where the magic happens for messages received from Arm to Kinematics
    This will receive an encoder value from the Arm as a JSON {left: int, right: int}
    These value will be converted to radians and then using forward kinematics they will
    be converted to (x, y) positions

    After getting the position it will then publish to the Virtual Environment
'''
def on_encoder(client: mqtt.Client, userdata: Kinematics, message: mqtt.MQTTMessage):
    if userdata.client_id not in message.topic:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Received ENCODER message on topic {message.topic}: {message.payload}")

        if "trainee" in message.topic or "trainer" in message.topic:
            # Convert JSON Message to Dict
            encoder_payload = json.loads(message.payload.decode())
            # print(f"    Updated 'trainee' encoder value: {message.payload.decode()}")

            # Convert encoder value to angle radians and update Thetas
            userdata.updateTheta(
                userdata.encoderToAngle(encoder_payload["left"]), 
                userdata.encoderToAngle(encoder_payload["right"]))

            # Set Flag
            userdata.encoder_flag = True

def on_current(client: mqtt.Client, userdata: MQTT_Client, message: mqtt.MQTTMessage):
    if userdata.client_id not in message.topic:
        if userdata.verbose:
            print(f"<MQTT_Callbacks>: Received CURRENT message on topic {message.topic}: {message.payload}")

        if "trainee" in message.topic:
            print(f"    TODO!!! Updating 'trainee' current value: {message.payload.decode()}")
            # userdata.test_curr = json.loads(message.payload.decode())