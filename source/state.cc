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

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board[i] = Piece::NO_PIECE;
    }
}

State::State()
{
    if (!initStateFromFENString(STANDARD_OPENING_FEN))
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
    if (!isPosWithinBoardLimits(pos))
    {
        return Piece::INVALID;
    }

    return board[pos];
}

bool State::isKingInCheck(Color const color)
{
    int const kingPos = getKingPosOfColor(color);
    int const rookDirections[4] = {NORTH, EAST, WEST, SOUTH};
    int const bishopDirections[4] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST};
    int const knightDirections[8] = {NORTH + NORTH + EAST, NORTH + NORTH + WEST, SOUTH + SOUTH + EAST, SOUTH + SOUTH + WEST,
                                     EAST + EAST + NORTH, EAST + EAST + SOUTH, WEST + WEST + NORTH, WEST + WEST + SOUTH};

    /* Check if king is attacked by a rook */
    for (auto const direction : rookDirections)
    {
        int tmpPos = kingPos + direction;
        while (getPieceAtPos(tmpPos) == Piece::NO_PIECE)
        {
            tmpPos += direction;
        }

        Piece const pieceAtTmpPos = getPieceAtPos(tmpPos);
        if (pieceAtTmpPos != Piece::INVALID &&
            getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            getPieceWithoutColor(pieceAtTmpPos) == Piece::ROOK)
        {
            return true;
        }
    }

    /* Check if king is attacked by a bishop */
    for (auto const direction : bishopDirections)
    {
        int tmpPos = kingPos + direction;
        while (getPieceAtPos(tmpPos) == Piece::NO_PIECE)
        {
            tmpPos += direction;
        }

        Piece const pieceAtTmpPos = getPieceAtPos(tmpPos);
        if (pieceAtTmpPos != Piece::INVALID &&
            getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            getPieceWithoutColor(pieceAtTmpPos) == Piece::BISHOP)
        {
            return true;
        }
    }

    /* Check if king is attacked by a knight */
    for (auto const direction : knightDirections)
    {
        int const tmpPos = kingPos + direction;
        if (getPieceAtPos(tmpPos) != Piece::INVALID &&
            getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            getPieceWithoutColor(getPieceAtPos(tmpPos)) == Piece::KNIGHT)
        {
            return true;
        }
    }

    /* Check if king is attacked by a pawn */
    if (color == Color::WHITE)
    {
        if (getPieceAtPos(kingPos + NORTH + EAST) == Piece::BLACK_PAWN ||
            getPieceAtPos(kingPos + NORTH + WEST) == Piece::BLACK_PAWN)
        {
            return true;
        }
    }
    else
    {
        if (getPieceAtPos(kingPos + SOUTH + EAST) == Piece::WHITE_PAWN ||
            getPieceAtPos(kingPos + SOUTH + WEST) == Piece::WHITE_PAWN)
        {
            return true;
        }
    }

    return false;
}

std::vector<Move> State::getLegalMovesForPos(int const pos)
{
    if (!isPosWithinBoardLimits(pos))
    {
        return {};
    }

    Piece piece = board[pos];
    if (piece == Piece::NO_PIECE)
    {
        return {};
    }

    Color color = getColorOfPiece(piece);
    if (color != turn)
    {
        return {};
    }

    if (isKingInCheck(color) && getPieceWithoutColor(piece) != Piece::KING)
    {
        return {};
    }

    std::vector<Move> moves = {};
    switch (piece)
    {
    case Piece::WHITE_PAWN:
        /* 1. One step upwards */
        if (getPieceAtPos(pos + BOARD_LENGTH) == Piece::NO_PIECE)
        {
            /* 1.1 Promotion */
            if (pos + BOARD_LENGTH + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos + BOARD_LENGTH, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos + BOARD_LENGTH, piece, Piece::WHITE_ROOK},
                              Move{pos, pos + BOARD_LENGTH, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos + BOARD_LENGTH, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos + BOARD_LENGTH, piece});

                /* 2. Two steps upwards */
                if (pos >= BOARD_LENGTH && pos < 2 * BOARD_LENGTH && getPieceAtPos(pos + 2 * BOARD_LENGTH) == Piece::NO_PIECE)
                {
                    moves.push_back(Move{pos, pos + 2 * BOARD_LENGTH, piece});
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        if ((getPieceAtPos(pos + BOARD_LENGTH - 1) != Piece::INVALID &&
             getPieceAtPos(pos + BOARD_LENGTH - 1) != Piece::NO_PIECE && getColorOfPiece(getPieceAtPos(pos + BOARD_LENGTH - 1)) != color) ||
            pos + BOARD_LENGTH - 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos + BOARD_LENGTH - 1 + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos + BOARD_LENGTH - 1, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos + BOARD_LENGTH - 1, piece, Piece::WHITE_ROOK},
                              Move{pos, pos + BOARD_LENGTH - 1, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos + BOARD_LENGTH - 1, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos + BOARD_LENGTH - 1, piece});
            }
        }

        /* 4. Capture right diagonal or En Passant */
        if ((getPieceAtPos(pos + BOARD_LENGTH + 1) != Piece::INVALID &&
             getPieceAtPos(pos + BOARD_LENGTH + 1) != Piece::NO_PIECE &&
             getColorOfPiece(getPieceAtPos(pos + BOARD_LENGTH + 1)) != color) ||
            pos + BOARD_LENGTH + 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos + BOARD_LENGTH + 1 + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos + BOARD_LENGTH + 1, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos + BOARD_LENGTH + 1, piece, Piece::WHITE_ROOK},
                              Move{pos, pos + BOARD_LENGTH + 1, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos + BOARD_LENGTH + 1, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos + BOARD_LENGTH + 1, piece});
            }
        }
        break;
    case Piece::BLACK_PAWN:
        /* 1. One step downards */
        if (getPieceAtPos(pos - BOARD_LENGTH) == Piece::NO_PIECE)
        {
            /* 1.1 Promotion */
            if (pos - 2 * BOARD_LENGTH < 0)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos - BOARD_LENGTH, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos - BOARD_LENGTH, piece, Piece::WHITE_ROOK},
                              Move{pos, pos - BOARD_LENGTH, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos - BOARD_LENGTH, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos - BOARD_LENGTH, piece});

                /* 2. Two steps downwards */
                if (pos >= BOARD_SIZE - 2 * BOARD_LENGTH && pos < BOARD_SIZE - BOARD_LENGTH && getPieceAtPos(pos - 2 * BOARD_LENGTH) == Piece::NO_PIECE)
                {
                    moves.push_back(Move{pos, pos - 2 * BOARD_LENGTH, piece});
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        if ((getPieceAtPos(pos - BOARD_LENGTH - 1) != Piece::INVALID &&
             getPieceAtPos(pos - BOARD_LENGTH - 1) != Piece::NO_PIECE && getColorOfPiece(getPieceAtPos(pos - BOARD_LENGTH - 1)) != color) ||
            pos - BOARD_LENGTH - 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos - BOARD_LENGTH - 1 - BOARD_LENGTH <= BOARD_SIZE)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos - BOARD_LENGTH - 1, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos - BOARD_LENGTH - 1, piece, Piece::WHITE_ROOK},
                              Move{pos, pos - BOARD_LENGTH - 1, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos - BOARD_LENGTH - 1, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos - BOARD_LENGTH - 1, piece});
            }
        }

        /* 4. Capture right diagonal or En Passant */
        if ((getPieceAtPos(pos - BOARD_LENGTH + 1) != Piece::INVALID &&
             getPieceAtPos(pos - BOARD_LENGTH + 1) != Piece::NO_PIECE &&
             getColorOfPiece(getPieceAtPos(pos - BOARD_LENGTH + 1)) != color) ||
            pos - BOARD_LENGTH + 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos - BOARD_LENGTH + 1 - BOARD_LENGTH <= BOARD_SIZE)
            {
                moves.insert(moves.end(),
                             {Move{pos, pos - BOARD_LENGTH + 1, piece, Piece::WHITE_QUEEN},
                              Move{pos, pos - BOARD_LENGTH + 1, piece, Piece::WHITE_ROOK},
                              Move{pos, pos - BOARD_LENGTH + 1, piece, Piece::WHITE_BISHOP},
                              Move{pos, pos - BOARD_LENGTH + 1, piece, Piece::WHITE_KNIGHT}});
            }
            else
            {
                moves.push_back(Move{pos, pos - BOARD_LENGTH + 1, piece});
            }
        }
        break;
    case Piece::WHITE_KNIGHT:
    case Piece::BLACK_KNIGHT:
    {
        int const directionIncrementals[8] = {NORTH + NORTH + EAST, NORTH + NORTH + WEST, SOUTH + SOUTH + EAST, SOUTH + SOUTH + WEST,
                                              EAST + EAST + NORTH, EAST + EAST + SOUTH, WEST + WEST + NORTH, WEST + WEST + SOUTH};

        for (int i = 0; i < 8; i++)
        {
            const int toPos = pos + directionIncrementals[i];
            if (getPieceAtPos(toPos) == Piece::NO_PIECE || getColorOfPiece(getPieceAtPos(toPos)) != color)
            {
                moves.push_back(Move{pos, toPos, piece});
            }
        }
        break;
    }
    case Piece::WHITE_QUEEN:
    case Piece::BLACK_QUEEN:
    /* Queen is the same as Bishop + Rook */
    case Piece::WHITE_BISHOP:
    case Piece::BLACK_BISHOP:
    {
        int const directionIncrementals[4] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST};

        for (int i = 0; i < 4; i++)
        {
            int tmpPos = pos + directionIncrementals[i];
            while (getPieceAtPos(tmpPos) == Piece::NO_PIECE)
            {
                moves.push_back(Move{pos, tmpPos, piece});
            }

            if (getPieceAtPos(tmpPos) != Piece::INVALID &&
                getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move{pos, tmpPos, piece});
            }
        }

        if (piece == Piece::WHITE_BISHOP || piece == Piece::BLACK_BISHOP)
        {
            break;
        }
    }
    case Piece::WHITE_ROOK:
    case Piece::BLACK_ROOK:
    {
        int const directionIncrementals[4] = {NORTH, EAST, WEST, SOUTH};

        for (int i = 0; i < 4; i++)
        {
            int tmpPos = pos + directionIncrementals[i];
            while (getPieceAtPos(tmpPos) == Piece::NO_PIECE)
            {
                moves.push_back(Move{pos, tmpPos, piece});
            }

            if (getPieceAtPos(tmpPos) != Piece::INVALID &&
                getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move{pos, tmpPos, piece});
            }
        }
        break;
    }
    case Piece::WHITE_KING:
        /* Castling King side */
        if (whiteCastlingKingside && getPieceAtPos(pos + 1) == Piece::NO_PIECE && getPieceAtPos(pos + 2) == Piece::NO_PIECE && getPieceAtPos(pos + 3) == Piece::WHITE_ROOK)
        {
            moves.push_back(Move{pos, pos + 2, piece});
        }

        /* Castling Queen side */
        if (whiteCastlingQueenside && getPieceAtPos(pos - 1) == Piece::NO_PIECE && getPieceAtPos(pos - 2) == Piece::NO_PIECE && getPieceAtPos(pos - 3) == Piece::NO_PIECE && getPieceAtPos(pos - 4) == Piece::WHITE_ROOK)
        {
            moves.push_back(Move{pos, pos - 2, piece});
        }
    case Piece::BLACK_KING:
    {
        int const directionIncrementals[8] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST, NORTH, EAST, WEST, SOUTH};

        for (int i = 0; i < 8; i++)
        {
            int const toPos = pos + directionIncrementals[i];
            if (getPieceAtPos(toPos) == Piece::NO_PIECE || (getPieceAtPos(toPos) != Piece::INVALID && getColorOfPiece(getPieceAtPos(toPos)) != color))
            {
                moves.push_back(Move{pos, toPos, piece});
            }
        }

        if (piece == Piece::BLACK_KING)
        {
            /* Castling King side */
            if (blackCastlingKingside && getPieceAtPos(pos + 1) == Piece::NO_PIECE && getPieceAtPos(pos + 2) == Piece::NO_PIECE && getPieceAtPos(pos + 3) == Piece::BLACK_ROOK)
            {
                moves.push_back(Move{pos, pos + 2, piece});
            }

            /* Castling Queen side */
            if (blackCastlingQueenside && getPieceAtPos(pos - 1) == Piece::NO_PIECE && getPieceAtPos(pos - 2) == Piece::NO_PIECE && getPieceAtPos(pos - 3) == Piece::NO_PIECE && getPieceAtPos(pos - 4) == Piece::BLACK_ROOK)
            {
                moves.push_back(Move{pos, pos - 2, piece});
            }
        }
        break;
    }
    }

    /* Check if move leaves own king attacked */
    std::vector<Move> legalMoves = {};
    for (Move const move : moves)
    {
        State stateCopy = State(*this);
        stateCopy.makeMove(move);
        if (!stateCopy.isKingInCheck(color))
        {
            legalMoves.push_back(move);
        }
    }

    return moves;
}

bool State::makeMove(Move const move)
{
    /* Move validation */
    std::vector<Move> const legalMoves = getLegalMovesForPos(move.from);
    if (std::find(legalMoves.begin(), legalMoves.end(), move) == legalMoves.end())
    {
        std::cerr << "Move is not legal" << std::endl;
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
    case Piece::WHITE_ROOK:
        if (move.from == 0)
        {
            whiteCastlingQueenside = false;
        }
        else if (move.from == BOARD_LENGTH - 1)
        {
            whiteCastlingKingside = false;
        }
        break;
    case Piece::BLACK_ROOK:
        if (move.from == BOARD_SIZE - BOARD_LENGTH)
        {
            blackCastlingQueenside = false;
        }
        else if (move.from == BOARD_SIZE - 1)
        {
            blackCastlingKingside = false;
        }
        break;
    case Piece::WHITE_PAWN:
        /* Promotion */
        if (move.to >= BOARD_SIZE - BOARD_LENGTH)
        {
            board[move.from] = Piece::NO_PIECE;
            board[move.to] = move.promotionPiece;
            passTurn();
            return true;
        }

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
        /* Promotion */
        if (move.to < BOARD_LENGTH)
        {
            board[move.from] = Piece::NO_PIECE;
            board[move.to] = move.promotionPiece;
            passTurn();
            return true;
        }

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
    passTurn();
    return true;
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