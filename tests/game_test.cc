// Rule tests for Game. The expectations follow the FIDE
// Laws of Chess, not the current implementation, so a failure is a rules bug.

#include "core/game.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "core/move.h"
#include "core/piece.h"
#include "core/position.h"

namespace chess {
namespace {

// Board index of a square in algebraic notation, e.g. Sq("e4"). Index 0 is a1
// and indices run along the rank first (b1 = 1, a2 = 8).
Position Sq(std::string_view square) {
  return Position::FromColumnRow(square[0] - 'a' + 1, square[1] - '1' + 1);
}

bool HasMove(const std::vector<Move>& moves, std::string_view from,
             std::string_view to, Piece promotion = Piece::kNone) {
  return std::ranges::any_of(moves, [&](const Move& m) {
    return m.from == Sq(from) && m.to == Sq(to) &&
           m.promotion_piece == promotion;
  });
}

std::vector<Move> MovesFrom(const Game& game, std::string_view from) {
  return game.GetLegalMovesForPos(Sq(from));
}

// Plays the move from -> to if it is among the legal moves. Use as
// ASSERT_TRUE(Play(...)) so a test stops at the first illegal move.
testing::AssertionResult Play(Game& game, std::string_view from,
                              std::string_view to,
                              Piece promotion = Piece::kNone) {
  const std::vector<Move> moves = game.GetAllLegalMoves();
  auto it = std::ranges::find_if(moves, [&](const Move& m) {
    return m.from == Sq(from) && m.to == Sq(to) &&
           m.promotion_piece == promotion;
  });
  if (it == moves.end()) {
    return testing::AssertionFailure() << from << to << " is not legal";
  }
  game.MakeMove(*it);
  return testing::AssertionSuccess();
}

// --- Start position -------------------------------------------------------

TEST(GameTest, StartPositionHasTwentyLegalMoves) {
  Game const game;
  EXPECT_EQ(game.GetAllLegalMoves().size(), 20u);
}

TEST(GameTest, MovesForTheSideNotToMove) {
  Game game;
  EXPECT_EQ(game.GetAllLegalMovesFor(Color::kBlack).size(), 20u);
  ASSERT_TRUE(Play(game, "e2", "e4"));
  EXPECT_EQ(game.GetAllLegalMovesFor(Color::kWhite).size(), 30u);
  EXPECT_EQ(game.GetAllLegalMovesFor(Color::kBlack).size(), 20u);
  EXPECT_EQ(game.GetTurn(), Color::kBlack);  // Unchanged.
}

TEST(GameTest, StartPositionFromFenHasTwentyLegalMoves) {
  Game const game(kStandardOpeningFen);
  EXPECT_EQ(game.GetAllLegalMoves().size(), 20u);
}

// --- FEN parsing ----------------------------------------------------------

TEST(GameFenTest, ParsesSideToMove) {
  Game const game("4k3/8/8/8/8/8/8/4K3 b - - 0 1");
  EXPECT_EQ(game.GetTurn(), Color::kBlack);
}

TEST(GameFenTest, ParsesPiecePlacement) {
  Game const game("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_EQ(game.GetPieceAtPos(Sq("a1")), Piece::kWhiteRook);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e1")), Piece::kWhiteKing);
  EXPECT_EQ(game.GetPieceAtPos(Sq("h1")), Piece::kWhiteRook);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e8")), Piece::kBlackKing);
  EXPECT_EQ(game.GetKingPos(Color::kWhite), Sq("e1"));
  EXPECT_EQ(game.GetKingPos(Color::kBlack), Sq("e8"));
}

TEST(GameFenTest, FromFenReturnsTheGameForAValidFen) {
  const std::optional<Game> game =
      Game::FromFen("4k3/8/8/8/8/8/8/R3K2R b K - 0 1");
  ASSERT_TRUE(game.has_value());
  const Game loaded = game.value_or(Game());
  EXPECT_EQ(loaded.GetTurn(), Color::kBlack);
  EXPECT_EQ(loaded.GetPieceAtPos(Sq("h1")), Piece::kWhiteRook);
  EXPECT_TRUE(loaded.GetCastlingKingSide(Color::kWhite));
  EXPECT_FALSE(loaded.GetCastlingQueenSide(Color::kWhite));
}

TEST(GameFenTest, FromFenRejectsInvalidFens) {
  for (std::string_view const fen : {
           "", "not a fen",
           "8/8/8/8/8/8/8/R7 w - - 0 1",      // No kings.
           "4k3/8/8/8/8/8/8/4K3",             // No side to move.
           "4k3/8/8/8/8/8/8/4K3 x - - 0 1",   // Bad side to move.
           "4k3/8/8/8/8/8/8/4K3 w KX - 0 1",  // Bad castling field.
           "4k3/8/8/8/8/8/8/4K3 w - e4 0 1",  // Bad en passant rank.
           "4k3/8/8/8/8/8/4K3 w - - 0 1",     // Seven ranks.
           "4k3/8/8/8/8/8/8/4K4 w - - 0 1",   // Nine files.
       }) {
    EXPECT_FALSE(Game::FromFen(fen).has_value()) << '"' << fen << '"';
  }
}

TEST(GameFenTest, RejectsFenWithoutKings) {
  // An invalid FEN leaves an empty board (see Game(std::string_view)).
  const Game game("8/8/8/8/8/8/8/R7 w - - 0 1");
  EXPECT_EQ(game.GetPieceAtPos(Sq("a1")), Piece::kNone);
}

TEST(GameFenTest, ParsesAllCastlingRights) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kWhite));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kWhite));
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameFenTest, ParsesNoCastlingRights) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kWhite));
  EXPECT_FALSE(game.GetCastlingQueenSide(Color::kWhite));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_FALSE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameFenTest, ParsesPartialCastlingRights) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R w Kq - 0 1");
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kWhite));
  EXPECT_FALSE(game.GetCastlingQueenSide(Color::kWhite));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameFenTest, ParsesMissingEnPassantSquare) {
  const Game game("4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1");
  EXPECT_FALSE(game.GetEnPassantPos().has_value());
}

TEST(GameFenTest, ParsesEnPassantSquare) {
  Game const game("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
  EXPECT_EQ(game.GetEnPassantPos(), Sq("d6"));
}

// --- Castling -------------------------------------------------------------

TEST(GameCastlingTest, WhiteCanCastleBothWays) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_TRUE(HasMove(moves, "e1", "g1"));
  EXPECT_TRUE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, BlackCanCastleBothWays) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e8");
  EXPECT_TRUE(HasMove(moves, "e8", "g8"));
  EXPECT_TRUE(HasMove(moves, "e8", "c8"));
}

TEST(GameCastlingTest, NoCastlingWithoutRights) {
  Game const game("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingWhenBlocked) {
  Game const game("r3k2r/8/8/8/8/8/8/RN2K1NR w KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingOutOfCheck) {
  Game const game("4k3/8/8/8/4r3/8/8/R3K2R w KQ - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingThroughAttackedSquare) {
  // The black rook on f8 attacks f1, which the king passes over.
  Game const game("4kr2/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_TRUE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, BlackNoCastlingThroughAttackedSquare) {
  // The white rook on d1 attacks d8, which the black king passes over.
  Game const game("r3k2r/8/8/8/8/8/8/3RK3 b kq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e8");
  EXPECT_FALSE(HasMove(moves, "e8", "c8"));
  EXPECT_TRUE(HasMove(moves, "e8", "g8"));
}

TEST(GameCastlingTest, NoCastlingIntoCheck) {
  Game const game("4k1r1/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e1"), "e1", "g1"));
}

TEST(GameCastlingTest, QueensideAllowedWhenOnlyB1IsAttacked) {
  // b1 is crossed by the rook, not the king, so an attack there is fine.
  Game const game("1r2k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_TRUE(HasMove(MovesFrom(game, "e1"), "e1", "c1"));
}

TEST(GameCastlingTest, KingsideCastlingMovesTheRook) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e1", "g1"));
  EXPECT_EQ(game.GetPieceAtPos(Sq("g1")), Piece::kWhiteKing);
  EXPECT_EQ(game.GetPieceAtPos(Sq("f1")), Piece::kWhiteRook);
  EXPECT_EQ(game.GetPieceAtPos(Sq("h1")), Piece::kNone);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e1")), Piece::kNone);
}

TEST(GameCastlingTest, QueensideCastlingMovesTheRook) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e8", "c8"));
  EXPECT_EQ(game.GetPieceAtPos(Sq("c8")), Piece::kBlackKing);
  EXPECT_EQ(game.GetPieceAtPos(Sq("d8")), Piece::kBlackRook);
  EXPECT_EQ(game.GetPieceAtPos(Sq("a8")), Piece::kNone);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e8")), Piece::kNone);
}

TEST(GameCastlingTest, KingMoveLosesBothRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e1", "f1"));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kWhite));
  EXPECT_FALSE(game.GetCastlingQueenSide(Color::kWhite));
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameCastlingTest, QueensideRookMoveLosesOnlyQueensideRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "a1", "a2"));
  EXPECT_FALSE(game.GetCastlingQueenSide(Color::kWhite));
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kWhite));
}

TEST(GameCastlingTest, KingsideRookMoveLosesOnlyKingsideRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  ASSERT_TRUE(Play(game, "h8", "h7"));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameCastlingTest, OtherRookOnTheSameFileKeepsTheRight) {
  // Only the rook on h1 matters for kingside castling, not one on h4.
  Game game("4k3/8/8/8/7R/8/8/R3K2R w KQ - 0 1");
  ASSERT_TRUE(Play(game, "h4", "g4"));
  EXPECT_TRUE(game.GetCastlingKingSide(Color::kWhite));
}

TEST(GameCastlingTest, CapturePromotionOnRookSquareLosesThatRight) {
  Game game("r3k2r/6P1/8/8/8/8/8/4K3 w kq - 0 1");
  ASSERT_TRUE(Play(game, "g7", "h8", Piece::kWhiteQueen));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
}

TEST(GameCastlingTest, CapturedRookLosesThatRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "h1", "h8"));
  EXPECT_FALSE(game.GetCastlingKingSide(Color::kBlack));
  EXPECT_TRUE(game.GetCastlingQueenSide(Color::kBlack));
  EXPECT_FALSE(HasMove(game.GetAllLegalMoves(), "e8", "g8"));
}

// --- En passant -----------------------------------------------------------

TEST(GameEnPassantTest, WhiteCapturesEnPassantFromFen) {
  Game game("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
  ASSERT_TRUE(Play(game, "e5", "d6"));
  EXPECT_EQ(game.GetPieceAtPos(Sq("d6")), Piece::kWhitePawn);
  EXPECT_EQ(game.GetPieceAtPos(Sq("d5")), Piece::kNone);
}

TEST(GameEnPassantTest, BlackCapturesEnPassantFromFen) {
  Game game("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1");
  ASSERT_TRUE(Play(game, "d4", "e3"));
  EXPECT_EQ(game.GetPieceAtPos(Sq("e3")), Piece::kBlackPawn);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e4")), Piece::kNone);
}

TEST(GameEnPassantTest, AvailableRightAfterDoublePush) {
  Game game("4k3/3p4/8/4P3/8/8/8/4K3 b - - 0 1");
  ASSERT_TRUE(Play(game, "d7", "d5"));
  EXPECT_EQ(game.GetEnPassantPos(), Sq("d6"));
  EXPECT_TRUE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

TEST(GameEnPassantTest, ExpiresAfterOneMove) {
  Game game("4k3/3p4/8/4P3/8/8/8/4K3 b - - 0 1");
  ASSERT_TRUE(Play(game, "d7", "d5"));
  ASSERT_TRUE(Play(game, "e1", "e2"));
  ASSERT_TRUE(Play(game, "e8", "e7"));
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
  EXPECT_FALSE(game.GetEnPassantPos().has_value());
}

TEST(GameEnPassantTest, NotAllowedWithoutFenSquare) {
  Game const game("4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

TEST(GameEnPassantTest, NoPhantomCaptureOntoA1WithoutFenSquare) {
  // "No en passant square" must not be stored as square 0 (a1), or the b2
  // pawn could capture "en passant" onto the empty a1.
  Game const game("4k3/8/8/8/8/8/1p6/4K3 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "b2");
  EXPECT_EQ(moves.size(), 4u);  // Only the four promotions on b1.
  EXPECT_FALSE(HasMove(moves, "b2", "a1", Piece::kBlackQueen));
}

TEST(GameEnPassantTest, IllegalWhenItExposesTheKingOnTheRank) {
  // Both pawns leave the 5th rank, opening it between the h5 rook and a5 king.
  Game const game("8/8/8/K2pP2r/8/8/8/4k3 w - d6 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

// --- Promotion ------------------------------------------------------------

TEST(GamePromotionTest, PushOffersAllFourPieces) {
  Game const game("k7/4P3/8/8/8/8/8/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e7");
  EXPECT_EQ(moves.size(), 4u);
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::kWhiteQueen));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::kWhiteRook));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::kWhiteBishop));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::kWhiteKnight));
}

TEST(GamePromotionTest, CapturesPromoteOnBothSides) {
  Game const game("k2r1r2/4P3/8/8/8/8/8/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e7");
  EXPECT_EQ(moves.size(), 12u);
  EXPECT_TRUE(HasMove(moves, "e7", "d8", Piece::kWhiteQueen));
  EXPECT_TRUE(HasMove(moves, "e7", "f8", Piece::kWhiteKnight));
}

TEST(GamePromotionTest, BlackPushOffersAllFourPieces) {
  Game const game("4k3/8/8/8/8/8/3p4/K7 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d2");
  EXPECT_EQ(moves.size(), 4u);
  EXPECT_TRUE(HasMove(moves, "d2", "d1", Piece::kBlackQueen));
  EXPECT_TRUE(HasMove(moves, "d2", "d1", Piece::kBlackKnight));
}

TEST(GamePromotionTest, BlackCapturesPromoteOnBothSides) {
  Game const game("7k/8/8/8/8/8/3p4/K1R1R3 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d2");
  EXPECT_EQ(moves.size(), 12u);
  EXPECT_TRUE(HasMove(moves, "d2", "c1", Piece::kBlackQueen));
  EXPECT_TRUE(HasMove(moves, "d2", "e1", Piece::kBlackRook));
}

TEST(GamePromotionTest, PromotedPieceReplacesThePawn) {
  Game game("k7/4P3/8/8/8/8/8/4K3 w - - 0 1");
  ASSERT_TRUE(Play(game, "e7", "e8", Piece::kWhiteKnight));
  EXPECT_EQ(game.GetPieceAtPos(Sq("e8")), Piece::kWhiteKnight);
  EXPECT_EQ(game.GetPieceAtPos(Sq("e7")), Piece::kNone);
}

// --- Pins and check -------------------------------------------------------

TEST(GamePinTest, AbsolutelyPinnedBishopCannotMove) {
  Game const game("4k3/4r3/8/8/8/8/4B3/4K3 w - - 0 1");
  EXPECT_TRUE(MovesFrom(game, "e2").empty());
}

TEST(GamePinTest, PinnedRookMovesOnlyAlongThePin) {
  Game const game("4k3/4r3/8/8/8/8/4R3/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e2");
  EXPECT_EQ(moves.size(), 5u);  // e3, e4, e5, e6 and capturing on e7.
  EXPECT_TRUE(HasMove(moves, "e2", "e7"));
  EXPECT_FALSE(HasMove(moves, "e2", "d2"));
}

TEST(GameCheckTest, DetectsCheck) {
  Game const game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  EXPECT_TRUE(game.IsKingInCheck(Color::kWhite));
  EXPECT_FALSE(game.IsKingInCheck(Color::kBlack));
}

TEST(GameCheckTest, KingCannotStepAlongTheCheckingLine) {
  // f1 is still attacked once the king leaves e1 (x-ray through the king).
  Game const game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  const std::vector<Move> moves = game.GetAllLegalMoves();
  EXPECT_EQ(moves.size(), 3u);
  EXPECT_TRUE(HasMove(moves, "e1", "d2"));
  EXPECT_TRUE(HasMove(moves, "e1", "e2"));
  EXPECT_TRUE(HasMove(moves, "e1", "f2"));
}

TEST(GameCheckTest, KnightCheckLeavesOnlyKingMoves) {
  // The d3 knight checks e1 and can't be blocked or taken: the king must go
  // to d1, d2, e2 or f1 (f2 is covered by the knight).
  Game const game("4k3/8/8/8/8/3n4/8/R3K3 w - - 0 1");
  EXPECT_TRUE(game.IsKingInCheck(Color::kWhite));
  const std::vector<Move> moves = game.GetAllLegalMoves();
  EXPECT_EQ(moves.size(), 4u);
  EXPECT_FALSE(HasMove(moves, "e1", "f2"));
}

struct CheckCase {
  std::string_view name;
  std::string_view fen;

  // Hidden friend that gtest finds through ADL to print the parameter.
  // [[maybe_unused]] because clangd skips gtest's template bodies, misses
  // that call and reports it as unused; the compilers see the use.
  [[maybe_unused]] friend void PrintTo(const CheckCase& c, std::ostream* os) {
    *os << c.name;
  }
};

class GameCheckDetectionTest : public testing::TestWithParam<CheckCase> {};

TEST_P(GameCheckDetectionTest, WhiteKingIsInCheck) {
  Game const game{GetParam().fen};
  EXPECT_TRUE(game.IsKingInCheck(Color::kWhite));
}

const auto kCheckCases = std::to_array<CheckCase>({
    {.name = "Pawn", .fen = "4k3/8/8/8/8/8/3p4/4K3 w - - 0 1"},
    {.name = "Knight", .fen = "4k3/8/8/8/8/5n2/8/4K3 w - - 0 1"},
    {.name = "Bishop", .fen = "4k3/8/8/8/1b6/8/8/4K3 w - - 0 1"},
    {.name = "Rook", .fen = "4k3/4r3/8/8/8/8/8/4K3 w - - 0 1"},
    {.name = "QueenDiagonal", .fen = "4k3/8/8/8/7q/8/8/4K3 w - - 0 1"},
    {.name = "QueenFile", .fen = "4k3/8/8/8/4q3/8/8/4K3 w - - 0 1"},
});

std::string CheckCaseName(const testing::TestParamInfo<CheckCase>& param_info) {
  return std::string(param_info.param.name);
}

INSTANTIATE_TEST_SUITE_P(EachPiece, GameCheckDetectionTest,
                         testing::ValuesIn(kCheckCases), CheckCaseName);

TEST(GameCheckTest, PawnDoesNotGiveCheckStraightAhead) {
  Game const game("4k3/8/8/8/8/8/4p3/4K3 w - - 0 1");
  EXPECT_FALSE(game.IsKingInCheck(Color::kWhite));
}

TEST(GameCheckTest, OnlyMovesThatResolveCheckAreLegal) {
  // Knight check from d3: the king must move or the knight must be taken.
  Game const game("4k3/8/8/8/8/3n4/8/R3K3 w - - 0 1");
  for (const Move& move : game.GetAllLegalMoves()) {
    Game child(game);
    child.MakeMove(move);
    EXPECT_FALSE(child.IsKingInCheck(Color::kWhite)) << move;
  }
}

TEST(GameCheckTest, KingsCannotStandNextToEachOther) {
  Game const game("8/8/8/3k4/8/3K4/8/8 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d3");
  EXPECT_FALSE(HasMove(moves, "d3", "d4"));
  EXPECT_FALSE(HasMove(moves, "d3", "c4"));
  EXPECT_FALSE(HasMove(moves, "d3", "e4"));
  EXPECT_EQ(moves.size(), 5u);
}

// --- Game end -------------------------------------------------------------

TEST(GameEndTest, FoolsMateIsAWinForBlack) {
  Game game("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
  EXPECT_TRUE(game.GetAllLegalMoves().empty());
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_EQ(game.GetResult(), Result::kBlackWin);
}

TEST(GameEndTest, BackRankMateIsAWinForWhite) {
  Game game("R5k1/5ppp/8/8/8/8/8/6K1 b - - 0 1");
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_EQ(game.GetResult(), Result::kWhiteWin);
}

TEST(GameEndTest, StalemateIsADraw) {
  Game game("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
  EXPECT_FALSE(game.IsKingInCheck(Color::kBlack));
  EXPECT_TRUE(game.GetAllLegalMoves().empty());
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_EQ(game.GetResult(), Result::kDraw);
}

TEST(GameEndTest, CheckWithAnEscapeIsNotGameOver) {
  Game game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetResult(), Result::kOngoing);
}

struct MaterialCase {
  std::string_view name;
  std::string_view fen;
  bool draw;

  // Hidden friend that gtest finds through ADL to print the parameter.
  // [[maybe_unused]] because clangd skips gtest's template bodies, misses
  // that call and reports it as unused; the compilers see the use.
  [[maybe_unused]] friend void PrintTo(const MaterialCase& c,
                                       std::ostream* os) {
    *os << c.name;
  }
};

class GameMaterialTest : public testing::TestWithParam<MaterialCase> {};

TEST_P(GameMaterialTest, DrawOnlyWhenNoMateIsPossible) {
  Game game{GetParam().fen};
  EXPECT_EQ(game.IsGameOver(), GetParam().draw);
  EXPECT_EQ(game.GetResult(),
            GetParam().draw ? Result::kDraw : Result::kOngoing);
}

const auto kMaterialCases = std::to_array<MaterialCase>({
    {.name = "KingVsKing",
     .fen = "4k3/8/8/8/8/8/8/4K3 w - - 0 1",
     .draw = true},
    {.name = "KingKnightVsKing",
     .fen = "4k3/8/8/8/8/8/8/4KN2 w - - 0 1",
     .draw = true},
    {.name = "KingVsKingBishop",
     .fen = "4kb2/8/8/8/8/8/8/4K3 w - - 0 1",
     .draw = true},
    // Bishops on the same square colour (c1 and f8 are both dark).
    {.name = "SameColourBishops",
     .fen = "4kb2/8/8/8/8/8/8/2B1K3 w - - 0 1",
     .draw = true},
    // g8 is light and c1 is dark: mate is still possible.
    {.name = "OppositeColourBishops",
     .fen = "4k1b1/8/8/8/8/8/8/2B1K3 w - - 0 1",
     .draw = false},
    {.name = "TwoKnightsVsKing",
     .fen = "4k3/8/8/8/8/8/8/1N2K1N1 w - - 0 1",
     .draw = false},
    {.name = "KingRookVsKing",
     .fen = "4k3/8/8/8/8/8/8/R3K3 w - - 0 1",
     .draw = false},
    {.name = "KingPawnVsKing",
     .fen = "4k3/8/8/8/8/8/4P3/4K3 w - - 0 1",
     .draw = false},
    {.name = "KingRookVsKingKnight",
     .fen = "4k1n1/8/8/8/8/8/8/R3K3 w - - 0 1",
     .draw = false},
    {.name = "KingQueenVsKingBishop",
     .fen = "4kb2/8/8/8/8/8/8/3QK3 w - - 0 1",
     .draw = false},
});

std::string MaterialCaseName(
    const testing::TestParamInfo<MaterialCase>& param_info) {
  return std::string(param_info.param.name);
}

INSTANTIATE_TEST_SUITE_P(InsufficientMaterial, GameMaterialTest,
                         testing::ValuesIn(kMaterialCases), MaterialCaseName);

}  // namespace
}  // namespace chess
