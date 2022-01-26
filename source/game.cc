#include "../include/game.h"

Game::Game() {
    running = false;
    window = nullptr;
    renderer = nullptr;
    screenWidth = 600;
    screenHeight = 600;
}

Game::~Game() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
};

bool Game::isRunning() {
    return running;
}

void Game::init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                     screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    renderer = SDL_CreateRenderer(window, -1, 0);

    Game::renderBoard();
    
    running = true;
}

void Game::eventHandler() {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch(event.type) {
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Game::resizeWindow(event.window.data2, event.window.data1);
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Game::renderBoard() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

    // Takes the smallest of screenHeight and screenWidth
    int rectEdge = screenHeight * (screenHeight < screenWidth) 
                   + screenWidth * (screenHeight >= screenWidth);

    int const boardStartingX = ((screenWidth - rectEdge) / 2) * (screenWidth > rectEdge);
    int const boardStartingY = ((screenHeight - rectEdge) / 2) * (screenHeight > rectEdge);
    rectEdge /= 8; // 8 squares per edge

    for(int x = 0; x < 8; x++) {
        for(int y = 0; y < 8; y++) {
            SDL_Rect rect;
            rect.w = rectEdge;
            rect.h = rectEdge;
            rect.x = boardStartingX + x*rectEdge;
            rect.y = boardStartingY + y*rectEdge;
            if((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1)) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } // white plane
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            } // black plane
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void Game::resizeWindow(int const height, int const width) {
    // Takes height/width if it is larger than the minimum, else it takes the minimum
    screenHeight = height * (height > MIN_SCREEN_HEIGHT) 
                   + MIN_SCREEN_HEIGHT * (height <= MIN_SCREEN_HEIGHT);
    screenWidth = width * (width > MIN_SCREEN_WIDTH) 
                  + MIN_SCREEN_WIDTH * (width <= MIN_SCREEN_WIDTH);
    Game::renderBoard();
}