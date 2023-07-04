#include "FastLED.h"
#include <LiquidCrystal.h>
#include "game_settings.h"
#include "hardware_settings.h"

#include "item.h"
#include "matrix.h"
#include "player.h"
#include "snake.h"
#include "statistics.h"

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
