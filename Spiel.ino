#include "FastLED.h"

#define LEDPIN 8
#define LEDCOUNT 256

class Matrix {

  private:
    CRGB leds[LEDCOUNT];
  public:
    Matrix();
    void show();
    void setPixelColor(byte x, byte y, CRGB color);
};

Matrix::Matrix() {
  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, LEDCOUNT);
    for(int i = 0; i < LEDCOUNT; i++) {
      leds[i] = 0x0000;
    }
}

void Matrix::setPixelColor(byte x, byte y, CRGB color) {
  if(x >= 16 || x < 0) return;
  if(y >= 16 || y < 0) return;

  if(y % 2 == 0) {
    leds[16 * y + (15 - x)] = color;

  }else {
    leds[16 * y + x] = color;
  }
}

void Matrix::show() {
  FastLED.show();
}

class Snake {
  public:
    Snake(Matrix* display,int initialLength, byte x, byte y, CRGB color);
    void reset();
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
    mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], 0x000000);
  }

  mLength = mStartLength;
  mDirection = UP;

  for(int i = 0; i < mLength; i++) {
    mSnakePartsX[i] = mStartX;
    mSnakePartsY[i] = mStartY + i;
    mDisplay->setPixelColor(mSnakePartsX[i], mSnakePartsY[i], mColor);
  }
}

bool Snake::moveForward() {

  //letzer Pixel (Schwanz) deaktivieren
  mDisplay->setPixelColor(mSnakePartsX[mLength - 1], mSnakePartsY[mLength - 1], 0x000000);

  for(int i = mLength - 1; i > 0; i--) {
    mSnakePartsX[i] = mSnakePartsX[i - 1];
    mSnakePartsY[i] = mSnakePartsY[i - 1];
  }

  switch(mDirection) {
    case 0: mSnakePartsX[0] = mSnakePartsX[1]; mSnakePartsY[0] = mSnakePartsY[1] - 1; break;
    case 1: mSnakePartsX[0] = mSnakePartsX[1] + 1; mSnakePartsY[0] = mSnakePartsY[1]; break;
    case 2: mSnakePartsX[0] = mSnakePartsX[1]; mSnakePartsY[0] = mSnakePartsY[1] + 1; break;
    case 3: mSnakePartsX[0] = mSnakePartsX[1] - 1; mSnakePartsY[0] = mSnakePartsY[1]; break;
  }

  if(mSnakePartsX[0] < 0 || mSnakePartsX[0] > 15 ||
     mSnakePartsY[0] < 0 || mSnakePartsY[0] > 15) {
      return false;
  }

  //neuen ersten Pixel (Kopf) aktivieren
  mDisplay->setPixelColor(mSnakePartsX[0], mSnakePartsY[0], mColor);
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

bool gamePaused = true;

void setup() {
  Serial.begin(9600);
  snake.moveForward();
  m.show();
}

void loop() {
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
