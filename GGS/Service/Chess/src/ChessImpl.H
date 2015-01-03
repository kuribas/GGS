// $Id: ChessImpl.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#ifndef ChessImpl_H
#define ChessImpl_H

#include "Global.H"
#include "String.H"
#include <vector>


class BoardType {

public:

  enum T {
    UNDEF   = 0,
    TYPE_6  = 6,    // 6x6
    TYPE_8  = 8,    // 8x8
    TYPE_10 = 10,   // 10x10
    TYPE_12 = 12    // 12x12
  };

  enum {
    MAX_BOARD_WIDTH = 12,
    DX = MAX_BOARD_WIDTH+4,    // knight!
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

public:

  
private:

  sint4 from;
  sint4 to;
  sint4 prom;
  
public:

  enum { NO_PROM=0, PROM_TO_KNIGHT, PROM_TO_BISHOP, 
         PROM_TO_ROOK, PROM_TO_QUEEN };
  
  enum { MV_UNDEF = -2, MV_PASS = -1 };

  Move(sint4 sq=MV_UNDEF);
  
  bool parse(std::ostream& os, const String &arg);
  void write(std::ostream &os) const;

  bool is_valid() const;
  
  static sint4 xy_to_ind(sint4 x, sint4 y);
  static void  ind_to_xy(sint4 ind, sint4 &x, sint4 &y);

  static void write_square(std::ostream &os, sint4 sq);
  static bool read_square (std::istream &is, sint4 &sq);

  friend bool operator == (const Move &m1, const Move &m2);
  friend class Board;
};

inline bool operator == (const Move &m1, const Move &m2) {
  return m1.from == m2.from && m1.to == m2.to && m1.prom == m2.prom;
}

typedef std::vector<Move> MoveList;


class Board {

private:

  enum { NONE=0, PAWN=1, KNIGHT=2, BISHOP=3, ROOK=4, QUEEN=5, KING=6 };
	 
  enum { BLACK=0, WHITE=1, COLOR_UNDEF=2 }; // do not change
  
  enum SqCont {
    EMPTY    =  0,
    BORDER   = 100, 

    W_PAWN   =  PAWN,
    W_KNIGHT =  KNIGHT,
    W_BISHOP =  BISHOP,
    W_ROOK   =  ROOK,
    W_QUEEN  =  QUEEN,
    W_KING   =  KING,
    
    B_PAWN   = -PAWN,
    B_KNIGHT = -KNIGHT,
    B_BISHOP = -BISHOP,
    B_ROOK   = -ROOK,
    B_QUEEN  = -QUEEN,
    B_KING   = -KING,
  };

  enum { DX=BoardType::DX };
  enum { W=-1, E=1, S=DX, N=-DX, SW=S+W, SE=S+E, NW=N+W, NE=N+E };
  enum { TWO_RANKS=2*DX };
  
  sint4 type;
  SqCont squares[BoardType::MAX_BOARD_SIZE];
  sint4 turn_color;              // BLACK | WHITE
  sint4 rev_move_num;            // number of reversible moves (plies)
  sint4 ep_square;               // < 0 -> no ep possible
  bool CK[2], CQ[2];             // castle status, 0-BLACK 1-WHITE

  void regular_setup();      // assumes correct borders and empty board
  void random_setup(int ra); // assumes correct borders and empty board

  void toggle_to_move();
  static const int d[8];
  static const int UP_ONE_RANK_FOR_COLOR[2];
  static const int HORI_VERT_D[4];
  static const int DIAG_D[4];
  static const int KING_D[8];
  static const int KNIGHT_D[8];  
  
  static sint4 opponent(sint4 col);
  static void write_cont(std::ostream &os, sint4 cont);
  static bool read_cont (std::istream &is, SqCont &cont);
  static void  add_normal_move(MoveList &ml, sint4 from, sint4 to);
  static sint4 man_of(sint4 sc);
  static bool  is_man(sint4 sc);
  static SqCont make_cont(sint4 pc, sint4 col);
  static sint4 color_of(sint4 sc);
  static bool  has_color(sint4 cf, sint4 col);

  
  sint4 king_loc(sint4 col) const;
  void attacked(sint4 sq, bool a[2]) const;
  void pseudo_legal_moves(sint4 col, MoveList &ml) const;
  bool in_check(sint4 col) const;
  void moves_on_circle(sint4 col, MoveList &ml, sint4 sq, const int d[8]) const;
  void moves_on_line(sint4 col, MoveList &ml, sint4 sq, int d) const;
  sint4 scan_line(sint4 sq, sint4 d) const;

  void append_knight_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const;
  void append_bishop_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const;
  void append_rook_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const;
  void append_queen_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const;
  void legal_moves(sint4 col, MoveList &ml) const;

  sint4 ll_sq() const;
  sint4 lr_sq() const;
  sint4 ul_sq() const;
  sint4 ur_sq() const;
  sint4 l_sq(sint4 col) const;
  sint4 r_sq(sint4 col) const;

  sint4 count_pieces(sint4 pc) const;

public:

  Board() {}

  void init(sint4 board_type, sint4 rand_type);  // ra < 0: regular setup

  sint4 get_to_move() const;
  sint4 get_type() const { return type; }
  bool legal_move(const Move &mv);
  void do_move(const Move &move);
  bool no_moves(bool opponent = false) const;
  bool is_finished() const;
  sint4 blacks_result() const;
  sint4 get_board_type() const { return type; }
  void write(std::ostream &os) const;
  void write_ggf(std::ostream &os, bool one_line = false) const;
  bool read_ggf (std::istream &is);
  sint4 get_rev_move_num() const { return rev_move_num; }
  uint4 adler32() const;

  bool sufficient_material(sint4 col) const;

  ~Board() {}

};

#endif

