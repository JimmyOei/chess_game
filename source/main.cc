#include <SDL2/SDL.h>

#include "../include/game.h"

#include <iostream>

int main(int argc, char *argv[]) {
    int const frameDelay = 1000 / 60;
    Uint32 frameStart;
    int frameTime;
    SDL_Event event;

    Game* game = new Game;

    game->init();
    while(game->isRunning()) {
        frameStart = SDL_GetTicks();

        if(SDL_WaitEvent(&event) != 0) {
            game->eventHandler(event);
            game->render();
        }

        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameStart) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    delete game;

    return 0;
}