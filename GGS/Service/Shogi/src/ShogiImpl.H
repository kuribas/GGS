#ifndef _SHOGIIMPL_H_
#define _SHOGIIMPL_H_

#include "Global.H"

using namespace std;

struct Point
{
   int x, y;
};

enum {
   HAS_DROPS = 1,
   PROM_BY_CAPTURE = 2,
   NO_PROM = 4,
   MUST_PROM = 8,
   DROP_ANY = 0x10,
   PAWNDROP_CAN_MATE = 0x20,
   HAS_STALEMATE = 0x40
};

enum {
   PAWN = 1,
   KING = 2
};

class MoveWithCheck;

struct Piece
{
   const char *name;
   const Piece *promote_to;
   const Piece *promote_from;
   int num;
   int rank;
   int properties;
   MoveWithCheck **moves;

   bool has_prop(int prop) const {
      return (properties & prop ? true : false); }
};

class Board
{
 public:
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
      char * const *piece_names;
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
      const Type *type;
      Point from, to;
      bool promote : 1;
      bool drop : 1;
      bool piece_given : 1;
      bool square_given : 1;
      int piece;

      Move(const Type *type);
      bool parse(const string mvs);

    private:
      bool read_piece(const char *&mvs);
      bool read_square(const char *&mvs, Point &square);
   };

 protected:
   enum {
      BLACK = 0,
      WHITE = 1,
      NO_SIDE = 2
   };

   const Type *type;
   Square *squares;
   int current_side;
   int game_result;   

   int xy_to_index(int x, int y) const{
      return x + y * type->width; }

   void index_to_xy(int index, int &x, int &y) const {
      y = index / type->width;
      x = index % type->width; }

   bool in_promotion_zone(Move &mv, int side) const {
      return (side == BLACK ?
	      (mv.to.y < type->promotion_zone ||
	       mv.from.y < type->promotion_zone) :
	      (mv.to.y >= type->height - type->promotion_zone ||
	       mv.from.y >= type->height - type->promotion_zone)); }

   int has_prop(int prop) const {
      return (type->properties & prop ? true : false); }

   int otherside(int side) const {
      return (side == 0); }
   
 public:
   const Type *get_type() const { return type; }

   static Board* new_board(sint4 type_p);

   const Piece *piece_at(int x, int y) const {
      return type->pieces[squares[xy_to_index(x, y)].piece]; }

   int get_result() const { return game_result; }

   int get_to_move() const { return current_side; }

   int side_at(int x, int y) const {
      return squares[xy_to_index(x, y)].side; }

   bool on_board(int x, int y) const {
      return (x < type->width && x >= 0 &&
	      y < type->height && y >= 0); }

   bool empty_square(int x, int y) const {
      return side_at(x, y) == NO_SIDE; }

   virtual void write(ostream &os) = 0;
   virtual void write_ggf(ostream &os, bool one_line) = 0;
   virtual bool read_pos(istream &is) = 0;

   void write_board(ostream &os);
   void write_board_ggf(ostream &os, bool one_line);
   bool read_board_pos(istream &is);

   bool one_legal_move(ostream &err, Move &mv);
   bool update_move(Move &mv);
   
   virtual void do_move(Move &mv) = 0;
   virtual bool legal_move(Move &mv) = 0;
   virtual bool is_finished() = 0;
   virtual void init_setup() = 0;

 protected:
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
   ~SmallBoard();
   void operator=(const SmallBoard &bd);

   virtual void do_move(Move &mv);
   virtual bool legal_move(Move &mv);
   virtual bool is_finished();
   virtual void init_setup();

   virtual void write(ostream &os);
   virtual void write_ggf(ostream &os, bool one_line);
   virtual bool read_pos(istream &is);

 private:
   int *captures[2];
   Point king[2];
   int external_storage;
   
   bool in_check(int side, Point *checking_piece = NULL);
   int pawns_on_column(int x, int side);
   void find_kings();
   
   int get_capture(int piece, int side) {
      return captures[side][piece]; }
   
   void do_drop(Move &mv);
   bool legal_drop(Move &mv);
   bool drop_between_check(Point checking_piece);
};

#endif
