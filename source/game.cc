#include "../include/game.h"

Game::Game(std::unique_ptr<Player> &playerWhite,
           std::unique_ptr<Player> &playerBlack)
    : playerWhite(std::move(playerWhite)),
      playerBlack(std::move(playerBlack)),
      state(new State())
{
}

Game::Game(std::unique_ptr<Player> &playerWhite,
           std::unique_ptr<Player> &playerBlack,
           State state)
    : playerWhite(std::move(playerWhite)),
      playerBlack(std::move(playerBlack)),
      state(new State(state))
{
}

Game::~Game()
{
}