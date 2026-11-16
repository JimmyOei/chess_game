// Smoke tests for Game: checks that the core library links and that move
// generation works from the start position. Rule and perft tests follow in
// step 2 of the migration.

#include <gtest/gtest.h>

#include "game.h"

namespace {

TEST(GameTest, StartPositionHasTwentyLegalMoves) {
  Game game;
  EXPECT_EQ(game.getAllLegalMoves().size(), 20u);
}

TEST(GameTest, StartPositionFromFenHasTwentyLegalMoves) {
  Game game(STANDARD_OPENING_FEN);
  EXPECT_EQ(game.getAllLegalMoves().size(), 20u);
}

}  // namespace
