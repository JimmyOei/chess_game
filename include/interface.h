#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <stdint.h>
#include <vector>

#include "state.h"
#include "game.h"
#include "piece.h"

#define MIN_SCREEN_WIDTH 500
#define MIN_SCREEN_HEIGHT 500

#define STARTING_SCREEN_WIDTH 600
#define STARTING_SCREEN_HEIGHT 600

#define NUM_OF_PIECES 12

#define DARK_PAWN_IMG "./pieces/darkPawn.png"
#define LIGHT_PAWN_IMG "./pieces/lightPawn.png"
#define DARK_KNIGT_IMG "./pieces/darkKnight.png"
#define LIGHT_KNIGHT_IMG "./pieces/lightKnight.png"
#define DARK_BISHOP_IMG "./pieces/darkBishop.png"
#define LIGHT_BISHOP_IMG "./pieces/lightBishop.png"
#define DARK_ROOK_IMG "./pieces/darkRook.png"
#define LIGHT_ROOK_IMG "./pieces/lightRook.png"
#define DARK_QUEEN_IMG "./pieces/darkQueen.png"
#define LIGHT_QUEEN_IMG "./pieces/lightQueen.png"
#define DARK_KING_IMG "./pieces/darkKing.png"
#define LIGHT_KING_IMG "./pieces/lightKing.png"

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
    Interface();

    ~Interface();

    bool isRunning();

    void initiate();

    void eventHandler(SDL_Event event);

    void render();

private:
    Game *game;

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *pieces[NUM_OF_PIECES];
    std::unordered_map<Piece, SDL_Texture*> pieces;

    int screenWidth;
    int screenHeight;

    int squareEdge;
    int boardStartingX;
    int boardStartingY;

    bool running;

    Piece dragPiece;
    int dragPieceTextureMouseX;
    int dragPieceTextureMouseY;
    int dragPiecePos;
    std::vector<Piece> *dragPieceLegalMoves;

    void menuGamemode(bool const color);

    void menuFEN();

    bool menu();

    uint8_t menuPawnPromotion();

    void loadPieces();

    SDL_Texture *getTexturePieceFromByte(Piece piece);

    void renderBoard();

    void renderState();

    void resizeWindow(int const height, int const width);

    void pickupDragPiece(int const mouseX, int const mouseY);

    void renderDragPiece(int const mouseX, int const mouseY);

    void releaseDragPiece(int const mouseX, int const mouseY);
};

#endif