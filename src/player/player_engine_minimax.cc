/**
 * This file contains the PlayerEngineMiniMax implementation: the evaluation
 * terms and the minimax search with alpha-beta pruning.
 *
 * Piece values and piece-square tables are from Tomasz Michniewski's
 * "Simplified Evaluation Function"
 * (https://www.chessprogramming.org/Simplified_Evaluation_Function).
 */

#include "player/player_engine_minimax.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "core/direction.h"
#include "core/game.h"
#include "core/move.h"
#include "core/piece.h"
#include "core/position.h"
#include "util/logger.h"

namespace chess {
namespace {

// Centipawns per legal move of difference in mobility.
constexpr int kMobilityWeight = 2;

// Centipawns lost for each extra pawn of a colour on the same file.
constexpr int kDoubledPawnPenalty = 20;

// Material value of a piece type. The king has none: it is never captured.
constexpr int PieceValue(PieceType const type) {
  switch (type) {
    case PieceType::kPawn:
      return 100;
    case PieceType::kKnight:
      return 320;
    case PieceType::kBishop:
      return 330;
    case PieceType::kRook:
      return 500;
    case PieceType::kQueen:
      return 900;
    case PieceType::kKing:
    case PieceType::kNone:
      return 0;
  }
  return 0;
}

// +1 for white and -1 for black: scores are from white's point of view.
constexpr int Sign(Color const color) {
  return color == Color::kWhite ? 1 : -1;
}

using SquareTable = std::array<int, kBoardSize>;

// Piece-square tables for white, written as the board is seen from white's
// side: the first row is rank 8 and the last is rank 1. Black uses them
// mirrored (see PlacementBonus()). The king table is for the middlegame.
constexpr SquareTable kPawnTable = {
    0,  0,  0,   0,   0,   0,   0,  0,   //
    50, 50, 50,  50,  50,  50,  50, 50,  //
    10, 10, 20,  30,  30,  20,  10, 10,  //
    5,  5,  10,  25,  25,  10,  5,  5,   //
    0,  0,  0,   20,  20,  0,   0,  0,   //
    5,  -5, -10, 0,   0,   -10, -5, 5,   //
    5,  10, 10,  -20, -20, 10,  10, 5,   //
    0,  0,  0,   0,   0,   0,   0,  0,
};
constexpr SquareTable kKnightTable = {
    -50, -40, -30, -30, -30, -30, -40, -50,  //
    -40, -20, 0,   0,   0,   0,   -20, -40,  //
    -30, 0,   10,  15,  15,  10,  0,   -30,  //
    -30, 5,   15,  20,  20,  15,  5,   -30,  //
    -30, 0,   15,  20,  20,  15,  0,   -30,  //
    -30, 5,   10,  15,  15,  10,  5,   -30,  //
    -40, -20, 0,   5,   5,   0,   -20, -40,  //
    -50, -40, -30, -30, -30, -30, -40, -50,
};
constexpr SquareTable kBishopTable = {
    -20, -10, -10, -10, -10, -10, -10, -20,  //
    -10, 0,   0,   0,   0,   0,   0,   -10,  //
    -10, 0,   5,   10,  10,  5,   0,   -10,  //
    -10, 5,   5,   10,  10,  5,   5,   -10,  //
    -10, 0,   10,  10,  10,  10,  0,   -10,  //
    -10, 10,  10,  10,  10,  10,  10,  -10,  //
    -10, 5,   0,   0,   0,   0,   5,   -10,  //
    -20, -10, -10, -10, -10, -10, -10, -20,
};
constexpr SquareTable kRookTable = {
    0,  0,  0,  0,  0,  0,  0,  0,   //
    5,  10, 10, 10, 10, 10, 10, 5,   //
    -5, 0,  0,  0,  0,  0,  0,  -5,  //
    -5, 0,  0,  0,  0,  0,  0,  -5,  //
    -5, 0,  0,  0,  0,  0,  0,  -5,  //
    -5, 0,  0,  0,  0,  0,  0,  -5,  //
    -5, 0,  0,  0,  0,  0,  0,  -5,  //
    0,  0,  0,  5,  5,  0,  0,  0,
};
constexpr SquareTable kQueenTable = {
    -20, -10, -10, -5, -5, -10, -10, -20,  //
    -10, 0,   0,   0,  0,  0,   0,   -10,  //
    -10, 0,   5,   5,  5,  5,   0,   -10,  //
    -5,  0,   5,   5,  5,  5,   0,   -5,   //
    0,   0,   5,   5,  5,  5,   0,   -5,   //
    -10, 5,   5,   5,  5,  5,   0,   -10,  //
    -10, 0,   5,   0,  0,  0,   0,   -10,  //
    -20, -10, -10, -5, -5, -10, -10, -20,
};
constexpr SquareTable kKingTable = {
    -30, -40, -40, -50, -50, -40, -40, -30,  //
    -30, -40, -40, -50, -50, -40, -40, -30,  //
    -30, -40, -40, -50, -50, -40, -40, -30,  //
    -30, -40, -40, -50, -50, -40, -40, -30,  //
    -20, -30, -30, -40, -40, -30, -30, -20,  //
    -10, -20, -20, -20, -20, -20, -20, -10,  //
    20,  20,  0,   0,   0,   0,   20,  20,   //
    20,  30,  10,  0,   0,   10,  30,  20,
};

constexpr const SquareTable& TableFor(PieceType const type) {
  switch (type) {
    case PieceType::kPawn:
      return kPawnTable;
    case PieceType::kKnight:
      return kKnightTable;
    case PieceType::kBishop:
      return kBishopTable;
    case PieceType::kRook:
      return kRookTable;
    case PieceType::kQueen:
      return kQueenTable;
    case PieceType::kKing:
    case PieceType::kNone:
      break;
  }
  return kKingTable;
}

// Bonus for `piece` standing on `pos`, from that piece's own point of view.
int PlacementBonus(Piece const piece, Position const pos) {
  // Rank as seen from the piece's own side: 1 is its home rank.
  int const own_row = ColorOf(piece) == Color::kWhite
                          ? pos.GetRow()
                          : kBoardLength + 1 - pos.GetRow();
  int const index =
      (kBoardLength - own_row) * kBoardLength + (pos.GetColumn() - 1);
  return TableFor(TypeOf(piece))[static_cast<std::size_t>(index)];
}

// Search-order key for `move` in `game`: higher is searched first. Captures
// come first, most valuable victim and then least valuable attacker first
// ("MVV-LVA"), then promotions, then quiet moves. Alpha-beta prunes far more
// when good moves are searched early.
int MoveOrderKey(const Game& game, const Move& move) {
  int key = 0;
  Piece const victim = game.GetPieceAtPos(move.to);
  if (victim != Piece::kNone) {
    key += 10 * PieceValue(TypeOf(victim)) - PieceValue(TypeOf(move.piece));
  } else if (TypeOf(move.piece) == PieceType::kPawn &&
             move.to == game.GetEnPassantPos()) {
    key += 10 * PieceValue(PieceType::kPawn) - PieceValue(PieceType::kPawn);
  }
  if (move.promotion_piece != Piece::kNone) {
    key += PieceValue(TypeOf(move.promotion_piece));
  }
  return key;
}

// `moves` sorted by MoveOrderKey(), best first. Stable, so equal keys keep
// the move generator's order and the search stays deterministic.
std::vector<Move> OrderMoves(const Game& game, std::vector<Move> moves) {
  std::vector<std::pair<int, Move>> keyed;
  keyed.reserve(moves.size());
  for (const Move& move : moves) {
    keyed.emplace_back(MoveOrderKey(game, move), move);
  }
  std::ranges::stable_sort(
      keyed, [](const auto& a, const auto& b) { return a.first > b.first; });
  for (std::size_t i = 0; i < keyed.size(); i++) {
    moves[i] = keyed[i].second;
  }
  return moves;
}

}  // namespace

Move PlayerEngineMiniMax::GetMove(const Game& game) {
  CHESS_LOG(LogLevel::kInfo) << "Player Engine MiniMax is calculating a move";
  CHESS_LOG(LogLevel::kInfo)
      << "Current score: " << Evaluate(game) << " turn: " << game.GetTurn();

  auto const start = std::chrono::steady_clock::now();
  Search search;
  if (time_limit_) {
    search.deadline = start + *time_limit_;
  }

  // Iterative deepening: each depth searches the previous depth's best move
  // first. Depth 1 always completes, so there is always a move to play.
  std::pair<Move, int> best_move;
  int completed_depth = 0;
  for (int depth = 1; depth <= max_depth_; depth++) {
    search.max_depth = depth;
    const std::pair<Move, int> result =
        MiniMax(game, 0, std::numeric_limits<int>::min(),
                std::numeric_limits<int>::max(), search);
    if (search.aborted && depth > 1) {
      break;  // Out of time: keep the last completed depth's move.
    }
    best_move = result;
    completed_depth = depth;
    search.first_root_move = result.first;
    if (std::abs(result.second) > kMateScore - kMaxSearchDepth) {
      break;  // A forced mate: searching deeper can't improve on it.
    }
  }

  if (best_move.first.from == kNoPosition) {
    CHESS_LOG(LogLevel::kError) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }
  last_search_stats_ = {
      .depth = completed_depth,
      .nodes = search.nodes,
      .elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - start),
      .score = best_move.second,
  };
  CHESS_LOG(LogLevel::kInfo)
      << "Player Engine MiniMax made move " << best_move.first
      << " with eval score " << best_move.second << " (depth "
      << completed_depth << ", " << search.nodes << " nodes, "
      << last_search_stats_.elapsed.count() << " ms)";

  return best_move.first;
}

int PlayerEngineMiniMax::EvaluatePieceValue(const Game& game) {
  int score = 0;
  for (int i = 0; i < kBoardSize; i++) {
    Piece const piece = game.GetPieceAtPos(Position(i));
    if (piece != Piece::kNone) {
      score += Sign(ColorOf(piece)) * PieceValue(TypeOf(piece));
    }
  }
  return score;
}

int PlayerEngineMiniMax::EvaluatePiecePlacement(const Game& game) {
  int score = 0;
  for (int i = 0; i < kBoardSize; i++) {
    Position const pos(i);
    Piece const piece = game.GetPieceAtPos(pos);
    if (piece != Piece::kNone) {
      score += Sign(ColorOf(piece)) * PlacementBonus(piece, pos);
    }
  }
  return score;
}

int PlayerEngineMiniMax::EvaluatePieceMobility(const Game& game) {
  int score = 0;
  for (Color const color : {Color::kWhite, Color::kBlack}) {
    score += Sign(color) * kMobilityWeight * game.CountPseudoLegalMoves(color);
  }
  return score;
}

int PlayerEngineMiniMax::EvaluatePawnStructure(const Game& game) {
  int score = 0;
  for (int column = 1; column <= kBoardLength; column++) {
    for (Color const color : {Color::kWhite, Color::kBlack}) {
      int pawns = 0;
      for (int row = 1; row <= kBoardLength; row++) {
        if (game.GetPieceAtPos(Position::FromColumnRow(column, row)) ==
            MakePiece(color, PieceType::kPawn)) {
          pawns++;
        }
      }
      if (pawns > 1) {
        score -= Sign(color) * kDoubledPawnPenalty * (pawns - 1);
      }
    }
  }
  return score;
}

int PlayerEngineMiniMax::Evaluate(const Game& game) {
  Game game_copy(game);
  if (game_copy.IsGameOver()) {
    switch (game_copy.GetResult()) {
      case Result::kWhiteWin:
        return kMateScore;
      case Result::kBlackWin:
        return -kMateScore;
      case Result::kDraw:
      case Result::kOngoing:
        return 0;
    }
  }
  return EvaluatePieceValue(game) + EvaluatePiecePlacement(game) +
         EvaluatePieceMobility(game) + EvaluatePawnStructure(game);
}

std::pair<Move, int> PlayerEngineMiniMax::MiniMax(const Game& game,
                                                  int const depth, int alpha,
                                                  int beta, Search& search) {
  // Looking at the clock is slow, so only every 1024 nodes. Depth 1 is never
  // aborted, so GetMove() always has a move.
  constexpr uint64_t kClockCheckInterval = 1024;
  search.nodes++;
  if (search.deadline && search.max_depth > 1 &&
      search.nodes % kClockCheckInterval == 0 &&
      std::chrono::steady_clock::now() >= *search.deadline) {
    search.aborted = true;
  }
  if (search.aborted) {
    return {Move(), 0};  // Discarded by GetMove().
  }

  // Mates closer to the root score further from zero, so the engine mates
  // as fast as it can and delays being mated.
  auto const adjust_mate = [depth](int const score) {
    if (score == kMateScore) {
      return score - depth;
    }
    if (score == -kMateScore) {
      return score + depth;
    }
    return score;
  };

  if (depth == search.max_depth) {
    return {Move(), adjust_mate(Evaluate(game))};  // Evaluate() spots mates.
  }

  std::vector<Move> moves = OrderMoves(game, game.GetAllLegalMoves());
  if (moves.empty()) {
    // Checkmate or stalemate.
    int const score = game.IsKingInCheck(game.GetTurn())
                          ? -Sign(game.GetTurn()) * kMateScore
                          : 0;
    return {Move(), adjust_mate(score)};
  }
  if (game.IsInsufficientMaterial()) {
    return {Move(), 0};
  }
  if (depth == 0 && search.first_root_move) {
    auto const it = std::ranges::find(moves, *search.first_root_move);
    if (it != moves.end()) {
      std::rotate(moves.begin(), it, it + 1);  // Move it to the front.
    }
  }

  // White maximises the score and black minimises it.
  bool const is_maximizing_player = game.GetTurn() == Color::kWhite;
  int best_eval = is_maximizing_player ? std::numeric_limits<int>::min()
                                       : std::numeric_limits<int>::max();
  Move best_move;
  for (const Move& move : moves) {
    Game child(game);
    child.MakeMove(move);
    int const eval = MiniMax(child, depth + 1, alpha, beta, search).second;
    if (search.aborted) {
      return {Move(), 0};
    }
    if (is_maximizing_player ? eval > best_eval : eval < best_eval) {
      best_eval = eval;
      best_move = move;
    }
    if (is_maximizing_player) {
      alpha = std::max(alpha, eval);
    } else {
      beta = std::min(beta, eval);
    }
    if (beta <= alpha) {
      break;  // The opponent won't allow this line.
    }
  }
  return {best_move, best_eval};
}

}  // namespace chess
