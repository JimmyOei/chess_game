#include "../include/testsuite.h"

void TestSuite::testPossiblePositions(Game game, int currentDepth, int totalDepth, std::unordered_map<int, int> &gameCounts, std::unordered_map<int, int> &checkmateCounts)
{
  std::vector<Move> allLegalMoves = game.getAllLegalMoves();
  gameCounts[currentDepth] += game.getAllLegalMoves().size();

  for (Move move : allLegalMoves)
  {
    Game newGame = Game(game);
    newGame.makeMove(move);
    if (newGame.isGameOver() && (newGame.getResult() == Result::WHITE_WIN || newGame.getResult() == Result::BLACK_WIN))
    {
      checkmateCounts[currentDepth]++;
    }

    else if (currentDepth != totalDepth)
    {
      testPossiblePositions(newGame, currentDepth + 1, totalDepth, gameCounts, checkmateCounts);
    }
  }
}

void TestSuite::menu()
{
  while (true)
  {
    std::string input;
    std::cout << ">> What would you like to do? (Input a letter)" << std::endl
              << "\"P\": Test number of possible positions in a given depth" << std::endl
              << "\"C\": Close test menu" << std::endl
              << std::endl
              << std::endl;
    getline(std::cin, input);
    std::cout << std::endl;
    if (input.length() == 1)
    {
      switch (input[0])
      {

      case 'P':
      case 'p':
      {
        int depth;
        while (true)
        {
          std::cout << ">> Input the depth (number of plies (half-moves)) to test the number of possible positions for" << std::endl
                    << std::endl;
          getline(std::cin, input);
          depth = std::stoi(input);
          if (depth > 0)
          {
            break;
          }
          std::cout << "Invalid input, please try again" << std::endl
                    << std::endl;
        }
        std::unordered_map<int, int> gameCounts;
        std::unordered_map<int, int> checkmateCounts;
        for (int i = 1; i <= depth; i++)
        {
          gameCounts[i] = 0;
          checkmateCounts[i] = 0;
        }
        std::cout << std::endl << "Calculating possible positions for depth " << depth << "..." << std::endl
                  << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        testPossiblePositions(Game(), 1, depth, gameCounts, checkmateCounts);
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "----------------------------------------------------------" << std::endl;
        for (int i = 1; i <= depth; i++)
        {
          std::cout << "Depth " << i << ": " << gameCounts[i] << " possible positions, of which " << checkmateCounts[i] << " are checkmates" << std::endl;
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl
                  << std::endl;
        break;
      }
      case 'C':
      case 'c':
        return;
      default:
        std::cout << "Invalid input, please try again" << std::endl
                  << std::endl;
      }
    }
    else
    {
      std::cout << "Invalid input, please try again" << std::endl
                << std::endl;
    }
  }
}