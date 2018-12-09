#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

// Globals
byte     ColorScheme[10][41];
uint16_t LEDCount;
String   MyName;
String   MQTTHost;
uint16_t MQTTPort;
String   MQTTUsername;
String   MQTTPassword;

// Hardware Bits
ESP8266WiFiMulti WMulti;
WiFiClient       ESPClient;
PubSubClient     MQTTClient(ESPClient);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  
  ReadConfig();

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

  // MQTT
  int bufferSize = MQTTHost.length() + 1;
  char hostBuffer[bufferSize];
  MQTTHost.toCharArray(hostBuffer, bufferSize);
  
  Serial.print("Connecting to: ");
  Serial.println(hostBuffer);
  
  MQTTClient.setServer("MQTTHost", MQTTPort);
  //MQTTClient.setCallback(processPacket);
}

void loop() {
  if (WMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
  MQTTLoop();

}
