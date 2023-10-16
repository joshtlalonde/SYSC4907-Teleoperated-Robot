import os

import paho.mqtt.client as mqtt
# from MQTT_Callbacks import MQTT_Callbacks

# TODO: Create a Logger??

class MQTT_Client:
    def __init__(self, cert_path: str, client_id: str = None):        
        # Create MQTT Client object
        self.client = mqtt.Client(protocol=mqtt.MQTTv31, client_id=client_id)
        # Settup for TLS connection to broker, points to certificate file 
        self.client.tls_set(cert_path)
        # Keep track of client name
        self.client_id = client_id

        # Attach the Client Callbacks class to handle the callbacks
        self.callbacks = MQTT_Callbacks()

        ### CONNECT ###
        # Setup callback function for when connection is made
        self.client.on_connect = self.callbacks.on_connect
        # Setup callback function for when disconnection occurs
        self.client.on_disconnect = self.callbacks.on_disconnect
        # Setup callback function for when publish request occurs
        self.client.on_publish = self.callbacks.on_publish
        # Setup callback function for when subscribe request is made
        self.client.on_subscribe = self.callbacks.on_subscribe
        # Setup callback function for when unsubscribe request occurs
        self.client.on_unsubscribe = self.callbacks.on_unsubscribe
        # Setup callback function for when message not in filter list occurs
        self.client.on_message = self.callbacks.on_message

    # Connect to Broker and initialize listing loop
    def connect(self, broker_hostname: str, broker_port: int = 8883):
        self.client.connect(host=broker_hostname, port=broker_port, keepalive=60)
        self.client.loop_forever()

    def publish(self, topic: str, payload, qos: int = 0, retain: bool = False):
        try:
            messageInfo: mqtt.MQTTMessageInfo = self.client.publish(topic, payload, qos, retain)
        except ValueError:
            raise ValueError("qos is not 0, 1 or 2. \
                              Or topic is None, has zero length or is invalid (contains a wildcard). \
                              Or if the length of the payload is greater than 268435455 bytes.")
        
        if messageInfo.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"Client {self.client_id} successfully published to {topic} \
                    with qos {qos} and message id {messageInfo.mid}. Message Content: {payload}")
        elif messageInfo.rc == mqtt.MQTT_ERR_NO_CONN:
            print(f"Client {self.client_id} failed to publish to {topic} \
                    with qos {qos} and message id {messageInfo.mid}. Client is not currently connected")
        else:
            print(f"Client {self.client_id} failed to publish to {topic} \
                    with qos {qos} and message id {messageInfo.mid}.")


    def subscribe(self, topic: str, qos: int = 0):
        try:
            (result, mid) = self.client.subscribe(topic, qos)
        except ValueError:
            raise ValueError("qos is not 0, 1 or 2. \
                              Or topic is None or has zero string length. \
                              Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"Client {self.client_id} successfully subscribed to {topic} \
                    with qos {qos}. Message ID: {mid}")
        else:
            print(f"Client {self.client_id} failed to subscribe to {topic} \
                    with qos {qos}. Message ID: {mid}")

    def unsubscribe(self, topic: str):
        try:
            (result, mid) = self.client.unsubscribe(topic)
        except ValueError:
            raise ValueError("Topic is None or has zero string length. \
                              Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"Client {self.client_id} successfully unsubscribed to {topic}. \
                    Message ID: {mid}")
        else:
            print(f"Client {self.client_id} failed to unsubscribed to {topic}. \
                    Message ID: {mid}")
            
    def message_callback_add(self, sub: str, callback):
        self.client.message_callback_add(sub, callback)

class MQTT_Callbacks(MQTT_Client):
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

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"rc: {rc}")

    def on_publish(self, client: mqtt.Client, userdata, mid):
        print(f"Published to topic with message id: {mid}")

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"mid: {mid}")

    def on_subscribe(self, client: mqtt.Client, userdata, mid, granted_qos):
        print(f"Subscribed to topic with message id: {mid} and qos: {granted_qos}")

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"mid: {mid}")
        print(f"granted_qos: {granted_qos}")

    def on_unsubscribe(self, client: mqtt.Client, userdata, mid):
        print(f"Unsubscribed to topic with message id: {mid}")

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"mid: {mid}")

    def on_message(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        print(f"Received message on topic {message.topic}: {message.payload}")

        print(f"client: {client}")
        print(f"userdata: {userdata}")
        print(f"message: {message}")

    def on_test_message(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        print(f"Received TEST message on topic {message.topic}: {message.payload}")

        super().publish("respond/topic", f"How is it going from {super().client_id}")

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.realpath(__file__)) + r"\certs\ca-root-cert.crt"
    # Create Clients
    client1 = MQTT_Client(cert_path=dir_path, client_id="Client1")
    client2 = MQTT_Client(cert_path=dir_path, client_id="Client2")
    # Connect Clients
    client1.connect(broker_hostname="LAPTOP-HL4N9U5Q")
    client2.connect(broker_hostname="LAPTOP-HL4N9U5Q")
    # Subscribe Clients
    client1.subscribe("test/topic")
    client2.subscribe("test/topic")
    
    # Create Test message Filter
    client1.message_callback_add("test/*", client1.callbacks.on_test_message)
    client2.message_callback_add("test/*", client1.callbacks.on_test_message)

    # Take turns publishing
    client1.publish("test/topic", "Hello From Client1")
    client1.publish("test/topic", "Hello From Client2")
    