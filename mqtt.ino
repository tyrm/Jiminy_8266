
void MQTTLoop() {
  if (!MQTTClient.connected()) {
    MQTTReconnect();
  }
  MQTTClient.loop();
}

void MQTTPong() {
  String message = String("<PONG|" + MyName + "|" + LEDCount + "|" + IpAddress2String(WiFi.localIP()) + ">");

  int bufferSize = message.length() + 1;
  char msgBuffer[bufferSize];
  message.toCharArray(msgBuffer, bufferSize);
  
  MQTTClient.publish("/jiminy/reply", msgBuffer);
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
