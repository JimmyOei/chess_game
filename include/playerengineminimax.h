#ifndef PLAYERENGINEMINIMAX_H
#define PLAYERENGINEMINIMAX_H

#include "player.h"

#define DEFAULT_MAX_DEPTH 4

class PlayerEngineMiniMax : public Player
{
public:
  PlayerEngineMiniMax();

  PlayerEngineMiniMax(int maxDepth);

  Move getMove(Game game) override;

private:
  int maxDepth;

  int evaluateGame(Game game);

  int evaluatePieceMobility(Game game);

  int evaluatePiecePlacement(Game game);

  int evaluatePawnStructure(Game game);

  int evaluatePieceValue(Game game);

  std::vector<Move> getAllLegalMoves(Game game);

  std::pair<Move, int> miniMax(Game game, int depth, int alpha, int beta, bool isMaximizingPlayer);
};

#endif