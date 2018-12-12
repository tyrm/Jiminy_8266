void AniLoop() {
  
  if (MyMode > 0) {
    switch (MyMode) {
      case 1:
        AniFire();
        break;
      case 2:
        AniSparkle();
        break;
    }
  }
}

void AniFire(){
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - LastRun >= 80) {
    LastRun = currentMillis;
    for(int i=0; i<LEDCount; i++){
      SetPixelBuffer(i, random(150)+105, random(40), 0, 0);
      int anum = random(LEDCount/4);
      if(anum==1){
        SetPixelBuffer(i, 0, 0, 0, 0);
      }
      int anums = random(LEDCount*5);
      if(anums==1){
        SetPixelBuffer(i, 255, 200, 0, 0);
      }
    }
    WritePixelBuffer();
  }
}

void AniSparkle(){
  static byte drop[MAX_LEDS];
  unsigned long currentMillis = millis();
  
  if (currentMillis - LastRun >= 50) {
    LastRun = currentMillis;

    if(random(10)==0){
      int pixelCount = LEDCount/20;
      
      for(int i=0; i<pixelCount; i++){
        drop[random(LEDCount)]=255;
      }
    }
    for(int i=0; i<LEDCount; i++){
      SetPixelBuffer(i, 0, 0, 0, drop[i]);
      
      if(drop[i]>5){
        drop[i]-=5;
      }else {
        drop[i]=0;
      };
    }
    WritePixelBuffer();
  }
}
