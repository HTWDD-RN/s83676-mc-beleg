#ifndef STATISTICS_H
#define STATISTICS_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "game_settings.h"

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

#endif
