#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include "game_settings.h"
#include "item.h"
#include "matrix.h"

class Snake {
  public:
    Snake(Matrix* display,int initialLength, byte x, byte y, CRGB color);
    void reset();
    void die() {
      mDead = true;
    }
    bool dead() {
      return mDead;
    }

    void blink();

    void initItems(Item* items) {
      gameItems = items;
    }
    bool moveForward(bool* moved);
    void turnRight();
    void turnLeft();

    int getScore() {
      return mLength - mStartLength;
    }
  private:
    Matrix* mDisplay;
    CRGB mColor;
    byte mDirection;

    byte mStartX;
    byte mStartY;

    boolean mDead;
    boolean mBlinkColor;

    int mStartLength;
    int mLength;
    byte mSnakePartsX[LEDCOUNT];
    byte mSnakePartsY[LEDCOUNT];

    int ticksSinceLastMove;

    Item* gameItems;

    byte UP = 0;
    byte RIGHT = 1;
    byte DOWN = 2;
    byte LEFT = 3;
};

#endif
