#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece knight, tokin, lance, rook;

  //                   name, prom_to, ~from, num, rank, props, moves
  BasicMove *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &knight,  NULL,  0,   0,    Piece::PAWN, pawnmoves};

  BasicMove *bishopmoves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &tokin, NULL, 1,   0,    0, bishopmoves};

  BasicMove *silvermoves[] = {&shortmove_507, NULL};
  Piece silver = { "Silver", "S", &lance, NULL, 2, 0, 0, silvermoves};

  BasicMove *goldmoves[] = {&shortmove_257, NULL};
  Piece gold = { "Gold", "G", &rook,   NULL,   3,   0,     0, goldmoves};

  BasicMove *knightmoves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", NULL, &pawn, 4, 0, 0, knightmoves};

  BasicMove *tokinmoves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, &bishop,  5,   0,    0, tokinmoves};

  BasicMove *lancemoves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", NULL, &silver, 6, 0,  0, lancemoves};

  BasicMove *rookmoves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", NULL, &gold, 7,   0,    0, rookmoves};

  BasicMove *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "King", "K", NULL,   NULL,   8,   0,    Piece::KING, kingmoves};


  //names in alphabetic order
  const char *shogi_names[] = {"+P", "B", "G", "K", "L", "N", "P", "R", "S"};

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
   pieces, micro_setup, 0,
   Board::HAS_DROPS | Board::DROP_ANY | Board::PROM_BY_CAPTURE | Board::PAWNDROP_CAN_MATE,
   4, 5, 9, shogi_names, name2pos, 2, "Micro Shogi", 1, 4, 0};
