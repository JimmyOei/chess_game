#ifndef PLAYERHUMAN_H
#define PLAYERHUMAN_H

#include "player.h"

class PlayerHuman : public Player
{
public:
  Move getMove(Game *game) override {
    logIt(LogLevel::ERROR) << "PlayerHuman::getMove() should not be called, it does nothing.";
    throw std::runtime_error("PlayerHuman::getMove() should not be called, it does nothing.");
  }

  virtual bool isHuman() const override { return true; }
};

#endif