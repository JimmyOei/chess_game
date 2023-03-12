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

/* board */
#define BOARD_LENGTH 8

class State {
    public:
        /**
         * @brief sets all private booleans to false, except turn
         *          turn will be set to true, because white starts the game
        */
        State();

        /**
         * @brief sets the byteBoard according to the given FEN-notation
         * 
         * @param FEN FEN-notation to set the byteBoard to
         * @return true if FEN is a legal FEN-notation and byteBoard
         *          is set succesfully, otherwise false
        */
        bool setByteBoardFromFEN(std::string FEN);

        /**
         * @brief gets the byte from the ByteBoard at position (x, y)
         * 
         * @param x point on the horizontal axis of the board
         * @param y point on the vertical axis of the board
         * @return byte/uint8_t on position (x, y)
        */
        uint8_t getByteFromByteBoard(int const x, int const y);

        /**
         * @brief translates the byteBoard to FEN-notation and returns this
         * 
         * @return the FEN-notation of the current state
        */
        std::string getFEN();

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

        /**
         * @brief sets the byte in the byteBoard at 
         *          position (x, y) to given byte
         * 
         * @param byte byte to put at position (x, y)
         * @param x point on the horizontal axis of the board
         * @param y point on the vertical axis of the board
        */
        void setByteInByteBoard(uint8_t const byte, int const x, int const y);

        /**
         * @brief setting the En Passant square positions 
         *          and boolean enPassant to true
         * 
         * @param x point on the horizontale axis
         * @param y point on the vertical axis
        */
        void setEnPassantSquare(int const x, int const y);
    
        /**
         * @brief checks if the piece pieceByte that just moved
         *          to position (x, y) is checking the enemy king,
         *          if so, set private boolean check to true
         * 
         * @param pieceByte byte of the piece that can create check
         * @param x point on the horizontale axis
         * @param y point on the vertical axis
        */
        void isCheck(uint8_t const pieceByte, 
                     int const x, int const y);

        void isMate();

        /**
         * @brief checks if the piece of pieceByte at previous position
         *          (prevX, prevY) can move to the new position (newX, newY)
         *          and returns if this is a legal move
         * 
         * @param pieceByte byte of piece that wants to move
         * @param prevX x-position of current position of piece
         * @param prevY y-position of current position of piece
         * @param newX x-position the piece wants to move to
         * @param newY y-position the piece wants to move to
         * @param enPassantMove will be set to true to indicate that
         *                      a pawn has moved two squares and may be
         *                      capture En Passant by the opponent next move
         * @param castlingMove will be set to true to indicate that the
         *                      move is a castling move
         * @return returns true if the move is a legal move, otherwise false
        */
        bool isLegalMove(uint8_t const pieceByte, 
                         int const prevX, int const prevY, 
                         int const newX, int const newY,
                         bool& enPassantMove, bool& castlingMove);

    private:
        uint8_t byteBoard[BOARD_LENGTH][BOARD_LENGTH];

        bool enPassant;
        int xEnPassantSquare;
        int yEnPassantSquare; 

        bool whiteCastlingQueenside;
        bool whiteCastlingKingside;
        bool blackCastlingQueenside;
        bool blackCastlingKingside;

        // true if the king of the color to move is under attack, else false
        bool check;

        // true for white's turn, false for black's turn
        bool turn;

        bool isWhitePiece(uint8_t const pieceByte) {
            return pieceByte & WHITE_PIECE;
        }

};

#endif