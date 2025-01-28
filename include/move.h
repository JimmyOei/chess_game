#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include "position.h"

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
    Position from;
    Position to;
    Piece::Type piece;
    Piece::Type promotionPiece = Piece::Type::BLANK;

    Move(Position from, Position to, Piece::Type piece, Piece::Type promotionPiece = Piece::Type::BLANK)
        : from(from), to(to), piece(piece), promotionPiece(promotionPiece) {}

    bool operator==(Move const &other) const
    {
        return from == other.from && to == other.to && piece == other.piece && promotionPiece == other.promotionPiece;
    }
};

#endif