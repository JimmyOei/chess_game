/**
 * This file contains Game: the board, side to move, castling rights and
 * en passant square of a chess game, plus legal move generation, making
 * moves, and detecting the end of the game.
 */

#ifndef CHESS_GAME_SRC_CORE_GAME_H_
#define CHESS_GAME_SRC_CORE_GAME_H_

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "core/direction.h"
#include "core/move.h"
#include "core/piece.h"
#include "core/position.h"

namespace chess {

constexpr std::string_view kStandardOpeningFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

enum class Result : uint8_t { kWhiteWin, kBlackWin, kDraw, kOngoing };

class Game {
 public:
  // The standard starting position.
  Game();

  // The position given in FEN notation. If the FEN is not valid, the game is
  // set to an empty board. Use FromFen() to detect an invalid FEN.
  explicit Game(std::string_view fen);

  // The position given in FEN notation, or nullopt if the FEN is not valid.
  [[nodiscard]] static std::optional<Game> FromFen(std::string_view fen);

  [[nodiscard]] Color GetTurn() const { return turn_; }

  [[nodiscard]] int GetMoveCounter() const { return move_counter_; }

  [[nodiscard]] Result GetResult() const { return result_; }

  // Square a pawn can capture en passant on, if any.
  [[nodiscard]] std::optional<Position> GetEnPassantPos() const {
    return en_passant_pos_;
  }

  [[nodiscard]] bool GetCastlingKingSide(Color color) const;

  [[nodiscard]] bool GetCastlingQueenSide(Color color) const;

  [[nodiscard]] Position GetKingPos(Color color) const;

  [[nodiscard]] Piece GetPieceAtPos(Position pos) const;

  // All pieces of `color`, with the square each one is on.
  [[nodiscard]] std::vector<std::pair<Piece, Position>> GetAllPiecesForColor(
      Color color) const;

  // All legal moves for the side to move.
  [[nodiscard]] std::vector<Move> GetAllLegalMoves() const;

  // All legal moves `color` would have if it were its turn (for the side not
  // to move, without the opponent's en passant square). For evaluation.
  [[nodiscard]] std::vector<Move> GetAllLegalMovesFor(Color color) const;

  [[nodiscard]] bool IsKingInCheck(Color color) const;

  // Legal moves of the piece on `pos`.
  [[nodiscard]] std::vector<Move> GetLegalMovesForPos(Position pos) const;

  // Plays `move`, which must be legal.
  void MakeMove(Move move);

  // Prints the game state to stdout, for debugging.
  void PrintGame() const;

  // True if the game has ended. Also sets the result (see GetResult()).
  [[nodiscard]] bool IsGameOver();

 private:
  struct CastlingRights {
    bool kingside = false;
    bool queenside = false;
  };

  // Sets the game to the given FEN. Returns false if the FEN is not valid.
  bool InitGameFromFen(std::string_view fen);

  void PassTurn(std::optional<Position> new_en_passant_pos = std::nullopt);

  // Clears the board and resets the state to its defaults.
  void InitGame();

  // True if a piece of colour `by` attacks `square`.
  [[nodiscard]] bool IsSquareAttacked(Position square, Color by) const;

  // True if the king on `from` would be in check on `through`, the square it
  // crosses while castling. A king may not castle across an attacked square.
  [[nodiscard]] bool CrossesAttackedSquare(Position from,
                                           Position through) const;

  // Pseudo-legal moves (they may leave the own king in check) of the piece on
  // `pos`, appended to `moves`.
  void AddPawnMoves(Position pos, std::vector<Move>& moves) const;
  void AddSlidingMoves(Position pos, std::span<const int> directions,
                       std::vector<Move>& moves) const;
  void AddStepMoves(Position pos, std::span<const int> offsets,
                    std::vector<Move>& moves) const;
  void AddCastlingMoves(Position pos, std::vector<Move>& moves) const;

  [[nodiscard]] Piece& SquareAt(Position const pos) {
    return board_[pos.ArrayIndex()];
  }
  [[nodiscard]] Piece SquareAt(Position const pos) const {
    return board_[pos.ArrayIndex()];
  }

  std::array<Piece, kBoardSize> board_{};
  Color turn_ = Color::kWhite;
  std::optional<Position> en_passant_pos_;
  // Indexed by ColorIndex().
  std::array<CastlingRights, 2> castling_{};
  std::array<Position, 2> king_pos_{kNoPosition, kNoPosition};

  int move_counter_ = 1;
  Result result_ = Result::kOngoing;
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_GAME_H_
