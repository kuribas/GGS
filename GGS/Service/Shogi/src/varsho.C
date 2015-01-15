#include "ShogiImpl.H"

namespace
{
  
  extern Piece tokin, gold_general, gold_general, gold_general, dragon_horse, dragon_king, crown_prince;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL, 0, 0, Piece::PAWN, pawn_moves};

  BasicMove *king_moves[] = {&shortmove_757, NULL};
  Piece king = { "King", "K", NULL, NULL, 1, 0, Piece::KING, king_moves};

  BasicMove *lance_moves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", &gold_general, NULL, 2, 0, 0, lance_moves};

  BasicMove *knight_moves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &gold_general, NULL, 3, 0, 0, knight_moves};

  BasicMove *silver_general_moves[] = {&shortmove_507, NULL};
  Piece silver_general = { "Silver General", "S", &gold_general, NULL, 4, 0, 0, silver_general_moves};

  BasicMove *gold_general_moves[] = {&shortmove_257, NULL};
  Piece gold_general = { "Gold General", "G", NULL, NULL, 5, 0, 0, gold_general_moves};

  BasicMove *bishop_moves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &dragon_horse, NULL, 6, 0, 0, bishop_moves};

  BasicMove *rook_moves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", &dragon_king, NULL, 7, 0, 0, rook_moves};

  BasicMove *dragon_horse_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece dragon_horse = { "Dragon Horse", "+B", NULL, NULL, 8, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "+R", NULL, NULL, 9, 0, 0, dragon_king_moves};

  BasicMove *tokin_moves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, NULL, 10, 0, 0, tokin_moves};

  BasicMove *drunk_elephant_moves[] = {&shortmove_557, NULL};
  Piece drunk_elephant = { "Drunk Elephant", "DE", &crown_prince, NULL, 11, 0, 0, drunk_elephant_moves};

  BasicMove *crown_prince_moves[] = {&shortmove_757, NULL};
  Piece crown_prince = { "Crown Prince", "CP", NULL, NULL, 12, 0, Piece::KING, crown_prince_moves};

  const char *names[] = { "+B", "+P", "+R", "B", "CP", "DE", "G", "K", "L", "N", "P", "R", "S" };

  int name2pos[] = { 8, 10, 9, 6, 12, 11, 5, 1, 2, 3, 0, 7, 4};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &knight, &silver_general, &gold_general, &bishop, &rook, &dragon_horse, &dragon_king, &tokin, &drunk_elephant, &crown_prince};

  Board::Square setup[] = {
     {2, 1}, {3, 1}, {4, 1}, {5, 1}, {1, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, 
     {0, 2}, {7, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {6, 1}, {0, 2}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {0, 2}, {6, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {7, 0}, {0, 2}, 
     {2, 0}, {3, 0}, {4, 0}, {5, 0}, {1, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}};

}

Board::Type sho_type = {
   pieces, setup, 3, 0, 9, 9, 13, names, name2pos, 2, "Sho Shogi", 11};
