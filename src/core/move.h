/**
 * This file contains Move: a piece moving from one square to another, with
 * the piece it promotes to, if any.
 */

#ifndef CHESS_GAME_SRC_CORE_MOVE_H_
#define CHESS_GAME_SRC_CORE_MOVE_H_

#include <ostream>

#include "core/piece.h"
#include "core/position.h"

namespace chess {

struct Move {
  Position from = kNoPosition;
  Position to = kNoPosition;
  Piece piece = Piece::kNone;
  Piece promotion_piece = Piece::kNone;

  Move() = default;

  Move(Position from_pos, Position to_pos, Piece moved_piece,
       Piece promotion = Piece::kNone)
      : from(from_pos),
        to(to_pos),
        piece(moved_piece),
        promotion_piece(promotion) {}

  bool operator==(Move const& other) const {
    return from == other.from && to == other.to && piece == other.piece &&
           promotion_piece == other.promotion_piece;
  }
};

inline std::ostream& operator<<(std::ostream& os, Move const& move) {
  os << PieceToChar(move.piece) << move.from << "-" << move.to;
  if (move.promotion_piece != Piece::kNone) {
    os << " -> " << PieceToChar(move.promotion_piece);
  }
  return os;
}

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_MOVE_H_
