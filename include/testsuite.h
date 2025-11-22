#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <iostream>

#include "game.h"

class TestSuite
{
public:
  void menu();

private:
  void testPossiblePositions(Game game, int currentDepth, int totalDepth, std::unordered_map<int, int> &gameCounts, std::unordered_map<int, int> &checkmateCounts);
};

#endif