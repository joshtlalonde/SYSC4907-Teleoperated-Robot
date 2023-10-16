import os
import time

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
        # self.callbacks = self.MQTT_Callbacks()

        ### CONNECT ###
        # Setup callback function for when connection is made
        self.client.on_connect = self.on_connect
        # Setup callback function for when disconnection occurs
        self.client.on_disconnect = self.on_disconnect
        # Setup callback function for when publish request occurs
        self.client.on_publish = self.on_publish
        # Setup callback function for when subscribe request is made
        self.client.on_subscribe = self.on_subscribe
        # Setup callback function for when unsubscribe request occurs
        self.client.on_unsubscribe = self.on_unsubscribe
        # Setup callback function for when message not in filter list occurs
        self.client.on_message = self.on_message

    # Connect to Broker and initialize listing loop
    def connect(self, broker_hostname: str, broker_port: int = 8883):
        self.client.connect(host=broker_hostname, port=broker_port, keepalive=60)
        self.client.loop_start()

    def disconnect(self):
        # TODO: Ensure any published messages have been sent (wait_for_publish())
        self.client.disconnect()

    def publish(self, topic: str, payload, qos: int = 0, retain: bool = False):
        try:
            messageInfo: mqtt.MQTTMessageInfo = self.client.publish(topic, payload, qos, retain)
        except ValueError:
            raise ValueError("qos is not 0, 1 or 2.",
                             "Or topic is None, has zero length or is invalid (contains a wildcard).",
                             "Or if the length of the payload is greater than 268435455 bytes.")
        
        if messageInfo.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"{self.client_id} successfully published to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}. Message Content: {payload}")
        elif messageInfo.rc == mqtt.MQTT_ERR_NO_CONN:
            print(f"{self.client_id} failed to publish to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}. Client is not currently connected")
        else:
            print(f"{self.client_id} failed to publish to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}.")


    def subscribe(self, topic: str, qos: int = 0):
        try:
            (result, mid) = self.client.subscribe(topic, qos)
        except ValueError:
            raise ValueError("qos is not 0, 1 or 2.",
                             "Or topic is None or has zero string length.",
                             "Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"{self.client_id} successfully subscribed to {topic}",
                  f"with qos {qos}. Message ID: {mid}")
        else:
            print(f"Client {self.client_id} failed to subscribe to {topic}",
                  f"with qos {qos}. Message ID: {mid}")

    def unsubscribe(self, topic: str):
        try:
            (result, mid) = self.client.unsubscribe(topic)
        except ValueError:
            raise ValueError("Topic is None or has zero string length.",
                             "Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"{self.client_id} successfully unsubscribed to {topic}.",
                  f"Message ID: {mid}")
        else:
            print(f"{self.client_id} failed to unsubscribed to {topic}.",
                  f"Message ID: {mid}")
            
    def message_callback_add(self, sub: str, callback):
        self.client.message_callback_add(sub, callback)


######################## CALLBACK FUNCTIONS ########################
### TODO: Find a way to have these in a separate file for easier reading

    def on_connect(self, client: mqtt.Client, userdata, flags, rc):
        if rc == 0:
            print(f"{self.client_id} Connected to MQTT Broker: {rc}")
        elif rc == 1:
            print(f"{self.client_id} Connection refused - incorrect protocol version: {rc}")
        elif rc == 2:
            print(f"{self.client_id} Connection refused - invalid client identifier: {rc}")
        elif rc == 3:
            print(f"{self.client_id} Connection refused - server unavailable: {rc}")
        elif rc == 4:
            print(f"{self.client_id} Connection refused - bad username or password: {rc}")
        elif rc == 5:
            print(f"{self.client_id} Connection refused - not authorised: {rc}")

    def on_disconnect(self, client: mqtt.Client, userdata, rc):
        if rc == 0:
            print(f"{self.client_id} Disconnected from MQTT Broker: {rc}")
        else:
            print(f"{self.client_id} Unexpected Disconnection from MQTT Broker: {rc}")

    def on_publish(self, client: mqtt.Client, userdata, mid):
        print(f"{self.client_id} Published to topic with message id: {mid}")

    def on_subscribe(self, client: mqtt.Client, userdata, mid, granted_qos):
        print(f"{self.client_id} Subscribed to topic with message id: {mid} and qos: {granted_qos}")

    def on_unsubscribe(self, client: mqtt.Client, userdata, mid):
        print(f"{self.client_id} Unsubscribed to topic with message id: {mid}")

    def on_message(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        print(f"{self.client_id} Received message on topic {message.topic}: {message.payload}")

    def on_test_message(self, client: mqtt.Client, userdata, message: mqtt.MQTTMessage):
        print(f"{self.client_id} Received TEST message on topic {message.topic}: {message.payload}")

        self.publish(f"respond/{self.client_id}", f"How is it going from {self.client_id}", qos=1)

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.realpath(__file__)) + r"\certs\ca-root-cert.crt"
    # Create Clients
    client1 = MQTT_Client(cert_path=dir_path, client_id="Client1")
    client2 = MQTT_Client(cert_path=dir_path, client_id="Client2")
    # Connect Clients
    print("Connect Client 1")
    client1.connect(broker_hostname="LAPTOP-HL4N9U5Q")
    print("Connect Client 2")
    client2.connect(broker_hostname="LAPTOP-HL4N9U5Q")
    # Subscribe Clients
    print("Subscribe Client 1")
    client1.subscribe(f"test/{client2.client_id}")
    client1.subscribe(f"respond/{client2.client_id}")
    print("Subscribe Client 2")
    client2.subscribe(f"test/{client1.client_id}")
    client2.subscribe(f"respond/{client1.client_id}")
    
    # Create Test message Filter
    print("Create filter Client 1")
    client1.message_callback_add("test/#", client1.on_test_message)
    print("Create filter Client 2")
    client2.message_callback_add("test/#", client2.on_test_message)

    time.sleep(1)

    # Take turns publishing
    print("Publish Client 1")
    client1.publish(f"test/{client1.client_id}", "Hello From Client1", qos=1)
    print("Publish Client 2")
    client2.publish(f"test/{client2.client_id}", "Hello From Client2", qos=1)
    
    time.sleep(1)

    # Disconnect clients
    client1.disconnect()
    client2.disconnect()