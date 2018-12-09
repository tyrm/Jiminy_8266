void InitColorSchemes() {
  byte Rainbow[] = {
    255, 0, 0, 0,   // Red
    255, 127, 0, 0, // Orange
    255, 255, 0, 0, // Yellow
    0, 255, 0, 0,   // Green
    0, 0, 255, 0,   // Blue
    100, 0, 255, 0  // Purple
  };
  LoadColorScheme(0, Rainbow, sizeof(Rainbow));

  byte TyrScheme[] = {
    0, 255, 0, 0,
    0, 0, 255, 0,
    0, 255, 255, 0,
    0, 255, 255, 0,
    0, 0, 255, 0,
    0, 255, 0, 0
  };
  LoadColorScheme(1, TyrScheme, sizeof(TyrScheme));
}

void LoadColorScheme(byte pos, byte colors[], int colorSize) {
  ColorScheme[pos][0] = colorSize/4;
  for (int x=0; x < colorSize; x=x+4){
    ColorScheme[pos][x+1] = colors[x];
    ColorScheme[pos][x+2] = colors[x+1];
    ColorScheme[pos][x+3] = colors[x+2];
    ColorScheme[pos][x+4] = colors[x+3];
  }
}
