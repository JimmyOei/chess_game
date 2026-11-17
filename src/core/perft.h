/**
 * This file contains perft: counting the positions reachable from a game in
 * a fixed number of plies, to check move generation against published
 * reference counts (https://www.chessprogramming.org/Perft_Results).
 */

#ifndef CHESS_GAME_SRC_CORE_PERFT_H_
#define CHESS_GAME_SRC_CORE_PERFT_H_

#include <array>
#include <cstdint>
#include <ostream>
#include <string_view>
#include <vector>

#include "core/game.h"

namespace chess {

// Number of positions reached after exactly `depth` plies (the leaf nodes of
// the legal move tree). Perft(game, 0) is 1.
[[nodiscard]] uint64_t Perft(const Game &game, int depth);

struct PerftCounts {
  uint64_t nodes = 0;
  uint64_t checkmates = 0;

  bool operator==(const PerftCounts &other) const = default;

  friend std::ostream &operator<<(std::ostream &os, const PerftCounts &c) {
    return os << c.nodes << " nodes, " << c.checkmates << " checkmates";
  }
};

// Counts for every depth from 1 to `max_depth` in one pass: element d - 1
// holds the positions after d plies and how many of them are checkmate.
// Slower than Perft, since it generates moves at the leaves to find mates.
[[nodiscard]] std::vector<PerftCounts> PerftByDepth(const Game &game,
                                                    int max_depth);

// Published counts for the standard starting position, element d - 1 for
// depth d (https://www.chessprogramming.org/Perft_Results#Initial_Position).
constexpr std::array<PerftCounts, 6> kStartPositionPerft = {{
    {.nodes = 20, .checkmates = 0},
    {.nodes = 400, .checkmates = 0},
    {.nodes = 8'902, .checkmates = 0},
    {.nodes = 197'281, .checkmates = 8},
    {.nodes = 4'865'609, .checkmates = 347},
    {.nodes = 119'060'324, .checkmates = 10'828},
}};

constexpr std::string_view kPerftResultsUrl =
    "https://www.chessprogramming.org/Perft_Results";

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_PERFT_H_
