/**
 * @file Game.cc
 * @author James Montyn (jamesmontyn.github.io)
 * @brief source file for the chess Game class
 * @version 0.1
 * @date 2022-01-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../include/game.h"

Game::Game() {
    running = false;
    _window = nullptr;
    _renderer = nullptr;
    _screenWidth = 600;
    _screenHeight = 600;
}

Game::~Game() {
    // destructor
};

bool Game::isRunning() {
    return this->running;
}

void Game::init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    _window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_RESIZABLE);
    
    SDL_SetWindowMinimumSize(_window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);

    _renderer = SDL_CreateRenderer(_window, -1, 0);

    this->running = true;
}


