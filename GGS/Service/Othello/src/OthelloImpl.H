// $Id: OthelloImpl.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#ifndef OthelloImpl_H
#define OthelloImpl_H

#include "Global.H"
#include "String.H"

#define NICOLET_RANDOM 1

class BoardType {

public:

  enum T {
    UNDEF   = 0,
    TYPE_4  = 4,        // 4x4
    TYPE_6  = 6,        // 6x6
    TYPE_8  = 8,        // 8x8
    TYPE_10 = 10,       // 10x10
    TYPE_12 = 12,       // 12x12
    TYPE_14 = 14,       // 14x14
    TYPE_88 = 88        // 10x10 octogon (88 squares)
  };

  enum {
    MAX_BOARD_WIDTH = 14,
    DX = MAX_BOARD_WIDTH+2,
    MAX_BOARD_SIZE = DX * DX,
    MAX_MOVE_NUM   = MAX_BOARD_SIZE
  };

  static bool  parse(std::istream &is, sint4 &bt);
  static bool  legal_rand_type(sint4 bt, sint4 rand_type);

  static sint4 board_width(sint4 bt);
  static sint4 board_squares(sint4 bt);
};


class Board;

class Move {

private:
  
  sint4 sq_index;    // base BoardType::DX
  
public:

  enum { UNDEF = -2, PASS = -1 };

  Move(sint4 sqindex = UNDEF);
  
  bool parse(std::ostream& os, const String &arg);
  void write(std::ostream &os) const;

  bool is_valid() const;
  bool is_pass() const;

  void from_xy(sint4 x, sint4 y);
  void to_xy(sint4 &x, sint4 &y) const;

  static sint4 xy_to_ind(sint4 x, sint4 y);
  static void  ind_to_xy(sint4 ind, sint4 &x, sint4 &y);

  friend class Board;
};


class Board {

public:

  static bool use_time_seed;
  static sint4 rand_int(sint4 limit);

private:

  sint4 turn_color;  // Color, not SqCont
  
  enum SqCont { EMPTY=0, BLACK=1, WHITE=-1, BORDER=-2 };
  
  sint4 type;
  SqCont squares[BoardType::MAX_BOARD_SIZE];

  sint4 disc_num(SqCont cont) const;
  sint4 disc_num() const;  
  
  void regular_setup();      // assumes correct borders and empty board
  void random_setup(int ra); // assumes correct borders and empty board
#if NICOLET_RANDOM  
  bool move_is_legal(const Move &mv);
  void random_setup_2(int ra);
#endif

  void toggle_to_move();
  static int d[8];

public:

  Board() {}

  void init(sint4 board_type, sint4 rand_type);  // ra < 4: regular setup

  sint4 get_to_move() const;
  sint4 get_type() const { return type; }
  bool make_move(const Move &mv);
  bool no_moves(bool opponent = false) const;
  bool is_finished() const;
  sint4 blacks_result() const;
  sint4 get_board_type() const { return type; }
  void write(std::ostream &os) const;
  void write_ggf(std::ostream &os, bool one_line = false) const;
  bool  read_ggf(std::istream &is);

  ~Board() {}

};

#endif

