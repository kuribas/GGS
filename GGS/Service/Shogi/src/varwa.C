#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece bird, tfalcon, prom_rabbit, bear, boar, prom_dog, prom_monkey;
  extern Piece prom_goose, rfalcon, gswallow, prom_owl, prom_crow, hhorse, plodox;
  
  BasicMove *sparrowmoves[] = {&shortmove_2, NULL};
  Piece sparrow = { "Sparrow", "SP", &bird, NULL, 0, 0, Piece::PAWN, sparrowmoves};

  BasicMove *eaglemoves[] = {&shortmove_550, &longmove_202, &nummove_5_3, NULL};
  Piece eagle = { "Eagle", "CE", NULL, NULL, 1, 0, 0, eaglemoves};

  BasicMove *ffalconmoves[] = {&longmove_505, &shortmove_2, NULL};
  Piece ffalcon = { "Ffalcon", "FFa", &tfalcon, NULL, 2, 0, 0, ffalconmoves};

  BasicMove *rabbitmoves[] = {&shortmove_705, &longmove_2, NULL};
  Piece rabbit = { "Rabbit", "RR", &prom_rabbit, NULL, 3, 0, 0, rabbitmoves};

  BasicMove *wolfmoves[] = {&shortmove_257, NULL};
  Piece wolf = { "Wolf", "VW", &bear, NULL, 4, 0, 0, wolfmoves};

  BasicMove *stagmoves[] = {&shortmove_507, NULL};
  Piece stag = { "Stag", "VSt", &boar, NULL, 5, 0, 0, stagmoves};

  BasicMove *dogmoves[] = {&shortmove_255, NULL};
  Piece dog = { "Dog", "BDg", &prom_dog, NULL, 6, 0, 0, dogmoves};

  BasicMove *monkeymoves[] = {&shortmove_207, NULL};
  Piece monkey = { "Monkey", "CM", &prom_monkey, NULL, 7, 0, 0, monkeymoves};

  BasicMove *goosemoves[] = {&shortmove_207, NULL};
  Piece goose = { "Goose", "FGs", &prom_goose, NULL, 8, 0, 0, goosemoves};

  BasicMove *cockmoves[] = {&shortmove_55, NULL};
  Piece cock = { "Cock", "FC", &rfalcon, NULL, 9, 0, 0, cockmoves};

  BasicMove *swallowmoves[] = {&longmove_50, &shortmove_202, NULL};
  Piece swallow = { "Swallow", "SW", &gswallow, NULL, 10, 0, 0, swallowmoves};

  BasicMove *crowmoves[] = {&shortmove_502, NULL};
  Piece crow = { "Crow", "SC", &prom_crow, NULL, 11, 0, 0, crowmoves};

  BasicMove *owlmoves[] = {&shortmove_502, NULL};
  Piece owl = { "Owl", "SO", &prom_owl, NULL, 12, 0, 0, owlmoves};

  BasicMove *lhorsemoves[] = {&longmove_2, &nummove_200_2, NULL};
  Piece lhorse = { "Lhorse", "LH", &hhorse, NULL, 13, 0, 0, lhorsemoves};

  BasicMove *oxcartmoves[] = {&longmove_2, NULL};
  Piece oxcart = { "Oxcart", "OC", &plodox, NULL, 14, 0, 0, oxcartmoves};

  BasicMove *foxmoves[] = {&shortmove_707, &jumpmove_707, NULL};
  Piece fox = { "Fox", "TFo", NULL, NULL, 15, 0, 0, foxmoves};

  BasicMove *cranemoves[] = {&shortmove_757, NULL};
  Piece crane = { "Crane", "CK", NULL, NULL, 16, 0, Piece::KING, cranemoves};

  BasicMove *tfalconmoves[] = {&longmove_707, &shortmove_50, NULL};
  Piece tfalcon = { "Tfalcon", "TF", NULL, &ffalcon, 17, 0, 0, tfalconmoves};

  BasicMove *gswallowmoves[] = {&longmove_252, NULL};
  Piece gswallow = { "Gswallow", "GSw", NULL, &swallow, 18, 0, 0, gswallowmoves};

  BasicMove *bearmoves[] = {&shortmove_757, NULL};
  Piece bear = { "Bear", "BE", NULL, &wolf, 19, 0, 0, bearmoves};

  BasicMove *plodoxmoves[] = {&shortmove_757, NULL};
  Piece plodox = { "Plodox", "PO", NULL, &oxcart, 20, 0, 0, plodoxmoves};

  BasicMove *boarmoves[] = {&shortmove_557, NULL};
  Piece boar = { "Boar", "RB", NULL, &stag, 21, 0, 0, boarmoves};

  BasicMove *rfalconmoves[] = {&longmove_202, &shortmove_55, NULL};
  Piece rfalcon = { "Rfalcon", "RF", NULL, &cock, 22, 0, 0, rfalconmoves};

  BasicMove *hhorsemoves[] = {&knightmove_202, NULL};
  Piece hhorse = { "Hhorse", "HH", NULL, &lhorse, 23, 0, 0, hhorsemoves};

  BasicMove *birdmoves[] = {&shortmove_257, NULL};
  Piece bird = { "Bird", "GBd", NULL, &sparrow, 24, 0, 0, birdmoves};

  BasicMove *prom_rabbitmoves[] = {&shortmove_707, &jumpmove_707, NULL};
  Piece prom_rabbit = { "Promoted Rabbit", "+RR", NULL, &rabbit, 25, 0, 0, prom_rabbitmoves};

  BasicMove *prom_dogmoves[] = {&shortmove_257, NULL};
  Piece prom_dog = { "Promoted Dog", "+BDg", NULL, &dog, 26, 0, 0, prom_dogmoves};

  BasicMove *prom_monkeymoves[] = {&shortmove_507, NULL};
  Piece prom_monkey = { "Promoted Monkey", "+CM", NULL, &monkey, 27, 0, 0, prom_monkeymoves};

  BasicMove *prom_goosemoves[] = {&longmove_50, &shortmove_202, NULL};
  Piece prom_goose = { "Promoted Goose", "+FGs", NULL, &goose, 28, 0, 0, prom_goosemoves};

  BasicMove *prom_crowmoves[] = {&longmove_505, &shortmove_2, NULL};
  Piece prom_crow = { "Promoted Crow", "+FC", NULL, &crow, 29, 0, 0, prom_crowmoves};

  BasicMove *prom_owlmoves[] = {&shortmove_550, &longmove_202, &nummove_5_3, NULL};
  Piece prom_owl = { "Promoted Owl", "+SO", NULL, &owl, 30, 0, 0, prom_owlmoves};

  const char *names[] = { "+BDg", "+CM", "+FC", "+FGs", "+RR", "+SO", "BDg", "BE", "CE", "CK", "CM", "FC", "FFa", "FGs", "GBd", "GSw", "HH", "LH", "OC", "PO", "RB", "RF", "RR", "SC", "SO", "SP", "SW", "TF", "TFo", "VSt", "VW" };

  int name2pos[] = {26, 27, 29, 28, 25, 30, 6, 19, 1, 16, 7, 9, 2, 8, 24, 18, 23, 13, 14, 20, 21, 22, 3, 11, 12, 0, 10, 17, 15, 5, 4};

  const Piece *pieces[] = {
     &sparrow, &eagle, &ffalcon, &rabbit, &wolf, &stag, &dog, &monkey, &goose, &cock, &swallow, &crow, &owl, &lhorse, &oxcart, &fox, &crane, &tfalcon, &gswallow, &bear, &plodox, &boar, &rfalcon, &hhorse, &bird, &prom_rabbit, &prom_dog, &prom_monkey, &prom_goose, &prom_crow, &prom_owl};

  Board::Square wa_setup[] = {
     {13, 1}, {7, 1}, {12, 1}, {9, 1}, {5, 1}, {16, 1}, {4, 1}, {8, 1}, {11, 1}, {6, 1}, {14, 1},
     {0, 2}, {1, 1}, {0, 2}, {0, 2}, {0, 2}, {10, 1}, {0, 2}, {0, 2}, {0, 2}, {2, 1}, {0, 2},
     {0, 1}, {0, 1}, {0, 1}, {3, 1}, {0, 1}, {0, 1}, {0, 1}, {15, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 1}, {0, 2}, {0, 2}, {0, 2}, {0, 1}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, 
     {0, 2}, {0, 2}, {0, 2}, {0, 0}, {0, 2}, {0, 2}, {0, 2}, {0, 0}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {15, 0}, {0, 0}, {0, 0}, {0, 0}, {3, 0}, {0, 0}, {0, 0}, {0, 0}, 
     {0, 2}, {2, 0}, {0, 2}, {0, 2}, {0, 2}, {10, 0}, {0, 2}, {0, 2}, {0, 2}, {1, 0}, {0, 2},
     {14, 0}, {6, 0}, {11, 0}, {8, 0}, {4, 0}, {16, 0}, {5, 0}, {9, 0}, {12, 0}, {7, 0}, {13, 0}};
}

SmallBoard::Type wa_type = {
   pieces, wa_setup, 3, 0, 11, 11, 31, names, name2pos, 4, "Wa Shogi", 7, 0, 0};

SmallBoard::Type wa2_type = {
   pieces, wa_setup, 3, Board::HAS_DROPS, 11, 11, 31, names, name2pos, 4, "Wa Shogi with drops", 8, 16, 1};
