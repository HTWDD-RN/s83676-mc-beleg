#include "snake.h"

Snake::Snake(Matrix* display, int initialLength, byte x, byte y, CRGB color) {
  mDisplay = display;
  mStartLength = initialLength;

  if(mStartLength < 1) {
    mStartLength = 1;
  }
  mLength = 0;
  mColor = color;

  mStartX = x;
  mStartY = y;
  reset();
}

void Snake::reset() {

  for(int i = 0; i < mLength; i++) {
    mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], 0x000000, true);
  }

  mLength = mStartLength;
  mDirection = UP;
  mDead = false;
  mBlinkColor = false;
  ticksSinceLastMove = 0;

  for(int i = 0; i < mLength; i++) {
    mSnakePartsX[i] = mStartX;
    mSnakePartsY[i] = mStartY + i;
    mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], mColor, true);
  }
}

void Snake::blink() {
  mBlinkColor = !mBlinkColor;
  for(int i = 0; i < mLength; i++) {
    if(mBlinkColor) {
      mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], 0xFF0000, true);
    } else {
      mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], mColor, true);
    }
  }
}

bool Snake::moveForward(bool* moved) {

  if(ticksSinceLastMove < 10 - (mLength - mStartLength) / 5) {
    ticksSinceLastMove++;
    *moved = false;
    return true;
  }
  ticksSinceLastMove = 0;
  *moved = true;

  byte newHeadX;
  byte newHeadY;

  switch(mDirection) {
    case 0: newHeadX = mSnakePartsX[0]; newHeadY = mSnakePartsY[0] - 1; break;
    case 1: newHeadX = mSnakePartsX[0] + 1; newHeadY = mSnakePartsY[0]; break;
    case 2: newHeadX = mSnakePartsX[0]; newHeadY = mSnakePartsY[0] + 1; break;
    case 3: newHeadX = mSnakePartsX[0] - 1; newHeadY = mSnakePartsY[0]; break;
  }

  //Kollission mit Rand
  if(newHeadX < 0 || newHeadX > 15 ||
     newHeadY < 0 || newHeadY > 15) {
      return false;
  }

  //prüfen, ob Item gefressen wurde
  bool gotItem = false;
  if(gameItems != 0) {
    for(int i = 0; i < MAX_ITEMS; i++) {
      if(!gameItems[i].exists()) continue;
      gotItem = gameItems[i].checkCollission(newHeadX, newHeadY);
      if(gotItem) {
        mLength++;
        break;
      }
    }
  }

  if(!gotItem) {
      //letzer Pixel (Schwanz) deaktivieren
    mDisplay->setPixelColor(mSnakePartsX[mLength - 1], mSnakePartsY[mLength - 1], 0x000000, true);
  }

  for(int i = mLength - 1; i > 0; i--) {
    mSnakePartsX[i] = mSnakePartsX[i - 1];
    mSnakePartsY[i] = mSnakePartsY[i - 1];
    //Kollission mit sich selbst
    if(mSnakePartsX[i] == newHeadX && mSnakePartsY[i] == newHeadY) {
      return false;
    }
  }

  mSnakePartsX[0] = newHeadX;
  mSnakePartsY[0] = newHeadY;

  //neuen ersten Pixel (Kopf) aktivieren
  mDisplay->setPixelColor(mSnakePartsX[0], mSnakePartsY[0], mColor, true);
  return true;
}

void Snake::turnRight() {
  mDirection += 1;
  if(mDirection > 3) {
    mDirection = 0;
  }
}

void Snake::turnLeft() {
  mDirection -= 1;
  // < 0 === > 3 (unsigned)
  if(mDirection > 3) {
    mDirection = 3;
  }
}
