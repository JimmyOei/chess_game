// #include "../include/interface.h"


// Interface::Interface() {
//     window = nullptr;
//     renderer = nullptr;
//     for(int i = 0; i < NUM_OF_PIECES; i++) {
//         pieces[i] = nullptr;
//     }
//     screenWidth = 600;
//     screenHeight = 600;
//     squareEdge = 0;
//     boardStartingX = 0;
//     boardStartingY = 0;
//     running = false;
//     dragPieceByte = NO_PIECE;
//     dragPieceTextureMouseX = 0;
//     dragPieceTextureMouseY = 0;
//     game = new Game;
// }

// Interface::~Interface() {
//     SDL_DestroyWindow(window);
//     SDL_DestroyRenderer(renderer);
//     for(int i = 0; i < 12; i++) {
//         SDL_DestroyTexture(pieces[i]);
//     }
//     IMG_Quit();
//     SDL_Quit();
// };

// bool Interface::isRunning() {
//     return running;
// }

// void Interface::init(State* state, int const gamemode) {
//     this->game->state = state;

//     if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
//         std::cerr << "Failed to initiate SDL: " << SDL_GetError() << std::endl;
//         return;
//     }
//     else {
//         std::cout << "Succesfully initiated SDL" << std::endl;
//     }

//     if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
//        std::cerr << "Failed to initiate IMG: " << SDL_GetError() << std::endl;
//        return;
//     }
//     else {
//         std::cout << "Succesfully initiated IMG" << std::endl;
//     }

//     window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
//                                      screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
//     SDL_SetWindowMinimumSize(window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
//     renderer = SDL_CreateRenderer(window, -1, 0);

//     loadPieces();
//     renderBoard();
    
//     running = true;
// }

// void Interface::eventHandler(SDL_Event event) {
//     switch(event.type) {
//         case SDL_WINDOWEVENT:
//             if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
//                 resizeWindow(event.window.data2, event.window.data1);
//             }
//             break;
//         case SDL_MOUSEBUTTONDOWN:
//             if(event.button.button == SDL_BUTTON_LEFT) {
//                 if(dragPieceByte == NO_PIECE) {
//                     int mouseX, mouseY;
//                     SDL_GetMouseState(&mouseX, &mouseY);
//                     pickupDragPiece(mouseX, mouseY);
//                 }
//             }
//             break;
//         case SDL_MOUSEBUTTONUP:
//             if(dragPieceByte != NO_PIECE) {
//                 int mouseX, mouseY;
//                 SDL_GetMouseState(&mouseX, &mouseY);
//                 releaseDragPiece(mouseX, mouseY);
//             }
//             break;
//         case SDL_QUIT:
//             running = false;
//             break;
//         default:
//             break;
//     }
// }

// void Interface::render() {
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
// 	SDL_RenderClear(renderer);
    
//     renderBoard();
//     renderState();

//     if(dragPieceByte != NO_PIECE) {
//         int mouseX, mouseY;
//         SDL_GetMouseState(&mouseX, &mouseY);
//         renderDragPiece(mouseX, mouseY);
//     }

//     SDL_RenderPresent(renderer);
// }

// void Interface::loadPieces() {
//     char* PNGLocations[NUM_OF_PIECES];
//     PNGLocations[0] = (char*)DARK_PAWN_IMG;
//     PNGLocations[1] = (char*)LIGHT_PAWN_IMG;
//     PNGLocations[2] = (char*)DARK_KNIGT_IMG;
//     PNGLocations[3] = (char*)LIGHT_KNIGHT_IMG;
//     PNGLocations[4] = (char*)DARK_BISHOP_IMG;
//     PNGLocations[5] = (char*)LIGHT_BISHOP_IMG;
//     PNGLocations[6] = (char*)DARK_ROOK_IMG;
//     PNGLocations[7] = (char*)LIGHT_ROOK_IMG;
//     PNGLocations[8] = (char*)DARK_QUEEN_IMG;
//     PNGLocations[9] = (char*)LIGHT_QUEEN_IMG;
//     PNGLocations[10] = (char*)DARK_KING_IMG;
//     PNGLocations[11] = (char*)LIGHT_KING_IMG;

//     for(int i = 0; i < NUM_OF_PIECES; i++) {
//         SDL_Surface* image = IMG_Load(PNGLocations[i]);
//         if(!image) {
//             std::cerr << "Failed to load image from: " << PNGLocations[i] << std::endl;
//         }
//         pieces[i] = SDL_CreateTextureFromSurface(renderer, image);
//         if(pieces[i] == NULL) {
//             std::cerr << "Failed to create texture from image: " << SDL_GetError() << std::endl;
//         }
//         SDL_FreeSurface(image);
//     }
// }

// SDL_Texture* Interface::getTexturePieceFromByte(uint8_t byte) {
//     // Mapping the byte for a piece to its corresponding index in the 'pieces' array
//     switch(byte) {
//         case BLACK_PAWN: return pieces[0];
//         case WHITE_PAWN: return pieces[1];
//         case BLACK_KNIGHT: return pieces[2];
//         case WHITE_KNIGHT: return pieces[3];
//         case BLACK_BISHOP: return pieces[4];
//         case WHITE_BISHOP: return pieces[5];
//         case BLACK_ROOK: return pieces[6];
//         case WHITE_ROOK: return pieces[7];
//         case BLACK_QUEEN: return pieces[8];
//         case WHITE_QUEEN: return pieces[9];
//         case BLACK_KING: return pieces[10];
//         default: return pieces[11];
//     }
// }

// void Interface::renderBoard() {
//     squareEdge = (screenHeight * (screenHeight < screenWidth) 
//                          + screenWidth * (screenHeight >= screenWidth)) / BOARD_LENGTH;

//     boardStartingX = ((screenWidth - (squareEdge * BOARD_LENGTH)) / 2) * (screenWidth > squareEdge);
//     boardStartingY = ((screenHeight - (squareEdge * BOARD_LENGTH)) / 2) * (screenHeight > squareEdge);

//     for(int y = 0; y < BOARD_LENGTH; y++) {
//         for(int x = 0; x < BOARD_LENGTH; x++) {
//             SDL_Rect rect;
//             rect.w = squareEdge;
//             rect.h = squareEdge;
//             rect.x = boardStartingX + x*squareEdge;
//             rect.y = boardStartingY + y*squareEdge;
//             if((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1)) {
//                 SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
//             } // white plane
//             else {
//                 SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
//             } // black plane
//             SDL_RenderFillRect(renderer, &rect);
//         }
//     }
// }

// void Interface::renderState() {
//     for(int y = 0; y < BOARD_LENGTH; y++) {
//         for(int x = 0; x < BOARD_LENGTH; x++) {
//             uint8_t byteFromByteBoard = game->state->getByteFromByteBoard(x, y);

//             if(byteFromByteBoard == NO_PIECE) {
//                 continue;
//             }
            
//             SDL_Rect rect;
//             rect.w = squareEdge;
//             rect.h = squareEdge;
//             rect.x = boardStartingX + x*squareEdge;
//             rect.y = boardStartingY + y*squareEdge;
//             SDL_RenderCopy(renderer, getTexturePieceFromByte(byteFromByteBoard), NULL, &rect);
//         }
//     }
// }

// void Interface::resizeWindow(int const height, int const width) {
//     screenHeight = height * (height > MIN_SCREEN_HEIGHT) 
//                    + MIN_SCREEN_HEIGHT * (height <= MIN_SCREEN_HEIGHT);
//     screenWidth = width * (width > MIN_SCREEN_WIDTH) 
//                   + MIN_SCREEN_WIDTH * (width <= MIN_SCREEN_WIDTH);
// }

// void Interface::pickupDragPiece(int const mouseX, int const mouseY) {
//     int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
//     int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;

//     // if mouse coords are inside chess board, try to pick up a piece
//     if((squareXOfMouse >= 0 && squareXOfMouse < BOARD_LENGTH) && (squareYOfMouse >= 0 && squareYOfMouse < BOARD_LENGTH)) {
//         dragPieceTextureMouseX = (mouseX - boardStartingX) % squareEdge;
//         dragPieceTextureMouseY = (mouseY - boardStartingY) % squareEdge;
//         dragPieceByte = game->state->getByteFromByteBoard(squareXOfMouse, squareYOfMouse);

//         if(dragPieceByte != NO_PIECE && (game->state->getTurn() ? dragPieceByte & WHITE_PIECE : dragPieceByte & BLACK_PIECE)) {
//             game->state->setByteInByteBoard(NO_PIECE, squareXOfMouse, squareYOfMouse);
//             dragPieceInitialSquareX = squareXOfMouse;
//             dragPieceInitialSquareY = squareYOfMouse;
//         } // there is a piece and it's of the current player's side
//         else {
//             dragPieceByte = NO_PIECE;
//         }
//     }
// }

// void Interface::renderDragPiece(int const mouseX, int const mouseY) {
//     SDL_Rect rect;
//     rect.w = squareEdge;
//     rect.h = squareEdge;
//     rect.x = mouseX - dragPieceTextureMouseX;
//     rect.y = mouseY - dragPieceTextureMouseY;

//     // Conditions for preventing dragging outside of chess board
//     if(rect.x < boardStartingX - (squareEdge / 2)) {
//         rect.x = boardStartingX - (squareEdge / 2);
//     }
//     else if(rect.x > (boardStartingX + BOARD_LENGTH*squareEdge - (squareEdge / 2))) {
//         rect.x = boardStartingX + BOARD_LENGTH*squareEdge - (squareEdge / 2);
//     }
//     if(rect.y < boardStartingY - (squareEdge / 2)) {
//         rect.y = boardStartingY - (squareEdge / 2);
//     }
//     else if(rect.y > (boardStartingY + BOARD_LENGTH*squareEdge - (squareEdge / 2))) {
//         rect.y = boardStartingY + BOARD_LENGTH*squareEdge - (squareEdge / 2);
//     }

//     SDL_RenderCopy(renderer, getTexturePieceFromByte(dragPieceByte), NULL, &rect);
// }

// void Interface::releaseDragPiece(int const mouseX, int const mouseY) {
//     int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
//     int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;
//     bool enPassantMove = false;
//     bool castlingMove = false;

//     // // Checks if mouse is inside board, it's the user's turn and it's a legal move, 
//     // // else the piece goes back to initial square
//     // if(((squareXOfMouse >= 0 && squareXOfMouse < BOARD_LENGTH) && (squareYOfMouse >= 0 && squareYOfMouse < BOARD_LENGTH))
//     //     && state->isLegalMove(dragPieceByte, 
//     //                           dragPieceInitialSquareX, dragPieceInitialSquareY, 
//     //                           squareXOfMouse, squareYOfMouse,
//     //                           enPassantMove, castlingMove)) {
//     //     state->setByteInByteBoard(dragPieceByte, squareXOfMouse, squareYOfMouse);
//     //     if(enPassantMove) {
//     //         state->setByteInByteBoard(NO_PIECE, squareXOfMouse, state->getTurn() ? squareYOfMouse + 1 : squareYOfMouse - 1);
//     //     }
//     //     state->passTurn();
//     //     state->isCheck(dragPieceByte, squareXOfMouse, squareYOfMouse);
//     //     if(dragPieceByte & 0b00000001) {
//     //         if(dragPieceInitialSquareY == 1 && squareYOfMouse == 3) {
//     //             state->setEnPassantSquare(squareXOfMouse, 2);
//     //         }
//     //         else if(dragPieceInitialSquareY = BOARD_LENGTH-2 && squareYOfMouse == BOARD_LENGTH-4) {
//     //             state->setEnPassantSquare(squareXOfMouse, BOARD_LENGTH-3);
//     //         }
//     //     }
//     // }
//     // else {
//     //     state->setByteInByteBoard(dragPieceByte, dragPieceInitialSquareX, dragPieceInitialSquareY);
//     // }

//     // reset dragPiece variables
//     dragPieceByte = NO_PIECE;
//     dragPieceInitialSquareX = 0;
//     dragPieceInitialSquareY = 0;
//     dragPieceTextureMouseX = 0;
//     dragPieceTextureMouseY = 0;
// }