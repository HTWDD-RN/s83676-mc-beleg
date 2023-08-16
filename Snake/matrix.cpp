#include "matrix.h"

Matrix::Matrix() {
  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);
    for(int i = 0; i < LEDCOUNT; i++) {
      leds[i] = 0x0000;
    }
}

bool Matrix::setPixelColor(byte x, byte y, CRGB color, bool overwrite) {
  if(x >= 16 || x < 0) return false;
  if(y >= 16 || y < 0) return false;

  int arrayPos = 16 * y + x;

  if(y % 2 == 0) {
    arrayPos = 16 * y + (15 - x);
  }

  CRGB off = 0x000000;

  if(!overwrite && leds[arrayPos] != off) {
    return false;
  }

  leds[arrayPos] = color;
  return true;
}

void Matrix::show() {
  FastLED.show();
}
