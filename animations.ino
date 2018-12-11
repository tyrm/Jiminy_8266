void AniLoop() {
  unsigned long currentMillis = millis();
  
  if (MyMode > 0 && currentMillis - LastRun >= MyDelay) {
    LastRun = currentMillis;
    switch (MyMode) {
      case 1:
        AniFire();
        break;
    }
  }
}

void AniFire(){
  for(int i=0; i<LEDCount; i++){
    SetPixelBuffer(i, random(120)+135, random(40), 0, 0);
    int anum = random(LEDCount/4);
    if(anum==1){
      SetPixelBuffer(i, 0, 0, 0, 0);
    }
    int anums = random(LEDCount*5);
    if(anums==1){
      SetPixelBuffer(i, 255, 255, 0, 0);
    }
  }
  WritePixelBuffer();
}
