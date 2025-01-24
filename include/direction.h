#ifndef DIRECTION_H
#define DIRECTION_H

#include "game.h"

/**
 * @brief namespace for the directions on the chess board
 */
namespace Direction
{
  /**
   * @brief Enum class for the cardinal directions
   *
   * Directions: NORTH, EAST, SOUTH, WEST
   */
  enum Cardinal
  {
    NORTH = BOARD_LENGTH,
    EAST = 1,
    SOUTH = -BOARD_LENGTH,
    WEST = -1
  };
  std::array<int, 4> const Cardinals = {NORTH, EAST, SOUTH, WEST};

  /**
   * @brief Enum class for the diagonal directions
   *
   * Directions: NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
   */
  enum Diagonal
  {
    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
  };
  std::array<int, 4> const Diagonals = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};

  /**
   * @brief Enum class for the knight moves
   *
   * Directions: NORTH_NORTH_EAST, EAST_EAST_NORTH, EAST_EAST_SOUTH, SOUTH_SOUTH_EAST, SOUTH_SOUTH_WEST, WEST_WEST_SOUTH, WEST_WEST_NORTH, NORTH_NORTH_WEST
   */
  enum KnightMove
  {
    NORTH_NORTH_EAST = NORTH + NORTH + EAST,
    EAST_EAST_NORTH = EAST + EAST + NORTH,
    EAST_EAST_SOUTH = EAST + EAST + SOUTH,
    SOUTH_SOUTH_EAST = SOUTH + SOUTH + EAST,
    SOUTH_SOUTH_WEST = SOUTH + SOUTH + WEST,
    WEST_WEST_SOUTH = WEST + WEST + SOUTH,
    WEST_WEST_NORTH = WEST + WEST + NORTH,
    NORTH_NORTH_WEST = NORTH + NORTH + WEST
  };
  std::array<int, 8> const KnightMoves = {NORTH_NORTH_EAST, EAST_EAST_NORTH, EAST_EAST_SOUTH, SOUTH_SOUTH_EAST, SOUTH_SOUTH_WEST, WEST_WEST_SOUTH, WEST_WEST_NORTH, NORTH_NORTH_WEST};
};

#endif