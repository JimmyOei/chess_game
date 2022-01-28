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

        std::string getFEN();

    private:
        uint8_t bitBoard[8][8]; 
};

#endif