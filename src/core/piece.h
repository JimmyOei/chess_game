/**
 * This file contains the piece and colour types, and helpers to split a
 * piece into its colour and type and to print it.
 *
 * A piece is one byte: the top two bits hold the colour (see Color) and the
 * low six bits hold the type (see PieceType), one bit per type.
 */

#ifndef CHESS_GAME_SRC_CORE_PIECE_H_
#define CHESS_GAME_SRC_CORE_PIECE_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>

namespace chess {

enum class Color : uint8_t { kWhite = 0b10000000, kBlack = 0b01000000 };

enum class PieceType : uint8_t {
  kNone = 0b00000000,
  kPawn = 0b00000001,
  kKnight = 0b00000010,
  kBishop = 0b00000100,
  kRook = 0b00001000,
  kQueen = 0b00010000,
  kKing = 0b00100000,
};

enum class Piece : uint8_t {
  kNone = 0b00000000,

  kWhitePawn = 0b10000001,
  kWhiteKnight = 0b10000010,
  kWhiteBishop = 0b10000100,
  kWhiteRook = 0b10001000,
  kWhiteQueen = 0b10010000,
  kWhiteKing = 0b10100000,

  kBlackPawn = 0b01000001,
  kBlackKnight = 0b01000010,
  kBlackBishop = 0b01000100,
  kBlackRook = 0b01001000,
  kBlackQueen = 0b01010000,
  kBlackKing = 0b01100000,
};

constexpr uint8_t kColorMask = 0b11000000;
constexpr uint8_t kTypeMask = 0b00111111;

// Colour of a piece. For Piece::kNone this is neither kWhite nor kBlack.
[[nodiscard]] constexpr Color ColorOf(Piece const piece) {
  return static_cast<Color>(static_cast<uint8_t>(piece) & kColorMask);
}

[[nodiscard]] constexpr PieceType TypeOf(Piece const piece) {
  return static_cast<PieceType>(static_cast<uint8_t>(piece) & kTypeMask);
}

[[nodiscard]] constexpr Piece MakePiece(Color const color,
                                        PieceType const type) {
  return static_cast<Piece>(static_cast<uint8_t>(color) |
                            static_cast<uint8_t>(type));
}

[[nodiscard]] constexpr Color Opponent(Color const color) {
  return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

// 0 for white and 1 for black, for per-colour arrays.
[[nodiscard]] constexpr std::size_t ColorIndex(Color const color) {
  return color == Color::kWhite ? 0 : 1;
}

[[nodiscard]] constexpr std::string_view PieceToString(Piece const piece) {
  switch (piece) {
    case Piece::kNone:
      return "No Piece";
    case Piece::kWhitePawn:
      return "White Pawn";
    case Piece::kWhiteKnight:
      return "White Knight";
    case Piece::kWhiteBishop:
      return "White Bishop";
    case Piece::kWhiteRook:
      return "White Rook";
    case Piece::kWhiteQueen:
      return "White Queen";
    case Piece::kWhiteKing:
      return "White King";
    case Piece::kBlackPawn:
      return "Black Pawn";
    case Piece::kBlackKnight:
      return "Black Knight";
    case Piece::kBlackBishop:
      return "Black Bishop";
    case Piece::kBlackRook:
      return "Black Rook";
    case Piece::kBlackQueen:
      return "Black Queen";
    case Piece::kBlackKing:
      return "Black King";
  }
  return "Invalid Piece";
}

// FEN letter of a piece: upper case for white, lower case for black, and a
// space for no piece.
[[nodiscard]] constexpr char PieceToChar(Piece const piece) {
  switch (piece) {
    case Piece::kWhitePawn:
      return 'P';
    case Piece::kWhiteKnight:
      return 'N';
    case Piece::kWhiteBishop:
      return 'B';
    case Piece::kWhiteRook:
      return 'R';
    case Piece::kWhiteQueen:
      return 'Q';
    case Piece::kWhiteKing:
      return 'K';
    case Piece::kBlackPawn:
      return 'p';
    case Piece::kBlackKnight:
      return 'n';
    case Piece::kBlackBishop:
      return 'b';
    case Piece::kBlackRook:
      return 'r';
    case Piece::kBlackQueen:
      return 'q';
    case Piece::kBlackKing:
      return 'k';
    case Piece::kNone:
      return ' ';
  }
  return ' ';
}

// Inverse of PieceToChar for the twelve FEN piece letters.
[[nodiscard]] constexpr std::optional<Piece> PieceFromChar(char const letter) {
  for (Piece const piece :
       {Piece::kWhitePawn, Piece::kWhiteKnight, Piece::kWhiteBishop,
        Piece::kWhiteRook, Piece::kWhiteQueen, Piece::kWhiteKing,
        Piece::kBlackPawn, Piece::kBlackKnight, Piece::kBlackBishop,
        Piece::kBlackRook, Piece::kBlackQueen, Piece::kBlackKing}) {
    if (PieceToChar(piece) == letter) {
      return piece;
    }
  }
  return std::nullopt;
}

[[nodiscard]] constexpr std::string_view ColorToString(Color const color) {
  switch (color) {
    case Color::kWhite:
      return "White";
    case Color::kBlack:
      return "Black";
  }
  return "Invalid Color";
}

inline std::ostream &operator<<(std::ostream &os, Piece const piece) {
  return os << PieceToString(piece);
}

inline std::ostream &operator<<(std::ostream &os, Color const color) {
  return os << ColorToString(color);
}

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_PIECE_H_
