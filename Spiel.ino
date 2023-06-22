#include "FastLED.h"

#define LEDPIN 8
#define LEDCOUNT 256

#define MAX_ITEMS 5
#define MIN_ITEM_DELAY 5000

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
  if(x >= 16 || x < 0) return;
  if(y >= 16 || y < 0) return;

  if(y % 2 == 0) {
    leds[16 * y + (15 - x)] = color;

  }else {
    leds[16 * y + x] = color;
  }

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
      return mDisplay->setPixelColor(x, y, 0xFF0000, false);
    }

    bool exists() {
      return mPosX != 255;
    }

    bool checkCollission(byte x, byte y);
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
    void initItems(Item* items) {
      gameItems = items;
    }
    bool moveForward();
    void turnRight();
    void turnLeft();
  private:
    Matrix* mDisplay;
    CRGB mColor;
    byte mDirection;

    byte mStartX;
    byte mStartY;

    int mStartLength;
    int mLength;
    byte mSnakePartsX[LEDCOUNT];
    byte mSnakePartsY[LEDCOUNT];

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

  for(int i = 0; i < mLength; i++) {
    mSnakePartsX[i] = mStartX;
    mSnakePartsY[i] = mStartY + i;
    mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], mColor, true);
  }
}

bool Snake::moveForward() {

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

Matrix m;
Snake snake(&m, 5, 4, 3, 0x00FF00);

long lastGeneratedItem;
long nextItemDelay;
Item items[MAX_ITEMS](&m);

bool gamePaused = true;

void setup() {
  Serial.begin(9600);
  snake.initItems(items);
  m.show();
  lastGeneratedItem = millis();
  nextItemDelay = MIN_ITEM_DELAY;
}

void loop() {

  //verhindert, dass bei pausiertem Spiel Items generiert werden
  if(gamePaused) {
    lastGeneratedItem = millis();
  }

  if(millis() - lastGeneratedItem > nextItemDelay) {
    byte randX = random(0, 16);
    byte randY = random(0, 16);
    for(int i = 0; i < MAX_ITEMS; i++) {
      if(items[i].exists()) continue;
      items[i].generateItem(randX, randY, 0);
      break;
    }

    lastGeneratedItem = millis();
  }

  if(Serial.available()) {
    char command = Serial.read();
    
    switch(command) {
      case 's': gamePaused = false; break;
      case 'g': snake.reset(); break;
      case 'r': snake.turnRight(); break;
      case 'l': snake.turnLeft(); break;
    }
  }
  delay(500);
  if(!gamePaused) {
      bool survived = snake.moveForward();
    if(!survived) {
      Serial.println("failed");
      gamePaused = true;
    }
  }
  m.show();
}
