#ifndef STATE_H 
#define STATE_H

#include <stdint.h>
#include <string>

/**
 * Chess board represented in uint8_t, 
 * which represents the piece and colour as follows:
 * pawn   = 0b00000001
 * knight = 0b00000010
 * bishop = 0b00000100
 * rook   = 0b00001000
 * queen  = 0b00010000
 * king   = 0b00100000
 *
 * black  = 0b01000000
 * white  = 0b10000000
 */

/* white pieces */
#define WHITE_PAWN 0b10000001
#define WHITE_KNIGHT 0b10000010
#define WHITE_BISHOP 0b10000100
#define WHITE_ROOK 0b10001000
#define WHITE_QUEEN 0b10010000
#define WHITE_KING 0b10100000

/* black pieces */
#define BLACK_PAWN 0b01000001
#define BLACK_KNIGHT 0b01000010
#define BLACK_BISHOP 0b01000100
#define BLACK_ROOK 0b01001000
#define BLACK_QUEEN 0b01010000
#define BLACK_KING 0b01100000

/* general */
#define NO_PIECE 0b00000000
#define WHITE_PIECE 0b10000000
#define BLACK_PIECE 0b01000000

class State {
    public: 
        State();

        bool setByteBoardFromFEN(std::string FEN);

        uint8_t getByteFromByteBoard(int const x, int const y);

        std::string getFEN();

        void passTurn();

        bool getTurn();

        void setByteInByteBoard(int const x, int const y, uint8_t const bit);

        void setEnPassantSquare(int const x, int const y);
    
        void isCheck(uint8_t const pieceByte, 
                     int const x, int const y);

        void isMate();

        bool isLegalMove(uint8_t const pieceByte, 
                         int const prevX, int const prevY, 
                         int const newX, int const newY,
                         bool& enPassantMove, bool& castlingMove);
        bool check;

    private:
        uint8_t byteBoard[8][8];

        bool enPassant;
        int xEnPassantSquare;
        int yEnPassantSquare; 

        bool whiteCastlingQueenside;
        bool whiteCastlingKingside;
        bool blackCastlingQueenside;
        bool blackCastlingKingside;


        // true for white's turn, false for black's turn
        bool turn;

        bool isWhitePiece(uint8_t const pieceByte) {
            return pieceByte & WHITE_PIECE;
        }

};

#endif