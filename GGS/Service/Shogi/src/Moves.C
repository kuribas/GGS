#include <stdlib.h>
#include "ShogiImpl.H"
#include <vector>


/**************************
 *      ShortMove        *
 **************************/

ShortMove::ShortMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool ShortMove::is_valid(Board &pos,
			 Board::Move &mv,
			 int side)
{
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(abs(dx) > 1 || abs(dy) > 1)
      return false;

   return (has_direction(moveinfo[side], dx, dy));
}

/*
bool ShortMove::can_reach(const Board &pos,
			  int from_x, int from_y,
			  int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}
*/

void ShortMove::valid_moves(const Board &pos,
			    int from_x, int from_y, int side,
			    vector<Point> &movelist)
{
   int info = moveinfo[side];
   Point i;

   for(int shift = 0; shift < 16; shift++)
      if(info & (1 << shift)) {
	  i.x = from_x + (shift % 4) - 1;
	  i.y = from_y + (shift / 4) - 1;
	  movelist.push_back(i);
      }
}

bool ShortMove::has_board_moves(const Board &pos,
				int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) - 1,
		       from_y + (shift / 4) - 1)))
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

   
bool LongMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   int info = moveinfo[side];
   int x, y;
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   x = mv.from.x;
   y = mv.from.y;

   while(true)
   {
       x += dx;
       y += dy;
       
       if(x == mv.to.x && y == mv.to.y)
	  return true;
       if(!pos.empty_square(x, y))
	  return false;
   }
}

/*
bool LongMove::can_reach(const Board &pos,
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
*/

void LongMove::valid_moves(const Board &pos,
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
	      
	      if(!pos.on_board(i.x, i.y))
		 break;

	      movelist.push_back(i);
	      
	      if(!pos.empty_square(i.x, i.y))
		  break;
	  }
      }
}

bool LongMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) - 1,
		       from_y + (shift / 4) - 1)))
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

bool KnightMove::is_valid(Board &pos,
			  Board::Move &mv,
			  int side)
{
   int info = moveinfo[side];

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(abs(mv.to.x - mv.from.x) != 1)
      return false;
   
   switch(mv.to.y - mv.from.y)
   {
     case -2:
	return (info & FORWARD) ? true : false;
     case 2:
	return (info & BACKWARD) ? true : false;
     default:
	return false;
   }
}

/*
bool KnightMove::can_reach(const Board &pos,
			   int from_x, int from_y,
			   int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}
*/

void KnightMove::valid_moves(const Board &pos,
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

bool KnightMove::has_board_moves(const Board &pos,
				 int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   if((info & BACKWARD) &&
      pos.on_board(from_x, from_y + 2))
      return true;

   if((info & FORWARD) &&
      pos.on_board(from_x, from_y - 2))
      return true;
   
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

bool JumpMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if((dx && abs(dx) != 2) ||
      (dy && abs(dy) != 2))
      return false;

   dx /= 2; dy /= 2;
   
   return (has_direction(moveinfo[side], dx, dy));
}

/*
bool JumpMove::can_reach(const Board &pos,
			 int from_x, int from_y,
			 int to_x, int to_y, int side)
{
   return is_valid(pos, from_x, from_y, to_x, to_y, side);
}
*/

void JumpMove::valid_moves(const Board &pos,
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

bool JumpMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) * 2 - 2,
		       from_y + (shift / 4) * 2 - 2)))
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

   
bool NumMove::is_valid(Board &pos,
		       Board::Move &mv,
		       int side)
{
   int info = moveinfo[side];
   int x, y;
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(abs(dx) > num ||
      abs(dy) > num)
      return false;

   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   x = mv.from.x;
   y = mv.from.y;

   while(true)
   {
       x += dx;
       y += dy;
       
       if(x == mv.to.x && y == mv.to.y)
	  return true;
       if(!pos.empty_square(x, y))
	  return false;
   }
}

/*
bool NumMove::can_reach(const Board &pos,
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
*/

void NumMove::valid_moves(const Board &pos,
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
	      
	      if(!pos.on_board(i.x, i.y))
		 break;

	      movelist.push_back(i);
	      
	      if(!pos.empty_square(i.x, i.y))
		  break;
	  }
      }
}

bool NumMove::has_board_moves(const Board &pos,
			      int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) - 1,
		       from_y + (shift / 4) - 1)))
	 return true;
   return false;
}

/**************************
 *       AreaMove         *
 **************************/

AreaMove::AreaMove(int num_p)
{
   num = num_p;
}

bool AreaMove::area_two_between(const Board &pos,
				int from_x, int from_y,
				int dx, int dy)
{
   int x, y, length, incx, incy;

   length = abs(abs(dx) - abs(dy)) + 1;
   
   incx = (abs(dx) != 2);
   incy = (abs(dy) != 2);

   x = from_x + max(dx, 0) - 1;
   y = from_y + max(dy, 0) - 1;
   
   while (length--){
       if(pos.on_board(x, y) &&
	  pos.empty_square(x, y))
	  return true;
       x += incx; y += incy;
   }

   return false;
}

bool AreaMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   int from_x = mv.from.x;
   int from_y = mv.from.y;
   int to_x = mv.to.x;
   int to_y = mv.to.y;
   
   int dx = to_x - from_x;
   int dy = to_y - from_y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
      
   if(max(abs(dx), abs(dy)) > num)
      return false;

   switch(max(abs(dx), abs(dy)))
   {
     case 1:
	return true;
     case 2:
     {
	 int x, y;

	 //can reach in two steps
	 if(area_two_between(pos, from_x, from_y, dx, dy))
	    return true;
	 
	 if(num == 2) return false;
	 if(dx == 0 || dy == 0) return false;

	 //can reach in three steps

	 //x = from_x + (abs(dx) == 2 ? 0 : -dx);
	 //y = from_y + (abs(dy) == 2 ? dy / 2 : dy);
	 x = from_x + (dx & 3) - 2;
	 y = from_y + (dy >> ((dy & 1) ^ 1));
	 
	 if(pos.on_board(x, y) &&
	    pos.empty_square(x, y) &&
	    area_two_between(pos, x, y, to_x - x, to_y - y))
	    return true;
	 
	 x = from_x + (dx >> ((dx & 1) ^ 1));
	 y = from_y + (dy & 3) - 2;
	 
	 return (pos.on_board(x, y) &&
		 pos.empty_square(x, y) &&
		 area_two_between(pos, x, y, to_x - x, to_y - y));
     }
     case 3:
     {
	 int x, y, length, incx, incy;

	 length = abs(abs(dx) - abs(dy));
	 if(length < 3) length++;
   
	 incx = (abs(dx) != 3);
	 incy = (abs(dy) != 3);

	 x = from_x + max(dx - 1, 0) - 1;
	 y = from_y + max(dy - 1, 0) - 1;
   
	 while (length--){
	     if(pos.on_board(x, y) &&
		pos.empty_square(x, y) &&
		area_two_between(pos, x, y, to_x - x, to_y - y))
		return true;
	     x += incx; y += incy;
	 }

	 return false;
     }
   }
   return false;
}

bool AreaMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   return true;
}

/*
  scan the diagonal from (from_x, from_y) upto
  but not including (to_x, to_y) for pieces.
  return true if none found.
*/
static inline bool scan_diag(const Board &pos,
			     int from_x, int from_y,
			     int to_x, int to_y, bool first)
{
   int dx = (to_x - from_x < 0 ? -1 : 1);
   int dy = (to_y - from_y < 0 ? -1 : 1);

   if(from_x == to_x) return true;

   if(!first) {
       from_x += dx;
       from_y += dy;
   }

   while(from_x != to_x) {
       if(!pos.empty_square(from_x, from_y))
	  return false;
       from_x += dx;
       from_y += dy;
   }
   return true;
}

DiaghookMove::DiaghookMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool DiaghookMove::is_valid(Board &pos,
			    Board::Move &mv,
			    int side)
{
   int from_x = mv.from.x;
   int from_y = mv.from.y;
   int to_x = mv.to.x;
   int to_y = mv.to.y;

   int info = moveinfo[side];
   int dx, dy;
   
   int midx, midy, midx2, midy2;
   
   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if((from_x + from_y) % 2 !=
      (to_x + to_y) % 2)
      return false;

   midx = (from_x + from_y + to_x - to_y) / 2;
   midy = (from_x + from_y - to_x + to_y) / 2;
   midx2 = (from_x - from_y + to_x + to_y) / 2;
   midy2 = (-from_x + from_y + to_x + to_y) / 2;

   dx = midx - from_x;
   dy = midy - from_y;
   if(dx) dx = (dx > 0 ? 1 : -1);
   if(dy) dy = (dy > 0 ? 1 : -1);
	
   if(pos.on_board(midx, midy) &&
      has_direction(info, dx, dy))
   {
      if(scan_diag(pos, from_x, from_y, midx, midy, false) &&
	 scan_diag(pos, midx, midy, to_x, to_y, true))
	 return true;
   }

   dx = midx2 - from_x;
   dy = midy2 - from_y;
   dx = (dx > 0 ? 1 : dx < 0 ? -1 : 0);
   dy = (dy > 0 ? 1 : dy < 0 ? -1 : 0);
   
   if(pos.on_board(midx2, midy2) &&
      has_direction(info, dx, dy))
   {
       if(scan_diag(pos, from_x, from_y, midx2, midy2, false) &&
	  scan_diag(pos, midx2, midy2, to_x, to_y, true))
	  return true;
   }
   return false;
}

bool DiaghookMove::has_board_moves(const Board &pos,
				   int from_x, int from_y, int side)
{
   return true;
}

static inline bool scan_horizontally(const Board &pos,
				     int from_x, int to_x,
				     int y, bool first)
{
   int dx = (to_x - from_x < 0 ? -1 : 1);

   if(to_x == from_x)
      return true;

   if(!first)
      from_x += dx;

   while(from_x != to_x) {
       if(!pos.empty_square(from_x, y))
	  return false;
       from_x += dx;
   }
   return true;
}

static inline bool scan_vertically(const Board &pos,
				   int x,
				   int from_y, int to_y,
				   bool first)
{
   int dy = (to_y - from_y < 0 ? -1 : 1);

   if(to_y == from_y)
      return true;

   if(!first)
      from_y += dy;

   while(from_y != to_y) {
       if(!pos.empty_square(x, from_y))
	  return false;
       from_y += dy;
   }
   return true;
}

bool HookMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(scan_horizontally(pos, mv.from.x, mv.to.x, mv.from.y, false) &&
      scan_vertically(pos, mv.to.x, mv.from.y, mv.to.y, true))
      return true;

   if(scan_vertically(pos, mv.from.x, mv.from.y, mv.to.y, false) &&
      scan_horizontally(pos, mv.from.x, mv.to.x, mv.to.y, true))
      return true;

   return false;
}

bool HookMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   return true;
}

MoveoverMove::MoveoverMove(int moveinfo_p, int num_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
   num = num_p;
}

bool MoveoverMove::is_valid(Board &pos,
			    Board::Move &mv,
			    int side)
{
   int info = moveinfo[side];
   int x, y;
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(abs(dx) > num ||
      abs(dy) > num)
      return false;

   if(max(abs(dx), abs(dy)) == 1)
      return false;

   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   x = mv.from.x + dx;
   y = mv.from.y + dy;

   while(true)
   {
       x += dx;
       y += dy;
       
       if(x == mv.to.x && y == mv.to.y)
	  return true;
       if(!pos.empty_square(x, y))
	  return false;
   }
}

bool MoveoverMove::has_board_moves(const Board &pos,
				   int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) * 2 - 2,
		       from_y + (shift / 4) * 2 - 2)))
	 return true;
   return false;
}

static inline bool valid_igui_move(const Board &pos,
				   Board::Move &mv)
{
   if(mv.type != Board::Move::IGUI)
      return false;

   if(sqr_distance(mv.to, mv.from) != 1)
      return false;

   return !pos.empty_square(mv.to.x, mv.to.y);
}

bool IguiMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   return valid_igui_move(pos, mv);
}

bool IguiMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   return false;
}

LionMove::LionMove(int max_moves_p)
{
   max_moves = max_moves_p;
}

bool LionMove::is_valid(Board &pos,
			Board::Move &mv,
			int side)
{
   int x, y;
   
   switch(mv.type)
   {
     case Board::Move::IGUI:
	return valid_igui_move(pos, mv);
	
     case Board::Move::PASS:
	for(y = -1; y <= 1; y++)
	   for(x = -1; x <= 1; x++)
	      if((x || y) &&
		 pos.on_board(mv.from.x + x, mv.from.y + y) &&
		 pos.empty_square(mv.from.x + x, mv.from.y + y))
		 return true;
	return false;
     
     case Board::Move::SIMPLE:
     {
	 int dist = sqr_distance(mv.to, mv.from);
	 if(dist == 1)
	    pos.partial_move = true;
	 return (dist <= max_moves);
     }

     case Board::Move::TRIPLE:
	if(max_moves < 3)
	   return false;
	
	if (sqr_distance(mv.to2, mv.to3) != 1)
	   return false;

	//else fall through
	
     case Board::Move::DOUBLE:
	if (sqr_distance(mv.from, mv.to) != 1 ||
	    sqr_distance(mv.to, mv.to2) != 1)
	   return false;

	if(max_moves == 3)
	   pos.partial_move = true;

	return true;
     default:
	return false;
   }
}

bool LionMove::has_board_moves(const Board &pos,
			       int from_x, int from_y, int side)
{
   return true;
}

bool SmalllionMove::is_valid(Board &pos,
			      Board::Move &mv,
			      int side)
{
   int info = moveinfo[side];
   int dx, dy;
      
   if(mv.type == Board::Move::PASS)
   {
       int shift, x, y;
       for(shift = 0; shift < 16; shift++)
	  if(info & (1 << shift)) {
	      x = mv.from.x + (shift % 4) - 1;
	      y = mv.from.y + (shift / 4) - 1;
	      if(pos.on_board(x, y) &&
		 pos.empty_square(x, y))
		 return true;
	  }
       return false;
   }

   dx = mv.to.x - mv.from.x;
   dy = mv.to.y - mv.from.y;
   
   if(abs(dx) > 2 || abs(dy) > 2)
      return false;

   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   switch(mv.type)
   {
     case Board::Move::TRIPLE:
	return false;

     case Board::Move::SIMPLE:
	return true;

     case Board::Move::DOUBLE:
	return (mv.from.x + dx == mv.to.x &&
		mv.from.y + dy == mv.to.y &&
		mv.to.x + dx == mv.to2.x &&
		mv.to.y + dy == mv.to2.y);

     case Board::Move::IGUI:
	return (mv.from.x + dx == mv.to.x &&
		mv.from.y + dy == mv.to.y &&
		!pos.empty_square(mv.to.x, mv.to.y));
   }
   return false; //shouldn't be reached
}

SmalllionMove::SmalllionMove(int moveinfo_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
}

bool SmalllionMove::has_board_moves(const Board &pos,
				    int from_x, int from_y, int side)
{
   int info = moveinfo[side];

   for(int shift = 0; shift < 16; shift++)
      if((info & (1 << shift)) &&
	 (pos.on_board(from_x + (shift % 4) - 1,
		       from_y + (shift / 4) - 1)))
	 return true;
   return false;
}

bool EmperorMove::is_valid(Board &pos,
			   Board::Move &mv,
			   int side)
{
   return (mv.type == Board::Move::SIMPLE);
}

bool EmperorMove::has_board_moves(const Board &pos,
				  int from_x, int from_y, int side)
{
   return true;
}

GeneralMove::GeneralMove(int moveinfo_p, int rank_p)
{
   moveinfo[BLACK] = moveinfo_p;
   moveinfo[WHITE] = reverse_direction(moveinfo_p);
   rank = rank_p;
}

bool GeneralMove::is_valid(Board &pos,
			   Board::Move &mv,
			   int side)
{
   int info = moveinfo[side];
   int x, y;
   int dx = mv.to.x - mv.from.x;
   int dy = mv.to.y - mv.from.y;

   if(mv.type != Board::Move::SIMPLE)
      return false;
   
   if(!normalize(dx, dy))
      return false;

   if(!has_direction(info, dx, dy))
      return false;

   if(pos.empty_square(mv.to))
      return false;

   x = mv.from.x;
   y = mv.from.y;
   while(true)
   {
       x += dx;
       y += dy;

       if(!pos.empty_square(x, y) &&
	  pos.piece_at(x, y)->rank >= rank)
	  return false;

       if(x == mv.to.x && y == mv.to.y)
	  return true;
   }
}

bool GeneralMove::has_board_moves(const Board &pos,
				  int from_x, int from_y, int side)
{
   return true;
}

#include "moves_def.inc"
