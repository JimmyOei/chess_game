/**
 * This file contains the board dimensions and the square offsets for moving
 * in each direction on the board.
 *
 * Squares are indexed 0 (a1) to 63 (h8), rank by rank.
 */

#ifndef CHESS_GAME_SRC_CORE_DIRECTION_H_
#define CHESS_GAME_SRC_CORE_DIRECTION_H_

#include <array>

namespace chess {

constexpr int kBoardLength = 8;
constexpr int kBoardSize = kBoardLength * kBoardLength;

namespace direction {

constexpr int kNorth = kBoardLength;
constexpr int kEast = 1;
constexpr int kSouth = -kBoardLength;
constexpr int kWest = -1;

constexpr int kNorthEast = kNorth + kEast;
constexpr int kSouthEast = kSouth + kEast;
constexpr int kSouthWest = kSouth + kWest;
constexpr int kNorthWest = kNorth + kWest;

constexpr std::array<int, 4> kCardinals = {kNorth, kEast, kSouth, kWest};

constexpr std::array<int, 4> kDiagonals = {kNorthEast, kSouthEast, kSouthWest,
                                           kNorthWest};

constexpr std::array<int, 8> kKnightMoves = {
    kNorth + kNorth + kEast, kEast + kEast + kNorth,  kEast + kEast + kSouth,
    kSouth + kSouth + kEast, kSouth + kSouth + kWest, kWest + kWest + kSouth,
    kWest + kWest + kNorth,  kNorth + kNorth + kWest};

}  // namespace direction

}  // namespace chess

#endif  // CHESS_GAME_SRC_CORE_DIRECTION_H_
