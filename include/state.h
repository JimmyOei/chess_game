#ifndef STATE_H 
#define STATE_H

#include <stdint.h>
#include <string>
#include <vector>

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

/* board */
#define BOARD_LENGTH 8
#define BOARD_SIZE BOARD_LENGTH*BOARD_LENGTH

/* turn */
#define WHITE true
#define BLACK false;

/* directions */
#define NORTH BOARD_LENGTH
#define EAST +1
#define WEST -1
#define SOUTH -BOARD_LENGTH

inline bool getColorOfPiece(uint8_t const pieceByte) {
    return pieceByte & WHITE_PIECE;
}

class State {
    public:
        /**
         * @brief sets all private booleans to false, except turn
         *          turn will be set to true, because white starts the game
        */
        State();

        bool withinBoardLimits(int const pos);

        /**
         * @brief passes turn and resetting the booleans enPassant 
         *          and check to false
        */
        void passTurn();

        /**
         * @brief getter for the boolean turn
         * 
         * @return returns the boolean value of turn:
         *          true for white
         *          false for black
        */
        bool getTurn();

        int getKingPos(bool const color);

        /**
         * @brief gets the byte from the ByteBoard at position (x, y)
         * 
         * @param x point on the horizontal axis of the board
         * @param y point on the vertical axis of the board
         * @return byte/uint8_t on position (x, y)
        */
        uint8_t getPieceAt(int const pos);

        // Note: pawn advance can be down with movePiece(pieceByteToAdvanceTo, prevPos, newPos)
        void movePiece(uint8_t const pieceByte, int const prevPos, int const newPos);

        int getEnPassantPos();

        /**
         * @brief sets the byteBoard according to the given FEN-notation
         * 
         * @param FEN FEN-notation to set the byteBoard to
         * @return true if FEN is a legal FEN-notation and byteBoard
         *          is set succesfully, otherwise false
        */
        bool setStateFromFEN(std::string FEN);

        // /**
        //  * @brief translates the byteBoard to FEN-notation and returns this
        //  * 
        //  * @return the FEN-notation of the current state
        // */
        // std::string getFEN();

        void debugPrintState();

    private:
        uint8_t byteBoard[BOARD_SIZE];

        // true for white's turn, false for black's turn
        bool turn;

        int enPassantPos;

        bool whiteCastlingQueenside;
        bool whiteCastlingKingside;
        bool blackCastlingQueenside;
        bool blackCastlingKingside;

        // NOT IMPLEMENTED YET
        // int halfMoveClock;
        // int fullMoveNumber;

        int whiteKingPos;
        int blackKingPos;

        void clearState();
};

#endif