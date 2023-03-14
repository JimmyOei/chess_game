#include "../include/state.h"

#include <iostream>

void State::clearState() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        byteBoard[i] = NO_PIECE;
    }
    turn = WHITE;
    enPassant = false;
    enPassantPos = 0;
    whiteCastlingQueenside = false;
    whiteCastlingKingside = false;
    blackCastlingQueenside = false;
    blackCastlingKingside = false;
    whiteKingPos = -1;
    blackKingPos = -1;
}

State::State() {
    clearState();
}

State::State(uint8_t byteBoard[BOARD_SIZE], bool const turn,
             bool const enPassant, int const enPassantPos, bool const whiteCastlingQueenside,
             bool const whiteCastlingKingside, bool const blackCastlingQueenside,
             bool const blackCastlingKingside) {
    this->turn = turn;
    this->enPassant = enPassant;
    this->whiteCastlingQueenside = whiteCastlingQueenside;
    this->whiteCastlingKingside = whiteCastlingKingside;
    this->blackCastlingQueenside = blackCastlingQueenside;
    this->blackCastlingKingside = blackCastlingQueenside;
    for(int i = 0; i < BOARD_SIZE; i++) {
        this->byteBoard[i] = byteBoard[i];
    }
}

State* State::copyState() {
    return new State(byteBoard, turn, enPassant, enPassantPos, 
                     whiteCastlingQueenside, whiteCastlingKingside,
                     blackCastlingQueenside, blackCastlingKingside);
}

bool State::withinBoardLimits(int const pos) {
    return pos >= 0 && pos < BOARD_SIZE;
}

void State::passTurn() {
    turn = !turn;
    enPassant = false;
}

bool State::getTurn() {
    return turn;
}

int State::getPosOfKing(bool const color) {
    return color ? whiteKingPos : blackKingPos;
}

uint8_t State::getByteFromByteBoard(int const pos) {
    return byteBoard[pos];
}

void State::setByteInByteBoard(uint8_t const byte, int const pos) {
    byteBoard[pos] = byte;
}

void State::setEnPassantPos(int const pos) {
    enPassantPos = pos;
    enPassant = true;
}

bool State::setStateFromFEN(std::string FEN) {
    clearState();
    int const lengthFEN = FEN.length();

    // setting the board
    int pos = BOARD_SIZE-1;
    int i = 0;

    // TODO: FIX THE MIRRORING
    while(i < lengthFEN && pos >= 0) {
        switch(FEN[i]) {
            case '/':
                pos += 1; // because we may just have moved to new row
                pos = pos - (pos % BOARD_LENGTH);
                if(pos >= BOARD_SIZE) {
                    return false;
                }
                break;
            case 'p': byteBoard[pos] = BLACK_PAWN; break;
            case 'P': byteBoard[pos] = WHITE_PAWN; break;
            case 'n': byteBoard[pos] = BLACK_KNIGHT; break;
            case 'N': byteBoard[pos] = WHITE_KNIGHT; break;
            case 'b': byteBoard[pos] = BLACK_BISHOP; break;
            case 'B': byteBoard[pos] = WHITE_BISHOP; break;
            case 'r': byteBoard[pos] = BLACK_ROOK; break;
            case 'R': byteBoard[pos] = WHITE_ROOK; break;
            case 'q': byteBoard[pos] = BLACK_QUEEN; break;
            case 'Q': byteBoard[pos] = WHITE_QUEEN; break;
            case 'k': byteBoard[pos] = BLACK_KING; blackKingPos = pos; break;
            case 'K': byteBoard[pos] = WHITE_KING; whiteKingPos = pos;  break;
            default:
                int const nrEmptySquares = FEN[i] - '0';
                if(nrEmptySquares > BOARD_LENGTH) {
                    return false;;
                }
                pos = pos - nrEmptySquares + 1;
                if(pos >= BOARD_SIZE) {
                    return false;
                }
                break;
        }
        std::cout << i << " " << FEN[i] << " "  << pos << " " << byteBoard[pos] << std::endl;
        pos--;
        i++;
    }
    if(whiteKingPos == -1 || blackKingPos == -1) {
        return false;
    } // no king(s) on the board

    i++;
    // who's turn is it
    if(i < lengthFEN && FEN[i-1] == ' ') {
        switch(FEN[i]) {
            case 'w': turn = WHITE; break;
            case 'b': turn = BLACK; break;
            default: return false;
        }
    }
    else {
        return false;
    }

    i += 2;
    if(i < lengthFEN && FEN[i-1] == ' ') {
        // castling data
        if(FEN[i] == 'K' || FEN[i] == 'Q' || FEN[i] == 'k' || FEN[i] == 'q') {
            if(FEN[i++] == 'K') {
                whiteCastlingKingside = true;
            }
            if(i < lengthFEN && FEN[i++] == 'Q') {
                whiteCastlingQueenside = true;
            }
            if(i < lengthFEN && FEN[i++] == 'k') {
                blackCastlingKingside = true;
            }
            if(i < lengthFEN && FEN[i++] == 'q') {
                blackCastlingQueenside = true;
            }
        }
        else if(FEN[i++] != '-') {
            return false;
        }
        if(FEN[i++] != ' ') {
            return false;
        }

        // En Passant data
        if(FEN[i++] == '-') {
            enPassant = false;
        }
        else {
            if(i < lengthFEN-1)
            enPassantPos = FEN[i++] - 'a';
            if(enPassantPos >= BOARD_LENGTH && i >= lengthFEN) {
                return false;
            }
            enPassantPos += (FEN[i++] - '0') * BOARD_LENGTH;
            if(enPassantPos >= BOARD_LENGTH) {
                return false;
            }
        }
    }
    else {
        return false;
    }

    return true;
}

// NOT NEEDED RIGHT NOW, SO LEAVING AS IS FOR NOW
// std::string State::getFEN() {
//     std::string FEN = "";
//     int countEmptySquares = 0;
    
//     for(int i = 0; i < BOARD_SIZE; i++) {
//             switch(byteBoard[i]) {
//                 case NO_PIECE:
//                     countEmptySquares = 1;
//                     i++; 
//                     while(byteBoard[i][j] == NO_PIECE && j < BOARD_LENGTH-1) {
//                         countEmptySquares++;
//                         j++;
//                     }
//                     j--;
//                     FEN += char('0' + countEmptySquares);
//                     break;
//                 case BLACK_PAWN: FEN += 'p'; break;
//                 case WHITE_PAWN: FEN += 'P'; break;
//                 case BLACK_KNIGHT: FEN += 'n'; break;
//                 case WHITE_KNIGHT: FEN += 'N'; break;
//                 case BLACK_BISHOP: FEN += 'b'; break;
//                 case WHITE_BISHOP: FEN += 'B'; break;
//                 case BLACK_ROOK: FEN += 'r'; break;
//                 case WHITE_ROOK: FEN += 'R'; break;
//                 case BLACK_QUEEN: FEN += 'q'; break;
//                 case WHITE_QUEEN: FEN += 'Q'; break;
//                 case BLACK_KING: FEN += 'k'; break;
//                 case WHITE_KING: FEN += 'K'; break;
//             }
//             countEmptySquares = 0;
//         }
//         FEN += '/';
//     }

//     FEN += ' ';
//     if(turn) {
//         FEN += 'w';
//     }
//     else {
//         FEN += 'b';
//     }

//     FEN += ' ';
//     if(whiteCastlingKingside || whiteCastlingQueenside || blackCastlingKingside || blackCastlingQueenside) {
//         if(whiteCastlingKingside) {
//             FEN += 'K';
//         }
//         if(whiteCastlingQueenside) {
//             FEN += 'Q';
//         }
//         if(blackCastlingKingside) {
//             FEN += 'k';
//         }
//         if(blackCastlingQueenside) {
//             FEN += 'q';
//         }
//     }
//     else {
//         FEN += '-';
//     }

//     FEN += ' ';
//     if(enPassant) {
//         FEN += char('a' + xEnPassantSquare);
//         FEN += char('0' + yEnPassantSquare);
//     }
//     else {
//         FEN += '-';
//     }

//     return FEN;
// }

void State::debugPrintState() {
    std::cout << "--------===== DEBUG STATE =====--------" << std::endl
              << "Turn: " << turn << std::endl
              << "enPassant: " << enPassant << std::endl
              << "enPassantPos: " << enPassantPos << std::endl
              << "whiteCastlingQueenside: " << whiteCastlingQueenside << std::endl
              << "whiteCastlingKingside: " << whiteCastlingKingside << std::endl
              << "blackCastlingQueenside: " << blackCastlingQueenside << std::endl
              << "blackCastlingKingside: " << blackCastlingKingside << std::endl
              << "whiteKingPos: " << whiteKingPos << std::endl
              << "blackKingPos: " << blackKingPos << std::endl
              << std::endl;

    for(int i = BOARD_LENGTH-1; i >= 0; i--) {
        for(int pos = i*BOARD_LENGTH; pos < (i+1)*BOARD_LENGTH; pos++) {
            std::cout << " ";
            switch(byteBoard[pos]) {
                case WHITE_PAWN: std::cout << "P"; break;
                case BLACK_PAWN: std::cout << "p"; break;
                case WHITE_KNIGHT: std::cout << "N"; break;
                case BLACK_KNIGHT: std::cout << "n"; break;
                case WHITE_BISHOP: std::cout << "B"; break;
                case BLACK_BISHOP: std::cout << "b"; break;
                case WHITE_ROOK: std::cout << "R"; break;
                case BLACK_ROOK: std::cout << "r"; break;
                case WHITE_QUEEN: std::cout << "Q"; break;
                case BLACK_QUEEN: std::cout << "q"; break;
                case WHITE_KING: std::cout << "K"; break;
                case BLACK_KING: std::cout << "k"; break;
                default: std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}   