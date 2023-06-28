#include "FastLED.h"
#include <LiquidCrystal.h>

#define LEDPIN 8
#define LEDCOUNT 256

#define BUTTONPIN 7
#define JOYSTICKX A2
#define JOYSTICKY A3

#define PLAYERS 1

#define MAX_ITEMS 5
#define MIN_ITEM_SPAWN_TICKS 100
#define BLINK_TICKS 10


class Statistics {
  public:
    Statistics(LiquidCrystal* display);
    void printPlayers();
    void setPlayerScore(byte playerId, int score);
    void playerDead(byte playerId);
    void reset();
  private:
    LiquidCrystal* mDisplay;
    int playerScores[PLAYERS];
    bool playersDead[PLAYERS];

};

Statistics::Statistics(LiquidCrystal* display) {
  mDisplay = display;
  mDisplay->begin(16, 2);
  mDisplay->clear();
  reset();
}

void Statistics::printPlayers() {
  mDisplay->clear();
  for(int i = 0; i < PLAYERS; i++) {
    mDisplay->write("Player ");
    mDisplay->print(i + 1);
    mDisplay->write(": ");
    mDisplay->print(playerScores[i]);
    if(playersDead[i]) {
      mDisplay->write(" X");
    }
  }
}

void Statistics::setPlayerScore(byte playerId, int score) {
  playerScores[playerId] = score;
}

void Statistics::playerDead(byte playerId) {
  playersDead[playerId] = true;
}

void Statistics::reset() {
  for(int i = 0; i < PLAYERS; i++) {
    playerScores[i] = 0;
    playersDead[i] = false;
  }
}

class Matrix {

  private:
    CRGB leds[LEDCOUNT];
  public:
    Matrix();
    void show();
    bool setPixelColor(byte x, byte y, CRGB color, bool overwrite);
};

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

  // if(y % 2 == 0) {
  //   leds[16 * y + (15 - x)] = color;

  // }else {
  //   leds[16 * y + x] = color;
  // }

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

bool Item::checkCollission(byte x, byte y) {
  bool collission = (x == mPosX && y == mPosY);
  if(collission) {
    mPosX = 255;
    mPosY = 255;
  }
  return collission;
}

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

Matrix m;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Player player(&m, 5, 4, 3, 0x00FF00);
Statistics stats(&lcd);

int ticksSinceLastItem;
int nextItemTickCount;
Item items[MAX_ITEMS](&m);

int ticksSinceLastAnimation;

byte gameState = 0;
long lastGameTick;

bool joystickButtonDefaultPosition = true;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTONPIN, INPUT);
  player.initItems(items);
  m.show();
  ticksSinceLastItem = 0;
  ticksSinceLastAnimation = 0;
  nextItemTickCount = MIN_ITEM_SPAWN_TICKS;
  lastGameTick = millis();
}

void gameTick() {
  lastGameTick = millis();

  if(ticksSinceLastItem > nextItemTickCount) {
    ticksSinceLastItem = 0;

    byte randX = random(0, 16);
    byte randY = random(0, 16);
    for(int i = 0; i < MAX_ITEMS; i++) {
      if(items[i].exists()) continue;

      boolean allowedGeneration = items[i].generateItem(randX, randY, 0);

      if(!allowedGeneration) {
        items[i].remove();
      }
      break;
    }
    nextItemTickCount = MIN_ITEM_SPAWN_TICKS * random(100, 500) / 100.0;
    Serial.print("next item will be generated in ");
    Serial.print(nextItemTickCount);
    Serial.println(" ticks");
  }
  ticksSinceLastItem++;


  if(gameState == 2) {
    bool survived = player.moveForward();

    stats.setPlayerScore(0, player.getScore());
    if(!survived) {
      player.die();
      stats.playerDead(0);
      gameState = 1;
    }
    stats.printPlayers();
  }

  if(player.dead() && ticksSinceLastAnimation > BLINK_TICKS) {
    ticksSinceLastAnimation = 0;
    player.blink();
  }
  ticksSinceLastAnimation++;
  m.show();
}

void loop() {

  if(!digitalRead(BUTTONPIN)) {
    joystickButtonDefaultPosition = true;
  }
  //start
  if(gameState == 0 && digitalRead(BUTTONPIN) && joystickButtonDefaultPosition) {
    lastGameTick = millis();
    gameState = 2;
    joystickButtonDefaultPosition = false;
  }

  //reset
  if(gameState == 1 && digitalRead(BUTTONPIN)  && joystickButtonDefaultPosition) {
    player.reset(); stats.reset(); stats.printPlayers();
    gameState = 0;
    for(int i = 0; i < MAX_ITEMS; i++) {
      items[i].clear();
    }
    joystickButtonDefaultPosition = false;
  }

  //verhindert, dass bei pausiertem Spiel Items generiert werden
  if(gameState != 2) {
    ticksSinceLastItem = 0;
  }

  int readX = analogRead(JOYSTICKX);
  int readY = analogRead(JOYSTICKY);

  player.handleJoyStick(readX);

  if(millis() - lastGameTick > 50) {
    gameTick();
  }
}
