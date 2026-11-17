/**
 * This file contains PlayerEngineRandom, an engine that plays a uniformly
 * random legal move.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_RANDOM_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_RANDOM_H_

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

#include "core/game.h"
#include "core/move.h"
#include "player/player.h"
#include "util/logger.h"

namespace chess {

class PlayerEngineRandom : public Player {
 public:
  [[nodiscard]] Move GetMove(const Game &game) override {
    CHESS_LOG(LogLevel::kInfo)
        << "Player Engine Random is calculating a random move";
    const std::vector<Move> all_legal_moves = game.GetAllLegalMoves();
    if (all_legal_moves.empty()) {
      CHESS_LOG(LogLevel::kError) << "Engine has no legal moves to make";
      throw std::runtime_error("Engine has no legal moves to make");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dis(0,
                                                   all_legal_moves.size() - 1);
    return all_legal_moves[dis(gen)];
  }
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_ENGINE_RANDOM_H_
