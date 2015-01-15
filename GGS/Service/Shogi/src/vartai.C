#include "ShogiImpl.H"

namespace
{
  extern Piece great_dragon, golden_bird, furious_fiend, wizard_stork, buddhist_spirit, teaching_king, crown_prince, great_elephant, wizard_stork, mountain_witch, prom_eastern_barbarian, standard_bearer, fragrant_elephant, white_elephant, prom_western_barbarian, prom_poisonous_snake, square_mover;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", NULL, NULL, 0, 0, Piece::PAWN, pawn_moves};

  BasicMove *emperor_moves[] = {&emperormove, NULL};
  Piece emperor = { "Emperor", "E", NULL, NULL, 1, 0, Piece::KING | Piece::EMPEROR, emperor_moves};

  BasicMove *lance_moves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", NULL, NULL, 2, 0, 0, lance_moves};

  BasicMove *silver_general_moves[] = {&shortmove_507, NULL};
  Piece silver_general = { "Silver General", "S", NULL, NULL, 3, 0, 0, silver_general_moves};

  BasicMove *gold_general_moves[] = {&shortmove_257, NULL};
  Piece gold_general = { "Gold General", "G", NULL, NULL, 4, 0, 0, gold_general_moves};
  
  BasicMove *copper_general_moves[] = {&shortmove_207, NULL};
  Piece copper_general = { "Copper General", "C", NULL, NULL, 5, 0, 0, copper_general_moves};

  BasicMove *bishop_moves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &gold_general, NULL, 6, 0, 0, bishop_moves};

  BasicMove *rook_moves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", &gold_general, NULL, 7, 0, 0, rook_moves};

  BasicMove *angry_boar_moves[] = {&shortmove_252, NULL};
  Piece angry_boar = { "Angry Boar", "AB", NULL, NULL, 8, 0, 0, angry_boar_moves};

  BasicMove *dragon_horse_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece dragon_horse = { "Dragon Horse", "DH", NULL, NULL, 9, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "DK", NULL, NULL, 10, 0, 0, dragon_king_moves};

  BasicMove *blind_bear_moves[] = {&shortmove_505, &longmove_200, NULL};
  Piece blind_bear = { "Blind Bear", "BB", NULL, NULL, 11, 0, 0, blind_bear_moves};

  BasicMove *ferocious_leopard_moves[] = {&shortmove_707, NULL};
  Piece ferocious_leopard = { "Ferocious Leopard", "FL", NULL, NULL, 12, 0, 0, ferocious_leopard_moves};

  BasicMove *free_king_moves[] = {&longmove_757, NULL};
  Piece free_king = { "Free King", "FK", NULL, NULL, 13, 0, 0, free_king_moves};

  BasicMove *blind_tiger_moves[] = {&shortmove_755, NULL};
  Piece blind_tiger = { "Blind Tiger", "BT", NULL, NULL, 14, 0, 0, blind_tiger_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &great_dragon, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &golden_bird, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", &furious_fiend, NULL, 17, 0, 0, lion_moves};

  BasicMove *reverse_chariot_moves[] = {&longmove_202, NULL};
  Piece reverse_chariot = { "Reverse Chariot", "RC", NULL, NULL, 18, 0, 0, reverse_chariot_moves};

  BasicMove *side_mover_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece side_mover = { "Side Mover", "SM", &gold_general, NULL, 19, 0, 0, side_mover_moves};

  BasicMove *vertical_mover_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece vertical_mover = { "Vertical Mover", "VM", &gold_general, NULL, 20, 0, 0, vertical_mover_moves};

  BasicMove *buddhist_devil_moves[] = {&shortmove_250, &nummove_5_3, NULL};
  Piece buddhist_devil = { "Buddhist Devil", "BDe", &gold_general, NULL, 21, 0, 0, buddhist_devil_moves};

  BasicMove *cat_sword_moves[] = {&shortmove_505, NULL};
  Piece cat_sword = { "Cat Sword", "CSw", NULL, NULL, 22, 0, 0, cat_sword_moves};

  BasicMove *capricorn_moves[] = {&diaghookmove_505, NULL};
  Piece capricorn = { "Capricorn", "Ca", &gold_general, NULL, 23, 0, 0, capricorn_moves};

  BasicMove *chinese_cock_moves[] = {&shortmove_255, NULL};
  Piece chinese_cock = { "Chinese Cock", "CC", &wizard_stork, NULL, 24, 0, 0, chinese_cock_moves};

  BasicMove *coiled_serpent_moves[] = {&shortmove_702, NULL};
  Piece coiled_serpent = { "Coiled Serpent", "CS", NULL, NULL, 25, 0, 0, coiled_serpent_moves};

  BasicMove *dark_spirit_moves[] = {&shortmove_145, NULL};
  Piece dark_spirit = { "Dark Spirit", "DSp", &buddhist_spirit, NULL, 26, 0, 0, dark_spirit_moves};

  BasicMove *evil_wolf_moves[] = {&shortmove_57, NULL};
  Piece evil_wolf = { "Evil Wolf", "EW", NULL, NULL, 27, 0, 0, evil_wolf_moves};

  BasicMove *deva_moves[] = {&shortmove_415, NULL};
  Piece deva = { "Deva", "DV", &teaching_king, NULL, 28, 0, 0, deva_moves};

  BasicMove *flying_dragon_moves[] = {&nummove_505_2, NULL};
  Piece flying_dragon = { "Flying Dragon", "FD", &gold_general, NULL, 29, 0, 0, flying_dragon_moves};

  BasicMove *donkey_moves[] = {&shortmove_50, &jumpmove_202, NULL};
  Piece donkey = { "Donkey", "Do", &gold_general, NULL, 30, 0, 0, donkey_moves};

  BasicMove *drunk_elephant_moves[] = {&shortmove_557, NULL};
  Piece drunk_elephant = { "Drunk Elephant", "DE", &crown_prince, NULL, 31, 0, 0, drunk_elephant_moves};

  BasicMove *earth_general_moves[] = {&shortmove_202, NULL};
  Piece earth_general = { "Earth General", "Ea", NULL, NULL, 32, 0, 0, earth_general_moves};

  BasicMove *go_between_moves[] = {&shortmove_202, NULL};
  Piece go_between = { "Go Between", "GB", NULL, NULL, 33, 0, 0, go_between_moves};

  BasicMove *guardian_of_the_gods_moves[] = {&shortmove_5, &nummove_252_3, NULL};
  Piece guardian_of_the_gods = { "Guardian Of The Gods", "GG", &gold_general, NULL, 34, 0, 0, guardian_of_the_gods_moves};

  BasicMove *knight_moves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &gold_general, NULL, 35, 0, 0, knight_moves};

  BasicMove *hook_mover_moves[] = {&hookmove, NULL};
  Piece hook_mover = { "Hook Mover", "HM", &gold_general, NULL, 36, 0, 0, hook_mover_moves};

  BasicMove *old_monkey_moves[] = {&shortmove_705, NULL};
  Piece old_monkey = { "Old Monkey", "OM", NULL, NULL, 37, 0, 0, old_monkey_moves};

  BasicMove *iron_general_moves[] = {&shortmove_7, NULL};
  Piece iron_general = { "Iron General", "I", NULL, NULL, 38, 0, 0, iron_general_moves};

  BasicMove *left_chariot_moves[] = {&shortmove_200, &longmove_403, NULL};
  Piece left_chariot = { "Left Chariot", "LCh", NULL, NULL, 39, 0, 0, left_chariot_moves};

  BasicMove *reclining_dragon_moves[] = {&shortmove_752, NULL};
  Piece reclining_dragon = { "Reclining Dragon", "RD", NULL, NULL, 40, 0, 0, reclining_dragon_moves};

  BasicMove *lion_dog_moves[] = {&nummove_757_3, NULL};
  Piece lion_dog = { "Lion Dog", "LD", &great_elephant, NULL, 41, 0, 0, lion_dog_moves};

  BasicMove *blue_dragon_moves[] = {&shortmove_1, &nummove_202_2, &longmove_54, NULL};
  Piece blue_dragon = { "Blue Dragon", "BD", NULL, NULL, 42, 0, 0, blue_dragon_moves};

  BasicMove *tile_general_moves[] = {&shortmove_205, NULL};
  Piece tile_general = { "Tile General", "T", NULL, NULL, 43, 0, 0, tile_general_moves};

  BasicMove *wrestler_moves[] = {&shortmove_50, &nummove_505_3, NULL};
  Piece wrestler = { "Wrestler", "Wr", &gold_general, NULL, 44, 0, 0, wrestler_moves};

  BasicMove *right_chariot_moves[] = {&shortmove_200, &longmove_106, NULL};
  Piece right_chariot = { "Right Chariot", "RCh", NULL, NULL, 45, 0, 0, right_chariot_moves};

  BasicMove *she_devil_moves[] = {&nummove_252_5, &nummove_505_2, NULL};
  Piece she_devil = { "She Devil", "SD", &gold_general, NULL, 46, 0, 0, she_devil_moves};

  BasicMove *stone_general_moves[] = {&shortmove_5, NULL};
  Piece stone_general = { "Stone General", "St", NULL, NULL, 47, 0, 0, stone_general_moves};

  BasicMove *violent_ox_moves[] = {&nummove_252_2, NULL};
  Piece violent_ox = { "Violent Ox", "VO", NULL, NULL, 48, 0, 0, violent_ox_moves};

  BasicMove *old_rat_moves[] = {&nummove_205_2, NULL};
  Piece old_rat = { "Old Rat", "OR", &wizard_stork, NULL, 49, 0, 0, old_rat_moves};

  BasicMove *blind_monkey_moves[] = {&shortmove_555, NULL};
  Piece blind_monkey = { "Blind Monkey", "BM", &mountain_witch, NULL, 50, 0, 0, blind_monkey_moves};

  BasicMove *crown_prince_moves[] = {&shortmove_757, NULL};
  Piece crown_prince = { "Crown Prince", "CP", NULL, NULL, 51, 0, Piece::KING, crown_prince_moves};

  BasicMove *dove_moves[] = {&nummove_505_5, &nummove_252_2, NULL};
  Piece dove = { "Dove", "D", NULL, NULL, 52, 0, 0, dove_moves};

  BasicMove *eastern_barbarian_moves[] = {&shortmove_55, &nummove_202_2, NULL};
  Piece eastern_barbarian = { "Eastern Barbarian", "EB", &prom_eastern_barbarian, NULL, 53, 0, 0, eastern_barbarian_moves};

  BasicMove *enchanted_badger_moves[] = {&nummove_52_2, NULL};
  Piece enchanted_badger = { "Enchanted Badger", "EBa", &dove, NULL, 54, 0, 0, enchanted_badger_moves};

  BasicMove *flying_horse_moves[] = {&shortmove_252, &nummove_5_2, NULL};
  Piece flying_horse = { "Flying Horse", "FH", &free_king, NULL, 55, 0, 0, flying_horse_moves};

  BasicMove *free_demon_moves[] = {&nummove_202_5, &longmove_555, NULL};
  Piece free_demon = { "Free Demon", "FDe", NULL, NULL, 56, 0, 0, free_demon_moves};

  BasicMove *free_tapir_moves[] = {&nummove_50_5, &longmove_707, NULL};
  Piece free_tapir = { "Free Tapir", "FTp", NULL, NULL, 57, 0, 0, free_tapir_moves};

  BasicMove *golden_bird_moves[] = {&nummove_50_2, &nummove_505_3, &longmove_202, NULL};
  Piece golden_bird = { "Golden Bird", "GBd", NULL, NULL, 58, 0, 0, golden_bird_moves};

  BasicMove *great_dragon_moves[] = {&nummove_202_2, &nummove_505_3, &longmove_50, NULL};
  Piece great_dragon = { "Great Dragon", "GD", NULL, NULL, 59, 0, 0, great_dragon_moves};

  BasicMove *howling_dog_moves[] = {&shortmove_200, &longmove_2, NULL};
  Piece howling_dog = { "Howling Dog", "HD", NULL, NULL, 60, 0, 0, howling_dog_moves};

  BasicMove *left_general_moves[] = {&shortmove_747, NULL};
  Piece left_general = { "Left General", "LG", NULL, NULL, 61, 0, 0, left_general_moves};

  BasicMove *right_general_moves[] = {&shortmove_717, NULL};
  Piece right_general = { "Right General", "RG", NULL, NULL, 62, 0, 0, right_general_moves};

  BasicMove *long_nosed_goblin_moves[] = {&shortmove_252, &diaghookmove_505, NULL};
  Piece long_nosed_goblin = { "Long Nosed Goblin", "LGn", NULL, NULL, 63, 0, 0, long_nosed_goblin_moves};

  BasicMove *neighbouring_king_moves[] = {&shortmove_557, NULL};
  Piece neighbouring_king = { "Neighbouring King", "NK", &standard_bearer, NULL, 64, 0, 0, neighbouring_king_moves};

  BasicMove *northern_barbarian_moves[] = {&shortmove_550, &nummove_5_2, NULL};
  Piece northern_barbarian = { "Northern Barbarian", "NB", &fragrant_elephant, NULL, 65, 0, 0, northern_barbarian_moves};

  BasicMove *old_kite_hawk_moves[] = {&shortmove_5, &nummove_252_2, NULL};
  Piece old_kite_hawk = { "Old Kite Hawk", "OK", &long_nosed_goblin, NULL, 66, 0, 0, old_kite_hawk_moves};

  BasicMove *rushing_bird_moves[] = {&longmove_557, NULL};
  Piece rushing_bird = { "Rushing Bird", "RB", &free_demon, NULL, 67, 0, 0, rushing_bird_moves};

  BasicMove *southern_barbarian_moves[] = {&shortmove_55, &nummove_500_2, NULL};
  Piece southern_barbarian = { "Southern Barbarian", "SB", &white_elephant, NULL, 68, 0, 0, southern_barbarian_moves};

  BasicMove *standard_bearer_moves[] = {&nummove_750_2, &longmove_7, NULL};
  Piece standard_bearer = { "Standard Bearer", "SBr", NULL, NULL, 69, 0, 0, standard_bearer_moves};

  BasicMove *water_buffalo_moves[] = {&nummove_202_2, &longmove_555, NULL};
  Piece water_buffalo = { "Water Buffalo", "WBf", &free_tapir, NULL, 70, 0, 0, water_buffalo_moves};

  BasicMove *western_barbarian_moves[] = {&shortmove_207, &nummove_50_2, NULL};
  Piece western_barbarian = { "Western Barbarian", "WB", &prom_western_barbarian, NULL, 71, 0, 0, western_barbarian_moves};

  BasicMove *whale_moves[] = {&longmove_702, NULL};
  Piece whale = { "Whale", "W", NULL, NULL, 72, 0, 0, whale_moves};

  BasicMove *white_elephant_moves[] = {&nummove_257_2, &longmove_500, NULL};
  Piece white_elephant = { "White Elephant", "WE", NULL, NULL, 73, 0, 0, white_elephant_moves};

  BasicMove *white_horse_moves[] = {&longmove_207, NULL};
  Piece white_horse = { "White Horse", "WH", NULL, NULL, 74, 0, 0, white_horse_moves};

  BasicMove *white_tiger_moves[] = {&shortmove_4, &nummove_50_2, &longmove_203, NULL};
  Piece white_tiger = { "White Tiger", "WT", NULL, NULL, 75, 0, 0, white_tiger_moves};

  BasicMove *wood_general_moves[] = {&nummove_5_2, NULL};
  Piece wood_general = { "Wood General", "Wo", NULL, NULL, 76, 0, 0, wood_general_moves};

  BasicMove *fierce_eagle_moves[] = {&shortmove_50, &nummove_505_2, NULL};
  Piece fierce_eagle = { "Fierce Eagle", "FE", NULL, NULL, 77, 0, 0, fierce_eagle_moves};

  BasicMove *flying_ox_moves[] = {&longmove_707, NULL};
  Piece flying_ox = { "Flying Ox", "FO", NULL, NULL, 78, 0, 0, flying_ox_moves};

  BasicMove *golden_deer_moves[] = {&nummove_500_2, &longmove_5, NULL};
  Piece golden_deer = { "Golden Deer", "GDe", NULL, NULL, 79, 0, 0, golden_deer_moves};

  BasicMove *horned_falcon_moves[] = {&smalllionmove_2, &longmove_755, NULL};
  Piece horned_falcon = { "Horned Falcon", "HF", NULL, NULL, 80, 0, 0, horned_falcon_moves};

  BasicMove *peacock_moves[] = {&nummove_500_2, &diaghookmove_5, NULL};
  Piece peacock = { "Peacock", "PC", NULL, NULL, 81, 0, 0, peacock_moves};

  BasicMove *poisonous_snake_moves[] = {&shortmove_50, &jumpmove_502, NULL};
  Piece poisonous_snake = { "Poisonous Snake", "PS", &prom_poisonous_snake, NULL, 82, 0, 0, poisonous_snake_moves};

  BasicMove *prancing_stag_moves[] = {&shortmove_507, &nummove_50_2, NULL};
  Piece prancing_stag = { "Prancing Stag", "PSt", &square_mover, NULL, 83, 0, 0, prancing_stag_moves};

  BasicMove *ramshead_soldier_moves[] = {&longmove_5, NULL};
  Piece ramshead_soldier = { "Ramshead Soldier", "RS", NULL, NULL, 84, 0, 0, ramshead_soldier_moves};

  BasicMove *side_dragon_moves[] = {&shortmove_200, &longmove_52, NULL};
  Piece side_dragon = { "Side Dragon", "SDr", &gold_general, NULL, 85, 0, 0, side_dragon_moves};

  BasicMove *sideways_chariot_moves[] = {&shortmove_500, &longmove_252, NULL};
  Piece sideways_chariot = { "Sideways Chariot", "SCh", NULL, NULL, 86, 0, 0, sideways_chariot_moves};

  BasicMove *silver_demon_moves[] = {&nummove_5_2, &longmove_500, NULL};
  Piece silver_demon = { "Silver Demon", "SDe", NULL, NULL, 87, 0, 0, silver_demon_moves};

  BasicMove *soaring_eagle_moves[] = {&smalllionmove_5, &longmove_752, NULL};
  Piece soaring_eagle = { "Soaring Eagle", "SE", NULL, NULL, 88, 0, 0, soaring_eagle_moves};

  BasicMove *soldier_moves[] = {&longmove_752, NULL};
  Piece soldier = { "Soldier", "So", NULL, NULL, 89, 0, 0, soldier_moves};

  BasicMove *turtle_snake_moves[] = {&shortmove_200, &nummove_500_2, &longmove_7, NULL};
  Piece turtle_snake = { "Turtle Snake", "TS", NULL, NULL, 90, 0, 0, turtle_snake_moves};

  BasicMove *vermillion_sparrow_moves[] = {&shortmove_200, &nummove_500_2, &longmove_7, NULL};
  Piece vermillion_sparrow = { "Vermillion Sparrow", "VS", NULL, NULL, 91, 0, 0, vermillion_sparrow_moves};

  BasicMove *violent_bear_moves[] = {&shortmove_50, &nummove_5_2, NULL};
  Piece violent_bear = { "Violent Bear", "VBe", NULL, NULL, 92, 0, 0, violent_bear_moves};

  BasicMove *buddhist_spirit_moves[] = {&longmove_757, &lionmove_2, NULL};
  Piece buddhist_spirit = { "Buddhist Spirit", "BSp", NULL, NULL, 93, 0, 0, buddhist_spirit_moves};

  BasicMove *fragrant_elephant_moves[] = {&nummove_752_2, &longmove_5, NULL};
  Piece fragrant_elephant = { "Fragrant Elephant", "FEl", NULL, NULL, 94, 0, 0, fragrant_elephant_moves};

  BasicMove *furious_fiend_moves[] = {&nummove_757_3, &lionmove_2, NULL};
  Piece furious_fiend = { "Furious Fiend", "FF", NULL, NULL, 95, 0, 0, furious_fiend_moves};

  BasicMove *great_elephant_moves[] = {&nummove_550_5, &nummove_207_3, NULL};
  Piece great_elephant = { "Great Elephant", "GE", NULL, NULL, 96, 0, 0, great_elephant_moves};

  BasicMove *mountain_witch_moves[] = {&shortmove_2, &longmove_705, NULL};
  Piece mountain_witch = { "Mountain Witch", "MW", NULL, NULL, 97, 0, 0, mountain_witch_moves};

  BasicMove *square_mover_moves[] = {&shortmove_5, &longmove_252, NULL};
  Piece square_mover = { "Square Mover", "SMo", NULL, NULL, 98, 0, 0, square_mover_moves};

  BasicMove *teaching_king_moves[] = {&longmove_757, NULL};
  Piece teaching_king = { "Teaching King", "TK", NULL, NULL, 99, 0, 0, teaching_king_moves};

  BasicMove *teaching_king2_moves[] = {&longmove_757, &lionmove_3, NULL};
  Piece teaching_king2 = { "Teaching King2", "TK", NULL, NULL, 99, 0, 0, teaching_king2_moves};

  BasicMove *wizard_stork_moves[] = {&shortmove_200, &longmove_507, NULL};
  Piece wizard_stork = { "Wizard Stork", "WS", NULL, NULL, 100, 0, 0, wizard_stork_moves};

  BasicMove *prom_eastern_barbarian_moves[] = {&lionmove_2, NULL};
  Piece prom_eastern_barbarian = { "Promoted Eastern Barbarian", "+EB", NULL, NULL, 101, 0, 0, prom_eastern_barbarian_moves};

  BasicMove *prom_western_barbarian_moves[] = {&nummove_757_3, NULL};
  Piece prom_western_barbarian = { "Promoted Western Barbarian", "+WB", NULL, NULL, 102, 0, 0, prom_western_barbarian_moves};

  BasicMove *prom_poisonous_snake_moves[] = {&hookmove, NULL};
  Piece prom_poisonous_snake = { "Promoted Poisonous Snake", "+PS", NULL, NULL, 103, 0, 0, prom_poisonous_snake_moves};

  const char *names[] = { "+EB", "+PS", "+WB", "AB", "B", "BB", "BD", "BDe", "BM", "BSp", "BT", "C", "CC", "CP", "CS", "CSw", "Ca", "D", "DE", "DH", "DK", "DSp", "DV", "Do", "E", "EB", "EBa", "EW", "Ea", "FD", "FDe", "FE", "FEl", "FF", "FH", "FK", "FL", "FO", "FTp", "G", "GB", "GBd", "GD", "GDe", "GE", "GG", "HD", "HF", "HM", "I", "Ky", "L", "LCh", "LD", "LG", "LGn", "Ln", "MW", "N", "NB", "NK", "OK", "OM", "OR", "P", "PC", "PS", "PSt", "Ph", "R", "RB", "RC", "RCh", "RD", "RG", "RS", "S", "SB", "SBr", "SCh", "SD", "SDe", "SDr", "SE", "SM", "SMo", "So", "St", "T", "TK", "TS", "VBe", "VM", "VO", "VS", "W", "WB", "WBf", "WE", "WH", "WS", "WT", "Wo", "Wr" };

  int name2pos[] = { 101, 103, 102, 8, 6, 11, 42, 21, 50, 93, 14, 5, 24, 51, 25, 22, 23, 52, 31, 9, 10, 26, 28, 30, 1, 53, 54, 27, 32, 29, 56, 77, 94, 95, 55, 13, 12, 78, 57, 4, 33, 58, 59, 79, 96, 34, 60, 80, 36, 38, 15, 2, 39, 41, 61, 63, 17, 97, 35, 65, 64, 66, 37, 49, 0, 81, 82, 83, 16, 7, 67, 18, 45, 40, 62, 84, 3, 68, 69, 86, 46, 87, 85, 88, 19, 98, 89, 47, 43, 99, 90, 92, 20, 48, 91, 72, 71, 70, 73, 74, 100, 75, 76, 44};

  const Piece *pieces[] = {
     &pawn, &emperor, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &angry_boar, &dragon_horse, &dragon_king, &blind_bear, &ferocious_leopard, &free_king, &blind_tiger, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &buddhist_devil, &cat_sword, &capricorn, &chinese_cock, &coiled_serpent, &dark_spirit, &evil_wolf, &deva, &flying_dragon, &donkey, &drunk_elephant, &earth_general, &go_between, &guardian_of_the_gods, &knight, &hook_mover, &old_monkey, &iron_general, &left_chariot, &reclining_dragon, &lion_dog, &blue_dragon, &tile_general, &wrestler, &right_chariot, &she_devil, &stone_general, &violent_ox, &old_rat, &blind_monkey, &crown_prince, &dove, &eastern_barbarian, &enchanted_badger, &flying_horse, &free_demon, &free_tapir, &golden_bird, &great_dragon, &howling_dog, &left_general, &right_general, &long_nosed_goblin, &neighbouring_king, &northern_barbarian, &old_kite_hawk, &rushing_bird, &southern_barbarian, &standard_bearer, &water_buffalo, &western_barbarian, &whale, &white_elephant, &white_horse, &white_tiger, &wood_general, &fierce_eagle, &flying_ox, &golden_deer, &horned_falcon, &peacock, &poisonous_snake, &prancing_stag, &ramshead_soldier, &side_dragon, &sideways_chariot, &silver_demon, &soaring_eagle, &soldier, &turtle_snake, &vermillion_sparrow, &violent_bear, &buddhist_spirit, &fragrant_elephant, &furious_fiend, &great_elephant, &mountain_witch, &square_mover, &teaching_king, &wizard_stork, &prom_eastern_barbarian, &prom_western_barbarian, &prom_poisonous_snake};

  const Piece *pieces2[] = {
     &pawn, &emperor, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &angry_boar, &dragon_horse, &dragon_king, &blind_bear, &ferocious_leopard, &free_king, &blind_tiger, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &buddhist_devil, &cat_sword, &capricorn, &chinese_cock, &coiled_serpent, &dark_spirit, &evil_wolf, &deva, &flying_dragon, &donkey, &drunk_elephant, &earth_general, &go_between, &guardian_of_the_gods, &knight, &hook_mover, &old_monkey, &iron_general, &left_chariot, &reclining_dragon, &lion_dog, &blue_dragon, &tile_general, &wrestler, &right_chariot, &she_devil, &stone_general, &violent_ox, &old_rat, &blind_monkey, &crown_prince, &dove, &eastern_barbarian, &enchanted_badger, &flying_horse, &free_demon, &free_tapir, &golden_bird, &great_dragon, &howling_dog, &left_general, &right_general, &long_nosed_goblin, &neighbouring_king, &northern_barbarian, &old_kite_hawk, &rushing_bird, &southern_barbarian, &standard_bearer, &water_buffalo, &western_barbarian, &whale, &white_elephant, &white_horse, &white_tiger, &wood_general, &fierce_eagle, &flying_ox, &golden_deer, &horned_falcon, &peacock, &poisonous_snake, &prancing_stag, &ramshead_soldier, &side_dragon, &sideways_chariot, &silver_demon, &soaring_eagle, &soldier, &turtle_snake, &vermillion_sparrow, &violent_bear, &buddhist_spirit, &fragrant_elephant, &furious_fiend, &great_elephant, &mountain_witch, &square_mover, &teaching_king2, &wizard_stork, &prom_eastern_barbarian, &prom_western_barbarian, &prom_poisonous_snake};

  
  Board::Square setup[] = {
     {2, 1}, {75, 1}, {72, 1}, {29, 1}, {63, 1}, {52, 1}, {7, 1}, {9, 1}, {10, 1}, {13, 1}, {4, 1}, {26, 1}, {1, 1}, {28, 1}, {4, 1}, {13, 1}, {10, 1}, {9, 1}, {7, 1}, {52, 1}, {63, 1}, {29, 1}, {72, 1}, {90, 1}, {2, 1}, 
     {18, 1}, {85, 1}, {88, 1}, {35, 1}, {82, 1}, {57, 1}, {6, 1}, {77, 1}, {73, 1}, {56, 1}, {3, 1}, {62, 1}, {51, 1}, {61, 1}, {3, 1}, {56, 1}, {73, 1}, {77, 1}, {6, 1}, {57, 1}, {82, 1}, {35, 1}, {88, 1}, {85, 1}, {18, 1}, 
     {86, 1}, {74, 1}, {84, 1}, {48, 1}, {22, 1}, {11, 1}, {87, 1}, {79, 1}, {50, 1}, {14, 1}, {46, 1}, {34, 1}, {64, 1}, {44, 1}, {21, 1}, {14, 1}, {50, 1}, {79, 1}, {87, 1}, {11, 1}, {22, 1}, {48, 1}, {84, 1}, {74, 1}, {86, 1}, 
     {89, 1}, {70, 1}, {12, 1}, {65, 1}, {68, 1}, {24, 1}, {80, 1}, {37, 1}, {67, 1}, {81, 1}, {58, 1}, {16, 1}, {17, 1}, {15, 1}, {59, 1}, {81, 1}, {66, 1}, {37, 1}, {80, 1}, {24, 1}, {53, 1}, {71, 1}, {12, 1}, {70, 1}, {89, 1}, 
     {45, 1}, {91, 1}, {76, 1}, {32, 1}, {47, 1}, {43, 1}, {38, 1}, {5, 1}, {49, 1}, {25, 1}, {40, 1}, {36, 1}, {31, 1}, {23, 1}, {40, 1}, {25, 1}, {49, 1}, {5, 1}, {38, 1}, {43, 1}, {47, 1}, {32, 1}, {76, 1}, {42, 1}, {39, 1}, 
     {60, 1}, {55, 1}, {54, 1}, {30, 1}, {78, 1}, {19, 1}, {20, 1}, {92, 1}, {69, 1}, {83, 1}, {8, 1}, {27, 1}, {41, 1}, {27, 1}, {8, 1}, {83, 1}, {69, 1}, {92, 1}, {20, 1}, {19, 1}, {78, 1}, {30, 1}, {54, 1}, {55, 1}, {60, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {60, 0}, {55, 0}, {54, 0}, {30, 0}, {78, 0}, {19, 0}, {20, 0}, {92, 0}, {69, 0}, {83, 0}, {8, 0}, {27, 0}, {41, 0}, {27, 0}, {8, 0}, {83, 0}, {69, 0}, {92, 0}, {20, 0}, {19, 0}, {78, 0}, {30, 0}, {54, 0}, {55, 0}, {60, 0}, 
     {39, 0}, {42, 0}, {76, 0}, {32, 0}, {47, 0}, {43, 0}, {38, 0}, {5, 0}, {49, 0}, {25, 0}, {40, 0}, {23, 0}, {31, 0}, {36, 0}, {40, 0}, {25, 0}, {49, 0}, {5, 0}, {38, 0}, {43, 0}, {47, 0}, {32, 0}, {76, 0}, {91, 0}, {45, 0}, 
     {89, 0}, {70, 0}, {12, 0}, {71, 0}, {53, 0}, {24, 0}, {80, 0}, {37, 0}, {66, 0}, {81, 0}, {59, 0}, {15, 0}, {17, 0}, {16, 0}, {58, 0}, {81, 0}, {67, 0}, {37, 0}, {80, 0}, {24, 0}, {68, 0}, {65, 0}, {12, 0}, {70, 0}, {89, 0}, 
     {86, 0}, {74, 0}, {84, 0}, {48, 0}, {22, 0}, {11, 0}, {87, 0}, {79, 0}, {50, 0}, {14, 0}, {21, 0}, {44, 0}, {64, 0}, {34, 0}, {46, 0}, {14, 0}, {50, 0}, {79, 0}, {87, 0}, {11, 0}, {22, 0}, {48, 0}, {84, 0}, {74, 0}, {86, 0}, 
     {18, 0}, {85, 0}, {88, 0}, {35, 0}, {82, 0}, {57, 0}, {6, 0}, {77, 0}, {73, 0}, {56, 0}, {3, 0}, {61, 0}, {51, 0}, {62, 0}, {3, 0}, {56, 0}, {73, 0}, {77, 0}, {6, 0}, {57, 0}, {82, 0}, {35, 0}, {88, 0}, {85, 0}, {18, 0}, 
     {2, 0}, {90, 0}, {72, 0}, {29, 0}, {63, 0}, {52, 0}, {7, 0}, {9, 0}, {10, 0}, {13, 0}, {4, 0}, {28, 0}, {1, 0}, {26, 0}, {4, 0}, {13, 0}, {10, 0}, {9, 0}, {7, 0}, {52, 0}, {63, 0}, {29, 0}, {72, 0}, {75, 0}, {2, 0}};

}

Board::Type tai_type = {
   pieces, setup, 0, Board::PROM_BY_CAPTURE, 25, 25, 104, names, name2pos, 3, "Tai Shogi", 19};

Board::Type tai2_type = {
   pieces2, setup, 0, Board::PROM_BY_CAPTURE, 25, 25, 104, names, name2pos, 3,
   "Tai Shogi with teaching king version 2", 20};
