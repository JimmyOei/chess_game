/**
 * This file contains the Interface implementation.
 */

#include "ui/interface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "core/direction.h"
#include "core/game.h"
#include "core/move.h"
#include "core/piece.h"
#include "core/position.h"
#include "player/player_engine_minimax.h"
#include "player/player_engine_random.h"
#include "player/player_human.h"
#include "ui/perft_menu.h"
#include "util/logger.h"

namespace chess {

namespace {

constexpr int kMinScreenWidth = 500;
constexpr int kMinScreenHeight = 500;

constexpr int kStartingScreenWidth = 600;
constexpr int kStartingScreenHeight = 600;

// Piece images, relative to the working directory (see the README).
constexpr std::array<std::pair<Piece, const char *>, 12> kPieceImages = {{
    {Piece::kBlackPawn, "../pieces/darkPawn.png"},
    {Piece::kWhitePawn, "../pieces/lightPawn.png"},
    {Piece::kBlackKnight, "../pieces/darkKnight.png"},
    {Piece::kWhiteKnight, "../pieces/lightKnight.png"},
    {Piece::kBlackBishop, "../pieces/darkBishop.png"},
    {Piece::kWhiteBishop, "../pieces/lightBishop.png"},
    {Piece::kBlackRook, "../pieces/darkRook.png"},
    {Piece::kWhiteRook, "../pieces/lightRook.png"},
    {Piece::kBlackQueen, "../pieces/darkQueen.png"},
    {Piece::kWhiteQueen, "../pieces/lightQueen.png"},
    {Piece::kBlackKing, "../pieces/darkKing.png"},
    {Piece::kWhiteKing, "../pieces/lightKing.png"},
}};

struct SquareColor {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// [light square, dark square][normal, highlighted as a move target].
constexpr std::array<std::array<SquareColor, 2>, 2> kSquareColors = {{
    {{{.red = 238, .green = 238, .blue = 210},
      {.red = 228, .green = 228, .blue = 150}}},
    {{{.red = 118, .green = 150, .blue = 86},
      {.red = 108, .green = 140, .blue = 26}}},
}};

}  // namespace

Interface::Interface()
    : players_{std::make_unique<PlayerHuman>(),
               std::make_unique<PlayerEngineRandom>()},
      game_(std::make_unique<Game>()),
      screen_width_(kStartingScreenWidth),
      screen_height_(kStartingScreenHeight) {}

Interface::~Interface() {
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
  }
  if (renderer_ != nullptr) {
    SDL_DestroyRenderer(renderer_);
  }
  for (auto &[piece, texture] : texture_pieces_) {
    SDL_DestroyTexture(texture);
  }
  IMG_Quit();
  SDL_Quit();
}

void Interface::MenuGamemode(Color const color) {
  std::unique_ptr<Player> &player = players_[ColorIndex(color)];
  while (true) {
    std::cout << ">> Input the player for " << color << "\n"
              << ">> \"1\": human player | \"2\": random move engine | \"3\": "
                 "minimax engine\n\n";
    std::string player_input;
    if (!std::getline(std::cin, player_input)) {
      return;  // No input left: keep the current player.
    }
    std::cout << '\n';
    if (player_input == "1") {
      player = std::make_unique<PlayerHuman>();
      return;
    }
    if (player_input == "2") {
      player = std::make_unique<PlayerEngineRandom>();
      return;
    }
    if (player_input == "3") {
      player = std::make_unique<PlayerEngineMiniMax>();
      return;
    }
    std::cerr << ">> Invalid input, please try again\n\n";
  }
}

void Interface::MenuFen() {
  // while (true)
  // {
  //     std::string FEN;
  //     std::cout << ">> Input a FEN-notation for the game or \"default\" for
  //     the standard opening position" << std::endl
  //               << ">> Example of expected input:
  //               \"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3\""
  //               << std::endl
  //               << std::endl;
  //     getline(std::cin, FEN);
  //     std::cout << std::endl;
  //     if (FEN == "default")
  //     {
  //         FEN = kStandardOpeningFen;
  //     }
  //     if (game->state->setStateFromFEN(FEN))
  //     {
  //         return;
  //     } // correct FEN notation
  //     std::cerr << ">> The given FEN-notation is invalid: " << FEN <<
  //     std::endl
  //               << ">> Please try again " << std::endl
  //               << std::endl;
  // }
}

bool Interface::Menu() {
  while (true) {
    std::string input;
    std::cout << ">> What would you like to do? (Input a letter)" << '\n'
              << "\"S\": Start the Chess game" << '\n'
              << "\"P\": Change the Player types" << '\n'
              << "\"T\": Open the Test menu" << '\n'
              << "\"Q\": Quit program" << '\n'
              << '\n';
    if (!std::getline(std::cin, input)) {
      return false;  // No input left: quit.
    }
    std::cout << '\n';
    if (input.length() == 1) {
      switch (input[0]) {
        case 'S':
        case 's':
          std::cout << "The game will begin. Good luck!" << '\n' << '\n';
          return true;
        case 'P':
        case 'p':
          MenuGamemode(Color::kWhite);
          MenuGamemode(Color::kBlack);
          break;
        case 'C':
        case 'c':
          MenuFen();
          Render();
          break;
        case 'T':
        case 't':
          RunPerftMenu();
          break;
        case 'Q':
        case 'q':
          return false;
        default:
          std::cout << "Invalid input, please try again" << '\n' << '\n';
      }
    } else {
      std::cout << "Invalid input, please try again" << '\n' << '\n';
    }
  }
}

Piece Interface::MenuPawnPromotion() {
  // Build the piece from the dragged pawn's colour. This used to convert the
  // colour to a bool, which is true for both colours, so black pawns
  // promoted to white pieces.
  Color const color = ColorOf(drag_piece_);
  while (true) {
    std::string input;
    std::cout << "\n>> Input the first letter of the piece you would like to "
                 "promote your pawn to\n"
              << ">> \"q\": queen | \"r\": rook | \"b\": bishop | \"k\": knight"
                 "\n\n";
    if (!std::getline(std::cin, input)) {
      return MakePiece(color, PieceType::kQueen);  // No input left.
    }
    if (input.length() == 1) {
      switch (input[0]) {
        case 'k':
        case 'K':
          return MakePiece(color, PieceType::kKnight);
        case 'b':
        case 'B':
          return MakePiece(color, PieceType::kBishop);
        case 'r':
        case 'R':
          return MakePiece(color, PieceType::kRook);
        case 'q':
        case 'Q':
          return MakePiece(color, PieceType::kQueen);
        default:
          break;
      }
    }
    std::cout << ">> Invalid input, please try again\n\n";
  }
}

void Interface::Initiate() {
  if (!Menu()) {
    return;
  }

  /* Initiate SDL */
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Failed to initiate SDL: " << SDL_GetError() << '\n' << '\n';
    return;
  }

  /* Initiate SDL_Img */
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    std::cerr << "Failed to initiate IMG: " << SDL_GetError() << '\n' << '\n';
    return;
  }

  /* Set up game window */
  window_ =
      SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       screen_width_, screen_height_, SDL_WINDOW_RESIZABLE);
  SDL_SetWindowMinimumSize(window_, kMinScreenWidth, kMinScreenHeight);
  renderer_ = SDL_CreateRenderer(window_, -1, 0);

  LoadTexturePieces();

  running_ = true;
}

void Interface::EventHandler(const SDL_Event &event) {
  switch (event.type) {
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        ResizeWindow(event.window.data2, event.window.data1);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if (!IsHumanTurn()) {
        break;
      }

      if (event.button.button == SDL_BUTTON_LEFT && !game_over_) {
        if (drag_piece_ == Piece::kNone) {
          int mouse_x = 0;
          int mouse_y = 0;
          SDL_GetMouseState(&mouse_x, &mouse_y);
          PickupDragPiece(mouse_x, mouse_y);
        }
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if (!IsHumanTurn()) {
        break;
      }

      if (drag_piece_ != Piece::kNone) {
        int mouse_x = 0;
        int mouse_y = 0;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        ReleaseDragPiece(mouse_x, mouse_y);
      }
      break;
    case SDL_QUIT:
      running_ = false;
      break;
    default:
      break;
  }
}

void Interface::Render() {
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDL_RenderClear(renderer_);

  RenderBoard();
  RenderState();

  if (drag_piece_ != Piece::kNone) {
    int mouse_x = 0;
    int mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    RenderDragPiece(mouse_x, mouse_y);
  }

  SDL_RenderPresent(renderer_);
}

void Interface::LoadTexturePieces() {
  for (const auto &[piece, path] : kPieceImages) {
    SDL_Surface *image = IMG_Load(path);
    if (image == nullptr) {
      std::cerr << "Failed to load image from: " << path << '\n';
      throw std::invalid_argument("Failed to load image.");
    }
    texture_pieces_[piece] = SDL_CreateTextureFromSurface(renderer_, image);
    SDL_FreeSurface(image);
    if (texture_pieces_[piece] == nullptr) {
      std::cerr << "Failed to create texture from image: " << SDL_GetError()
                << '\n';
      throw std::invalid_argument("Failed to create texture from image.");
    }
  }
}

SDL_Texture *Interface::GetTexturePiece(Piece piece) {
  if (texture_pieces_.find(piece) == texture_pieces_.end()) {
    throw std::invalid_argument("Failed to find texture for piece.");
  }

  return texture_pieces_[piece];
}

void Interface::HandlePlayerTurn() {
  if (game_->IsGameOver()) {
    switch (game_->GetResult()) {
      case Result::kWhiteWin:
        CHESS_LOG(LogLevel::kInfo) << "Game over. White wins!";
        break;
      case Result::kBlackWin:
        CHESS_LOG(LogLevel::kInfo) << "Game over. Black wins!";
        break;
      case Result::kDraw:
        CHESS_LOG(LogLevel::kInfo) << "Game over. Draw!";
        break;
      default:
        CHESS_LOG(LogLevel::kError)
            << "Game over. Game still says it's ongoing. "
               "This should not happen.";
        throw std::invalid_argument(
            "Game over. Game still says it's ongoing. This should not happen.");
    }
    game_over_ = true;
    return;
  }
  Color const color = game_->GetTurn();
  Player &player = PlayerFor(color);
  if (player.IsHuman()) {
    return;  // Humans move through the mouse events.
  }
  Move const move = player.GetMove(*game_);
  game_->MakeMove(move);
  if (move.promotion_piece != Piece::kNone) {
    CHESS_LOG(LogLevel::kInfo)
        << "Player Engine " << color << " made move " << move
        << " with promotion to " << move.promotion_piece;
  } else {
    CHESS_LOG(LogLevel::kInfo)
        << "Player Engine " << color << " made move " << move;
  }
}

void Interface::RenderBoard() {
  // Length of an edge of a square on the board
  square_edge_length_ = (screen_height_ * (screen_height_ < screen_width_) +
                         screen_width_ * (screen_height_ >= screen_width_)) /
                        kBoardLength;

  // Top left square position of the board on the window
  board_starting_x_ =
      ((screen_width_ - (square_edge_length_ * kBoardLength)) / 2) *
      (screen_width_ > square_edge_length_);
  board_starting_y_ =
      ((screen_height_ - (square_edge_length_ * kBoardLength)) / 2) *
      (screen_height_ > square_edge_length_);

  int pos = 0;
  for (int y = kBoardLength - 1; y >= 0; y--) {
    for (int x = 0; x < kBoardLength; x++) {
      SDL_Rect rect;
      rect.w = square_edge_length_;
      rect.h = square_edge_length_;
      rect.x = board_starting_x_ + x * square_edge_length_;
      rect.y = board_starting_y_ + y * square_edge_length_;
      // Squares the dragged piece can move to are highlighted.
      bool const is_move_target = std::ranges::any_of(
          drag_piece_legal_moves_,
          [pos](const Move &move) { return move.to == Position(pos); });
      bool const is_light_square = (x + y) % 2 == 0;
      const SquareColor &fill =
          kSquareColors[is_light_square ? 0 : 1][is_move_target ? 1 : 0];
      SDL_SetRenderDrawColor(renderer_, fill.red, fill.green, fill.blue, 255);
      pos++;
      SDL_RenderFillRect(renderer_, &rect);
    }
  }
}

void Interface::RenderState() {
  int pos = 0;
  for (int y = kBoardLength - 1; y >= 0; y--) {
    for (int x = 0; x < kBoardLength; x++) {
      if (Position(pos) == drag_piece_pos_) {
        pos++;
        continue;
      }
      Piece const piece = game_->GetPieceAtPos(Position(pos++));

      if (piece == Piece::kNone) {
        continue;
      }

      SDL_Rect rect;
      rect.w = square_edge_length_;
      rect.h = square_edge_length_;
      rect.x = board_starting_x_ + x * square_edge_length_;
      rect.y = board_starting_y_ + y * square_edge_length_;
      SDL_RenderCopy(renderer_, GetTexturePiece(piece), nullptr, &rect);
    }
  }
}

void Interface::ResizeWindow(int const height, int const width) {
  screen_height_ = height * (height > kMinScreenHeight) +
                   kMinScreenHeight * (height <= kMinScreenHeight);
  screen_width_ = width * (width > kMinScreenWidth) +
                  kMinScreenWidth * (width <= kMinScreenWidth);
}

void Interface::PickupDragPiece(int const mouse_x, int const mouse_y) {
  CHESS_LOG(LogLevel::kDebug)
      << "Picking up piece at mouse coordinates (x, y): (" << mouse_x << ", "
      << mouse_y << ")";
  int const square_x_of_mouse =
      (mouse_x - board_starting_x_) / square_edge_length_;
  int const square_y_of_mouse =
      (mouse_y - board_starting_y_) / square_edge_length_;

  // if mouse coords are inside chess board, try to pick up a piece
  if ((square_x_of_mouse >= 0 && square_x_of_mouse < kBoardLength) &&
      (square_y_of_mouse >= 0 && square_y_of_mouse < kBoardLength)) {
    drag_piece_texture_mouse_x_ =
        (mouse_x - board_starting_x_) % square_edge_length_;
    drag_piece_texture_mouse_y_ =
        (mouse_y - board_starting_y_) % square_edge_length_;
    drag_piece_pos_ = Position::FromColumnRow(square_x_of_mouse + 1,
                                              kBoardLength - square_y_of_mouse);
    if (!drag_piece_pos_.IsValid()) {
      drag_piece_ = Piece::kNone;
      drag_piece_pos_ = kNoPosition;
      return;
    }
    drag_piece_ = game_->GetPieceAtPos(drag_piece_pos_);

    if (drag_piece_ == Piece::kNone ||
        game_->GetTurn() != ColorOf(drag_piece_)) {
      drag_piece_ = Piece::kNone;
      drag_piece_pos_ = kNoPosition;
      return;
    }  // there is no piece at this square or it's of the opponent's color
    drag_piece_legal_moves_ = game_->GetLegalMovesForPos(drag_piece_pos_);
  }
  CHESS_LOG(LogLevel::kInfo)
      << "Picked up piece " << drag_piece_ << " at " << drag_piece_pos_;
}

void Interface::RenderDragPiece(int const mouse_x, int const mouse_y) {
  SDL_Rect rect;
  rect.w = square_edge_length_;
  rect.h = square_edge_length_;
  rect.x = mouse_x - drag_piece_texture_mouse_x_;
  rect.y = mouse_y - drag_piece_texture_mouse_y_;

  // Conditions for preventing dragging outside of chess board
  if (rect.x < board_starting_x_ - (square_edge_length_ / 2)) {
    rect.x = board_starting_x_ - (square_edge_length_ / 2);
  } else if (rect.x > (board_starting_x_ + kBoardLength * square_edge_length_ -
                       (square_edge_length_ / 2))) {
    rect.x = board_starting_x_ + kBoardLength * square_edge_length_ -
             (square_edge_length_ / 2);
  }
  if (rect.y < board_starting_y_ - (square_edge_length_ / 2)) {
    rect.y = board_starting_y_ - (square_edge_length_ / 2);
  } else if (rect.y > (board_starting_y_ + kBoardLength * square_edge_length_ -
                       (square_edge_length_ / 2))) {
    rect.y = board_starting_y_ + kBoardLength * square_edge_length_ -
             (square_edge_length_ / 2);
  }

  SDL_RenderCopy(renderer_, GetTexturePiece(drag_piece_), nullptr, &rect);
}

void Interface::ReleaseDragPiece(int const mouse_x, int const mouse_y) {
  CHESS_LOG(LogLevel::kDebug)
      << "Releasing piece " << drag_piece_ << " at mouse coordinates (x, y): ("
      << mouse_x << ", " << mouse_y << ")";
  int const square_x_of_mouse =
      (mouse_x - board_starting_x_) / square_edge_length_;
  int const square_y_of_mouse =
      (mouse_y - board_starting_y_) / square_edge_length_;
  Position const new_drag_piece_pos = Position::FromColumnRow(
      square_x_of_mouse + 1, kBoardLength - square_y_of_mouse);

  for (auto &legal_move : drag_piece_legal_moves_) {
    if (legal_move.to == new_drag_piece_pos) {
      if (legal_move.promotion_piece != Piece::kNone) {
        // Promotion: ask which piece, since the four moves only differ there.
        const Move promotion_move(legal_move.from, legal_move.to,
                                  legal_move.piece, MenuPawnPromotion());
        game_->MakeMove(promotion_move);
        CHESS_LOG(LogLevel::kInfo)
            << "Player Human " << ColorOf(legal_move.piece) << " made move "
            << promotion_move << " with promotion to "
            << promotion_move.promotion_piece;
        break;
      }

      game_->MakeMove(legal_move);
      CHESS_LOG(LogLevel::kInfo) << "Player Human " << ColorOf(legal_move.piece)
                                 << " made move " << legal_move;
      break;
    }
  }

  // reset drag_piece_ variables
  drag_piece_ = Piece::kNone;
  drag_piece_pos_ = kNoPosition;
  drag_piece_legal_moves_.clear();
}

}  // namespace chess
