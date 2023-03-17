#ifndef GAME_H 
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <stdint.h>

#include "state.h"

class Game {
    public: 
        Game();

        ~Game();

        State* state;

        std::vector<int> getLegalMoves(uint8_t const pieceByte, int const pos);

    private:
        int gamemode;

        bool isKingAttacked();

        std::vector<int> getPossibleMoves(uint8_t const pieceByte, int const pos);


};

#endif