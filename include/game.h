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

        // TODO: getAttack.. update.. could maybe be private

        bool isKingAttacked(State* state, uint8_t colorOfKing);

        std::vector<int> getLegalMovesOfPiece(uint8_t const pieceByte, int const pos);


    private:
        int gamemode;

        // TODO: use for mate/check
        int xWhiteKing;
        int yWhiteKing;
        int xBlackKing;
        int yBlackKing;

        std::vector<int> getPossibleMoves(uint8_t const pieceByte, int const pos);


};

#endif