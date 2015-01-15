#include "ShogiImpl.H"
#include "Moves.H"

namespace
{
  extern Piece dolphin2;

  //                   name, prom_to, ~from, num, rank, props, moves
  BasicMove *dolphinmoves[] = {&shortmove_2, NULL};
  Piece dolphin = { "Dolphin", "D", &dolphin2, NULL, 0, 0, Piece::PAWN, dolphinmoves};

  BasicMove *humpbackmoves[] = {&shortmove_705, NULL};
  Piece humpback = { "Humpback", "H", NULL, NULL, 1, 0, 0, humpbackmoves};

  BasicMove *greywhalemoves[] = {&longmove_502, NULL};
  Piece greywhale = { "Greywhale", "G", NULL, NULL, 2, 0, 0, greywhalemoves};

  BasicMove *bluewhalemoves[] = {&shortmove_207, NULL};
  Piece bluewhale = { "Bluewhale", "B", NULL, NULL, 3, 0, 0, bluewhalemoves};

  BasicMove *narwhalmoves[] = {&shortmove_250, &jumpmove_2, NULL};
  Piece narwhal = { "Narwhal", "N", NULL, NULL, 4, 0, 0, narwhalmoves};

  BasicMove *killerwhalemoves[] = {&longmove_252, &shortmove_505, NULL};
  Piece killerwhale = { "Killerwhale", "K", NULL, NULL, 5, 0, 0, killerwhalemoves};

  BasicMove *dolphin2moves[] = {&longmove_500, NULL};
  Piece dolphin2 = { "Dolphin2", "D", NULL, &dolphin, 6, 0, 0, dolphin2moves};

  BasicMove *porpoisemoves[] = {&shortmove_50, NULL};
  Piece porpoise = { "Porpoise", "P", NULL, &killerwhale, 7, 0, 0, porpoisemoves};

  BasicMove *whitewhalemoves[] = {&shortmove_757, NULL};
  Piece whitewhale = { "Whitewhale", "W", NULL, NULL, 8, 0, Piece::KING, whitewhalemoves};


  //names in alphabetic order
  const char *names[] = {"B", "D", "D", "G", "H", "K", "N", "P", "W"};

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
   pieces, whale_setup, 0, Board::HAS_DROPS | Board::NO_PROM, 6, 6, 9, names, name2pos, 1, "Whale Shogi", 4, 6, 0};
