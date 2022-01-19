/**
 * @file main.cc
 * @author James Montyn (jamesmontyn.github.io)
 * @brief main.cc file for the chess engine program
 * @version 0.1
 * @date 2022-01-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// for initializing and shutdown functions
#include <SDL2/SDL.h>
 
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
 
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include "../include/game.h"

int main(int argc, char *argv[]) {
    Game* game = new Game;

    game->init();
    while(game->isRunning()) {
        // event loop
    }

    delete game;

    return 0;
}