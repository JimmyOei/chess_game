#ifndef GAME_H 
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <stdint.h>

#include "../include/state.h"



class Game {
    public: 
        Game();

        ~Game();

        State* state;

        // TODO: getAttack.. update.. could maybe be private

        std::vector<std::pair<int, int>> getAttackFieldOfPiece(uint8_t const pieceByte,
                                                               int const x, int const y);

        bool isKingAttacked(State* state, uint8_t colorOfKing);

        std::vector<std::pair<int, int>> getLegalMovesOfPiece(uint8_t const pieceByte,
                                                              int const x, int const y);


    private:
        int gamemode;

        // TODO: use for mate/check
        int xWhiteKing;
        int yWhiteKing;
        int xBlackKing;
        int yBlackKing;

        bool isWhitePiece(uint8_t const pieceByte) {
            return pieceByte & WHITE_PIECE;
        }

        void clearAttackFieldWhite();
        void clearAttackFieldBlack();

};

#endif