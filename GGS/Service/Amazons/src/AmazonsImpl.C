// $Id: AmazonsImpl.C 160 2007-06-22 15:21:10Z mburo $
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
 
#include "AmazonsImpl.H"
#include "RegularBoardGame.H"
#include <algorithm>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
  //  strerr << (sint4)t << endl;
  
  if (t == TYPE_4)  return 4;
  if (t == TYPE_6)  return 6;
  if (t == TYPE_8)  return 8;
  if (t == TYPE_10) return 10;
  if (t == TYPE_12) return 12;
  if (t == TYPE_14) return 14;

  return -1; // illegal type
}

sint4 BoardType::board_squares(sint4 t)
{
  //  strerr << (sint4)t << endl;
  
  if (t == TYPE_4)  return 16;
  if (t == TYPE_6)  return 36;
  if (t == TYPE_8)  return 64;
  if (t == TYPE_10) return 100;
  if (t == TYPE_12) return 144;
  if (t == TYPE_14) return 196;

  return -1; // illegal type
}


bool BoardType::legal_rand_type(sint4 /*t*/, sint4 rand_type)
{
  return rand_type < 0;
}


//-------------------------------------------------------------------


sint4 Move::xy_to_ind(sint4 x, sint4 y)
{
  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);    
  return (y+1) * BoardType::DX + x + 1;  
}


void Move::ind_to_xy(sint4 ind, sint4 &x, sint4 &y)
{
  assert(ind >= 0 && ind < BoardType::MAX_BOARD_SIZE);
  y = ind / BoardType::DX - 1;
  x = ind % BoardType::DX - 1;

  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);
}

Move::Move(sint4 f, sint4 t, sint4 a)
{
  from = f;
  to = t;
  arrow = a;
}


// parse ascii-move [e.g. "A1-A8-B8" ]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  from = to = arrow = UNDEF;
  
  vector<String> vec;
  String::parse(s, vec, '-');

  if (vec.size() != 3) {
    os << "illegal coords; e.g. a1-a8-b8";
    return false;
  }
    
  sint4 coords[3];

  FORT (i, 3) {

    if (vec[i].size() < 2) { os << "illegal coords; e.g. a1-a8-b8"; return false; }
    
    istringstream iss(vec[i]);

    int x = toupper(iss.get()) - 'A';

    if (x < 0 || x >= BoardType::MAX_BOARD_WIDTH) {
      os << "move-x out of range";
      return false;
    }

    int y;
    
    iss >> y;
    if (!iss) {
      os << "missing or illegal move-y";
      return false;
    }

    --y;

    if (y < 0 || y >= BoardType::MAX_BOARD_WIDTH) {
      os << "move-y out of range";
      return false;
    }

    char c;
    iss >> c;
    if (iss) { os << "illegal move '" << c << "'"; return false; }

    coords[i] = xy_to_ind(x, y);
  }

  from  = coords[0];
  to    = coords[1];
  arrow = coords[2];

  return true;
}


bool Move::is_valid() const
{
  return
    from >= 0 && from < BoardType::MAX_BOARD_SIZE &&
    to >= 0 && to < BoardType::MAX_BOARD_SIZE &&
    arrow >= 0 && arrow < BoardType::MAX_BOARD_SIZE;
}

void Move::get_coords(sint4 &f, sint4 &t, sint4 &a) const
{
  f = from;
  t = to;
  a = arrow;
}

void Move::write(ostream &os) const
{
  if (from == UNDEF) {

    os << "?";

  } else if (from == PASS) {

    os << "PA";

  } else {

    int x, y;

    ind_to_xy(from, x, y);
    Form(os, "%c%d-", 'A'+x, y+1);

    ind_to_xy(to, x, y);
    Form(os, "%c%d-", 'A'+x, y+1);

    ind_to_xy(arrow, x, y);
    Form(os, "%c%d", 'A'+x, y+1);
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



static sint4 transform_index(sint4 w, sint4 ind, sint4 t)
{
  if (t == 0) return ind; // identity

  sint4 x, y;

  if (t < 4) Move::ind_to_xy(ind, x, y); else{ Move::ind_to_xy(ind, y, x); t -= 4; }

  switch (t) {
  case 0: break;
  case 1: x = w-1-x; break;
  case 2: y = w-1-y; break;
  case 3: x = w-1-x; y = w-1-y; break;
  default: ERR("illegal transformation");
  }

  return Move::xy_to_ind(x, y);
}

void Board::regular_setup()
{
  int w = BoardType::board_width(type);
  
  assert((w & 1) == 0);
  int u = (w-1)/3;

  squares[Move::xy_to_ind(0, u)]     = BLACK;
  squares[Move::xy_to_ind(u, 0)]     = BLACK;
  squares[Move::xy_to_ind(w-u-1, 0)] = BLACK;
  squares[Move::xy_to_ind(w-1, u)]   = BLACK;
	  
  squares[Move::xy_to_ind(0, w-u-1)]   = WHITE;
  squares[Move::xy_to_ind(u, w-1)]     = WHITE;
  squares[Move::xy_to_ind(w-u-1, w-1)] = WHITE;
  squares[Move::xy_to_ind(w-1, w-u-1)] = WHITE;

  turn_color = Color::BLACK;
}


void Board::random_setup()
{
  int w = BoardType::board_width(type);

#if 0

  // old
  
  // block two squares on 2 middle rows

  FORT (i, 2) {

    sint4 ind;
    
    FOREVER {

      sint4 x = ::ra.num(w);
      sint4 y = ::ra.num(2) + w/2 - 1;
      ind = Move::xy_to_ind(x, y);
      if (squares[ind] == EMPTY) break;
    }

    squares[ind] = HIT;

  }
  
  // place 8 amazons randomly
  
  FORT (i, 8) {

    sint4 ind;
    
    FOREVER {

      sint4 x = ::ra.num(w);
      sint4 y = ::ra.num(w);
      ind = Move::xy_to_ind(x, y);
      if (squares[ind] == EMPTY) break;
    }

    squares[ind] = i < 4 ? BLACK : WHITE;
  }

#else

  // generate symmetric position, no hit squares

  sint4 valid_trans[6] = { 1,2,3,4,7,7 };
  // no identity, no 90-degree rotation, mirror on point twice as likely
  
  sint4 trans = valid_trans[::ra.num(6)];

  // place 4 amazons pairs randomly

  FORT (i, 4) {

    sint4 ind, trans_ind;

    FOREVER {
    
      FOREVER {
	sint4 x = ::ra.num(w);
	sint4 y = ::ra.num(w);
	ind = Move::xy_to_ind(x, y);
	if (squares[ind] == EMPTY) break;
      }
      
      trans_ind = transform_index(w, ind, trans);
      if (squares[trans_ind] == EMPTY && ind != trans_ind) break;
    }

    squares[ind] = BLACK;
    squares[trans_ind] = WHITE;	
  }

#endif
  
  turn_color = Color::BLACK;
}


// ra < 0 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  white_passes = 0;
  score_message_sent = false;
  
  sint4 w = BoardType::board_width(type);

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) squares[i] = Board::BORDER;

  FORT (y, w) {
    FORT (x, w) {
      squares[Move::xy_to_ind(x, y)] = Board::EMPTY;
    }
  }

  if (ra < 0) regular_setup(); else random_setup();
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
  sint4 check_color = turn_color;

  if (opponent) check_color = Color::opponent(turn_color);
    
  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == SqCont(check_color)) {
      FORT (j, 8) {
        if (squares[i+d[j]] == EMPTY) return false;
      }
    }
  }

  return true;
}


sint4 Board::reachable_squares_num(SqCont col) const
{
  Board bo(*this);

   // mark all empty squares reachable by color

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (bo.squares[i] == col) bo.mark_empty(i, col);
  }

  return bo.sq_num(col) - AMAZONS_NUM;
}


sint4 Board::unreachable_squares_num() const
{
  Board bo(*this);

  // mark all empty squares reachable by color

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if      (bo.squares[i] == BLACK) bo.mark_empty(i, BLACK);
    else if (bo.squares[i] == WHITE) bo.mark_empty(i, BLACK);    
  }

  return bo.sq_num(EMPTY);
}


bool Board::is_finished() const
{
  return
    reachable_squares_num(BLACK) == 0 &&
    reachable_squares_num(WHITE) == 0;

#if 0

  // early stop
  
  return
    reachable_squares_num(BLACK) + reachable_squares_num(WHITE) +
    unreachable_squares_num() + 2*AMAZONS_NUM + sq_num(HIT)

    ==

    BoardType::board_squares(type);

#endif
  
}


int Board::sq_num(Board::SqCont cont) const
{
  int n = 0;

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == cont) n++;
  }

  return n;
}

void Board::mark_empty(sint4 pos, SqCont cont)
{
  FORT (i, 8) {

    sint4 npos = pos+d[i];
    
    if (squares[npos] == EMPTY) {
      squares[npos] = cont;
      mark_empty(npos, cont);
    }
  }
}


sint4 Board::blacks_result() const
{
  // assert(is_finished());

  sint4 res = reachable_squares_num(BLACK) - reachable_squares_num(WHITE) +
    white_passes;

  if (turn_color == Color::WHITE) res = -res;
  if (res <= 0) res -= 1;
  if (turn_color == Color::WHITE) res = -res;
  return res;
}


static bool on_same_line(sint4 ind1, sint4 ind2)
{
  sint4 x1, y1, x2, y2;

  Move::ind_to_xy(ind1, x1, y1);
  Move::ind_to_xy(ind2, x2, y2);

  return x1 == x2 || y1 == y2 || abs(x1-x2) == abs(y1-y2);
}


static sint4 sign(sint4 d)
{
  if (d > 0) return 1;
  if (d < 0) return -1;
  return 0;
}


bool Board::empty_line(sint4 ind1, sint4 ind2) const
{
  if (ind1 == ind2) return true;
  if (!on_same_line(ind1, ind2)) return false;
  sint4 x1, y1, x2, y2;

  Move::ind_to_xy(ind1, x1, y1);
  Move::ind_to_xy(ind2, x2, y2);

  sint4 dx = sign(x2-x1);
  sint4 dy = sign(y2-y1);
  sint4 d = dx + dy * BoardType::DX;
  sint4 p = ind1 + d;

  while (p != ind2) {
    if (squares[p] != EMPTY) return false;
    p += d;
  }
  
  return true;
}


// true iff move ok

bool Board::make_move(const Move &mv)
{
  assert(mv.is_valid());

  sint4 from, to, arrow;

  mv.get_coords(from, to, arrow);

  if (to == arrow) return false;
  if (squares[from]  != turn_color) return false;
  if (squares[to]    != EMPTY)      return false;
  if (from != arrow && squares[arrow] != EMPTY) return false;    

  if (!on_same_line(from, to)) return false;
  if (!on_same_line(to, arrow)) return false;  
  
  if (!empty_line(from, to)) return false;

  squares[to]   = (Board::SqCont)turn_color;
  squares[from] = EMPTY;

  if (!empty_line(to, arrow)) {
    squares[to] = EMPTY;
    squares[from] = (Board::SqCont)turn_color;
    return false;
  }

  squares[arrow] = HIT;
  
  if (no_moves(true))
    white_passes += turn_color == BLACK ? 1 : -1;
  else
    toggle_to_move();
  
  return true;
}


void Board::write_ggf(ostream &os, bool one_line) const
{
  sint4 w = BoardType::board_width(type);

  os << type;
  if (!one_line) os << EOL; else os << " ";

  FORT (y, w) {
    FORT (x, w) {
      switch (squares[Move::xy_to_ind(x,y)]) {
      case BLACK:  os << "*"; break;
      case WHITE:  os << "O"; break;
      case HIT:    os << "+"; break;
      case EMPTY:  os << "-"; break;
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
        case '+': squares[ind] = HIT;    break;
        case '-': squares[ind] = EMPTY;  break;
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
      if      (co == BLACK) os << " *";
      else if (co == WHITE) os << " O";
      else if (co == HIT)   os << " +";
      else if (co == EMPTY) os << " -";
      else                  os << "  ";
    }
    Form( os, " %-2d", y+1);
    os << EOL;
  }

  os << "  ";
  FORT (x, w) Form( os, " %c", 'A'+x);
  os << EOL << EOL;
  if (get_to_move() == Color::BLACK)
    os << "* to move";
  else
    os << "O to move";
  os << EOL;
}


void Board::comment(string &msg) {  
  
  sint4 r_black, r_white;

  msg.erase();

  r_black = reachable_squares_num(BLACK);
  r_white = reachable_squares_num(WHITE);

  if (r_black + r_white + unreachable_squares_num() + 2*AMAZONS_NUM + sq_num(HIT)
      !=
      BoardType::board_squares(type))

    return; // not separated

  {
    String s;
    
    s.form("score B %d [ T(B)=%d T(W)=%d ToMove=%c P=%d ]",
	   blacks_result(),
	   r_black, r_white,
	   turn_color == BLACK ? 'B' : 'W',
	   white_passes);

    if (!score_message_sent) {
      score_message_sent = true;
      s += "\\where: T=Territory, P= #white's pass moves (<0:black's)";      
      s += "\\Players: A mutual \"score <your-result>\" command ends the game";
    }
    msg = s;
  }
  
}

