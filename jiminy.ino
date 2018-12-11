#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

// Globals
byte      ColorScheme[10][41];
uint16_t  LEDCount;
IPAddress MQTTHost;
uint16_t  MQTTPort;
String    MQTTUsername;
String    MQTTPassword;
String    MyName;

unsigned long LastRun = 0;
int  MyDelay = 100;
byte MyMode = 0;
// 0 - Static No Animation
// 1 - Fire

// Hardware Bits
ESP8266WiFiMulti WMulti;
WiFiClient       ESPClient;
PubSubClient     MQTTClient(ESPClient);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  
  ReadConfig();
  InitColorSchemes();

  // Wifi
  WiFi.mode(WIFI_STA);
  
  Serial.print("Connecting Wifi");
  while (WMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  MyName = GetName();
  
  MQTTClient.setServer(MQTTHost, MQTTPort);
  MQTTClient.setCallback(MQTTProcessPacket);
}

void loop() {
  if (WMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
  MQTTLoop();
  AniLoop();
}
