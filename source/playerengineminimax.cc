#include "../include/playerengineminimax.h"

PlayerEngineMiniMax::PlayerEngineMiniMax() : maxDepth(3) {};

PlayerEngineMiniMax::PlayerEngineMiniMax(int maxDepth) : maxDepth(maxDepth) {};

std::vector<Move> PlayerEngineMiniMax::getAllLegalMoves(Game game)
{
  std::vector<Move> allLegalMoves;
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    std::vector<Move> legalMoves = game.getLegalMovesForPos(i);
    if (legalMoves.size() > 0)
    {
      allLegalMoves.insert(allLegalMoves.end(), legalMoves.begin(), legalMoves.end());
    }
  }

  if(allLegalMoves.size() == 0)
  {
    logIt(LogLevel::ERROR) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }

  return allLegalMoves;
}

Move PlayerEngineMiniMax::getMove(Game game)
{
  logIt(LogLevel::INFO) << "Player Engine MiniMax is calculating a move";
  std::pair<Move, int> bestMove = miniMax(game, 0, INT_MIN, INT_MAX, game.getTurn() == Piece::Color::WHITE);
  if (bestMove.first.from == -1)
  {
    logIt(LogLevel::ERROR) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }
  logIt(LogLevel::INFO) << "Player Engine MiniMax made move " << bestMove.first << " with eval score " << bestMove.second;

  return bestMove.first;
}

int PlayerEngineMiniMax::evaluateGame(Game game)
{
  int score = 0;
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    Piece::Type piece = game.getPieceAtPos(i);
    if (piece == Piece::Type::BLANK)
    {
      continue;
    }

    switch (piece)
    {
    case Piece::Type::WHITE_PAWN:
      score += 1;
      break;
    case Piece::Type::WHITE_KNIGHT:
      score += 3;
      break;
    case Piece::Type::WHITE_BISHOP:
      score += 3;
      break;
    case Piece::Type::WHITE_ROOK:
      score += 5;
      break;
    case Piece::Type::WHITE_QUEEN:
      score += 9;
      break;
    case Piece::Type::BLACK_PAWN:
      score -= 1;
      break;
    case Piece::Type::BLACK_KNIGHT:
      score -= 3;
      break;
    case Piece::Type::BLACK_BISHOP:
      score -= 3;
      break;
    case Piece::Type::BLACK_ROOK:
      score -= 5;
      break;
    case Piece::Type::BLACK_QUEEN:
      score -= 9;
      break;
    default:
      break;
    }
  }
  if (game.isGameOver())
  {
    if (game.getTurn() == Piece::Color::WHITE)
    {
      score -= 1000;
    }
    else
    {
      score += 1000;
    }
  }
  return score;
}

std::pair<Move, int> PlayerEngineMiniMax::miniMax(Game game, int depth, int alpha, int beta, bool isMaximizingPlayer)
{
  if (depth == maxDepth || game.isGameOver())
  {
    return std::make_pair(Move(), evaluateGame(game));
  }

  std::vector<Move> allLegalMoves = getAllLegalMoves(game);

  if (isMaximizingPlayer)
  {
    int maxEval = INT_MIN;
    Move bestMove = Move();
    for (auto &move : allLegalMoves)
    {
      Game newGame = Game(game);
      newGame.makeMove(move);
      int eval = miniMax(newGame, depth + 1, alpha, beta, false).second;
      if (eval > maxEval)
      {
        maxEval = eval;
        bestMove = move;
      }
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
      {
        break;
      }
    }
    return std::make_pair(bestMove, maxEval);
  }
  else
  {
    int minEval = INT_MAX;
    Move bestMove = Move();
    for (auto &move : allLegalMoves)
    {
      Game newGame = Game(game);
      newGame.makeMove(move);
      int eval = miniMax(newGame, depth + 1, alpha, beta, true).second;
      if (eval < minEval)
      {
        minEval = eval;
        bestMove = move;
      }
      beta = std::min(beta, eval);
      if (beta <= alpha)
      {
        break;
      }
    }
    return std::make_pair(bestMove, minEval);
  }
}