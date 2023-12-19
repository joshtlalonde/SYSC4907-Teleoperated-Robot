/* TO READ:
 Before attempting to understand this script, the following will really help out:
    1) Familiarize yourselves with Unity, specifically take the Unity Essentials and Junior Programmer Learning Pathways on Unity Learn (https://learn.unity.com/)
        - do not be intimidated by the long estimated times to complete, we were able to complete the entire Junior Programmer pathway over a few days 
    2) Familiarize yourselves with the way we setup our Input Manager (Edit > Project Settings > Input Manager), additional inputs were created to support 6DoF
        and existing inputs were bound to specific keys on the keyboard 
    3) Familiarize yourselves with the 'Tags' we created, these are used to identify what object the scalpel collided with, the Junior Programmer pathway goes over the use of Tags
    4) Familiarize yourselves with UDP, specifically:
        -how to find an available port (we used netstat -ano in the command prompt of the PC)
        -the flow of data between a client and server and what variable types are supported
        -how to setup a port, how to send and read data and how to convert between variable types
*/

//These are all relevant header files needed for this script

using System;
using System.Text; 
using System.Net;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Globalization;
using System.Net.Sockets;
using UnityEngine;

public class TheScript : MonoBehaviour
{
    private Rigidbody playerRb;
    public ParticleSystem blood;

    //Movement inputs for 2DoF
    Vector3 movementInput;

    Vector3 movementForce;

    private List<Vector3> scalpelPositions = new List<Vector3>();


    private float chestDepth = 0f; // Variable to track how deep the scalpel is in the chest
    private float maxDepth = 3f; // Adjust the maximum depth as needed

    //Speed and scalpel contact variables 
    public float speed = 0.3f;
    public float noScalpelContactSpeed = 0.6f; // Speed of scalpel when there is no contact
    public float scalpelContactSpeed = 0.05f;  // Speed of scalpel when there is contact
    public float boneContactSpeed = 0.02f;     // Speed of scalpel when there is bone contact

    //initial scalpel position and rotation variables
    private float initPosX = 0; 
    private float initPosY = 1.12f;
    private float initPosZ = -0.05f; //Adjust z position to make scalpel more centred

    //Environment boundaries
    private float bound = 0.1f;
    private float boundZ = 0.15f; //bound for forward and back movement

    //UDP variables
    private string IP_address = "192.168.101.249"; //IP address of the Arduino Nano
    private int udpPort = 1556; //Available UDP port 
    private string caseContactNumber = "1"; // Case 1: No Contact | Case 2: Chest Contact | Case 3: Bone/Patient Gown Contact

    async void Start()
    {
        playerRb = GetComponent<Rigidbody>();
        blood.Stop();

        Debug.Log("I am alive!");  
        // Client_Connection_Samples mqttClient = new Client_Connection_Samples();
        
        var myMqttClient = new TestMQTT("SomeID");
        // await mqttClient.Connect_Client();

        try
        {
            await myMqttClient.Connect_Client("LAPTOP-HL4N9U5Q", "./Assets/Scripts/certs/ca-root-cert.crt");
        }
        catch (Exception ex)
        {
            Debug.Log($"An error occurred: {ex.Message}");
        }
    }

    void Update()
    {
        float h = Input.GetAxisRaw("Mouse X"); // Movement of the mouse in the Y direction
        float u = Input.GetAxisRaw("Mouse Y"); // Movement of the mouse in the X direction


        movementInput = new Vector3(0f, u, h);
        scalpelPositions.Add(transform.position);

        if (scalpelPositions.Count > 1000)
        {
            scalpelPositions.RemoveAt(0);
        }

        chestDepth = Mathf.Clamp(initPosY - transform.position.y, 0f, maxDepth);


        // reset the scalpel's original position and orientation when the user presses C key
        if (Input.GetKeyDown(KeyCode.C)) 
        {
            transform.position = new Vector3(initPosX, initPosY, initPosZ);
        }
  
        ScalpelMovement();
        Bounds();
        PrintToScreen();
        TransmitOverUDP(caseContactNumber);
    }


    private void OnTriggerEnter(Collider other)
    {

        //if the scalpel collides with the patient's exposed chest
        if (other.gameObject.CompareTag("Chest"))
        {
            caseContactNumber = "2";
            blood.Play();
            playerRb.constraints = RigidbodyConstraints.FreezePositionX; //constrain scalpel movement upon collision
            speed = scalpelContactSpeed; //reduce scalpel speed to mimic cutting
        }

        //if the scalpel collides with bone or the patient gown
        else if (other.gameObject.CompareTag("Bone"))
        {
            caseContactNumber = "3";
            playerRb.constraints = RigidbodyConstraints.FreezePositionX;
            speed = boneContactSpeed;
            blood.Stop();
        }

    	else
    	{
            caseContactNumber = "1";
            playerRb.constraints = RigidbodyConstraints.None;
            speed = noScalpelContactSpeed; // Set regular speed for case 1
	        blood.Stop();
        }
    
    }

    //A function to control the 2DoF movement of the scalpel
    void ScalpelMovement()
    {

        if (IsInContactWithTag("Chest"))
        {
                blood.Play();
                caseContactNumber = "2";
                playerRb.constraints = RigidbodyConstraints.FreezePositionX; //constrain scalpel movement upon collision
                speed = scalpelContactSpeed; //reduce scalpel speed to mimic cutting
        } 


        else if (IsInContactWithTag("Bone"))
            {
                caseContactNumber = "3";
                playerRb.constraints = RigidbodyConstraints.FreezePositionX;
                speed = boneContactSpeed;
                blood.Stop();
            }


        else 
            {
                blood.Stop(); // Stop the blood simulation
                caseContactNumber = "1";
                playerRb.constraints = RigidbodyConstraints.None;
                speed = noScalpelContactSpeed; // Set regular speed for case 1
            }

        // Reset constraints when transitioning from case 2
        if (caseContactNumber == "2" && playerRb.constraints != RigidbodyConstraints.None)
        {
            playerRb.constraints = RigidbodyConstraints.None;
        }



        bool IsInContactWithTag(string tag)
        {
            Collider[] colliders = Physics.OverlapSphere(transform.position, 0.1f); // Adjust the radius as needed

            foreach (Collider collider in colliders)
            {
                if (collider.CompareTag(tag))
                {
                    return true;
                }
            }

            return false;
        }


        // Apply force based on the contact case
        switch (caseContactNumber)
        {
            case "2":
                // Check if the scalpel is within a certain depth range to prevent continuous forces
                if (chestDepth < maxDepth)
                {
                    // Gradually raise the scalpel in the Y-axis
                    float riseSpeed = 0.01f + chestDepth * 0.1f; // Adjust the rise speed according to your preference
                    Vector3 riseForce = Vector3.down * riseSpeed;
                    playerRb.AddRelativeForce(riseForce);
                    Debug.Log("Rise force applied: " + riseForce.magnitude);
            blood.Play();
                }
                break;

            case "3":
                // Apply force for bone contact
                movementForce = movementInput * speed;
                playerRb.AddRelativeForce(movementForce);
                break;

        case "1": 
            // Apply force for case 1
                movementForce = movementInput * speed;
                playerRb.AddRelativeForce(movementForce);
                break;

            default:
                // Apply force for other cases
                movementForce = movementInput * speed;
                playerRb.AddRelativeForce(movementForce);
            break;
        }


        // Print the force magnitude to the console
        Debug.Log("Force applied: " + movementForce.magnitude);

        GetComponent<Rigidbody>().AddRelativeForce(movementInput * speed);


    }


    //A function which sets the bounds of the environment
    void Bounds()
    {
        if (transform.position.x < initPosX - bound)
        {
            transform.position = new Vector3((initPosX - bound), transform.position.y, transform.position.z);
            playerRb.velocity = Vector3.zero;
        }
        if (transform.position.x > initPosX + bound)
        {
            transform.position = new Vector3((initPosX + bound), transform.position.y, transform.position.z);
            playerRb.velocity = Vector3.zero;
        }
        if (transform.position.z < initPosZ - boundZ)
        {
            transform.position = new Vector3(transform.position.x, transform.position.y, (initPosZ - boundZ));
            playerRb.velocity = Vector3.zero;
        }
        if (transform.position.z > initPosZ + boundZ)
        {
            transform.position = new Vector3(transform.position.x, transform.position.y, (initPosZ + boundZ));
            playerRb.velocity = Vector3.zero;
        }
        if (transform.position.y < initPosY - bound)
        {
            transform.position = new Vector3(transform.position.x, (initPosY - bound), transform.position.z);
            playerRb.velocity = Vector3.zero;
        }
        if (transform.position.y > initPosY + bound)
        {
            transform.position = new Vector3(transform.position.x,(initPosY + bound), transform.position.z);
            playerRb.velocity = Vector3.zero;
        }
    }
    
    //A function to print the contact case number to the console, this is helpful for debugging but can be omitted
    void PrintToScreen()
    {
        Debug.Log("case " + caseContactNumber);
    }

    //A function to send the caseContactNumber over UDP to the Arduino Nano microcontroller, which will be used to control the linear actuators and the vibrational motor
    private void TransmitOverUDP(String dataToSend)
    {
        //create the socket
        Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

        //connect to the socket
        socket.Connect(IP_address, udpPort);

        //convert the data from string into a character array to prepare it to be sent
        byte[] sendBytes = Encoding.ASCII.GetBytes(dataToSend);
        
        //Send the character array over UDP
        socket.Send(sendBytes);
    }
}

