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
    result = Result::ONGOING;
    moveCounter = 1;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board[i] = Piece::Type::BLANK;
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
      blackKingPos(game.blackKingPos),
      moveCounter(game.moveCounter),
      result(game.result)
{
    std::copy(game.board, game.board + BOARD_SIZE, board);
}

void Game::passTurn(Position newEnPassantPos = -1)
{
    if (turn == Piece::Color::WHITE)
    {
        turn = Piece::Color::BLACK;
    }
    else
    {
        turn = Piece::Color::WHITE;
        moveCounter++;
    }
    enPassantPos = newEnPassantPos;
}

int Game::getMoveCounter()
{
    return moveCounter;
}

Result Game::getResult()
{
    return result;
}

Piece::Color Game::getTurn()
{
    return turn;
}

Position Game::getEnPassantPos()
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

Position Game::getKingPosOfPiece(Piece::Color const color)
{
    if (color == Piece::Color::WHITE)
    {
        return whiteKingPos;
    }
    return blackKingPos;
}

Piece::Type Game::getPieceAtPos(Position const pos)
{
    if (!pos.isValid())
    {
        throw std::out_of_range("Position is out of range");
    }

    return board[pos];
}

std::vector<std::pair<Piece::Type, Position>> Game::getAllPiecesForColor(Piece::Color const color)
{
    std::vector<std::pair<Piece::Type, Position>> pieces;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i] != Piece::Type::BLANK && Piece::getColorOfPiece(board[i]) == color)
        {
            pieces.push_back(std::make_pair(board[i], Position(i)));
        }
    }
    return pieces;
}

std::vector<Move> Game::getAllLegalMoves()
{
    std::vector<Move> allLegalMoves;
    std::vector<std::pair<Piece::Type, Position>> allPieces = getAllPiecesForColor(getTurn());

    for (auto [piece, pos] : allPieces)
    {
        std::vector<Move> legalMoves = getLegalMovesForPos(pos);
        if (legalMoves.size() > 0)
        {
            allLegalMoves.insert(allLegalMoves.end(), legalMoves.begin(), legalMoves.end());
        }
    }

    return allLegalMoves;
}

bool Game::isKingInCheck(Piece::Color const color)
{
    Position const kingPos = getKingPosOfPiece(color);

    /* Check if king is attacked by a rook */
    for (auto const cardinal : Direction::Cardinals)
    {
        Position prevPos = kingPos;
        Position tmpPos = kingPos + cardinal;
        while (tmpPos.isValid() && (prevPos.getRow() == tmpPos.getRow() || prevPos.getColumn() == tmpPos.getColumn()) && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
        {
            prevPos = tmpPos;
            tmpPos += cardinal;
        }

        if (tmpPos.isValid() && (prevPos.getRow() == tmpPos.getRow() || prevPos.getColumn() == tmpPos.getColumn()))
        {
            Piece::Type piece = getPieceAtPos(tmpPos);
            if (
                Piece::getColorOfPiece(piece) != color &&
                (Piece::getPieceTypeWithoutColor(piece) == Piece::Type::QUEEN ||
                 Piece::getPieceTypeWithoutColor(piece) == Piece::Type::ROOK))
            {
                return true;
            }
        }
    }

    /* Check if king is attacked by a bishop */
    for (auto const diagonal : Direction::Diagonals)
    {
        Position prevPos = kingPos;
        Position tmpPos = kingPos + diagonal;
        while (tmpPos.isValid() && abs(prevPos.getColumn() - tmpPos.getColumn()) == abs(prevPos.getRow() - tmpPos.getRow()) && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
        {
            prevPos = tmpPos;
            tmpPos += diagonal;
        }

        if (tmpPos.isValid() && abs(prevPos.getColumn() - tmpPos.getColumn()) == abs(prevPos.getRow() - tmpPos.getRow()))
        {
            Piece::Type piece = getPieceAtPos(tmpPos);
            if (
                Piece::getColorOfPiece(piece) != color &&
                (Piece::getPieceTypeWithoutColor(piece) == Piece::Type::QUEEN ||
                 Piece::getPieceTypeWithoutColor(piece) == Piece::Type::BISHOP))
            {
                return true;
            }
        }
    }

    /* Check if king is attacked by a knight */
    for (auto const knightMove : Direction::KnightMoves)
    {
        Position const tmpPos = kingPos + knightMove;
        if (tmpPos.isValid() &&
            abs(tmpPos.getColumn() - kingPos.getColumn()) <= 2 &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(getPieceAtPos(tmpPos)) == Piece::Type::KNIGHT)
        {
            return true;
        }
    }

    /* Check if king is attacked by a pawn */
    if (color == Piece::Color::WHITE)
    {
        if ((Position(kingPos + Direction::Diagonal::NORTH_EAST).isValid() && getPieceAtPos(kingPos + Direction::Diagonal::NORTH_EAST) == Piece::Type::BLACK_PAWN) ||
            (Position(kingPos + Direction::Diagonal::NORTH_WEST).isValid() && getPieceAtPos(kingPos + Direction::Diagonal::NORTH_WEST) == Piece::Type::BLACK_PAWN))
        {
            return true;
        }
    }
    else
    {
        if ((Position(kingPos + Direction::Diagonal::SOUTH_EAST).isValid() && getPieceAtPos(kingPos + Direction::Diagonal::SOUTH_EAST) == Piece::Type::WHITE_PAWN) ||
            (Position(kingPos + Direction::Diagonal::SOUTH_WEST).isValid() && getPieceAtPos(kingPos + Direction::Diagonal::SOUTH_WEST) == Piece::Type::WHITE_PAWN))
        {
            return true;
        }
    }

    /* Check if king is attacked by other king */
    for (auto const cardinal : Direction::Cardinals)
    {
        Position const tmpPos = kingPos + cardinal;
        if (tmpPos.isValid() &&
            (tmpPos.getRow() == kingPos.getRow() || tmpPos.getColumn() == kingPos.getColumn()) &&
            getPieceAtPos(tmpPos) != Piece::Type::BLANK &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(getPieceAtPos(tmpPos)) == Piece::Type::KING)
        {
            return true;
        }
    }
    for (auto const diagonal : Direction::Diagonals)
    {
        Position const tmpPos = kingPos + diagonal;
        if (tmpPos.isValid() &&
            abs(tmpPos.getColumn() - kingPos.getColumn()) == abs(tmpPos.getRow() - kingPos.getRow()) &&
            getPieceAtPos(tmpPos) != Piece::Type::BLANK &&
            Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color &&
            Piece::getPieceTypeWithoutColor(getPieceAtPos(tmpPos)) == Piece::Type::KING)
        {
            return true;
        }
    }

    return false;
}

std::vector<Move> Game::getLegalMovesForPos(Position const pos)
{
    if (!pos.isValid())
    {
        return std::vector<Move>();
    }

    Piece::Type piece = getPieceAtPos(pos);
    if (piece == Piece::Type::BLANK)
    {
        return std::vector<Move>();
    }

    Piece::Color color = Piece::getColorOfPiece(piece);
    if (color != turn)
    {
        return std::vector<Move>();
    }

    logIt(LogLevel::DEBUG) << "Getting legal moves for position " << pos << " " << piece;

    std::vector<Move> moves;
    switch (piece)
    {
    case Piece::Type::WHITE_PAWN:
    {
        /* 1. One step upwards */
        Position tmpPos = pos + Direction::Cardinal::NORTH;
        if (tmpPos.isValid() && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
        {
            /* 1.1 Promotion */
            if (tmpPos.getRow() == BOARD_LENGTH)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));

                /* 2. Two steps upwards */
                if (pos.getRow() == 2 && getPieceAtPos(tmpPos + Direction::Cardinal::NORTH) == Piece::Type::BLANK)
                {
                    moves.push_back(Move(pos, tmpPos + Direction::Cardinal::NORTH, piece));
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        tmpPos = pos + Direction::Diagonal::NORTH_WEST;
        if (tmpPos.isValid() &&
            abs(pos.getColumn() - tmpPos.getColumn()) == abs(pos.getRow() - tmpPos.getRow()) &&
            ((getPieceAtPos(tmpPos) != Piece::Type::BLANK && Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color) || tmpPos == enPassantPos))
        {
            /* Capture is a promotion */
            if (tmpPos.getRow() == BOARD_LENGTH)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        /* 4. Capture right diagonal or En Passant */
        tmpPos = pos + Direction::Diagonal::NORTH_EAST;
        if (tmpPos.isValid() &&
            abs(pos.getColumn() - tmpPos.getColumn()) == abs(pos.getRow() - tmpPos.getRow()) &&
            ((getPieceAtPos(tmpPos) != Piece::Type::BLANK && Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color) ||
             tmpPos == enPassantPos))
        {
            /* Capture is a promotion */
            if (tmpPos + Direction::Cardinal::NORTH >= BOARD_SIZE)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::WHITE_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }
        break;
    }
    case Piece::Type::BLACK_PAWN:
    {
        /* 1. One step downard */
        Position tmpPos = pos + Direction::Cardinal::SOUTH;
        if (tmpPos.isValid() && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
        {
            /* 1.1 Promotion */
            if (tmpPos.getRow() == 1)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));

                /* 2. Two steps downwards */
                if (pos.getRow() == BOARD_LENGTH - 1 && getPieceAtPos(tmpPos + Direction::Cardinal::SOUTH) == Piece::Type::BLANK)
                {
                    moves.push_back(Move(pos, tmpPos + Direction::Cardinal::SOUTH, piece));
                }
            }
        }

        /* 3. Capture left diagonal or En Passant */
        tmpPos = pos + Direction::Diagonal::SOUTH_WEST;
        if (tmpPos.isValid() &&
            abs(pos.getColumn() - tmpPos.getColumn()) == abs(pos.getRow() - tmpPos.getRow()) &&
            ((getPieceAtPos(tmpPos) != Piece::Type::BLANK && Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color) ||
             tmpPos == enPassantPos))
        {
            /* Capture is a promotion */
            if (tmpPos.getRow() == 1)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        /* 4. Capture right diagonal or En Passant */
        tmpPos = pos + Direction::Diagonal::SOUTH_EAST;
        if (tmpPos.isValid() &&
            abs(pos.getColumn() - tmpPos.getColumn()) == abs(pos.getRow() - tmpPos.getRow()) &&
            ((getPieceAtPos(tmpPos) != Piece::Type::BLANK && Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color) ||
             tmpPos == enPassantPos))
        {
            /* Capture is a promotion */
            if (tmpPos + Direction::Cardinal::SOUTH >= BOARD_SIZE)
            {
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_QUEEN));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_ROOK));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_BISHOP));
                moves.push_back(Move(pos, tmpPos, piece, Piece::Type::BLACK_KNIGHT));
            }
            else
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }
        break;
    }
    case Piece::Type::WHITE_KNIGHT:
    case Piece::Type::BLACK_KNIGHT:
    {
        for (auto &knightMove : Direction::KnightMoves)
        {
            const Position toPos = pos + knightMove;
            if (toPos.isValid() && abs(toPos.getColumn() - pos.getColumn()) <= 2 && abs(toPos.getRow() - pos.getRow()) <= 2 && (getPieceAtPos(toPos) == Piece::Type::BLANK || Piece::getColorOfPiece(getPieceAtPos(toPos)) != color))
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
        for (auto &diagonal : Direction::Diagonals)
        {
            Position tmpPos = pos + diagonal;
            if (abs(tmpPos.getColumn() - pos.getColumn()) != abs(tmpPos.getRow() - pos.getRow()))
            {
                continue;
            }
            while (tmpPos.isValid() && abs(tmpPos.getColumn() - Position(tmpPos + diagonal).getColumn()) == abs(tmpPos.getRow() - Position(tmpPos + diagonal).getRow()) && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
            {
                moves.push_back(Move(pos, tmpPos, piece));
                tmpPos += diagonal;
            }

            if (tmpPos.isValid() &&
                Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        if (Piece::getPieceTypeWithoutColor(piece) == Piece::Type::BISHOP)
        {
            break;
        }
    }
    case Piece::Type::WHITE_ROOK:
    case Piece::Type::BLACK_ROOK:
    {
        for (auto &cardinal : Direction::Cardinals)
        {
            Position tmpPos = pos + cardinal;
            if ((tmpPos.getRow() != pos.getRow() && tmpPos.getColumn() != pos.getColumn()))
            {
                continue;
            }
            while (tmpPos.isValid() && (tmpPos.getRow() == Position(tmpPos + cardinal).getRow() || tmpPos.getColumn() == Position(tmpPos + cardinal).getColumn()) && getPieceAtPos(tmpPos) == Piece::Type::BLANK)
            {
                moves.push_back(Move(pos, tmpPos, piece));
                tmpPos += cardinal;
            }

            if (tmpPos.isValid() &&
                Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color)
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        break;
    }
    case Piece::Type::WHITE_KING:
    {
        if (!isKingInCheck(color))
        {
            /* Castling King side */
            if (whiteCastlingKingside && getPieceAtPos(pos + 1) == Piece::Type::BLANK && getPieceAtPos(pos + 2) == Piece::Type::BLANK && getPieceAtPos(pos + 3) == Piece::Type::WHITE_ROOK)
            {
                moves.push_back(Move(pos, pos + 2, piece));
            }

            /* Castling Queen side */
            if (whiteCastlingQueenside && getPieceAtPos(pos - 1) == Piece::Type::BLANK && getPieceAtPos(pos - 2) == Piece::Type::BLANK && getPieceAtPos(pos - 3) == Piece::Type::BLANK && getPieceAtPos(pos - 4) == Piece::Type::WHITE_ROOK)
            {
                moves.push_back(Move(pos, pos - 2, piece));
            }
        }
    }
    case Piece::Type::BLACK_KING:
    {
        for (auto &cardinal : Direction::Cardinals)
        {
            Position tmpPos = pos + cardinal;
            if (tmpPos.isValid() && (tmpPos.getRow() == pos.getRow() || tmpPos.getColumn() == pos.getColumn()) && (getPieceAtPos(tmpPos) == Piece::Type::BLANK || Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color))
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        for (auto &diagonal : Direction::Diagonals)
        {
            Position tmpPos = pos + diagonal;
            if (tmpPos.isValid() && abs(tmpPos.getColumn() - pos.getColumn()) == abs(tmpPos.getRow() - pos.getRow()) && (getPieceAtPos(tmpPos) == Piece::Type::BLANK || Piece::getColorOfPiece(getPieceAtPos(tmpPos)) != color))
            {
                moves.push_back(Move(pos, tmpPos, piece));
            }
        }

        if (piece == Piece::Type::BLACK_KING && !isKingInCheck(color))
        {
            /* Castling King side */
            if (blackCastlingKingside && getPieceAtPos(pos + Direction::Cardinal::EAST) == Piece::Type::BLANK && getPieceAtPos(pos + 2 * Direction::Cardinal::EAST) == Piece::Type::BLANK && getPieceAtPos(pos + 3 * Direction::Cardinal::EAST) == Piece::Type::BLACK_ROOK)
            {
                moves.push_back(Move(pos, pos + 2 * Direction::Cardinal::EAST, piece));
            }

            /* Castling Queen side */
            if (blackCastlingQueenside && getPieceAtPos(pos + Direction::Cardinal::WEST) == Piece::Type::BLANK && getPieceAtPos(pos + 2 * Direction::Cardinal::WEST) == Piece::Type::BLANK && getPieceAtPos(pos + 3 * Direction::Cardinal::WEST) == Piece::Type::BLANK && getPieceAtPos(pos + 4 * Direction::Cardinal::WEST) == Piece::Type::BLACK_ROOK)
            {
                moves.push_back(Move(pos, pos + 2 * Direction::Cardinal::WEST, piece));
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

    return legalMoves;
}

void Game::makeMove(Move const move)
{
    Position newEnPassantPos = -1;
    switch (move.piece)
    {
    case Piece::Type::WHITE_KING:
        if (whiteCastlingKingside || whiteCastlingQueenside)
        {
            if (move.to == move.from + 2 * Direction::Cardinal::EAST)
            {
                /* Castling king side */
                board[move.to + Direction::Cardinal::EAST] = Piece::Type::BLANK;
                board[move.to + Direction::Cardinal::WEST] = Piece::Type::WHITE_ROOK;
            }
            else if (move.to == move.from + 2 * Direction::Cardinal::WEST)
            {
                /* Castling queen side */
                board[move.to + 2 * Direction::Cardinal::WEST] = Piece::Type::BLANK;
                board[move.to + Direction::Cardinal::EAST] = Piece::Type::WHITE_ROOK;
            }
            whiteCastlingKingside = false;
            whiteCastlingQueenside = false;
        }
        whiteKingPos = move.to;
        break;
    case Piece::Type::BLACK_KING:
        if (blackCastlingKingside || blackCastlingQueenside)
        {
            if (move.to == move.from + 2 * Direction::Cardinal::EAST)
            {
                /* Castling king side */
                board[move.to + Direction::Cardinal::EAST] = Piece::Type::BLANK;
                board[move.to + Direction::Cardinal::WEST] = Piece::Type::BLACK_ROOK;
            }
            else if (move.to == move.from - 2)
            {
                /* Castling queen side */
                board[move.to + 2 * Direction::Cardinal::WEST] = Piece::Type::BLANK;
                board[move.to + Direction::Cardinal::EAST] = Piece::Type::BLACK_ROOK;
            }
            blackCastlingKingside = false;
            blackCastlingQueenside = false;
        }
        blackKingPos = move.to;
        break;
    case Piece::Type::WHITE_ROOK:
        if (move.from.getColumn() == 0)
        {
            whiteCastlingQueenside = false;
        }
        else if (move.from.getColumn() == BOARD_LENGTH)
        {
            whiteCastlingKingside = false;
        }
        break;
    case Piece::Type::BLACK_ROOK:
        if (move.from.getColumn() == 0)
        {
            blackCastlingQueenside = false;
        }
        else if (move.from.getColumn() == BOARD_LENGTH)
        {
            blackCastlingKingside = false;
        }
        break;
    case Piece::Type::WHITE_PAWN:
        /* Promotion */
        if (move.to.getRow() == BOARD_LENGTH)
        {
            board[move.from] = Piece::Type::BLANK;
            board[move.to] = move.promotionPiece;
            passTurn();
            return;
        }

        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to + Direction::Cardinal::SOUTH] = Piece::Type::BLANK;
        }
        else if (move.to == move.from + 2 * Direction::Cardinal::NORTH)
        {
            newEnPassantPos = move.from + Direction::Cardinal::NORTH;
        }
        break;
    case Piece::Type::BLACK_PAWN:
        /* Promotion */
        if (move.to.getRow() == 1)
        {
            board[move.from] = Piece::Type::BLANK;
            board[move.to] = move.promotionPiece;
            passTurn();
            return;
        }

        if (move.to == enPassantPos)
        {
            // Capturing the en passant piece
            board[move.to + Direction::Cardinal::NORTH] = Piece::Type::BLANK;
        }
        else if (move.to == move.from + 2 * Direction::Cardinal::SOUTH)
        {
            newEnPassantPos = move.from + Direction::Cardinal::SOUTH;
        }
        break;
    default:
        break;
    }

    board[move.from] = Piece::Type::BLANK;
    board[move.to] = move.piece;
    passTurn(newEnPassantPos);
    return;
}

bool Game::isGameOver()
{
    if (result != Result::ONGOING)
    {
        return true;
    }

    bool isCheck = isKingInCheck(turn);
    if (isKingInCheck(turn))
    {
        std::vector<Move> legalMoves;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (Piece::getColorOfPiece(board[i]) == turn && board[i] != Piece::Type::BLANK)
            {
                std::vector<Move> moves = getLegalMovesForPos(Position(i));
                if (!moves.empty())
                {
                    return false;
                }
            }
        }
        result = turn == Piece::Color::WHITE ? Result::BLACK_WIN : Result::WHITE_WIN;
        return true;
    }

    /* Draws */
    std::vector<std::pair<Piece::Type, Position>> whitePieces = getAllPiecesForColor(Piece::Color::WHITE);
    std::vector<std::pair<Piece::Type, Position>> blackPieces = getAllPiecesForColor(Piece::Color::BLACK);
    if (whitePieces.size() == 1 && blackPieces.size() == 1)
    {
        result = Result::DRAW;
        return true;
    }
    else if (whitePieces.size() == 1 && blackPieces.size() == 2)
    {
        if (blackPieces[0].first == Piece::Type::BLACK_KNIGHT || blackPieces[1].first == Piece::Type::BLACK_KNIGHT || blackPieces[0].first == Piece::Type::BLACK_BISHOP || blackPieces[1].first == Piece::Type::BLACK_BISHOP)
        {
            result = Result::DRAW;
            return true;
        }
    }
    else if (whitePieces.size() == 2 && blackPieces.size() == 1)
    {
        if (whitePieces[0].first == Piece::Type::WHITE_KNIGHT || whitePieces[1].first == Piece::Type::WHITE_KNIGHT || whitePieces[0].first == Piece::Type::WHITE_BISHOP || whitePieces[1].first == Piece::Type::WHITE_BISHOP)
        {
            result = Result::DRAW;
            return true;
        }
    }
    else if (whitePieces.size() == 2 && blackPieces.size() == 2)
    {
        if (((whitePieces[0].first == Piece::Type::WHITE_BISHOP || blackPieces[1].first == Piece::Type::BLACK_BISHOP) ||
             (whitePieces[0].first == Piece::Type::WHITE_KNIGHT || blackPieces[1].first == Piece::Type::BLACK_KNIGHT)) &&
            ((blackPieces[0].first == Piece::Type::BLACK_BISHOP || blackPieces[1].first == Piece::Type::BLACK_BISHOP) ||
             (blackPieces[0].first == Piece::Type::BLACK_KNIGHT || blackPieces[1].first == Piece::Type::BLACK_KNIGHT)))
        {
            result = Result::DRAW;
            return true;
        }
    }

    return false;
}

bool Game::initGameFromFENString(std::string FENString)
{
    logIt(LogLevel::DEBUG) << "Initializing game from FENString: " << FENString;
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