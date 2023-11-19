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

        IPAddress getIpAddr();

        bool begin(char* ssid, char* password);
        bool connect(IPAddress& ipAddr, int port);
        bool disconnect();
        // TODO: Add in Ethernet Client Actions (May not be needed for our purpose)
};

#endif