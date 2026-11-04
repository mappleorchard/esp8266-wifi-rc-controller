#include <WiFiS3.h>
#include <WiFiUdp.h>

const char* ssid = "ESP8266_AP";
const char* pass = "password123";

IPAddress serverIP(192, 168, 4, 1);  // ESP8266 AP IP
unsigned int localPort = 8765;       // R4 listens on this port
unsigned int targetPort = 5678;      // ESP8266 listens on this port

WiFiUDP Udp;
char packetBuffer[255];  // Buffer for incoming packets
char replyBuffer[32];

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  Serial.print("Connecting to AP: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    analogWrite(13, 10);
    Serial.print(".");
  }
  digitalWrite(13, 1);
  delay(300);
  digitalWrite(13, 0);
  delay(300);
  digitalWrite(13, 1);
  delay(300);
  digitalWrite(13, 0);
  delay(300);
  Serial.println("\nConnected!");
  Serial.print("\nIP Address: ");
  // Print the local IP address
  Serial.println(WiFi.localIP());
  Serial.println("192.168.4.2");
  // Bind to local port to receive data
  Udp.begin(localPort);
  Serial.print("Listening on UDP port: ");
  Serial.println(localPort);
  Serial.print("Sending on UDP port: ");
  Serial.println(targetPort);
}

void loop() {
  // --- 1. SEND DATA TO ESP8266 ---
  int mac = WiFi.localIP();
  UDP_send(mac);

  // --- 2. CHECK FOR INCOMING DATA FROM ESP8266 ---
  UDP_recieve();
  digitalWrite(13, UDP_recieve());
}
void UDP_send(int message) {  //sends udp int to serverIP and targetPort
  Udp.beginPacket(serverIP, targetPort);
  Udp.print(message);
  Udp.endPacket();
  Serial.print("Sent to ESP: ");
  Serial.println(message);
}
int UDP_recieve() {  // gets udp message from localPortand stores it in packetBuffer
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;  // Null-terminate string
    }
    Serial.print("Received from ESP: ");
    Serial.println(packetBuffer);
    
    return atoi(packetBuffer);
  }
}