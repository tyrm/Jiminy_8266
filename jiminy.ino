#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Connect to the WiFi
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "mqtt";

String myName = "";

// Init neopixels
#define PIN        15
#define NUM_LEDS   7
#define BRIGHTNESS 50
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Allow time for ESP to init
  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  
  myName = getName();

  client.setServer(mqtt_server, 1883);
  client.setCallback(processPacket);
  
  pixels.setBrightness(BRIGHTNESS);
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
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

void parseCommand(char command[],char opts[][4],int optLens[]) {
  if (String(command) == "PING") {
    pong();
  }
}

void pong() {
  String message = String("<PONG|" + myName + ">");
  
  char msgBuffer[message.length() + 1];
  message.toCharArray(msgBuffer, message.length() + 1);
  
  client.publish("/jiminy/reply", msgBuffer);
}

void processPacket(char* topic, byte* payload, unsigned int length) {
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
        parseCommand(command, opts, optLens);
        break;
      }
      else if (receivedChar != '|') {
        error = true;
        break;
      }
    }
    else if (i > 5) {
      optionCur = 1;
      if (receivedChar == '>') {
        parseCommand(command, opts, optLens);
        break;
      }
      else if (receivedChar == '|') {
        optionIndex++;
        optionCur = 0;
      }
      else {
        opts[optionIndex-1][optionCur] = receivedChar;
        optLens[optionIndex] = optionCur + 1;

        optionCur++;
      }
    }
  }

  if (error) {
    Serial.println("ERROR");
  }
  else {
    // Debug
    Serial.print("Command: ");
    for (int i=0; i<sizeof(command); i++) {
      Serial.print(command[i]);
    }
    Serial.println();
    for (int i=0; i<optionIndex; i++) {
      Serial.print(i);
      Serial.print(": [");
      for (int j=0; j<optLens[i]; j++) {
        Serial.print(opts[i][j]);
      }
      Serial.println("]");
    }
    Serial.println(); 
  }

}

String getName() {
  byte mac[6];
  WiFi.macAddress(mac);

  return String(String(mac[5], HEX) + String(mac[4], HEX) + String(mac[3], HEX)) + String(String(mac[2], HEX) + String(mac[1], HEX) + String(mac[0], HEX));
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

       char topicBuffer[myTopic.length() + 1];
       myTopic.toCharArray(topicBuffer, myTopic.length() + 1);
       
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

