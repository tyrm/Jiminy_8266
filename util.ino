#define LED_PIN   0

String FormatHex8(uint8_t data) {
  String value = "";
  if (data<0x10) {value = value + "0";} 
  value = value + String(data, HEX);

  return value;
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