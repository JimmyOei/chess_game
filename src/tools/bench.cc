/**
 * This file contains chess_bench, a benchmark for the engine and move
 * generation on a fixed set of positions:
 *
 *   1. Search to a fixed depth: time (median of several runs) and nodes/s.
 *   2. Search with a time budget: the depth reached and nodes/s.
 *   3. Perft: move generation speed, checked against the reference counts.
 *      Perft counts leaf positions without playing the last move, so its
 *      leaves/s is not comparable with the search's nodes/s.
 *
 * Usage: chess_bench [--depth N] [--time MS] [--repeat R]
 * Build with the release preset; debug numbers are not meaningful.
 */

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <system_error>
#include <vector>

#include "core/game.h"
#include "core/perft.h"
#include "player/player_engine_minimax.h"

namespace chess {
namespace {

struct BenchPosition {
  std::string_view name;
  std::string_view fen;
};

// The start position, two common openings and two positions from the perft
// suite (https://www.chessprogramming.org/Perft_Results).
constexpr std::array<BenchPosition, 5> kPositions = {{
    {.name = "Start",
     .fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
    {.name = "Italian",
     .fen = "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - "
            "4 4"},
    {.name = "Kiwipete",
     .fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "
            "0 1"},
    {.name = "Middlegame",
     .fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - "
            "- 0 10"},
    {.name = "Endgame", .fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"},
}};

struct PerftBench {
  std::string_view name;
  std::string_view fen;
  int depth;
  uint64_t expected_nodes;
};

constexpr std::array<PerftBench, 2> kPerftBenches = {{
    {.name = "Start",
     .fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     .depth = 5,
     .expected_nodes = 4'865'609},
    {.name = "Kiwipete",
     .fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "
            "0 1",
     .depth = 4,
     .expected_nodes = 4'085'603},
}};

struct Options {
  int depth = 4;
  int time_ms =
      static_cast<int>(PlayerEngineMiniMax::kDefaultTimeLimit.count());
  int repeat = 3;
};

std::optional<int> ParseInt(std::string_view const text) {
  int value = 0;
  auto const [end, error] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (error != std::errc() || end != text.data() + text.size() || value < 1) {
    return std::nullopt;
  }
  return value;
}

std::optional<Options> ParseOptions(int const argc, char** const argv) {
  Options options;
  std::vector<std::string_view> const args(argv + 1, argv + argc);
  // Every option takes a value: --name value.
  for (std::size_t i = 0; i < args.size(); i += 2) {
    std::optional<int> const value =
        i + 1 < args.size() ? ParseInt(args[i + 1]) : std::nullopt;
    if (!value) {
      return std::nullopt;
    }
    if (args[i] == "--depth") {
      options.depth = *value;
    } else if (args[i] == "--time") {
      options.time_ms = *value;
    } else if (args[i] == "--repeat") {
      options.repeat = *value;
    } else {
      return std::nullopt;
    }
  }
  return options;
}

// Runs `engine` on `game` with the engine's log lines (on stderr) silenced,
// so they don't mix with the results.
PlayerEngineMiniMax::SearchStats Search(PlayerEngineMiniMax& engine,
                                        const Game& game) {
  std::ostringstream const discarded;
  std::streambuf* const stderr_buffer = std::cerr.rdbuf(discarded.rdbuf());
  (void)engine.GetMove(game);
  std::cerr.rdbuf(stderr_buffer);
  return engine.GetLastSearchStats();
}

uint64_t NodesPerSecond(uint64_t const nodes,
                        std::chrono::milliseconds const elapsed) {
  return elapsed.count() > 0
             ? nodes * 1000 / static_cast<uint64_t>(elapsed.count())
             : 0;
}

void BenchFixedDepth(const Options& options) {
  std::printf(
      "\n1. Search to depth %d (median of %d runs)\n"
      "%-12s %10s %12s %12s\n",
      options.depth, options.repeat, "Position", "Time (ms)", "Nodes",
      "Nodes/s");
  std::chrono::milliseconds total_time{0};
  uint64_t total_nodes = 0;
  for (const BenchPosition& position : kPositions) {
    const Game game(position.fen);
    std::vector<PlayerEngineMiniMax::SearchStats> runs;
    for (int run = 0; run < options.repeat; run++) {
      PlayerEngineMiniMax engine(options.depth);
      runs.push_back(Search(engine, game));
    }
    std::ranges::sort(runs, {}, &PlayerEngineMiniMax::SearchStats::elapsed);
    const PlayerEngineMiniMax::SearchStats& median = runs[runs.size() / 2];
    std::printf("%-12s %10lld %12llu %12llu\n", position.name.data(),
                static_cast<long long>(median.elapsed.count()),
                static_cast<unsigned long long>(median.nodes),
                static_cast<unsigned long long>(
                    NodesPerSecond(median.nodes, median.elapsed)));
    total_time += median.elapsed;
    total_nodes += median.nodes;
  }
  std::printf(
      "%-12s %10lld %12llu %12llu\n", "Total",
      static_cast<long long>(total_time.count()),
      static_cast<unsigned long long>(total_nodes),
      static_cast<unsigned long long>(NodesPerSecond(total_nodes, total_time)));
}

void BenchTimeLimit(const Options& options) {
  std::printf(
      "\n2. Search with a %d ms time limit\n"
      "%-12s %6s %12s %12s\n",
      options.time_ms, "Position", "Depth", "Nodes", "Nodes/s");
  std::chrono::milliseconds total_time{0};
  uint64_t total_nodes = 0;
  for (const BenchPosition& position : kPositions) {
    PlayerEngineMiniMax engine{std::chrono::milliseconds(options.time_ms)};
    const PlayerEngineMiniMax::SearchStats stats =
        Search(engine, Game(position.fen));
    std::printf("%-12s %6d %12llu %12llu\n", position.name.data(), stats.depth,
                static_cast<unsigned long long>(stats.nodes),
                static_cast<unsigned long long>(
                    NodesPerSecond(stats.nodes, stats.elapsed)));
    total_time += stats.elapsed;
    total_nodes += stats.nodes;
  }
  std::printf(
      "%-12s %6s %12llu %12llu\n", "Total", "",
      static_cast<unsigned long long>(total_nodes),
      static_cast<unsigned long long>(NodesPerSecond(total_nodes, total_time)));
}

// Returns false if a perft count doesn't match the reference.
bool BenchPerft() {
  std::printf(
      "\n3. Perft (move generation; leaves are counted without playing "
      "the last move)\n%-12s %6s %12s %10s %12s\n",
      "Position", "Depth", "Leaves", "Time (ms)", "Leaves/s");
  bool all_correct = true;
  for (const PerftBench& bench : kPerftBenches) {
    const Game game(bench.fen);
    auto const start = std::chrono::steady_clock::now();
    uint64_t const nodes = Perft(game, bench.depth);
    auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);
    bool const correct = nodes == bench.expected_nodes;
    all_correct = all_correct && correct;
    std::printf("%-12s %6d %12llu %10lld %12llu%s\n", bench.name.data(),
                bench.depth, static_cast<unsigned long long>(nodes),
                static_cast<long long>(elapsed.count()),
                static_cast<unsigned long long>(NodesPerSecond(nodes, elapsed)),
                correct ? "" : "  WRONG COUNT");
  }
  return all_correct;
}

}  // namespace
}  // namespace chess

int main(int argc, char* argv[]) {
  std::optional<chess::Options> const options = chess::ParseOptions(argc, argv);
  if (!options) {
    std::fprintf(stderr,
                 "Usage: chess_bench [--depth N] [--time MS] [--repeat R]\n");
    return 2;
  }
#ifndef NDEBUG
  std::printf(
      "Warning: this is not an optimised build, so the numbers say little. "
      "Use the release preset.\n");
#endif
  chess::BenchFixedDepth(*options);
  chess::BenchTimeLimit(*options);
  return chess::BenchPerft() ? 0 : 1;
}
