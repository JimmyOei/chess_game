/**
 * This file contains the perft console menu.
 */

#include "ui/perft_menu.h"

#include <charconv>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "core/game.h"
#include "core/perft.h"

namespace chess {

namespace {

// Depths above this take minutes or more with the current move generator.
constexpr int kMaxDepth = static_cast<int>(kStartPositionPerft.size());

// Reads a depth from 1 to kMaxDepth. Returns nullopt if input ends.
std::optional<int> ReadDepth() {
  while (true) {
    std::cout << ">> Input the depth (number of plies, i.e. half-moves) from 1 "
                 "to "
              << kMaxDepth << " (depth " << kMaxDepth
              << " takes several minutes)\n\n";
    std::string input;
    if (!std::getline(std::cin, input)) {
      return std::nullopt;
    }
    int depth = 0;
    auto const [end, error] =
        std::from_chars(input.data(), input.data() + input.size(), depth);
    if (error == std::errc() && end == input.data() + input.size() &&
        depth >= 1 && depth <= kMaxDepth) {
      return depth;
    }
    std::cout << "Invalid input, please try again\n\n";
  }
}

void PrintPositionCounts(int const depth) {
  std::cout << "\nCalculating possible positions from the starting position "
               "up to depth "
            << depth << "...\n\n";

  auto const start = std::chrono::steady_clock::now();
  const std::vector<PerftCounts> counts = PerftByDepth(Game(), depth);
  auto const end = std::chrono::steady_clock::now();

  std::cout << std::setw(5) << "Depth" << std::setw(14) << "Positions"
            << std::setw(14) << "Reference" << std::setw(12) << "Checkmates"
            << std::setw(12) << "Reference" << "\n";
  for (std::size_t i = 0; i < counts.size(); i++) {
    const PerftCounts &ours = counts[i];
    const PerftCounts &reference = kStartPositionPerft[i];
    std::cout << std::setw(5) << i + 1 << std::setw(14) << ours.nodes
              << std::setw(14) << reference.nodes << std::setw(12)
              << ours.checkmates << std::setw(12) << reference.checkmates
              << (ours == reference ? "  OK" : "  MISMATCH") << "\n";
  }
  std::cout << "\nTime taken: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << " ms\n"
            << "Reference counts: " << kPerftResultsUrl << "\n\n";
}

}  // namespace

void RunPerftMenu() {
  while (true) {
    std::cout << ">> What would you like to do? (Input a letter)\n"
              << "\"P\": Count the possible positions up to a given depth\n"
              << "\"C\": Close test menu\n\n";
    std::string input;
    if (!std::getline(std::cin, input)) {
      return;
    }
    std::cout << "\n";
    if (input == "P" || input == "p") {
      std::optional<int> const depth = ReadDepth();
      if (!depth) {
        return;
      }
      PrintPositionCounts(*depth);
    } else if (input == "C" || input == "c") {
      return;
    } else {
      std::cout << "Invalid input, please try again\n\n";
    }
  }
}

}  // namespace chess
