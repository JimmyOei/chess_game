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

#define STANDARD_OPENING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

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
#define BOARD_SIZE BOARD_LENGTH * BOARD_LENGTH

/* directions */
#define NORTH BOARD_LENGTH
#define EAST +1
#define WEST -1
#define SOUTH -BOARD_LENGTH

enum class Color
{
    WHITE,
    BLACK
};

inline bool getColorOfPiece(uint8_t const pieceByte)
{
    return pieceByte & WHITE_PIECE;
}

class State
{
public:
    /**
     * @brief default constructor, initiates the state to the standard chess starting position
     */
    State();

    /**
     * @brief initiates the state from a given FEN-notation
     *          if the FEN-notation is not legal, the state
     *          will be set to an empty board
     *
     * @param FEN FEN-notation to create the state from
     */
    State(const std::string FEN);

    /**
     * @brief copy constructor
     *
     * @param state state to copy
     */
    State(State const &state);

    ~State() = default;

    bool isPosWithinBoardLimits(int const pos);

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
    Color getTurn();

    /**
     * @brief getter for the position of the king of a given color
     * 
     * @param color color of the king to get the position of
     * 
     * @return position of the king of the given color
     */
    int getKingPosOfColor(Color const color);

    uint8_t getPieceAtPos(int const pos);

    // Note: pawn advance can be down with movePiece(pieceByteToAdvanceTo, prevPos, newPos)
    void movePiece(uint8_t const pieceByte, int const prevPos, int const newPos);

    int getEnPassantPos();

    bool getCastlingKingSide(Color const color);

    bool getCastlingQueenSide(Color const color);

    void setSpecialMovesData(uint8_t const pieceByte, int const prevPos, int const newPos);

    /**
     * @brief initiates the state according to the given FEN-notation
     *
     * @param FEN FEN-notation to set the state to
     * @return true if FEN is a legal FEN-notation and state
     *          is set succesfully, otherwise false
     */
    bool initStateFromFEN(std::string FEN);

    // /**
    //  * @brief translates the byteBoard to FEN-notation and returns this
    //  *
    //  * @return the FEN-notation of the current state
    // */
    // std::string getFEN();

    void debugPrintState();

private:
    uint8_t byteBoard[BOARD_SIZE];

    Color turn;

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

    void initState();
};

#endif