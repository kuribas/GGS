#include "ShogiImpl.H"
#include "Moves.H"
#include <iostream>
#include <string.h>

#define MAX_BOARD_SIZE (13 * 13)
#define MAX_CAPTURES 20


bool SmallBoard::legal_move(Move &mv)
{
   const Piece *piece;
   int i;

   Square tmpSquares[MAX_BOARD_SIZE];
   int tmpCaptures[2][MAX_CAPTURES];
   SmallBoard tmpBoard(tmpSquares, tmpCaptures[0], tmpCaptures[1]);

   if(mv.type == Move::DROP)
      return legal_drop(mv);
      
   if(mv.type != Move::SIMPLE)
      return false;

   if(side_at(mv.to) == current_side ||
      side_at(mv.from) != current_side)
      return false;
       
   piece = piece_at(mv.from);

   if(!valid_piece_move(piece, mv, current_side))
      return false;

   tmpBoard = *this;
       
   tmpBoard.do_move(mv);
   if(tmpBoard.in_check(current_side))
      return false;
	   
   if(type->has_prop(PROM_BY_CAPTURE))
   {
       if(piece->promote_to &&
	  !empty_square(mv.to))
	  promote_next = MUST_PROMOTE;
       else
	  promote_next = MUST_STAY;
   }
   else if (type->has_prop(NO_PROM))
      promote_next = MUST_STAY;
   else if(piece->promote_to &&
	   in_promotion_zone(mv, current_side))
   {
       if(type->has_prop(MUST_PROM))
	  promote_next = MUST_PROMOTE;
       else
       {
	   promote_next = MUST_PROMOTE;
	   for(i = 0; piece->moves[i]; i++) {
	       if(piece->moves[i]->has_board_moves(tmpBoard, mv.to.x, mv.to.y, current_side))
		  promote_next = CAN_PROMOTE;
	   }
       }
   }
   else promote_next = MUST_STAY;
	    
   return true;
}
   
bool SmallBoard::legal_drop(Move &mv)
{
   const Piece *piece;
   int i;

   Square tmpSquares[MAX_BOARD_SIZE];
   int tmpCaptures[2][MAX_CAPTURES];
   SmallBoard tmpBoard(tmpSquares, tmpCaptures[0], tmpCaptures[1]);

   if (!type->has_prop(HAS_DROPS))
      return false;
   
   if(!empty_square(mv.to))
      return false;
   
   piece = type->pieces[mv.piece];

   //we can drop any (micro-shogi)
   if(type->has_prop(DROP_ANY))
   {
       if(piece->promote_from) //try the unpromoted form
	  piece = piece->promote_from;

       if(get_capture(piece->num, current_side) == 0)
	  return false;
       
       tmpBoard = *this;
       tmpBoard.do_drop(mv);

       return !tmpBoard.in_check(current_side);
   }
         
   if(piece->promote_from || piece->has_prop(Piece::KING))
      return false;

   if(get_capture(mv.piece, current_side) == 0)
      return false;
   
   if(piece->has_prop(Piece::PAWN) &&
      pawns_on_column(mv.to.x, current_side) == ((SmallBoard::Type *)type)->max_pawns_on_column)
      return false;

   tmpBoard = *this;
   tmpBoard.do_drop(mv);

   if(tmpBoard.in_check(current_side))
      return false;

   for(i = 0; piece->moves[i]; i++) {
       if(piece->moves[i]->has_board_moves(tmpBoard, mv.to.x, mv.to.y, current_side))
	  goto cont;
   } return false;
 cont:

   if(piece->has_prop(Piece::PAWN) && !type->has_prop(PAWNDROP_CAN_MATE)) {
       int y = mv.to.y + (current_side == BLACK ? -1 : 1);
       
       if(tmpBoard.side_at(mv.to.x, y) == otherside(current_side) &&
	  tmpBoard.piece_at(mv.to.x, y)->has_prop(Piece::KING) && 
 	  tmpBoard.is_finished())
	  return false;
   }

   return true;
}

int SmallBoard::pawns_on_column(int x, int side)
{
   int y, sum = 0;

   for(y = 0; y < type->height; y++) {
       if(side_at(x, y) == side &&
	  squares[xy_to_index(x, y)].piece == 0)
	  sum++; }
   
   return sum;
}

bool SmallBoard::in_check(int side, Point *checking_piece)
{
   Move mv;
   int other, x, y;
   const Piece *p;
   
   other = otherside(side);
   mv.type = Move::SIMPLE;
   mv.to.x = king[side].x;
   mv.to.y = king[side].y;

   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
      {
	  if(side_at(x, y) != other) continue;
	  p = piece_at(x, y);
	  
	  mv.from.x = x;
	  mv.from.y = y;
	  if(valid_piece_move(p, mv, other)) {
	      if(checking_piece) {
		  checking_piece->x = x;
		  checking_piece->y = y; }
	      return true; }
      }
   return false;
}

bool SmallBoard::is_finished()
{
   Move mv(type);
   const Piece *piece;
   Point checking_piece;
   static vector<Point> movelist;
   int x, y;
   unsigned int i;

   Square tmpSquares[MAX_BOARD_SIZE];
   int tmpCaptures[2][MAX_CAPTURES];
   SmallBoard tmpBoard(tmpSquares, tmpCaptures[0], tmpCaptures[1]);
   
   mv.type = Move::SIMPLE;

   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
      {
	  if(side_at(x, y) != current_side)
	     continue;

	  piece = piece_at(x, y);
	  
	  mv.from.x = x;
	  mv.from.y = y;

	  movelist.clear();
	  for(i = 0; piece->moves[i]; i++)
	     ((MoveWithCheck *)(piece->moves[i]))->
	       valid_moves(*this, x, y, current_side, movelist);
	  
	  for(i = 0; i < movelist.size(); i++) {
	      if(!on_board(movelist[i].x, movelist[i].y) ||
		 side_at(movelist[i]) == current_side)
		 continue;
	      
	      tmpBoard = *this;
	      mv.to.x = movelist[i].x;
	      mv.to.y = movelist[i].y;
	      tmpBoard.do_move(mv);
	      if(!tmpBoard.in_check(current_side))
		 return false;
	  }
      }

   if(in_check(current_side, &checking_piece)) {
       if(type->has_prop(HAS_DROPS) &&
	  drop_between_check(checking_piece))
	      //can drop a piece between the check giving piece
	      return false;
       else {
	   game_result = (current_side == BLACK ? 1 : -1);
	   return true;
       }
   }else { //cannot drop a piece
       for(int i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  if(get_capture(i, current_side)) return false;

       if(type->has_prop(HAS_STALEMATE))
	  game_result = 0;
       else
	  game_result = (current_side == BLACK ? 1 : -1);
       return true;
   }
}

static inline
bool normalize(int &dx, int &dy)
{
   if(dx) {
       if(dy && abs(dx) != abs(dy)) return false;
       dx = (dx > 0 ? 1 : -1);
   }

   if(dy)
      dy = (dy > 0 ? 1 : -1);

   return true;
}


bool SmallBoard::drop_between_check(Point checking_piece)
{
   int kx, ky, i;
   int dx, dy;
   Move mv(type);

   kx = king[current_side].x;
   ky = king[current_side].y;

   //look for a non-pawn piece to drop
   for(i = 1; i < ((SmallBoard::Type *)type)->captures_size; i++)
      if(get_capture(i, current_side)) break;

   if(i == ((SmallBoard::Type *)type)->captures_size) {
       if(get_capture(0, current_side)) i = 0;  //we have only a pawn to drop
       else return false;
   }

   mv.piece = i;
   mv.type = Move::DROP;

   dx = kx - checking_piece.x;
   dy = ky - checking_piece.y;

   if(!normalize(dx, dy))
      return false;
   
   mv.to.x = checking_piece.x + dx;
   mv.to.y = checking_piece.y + dy;
   while(! (mv.to.x == kx && mv.to.y == ky)) {
       if(legal_drop(mv))
	  return true;
       mv.to.x += dx;
       mv.to.y += dy;
   }
   return false;
}

void SmallBoard::do_move(Move &mv)
{
   const Piece *piece, *old;
   int i;
   
   if(mv.type == Move::DROP) { do_drop(mv); return; }

   piece = piece_at(mv.from);

   if(type->has_prop(NO_PROM)) {
       /* extra move for whale shogi
	  This is not really promotion, just a trick to
	  give the dolfin an extra move when reaching the final row */
       if(piece->has_prop(Piece::PAWN)) {
	   if(mv.to.y == (current_side == BLACK ? 0 : type->height - 1))
	      piece = piece->promote_to; //promote
	   else if(mv.from.y == (current_side == BLACK ? 0 : type->height - 1))
	      piece = piece->promote_from; //demote
       }
   } else if(type->has_prop(PROM_BY_CAPTURE)) {
       // micro shogi: promote when capturing a piece
       if(!empty_square(mv.to)) {
	   if(piece->promote_to)
	      piece = piece->promote_to;
	   else if(piece->promote_from)
	      piece = piece->promote_from;
       }
   }else if(mv.promote && piece->promote_to)
      piece = piece->promote_to;

   squares[xy_to_index(mv.from.x, mv.from.y)].side = NO_SIDE;

   if(type->has_prop(HAS_DROPS) &&
      !empty_square(mv.to)) {
       old = piece_at(mv.to);

       if(old->promote_from)
	  old = old->promote_from;

       if(old->num < ((SmallBoard::Type *)type)->captures_size)
	  return;
       captures[current_side][old->num]++;
   }

   i = xy_to_index(mv.to.x, mv.to.y);
   squares[i].side = current_side;
   squares[i].piece = piece->num;

   if(piece->has_prop(Piece::KING)) {
       king[current_side].x = mv.to.x;
       king[current_side].y = mv.to.y;
   }
   
   current_side = otherside(current_side);
}

void SmallBoard::do_drop(Move &mv)
{
   int i = xy_to_index(mv.to.x, mv.to.y);
   int capnum;
   const Piece *piece;

   piece = type->pieces[mv.piece];
   capnum = (piece->promote_from ?
	     piece->promote_from->num :
	     mv.piece);
   
   if(capnum < ((SmallBoard::Type *)type)->captures_size)
      return;
       
   captures[current_side][capnum]--;
   squares[i].side = current_side;
   squares[i].piece = mv.piece;
   
   current_side = otherside(current_side);
}

Board *SmallBoard::duplicate()
{
   return (Board *) new SmallBoard(*this);
}

SmallBoard::SmallBoard(const SmallBoard &board)
{
   type = board.type;
   squares = new Square[type->width * type->height];

   memcpy(squares, board.squares, type->height * type->width * sizeof(Square));
   current_side = board.current_side;
   game_result = board.game_result;
   promote_next = board.promote_next;
   
   external_storage = false;
   captures[0] = new int[((SmallBoard::Type *)type)->captures_size];
   captures[1] = new int[((SmallBoard::Type *)type)->captures_size];

   memcpy(captures[0], board.captures[0], ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   memcpy(captures[1], board.captures[1], ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   
   king[0] = board.king[0];
   king[1] = board.king[1];
}


SmallBoard::SmallBoard(Square *board, int *captures1, int *captures2)
{
   external_storage = true;
   squares = board;
   captures[0] = captures1;
   captures[1] = captures2;
}

SmallBoard::SmallBoard(const SmallBoard::Type *type_a)
{
   type = (const Board::Type *)type_a;

   squares = new Square[type->width * type->height];
   captures[0] = new int[((SmallBoard::Type *)type)->captures_size];
   captures[1] = new int[((SmallBoard::Type *)type)->captures_size];
   current_side = BLACK;
   external_storage = false;
}

SmallBoard::~SmallBoard()
{
   if(external_storage)
      return;
   
   if(squares) delete squares;

   if(captures[0]) delete captures[0];
   if(captures[1]) delete captures[1];
}

void SmallBoard::operator=(const SmallBoard &bd)
{
   type = bd.type;
   current_side = bd.current_side;
   king[0] = bd.king[0];
   king[1] = bd.king[1];
   
   memcpy(squares, bd.squares, sizeof(Square) * type->width * type->height);
   memcpy(captures[0], bd.captures[0], ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   memcpy(captures[1], bd.captures[1], ((SmallBoard::Type *)type)->captures_size * sizeof(int));
}

void SmallBoard::find_kings() {
   int x, y, side;

   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
      {
	  if(empty_square(x, y) ||
	     !piece_at(x, y)->has_prop(Piece::KING))
	     continue;

	  side = side_at(x, y);
	  king[side].x = x;
	  king[side].y = y;
      }
}
  
void SmallBoard::init_setup()
{
   memcpy(squares, type->setup, sizeof(Square) * type->width * type->height);
   memset(captures[0], 0, ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   memset(captures[1], 0, ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   current_side = BLACK;

   find_kings();
}

void SmallBoard::set_pos(Square *pos, int *cap_black, int *cap_white, int to_move)
{
   memcpy(squares, type->setup, sizeof(Square) * type->width * type->height);
   memcpy(captures[0], cap_black, ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   memcpy(captures[1], cap_white, ((SmallBoard::Type *)type)->captures_size * sizeof(int));
   current_side = to_move;

   update_pos();
}

void SmallBoard::write_ggf(ostream &os, bool one_line)
{
   int i;

   if(type->has_prop(HAS_DROPS))
   {
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  os << captures[0][i] << " ";
       if(!one_line) os << EOL;
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  os << captures[1][i] << " ";
       
       if(!one_line) os << EOL << EOL;
   }
   
   write_board_ggf(os, one_line);
}

void SmallBoard::update_pos()
{
   int x;
   
   if(type->has_prop(NO_PROM)) {
       // Whale shogi: if the pawn is on the last rank,
       // it gets the promoted form.
       for(x = 0; x < type->width; x++) {
	   if(squares[x].side != BLACK ||
	      squares[x].piece != 0)
	      continue;

	   squares[x].piece = piece_at(x, 0)->promote_to->num;
       }

       for(x = 0; x < type->width; x++) {
	   int i = xy_to_index(x, type->height - 1);
	   if(squares[i].side != WHITE ||
	      squares[i].piece != 0)
	      continue;

	   squares[i].piece = piece_at(x, 0)->promote_to->num;
       }
   }

   find_kings();
}

bool SmallBoard::read_pos(istream &is)
{
   int i;

   if(type->has_prop(HAS_DROPS))
   {
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  is >> captures[0][i];
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  is >> captures[1][i];
   }
   if(!read_board_pos(is))
      return false;

   update_pos();
   return true;
}

void SmallBoard::write(ostream &os)
{
   int i;

   if(type->has_prop(HAS_DROPS))
   {
       os << "* in hand: ";
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  if(captures[0][i])
	  {
	      os << type->pieces[i]->name;

	      if(type->has_prop(DROP_ANY) &&
		 type->pieces[i]->promote_to)
		 os << '/' << type->pieces[i]->promote_to->name;

	      os << ": " << captures[0][i] << " ";
	  }
              
       os << EOL << "O in hand: ";
       for(i = 0; i < ((SmallBoard::Type *)type)->captures_size; i++)
	  if(captures[1][i])
	  {
	      os << type->pieces[i]->name;

	      if(type->has_prop(DROP_ANY) &&
		 type->pieces[i]->promote_to)
		 os << '/' << type->pieces[i]->promote_to->name;

	      os << ": " << captures[1][i] << " ";
	  }
       os << EOL << EOL;
   }
   write_board(os);
}
