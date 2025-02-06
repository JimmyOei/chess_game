#ifndef PLAYER_H
#define PLAYER_H

#include "move.h"
#include "game.h"
class Player
{
public:
  virtual ~Player() = default;

  virtual Move getMove(Game game) = 0;

  virtual bool isHuman() const { return false; };
};

#endif