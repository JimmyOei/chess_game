// Rule tests for Game. The expectations follow the FIDE
// Laws of Chess, not the current implementation, so a failure is a rules bug.

#include "game.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "move.h"
#include "piece.h"
#include "position.h"

namespace {

// Board index of a square in algebraic notation, e.g. Sq("e4"). Index 0 is a1
// and indices run along the rank first (b1 = 1, a2 = 8).
Position Sq(std::string_view square) {
  return {(square[1] - '1') * BOARD_LENGTH + (square[0] - 'a')};
}

bool HasMove(const std::vector<Move>& moves, std::string_view from,
             std::string_view to, Piece::Type promotion = Piece::Type::BLANK) {
  return std::ranges::any_of(moves, [&](const Move& m) {
    return m.from == Sq(from) && m.to == Sq(to) &&
           m.promotionPiece == promotion;
  });
}

std::vector<Move> MovesFrom(Game& game, std::string_view from) {
  return game.getLegalMovesForPos(Sq(from));
}

// Plays the move from -> to if it is among the legal moves. Use as
// ASSERT_TRUE(Play(...)) so a test stops at the first illegal move.
testing::AssertionResult Play(Game& game, std::string_view from,
                              std::string_view to,
                              Piece::Type promotion = Piece::Type::BLANK) {
  const std::vector<Move> moves = game.getAllLegalMoves();
  auto it = std::ranges::find_if(moves, [&](const Move& m) {
    return m.from == Sq(from) && m.to == Sq(to) &&
           m.promotionPiece == promotion;
  });
  if (it == moves.end()) {
    return testing::AssertionFailure() << from << to << " is not legal";
  }
  game.makeMove(*it);
  return testing::AssertionSuccess();
}

// --- Start position -------------------------------------------------------

TEST(GameTest, StartPositionHasTwentyLegalMoves) {
  Game game;
  EXPECT_EQ(game.getAllLegalMoves().size(), 20u);
}

TEST(GameTest, StartPositionFromFenHasTwentyLegalMoves) {
  Game game(STANDARD_OPENING_FEN);
  EXPECT_EQ(game.getAllLegalMoves().size(), 20u);
}

// --- FEN parsing ----------------------------------------------------------

TEST(GameFenTest, ParsesSideToMove) {
  Game game("4k3/8/8/8/8/8/8/4K3 b - - 0 1");
  EXPECT_EQ(game.getTurn(), Piece::Color::BLACK);
}

TEST(GameFenTest, ParsesPiecePlacement) {
  Game game("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_EQ(game.getPieceAtPos(Sq("a1")), Piece::Type::WHITE_ROOK);
  EXPECT_EQ(game.getPieceAtPos(Sq("e1")), Piece::Type::WHITE_KING);
  EXPECT_EQ(game.getPieceAtPos(Sq("h1")), Piece::Type::WHITE_ROOK);
  EXPECT_EQ(game.getPieceAtPos(Sq("e8")), Piece::Type::BLACK_KING);
  EXPECT_EQ(game.getKingPosOfPiece(Piece::Color::WHITE), Sq("e1"));
  EXPECT_EQ(game.getKingPosOfPiece(Piece::Color::BLACK), Sq("e8"));
}

TEST(GameFenTest, ParsesAllCastlingRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::WHITE));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::WHITE));
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameFenTest, ParsesNoCastlingRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::WHITE));
  EXPECT_FALSE(game.getCastlingQueenSide(Piece::Color::WHITE));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_FALSE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameFenTest, ParsesPartialCastlingRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w Kq - 0 1");
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::WHITE));
  EXPECT_FALSE(game.getCastlingQueenSide(Piece::Color::WHITE));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameFenTest, ParsesEnPassantSquare) {
  Game game("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
  EXPECT_EQ(game.getEnPassantPos(), Sq("d6"));
}

// --- Castling -------------------------------------------------------------

TEST(GameCastlingTest, WhiteCanCastleBothWays) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_TRUE(HasMove(moves, "e1", "g1"));
  EXPECT_TRUE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, BlackCanCastleBothWays) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e8");
  EXPECT_TRUE(HasMove(moves, "e8", "g8"));
  EXPECT_TRUE(HasMove(moves, "e8", "c8"));
}

TEST(GameCastlingTest, NoCastlingWithoutRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingWhenBlocked) {
  Game game("r3k2r/8/8/8/8/8/8/RN2K1NR w KQkq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingOutOfCheck) {
  Game game("4k3/8/8/8/4r3/8/8/R3K2R w KQ - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_FALSE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, NoCastlingThroughAttackedSquare) {
  // The black rook on f8 attacks f1, which the king passes over.
  Game game("4kr2/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e1");
  EXPECT_FALSE(HasMove(moves, "e1", "g1"));
  EXPECT_TRUE(HasMove(moves, "e1", "c1"));
}

TEST(GameCastlingTest, BlackNoCastlingThroughAttackedSquare) {
  // The white rook on d1 attacks d8, which the black king passes over.
  Game game("r3k2r/8/8/8/8/8/8/3RK3 b kq - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e8");
  EXPECT_FALSE(HasMove(moves, "e8", "c8"));
  EXPECT_TRUE(HasMove(moves, "e8", "g8"));
}

TEST(GameCastlingTest, NoCastlingIntoCheck) {
  Game game("4k1r1/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e1"), "e1", "g1"));
}

TEST(GameCastlingTest, QueensideAllowedWhenOnlyB1IsAttacked) {
  // b1 is crossed by the rook, not the king, so an attack there is fine.
  Game game("1r2k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
  EXPECT_TRUE(HasMove(MovesFrom(game, "e1"), "e1", "c1"));
}

TEST(GameCastlingTest, KingsideCastlingMovesTheRook) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e1", "g1"));
  EXPECT_EQ(game.getPieceAtPos(Sq("g1")), Piece::Type::WHITE_KING);
  EXPECT_EQ(game.getPieceAtPos(Sq("f1")), Piece::Type::WHITE_ROOK);
  EXPECT_EQ(game.getPieceAtPos(Sq("h1")), Piece::Type::BLANK);
  EXPECT_EQ(game.getPieceAtPos(Sq("e1")), Piece::Type::BLANK);
}

TEST(GameCastlingTest, QueensideCastlingMovesTheRook) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e8", "c8"));
  EXPECT_EQ(game.getPieceAtPos(Sq("c8")), Piece::Type::BLACK_KING);
  EXPECT_EQ(game.getPieceAtPos(Sq("d8")), Piece::Type::BLACK_ROOK);
  EXPECT_EQ(game.getPieceAtPos(Sq("a8")), Piece::Type::BLANK);
  EXPECT_EQ(game.getPieceAtPos(Sq("e8")), Piece::Type::BLANK);
}

TEST(GameCastlingTest, KingMoveLosesBothRights) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "e1", "f1"));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::WHITE));
  EXPECT_FALSE(game.getCastlingQueenSide(Piece::Color::WHITE));
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameCastlingTest, QueensideRookMoveLosesOnlyQueensideRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "a1", "a2"));
  EXPECT_FALSE(game.getCastlingQueenSide(Piece::Color::WHITE));
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::WHITE));
}

TEST(GameCastlingTest, KingsideRookMoveLosesOnlyKingsideRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
  ASSERT_TRUE(Play(game, "h8", "h7"));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameCastlingTest, OtherRookOnTheSameFileKeepsTheRight) {
  // Only the rook on h1 matters for kingside castling, not one on h4.
  Game game("4k3/8/8/8/7R/8/8/R3K2R w KQ - 0 1");
  ASSERT_TRUE(Play(game, "h4", "g4"));
  EXPECT_TRUE(game.getCastlingKingSide(Piece::Color::WHITE));
}

TEST(GameCastlingTest, CapturePromotionOnRookSquareLosesThatRight) {
  Game game("r3k2r/6P1/8/8/8/8/8/4K3 w kq - 0 1");
  ASSERT_TRUE(Play(game, "g7", "h8", Piece::Type::WHITE_QUEEN));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
}

TEST(GameCastlingTest, CapturedRookLosesThatRight) {
  Game game("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(Play(game, "h1", "h8"));
  EXPECT_FALSE(game.getCastlingKingSide(Piece::Color::BLACK));
  EXPECT_TRUE(game.getCastlingQueenSide(Piece::Color::BLACK));
  EXPECT_FALSE(HasMove(game.getAllLegalMoves(), "e8", "g8"));
}

// --- En passant -----------------------------------------------------------

TEST(GameEnPassantTest, WhiteCapturesEnPassantFromFen) {
  Game game("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
  ASSERT_TRUE(Play(game, "e5", "d6"));
  EXPECT_EQ(game.getPieceAtPos(Sq("d6")), Piece::Type::WHITE_PAWN);
  EXPECT_EQ(game.getPieceAtPos(Sq("d5")), Piece::Type::BLANK);
}

TEST(GameEnPassantTest, BlackCapturesEnPassantFromFen) {
  Game game("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1");
  ASSERT_TRUE(Play(game, "d4", "e3"));
  EXPECT_EQ(game.getPieceAtPos(Sq("e3")), Piece::Type::BLACK_PAWN);
  EXPECT_EQ(game.getPieceAtPos(Sq("e4")), Piece::Type::BLANK);
}

TEST(GameEnPassantTest, AvailableRightAfterDoublePush) {
  Game game("4k3/3p4/8/4P3/8/8/8/4K3 b - - 0 1");
  ASSERT_TRUE(Play(game, "d7", "d5"));
  EXPECT_EQ(game.getEnPassantPos(), Sq("d6"));
  EXPECT_TRUE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

TEST(GameEnPassantTest, ExpiresAfterOneMove) {
  Game game("4k3/3p4/8/4P3/8/8/8/4K3 b - - 0 1");
  ASSERT_TRUE(Play(game, "d7", "d5"));
  ASSERT_TRUE(Play(game, "e1", "e2"));
  ASSERT_TRUE(Play(game, "e8", "e7"));
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

TEST(GameEnPassantTest, NotAllowedWithoutFenSquare) {
  Game game("4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

TEST(GameEnPassantTest, NoPhantomCaptureOntoA1WithoutFenSquare) {
  // "No en passant square" must not be stored as square 0 (a1), or the b2
  // pawn could capture "en passant" onto the empty a1.
  Game game("4k3/8/8/8/8/8/1p6/4K3 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "b2");
  EXPECT_EQ(moves.size(), 4u);  // Only the four promotions on b1.
  EXPECT_FALSE(HasMove(moves, "b2", "a1", Piece::Type::BLACK_QUEEN));
}

TEST(GameEnPassantTest, IllegalWhenItExposesTheKingOnTheRank) {
  // Both pawns leave the 5th rank, opening it between the h5 rook and a5 king.
  Game game("8/8/8/K2pP2r/8/8/8/4k3 w - d6 0 1");
  EXPECT_FALSE(HasMove(MovesFrom(game, "e5"), "e5", "d6"));
}

// --- Promotion ------------------------------------------------------------

TEST(GamePromotionTest, PushOffersAllFourPieces) {
  Game game("k7/4P3/8/8/8/8/8/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e7");
  EXPECT_EQ(moves.size(), 4u);
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::Type::WHITE_QUEEN));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::Type::WHITE_ROOK));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::Type::WHITE_BISHOP));
  EXPECT_TRUE(HasMove(moves, "e7", "e8", Piece::Type::WHITE_KNIGHT));
}

TEST(GamePromotionTest, CapturesPromoteOnBothSides) {
  Game game("k2r1r2/4P3/8/8/8/8/8/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e7");
  EXPECT_EQ(moves.size(), 12u);
  EXPECT_TRUE(HasMove(moves, "e7", "d8", Piece::Type::WHITE_QUEEN));
  EXPECT_TRUE(HasMove(moves, "e7", "f8", Piece::Type::WHITE_KNIGHT));
}

TEST(GamePromotionTest, BlackPushOffersAllFourPieces) {
  Game game("4k3/8/8/8/8/8/3p4/K7 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d2");
  EXPECT_EQ(moves.size(), 4u);
  EXPECT_TRUE(HasMove(moves, "d2", "d1", Piece::Type::BLACK_QUEEN));
  EXPECT_TRUE(HasMove(moves, "d2", "d1", Piece::Type::BLACK_KNIGHT));
}

TEST(GamePromotionTest, BlackCapturesPromoteOnBothSides) {
  Game game("7k/8/8/8/8/8/3p4/K1R1R3 b - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d2");
  EXPECT_EQ(moves.size(), 12u);
  EXPECT_TRUE(HasMove(moves, "d2", "c1", Piece::Type::BLACK_QUEEN));
  EXPECT_TRUE(HasMove(moves, "d2", "e1", Piece::Type::BLACK_ROOK));
}

TEST(GamePromotionTest, PromotedPieceReplacesThePawn) {
  Game game("k7/4P3/8/8/8/8/8/4K3 w - - 0 1");
  ASSERT_TRUE(Play(game, "e7", "e8", Piece::Type::WHITE_KNIGHT));
  EXPECT_EQ(game.getPieceAtPos(Sq("e8")), Piece::Type::WHITE_KNIGHT);
  EXPECT_EQ(game.getPieceAtPos(Sq("e7")), Piece::Type::BLANK);
}

// --- Pins and check -------------------------------------------------------

TEST(GamePinTest, AbsolutelyPinnedBishopCannotMove) {
  Game game("4k3/4r3/8/8/8/8/4B3/4K3 w - - 0 1");
  EXPECT_TRUE(MovesFrom(game, "e2").empty());
}

TEST(GamePinTest, PinnedRookMovesOnlyAlongThePin) {
  Game game("4k3/4r3/8/8/8/8/4R3/4K3 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "e2");
  EXPECT_EQ(moves.size(), 5u);  // e3, e4, e5, e6 and capturing on e7.
  EXPECT_TRUE(HasMove(moves, "e2", "e7"));
  EXPECT_FALSE(HasMove(moves, "e2", "d2"));
}

TEST(GameCheckTest, DetectsCheck) {
  Game game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  EXPECT_TRUE(game.isKingInCheck(Piece::Color::WHITE));
  EXPECT_FALSE(game.isKingInCheck(Piece::Color::BLACK));
}

TEST(GameCheckTest, KingCannotStepAlongTheCheckingLine) {
  // f1 is still attacked once the king leaves e1 (x-ray through the king).
  Game game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  const std::vector<Move> moves = game.getAllLegalMoves();
  EXPECT_EQ(moves.size(), 3u);
  EXPECT_TRUE(HasMove(moves, "e1", "d2"));
  EXPECT_TRUE(HasMove(moves, "e1", "e2"));
  EXPECT_TRUE(HasMove(moves, "e1", "f2"));
}

TEST(GameCheckTest, KnightCheckLeavesOnlyKingMoves) {
  // The d3 knight checks e1 and can't be blocked or taken: the king must go
  // to d1, d2, e2 or f1 (f2 is covered by the knight).
  Game game("4k3/8/8/8/8/3n4/8/R3K3 w - - 0 1");
  EXPECT_TRUE(game.isKingInCheck(Piece::Color::WHITE));
  const std::vector<Move> moves = game.getAllLegalMoves();
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
  Game game{std::string(GetParam().fen)};
  EXPECT_TRUE(game.isKingInCheck(Piece::Color::WHITE));
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
  Game game("4k3/8/8/8/8/8/4p3/4K3 w - - 0 1");
  EXPECT_FALSE(game.isKingInCheck(Piece::Color::WHITE));
}

TEST(GameCheckTest, OnlyMovesThatResolveCheckAreLegal) {
  // Knight check from d3: the king must move or the knight must be taken.
  Game game("4k3/8/8/8/8/3n4/8/R3K3 w - - 0 1");
  for (const Move& move : game.getAllLegalMoves()) {
    Game child(game);
    child.makeMove(move);
    EXPECT_FALSE(child.isKingInCheck(Piece::Color::WHITE)) << move;
  }
}

TEST(GameCheckTest, KingsCannotStandNextToEachOther) {
  Game game("8/8/8/3k4/8/3K4/8/8 w - - 0 1");
  const std::vector<Move> moves = MovesFrom(game, "d3");
  EXPECT_FALSE(HasMove(moves, "d3", "d4"));
  EXPECT_FALSE(HasMove(moves, "d3", "c4"));
  EXPECT_FALSE(HasMove(moves, "d3", "e4"));
  EXPECT_EQ(moves.size(), 5u);
}

// --- Game end -------------------------------------------------------------

TEST(GameEndTest, FoolsMateIsAWinForBlack) {
  Game game("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
  EXPECT_TRUE(game.getAllLegalMoves().empty());
  EXPECT_TRUE(game.isGameOver());
  EXPECT_EQ(game.getResult(), Result::BLACK_WIN);
}

TEST(GameEndTest, BackRankMateIsAWinForWhite) {
  Game game("R5k1/5ppp/8/8/8/8/8/6K1 b - - 0 1");
  EXPECT_TRUE(game.isGameOver());
  EXPECT_EQ(game.getResult(), Result::WHITE_WIN);
}

TEST(GameEndTest, StalemateIsADraw) {
  Game game("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
  EXPECT_FALSE(game.isKingInCheck(Piece::Color::BLACK));
  EXPECT_TRUE(game.getAllLegalMoves().empty());
  EXPECT_TRUE(game.isGameOver());
  EXPECT_EQ(game.getResult(), Result::DRAW);
}

TEST(GameEndTest, CheckWithAnEscapeIsNotGameOver) {
  Game game("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  EXPECT_FALSE(game.isGameOver());
  EXPECT_EQ(game.getResult(), Result::ONGOING);
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
  Game game{std::string(GetParam().fen)};
  EXPECT_EQ(game.isGameOver(), GetParam().draw);
  EXPECT_EQ(game.getResult(), GetParam().draw ? Result::DRAW : Result::ONGOING);
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
