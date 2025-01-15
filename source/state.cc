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
    if (!initStateFromFENString(STANDARD_OPENING_FENString))
    {
        std::cerr << ">> Invalid FENString-notation. State is set to an empty board." << std::endl;
        initState();
    }
}

State::State(std::string FENString)
{
    if (!initStateFromFENString(FENString))
    {
        std::cerr << ">> Invalid FENString-notation. State is set to an empty board." << std::endl;
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

bool State::makeMove(Move const move)
{
    /* Move validation */
    if (!isPosWithinBoardLimits(move.from) || !isPosWithinBoardLimits(move.to) || board[move.from] != move.piece)
    {
        std::cerr << "Invalid move" << std::endl;
        return false;
    }

    switch (move.piece)
    {
    case Piece::WHITE_KING:
        if (whiteCastlingKingside || whiteCastlingQueenside)
        {
            if (move.to == move.from + 2)
            {
                /* Castling king side */
                board[move.to + 1] = Piece::NO_PIECE;
                board[move.to - 1] = Piece::WHITE_ROOK;
            }
            else if (move.to == move.from - 2)
            {
                /* Castling queen side */
                board[move.to - 2] = Piece::NO_PIECE;
                board[move.to + 1] = Piece::WHITE_ROOK;
            }
            whiteCastlingKingside = false;
            whiteCastlingQueenside = false;
        }
        whiteKingPos = move.to;
        break;
    case Piece::BLACK_KING:
        if (blackCastlingKingside || blackCastlingQueenside)
        {
            if (move.to == move.from + 2)
            {
                /* Castling king side */
                board[move.to + 1] = Piece::NO_PIECE;
                board[move.to - 1] = Piece::BLACK_ROOK;
            }
            else if (move.to == move.from - 2)
            {
                /* Castling queen side */
                board[move.to - 2] = Piece::NO_PIECE;
                board[move.to + 1] = Piece::BLACK_ROOK;
            }
            blackCastlingKingside = false;
            blackCastlingQueenside = false;
        }
        blackKingPos = move.to;
        break;
    case Piece::WHITE_PAWN:
        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to - BOARD_LENGTH] = Piece::NO_PIECE;
        }
        else if (move.to == move.from + BOARD_LENGTH + BOARD_LENGTH)
        {
            enPassantPos = move.from + BOARD_LENGTH;
        }
        break;
    case Piece::BLACK_PAWN:
        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to + BOARD_LENGTH] = Piece::NO_PIECE;
        }
        else if (move.to == move.from - BOARD_LENGTH - BOARD_LENGTH)
        {
            enPassantPos = move.from - BOARD_LENGTH;
        }
        break;
    }

    board[move.from] = Piece::NO_PIECE;
    board[move.to] = move.piece;
    return true;
}


std::vector<Move> State::getLegalMovesForPos(int const pos)
{
    /**
     * A legal move of a piece is:
     * 1. a move according to the rules of the piece ----> Game::getPossibleMoves
     * 2. a move that is within the bounds of the board ----> Interface
     * 3. a move that is either: ----> Game::getPossibleMoves
     *    3.1 to an empty square
     *    3.2 to a piece of the opponent's color
     * 4. [RULE 4 is exempted for knights] a move that does not "hop over" pieces ----> Game::getPossibleMoves
     * 5. a move that does not leave your own king attacked ----> Game::getLegalMoves
     */

    std::vector<Move> *legalMoves = getPossibleMoves(this->state, pieceByte, pos);
    State *stateCopy = state->copyState();
    bool colorPieceByte = getColorOfPiece(pieceByte);

    for (int i = 0; i < legalMoves->size(); i++)
    {
        int newPos = legalMoves->at(i);
        uint8_t tmp = stateCopy->getPieceAt(newPos);
        stateCopy->movePiece(pieceByte, pos, newPos);
        if (isPosAttacked(stateCopy, stateCopy->getKingPos(colorPieceByte), !colorPieceByte))
        {
            legalMoves->erase(legalMoves->begin() + i);
            i--;
        }
        stateCopy->movePiece(pieceByte, newPos, pos);
        stateCopy->movePiece(tmp, newPos, newPos);
    }

    // special move: castling
    bool isKingAttacked = isPosAttacked(state, state->getKingPos(colorPieceByte), !colorPieceByte);
    if (!isKingAttacked && (pieceByte == WHITE_KING || pieceByte == BLACK_KING))
    {
        if (state->getCastlingKingSide(colorPieceByte) && state->getPieceAt(pos + 1) == NO_PIECE && !isPosAttacked(state, pos + 1, !colorPieceByte) && state->getPieceAt(pos + 2) == NO_PIECE && !isPosAttacked(state, pos + 2, !colorPieceByte))
        {
            legalMoves->push_back(pos + 2);
        }
        if (state->getCastlingQueenSide(colorPieceByte) && state->getPieceAt(pos - 3) == NO_PIECE && state->getPieceAt(pos - 2) == NO_PIECE && !isPosAttacked(state, pos - 2, !colorPieceByte) && state->getPieceAt(pos - 1) == NO_PIECE && !isPosAttacked(state, pos - 1, !colorPieceByte))
        {
            legalMoves->push_back(pos - 2);
        }
    }

    delete stateCopy;
    std::sort(legalMoves->begin(), legalMoves->end());
    return legalMoves;
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

bool State::initStateFromFENString(std::string FENString)
{
    initState();
    int const lengthFENString = FENString.length();
    int i = 0;

    int row = BOARD_LENGTH - 1;          // top row (black's first row)
    int column = 0;                      // most left column
    int pos = BOARD_SIZE - BOARD_LENGTH; // index of most top left square
    while (i < lengthFENString && row >= 0 && column <= BOARD_LENGTH && FENString[i] != ' ')
    {
        switch (FENString[i])
        {
        case '/':
            row--;
            column = -1;
            pos = row * BOARD_LENGTH - 1;
            break;
        case 'p':
            board[pos] = Piece::BLACK_PAWN;
            break;
        case 'P':
            board[pos] = Piece::WHITE_PAWN;
            break;
        case 'n':
            board[pos] = Piece::BLACK_KNIGHT;
            break;
        case 'N':
            board[pos] = Piece::WHITE_KNIGHT;
            break;
        case 'b':
            board[pos] = Piece::BLACK_BISHOP;
            break;
        case 'B':
            board[pos] = Piece::WHITE_BISHOP;
            break;
        case 'r':
            board[pos] = Piece::BLACK_ROOK;
            break;
        case 'R':
            board[pos] = Piece::WHITE_ROOK;
            break;
        case 'q':
            board[pos] = Piece::BLACK_QUEEN;
            break;
        case 'Q':
            board[pos] = Piece::WHITE_QUEEN;
            break;
        case 'k':
            board[pos] = Piece::BLACK_KING;
            blackKingPos = pos;
            break;
        case 'K':
            board[pos] = Piece::WHITE_KING;
            whiteKingPos = pos;
            break;
        default:
            int const tmp = FENString[i] - '0';
            if (tmp < 0 || tmp > BOARD_LENGTH)
            {
                return false;
            }
            column += tmp - 1;
            pos += tmp - 1;
            break;
        }
        pos++;
        column++;
        i++;
    }
    if (whiteKingPos == -1 || blackKingPos == -1 || column != BOARD_LENGTH || row != 0)
    {
        // no king(s) on the board or FENString did not cover whole board (incorrect notation)
        return false;
    }

    i++;
    if (i < lengthFENString && FENString[i - 1] == ' ')
    {
        switch (FENString[i])
        {
        case 'w':
            turn = Color::WHITE;
            break;
        case 'b':
            turn = Color::BLACK;
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
    if (i < lengthFENString && FENString[i - 1] == ' ')
    {
        if (FENString[i] == 'K' || FENString[i] == 'Q' || FENString[i] == 'k' || FENString[i] == 'q')
        {
            if (FENString[i++] == 'K')
            {
                whiteCastlingKingside = true;
            }
            if (i < lengthFENString && FENString[i++] == 'Q')
            {
                whiteCastlingQueenside = true;
            }
            if (i < lengthFENString && FENString[i++] == 'k')
            {
                blackCastlingKingside = true;
            }
            if (i < lengthFENString && FENString[i++] == 'q')
            {
                blackCastlingQueenside = true;
            }
        }
        else if (FENString[i++] != '-')
        {
            return false;
        }
        if (FENString[i++] != ' ')
        {
            return false;
        }

        if (FENString[i++] != '-')
        {
            if (i < lengthFENString - 1)
                enPassantPos = FENString[i++] - 'a';
            if (enPassantPos >= BOARD_LENGTH && i >= lengthFENString)
            {
                return false;
            }
            enPassantPos += (FENString[i++] - '0') * BOARD_LENGTH;
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

void State::printState()
{
    std::cout << "--------===== PRINTING STATE =====--------" << std::endl
              << "Turn: " << (turn == Color::WHITE ? "White" : "Black") << std::endl
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
            case Piece::WHITE_PAWN:
                std::cout << "P";
                break;
            case Piece::BLACK_PAWN:
                std::cout << "p";
                break;
            case Piece::WHITE_KNIGHT:
                std::cout << "N";
                break;
            case Piece::BLACK_KNIGHT:
                std::cout << "n";
                break;
            case Piece::WHITE_BISHOP:
                std::cout << "B";
                break;
            case Piece::BLACK_BISHOP:
                std::cout << "b";
                break;
            case Piece::WHITE_ROOK:
                std::cout << "R";
                break;
            case Piece::BLACK_ROOK:
                std::cout << "r";
                break;
            case Piece::WHITE_QUEEN:
                std::cout << "Q";
                break;
            case Piece::BLACK_QUEEN:
                std::cout << "q";
                break;
            case Piece::WHITE_KING:
                std::cout << "K";
                break;
            case Piece::BLACK_KING:
                std::cout << "k";
                break;
            default:
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}