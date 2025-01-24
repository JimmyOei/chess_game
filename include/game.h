#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>

#include "piece.h"
#include "move.h"
#include "position.h"
#include "direction.h"

#define STANDARD_OPENING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

/* board */
#define BOARD_LENGTH 8
#define BOARD_SIZE BOARD_LENGTH * BOARD_LENGTH

class Game
{
public:
    /**
     * @brief default constructor, initiates the game to the standard chess starting position
     */
    Game();

    /**
     * @brief initiates the game from a given FEN-notation
     *          if the FEN-notation is not legal, the game
     *          will be set to an empty board
     *
     * @param FEN FEN-notation to create the game from
     */
    Game(const std::string FEN);

    /**
     * @brief copy constructor
     *
     * @param game game to copy
     */
    Game(Game const &game);

    ~Game() = default;

    /**
     * @brief getter for the boolean turn
     *
     * @return returns the color of the player who's turn it is
     */
    Piece::Color getTurn();

    /**
     * @brief getter for the position of the en passant pawn
     */
    Position getEnPassantPos();

    /**
     * @brief getter for the boolean castling king side
     */
    bool getCastlingKingSide(Piece::Color const color);

    /**
     * @brief getter for the boolean castling queen side
     */
    bool getCastlingQueenSide(Piece::Color const color);

    /**
     * @brief getter for the position of the king of a given color
     *
     * @param color color of the king to get the position of
     * @return position of the king of the given color
     */
    Position getKingPosOfPiece(Piece::Color const color);

    /**
     * @brief getter for the piece at a given position
     *
     * @param pos position to get the piece at
     * @return the piece at the given position
     */
    Piece::Type getPieceAtPos(Position const pos);

    /**
     * @brief getter for if the king of color who's turn it is, is in check
     * 
     * @return true if the king of the color who's turn it is, is in check, else false
     */
    bool isKingInCheck(Piece::Color const color);

    /** 
     * @brief gets all legal moves for a given position
     * 
     * @param pos position to get the legal moves for
     * @return vector of legal moves for the given position
    */
    std::vector<Move> getLegalMovesForPos(int const pos);

    /**
     * @brief makes a move on the board
     * 
     * @param move move to make
     * @return true if the move is legal and made, otherwise false
     */
    bool makeMove(Move const move);

    /**
     * @brief prints the current game of the board to stdout for debugging
     */
    void printGame();

private:
    /**
     * @brief initiates the game according to the given FEN-notation
     *
     * @param FEN FEN-notation to set the game to
     * @return true if FEN is a legal FEN-notation and game
     *          is set succesfully, otherwise false
     */
    bool initGameFromFENString(std::string FEN);

    /**
     * @brief passes turn and resetting the booleans enPassant
     *          and check to false
     */
    void passTurn();

    /**
     * @brief initiates the game to the standard chess starting position
     */
    void initGame();

    /* Chess game data */
    Piece::Type board[BOARD_SIZE];
    Piece::Color turn;
    Position enPassantPos;
    bool whiteCastlingQueenside;
    bool whiteCastlingKingside;
    bool blackCastlingQueenside;
    bool blackCastlingKingside;
    Position whiteKingPos;
    Position blackKingPos;

    // NOT IMPLEMENTED YET
    // int halfMoveClock;
    // int fullMoveNumber;
};

#endif