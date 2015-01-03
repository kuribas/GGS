#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece knight, tokin, lance, rook;

  //                   name, prom_to, ~from, num, rank, props, moves
  MoveWithCheck *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "P", &knight,  NULL,  0,   0,    PAWN, pawnmoves};

  MoveWithCheck *bishopmoves[] = {&longmove_505, NULL};
  Piece bishop = { "B", &tokin, NULL, 1,   0,    0, bishopmoves};

  MoveWithCheck *silvermoves[] = {&shortmove_507, NULL};
  Piece silver = { "S", &lance, NULL, 2, 0, 0, silvermoves};

  MoveWithCheck *goldmoves[] = {&shortmove_257, NULL};
  Piece gold = { "G", &rook,   NULL,   3,   0,     0, goldmoves};

  MoveWithCheck *knightmoves[] = {&knightmove_2, NULL};
  Piece knight = { "N", NULL, &pawn, 4, 0, 0, knightmoves};

  MoveWithCheck *tokinmoves[] = {&shortmove_257, NULL};
  Piece tokin = { "+P", NULL, &bishop,  5,   0,    0, tokinmoves};

  MoveWithCheck *lancemoves[] = {&longmove_2, NULL};
  Piece lance = { "L", NULL, &silver, 6, 0,  0, lancemoves};

  MoveWithCheck *rookmoves[] = {&longmove_252, NULL};
  Piece rook = { "R", NULL, &gold, 7,   0,    0, rookmoves};

  MoveWithCheck *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "K", NULL,   NULL,   8,   0,    KING, kingmoves};


  //names in alphabetic order
  char *shogi_names[] = {"+P", "B", "G", "K", "L", "N", "P", "R", "S"};

  int name2pos[] = {
     5, 1, 3, 8, 6, 4, 0, 7, 2};

  const Piece *pieces[] = {
     &pawn, &bishop, &silver, &gold, &knight, &tokin, &lance, &rook, &king};

  Board::Square micro_setup[] = {
     {8, 1}, {1, 1}, {3, 1}, {2, 1},
     {0, 1}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 0},
     {2, 0}, {3, 0}, {1, 0}, {8, 0}};
}

SmallBoard::Type micro_type = {
   pieces, micro_setup, 0, HAS_DROPS | DROP_ANY | PROM_BY_CAPTURE | PAWNDROP_CAN_MATE,
   4, 5, 9, shogi_names, name2pos, 2, "Micro Shogi", 1, 4, 0};
