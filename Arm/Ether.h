#ifndef ETHER_h
#define ETHER_h

// #include "Arduino.h"
#include <SPI.h>
#include "EthernetENC.h"

class Ether {
    private:
        IPAddress ipAddr;
        byte* mac;
        // EthernetClient client;

    public:
        Ether(IPAddress& ipAddr, byte* mac);

        IPAddress& getIpAddr();
        byte* getMac();

        bool begin();
        bool connect(IPAddress& ipAddr, int port);
        // TODO: Add in Ethernet Client Actions (May not be needed for our purpose)
};

#endif