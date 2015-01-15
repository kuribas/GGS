#ifndef _SHOGIIMPL_H_
#define _SHOGIIMPL_H_

#include <list>
#include <algorithm>
#include <string>

#define BITS_IN_INT (sizeof(int) * 8)
#define EOL '\n'


using namespace std;

/* optimized version of abs */
inline
int abs(int a)
{
   return (a ^ (a >> (BITS_IN_INT - 1) )) -
     (a >> (BITS_IN_INT - 1));
}

struct Point
{
   int x, y;
};

inline bool operator==(Point a, Point b)
{
   return a.x == b.x && a.y == b.y;
}

inline bool operator!=(Point a, Point b)
{
   return a.x != b.x || a.y != b.y;
}


inline int sqr_distance(Point p1, Point p2)
{
   return max(abs(p1.x - p2.x),
	      abs(p1.y - p2.y));
}


class BasicMove;

struct Piece
{
   enum {
      PAWN = 1,
      KING = 2,
      EMPEROR = 4,
      LION = 8,
      PAWNLIKE = 0x10,
      FIREDEMON = 0x20
   };

   const char *long_name;
   const char *name;
   const Piece *promote_to;
   const Piece *promote_from;
   int num;
   int rank;
   int properties;
   BasicMove **moves;
   
   bool has_prop(int prop) const {
      return (properties & prop ? true : false); }
};

class Board
{
 public:
   enum {
      HAS_DROPS = 1,
      PROM_BY_CAPTURE = 2,
      NO_PROM = 4,
      MUST_PROM = 8,
      DROP_ANY = 0x10,
      PAWNDROP_CAN_MATE = 0x20,
      HAS_STALEMATE = 0x40,
      RESTRICTED_LION = 0x80,
      PROMOTION_RESTRICT = 0x100,
      PAWN_PROM_RESTRICT = 0x200,
      HAS_FIREDEMON = 0x400
   };

   struct Square {
      short piece;
      short side;
   };

   struct Type {
      const Piece **pieces;
      const Square *setup;
      int promotion_zone;
      int properties;
      int width, height;
      int nPieces;
      const char *const *piece_names;
      int *name2pos;
      int name_max;
      const char *name;
      int num;
      
      bool has_prop(int prop) const {
	 return (properties & prop ? true : false); }

      int num_from_name(const char *name) const;
   };
   
   class Move
   {
    public:
      enum {
	 SIMPLE = 1,
	 DROP,
	 IGUI,
	 PASS,
	 DOUBLE,
	 TRIPLE
      };
      const Type *bdtype;
      Point from, to, to2, to3;
      bool promote : 1;
      bool piece_given : 1;
      bool square_given : 1;
      int type;
      int piece;
      
      Move(const Type *type);
      Move() { }
      bool parse(const string mvs);

    private:
      bool read_piece(const char *&mvs);
      bool read_square(const char *&mvs, Point &square);
      inline bool read_next_move(const char *&mvs, Point &sqr);
   };

 public:
   enum {
      BLACK = 0,
      WHITE = 1,
      NO_SIDE = 2
   };

 protected:
   const Type *type;
   Square *squares;
   int current_side;
   int game_result;

   enum {
      CANNOT_PROMOTE = 0,
      MUST_PROMOTE = 1,
      MUST_STAY = 2,
      CAN_PROMOTE = 3
   };
   
   int promote_next;
   
   bool row_in_prom_zone(int row, int side) const {
      return (side == BLACK ?
	      row < type->promotion_zone :
	      row >= type->height - type->promotion_zone); }

   int xy_to_index(int x, int y) const{
      return x + y * type->width; }

   void index_to_xy(int index, int &x, int &y) const {
      y = index / type->width;
      x = index % type->width; }

   int has_prop(int prop) const {
      return (type->properties & prop ? true : false); }

   int otherside(int side) const {
      return (side == 0); }

 public:
   int partial_move;
   virtual ~Board();
   int can_promote() const { return promote_next; }
   
   const Piece *lookup_piece(const char *piece);
   const Type *get_type() const { return type; }

   static Board* new_board(int type_p);

   const Piece *piece_at(int x, int y) const {
      return type->pieces[squares[xy_to_index(x, y)].piece]; }

   const Piece *piece_at(Point p) const {
      return piece_at(p.x, p.y); }

   int get_result() const { return game_result; }
   int get_to_move() const { return current_side; }
   int side_at(int x, int y) const {
      return squares[xy_to_index(x, y)].side; }

   int side_at(Point p) const {
      return side_at(p.x, p.y); }

   bool on_board(int x, int y) const {
      return (x < type->width && x >= 0 &&
	      y < type->height && y >= 0); }

   bool on_board(Point p) const {
      return on_board(p.x, p.y); }

   bool empty_square(int x, int y) const {
      return side_at(x, y) == NO_SIDE; }

   bool empty_square(Point p) const {
      return side_at(p) == NO_SIDE; }

   virtual void write(ostream &os) = 0;
   virtual void write_ggf(ostream &os, bool one_line) = 0;
   virtual bool read_pos(istream &is) = 0;
   virtual Board *duplicate();
   
   void write_board(ostream &os);
   void write_board_ggf(ostream &os, bool one_line);
   bool read_board_pos(istream &is);

   bool one_legal_move(Move &mv);
   bool update_move(Move &mv);
   
   virtual void do_move(Move &mv) = 0;
   virtual bool legal_move(Move &mv) = 0;
   virtual bool is_finished() = 0;
   virtual void init_setup() = 0;

 protected:
   void clear_square(Point sqr) {
      clear_square(sqr.x, sqr.y);
   }
   void clear_square(int x, int y) {
      squares[xy_to_index(x, y)].side = NO_SIDE;
   }
   
   void set_at(Point sqr, int piece, int side) {
      squares[xy_to_index(sqr.x, sqr.y)].piece = piece;
      squares[xy_to_index(sqr.x, sqr.y)].side = side;
   }
   inline bool valid_piece_move(const Piece *p, Move &mv, int side);
   bool find_next_legal(int &x, int &y, Move &mv);
   void write_square(ostream &os, int side, const Piece* piece);
   bool read_square(istream &is, int pos);
};

class SmallBoard : public Board
{
 public:
   struct Type {
      Board::Type type;
      int captures_size;
      int max_pawns_on_column;
   };

 public:
   SmallBoard(const SmallBoard::Type *type);
   SmallBoard(Square *board, int *captures1, int *captures2);
   SmallBoard(const SmallBoard &brd);
   virtual ~SmallBoard();
   void operator=(const SmallBoard &bd);

   virtual void do_move(Move &mv);
   virtual bool legal_move(Move &mv);
   virtual bool is_finished();
   virtual void init_setup();
   int get_capture(int piece, int side) {
      return captures[side][piece]; }
   
   virtual Board *duplicate();
   virtual void write(ostream &os);
   virtual void write_ggf(ostream &os, bool one_line);
   virtual bool read_pos(istream &is);

   void set_pos(Square *pos, int *black, int *white, int to_move);
      
 private:
   int *captures[2];
   Point king[2];
   int external_storage;

   bool in_promotion_zone(Move &mv, int side) const {
      return (row_in_prom_zone(mv.from.y, side) ||
	      row_in_prom_zone(mv.to.y, side)); }

   bool in_check(int side, Point *checking_piece = NULL);
   int pawns_on_column(int x, int side);
   void find_kings();
   
   void do_drop(Move &mv);
   bool legal_drop(Move &mv);
   bool drop_between_check(Point checking_piece);
   void update_pos();
};

class LargeBoard : public Board
{
 public:
   LargeBoard(const Board::Type *type);
   LargeBoard(const LargeBoard &brd);
   virtual ~LargeBoard();
   
   virtual void do_move(Move &mv);
   virtual bool legal_move(Move &mv);
   virtual bool is_finished();
   virtual void init_setup();
   
   virtual Board *duplicate();
   virtual void write(ostream &os);
   virtual void write_ggf(ostream &os, bool one_line);
   virtual bool read_pos(istream &is);

   void set_pos(Square *pos, int to_move);
   bool is_partial() { return partial_move; }

 public:
   int n_burns;
   Point burned_pieces[8];

 private:
   list<Point> no_prom;
   bool can_capture_lion;
   int nKings[2];
   int piececount[2];
   int has_emperor[2];

 private:
   inline bool promotion_restricted(Point p, int side);

   inline void restrict_promotion(Point p) {
      no_prom.push_front(p); }

   inline void remove_restriction(int x, int y, int side) {
      Point p = {x, y};
      if(!row_in_prom_zone(p.y, side))
	 return;
      no_prom.remove(p); }

   inline bool in_promotion_zone(Move &mv, int side);
   inline bool burned_lion(Move &mv, int side);
   int get_promotion(Move &mv, const Piece *p);

   bool valid_sides(Move &mv);
   bool valid_promotion(Move &move, const Piece *p);
   bool valid_lion_move(Move &mv);
   bool valid_emperor_move(Move &mv);
   inline bool captures_lion(Move &mv);
   bool is_defended_from(Point target, Point piece);
   bool is_capture(Move &mv);
   bool next_to_firedemon(Point sqr, int side);
   inline bool pass_firedemon(Move &mv, int side);
   bool king_can_capture(int side);
   void remove_at(int x, int y);
   void remove_at(Point p) { remove_at(p.x, p.y); }
   void count_pieces();
};

#include "Moves.H"

inline bool Board::valid_piece_move(const Piece *p, Move &mv, int side)
{
   for(int i = 0; p->moves[i]; i++)
      if(p->moves[i]->is_valid(*this, mv, side))
	 return true;
   return false;
}

inline bool LargeBoard::in_promotion_zone(Move &mv, int side)
{
   switch(mv.type) {
     case Move::TRIPLE:
	if(row_in_prom_zone(mv.to3.y, side))
	   return true;
     case Move::DOUBLE:
	if(row_in_prom_zone(mv.to2.y, side))
	   return true;
     case Move::IGUI:
     case Move::SIMPLE:
	if(row_in_prom_zone(mv.to.y, side))
	   return true;
     case Move::PASS:
	if(row_in_prom_zone(mv.from.y, side))
	   return true;
     default:
	return false;
   }
}

inline bool LargeBoard::promotion_restricted(Point p, int side)
{
   list<Point>::iterator result;

   if(!row_in_prom_zone(p.y, side))
      return false;

   result = find(no_prom.begin(),
		 no_prom.end(), p);
   return (result != no_prom.end());
}

#endif
