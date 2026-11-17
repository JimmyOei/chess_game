/**
 * This file contains Player, the interface for anything that picks moves:
 * a human at the board or an engine.
 */

#ifndef CHESS_GAME_SRC_PLAYER_PLAYER_H_
#define CHESS_GAME_SRC_PLAYER_PLAYER_H_

#include "core/game.h"
#include "core/move.h"

namespace chess {

class Player {
 public:
  Player() = default;
  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;
  Player(Player &&) = delete;
  Player &operator=(Player &&) = delete;
  virtual ~Player() = default;

  // Picks a legal move for the side to move in `game`.
  [[nodiscard]] virtual Move GetMove(const Game &game) = 0;

  // Humans move by dragging pieces in the interface, not through GetMove().
  [[nodiscard]] virtual bool IsHuman() const { return false; }
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_PLAYER_PLAYER_H_
