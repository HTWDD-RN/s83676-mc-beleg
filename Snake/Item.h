#ifndef ITEM_H
#define ITEM_H

#include <Arduino.h>
#include "game_settings.h"
#include "matrix.h"

class Item {
  public:
    Item(Matrix* display);
    bool generateItem(byte x, byte y, byte type);
    bool exists();
    bool checkCollission(byte x, byte y);
    void remove();
    void clear();
  private:
    Matrix* mDisplay;
    byte mPosX;
    byte mPosY;
    byte mType;
};

#endif
