/**
 * This file contains the PlayerEngineMiniMax implementation: the evaluation
 * terms and the minimax search with alpha-beta pruning.
 */

#include "player/player_engine_minimax.h"

#include <algorithm>
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

std::vector<Move> PlayerEngineMiniMax::GetAllLegalMoves(const Game &game) {
  std::vector<Move> all_legal_moves = game.GetAllLegalMoves();
  if (all_legal_moves.empty()) {
    CHESS_LOG(LogLevel::kError) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }
  return all_legal_moves;
}

Move PlayerEngineMiniMax::GetMove(const Game &game) {
  CHESS_LOG(LogLevel::kInfo) << "Player Engine MiniMax is calculating a move";
  CHESS_LOG(LogLevel::kInfo)
      << "Current score: " << EvaluateGame(game) << " turn: " << game.GetTurn();
  const std::pair<Move, int> best_move =
      MiniMax(game, 0, std::numeric_limits<int>::min(),
              std::numeric_limits<int>::max(), game.GetTurn() == Color::kWhite);
  if (best_move.first.from == kNoPosition) {
    CHESS_LOG(LogLevel::kError) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }
  CHESS_LOG(LogLevel::kInfo)
      << "Player Engine MiniMax made move " << best_move.first
      << " with eval score " << best_move.second;

  return best_move.first;
}

namespace {

// Material value of a piece type, in tenths of a pawn. The king has no value:
// it can't be captured.
constexpr int PieceValue(PieceType const type) {
  switch (type) {
    case PieceType::kPawn:
      return 10;
    case PieceType::kKnight:
    case PieceType::kBishop:
      return 30;
    case PieceType::kRook:
      return 50;
    case PieceType::kQueen:
      return 90;
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

}  // namespace

int PlayerEngineMiniMax::EvaluatePieceValue(const Game &game) {
  int score = 0;
  for (int i = 0; i < kBoardSize; i++) {
    Piece const piece = game.GetPieceAtPos(Position(i));
    if (piece != Piece::kNone) {
      score += Sign(ColorOf(piece)) * PieceValue(TypeOf(piece));
    }
  }
  return score;
}

int PlayerEngineMiniMax::EvaluatePieceMobility(const Game &game) {
  // TODO(Jimmy): this term is always 0. It only adds the move count of
  // pieces that have no legal moves, and it should add white's mobility and
  // subtract black's. Fix together with tests for the engine.
  int score = 0;
  for (int i = 0; i < kBoardSize; i++) {
    if (game.GetPieceAtPos(Position(i)) == Piece::kNone) {
      continue;
    }

    const std::vector<Move> legal_moves = game.GetLegalMovesForPos(Position(i));
    if (legal_moves.empty()) {
      score += static_cast<int>(legal_moves.size());
    }
  }

  return score;
}

int PlayerEngineMiniMax::EvaluatePiecePlacement(const Game & /*game*/) {
  // TODO(Jimmy): score piece placement (e.g. piece-square tables).
  return 0;
}

int PlayerEngineMiniMax::EvaluatePawnStructure(const Game &game) {
  int score = 0;

  // Doubled pawns: each extra pawn of a colour on the same file costs 5.
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
        score -= Sign(color) * 5 * (pawns - 1);
      }
    }
  }

  return score;
}

int PlayerEngineMiniMax::EvaluateGame(const Game &game) {
  int score = 0;

  score += EvaluatePieceValue(game);
  score += EvaluatePieceMobility(game);
  score += EvaluatePiecePlacement(game);
  score += EvaluatePawnStructure(game);

  // TODO(Jimmy): a draw (stalemate, insufficient material) is scored as a
  // loss for the side to move here. Fix together with tests for the engine.
  Game game_copy(game);  // IsGameOver() records the result.
  if (game_copy.IsGameOver()) {
    score -= Sign(game_copy.GetTurn()) * 1000;
  }
  return score;
}

std::pair<Move, int> PlayerEngineMiniMax::MiniMax(
    const Game &game, int const depth, int alpha, int beta,
    bool const is_maximizing_player) const {
  Game game_copy(game);  // IsGameOver() records the result.
  if (depth == max_depth_ || game_copy.IsGameOver()) {
    return std::make_pair(Move(), EvaluateGame(game));
  }

  const std::vector<Move> all_legal_moves = GetAllLegalMoves(game);

  // White maximises the score and black minimises it.
  int best_eval = is_maximizing_player ? std::numeric_limits<int>::min()
                                       : std::numeric_limits<int>::max();
  Move best_move;
  for (const Move &move : all_legal_moves) {
    Game child(game);
    child.MakeMove(move);
    int const eval =
        MiniMax(child, depth + 1, alpha, beta, !is_maximizing_player).second;
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
