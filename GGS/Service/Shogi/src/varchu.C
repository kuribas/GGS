#include "ShogiImpl.H"

namespace
{
  extern Piece tokin, white_horse, prom_silver_general, prom_gold_general, prom_copper_general, prom_bishop, prom_rook, flying_stag, horned_falcon, soaring_eagle, crown_prince, prom_ferocious_leopard, prom_go_between, lion, whale, free_boar, flying_ox;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL, 0, 0, Piece::PAWN | Piece::PAWNLIKE, pawn_moves};

  BasicMove *king_moves[] = {&shortmove_757, NULL};
  Piece king = { "King", "K", NULL, NULL, 1, 0, Piece::KING, king_moves};

  BasicMove *lance_moves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", &white_horse, NULL, 2, 0, 0, lance_moves};

  BasicMove *silver_general_moves[] = {&shortmove_507, NULL};
  Piece silver_general = { "Silver General", "S", &prom_silver_general, NULL, 3, 0, 0, silver_general_moves};

  BasicMove *gold_general_moves[] = {&shortmove_257, NULL};
  Piece gold_general = { "Gold General", "G", &prom_gold_general, NULL, 4, 0, 0, gold_general_moves};

  BasicMove *copper_general_moves[] = {&shortmove_207, NULL};
  Piece copper_general = { "Copper General", "C", &prom_copper_general, NULL, 5, 0, 0, copper_general_moves};

  BasicMove *bishop_moves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &prom_bishop, NULL, 6, 0, 0, bishop_moves};

  BasicMove *rook_moves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", &prom_rook, NULL, 7, 0, 0, rook_moves};

  BasicMove *blind_tiger_moves[] = {&shortmove_755, NULL};
  Piece blind_tiger = { "Blind Tiger", "BT", &flying_stag, NULL, 8, 0, 0, blind_tiger_moves};

  BasicMove *dragon_horse_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece dragon_horse = { "Dragon Horse", "DH", &horned_falcon, NULL, 9, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "DK", &soaring_eagle, NULL, 10, 0, 0, dragon_king_moves};

  BasicMove *drunk_elephant_moves[] = {&shortmove_557, NULL};
  Piece drunk_elephant = { "Drunk Elephant", "DE", &crown_prince, NULL, 11, 0, 0, drunk_elephant_moves};

  BasicMove *ferocious_leopard_moves[] = {&shortmove_707, NULL};
  Piece ferocious_leopard = { "Ferocious Leopard", "FL", &prom_ferocious_leopard, NULL, 12, 0, 0, ferocious_leopard_moves};

  BasicMove *free_king_moves[] = {&longmove_757, NULL};
  Piece free_king = { "Free King", "FK", NULL, NULL, 13, 0, 0, free_king_moves};

  BasicMove *go_between_moves[] = {&shortmove_202, NULL};
  Piece go_between = { "Go Between", "GB", &prom_go_between, NULL, 14, 0, Piece::PAWNLIKE, go_between_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &lion, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &free_king, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", NULL, NULL, 17, 0, Piece::LION, lion_moves};

  BasicMove *reverse_chariot_moves[] = {&longmove_202, NULL};
  Piece reverse_chariot = { "Reverse Chariot", "RC", &whale, NULL, 18, 0, 0, reverse_chariot_moves};

  BasicMove *side_mover_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece side_mover = { "Side Mover", "SM", &free_boar, NULL, 19, 0, 0, side_mover_moves};

  BasicMove *vertical_mover_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece vertical_mover = { "Vertical Mover", "VM", &flying_ox, NULL, 20, 0, 0, vertical_mover_moves};

  BasicMove *tokin_moves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, NULL, 21, 0, 0, tokin_moves};

  BasicMove *crown_prince_moves[] = {&shortmove_757, NULL};
  Piece crown_prince = { "Crown Prince", "+DE", NULL, NULL, 22, 0, Piece::KING, crown_prince_moves};

  BasicMove *flying_stag_moves[] = {&shortmove_555, &longmove_202, NULL};
  Piece flying_stag = { "Flying Stag", "+BT", NULL, NULL, 23, 0, 0, flying_stag_moves};

  BasicMove *flying_ox_moves[] = {&longmove_707, NULL};
  Piece flying_ox = { "Flying Ox", "+VM", NULL, NULL, 24, 0, 0, flying_ox_moves};

  BasicMove *free_boar_moves[] = {&longmove_555, NULL};
  Piece free_boar = { "Free Boar", "+SM", NULL, NULL, 25, 0, 0, free_boar_moves};

  BasicMove *horned_falcon_moves[] = {&smalllionmove_2, &longmove_755, NULL};
  Piece horned_falcon = { "Horned Falcon", "+DH", NULL, NULL, 26, 0, 0, horned_falcon_moves};

  BasicMove *soaring_eagle_moves[] = {&smalllionmove_5, &longmove_752, NULL};
  Piece soaring_eagle = { "Soaring Eagle", "+DK", NULL, NULL, 27, 0, 0, soaring_eagle_moves};

  BasicMove *whale_moves[] = {&longmove_702, NULL};
  Piece whale = { "Whale", "+RC", NULL, NULL, 28, 0, 0, whale_moves};

  BasicMove *white_horse_moves[] = {&longmove_207, NULL};
  Piece white_horse = { "White Horse", "+L", NULL, NULL, 29, 0, 0, white_horse_moves};

  BasicMove *prom_silver_general_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece prom_silver_general = { "Promoted Silver General", "+S", NULL, NULL, 30, 0, 0, prom_silver_general_moves};

  BasicMove *prom_gold_general_moves[] = {&longmove_252, NULL};
  Piece prom_gold_general = { "Promoted Gold General", "+G", NULL, NULL, 31, 0, 0, prom_gold_general_moves};

  BasicMove *prom_copper_general_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece prom_copper_general = { "Promoted Copper General", "+C", NULL, NULL, 32, 0, 0, prom_copper_general_moves};

  BasicMove *prom_bishop_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece prom_bishop = { "Promoted Bishop", "+B", NULL, NULL, 33, 0, 0, prom_bishop_moves};

  BasicMove *prom_rook_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece prom_rook = { "Promoted Rook", "+R", NULL, NULL, 34, 0, 0, prom_rook_moves};

  BasicMove *prom_ferocious_leopard_moves[] = {&longmove_505, NULL};
  Piece prom_ferocious_leopard = { "Promoted Ferocious Leopard", "+FL", NULL, NULL, 35, 0, 0, prom_ferocious_leopard_moves};

  BasicMove *prom_go_between_moves[] = {&shortmove_557, NULL};
  Piece prom_go_between = { "Promoted Go Between", "+GB", NULL, NULL, 36, 0, 0, prom_go_between_moves};

  const char *names[] = { "+B", "+BT", "+C", "+DE", "+DH", "+DK", "+FL", "+G", "+GB", "+L", "+P", "+R", "+RC", "+S", "+SM", "+VM", "B", "BT", "C", "DE", "DH", "DK", "FK", "FL", "G", "GB", "K", "Ky", "L", "Ln", "P", "Ph", "R", "RC", "S", "SM", "VM" };

  int name2pos[] = { 33, 23, 32, 22, 26, 27, 35, 31, 36, 29, 21, 34, 28, 30, 25, 24, 6, 8, 5, 11, 9, 10, 13, 12, 4, 14, 1, 15, 2, 17, 0, 16, 7, 18, 3, 19, 20};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &blind_tiger, &dragon_horse, &dragon_king, &drunk_elephant, &ferocious_leopard, &free_king, &go_between, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &tokin, &crown_prince, &flying_stag, &flying_ox, &free_boar, &horned_falcon, &soaring_eagle, &whale, &white_horse, &prom_silver_general, &prom_gold_general, &prom_copper_general, &prom_bishop, &prom_rook, &prom_ferocious_leopard, &prom_go_between};

  Board::Square setup[] = { 
     {2, 1}, {12, 1}, {5, 1}, {3, 1}, {4, 1}, {11, 1}, {1, 1}, {4, 1}, {3, 1}, {5, 1}, {12, 1}, {2, 1}, 
     {18, 1}, {0, 2}, {6, 1}, {0, 2}, {8, 1}, {16, 1}, {15, 1}, {8, 1}, {0, 2}, {6, 1}, {0, 2}, {18, 1}, 
     {19, 1}, {20, 1}, {7, 1}, {9, 1}, {10, 1}, {13, 1}, {17, 1}, {10, 1}, {9, 1}, {7, 1}, {20, 1}, {19, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {14, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 1}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {14, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 0}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {19, 0}, {20, 0}, {7, 0}, {9, 0}, {10, 0}, {17, 0}, {13, 0}, {10, 0}, {9, 0}, {7, 0}, {20, 0}, {19, 0}, 
     {18, 0}, {0, 2}, {6, 0}, {0, 2}, {8, 0}, {15, 0}, {16, 0}, {8, 0}, {0, 2}, {6, 0}, {0, 2}, {18, 0}, 
     {2, 0}, {12, 0}, {5, 0}, {3, 0}, {4, 0}, {1, 0}, {11, 0}, {4, 0}, {3, 0}, {5, 0}, {12, 0}, {2, 0}};

}

Board::Type chu_type = {
   pieces, setup, 4, Board::RESTRICTED_LION | Board::PROMOTION_RESTRICT | Board::PAWN_PROM_RESTRICT, 12, 12, 37, names, name2pos, 3, "Chu Shogi", 12};
