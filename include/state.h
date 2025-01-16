#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <string>
#include <vector>

#include "piece.h"
#include "move.h"

#define STANDARD_OPENING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

/* board */
#define BOARD_LENGTH 8
#define BOARD_SIZE BOARD_LENGTH *BOARD_LENGTH

/* directions */
#define NORTH BOARD_LENGTH
#define EAST +1
#define WEST -1
#define SOUTH -BOARD_LENGTH

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
     * @brief getter for if the king of color who's turn it is, is in check
     * 
     * @return true if the king of the color who's turn it is, is in check, else false
     */
    bool isCheck();

    /**
     * @brief getter for the position of the king of a given color
     *
     * @param color color of the king to get the position of
     * @return position of the king of the given color
     */
    int getKingPosOfColor(Color const color);

    /**
     * @brief getter for the piece at a given position
     *
     * @param pos position to get the piece at
     * @return the piece at the given position
     */
    Piece getPieceAtPos(int const pos);

    bool makeMove(Move const move);

    std::vector<Move> getLegalMovesForPos(int const pos);

    int getEnPassantPos();

    bool getCastlingKingSide(Color const color);

    bool getCastlingQueenSide(Color const color);

    /**
     * @brief initiates the state according to the given FEN-notation
     *
     * @param FEN FEN-notation to set the state to
     * @return true if FEN is a legal FEN-notation and state
     *          is set succesfully, otherwise false
     */
    bool initStateFromFENString(std::string FEN);

    /**
     * @brief prints the current state of the board to stdout for debugging
     */
    void printState();

private:
    Piece board[BOARD_SIZE];

    Color turn;

    bool check;

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