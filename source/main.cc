#include <SDL2/SDL.h>

#include "../include/interface.h"

#define TARGET_FPS 60
#define ENGINE_DELAY 1000

int main(int argc, char *argv[])
{
    std::cout << "---------========== Chess Game ==========----------" << std::endl
              << "  Made By James Montyn at github.com/JamesMontyn " << std::endl
              << "  Programmed in C++, with SDL 2.0                " << std::endl
              << "-------------------------------------------------" << std::endl
              << std::endl;
    int const frameDelay = 1000 / TARGET_FPS;
    Interface *interface = new Interface;
    Uint32 frameStart;
    Uint32 lastEngineMove = SDL_GetTicks();
    int frameTime;
    SDL_Event event;

    interface->initiate();

    while (interface->isRunning())
    {
        frameStart = SDL_GetTicks();

        /* Process engine moves */
        if (!interface->isGameOver() && frameStart - lastEngineMove >= ENGINE_DELAY)
        {
            lastEngineMove = frameStart;
            interface->handlePlayerTurn();
        }
        interface->render();

        /* Event handling */
        while (SDL_PollEvent(&event))
        {
            interface->eventHandler(event);
        }

        /* Frame timing */
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    std::cout << "Thank you for playing this Chess program!" << std::endl
              << std::endl;

    delete interface;

    return 0;
}