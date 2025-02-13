#include "../include/playerengineminimax.h"

PlayerEngineMiniMax::PlayerEngineMiniMax() : maxDepth(DEFAULT_MAX_DEPTH) {};

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
  logIt(LogLevel::INFO) << "Current score: " << evaluateGame(game) << " turn: " << game.getTurn();
  std::pair<Move, int> bestMove = miniMax(game, 0, INT_MIN, INT_MAX, game.getTurn() == Piece::Color::WHITE);
  if (bestMove.first.from == -1)
  {
    logIt(LogLevel::ERROR) << "Engine has no legal moves to make";
    throw std::runtime_error("Engine has no legal moves to make");
  }
  logIt(LogLevel::INFO) << "Player Engine MiniMax made move " << bestMove.first << " with eval score " << bestMove.second;

  return bestMove.first;
}

int PlayerEngineMiniMax::evaluatePieceValue(Game game)
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
      score += 10;
      break;
    case Piece::Type::WHITE_KNIGHT:
      score += 30;
      break;
    case Piece::Type::WHITE_BISHOP:
      score += 30;
      break;
    case Piece::Type::WHITE_ROOK:
      score += 50;
      break;
    case Piece::Type::WHITE_QUEEN:
      score += 90;
      break;
    case Piece::Type::BLACK_PAWN:
      score -= 10;
      break;
    case Piece::Type::BLACK_KNIGHT:
      score -= 30;
      break;
    case Piece::Type::BLACK_BISHOP:
      score -= 30;
      break;
    case Piece::Type::BLACK_ROOK:
      score -= 50;
      break;
    case Piece::Type::BLACK_QUEEN:
      score -= 90;
      break;
    default:
      break;
    }
  }

  return score;
}

int PlayerEngineMiniMax::evaluatePieceMobility(Game game)
{
  int score = 0;
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    Piece::Type piece = game.getPieceAtPos(i);
    if (piece == Piece::Type::BLANK)
    {
      continue;
    }

    std::vector<Move> legalMoves = game.getLegalMovesForPos(i);
    if (legalMoves.size() == 0)
    {
      if (Piece::getColorOfPiece(piece) == Piece::Color::WHITE)
      {
        score += legalMoves.size();
      }
      else
      {
        score += legalMoves.size();
      }
    }
  }

  return score;
}

int PlayerEngineMiniMax::evaluatePieceDevelopment(Game game)
{
  if(game.getMoveCounter() > 15) {
    return 0;
  }

  int score = 0;
  if(game.getPieceAtPos(4) == Piece::Type::WHITE_KING) {
    score -= 5;
  }
  if(game.getPieceAtPos(60) == Piece::Type::BLACK_KING) {
    score += 5;
  }

  if(game.getPieceAtPos(1) == Piece::Type::WHITE_KNIGHT) {
    score -= 5;
  }
  if(game.getPieceAtPos(2) == Piece::Type::WHITE_BISHOP) {
    score -= 5;
  }
  if(game.getPieceAtPos(3) == Piece::Type::WHITE_QUEEN) {
    score -= 5;
  }
  if(game.getPieceAtPos(5) == Piece::Type::WHITE_BISHOP) {
    score -= 5;
  }
  if(game.getPieceAtPos(6) == Piece::Type::WHITE_KNIGHT) {
    score -= 5;
  }

  if(game.getPieceAtPos(57) == Piece::Type::BLACK_KNIGHT) {
    score += 5;
  }
  if(game.getPieceAtPos(58) == Piece::Type::BLACK_BISHOP) {
    score += 5;
  }
  if(game.getPieceAtPos(59) == Piece::Type::BLACK_QUEEN) {
    score += 5;
  }
  if(game.getPieceAtPos(61) == Piece::Type::BLACK_BISHOP) {
    score += 5;
  }
  if(game.getPieceAtPos(62) == Piece::Type::BLACK_KNIGHT) {
    score += 5;
  }

  return score;
}

int PlayerEngineMiniMax::evaluatePawnStructure(Game game) {
  int score = 0;

  /* Check double pawns */
  for (int i = 0; i < BOARD_LENGTH; i++)
  {
    int whitePawns = 0;
    int blackPawns = 0;
    for (int j = i; j < BOARD_SIZE; j += BOARD_LENGTH)
    {
      if (game.getPieceAtPos(Position(j)) == Piece::Type::WHITE_PAWN)
      {
        whitePawns++;
      }
      if (game.getPieceAtPos(Position(j)) == Piece::Type::BLACK_PAWN)
      {
        blackPawns++;
      }
    }
    if (whitePawns > 1)
    {
      score -= 5*(whitePawns-1);
    }
    if (blackPawns > 1)
    {
      score += 5*(blackPawns-1);
    }
  }

  return score;
}

int PlayerEngineMiniMax::evaluateGame(Game game)
{
  int score = 0;

  score += evaluatePieceValue(game);
  score += evaluatePieceMobility(game);
  score += evaluatePieceDevelopment(game);
  score += evaluatePawnStructure(game);

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