#include <stdlib.h>
#include "ShogiImpl.H"
#include "Moves.H"
#include <vector>

#define BITS_IN_INT (sizeof(int) * 8)

/* optimized version of abs */
inline
int abs(int a)
{
   return (a ^ (a >> (BITS_IN_INT - 1) )) -
     (a >> (BITS_IN_INT - 1));
}

#define max(a, b) ((a) > (b) ? a : b)
#define min(a, b) ((a) < (b) ? a : b)

/**************************
 *      ShortMove        *
 **************************/

ShortMove::ShortMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool ShortMove::is_valid(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side)
{
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(abs(dx) > 1 || abs(dy) > 1)
      return false;

   return (has_direction(moveinfo[side], dx, dy));
}

bool ShortMove::can_reach(const Board *pos,
			   int from_x, int from_y,
			   int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}

void ShortMove::valid_moves(const Board *pos,
			    int from_x, int from_y, int side,
			    vector<Point> &movelist)
{
   int info = moveinfo[side];
   Point i;

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift)) {
	  i.x = from_x + (shift % 4) - 1;
	  i.y = from_y + shift / 4 - 1;
	  movelist.push_back(i);
      }
}

bool ShortMove::has_board_moves(const Board *pos,
				int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
	 if(pos->on_board(from_x + (shift % 4) - 1,
			  from_y + shift / 4 - 1))
	    return true;
   return false;
}

/****************************
 *        LongMove          *
 ****************************/

LongMove::LongMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

   
bool LongMove::is_valid(const Board *pos,
			int from_x, int from_y,
			int to_x, int to_y, int side)
{
   int info = moveinfo[side];
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   while(true)
   {
       from_x += dx;
       from_y += dy;
       
       if(from_x == to_x && from_y == to_y)
	  return true;
       if(!pos->empty_square(from_x, from_y))
	  return false;
   }
}

bool LongMove::can_reach(const Board *pos,
			   int from_x, int from_y,
			   int to_x, int to_y, int side)
{
   int info = moveinfo[side];
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(!normalize(dx, dy))
      return false;

   return (has_direction(info, dx, dy));
}

void LongMove::valid_moves(const Board *pos,
			   int from_x, int from_y, int side,
			   vector<Point> &movelist)
{
   int info = moveinfo[side];
   
   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
      {
	  int dx = (shift % 4) - 1;
	  int dy = (shift / 4) - 1;
	  Point i = {from_x, from_y};
	  
	  while(true)
	  {
	      i.x += dx;
	      i.y += dy;
	      
	      if(!pos->on_board(i.x, i.y))
		 break;

	      movelist.push_back(i);
	      
	      if(!pos->empty_square(i.x, i.y))
		  break;
	  }
      }
}

bool LongMove::has_board_moves(const Board *pos,
			       int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
	 if(pos->on_board(from_x + (shift % 4) - 1,
			  from_y + (shift / 4) - 1))
	    return true;
   return false;
}

/***********************
 *   KnightMove        *
 ***********************/

KnightMove::KnightMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool KnightMove::is_valid(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side)
{
   int info = moveinfo[side];

   if(abs(to_x - from_x) != 1)
      return false;
   
   switch(to_y - from_y)
   {
     case -2:
	return (info & FORWARD) ? true : false;
     case 2:
	return (info & BACKWARD) ? true : false;
     default:
	return false;
   }
}

bool KnightMove::can_reach(const Board *pos,
			   int from_x, int from_y,
			   int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}

void KnightMove::valid_moves(const Board *pos,
			     int from_x, int from_y, int side,
			     vector<Point> &movelist)
{
   int info = moveinfo[side];
   Point i;

   if(info & BACKWARD) {
       i.y = from_y + 2;
       i.x = from_x - 1; movelist.push_back(i);
       i.x = from_x + 1; movelist.push_back(i);
   }
   if(info & FORWARD) {
       i.y = from_y - 2;
       i.x = from_x - 1; movelist.push_back(i);
       i.x = from_x + 1; movelist.push_back(i);
   }
}

bool KnightMove::has_board_moves(const Board *pos,
				 int from_x, int from_y, int side)
{
   int info = moveinfo[side];
   int row;

   if(info & BACKWARD) {
       row = from_y + 2;
       if(pos->on_board(from_x - 1, row) ||
	  pos->on_board(from_x + 1, row))
	  return true;
   }
   if(info & FORWARD) {
       row = from_y - 2;
       if(pos->on_board(from_x - 1, row) ||
	  pos->on_board(from_x + 1, row))
	  return true;
   }
   return false;
}

/***********************
 *   JumpMove       *
 ***********************/

JumpMove::JumpMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool JumpMove::is_valid(const Board *pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side)
{
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if((dx && abs(dx) != 2) ||
      (dy && abs(dy) != 2))
      return false;

   dx /= 2; dy /= 2;
   
   return (has_direction(moveinfo[side], dx, dy));
}

bool JumpMove::can_reach(const Board *pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}

void JumpMove::valid_moves(const Board *pos,
			   int from_x, int from_y, int side,
			   vector<Point> &movelist)
{
   int info = moveinfo[side];
   Point square;

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift)) {
	  square.x = from_x + (shift % 4) * 2 - 2;
	  square.y = from_y + (shift / 4) * 2 - 2;
	  movelist.push_back(square);
      }
}

bool JumpMove::has_board_moves(const Board *pos,
			       int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
	 if(pos->on_board(from_x + (shift % 4) * 2 - 2,
			  from_y + (shift / 4) * 2 - 2));
	    return true;
   return false;
}

/****************************
 *        NumMove           *
 ****************************/

NumMove::NumMove(int moveinfo_p, int num_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
   num = num_p;
}

   
bool NumMove::is_valid(const Board *pos,
		       int from_x, int from_y,
		       int to_x, int to_y, int side)
{
   int info = moveinfo[side];
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(abs(dx) > num ||
      abs(dy) > num)
      return false;

   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   while(true)
   {
       from_x += dx;
       from_y += dy;
       
       if(from_x == to_x && from_y == to_y)
	  return true;
       if(!pos->empty_square(from_x, from_y))
	  return false;
   }
}

bool NumMove::can_reach(const Board *pos,
			int from_x, int from_y,
			int to_x, int to_y, int side)
{
   int info = moveinfo[side];
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(abs(dx) > num ||
      abs(dy) > num)
      return false;

   if(!normalize(dx, dy))
      return false;

   return (has_direction(info, dx, dy));
}

void NumMove::valid_moves(const Board *pos,
			  int from_x, int from_y, int side,
			  vector<Point> &movelist)
{
   int info = moveinfo[side];
   
   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
      {
	  int dx = (shift % 4) - 1;
	  int dy = (shift / 4) - 1;
	  Point i = {from_x, from_y};
	  int n = 0;
	  
	  while(true)
	  {
	      if(n++ >= num)break;

	      i.x += dx;
	      i.y += dy;
	      
	      if(!pos->on_board(i.x, i.y))
		 break;

	      movelist.push_back(i);
	      
	      if(!pos->empty_square(i.x, i.y))
		  break;
	  }
      }
}

bool NumMove::has_board_moves(const Board *pos,
			      int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift))
	 if(pos->on_board(from_x + (shift % 4) - 1,
			  from_y + shift / 4 - 1))
	    return true;
   return false;
}

/**************************
 *       AreaMove         *
 **************************/

// AreaMove::AreaMove(int num_p)
// {
//    num = num_p;
// }

// static bool
// area_two_between(const Board *pos,
// 		 int from_x, int from_y,
// 		 int dx, int dy)
// {
//    int x, y, length, incx, incy;

//    length = abs(abs(dx) - abs(dy)) + 1;
   
//    incx = (abs(dx) != 2);
//    incy = (abs(dy) != 2);

//    x = from_x + max(dx, 0) - 1;
//    y = from_y + max(dy, 0) - 1;
   
//    while (length--){
//        if(pos->on_board(x, y) &&
// 	  pos->empty_square(x, y))
// 	  return true;
//        x += incx; y += incy;
//    }

//    return false;
// }

// bool AreaMove::is_valid(const Board *pos,
// 			int from_x, int from_y,
// 			int to_x, int to_y, int side)
// {
//    int dx = to_x - from_x;
//    int dy = to_y - from_y;

//    if(max(abs(dx), abs(dy)) > num)
//       return false;

//    switch(max(abs(dx), abs(dy)))
//    {
//      case 1:
// 	return true;
//      case 2:
//      {
// 	 int dx2, dy2;

// 	 //can reach in two steps
// 	 if(area_two_between(pos, from_x, from_y, dx, dy))
// 	    return true;
	 
// 	 if(num == 2) return false;
// 	 if(abs(dx) + abs(dy) == 2) return false;

// 	 //can reach in three steps

// 	 //dx1 = (abs(dx) == 2 ? 0 : -dx);
// 	 //dy2 = (abs(dy) == 2 ? dy / 2 : dy);
// 	 dx2 = (dx & 3) - 2;
// 	 dy2 = dy >> ((dy & 1) ^ 1);
	
// 	 if(area_two_between(pos, from_x + dx2, from_y + dy2, dx2, dy2))
// 	    return true;
	 
// 	 dx2 = dx >> ((dx & 1) ^ 1);
// 	 dy2 = (dy & 3) - 2;
	 
// 	 return (area_two_between(pos, from_x + dx2, from_y + dy2, dx2, dy2));
//      }
//      case 3:
//      {
// 	 int x, y, length, incx, incy;

// 	 length = abs(abs(dx) - abs(dy));
// 	 if(length < 3) length++;
   
// 	 incx = (abs(dx) != 3);
// 	 incy = (abs(dy) != 3);

// 	 x = from_x + max(dx - 1, 0) - 1;
// 	 y = from_y + max(dy - 1, 0) - 1;
   
// 	 while (length--){
// 	     if(pos->on_board(x, y) &&
// 		pos->empty_square(x, y) &&
// 		area_two_between(pos, x, y, to_x - x, to_y - y))
// 		return true;
// 	     x += incx; y += incy;
// 	 }

// 	 return false;
//      }
//    }
//    return false;
// }

// bool AreaMove::has_board_moves(const Board *pos,
// 			       int from_x, int from_y, int side)
// {
//    return true;
// }
//

#include "moves_def.inc"

