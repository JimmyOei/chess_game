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

class State {
    public: 
        State();

        bool setByteBoardFromFEN(std::string FEN);

        uint8_t getByteFromByteBoard(int const x, int const y);

        std::string getFEN();

        void setTurn(bool turn);

        bool getTurn();

        void setByteInByteBoard(int const x, int const y, uint8_t const bit);

        bool isLegalMove(uint8_t const pieceByte, 
                         int const prevX, int const prevY, 
                         int const newX, int const newY);

    private:
        uint8_t byteBoard[8][8]; 

        // true for white's turn, false for black's turn
        bool turn;
};

#endif