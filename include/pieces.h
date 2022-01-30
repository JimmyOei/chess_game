#ifndef STATE_H 
#define STATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define NUMBER_OF_PIECES 12

enum class piecesNames {
    darkPawn,
    lightPawn,
    darkKnight,
    lightKnight,
    darkBishop,
    lightBishop,
    darkRook,
    lightRook,
    darkQueen,
    lightQueen,
    darkKing,
    lightKing
};

class Pieces {
    public: 
        void init();

    private:
        SDL_Texture* pieces[NUMBER_OF_PIECES];
};

#endif