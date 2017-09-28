#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi Config
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "mqtt";

// LED Config
#define PIN        15
#define NUM_LEDS   7
#define BRIGHTNESS 50

// Globals
const byte        ledPin            = 0; // Pin with LED on Adafruit Huzzah
boolean           ledState          = false;
String            myName            = "";
Adafruit_NeoPixel pixels            = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);
WiFiClient        espClient;
PubSubClient      client(espClient);

// Functions
void initPixels() {
  pixels.setBrightness(BRIGHTNESS);
  pixels.begin();
  pixels.show(); // ball pixels to 'off'
}

void setPixelBuffer(byte index, byte red, byte green, byte blue) {
  pixels.setPixelColor(index, pixels.Color(red,green,blue));
}

void writePixelBuffer() {
  pixels.show();
}

// Core
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); 

  // Allow time for ESP to init
  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    toggleLED();
    delay(1000);
  }
  
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

  return String(String(mac[5], HEX) + String(mac[4], HEX) + String(mac[3], HEX) + String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX));
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
    if (optCount >= 4 && optCount % 3 == 1) { // check length
      int pixCount = optCount/3;
      Serial.print("  Got color data for ");
      Serial.print(pixCount);
      Serial.println(" pixels.");
      for (byte i=0; i<pixCount; i++) {
        int offset = i * 3;
        setPixelBuffer(optStrs[0].toInt()+i, optStrs[offset+1].toInt(), optStrs[offset+2].toInt(), optStrs[offset+3].toInt());
      }
      writePixelBuffer();
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
  
  bool error = false;
  
  for (int i=0; i<length; i++) {
    char receivedChar = (char)payload[i];
    if (i == 0){
      if (receivedChar != '<') {
        error = true;
        break;
      }
    }
    else if (i > 0 && i < 5) {
      command[i-1] = receivedChar;
    }
    else if (i == 5) {
      if (receivedChar == '>') {
        parseCommand(command, opts, optLens, optCount);
        break;
      }
      else if (receivedChar != '|') {
        error = true;
        break;
      }
    }
    else if (i > 5) {
      if (receivedChar == '>') {
        parseCommand(command, opts, optLens, optCount);
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

  if (error) {
    Serial.println("ERROR");
  }

  Serial.print("Packet process time: ");
  Serial.print(millis() - procStart);
  Serial.println(" ms");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266 Client")) {
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

