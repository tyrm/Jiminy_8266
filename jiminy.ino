#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Globals
uint16_t LEDCount;

uint8_t Red;
uint8_t Blue;
uint8_t Green;
uint8_t White;

uint8_t Brightness;

// Hardware Bits
ESP8266WiFiMulti WMulti;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  
  EEPROM.begin(512);
  
  // ** Confirm Header
  bool Failure = false;
  uint8_t ExpectedHeader[] = {0x4A, 0x49, 0x4D, 0x01};
  for (int i = 0; i < 4; i++) {
    uint8_t HeaderFrame = EEPROM.read(i);
    if (HeaderFrame != ExpectedHeader[i])
      Failure = true;
  }
  if (Failure) {
    Serial.println("Invalid header");
    while(true);
  }
  else {
    Serial.println("Jiminy header v1 found\nReading Config...");
  }

  // ** Reading Config **
  uint16_t ee_cursor = 64;
  uint8_t ee_len = 0;
  
  // [0-1] LED Count
  uint8_t low = EEPROM.read(4);
  uint8_t high = EEPROM.read(5);
  uint8_t count_bytes[] = {low, high};
  LEDCount = *( uint16_t* ) count_bytes;
  
  Serial.print(" LED Count: ");
  Serial.println(LEDCount, DEC);

  // Colors
  Red = EEPROM.read(6);
  Green = EEPROM.read(7);
  Blue = EEPROM.read(8);
  White = EEPROM.read(9);
  Brightness = EEPROM.read(10);
  
  Serial.print(" Starting Color: #");
  Serial.print(FormatHex8(Red));
  Serial.print(FormatHex8(Green));
  Serial.print(FormatHex8(Blue));
  Serial.println(FormatHex8(White));
  
  Serial.print(" Starting Brightness: ");
  Serial.println(Brightness, DEC);
  
  // Wifi
  WiFi.mode(WIFI_STA);

  VerifyBreakChar(ee_cursor);
  ee_cursor++;
  
  uint8_t SSIDCount = EEPROM.read(ee_cursor);
  ee_cursor++;

  VerifyBreakChar(ee_cursor);
  ee_cursor++;

  Serial.print("\n Found ");
  Serial.print(SSIDCount, DEC);
  Serial.println(" wifi passwords.");

  for (int i = 0; i < SSIDCount; i++) {
    // SSID
    ee_len = EEPROM.read(ee_cursor);
    ee_cursor++;
    
    char r_ssid[ee_len+1];
    for (int i = 0; i < ee_len; i++) {
      r_ssid[i] = char(EEPROM.read(ee_cursor));
      ee_cursor++;
    }
    r_ssid[ee_len] = 0x00;
    
    VerifyBreakChar(ee_cursor);
    ee_cursor++;
    
    // Password
    ee_len = EEPROM.read(ee_cursor);
    ee_cursor++;
  
    char r_password[ee_len+1];
    for (int i = 0; i < ee_len; i++) {
      r_password[i] = char(EEPROM.read(ee_cursor));
      ee_cursor++;
    }
    r_password[ee_len] = 0x00;
    
    VerifyBreakChar(ee_cursor);
    ee_cursor++;
    
    //Serial.print("  SSID[");
    //Serial.print(r_ssid);
    //Serial.print("] Password[");
    //Serial.print(r_password);
    //Serial.println("]");

    WMulti.addAP(r_ssid, r_password);
  }
  
  Serial.print(" Connecting Wifi");
  while (WMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n WiFi connected");
  Serial.print(" IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT Server Config
  // Host
  ee_len = EEPROM.read(ee_cursor);
  ee_cursor++;
  
  char MQTTHost[ee_len+1];
  for (int i = 0; i < ee_len; i++) {
    MQTTHost[i] = char(EEPROM.read(ee_cursor));
    ee_cursor++;
  }
  MQTTHost[ee_len] = 0x00;
  
  VerifyBreakChar(ee_cursor);
  ee_cursor++;
  
  // Username
  ee_len = EEPROM.read(ee_cursor);
  ee_cursor++;
  
  char MQTTUsername[ee_len+1];
  for (int i = 0; i < ee_len; i++) {
    MQTTUsername[i] = char(EEPROM.read(ee_cursor));
    ee_cursor++;
  }
  MQTTUsername[ee_len] = 0x00;
  
  VerifyBreakChar(ee_cursor);
  ee_cursor++;
  
  // Password
  ee_len = EEPROM.read(ee_cursor);
  ee_cursor++;
  
  char MQTTPassword[ee_len+1];
  for (int i = 0; i < ee_len; i++) {
    MQTTPassword[i] = char(EEPROM.read(ee_cursor));
    ee_cursor++;
  }
  MQTTPassword[ee_len] = 0x00;
  
  VerifyBreakChar(ee_cursor);
  ee_cursor++;

  // [11-12] mqtt_port
  low = EEPROM.read(11);
  high = EEPROM.read(12);
  uint8_t mqtt_port_bytes[] = {low, high};
  uint16_t MQTTPort = *( uint16_t* ) mqtt_port_bytes;
  
  Serial.print("\n MQTT Server: ");
  Serial.print(MQTTHost);
  Serial.print(":");
  Serial.println(MQTTPort, DEC);
  
  Serial.print("  Login: ");
  Serial.print(MQTTUsername);
  Serial.print("/");
  Serial.println(MQTTPassword);

  Serial.println("Successfully Read Config...");
  EEPROM.end();

}

void loop() {
  if (WMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
  // put your main code here, to run repeatedly:

}
