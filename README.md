## Chess Game

A chess game in C++20 with an SDL2 board, built as a personal project. Play
against another person or a built-in engine, or watch two engines play each
other.

### Move Generation
Move generation is checked with perft against the published reference counts. https://chessprogramming.org/Perft_Results using perft tests.

### Engines

#### Random
An engine that takes random moves.

#### Minimax
A minimax engine, optimized with:
- Alhpa-beta pruning
- Iterative deepening under a 3 seconds time budget
- With piece values and piece-square tables from Tomasz Michniewski https://chessprogramming.org/Simplified_Evaluation_Function
- MVV-LVA move ordering heuristics https://chessprogramming.org/MVV-LVA

Results of running `build/release/chess_bench --depth 4 --time 3000 --repeat 3`:

| Position   | Depth 4: time | Depth 4: nodes/s | 3 s: depth reached | 3 s: nodes/s |
|------------|--------------:|-----------------:|-------------------:|-------------:|
| Start      |         34 ms |             295k |                  5 |         289k |
| Italian    |         50 ms |             260k |                  5 |         275k |
| Kiwipete   |         43 ms |             233k |                  6 |         241k |
| Middlegame |         47 ms |             242k |                  5 |         251k |
| Endgame    |          2 ms |             557k |                  8 |         410k |
| **Total**  |    **176 ms** |         **259k** |                    |     **293k** |

| Perft    | Depth | Leaf positions | Time   | Leaves/s |
|----------|------:|---------------:|-------:|---------:|
| Start    |     5 |      4,865,609 | 575 ms |    8.5M  |
| Kiwipete |     4 |      4,085,603 | 407 ms |   10.0M  |

Perft counts leaf positions without playing the last move, so leaves/s is not
comparable with the search's nodes/s.

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

To play, use the release build: the minimax engine thinks for 3 seconds per
move and searches about two plies deeper there than under the debug build's
sanitizers.
```bash
cmake --preset release && cmake --build --preset release
cd build/release/bin && ./chess_game
```

`ctest --preset release` also runs the deep perft tests, which are too slow
under the debug build's sanitizers.

## License

The chess pieces in this project are made by Cburnett and unchanged.
They are licensed with the Creative Commons Attribution-Share Alike 3.0 Unported license
and can be found in the following link: https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent
and https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces.
