#include "../include/game.h"

#include <iostream>

void Game::initGame()
{
    turn = Piece::Color::WHITE;
    enPassantPos = 0;
    whiteCastlingQueenside = true;
    whiteCastlingKingside = true;
    blackCastlingQueenside = true;
    blackCastlingKingside = true;
    whiteKingPos = 0;
    blackKingPos = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board[i] = Piece::Type::NO_PIECE;
    }
}

Game::Game()
{
    if (!initGameFromFENString(STANDARD_OPENING_FEN))
    {
        std::cerr << ">> Invalid FENString-notation. Game is set to an empty board." << std::endl;
        initGame();
    }
}

Game::Game(std::string FENString)
{
    if (!initGameFromFENString(FENString))
    {
        std::cerr << ">> Invalid FENString-notation. Game is set to an empty board." << std::endl;
        initGame();
    }
}

Game::Game(Game const &game)
    : turn(game.turn),
      enPassantPos(game.enPassantPos),
      whiteCastlingQueenside(game.whiteCastlingQueenside),
      whiteCastlingKingside(game.whiteCastlingKingside),
      blackCastlingQueenside(game.blackCastlingQueenside),
      blackCastlingKingside(game.blackCastlingKingside),
      whiteKingPos(game.whiteKingPos),
      blackKingPos(game.blackKingPos)
{
    memcpy(board, game.board, BOARD_SIZE);
}

bool Game::isPosWithinBoardLimits(int const pos)
{
    return pos >= 0 && pos < BOARD_SIZE;
}

void Game::passTurn()
{
    if (turn == Piece::Color::WHITE)
    {
        turn = Piece::Color::BLACK;
    }
    else
    {
        turn = Piece::Color::WHITE;
    }
    enPassantPos = -1;
}

Piece::Color Game::getTurn()
{
    return turn;
}

int Game::getEnPassantPos()
{
    return enPassantPos;
}

bool Game::getCastlingKingSide(Piece::Color const color)
{
    return color == Piece::Color::WHITE ? whiteCastlingKingside : blackCastlingKingside;
}

bool Game::getCastlingQueenSide(Piece::Color const color)
{
    return color == Piece::Color::BLACK ? whiteCastlingQueenside : blackCastlingQueenside;
}

int Game::getKingPosOfPiece(Piece::Color const color)
{
    if (color == Piece::Color::WHITE)
    {
        return whiteKingPos;
    }
    return blackKingPos;
}

Piece::Type Game::getPieceAtPos(int const pos)
{
    if (!isPosWithinBoardLimits(pos))
    {
        return Piece::Type::INVALID;
    }

    return board[pos];
}

bool Game::isKingInCheck(Piece::Color const color)
{
    int const kingPos = getKingPosOfPiece(color);
    int const rookDirections[4] = {NORTH, EAST, WEST, SOUTH};
    int const bishopDirections[4] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST};
    int const knightDirections[8] = {NORTH + NORTH + EAST, NORTH + NORTH + WEST, SOUTH + SOUTH + EAST, SOUTH + SOUTH + WEST,
                                     EAST + EAST + NORTH, EAST + EAST + SOUTH, WEST + WEST + NORTH, WEST + WEST + SOUTH};

    /* Check if king is attacked by a rook */
    for (auto const direction : rookDirections)
    {
        int tmpPos = kingPos + direction;
        while (getPieceAtPos(tmpPos) == Piece::Type::NO_PIECE)
        {
            tmpPos += direction;
        }

        Piece::Type const pieceAtTmpPos = getPieceAtPos(tmpPos);
        if (pieceAtTmpPos != Piece::Type::INVALID &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(pieceAtTmpPos) == Piece::Type::ROOK)
        {
            return true;
        }
    }

    /* Check if king is attacked by a bishop */
    for (auto const direction : bishopDirections)
    {
        int tmpPos = kingPos + direction;
        while (getPieceAtPos(tmpPos) == Piece::Type::NO_PIECE)
        {
            tmpPos += direction;
        }

        Piece::Type const pieceAtTmpPos = getPieceAtPos(tmpPos);
        if (pieceAtTmpPos != Piece::Type::INVALID &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(pieceAtTmpPos) == Piece::Type::BISHOP)
        {
            return true;
        }
    }

    /* Check if king is attacked by a knight */
    for (auto const direction : knightDirections)
    {
        int const tmpPos = kingPos + direction;
        if (getPieceAtPos(tmpPos) != Piece::Type::INVALID &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(getPieceAtPos(tmpPos)) == Piece::Type::KNIGHT)
        {
            return true;
        }
    }

    /* Check if king is attacked by a pawn */
    if (color == Piece::Color::WHITE)
    {
        if (getPieceAtPos(kingPos + NORTH + EAST) == Piece::Type::BLACK_PAWN ||
            getPieceAtPos(kingPos + NORTH + WEST) == Piece::Type::BLACK_PAWN)
        {
            return true;
        }
    }
    else
    {
        if (getPieceAtPos(kingPos + SOUTH + EAST) == Piece::Type::WHITE_PAWN ||
            getPieceAtPos(kingPos + SOUTH + WEST) == Piece::Type::WHITE_PAWN)
        {
            return true;
        }
    }

    return false;
}

std::vector<Move> Game::getLegalMovesForPos(int const pos)
{
    if (!isPosWithinBoardLimits(pos))
    {
        return std::vector<Move>();
    }

    Piece::Type piece = board[pos];
    if (piece == Piece::Type::NO_PIECE)
    {
        return std::vector<Move>();
    }

    Piece::Color color = Piece::getColorOfPiece(piece);
    if (color != turn)
    {
        return std::vector<Move>();
    }

    if (isKingInCheck(color) && Piece::getPieceTypeWithoutColor(piece) != Piece::Type::KING)
    {
        return std::vector<Move>();
    }

    std::vector<Move> moves;
    switch (piece)
    {
    case Piece::Type::WHITE_PAWN:
        /* 1. One step upwards */
        if (getPieceAtPos(pos + BOARD_LENGTH) == Piece::Type::NO_PIECE)
        {
            /* 1.1 Promotion */
            if (pos + BOARD_LENGTH + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos + BOARD_LENGTH, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos + BOARD_LENGTH, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos + BOARD_LENGTH, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH, piece));

                /* 2. Two steps upwards */
                if (pos >= BOARD_LENGTH && pos < 2 * BOARD_LENGTH && getPieceAtPos(pos + 2 * BOARD_LENGTH) == Piece::Type::NO_PIECE)
                {
                    moves.push_back(Move(pos, pos + 2 * BOARD_LENGTH, piece));
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        if ((getPieceAtPos(pos + BOARD_LENGTH - 1) != Piece::Type::INVALID &&
             getPieceAtPos(pos + BOARD_LENGTH - 1) != Piece::Type::NO_PIECE && Piece::getColorOfPiece(getPieceAtPos(pos + BOARD_LENGTH - 1)) != color) ||
            pos + BOARD_LENGTH - 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos + BOARD_LENGTH - 1 + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH - 1, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos + BOARD_LENGTH - 1, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos + BOARD_LENGTH - 1, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos + BOARD_LENGTH - 1, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH - 1, piece));
            }
        }

        /* 4. Capture right diagonal or En Passant */
        if ((getPieceAtPos(pos + BOARD_LENGTH + 1) != Piece::Type::INVALID &&
             getPieceAtPos(pos + BOARD_LENGTH + 1) != Piece::Type::NO_PIECE &&
             Piece::getColorOfPiece(getPieceAtPos(pos + BOARD_LENGTH + 1)) != color) ||
            pos + BOARD_LENGTH + 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos + BOARD_LENGTH + 1 + BOARD_LENGTH >= BOARD_SIZE)
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH + 1, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos + BOARD_LENGTH + 1, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos + BOARD_LENGTH + 1, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos + BOARD_LENGTH + 1, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos + BOARD_LENGTH + 1, piece));
            }
        }
        break;
    case Piece::Type::BLACK_PAWN:
        /* 1. One step downards */
        if (getPieceAtPos(pos - BOARD_LENGTH) == Piece::Type::NO_PIECE)
        {
            /* 1.1 Promotion */
            if (pos - 2 * BOARD_LENGTH < 0)
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos - BOARD_LENGTH, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos - BOARD_LENGTH, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos - BOARD_LENGTH, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH, piece));

                /* 2. Two steps downwards */
                if (pos >= BOARD_SIZE - 2 * BOARD_LENGTH && pos < BOARD_SIZE - BOARD_LENGTH && getPieceAtPos(pos - 2 * BOARD_LENGTH) == Piece::Type::NO_PIECE)
                {
                    moves.push_back(Move(pos, pos - 2 * BOARD_LENGTH, piece));
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        if ((getPieceAtPos(pos - BOARD_LENGTH - 1) != Piece::Type::INVALID &&
             getPieceAtPos(pos - BOARD_LENGTH - 1) != Piece::Type::NO_PIECE && Piece::getColorOfPiece(getPieceAtPos(pos - BOARD_LENGTH - 1)) != color) ||
            pos - BOARD_LENGTH - 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos - BOARD_LENGTH - 1 - BOARD_LENGTH <= BOARD_SIZE)
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH - 1, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos - BOARD_LENGTH - 1, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos - BOARD_LENGTH - 1, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos - BOARD_LENGTH - 1, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH - 1, piece));
            }
        }

        /* 4. Capture right diagonal or En Passant */
        if ((getPieceAtPos(pos - BOARD_LENGTH + 1) != Piece::Type::INVALID &&
             getPieceAtPos(pos - BOARD_LENGTH + 1) != Piece::Type::NO_PIECE &&
             Piece::getColorOfPiece(getPieceAtPos(pos - BOARD_LENGTH + 1)) != color) ||
            pos - BOARD_LENGTH + 1 == enPassantPos)
        {
            /* Capture is a promotion */
            if (pos - BOARD_LENGTH + 1 - BOARD_LENGTH <= BOARD_SIZE)
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH + 1, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, pos - BOARD_LENGTH + 1, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, pos - BOARD_LENGTH + 1, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, pos - BOARD_LENGTH + 1, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, pos - BOARD_LENGTH + 1, piece));
            }
        }
        break;
    case Piece::Type::WHITE_KNIGHT:
    case Piece::Type::BLACK_KNIGHT:
    {
        int const directionIncrementals[8] = {NORTH + NORTH + EAST, NORTH + NORTH + WEST, SOUTH + SOUTH + EAST, SOUTH + SOUTH + WEST,
                                              EAST + EAST + NORTH, EAST + EAST + SOUTH, WEST + WEST + NORTH, WEST + WEST + SOUTH};

        for (int i = 0; i < 8; i++)
        {
            const int toPos = pos + directionIncrementals[i];
            if (getPieceAtPos(toPos) == Piece::Type::NO_PIECE || Piece::getColorOfPiece(getPieceAtPos(toPos)) != color)
            {
                moves.push_back(Move(pos, toPos, piece));
            }
        }
        break;
    }
    case Piece::Type::WHITE_QUEEN:
    case Piece::Type::BLACK_QUEEN:
    /* Queen is the same as Bishop + Rook */
    case Piece::Type::WHITE_BISHOP:
    case Piece::Type::BLACK_BISHOP:
    {
        int const directionIncrementals[4] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST};

        for (int i = 0; i < 4; i++)
        {
            int tmpPos = pos + directionIncrementals[i];
            while (getPieceAtPos(tmpPos) == Piece::Type::NO_PIECE)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }

            if (getPieceAtPos(tmpPos) != Piece::Type::INVALID &&
                Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        if (piece == Piece::Type::WHITE_BISHOP || piece == Piece::Type::BLACK_BISHOP)
        {
            break;
        }
    }
    case Piece::Type::WHITE_ROOK:
    case Piece::Type::BLACK_ROOK:
    {
        int const directionIncrementals[4] = {NORTH, EAST, WEST, SOUTH};

        for (int i = 0; i < 4; i++)
        {
            int tmpPos = pos + directionIncrementals[i];
            while (getPieceAtPos(tmpPos) == Piece::Type::NO_PIECE)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }

            if (getPieceAtPos(tmpPos) != Piece::Type::INVALID &&
                Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }
        break;
    }
    case Piece::Type::WHITE_KING:
        /* Castling King side */
        if (whiteCastlingKingside && getPieceAtPos(pos + 1) == Piece::Type::NO_PIECE && getPieceAtPos(pos + 2) == Piece::Type::NO_PIECE && getPieceAtPos(pos + 3) == Piece::Type::WHITE_ROOK)
        {
            moves.push_back(Move(pos, pos + 2, piece));
        }

        /* Castling Queen side */
        if (whiteCastlingQueenside && getPieceAtPos(pos - 1) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 2) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 3) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 4) == Piece::Type::WHITE_ROOK)
        {
            moves.push_back(Move(pos, pos - 2, piece));
        }
    case Piece::Type::BLACK_KING:
    {
        int const directionIncrementals[8] = {NORTH + EAST, NORTH + WEST, SOUTH + EAST, SOUTH + WEST, NORTH, EAST, WEST, SOUTH};

        for (int i = 0; i < 8; i++)
        {
            int const toPos = pos + directionIncrementals[i];
            if (getPieceAtPos(toPos) == Piece::Type::NO_PIECE || (getPieceAtPos(toPos) != Piece::Type::INVALID && Piece::getColorOfPiece(getPieceAtPos(toPos)) != color))
            {
                moves.push_back(Move(pos, toPos, piece));
            }
        }

        if (piece == Piece::Type::BLACK_KING)
        {
            /* Castling King side */
            if (blackCastlingKingside && getPieceAtPos(pos + 1) == Piece::Type::NO_PIECE && getPieceAtPos(pos + 2) == Piece::Type::NO_PIECE && getPieceAtPos(pos + 3) == Piece::Type::BLACK_ROOK)
            {
                moves.push_back(Move(pos, pos + 2, piece));
            }

            /* Castling Queen side */
            if (blackCastlingQueenside && getPieceAtPos(pos - 1) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 2) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 3) == Piece::Type::NO_PIECE && getPieceAtPos(pos - 4) == Piece::Type::BLACK_ROOK)
            {
                moves.push_back(Move(pos, pos - 2, piece));
            }
        }
        break;
    }
    default:
        throw std::runtime_error("Invalid piece type");
    }

    /* Check if move leaves own king attacked */
    std::vector<Move> legalMoves;
    for (Move const move : moves)
    {
        Game gameCopy = Game(*this);
        gameCopy.makeMove(move);
        if (!gameCopy.isKingInCheck(color))
        {
            legalMoves.push_back(move);
        }
    }

    return moves;
}

bool Game::makeMove(Move const move)
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
    case Piece::Type::WHITE_KING:
        if (whiteCastlingKingside || whiteCastlingQueenside)
        {
            if (move.to == move.from + 2)
            {
                /* Castling king side */
                board[move.to + 1] = Piece::Type::NO_PIECE;
                board[move.to - 1] = Piece::Type::WHITE_ROOK;
            }
            else if (move.to == move.from - 2)
            {
                /* Castling queen side */
                board[move.to - 2] = Piece::Type::NO_PIECE;
                board[move.to + 1] = Piece::Type::WHITE_ROOK;
            }
            whiteCastlingKingside = false;
            whiteCastlingQueenside = false;
        }
        whiteKingPos = move.to;
        break;
    case Piece::Type::BLACK_KING:
        if (blackCastlingKingside || blackCastlingQueenside)
        {
            if (move.to == move.from + 2)
            {
                /* Castling king side */
                board[move.to + 1] = Piece::Type::NO_PIECE;
                board[move.to - 1] = Piece::Type::BLACK_ROOK;
            }
            else if (move.to == move.from - 2)
            {
                /* Castling queen side */
                board[move.to - 2] = Piece::Type::NO_PIECE;
                board[move.to + 1] = Piece::Type::BLACK_ROOK;
            }
            blackCastlingKingside = false;
            blackCastlingQueenside = false;
        }
        blackKingPos = move.to;
        break;
    case Piece::Type::WHITE_ROOK:
        if (move.from == 0)
        {
            whiteCastlingQueenside = false;
        }
        else if (move.from == BOARD_LENGTH - 1)
        {
            whiteCastlingKingside = false;
        }
        break;
    case Piece::Type::BLACK_ROOK:
        if (move.from == BOARD_SIZE - BOARD_LENGTH)
        {
            blackCastlingQueenside = false;
        }
        else if (move.from == BOARD_SIZE - 1)
        {
            blackCastlingKingside = false;
        }
        break;
    case Piece::Type::WHITE_PAWN:
        /* Promotion */
        if (move.to >= BOARD_SIZE - BOARD_LENGTH)
        {
            board[move.from] = Piece::Type::NO_PIECE;
            board[move.to] = move.promotionPiece;
            passTurn();
            return true;
        }

        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to - BOARD_LENGTH] = Piece::Type::NO_PIECE;
        }
        else if (move.to == move.from + BOARD_LENGTH + BOARD_LENGTH)
        {
            enPassantPos = move.from + BOARD_LENGTH;
        }
        break;
    case Piece::Type::BLACK_PAWN:
        /* Promotion */
        if (move.to < BOARD_LENGTH)
        {
            board[move.from] = Piece::Type::NO_PIECE;
            board[move.to] = move.promotionPiece;
            passTurn();
            return true;
        }

        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to + BOARD_LENGTH] = Piece::Type::NO_PIECE;
        }
        else if (move.to == move.from - BOARD_LENGTH - BOARD_LENGTH)
        {
            enPassantPos = move.from - BOARD_LENGTH;
        }
        break;
    default:
        throw std::runtime_error("Invalid piece type");
    }

    board[move.from] = Piece::Type::NO_PIECE;
    board[move.to] = move.piece;
    passTurn();
    return true;
}

bool Game::initGameFromFENString(std::string FENString)
{
    initGame();
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
            board[pos] = Piece::Type::BLACK_PAWN;
            break;
        case 'P':
            board[pos] = Piece::Type::WHITE_PAWN;
            break;
        case 'n':
            board[pos] = Piece::Type::BLACK_KNIGHT;
            break;
        case 'N':
            board[pos] = Piece::Type::WHITE_KNIGHT;
            break;
        case 'b':
            board[pos] = Piece::Type::BLACK_BISHOP;
            break;
        case 'B':
            board[pos] = Piece::Type::WHITE_BISHOP;
            break;
        case 'r':
            board[pos] = Piece::Type::BLACK_ROOK;
            break;
        case 'R':
            board[pos] = Piece::Type::WHITE_ROOK;
            break;
        case 'q':
            board[pos] = Piece::Type::BLACK_QUEEN;
            break;
        case 'Q':
            board[pos] = Piece::Type::WHITE_QUEEN;
            break;
        case 'k':
            board[pos] = Piece::Type::BLACK_KING;
            blackKingPos = pos;
            break;
        case 'K':
            board[pos] = Piece::Type::WHITE_KING;
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
            turn = Piece::Color::WHITE;
            break;
        case 'b':
            turn = Piece::Color::BLACK;
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

void Game::printGame()
{
    std::cout << "--------===== PRINTING STATE =====--------" << std::endl
              << "Turn: " << (turn == Piece::Color::WHITE ? "White" : "Black") << std::endl
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
            case Piece::Type::WHITE_PAWN:
                std::cout << "P";
                break;
            case Piece::Type::BLACK_PAWN:
                std::cout << "p";
                break;
            case Piece::Type::WHITE_KNIGHT:
                std::cout << "N";
                break;
            case Piece::Type::BLACK_KNIGHT:
                std::cout << "n";
                break;
            case Piece::Type::WHITE_BISHOP:
                std::cout << "B";
                break;
            case Piece::Type::BLACK_BISHOP:
                std::cout << "b";
                break;
            case Piece::Type::WHITE_ROOK:
                std::cout << "R";
                break;
            case Piece::Type::BLACK_ROOK:
                std::cout << "r";
                break;
            case Piece::Type::WHITE_QUEEN:
                std::cout << "Q";
                break;
            case Piece::Type::BLACK_QUEEN:
                std::cout << "q";
                break;
            case Piece::Type::WHITE_KING:
                std::cout << "K";
                break;
            case Piece::Type::BLACK_KING:
                std::cout << "k";
                break;
            default:
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}