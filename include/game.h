#ifndef GAME_H
#define GAME_H

#include "state.h"
#include "player.h"
#include "piece.h"

class Game
{
public:
    /**
     * @brief constructor for game object
     * 
     * @param playerWhite Player object for white
     * @param playerblack Player object for black
     */
    Game(std::unique_ptr<Player> &playerWhite,
         std::unique_ptr<Player> &playerBlack);

    /**
     * @brief constructs a game using the given state
     * 
     * @param playerWhite Player object for white
     * @param playerblack Player object for black
     * @param state State object to start the game with
     */
    Game(std::unique_ptr<Player> &playerWhite,
        std::unique_ptr<Player> &playerBlack,
        State state);

    ~Game();

private:
    std::unique_ptr<Player> playerWhite;
    std::unique_ptr<Player> playerBlack;
    std::unique_ptr<State> state;
};

#endif