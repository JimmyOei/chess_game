#ifndef GAME_H 
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <stdint.h>

#include "state.h"

#define MIN_SCREEN_WIDTH 500
#define MIN_SCREEN_HEIGHT 500

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


class Game {
    public: 
        Game();

        ~Game();

        bool isRunning();

        void init();

        void eventHandler();

        void render();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Texture* pieces[NUM_OF_PIECES];

        int screenWidth;
        int screenHeight;

        int squareEdge;
        int boardStartingX;
        int boardStartingY;

        bool running;

        uint8_t dragPieceByte;
        int dragPieceTextureMouseX;
        int dragPieceTextureMouseY;
        int dragPieceInitialSquareX;
        int dragPieceInitialSquareY;

        State* state;

        void loadPieces();

        SDL_Texture* getTexturePieceFromByte(uint8_t byte);

        void renderBoard();

        void renderState();

        void resizeWindow(int const height, int const width);

        void pickupDragPiece(int const mouseX, int const mouseY);

        void renderDragPiece(int const mouseX, int const mouseY);

        void releaseDragPiece(int const mouseX, int const mouseY);
};

#endif