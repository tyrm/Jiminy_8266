#define LED_PIN   0

int CountPipes(byte* payload, unsigned int length) {
  int count = 0;
  
  for (int i=0; i<length; i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '|') {
      count += 1;
    }
  }
  
  return count;
}

String FormatHex8(uint8_t data) {
  String value = "";
  if (data<0x10) {value = value + "0";} 
  value = value + String(data, HEX);

  return value;
}

String GetName() {
  byte mac[6];
  WiFi.macAddress(mac);

  return String(FormatHex8(mac[5]) + FormatHex8(mac[4]) + FormatHex8(mac[3]) + FormatHex8(mac[2]) + FormatHex8(mac[1]) + FormatHex8(mac[0]));
}

String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

void ReadConfig() {
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
  uint8_t r_red = EEPROM.read(6);
  uint8_t r_green = EEPROM.read(7);
  uint8_t r_blue = EEPROM.read(8);
  uint8_t r_white = EEPROM.read(9);
  uint8_t r_brightness = EEPROM.read(10);
  
  Serial.print(" Starting Color: #");
  Serial.print(FormatHex8(r_red));
  Serial.print(FormatHex8(r_green));
  Serial.print(FormatHex8(r_blue));
  Serial.println(FormatHex8(r_white));
  
  Serial.print(" Starting Brightness: ");
  Serial.println(r_brightness, DEC);
  
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

    WMulti.addAP(r_ssid, r_password);
  }
  
  // Username
  ee_len = EEPROM.read(ee_cursor);
  ee_cursor++;
  
  char r_username[ee_len];
  for (int i = 0; i < ee_len; i++) {
    r_username[i] = char(EEPROM.read(ee_cursor));
    ee_cursor++;
  }
  MQTTUsername = r_username;
  
  VerifyBreakChar(ee_cursor);
  ee_cursor++;
  
  // Password
  ee_len = EEPROM.read(ee_cursor);
  ee_cursor++;
  
  char r_password[ee_len];
  for (int i = 0; i < ee_len; i++) {
    r_password[i] = char(EEPROM.read(ee_cursor));
    ee_cursor++;
  }
  MQTTPassword = r_password;
  
  VerifyBreakChar(ee_cursor);
  ee_cursor++;

  // [11-14] mqtt_server_ip
  uint8_t IP1 = EEPROM.read(11);
  uint8_t IP2 = EEPROM.read(12);
  uint8_t IP3 = EEPROM.read(13);
  uint8_t IP4 = EEPROM.read(14);

  MQTTHost = IPAddress(IP1, IP2, IP3, IP4);

  // [15-16] mqtt_port
  low = EEPROM.read(15);
  high = EEPROM.read(16);
  uint8_t mqtt_port_bytes[] = {low, high};
  MQTTPort = *( uint16_t* ) mqtt_port_bytes;
  
  Serial.print("\n MQTT Server: ");
  Serial.print(MQTTHost);
  Serial.print(":");
  Serial.println(MQTTPort, DEC);
  
  Serial.print("  Login: ");
  Serial.print(MQTTUsername);
  Serial.print("/");
  Serial.println(MQTTPassword);

  Serial.println("Successfully Read Config...\n");
  EEPROM.end();

  InitPixels(LEDCount, r_red, r_green, r_blue, r_white, r_brightness);
}

void ToggleLED() {
  static boolean ledState = false;
  
  if (ledState) {
    digitalWrite(LED_PIN, HIGH);
    ledState = false;
  }
  else {
    digitalWrite(LED_PIN, LOW);
    ledState = true;
  }
}

void VerifyBreakChar(uint16_t location) {
  uint8_t Data = EEPROM.read(location);
  if (Data != 0x1D) {
    Serial.print("Config error: break not found at 0x");
    Serial.print(FormatHex8(location));
    Serial.print(". Expected 0x1D got 0x");
    Serial.println(FormatHex8(Data));
    while(true);
  }
}
