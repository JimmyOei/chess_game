/**
 * This file contains the perft implementations.
 */

#include "core/perft.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/game.h"
#include "core/move.h"

namespace chess {

uint64_t Perft(const Game &game, int const depth) {
  if (depth == 0) {
    return 1;
  }
  const std::vector<Move> moves = game.GetAllLegalMoves();
  if (depth == 1) {
    return moves.size();  // Bulk counting: no need to play the last ply.
  }
  uint64_t nodes = 0;
  for (const Move &move : moves) {
    Game child(game);
    child.MakeMove(move);
    nodes += Perft(child, depth - 1);
  }
  return nodes;
}

namespace {

// Adds the subtree below `game`, a position reached after `depth` plies.
void CountSubtree(const Game &game, int const depth, int const max_depth,
                  std::vector<PerftCounts> &counts) {
  const std::vector<Move> moves = game.GetAllLegalMoves();
  if (depth > 0 && moves.empty() && game.IsKingInCheck(game.GetTurn())) {
    counts[static_cast<std::size_t>(depth - 1)].checkmates++;
  }
  if (depth == max_depth) {
    return;
  }
  for (const Move &move : moves) {
    Game child(game);
    child.MakeMove(move);
    counts[static_cast<std::size_t>(depth)].nodes++;
    CountSubtree(child, depth + 1, max_depth, counts);
  }
}

}  // namespace

std::vector<PerftCounts> PerftByDepth(const Game &game, int const max_depth) {
  std::vector<PerftCounts> counts(
      static_cast<std::size_t>(max_depth > 0 ? max_depth : 0));
  if (max_depth > 0) {
    CountSubtree(game, 0, max_depth, counts);
  }
  return counts;
}

}  // namespace chess
