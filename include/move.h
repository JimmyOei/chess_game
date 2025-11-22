#ifndef MOVE_H
#define MOVE_H

#include <iostream>

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

    Move(): from(-1), to(-1), piece(Piece::Type::BLANK), promotionPiece(Piece::Type::BLANK) {}

    Move(Position from, Position to, Piece::Type piece, Piece::Type promotionPiece = Piece::Type::BLANK)
        : from(from), to(to), piece(piece), promotionPiece(promotionPiece) {}

    Move(const Move& other) : from(other.from), to(other.to), piece(other.piece), promotionPiece(other.promotionPiece) {}

    Move(Move&& other) : from(other.from), to(other.to), piece(other.piece), promotionPiece(other.promotionPiece) {}

    Move &operator=(const Move &other)
    {
        from = other.from;
        to = other.to;
        piece = other.piece;
        promotionPiece = other.promotionPiece;
        return *this;
    }

    bool operator==(Move const &other) const
    {
        return from == other.from && to == other.to && piece == other.piece && promotionPiece == other.promotionPiece;
    }
};

static inline std::ostream &operator<<(std::ostream &os, Move const &move)
{
    os << Piece::pieceToChar(move.piece) << move.from << "-" << move.to;
    if (move.promotionPiece != Piece::Type::BLANK)
    {
        os << " -> " << Piece::pieceToChar(move.promotionPiece);
    }
    return os;
}

#endif