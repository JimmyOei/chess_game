#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

/**
 * @brief struct for a move
 *
 * from: position of the piece to move
 * to: position to move the piece to
 * piece: piece to move
 * promotionPiece: piece to promote to if any, otherwise Piece::NO_PIECE
 */
struct Move
{
    int from;
    int to;
    Piece::Type piece;
    Piece::Type promotionPiece = Piece::Type::NO_PIECE;
};

#endif