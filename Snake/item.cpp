#include "item.h"

bool Item::checkCollission(byte x, byte y) {
  bool collission = (x == mPosX && y == mPosY);
  if(collission) {
    mPosX = 255;
    mPosY = 255;
  }
  return collission;
}
