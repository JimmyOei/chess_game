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

        void updateWhiteAttackField();

        void updateBlackAttackField();

        bool isKingAttacked(State* state, uint8_t colorOfKing);

        std::vector<std::pair<int, int>> getLegalMovesOfPiece(uint8_t const pieceByte,
                                                              int const x, int const y);

        /**
         * @brief checks if the piece of pieceByte at previous position
         *          (prevX, prevY) can move to the new position (newX, newY)
         *          and returns if this is a legal move
         * 
         * @param pieceByte byte of piece that wants to move
         * @param prevX x-position of current position of piece
         * @param prevY y-position of current position of piece
         * @param newX x-position the piece wants to move to
         * @param newY y-position the piece wants to move to
         * @param enPassantMove will be set to true to indicate that
         *                      a pawn has moved two squares and may be
         *                      capture En Passant by the opponent next move
         * @param castlingMove will be set to true to indicate that the
         *                      move is a castling move
         * @return returns true if the move is a legal move, otherwise false
        */
        bool isLegalMove(uint8_t const pieceByte, 
                         int const prevX, int const prevY, 
                         int const newX, int const newY,
                         bool& enPassantMove, bool& castlingMove);


    private:
        bool attackFieldWhite[BOARD_LENGTH][BOARD_LENGTH];
        bool attackFieldBlack[BOARD_LENGTH][BOARD_LENGTH];

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