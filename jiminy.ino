#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi Config
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "mqtt";

WiFiClient   espClient;
PubSubClient client(espClient);

// LED Config
#define PIN        15
#define BRIGHTNESS 100

// Gold Box
//#define NUM_LEDS   70

// Spice Box
//#define NUM_LEDS   46

// Glass Table
//#define NUM_LEDS   54

// Glass Ikea
#define NUM_LEDS   38

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

//*********//
// Globals //
//*********//
byte              colorScheme[10][41];
const byte        ledPin              = 0; // Pin with LED on Adafruit Huzzah
boolean           ledState            = false;
byte              myMode              = 0;
String            myName              = "";

//***********//
// Functions //
//***********//
void initPixels() {
  pixels.setBrightness(BRIGHTNESS);
  pixels.begin();
  for (byte i=0; i<NUM_LEDS; i++) {
    setPixelBuffer(i, 0, 0, 0, 255);
  }
  pixels.show(); // ball pixels to 'off'
}


void setPixelBrightness(byte b) {
  pixels.setBrightness(b);
}

void setPixelBuffer(byte index, byte red, byte green, byte blue, byte white) {
  pixels.setPixelColor(index, pixels.Color(red, green, blue, white));
}

void writePixelBuffer() {
  pixels.show();
}

//**********//
// Patterns //
//**********//

// 0 - Alternate
void patAlternate(byte schemeID) {
  if (colorScheme[schemeID][0] > 0) {
    Serial.print("  Pattern: Alternating [");
    Serial.print(schemeID);
    Serial.println("]");
    for (int i=0; i<NUM_LEDS; i++) {
      int schIndex = i % colorScheme[schemeID][0];
      int offset = schIndex * 4;
      
      setPixelBuffer(i, colorScheme[schemeID][offset+1], colorScheme[schemeID][offset+2], colorScheme[schemeID][offset+3], colorScheme[schemeID][offset+4]);
    }
    writePixelBuffer();
  }
}

// 1 - gradient
// 2 - reverse gradient
void patGradient(byte schemeID, boolean reverse=false) {
  if (colorScheme[schemeID][0] > 1) {
    Serial.print("  Pattern: ");
    if (reverse) {
      Serial.print("Reversed ");
    }
    Serial.print("Gradient [");
    Serial.print(schemeID);
    Serial.println("]");
    
    int offset            = 0;
    int segmentSize       = NUM_LEDS/(colorScheme[schemeID][0]-1);
    byte segmentRemainder = NUM_LEDS%(colorScheme[schemeID][0]-1);

    for (int i=0; i<colorScheme[schemeID][0]-1; i++) {
      // Add remainders to fill all pixels
      int segment = segmentSize;
      if (i<segmentRemainder){
        segment++;
      }

      int offsetA;
      int offsetB;
      
      if (!reverse){
        offsetA = i*4;
        offsetB = (i+1)*4;
      }
      else {
        offsetA = ((colorScheme[schemeID][0]-1)-i)*4;
        offsetB = ((colorScheme[schemeID][0]-2)-i)*4;
      }

      byte ar = colorScheme[schemeID][offsetA+1];
      byte ag = colorScheme[schemeID][offsetA+2];
      byte ab = colorScheme[schemeID][offsetA+3];
      byte aw = colorScheme[schemeID][offsetA+4];

      byte br = colorScheme[schemeID][offsetB+1];
      byte bg = colorScheme[schemeID][offsetB+2];
      byte bb = colorScheme[schemeID][offsetB+3];
      byte bw = colorScheme[schemeID][offsetB+4];
      
      for (int j=0; j<segment; j++) {
        int lowMax = segment-1;
        
        byte xr = map(j, 0, lowMax, ar, br);
        byte xg = map(j, 0, lowMax, ag, bg);
        byte xb = map(j, 0, lowMax, ab, bb);
        byte xw = map(j, 0, lowMax, aw, bw);
        
        setPixelBuffer(j+offset, xr, xg, xb, xw);
      }
      offset = offset + segment;
    }
    writePixelBuffer();
  }
}

// 3 - filled segments
// 4 - filled segments
void patFilledSegments(byte schemeID, boolean reverse=false) {
  if (colorScheme[schemeID][0] > 1) {
    Serial.print("  Pattern: ");
    if (reverse) {
      Serial.print("Reversed ");
    }
    Serial.print("Gradient [");
    Serial.print(schemeID);
    Serial.println("]");
    
    int offset            = 0;
    int segmentSize       = NUM_LEDS/colorScheme[schemeID][0];
    byte segmentRemainder = NUM_LEDS%colorScheme[schemeID][0];

    for (int i=0; i<colorScheme[schemeID][0]; i++) {
      // Add remainders to fill all pixels
      int segment = segmentSize;
      if (i<segmentRemainder){
        segment++;
      }

      int colorOffset;
      
      if (!reverse){
        colorOffset = i*4;
      }
      else {
        colorOffset = ((colorScheme[schemeID][0]-1)-i)*4;
      }
      
      for (int j=0; j<segment; j++) {
        setPixelBuffer(j+offset, colorScheme[schemeID][colorOffset+1], colorScheme[schemeID][colorOffset+2], colorScheme[schemeID][colorOffset+3], colorScheme[schemeID][colorOffset+4]);
      }
      offset = offset + segment;
    }
    writePixelBuffer();
  }
}

//******//
// Core //
//******//
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); 

  int colorSchemeCount = sizeof(colorScheme)/sizeof(colorScheme[0]);
  for (int i=0; i<colorSchemeCount; i++) {
    colorScheme[i][0] = 0;
  }

  // Allow time for ESP to init
  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    toggleLED();
    delay(1000);
  }

  // connect to wifi 
  setupWifi();
  
  myName = getName();

  client.setServer(mqtt_server, 1883);
  client.setCallback(processPacket);

  initPixels();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

int countPipes(byte* payload, unsigned int length) {
  int count = 0;
  
  for (int i=0; i<length; i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '|') {
      count += 1;
    }
  }
  
  return count;
}

String getName() {
  byte mac[6];
  WiFi.macAddress(mac);

  return String(formatHex8(mac[5]) + formatHex8(mac[4]) + formatHex8(mac[3]) + formatHex8(mac[2]) + formatHex8(mac[1]) + formatHex8(mac[0]));
}

void parseCommand(char command[],char opts[][4],int optLens[], int optCount) {
  // Toggle LED to indicate valid Packet received
  toggleLED();
  
  // Make Strings
  String commandStr = String(command);
  String optStrs[optCount];
  for (int i=0; i<optCount; i++) {
    switch (optLens[i]){
      case 1:
        optStrs[i] = String(opts[i][0]);
        break;
      case 2:
        optStrs[i] = String(String(opts[i][0]) + String(opts[i][1]));
        break;
      case 3:
        optStrs[i] = String(String(opts[i][0]) + String(opts[i][1]) + String(opts[i][2]));
        break;
      default:
        optStrs[i] = "";
        break;
    }
  }

  // Parse and Execute Command
  if (commandStr == "PING") {
    Serial.println("  Got PING");
    pong();
  }
  else if (commandStr == "SETP") {
    if (optCount >= 5 && optCount % 4 == 1) { // check length
      int pixCount = optCount/4;
      Serial.print("  Got color+white data for ");
      Serial.print(pixCount);
      Serial.println(" pixels.");
      for (byte i=0; i<pixCount; i++) {
        int offset = i * 4;
        setPixelBuffer(optStrs[0].toInt()+i,optStrs[offset+1].toInt(), optStrs[offset+2].toInt(), optStrs[offset+3].toInt(), optStrs[offset+4].toInt());
      }
      writePixelBuffer();
    }
  }
  else if (commandStr == "SETA") {
    if (optCount == 4) { // check length
      Serial.println("  Got color+white data for all pixels.");
      for (byte i=0; i<NUM_LEDS; i++) {
        setPixelBuffer(i, optStrs[0].toInt(), optStrs[1].toInt(), optStrs[2].toInt(), optStrs[3].toInt());
      }
      writePixelBuffer();
    }
  }
  else if (commandStr == "BRIG") {
    if (optCount == 1) { // check length
      Serial.print("  Got brightness ");
      Serial.print(optStrs[0]);
      Serial.println(".");
      setPixelBrightness(optStrs[0].toInt());
      writePixelBuffer();
    }
  }
  else if (commandStr == "CLRS") {
    if (optCount >= 5 && optCount <= 41 && optCount % 4 == 1) { // check length
      byte schemeID = optStrs[0].toInt();
      int pixCount = optCount/4;
      Serial.print("  Got color scheme ");
      Serial.print(optStrs[0]);
      Serial.println(".");
      // Store Number of Colors in Element 0
      colorScheme[schemeID][0] = pixCount;
      for (byte i=0; i<pixCount; i++) {
        int offset = i*4;
        
        colorScheme[schemeID][offset+1] = optStrs[offset+1].toInt();
        colorScheme[schemeID][offset+2] = optStrs[offset+2].toInt();
        colorScheme[schemeID][offset+3] = optStrs[offset+3].toInt();
        colorScheme[schemeID][offset+4] = optStrs[offset+4].toInt();
      }
    }
  }
  else if (commandStr == "PATS") {
    if (optCount == 2) { // check length
      switch (optStrs[0].toInt()) {
        case 0:
          patAlternate(optStrs[1].toInt());
          break;
        case 1:
          patGradient(optStrs[1].toInt());
          break;
        case 2:
          patGradient(optStrs[1].toInt(), true);
          break;
        case 3:
          patFilledSegments(optStrs[1].toInt());
          break;
        case 4:
          patFilledSegments(optStrs[1].toInt(), true);
          break;
      }
    }
  }
}

void pong() {
  String message = String("<PONG|" + myName + "|" + NUM_LEDS + ">");

  int bufferSize = message.length() + 1;
  char msgBuffer[bufferSize];
  message.toCharArray(msgBuffer, bufferSize);
  
  client.publish("/jiminy/reply", msgBuffer);
}

void processPacket(char* topic, byte* payload, unsigned int length) {
  int procStart = millis();
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  char command[5] = {'\0'};

  int optCount = countPipes(payload, length);
  char opts[optCount][4];
  int optLens[optCount];
  int optionIndex = 0;
  int optionCur = 0;

  bool started = false;
  bool error = false;
  
  for (int i=0; i<length; i++) {
    char receivedChar = (char)payload[i];
    if (i == 0){
      if (receivedChar == '<') {
        started = true;
      }
      else {
        error = true;
        break;
      }
    }
    else if (i > 0 && i < 5) {
      command[i-1] = receivedChar;
    }
    else if (i == 5) {
      if (receivedChar == '>') {
        break;
      }
      else if (receivedChar != '|') {
        error = true;
        break;
      }
    }
    else if (i > 5) {
      if (receivedChar == '>') {
        break;
      }
      else if (receivedChar == '|') {
        optionIndex++;
        optionCur = 0;
      }
      else {
        opts[optionIndex][optionCur] = receivedChar;
        optLens[optionIndex] = optionCur + 1;

        optionCur++;
      }
    }
  }

  if (!error) {
    parseCommand(command, opts, optLens, optCount);
  }
  else {
    Serial.println("ERROR");
  }

  Serial.print("Packet process time: ");
  Serial.print(millis() - procStart);
  Serial.println(" ms");
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    int bufferSize = myName.length() + 1;
    char nameBuffer[bufferSize];
    myName.toCharArray(nameBuffer, bufferSize);
    
    // Attempt to connect
    if (client.connect(nameBuffer)) {
      Serial.println("connected");
       // ... and subscribe to topic
       client.subscribe("/jiminy/c/all");
       String myTopic = String("/jiminy/c/" + myName);

       int bufferSize = myTopic.length() + 1;
       char topicBuffer[bufferSize];
       myTopic.toCharArray(topicBuffer, bufferSize);
       
       client.subscribe(topicBuffer);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Let server know we're here
  pong();
}

void setupWifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void toggleLED() {
  if (ledState) {
    digitalWrite(ledPin, HIGH);
    ledState = false;
  }
  else {
    digitalWrite(ledPin, LOW);
    ledState = true;
  }
}

String formatHex8(char data) {
  String value = "";
  if (data<0x10) {value = value + "0";} 
  value = value + String(data, HEX);

  return value;
}
