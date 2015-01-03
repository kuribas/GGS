#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece dolphin2;

  //                   name, prom_to, ~from, num, rank, props, moves
  MoveWithCheck *dolphinmoves[] = {&shortmove_2, NULL};
  Piece dolphin = { "D", &dolphin2, NULL, 0, 0, PAWN, dolphinmoves};

  MoveWithCheck *humpbackmoves[] = {&shortmove_705, NULL};
  Piece humpback = { "H", NULL, NULL, 1, 0, 0, humpbackmoves};

  MoveWithCheck *greywhalemoves[] = {&longmove_502, NULL};
  Piece greywhale = { "G", NULL, NULL, 2, 0, 0, greywhalemoves};

  MoveWithCheck *bluewhalemoves[] = {&shortmove_207, NULL};
  Piece bluewhale = { "B", NULL, NULL, 3, 0, 0, bluewhalemoves};

  MoveWithCheck *narwhalmoves[] = {&shortmove_250, &jumpmove_2, NULL};
  Piece narwhal = { "N", NULL, NULL, 4, 0, 0, narwhalmoves};

  MoveWithCheck *killerwhalemoves[] = {&longmove_252, &shortmove_505, NULL};
  Piece killerwhale = { "K", NULL, NULL, 5, 0, 0, killerwhalemoves};

  MoveWithCheck *dolphin2moves[] = {&longmove_500, NULL};
  Piece dolphin2 = { "D", NULL, &dolphin, 6, 0, 0, dolphin2moves};

  MoveWithCheck *porpoisemoves[] = {&shortmove_50, NULL};
  Piece porpoise = { "P", NULL, &killerwhale, 7, 0, 0, porpoisemoves};

  MoveWithCheck *whitewhalemoves[] = {&shortmove_757, NULL};
  Piece whitewhale = { "W", NULL, NULL, 8, 0, KING, whitewhalemoves};


  //names in alphabetic order
  char *names[] = {"B", "D", "D", "G", "H", "K", "N", "P", "W"};

  int name2pos[] = {3, 0, 0, 2, 1, 5, 4, 7, 8};

  const Piece *pieces[] = {&dolphin, &humpback, &greywhale, &bluewhale,
			   &narwhal, &killerwhale, &dolphin2, &porpoise, &whitewhale};

  Board::Square whale_setup[] = {
     {3, 1}, {4, 1}, {7, 1}, {8, 1}, {2, 1}, {1, 1},
     {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2}, {0, 2},
     {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
     {1, 0}, {2, 0}, {8, 0}, {7, 0}, {4, 0}, {3, 0}};

}

SmallBoard::Type whale_type = {
   pieces, whale_setup, 0, HAS_DROPS | NO_PROM, 6, 6, 9, names, name2pos, 1, "Whale Shogi", 4, 6, 0};
