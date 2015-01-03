#include "ShogiImpl.H"
#include <String.H>

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

const static SmallBoard::Type *small_types[] = {
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

Board::Move::Move(const Board::Type *type_p)
{
   type = type_p;
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

   if(!isalpha(*ptr))
      return false;

   square.x = type->width - x;
   square.y = *ptr - 'a';
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
   piecenum = type->num_from_name(name);

   if(piecenum == -1)
      return false;

   piece = piecenum;
   mvs += i;
   return true;
}

bool Board::Move::parse(const string mvs)
{
   const char *ptr = mvs.c_str();

   if(read_piece(ptr))
      piece_given = true;
   else
      piece_given = false;
   
   if(read_square(ptr, from))
      square_given = true;
   else
      square_given = false;

   if(!piece_given && !square_given)
      return false;
   
   switch(*ptr) {
     case '-':
	drop = false; break;
     case '*':
	drop = true;
	if(!piece_given || square_given)
	   return false;
	break;
     default:
	return false;
   }
   ptr++;

   if(!read_square(ptr, to))
      return false;

   if(*ptr == '+') {
       promote = true;
       ptr++;
   }else
      promote = false;

   if(*ptr != 0)
      return false;

   return true;
}

Board *Board::new_board(sint4 type)
{
   if(type < 0 || type > 10)return false;

   return new SmallBoard(small_types[type]);
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

   if(mv.drop || mv.square_given)
      return true;  // no need to update

   x = y = 0;
   return find_next_legal(x, y, mv);
}

/* return true if move describes one legal move */
bool Board::one_legal_move(ostream &err, Move &mv)
{
   int x, y;
   
   if(mv.square_given)
   {
       if(mv.piece_given) {
	   if(squares[xy_to_index(mv.from.x, mv.from.y)].piece != mv.piece) {
	       err << "No such piece at given square. ";
	       return false; }
       }
       return legal_move(mv);
   }

   x = y = 0;
   if(!find_next_legal(x, y, mv))
      return false;

   if(find_next_legal(++x, y, mv))
   {
       err << "Ambiguous piece.  Use source square instead. ";
       return false;
   }

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


void Board::write_board(ostream &os)
{
   int x, y, len;
   char tmp[5];
   
   len = get_type()->name_max + 1;

   for(y = 0; y < get_type()->height; y++)
   {
       os << (char)('a'+y) << " ";
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
       os << EOL;
   }

   os << "  ";
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
   String s;
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

   for(x = 0; x < type->width; x++) {
       for(y = 0; y < type->height; y++) {
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
   
   for(x = 0; x < get_type()->width; x++) {
       for(y = 0; y < get_type()->height; y++)
	  if(!read_square(is, xy_to_index(x, y))) {
	      return false;
	  }
   }
   
   is >> c;
   if (c == '*') current_side = BLACK;
   else if (c == 'O') current_side = WHITE;
   else ERR("illegal color to move");

   return true;
}
