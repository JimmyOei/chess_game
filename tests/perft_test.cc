// Perft tests for move generation: count the leaf nodes of the legal move
// tree to a fixed depth and compare against the published reference counts
// (https://www.chessprogramming.org/Perft_Results).
// Deep cases only run in optimised builds (NDEBUG), since the debug
// preset runs under ASan/UBSan.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "game.h"
#include "move.h"

namespace {

// Number of leaf nodes reachable from `game` in exactly `depth` plies.
// Recursion is the natural shape here and depth is at most 5 plies.
uint64_t Perft(Game& game, int depth) {  // NOLINT(misc-no-recursion)
  const std::vector<Move> moves = game.getAllLegalMoves();
  if (depth == 1) return moves.size();
  uint64_t nodes = 0;
  for (const Move& move : moves) {
    Game child(game);
    child.makeMove(move);
    nodes += Perft(child, depth - 1);
  }
  return nodes;
}

struct PerftCase {
  std::string_view name;
  std::string_view fen;
  int depth;
  uint64_t nodes;

  // Hidden friend that gtest finds through ADL to print the parameter.
  // [[maybe_unused]] because clangd skips gtest's template bodies, misses
  // that call and reports it as unused; the compilers see the use.
  [[maybe_unused]] friend void PrintTo(const PerftCase& c, std::ostream* os) {
    *os << c.name << " depth " << c.depth;
  }
};

constexpr std::string_view kStartFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
// Castling both ways, en passant, promotions and pins in one position.
constexpr std::string_view kKiwipeteFen =
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
// Sparse endgame: en passant discovered checks, rook/king on the same rank.
constexpr std::string_view kPosition3Fen =
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
// Promotions with capture, black castling only, white king already castled.
constexpr std::string_view kPosition4Fen =
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
constexpr std::string_view kPosition5Fen =
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
constexpr std::string_view kPosition6Fen =
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

const auto kShallowCases = std::to_array<PerftCase>({
    {.name = "Start", .fen = kStartFen, .depth = 1, .nodes = 20},
    {.name = "Start", .fen = kStartFen, .depth = 2, .nodes = 400},
    {.name = "Start", .fen = kStartFen, .depth = 3, .nodes = 8'902},
    {.name = "Kiwipete", .fen = kKiwipeteFen, .depth = 1, .nodes = 48},
    {.name = "Kiwipete", .fen = kKiwipeteFen, .depth = 2, .nodes = 2'039},
    {.name = "Position3", .fen = kPosition3Fen, .depth = 1, .nodes = 14},
    {.name = "Position3", .fen = kPosition3Fen, .depth = 2, .nodes = 191},
    {.name = "Position3", .fen = kPosition3Fen, .depth = 3, .nodes = 2'812},
    {.name = "Position4", .fen = kPosition4Fen, .depth = 1, .nodes = 6},
    {.name = "Position4", .fen = kPosition4Fen, .depth = 2, .nodes = 264},
    {.name = "Position5", .fen = kPosition5Fen, .depth = 1, .nodes = 44},
    {.name = "Position5", .fen = kPosition5Fen, .depth = 2, .nodes = 1'486},
    {.name = "Position6", .fen = kPosition6Fen, .depth = 1, .nodes = 46},
    {.name = "Position6", .fen = kPosition6Fen, .depth = 2, .nodes = 2'079},
});

#ifdef NDEBUG
const auto kDeepCases = std::to_array<PerftCase>({
    {.name = "Start", .fen = kStartFen, .depth = 4, .nodes = 197'281},
    {.name = "Start", .fen = kStartFen, .depth = 5, .nodes = 4'865'609},
    {.name = "Kiwipete", .fen = kKiwipeteFen, .depth = 3, .nodes = 97'862},
    {.name = "Kiwipete", .fen = kKiwipeteFen, .depth = 4, .nodes = 4'085'603},
    {.name = "Position3", .fen = kPosition3Fen, .depth = 4, .nodes = 43'238},
    {.name = "Position3", .fen = kPosition3Fen, .depth = 5, .nodes = 674'624},
    {.name = "Position4", .fen = kPosition4Fen, .depth = 3, .nodes = 9'467},
    {.name = "Position4", .fen = kPosition4Fen, .depth = 4, .nodes = 422'333},
    {.name = "Position5", .fen = kPosition5Fen, .depth = 3, .nodes = 62'379},
    {.name = "Position6", .fen = kPosition6Fen, .depth = 3, .nodes = 89'890},
});
#endif

class PerftTest : public testing::TestWithParam<PerftCase> {};

TEST_P(PerftTest, MatchesReferenceNodeCount) {
  const PerftCase& c = GetParam();
  Game game{std::string(c.fen)};
  EXPECT_EQ(Perft(game, c.depth), c.nodes);
}

std::string CaseName(const testing::TestParamInfo<PerftCase>& info) {
  return std::string(info.param.name) + "Depth" +
         std::to_string(info.param.depth);
}

INSTANTIATE_TEST_SUITE_P(Shallow, PerftTest, testing::ValuesIn(kShallowCases),
                         CaseName);

#ifdef NDEBUG
INSTANTIATE_TEST_SUITE_P(Deep, PerftTest, testing::ValuesIn(kDeepCases),
                         CaseName);
#endif

}  // namespace
