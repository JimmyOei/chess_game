#ifndef POSITION_H
#define POSITION_H

#include "game.h"
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

  bool isValid() const
  {
    return _pos >= 0 && _pos < BOARD_SIZE;
  }

  /**
   * @brief gives the position in chess notation
   * 
   * @return std::string position in chess notation
   */
  std::string toChessNotation() const
  {
    return std::string(1, 'a' + (_pos % BOARD_LENGTH)) + std::to_string(BOARD_LENGTH + (_pos / BOARD_LENGTH));
  }

  std::ostream &operator<<(std::ostream &os) const
  {
    return os << toChessNotation();
  }

private:
  int _pos;
};

#endif
