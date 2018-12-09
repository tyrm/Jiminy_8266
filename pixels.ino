#include <Adafruit_NeoPixel.h>
#define LED_PIN 15

Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(255, LED_PIN, NEO_GRBW + NEO_KHZ800);;

void InitPixels(int count, byte red, byte green, byte blue, byte white, byte brightness) {
  Pixels.begin();

  // Set inital color
  SetPixelBrightness(brightness);
  for (byte i=0; i<count; i++) {
    SetPixelBuffer(i, red, green, blue, white);
  }
  WritePixelBuffer();
}

void SetPixelBrightness(byte b) {
  Pixels.setBrightness(b);
}

void SetPixelBuffer(byte index, byte red, byte green, byte blue, byte white) {
  Pixels.setPixelColor(index, Pixels.Color(red, green, blue, white));
}

void WritePixelBuffer() {
  Pixels.show();
}
