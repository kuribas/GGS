#include "ShogiImpl.H"

namespace
{
  
  extern Piece tokin, white_horse, prom_silver_general, prom_gold_general, prom_copper_general, prom_bishop, prom_rook, flying_stag, horned_falcon, soaring_eagle, crown_prince, prom_ferocious_leopard, prom_go_between, lion, whale, free_boar, flying_ox, prom_angry_boar, prom_cat_sword, prom_evil_wolf, prom_flying_dragon, prom_iron_general, prom_stone_general, prom_violent_ox, prom_knight;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL, 0, 0, Piece::PAWN, pawn_moves};

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
  Piece go_between = { "Go Between", "GB", &prom_go_between, NULL, 14, 0, 0, go_between_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &lion, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &free_king, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", NULL, NULL, 17, 0, 0, lion_moves};

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

  BasicMove *angry_boar_moves[] = {&shortmove_252, NULL};
  Piece angry_boar = { "Angry Boar", "AB", &prom_angry_boar, NULL, 30, 0, 0, angry_boar_moves};

  BasicMove *cat_sword_moves[] = {&shortmove_505, NULL};
  Piece cat_sword = { "Cat Sword", "CSw", &prom_cat_sword, NULL, 31, 0, 0, cat_sword_moves};

  BasicMove *evil_wolf_moves[] = {&shortmove_57, NULL};
  Piece evil_wolf = { "Evil Wolf", "EW", &prom_evil_wolf, NULL, 32, 0, 0, evil_wolf_moves};

  BasicMove *flying_dragon_moves[] = {&nummove_505_2, NULL};
  Piece flying_dragon = { "Flying Dragon", "FD", &prom_flying_dragon, NULL, 33, 0, 0, flying_dragon_moves};

  BasicMove *iron_general_moves[] = {&shortmove_7, NULL};
  Piece iron_general = { "Iron General", "I", &prom_iron_general, NULL, 34, 0, 0, iron_general_moves};

  BasicMove *stone_general_moves[] = {&shortmove_5, NULL};
  Piece stone_general = { "Stone General", "St", &prom_stone_general, NULL, 35, 0, 0, stone_general_moves};

  BasicMove *violent_ox_moves[] = {&nummove_252_2, NULL};
  Piece violent_ox = { "Violent Ox", "VO", &prom_violent_ox, NULL, 36, 0, 0, violent_ox_moves};

  BasicMove *knight_moves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &prom_knight, NULL, 37, 0, 0, knight_moves};

  BasicMove *prom_silver_general_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece prom_silver_general = { "Promoted Silver General", "+S", NULL, NULL, 38, 0, 0, prom_silver_general_moves};

  BasicMove *prom_gold_general_moves[] = {&longmove_252, NULL};
  Piece prom_gold_general = { "Promoted Gold General", "+G", NULL, NULL, 39, 0, 0, prom_gold_general_moves};

  BasicMove *prom_copper_general_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece prom_copper_general = { "Promoted Copper General", "+C", NULL, NULL, 40, 0, 0, prom_copper_general_moves};

  BasicMove *prom_bishop_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece prom_bishop = { "Promoted Bishop", "+B", NULL, NULL, 41, 0, 0, prom_bishop_moves};

  BasicMove *prom_rook_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece prom_rook = { "Promoted Rook", "+R", NULL, NULL, 42, 0, 0, prom_rook_moves};

  BasicMove *prom_ferocious_leopard_moves[] = {&longmove_505, NULL};
  Piece prom_ferocious_leopard = { "Promoted Ferocious Leopard", "+FL", NULL, NULL, 43, 0, 0, prom_ferocious_leopard_moves};

  BasicMove *prom_go_between_moves[] = {&shortmove_557, NULL};
  Piece prom_go_between = { "Promoted Go Between", "+GB", NULL, NULL, 44, 0, 0, prom_go_between_moves};

  BasicMove *prom_angry_boar_moves[] = {&shortmove_257, NULL};
  Piece prom_angry_boar = { "Promoted Angry Boar", "+AB", NULL, NULL, 45, 0, 0, prom_angry_boar_moves};

  BasicMove *prom_cat_sword_moves[] = {&shortmove_257, NULL};
  Piece prom_cat_sword = { "Promoted Cat Sword", "+CSw", NULL, NULL, 46, 0, 0, prom_cat_sword_moves};

  BasicMove *prom_evil_wolf_moves[] = {&shortmove_257, NULL};
  Piece prom_evil_wolf = { "Promoted Evil Wolf", "+EW", NULL, NULL, 47, 0, 0, prom_evil_wolf_moves};

  BasicMove *prom_flying_dragon_moves[] = {&shortmove_257, NULL};
  Piece prom_flying_dragon = { "Promoted Flying Dragon", "+FD", NULL, NULL, 48, 0, 0, prom_flying_dragon_moves};

  BasicMove *prom_iron_general_moves[] = {&shortmove_257, NULL};
  Piece prom_iron_general = { "Promoted Iron General", "+I", NULL, NULL, 49, 0, 0, prom_iron_general_moves};

  BasicMove *prom_stone_general_moves[] = {&shortmove_257, NULL};
  Piece prom_stone_general = { "Promoted Stone General", "+St", NULL, NULL, 50, 0, 0, prom_stone_general_moves};

  BasicMove *prom_violent_ox_moves[] = {&shortmove_257, NULL};
  Piece prom_violent_ox = { "Promoted Violent Ox", "+VO", NULL, NULL, 51, 0, 0, prom_violent_ox_moves};

  BasicMove *prom_knight_moves[] = {&shortmove_257, NULL};
  Piece prom_knight = { "Promoted Knight", "+N", NULL, NULL, 52, 0, 0, prom_knight_moves};

  const char *names[] = { "+AB", "+B", "+BT", "+C", "+CSw", "+DE", "+DH", "+DK", "+EW", "+FD", "+FL", "+G", "+GB", "+I", "+L", "+N", "+P", "+R", "+RC", "+S", "+SM", "+St", "+VM", "+VO", "AB", "B", "BT", "C", "CSw", "DE", "DH", "DK", "EW", "FD", "FK", "FL", "G", "GB", "I", "K", "Ky", "L", "Ln", "N", "P", "Ph", "R", "RC", "S", "SM", "St", "VM", "VO" };

  int name2pos[] = { 45, 41, 23, 40, 46, 22, 26, 27, 47, 48, 43, 39, 44, 49, 29, 52, 21, 42, 28, 38, 25, 50, 24, 51, 30, 6, 8, 5, 31, 11, 9, 10, 32, 33, 13, 12, 4, 14, 34, 1, 15, 2, 17, 37, 0, 16, 7, 18, 3, 19, 35, 20, 36};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &blind_tiger, &dragon_horse, &dragon_king, &drunk_elephant, &ferocious_leopard, &free_king, &go_between, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &tokin, &crown_prince, &flying_stag, &flying_ox, &free_boar, &horned_falcon, &soaring_eagle, &whale, &white_horse, &angry_boar, &cat_sword, &evil_wolf, &flying_dragon, &iron_general, &stone_general, &violent_ox, &knight, &prom_silver_general, &prom_gold_general, &prom_copper_general, &prom_bishop, &prom_rook, &prom_ferocious_leopard, &prom_go_between, &prom_angry_boar, &prom_cat_sword, &prom_evil_wolf, &prom_flying_dragon, &prom_iron_general, &prom_stone_general, &prom_violent_ox, &prom_knight};

  Board::Square setup[] = {
     {2, 1}, {37, 1}, {35, 1}, {34, 1}, {5, 1}, {3, 1}, {4, 1}, {1, 1}, {4, 1}, {3, 1}, {5, 1}, {34, 1}, {35, 1}, {37, 1}, {2, 1}, 
     {18, 1}, {0, 2}, {31, 1}, {0, 2}, {12, 1}, {0, 2}, {8, 1}, {11, 1}, {8, 1}, {0, 2}, {12, 1}, {0, 2}, {31, 1}, {0, 2}, {18, 1}, 
     {0, 2}, {36, 1}, {0, 2}, {30, 1}, {0, 2}, {32, 1}, {16, 1}, {17, 1}, {15, 1}, {32, 1}, {0, 2}, {30, 1}, {0, 2}, {36, 1}, {0, 2}, 
     {7, 1}, {33, 1}, {19, 1}, {20, 1}, {6, 1}, {9, 1}, {10, 1}, {13, 1}, {10, 1}, {9, 1}, {6, 1}, {20, 1}, {19, 1}, {33, 1}, {7, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {14, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {7, 0}, {33, 0}, {19, 0}, {20, 0}, {6, 0}, {9, 0}, {10, 0}, {13, 0}, {10, 0}, {9, 0}, {6, 0}, {20, 0}, {19, 0}, {33, 0}, {7, 0}, 
     {0, 2}, {36, 0}, {0, 2}, {30, 0}, {0, 2}, {32, 0}, {15, 0}, {17, 0}, {16, 0}, {32, 0}, {0, 2}, {30, 0}, {0, 2}, {36, 0}, {0, 2}, 
     {18, 0}, {0, 2}, {31, 0}, {0, 2}, {12, 0}, {0, 2}, {8, 0}, {11, 0}, {8, 0}, {0, 2}, {12, 0}, {0, 2}, {31, 0}, {0, 2}, {18, 0}, 
     {2, 0}, {37, 0}, {35, 0}, {34, 0}, {5, 0}, {3, 0}, {4, 0}, {1, 0}, {4, 0}, {3, 0}, {5, 0}, {34, 0}, {35, 0}, {37, 0}, {2, 0}};

     
}

Board::Type dai_type = {
   pieces, setup, 0, 0, 15, 15, 53, names, name2pos, 4, "Dai Shogi", 13};
