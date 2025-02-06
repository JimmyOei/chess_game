#ifndef PLAYERENGINERANDOM_H
#define PLAYERENGINERANDOM_H

#include <random>

#include "player.h"

class PlayerEngineRandom : public Player
{
public:
  Move getMove(Game game) override
  {
    logIt(LogLevel::INFO) << "Player Engine Random is calculating a random move";
    std::vector<Move> allLegalMoves;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
      std::vector<Move> legalMoves = game.getLegalMovesForPos(i);
      if(legalMoves.size() > 0) {
        allLegalMoves.insert(allLegalMoves.end(), legalMoves.begin(), legalMoves.end());
      }
    }

    if (allLegalMoves.size() == 0)
    {
      logIt(LogLevel::ERROR) << "Engine has no legal moves to make";
      throw std::runtime_error("Engine has no legal moves to make");
    }

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, allLegalMoves.size() - 1);

    return allLegalMoves[dis(gen)];
  }
};

#endif