// Tests for PlayerEngineMiniMax: the moves it picks in positions with a
// clear best move, and properties of its evaluation (symmetry, draws).

#include "player/player_engine_minimax.h"

#include <gtest/gtest.h>

#include <cctype>
#include <string>
#include <string_view>

#include "core/game.h"
#include "core/move.h"
#include "core/position.h"

namespace chess {
namespace {

Position Sq(std::string_view square) {
  return Position::FromColumnRow(square[0] - 'a' + 1, square[1] - '1' + 1);
}

// The same position with the colours swapped: ranks mirrored, piece colours
// swapped, and the other side to move. Its evaluation must be the negation.
std::string MirrorFen(std::string_view fen) {
  std::string_view const board = fen.substr(0, fen.find(' '));
  std::string mirrored;
  std::size_t end = board.size();
  while (true) {
    std::size_t const start = board.rfind('/', end - 1);
    std::size_t const begin = start == std::string_view::npos ? 0 : start + 1;
    for (char const c : board.substr(begin, end - begin)) {
      mirrored += std::isupper(static_cast<unsigned char>(c))
                      ? static_cast<char>(std::tolower(c))
                      : static_cast<char>(std::toupper(c));
    }
    if (start == std::string_view::npos) {
      break;
    }
    mirrored += '/';
    end = start;
  }
  char const side = fen[board.size() + 1] == 'w' ? 'b' : 'w';
  return mirrored + ' ' + side + " - - 0 1";
}

// Plays the engine's move for the side to move and returns the new game.
Game PlayEngineMove(const Game& game, int depth) {
  PlayerEngineMiniMax engine(depth);
  Game after(game);
  after.MakeMove(engine.GetMove(game));
  return after;
}

TEST(PlayerEngineMiniMaxTest, MirrorFenSwapsColours) {
  EXPECT_EQ(MirrorFen("4k3/8/8/8/8/8/4P3/R3K3 w - - 0 1"),
            "r3k3/4p3/8/8/8/8/8/4K3 b - - 0 1");
}

TEST(PlayerEngineMiniMaxTest, StartPositionEvaluatesToZero) {
  EXPECT_EQ(PlayerEngineMiniMax::Evaluate(Game()), 0);
}

TEST(PlayerEngineMiniMaxTest, EvaluationIsColourSymmetric) {
  for (std::string_view const fen : {
           "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w - - 0 1",
           "rnbqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w - - 0 1",
           "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
       }) {
    EXPECT_EQ(PlayerEngineMiniMax::Evaluate(Game(fen)),
              -PlayerEngineMiniMax::Evaluate(Game(MirrorFen(fen))))
        << fen;
  }
}

TEST(PlayerEngineMiniMaxTest, StalemateEvaluatesAsDraw) {
  // Black to move has no legal moves and is not in check.
  EXPECT_EQ(
      PlayerEngineMiniMax::Evaluate(Game("k7/8/1Q6/8/8/8/8/7K b - - 0 1")), 0);
}

TEST(PlayerEngineMiniMaxTest, CheckmateOutweighsAnyMaterial) {
  // Fool's mate: white is mated, with equal material.
  EXPECT_LT(
      PlayerEngineMiniMax::Evaluate(Game(
          "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3")),
      -10'000);
  // Back-rank mate by a lone rook against three pawns.
  EXPECT_GT(
      PlayerEngineMiniMax::Evaluate(Game("R5k1/5ppp/8/8/8/8/8/6K1 b - - 0 1")),
      10'000);
}

TEST(PlayerEngineMiniMaxTest, CentralKnightBeatsCornerKnight) {
  // The pawns keep this from being a draw by insufficient material.
  EXPECT_GT(PlayerEngineMiniMax::Evaluate(
                Game("4k3/4p3/8/8/3N4/8/4P3/4K3 w - - 0 1")),
            PlayerEngineMiniMax::Evaluate(
                Game("4k3/4p3/8/8/8/8/4P3/N3K3 w - - 0 1")));
}

TEST(PlayerEngineMiniMaxTest, MobilityCountsBothSides) {
  EXPECT_EQ(PlayerEngineMiniMax::EvaluatePieceMobility(Game()), 0);
  // After 1. e4 white has 30 moves and black 20.
  const Game after_e4(
      "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
  EXPECT_GT(PlayerEngineMiniMax::EvaluatePieceMobility(after_e4), 0);
  EXPECT_EQ(
      PlayerEngineMiniMax::EvaluatePieceMobility(after_e4),
      -PlayerEngineMiniMax::EvaluatePieceMobility(
          Game("rnbqkbnr/pppp1ppp/8/4p3/8/8/PPPPPPPP/RNBQKBNR w KQkq e6 0 1")));
}

TEST(PlayerEngineMiniMaxTest, WhiteFindsMateInOne) {
  for (int const depth : {1, 2, 3}) {
    Game after =
        PlayEngineMove(Game("6k1/5ppp/8/8/8/8/8/R5K1 w - - 0 1"), depth);
    EXPECT_TRUE(after.IsGameOver()) << "depth " << depth;
    EXPECT_EQ(after.GetResult(), Result::kWhiteWin) << "depth " << depth;
  }
}

TEST(PlayerEngineMiniMaxTest, BlackFindsMateInOne) {
  for (int const depth : {1, 2, 3}) {
    Game after =
        PlayEngineMove(Game("r5k1/8/8/8/8/8/5PPP/6K1 b - - 0 1"), depth);
    EXPECT_TRUE(after.IsGameOver()) << "depth " << depth;
    EXPECT_EQ(after.GetResult(), Result::kBlackWin) << "depth " << depth;
  }
}

TEST(PlayerEngineMiniMaxTest, PrefersMateOverStalemate) {
  // Qg6 stalemates; Qh6 and Qh3 mate.
  Game after = PlayEngineMove(Game("7k/5K2/4Q3/8/8/8/8/8 w - - 0 1"), 1);
  EXPECT_TRUE(after.IsGameOver());
  EXPECT_EQ(after.GetResult(), Result::kWhiteWin);
}

TEST(PlayerEngineMiniMaxTest, DoesNotStalemateWhenWinning) {
  // Qb6 stalemates the a8 king, and there is no mate in one.
  for (int const depth : {1, 2}) {
    Game after = PlayEngineMove(Game("k7/8/8/8/8/8/8/1Q5K w - - 0 1"), depth);
    EXPECT_FALSE(after.IsGameOver()) << "depth " << depth;
    EXPECT_NE(after.GetPieceAtPos(Sq("b6")), Piece::kWhiteQueen)
        << "depth " << depth;
  }
}

}  // namespace
}  // namespace chess
