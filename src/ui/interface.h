/**
 * This file contains Interface, the SDL2 front end: the console menus, the
 * board window, dragging pieces for human players, and asking the engines
 * for their moves.
 */

#ifndef CHESS_GAME_SRC_UI_INTERFACE_H_
#define CHESS_GAME_SRC_UI_INTERFACE_H_

#include <SDL2/SDL.h>

#include <array>
#include <cstdint>
#include <future>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core/game.h"
#include "core/move.h"
#include "core/piece.h"
#include "core/position.h"
#include "player/player.h"

namespace chess {

class Interface {
 public:
  Interface();

  Interface(const Interface&) = delete;
  Interface& operator=(const Interface&) = delete;
  Interface(Interface&&) = delete;
  Interface& operator=(Interface&&) = delete;

  // Destroys the window, renderer and textures, and shuts down SDL.
  ~Interface();

  [[nodiscard]] bool IsRunning() const { return running_; }

  [[nodiscard]] bool IsGameOver() const { return game_over_; }

  // Shows the console menu, then opens the game window. Does nothing more if
  // the user quits from the menu.
  void Initiate();

  // Call every frame. Detects game over, and on an engine's turn starts its
  // search on a background thread (at least kMinEngineMoveDelayMs after the
  // previous move), then plays the move once the search is done.
  void HandlePlayerTurn();

  void EventHandler(const SDL_Event& event);

  void Render();

 private:
  // Indexed by ColorIndex().
  std::array<std::unique_ptr<Player>, 2> players_;
  std::unique_ptr<Game> game_;
  // The engine move being searched, if any. Declared after players_ so it is
  // destroyed first: destroying it waits for the search to finish.
  std::future<Move> pending_engine_move_;
  // SDL_GetTicks() when the last move was made.
  uint32_t last_move_ticks_ = 0;

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;

  std::unordered_map<Piece, SDL_Texture*> texture_pieces_;

  int screen_width_;
  int screen_height_;

  // Length of an edge of a square on the board.
  int square_edge_length_ = 0;

  // Top left corner of the board in the window.
  int board_starting_x_ = 0;
  int board_starting_y_ = 0;

  bool running_ = false;
  bool game_over_ = false;

  // The piece a human is dragging, or Piece::kNone.
  Piece drag_piece_ = Piece::kNone;
  // Where the mouse grabbed the piece, relative to the piece's square.
  int drag_piece_texture_mouse_x_ = 0;
  int drag_piece_texture_mouse_y_ = 0;
  Position drag_piece_pos_ = kNoPosition;
  std::vector<Move> drag_piece_legal_moves_;

  [[nodiscard]] Player& PlayerFor(Color const color) {
    return *players_[ColorIndex(color)];
  }

  // True if a human moves next, by dragging a piece.
  [[nodiscard]] bool IsHumanTurn() {
    return PlayerFor(game_->GetTurn()).IsHuman();
  }

  void MenuGamemode(Color color);

  // Asks for a FEN and sets up the game from it.
  void MenuFen();

  // Returns false if the user chose to quit.
  bool Menu();

  Piece MenuPawnPromotion();

  // Loads the piece images into texture_pieces_.
  void LoadTexturePieces();

  // Throws std::invalid_argument if there is no texture for `piece`.
  SDL_Texture* GetTexturePiece(Piece piece);

  void RenderBoard();

  void RenderState();

  void ResizeWindow(int height, int width);

  void PickupDragPiece(int mouse_x, int mouse_y);

  void RenderDragPiece(int mouse_x, int mouse_y);

  void ReleaseDragPiece(int mouse_x, int mouse_y);
};

}  // namespace chess

#endif  // CHESS_GAME_SRC_UI_INTERFACE_H_
