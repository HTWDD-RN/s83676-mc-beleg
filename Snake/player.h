#ifndef PLAYER_H
#define PLAYER_H

#include <Arduino.h>
#include "hardware_settings.h"
#include "matrix.h"
#include "snake.h"

class Player: public Snake {
  public:
    Player(Matrix* display, int initialLength, byte x, byte y, CRGB color)
      :Snake(display, initialLength, x, y, color) {
        mAllowNextDirection = true;
        mJoyStickXDefaultPosition = true;
      }
    void handleJoyStick(int joyStickX);
    bool moveForward();
  private:
    bool mAllowNextDirection;
    bool mJoyStickXDefaultPosition;
};

#endif
