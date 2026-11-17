/**
 * This file contains the console menu that counts the possible positions
 * from the starting position up to a chosen depth, and compares them with
 * the published perft counts.
 */

#ifndef CHESS_GAME_SRC_UI_PERFT_MENU_H_
#define CHESS_GAME_SRC_UI_PERFT_MENU_H_

namespace chess {

// Runs the menu on stdin/stdout until the user closes it or input ends.
void RunPerftMenu();

}  // namespace chess

#endif  // CHESS_GAME_SRC_UI_PERFT_MENU_H_
