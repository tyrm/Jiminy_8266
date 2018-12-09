// 0 - Alternate
void PatAlternate(byte schemeID) {
  if (ColorScheme[schemeID][0] > 0) {
    Serial.print("  Pattern: Alternating [");
    Serial.print(schemeID);
    Serial.println("]");
    for (int i=0; i<LEDCount; i++) {
      int schIndex = i % ColorScheme[schemeID][0];
      int offset = schIndex * 4;
      
      SetPixelBuffer(i, ColorScheme[schemeID][offset+1], ColorScheme[schemeID][offset+2], ColorScheme[schemeID][offset+3], ColorScheme[schemeID][offset+4]);
    }
    WritePixelBuffer();
  }
}

// 1 - gradient
// 2 - reverse gradient
void PatGradients(byte schemeID, boolean reverse) {
  if (ColorScheme[schemeID][0] > 1) {
    Serial.print("  Pattern: ");
    if (reverse) {
      Serial.print("Reversed ");
    }
    Serial.print("Gradient [");
    Serial.print(schemeID);
    Serial.println("]");
    
    int offset            = 0;
    int segmentSize       = LEDCount/(ColorScheme[schemeID][0]-1);
    byte segmentRemainder = LEDCount%(ColorScheme[schemeID][0]-1);

    for (int i=0; i<ColorScheme[schemeID][0]-1; i++) {
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
        offsetA = ((ColorScheme[schemeID][0]-1)-i)*4;
        offsetB = ((ColorScheme[schemeID][0]-2)-i)*4;
      }

      byte ar = ColorScheme[schemeID][offsetA+1];
      byte ag = ColorScheme[schemeID][offsetA+2];
      byte ab = ColorScheme[schemeID][offsetA+3];
      byte aw = ColorScheme[schemeID][offsetA+4];

      byte br = ColorScheme[schemeID][offsetB+1];
      byte bg = ColorScheme[schemeID][offsetB+2];
      byte bb = ColorScheme[schemeID][offsetB+3];
      byte bw = ColorScheme[schemeID][offsetB+4];
      
      for (int j=0; j<segment; j++) {
        int lowMax = segment-1;
        
        byte xr = map(j, 0, lowMax, ar, br);
        byte xg = map(j, 0, lowMax, ag, bg);
        byte xb = map(j, 0, lowMax, ab, bb);
        byte xw = map(j, 0, lowMax, aw, bw);
        
        SetPixelBuffer(j+offset, xr, xg, xb, xw);
      }
      offset = offset + segment;
    }
    WritePixelBuffer();
  }
}

// 3 - filled segments
// 4 - filled segments
void PatFilledSegments(byte schemeID, boolean reverse) {
  if (ColorScheme[schemeID][0] > 1) {
    Serial.print("  Pattern: ");
    if (reverse) {
      Serial.print("Reversed ");
    }
    Serial.print("Gradient [");
    Serial.print(schemeID);
    Serial.println("]");
    
    int offset            = 0;
    int segmentSize       = LEDCount/ColorScheme[schemeID][0];
    byte segmentRemainder = LEDCount%ColorScheme[schemeID][0];

    for (int i=0; i<ColorScheme[schemeID][0]; i++) {
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
        colorOffset = ((ColorScheme[schemeID][0]-1)-i)*4;
      }
      
      for (int j=0; j<segment; j++) {
        SetPixelBuffer(j+offset, ColorScheme[schemeID][colorOffset+1], ColorScheme[schemeID][colorOffset+2], ColorScheme[schemeID][colorOffset+3], ColorScheme[schemeID][colorOffset+4]);
      }
      offset = offset + segment;
    }
    WritePixelBuffer();
  }
}
