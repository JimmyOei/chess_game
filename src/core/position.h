/**
 * This file contains Position, a square on the board as an index from
 * 0 (a1) to 63 (h8), rank by rank, and kNoPosition for "no square".
 */

#ifndef CHESS_GAME_SRC_CORE_POSITION_H_
#define CHESS_GAME_SRC_CORE_POSITION_H_

#include <cstddef>
#include <ostream>
#include <string>

#include "core/direction.h"

namespace chess {

class Position {
 public:
  constexpr Position() = default;

  constexpr explicit Position(int const index) : index_(index) {}

  // The square on `column` (1 = a to 8 = h) of `row` (1 to 8).
  [[nodiscard]] static constexpr Position FromColumnRow(int const column,
                                                        int const row) {
    return Position((row - 1) * kBoardLength + (column - 1));
  }

  [[nodiscard]] constexpr int Index() const { return index_; }

  // Index for looking the square up in a board array. Requires IsValid().
  [[nodiscard]] constexpr std::size_t ArrayIndex() const {
    return static_cast<std::size_t>(index_);
  }

  [[nodiscard]] constexpr Position operator+(int const offset) const {
    return Position(index_ + offset);
  }

  [[nodiscard]] constexpr Position operator-(int const offset) const {
    return Position(index_ - offset);
  }

  constexpr Position& operator+=(int const offset) {
    index_ += offset;
    return *this;
  }

  constexpr Position& operator-=(int const offset) {
    index_ -= offset;
    return *this;
  }

  constexpr bool operator==(const Position& other) const = default;

  [[nodiscard]] constexpr bool IsValid() const {
    return index_ >= 0 && index_ < kBoardSize;
  }

  [[nodiscard]] constexpr bool IsAtEdge() const {
    return index_ % kBoardLength == 0 ||
           index_ % kBoardLength == kBoardLength - 1 || index_ < kBoardLength ||
           index_ >= kBoardSize - kBoardLength;
  }

  // Rank, from 1 to 8.
  [[nodiscard]] constexpr int GetRow() const {
    return (index_ / kBoardLength) + 1;
  }

  // File, from 1 (a) to 8 (h).
  [[nodiscard]] constexpr int GetColumn() const {
    return (index_ % kBoardLength) + 1;
  }

  // Square in algebraic notation, e.g. "e4".
  [[nodiscard]] std::string ToChessNotation() const {
    return std::string(1, static_cast<char>('a' + GetColumn() - 1)) +
           std::to_string(GetRow());
  }

 private:
  int index_ = 0;
};

// "No square", e.g. no en passant square or the squares of an empty Move.
inline constexpr Position kNoPosition{-1};

inline std::ostream& operator<<(std::ostream& os, const Position& pos) {
  return os << pos.ToChessNotation();
}

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_POSITION_H_
