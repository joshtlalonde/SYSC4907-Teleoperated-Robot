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
using System.Diagnostics;
using UnityEngine;
using Debug = UnityEngine.Debug;

public class TheScript : MonoBehaviour
{
    public static TheScript Instance;

    private Rigidbody playerRb;
    public ParticleSystem blood;

    //Movement inputs for 2DoF
    Vector3 movementInput;

    Vector3 movementForce;

    Vector3 prevMovementForce;

    private List<Vector3> scalpelPositions = new List<Vector3>();


    private float chestDepth = 0f; // Variable to track how deep the scalpel is in the chest
    private float maxDepth = 3f; // Adjust the maximum depth as needed

    //Speed and scalpel contact variables 
    public float speed = 0.3f;


    //initial scalpel position and rotation variables
    private float initPosX = 0; 
    private float initPosY = 1.17f;
    private float initPosZ = -0.25f; //Adjust z position to make scalpel more centred

    // Scaple Position Change
    private float posX;
    private float posY;

    //Environment boundaries
    private float bound = 0.13f;
    private float boundZ = 0.15f; //bound for forward and back movement


    private string caseContactNumber = "1"; // Case 1: No Contact | Case 2: Chest Contact | Case 3: Bone/Patient Gown Contact

    private bool mqttFlag = false; // Flag Used to determine if position is being updated from MQTT or the mouse

    private MQTT_Client mqttClient; // Object used to define the MQTT client

    private Stopwatch stopwatch; // Clock that keeps track of time
    private float mqttTimer = 0.25f; // Interval to send MQTT Force messages (250ms)

    // Function needed to pass object to MQTT Instance 
    private void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
        else if (Instance != this)
        {
            Destroy(gameObject);
        }
    }

    /** 
     * Updates the X and Y position of the Scaple
     * ISSUE: The issue with this is that this value is used to update the relative force to move the scaple in a direction
     *        So how do we convert the {x, y} coord value to just move towards that position
     *        So this value is the relative value on how far to move. So we would need to keep track of the current position
     *        and then update the position to just be the relative difference
     * We will need to actually test the arm to see how this works but this is what I ended up doing:
     *   We use the difference in position change and that gets turned into the force so if there is no change it does not move
     */
    public void MQTT_Update(float posX, float posY) {
        Debug.Log("Update Position: {" + posX + ", " + posY + "}");
        this.posX = posX;
        this.posY = posY;

        // mqttFlag = true;
    }

    async void Start()
    {
        playerRb = GetComponent<Rigidbody>();
        blood.Stop();
        
        // Set Default location of scaple
        transform.position = new Vector3(initPosX, initPosY, initPosZ);

        // Initialize MQTT Client
        mqttClient = new MQTT_Client("virt");

        // Connect MQTT Client and Subscribe to Position Topic
        try {
            await mqttClient.Connect("LAPTOP-HL4N9U5Q", "./Assets/Scripts/certs/ca-root-cert.crt");
            await mqttClient.Subscribe("position/#", 2);
        } catch (Exception ex) {
            Debug.Log($"An error occurred: {ex.Message}");
        }

        // Initialize Stopwatch to only Publish every certain ms
        stopwatch = Stopwatch.StartNew();
    }

    void FixedUpdate()
    {
        // if (!mqttFlag) {
        //     posX = Input.GetAxisRaw("Mouse X"); // Movement of the mouse in the Y direction
        //     posY = Input.GetAxisRaw("Mouse Y"); // Movement of the mouse in the X direction
        // }

        movementInput = new Vector3(posX, posY, 0f);
        
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

        Debug.Log("Mouse X: " + posX + ", Mouse Y: " + posY + ", movementInput: " + movementInput);
  
        ScalpelMovement();
        Bounds();
        PrintToScreen();
    }

    private void OnTriggerEnter(Collider other)
    {

        //if the scalpel collides with the patient's exposed chest
        //if (other.gameObject.CompareTag("Chest"))
	if (scalpelPosition.y < 1.143977f && scalpelPosition.z > -0.3389f && scalpelPosition.z < -0.2487f)
        {
	
            caseContactNumber = "2";
            playerRb.constraints = RigidbodyConstraints.FreezePositionX; //constrain scalpel movement upon collision
         
        }

        //if the scalpel collides with bone or the patient gown
        //else if (other.gameObject.CompareTag("Bone"))
	else if (scalpelPosition.y < 1.143977f && scalpelPosition.z < -0.3389f && scalpelPosition.z > -0.2487f)
        {
            caseContactNumber = "3";
            playerRb.constraints = RigidbodyConstraints.FreezePositionX;
        }

        else
        {
            caseContactNumber = "1";
            playerRb.constraints = RigidbodyConstraints.None;
        }

    }

    //A function to control the 2DoF movement of the scalpel
    async     void ScalpelMovement()
    {


        bool isInChestContact = IsInContactWithTag("Chest");
        bool isInBoneContact = IsInContactWithTag("Bone");

        if (scalpelPosition.y < 1.143977f)
        {
	    if (scalpelPosition.z > -0.3389f && scalpelPosition.z < -0.2187f)
	    {
            caseContactNumber = "2";
	    }
	    else
	    { 
	     caseContactNumber = "3";
	    }
        }


        else
        {
            caseContactNumber = "1";
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
                    Debug.Log($"Scalpel is in contact with tag: {tag}");
                    return true;
                }
            }

            return false;
        }


        // Apply force based on the contact case
switch (caseContactNumber)
{
case "3":
            // Check if the scalpel is within a certain depth range to prevent continuous forces
            movementForce = movementInput * scalpelSpeed * 500; 
            break;


case "2":
            // Apply force based on the provided equation for case 2
	    movementForce = movementInput * scalpelSpeed * 200; 
            break;



case "1":

        default:
            playerRb.AddRelativeForce(movementForce);
            break;
}


            Debug.Log("Force applied: " + movementForce.magnitude);


        GetComponent<Rigidbody>().AddRelativeForce(movementInput * speed);



        // Print the force magnitude to the console
        Debug.Log("Force applied: " + movementForce.magnitude);

        // GetComponent<Rigidbody>().AddRelativeForce(movementInput * speed);

        /** Publish the MQTT Force Magnitude to apply to Physical Arm */
        if (stopwatch.Elapsed.TotalSeconds >= mqttTimer)
        {
            if (!(prevMovementForce.x == movementForce.x && prevMovementForce.y == movementForce.y)) {
                if (caseContactNumber == "2" || caseContactNumber == "3") {
                    try {
                        MQTT_Force mqttForce = new MQTT_Force { x = movementForce.x, y = movementForce.y };
                        await mqttClient.Publish("force", JsonUtility.ToJson(mqttForce));
                    } catch (Exception ex) {
                        Debug.Log($"An error occurred: {ex.Message}");
                    }
                } else {
                    try {
                        MQTT_Force mqttForce = new MQTT_Force { x = 0, y = 0 };
                        await mqttClient.Publish("force", JsonUtility.ToJson(mqttForce));
                    } catch (Exception ex) {
                        Debug.Log($"An error occurred: {ex.Message}");
                    }
                }

                prevMovementForce = movementForce;
            }


            // Reset the stopwatch
            stopwatch.Restart();
        }

        
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
}

