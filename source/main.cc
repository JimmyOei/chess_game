#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "../include/game.h"

#include <iostream>

int main(int argc, char *argv[]) {
    int const frameDelay = 1000 / 60;
    Uint32 frameStart = Uint32();
    int frameTime = int();

    Game* game = new Game;

    game->init();
    while(game->isRunning()) {
        frameStart = SDL_GetTicks();

        game->eventHandler();
        
        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameStart) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    delete game;

    return 0;
}