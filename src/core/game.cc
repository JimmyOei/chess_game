/**
 * This file contains the Game implementation: FEN parsing, legal move
 * generation, making moves, and detecting the end of the game.
 */

#include "core/game.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "util/logger.h"

namespace chess {

namespace {

constexpr int kKingColumn = 5;           // e-file
constexpr int kKingsideRookColumn = 8;   // h-file
constexpr int kQueensideRookColumn = 1;  // a-file

constexpr std::array<PieceType, 4> kPromotionTypes = {
    PieceType::kQueen, PieceType::kRook, PieceType::kBishop,
    PieceType::kKnight};

// Direction a pawn of `color` moves in.
constexpr int Forward(Color const color) {
  return color == Color::kWhite ? direction::kNorth : direction::kSouth;
}

// Rank the pieces of `color` start on.
constexpr int HomeRow(Color const color) {
  return color == Color::kWhite ? 1 : kBoardLength;
}

// Rank the pawns of `color` start on.
constexpr int PawnStartRow(Color const color) {
  return HomeRow(color) + (color == Color::kWhite ? 1 : -1);
}

// Rank a pawn of `color` promotes on.
constexpr int PromotionRow(Color const color) {
  return color == Color::kWhite ? kBoardLength : 1;
}

// The square one `offset` from `from`, or nullopt if that step leaves the
// board.
std::optional<Position> Step(Position const from, int const offset) {
  Position const to = from + offset;
  if (!to.IsValid() || std::abs(to.GetColumn() - from.GetColumn()) > 2) {
    return std::nullopt;
  }
  return to;
}

}  // namespace

void Game::InitGame() {
  turn_ = Color::kWhite;
  en_passant_pos_.reset();
  castling_ = {};
  king_pos_ = {kNoPosition, kNoPosition};
  result_ = Result::kOngoing;
  move_counter_ = 1;
  board_.fill(Piece::kNone);
}

Game::Game() : Game(kStandardOpeningFen) {}

Game::Game(std::string_view const fen) {
  if (!InitGameFromFen(fen)) {
    std::cerr << ">> Invalid FEN notation. Game is set to an empty board.\n";
    InitGame();
  }
}

std::optional<Game> Game::FromFen(std::string_view const fen) {
  Game game;
  if (!game.InitGameFromFen(fen)) {
    return std::nullopt;
  }
  return game;
}

void Game::PassTurn(std::optional<Position> const new_en_passant_pos) {
  if (turn_ == Color::kBlack) {
    move_counter_++;
  }
  turn_ = Opponent(turn_);
  en_passant_pos_ = new_en_passant_pos;
}

bool Game::GetCastlingKingSide(Color const color) const {
  return castling_[ColorIndex(color)].kingside;
}

bool Game::GetCastlingQueenSide(Color const color) const {
  return castling_[ColorIndex(color)].queenside;
}

Position Game::GetKingPos(Color const color) const {
  return king_pos_[ColorIndex(color)];
}

Piece Game::GetPieceAtPos(Position const pos) const {
  if (!pos.IsValid()) {
    throw std::out_of_range("Position is out of range");
  }
  return SquareAt(pos);
}

std::vector<std::pair<Piece, Position>> Game::GetAllPiecesForColor(
    Color const color) const {
  std::vector<std::pair<Piece, Position>> pieces;
  for (int i = 0; i < kBoardSize; i++) {
    Piece const piece = SquareAt(Position(i));
    if (piece != Piece::kNone && ColorOf(piece) == color) {
      pieces.emplace_back(piece, Position(i));
    }
  }
  return pieces;
}

std::vector<Move> Game::GetAllLegalMoves() const {
  std::vector<Move> all_legal_moves;
  for (auto const& [piece, pos] : GetAllPiecesForColor(turn_)) {
    std::vector<Move> const legal_moves = GetLegalMovesForPos(pos);
    all_legal_moves.insert(all_legal_moves.end(), legal_moves.begin(),
                           legal_moves.end());
  }
  return all_legal_moves;
}

int Game::CountPseudoLegalMoves(Color const color) const {
  std::vector<Move> moves;
  moves.reserve(128);
  for (auto const& [piece, pos] : GetAllPiecesForColor(color)) {
    AddPseudoLegalMoves(pos, /*include_castling=*/false, moves);
  }
  return static_cast<int>(moves.size());
}

bool Game::IsSquareAttacked(Position const square, Color const by) const {
  // Looks outward from `square` for an attacker of each kind.
  auto const is_attacker = [&](std::optional<Position> const from,
                               PieceType const type) {
    return from && SquareAt(*from) == MakePiece(by, type);
  };
  auto const slider_attacks = [&](std::span<const int> const directions,
                                  PieceType const type) {
    for (int const direction : directions) {
      std::optional<Position> current = Step(square, direction);
      while (current && SquareAt(*current) == Piece::kNone) {
        current = Step(*current, direction);
      }
      if (is_attacker(current, type) ||
          is_attacker(current, PieceType::kQueen)) {
        return true;
      }
    }
    return false;
  };

  if (slider_attacks(direction::kCardinals, PieceType::kRook) ||
      slider_attacks(direction::kDiagonals, PieceType::kBishop)) {
    return true;
  }
  for (int const offset : direction::kKnightMoves) {
    if (is_attacker(Step(square, offset), PieceType::kKnight)) {
      return true;
    }
  }
  for (int const offset : direction::kCardinals) {
    if (is_attacker(Step(square, offset), PieceType::kKing)) {
      return true;
    }
  }
  for (int const offset : direction::kDiagonals) {
    if (is_attacker(Step(square, offset), PieceType::kKing)) {
      return true;
    }
  }
  // A pawn of `by` attacks diagonally forward, so it stands diagonally
  // behind `square` from its own point of view.
  for (int const side : {direction::kEast, direction::kWest}) {
    if (is_attacker(Step(square, side - Forward(by)), PieceType::kPawn)) {
      return true;
    }
  }
  return false;
}

bool Game::IsKingInCheck(Color const color) const {
  return IsSquareAttacked(GetKingPos(color), Opponent(color));
}

bool Game::CrossesAttackedSquare(Position const from,
                                 Position const through) const {
  Game game_copy(*this);
  Piece const king = game_copy.GetPieceAtPos(from);
  game_copy.MakeMove(Move(from, through, king));
  return game_copy.IsKingInCheck(ColorOf(king));
}

void Game::AddPawnMoves(Position const pos, std::vector<Move>& moves) const {
  Piece const pawn = SquareAt(pos);
  Color const color = ColorOf(pawn);
  int const forward = Forward(color);
  // A pawn reaching the last rank becomes one of the four promotion pieces.
  auto const add = [&](Position const to) {
    if (to.GetRow() == PromotionRow(color)) {
      for (PieceType const type : kPromotionTypes) {
        moves.emplace_back(pos, to, pawn, MakePiece(color, type));
      }
    } else {
      moves.emplace_back(pos, to, pawn);
    }
  };

  std::optional<Position> const one_step = Step(pos, forward);
  if (one_step && SquareAt(*one_step) == Piece::kNone) {
    add(*one_step);
    if (pos.GetRow() == PawnStartRow(color)) {
      std::optional<Position> const two_steps = Step(*one_step, forward);
      if (two_steps && SquareAt(*two_steps) == Piece::kNone) {
        moves.emplace_back(pos, *two_steps, pawn);
      }
    }
  }

  for (int const side : {direction::kWest, direction::kEast}) {
    std::optional<Position> const target = Step(pos, forward + side);
    if (!target) {
      continue;
    }
    Piece const captured = SquareAt(*target);
    // The en passant square only belongs to the side to move.
    if ((captured != Piece::kNone && ColorOf(captured) != color) ||
        (color == turn_ && *target == en_passant_pos_)) {
      add(*target);
    }
  }
}

void Game::AddSlidingMoves(Position const pos,
                           std::span<const int> const directions,
                           std::vector<Move>& moves) const {
  Piece const piece = SquareAt(pos);
  for (int const direction : directions) {
    std::optional<Position> target = Step(pos, direction);
    while (target && SquareAt(*target) == Piece::kNone) {
      moves.emplace_back(pos, *target, piece);
      target = Step(*target, direction);
    }
    if (target && ColorOf(SquareAt(*target)) != ColorOf(piece)) {
      moves.emplace_back(pos, *target, piece);  // Capture.
    }
  }
}

void Game::AddStepMoves(Position const pos, std::span<const int> const offsets,
                        std::vector<Move>& moves) const {
  Piece const piece = SquareAt(pos);
  for (int const offset : offsets) {
    std::optional<Position> const target = Step(pos, offset);
    if (target && (SquareAt(*target) == Piece::kNone ||
                   ColorOf(SquareAt(*target)) != ColorOf(piece))) {
      moves.emplace_back(pos, *target, piece);
    }
  }
}

void Game::AddCastlingMoves(Position const pos,
                            std::vector<Move>& moves) const {
  Piece const king = SquareAt(pos);
  Color const color = ColorOf(king);
  CastlingRights const rights = castling_[ColorIndex(color)];
  if ((!rights.kingside && !rights.queenside) ||
      pos != Position::FromColumnRow(kKingColumn, HomeRow(color)) ||
      IsKingInCheck(color)) {
    return;
  }
  Piece const rook = MakePiece(color, PieceType::kRook);
  auto const empty = [&](int const offset) {
    return SquareAt(pos + offset) == Piece::kNone;
  };

  if (rights.kingside && empty(direction::kEast) &&
      empty(2 * direction::kEast) &&
      SquareAt(pos + 3 * direction::kEast) == rook &&
      !CrossesAttackedSquare(pos, pos + direction::kEast)) {
    moves.emplace_back(pos, pos + 2 * direction::kEast, king);
  }
  if (rights.queenside && empty(direction::kWest) &&
      empty(2 * direction::kWest) && empty(3 * direction::kWest) &&
      SquareAt(pos + 4 * direction::kWest) == rook &&
      !CrossesAttackedSquare(pos, pos + direction::kWest)) {
    moves.emplace_back(pos, pos + 2 * direction::kWest, king);
  }
}

void Game::AddPseudoLegalMoves(Position const pos, bool const include_castling,
                               std::vector<Move>& moves) const {
  switch (TypeOf(SquareAt(pos))) {
    case PieceType::kPawn:
      AddPawnMoves(pos, moves);
      break;
    case PieceType::kKnight:
      AddStepMoves(pos, direction::kKnightMoves, moves);
      break;
    case PieceType::kBishop:
      AddSlidingMoves(pos, direction::kDiagonals, moves);
      break;
    case PieceType::kRook:
      AddSlidingMoves(pos, direction::kCardinals, moves);
      break;
    case PieceType::kQueen:
      AddSlidingMoves(pos, direction::kDiagonals, moves);
      AddSlidingMoves(pos, direction::kCardinals, moves);
      break;
    case PieceType::kKing:
      AddStepMoves(pos, direction::kCardinals, moves);
      AddStepMoves(pos, direction::kDiagonals, moves);
      if (include_castling) {
        AddCastlingMoves(pos, moves);
      }
      break;
    case PieceType::kNone:
      break;
  }
}

std::vector<Move> Game::GetLegalMovesForPos(Position const pos) const {
  if (!pos.IsValid()) {
    return {};
  }
  Piece const piece = SquareAt(pos);
  if (piece == Piece::kNone || ColorOf(piece) != turn_) {
    return {};
  }

  CHESS_LOG(LogLevel::kDebug)
      << "Getting legal moves for position " << pos << " " << piece;

  std::vector<Move> moves;
  AddPseudoLegalMoves(pos, /*include_castling=*/true, moves);

  // Keep only the moves that don't leave the own king in check.
  std::vector<Move> legal_moves;
  for (Move const& move : moves) {
    Game game_copy(*this);
    game_copy.MakeMove(move);
    if (!game_copy.IsKingInCheck(ColorOf(piece))) {
      legal_moves.push_back(move);
    }
  }
  return legal_moves;
}

void Game::MakeMove(Move const move) {
  Color const color = ColorOf(move.piece);
  std::optional<Position> new_en_passant_pos;

  // A move from or onto a rook's home square ends that castling right
  for (Color const side : {Color::kWhite, Color::kBlack}) {
    CastlingRights& rights = castling_[ColorIndex(side)];
    for (Position const square : {move.from, move.to}) {
      if (square ==
          Position::FromColumnRow(kKingsideRookColumn, HomeRow(side))) {
        rights.kingside = false;
      } else if (square ==
                 Position::FromColumnRow(kQueensideRookColumn, HomeRow(side))) {
        rights.queenside = false;
      }
    }
  }

  switch (TypeOf(move.piece)) {
    case PieceType::kKing: {
      Piece const rook = MakePiece(color, PieceType::kRook);
      if (move.to == move.from + 2 * direction::kEast) {
        SquareAt(move.to + direction::kEast) = Piece::kNone;
        SquareAt(move.to + direction::kWest) = rook;
      } else if (move.to == move.from + 2 * direction::kWest) {
        SquareAt(move.to + 2 * direction::kWest) = Piece::kNone;
        SquareAt(move.to + direction::kEast) = rook;
      }
      castling_[ColorIndex(color)] = {};
      king_pos_[ColorIndex(color)] = move.to;
      break;
    }
    case PieceType::kPawn:
      if (move.to.GetRow() == PromotionRow(color)) {
        SquareAt(move.from) = Piece::kNone;
        SquareAt(move.to) = move.promotion_piece;
        PassTurn();
        return;
      }
      if (move.to == en_passant_pos_) {
        SquareAt(move.to - Forward(color)) = Piece::kNone;
      } else if (move.to == move.from + 2 * Forward(color)) {
        new_en_passant_pos = move.from + Forward(color);
      }
      break;
    default:
      break;
  }

  SquareAt(move.from) = Piece::kNone;
  SquareAt(move.to) = move.piece;
  PassTurn(new_en_passant_pos);
}

bool Game::IsGameOver() {
  if (result_ != Result::kOngoing) {
    return true;
  }

  // No legal moves: checkmate if in check, otherwise stalemate (a draw).
  bool has_legal_move = false;
  for (int i = 0; i < kBoardSize && !has_legal_move; i++) {
    if (SquareAt(Position(i)) != Piece::kNone &&
        ColorOf(SquareAt(Position(i))) == turn_) {
      has_legal_move = !GetLegalMovesForPos(Position(i)).empty();
    }
  }
  if (!has_legal_move) {
    if (IsKingInCheck(turn_)) {
      result_ = turn_ == Color::kWhite ? Result::kBlackWin : Result::kWhiteWin;
    } else {
      result_ = Result::kDraw;
    }
    return true;
  }

  if (IsInsufficientMaterial()) {
    result_ = Result::kDraw;
    return true;
  }

  return false;
}

bool Game::IsInsufficientMaterial() const {
  std::vector<std::pair<Piece, Position>> non_kings;
  for (Color const color : {Color::kWhite, Color::kBlack}) {
    for (auto const& [piece, pos] : GetAllPiecesForColor(color)) {
      if (TypeOf(piece) != PieceType::kKing) {
        non_kings.emplace_back(piece, pos);
      }
    }
  }
  auto const is_minor = [](Piece const piece) {
    PieceType const type = TypeOf(piece);
    return type == PieceType::kKnight || type == PieceType::kBishop;
  };
  auto const is_bishop = [](Piece const piece) {
    return TypeOf(piece) == PieceType::kBishop;
  };
  auto const square_color = [](Position const pos) {
    return (pos.GetRow() + pos.GetColumn()) % 2;
  };
  return non_kings.empty() ||
         (non_kings.size() == 1 && is_minor(non_kings[0].first)) ||
         (non_kings.size() == 2 && is_bishop(non_kings[0].first) &&
          is_bishop(non_kings[1].first) &&
          ColorOf(non_kings[0].first) != ColorOf(non_kings[1].first) &&
          square_color(non_kings[0].second) ==
              square_color(non_kings[1].second));
}

bool Game::InitGameFromFen(std::string_view const fen) {
  CHESS_LOG(LogLevel::kDebug) << "Initializing game from fen: " << fen;
  InitGame();
  auto const char_at = [fen](std::size_t const index) {
    return index < fen.size() ? fen[index] : '\0';
  };
  std::size_t i = 0;

  int row = kBoardLength - 1;           // top row (black's first row)
  int column = 0;                       // most left column
  int pos = kBoardSize - kBoardLength;  // index of most top left square
  while (i < fen.size() && row >= 0 && column <= kBoardLength &&
         char_at(i) != ' ') {
    switch (char_at(i)) {
      case '/':
        row--;
        column = -1;
        pos = row * kBoardLength - 1;
        break;
      default:
        if (std::optional<Piece> const piece = PieceFromChar(char_at(i))) {
          SquareAt(Position(pos)) = *piece;
          if (TypeOf(*piece) == PieceType::kKing) {
            king_pos_[ColorIndex(ColorOf(*piece))] = Position(pos);
          }
          break;
        }
        int const empty_squares = char_at(i) - '0';
        if (empty_squares < 0 || empty_squares > kBoardLength) {
          return false;
        }
        column += empty_squares - 1;
        pos += empty_squares - 1;
        break;
    }
    pos++;
    column++;
    i++;
  }
  if (GetKingPos(Color::kWhite) == kNoPosition ||
      GetKingPos(Color::kBlack) == kNoPosition || column != kBoardLength ||
      row != 0) {
    return false;
  }

  i++;
  if (i < fen.size() && char_at(i - 1) == ' ') {
    switch (char_at(i)) {
      case 'w':
        turn_ = Color::kWhite;
        break;
      case 'b':
        turn_ = Color::kBlack;
        break;
      default:
        return false;
    }
  } else {
    return false;
  }

  i += 2;
  if (i < fen.size() && char_at(i - 1) == ' ') {
    if (char_at(i) == '-') {
      i++;
    } else {
      std::size_t const field_start = i;
      while (i < fen.size() && char_at(i) != ' ') {
        switch (char_at(i)) {
          case 'K':
            castling_[ColorIndex(Color::kWhite)].kingside = true;
            break;
          case 'Q':
            castling_[ColorIndex(Color::kWhite)].queenside = true;
            break;
          case 'k':
            castling_[ColorIndex(Color::kBlack)].kingside = true;
            break;
          case 'q':
            castling_[ColorIndex(Color::kBlack)].queenside = true;
            break;
          default:
            return false;
        }
        i++;
      }
      if (i == field_start) {
        return false;
      }
    }
    if (char_at(i++) != ' ') {
      return false;
    }

    if (char_at(i) == '-') {
      i++;
    } else {
      if (i + 1 >= fen.size()) {
        return false;
      }
      int const file = char_at(i) - 'a';
      int const rank = char_at(i + 1) - '1';
      if (file < 0 || file >= kBoardLength || (rank != 2 && rank != 5)) {
        return false;
      }
      en_passant_pos_ = Position(rank * kBoardLength + file);
      i += 2;
    }
  } else {
    return false;
  }

  return true;
}

void Game::PrintGame() const {
  std::cout << "--------===== PRINTING STATE =====--------\n"
            << "Turn: " << turn_ << '\n'
            << "En passant square: "
            << (en_passant_pos_ ? en_passant_pos_->ToChessNotation() : "-")
            << '\n';
  for (Color const color : {Color::kWhite, Color::kBlack}) {
    std::cout << color << ": king on " << GetKingPos(color)
              << ", castling kingside " << GetCastlingKingSide(color)
              << ", queenside " << GetCastlingQueenSide(color) << '\n';
  }
  std::cout << '\n';

  for (int row = kBoardLength; row >= 1; row--) {
    for (int column = 1; column <= kBoardLength; column++) {
      Piece const piece = SquareAt(Position::FromColumnRow(column, row));
      std::cout << ' ' << (piece == Piece::kNone ? '.' : PieceToChar(piece));
    }
    std::cout << '\n';
  }
}

}  // namespace chess
