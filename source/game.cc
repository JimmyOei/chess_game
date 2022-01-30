#include "../include/game.h"

Game::Game() {
    window = nullptr;
    renderer = nullptr;
    for(int i = 0; i < NUM_OF_PIECES; i++) {
        pieces[i] = nullptr;
    }
    screenWidth = 600;
    screenHeight = 600;
    running = false;
    state = new State;
}

Game::~Game() {
    state->~State();
    state = nullptr;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    for(int i = 0; i < 12; i++) {
        SDL_DestroyTexture(pieces[i]);
    }
    IMG_Quit();
};

bool Game::isRunning() {
    return running;
}

void Game::init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Failed to initiate SDL: " << SDL_GetError() << std::endl;
        return;
    }
    else {
        std::cout << "Succesfully initiated SDL" << std::endl;
    }

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
       std::cerr << "Failed to initiate IMG: " << SDL_GetError() << std::endl;
       return;
    }
    else {
        std::cout << "Succesfully initiated IMG" << std::endl;
    }

    Game::loadPieces();

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

void Game::loadPieces() {
    char* PNGLocations[NUM_OF_PIECES];
    PNGLocations[0] = (char*)DARK_PAWN_IMG;
    PNGLocations[1] = (char*)LIGHT_PAWN_IMG;
    PNGLocations[2] = (char*)DARK_KNIGT_IMG;
    PNGLocations[3] = (char*)LIGHT_KNIGHT_IMG;
    PNGLocations[4] = (char*)DARK_BISHOP_IMG;
    PNGLocations[5] = (char*)LIGHT_BISHOP_IMG;
    PNGLocations[6] = (char*)DARK_ROOK_IMG;
    PNGLocations[7] = (char*)LIGHT_ROOK_IMG;
    PNGLocations[8] = (char*)DARK_QUEEN_IMG;
    PNGLocations[9] = (char*)LIGHT_QUEEN_IMG;
    PNGLocations[10] = (char*)DARK_KING_IMG;
    PNGLocations[11] = (char*)LIGHT_KING_IMG;

    for(int i = 0; i < NUM_OF_PIECES; i++) {
        SDL_Surface* image = IMG_Load(PNGLocations[i]);
        if(!image) {
            std::cerr << "Failed to load image from: " << PNGLocations[i] << std::endl;
        }
        pieces[i] = SDL_CreateTextureFromSurface(renderer, image);
        SDL_FreeSurface(image);
    }
}

void Game::renderBoard() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

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

    renderState();

    SDL_RenderPresent(renderer);
}

void Game::renderState() {
    std::string FEN = state->getFEN();
    std::cout << FEN << std::endl;
}

void Game::resizeWindow(int const height, int const width) {
    screenHeight = height * (height > MIN_SCREEN_HEIGHT) 
                   + MIN_SCREEN_HEIGHT * (height <= MIN_SCREEN_HEIGHT);
    screenWidth = width * (width > MIN_SCREEN_WIDTH) 
                  + MIN_SCREEN_WIDTH * (width <= MIN_SCREEN_WIDTH);
    Game::renderBoard();
}