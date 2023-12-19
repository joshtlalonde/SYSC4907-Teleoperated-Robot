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
// using MQTTnet.Client.Options;
// using MQTTnet.Client.Subscribing;
using MQTTnet.Protocol;
using MQTTnet.Client;
using MQTTnet.Formatter;

public class TestMQTT {
    private IMqttClient mqttClient; // MQTT Client Instance
    private string mqttClientId; // MQTT Client ID
    private MqttFactory mqttFactory; // MQTTFactory instance

    public TestMQTT(string mqttClientId)
    {
        // Setup MQTTFactory
        mqttFactory = new MqttFactory();
        // Create MQTTClient Instance
        mqttClient = mqttFactory.CreateMqttClient();
    }

    private const string cert = @"-----BEGIN CERTIFICATE-----
MIIDqTCCApECFEShFwlSY0ZRsbknfESNroARPO5qMA0GCSqGSIb3DQEBCwUAMIGQ
MQswCQYDVQQGEwJDQTEQMA4GA1UECAwHT250YXJpbzEPMA0GA1UEBwwGT3R0YXdh
MRwwGgYDVQQKDBNDYXJsZXRvbiBVbml2ZXJzaXR5MREwDwYDVQQLDAhTWVNDNDkw
NzEtMCsGCSqGSIb3DQEJARYeam9zaHRsYWxvbmRlQGNtYWlsLmNhcmxldG9uLmNh
MB4XDTIzMTAxNjE0NDIyOFoXDTI0MTAxNTE0NDIyOFowgZAxCzAJBgNVBAYTAkNB
MRAwDgYDVQQIDAdPbnRhcmlvMQ8wDQYDVQQHDAZPdHRhd2ExHDAaBgNVBAoME0Nh
cmxldG9uIFVuaXZlcnNpdHkxETAPBgNVBAsMCFNZU0M0OTA3MS0wKwYJKoZIhvcN
AQkBFh5qb3NodGxhbG9uZGVAY21haWwuY2FybGV0b24uY2EwggEiMA0GCSqGSIb3
DQEBAQUAA4IBDwAwggEKAoIBAQC87DADBiBgpq3rj6JtYvP42qe65eKCAzVl8XPH
YELU14jAmjEbrqK5u+MKu5XFA0VH1aiW7KreB94Rrk7u8kwL0TBpzH6nsvGQ8xOb
qKa5bjC/bnjH84SfuteP+N7ys1Am9/4tvtkWfSSgGFDwsArB26JR2dqsUHVlIu2V
EUF87w0eov/IHFc1sLHlSXCr+r5BmyDY5hIKUPMooCEZuNAtOwemPPj9Uf0GCPUk
TXCD3ruFfQZjnRWS6VefQgOK3exuBdJ5C75wC8Mi85V1cA2FpEnkDyghoeMi3Z5W
aVLxLQ+9ZrbOfjDfP5U5GeoFESElS+huf6GSS6fSbnBZ7eUHAgMBAAEwDQYJKoZI
hvcNAQELBQADggEBABlsUSFzGouX9MJIz3eKcJSDI8ofrNE7jBZiKkc2TrrdqS13
baEPVPdbil6KwJTb/fYJOculKsa4jZ6O3O9l4OmcE6GH3+lYuxfQHY4Vj/2eWjz6
1tHsw3PpytoGrCIaVGzw6Ngq5o9NG+sxHlGT/8d7IxeRpICzkuqwShCGIhjTFpuo
5UFtfD7bziq+eq862F+UX+EelA5sm3VlywcYDs86R4kB6fpAbOlNDBDUSCp8+T6A
KYHJBEI3d1FqFcdGTPMIcJr69Mspw0PAtZbsbtLexoHPagfOQYXQ+Mb3BTcBMmYr
6YKIpJlY4RDKzDencpJlyO9KUaU2dpCB7q7ZaXc=
-----END CERTIFICATE-----";

    /*
     * Sets up TLS Options and connects to Broker
     */
    public async Task Connect_Client(string BrokerHostname, string TlsCertFilePath)
    {

        // Get the TLS Certificates
        Debug.Log("Setup TLS Certificates");
        var certificates = new List<X509Certificate2>();
        // X509Certificate2 TlsCert = new X509Certificate2(File.ReadAllBytes(TlsCertFilePath));
        var TlsCert = new X509Certificate2("./Assets/Scripts/certs/client_sercert.pfx", "1234");
        certificates.Add(TlsCert);

        // Setup TLS Options
        Debug.Log("Build TLS Options");
        var mqttClientTlsOptions = new MqttClientTlsOptionsBuilder()
            .UseTls(true)
            .WithSslProtocols(SslProtocols.Tls12)
            .WithAllowUntrustedCertificates()
            .WithClientCertificates(certificates)
            .Build(); // Build the MqttClientTlsOptions

        // Setup MQTT Client Options
        Debug.Log("Build MQTT Client Options");
        var mqttClientOptions = new MqttClientOptionsBuilder()
            .WithTlsOptions(mqttClientTlsOptions)
            .WithTcpServer(BrokerHostname, 8883) // Setup Server Connection
            .Build(); // Build the MqttClientOptions

        // Connect to Broker
        Debug.Log("Connecting to MQTT Broker...");
        // This will throw an exception if the server is not available.
        // The result from this message returns additional data which was sent 
        // from the server. Please refer to the MQTT protocol specification for details.
        var response = await mqttClient.ConnectAsync(mqttClientOptions, CancellationToken.None);
        Debug.Log("The MQTT client is connected.");

        // return response;
    }
}