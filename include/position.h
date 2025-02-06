#ifndef POSITION_H
#define POSITION_H

#include <iostream>

#include "direction.h"

/**
 * @brief Class for a position on the chess board
 *
 * This class is used to represent a position on the chess board.
 * It is used to represent the position of a piece on the board.
 */
class Position
{
public:
  Position() : _pos(0) {}

  Position(int pos) : _pos(pos) {}

  operator int() const
  {
    return _pos;
  }

  Position operator+=(int const &rhs)
  {
    _pos += rhs;
    return *this;
  }

  Position operator-=(int const &rhs)
  {
    _pos -= rhs;
    return *this;
  }

  bool operator==(int const &rhs) const
  {
    return _pos == rhs;
  }

  bool operator==(Position const &other) const
  {
    return _pos == other._pos;
  }

  bool isValid() const
  {
    return _pos >= 0 && _pos < BOARD_SIZE;
  }

  bool isAtEdge() const
  {
    return _pos % BOARD_LENGTH == 0 || _pos % BOARD_LENGTH == BOARD_LENGTH - 1 || _pos < BOARD_LENGTH || _pos >= BOARD_SIZE - BOARD_LENGTH;
  }

  int getRow() const
  {
    return (_pos / BOARD_LENGTH) + 1;
  }

  int getColumn() const
  {
    return (_pos % BOARD_LENGTH) + 1;
  }

  /**
   * @brief gives the position in chess notation
   *
   * @return std::string position in chess notation
   */
  std::string toChessNotation() const
  {
    return std::string(1, 'a' + getColumn() - 1) + std::to_string(getRow());
  }

private:
  int _pos;

  friend std::ostream &operator<<(std::ostream &os, Position const &pos);
};

inline std::ostream &operator<<(std::ostream &os, const Position& p)
{
  return os << p.toChessNotation();
}

#endif
