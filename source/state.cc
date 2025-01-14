#include "../include/state.h"

#include <iostream>

void State::initState()
{
    turn = Color::WHITE;
    enPassantPos = 0;
    whiteCastlingQueenside = true;
    whiteCastlingKingside = true;
    blackCastlingQueenside = true;
    blackCastlingKingside = true;
    whiteKingPos = 0;
    blackKingPos = 0;

    // Setting to board to empty, 0 is value for NO_PIECE
    std::memset(board, 0, BOARD_SIZE);
}

State::State()
{
    if(!initStateFromFEN(STANDARD_OPENING_FEN)) {
        std::cerr << ">> Invalid FEN-notation. State is set to an empty board." << std::endl;
        initState();
    }
}

State::State(std::string FEN)
{
    if (!initStateFromFEN(FEN))
    {
        std::cerr << ">> Invalid FEN-notation. State is set to an empty board." << std::endl;
        initState();
    }
}

State::State(State const &state)
    : turn(state.turn),
      enPassantPos(state.enPassantPos),
      whiteCastlingQueenside(state.whiteCastlingQueenside),
      whiteCastlingKingside(state.whiteCastlingKingside),
      blackCastlingQueenside(state.blackCastlingQueenside),
      blackCastlingKingside(state.blackCastlingKingside),
      whiteKingPos(state.whiteKingPos),
      blackKingPos(state.blackKingPos)
{
    memcpy(board, state.board, BOARD_SIZE);
}

bool State::isPosWithinBoardLimits(int const pos)
{
    return pos >= 0 && pos < BOARD_SIZE;
}

void State::passTurn()
{
    if (turn == Color::WHITE)
    {
        turn = Color::BLACK;
    }
    else
    {
        turn = Color::WHITE;
    }
    enPassantPos = -1;
}

Color State::getTurn()
{
    return turn;
}

int State::getKingPosOfColor(Color const color)
{
    if (color == Color::WHITE)
    {
        return whiteKingPos;
    }
    return blackKingPos;
}

Piece State::getPieceAtPos(int const pos)
{
    return board[pos];
}

void State::makeMove(Piece const piece, int const from, int const to)
{
    switch (piece)
    {
    case Piece::WHITE_KING:
        if (to == from + 2)
        {
            board[to + 1] = Piece::NO_PIECE;
            board[to - 1] = Piece::WHITE_ROOK;
        } // castling king side
        else if (to == from - 2)
        {
            board[to - 2] = Piece::NO_PIECE;
            board[to + 1] = Piece::WHITE_ROOK;
        } // castling queen side
        whiteKingPos = to;
        break;
    case Piece::BLACK_KING:
        if (to == from + 2)
        {
            board[to + 1] = Piece::NO_PIECE;
            board[to - 1] = Piece::BLACK_ROOK;
        } // castling king side
        else if (to == from - 2)
        {
            board[to - 2] = Piece::NO_PIECE;
            board[to + 1] = Piece::BLACK_ROOK;
        } // castling queen side
        blackKingPos = to;
        break;
    case Piece::WHITE_PAWN:
        if (to == enPassantPos)
        {
            board[to - BOARD_LENGTH] = Piece::NO_PIECE;
        }
        break;
    case Piece::BLACK_PAWN:
        if (to == enPassantPos)
        {
            board[to + BOARD_LENGTH] = Piece::NO_PIECE;
        }
        break;
    }

    board[from] = Piece::NO_PIECE;
    board[to] = piece;
}

int State::getEnPassantPos()
{
    return enPassantPos;
}

bool State::getCastlingKingSide(Color const color)
{
    return color == Color::WHITE ? whiteCastlingKingside : blackCastlingKingside;
}

bool State::getCastlingQueenSide(Color const color)
{
    return color == Color::BLACK ? whiteCastlingQueenside : blackCastlingQueenside;
}

void State::setSpecialMovesData(uint8_t const pieceByte, int const from, int const to)
{
    switch (pieceByte)
    {
    case WHITE_PAWN:
        if (to == from + BOARD_LENGTH + BOARD_LENGTH)
        {
            enPassantPos = from + BOARD_LENGTH;
        }
        break;
    case BLACK_PAWN:
        if (to == from - BOARD_LENGTH - BOARD_LENGTH)
        {
            enPassantPos = from - BOARD_LENGTH;
        }
        break;
    case WHITE_KING:
        if (whiteCastlingKingside)
        {
            whiteCastlingKingside = false;
        }
        if (whiteCastlingQueenside)
        {
            whiteCastlingQueenside = false;
        }
        break;
    case BLACK_KING:
        if (blackCastlingKingside)
        {
            blackCastlingKingside = false;
        }
        if (blackCastlingQueenside)
        {
            blackCastlingQueenside = false;
        }
        break;
    case WHITE_ROOK:
        if (whiteCastlingKingside && from == 0)
        {
            whiteCastlingKingside = false;
        }
        else if (whiteCastlingQueenside && from == BOARD_LENGTH - 1)
        {
            whiteCastlingQueenside = false;
        }
    case BLACK_ROOK:
        if (blackCastlingKingside && from == BOARD_SIZE - 1)
        {
            blackCastlingKingside = false;
        }
        else if (blackCastlingQueenside && from == BOARD_SIZE - BOARD_LENGTH)
        {
            blackCastlingQueenside = false;
        }
    }
}

bool State::initStateFromFEN(std::string FEN)
{
    initState();
    int const lengthFEN = FEN.length();
    int i = 0;

    // setting the board
    int posY = BOARD_LENGTH - 1;
    int posX = 0;
    int pos = BOARD_SIZE - BOARD_LENGTH;
    while (i < lengthFEN && posY >= 0 && posX <= BOARD_LENGTH && FEN[i] != ' ')
    {
        switch (FEN[i])
        {
        case '/':
            posY--;
            posX = -1;
            pos = posY * BOARD_LENGTH - 1;
            break;
        case 'p':
            board[pos] = BLACK_PAWN;
            break;
        case 'P':
            board[pos] = WHITE_PAWN;
            break;
        case 'n':
            board[pos] = BLACK_KNIGHT;
            break;
        case 'N':
            board[pos] = WHITE_KNIGHT;
            break;
        case 'b':
            board[pos] = BLACK_BISHOP;
            break;
        case 'B':
            board[pos] = WHITE_BISHOP;
            break;
        case 'r':
            board[pos] = BLACK_ROOK;
            break;
        case 'R':
            board[pos] = WHITE_ROOK;
            break;
        case 'q':
            board[pos] = BLACK_QUEEN;
            break;
        case 'Q':
            board[pos] = WHITE_QUEEN;
            break;
        case 'k':
            board[pos] = BLACK_KING;
            blackKingPos = pos;
            break;
        case 'K':
            board[pos] = WHITE_KING;
            whiteKingPos = pos;
            break;
        default:
            int const tmp = FEN[i] - '0';
            if (tmp < 0 || tmp > BOARD_LENGTH)
            {
                return false;
            }
            posX += tmp - 1;
            pos += tmp - 1;
            break;
        }
        pos++;
        posX++;
        i++;
    }
    if (whiteKingPos == -1 || blackKingPos == -1 || posX != BOARD_LENGTH || posY != 0)
    {
        return false;
    } // no king(s) on the board or fen did not cover whole board (incorrect notation)

    i++;
    // who's turn is it
    if (i < lengthFEN && FEN[i - 1] == ' ')
    {
        switch (FEN[i])
        {
        case 'w':
            turn = WHITE;
            break;
        case 'b':
            turn = BLACK;
            break;
        default:
            return false;
        }
    }
    else
    {
        return false;
    }

    i += 2;
    if (i < lengthFEN && FEN[i - 1] == ' ')
    {
        // castling data
        if (FEN[i] == 'K' || FEN[i] == 'Q' || FEN[i] == 'k' || FEN[i] == 'q')
        {
            if (FEN[i++] == 'K')
            {
                whiteCastlingKingside = true;
            }
            if (i < lengthFEN && FEN[i++] == 'Q')
            {
                whiteCastlingQueenside = true;
            }
            if (i < lengthFEN && FEN[i++] == 'k')
            {
                blackCastlingKingside = true;
            }
            if (i < lengthFEN && FEN[i++] == 'q')
            {
                blackCastlingQueenside = true;
            }
        }
        else if (FEN[i++] != '-')
        {
            return false;
        }
        if (FEN[i++] != ' ')
        {
            return false;
        }

        // En Passant data
        if (FEN[i++] != '-')
        {
            if (i < lengthFEN - 1)
                enPassantPos = FEN[i++] - 'a';
            if (enPassantPos >= BOARD_LENGTH && i >= lengthFEN)
            {
                return false;
            }
            enPassantPos += (FEN[i++] - '0') * BOARD_LENGTH;
            if (enPassantPos >= BOARD_SIZE)
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

// NOT NEEDED RIGHT NOW, SO LEAVING AS IS FOR NOW
// std::string State::getFEN() {
//     std::string FEN = "";
//     int countEmptySquares = 0;

//     for(int i = 0; i < BOARD_SIZE; i++) {
//             switch(board[i]) {
//                 case NO_PIECE:
//                     countEmptySquares = 1;
//                     i++;
//                     while(board[i][j] == NO_PIECE && j < BOARD_LENGTH-1) {
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

void State::debugPrintState()
{
    std::cout << "--------===== DEBUG STATE =====--------" << std::endl
              << "Turn: " << turn << std::endl
              << "enPassantPos: " << enPassantPos << std::endl
              << "whiteCastlingQueenside: " << whiteCastlingQueenside << std::endl
              << "whiteCastlingKingside: " << whiteCastlingKingside << std::endl
              << "blackCastlingQueenside: " << blackCastlingQueenside << std::endl
              << "blackCastlingKingside: " << blackCastlingKingside << std::endl
              << "whiteKingPos: " << whiteKingPos << std::endl
              << "blackKingPos: " << blackKingPos << std::endl
              << std::endl;

    for (int i = BOARD_LENGTH - 1; i >= 0; i--)
    {
        for (int pos = i * BOARD_LENGTH; pos < (i + 1) * BOARD_LENGTH; pos++)
        {
            std::cout << " ";
            switch (board[pos])
            {
            case WHITE_PAWN:
                std::cout << "P";
                break;
            case BLACK_PAWN:
                std::cout << "p";
                break;
            case WHITE_KNIGHT:
                std::cout << "N";
                break;
            case BLACK_KNIGHT:
                std::cout << "n";
                break;
            case WHITE_BISHOP:
                std::cout << "B";
                break;
            case BLACK_BISHOP:
                std::cout << "b";
                break;
            case WHITE_ROOK:
                std::cout << "R";
                break;
            case BLACK_ROOK:
                std::cout << "r";
                break;
            case WHITE_QUEEN:
                std::cout << "Q";
                break;
            case BLACK_QUEEN:
                std::cout << "q";
                break;
            case WHITE_KING:
                std::cout << "K";
                break;
            case BLACK_KING:
                std::cout << "k";
                break;
            default:
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}