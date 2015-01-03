// $Id: AtaxxImpl.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Michael Buro, mic@research.nj.nec.com
    NEC Research Institute
    4 Independence Way
    Princeton, NJ 08540, USA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "AtaxxImpl.H"
#include "RegularBoardGame.H"
#include <algorithm>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
  //  strerr << (sint4)t << endl;

  if (t < 4 || t > MAX_BOARD_WIDTH) return -1;
  return t;
}

sint4 BoardType::board_squares(sint4 t)
{
  //  strerr << (sint4)t << endl;

  if (t < MIN_BOARD_WIDTH) ERR("illegal board type");
  return t*t;
}


bool BoardType::legal_rand_type(sint4 t, sint4 rand_type)
{
  return rand_type >= 1 && rand_type <= board_squares(t)/2;
}


//-------------------------------------------------------------------


sint4 Move::xy_to_ind(sint4 x, sint4 y)
{
  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);    
  return (y+2) * BoardType::DX + x + 2;
}


void Move::ind_to_xy(sint4 ind, sint4 &x, sint4 &y)
{
  assert(ind >= 0 && ind < BoardType::MAX_BOARD_SIZE);
  y = ind / BoardType::DX - 2;
  x = ind % BoardType::DX - 2;

  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);
}

// parse ascii-move [e.g. A7-A8]
// return true iff syntax and limited sematics are correct

bool Move::parse(ostream &os, const String &s)
{
  from = to = UNDEF;
    
  vector<String> vec;
  String::parse(s, vec, '-');

  if (vec.size() != 2) {
    os << "illegal move syntax (e.g. a1-a2)";
    return false;
  }

  sint4 coords[2];
  
  FORT (i, 2) {

    istringstream iss(vec[i]);

    if (!Move::read_square(iss, coords[i])) {
      os << "illegal square";
      return false;
    }
  }

  from = coords[0];
  to   = coords[1];

  if (!is_valid()) {
    os << "invalid move";
    return false;
  }
  
  return true;
}

bool Move::is_valid() const
{
  if (from < 0 || from >= BoardType::MAX_BOARD_SIZE ||
      to < 0   || to >= BoardType::MAX_BOARD_SIZE ||
      from == to) return false;

  sint4 x0,y0,x1,y1;

  Move::ind_to_xy(from, x0, y0);
  Move::ind_to_xy(to, x1, y1);

  sint4 absx = abs(x0-x1), absy = abs(y0-y1);

  if (absx >= 3 || absy >= 3) return false;

  return true;
}


bool Move::read_square(istream &is, sint4 &sq)
{
  char c;

  is >> c;
  if (!is) return false;

  if (c == '?' || c == '%') { sq = -1; return true; }

  int x = toupper(c) - 'A';
  if (x < 0 || x >= BoardType::MAX_BOARD_WIDTH) return false;

  int y;
  is >> y;
  if (!is) return false;
  --y;
  if (y < 0 || y >= BoardType::MAX_BOARD_WIDTH) return false;

  sq = xy_to_ind(x, y);

  //errsttr << "SQUARE=" << sq << " x=" << x << " y=" << y << endl;
  
  return true;
}


void Move::write(ostream &os) const
{
  //  assert(is_valid());

  if (from == UNDEF) {

    os << "?";

  } else {
    
    int x0, y0, x1, y1;
    Move::ind_to_xy(from, x0, y0);
    Move::ind_to_xy(to, x1, y1);    
    Form(os, "%c%d-%c%d", 'A'+x0, y0+1, 'A'+x1, y1+1);
  }

}


//-------------------------------------------------------------------


int Board::d[8] =
{
  +1, -1,
  BoardType::DX, -BoardType::DX,
  BoardType::DX+1, -(BoardType::DX+1),
  BoardType::DX-1, -(BoardType::DX-1)
};


void Board::regular_setup()
{
  int width = BoardType::board_width(type);
  sint4 w1 = width-1;

  squares[Move::xy_to_ind(0, 0)]   = BLACK;
  squares[Move::xy_to_ind(w1, w1)] = BLACK;
  squares[Move::xy_to_ind(w1, 0)]  = WHITE;
  squares[Move::xy_to_ind(0, w1)]  = WHITE;

  turn_color = Color::BLACK;
}


void Board::random_setup(int ra)
{
  regular_setup();

  // block ra squares

  FORS (i, ra) {

    int rs;

    do {
      rs = random() % BoardType::MAX_BOARD_SIZE;
    } while (squares[rs] != EMPTY);

    squares[rs] = BLOCKED;
  }
}


// ra <= 0 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  rev_move_num = 0;
  
  sint4 w = BoardType::board_width(type);

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) squares[i] = BORDER;

  FORT (y, w) {
    FORT (x, w) {
      squares[Move::xy_to_ind(x, y)] = EMPTY;
    }
  }
  
  if (ra <= 0) regular_setup(); else random_setup(ra);
}


sint4 Board::get_to_move() const
{
  return turn_color;
}

void Board::toggle_to_move()
{
  turn_color = Color::opponent(turn_color);
} 

bool Board::no_moves(bool opponent) const
{
  Board bo = *this;
  if (opponent) bo.toggle_to_move();
  SqCont turn_cont = (bo.turn_color == Color::BLACK) ? BLACK : WHITE;
  
  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == turn_cont) {
      for (sint4 dx = -2; dx <= 2; dx++) {
	for (sint4 dy = -2; dy <= 2; dy++) {
	  if (squares[i + dx + dy * BoardType::DX] == EMPTY) return false;
	}
      }
    }
  }

  return true;
}


bool Board::is_finished() const
{
  if (disc_num(BLACK) == 0) return true;
  if (disc_num(WHITE) == 0) return true;  
  return no_moves(false) && no_moves(true);
}


int Board::disc_num(Board::SqCont cont) const
{
  int n = 0;

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == cont) n++;
  }

  return n;
}

int Board::disc_num() const
{
  int n = 0;

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == BLACK || squares[i] == WHITE) n++;
  }

  return n;
}

sint4 Board::blacks_result() const
{
  sint4 nb = disc_num(BLACK);
  sint4 nw = disc_num(WHITE);
  sint4 ne = disc_num(EMPTY);  
  
  if (disc_num(BLACK) == 0) return -(nw+ne);
  if (disc_num(WHITE) == 0) return (nb+ne);

  return nb - nw;
}

// true iff move ok

bool Board::legal_move(const Move &mv)
{
  if (!mv.is_valid()) return false;

  SqCont turn_cont = (turn_color == Color::BLACK) ? BLACK : WHITE;

  if (squares[mv.from] != turn_cont) return false;
  if (squares[mv.to]   != EMPTY) return false;  
  return true;
}


// assumes move is legal

void Board::play_move(const Move &mv)
{
  assert(legal_move(mv));

  SqCont turn_cont = (turn_color == Color::BLACK) ? BLACK : WHITE;
  SqCont opp_cont  = (turn_color == Color::BLACK) ? WHITE : BLACK;
    
  squares[mv.to] = turn_cont;
    
  sint4 x0,y0,x1,y1;

  Move::ind_to_xy(mv.from, x0, y0);
  Move::ind_to_xy(mv.to, x1, y1);

  sint4 absx = abs(x0-x1), absy = abs(y0-y1);

  if (absx >= 2 || absy >= 2) {

    // jumping

    squares[mv.from] = EMPTY;
    rev_move_num++;

  } else rev_move_num = 0;

  // flip opponent's discs in vicinity

  FORS (i, 8) {
    if (squares[mv.to+d[i]] == opp_cont)
      squares[mv.to+d[i]] = turn_cont;
  }

  if (!no_moves(true)) turn_color = Color::opponent(turn_color);
}


void Board::write_ggf(ostream &os, bool one_line) const
{
  sint4 w = BoardType::board_width(type);

  os << type;
  if (!one_line) os << EOL; else os << " ";

  FORT (y, w) {
    FORT (x, w) {
      switch (squares[Move::xy_to_ind(x,y)]) {
      case BLACK:    os << "*"; break;
      case WHITE:    os << "O"; break;
      case EMPTY:    os << "-"; break;
      case BLOCKED:  os << "+"; break;	
      default: break;
      }
    }

    if (one_line) os << " "; else os << EOL;
  }

  if (get_to_move() == Color::BLACK) os << "*"; else os << "O";
}


bool Board::read_ggf(istream &is)
{
  char c;
  String s;
  ostringstream os;
  
  is >> type;
  if (!is) ERR("illegal board type");

  init(type, 0);
   
  sint4 w = BoardType::board_width(type);

  FORT (y, w) {
    FORT (x, w) {
      sint4 ind = Move::xy_to_ind(x,y);
      if (squares[ind] != BORDER) {
	is >> c;
	//      errstr << "square " << c << endl << flush;
	if (!is) return false;
	
	switch (c) {
	case '*': squares[ind] = BLACK;  break;
	case 'O': squares[ind] = WHITE;  break;
	case '-': squares[ind] = EMPTY;  break;
	case '+': squares[ind] = BLOCKED;  break;	  
	default: ERR("unknown square cont");
	}
      }
    }
  }

  is >> c;

  if (c == '*') turn_color = Color::BLACK;
  else if (c == 'O') turn_color = Color::WHITE;
  else ERR("illegal color to move");

  return true;
}


void Board::write(ostream &os) const
{
  sint4 w = BoardType::board_width(type);
  
  os << "  ";
  FORT (x, w) Form( os, " %c", 'A'+x);
  os << EOL;
      
  FORT (y, w) {
    Form( os, "%2d", y+1);
    FORT (x, w) {
      Board::SqCont co = squares[Move::xy_to_ind(x, y)];
      if      (co == BLACK)   os << " *";
      else if (co == WHITE)   os << " O";
      else if (co == EMPTY)   os << " -";
      else if (co == BLOCKED) os << " +";
      else                            os << "  ";
    }
    Form( os, " %-2d", y+1);
    os << EOL;
  }

  os << "  ";
  FORT (x, w) Form( os, " %c", 'A'+x);
  os << EOL << EOL;
  if (get_to_move() == Color::BLACK) os << "* to move"; else os << "O to move";
  os << EOL;
}


/* ADLER32 (Adler-32 checksum)
This contains a checksum value of the uncompressed data
(excluding any dictionary data) computed according to Adler-32
algorithm. This algorithm is a 32-bit extension and improvement
of the Fletcher algorithm, used in the ITU-T X.224 / ISO 8073
standard. See references [4] and [5] in Chapter 3, below)

Adler-32 is composed of two sums accumulated per byte: s1 is
the sum of all bytes, s2 is the sum of all s1 values. Both sums
are done modulo 65521. s1 is initialized to 1, s2 to zero.  The
Adler-32 checksum is stored as s2*65536 + s1 in most-
significant-byte first (network) order.
*/

static uint4 adler32(const vector<uint1> &v)
{
  const uint4 BASE = 65521;
  uint4 s1 = 1, s2 = 0;
  uint4 n = v.size();

  FORU (i, n) {
    s1 += v[i]; if (s1 >= BASE) s1 -= BASE;
    s2 += s1;   if (s2 >= BASE) s2 -= BASE;
  }
  return (s2 << 16) + s1;
}


uint4 Board::adler32() const
{
  vector<uint1> v;
  v.reserve(BoardType::MAX_BOARD_SIZE+1);
  
  FORS (i, sint4(BoardType::MAX_BOARD_SIZE)) v.push_back(squares[i]);
  v.push_back(turn_color);
  return ::adler32(v);
}

