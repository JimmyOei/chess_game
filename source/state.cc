#include "../include/state.h"

#include <iostream>

void State::clearByteBoard() {
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            byteBoard[i][j] = NO_PIECE;
        }
    }
}

State::State() {
    turn = false;
    enPassant = false;
    xEnPassantSquare = 0;
    yEnPassantSquare = 0;
    whiteCastlingQueenside = true;
    whiteCastlingKingside = true;
    blackCastlingQueenside = true;
    blackCastlingKingside = true;
    clearByteBoard();
}

State::State(State* state) {
    this->setByteBoardFromFEN(state->getFEN());
}

bool State::withinBoardLimits(int const x, int const y) {
    return x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH;
}

void State::passTurn() {
    turn = !turn;
    enPassant = false;
}

bool State::getTurn() {
    return turn;
}

uint8_t State::getByteFromByteBoard(int const x, int const y) {
    // if(!withinBoardLimits(x, y)) {
    //     std::cerr << "[getByteFromByteBoard] out-of-bounds position." << std::endl;
    //     exit(1);
    // }
    return byteBoard[y][x];
}

void State::setByteInByteBoard(uint8_t const byte, int const x, int const y) {
    // if(!withinBoardLimits(x, y)) {
    //     std::cerr << "[setByteInByteBoard] out-of-bounds position." << std::endl;
    //     exit(1);
    // }
    byteBoard[y][x] = byte;
}

void State::setEnPassantSquare(int const x, int const y) {
    // if(!withinBoardLimits(x, y)) {
    //     std::cerr << "[setEnPassantSquare] out-of-bounds position." << std::endl;
    //     exit(1);
    // }
    xEnPassantSquare = x;
    yEnPassantSquare = y;
    enPassant = true;
}

bool State::setByteBoardFromFEN(std::string FEN) {
    clearByteBoard();
    int const lengthFEN = FEN.length();
    int byteBoardX = 0;
    int byteBoardY = 0;

    int i = 0;
    while(i < lengthFEN && (byteBoardY < 7 || byteBoardX < 8)) {
        switch(FEN[i]) {
            case '/':
                byteBoardY++;
                if(byteBoardY > 7) {
                    return false;
                } 
                byteBoardX = -1; // reset to -1, increment at the end to 0
                break;
            case 'p': byteBoard[byteBoardY][byteBoardX] = BLACK_PAWN; break;
            case 'P': byteBoard[byteBoardY][byteBoardX] = WHITE_PAWN; break;
            case 'n': byteBoard[byteBoardY][byteBoardX] = BLACK_KNIGHT; break;
            case 'N': byteBoard[byteBoardY][byteBoardX] = WHITE_KNIGHT; break;
            case 'b': byteBoard[byteBoardY][byteBoardX] = BLACK_BISHOP; break;
            case 'B': byteBoard[byteBoardY][byteBoardX] = WHITE_BISHOP; break;
            case 'r': byteBoard[byteBoardY][byteBoardX] = BLACK_ROOK; break;
            case 'R': byteBoard[byteBoardY][byteBoardX] = WHITE_ROOK; break;
            case 'q': byteBoard[byteBoardY][byteBoardX] = BLACK_QUEEN; break;
            case 'Q': byteBoard[byteBoardY][byteBoardX] = WHITE_QUEEN; break;
            case 'k': byteBoard[byteBoardY][byteBoardX] = BLACK_KING; break;
            case 'K': byteBoard[byteBoardY][byteBoardX] = WHITE_KING; break;
            default:
                int const nrEmptySquares = FEN[i] - '0';
                if(byteBoardX + nrEmptySquares > 8) {
                    return false;
                }
                for(int i = 0; i < nrEmptySquares; i++) {
                    byteBoard[byteBoardY][byteBoardX] = NO_PIECE;
                    byteBoardX++;
                }
                byteBoardX--;
                break;
        }
        if(byteBoardX > 7) {
            return false;
        }
        byteBoardX++;
        i++;
    }

    i++;
    if(i < lengthFEN && FEN[i-1] == ' ') {
        if(FEN[i] == 'w') {
            turn = true;
        }
        else if(FEN[i] == 'b') {
            turn = false;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    i += 2;
    if(i < lengthFEN && FEN[i-1] == ' ') {
        if(FEN[i] == '-') {
            i++;
        }
        else if(FEN[i] == 'K' || FEN[i] == 'Q' || FEN[i] == 'k' || FEN[i] == 'q') {
            if(FEN[i] == 'K') {
                whiteCastlingKingside = true;
                i++;
            }
            if(i < lengthFEN && FEN[i] == 'Q') {
                whiteCastlingQueenside = true;
                i++;
            }
            if(i < lengthFEN && FEN[i] == 'k') {
                blackCastlingKingside = true;
                i++;
            }
            if(i < lengthFEN && FEN[i] == 'q') {
                blackCastlingQueenside = true;
                i++;
            }
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    
    i++;
    if(i < lengthFEN && FEN[i-1] == ' ') {
        if(FEN[i] != '-') {
            xEnPassantSquare = FEN[i] - 'a';
            if(xEnPassantSquare < 0 || xEnPassantSquare > 7) {
                return false;
            }
            i++;
            if(i < lengthFEN && !((FEN[i] - '0') < 0 || (FEN[i] - '0') > 7)) {
                yEnPassantSquare = BOARD_LENGTH - (FEN[i] - '0');
                enPassant = true;
            }
            else {
                return false;
            }
        }
    }

    return true;
}

std::string State::getFEN() {
    std::string FEN = "";
    int countEmptySquares = 0;
    
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            switch(byteBoard[i][j]) {
                case NO_PIECE:
                    countEmptySquares = 1;
                    j++;
                    while(byteBoard[i][j] == NO_PIECE && j < BOARD_LENGTH-1) {
                        countEmptySquares++;
                        j++;
                    }
                    j--;
                    FEN += char('0' + countEmptySquares);
                    break;
                case BLACK_PAWN: FEN += 'p'; break;
                case WHITE_PAWN: FEN += 'P'; break;
                case BLACK_KNIGHT: FEN += 'n'; break;
                case WHITE_KNIGHT: FEN += 'N'; break;
                case BLACK_BISHOP: FEN += 'b'; break;
                case WHITE_BISHOP: FEN += 'B'; break;
                case BLACK_ROOK: FEN += 'r'; break;
                case WHITE_ROOK: FEN += 'R'; break;
                case BLACK_QUEEN: FEN += 'q'; break;
                case WHITE_QUEEN: FEN += 'Q'; break;
                case BLACK_KING: FEN += 'k'; break;
                case WHITE_KING: FEN += 'K'; break;
            }
            countEmptySquares = 0;
        }
        FEN += '/';
    }

    FEN += ' ';
    if(turn) {
        FEN += 'w';
    }
    else {
        FEN += 'b';
    }

    FEN += ' ';
    if(whiteCastlingKingside || whiteCastlingQueenside || blackCastlingKingside || blackCastlingQueenside) {
        if(whiteCastlingKingside) {
            FEN += 'K';
        }
        if(whiteCastlingQueenside) {
            FEN += 'Q';
        }
        if(blackCastlingKingside) {
            FEN += 'k';
        }
        if(blackCastlingQueenside) {
            FEN += 'q';
        }
    }
    else {
        FEN += '-';
    }

    FEN += ' ';
    if(enPassant) {
        FEN += char('a' + xEnPassantSquare);
        FEN += char('0' + yEnPassantSquare);
    }
    else {
        FEN += '-';
    }

    return FEN;
}