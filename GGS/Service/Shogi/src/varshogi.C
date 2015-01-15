#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece tokin, horse, dragon, prom_lance, prom_knight, prom_silver;

  //                   name, prom_to, ~from, num, rank, props, moves
  BasicMove *pawnmoves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin,  NULL,  0,   0,    Piece::PAWN, pawnmoves};

  BasicMove *bishopmoves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &horse, NULL, 1,   0,    0, bishopmoves};

  BasicMove *rookmoves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", &dragon, NULL,  2,   0,    0, rookmoves};

  BasicMove *lancemoves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", &prom_lance, NULL, 3, 0,  0, lancemoves};

  BasicMove *silvermoves[] = {&shortmove_507, NULL};
  Piece silver = { "Silver", "S", &prom_silver, NULL, 4, 0, 0, silvermoves};

  BasicMove *goldmoves[] = {&shortmove_257, NULL};
  Piece gold = { "Gold", "G", NULL,   NULL,   5,   0,     0, goldmoves};

  BasicMove *knightmoves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &prom_knight, NULL, 6, 0, 0, knightmoves};


  BasicMove *kingmoves[] = {&shortmove_777, NULL};
  Piece king = { "King", "K", NULL,   NULL,   7,   0,    Piece::KING, kingmoves};


  BasicMove *tokinmoves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, &pawn,  8,   0,    0, tokinmoves};

  BasicMove *horsemoves[] = {&longmove_505, &shortmove_252, NULL};
  Piece horse = { "Horse", "+B", NULL, &bishop, 9,  0,    0, horsemoves};

  BasicMove *dragonmoves[] = {&longmove_252, &shortmove_505, NULL};
  Piece dragon = { "Dragon", "+R", NULL, &rook, 10,  0,    0, dragonmoves};

  BasicMove *prom_lancemoves[] = {&shortmove_257, NULL};
  Piece prom_lance = { "Promoted Lance", "+L", NULL, &lance, 11,   0,  0, prom_lancemoves};

  BasicMove *prom_knightmoves[] = {&shortmove_257, NULL};
  Piece prom_knight = { "Promoted Knight", "+N", NULL, &knight, 12, 0,  0, prom_knightmoves};

  BasicMove *prom_silvermoves[] = {&shortmove_257, NULL};
  Piece prom_silver = { "Promoted Silver", "+S", NULL, &silver, 13, 0,  0, prom_silvermoves};


  //names in alphabetic order
  const char *shogi_names[] = {"+B", "+L", "+N", "+P", "+R", "+S",
			 "B", "G", "K", "L", "N", "P", "R", "S"};

  int name2pos[] = {9, 11, 12, 8, 10, 13,
		    1, 5, 7, 3, 6, 0, 2, 4};

  Board::Square mini_setup[] = {
     {2, 1}, {1, 1}, {4, 1}, {5, 1}, {7, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {7, 0}, {5, 0}, {4, 0}, {1, 0}, {2, 0}};

  Board::Square judkins_setup[] = {
     {2, 1}, {1, 1}, {6, 1}, {4, 1}, {5, 1}, {7, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {7, 0}, {5, 0}, {4, 0}, {6, 0}, {1, 0}, {2, 0}};

  Board::Square shogi_setup[] = {
     {3, 1}, {6, 1}, {4, 1}, {5, 1}, {7, 1}, {5, 1}, {4, 1}, {6, 1}, {3, 1},
     {0, 2}, {2, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {1, 1}, {0, 2},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {0, 2}, {1, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {2, 0}, {0, 2},
     {3, 0}, {6, 0}, {4, 0}, {5, 0}, {7, 0}, {5, 0}, {4, 0}, {6, 0}, {3, 0}};
   
  const Piece *pieces[] = {
     &pawn, &bishop, &rook, &lance, &silver, &gold, &knight,
     &king, &tokin, &horse, &dragon, &prom_lance, &prom_knight, &prom_silver};
}

SmallBoard::Type judkins_type = {
   pieces, judkins_setup, 1, Board::HAS_DROPS, 6, 6, 14, shogi_names, name2pos, 2, "Judkin's Shogi", 3, 7, 1};

SmallBoard::Type shogi_type = {
   pieces, shogi_setup, 3, Board::HAS_DROPS, 9, 9, 14, shogi_names, name2pos, 2, "Regular Shogi", 0, 7, 1};

SmallBoard::Type mini_type = {
   pieces, mini_setup, 1, Board::HAS_DROPS, 5, 5, 14, shogi_names, name2pos, 2, "Mini Shogi", 2, 6, 1};
