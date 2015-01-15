#include "ShogiImpl.H"
#include <string.h>
#include <stdio.h>
#include <iostream>

extern SmallBoard::Type shogi_type,
  micro_type,
  mini_type,
  judkins_type,
  whale_type,
  yari_type,
  tori_type,
  wa_type,
  wa2_type,
  heian_type,
  heiandai_type;

const SmallBoard::Type *small_types[] = {
   &shogi_type,
   &micro_type,
   &mini_type,
   &judkins_type,
   &whale_type,
   &yari_type,
   &tori_type,
   &wa_type,
   &wa2_type,
   &heian_type,
   &heiandai_type};

extern Board::Type sho_type,
  chu_type,
  dai_type,
  tenjiku_type,
  tenjiku2_type,
  daidai_type,
  makadaidai_type,
  makadaidai2_type,
  tai_type,
  tai2_type;

const Board::Type *large_types[] = {
   &sho_type,
   &chu_type,
   &dai_type,
   &tenjiku_type,
   &tenjiku2_type,
   &daidai_type,
   &makadaidai_type,
   &makadaidai2_type,
   &tai_type,
   &tai2_type};

/*
static inline
bool isdigit(int chr) {
   return chr >= '0' && chr <= '9';  }

static inline
bool isalpha(int chr) {
   return
     (chr >= 'a' && chr <= 'z') ||
     (chr >= 'A' && chr <= 'Z');
}
*/

Board::~Board()
{}

Board *Board::duplicate()
{}

Board::Move::Move(const Board::Type *bdtype_p)
{
   bdtype = bdtype_p;
}

bool Board::Move::read_square(const char *&mvs, Point &square)
{
   const char *ptr = mvs;
   int x = 0;

   if(!isdigit(*ptr))
      return false;
   x = *ptr - '0';
   ptr++;

   if(isdigit(*ptr)) {
       x = (x * 10) + *ptr - '0';
       ptr++; }

   if(x == 0)
      return false;
   
   if(!isalpha(*ptr))
      return false;

   square.x = bdtype->width - x;
   square.y = *ptr - 'a';

   if(square.x < 0 ||
      square.y >= bdtype->height)
      return false;

   mvs = ptr + 1;
   return true;
}

bool Board::Move::read_piece(const char *&mvs)
{
   int i, piecenum;
   char name[6];
   
   if(!isalpha(*mvs) && *mvs != '+')
      return false;
   name[0] = *mvs;

   for(i = 1; i < 5; i++) {
      if(!isalpha(mvs[i])) break;
      name[i] = mvs[i];
   }
   name[i] = 0;
   piecenum = bdtype->num_from_name(name);

   if(piecenum == -1)
      return false;

   piece = piecenum;
   mvs += i;
   return true;
}

inline bool Board::Move::read_next_move(const char *&mvs, Point &sqr)
{
   if(!read_square(mvs, sqr))
      return false;

   if(*mvs == '+') {
       promote = true;
       return (*++mvs == 0);
   }
   return true;
}

bool Board::Move::parse(const string mvs)
{
   const char *ptr = mvs.c_str();
   promote = false;

   piece_given = read_piece(ptr);
   square_given = read_square(ptr, from);

   if(!piece_given && !square_given)
      return false;

   switch(*ptr++)
   {
     case '*':
	type = DROP;
	if(!piece_given || square_given)
	   return false;

	if(!read_square(ptr, to))
	   return false;

	return (*ptr == 0);
     case '!':
	if(*ptr == 0) {
	    type = PASS;
	    return true;
	}
	if(!read_square(ptr, to))
	   return false;
	
	type = IGUI;
	return (*ptr == 0);
     case '-': break;
     default:
	return false;
   }

   if(!read_next_move(ptr, to))
      return false;

   if(*ptr == 0) {
       type = SIMPLE; return true; }
   if(*ptr != '-')
      return false;
   
   if(!read_next_move(++ptr, to2))
      return false;

   if(*ptr == 0) {
       type = DOUBLE; return true; }
   if(*ptr != '-')
      return false;

   if(!read_next_move(++ptr, to3))
      return false;

   type = TRIPLE;
   return (*ptr == 0);
}

Board *Board::new_board(int type)
{
   if(type < 0)
      return NULL;
   if(type < 11)
      return new SmallBoard(small_types[type]);
   else if(type < 21)
      return new LargeBoard(large_types[type - 11]);
   else
      return NULL;
}

int Board::Type::num_from_name(const char *name) const
{
   int m, a = 0, b = nPieces, r;

   while(a < b) {
       m = (a + b - 1) / 2;
       r = strcmp(name, piece_names[m]);
       if (r == 0)
	  return name2pos[m];
       else if (r > 0)
	  a = m + 1;
       else
	  b = m;
   }
   return -1;
}

bool Board::find_next_legal(int &x, int &y, Move &mv)
{
   while(true)
   {
       if(x >= type->width)
       {
	   x = 0; y++;
	   if(y >= type->height)
	      return false;
       }
	   
       if(squares[xy_to_index(x, y)].side == current_side &&
	  squares[xy_to_index(x, y)].piece == mv.piece) {
	   mv.from.x = x;
	   mv.from.y = y;
	   if(legal_move(mv))
	      return true;
       }
       x++;
   }
}

bool Board::update_move(Move &mv)
{
   int x, y;

   if(mv.type == Move::DROP || mv.square_given)
      return true;  // no need to update

   x = y = 0;
   return find_next_legal(x, y, mv);
}

/* Return true if move describes one legal move
   When the given only a piecename, there must
   be exactly one piece for which this move is
   legal.
*/

bool Board::one_legal_move(Move &mv)
{
   int x, y;
   
   if(mv.type == Move::DROP)
      return legal_move(mv);
   
   if(mv.square_given)
   {
       if(mv.piece_given) {
	   if(squares[xy_to_index(mv.from.x, mv.from.y)].piece != mv.piece) {
	       return false; }
       }
       return legal_move(mv);
   }

   x = y = 0;
   if(!find_next_legal(x, y, mv))
      return false;

   if(find_next_legal(++x, y, mv))
      return false;
   
   return true;
}


static inline
void write_aligned(ostream &os, const char *piece, int length)
{
   char str[6] = "     ";
   int plen = strlen(piece);
   int start = (length - plen) / 2;

   memcpy(str + start, piece, plen);
   str[length] = 0;

   os << str;
}

static inline
void strupcase(char *dest, const char *str)
{
   int i;
   for(i = 0; str[i]; i++) {
       if(str[i] >= 'a' && str[i] <= 'z')
	  dest[i] = str[i] - 'a' + 'A';
       else
	  dest[i] = str[i];
   }
   dest[i] = 0;
}

static inline
void strdowncase(char *dest, const char *str)
{
   int i;
   for(i = 0; str[i]; i++) {
       if(str[i] >= 'A' && str[i] <= 'Z')
	  dest[i] = str[i] - 'A' + 'a';
       else
	  dest[i] = str[i];
   }
   dest[i] = 0;
}

const Piece *Board::lookup_piece(const char *name)
{
   int piecenum;
   const Piece *piece;

   piecenum = type->num_from_name(name);
   if(piecenum != -1)
      piece = type->pieces[piecenum];
   else {
       if(name[0] != '+')
	  return NULL;
       
       piecenum = type->num_from_name(name + 1);
       if(piecenum == -1)
	  return NULL;

       piece = type->pieces[piecenum];
       piece = piece->promote_to;
   }
   
   return piece;
}

void Board::write_board(ostream &os)
{
   int x, y, len;
   char tmp[5];
   
   len = get_type()->name_max + 1;

   os << "    ";
   for(x = get_type()->width; x > 0; x--) {
       sprintf(tmp, "%i", x);
       write_aligned(os, tmp, len);
   }
   os << EOL;

   os << "  +-";
   for(x = get_type()->width * len; x > 0; x--) os << '-';
   os << '+' << EOL;

   for(y = 0; y < get_type()->height; y++)
   {
       os << (char)('a'+y) << " | ";
       for(x = 0; x < get_type()->width; x++)
       {
	   switch(side_at(x, y)) {
	     case WHITE:
		strdowncase(tmp, piece_at(x, y)->name);
		write_aligned(os, tmp, len);
		break;
	     case BLACK:
		strupcase(tmp, piece_at(x, y)->name);
		write_aligned(os, tmp, len);
		break;
	     case NO_SIDE:
		write_aligned(os, ((x+y) % 2 ? "-" : " "), len);
	   }
       }
       os << "| " << (char)('a'+y) << EOL;
   }

   os << "  '-";
   for(x = get_type()->width * len; x > 0; x--) os << '-';
   os << '\'' << EOL;

   os << "    ";
   for(x = get_type()->width; x > 0; x--) {
       sprintf(tmp, "%i", x);
       write_aligned(os, tmp, len);
   }
   os << EOL << EOL;

   os << (current_side == BLACK ? "* to move" : "O to move") << EOL;
}

void Board::write_square(ostream &os, int side,
			 const Piece *piece)
{
}

bool Board::read_square(istream &is, int pos)
{
   string s;
   char c;
   int piecenum;

   is >> c;
   switch(c) {
     case '-':
	squares[pos].side = NO_SIDE;
	return true;
     case '*':
	squares[pos].side = BLACK;
	break;
     case 'O':
	squares[pos].side = WHITE;
	break;
     default:
	return false;
   }

   is >> s;
   piecenum = type->num_from_name(s.c_str());
   if(piecenum == -1) {
       return false;
   }

   squares[pos].piece = piecenum;
   return true;
}

void Board::write_board_ggf(ostream &os, bool one_line)
{
   int x, y;

   for(y = 0; y < type->height; y++) {
       for(x = 0; x < type->width; x++) {
	   switch (side_at(x, y)) {
	     case BLACK:
		os << "*" << piece_at(x, y)->name << " "; break;
	     case WHITE:
		os << "O" << piece_at(x, y)->name << " "; break;
	     default:
		os << "- "; break;
	   }
       }
	   //	  write_square(os, side_at(x,  y), piece_at(x, y));
       os << (one_line ? ' ' : EOL);
   }
   
   os << (current_side == BLACK ? "*" : "O");
}

bool Board::read_board_pos(istream &is)
{
   int x, y;
   char c;

   for(y = 0; y < get_type()->height; y++)
      for(x = 0; x < get_type()->width; x++) {
	  if(!read_square(is, xy_to_index(x, y))) {
	      return false;
	  }
   }
   
   is >> c;
   if (c == '*') current_side = BLACK;
   else if (c == 'O') current_side = WHITE;
   
   return true;
}
