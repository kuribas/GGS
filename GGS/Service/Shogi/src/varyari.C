#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece ysilver, rook, prom_ybishop, prom_yknight;

  //                   name, prom_to, ~from, num, rank, props, moves
  BasicMove *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &ysilver,  NULL,  0,   0,  Piece::PAWN, pawnmoves};

  BasicMove *yrookmoves[] = {&longmove_52, NULL};
  Piece yrook = { "Yari Rook", "YR", &rook, NULL, 1, 0, 0, yrookmoves};

  BasicMove *ybishopmoves[] = {&longmove_2, &shortmove_5, NULL};
  Piece ybishop = { "Yari Bishop", "YB", &prom_ybishop, NULL, 2, 0, 0, ybishopmoves};

  BasicMove *yknightmoves[] = {&longmove_2, &knightmove_2, NULL};
  Piece yknight = { "Yari Knight", "YN", &prom_yknight, NULL, 3, 0, 0, yknightmoves};

  BasicMove *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "King", "K", NULL,   NULL,   4,   0,  Piece::KING, kingmoves};

  BasicMove *rookmoves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "+YR", NULL, &yrook,  5,   0,    0, rookmoves};

  BasicMove *prom_ybishopmoves[] = {&shortmove_57, &longmove_200, NULL};
  Piece prom_ybishop = { "Promoted Yari Bishop", "+YB", NULL, &ybishop, 6, 0, 0, prom_ybishopmoves};

  BasicMove *prom_yknightmoves[] = {&shortmove_57, &longmove_200, NULL};
  Piece prom_yknight = { "Promoted Yari Knight", "+YN", NULL, &yknight, 7, 0, 0, prom_yknightmoves};

  BasicMove *ysilvermoves[] = {&shortmove_7, &longmove_200, NULL};
  Piece ysilver = { "Yari Silver", "+P", NULL, &pawn, 8, 0, 0, ysilvermoves};


  //names in alphabetic order
  const char *names[] = {"+P", "+YB", "+YN", "+YR", "K", "P", "YB", "YN", "YR"};

  int name2pos[] = {
     8, 6, 7, 5, 4, 0, 2, 3, 1};

  const Piece *pieces[] = {
     &pawn, &yrook, &ybishop, &yknight, &king, &rook, &prom_ybishop, &prom_yknight, &ysilver};

  Board::Square yari_setup[] = {
     {1, 1}, {3, 1}, {3, 1}, {4, 1}, {2, 1}, {2, 1}, {1, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {1, 0}, {2, 0}, {2, 0}, {4, 0}, {3, 0}, {3, 0}, {1, 0}};
}

SmallBoard::Type yari_type = {
   pieces, yari_setup, 0, Board::HAS_DROPS | Board::PAWNDROP_CAN_MATE | Board::HAS_STALEMATE, 7, 9, 9, names,
   name2pos, 3, "Yari Shogi", 5, 4, 3};
