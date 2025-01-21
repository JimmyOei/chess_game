#ifndef PLAYERHUMAN_H
#define PLAYERHUMAN_H

#include "player.h"

class PlayerHuman : public Player
{
public:
  Move getMove(const Game *game) override;
};

#endif