#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>

#include "piece.h"
#include "move.h"
#include "position.h"
#include "direction.h"
#include "logger.h"

#define STANDARD_OPENING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

enum Result
{
    WHITE_WIN,
    BLACK_WIN,
    DRAW,
    ONGOING
};

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

    int getMoveCounter();

    Result getResult();

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
     * @brief gets all pieces of a given color
     *
     * @param color color of the pieces to get
     * @return vector of all pieces of the given color as a pair of the piece type and the position
     */
    std::vector<std::pair<Piece::Type, Position>> getAllPiecesForColor(Piece::Color const color);

    std::vector<Move> getAllLegalMoves();

    /**
     * @brief checks if the king of a given color is in check
     *
     * @param color color of the king to check
     * @return true if the king is in check, otherwise false
     */
    bool isKingInCheck(Piece::Color const color);

    /**
     * @brief gets all legal moves for a given position
     *
     * @param pos position to get the legal moves for
     * @return vector of legal moves for the given position
     */
    std::vector<Move> getLegalMovesForPos(Position const pos);

    /**
     * @brief makes a move on the board
     *
     * @param move move to make
     * @return true if the move is legal and made, otherwise false
     */
    void makeMove(Move const move);

    /**
     * @brief prints the current game of the board to stdout for debugging
     */
    void printGame();

    bool isGameOver();

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
    void passTurn(Position newEnPassantPos);

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

    int moveCounter;
    Result result;
};

#endif