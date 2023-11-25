import json

from MQTT_Client import MQTT_Client
import paho.mqtt.client as mqtt

def on_connect(client: mqtt.Client, userdata: MQTT_Client, flags, rc):
    if rc == 0:
        print(f"<MQTT_Callbacks>: Connected to MQTT Broker: {rc}")
    elif rc == 1:
        print(f"<MQTT_Callbacks>: Connection refused - incorrect protocol version: {rc}")
    elif rc == 2:
        print(f"<MQTT_Callbacks>: Connection refused - invalid client identifier: {rc}")
    elif rc == 3:
        print(f"<MQTT_Callbacks>: Connection refused - server unavailable: {rc}")
    elif rc == 4:
        print(f"<MQTT_Callbacks>: Connection refused - bad username or password: {rc}")
    elif rc == 5:
        print(f"<MQTT_Callbacks>: Connection refused - not authorised: {rc}")
        # Attempt to reconnect
        client.reconnect()

def on_disconnect(client: mqtt.Client, userdata: MQTT_Client, rc):
    if rc == 0:
        print(f"<MQTT_Callbacks>: Disconnected from MQTT Broker: {rc}")
    else:
        print(f"<MQTT_Callbacks>: Unexpected Disconnection from MQTT Broker: {rc}")
        # Attempt to reconnect
        client.reconnect()

def on_publish(client: mqtt.Client, userdata: MQTT_Client, mid):
    print(f"<MQTT_Callbacks>: Published to topic with message id: {mid}")

def on_subscribe(client: mqtt.Client, userdata: MQTT_Client, mid, granted_qos):
    print(f"<MQTT_Callbacks>: Subscribed to topic with message id: {mid} and qos: {granted_qos}")

def on_unsubscribe(client: mqtt.Client, userdata: MQTT_Client, mid):
    print(f"<MQTT_Callbacks>: Unsubscribed to topic with message id: {mid}")

def on_message(client: mqtt.Client, userdata: MQTT_Client, message: mqtt.MQTTMessage):
    print(f"<MQTT_Callbacks>: Received message on topic {message.topic}: {message.payload}")

def on_encoder(client: mqtt.Client, userdata: MQTT_Client, message: mqtt.MQTTMessage):
    if userdata.client_id not in message.topic:
        print(f"<MQTT_Callbacks>: Received ENCODER message on topic {message.topic}: {message.payload}")

        if "trainee" in message.topic:
            print(f"    Updating 'trainee' encoder value: {message.payload.decode()}")
            userdata.test_enc = json.loads(message.payload.decode())

def on_current(client: mqtt.Client, userdata: MQTT_Client, message: mqtt.MQTTMessage):
    if userdata.client_id not in message.topic:
        print(f"<MQTT_Callbacks>: Received CURRENT message on topic {message.topic}: {message.payload}")

        if "trainee" in message.topic:
            print(f"    Updating 'trainee' current value: {message.payload.decode()}")
            userdata.test_curr = json.loads(message.payload.decode())