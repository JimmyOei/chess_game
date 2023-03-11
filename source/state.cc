#include "../include/state.h"

#include <iostream>

State::State() {
    enPassant = false;
    whiteCastlingQueenside = false;
    whiteCastlingKingside = false;
    blackCastlingQueenside = false;
    blackCastlingKingside = false;
    turn = true;
}

bool State::setByteBoardFromFEN(std::string FEN) {
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
            case 'p':
                byteBoard[byteBoardY][byteBoardX] = 0b01000001;
                break;
            case 'P':
                byteBoard[byteBoardY][byteBoardX] = 0b10000001;
                break;
            case 'n':
                byteBoard[byteBoardY][byteBoardX] = 0b01000010;
                break;
            case 'N':
                byteBoard[byteBoardY][byteBoardX] = 0b10000010;
                break;
            case 'b':
                byteBoard[byteBoardY][byteBoardX] = 0b01000100;
                break;
            case 'B':
                byteBoard[byteBoardY][byteBoardX] = 0b10000100;
                break;
            case 'r':
                byteBoard[byteBoardY][byteBoardX] = 0b01001000;
                break;
            case 'R':
                byteBoard[byteBoardY][byteBoardX] = 0b10001000;
                break;
            case 'q':
                byteBoard[byteBoardY][byteBoardX] = 0b01010000;
                break;
            case 'Q':
                byteBoard[byteBoardY][byteBoardX] = 0b10010000;
                break;
            case 'k':
                byteBoard[byteBoardY][byteBoardX] = 0b01100000;
                break;
            case 'K':
                byteBoard[byteBoardY][byteBoardX] = 0b10100000;
                break;
            default:
                int const nrEmptySquares = FEN[i] - '0';
                if(byteBoardX + nrEmptySquares > 8) {
                    return false;
                }
                for(int i = 0; i < nrEmptySquares; i++) {
                    byteBoard[byteBoardY][byteBoardX] = 0b00000000;
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
                yEnPassantSquare = 8 - (FEN[i] - '0'); // array (size 8) starts counting y from top to bottom
                enPassant = true;
            }
            else {
                return false;
            }
        }
    }

    return true;
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
                    FEN += char('0' + countEmptySquares);
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

void State::passTurn() {
    turn = !turn;
    enPassant = false;
}

bool State::getTurn() {
    return turn;
}

void State::setByteInByteBoard(int const x, int const y, uint8_t const bit) {
    byteBoard[y][x] = bit;
}

void State::setEnPassantSquare(int const x, int const y) {
    xEnPassantSquare = x;
    yEnPassantSquare = y;
    enPassant = true;
}

bool State::isLegalMove(uint8_t const pieceByte, 
                        int const prevX, int const prevY, 
                        int const newX, int const newY,
                        bool& enPassantMove, bool& castlingMove) {

    if(newX == prevX && newY == prevY) {
        return false;
    }
    
    uint8_t pieceOfCapture = getByteFromByteBoard(newX, newY);
    if(pieceByte & 0b10000000) {
        if(pieceOfCapture & 0b10000000) {
            return false;
        }
        switch(pieceByte) {
            case 0b10000001:
                if(enPassant && xEnPassantSquare == newX && yEnPassantSquare == newY) {
                    enPassantMove = true;
                    return true;
                }

                else if(newY < prevY 
                        && (prevY - newY <= (1 + (prevY == 6 && getByteFromByteBoard(prevX, prevY-1) == 0b00000000)))
                        && ((newX == prevX && pieceOfCapture == 0b00000000)
                            || (((newX - prevX == 1) || (prevX - newX == 1)) 
                                && pieceOfCapture != 0b00000000))) {
                    return true;
                }
                break;
            case 0b10000010:
                if(((newY == prevY-2 || newY == prevY+2) && (newX == prevX-1 || newX == prevX+1)) 
                    || ((newY == prevY-1 || newY == prevY+1) && (newX == prevX-2 || newX == prevX+2))) {
                    return true;
                }
                break;
            case 0b10010000: // queen is same as bishop + rook
            case 0b10000100:
                if((newY - prevY) == (newX - prevX) || (newY - prevY) == (prevX - newX)) {       
                    bool const upwardsDiagonal = (newY - prevY) < 0;
                    bool const leftsideDiagonal = (newX - prevX) < 0;
                    int const diagonalSteps = upwardsDiagonal ? prevY - newY : newY - prevY; 

                    // Checks if diagonal from previous square till new square is empty
                    for(int i = 1; i < diagonalSteps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideDiagonal) + i*!leftsideDiagonal), 
                                                (prevY+(i*(-1 * upwardsDiagonal) + i*!upwardsDiagonal))) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                if(pieceByte != 0b10010000) {
                    break;
                } // no break if the piece is a queen
            case 0b10001000:
                if(newX == prevX && newY != prevY) {
                    bool const upwardsMove = (newY - prevY) < 0;
                    int const steps = upwardsMove ? prevY - newY : newY - prevY;

                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(newX, (prevY+(i*(-1 * upwardsMove) + i*!upwardsMove))) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                else if(newX != prevX && newY == prevY) {
                    bool const leftsideMove = (newX - prevX) < 0;
                    int const steps = leftsideMove ? prevY - newY : newY - prevY;

                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideMove) + i*!leftsideMove), newY) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                break;
            case 0b10100000:
                if((newX - prevX) < 2 && (newX - prevX) > -2 && (newY - prevY) < 2 && (newY - prevY) > -2) {
                    return true;
                }
                break;
        }
    }
    else {
        if(pieceOfCapture & 0b01000000) {
            return false;
        }
        switch(pieceByte) {
            case 0b01000001:
                if(enPassant && xEnPassantSquare == newX && yEnPassantSquare == newY) {
                    enPassantMove = true;
                    return true;
                }

                else if(newY > prevY
                        && (newY - prevY <= (1 + (prevY == 1 && getByteFromByteBoard(prevX, prevY+1) == 0b00000000)))
                        && ((newX == prevX && pieceOfCapture == 0b00000000)
                            || (((newX - prevX == 1) || (prevX - newX == 1)) 
                                && pieceOfCapture != 0b00000000))) {
                    return true;
                }
                break;
            case 0b01000010:
                if(((newY == prevY-2 || newY == prevY+2) && (newX == prevX-1 || newX == prevX+1)) 
                    || ((newY == prevY-1 || newY == prevY+1) && (newX == prevX-2 || newX == prevX+2))) {
                    return true;
                }
            case 0b01010000: // queen is same as bishop + rook
            case 0b01000100:
                if((newY - prevY) == (newX - prevX) || (newY - prevY) == (prevX - newX)) {       
                    bool const upwardsDiagonal = (newY - prevY) < 0;
                    bool const leftsideDiagonal = (newX - prevX) < 0;
                    int const diagonalSteps = upwardsDiagonal ? prevY - newY : newY - prevY;  

                    // Checks if diagonal from previous square till new square is empty
                    for(int i = 1; i < diagonalSteps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideDiagonal) + i*!leftsideDiagonal), 
                                                (prevY+(i*(-1 * upwardsDiagonal) + i*!upwardsDiagonal))) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                if(pieceByte != 0b10010000) {
                    break;
                } // no break if the piece is a queen
            case 0b01001000:
                if(newX == prevX && newY != prevY) {
                    bool const upwardsMove = (newY - prevY) < 0;
                    int const steps = upwardsMove ? prevY - newY : newY - prevY;
                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(newX, (prevY+(i*(-1 * upwardsMove) + i*!upwardsMove))) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                else if(newX != prevX && newY == prevY) {
                    bool const leftsideMove = (newX - prevX) < 0;
                    int const steps = leftsideMove ? prevY - newY : newY - prevY;

                    for(int i = 1; i < steps; i++) {
                        if(getByteFromByteBoard(prevX+(i*(-1 * leftsideMove) + i*!leftsideMove), newY) != 0b00000000) {
                            return false;
                        }
                    }

                    return true;
                }
                break;
            case 0b01100000:
                if((newX - prevX) < 2 && (newX - prevX) > -2 && (newY - prevY) < 2 && (newY - prevY) > -2) {
                    return true;
                }
                break;
        }
    }

    return false;
}