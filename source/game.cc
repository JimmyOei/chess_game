// #include "../include/game.h"

// void Game::clearAttackFieldBlack() {
//     for(int i = 0; i < BOARD_LENGTH; i++) {
//         for(int j = 0; j < BOARD_LENGTH; j++) {
//             attackFieldBlack[i][j] = false;
//         }
//     }
// }

// void Game::clearAttackFieldWhite() {
//     for(int i = 0; i < BOARD_LENGTH; i++) {
//         for(int j = 0; j < BOARD_LENGTH; j++) {
//             attackFieldWhite[i][j] = false;
//         }
//     }
// }

// Game::Game() {
//     state = new State();
// }

// Game::~Game() {
//     delete state;
// }

// // std::vector<std::pair<int, int>> Game::getAttackFieldOfPiece(uint8_t const pieceByte,
// //                                                               int const x, int const y) {
// //     if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
// //         std::cerr << "[getAttackFieldOfPiece] out-of-bounds position." << std::endl;
// //         exit(1);
// //     }

// //     std::vector<std::pair<int, int>> attackField;
// //     switch(pieceByte) {
// //         case WHITE_PAWN:
// //             if(y > 0) {
// //                 if(x > 0 && state->getByteFromByteBoard(x-1,y-1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x-1, y-1));
// //                 }
// //                 if(x < BOARD_LENGTH-1 && state->getByteFromByteBoard(x+1,y-1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x+1, y-1));
// //                 }
// //             }
// //             break;
// //         case BLACK_PAWN:
// //             if(y < BOARD_LENGTH-1 && (x > 0 && state->getByteFromByteBoard(x-1,y+1) != NO_PIECE)) {
// //                 attackField.push_back(std::pair(x-1, y+1));
// //             }
// //             if(y < BOARD_LENGTH-1 && (x < BOARD_LENGTH-1 && state->getByteFromByteBoard(x+1,y+1) != NO_PIECE)) {
// //                 attackField.push_back(std::pair(x+1, y+1));
// //             }
// //             break;
// //         case WHITE_KNIGHT:
// //         case BLACK_KNIGHT:
// //             if(y > 1) {
// //                 if(x > 0 && state->getByteFromByteBoard(x-1, y-2) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x-1, y-2));
// //                 }
// //                 if(x < BOARD_LENGTH-1 && state->getByteFromByteBoard(x+1, y-2) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x+1, y-2));
// //                 }
// //             }
// //             if(y < BOARD_LENGTH-2) {
// //                 if(x > 0 && state->getByteFromByteBoard(x-1, y+2) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x-1, y+2));
// //                 }
// //                 if(x < BOARD_LENGTH-1 && state->getByteFromByteBoard(x+1, y+2) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x+1, y+2));
// //                 }
// //             }
// //             if(x > 1) {
// //                 if(y > 0 && state->getByteFromByteBoard(x-2, y-1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x-2, y-1));
// //                 }
// //                 if(y < BOARD_LENGTH-1 && state->getByteFromByteBoard(x-2, y+1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x-2, y+1));
// //                 }
// //             }
// //             if(x < BOARD_LENGTH-2) {
// //                 if(y > 0 && state->getByteFromByteBoard(x+2, y-1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x+2, y-1));
// //                 }
// //                 if(y < BOARD_LENGTH-1 && state->getByteFromByteBoard(x+2, y+1) != NO_PIECE) {
// //                     attackField.push_back(std::pair(x+2, y+1));
// //                 }
// //             }
// //             break;
// //         case WHITE_QUEEN: // queen is same as bishop + rook
// //         case BLACK_QUEEN:
// //         case WHITE_BISHOP:
// //         case BLACK_BISHOP: {
// //             int tmpX = x;
// //             int tmpY = y;
// //             while(tmpX > 0 && tmpY > 0) {
// //                 tmpX--;
// //                 tmpY--;
// //                 attackField.push_back(std::pair(tmpX, tmpY));
// //                 if(state->getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             tmpX = x;
// //             tmpY = y;
// //             while(tmpX > 0 && tmpY < BOARD_LENGTH-1) {
// //                 tmpX--;
// //                 tmpY++;
// //                 attackField.push_back(std::pair(tmpX, tmpY));
// //                 if(state->getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             tmpX = x;
// //             tmpY = y;
// //             while(tmpX < BOARD_LENGTH-1 && tmpY > 0) {
// //                 tmpX++;
// //                 tmpY--;
// //                 attackField.push_back(std::pair(tmpX, tmpY));
// //                 if(state->getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             tmpX = x;
// //             tmpY = y;
// //             while(tmpX < BOARD_LENGTH-1 && tmpY < BOARD_LENGTH-1) {
// //                 tmpX++;
// //                 tmpY++;
// //                 attackField.push_back(std::pair(tmpX, tmpY));
// //                 if(state->getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }

// //             if(pieceByte != WHITE_QUEEN && pieceByte != BLACK_QUEEN) {
// //                 break;
// //             } // no break if the piece is a queen (queen can also do rook moves)
// //         }
// //         case WHITE_ROOK:
// //         case BLACK_ROOK:
// //             int tmpX = x;
// //             while(tmpX > 0) {
// //                 tmpX--;
// //                 attackField.push_back(std::pair(tmpX, y));
// //                 if(state->getByteFromByteBoard(tmpX, y) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             tmpX = x;
// //             while(tmpX < BOARD_LENGTH-1) {
// //                 tmpX++;
// //                 attackField.push_back(std::pair(tmpX, y));
// //                 if(state->getByteFromByteBoard(tmpX, y) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             int tmpY = y;
// //             while(tmpY > 0) {
// //                 tmpY--;
// //                 attackField.push_back(std::pair(x, tmpY));
// //                 if(state->getByteFromByteBoard(x, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             tmpY = y;
// //             while(tmpY < BOARD_LENGTH-1) {
// //                 tmpY++;
// //                 attackField.push_back(std::pair(x, tmpY));
// //                 if(state->getByteFromByteBoard(x, tmpY) != NO_PIECE) {
// //                     break;
// //                 }
// //             }
// //             break;
// //     }

// //     return attackField;                                                   
// // }

// bool Game::isKingAttacked(State* state, uint8_t colorOfKing) {
//     return false;
// }


// std::vector<std::pair<int, int>> Game::getLegalMovesOfPiece(uint8_t const pieceByte,
//                                                              int const x, int const y) {
//     if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
//         std::cerr << "[getLegalMovesOfPiece] out-of-bounds position." << std::endl;
//         exit(1);
//     }

//     /**
//      * A legal move of a piece is:
//      * 1. a move according to the rules of the piece
//      * 2. a move that is within the bounds of the board
//      * 3. a move that is either:
//      *    3.1 to an empty square
//      *    3.2 to a piece of the opponent's color
//      * 4. [RULE 4 is exempted for knights] a move that does not "hop over" pieces
//      * 5. a move that does not leaves your own king attacked afterwards
//     */

//     // TODO: for en passant we can just check if difference prevY and newY is 2

//     std::vector<std::pair<int, int>> legalMoves;
//     State* stateCopy = new State(this->state);
//     int tmpX = x;
//     int tmpY = y;
//     uint8_t colorOfPieceByte = isWhitePiece(pieceByte) ? WHITE_PIECE : BLACK_PIECE;
//     switch(pieceByte) {
//         case WHITE_PAWN:
//             if(tmpY == BOARD_LENGTH-1 
//                && stateCopy->withinBoardLimits(tmpX, ----tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // two step upwards move (only possible if at y=BOARD_LENGTH-1)
//             if(stateCopy->withinBoardLimits(tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // one step upwards move
//             if(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp != NO_PIECE && !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left diagonal take
//             if(stateCopy->withinBoardLimits(++++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp != NO_PIECE && !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right diagonal take
//             break;
//         case BLACK_PAWN:
//             if(tmpY == 1 
//                && stateCopy->withinBoardLimits(tmpX, ++++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // two step upwards move (only possible if at y=1)
//             if(stateCopy->withinBoardLimits(tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // one step upwards move
//             if(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp != NO_PIECE && !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left diagonal take
//             if(stateCopy->withinBoardLimits(++++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp != NO_PIECE && !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right diagonal take
//             break;
//         case WHITE_KNIGHT:
//         case BLACK_KNIGHT:
//             if(stateCopy->withinBoardLimits(--tmpX, ----tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // up-up-left move/take
//             if(stateCopy->withinBoardLimits(--tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left-left-up move/take
//             if(stateCopy->withinBoardLimits(tmpX, ++++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left-left-down move/take
//             if(stateCopy->withinBoardLimits(++tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // down-down-left move/take
//             if(stateCopy->withinBoardLimits(++++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // down-down-right move/take
//             if(stateCopy->withinBoardLimits(++tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right-right-down move/take
//             if(stateCopy->withinBoardLimits(tmpX, ----tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right-right-up move/take
//             break;
//         case WHITE_QUEEN: // queen is same as bishop + rook
//         case BLACK_QUEEN:
//         case WHITE_BISHOP:
//         case BLACK_BISHOP: {
//             while(stateCopy->withinBoardLimits(--tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // left-upwards diagonal
//             tmpX = x;
//             tmpY = y;
//             while(stateCopy->withinBoardLimits(--tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // left-downwards diagonal
//             tmpX = x;
//             tmpY = y;
//             while(stateCopy->withinBoardLimits(++tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // right-downwards diagonal
//             tmpX = x;
//             tmpY = y;
//             while(stateCopy->withinBoardLimits(++tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // right-upwards diagonal

//             if(pieceByte != WHITE_QUEEN && pieceByte != BLACK_QUEEN) {
//                 break;
//             } // no break if the piece is a queen (queen can also do rook moves)
            
//             // reset tmpX, tmpY for queen to check rook moves
//             tmpX = x;
//             tmpY = y;
//         }
//         case WHITE_ROOK:
//         case BLACK_ROOK:
//             while(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // horizontal left
//             tmpX = x;
//             while(stateCopy->withinBoardLimits(++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // horizontal right
//             while(stateCopy->withinBoardLimits(tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // vertical upwards
//             tmpY = y;
//             while(stateCopy->withinBoardLimits(tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                 if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                     legalMoves.push_back(std::pair(tmpX, tmpY));
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//                 if(tmp != NO_PIECE) {
//                     break;
//                 } // cannot hop over piece so we break
//             } // vertical downwards
//             break;
//         case WHITE_KING:
//         case BLACK_KING:
//             if(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left move/take
//             if(stateCopy->withinBoardLimits(tmpX, ++tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left-down move/take
//             if(stateCopy->withinBoardLimits(++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // down move/take
//             if(stateCopy->withinBoardLimits(++tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right-down move/take
//             if(stateCopy->withinBoardLimits(tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right move/take
//             if(stateCopy->withinBoardLimits(tmpX, --tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // right-up move/take
//             if(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // up move/take
//             if(stateCopy->withinBoardLimits(--tmpX, tmpY)) {
//                 uint8_t tmp = stateCopy->getByteFromByteBoard(tmpX,tmpY);
//                 if(tmp == NO_PIECE || !(tmp & colorOfPieceByte)) {
//                     stateCopy->setByteInByteBoard(tmpX, tmpY, pieceByte);
//                     if(!isKingAttacked(stateCopy, colorOfPieceByte)) {
//                         legalMoves.push_back(std::pair(tmpX, tmpY));
//                     }
//                 }
//                 stateCopy->setByteInByteBoard(tmpX, tmpY, tmp);
//             } // left-up move/take
//     }

//     return legalMoves;  
// }