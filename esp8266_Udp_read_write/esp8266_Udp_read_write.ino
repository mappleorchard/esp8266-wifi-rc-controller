#include <OLEDDisplay.h>
#include "SSD1306Wire.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *apSSID = "ESP8266_AP";
const char *apPassword = "password123";
unsigned int localPort = 5678;   // ESP8266 listens on this port
unsigned int targetPort = 8765;  // R4 listens on this port
IPAddress remoteIP;

SSD1306Wire display(0x3c, 14, 12);  // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins
WiFiUDP Udp;
char packetBuffer[255];  // Buffer for incoming packets
char replyBuffer[32];

void setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.println(apSSID);
  display.println(apPassword);
  WiFi.softAP(apSSID, apPassword);
  Serial.print("\nAP IP address: ");
  Serial.println(WiFi.softAPIP());
  display.drawString(10, 25, String(WiFi.softAPgetStationNum()) + " devices connected");
  if (WiFi.softAPgetStationNum() == 0) {
    while (WiFi.softAPgetStationNum() == 0) {
      delay(50);
    }
    display.drawString(10, 25, String(WiFi.softAPgetStationNum()) + " devices connected");
  }
  display.drawString(10, 25, String(WiFi.softAPgetStationNum()) + " devices connected");

  Udp.begin(localPort);
  Serial.print("Listening on UDP port: ");
  Serial.println(targetPort);
  Serial.print("Sending on UDP port: ");
  Serial.println(localPort);
}

void loop() {
  UDP_recieve();  //listen
  remoteIP = Udp.remoteIP();
  // --- 1. SENDS DATA TO ARDUINO ---
  UDP_send(digitalRead(0));
}
void UDP_send(int message) {  //sends udp int to serverIP and targetPort
  Udp.beginPacket(remoteIP, targetPort);
  Udp.print(message);
  Udp.endPacket();
  Serial.print("Sent to r4: ");
  Serial.println(message);
}
int UDP_recieve() {  // gets udp message from localPortand stores it in packetBuffer
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;  // Null-terminate string
    }
    Serial.print("Received from r4: ");
    Serial.println(packetBuffer);
  }
  return atoi(packetBuffer);
}