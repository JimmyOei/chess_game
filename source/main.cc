#include <SDL2/SDL.h>

#include "../include/interface.h"

int main(int argc, char *argv[]) {
    int const frameDelay = 1000 / 60;
    Interface* interface = new Interface;
    Uint32 frameStart;
    int frameTime;
    SDL_Event event;

    interface->initiate();

    while(interface->isRunning()) {
        frameStart = SDL_GetTicks();

        if(SDL_WaitEvent(&event) != 0) {
            interface->eventHandler(event);
            interface->render();
        }

        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameStart) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    delete interface;

    return 0;
}