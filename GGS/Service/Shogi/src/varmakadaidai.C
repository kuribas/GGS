#include "ShogiImpl.H"

namespace
{
  extern Piece emperor, tokin, prom_lance, free_silver, free_gold, free_copper, prom_bishop, prom_rook, free_boar, free_bear, free_leopard, free_tiger, great_dragon, golden_bird, furious_fiend, prom_reverse_chariot, prom_side_mover, prom_vertical_mover, prom_buddhist_devil, free_cat, prom_capricorn, wizard_stork, free_serpent, buddhist_spirit, free_wolf, teaching_king, prom_flying_dragon, prom_donkey, prince, free_earth, free_goer, prom_guardian_of_the_gods, prom_knight, prom_hook_mover, mountain_witch, free_iron, prom_left_chariot, free_dragon, prom_lion_dog, prom_side_flyer, free_tile, prom_wrestler, prom_right_chariot, prom_she_devil, free_stone, prom_violent_ox, bat;

  BasicMove *pawn_moves[] = {&shortmove_2, NULL};
  Piece pawn = { "Pawn", "P", &tokin, NULL, 0, 0, Piece::PAWN, pawn_moves};

  BasicMove *king_moves[] = {&shortmove_757, NULL};
  Piece king = { "King", "K", &emperor, NULL, 1, 0, Piece::KING, king_moves};

  BasicMove *lance_moves[] = {&longmove_2, NULL};
  Piece lance = { "Lance", "L", &prom_lance, NULL, 2, 0, 0, lance_moves};

  BasicMove *silver_general_moves[] = {&shortmove_507, NULL};
  Piece silver_general = { "Silver General", "S", &free_silver, NULL, 3, 0, 0, silver_general_moves};

  BasicMove *gold_general_moves[] = {&shortmove_257, NULL};
  Piece gold_general = { "Gold General", "G", &free_gold, NULL, 4, 0, 0, gold_general_moves};

  BasicMove *copper_general_moves[] = {&shortmove_207, NULL};
  Piece copper_general = { "Copper General", "C", &free_copper, NULL, 5, 0, 0, copper_general_moves};

  BasicMove *bishop_moves[] = {&longmove_505, NULL};
  Piece bishop = { "Bishop", "B", &prom_bishop, NULL, 6, 0, 0, bishop_moves};

  BasicMove *rook_moves[] = {&longmove_252, NULL};
  Piece rook = { "Rook", "R", &prom_rook, NULL, 7, 0, 0, rook_moves};

  BasicMove *angry_boar_moves[] = {&shortmove_252, NULL};
  Piece angry_boar = { "Angry Boar", "AB", &free_boar, NULL, 8, 0, 0, angry_boar_moves};

  BasicMove *dragon_horse_moves[] = {&shortmove_252, &longmove_505, NULL};
  Piece dragon_horse = { "Dragon Horse", "DH", NULL, NULL, 9, 0, 0, dragon_horse_moves};

  BasicMove *dragon_king_moves[] = {&shortmove_505, &longmove_252, NULL};
  Piece dragon_king = { "Dragon King", "DK", NULL, NULL, 10, 0, 0, dragon_king_moves};

  BasicMove *blind_bear_moves[] = {&shortmove_505, &longmove_200, NULL};
  Piece blind_bear = { "Blind Bear", "BB", &free_bear, NULL, 11, 0, 0, blind_bear_moves};

  BasicMove *ferocious_leopard_moves[] = {&shortmove_707, NULL};
  Piece ferocious_leopard = { "Ferocious Leopard", "FL", &free_leopard, NULL, 12, 0, 0, ferocious_leopard_moves};

  BasicMove *free_king_moves[] = {&longmove_757, NULL};
  Piece free_king = { "Free King", "FK", NULL, NULL, 13, 0, 0, free_king_moves};

  BasicMove *blind_tiger_moves[] = {&shortmove_755, NULL};
  Piece blind_tiger = { "Blind Tiger", "BT", &free_tiger, NULL, 14, 0, 0, blind_tiger_moves};

  BasicMove *kylin_moves[] = {&shortmove_505, &jumpmove_252, NULL};
  Piece kylin = { "Kylin", "Ky", &great_dragon, NULL, 15, 0, 0, kylin_moves};

  BasicMove *phoenix_moves[] = {&shortmove_252, &jumpmove_505, NULL};
  Piece phoenix = { "Phoenix", "Ph", &golden_bird, NULL, 16, 0, 0, phoenix_moves};

  BasicMove *lion_moves[] = {&lionmove_2, NULL};
  Piece lion = { "Lion", "Ln", &furious_fiend, NULL, 17, 0, 0, lion_moves};

  BasicMove *reverse_chariot_moves[] = {&longmove_202, NULL};
  Piece reverse_chariot = { "Reverse Chariot", "RC", &prom_reverse_chariot, NULL, 18, 0, 0, reverse_chariot_moves};

  BasicMove *side_mover_moves[] = {&shortmove_202, &longmove_50, NULL};
  Piece side_mover = { "Side Mover", "SM", &prom_side_mover, NULL, 19, 0, 0, side_mover_moves};

  BasicMove *vertical_mover_moves[] = {&shortmove_50, &longmove_202, NULL};
  Piece vertical_mover = { "Vertical Mover", "VM", &prom_vertical_mover, NULL, 20, 0, 0, vertical_mover_moves};

  BasicMove *buddhist_devil_moves[] = {&shortmove_250, &nummove_5_3, NULL};
  Piece buddhist_devil = { "Buddhist Devil", "BDe", &prom_buddhist_devil, NULL, 21, 0, 0, buddhist_devil_moves};

  BasicMove *cat_sword_moves[] = {&shortmove_505, NULL};
  Piece cat_sword = { "Cat Sword", "CSw", &free_cat, NULL, 22, 0, 0, cat_sword_moves};

  BasicMove *capricorn_moves[] = {&diaghookmove_505, NULL};
  Piece capricorn = { "Capricorn", "Ca", &prom_capricorn, NULL, 23, 0, 0, capricorn_moves};

  BasicMove *chinese_cock_moves[] = {&shortmove_255, NULL};
  Piece chinese_cock = { "Chinese Cock", "CC", &wizard_stork, NULL, 24, 0, 0, chinese_cock_moves};

  BasicMove *coiled_serpent_moves[] = {&shortmove_702, NULL};
  Piece coiled_serpent = { "Coiled Serpent", "CS", &free_serpent, NULL, 25, 0, 0, coiled_serpent_moves};

  BasicMove *dark_spirit_moves[] = {&shortmove_145, NULL};
  Piece dark_spirit = { "Dark Spirit", "DSp", &buddhist_spirit, NULL, 26, 0, 0, dark_spirit_moves};

  BasicMove *evil_wolf_moves[] = {&shortmove_57, NULL};
  Piece evil_wolf = { "Evil Wolf", "EW", &free_wolf, NULL, 27, 0, 0, evil_wolf_moves};

  BasicMove *deva_moves[] = {&shortmove_415, NULL};
  Piece deva = { "Deva", "DV", &teaching_king, NULL, 28, 0, 0, deva_moves};

  BasicMove *flying_dragon_moves[] = {&nummove_505_2, NULL};
  Piece flying_dragon = { "Flying Dragon", "FD", &prom_flying_dragon, NULL, 29, 0, 0, flying_dragon_moves};

  BasicMove *donkey_moves[] = {&shortmove_50, &jumpmove_202, NULL};
  Piece donkey = { "Donkey", "Do", &prom_donkey, NULL, 30, 0, 0, donkey_moves};

  BasicMove *drunk_elephant_moves[] = {&shortmove_557, NULL};
  Piece drunk_elephant = { "Drunk Elephant", "DE", &prince, NULL, 31, 0, 0, drunk_elephant_moves};

  BasicMove *earth_general_moves[] = {&shortmove_202, NULL};
  Piece earth_general = { "Earth General", "Ea", &free_earth, NULL, 32, 0, 0, earth_general_moves};

  BasicMove *go_between_moves[] = {&shortmove_202, NULL};
  Piece go_between = { "Go Between", "GB", &free_goer, NULL, 33, 0, 0, go_between_moves};

  BasicMove *guardian_of_the_gods_moves[] = {&shortmove_5, &nummove_252_3, NULL};
  Piece guardian_of_the_gods = { "Guardian Of The Gods", "GG", &prom_guardian_of_the_gods, NULL, 34, 0, 0, guardian_of_the_gods_moves};

  BasicMove *knight_moves[] = {&knightmove_2, NULL};
  Piece knight = { "Knight", "N", &prom_knight, NULL, 35, 0, 0, knight_moves};

  BasicMove *hook_mover_moves[] = {&hookmove, NULL};
  Piece hook_mover = { "Hook Mover", "HM", &prom_hook_mover, NULL, 36, 0, 0, hook_mover_moves};

  BasicMove *old_monkey_moves[] = {&shortmove_705, NULL};
  Piece old_monkey = { "Old Monkey", "OM", &mountain_witch, NULL, 37, 0, 0, old_monkey_moves};

  BasicMove *iron_general_moves[] = {&shortmove_7, NULL};
  Piece iron_general = { "Iron General", "I", &free_iron, NULL, 38, 0, 0, iron_general_moves};

  BasicMove *left_chariot_moves[] = {&shortmove_200, &longmove_403, NULL};
  Piece left_chariot = { "Left Chariot", "LCh", &prom_left_chariot, NULL, 39, 0, 0, left_chariot_moves};

  BasicMove *reclining_dragon_moves[] = {&shortmove_752, NULL};
  Piece reclining_dragon = { "Reclining Dragon", "RD", &free_dragon, NULL, 40, 0, 0, reclining_dragon_moves};

  BasicMove *lion_dog_moves[] = {&nummove_757_3, NULL};
  Piece lion_dog = { "Lion Dog", "LD", &prom_lion_dog, NULL, 41, 0, 0, lion_dog_moves};

  BasicMove *side_flyer_moves[] = {&shortmove_505, &longmove_50, NULL};
  Piece side_flyer = { "Side Flyer", "SF", &prom_side_flyer, NULL, 42, 0, 0, side_flyer_moves};

  BasicMove *tile_general_moves[] = {&shortmove_205, NULL};
  Piece tile_general = { "Tile General", "T", &free_tile, NULL, 43, 0, 0, tile_general_moves};

  BasicMove *wrestler_moves[] = {&shortmove_50, &nummove_505_3, NULL};
  Piece wrestler = { "Wrestler", "Wr", &prom_wrestler, NULL, 44, 0, 0, wrestler_moves};

  BasicMove *right_chariot_moves[] = {&shortmove_200, &longmove_106, NULL};
  Piece right_chariot = { "Right Chariot", "RCh", &prom_right_chariot, NULL, 45, 0, 0, right_chariot_moves};

  BasicMove *she_devil_moves[] = {&nummove_252_5, &nummove_505_2, NULL};
  Piece she_devil = { "She Devil", "SD", &prom_she_devil, NULL, 46, 0, 0, she_devil_moves};

  BasicMove *stone_general_moves[] = {&shortmove_5, NULL};
  Piece stone_general = { "Stone General", "St", &free_stone, NULL, 47, 0, 0, stone_general_moves};

  BasicMove *violent_ox_moves[] = {&nummove_252_2, NULL};
  Piece violent_ox = { "Violent Ox", "VO", &prom_violent_ox, NULL, 48, 0, 0, violent_ox_moves};

  BasicMove *old_rat_moves[] = {&nummove_205_2, NULL};
  Piece old_rat = { "Old Rat", "OR", &bat, NULL, 49, 0, 0, old_rat_moves};

  BasicMove *furious_fiend_moves[] = {&nummove_757_3, &lionmove_2, NULL};
  Piece furious_fiend = { "Furious Fiend", "FF", NULL, NULL, 50, 0, 0, furious_fiend_moves};

  BasicMove *bat_moves[] = {&longmove_502, NULL};
  Piece bat = { "Bat", "Bt", NULL, NULL, 51, 0, 0, bat_moves};

  BasicMove *buddhist_spirit_moves[] = {&longmove_757, &lionmove_2, NULL};
  Piece buddhist_spirit = { "Buddhist Spirit", "BSp", NULL, NULL, 52, 0, 0, buddhist_spirit_moves};

  BasicMove *emperor_moves[] = {&emperormove, NULL};
  Piece emperor = { "Emperor", "E", NULL, NULL, 53, 0, Piece::KING | Piece::EMPEROR, emperor_moves};

  BasicMove *free_bear_moves[] = {&jumpmove_5, &longmove_555, NULL};
  Piece free_bear = { "Free Bear", "FBe", NULL, NULL, 54, 0, 0, free_bear_moves};

  BasicMove *free_boar_moves[] = {&longmove_555, NULL};
  Piece free_boar = { "Free Boar", "FBo", NULL, NULL, 55, 0, 0, free_boar_moves};

  BasicMove *free_cat_moves[] = {&longmove_505, NULL};
  Piece free_cat = { "Free Cat", "FC", NULL, NULL, 56, 0, 0, free_cat_moves};

  BasicMove *free_copper_moves[] = {&longmove_207, NULL};
  Piece free_copper = { "Free Copper", "FCo", NULL, NULL, 57, 0, 0, free_copper_moves};

  BasicMove *free_dragon_moves[] = {&shortmove_700, &longmove_7, NULL};
  Piece free_dragon = { "Free Dragon", "FDr", NULL, NULL, 58, 0, 0, free_dragon_moves};

  BasicMove *free_earth_moves[] = {&longmove_202, NULL};
  Piece free_earth = { "Free Earth", "FEa", NULL, NULL, 59, 0, 0, free_earth_moves};

  BasicMove *free_goer_moves[] = {&longmove_202, NULL};
  Piece free_goer = { "Free Goer", "FG", NULL, NULL, 60, 0, 0, free_goer_moves};

  BasicMove *free_gold_moves[] = {&longmove_257, NULL};
  Piece free_gold = { "Free Gold", "FGo", NULL, NULL, 61, 0, 0, free_gold_moves};

  BasicMove *free_iron_moves[] = {&longmove_7, NULL};
  Piece free_iron = { "Free Iron", "FIr", NULL, NULL, 62, 0, 0, free_iron_moves};

  BasicMove *free_leopard_moves[] = {&longmove_707, NULL};
  Piece free_leopard = { "Free Leopard", "FLp", NULL, NULL, 63, 0, 0, free_leopard_moves};

  BasicMove *free_serpent_moves[] = {&longmove_702, NULL};
  Piece free_serpent = { "Free Serpent", "FSe", NULL, NULL, 64, 0, 0, free_serpent_moves};

  BasicMove *free_silver_moves[] = {&longmove_507, NULL};
  Piece free_silver = { "Free Silver", "FSi", NULL, NULL, 65, 0, 0, free_silver_moves};

  BasicMove *free_stone_moves[] = {&longmove_5, NULL};
  Piece free_stone = { "Free Stone", "FSt", NULL, NULL, 66, 0, 0, free_stone_moves};

  BasicMove *free_tiger_moves[] = {&longmove_755, NULL};
  Piece free_tiger = { "Free Tiger", "FT", NULL, NULL, 67, 0, 0, free_tiger_moves};

  BasicMove *free_tile_moves[] = {&longmove_205, NULL};
  Piece free_tile = { "Free Tile", "FTi", NULL, NULL, 68, 0, 0, free_tile_moves};

  BasicMove *free_wolf_moves[] = {&nummove_50_5, &longmove_707, NULL};
  Piece free_wolf = { "Free Wolf", "FW", NULL, NULL, 69, 0, 0, free_wolf_moves};

  BasicMove *golden_bird_moves[] = {&nummove_50_2, &nummove_505_3, &longmove_202, NULL};
  Piece golden_bird = { "Golden Bird", "GBd", NULL, NULL, 70, 0, 0, golden_bird_moves};

  BasicMove *great_dragon_moves[] = {&nummove_202_2, &nummove_505_3, &longmove_50, NULL};
  Piece great_dragon = { "Great Dragon", "GD", NULL, NULL, 71, 0, 0, great_dragon_moves};

  BasicMove *mountain_witch_moves[] = {&shortmove_2, &longmove_705, NULL};
  Piece mountain_witch = { "Mountain Witch", "MW", NULL, NULL, 72, 0, 0, mountain_witch_moves};

  BasicMove *prince_moves[] = {&shortmove_757, NULL};
  Piece prince = { "Prince", "Pr", NULL, NULL, 73, 0, Piece::KING, prince_moves};

  BasicMove *tokin_moves[] = {&shortmove_257, NULL};
  Piece tokin = { "Tokin", "+P", NULL, NULL, 74, 0, 0, tokin_moves};

  BasicMove *teaching_king_moves[] = {&longmove_757, NULL};
  Piece teaching_king = { "Teaching King", "TK", NULL, NULL, 75, 0, 0, teaching_king_moves};

  BasicMove *teaching_king2_moves[] = {&longmove_757, &lionmove_3, NULL};
  Piece teaching_king2 = { "Teaching King2", "TK", NULL, NULL, 75, 0, 0, teaching_king2_moves};

  BasicMove *wizard_stork_moves[] = {&shortmove_200, &longmove_507, NULL};
  Piece wizard_stork = { "Wizard Stork", "WS", NULL, NULL, 76, 0, 0, wizard_stork_moves};

  BasicMove *prom_lance_moves[] = {&shortmove_257, NULL};
  Piece prom_lance = { "Promoted Lance", "+L", NULL, NULL, 77, 0, 0, prom_lance_moves};

  BasicMove *prom_bishop_moves[] = {&shortmove_257, NULL};
  Piece prom_bishop = { "Promoted Bishop", "+B", NULL, NULL, 78, 0, 0, prom_bishop_moves};

  BasicMove *prom_rook_moves[] = {&shortmove_257, NULL};
  Piece prom_rook = { "Promoted Rook", "+R", NULL, NULL, 79, 0, 0, prom_rook_moves};

  BasicMove *prom_reverse_chariot_moves[] = {&shortmove_257, NULL};
  Piece prom_reverse_chariot = { "Promoted Reverse Chariot", "+RC", NULL, NULL, 80, 0, 0, prom_reverse_chariot_moves};

  BasicMove *prom_side_mover_moves[] = {&shortmove_257, NULL};
  Piece prom_side_mover = { "Promoted Side Mover", "+SM", NULL, NULL, 81, 0, 0, prom_side_mover_moves};

  BasicMove *prom_vertical_mover_moves[] = {&shortmove_257, NULL};
  Piece prom_vertical_mover = { "Promoted Vertical Mover", "+VM", NULL, NULL, 82, 0, 0, prom_vertical_mover_moves};

  BasicMove *prom_buddhist_devil_moves[] = {&shortmove_257, NULL};
  Piece prom_buddhist_devil = { "Promoted Buddhist Devil", "+BDe", NULL, NULL, 83, 0, 0, prom_buddhist_devil_moves};

  BasicMove *prom_capricorn_moves[] = {&shortmove_257, NULL};
  Piece prom_capricorn = { "Promoted Capricorn", "+Ca", NULL, NULL, 84, 0, 0, prom_capricorn_moves};

  BasicMove *prom_flying_dragon_moves[] = {&shortmove_257, NULL};
  Piece prom_flying_dragon = { "Promoted Flying Dragon", "+FD", NULL, NULL, 85, 0, 0, prom_flying_dragon_moves};

  BasicMove *prom_donkey_moves[] = {&shortmove_257, NULL};
  Piece prom_donkey = { "Promoted Donkey", "+Do", NULL, NULL, 86, 0, 0, prom_donkey_moves};

  BasicMove *prom_guardian_of_the_gods_moves[] = {&shortmove_257, NULL};
  Piece prom_guardian_of_the_gods = { "Promoted Guardian Of The Gods", "+GG", NULL, NULL, 87, 0, 0, prom_guardian_of_the_gods_moves};

  BasicMove *prom_knight_moves[] = {&shortmove_257, NULL};
  Piece prom_knight = { "Promoted Knight", "+N", NULL, NULL, 88, 0, 0, prom_knight_moves};

  BasicMove *prom_hook_mover_moves[] = {&shortmove_257, NULL};
  Piece prom_hook_mover = { "Promoted Hook Mover", "+HM", NULL, NULL, 89, 0, 0, prom_hook_mover_moves};

  BasicMove *prom_left_chariot_moves[] = {&shortmove_257, NULL};
  Piece prom_left_chariot = { "Promoted Left Chariot", "+LCh", NULL, NULL, 90, 0, 0, prom_left_chariot_moves};

  BasicMove *prom_lion_dog_moves[] = {&shortmove_257, NULL};
  Piece prom_lion_dog = { "Promoted Lion Dog", "+LD", NULL, NULL, 91, 0, 0, prom_lion_dog_moves};

  BasicMove *prom_side_flyer_moves[] = {&shortmove_257, NULL};
  Piece prom_side_flyer = { "Promoted Side Flyer", "+SF", NULL, NULL, 92, 0, 0, prom_side_flyer_moves};

  BasicMove *prom_wrestler_moves[] = {&shortmove_257, NULL};
  Piece prom_wrestler = { "Promoted Wrestler", "+Wr", NULL, NULL, 93, 0, 0, prom_wrestler_moves};

  BasicMove *prom_right_chariot_moves[] = {&shortmove_257, NULL};
  Piece prom_right_chariot = { "Promoted Right Chariot", "+RCh", NULL, NULL, 94, 0, 0, prom_right_chariot_moves};

  BasicMove *prom_she_devil_moves[] = {&shortmove_257, NULL};
  Piece prom_she_devil = { "Promoted She Devil", "+SD", NULL, NULL, 95, 0, 0, prom_she_devil_moves};

  BasicMove *prom_violent_ox_moves[] = {&shortmove_257, NULL};
  Piece prom_violent_ox = { "Promoted Violent Ox", "+VO", NULL, NULL, 96, 0, 0, prom_violent_ox_moves};

  const char *names[] = { "+B", "+BDe", "+Ca", "+Do", "+FD", "+GG", "+HM", "+L", "+LCh", "+LD", "+N", "+P", "+R", "+RC", "+RCh", "+SD", "+SF", "+SM", "+VM", "+VO", "+Wr", "AB", "B", "BB", "BDe", "BSp", "BT", "Bt", "C", "CC", "CS", "CSw", "Ca", "DE", "DH", "DK", "DSp", "DV", "Do", "E", "EW", "Ea", "FBe", "FBo", "FC", "FCo", "FD", "FDr", "FEa", "FF", "FG", "FGo", "FIr", "FK", "FL", "FLp", "FSe", "FSi", "FSt", "FT", "FTi", "FW", "G", "GB", "GBd", "GD", "GG", "HM", "I", "K", "Ky", "L", "LCh", "LD", "Ln", "MW", "N", "OM", "OR", "P", "Ph", "Pr", "R", "RC", "RCh", "RD", "S", "SD", "SF", "SM", "St", "T", "TK", "VM", "VO", "WS", "Wr" };

  int name2pos[] = { 78, 83, 84, 86, 85, 87, 89, 77, 90, 91, 88, 74, 79, 80, 94, 95, 92, 81, 82, 96, 93, 8, 6, 11, 21, 52, 14, 51, 5, 24, 25, 22, 23, 31, 9, 10, 26, 28, 30, 53, 27, 32, 54, 55, 56, 57, 29, 58, 59, 50, 60, 61, 62, 13, 12, 63, 64, 65, 66, 67, 68, 69, 4, 33, 70, 71, 34, 36, 38, 1, 15, 2, 39, 41, 17, 72, 35, 37, 49, 0, 16, 73, 7, 18, 45, 40, 3, 46, 42, 19, 47, 43, 75, 20, 48, 76, 44};

  const Piece *pieces[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &angry_boar, &dragon_horse, &dragon_king, &blind_bear, &ferocious_leopard, &free_king, &blind_tiger, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &buddhist_devil, &cat_sword, &capricorn, &chinese_cock, &coiled_serpent, &dark_spirit, &evil_wolf, &deva, &flying_dragon, &donkey, &drunk_elephant, &earth_general, &go_between, &guardian_of_the_gods, &knight, &hook_mover, &old_monkey, &iron_general, &left_chariot, &reclining_dragon, &lion_dog, &side_flyer, &tile_general, &wrestler, &right_chariot, &she_devil, &stone_general, &violent_ox, &old_rat, &furious_fiend, &bat, &buddhist_spirit, &emperor, &free_bear, &free_boar, &free_cat, &free_copper, &free_dragon, &free_earth, &free_goer, &free_gold, &free_iron, &free_leopard, &free_serpent, &free_silver, &free_stone, &free_tiger, &free_tile, &free_wolf, &golden_bird, &great_dragon, &mountain_witch, &prince, &tokin, &teaching_king, &wizard_stork, &prom_lance, &prom_bishop, &prom_rook, &prom_reverse_chariot, &prom_side_mover, &prom_vertical_mover, &prom_buddhist_devil, &prom_capricorn, &prom_flying_dragon, &prom_donkey, &prom_guardian_of_the_gods, &prom_knight, &prom_hook_mover, &prom_left_chariot, &prom_lion_dog, &prom_side_flyer, &prom_wrestler, &prom_right_chariot, &prom_she_devil, &prom_violent_ox};

    const Piece *pieces2[] = {
     &pawn, &king, &lance, &silver_general, &gold_general, &copper_general, &bishop, &rook, &angry_boar, &dragon_horse, &dragon_king, &blind_bear, &ferocious_leopard, &free_king, &blind_tiger, &kylin, &phoenix, &lion, &reverse_chariot, &side_mover, &vertical_mover, &buddhist_devil, &cat_sword, &capricorn, &chinese_cock, &coiled_serpent, &dark_spirit, &evil_wolf, &deva, &flying_dragon, &donkey, &drunk_elephant, &earth_general, &go_between, &guardian_of_the_gods, &knight, &hook_mover, &old_monkey, &iron_general, &left_chariot, &reclining_dragon, &lion_dog, &side_flyer, &tile_general, &wrestler, &right_chariot, &she_devil, &stone_general, &violent_ox, &old_rat, &furious_fiend, &bat, &buddhist_spirit, &emperor, &free_bear, &free_boar, &free_cat, &free_copper, &free_dragon, &free_earth, &free_goer, &free_gold, &free_iron, &free_leopard, &free_serpent, &free_silver, &free_stone, &free_tiger, &free_tile, &free_wolf, &golden_bird, &great_dragon, &mountain_witch, &prince, &tokin, &teaching_king2, &wizard_stork, &prom_lance, &prom_bishop, &prom_rook, &prom_reverse_chariot, &prom_side_mover, &prom_vertical_mover, &prom_buddhist_devil, &prom_capricorn, &prom_flying_dragon, &prom_donkey, &prom_guardian_of_the_gods, &prom_knight, &prom_hook_mover, &prom_left_chariot, &prom_lion_dog, &prom_side_flyer, &prom_wrestler, &prom_right_chariot, &prom_she_devil, &prom_violent_ox};

  
  Board::Square setup[] = {
     {2, 1}, {32, 1}, {47, 1}, {43, 1}, {38, 1}, {5, 1}, {3, 1}, {4, 1}, {26, 1}, {1, 1}, {28, 1}, {4, 1}, {3, 1}, {5, 1}, {38, 1}, {43, 1}, {47, 1}, {32, 1}, {2, 1}, 
     {18, 1}, {0, 2}, {22, 1}, {0, 2}, {37, 1}, {0, 2}, {40, 1}, {12, 1}, {14, 1}, {31, 1}, {14, 1}, {12, 1}, {25, 1}, {0, 2}, {24, 1}, {0, 2}, {22, 1}, {0, 2}, {18, 1}, 
     {0, 2}, {49, 1}, {0, 2}, {8, 1}, {0, 2}, {11, 1}, {0, 2}, {27, 1}, {16, 1}, {17, 1}, {15, 1}, {27, 1}, {0, 2}, {11, 1}, {0, 2}, {8, 1}, {0, 2}, {49, 1}, {0, 2}, 
     {30, 1}, {0, 2}, {35, 1}, {0, 2}, {48, 1}, {0, 2}, {29, 1}, {46, 1}, {34, 1}, {41, 1}, {44, 1}, {21, 1}, {29, 1}, {0, 2}, {48, 1}, {0, 2}, {35, 1}, {0, 2}, {30, 1}, 
     {7, 1}, {45, 1}, {19, 1}, {42, 1}, {20, 1}, {6, 1}, {9, 1}, {10, 1}, {36, 1}, {13, 1}, {23, 1}, {10, 1}, {9, 1}, {6, 1}, {20, 1}, {42, 1}, {19, 1}, {39, 1}, {7, 1}, 
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
{0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {33, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {7, 0}, {39, 0}, {19, 0}, {42, 0}, {20, 0}, {6, 0}, {9, 0}, {10, 0}, {23, 0}, {13, 0}, {36, 0}, {10, 0}, {9, 0}, {6, 0}, {20, 0}, {42, 0}, {19, 0}, {45, 0}, {7, 0}, 
     {30, 0}, {0, 2}, {35, 0}, {0, 2}, {48, 0}, {0, 2}, {29, 0}, {21, 0}, {44, 0}, {41, 0}, {34, 0}, {46, 0}, {29, 0}, {0, 2}, {48, 0}, {0, 2}, {35, 0}, {0, 2}, {30, 0}, 
     {0, 2}, {49, 0}, {0, 2}, {8, 0}, {0, 2}, {11, 0}, {0, 2}, {27, 0}, {15, 0}, {17, 0}, {16, 0}, {27, 0}, {0, 2}, {11, 0}, {0, 2}, {8, 0}, {0, 2}, {49, 0}, {0, 2}, 
     {18, 0}, {0, 2}, {22, 0}, {0, 2}, {24, 0}, {0, 2}, {25, 0}, {12, 0}, {14, 0}, {31, 0}, {14, 0}, {12, 0}, {40, 0}, {0, 2}, {37, 0}, {0, 2}, {22, 0}, {0, 2}, {18, 0}, 
     {2, 0}, {32, 0}, {47, 0}, {43, 0}, {38, 0}, {5, 0}, {3, 0}, {4, 0}, {28, 0}, {1, 0}, {26, 0}, {4, 0}, {3, 0}, {5, 0}, {38, 0}, {43, 0}, {47, 0}, {32, 0}, {2, 0}};
     
}
  
Board::Type makadaidai_type = {
   pieces, setup, 0, Board::PROM_BY_CAPTURE, 19, 19, 97, names, name2pos, 4, "Maka-Dai-Dai Shogi", 17};

Board::Type makadaidai2_type = {
   pieces2, setup, 0, Board::PROM_BY_CAPTURE, 19, 19, 97, names, name2pos, 4,
   "Maka-Dai-Dai Shogi with teaching king version 2", 18};

