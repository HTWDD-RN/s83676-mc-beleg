#include "statistics.h"

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
