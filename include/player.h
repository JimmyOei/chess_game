#ifndef PLAYER_H
#define PLAYER_H

#include "move.h"

class Player
{
public:
  virtual ~Player() = default;

  virtual Move getMove(const Game *game) = 0;
};

#endif