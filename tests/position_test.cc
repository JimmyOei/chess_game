// Tests for Position: square indexing, offsets, and algebraic notation.

#include "core/position.h"

#include <gtest/gtest.h>

#include "core/direction.h"

namespace chess {
namespace {

TEST(PositionTest, IndexesRankByRankFromA1) {
  EXPECT_EQ(Position::FromColumnRow(1, 1).Index(), 0);   // a1
  EXPECT_EQ(Position::FromColumnRow(8, 1).Index(), 7);   // h1
  EXPECT_EQ(Position::FromColumnRow(1, 2).Index(), 8);   // a2
  EXPECT_EQ(Position::FromColumnRow(8, 8).Index(), 63);  // h8
}

TEST(PositionTest, RowAndColumnAreOneBased) {
  const Position e4 = Position::FromColumnRow(5, 4);
  EXPECT_EQ(e4.GetColumn(), 5);
  EXPECT_EQ(e4.GetRow(), 4);
  EXPECT_EQ(e4.ToChessNotation(), "e4");
}

TEST(PositionTest, AddingAnOffsetMovesTheSquare) {
  const Position e4 = Position::FromColumnRow(5, 4);
  EXPECT_EQ(e4 + direction::kNorth, Position::FromColumnRow(5, 5));
  EXPECT_EQ(e4 + direction::kSouthWest, Position::FromColumnRow(4, 3));
  EXPECT_EQ(e4 - direction::kEast, Position::FromColumnRow(4, 4));

  Position square = e4;
  square += direction::kWest;
  EXPECT_EQ(square, Position::FromColumnRow(4, 4));
}

TEST(PositionTest, OnlyBoardSquaresAreValid) {
  EXPECT_TRUE(Position(0).IsValid());
  EXPECT_TRUE(Position(kBoardSize - 1).IsValid());
  EXPECT_FALSE(Position(kBoardSize).IsValid());
  EXPECT_FALSE(kNoPosition.IsValid());
  EXPECT_FALSE((Position::FromColumnRow(5, 8) + direction::kNorth).IsValid());
}

}  // namespace
}  // namespace chess
