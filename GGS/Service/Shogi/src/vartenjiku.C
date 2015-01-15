#include "ShogiImpl.H"

namespace
{
  extern Piece tokin, white_horse, prom_silver_general, prom_gold_general, prom_copper_general, prom_bishop, prom_rook, flying_stag, prom_dragon_horse, prom_dragon_king, crown_prince, prom_ferocious_leopard, free_eagle, prom_knight, prom_kylin, prom_phoenix, lion_hawk, whale, free_boar, flying_ox, multi_general, prom_horned_falcon, prom_soaring_eagle, prom_iron_general, heavenly_tetrarchs, fire_demon, prom_vertical_soldier, prom_side_soldier, vice_general, great_general;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL, 0, 0, Piece::PAWN, pawn_moves};

  BasicMove *king_moves[] = {&shortmove_757, NULL};
  Piece king = { "King", "K", NULL, NULL, 1, 4, Piece::KING, king_moves};

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
  Piece dragon_horse = { "Dragon Horse", "DH", &prom_dragon_horse, NULL, 9, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "DK", &prom_dragon_king, NULL, 10, 0, 0, dragon_king_moves};

  BasicMove *drunk_elephant_moves[] = {&shortmove_557, NULL};
  Piece drunk_elephant = { "Drunk Elephant", "DE", &crown_prince, NULL, 11, 0, 0, drunk_elephant_moves};

  BasicMove *ferocious_leopard_moves[] = {&shortmove_707, NULL};
  Piece ferocious_leopard = { "Ferocious Leopard", "FL", &prom_ferocious_leopard, NULL, 12, 0, 0, ferocious_leopard_moves};

  BasicMove *free_king_moves[] = {&longmove_757, NULL};
  Piece free_king = { "Free King", "FK", &free_eagle, NULL, 13, 0, 0, free_king_moves};

  BasicMove *knight_moves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &prom_knight, NULL, 14, 0, 0, knight_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &prom_kylin, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &prom_phoenix, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", &lion_hawk, NULL, 17, 0, 0, lion_moves};

  BasicMove *reverse_chariot_moves[] = {&longmove_202, NULL};
  Piece reverse_chariot = { "Reverse Chariot", "RC", &whale, NULL, 18, 0, 0, reverse_chariot_moves};

  BasicMove *side_mover_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece side_mover = { "Side Mover", "SM", &free_boar, NULL, 19, 0, 0, side_mover_moves};

  BasicMove *vertical_mover_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece vertical_mover = { "Vertical Mover", "VM", &flying_ox, NULL, 20, 0, 0, vertical_mover_moves};

  BasicMove *tokin_moves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, NULL, 21, 0, 0, tokin_moves};

  BasicMove *crown_prince_moves[] = {&shortmove_757, NULL};
  Piece crown_prince = { "Crown Prince", "CP", NULL, NULL, 22, 0, Piece::KING, crown_prince_moves};

  BasicMove *flying_stag_moves[] = {&shortmove_555, &longmove_202, NULL};
  Piece flying_stag = { "Flying Stag", "FS", NULL, NULL, 23, 0, 0, flying_stag_moves};

  BasicMove *flying_ox_moves[] = {&longmove_707, NULL};
  Piece flying_ox = { "Flying Ox", "FO", NULL, NULL, 24, 0, 0, flying_ox_moves};

  BasicMove *free_boar_moves[] = {&longmove_555, NULL};
  Piece free_boar = { "Free Boar", "FBo", NULL, NULL, 25, 0, 0, free_boar_moves};

  BasicMove *whale_moves[] = {&longmove_702, NULL};
  Piece whale = { "Whale", "W", NULL, NULL, 26, 0, 0, whale_moves};

  BasicMove *white_horse_moves[] = {&longmove_207, NULL};
  Piece white_horse = { "White Horse", "WH", NULL, NULL, 27, 0, 0, white_horse_moves};

  BasicMove *dog_moves[] = {&shortmove_502, NULL};
  Piece dog = { "Dog", "Dg", &multi_general, NULL, 28, 0, 0, dog_moves};

  BasicMove *free_eagle_moves[] = {&jumpmove_252, &longmove_757, NULL};
  Piece free_eagle = { "Free Eagle", "FEg", NULL, NULL, 29, 0, 0, free_eagle_moves};

  BasicMove *horned_falcon_moves[] = {&smalllionmove_2, &longmove_755, NULL};
  Piece horned_falcon = { "Horned Falcon", "HF", &prom_horned_falcon, NULL, 30, 0, 0, horned_falcon_moves};

  BasicMove *soaring_eagle_moves[] = {&smalllionmove_5, &longmove_752, NULL};
  Piece soaring_eagle = { "Soaring Eagle", "SE", &prom_soaring_eagle, NULL, 31, 0, 0, soaring_eagle_moves};

  BasicMove *multi_general_moves[] = {&longmove_502, NULL};
  Piece multi_general = { "Multi General", "MGn", NULL, NULL, 32, 0, 0, multi_general_moves};

  BasicMove *lion_hawk_moves[] = {&longmove_505, &areamove_2, NULL};
  Piece lion_hawk = { "Lion Hawk", "LHk", NULL, NULL, 33, 0, 0, lion_hawk_moves};

  BasicMove *lion_hawk2_moves[] = {&longmove_505, &lionmove_2, NULL};
  Piece lion_hawk2 = { "Lion Hawk2", "LHk", NULL, NULL, 33, 0, 0, lion_hawk2_moves};

  BasicMove *iron_general_moves[] = {&shortmove_7, NULL};
  Piece iron_general = { "Iron General", "I", &prom_iron_general, NULL, 34, 0, 0, iron_general_moves};

  BasicMove *chariot_soldier_moves[] = {&nummove_50_2, &longmove_707, NULL};
  Piece chariot_soldier = { "Chariot Soldier", "ChS", &heavenly_tetrarchs, NULL, 35, 0, 0, chariot_soldier_moves};

  BasicMove *heavenly_tetrarchs_moves[] = {&moveovermove_505_64, &moveovermove_50_3, &iguimove, NULL};
  Piece heavenly_tetrarchs = { "Heavenly Tetrarchs", "HT", NULL, NULL, 36, 0, 0, heavenly_tetrarchs_moves};

  BasicMove *water_buffalo_moves[] = {&nummove_202_2, &longmove_555, NULL};
  Piece water_buffalo = { "Water Buffalo", "WBf", &fire_demon, NULL, 37, 0, 0, water_buffalo_moves};

  BasicMove *fire_demon_moves[] = {&longmove_707, &areamove_3, NULL};
  Piece fire_demon = { "Fire Demon", "FiD", NULL, NULL, 38, 0, Piece::FIREDEMON, fire_demon_moves};

  BasicMove *vertical_soldier_moves[] = {&shortmove_200, &nummove_50_2, &longmove_2, NULL};
  Piece vertical_soldier = { "Vertical Soldier", "VSo", &prom_vertical_soldier, NULL, 39, 0, 0, vertical_soldier_moves};

  BasicMove *side_soldier_moves[] = {&shortmove_200, &nummove_2_2, &longmove_50, NULL};
  Piece side_soldier = { "Side Soldier", "SSo", &prom_side_soldier, NULL, 40, 0, 0, side_soldier_moves};

  BasicMove *bishop_general_moves[] = {&longmove_505, &generalmove_505_1, NULL};
  Piece bishop_general = { "Bishop General", "BGn", &vice_general, NULL, 41, 1, 0, bishop_general_moves};

  BasicMove *vice_general_moves[] = {&longmove_505, &generalmove_505_2, &areamove_3, NULL};
  Piece vice_general = { "Vice General", "VGn", NULL, NULL, 42, 2, 0, vice_general_moves};

  BasicMove *rook_general_moves[] = {&longmove_252, &generalmove_252_1, NULL};
  Piece rook_general = { "Rook General", "RGn", &great_general, NULL, 43, 1, 0, rook_general_moves};

  BasicMove *great_general_moves[] = {&longmove_757, &generalmove_757_3, NULL};
  Piece great_general = { "Great General", "GGn", NULL, NULL, 44, 3, 0, great_general_moves};

  BasicMove *prom_silver_general_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece prom_silver_general = { "Promoted Silver General", "+S", NULL, NULL, 45, 0, 0, prom_silver_general_moves};

  BasicMove *prom_gold_general_moves[] = {&longmove_252, NULL};
  Piece prom_gold_general = { "Promoted Gold General", "+G", NULL, NULL, 46, 0, 0, prom_gold_general_moves};

  BasicMove *prom_copper_general_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece prom_copper_general = { "Promoted Copper General", "+C", NULL, NULL, 47, 0, 0, prom_copper_general_moves};

  BasicMove *prom_bishop_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece prom_bishop = { "Promoted Bishop", "+B", NULL, NULL, 48, 0, 0, prom_bishop_moves};

  BasicMove *prom_rook_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece prom_rook = { "Promoted Rook", "+R", NULL, NULL, 49, 0, 0, prom_rook_moves};

  BasicMove *prom_dragon_horse_moves[] = {&smalllionmove_2, &longmove_755, NULL};
  Piece prom_dragon_horse = { "Promoted Dragon Horse", "+DH", NULL, NULL, 50, 0, 0, prom_dragon_horse_moves};

  BasicMove *prom_dragon_king_moves[] = {&smalllionmove_5, &longmove_752, NULL};
  Piece prom_dragon_king = { "Promoted Dragon King", "+DK", NULL, NULL, 51, 0, 0, prom_dragon_king_moves};

  BasicMove *prom_ferocious_leopard_moves[] = {&longmove_505, NULL};
  Piece prom_ferocious_leopard = { "Promoted Ferocious Leopard", "+FL", NULL, NULL, 52, 0, 0, prom_ferocious_leopard_moves};

  BasicMove *prom_knight_moves[] = {&shortmove_200, &nummove_2_2, &longmove_50, NULL};
  Piece prom_knight = { "Promoted Knight", "+N", NULL, NULL, 53, 0, 0, prom_knight_moves};

  BasicMove *prom_kylin_moves[] = {&lionmove_2, NULL};
  Piece prom_kylin = { "Promoted Kylin", "+Ky", NULL, NULL, 54, 0, 0, prom_kylin_moves};

  BasicMove *prom_phoenix_moves[] = {&longmove_757, NULL};
  Piece prom_phoenix = { "Promoted Phoenix", "+Ph", NULL, NULL, 55, 0, 0, prom_phoenix_moves};

  BasicMove *prom_horned_falcon_moves[] = {&longmove_505, &generalmove_505_1, NULL};
  Piece prom_horned_falcon = { "Promoted Horned Falcon", "+HF", NULL, NULL, 56, 1, 0, prom_horned_falcon_moves};

  BasicMove *prom_soaring_eagle_moves[] = {&longmove_252, &generalmove_252_1, NULL};
  Piece prom_soaring_eagle = { "Promoted Soaring Eagle", "+SE", NULL, NULL, 57, 1, 0, prom_soaring_eagle_moves};

  BasicMove *prom_iron_general_moves[] = {&shortmove_200, &nummove_50_2, &longmove_2, NULL};
  Piece prom_iron_general = { "Promoted Iron General", "+I", NULL, NULL, 58, 0, 0, prom_iron_general_moves};

  BasicMove *prom_vertical_soldier_moves[] = {&nummove_50_2, &longmove_707, NULL};
  Piece prom_vertical_soldier = { "Promoted Vertical Soldier", "+VSo", NULL, NULL, 59, 0, 0, prom_vertical_soldier_moves};

  BasicMove *prom_side_soldier_moves[] = {&nummove_202_2, &longmove_555, NULL};
  Piece prom_side_soldier = { "Promoted Side Soldier", "+SSo", NULL, NULL, 60, 0, 0, prom_side_soldier_moves};

  const char *names[] = { "+B", "+C", "+DH", "+DK", "+FL", "+G", "+HF", "+I", "+Ky", "+N", "+P", "+Ph", "+R", "+S", "+SE", "+SSo", "+VSo", "B", "BGn", "BT", "C", "CP", "ChS", "DE", "DH", "DK", "Dg", "FBo", "FEg", "FK", "FL", "FO", "FS", "FiD", "G", "GGn", "HF", "HT", "I", "K", "Ky", "L", "LHk", "Ln", "MGn", "N", "P", "Ph", "R", "RC", "RGn", "S", "SE", "SM", "SSo", "VGn", "VM", "VSo", "W", "WBf", "WH" };

  int name2pos[] = { 48, 47, 50, 51, 52, 46, 56, 58, 54, 53, 21, 55, 49, 45, 57, 60, 59, 6, 41, 8, 5, 22, 35, 11, 9, 10, 28, 25, 29, 13, 12, 24, 23, 38, 4, 44, 30, 36, 34, 1, 15, 2, 33, 17, 32, 14, 0, 16, 7, 18, 43, 3, 31, 19, 40, 42, 20, 39, 26, 37, 27};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &blind_tiger, &dragon_horse, &dragon_king, &drunk_elephant, &ferocious_leopard, &free_king, &knight, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &tokin, &crown_prince, &flying_stag, &flying_ox, &free_boar, &whale, &white_horse, &dog, &free_eagle, &horned_falcon, &soaring_eagle, &multi_general, &lion_hawk, &iron_general, &chariot_soldier, &heavenly_tetrarchs, &water_buffalo, &fire_demon, &vertical_soldier, &side_soldier, &bishop_general, &vice_general, &rook_general, &great_general, &prom_silver_general, &prom_gold_general, &prom_copper_general, &prom_bishop, &prom_rook, &prom_dragon_horse, &prom_dragon_king, &prom_ferocious_leopard, &prom_knight, &prom_kylin, &prom_phoenix, &prom_horned_falcon, &prom_soaring_eagle, &prom_iron_general, &prom_vertical_soldier, &prom_side_soldier};

    const Piece *pieces2[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &blind_tiger, &dragon_horse, &dragon_king, &drunk_elephant, &ferocious_leopard, &free_king, &knight, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &tokin, &crown_prince, &flying_stag, &flying_ox, &free_boar, &whale, &white_horse, &dog, &free_eagle, &horned_falcon, &soaring_eagle, &multi_general, &lion_hawk2, &iron_general, &chariot_soldier, &heavenly_tetrarchs, &water_buffalo, &fire_demon, &vertical_soldier, &side_soldier, &bishop_general, &vice_general, &rook_general, &great_general, &prom_silver_general, &prom_gold_general, &prom_copper_general, &prom_bishop, &prom_rook, &prom_dragon_horse, &prom_dragon_king, &prom_ferocious_leopard, &prom_knight, &prom_kylin, &prom_phoenix, &prom_horned_falcon, &prom_soaring_eagle, &prom_iron_general, &prom_vertical_soldier, &prom_side_soldier};

  Board::Square setup[] = {
     {2, 1}, {14, 1}, {12, 1}, {34, 1}, {5, 1}, {3, 1}, {4, 1}, {11, 1}, {1, 1}, {4, 1}, {3, 1}, {5, 1}, {34, 1}, {12, 1}, {14, 1}, {2, 1}, 
     {18, 1}, {0, 2}, {35, 1}, {35, 1}, {0, 2}, {8, 1}, {16, 1}, {13, 1}, {17, 1}, {15, 1}, {8, 1}, {0, 2}, {35, 1}, {35, 1}, {0, 2}, {18, 1}, 
     {40, 1}, {39, 1}, {6, 1}, {9, 1}, {10, 1}, {37, 1}, {38, 1}, {29, 1}, {33, 1}, {38, 1}, {37, 1}, {10, 1}, {9, 1}, {6, 1}, {39, 1}, {40, 1}, 
     {19, 1}, {20, 1}, {7, 1}, {30, 1}, {31, 1}, {41, 1}, {43, 1}, {42, 1}, {44, 1}, {43, 1}, {41, 1}, {31, 1}, {30, 1}, {7, 1}, {20, 1}, {19, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {28, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {28, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {28, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {28, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {19, 0}, {20, 0}, {7, 0}, {30, 0}, {31, 0}, {41, 0}, {43, 0}, {44, 0}, {42, 0}, {43, 0}, {41, 0}, {31, 0}, {30, 0}, {7, 0}, {20, 0}, {19, 0}, 
     {40, 0}, {39, 0}, {6, 0}, {9, 0}, {10, 0}, {37, 0}, {38, 0}, {33, 0}, {29, 0}, {38, 0}, {37, 0}, {10, 0}, {9, 0}, {6, 0}, {39, 0}, {40, 0}, 
     {18, 0}, {0, 2}, {35, 0}, {35, 0}, {0, 2}, {8, 0}, {15, 0}, {17, 0}, {13, 0}, {16, 0}, {8, 0}, {0, 2}, {35, 0}, {35, 0}, {0, 2}, {18, 0}, 
     {2, 0}, {14, 0}, {12, 0}, {34, 0}, {5, 0}, {3, 0}, {4, 0}, {1, 0}, {11, 0}, {4, 0}, {3, 0}, {5, 0}, {34, 0}, {12, 0}, {14, 0}, {2, 0}};

}

Board::Type tenjiku_type = {
   pieces, setup, 5, Board::HAS_FIREDEMON | Board::PROMOTION_RESTRICT, 16, 16, 61, names, name2pos, 3, "Tenjiku Shogi", 14};

Board::Type tenjiku2_type = {
   pieces2, setup, 5, Board::HAS_FIREDEMON | Board::PROMOTION_RESTRICT, 16, 16, 61, names, name2pos, 3, "Tenjiku Shogi (Modern Lion-Hawk)", 15};
