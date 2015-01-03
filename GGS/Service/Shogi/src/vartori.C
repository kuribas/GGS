#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece goose, eagle;
  //                   name, prom_to, ~from, num, rank, props, moves
  MoveWithCheck *swallowmoves[] = {&shortmove_2, NULL};
  Piece swallow = { "Sw", &goose, NULL, 0, 0, PAWN, swallowmoves};

  MoveWithCheck *leftquailmoves[] = {&longmove_402, &shortmove_100, NULL};
  Piece leftquail = { "lQ", NULL, NULL, 1, 0, 0, leftquailmoves};

  MoveWithCheck *rightquailmoves[] = {&longmove_102, &shortmove_400, NULL};
  Piece rightquail = { "rQ", NULL, NULL, 2, 0, 0, rightquailmoves};

  MoveWithCheck *falconmoves[] = {&shortmove_557, NULL};
  Piece falcon = { "Fa", &eagle,    NULL,  3,  0, 0, falconmoves};

  MoveWithCheck *cranemoves[] = {&shortmove_707, NULL};
  Piece crane = { "Cr", NULL,     NULL,  4, 0, 0, cranemoves};

  MoveWithCheck *phaesantmoves[] = {&shortmove_500, &jumpmove_2, NULL};
  Piece phaesant = { "Pt", NULL, NULL,   5, 0, 0, phaesantmoves};

  MoveWithCheck *phoenixmoves[] = {&shortmove_777, NULL};
  Piece phoenix = { "Ph", NULL,   NULL,   6,   0,  KING, phoenixmoves};

  MoveWithCheck *goosemoves[] = {&jumpmove_205, NULL};
  Piece goose = { "Go", NULL, &swallow, 7, 0, 0, goosemoves};

  MoveWithCheck *eaglemoves[] = {&longmove_205, &shortmove_52, &nummove_500_2, NULL};
  Piece eagle = { "Eg", NULL, &falcon, 8, 0, 0, eaglemoves};

  //names in alphabetic order
  char *names[] = {"Cr", "Eg", "Fa", "Go", "Ph", "Pt", "Sw", "lQ", "rQ"};

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
   pieces, tori_setup, 2, HAS_DROPS | MUST_PROM, 7, 7, 9, names, name2pos, 2, "Tori Shogi", 6, 6, 2};
