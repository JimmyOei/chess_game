#ifndef PIECES_H
#define PIECES_H

#include <stdint.h>

/**
 * @brief Enum class for the pieces in the chess game
 * 
 * This enum class is used to represent the pieces in the chess game.
 * Each piece is represented by a byte, where the first 2 bits represent the colour of the piece,
 * and the last 7 bits represent the type of the piece.
 */
enum class Piece
{
  /* General */
  NO_PIECE = 0b00000000,
  INVALID = 0b11111111,

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


/**
* @brief enum class for the color of the pieces
*/
enum class Color
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
Color getColorOfPiece(Piece const piece)
{
  return static_cast<Color>(static_cast<uint8_t>(piece & 0b11000000));
};

#endif