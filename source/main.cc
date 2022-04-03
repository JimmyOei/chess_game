#include <SDL2/SDL.h>

#include <iostream>

#include "../include/game.h"
#include "../include/state.h"


void menu(State* state, int& gamemode) {
    std::cout << "---------------Chess Game & Engine---------------" << std::endl
              << "  Made By James Montyn at github.com/JamesMontyn " << std::endl
              << "  Programmed in C++, with SDL 2.0                " << std::endl
              << "-------------------------------------------------" << std::endl << std::endl;

    std::string FEN;
    std::string inputGamemode;
    bool menuDone = false;
    while(!menuDone) {
        std::cout << ">> Input the number of the gamemode of Chess you would like to play" << std::endl
                  << ">> \"1\": 2 players | \"2\": 1 player against engine | \"3\": 1 player against random moves" << std::endl << std::endl;
        getline(std::cin, inputGamemode);
        std::cout << std::endl;
        if(inputGamemode.length() == 1) {
            switch(inputGamemode[0]) {
                case '1':
                    gamemode = 1;
                    menuDone = true;
                    break;
                case '2':
                    gamemode = 2;
                    menuDone = true;
                    break;
                case '3':
                    gamemode = 3;
                    menuDone = true;
                    break;
            }
        }
        if(!menuDone) {
            std::cerr << ">> Invalid input, please try again" << std::endl << std::endl;
        }
    }
    menuDone = false;
    while(!menuDone) {
        std::cout << ">> Input a FEN-notation for the game or \"default\" for a new standard game" << std::endl
                  << ">> Example of expected input: \"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3\"" << std::endl << std::endl;
        getline(std::cin, FEN);
        std::cout << std::endl;
        if(FEN == "default") {
            FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
        }
        if(!state->setByteBoardFromFEN(FEN)) {
            std::cerr << ">> The given FEN-notation is invalid: " << FEN << std::endl << std::endl;
        }
        else {
            menuDone = true;
        }
    }
    std::cout << ">> Good luck!" << std::endl;
    std::cout << state->getFEN() << std::endl;
}

int main(int argc, char *argv[]) {
    int const frameDelay = 1000 / 60;
    Uint32 frameStart;
    int frameTime;
    SDL_Event event;

    Game* game = new Game;
    State* state = new State;
    int gamemode = 0;
    menu(state, gamemode);
    game->init(state, gamemode);

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