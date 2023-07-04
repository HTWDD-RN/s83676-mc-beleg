#include "player.h"

void Player::handleJoyStick(int joyStickX) {
  if(joyStickX < 350 && mAllowNextDirection && mJoyStickXDefaultPosition) {
    turnLeft();
    mAllowNextDirection = false;
    mJoyStickXDefaultPosition = false;
  }
  
  if(joyStickX > 650 && mAllowNextDirection && mJoyStickXDefaultPosition) {
    turnRight();
    mAllowNextDirection = false;
    mJoyStickXDefaultPosition = false;
  }

  if(joyStickX >= 350 && joyStickX <= 650) {
    mJoyStickXDefaultPosition = true;
  }
}

bool Player::moveForward() {
  bool hasMoved;
  bool survived = Snake::moveForward(&hasMoved);
  if(hasMoved) {
    mAllowNextDirection = true;
  }
  return survived;
}
