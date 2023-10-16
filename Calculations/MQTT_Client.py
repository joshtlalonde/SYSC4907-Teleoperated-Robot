import paho.mqtt.client as mqtt
from MQTT_Callbacks import MQTT_Callbacks

# TODO: Create a Logger??

class MQTT_Client:
    def __init__(self, cert_path: str):        
        # Create MQTT Client object
        self.client = mqtt.Client(protocol=mqtt.MQTTv31)
        # Settup for TLS connection to broker, points to certificate file 
        self.client.tls_set(cert_path)

        # Attach the Client Callbacks class to handle the callbacks
        callbacks = MQTT_Callbacks()

        ### CONNECT ###
        # Setup callback function for when connection is made
        self.client.on_connect = callbacks.on_connect
        # Setup callback function for when disconnection occurs
        self.client.on_disconnect = callbacks.on_disconnect

    # Connect to Broker and initialize listing loop
    def connect(self, broker_hostname: str, broker_port: int = 8883, client_id: str = None):
        self.client.connect(host=broker_hostname, port=broker_port, keepalive=60, bind_address="192.168.2.12")
        self.client.loop_forever()

    def publish(self):
        pass

    def subscribe(self):
        pass

if __name__ == "__main__":
    client = MQTT_Client(r"C:\Users\Josh's PC\Documents\University\Classes\Capstone Project\MQTT\certs\ca-root-cert.crt")
    client.connect("LAPTOP-HL4N9U5Q")
    # client.connect("192.168.2.18", 8883)
    # client.connect("mqtt.eclipseprojects.io", broker_port=1883)