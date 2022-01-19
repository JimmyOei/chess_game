/**
 * @file Game.h
 * @author James Montyn (jamesmontyn.github.io)
 * @brief Header file for the chess Game class
 * @version 0.1
 * @date 2022-01-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef GAME_H // prevents multiple
#define GAME_H // includes of this file

// for initializing and shutdown functions
#include <SDL2/SDL.h>
 
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
 
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#define MIN_SCREEN_WIDTH 400
#define MIN_SCREEN_HEIGHT 400

class Game {
    public: 
        /**
         * @brief Constructor for a Game object
         */
        Game();

        /**
         * @brief Destructor for a Game object
         */
        ~Game();

        /**
         * @brief Getter for private boolean "running"
         * @return true if the game is running
         * @return false if the game is not running
         */
        bool isRunning();

        /**
         * @brief Initiates the game by making the SDL window
         */
        void init();


    private:
        SDL_Window* _window;
        SDL_Renderer* _renderer;

        int _screenWidth;
        int _screenHeight;

        bool running;
};

#endif