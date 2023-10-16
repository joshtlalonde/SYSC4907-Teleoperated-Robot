import paho.mqtt.client as mqtt

class MQTT_Callbacks:
    def __init__(self):
        pass

    def on_connect(self, client: mqtt.Client, userdata, flags, rc):
        if rc == 0:
            print(f"Connected to MQTT Broker: {rc}")
        elif rc == 1:
            print(f"Connection refused - incorrect protocol version: {rc}")
        elif rc == 2:
            print(f"Connection refused - invalid client identifier: {rc}")
        elif rc == 3:
            print(f"Connection refused - server unavailable: {rc}")
        elif rc == 4:
            print(f"Connection refused - bad username or password: {rc}")
        elif rc == 5:
            print(f"Connection refused - not authorised: {rc}")

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"flags: {flags}")
        print(f"rc: {rc}")

    def on_disconnect(self, client: mqtt.Client, userdata, rc):
        if rc == 0:
            print(f"Disconnected from MQTT Broker: {rc}")
        else:
            print(f"Unexpected Disconnection from MQTT Broker: {rc}")

        print(client)
        print(rc)
        print(userdata)