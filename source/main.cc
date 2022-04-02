#include <SDL2/SDL.h>

#include <iostream>

#include "../include/game.h"
#include "../include/state.h"


void menu(State* state) {
    std::cout << "---------------Chess Game & Engine---------------" << std::endl
              << "  Made By James Montyn at github.com/JamesMontyn " << std::endl
              << "  Programmed in C++, with SDL 2.0                " << std::endl
              << "-------------------------------------------------" << std::endl << std::endl;

    std::string FEN;
    bool menuDone = false;
    while(!menuDone) {
        std::cout << ">> Input a FEN-notation for the game or \"default\" for a new standard game" << std::endl
                << ">> Example of expected input: \"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w\"" << std::endl << std::endl;
        getline(std::cin, FEN);
        std::cout << std::endl;
        if(FEN == "default") {
            FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
            state->setTurn(true);
        }
        else {
            int const FENlength = FEN.length();
            if(FEN[FENlength - 2] != ' ') {
                std::cerr << ">> The given FEN-notation is invalid: " << FEN << std::endl << std::endl;
                continue;
            }
            if(FEN[FENlength - 1] == 'w') {
                FEN.pop_back();
                FEN.pop_back();
                state->setTurn(true);
            }
            else if(FEN[FENlength - 1] == 'b') {
                FEN.pop_back();
                FEN.pop_back();
                state->setTurn(false);
            }             
        }
        if(!state->setByteBoardFromFEN(FEN)) {
            std::cerr << ">> The given FEN-notation is invalid: " << FEN << std::endl << std::endl;
        }
        else {
            menuDone = true;
        }
    }
    std::cout << ">> Good luck!" << std::endl;
}

int main(int argc, char *argv[]) {
    int const frameDelay = 1000 / 60;
    Uint32 frameStart;
    int frameTime;
    SDL_Event event;

    Game* game = new Game;
    State* state = new State;
    menu(state);
    game->init(state);

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
    delete state;

    return 0;
}