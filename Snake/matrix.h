#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>
#include "FastLED.h"
#include "game_settings.h"
#include "hardware_settings.h"

class Matrix {
  private:
    CRGB leds[LEDCOUNT];
  public:
    Matrix();
    void show();
    bool setPixelColor(byte x, byte y, CRGB color, bool overwrite);
};

#endif
