#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>
#include <iostream>

/**
 * @brief Enum class for the pieces in the chess game
 *
 * This enum class is used to represent the pieces in the chess game.
 * Each piece is represented by a byte, where the first 2 bits represent the colour of the piece,
 * and the last 7 bits represent the type of the piece.
 */
namespace Piece
{

  enum Type
  {
    /* General */
    BLANK = 0b00000000,
    PAWN = 0b00000001,
    KNIGHT = 0b00000010,
    BISHOP = 0b00000100,
    ROOK = 0b00001000,
    QUEEN = 0b00010000,
    KING = 0b00100000,

    /* White pieces */
    WHITE_PAWN = 0b10000001,
    WHITE_KNIGHT = 0b10000010,
    WHITE_BISHOP = 0b10000100,
    WHITE_ROOK = 0b10001000,
    WHITE_QUEEN = 0b10010000,
    WHITE_KING = 0b10100000,

    /* Black pieces */
    BLACK_PAWN = 0b01000001,
    BLACK_KNIGHT = 0b01000010,
    BLACK_BISHOP = 0b01000100,
    BLACK_ROOK = 0b01001000,
    BLACK_QUEEN = 0b01010000,
    BLACK_KING = 0b01100000
  };

  // std::ostream& operator<<(std::ostream& os, const Type& type) {
  //   switch (type){
  //     case BLANK: return os << "No Piece";
  //     case PAWN: return os << "Pawn";
  //     case KNIGHT: return os << "Knight";
  //     case BISHOP: return os << "Bishop";
  //     case ROOK: return os << "Rook";
  //     case QUEEN: return os << "Queen";
  //     case KING: return os << "King";
  //     case WHITE_PAWN: return os << "White Pawn";
  //     case WHITE_KNIGHT: return os << "White Knight";
  //     case WHITE_BISHOP: return os << "White Bishop";
  //     case WHITE_ROOK: return os << "White Rook";
  //     case WHITE_QUEEN: return os << "White Queen";
  //     case WHITE_KING: return os << "White King";
  //     case BLACK_PAWN: return os << "Black Pawn";
  //     case BLACK_KNIGHT: return os << "Black Knight";
  //     case BLACK_BISHOP: return os << "Black Bishop";
  //     case BLACK_ROOK: return os << "Black Rook";
  //     case BLACK_QUEEN: return os << "Black Queen";
  //     case BLACK_KING: return os << "Black King";
  //     default: return os << "Invalid Piece";
  //   }
  // }

  static inline std::string pieceToString(Type const piece)
  {
    switch (piece)
    {
    case BLANK:
      return "No Piece";
    case PAWN:
      return "Pawn";
    case KNIGHT:
      return "Knight";
    case BISHOP:
      return "Bishop";
    case ROOK:
      return "Rook";
    case QUEEN:
      return "Queen";
    case KING:
      return "King";
    case WHITE_PAWN:
      return "White Pawn";
    case WHITE_KNIGHT:
      return "White Knight";
    case WHITE_BISHOP:
      return "White Bishop";
    case WHITE_ROOK:
      return "White Rook";
    case WHITE_QUEEN:
      return "White Queen";
    case WHITE_KING:
      return "White King";
    case BLACK_PAWN:
      return "Black Pawn";
    case BLACK_KNIGHT:
      return "Black Knight";
    case BLACK_BISHOP:
      return "Black Bishop";
    case BLACK_ROOK:
      return "Black Rook";
    case BLACK_QUEEN:
      return "Black Queen";
    case BLACK_KING:
      return "Black King";
    default:
      return "Invalid Piece";
    }
  }

  /**
   * @brief enum class for the color of the pieces
   */
  enum Color
  {
    WHITE = 0b10000000,
    BLACK = 0b01000000
  };

  /**
   * @brief getter for the color of a given piece
   *
   * @param piece piece to get the color of
   * @return color of the piece (see enum class Color)
   */
  static inline Color getColorOfPiece(Piece::Type const piece)
  {
    return static_cast<Color>(static_cast<uint8_t>(piece) & 0b11000000);
  };

  /**
   * @brief getter for the piece without the color
   *
   * @param piece piece to get the type of
   * @return type of the piece (see enum class Piece)
   */
  static inline Piece::Type getPieceTypeWithoutColor(Piece::Type const piece)
  {
    return static_cast<Piece::Type>(static_cast<uint8_t>(piece) & 0b00111111);
  };
};

#endif