#include "Ether.h"

// Constructor definition
Ether::Ether(IPAddress& ipAddr, byte* mac) 
            : ipAddr(ipAddr) 
{
    this->ipAddr = ipAddr;
    this->mac = mac;

    Ethernet.init(34);
}

IPAddress& Ether::getIpAddr() {
    return this->ipAddr;
}

byte* Ether::getMac() {
    return this->mac;
}

bool Ether::begin() {
    Serial.println("<ETHER>: Connecting to Ethernet...");

    // TODO: I think LinkOFF is when there is nothing plugged in at all so
    if (Ethernet.linkStatus() != LinkON) {
        Serial.println("<ETHER>: Link status: Off");
        Serial.println("<ETHER>: Ethernet is not connected");
        return false;
    }
 
    while (Ethernet.linkStatus() != LinkON) {
        if (Ethernet.begin(this->mac) == 0) {
            Serial.print("<ETHER>: Failed to configure Ethernet using DHCP...");
            Serial.print(" Attempting connection with IP Address ");
            Serial.println(this->ipAddr);

            Ethernet.begin(this->mac, this->ipAddr);
        }

        // Delay for 2 seconds for startup
        delay(2000);

        if (Ethernet.linkStatus() == Unknown) {
            Serial.println("<ETHER>: Link status unknown. Link status detection is only available with W5200 and W5500.");
            return false;
        }
        else if (Ethernet.linkStatus() == LinkON) {
            Serial.println("<ETHER>: Link status: On");
            Serial.println("<ETHER>: Ethernet is connected");
            
            Serial.print("<ETHER>: IP Address: ");
            Serial.println(this->ipAddr);
            Serial.print("<ETHER>: MAC Address: ");
            for (int i = 0; i < 5; i++) {
              Serial.print(this->mac[i]);
              Serial.print(":");
            }
            Serial.println(this->mac[5]);
        }
        else if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println("<ETHER>: Link status: Off");
            Serial.println("<ETHER>: Ethernet is not connected");
            return false;
        }
        else {
            Serial.print("<ETHER>: Link Status Return Code: ");
            Serial.println(Ethernet.linkStatus());
            return false;
        }
    }

    return true;
}

bool Ether::connect(IPAddress& ipAddr, int port) {
    Serial.println("TODO: Implement Ether.connect()");
}
