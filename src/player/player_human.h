/**
 * This file contains PlayerHuman, a player whose moves come from dragging
 * pieces in the interface.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_HUMAN_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_HUMAN_H_

#include <stdexcept>

#include "core/game.h"
#include "core/move.h"
#include "player/player.h"
#include "util/logger.h"

namespace chess {

class PlayerHuman : public Player {
 public:
  [[nodiscard]] Move GetMove(const Game& /*game*/) override {
    CHESS_LOG(LogLevel::kError)
        << "PlayerHuman::GetMove() should not be called, it does nothing.";
    throw std::runtime_error(
        "PlayerHuman::GetMove() should not be called, it does nothing.");
  }

  [[nodiscard]] bool IsHuman() const override { return true; }
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_HUMAN_H_
