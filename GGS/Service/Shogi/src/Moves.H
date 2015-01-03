#ifndef _MOVES_H_
#define _MOVES_H_

#include "ShogiImpl.H"
#include <vector>

enum
{
   BLACK = 0,
   WHITE = 1
};

enum
{
   FORWARD_LEFT =  1,
   FORWARD =       2,
   FORWARD_RIGHT = 4,
   
   LEFT  = 0x10,
   RIGHT = 0x40,
   
   BACKWARD_LEFT =  0x100,
   BACKWARD =       0x200,
   BACKWARD_RIGHT = 0x400
};

/*       _\|/_
	 (o o)
 +----oOO-{_}-OOo-----------+
 |                          |
 |                          |
 |        BasicMove:        |
 |                          |
 | Base Class for all moves |
 |   (except LionMove)      |
 |                          |
 +-------------------------*/


class BasicMove
{
 public:
   virtual bool is_valid(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side) = 0;

   virtual bool has_board_moves(const Board *pos,
				int from_x, int from_y, int side) = 0;

    protected:
   inline int reverse_direction(int direction);
   inline bool normalize(int &dx, int &dy);
   int has_direction(int flags, int dx, int dy)
     { return (flags & (1 << ((dx + 1) + (dy + 1) * 4))); }
};

inline
int BasicMove::reverse_direction(int direction)
{
   // swap LEFT <=> RIGHT
   direction = ((direction & 0x111) << 2) | ((direction & 0x444) >> 2) |
               (direction & 0x222);
   // swap FORWARD <=> BACKWARD
   direction = ((direction & 0xF00) >> 8) | ((direction & 0x00F) << 8) |
               (direction & 0x0F0);
   return direction;
};

inline
bool BasicMove::normalize(int &dx, int &dy)
{
   if(dx) {
       if(dy && abs(dx) != abs(dy)) return false;
       dx = (dx > 0 ? 1 : -1);
   }

   if(dy)
      dy = (dy > 0 ? 1 : -1);

   return true;
};

/***************************************
 *            MoveWithCheck:           *
 *                                     *
 *   Base Class for moves in games     *
 *   with Check (where putting your    *
 *   king in danger is not allowed)    *
 *                                     *
 ***************************************/

class MoveWithCheck : public BasicMove
{
 public:
   // true if this piece could attack the destination square
   virtual bool can_reach(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side) = 0;

   virtual void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist) = 0;
};

/**************************************
 *            All Real Moves          *
 **************************************/

/***  ShortMove  ***/

// A move to an adjacent square
class ShortMove : public MoveWithCheck
{
 public:
   ShortMove(int moveinfo_p);

   virtual bool is_valid(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side);
   virtual bool can_reach(const Board *pos,
			  int from_x, int from_y, int to_x, int to_y, int side);
   virtual bool has_board_moves(const Board *pos,
				int from_x, int from_y, int side);
   void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist);

 private:
   int moveinfo[2];
};

/***  LongMove  ***/

// A move over vacant squares in some directions
class LongMove : public MoveWithCheck
{
 public:
   LongMove(int moveinfo_p);

   virtual bool is_valid(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side);
   virtual bool can_reach(const Board *pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side);
   virtual bool has_board_moves(const Board *pos,
				int from_x, int from_y, int side);
   void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist);

 private:
   int moveinfo[2];
};

/***  KnightMove  ***/

// A shogi-Knight move (jump to a square 2 forward and 1 left or right)
class KnightMove : public MoveWithCheck
{
 public:
   KnightMove(int info);
   virtual bool is_valid(const Board *pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side);
    virtual bool can_reach(const Board *pos,
			   int from_x, int from_y,
			   int to_x, int to_y, int side);
    virtual bool has_board_moves(const Board *pos,
				 int from_x, int from_y, int side);
   void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist);
 private:
    int moveinfo[2];
};

/***  JumpMove  ***/

// A move that jumps over an adjacent square
class JumpMove : public MoveWithCheck
{
 public:
   JumpMove(int moveinfo);
   
   virtual bool is_valid(const Board *pos,
			int from_x, int from_y,
			int to_x, int to_y, int side);
   virtual bool can_reach(const Board *pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side);
   virtual bool has_board_moves(const Board *pos,
				int from_x, int from_y, int side);
   void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist);
 private:
   int moveinfo[2];
};

/***  NumMove  ***/

// A move that moves maximum n square in the given directions
class NumMove : public MoveWithCheck
{
 public:
   NumMove(int moveinfo, int num);
   
   virtual bool is_valid(const Board *pos,
			int from_x, int from_y,
			int to_x, int to_y, int side);
   virtual bool can_reach(const Board *pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side);
   virtual bool has_board_moves(const Board *pos,
				int from_x, int from_y, int side);
   void valid_moves(const Board *pos,
		    int from_x, int from_y, int side,
		    vector<Point> &movelist);
 private:
   int moveinfo[2];
   int num;
};

#include "moves_decl.inc"

#endif
