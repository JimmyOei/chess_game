/**
 * This file contains the entry point: it runs the interface's frame loop,
 * letting engines move at most once per kEngineDelayMs.
 */

#include <SDL2/SDL.h>

#include <cstdint>
#include <iostream>
#include <memory>

#include "ui/interface.h"

namespace {

constexpr uint32_t kTargetFps = 60;
constexpr uint32_t kFrameDelayMs = 1000 / kTargetFps;
constexpr uint32_t kEngineDelayMs = 1000;

}  // namespace

// SDL may rename main to SDL_main on some platforms, which needs this exact
// signature.
int main(int /*argc*/, char* /*argv*/[]) {
  std::cout << "---------========== Chess Game ==========----------\n"
            << "  Made By Jimmy Oei at github.com/JimmyOei\n"
            << "-------------------------------------------------\n\n";
  auto interface = std::make_unique<chess::Interface>();
  uint32_t last_engine_move = SDL_GetTicks();
  SDL_Event event;

  interface->Initiate();

  while (interface->IsRunning()) {
    uint32_t const frame_start = SDL_GetTicks();

    /* Process engine moves */
    if (!interface->IsGameOver() &&
        frame_start - last_engine_move >= kEngineDelayMs) {
      last_engine_move = frame_start;
      interface->HandlePlayerTurn();
    }
    interface->Render();

    /* Event handling */
    while (SDL_PollEvent(&event) != 0) {
      interface->EventHandler(event);
    }

    /* Frame timing */
    uint32_t const frame_time = SDL_GetTicks() - frame_start;
    if (kFrameDelayMs > frame_time) {
      SDL_Delay(kFrameDelayMs - frame_time);
    }
  }
  std::cout << "Thank you for playing this Chess program!\n\n";

  return 0;
}
