#include "../include/state.h"

#include <iostream>

State::State() {
    turn = true;

    byteBoard[0][0] = 0b01001000;
    byteBoard[0][1] = 0b01000010;
    byteBoard[0][2] = 0b01000100;
    byteBoard[0][3] = 0b01010000;
    byteBoard[0][4] = 0b01100000;
    byteBoard[0][5] = 0b01000100;
    byteBoard[0][6] = 0b01000010;
    byteBoard[0][7] = 0b01001000;
    for(int m = 0; m < 8; m++) {
        byteBoard[1][m] = 0b01000001;
    } // black pawns
    
    for(int i = 2; i < 6; i++) {
        for(int j = 0; j < 8; j++) {
            byteBoard[i][j] = 0b00000000;
        }
    } // empty squares

    byteBoard[7][0] = 0b10001000;
    byteBoard[7][1] = 0b10000010;
    byteBoard[7][2] = 0b10000100;
    byteBoard[7][3] = 0b10010000;
    byteBoard[7][4] = 0b10100000;
    byteBoard[7][5] = 0b10000100;
    byteBoard[7][6] = 0b10000010;
    byteBoard[7][7] = 0b10001000;
    for(int m = 0; m < 8; m++) {
        byteBoard[6][m] = 0b10000001;
    } // white pawns
}

uint8_t State::getByteFromByteBoard(int const x, int const y) {
    return byteBoard[y][x];
}

std::string State::getFEN() {
    std::string FEN = "";
    int countEmptySquares = 0;
    
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            switch(byteBoard[i][j]) {
                case 0b00000000:
                    countEmptySquares = 1;
                    j++;
                    while(byteBoard[i][j] == 0b00000000 && j < 8) {
                        countEmptySquares++;
                        j++;
                    }
                    j--;
                    FEN += std::to_string(countEmptySquares);
                    break;
                case 0b01000001:
                    FEN += 'p';
                    break;
                case 0b10000001:
                    FEN += 'P';
                    break;
                case 0b01000010:
                    FEN += 'n';
                    break;
                case 0b10000010:
                    FEN += 'N';
                    break;
                case 0b01000100:
                    FEN += 'b';
                    break;
                case 0b10000100:
                    FEN += 'B';
                    break;
                case 0b01001000:
                    FEN += 'r';
                    break;
                case 0b10001000:
                    FEN += 'R';
                    break;
                case 0b01010000:
                    FEN += 'q';
                    break;
                case 0b10010000:
                    FEN += 'Q';
                    break;
                case 0b01100000:
                    FEN += 'k';
                    break;
                case 0b10100000:
                    FEN += 'K';
                    break;
            }
            countEmptySquares = 0;
        }
        FEN += '/';
    }
    return FEN;
}

bool State::getTurn() {
    return turn;
}

void State::setByteInByteBoard(int const x, int const y, uint8_t const bit) {
    byteBoard[y][x] = bit;
}