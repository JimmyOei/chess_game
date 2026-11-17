## Chess Game

This is a personal project. I am creating a chess game in C++ with SDL 2.0.

## Building and running

Tested on Ubuntu 24.04 under WSL2.

1. Install the compiler, build tools and SDL2:
   ```bash
   sudo apt update
   sudo apt install build-essential gdb cmake ninja-build pkg-config libsdl2-dev libsdl2-image-dev
   ```
2. Configure, build and test:
   ```bash
   cmake --preset debug
   cmake --build --preset debug
   ctest --preset debug
   ```
3. Run the game (it loads the piece images relative to its directory):
   ```bash
   cd build/debug/bin && ./chess_game
   ```

`ctest --preset release` also runs the deep perft tests, which are too slow
under the debug build's sanitizers.

## Code layout

All code is in the `chess` namespace.

- `src/core/`: board types, the `Game` rules and move generation, and perft.
  No SDL, so it can be tested headless.
- `src/player/`: the `Player` interface, the human player, and the random and
  minimax engines.
- `src/ui/`: the SDL window, the console menus and `main`.
- `src/util/`: the logger.
- `tests/`: GoogleTest tests, one file per class, plus perft.

## License

The chess pieces in this project are made by Cburnett and unchanged.
They are licensed with the Creative Commons Attribution-Share Alike 3.0 Unported license
and can be found in the following link: https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent
and https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces.
