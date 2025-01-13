#ifndef PLAYER_H
#define PLAYER_H

#include <move.h>
#include <state.h>

class Player
{
public:
  virtual ~Player() = default;

  virtual Move getMove(const State *state) = 0;
};

#endif