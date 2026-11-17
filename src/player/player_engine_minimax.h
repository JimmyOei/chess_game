/**
 * This file contains PlayerEngineMiniMax, an engine that searches a fixed
 * number of plies with minimax and alpha-beta pruning, and its evaluation.
 *
 * Scores are in centipawns (a pawn is 100) from white's point of view:
 * positive is good for white.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_

#include <chrono>
#include <cstdint>
#include <optional>
#include <utility>

#include "core/game.h"
#include "core/move.h"
#include "player/player.h"

namespace chess {

class PlayerEngineMiniMax : public Player {
 public:
  // Thinking time per move of the default engine.
  static constexpr std::chrono::milliseconds kDefaultTimeLimit{3000};

  // Deepest search, also for a time-limited engine.
  static constexpr int kMaxSearchDepth = 64;

  // Score of a checkmate
  static constexpr int kMateScore = 1'000'000;

  // Searches deeper and deeper (iterative deepening) until kDefaultTimeLimit
  // is used up, then plays the best move of the deepest completed search.
  PlayerEngineMiniMax() = default;

  // Searches exactly `max_depth` plies, however long that takes. The result
  // doesn't depend on the machine's speed, so tests use this.
  explicit PlayerEngineMiniMax(int max_depth)
      : max_depth_(max_depth), time_limit_(std::nullopt) {}

  explicit PlayerEngineMiniMax(std::chrono::milliseconds time_limit)
      : time_limit_(time_limit) {}

  [[nodiscard]] Move GetMove(const Game& game) override;

  // What the last GetMove() call searched.
  struct SearchStats {
    // Deepest search that completed, in plies.
    int depth = 0;
    // Positions visited, including the unfinished deepest search.
    uint64_t nodes = 0;
    std::chrono::milliseconds elapsed{0};
    // Score of the chosen move (see Evaluate()).
    int score = 0;
  };

  [[nodiscard]] const SearchStats& GetLastSearchStats() const {
    return last_search_stats_;
  }

  // Static evaluation of `game`: -kMateScore or +kMateScore if a side is
  // checkmated, 0 for a draw, otherwise the sum of the terms below.
  [[nodiscard]] static int Evaluate(const Game& game);

  // Material: the piece values of white minus those of black.
  [[nodiscard]] static int EvaluatePieceValue(const Game& game);

  // Piece-square tables: bonuses for good squares
  [[nodiscard]] static int EvaluatePiecePlacement(const Game& game);

  // White's pseudo-legal move count minus black's, weighted.
  [[nodiscard]] static int EvaluatePieceMobility(const Game& game);

  // Penalty for doubled pawns.
  [[nodiscard]] static int EvaluatePawnStructure(const Game& game);

 private:
  // State of one GetMove() call, shared by the whole search tree.
  struct Search {
    int max_depth = 0;
    std::optional<std::chrono::steady_clock::time_point> deadline;
    // Searched first at the root: the best move of the previous depth.
    std::optional<Move> first_root_move;
    uint64_t nodes = 0;
    // Set once the deadline passes; the unfinished search is then discarded.
    bool aborted = false;
  };

  int max_depth_ = kMaxSearchDepth;
  std::optional<std::chrono::milliseconds> time_limit_ = kDefaultTimeLimit;
  SearchStats last_search_stats_;

  // Best move and its score for the side to move in `game`, searching
  // `search.max_depth - depth` more plies. `depth` is the number of plies
  // from the root.
  [[nodiscard]] static std::pair<Move, int> MiniMax(const Game& game, int depth,
                                                    int alpha, int beta,
                                                    Search& search);
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_MINIMAX_H_
