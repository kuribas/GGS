#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece goose, eagle;
  //                   name, prom_to, ~from, num, rank, props, moves
  BasicMove *swallowmoves[] = {&shortmove_2, NULL};
  Piece swallow = { "Swallow", "Sw", &goose, NULL, 0, 0, Piece::PAWN, swallowmoves};

  BasicMove *leftquailmoves[] = {&longmove_402, &shortmove_100, NULL};
  Piece leftquail = { "Leftquail", "lQ", NULL, NULL, 1, 0, 0, leftquailmoves};

  BasicMove *rightquailmoves[] = {&longmove_102, &shortmove_400, NULL};
  Piece rightquail = { "Rightquail", "rQ", NULL, NULL, 2, 0, 0, rightquailmoves};

  BasicMove *falconmoves[] = {&shortmove_557, NULL};
  Piece falcon = { "Falcon", "Fa", &eagle,    NULL,  3,  0, 0, falconmoves};

  BasicMove *cranemoves[] = {&shortmove_707, NULL};
  Piece crane = { "Crane", "Cr", NULL,     NULL,  4, 0, 0, cranemoves};

  BasicMove *phaesantmoves[] = {&shortmove_500, &jumpmove_2, NULL};
  Piece phaesant = { "Phaesant", "Pt", NULL, NULL,   5, 0, 0, phaesantmoves};

  BasicMove *phoenixmoves[] = {&shortmove_777, NULL};
  Piece phoenix = { "Phoenix", "Ph", NULL,   NULL,   6,   0,  Piece::KING, phoenixmoves};

  BasicMove *goosemoves[] = {&jumpmove_205, NULL};
  Piece goose = { "Goose", "Go", NULL, &swallow, 7, 0, 0, goosemoves};

  BasicMove *eaglemoves[] = {&longmove_205, &shortmove_52, &nummove_500_2, NULL};
  Piece eagle = { "Eagle", "Eg", NULL, &falcon, 8, 0, 0, eaglemoves};

  //names in alphabetic order
  const char *names[] = {"Cr", "Eg", "Fa", "Go", "Ph", "Pt", "Sw", "lQ", "rQ"};

  int name2pos[] = {
     4, 8, 3, 7, 6, 5, 0, 1, 2};

  const Piece *pieces[] = {
     &swallow, &leftquail, &rightquail, &falcon, &crane, &phaesant, &phoenix, &goose, &eagle};

  Board::Square tori_setup[] = {
     {2, 1}, {5, 1}, {4, 1}, {6, 1}, {4, 1}, {5, 1}, {1, 1},
     {0, 2}, {0, 2}, {0, 2}, {3, 1}, {0, 2}, {0, 2}, {0, 2},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 1}, {0, 2}, {0, 0}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {0, 2}, {0, 2}, {0, 2}, {3, 0}, {0, 2}, {0, 2}, {0, 2},
     {1, 0}, {5, 0}, {4, 0}, {6, 0}, {4, 0}, {5, 0}, {2, 0}};
}

SmallBoard::Type tori_type = {
   pieces, tori_setup, 2, Board::HAS_DROPS | Board::MUST_PROM, 7, 7, 9, names, name2pos, 2, "Tori Shogi", 6, 6, 2};
