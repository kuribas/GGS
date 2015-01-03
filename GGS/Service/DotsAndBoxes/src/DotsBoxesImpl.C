// $Id: DotsBoxesImpl.C 160 2007-06-22 15:21:10Z mburo $
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

#include "DotsBoxesImpl.H"
#include "RegularBoardGame.H"
#include <algorithm>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
  //  strerr << (sint4)t << endl;

  if (t == TYPE_3)  return 3;
  if (t == TYPE_4)  return 4;
  if (t == TYPE_5)  return 5;  
  if (t == TYPE_6)  return 6;
  if (t == TYPE_7)  return 7;
  if (t == TYPE_8)  return 8;
  if (t == TYPE_9)  return 9;
  if (t == TYPE_10) return 10;
  if (t == TYPE_11) return 11;
  if (t == TYPE_12) return 12;

  return -1; //  illegal board type
}

sint4 BoardType::board_squares(sint4 t)
{
  //  strerr << (sint4)t << endl;

  sint4 w = board_width(t);
  if (w < 0) return w;
  return w*w;
}


bool BoardType::legal_rand_type(sint4 t, sint4 rand_type)
{
  sint4 E = 2*board_width(t)*(board_width(t)-1);
  return rand_type >= 1 && rand_type <= (E*3)/4;
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


Move::Move()
{
  edge = UNDEF;
}


// parse ascii-move [e.g. "A1-B1" ]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  edge = UNDEF;

  if (s.size() < 2) { os << "illegal move"; return false; }

  istringstream iss(s);
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

  if (((x+y) & 1) != 1) {
    os << "x,y must have different parity";
    return false;
  }
  
  edge = xy_to_ind(x, y);
  
  char c;
  iss >> c;

  if (iss) { os << "illegal move '" << c << "'"; return false; }

  return true;
}


bool Move::is_valid() const
{
  return edge != UNDEF;
}


void Move::write(ostream &os) const
{
  // assert(is_valid());

  if (edge == UNDEF) {

    os << "?";

  } else {

    int x, y;
    ind_to_xy(edge, x, y);
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


void Board::regular_setup()
{
  turn_color = Color::BLACK;
}


void Board::random_setup(int randtype)
{
  sint4 w = BoardType::board_width(type);

  // make randtype moves

  turn_color = Color::BLACK;
  
  FORT (i, randtype) {

    Move mv;

    do {

      sint4 x, y;
      do {
	x = ::ra.num(2*w-1);
	y = ::ra.num(2*w-1);
      } while (((x+y) & 1) == 0);

      mv.edge = Move::xy_to_ind(x, y);

    } while (!make_move(mv));

    // cout << "RANDMOVE "; mv.write(cout); cout  << endl;
    
  }
}


// ra == 0 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  
  sint4 w = BoardType::board_width(type);

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) squares[i] = Board::BORDER;

  FORT (y, 2*w-1) {
    FORT (x, 2*w-1) {
      squares[Move::xy_to_ind(x, y)] = Board::EMPTY;
    }
  }
  
  if (ra == 0) regular_setup(); else random_setup(ra);
}


sint4 Board::get_to_move() const
{
  return turn_color;
}

void Board::toggle_to_move()
{
  turn_color = Color::opponent(turn_color);
} 

bool Board::is_finished() const
{
  sint4 w = BoardType::board_width(type);
  return box_count(EDGE) == 2*w*(w-1);
}

sint4 Board::box_count(Board::SqCont cont) const
{
  sint4 n = 0;

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == cont) n++;
  }

  return n;
}

sint4 Board::blacks_result() const
{
  assert(is_finished());

  return box_count(BLACK) - box_count(WHITE);
}


static bool occupied(sint4 cont)
{
  return cont == Board::EDGE;
}


// true iff move ok

bool Board::make_move(const Move &mv)
{
  if (!mv.is_valid()) return false;

  sint4 ed = mv.edge;
  
  if (squares[ed] != EMPTY) return false;

  squares[ed] = EDGE;
  
  sint4 x, y;

  Move::ind_to_xy(mv.edge, x, y);

  SqCont move_cont = (turn_color == Color::BLACK) ? BLACK : WHITE;
  
  bool new_box = false;
  const sint4 DX = BoardType::DX;
  
  if ((y & 1) == 0) {

    //  o-o
    
    if (occupied(squares[ed+DX-1]) &&
	occupied(squares[ed+DX+1]) &&
	occupied(squares[ed+2*DX])) {

      // o-o
      //  X
      
      squares[ed+DX] = move_cont;
      new_box = true;
    }

    if (occupied(squares[ed-DX-1]) && 
	occupied(squares[ed-DX+1]) &&
	occupied(squares[ed-2*DX])) {

      //  X
      // o-o

      squares[ed-DX] = move_cont;
      new_box = true;
    }
    
  } else {

    //  o
    //  |
    //  o
    
    if (occupied(squares[ed-DX+1]) &&
	occupied(squares[ed+2]) &&
	occupied(squares[ed+DX+1])) {

      //  o
      //  |X
      //  o

      squares[ed+1] = move_cont;
      new_box = true;
    }

    if (occupied(squares[ed-DX-1]) && 
	occupied(squares[ed-2]) &&
	occupied(squares[ed+DX-1])) {

      //   o
      //  X|
      //   o

      squares[ed-1] = move_cont;
      new_box = true;
    }
  }

  // if (new_box) cout << "NEWBOX" << endl;
  
  if (!new_box) toggle_to_move();
  return true;
}


void Board::write_ggf(ostream &os, bool one_line) const
{
  gen_write(os, true, one_line);
}


void Board::write(ostream &os) const
{
  gen_write(os, false, false);
}


bool Board::read_ggf(istream &is)
{
  char c;
  SqCont sq, ed;
  String s;
  ostringstream os;
  
  is >> type;
  if (!is) ERR("illegal board type");

  init(type, 0);
   
  sint4 w = BoardType::board_width(type);

  FORT (y, 2*w-1) {

    if ((y & 1) == 0) {
    
      FORT (x, w-1) {
	is >> c; if (c != NODE) ERR(char(NODE)+" expected");
	is >> c;
	if (c == OCC_EDGE_H) ed = EDGE;
	else if (c == EMPTY_EDGE) ed = EMPTY;
	else ERR("edge expected");
	squares[Move::xy_to_ind(2*x+1, y)] = ed;
      }
      is >> c; if (c != NODE) ERR(char(NODE)+" expected");

    } else {

      FORT (x, w-1) {

	is >> c;
	if (c == OCC_EDGE_V) ed = EDGE;
	else if (c == EMPTY_EDGE) ed = EMPTY;
	else ERR("edge expected");
	
	squares[Move::xy_to_ind(2*x, y)] = ed;

	is >> c;
	if      (c == BLACK_SQUARE) sq = BLACK;
	else if (c == WHITE_SQUARE) sq = WHITE;
	else if (c == EMPTY_SQUARE) sq = EMPTY;
	else ERR("sqcont expected");

	squares[Move::xy_to_ind(2*x+1, y)] = sq;
      }
      
      is >> c;
      if (c == OCC_EDGE_V) ed = EDGE;
      else if (c == EMPTY_EDGE) ed = EMPTY;
      else ERR("edge expected");
      
      squares[Move::xy_to_ind(2*w-2, y)] = ed;
    }
  }

  is >> c;

  if      (c == BLACK_SQUARE) turn_color = Color::BLACK;
  else if (c == WHITE_SQUARE) turn_color = Color::WHITE;
  else ERR("illegal color to move");

  return true;
}


void Board::gen_write(ostream &os, bool ggf, bool one_line) const
{
  sint4 w = BoardType::board_width(type);

  // !ggf settings
  
  string empty_edge_h  = "  ";
  string occ_edge_h    = "--";
  string empty_edge_v  = " ";
  string occ_edge_v    = "|";  
  string empty_square  = "  ";
  string black_square  = "##";
  string white_square  = "()";
  string node          = "+";
  
  if (!ggf) {
    
    os << "   ";
    FORT (x, 2*w-2) { Form(os, "%c%c ", 'A'+x, 'B'+x); x++; }
    os << char('A'+2*w-2);
    os << EOL;
    
  } else {

    os << type;
    if (one_line) os << " "; else os << EOL;

    empty_edge_h  = EMPTY_EDGE;
    occ_edge_h    = OCC_EDGE_H;
    empty_edge_v  = EMPTY_EDGE;
    occ_edge_v    = OCC_EDGE_V;  
    empty_square  = EMPTY_SQUARE;
    black_square  = BLACK_SQUARE;
    white_square  = WHITE_SQUARE;
    node          = NODE;
  }
      
  FORT (y, 2*w-1) {

    if (!ggf) Form(os, "%2d ", y+1);

    if ((y & 1) == 0) {
    
      FORT (x, w-1) {
	os << node;
	if (squares[Move::xy_to_ind(2*x+1, y)] != EMPTY)
	  os << occ_edge_h;
	else
	  os << empty_edge_h;
      }
      os << node;

    } else {

      FORT (x, w-1) {
	if (squares[Move::xy_to_ind(2*x, y)] != EMPTY)
	  os << occ_edge_v;
	else
	  os << empty_edge_v;
	
	if      (squares[Move::xy_to_ind(2*x+1, y)] == BLACK) os << black_square;
	else if (squares[Move::xy_to_ind(2*x+1, y)] == WHITE) os << white_square;
	else                                                  os << empty_square;
      }
      
      if (squares[Move::xy_to_ind(2*w-2, y)] != EMPTY)
	os << occ_edge_v;
      else
	os << empty_edge_v;
    }

    if (!ggf) Form(os, " %-2d ", y+1);
    if (!ggf || !one_line) os << EOL; else os << " ";
  }

  if (!ggf) {
    
    os << "   ";
    FORT (x, 2*w-2) { Form(os, "%c%c ", 'A'+x, 'B'+x); x++; }
    os << char('A'+2*w-2);
    os << EOL << EOL;
    if (get_to_move() == Color::BLACK) os << black_square; else os << white_square;
    os << " to move" << EOL;

  } else {

    if (!one_line) os << EOL;
    if (get_to_move() == Color::BLACK) os << black_square; else os << white_square;
    
  }
}


/*

  AB CD EF G
 1+--+--+--+
 2|()|##| 
 3+--+--+--+

*/
