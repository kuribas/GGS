#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece tokin, gold, prom_dragon;

  // moves
  //                   name, prom_to, ~from, num, rank, props,
  BasicMove *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL,  0,   0,    Piece::PAWN, pawnmoves};

  BasicMove *lancemoves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", &gold, NULL,  1, 0,  0, lancemoves};
  
  BasicMove *silvermoves[] = {&shortmove_507, NULL};
  Piece silver = { "Silver", "S", &gold, NULL, 2, 0, 0, silvermoves};

  BasicMove *goldmoves[] = {&shortmove_257, NULL};
  Piece gold = { "Gold", "G", NULL,   NULL,  3, 0,  0, goldmoves};

  BasicMove *coppermoves[] = {&shortmove_207, NULL};
  Piece copper = { "Copper", "C", &gold, NULL, 4, 0, 0, coppermoves};

  BasicMove *ironmoves[] = {&shortmove_7, NULL};
  Piece iron = { "Iron", "I", &gold, NULL, 5, 0,   0, ironmoves};

  BasicMove *dragonmoves[] = {&longmove_505, NULL};
  Piece dragon = { "Dragon", "FD", &prom_dragon, NULL, 6, 0, 0, dragonmoves};

  BasicMove *sidemovermoves[] = {&longmove_50, &shortmove_202, NULL};
  Piece sidemover = { "Sidemover", "SM", &gold, NULL, 7, 0, 0, sidemovermoves};

  BasicMove *tigermoves[] = {&shortmove_505, NULL};
  Piece tiger = { "Tiger", "FT", &gold, NULL, 8, 0, 0, tigermoves};

  BasicMove *chariotmoves[] = {&longmove_202, NULL};
  Piece chariot = { "Chariot", "FC", &gold, NULL, 9, 0, 0, chariotmoves};

  BasicMove *gobetweenmoves[] = {&shortmove_202, NULL};
  Piece gobetween = { "Gobetween", "GB", &gold, NULL, 10, 0, 0, gobetweenmoves};

  BasicMove *prom_dragonmoves[] = {&longmove_505, &shortmove_252, NULL};
  Piece prom_dragon = { "Promoted Dragon", "+FD", NULL, NULL, 11, 0, 0, prom_dragonmoves};

  BasicMove *knightmoves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &gold, NULL, 12, 0, 0, knightmoves};

  BasicMove *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "King", "K", NULL,   NULL,   13,   0,    Piece::KING, kingmoves};

  BasicMove *tokinmoves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, &pawn,  14,   0,    0, tokinmoves};

  //names in alphabetic order
  const char *names[] = { "+FD", "+P", "C", "FC", "FD", "FT", "G", "GB", "I",
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
