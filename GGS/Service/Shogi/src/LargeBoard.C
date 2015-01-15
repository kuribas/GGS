#include "ShogiImpl.H"
#include <iostream>
#include <string.h>

bool LargeBoard::valid_sides(Move &mv)
{
   switch(mv.type)
   {
     case Move::TRIPLE:
	if(side_at(mv.to3) == current_side &&
	   mv.to3 != mv.from)
	   return false;
     case Move::DOUBLE:
	if(side_at(mv.to2) == current_side &&
	   mv.to2 != mv.from)
	   return false;
     case Move::IGUI:
     case Move::SIMPLE:
	if(side_at(mv.to) == current_side)
	   return false;
     case Move::PASS:
	return (side_at(mv.from) == current_side);
    default:
       return false; //shouldn't be reached
   }
}

inline bool LargeBoard::captures_lion(Move &mv)
{
   if(mv.type == Move::PASS)
      return false;
   
   if(mv.type == Move::DOUBLE &&
      mv.to2 != mv.from       &&
      !empty_square(mv.to2)   &&
      piece_at(mv.to2)->has_prop(Piece::LION))
      return true;
     
   if (!empty_square(mv.to) &&
       piece_at(mv.to)->has_prop(Piece::LION))
      return true;

   return false;
}

bool LargeBoard::next_to_firedemon(Point sqr, int side)
{
   int x, y;

   for(y = -1; y <= 1; y++)
      for(x = -1; x <= 1; x++)
	 if(x || y)
	 {
	     if(on_board(sqr.x + x, sqr.y + y) &&
		side_at(sqr.x + x, sqr.y + y) == side &&
		piece_at(sqr.x + x, sqr.y + y)->has_prop(Piece::FIREDEMON))
		return true;
	 }
   return false;
}


inline bool LargeBoard::burned_lion(Move &mv, int side)
{
   if(!type->has_prop(HAS_FIREDEMON))
      return false;

   if(mv.type == Move::SIMPLE)
      return next_to_firedemon(mv.to, otherside(side));

   return false;
}

bool LargeBoard::legal_move(Move &mv)
{
   const Piece *p;
   bool saved_partial;

   if(mv.type == Move::DROP)
      return false;

   if(!valid_sides(mv))
      return false;
 
   partial_move = false;
   
   p = piece_at(mv.from);
   if(!valid_piece_move(p, mv, current_side))
      return false;

   // save partial_move flag, because it can be changed later
   saved_partial = partial_move;

   //if a lion ends it move next to a firedemon, it cannot move
   // further
   if(burned_lion(mv, current_side))
      saved_partial = false;
      
   //test restrictions
   if(type->has_prop(RESTRICTED_LION))
   {
       if(p->has_prop(Piece::LION))
       {
	   if(!valid_lion_move(mv))
	      return false;
       }
       else if(captures_lion(mv) &&
	       !can_capture_lion)
	  return false;
   }

   if(p->has_prop(Piece::EMPEROR) &&
      !valid_emperor_move(mv))
      return false;

   //check promotion
   promote_next = get_promotion(mv, p);
   
   partial_move = saved_partial;
   return true;
}

int LargeBoard::get_promotion(Move &mv, const Piece *p)
{
   int i;
   
   if(type->has_prop(PROM_BY_CAPTURE))
   {
       if (p->promote_to &&
	   is_capture(mv))
	  return MUST_PROMOTE;
       else
	  return MUST_STAY;
   }
   
   if(!p->promote_to ||
      !in_promotion_zone(mv, current_side))
   {
       return MUST_STAY;
   }

   for(i = 0; p->moves[i]; i++)
      if(p->moves[i]->has_board_moves(*this, mv.to.x, mv.to.y, current_side))
	 goto cont;
   
   return MUST_PROMOTE;  // no valid moves, must promote
 cont:
   
   if(type->has_prop(PAWN_PROM_RESTRICT) && p->has_prop(Piece::PAWN))
   {
       //special promotion restriction for Chu shogi:
       //pawn can only promote when entering promotion zone
       if (row_in_prom_zone(mv.from.y, current_side))
	  return MUST_STAY;
       return CAN_PROMOTE;
   }

   if(type->has_prop(PROMOTION_RESTRICT))
   {
       if(!is_capture(mv) &&
	  promotion_restricted(mv.from, current_side))
	  return MUST_STAY;
   }

   return CAN_PROMOTE;
}

bool LargeBoard::valid_lion_move(Move &mv)
{
   switch(mv.type)
   {
     case Move::IGUI:
     case Move::PASS:
     case Move::TRIPLE:
	return true;
     case Move::SIMPLE:
	//capturing a defended lion on the second square is not allowed
	return ! ((sqr_distance(mv.from, mv.to) == 2) &&
		  !empty_square(mv.to) &&
		  piece_at(mv.to)->has_prop(Piece::LION) &&
		  is_defended_from(mv.to, mv.from)); 

     case Move::DOUBLE:
	if(sqr_distance(mv.from, mv.to2) < 2 ||
	   empty_square(mv.to2) ||
	   !piece_at(mv.to2)->has_prop(Piece::LION))
	   return true;

	if(!empty_square(mv.to) &&
	   !piece_at(mv.to)->has_prop(Piece::PAWNLIKE))
	   return true;

	return !is_defended_from(mv.to2, mv.from);
     default:
	return false;
   }
}

bool LargeBoard::valid_emperor_move(Move &mv)
{
   return (empty_square(mv.to) ||
	   !is_defended_from(mv.to, mv.from));
}

bool LargeBoard::is_defended_from(Point target, Point piece)
{
   int x, y, side, other;
   bool defended;
   const Piece *p;
   Move mv;
   
   //temporarily remove piece from play
   side = side_at(piece);
   other = otherside(side);

   if(has_emperor[other] &&
      !piece_at(target)->has_prop(Piece::EMPEROR))
      return true;
   
   squares[xy_to_index(piece.x, piece.y)].side = NO_SIDE;

   mv.type = Move::SIMPLE;
   mv.to = target;
   
   defended = false;

   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
      {
	  if(side_at(x, y) != other) continue;
	  if(x == target.x && y == target.y) continue;
	  p = piece_at(x, y);
	  
	  mv.from.x = x;
	  mv.from.y = y;
	  if(valid_piece_move(p, mv, other)) {
	      defended = true; goto cont; }
      }

 cont:
   //place piece again
   squares[xy_to_index(piece.x, piece.y)].side = side;

   return defended;
}

bool LargeBoard::is_capture(Move &mv)
{
   switch(mv.type)
   {
     case Move::TRIPLE:
	if(!empty_square(mv.to3) &&
	   mv.to3 != mv.from)
	   return true;
     case Move::DOUBLE:
	if(!empty_square(mv.to2) &&
	   mv.to2 != mv.from)
	   return true;
     case Move::SIMPLE:
     case Move::IGUI:
	if(!empty_square(mv.to))
	   return true;
     default:
	return false;
   }
}

void LargeBoard::do_move(Move &mv)
{
   bool burn, capture, lion;
   int x, y;
   Point dest;
   
   const Piece *p = piece_at(mv.from);

   n_burns = 0;

   burn = p->has_prop(Piece::FIREDEMON);
   capture = is_capture(mv);
   lion = p->has_prop(Piece::LION);

   can_capture_lion = true;

   if(mv.promote == true ||
      (type->has_prop(PROM_BY_CAPTURE) && capture))
   {
      if(p->promote_to)
      {
	  if(p->has_prop(Piece::KING))
	     nKings[current_side]--;

	  p = p->promote_to;

	  if(p->has_prop(Piece::KING))
	     nKings[current_side]++;

	  if(p->has_prop(Piece::EMPEROR))
	     has_emperor[current_side]++;
      }
   }

   switch(mv.type)
   {
     case Move::TRIPLE:
	clear_square(mv.from);
	remove_at(mv.to);
	remove_at(mv.to2);
	remove_at(mv.to3);
	dest = mv.to3;
	set_at(mv.to3, p->num, current_side);
	break;

     case Move::DOUBLE:
	clear_square(mv.from);
	remove_at(mv.to);
	remove_at(mv.to2);
	dest = mv.to2;
	set_at(mv.to2, p->num, current_side);
	break;
	
     case Move::SIMPLE:
	clear_square(mv.from);
	remove_at(mv.to);
	dest = mv.to;
	set_at(mv.to, p->num, current_side);
	break;

     case Move::IGUI:
	remove_at(mv.to);
	dest = mv.from;
	break;

     case Move::PASS:
	break;
   }

   if(type->has_prop(PROMOTION_RESTRICT))
   {
       if((in_promotion_zone(mv, current_side)) &&
	  p->promote_to &&
	  (mv.piece != 0 || !type->has_prop(PAWN_PROM_RESTRICT)))
       {
	   if(mv.promote == false &&
	      !row_in_prom_zone(mv.from.y, current_side) &&
	      row_in_prom_zone(dest.y, current_side))
	   {
	       restrict_promotion(dest);
	   }
	   else
	   {
	       remove_restriction(mv.from.x, mv.from.y, current_side);
	   }
       }
   }
   
   if(type->has_prop(HAS_FIREDEMON))
   {
       if(burn)
       {
	   for(x = -1; x <= 1; x++)
	      for(y = -1; y <= 1; y++)
		 if((x || y) &&
		    on_board(mv.to.x + x, mv.to.y + y) &&
		    side_at(mv.to.x + x, mv.to.y + y) == otherside(current_side))
		 {
		     if(piece_at(mv.to.x + x, mv.to.y + y)->has_prop(Piece::FIREDEMON))
		     {
			 burned_pieces[n_burns++] = mv.to;
			 remove_at(mv.to); //demon has burned itself
		     }
		     else
		     {
			 burned_pieces[n_burns++] = mv.to;
			 remove_at(mv.to.x + x, mv.to.y + y);
		     }
		 }
       }
       else if(next_to_firedemon(dest, otherside(current_side)))
       {
	   burned_pieces[n_burns++] = mv.to;
	   remove_at(dest);
       }
   }

   if(lion)
      can_capture_lion = true;

   current_side = otherside(current_side);
}

void LargeBoard::remove_at(int x, int y)
{
   int side;
   const Piece *p;

   side = side_at(x, y);
   
   if(side == NO_SIDE) return;

   p = piece_at(x, y);
   
   if(p->has_prop(Piece::KING))
      nKings[side]--;

   if(p->has_prop(Piece::EMPEROR))
      has_emperor[side]--;

   if(type->has_prop(RESTRICTED_LION) &&
      p->has_prop(Piece::LION))
      can_capture_lion = false;

   if(type->has_prop(PROMOTION_RESTRICT))
      remove_restriction(x, y, side);

   piececount[side]--;

   clear_square(x, y);
}

bool LargeBoard::is_finished()
{
   int other = otherside(current_side);
   
   if(nKings[BLACK] == 0) {
       game_result = -1;
       return true; }

   if(nKings[WHITE] == 0) {
       game_result = 1;
       return true; }

   /* If a player can capture all pieces except the opposing
      king, it is a win.  I have added a exception:
      when the opposing king can capture on the next move the
      game is not finished.
   */
   
   if(piececount[current_side] == 1)
   {
       if(king_can_capture(current_side))
	  return false; // game not finished yet

       if(piececount[other] > 2) {
	   game_result = (current_side == BLACK ? -1 : 1); //loss
	   return true;
       }

       if(piececount[other] == 1)
	  game_result = 0; //king <=> king: draw
       else
	  //king <=> king + other: loss
	  game_result = (current_side == BLACK ? -1 : 1); 

       return true;
   }

   if(piececount[other] == 1) {
       game_result = (current_side == BLACK ? 1 : -1); //win
       return true;
   }
   
   return false;
}

bool LargeBoard::king_can_capture(int side)
{
   int x, y;
   int other = otherside(side);
   const Piece *king;
   Move mv;

   mv.type = Move::SIMPLE;
   king = NULL;
   
   // find king
   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
	 if(side_at(x, y) == side) {
	     king = piece_at(x, y);
	     mv.from.x = x;
	     mv.from.y = y;
	     break; }

   //assert(king != NULL);
   
   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
	 if(side_at(x, y) == other) {
	     mv.to.x = x;
	     mv.to.y = y;
	     if(king->moves[0]->is_valid(*this, mv, side))
		return true;
	 }   
   return false;
}

void LargeBoard::count_pieces()
{
   int x, y;
   
   piececount[BLACK] = piececount[WHITE] = 0;
   nKings[BLACK] = nKings[WHITE] = 0;
   has_emperor[BLACK] = has_emperor[WHITE] = 0;
   
   for(y = 0; y < type->height; y++)
      for(x = 0; x < type->width; x++)
      {
	  if(empty_square(x, y))continue;

	  if(piece_at(x, y)->has_prop(Piece::KING))
	     nKings[side_at(x, y)]++;

	  if(piece_at(x, y)->has_prop(Piece::EMPEROR))
	     has_emperor[side_at(x, y)]++;
	  
	  piececount[side_at(x, y)]++;
      }
}

LargeBoard::LargeBoard(const Board::Type *type_a)
{
   type = (const Board::Type *)type_a;

   squares = new Square[type->width * type->height];
}

Board *LargeBoard::duplicate()
{
   return (Board *) new LargeBoard(*this);
}

LargeBoard::LargeBoard(const LargeBoard &board)
{
   type = board.type;
   squares = new Square[type->width * type->height];

   memcpy(squares, board.squares, type->width * type->height * sizeof(Square));
   current_side = board.current_side;
   game_result = board.game_result;
   promote_next = board.promote_next;
   
   n_burns = board.n_burns;
   memcpy(burned_pieces,  board.burned_pieces, sizeof(burned_pieces));
   no_prom = board.no_prom;
   can_capture_lion = board.can_capture_lion;
   memcpy(nKings, board.nKings, sizeof(nKings));
   memcpy(piececount, board.piececount, sizeof(piececount));
   memcpy(has_emperor, board.has_emperor, sizeof(has_emperor));
   partial_move = board.partial_move;
}

LargeBoard::~LargeBoard()
{
   if(squares) delete squares;
}

void LargeBoard::init_setup()
{
   memcpy(squares, type->setup, sizeof(Square) * type->width * type->height);
   current_side = BLACK;
   can_capture_lion = true;
   count_pieces();
   no_prom.clear();
}

void LargeBoard::set_pos(Square *pos, int to_move)
{
   memcpy(squares, type->setup, sizeof(Square) * type->width * type->height);
   current_side = to_move;
   can_capture_lion = true;
   count_pieces();
   no_prom.clear();
}

void LargeBoard::write_ggf(ostream &os, bool one_line)
{
   list<Point>::iterator i;

   os << can_capture_lion << (one_line ? ' ' : EOL);

   os << no_prom.size() << ' ';
   for(i = no_prom.begin(); i != no_prom.end(); i++)
      os << i->x << " " << i->y << " ";
   os << (one_line ? ' ' : EOL);
   
   write_board_ggf(os, one_line);
}

bool LargeBoard::read_pos(istream &is)
{
   int i;
   Point p;

   is >> can_capture_lion;
   is >> i;
   for(; i > 0; i--) {
       is >> p.x >> p.y;
       no_prom.push_front(p); }

   if(!read_board_pos(is))
      return false;

   count_pieces();
   return true;
}

void LargeBoard::write(ostream &os)
{
   if(type->has_prop(RESTRICTED_LION))
   {
       os << (current_side == BLACK ? '*' : 'O');
       os << " can capture a lion: " <<
	 (can_capture_lion ? "true" : "false") << EOL << EOL;
   }
   
   write_board(os);
}
