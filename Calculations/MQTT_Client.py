import os
import time
import json

import paho.mqtt.client as mqtt

# TODO: Create a Logger??

class MQTT_Client:
    def __init__(self, cert_path: str, client_id: str = None): 
        # Import MQTT_Callbacks, must be here due to circular dependency
        import MQTT_Callbacks as callbacks

        # Create MQTT Client object
        self.client = mqtt.Client(protocol=mqtt.MQTTv31, client_id=client_id)
        # Settup for TLS connection to broker, points to certificate file 
        self.client.tls_set(cert_path)
        # Keep track of client name
        self.client_id = client_id
        # Setup the userdata passed to the callbacks function
        self.client.user_data_set(self)

        # Setup callback function for when connection is made
        self.client.on_connect = callbacks.on_connect
        # Setup callback function for when disconnection occurs
        self.client.on_disconnect = callbacks.on_disconnect
        # Setup callback function for when publish request occurs
        self.client.on_publish = callbacks.on_publish
        # Setup callback function for when subscribe request is made
        self.client.on_subscribe = callbacks.on_subscribe
        # Setup callback function for when unsubscribe request occurs
        self.client.on_unsubscribe = callbacks.on_unsubscribe
        # Setup callback function for when message not in filter list occurs
        self.client.on_message = callbacks.on_message
        # Setup callback function for when encoder message is received
        self.client.message_callback_add(f"encoder/#", callbacks.on_encoder)
        # Setup callback function for when current message is received
        self.client.message_callback_add(f"current/#", callbacks.on_current)

    # Connect to Broker and initialize listing loop
    def connect(self, broker_hostname: str, broker_port: int = 8883):
        self.client.connect(host=broker_hostname, port=broker_port, keepalive=60)
        self.client.loop_start()
        # TODO: Handle Failure to connect

    def disconnect(self):
        # TODO: Ensure any published messages have been sent (wait_for_publish())
        self.client.disconnect()

    def publish(self, topic: str, payload, qos: int = 0, retain: bool = False):
        try:
            messageInfo: mqtt.MQTTMessageInfo = self.client.publish(f"{topic}/{self.client_id}", payload, qos, retain)
        except ValueError:
            raise ValueError("<MQTT_Client>: qos is not 0, 1 or 2.",
                             "Or topic is None, has zero length or is invalid (contains a wildcard).",
                             "Or if the length of the payload is greater than 268435455 bytes.")
        
        if messageInfo.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"<MQTT_Client>: {self.client_id} successfully published to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}. Message Content: {payload}")
        elif messageInfo.rc == mqtt.MQTT_ERR_NO_CONN:
            print(f"<MQTT_Client>: {self.client_id} failed to publish to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}. Client is not currently connected")
        else:
            print(f"<MQTT_Client>: {self.client_id} failed to publish to {topic}",
                  f"with qos {qos} and message id {messageInfo.mid}.")


    def subscribe(self, topic: str, qos: int = 0):
        try:
            (result, mid) = self.client.subscribe(topic, qos)
        except ValueError:
            raise ValueError("<MQTT_Client>: qos is not 0, 1 or 2.",
                             "Or topic is None or has zero string length.",
                             "Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"<MQTT_Client>: {self.client_id} successfully subscribed to {topic}",
                  f"with qos {qos}. Message ID: {mid}")
        else:
            print(f"<MQTT_Client>: Client {self.client_id} failed to subscribe to {topic}",
                  f"with qos {qos}. Message ID: {mid}")

    def unsubscribe(self, topic: str):
        try:
            (result, mid) = self.client.unsubscribe(topic)
        except ValueError:
            raise ValueError("<MQTT_Client>: Topic is None or has zero string length.",
                             "Or topic is not a string, tuple or list.")
        
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"<MQTT_Client>: {self.client_id} successfully unsubscribed to {topic}.",
                  f"Message ID: {mid}")
        else:
            print(f"<MQTT_Client>: {self.client_id} failed to unsubscribed to {topic}.",
                  f"Message ID: {mid}")
            
    def message_callback_add(self, sub: str, callback):
        self.client.message_callback_add(sub, callback)

if __name__ == "__main__":
    ##### TESTING #####
    dir_path = os.path.dirname(os.path.realpath(__file__)) + r"\certs\ca-root-cert.crt"
    # Create Clients
    client1 = MQTT_Client(cert_path=dir_path, client_id="trainee") # For Testing set to trainee
    # Connect Clients
    print("Connect Client 1")
    client1.connect(broker_hostname="LAPTOP-HL4N9U5Q")

    # Subscribe Clients
    print("Subscribe Client 1")
    client1.subscribe(f"encoder/#")
    client1.subscribe(f"current/#")

    time.sleep(1)

    test_enc = {'left': 0, 'right': 0}
    test_curr = {'left': 0, 'right': 0}
    test_pos = {'x': 0, 'y': 0}

    # while(1):
    #     if test_enc['left'] > 8000:
    #         test_enc['left'] = 0
    #         test_enc['right'] = 200

    #     test_enc['left'] = test_enc['left'] - 20
    #     test_enc['right'] = test_enc['right'] + 20

    #     # Take turns publishing
    #     print("Publish Client 1")
    #     client1.publish(f"encoder", json.dumps(test_enc), qos=1)
    #     # client1.publish(f"current/{client1.client_id}", json.dumps(client1.test_curr), qos=1)
    #     # client1.publish(f"position/{client1.client_id}", json.dumps(client1.test_pos), qos=1)
    
    #     time.sleep(5)

    while 1:
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': -20, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': -20, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': -10, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 0, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 0, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 10, 'right': 30}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 20, 'right': 30}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 30, 'right': 30}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 20, 'right': 20}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 10, 'right': 10}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': 0, 'right': 0}
        time.sleep(0.1)
        client1.publish(f"encoder", json.dumps(test_enc), qos=1)
        test_enc = {'left': -10, 'right': 10}
        time.sleep(0.1)

    # Disconnect clients
    client1.disconnect()
    # client2.disconnect()