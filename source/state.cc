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

bool State::isLegalMove(uint8_t const pieceByte, 
                        int const prevX, int const prevY, 
                        int const newX, int const newY) {

    if(newX == prevX && newY == prevY) {
        return false;
    }
    
    uint8_t pieceOfCapture = getByteFromByteBoard(newX, newY);
    if(pieceByte & 0b10000000) {
        switch(pieceByte) {
            case 0b10000001:
                if(((newY < prevY && (prevY - newY <= (1 + (prevY == 6 && getByteFromByteBoard(prevX, prevY-1) == 0b00000000)))))
                     && ((newX == prevX && pieceOfCapture == 0b00000000)
                         || (((newX - prevX == 1) || (prevX - newX == 1)) 
                             && !(pieceOfCapture == 0b00000000 || pieceOfCapture & 0b10000000)))) {
                    return true;
                }
                break;
            case 0b10000010:
                if((((newY == prevY-2 || newY == prevY+2) && (newX == prevX-1 || newX == prevX+1)) 
                    || ((newY == prevY-1 || newY == prevY+1) && (newX == prevX-2 || newX == prevX+2)))
                   && !(pieceOfCapture & 0b10000000)) {
                       return true;
                   }
                break;
            case 0b10010000: // queen is same as bishop + rook
            case 0b10000100:
                if((newY - prevY) == (newX - prevX) || (newY - prevY) == (prevX - newX)) {       
                    bool const upwardsDiagonal = (newY - prevY) < 0;
                    bool const leftsideDiagonal = (newX - prevX) < 0;
                    int const diagonalSteps = (prevY - newY) * upwardsDiagonal + (newY - prevY) * !upwardsDiagonal; 

                    // Checks if diagonal from previous square till new square is empty
                    for(int i = 1; i < diagonalSteps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideDiagonal) + i*!leftsideDiagonal), 
                                                (prevY+(i*(-1 * upwardsDiagonal) + i*!upwardsDiagonal))) != 0b00000000) {
                            return false;
                        }
                    }

                    if(!(pieceOfCapture & 0b10000000)) {
                        return true;
                    }
                }
                if(pieceByte != 0b10010000) {
                    break;
                } // no break if the piece is a queen
            case 0b10001000:
                if(newX == prevX && newY != prevY) {
                    bool const upwardsMove = (newY - prevY) < 0;
                    int const steps = (prevY - newY) * upwardsMove + (newY - prevY) * !upwardsMove;

                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(newX, (prevY+(i*(-1 * upwardsMove) + i*!upwardsMove))) != 0b00000000) {
                            return false;
                        }
                    }

                    if(!(pieceOfCapture & 0b10000000)) {
                        return true;
                    }
                }
                else if(newX != prevX && newY == prevY) {
                    bool const leftsideMove = (newX - prevX) < 0;
                    int const steps = (prevX - newX) * leftsideMove + (newX - prevX) * !leftsideMove;

                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideMove) + i*!leftsideMove), newY) != 0b00000000) {
                            return false;
                        }
                    }

                    if(!(pieceOfCapture & 0b10000000)) {
                        return true;
                    }
                }
                break;
            case 0b10100000:
                if((newX - prevX) < 2 && (newX - prevX) > -2 && (newY - prevY) < 2 && (newY - prevY) > -2
                   && !(pieceOfCapture & 0b10000000)) {
                    return true;
                }
                break;
        }
    }
    else {
        switch(pieceByte) {
            case 0b01000001:

                break;
            case 0b01000010:

                break;
            case 0b01000100:

                break;
            case 0b01001000:

                break;
            case 0b01010000:

                break;
            case 0b01100000:

                break;
        }
    }

    return false;
}