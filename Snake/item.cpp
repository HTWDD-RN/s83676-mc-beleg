#include "item.h"

Item::Item(Matrix* display) {
  mDisplay = display;
  mPosX = 255;
  mPosY = 255;
}

bool Item::generateItem(byte x, byte y, byte type) {
  mPosX = x;
  mPosY = y;
  mType = type;
  return mDisplay->setPixelColor(x, y, 0x0000FF, false);
}

bool Item::exists() {
  return mPosX != 255;
}

bool Item::checkCollission(byte x, byte y) {
  bool collission = (x == mPosX && y == mPosY);
  if(collission) {
    mPosX = 255;
    mPosY = 255;
  }
  return collission;
}

void Item::remove() {
  mPosX = 255;
  mPosY = 255;
}

void Item::clear() {
  mDisplay->setPixelColor(mPosX, mPosY, 0x000000, true);
  remove();
}
