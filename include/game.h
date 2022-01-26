#ifndef GAME_H 
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <iostream>

#define MIN_SCREEN_WIDTH 500
#define MIN_SCREEN_HEIGHT 500

class Game {
    public: 
        Game();

        ~Game();

        bool isRunning();

        void init();

        void eventHandler();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;

        int screenWidth;
        int screenHeight;

        bool running;

        void renderBoard();

        void resizeWindow(int const width, int const height);
};

#endif