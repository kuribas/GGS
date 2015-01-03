#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece tokin, gold, prom_dragon;

  // moves
  //                   name, prom_to, ~from, num, rank, props,
  MoveWithCheck *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "P", &tokin, NULL,  0,   0,    PAWN, pawnmoves};

  MoveWithCheck *lancemoves[] = {&longmove_2, NULL};
  Piece lance = { "L", &gold, NULL,  1, 0,  0, lancemoves};
  
  MoveWithCheck *silvermoves[] = {&shortmove_507, NULL};
  Piece silver = { "S", &gold, NULL, 2, 0, 0, silvermoves};

  MoveWithCheck *goldmoves[] = {&shortmove_257, NULL};
  Piece gold = { "G", NULL,   NULL,  3, 0,  0, goldmoves};

  MoveWithCheck *coppermoves[] = {&shortmove_207, NULL};
  Piece copper = { "C", &gold, NULL, 4, 0, 0, coppermoves};

  MoveWithCheck *ironmoves[] = {&shortmove_7, NULL};
  Piece iron = { "I", &gold, NULL, 5, 0,   0, ironmoves};

  MoveWithCheck *dragonmoves[] = {&longmove_505, NULL};
  Piece dragon = { "FD", &prom_dragon, NULL, 6, 0, 0, dragonmoves};

  MoveWithCheck *sidemovermoves[] = {&longmove_50, &shortmove_202, NULL};
  Piece sidemover = { "SM", &gold, NULL, 7, 0, 0, sidemovermoves};

  MoveWithCheck *tigermoves[] = {&shortmove_505, NULL};
  Piece tiger = { "FT", &gold, NULL, 8, 0, 0, tigermoves};

  MoveWithCheck *chariotmoves[] = {&longmove_202, NULL};
  Piece chariot = { "FC", &gold, NULL, 9, 0, 0, chariotmoves};

  MoveWithCheck *gobetweenmoves[] = {&shortmove_202, NULL};
  Piece gobetween = { "GB", &gold, NULL, 10, 0, 0, gobetweenmoves};

  MoveWithCheck *prom_dragonmoves[] = {&longmove_505, &shortmove_252, NULL};
  Piece prom_dragon = { "+FD", NULL, NULL, 11, 0, 0, prom_dragonmoves};

  MoveWithCheck *knightmoves[] = {&knightmove_2, NULL};
  Piece knight = { "N", &gold, NULL, 12, 0, 0, knightmoves};

  MoveWithCheck *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "K", NULL,   NULL,   13,   0,    KING, kingmoves};

  MoveWithCheck *tokinmoves[] = {&shortmove_257, NULL};
  Piece tokin = { "+P", NULL, &pawn,  14,   0,    0, tokinmoves};

  //names in alphabetic order
  char *names[] = { "+FD", "+P", "C", "FC", "FD", "FT", "G", "GB", "I",
		    "K", "L", "N", "P", "S", "SM" };
  int name2pos[] = {11, 14, 4, 9, 6, 8, 3, 10, 5, 13, 1, 12, 0, 2, 7};

  const Piece *pieces[] = {
     &pawn, &lance, &silver, &gold, &copper, &iron, &dragon, &sidemover, &tiger,
     &chariot, &gobetween, &prom_dragon, &knight, &king, &tokin};

  Board::Square heian_setup[] = {
     {1, 1}, {12, 1}, {2, 1}, {3, 1}, {13, 1}, {3, 1}, {2, 1}, {12, 1}, {1, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {1, 0}, {12, 0}, {2, 0}, {3, 0}, {13, 0}, {3, 0}, {2, 0}, {12, 0}, {1, 0}};

  Board::Square heiandai_setup[] = {
     {1, 1}, {12, 1}, {5, 1}, {4, 1}, {2, 1}, {3, 1}, {13, 1}, {3, 1}, {2, 1}, {4, 1}, {5, 1}, {12, 1}, {1, 1},
     {9, 1}, {6, 1}, {0, 2}, {0, 2}, {8, 1}, {0, 2}, {7, 1}, {0, 2}, {8, 1}, {0, 2}, {0, 2}, {6, 1}, {9, 1},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {10, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {10, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {9, 0}, {6, 0}, {0, 2}, {0, 2}, {8, 0}, {0, 2}, {7, 0}, {0, 2}, {8, 0}, {0, 2}, {0, 2}, {6, 0}, {9, 0},
     {1, 0}, {12, 0}, {5, 0}, {4, 0}, {2, 0}, {3, 0}, {13, 0}, {3, 0}, {2, 0}, {4, 0}, {5, 0}, {12, 0}, {1, 0}};

}

SmallBoard::Type heian_type = {
   pieces, heian_setup, 3, 0, 9, 8, 15, names, name2pos, 3, "Heian Shogi", 9, 0, 0};

SmallBoard::Type heiandai_type = {
   pieces, heiandai_setup, 3, 0, 13, 13, 15, names, name2pos, 3, "Heian-Dai Shogi", 10, 0, 0};
