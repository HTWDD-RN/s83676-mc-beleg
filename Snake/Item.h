#ifndef ITEM_H
#define ITEM_H

#include <Arduino.h>
#include "game_settings.h"
#include "matrix.h"

class Item {
  public:
    Item(Matrix* display) {
      mDisplay = display;
      mPosX = 255;
      mPosY = 255;
    }
    bool generateItem(byte x, byte y, byte type) {
      mPosX = x;
      mPosY = y;
      mType = type;
      return mDisplay->setPixelColor(x, y, 0x0000FF, false);
    }

    bool exists() {
      return mPosX != 255;
    }

    bool checkCollission(byte x, byte y);

    void remove() {
      mPosX = 255;
      mPosY = 255;
    }

    void clear() {
      mDisplay->setPixelColor(mPosX, mPosY, 0x000000, true);
      remove();
    }
  private:
    Matrix* mDisplay;
    byte mPosX;
    byte mPosY;
    byte mType;
};

#endif
