using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using UnityEngine;
// using System.Text.Json;

using MQTTnet;
using MQTTnet.Protocol;
using MQTTnet.Client;
using MQTTnet.Formatter;

public struct MQTT_Position
{
    public float x;
    public float y;
}

/**
 * Class for handling different MQTT Messages
 */
public static class MQTT_Callbacks
{
    /**
     * Handles Position messages
     * This function will update the position values of the scalple within the simulation
     */
    public static async Task OnPosition(MqttApplicationMessageReceivedEventArgs eventArgs) {
        if (eventArgs.ApplicationMessage.Topic.Contains("position")) {
            string rawPayload = eventArgs.ApplicationMessage.ConvertPayloadToString();
            Debug.Log("Received 'Position' message: " + rawPayload);

            // Convert JSON string to Object
            MQTT_Position jsonUtility = JsonUtility.FromJson<MQTT_Position>(rawPayload);
            // Debug.Log(jsonUtility.x);

            TheScript.Instance.MQTT_Update(jsonUtility.x, jsonUtility.y);
        }
    }
}