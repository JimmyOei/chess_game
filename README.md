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

## License

The chess pieces in this project are made by Cburnett and unchanged.
They are licensed with the Creative Commons Attribution-Share Alike 3.0 Unported license
and can be found in the following link: https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent
and https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces.
