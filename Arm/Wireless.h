#ifndef WIRELESS_h
#define WIRELESS_h

// #include "Arduino.h"
#include "WiFi.h"

class Wireless {
    private:
        WiFiClient client;

    public:
        Wireless(IPAddress& ipAddr, IPAddress& dns, IPAddress& gateway, IPAddress& subnet);
        Wireless();

        /**
          Get the IP Address of the Ether Object
          Returns:
            IPAddress object
        */
        IPAddress getIpAddr();

        /**
          Get the MAC Address of the Ether Object
          Returns:
            byte* mac or char* mac
        */
        void getMacAddr(byte* mac);
        void getMacAddr(char* mac);

        /** 
          Connect to the internet through the wireless interface
          Inputs:
            char* ssid: The Network name of the Wifi you want to connect to
            char* password: The password of the Wifi you want to connect to
          Returns:
            true: When connection is successful
            false: When connection fails
        */
        bool begin(char* ssid, char* password);

        /**
          Connect to a specific webpage or application
          Inputs:
            IPAddress ipAddr: The IP address of the application you want to connect to
            int port: The port number of the application to connect to
          Returns:
            true: When connection is successful
            false: When connection fails
        */
        bool connect(IPAddress& ipAddr, int port);

        /**
          Disconnects from the Wifi Network
          Returns:
            true: When connection is successful
            false: When connection fails
        */
        bool disconnect();
};

#endif