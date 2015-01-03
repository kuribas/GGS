// $Id: SnortImpl.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#ifndef SnortImpl_H
#define SnortImpl_H

#include "Global.H"
#include "String.H"

class BoardType {

public:

  enum T {
    UNDEF   = 0,
    TYPE_MAX = 25
  };

  enum {
    MAX_BOARD_WIDTH = 2*TYPE_MAX, // 2*MAX(EDGE LENGTH)
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
  
  sint4 index;  // base BoardType::DX
  
public:

  enum { UNDEF = -2 };

  Move();
  
  bool parse(std::ostream& os, const String &arg);
  void write(std::ostream &os) const;

  bool is_valid() const;
  
  static sint4 xy_to_ind(sint4 x, sint4 y);
  static void  ind_to_xy(sint4 ind, sint4 &x, sint4 &y);

  friend class Board;
};


class Board {

public:

  static bool use_time_seed;
  static sint4 rand_int(sint4 limit);

  enum SqCont { BORDER = -1, EMPTY=0, BLACK=1, WHITE=2, 
                NEARBLACK = 3, NEARWHITE = 4, BLOCKED = 5 };

  // ggf char constants
  enum {
    EMPTY_SQUARE = -9,
    EMPTY_EDGE   = -73,
    OCC_EDGE_H   = '-',
    OCC_EDGE_V   = '|',
    BLACK_SQUARE = '*',
    WHITE_SQUARE = 'O',
    NODE         = '+'
  };
  
private:

  sint4 turn_color;  // Color, not SqCont
  
  sint4 type;
  SqCont squares[BoardType::MAX_BOARD_SIZE];

  sint4 count(SqCont cont) const;
  
  void regular_setup();      // assumes correct borders and empty board
  void random_setup(int ra); // assumes correct borders and empty board

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
  void gen_write(std::ostream &os, bool ggf, bool one_line) const;
  void write(std::ostream &os) const;
  void write_ggf(std::ostream &os, bool one_line = false) const;
  bool read_ggf(std::istream &is);

// MM for testing
    void make_random_move();
    void make_good_move();
    int eval(int move) const;

  ~Board() {}

};

#endif
