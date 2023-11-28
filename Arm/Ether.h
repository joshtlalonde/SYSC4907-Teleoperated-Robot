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

        /**
          Get the IP Address of the Ether Object
          Returns:
            IPAddress object
        */
        IPAddress& getIpAddr();

        /**
          Get the MAC Address of the Ether Object
          Returns:
            Byte[]: {XX, XX, XX, XX, XX, XX}
        */
        byte* getMac();

        /** 
          Connect to the internet through the Ethernet Port
          Returns:
            true: When connection is successful
            false: When connection fails
        */
        bool begin();
        bool connect(IPAddress& ipAddr, int port);
        // TODO: Add in Ethernet Client Actions (May not be needed for our purpose)
};

#endif