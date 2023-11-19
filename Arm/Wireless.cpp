
#include "Wireless.h"

Wireless::Wireless(IPAddress& ipAddr, IPAddress& dns, IPAddress& gateway, IPAddress& subnet) {
  WiFi.config(ipAddr, dns, gateway, subnet);
}
Wireless::Wireless() {}

IPAddress Wireless::getIpAddr() {
  return WiFi.localIP();
}

bool Wireless::begin(char* ssid, char* password) {
  int loop = 0;
  Serial.printf("<Wireless>: Attempting to connect to %s...", ssid);

  do {
    if (loop > 5) {
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