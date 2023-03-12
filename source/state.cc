#include "../include/state.h"

#include <iostream>

void State::clearByteBoard() {
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            byteBoard[i][j] = NO_PIECE;
        }
    }
}

void State::clearAttackFieldBlack() {
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            attackFieldBlack[i][j] = false;
        }
    }
}

void State::clearAttackFieldWhite() {
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            attackFieldWhite[i][j] = false;
        }
    }
}

State::State() {
    enPassant = false;
    whiteCastlingQueenside = false;
    whiteCastlingKingside = false;
    blackCastlingQueenside = false;
    blackCastlingKingside = false;
    check = false;
    turn = true;
    clearByteBoard();
    clearAttackFieldBlack();
    clearAttackFieldWhite();
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

uint8_t State::getByteFromByteBoard(int const x, int const y) {
    return byteBoard[y][x];
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

void State::passTurn() {
    turn = !turn;
    enPassant = false;
    check = false;
}

bool State::getTurn() {
    return turn;
}

void State::setByteInByteBoard(uint8_t const byte, int const x, int const y) {
    if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
        std::cerr << "[setByteInByteBoard] out-of-bounds position." << std::endl;
        exit(1);
    }
    byteBoard[y][x] = byte;
}

void State::setEnPassantSquare(int const x, int const y) {
    if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
        std::cerr << "[setEnPassantSquare] out-of-bounds position." << std::endl;
        exit(1);
    }
    xEnPassantSquare = x;
    yEnPassantSquare = y;
    enPassant = true;
}

void State::isCheck(uint8_t const pieceByte, 
                     int const x, int const y) {
    if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
        std::cerr << "[isCheck] out-of-bounds position." << std::endl;
        exit(1);
    }

    if(pieceByte == WHITE_KING || pieceByte == BLACK_KING) {
        // kings cannot check
        return;
    }
    uint8_t const kingToCheck = isWhitePiece(pieceByte) ? BLACK_KING : WHITE_KING;

    switch(pieceByte) {
        case WHITE_PAWN:
            if(y > 0 && ((x > 0 && getByteFromByteBoard(x-1,y-1) == kingToCheck) 
                         || (x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1,y-1) == kingToCheck))) {
                    check = true;
            }
            break;
        case BLACK_PAWN:
            if(y < BOARD_LENGTH-1 && ((x > 0 && getByteFromByteBoard(x-1,y+1) == kingToCheck) 
                         || (x < 7 && getByteFromByteBoard(x+1,y+1) == kingToCheck))) {
                check = true;
            }
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            if((y > 1 && ((x > 0 && getByteFromByteBoard(x-1, y-2) == kingToCheck) 
                          || (x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1, y-2) == kingToCheck)))
               || (y < BOARD_LENGTH-2 && ((x > 0 && getByteFromByteBoard(x-1, y+2) == kingToCheck) 
                             || (x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1, y+2) == kingToCheck)))
               || (x < 1 && ((y > 0 && getByteFromByteBoard(x-2, y-1) == kingToCheck) 
                             || (y < BOARD_LENGTH-1 && getByteFromByteBoard(x-2, y+1) == kingToCheck)))
               || (x < BOARD_LENGTH-2 && ((y > 0 && getByteFromByteBoard(x+2, y-1) == kingToCheck) 
                             || (y < BOARD_LENGTH-1 && getByteFromByteBoard(x+2, y+1) == kingToCheck)))) {
                check = true;
            }
            break;
        case WHITE_QUEEN: // queen is same as bishop + rook
        case BLACK_QUEEN:
        case WHITE_BISHOP:
        case BLACK_BISHOP: {
            int tmpX = x;
            int tmpY = y;
            while(tmpX > 0 && tmpY > 0) {
                tmpX--;
                tmpY--;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX > 0 && tmpY < BOARD_LENGTH-1) {
                tmpX--;
                tmpY++;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX < BOARD_LENGTH-1 && tmpY > 0) {
                tmpX++;
                tmpY--;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX < BOARD_LENGTH-1 && tmpY < BOARD_LENGTH-1) {
                tmpX++;
                tmpY++;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }

            if(pieceByte != WHITE_QUEEN && pieceByte != BLACK_QUEEN) {
                break;
            } // no break if the piece is a queen (queen can also do rook moves)
        }
        case WHITE_ROOK:
        case BLACK_ROOK:
            int tmpX = x;
            while(tmpX > 0) {
                tmpX--;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, y);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            while(tmpX < BOARD_LENGTH-1) {
                tmpX++;
                uint8_t byteFromBoard = getByteFromByteBoard(tmpX, y);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            int tmpY = y;
            while(tmpY > 0) {
                tmpY--;
                uint8_t byteFromBoard = getByteFromByteBoard(x, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            tmpY = y;
            while(tmpY < BOARD_LENGTH-1) {
                tmpY++;
                uint8_t byteFromBoard = getByteFromByteBoard(x, tmpY);
                if(byteFromBoard == kingToCheck) {
                    check = true;
                    return;
                }
                if(byteFromBoard != NO_PIECE) {
                    break;
                }
            }
            break;
    }
}

std::vector<std::pair<int, int>> State::getAttackFieldOfPiece(uint8_t const pieceByte,
                                                              int const x, int const y) {
    if(!(x >= 0 && x < BOARD_LENGTH && y >= 0 && y < BOARD_LENGTH)) {
        std::cerr << "[getAttackFieldOfPiece] out-of-bounds position." << std::endl;
        exit(1);
    }

    std::vector<std::pair<int, int>> attackField;
    switch(pieceByte) {
        case WHITE_PAWN:
            if(y > 0) {
                if(x > 0 && getByteFromByteBoard(x-1,y-1) != NO_PIECE) {
                    attackField.push_back(std::pair(x-1, y-1));
                }
                if(x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1,y-1) != NO_PIECE) {
                    attackField.push_back(std::pair(x+1, y-1));
                }
            }
            break;
        case BLACK_PAWN:
            if(y < BOARD_LENGTH-1 && (x > 0 && getByteFromByteBoard(x-1,y+1) != NO_PIECE)) {
                attackField.push_back(std::pair(x-1, y+1));
            }
            if(y < BOARD_LENGTH-1 && (x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1,y+1) != NO_PIECE)) {
                attackField.push_back(std::pair(x+1, y+1));
            }
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            if(y > 1) {
                if(x > 0 && getByteFromByteBoard(x-1, y-2) != NO_PIECE) {
                    attackField.push_back(std::pair(x-1, y-2));
                }
                if(x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1, y-2) != NO_PIECE) {
                    attackField.push_back(std::pair(x+1, y-2));
                }
            }
            if(y < BOARD_LENGTH-2) {
                if(x > 0 && getByteFromByteBoard(x-1, y+2) != NO_PIECE) {
                    attackField.push_back(std::pair(x-1, y+2));
                }
                if(x < BOARD_LENGTH-1 && getByteFromByteBoard(x+1, y+2) != NO_PIECE) {
                    attackField.push_back(std::pair(x+1, y+2));
                }
            }
            if(x > 1) {
                if(y > 0 && getByteFromByteBoard(x-2, y-1) != NO_PIECE) {
                    attackField.push_back(std::pair(x-2, y-1));
                }
                if(y < BOARD_LENGTH-1 && getByteFromByteBoard(x-2, y+1) != NO_PIECE) {
                    attackField.push_back(std::pair(x-2, y+1));
                }
            }
            if(x < BOARD_LENGTH-2) {
                if(y > 0 && getByteFromByteBoard(x+2, y-1) != NO_PIECE) {
                    attackField.push_back(std::pair(x+2, y-1));
                }
                if(y < BOARD_LENGTH-1 && getByteFromByteBoard(x+2, y+1) != NO_PIECE) {
                    attackField.push_back(std::pair(x+2, y+1));
                }
            }
            break;
        case WHITE_QUEEN: // queen is same as bishop + rook
        case BLACK_QUEEN:
        case WHITE_BISHOP:
        case BLACK_BISHOP: {
            int tmpX = x;
            int tmpY = y;
            while(tmpX > 0 && tmpY > 0) {
                tmpX--;
                tmpY--;
                attackField.push_back(std::pair(tmpX, tmpY));
                if(getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX > 0 && tmpY < BOARD_LENGTH-1) {
                tmpX--;
                tmpY++;
                attackField.push_back(std::pair(tmpX, tmpY));
                if(getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX < BOARD_LENGTH-1 && tmpY > 0) {
                tmpX++;
                tmpY--;
                attackField.push_back(std::pair(tmpX, tmpY));
                if(getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            tmpY = y;
            while(tmpX < BOARD_LENGTH-1 && tmpY < BOARD_LENGTH-1) {
                tmpX++;
                tmpY++;
                attackField.push_back(std::pair(tmpX, tmpY));
                if(getByteFromByteBoard(tmpX, tmpY) != NO_PIECE) {
                    break;
                }
            }

            if(pieceByte != WHITE_QUEEN && pieceByte != BLACK_QUEEN) {
                break;
            } // no break if the piece is a queen (queen can also do rook moves)
        }
        case WHITE_ROOK:
        case BLACK_ROOK:
            int tmpX = x;
            while(tmpX > 0) {
                tmpX--;
                attackField.push_back(std::pair(tmpX, y));
                if(getByteFromByteBoard(tmpX, y) != NO_PIECE) {
                    break;
                }
            }
            tmpX = x;
            while(tmpX < BOARD_LENGTH-1) {
                tmpX++;
                attackField.push_back(std::pair(tmpX, y));
                if(getByteFromByteBoard(tmpX, y) != NO_PIECE) {
                    break;
                }
            }
            int tmpY = y;
            while(tmpY > 0) {
                tmpY--;
                attackField.push_back(std::pair(x, tmpY));
                if(getByteFromByteBoard(x, tmpY) != NO_PIECE) {
                    break;
                }
            }
            tmpY = y;
            while(tmpY < BOARD_LENGTH-1) {
                tmpY++;
                attackField.push_back(std::pair(x, tmpY));
                if(getByteFromByteBoard(x, tmpY) != NO_PIECE) {
                    break;
                }
            }
            break;
    }

    return attackField;                                                   
}

void State::updateAttackFields() {
    clearAttackFieldBlack();
    clearAttackFieldWhite();
    for(int i = 0; i < BOARD_LENGTH; i++) {
        for(int j = 0; j < BOARD_LENGTH; j++) {
            if(byteBoard[i][j] != NO_PIECE) {
                std::vector<std::pair<int, int>> attackField = getAttackFieldOfPiece(byteBoard[i][j], i, j);
                if(isWhitePiece(byteBoard[i][j])) {
                    for(int k = 0; k < attackField.size(); k++) {
                        attackFieldWhite[attackField[k].first][attackField[k].second] = true;
                    }
                }
                else {
                    for(int k = 0; k < attackField.size(); k++) {
                        attackFieldBlack[attackField[k].first][attackField[k].second] = true;
                    }
                }
            }
        }
    }
}

std::vector<std::pair<int, int>> State::getLegalMovesOfPiece(uint8_t const pieceByte,
                                                             int const x, int const y) {
    //TODO
    std::vector<std::pair<int, int>> legalMoves = getAttackFieldOfPiece(pieceByte, x, y);
    
    // add fields for pawn, because pawn can move to "non-attacking" fields
    return legalMoves;
}

bool State::isLegalMove(uint8_t const pieceByte, 
                        int const prevX, int const prevY, 
                        int const newX, int const newY,
                        bool& enPassantMove, bool& castlingMove) {
    if(!(prevX >= 0 && prevX < BOARD_LENGTH && prevY >= 0 && prevY < BOARD_LENGTH)
        || !(newX >= 0 && newX < BOARD_LENGTH && newY >= 0 && newY < BOARD_LENGTH)) {
        std::cerr << "[isLegalMove] out-of-bounds position." << std::endl;
        exit(1);
    }

    if(newX == prevX && newY == prevY) {
        return false;
    }
    
    uint8_t pieceOfCapture = getByteFromByteBoard(newX, newY);
    if(pieceOfCapture != NO_PIECE && isWhitePiece(pieceOfCapture) == isWhitePiece(pieceByte)) {
        // can never capture similar colored piece
        return false;
    }
    std::vector<std::pair<int, int>> attackField = getAttackFieldOfPiece(pieceByte, prevX, prevY);

    for(int i = 0; i < attackField.size(); i++) {
        std::cout << "attacking: " << attackField[i].first << " " << attackField[i].second << std::endl;
        if(newX == attackField[i].first && newY == attackField[i].second) {
            return true;
        }
    }

    return false;
}