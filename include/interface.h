#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdint.h>
#include <vector>

#include "game.h"
#include "player.h"
#include "playerhuman.h"
#include "playerenginerandom.h"
#include "playerengineminimax.h"
#include "testsuite.h"

#define MIN_SCREEN_WIDTH 500
#define MIN_SCREEN_HEIGHT 500

#define STARTING_SCREEN_WIDTH 600
#define STARTING_SCREEN_HEIGHT 600

#define NUM_OF_PIECES 12

#define DARK_PAWN_IMG "../pieces/darkPawn.png"
#define LIGHT_PAWN_IMG "../pieces/lightPawn.png"
#define DARK_KNIGT_IMG "../pieces/darkKnight.png"
#define LIGHT_KNIGHT_IMG "../pieces/lightKnight.png"
#define DARK_BISHOP_IMG "../pieces/darkBishop.png"
#define LIGHT_BISHOP_IMG "../pieces/lightBishop.png"
#define DARK_ROOK_IMG "../pieces/darkRook.png"
#define LIGHT_ROOK_IMG "../pieces/lightRook.png"
#define DARK_QUEEN_IMG "../pieces/darkQueen.png"
#define LIGHT_QUEEN_IMG "../pieces/lightQueen.png"
#define DARK_KING_IMG "../pieces/darkKing.png"
#define LIGHT_KING_IMG "../pieces/lightKing.png"

#define STANDARD_OPENING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

/**
 * @brief Interface class for the chess game
 *
 * This class is responsible for the graphical user interface of the chess game.
 * It uses SDL2 and SDL2_image for rendering and handling events.
 */
class Interface
{
public:
    /**
     * @brief constructor for the interface object
     */
    Interface();

    /**
     * @brief destructor for the interface object
     *
     * Cleans up SDL and SDL_image
     */
    ~Interface();

    /**
     * @brief getter for if the interface is running
     */
    bool isRunning();

    bool isGameOver();

    void initiate();

    void handlePlayerTurn();

    void eventHandler(SDL_Event event);

    void render();

private:
    std::unique_ptr<Player> playerWhite;
    std::unique_ptr<Player> playerBlack;
    std::unique_ptr<Game> game;
    TestSuite testSuite;

    SDL_Window *window;
    SDL_Renderer *renderer;

    std::unordered_map<Piece::Type, SDL_Texture *> texturePieces;

    int screenWidth;
    int screenHeight;

    // Length of an edge of a square on the board
    int squareEdgeLength;

    // Top left square position of the board on the window
    int boardStartingX;
    int boardStartingY;

    // bool for if the interface is running
    bool running;
    bool gameOver;

    Piece::Type dragPiece;
    int dragPieceTextureMouseX;
    int dragPieceTextureMouseY;
    Position dragPiecePos;
    std::vector<Move> dragPieceLegalMoves;

    void menuGamemode(Piece::Color const color);

    void menuFEN();

    bool menu();

    Piece::Type menuPawnPromotion();

    /**
     * @brief loads the textures for the pieces into the texturePieces unordered map
     *          as SDL_Texture pointers
     */
    void loadTexturePieces();

    /**
     * @brief getter for the texture of a piece
     *
     * @param piece piece to get the texture of
     * @return SDL_Texture* texture of the piece
     *          or throws an invalid argument exception if the piece is not found
     */
    SDL_Texture *getTexturePiece(Piece::Type piece);

    void renderBoard();

    void renderState();

    void resizeWindow(int const height, int const width);

    void pickupDragPiece(int const mouseX, int const mouseY);

    void renderDragPiece(int const mouseX, int const mouseY);

    void releaseDragPiece(int const mouseX, int const mouseY);
};

#endif