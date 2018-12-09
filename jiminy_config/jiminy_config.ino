#include <EEPROM.h>

// Static Sized Data
// [0-3] Header
// [4-5] LED Count
const uint16_t LEDCount = 46;
// [6]   start red
const uint8_t StartRed = 0;
// [7]   start green
const uint8_t StartGreen = 0;
// [8]   start blue
const uint8_t StartBlue = 0;
// [9]   start white
const uint8_t StartWhite = 255;
// [10]   start brigtness
const uint8_t StartBrightness = 100;
// [11-14] mqtt_server_ip
const uint8_t IP1 = 192;
const uint8_t IP2 = 168;
const uint8_t IP3 = 1;
const uint8_t IP4 = 100;
// [15-16] mqtt_port
const uint16_t MQTTPort = 1883;

// Dynamic Sized Data
// wifi password pairs
char *wifi[][2] = {
  {"ssid1","password1"},
  {"ssid2","password2"},
  {"ssid3","P@55w0rd!E"}
};
// mqtt_server
const char     mqtt_username[]  = "jiminy";
const char     mqtt_password[]  = "password";

const char     break_char = 0x1D;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  EEPROM.begin(512);

  // Write Header
  EEPROM.write(0, 0x4A); // J
  Serial.print(".");
  EEPROM.write(1, 0x49); // I
  Serial.print(".");
  EEPROM.write(2, 0x4D); // M
  Serial.print(".");
  EEPROM.write(3, 0x01); // Version 1
  Serial.print(".");

  // Write LED Count
  EEPROM.write(4, lowByte(LEDCount));
  Serial.print(".");
  EEPROM.write(5, highByte(LEDCount));
  Serial.print(".");

  // Write Starting Colors
  EEPROM.write(6, StartRed);
  Serial.print(".");
  EEPROM.write(7, StartGreen);
  Serial.print(".");
  EEPROM.write(8, StartBlue);
  Serial.print(".");
  EEPROM.write(9, StartWhite);
  Serial.print(".");
  EEPROM.write(10, StartBrightness);
  Serial.print(".");

  // Write IP
  EEPROM.write(11, IP1);
  Serial.print(".");
  EEPROM.write(12, IP2);
  Serial.print(".");
  EEPROM.write(13, IP3);
  Serial.print(".");
  EEPROM.write(14, IP4);
  Serial.print(".");

  // Write MQTT
  EEPROM.write(15, lowByte(MQTTPort));
  Serial.print(".");
  EEPROM.write(16, highByte(MQTTPort));
  Serial.print(".");

   // ** Write Variables with Dynamic Locations**
  uint16_t ee_cursor = 64;
  uint8_t  ee_len = 0;

  // Start with break_char
  EEPROM.write(ee_cursor, break_char);
  ee_cursor++;
  Serial.print(".");

  // Write Wifi Count
  uint8_t wifi_count = sizeof(wifi)/sizeof(wifi[0]);
  EEPROM.write(ee_cursor, wifi_count);
  ee_cursor++;
  Serial.print(".");
  EEPROM.write(ee_cursor, break_char);
  ee_cursor++;
  Serial.print(".");

  for (int i = 0; i < wifi_count; i++) {

    String ssid = wifi[i][0];
    EEPROM.write(ee_cursor, ssid.length());
    ee_cursor++;
    Serial.print(".");
    for (int j = 0; j < ssid.length(); j++) {
      EEPROM.write(ee_cursor, wifi[i][0][j]);
      ee_cursor++;
      Serial.print(".");
    }

    EEPROM.write(ee_cursor, break_char);
    ee_cursor++;
    Serial.print(".");

    String password = wifi[i][1];
    EEPROM.write(ee_cursor, password.length());
    ee_cursor++;
    Serial.print(".");
    for (int j = 0; j < password.length(); j++) {
      EEPROM.write(ee_cursor, wifi[i][1][j]);
      ee_cursor++;
      Serial.print(".");
    }

    EEPROM.write(ee_cursor, break_char);
    ee_cursor++;
    Serial.print(".");
  }

  // Write server username
  ee_len = sizeof(mqtt_username);
  EEPROM.write(ee_cursor, ee_len);
  ee_cursor++;
  Serial.print(".");

  for (int i = 0; i < ee_len; i++) {
    EEPROM.write(ee_cursor, mqtt_username[i]);
    ee_cursor++;
    Serial.print(".");
  }

  EEPROM.write(ee_cursor, break_char);
  ee_cursor++;
  Serial.print(".");

  // Write server password
  ee_len = sizeof(mqtt_password);
  EEPROM.write(ee_cursor, ee_len);
  ee_cursor++;
  Serial.print(".");

  for (int i = 0; i < ee_len; i++) {
    EEPROM.write(ee_cursor, mqtt_password[i]);
    ee_cursor++;
    Serial.print(".");
  }

  EEPROM.write(ee_cursor, break_char);
  ee_cursor++;
  Serial.print(".");

  EEPROM.commit();
  EEPROM.end();

  Serial.print("Done writing data to EEPROM at ");
  Serial.println(ee_cursor, DEC);
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

}
