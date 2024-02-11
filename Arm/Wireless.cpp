
#include "Wireless.h"

Wireless::Wireless(IPAddress& ipAddr, IPAddress& dns, IPAddress& gateway, IPAddress& subnet) {
  WiFi.config(ipAddr, dns, gateway, subnet);
}
Wireless::Wireless() {}

IPAddress Wireless::getIpAddr() {
  return WiFi.localIP();
}

void Wireless::getMacAddr(byte* mac) {
  WiFi.macAddress(mac);
}

void Wireless::getMacAddr(char* mac) {
  byte mac_byte[6];

  WiFi.macAddress(mac_byte);

  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
         mac_byte[0], mac_byte[1], mac_byte[2],
         mac_byte[3], mac_byte[4], mac_byte[5]);
}

bool Wireless::begin(char* ssid, char* password) {
  int loop = 0;
  Serial.printf("<Wireless>: Attempting to connect to %s...", ssid);

  do {
    if (loop > 10) {
      Serial.println(" FAILED TO CONNECT");
      return false;
    }
    Serial.print(".");

    WiFi.begin(ssid, password);
    
    loop++;
    delay(1000);
  } while (WiFi.status() != WL_CONNECTED);
  Serial.println(" Connected");

  return true;
}

bool Wireless::connect(IPAddress& ipAddr, int port) {
  int loop = 0;
  Serial.printf("<Wireless>: Wirelessly connecting to %s:%d...", ipAddr.toString(), port);

  do {
    if (loop > 5) {
      Serial.println(" FAILED TO CONNECT");
      return false;
    }
    Serial.print(".");

    this->client.connect(ipAddr, port);
    
    loop++;
    delay(1000);
  } while (!this->client.connected());
  Serial.println(" Connected");

  return true;
}

bool Wireless::disconnect() {
  WiFi.disconnect();
}