using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Security;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using UnityEngine;

using MQTTnet;
using MQTTnet.Protocol;
using MQTTnet.Client;
using MQTTnet.Formatter;
using System.Reflection;

public class MQTT_Client {
    private IMqttClient mqttClient; // MQTT Client Instance
    private string mqttClientId; // MQTT Client ID
    private MqttFactory mqttFactory; // MQTTFactory instance

    public MQTT_Client(string mqttClientId)
    {
        // Set Client ID
        this.mqttClientId = mqttClientId;
        
        // Setup MQTTFactory
        mqttFactory = new MqttFactory();
        // Create MQTTClient Instance
        mqttClient = mqttFactory.CreateMqttClient();

        // Connect the Message Callback function
        AddMessageCallback(MQTT_Callbacks.OnPosition);
    }

    /*
     * Sets up TLS Options and connects to Broker
     *  
     * BrokerHostname: String of the Broker Host name
     * TlsCertFilePath: Path to the TLS Cert File
     */
    public async Task Connect(string BrokerHostname, string TlsCertFilePath)
    {
        // Get the TLS Certificates
        var certificates = new List<X509Certificate2>();
        X509Certificate2 TlsCert = new X509Certificate2(TlsCertFilePath);
        certificates.Add(TlsCert);

        // Setup TLS Options
        var mqttClientTlsOptions = new MqttClientTlsOptionsBuilder()
            .UseTls(true)
            .WithSslProtocols(SslProtocols.Tls12)
            .WithAllowUntrustedCertificates()
            .WithClientCertificates(certificates)
            .WithIgnoreCertificateChainErrors()
            .WithIgnoreCertificateRevocationErrors()
            .Build(); // Build the MqttClientTlsOptions

        // Setup MQTT Client Options
        var mqttClientOptions = new MqttClientOptionsBuilder()
            .WithClientId(mqttClientId)
            .WithTlsOptions(mqttClientTlsOptions)
            .WithTcpServer(BrokerHostname, 8883) // Setup Server Connection
            .Build(); // Build the MqttClientOptions

        // Connect to Broker
        Debug.Log("Connecting to MQTT Broker...");
        var response = await mqttClient.ConnectAsync(mqttClientOptions, CancellationToken.None);
        Debug.Log("The MQTT client is connected.");

        // return response;
    }

    /*
     * This sample disconnects in a clean way. This will send a MQTT DISCONNECT packet
     * to the server and close the connection afterwards.
     */
    public async Task Disconnect() {
        // Setup Disconnect Options
        var MqttClientDisconnectOptions = new MqttClientDisconnectOptionsBuilder()
            .WithReason(MqttClientDisconnectOptionsReason.NormalDisconnection)
            .Build();

        // This will send the DISCONNECT packet. Calling _Dispose_ without DisconnectAsync the 
        // connection is closed in a "not clean" way. See MQTT specification for more details.
        await mqttClient.DisconnectAsync(MqttClientDisconnectOptions);
    }

    /* 
     * Publish a message to the specified MQTT Topic
     * 
     * Topic: String for the topic that this message will be published to
     * Publish: String that will be pubished on the specified topic
     */
    public async Task Publish(string topic, string payload) {
        // Setup Application Message
        var applicationMessage = new MqttApplicationMessageBuilder()
                // .WithClientId(mqttClientId)
                .WithTopic(topic + "/" + mqttClientId)
                .WithPayload(payload)
                .Build();

        // Publish the Message
        await mqttClient.PublishAsync(applicationMessage, CancellationToken.None);
    }

    /*
     * Subscribe to a Topic to start receiving messages on that Topic
     *
     * topic: String for the topic to subscribe to
     * qos: Int value for the QoS level to subscribe as
     *
     * There are 3 QoS levels in MQTT:
     *   - At most once  (0): Message gets delivered no time, once or multiple times.
     *   - At least once (1): Message gets delivered at least once (one time or more often).
     *   - Exactly once  (2): Message gets delivered exactly once (It's ensured that the message only comes once).
     */
    public async Task Subscribe(string topic, int qos) {
        MqttQualityOfServiceLevel QoS = MqttQualityOfServiceLevel.AtMostOnce;
        // Determine QoS
        if (qos == 0) {
            QoS = MqttQualityOfServiceLevel.AtMostOnce;
        } else if (qos == 1) {
            QoS = MqttQualityOfServiceLevel.AtLeastOnce;
        } else if (qos == 2) {
            QoS = MqttQualityOfServiceLevel.ExactlyOnce;
        }

        // Setup Topic Filter Options
        var mqttTopicFilter = new MqttTopicFilterBuilder()
            .WithTopic(topic)
            .WithQualityOfServiceLevel(QoS)
            .Build();

        // Setup Subscribe Options
        var mqttClientSubscribeOptions = new MqttClientSubscribeOptionsBuilder()
                .WithTopicFilter(mqttTopicFilter)
                .Build();

        // Subscribe to a Topic
        await mqttClient.SubscribeAsync(mqttClientSubscribeOptions, CancellationToken.None);
    }

    /**
     * Unsubscribe from a Topic
     * 
     * topic: String for the name of the topic that you are unsubscribing from
     */
    public async Task Unsubscribe(string topic) {
        // Setup Unsubscribe Options
        var mqttClientUnsubscribeOptions = new MqttClientUnsubscribeOptionsBuilder()
                .WithTopicFilter(topic)
                .Build();

        // Unsubscribe from the Topic
        await mqttClient.UnsubscribeAsync(mqttClientUnsubscribeOptions, CancellationToken.None);
    }

    public void AddMessageCallback(Func<MqttApplicationMessageReceivedEventArgs, Task> OnApplicationMessageReceivedAsync) {
        mqttClient.ApplicationMessageReceivedAsync += OnApplicationMessageReceivedAsync;
    }
}

