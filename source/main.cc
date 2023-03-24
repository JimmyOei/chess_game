#include <SDL2/SDL.h>

#include "../include/interface.h"

int main(int argc, char *argv[]) {
    std::cout << "---------========== Chess Game ==========----------" << std::endl
              << "  Made By James Montyn at github.com/JamesMontyn " << std::endl
              << "  Programmed in C++, with SDL 2.0                " << std::endl
              << "-------------------------------------------------" << std::endl << std::endl;
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
    std::cout << "Thank you for playing this Chess program!" << std::endl << std::endl;

    delete interface;

    return 0;
}