#include "ShogiImpl.H"

namespace
{
  extern Piece mountain_witch, great_dragon, golden_bird, furious_fiend, prom_western_barbarian, prom_eastern_barbarian, she_devil, great_elephant, standard_bearer, wizard_stork, square_mover, white_elephant;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", NULL, NULL, 0, 0, Piece::PAWN, pawn_moves};

  BasicMove *king_moves[] = {&shortmove_757, NULL};
  Piece king = { "King", "K", NULL, NULL, 1, 0, Piece::KING, king_moves};

  BasicMove *lance_moves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", NULL, NULL, 2, 0, 0, lance_moves};

  BasicMove *silver_general_moves[] = {&shortmove_507, NULL};
  Piece silver_general = { "Silver General", "S", NULL, NULL, 3, 0, 0, silver_general_moves};

  BasicMove *gold_general_moves[] = {&shortmove_257, NULL};
  Piece gold_general = { "Gold General", "G", NULL, NULL, 4, 0, 0, gold_general_moves};

  BasicMove *copper_general_moves[] = {&shortmove_207, NULL};
  Piece copper_general = { "Copper General", "C", NULL, NULL, 5, 0, 0, copper_general_moves};

  BasicMove *bishop_moves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", NULL, NULL, 6, 0, 0, bishop_moves};

  BasicMove *rook_moves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", NULL, NULL, 7, 0, 0, rook_moves};

  BasicMove *angry_boar_moves[] = {&shortmove_252, NULL};
  Piece angry_boar = { "Angry Boar", "AB", NULL, NULL, 8, 0, 0, angry_boar_moves};

  BasicMove *dragon_horse_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece dragon_horse = { "Dragon Horse", "DH", NULL, NULL, 9, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "DK", NULL, NULL, 10, 0, 0, dragon_king_moves};

  BasicMove *blind_monkey_moves[] = {&shortmove_555, NULL};
  Piece blind_monkey = { "Blind Monkey", "BM", &mountain_witch, NULL, 11, 0, 0, blind_monkey_moves};

  BasicMove *ferocious_leopard_moves[] = {&shortmove_707, NULL};
  Piece ferocious_leopard = { "Ferocious Leopard", "FL", NULL, NULL, 12, 0, 0, ferocious_leopard_moves};

  BasicMove *free_king_moves[] = {&longmove_757, NULL};
  Piece free_king = { "Free King", "FK", NULL, NULL, 13, 0, 0, free_king_moves};

  BasicMove *blue_dragon_moves[] = {&shortmove_1, &nummove_202_2, &longmove_54, NULL};
  Piece blue_dragon = { "Blue Dragon", "BD", NULL, NULL, 14, 0, 0, blue_dragon_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &great_dragon, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &golden_bird, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", &furious_fiend, NULL, 17, 0, 0, lion_moves};

  BasicMove *reverse_chariot_moves[] = {&longmove_202, NULL};
  Piece reverse_chariot = { "Reverse Chariot", "RC", NULL, NULL, 18, 0, 0, reverse_chariot_moves};

  BasicMove *side_mover_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece side_mover = { "Side Mover", "SM", NULL, NULL, 19, 0, 0, side_mover_moves};

  BasicMove *vertical_mover_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece vertical_mover = { "Vertical Mover", "VM", NULL, NULL, 20, 0, 0, vertical_mover_moves};

  BasicMove *western_barbarian_moves[] = {&shortmove_207, &nummove_50_2, NULL};
  Piece western_barbarian = { "Western Barbarian", "WB", &prom_western_barbarian, NULL, 21, 0, 0, western_barbarian_moves};

  BasicMove *cat_sword_moves[] = {&shortmove_505, NULL};
  Piece cat_sword = { "Cat Sword", "CSw", &dragon_horse, NULL, 22, 0, 0, cat_sword_moves};

  BasicMove *dove_moves[] = {&nummove_505_5, &nummove_252_2, NULL};
  Piece dove = { "Dove", "D", NULL, NULL, 23, 0, 0, dove_moves};

  BasicMove *eastern_barbarian_moves[] = {&shortmove_55, &nummove_202_2, NULL};
  Piece eastern_barbarian = { "Eastern Barbarian", "EB", &prom_eastern_barbarian, NULL, 24, 0, 0, eastern_barbarian_moves};

  BasicMove *enchanted_badger_moves[] = {&nummove_52_2, NULL};
  Piece enchanted_badger = { "Enchanted Badger", "EBa", &dove, NULL, 25, 0, 0, enchanted_badger_moves};

  BasicMove *enchanted_fox_moves[] = {&nummove_205_2, NULL};
  Piece enchanted_fox = { "Enchanted Fox", "EF", &she_devil, NULL, 26, 0, 0, enchanted_fox_moves};

  BasicMove *evil_wolf_moves[] = {&shortmove_57, NULL};
  Piece evil_wolf = { "Evil Wolf", "EW", NULL, NULL, 27, 0, 0, evil_wolf_moves};

  BasicMove *side_chariot_moves[] = {&shortmove_500, &longmove_252, NULL};
  Piece side_chariot = { "Side Chariot", "FCh", NULL, NULL, 28, 0, 0, side_chariot_moves};

  BasicMove *flying_dragon_moves[] = {&nummove_505_2, NULL};
  Piece flying_dragon = { "Flying Dragon", "FD", &dragon_king, NULL, 29, 0, 0, flying_dragon_moves};

  BasicMove *flying_horse_moves[] = {&shortmove_252, &nummove_5_2, NULL};
  Piece flying_horse = { "Flying Horse", "FH", &free_king, NULL, 30, 0, 0, flying_horse_moves};

  BasicMove *fragrant_elephant_moves[] = {&nummove_752_2, &longmove_5, NULL};
  Piece fragrant_elephant = { "Fragrant Elephant", "FEl", NULL, NULL, 31, 0, 0, fragrant_elephant_moves};

  BasicMove *free_demon_moves[] = {&nummove_202_5, &longmove_555, NULL};
  Piece free_demon = { "Free Demon", "FDe", NULL, NULL, 32, 0, 0, free_demon_moves};

  BasicMove *free_tapir_moves[] = {&nummove_50_5, &longmove_707, NULL};
  Piece free_tapir = { "Free Tapir", "FTp", NULL, NULL, 33, 0, 0, free_tapir_moves};

  BasicMove *golden_bird_moves[] = {&nummove_50_2, &nummove_505_3, &longmove_202, NULL};
  Piece golden_bird = { "Golden Bird", "GBd", NULL, NULL, 34, 0, 0, golden_bird_moves};

  BasicMove *great_dragon_moves[] = {&nummove_202_2, &nummove_505_3, &longmove_50, NULL};
  Piece great_dragon = { "Great Dragon", "GD", NULL, NULL, 35, 0, 0, great_dragon_moves};

  BasicMove *hook_mover_moves[] = {&hookmove, NULL};
  Piece hook_mover = { "Hook Mover", "HM", NULL, NULL, 36, 0, 0, hook_mover_moves};

  BasicMove *howling_dog_moves[] = {&shortmove_200, &longmove_2, NULL};
  Piece howling_dog = { "Howling Dog", "HD", NULL, NULL, 37, 0, 0, howling_dog_moves};

  BasicMove *iron_general_moves[] = {&shortmove_7, NULL};
  Piece iron_general = { "Iron General", "I", NULL, NULL, 38, 0, 0, iron_general_moves};

  BasicMove *left_chariot_moves[] = {&shortmove_200, &longmove_403, NULL};
  Piece left_chariot = { "Left Chariot", "LCh", NULL, NULL, 39, 0, 0, left_chariot_moves};

  BasicMove *left_general_moves[] = {&shortmove_747, NULL};
  Piece left_general = { "Left General", "LG", NULL, NULL, 40, 0, 0, left_general_moves};

  BasicMove *lion_dog_moves[] = {&nummove_757_3, NULL};
  Piece lion_dog = { "Lion Dog", "LD", &great_elephant, NULL, 41, 0, 0, lion_dog_moves};

  BasicMove *long_nosed_goblin_moves[] = {&shortmove_252, &diaghookmove_505, NULL};
  Piece long_nosed_goblin = { "Long Nosed Goblin", "LGn", NULL, NULL, 42, 0, 0, long_nosed_goblin_moves};

  BasicMove *neighbouring_king_moves[] = {&shortmove_557, NULL};
  Piece neighbouring_king = { "Neighbouring King", "NK", &standard_bearer, NULL, 43, 0, 0, neighbouring_king_moves};

  BasicMove *northern_barbarian_moves[] = {&shortmove_550, &nummove_5_2, NULL};
  Piece northern_barbarian = { "Northern Barbarian", "NB", &fragrant_elephant, NULL, 44, 0, 0, northern_barbarian_moves};

  BasicMove *old_kite_hawk_moves[] = {&shortmove_5, &nummove_252_2, NULL};
  Piece old_kite_hawk = { "Old Kite Hawk", "OK", &long_nosed_goblin, NULL, 45, 0, 0, old_kite_hawk_moves};

  BasicMove *old_rat_moves[] = {&nummove_205_2, NULL};
  Piece old_rat = { "Old Rat", "OR", &wizard_stork, NULL, 46, 0, 0, old_rat_moves};

  BasicMove *poisonous_snake_moves[] = {&shortmove_50, &jumpmove_502, NULL};
  Piece poisonous_snake = { "Poisonous Snake", "PS", &hook_mover, NULL, 47, 0, 0, poisonous_snake_moves};

  BasicMove *prancing_stag_moves[] = {&shortmove_507, &nummove_50_2, NULL};
  Piece prancing_stag = { "Prancing Stag", "PSt", &square_mover, NULL, 48, 0, 0, prancing_stag_moves};

  BasicMove *right_chariot_moves[] = {&shortmove_200, &longmove_106, NULL};
  Piece right_chariot = { "Right Chariot", "RCh", NULL, NULL, 49, 0, 0, right_chariot_moves};

  BasicMove *right_general_moves[] = {&shortmove_717, NULL};
  Piece right_general = { "Right General", "RG", NULL, NULL, 50, 0, 0, right_general_moves};

  BasicMove *rushing_bird_moves[] = {&longmove_557, NULL};
  Piece rushing_bird = { "Rushing Bird", "RB", &free_demon, NULL, 51, 0, 0, rushing_bird_moves};

  BasicMove *savage_tiger_moves[] = {&shortmove_5, &nummove_202_2, NULL};
  Piece savage_tiger = { "Savage Tiger", "ST", NULL, NULL, 52, 0, 0, savage_tiger_moves};

  BasicMove *she_devil_moves[] = {&nummove_252_5, &nummove_505_2, NULL};
  Piece she_devil = { "She Devil", "SD", NULL, NULL, 53, 0, 0, she_devil_moves};

  BasicMove *southern_barbarian_moves[] = {&shortmove_55, &nummove_500_2, NULL};
  Piece southern_barbarian = { "Southern Barbarian", "SB", &white_elephant, NULL, 54, 0, 0, southern_barbarian_moves};

  BasicMove *square_mover_moves[] = {&shortmove_5, &longmove_252, NULL};
  Piece square_mover = { "Square Mover", "SMo", NULL, NULL, 55, 0, 0, square_mover_moves};

  BasicMove *standard_bearer_moves[] = {&nummove_750_2, &longmove_7, NULL};
  Piece standard_bearer = { "Standard Bearer", "SBr", NULL, NULL, 56, 0, 0, standard_bearer_moves};

  BasicMove *stone_general_moves[] = {&shortmove_5, NULL};
  Piece stone_general = { "Stone General", "St", NULL, NULL, 57, 0, 0, stone_general_moves};

  BasicMove *violent_bear_moves[] = {&shortmove_50, &nummove_5_2, NULL};
  Piece violent_bear = { "Violent Bear", "VBe", NULL, NULL, 58, 0, 0, violent_bear_moves};

  BasicMove *violent_ox_moves[] = {&nummove_252_2, NULL};
  Piece violent_ox = { "Violent Ox", "VO", NULL, NULL, 59, 0, 0, violent_ox_moves};

  BasicMove *water_buffalo_moves[] = {&nummove_202_2, &longmove_555, NULL};
  Piece water_buffalo = { "Water Buffalo", "WBf", &free_tapir, NULL, 60, 0, 0, water_buffalo_moves};

  BasicMove *white_elephant_moves[] = {&nummove_257_2, &longmove_500, NULL};
  Piece white_elephant = { "White Elephant", "WE", NULL, NULL, 61, 0, 0, white_elephant_moves};

  BasicMove *white_tiger_moves[] = {&shortmove_4, &nummove_50_2, &longmove_203, NULL};
  Piece white_tiger = { "White Tiger", "WT", NULL, NULL, 62, 0, 0, white_tiger_moves};

  BasicMove *wood_general_moves[] = {&nummove_5_2, NULL};
  Piece wood_general = { "Wood General", "Wo", NULL, NULL, 63, 0, 0, wood_general_moves};

  BasicMove *furious_fiend_moves[] = {&nummove_757_3, &lionmove_2, NULL};
  Piece furious_fiend = { "Furious Fiend", "FF", NULL, NULL, 64, 0, 0, furious_fiend_moves};

  BasicMove *great_elephant_moves[] = {&nummove_550_5, &nummove_207_3, NULL};
  Piece great_elephant = { "Great Elephant", "GE", NULL, NULL, 65, 0, 0, great_elephant_moves};

  BasicMove *mountain_witch_moves[] = {&shortmove_2, &longmove_705, NULL};
  Piece mountain_witch = { "Mountain Witch", "MW", NULL, NULL, 66, 0, 0, mountain_witch_moves};

  BasicMove *wizard_stork_moves[] = {&shortmove_200, &longmove_507, NULL};
  Piece wizard_stork = { "Wizard Stork", "WS", NULL, NULL, 67, 0, 0, wizard_stork_moves};

  BasicMove *prom_western_barbarian_moves[] = {&nummove_757_3, NULL};
  Piece prom_western_barbarian = { "Promoted Western Barbarian", "+WB", NULL, NULL, 68, 0, 0, prom_western_barbarian_moves};

  BasicMove *prom_eastern_barbarian_moves[] = {&lionmove_2, NULL};
  Piece prom_eastern_barbarian = { "Promoted Eastern Barbarian", "+EB", NULL, NULL, 69, 0, 0, prom_eastern_barbarian_moves};

  const char *names[] = { "+EB", "+WB", "AB", "B", "BD", "BM", "C", "CSw", "D", "DH", "DK", "EB", "EBa", "EF", "EW", "FCh", "FD", "FDe", "FEl", "FF", "FH", "FK", "FL", "FTp", "G", "GBd", "GD", "GE", "HD", "HM", "I", "K", "Ky", "L", "LCh", "LD", "LG", "LGn", "Ln", "MW", "NB", "NK", "OK", "OR", "P", "PS", "PSt", "Ph", "R", "RB", "RC", "RCh", "RG", "S", "SB", "SBr", "SD", "SM", "SMo", "ST", "St", "VBe", "VM", "VO", "WB", "WBf", "WE", "WS", "WT", "Wo" };

  int name2pos[] = { 69, 68, 8, 6, 14, 11, 5, 22, 23, 9, 10, 24, 25, 26, 27, 28, 29, 32, 31, 64, 30, 13, 12, 33, 4, 34, 35, 65, 37, 36, 38, 1, 15, 2, 39, 41, 40, 42, 17, 66, 44, 43, 45, 46, 0, 47, 48, 16, 7, 51, 18, 49, 50, 3, 54, 56, 53, 19, 55, 52, 57, 58, 20, 59, 21, 60, 61, 67, 62, 63};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &angry_boar, &dragon_horse, &dragon_king, &blind_monkey, &ferocious_leopard, &free_king, &blue_dragon, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &western_barbarian, &cat_sword, &dove, &eastern_barbarian, &enchanted_badger, &enchanted_fox, &evil_wolf, &side_chariot, &flying_dragon, &flying_horse, &fragrant_elephant, &free_demon, &free_tapir, &golden_bird, &great_dragon, &hook_mover, &howling_dog, &iron_general, &left_chariot, &left_general, &lion_dog, &long_nosed_goblin, &neighbouring_king, &northern_barbarian, &old_kite_hawk, &old_rat, &poisonous_snake, &prancing_stag, &right_chariot, &right_general, &rushing_bird, &savage_tiger, &she_devil, &southern_barbarian, &square_mover, &standard_bearer, &stone_general, &violent_bear, &violent_ox, &water_buffalo, &white_elephant, &white_tiger, &wood_general, &furious_fiend, &great_elephant, &mountain_witch, &wizard_stork, &prom_western_barbarian, &prom_eastern_barbarian};

  Board::Square setup[] = {
     {2, 1}, {36, 1}, {23, 1}, {7, 1}, {55, 1}, {10, 1}, {33, 1}, {50, 1}, {1, 1}, {40, 1}, {13, 1}, {32, 1}, {9, 1}, {28, 1}, {53, 1}, {42, 1}, {2, 1}, 
     {18, 1}, {47, 1}, {41, 1}, {11, 1}, {29, 1}, {51, 1}, {15, 1}, {4, 1}, {43, 1}, {4, 1}, {16, 1}, {22, 1}, {48, 1}, {46, 1}, {17, 1}, {45, 1}, {18, 1}, 
     {0, 2}, {6, 1}, {0, 2}, {25, 1}, {0, 2}, {30, 1}, {0, 2}, {3, 1}, {35, 1}, {3, 1}, {0, 2}, {60, 1}, {0, 2}, {26, 1}, {0, 2}, {20, 1}, {0, 2}, 
     {62, 1}, {61, 1}, {54, 1}, {24, 1}, {63, 1}, {57, 1}, {38, 1}, {5, 1}, {34, 1}, {5, 1}, {38, 1}, {57, 1}, {63, 1}, {21, 1}, {44, 1}, {31, 1}, {14, 1}, 
     {49, 1}, {19, 1}, {59, 1}, {8, 1}, {27, 1}, {58, 1}, {12, 1}, {52, 1}, {56, 1}, {52, 1}, {12, 1}, {58, 1}, {27, 1}, {8, 1}, {59, 1}, {19, 1}, {39, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {37, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {37, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {37, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {37, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {39, 0}, {19, 0}, {59, 0}, {8, 0}, {27, 0}, {58, 0}, {12, 0}, {52, 0}, {56, 0}, {52, 0}, {12, 0}, {58, 0}, {27, 0}, {8, 0}, {59, 0}, {19, 0}, {49, 0}, 
     {14, 0}, {31, 0}, {44, 0}, {21, 0}, {63, 0}, {57, 0}, {38, 0}, {5, 0}, {34, 0}, {5, 0}, {38, 0}, {57, 0}, {63, 0}, {24, 0}, {54, 0}, {61, 0}, {62, 0}, 
     {0, 2}, {20, 0}, {0, 2}, {26, 0}, {0, 2}, {60, 0}, {0, 2}, {3, 0}, {35, 0}, {3, 0}, {0, 2}, {30, 0}, {0, 2}, {25, 0}, {0, 2}, {6, 0}, {0, 2}, 
     {18, 0}, {45, 0}, {17, 0}, {46, 0}, {48, 0}, {22, 0}, {16, 0}, {4, 0}, {43, 0}, {4, 0}, {15, 0}, {51, 0}, {29, 0}, {11, 0}, {41, 0}, {47, 0}, {18, 0}, 
     {2, 0}, {42, 0}, {53, 0}, {28, 0}, {9, 0}, {32, 0}, {13, 0}, {40, 0}, {1, 0}, {50, 0}, {33, 0}, {10, 0}, {55, 0}, {7, 0}, {23, 0}, {36, 0}, {2, 0}};
     
}

Board::Type daidai_type = {
   pieces, setup, 0, Board::PROM_BY_CAPTURE, 17, 17, 70, names, name2pos, 3, "Dai-Dai Shogi", 16};
