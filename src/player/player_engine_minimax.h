/**
 * This file contains PlayerEngineMiniMax, an engine that searches a fixed
 * number of plies with minimax and alpha-beta pruning.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_

#include <utility>
#include <vector>

#include "core/game.h"
#include "core/move.h"
#include "player/player.h"

namespace chess {

class PlayerEngineMiniMax : public Player {
 public:
  static constexpr int kDefaultMaxDepth = 4;

  PlayerEngineMiniMax() = default;

  explicit PlayerEngineMiniMax(int max_depth) : max_depth_(max_depth) {}

  [[nodiscard]] Move GetMove(const Game &game) override;

 private:
  int max_depth_ = kDefaultMaxDepth;

  // Score from white's point of view: positive is good for white.
  [[nodiscard]] static int EvaluateGame(const Game &game);

  [[nodiscard]] static int EvaluatePieceMobility(const Game &game);

  [[nodiscard]] static int EvaluatePiecePlacement(const Game &game);

  [[nodiscard]] static int EvaluatePawnStructure(const Game &game);

  [[nodiscard]] static int EvaluatePieceValue(const Game &game);

  // Legal moves for the side to move. Throws if there are none.
  [[nodiscard]] static std::vector<Move> GetAllLegalMoves(const Game &game);

  [[nodiscard]] std::pair<Move, int> MiniMax(const Game &game, int depth,
                                             int alpha, int beta,
                                             bool is_maximizing_player) const;
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
