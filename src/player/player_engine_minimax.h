/**
 * This file contains PlayerEngineMiniMax, an engine that searches a fixed
 * number of plies with minimax and alpha-beta pruning, and its evaluation.
 *
 * Scores are in centipawns (a pawn is 100) from white's point of view:
 * positive is good for white.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_

#include <utility>

#include "core/game.h"
#include "core/move.h"
#include "player/player.h"

namespace chess {

class PlayerEngineMiniMax : public Player {
 public:
  static constexpr int kDefaultMaxDepth = 4;

  // Score of a checkmate
  static constexpr int kMateScore = 1'000'000;

  PlayerEngineMiniMax() = default;

  explicit PlayerEngineMiniMax(int max_depth) : max_depth_(max_depth) {}

  [[nodiscard]] Move GetMove(const Game& game) override;

  // Static evaluation of `game`: -kMateScore or +kMateScore if a side is
  // checkmated, 0 for a draw, otherwise the sum of the terms below.
  [[nodiscard]] static int Evaluate(const Game& game);

  // Material: the piece values of white minus those of black.
  [[nodiscard]] static int EvaluatePieceValue(const Game& game);

  // Piece-square tables: bonuses for good squares
  [[nodiscard]] static int EvaluatePiecePlacement(const Game& game);

  // White's legal-move count minus black's, weighted.
  [[nodiscard]] static int EvaluatePieceMobility(const Game& game);

  // Penalty for doubled pawns.
  [[nodiscard]] static int EvaluatePawnStructure(const Game& game);

 private:
  int max_depth_ = kDefaultMaxDepth;

  // Best move and its score for the side to move in `game`, searching
  // `max_depth_ - depth` more plies. `depth` is the number of plies from the
  // root.
  [[nodiscard]] std::pair<Move, int> MiniMax(const Game& game, int depth,
                                             int alpha, int beta) const;
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
