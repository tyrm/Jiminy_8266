#include "patterns.h"

void MQTTLoop() {
  if (!MQTTClient.connected()) {
    MQTTReconnect();
  }
  MQTTClient.loop();
}

void MQTTParseCommand(char command[],char opts[][4],int optLens[], int optCount) {
  // Toggle LED to indicate valid Packet received
  ToggleLED();
  
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
    MQTTPong();
  }
  else if (commandStr == "SETP") {
    if (optCount >= 5 && optCount % 4 == 1) { // check length
      int pixCount = optCount/4;
      Serial.print("  Got color+white data for ");
      Serial.print(pixCount);
      Serial.println(" pixels.");
      for (byte i=0; i<pixCount; i++) {
        int offset = i * 4;
        SetPixelBuffer(optStrs[0].toInt()+i,optStrs[offset+1].toInt(), optStrs[offset+2].toInt(), optStrs[offset+3].toInt(), optStrs[offset+4].toInt());
      }
      WritePixelBuffer();
    }
  }
  else if (commandStr == "SETA") {
    if (optCount == 4) { // check length
      Serial.println("  Got color+white data for all pixels.");
      for (byte i=0; i<LEDCount; i++) {
        SetPixelBuffer(i, optStrs[0].toInt(), optStrs[1].toInt(), optStrs[2].toInt(), optStrs[3].toInt());
      }
      WritePixelBuffer();
    }
  }
  else if (commandStr == "BRIG") {
    if (optCount == 1) { // check length
      Serial.print("  Got brightness ");
      Serial.print(optStrs[0]);
      Serial.println(".");
      SetPixelBrightness(optStrs[0].toInt());
      WritePixelBuffer();
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
      ColorScheme[schemeID][0] = pixCount;
      for (byte i=0; i<pixCount; i++) {
        int offset = i*4;
        
        ColorScheme[schemeID][offset+1] = optStrs[offset+1].toInt();
        ColorScheme[schemeID][offset+2] = optStrs[offset+2].toInt();
        ColorScheme[schemeID][offset+3] = optStrs[offset+3].toInt();
        ColorScheme[schemeID][offset+4] = optStrs[offset+4].toInt();
      }
    }
  }
  else if (commandStr == "PATS") {
    if (optCount == 2) { // check length
      switch (optStrs[0].toInt()) {
        case 0:
          PatAlternate(optStrs[1].toInt());
          break;
        case 1:
          PatGradients(optStrs[1].toInt(), false);
          break;
        case 2:
          PatGradients(optStrs[1].toInt(), true);
          break;
        case 3:
          PatFilledSegments(optStrs[1].toInt(), false);
          break;
        case 4:
          PatFilledSegments(optStrs[1].toInt(), true);
          break;
      }
    }
  }
}

void MQTTPong() {
  String message = String("<PONG|" + MyName + "|" + LEDCount + "|" + IpAddress2String(WiFi.localIP()) + ">");

  int bufferSize = message.length() + 1;
  char msgBuffer[bufferSize];
  message.toCharArray(msgBuffer, bufferSize);
  
  MQTTClient.publish("/jiminy/reply", msgBuffer);
}

void MQTTProcessPacket(char* topic, byte* payload, unsigned int length) {
  int procStart = millis();
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  char command[5] = {'\0'};

  int optCount = CountPipes(payload, length);
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
    MQTTParseCommand(command, opts, optLens, optCount);
  }
  else {
    Serial.println("ERROR");
  }

  Serial.print("Packet process time: ");
  Serial.print(millis() - procStart);
  Serial.println(" ms");
  Serial.println();
}

void MQTTReconnect() {
  // Loop until we're reconnected
  while (!MQTTClient.connected()) {
    
    int bufferSize = MyName.length() + 1;
    char nameBuffer[bufferSize];
    MyName.toCharArray(nameBuffer, bufferSize);

    bufferSize = sizeof(MQTTUsername) + 1;
    char usernameBuffer[bufferSize];
    MQTTUsername.toCharArray(usernameBuffer, bufferSize);
    
    bufferSize = sizeof(MQTTPassword) + 1;
    char passwordBuffer[bufferSize];
    MQTTPassword.toCharArray(passwordBuffer, bufferSize);
    
    Serial.print("Attempting MQTT connection ...");
    
    // Attempt to connect
    if (MQTTClient.connect(nameBuffer, usernameBuffer, passwordBuffer)) {
      Serial.println("connected");
       // ... and subscribe to topic
       MQTTClient.subscribe("/jiminy/c/all");
       String myTopic = String("/jiminy/c/" + MyName);

       int bufferSize = myTopic.length() + 1;
       char topicBuffer[bufferSize];
       myTopic.toCharArray(topicBuffer, bufferSize);
       
       MQTTClient.subscribe(topicBuffer);
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Let server know we're here
  MQTTPong();
}
