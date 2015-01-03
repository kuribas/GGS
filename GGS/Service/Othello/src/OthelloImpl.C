// $Id: OthelloImpl.C 160 2007-06-22 15:21:10Z mburo $
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

#include "OthelloImpl.H"
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
  if (t == TYPE_88) return 10;

  return -1; //  illegal board type
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
  if (t == TYPE_88) return 88;

  return -1; // illegal board type
}


bool BoardType::legal_rand_type(sint4 t, sint4 rand_type)
{
  sint4 b = 0;
  
  switch (t) {
  case TYPE_4:  b = 0; break;
  case TYPE_6:  b = board_squares(t) - 16; break;
  case TYPE_8:  b = board_squares(t) - 16; break;
  case TYPE_10: b = board_squares(t) - 16; break;
  case TYPE_12: b = board_squares(t) - 16; break;
  case TYPE_14: b = board_squares(t) - 16; break;
  case TYPE_88: b = board_squares(t) - 40; break;

  default: { ERR("illegal board type"); }
  }

  return rand_type >= 4 && rand_type <= b;
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


void Move::from_xy(sint4 x, sint4 y)
{
  sq_index = xy_to_ind(x, y);
}


void Move::to_xy(sint4 &x, sint4 &y) const
{
  ind_to_xy(sq_index, x, y);
}


Move::Move(sint4 i)
{
  sq_index = i;
}


// parse ascii-move [e.g. "E13" ]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  sq_index = UNDEF;

  if (s == "pa" || s == "PA" || s == "pass" || s == "PASS") {

    // pass move

    sq_index = PASS;

  } else {

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

    from_xy(x, y);

    char c;
    iss >> c;

    if (iss) { os << "illegal move '" << c << "'"; return false; }
  }

  return true;
}


bool Move::is_pass() const { return sq_index == PASS; }


bool Move::is_valid() const
{
  return is_pass() || (sq_index >= 0 && sq_index < BoardType::MAX_BOARD_SIZE);
}


void Move::write(ostream &os) const
{
  //  assert(is_valid());

  if (sq_index == UNDEF) {

    os << "?";

  } else if (sq_index == PASS) {

    os << "pass";

  } else {

    int x, y;
    to_xy(x, y);
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
  int width = BoardType::board_width(type);
  
  assert((width & 1) == 0);
  int ul = width/2-1;

  squares[Move::xy_to_ind(ul, ul)]     = WHITE;
  squares[Move::xy_to_ind(ul+1, ul+1)] = WHITE;
  squares[Move::xy_to_ind(ul, ul+1)]   = BLACK;
  squares[Move::xy_to_ind(ul+1, ul)]   = BLACK;

  turn_color = Color::BLACK;
}


void Board::random_setup(int ra)
{
  assert((BoardType::board_width(type) & 1) == 0);

  if (ra < 3) ra = 4;

  // compute all corners (>= 4 border neighbors)

  int w = BoardType::board_width(type);;
  bool corners[BoardType::MAX_BOARD_SIZE];

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    int nb = 0;
    if (squares[i] == EMPTY) {
      FORT (j, 8) {
	if (squares[i+d[j]] == BORDER) nb++;
      }
    }
    corners[i] = nb >= 4;
  }

  // compute all squares adjacent to corners (+corners -> squares to avoid)

  bool avoid[BoardType::MAX_BOARD_SIZE];

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    avoid[i] = false;
    if (squares[i] == EMPTY) {
      if (corners[i])
	avoid[i] = true;
      else {
	FORT (j, 8) {
	  if (corners[i+d[j]]) { avoid[i] = true; break; }
	}
      }
    } else avoid[i] = true;
  }

  // compute lists of squares with equal distance to center
  // (max-norm)

  vector< vector<int> > lists;

  FORT (i, w) lists.push_back( vector<int>() );

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == EMPTY && !avoid[i]) {
      int x, y;
      Move::ind_to_xy(i, x, y);
      int d = (max(abs(2*x-(w-1)), abs(2*y-(w-1))) - 1) / 2;
      lists[d].push_back(i);
    }
  }

  // cat shuffled lists
  
  vector<int> mm;

  FORT (i, w) {
    if (lists[i].size() > 0) {
      random_shuffle(lists[i].begin(), lists[i].end());

#if 0
      FORT (j, lists[i].size()) {
	Move mv(lists[i][j]);
	mv.write(strerr);
	strerr << " ";
      }
      strerr << EOL;
#endif
      
      mm.insert(mm.end(), lists[i].begin(), lists[i].end());
    }
  } 

  // shuffle prefix

  if (ra > (int)mm.size()) ra = (int)mm.size();

  random_shuffle(mm.begin(), mm.begin() + ra);

  // color distribution, allow 1/3 imbalance
  
  int rnd_white = ra / 2;
  if ((ra & 1) && ::ra.num(2)) rnd_white++; // odd ra, one more white discs 50%

  int imb = rnd_white/3;
  rnd_white += ::ra.num(2*imb+1) - imb;
  
  int rnd_black = ra - rnd_white;
  vector<SqCont> rnd_colors;

  FORT (i, rnd_white) rnd_colors.push_back(WHITE);
  FORT (i, rnd_black) rnd_colors.push_back(BLACK);

  // randomize color sequence
  
  random_shuffle(rnd_colors.begin(), rnd_colors.end());

  // at last: fill squares
  
  FORT (i, ra) squares[mm[i]] = rnd_colors[i];

  // regular othello parity
  
  turn_color = (ra & 1) ? Color::WHITE : Color::BLACK;
}


// ra <= 3 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  
  sint4 w = BoardType::board_width(type);

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) squares[i] = Board::BORDER;

  FORT (y, w) {
    FORT (x, w) {
      squares[Move::xy_to_ind(x, y)] = Board::EMPTY;
    }
  }

  if (type == BoardType::TYPE_88) {

    squares[Move::xy_to_ind(0,0)] = Board::BORDER;
    squares[Move::xy_to_ind(0,1)] = Board::BORDER;
    squares[Move::xy_to_ind(1,0)] = Board::BORDER;
	
    squares[Move::xy_to_ind(9,9)] = Board::BORDER;
    squares[Move::xy_to_ind(9,8)] = Board::BORDER;
    squares[Move::xy_to_ind(8,9)] = Board::BORDER;
	
    squares[Move::xy_to_ind(0,9)] = Board::BORDER;
    squares[Move::xy_to_ind(0,8)] = Board::BORDER;
    squares[Move::xy_to_ind(1,9)] = Board::BORDER;
	
    squares[Move::xy_to_ind(9,0)] = Board::BORDER;
    squares[Move::xy_to_ind(8,0)] = Board::BORDER;
    squares[Move::xy_to_ind(9,1)] = Board::BORDER;
  }

  if (ra <= 3) regular_setup();
#if NICOLET_RANDOM
  else {
    if ((ra < 10) || ::ra.num(2))
      random_setup(ra);
    else
      random_setup_2(ra);
    }
#else	
   else random_setup(ra);
#endif	  
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
    
  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    Move mv(i);
    if (bo.make_move(mv)) return false;
  }

  return true;
}


bool Board::is_finished() const
{
  if (!no_moves(false)) return false;
  return no_moves(true);
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
  assert(is_finished());

  int b_num = disc_num(BLACK);
  int w_num = disc_num(WHITE);
  int e_num = disc_num(EMPTY);

  int res = b_num - w_num;

  if (e_num)
    if      (res > 0) res += e_num;
    else if (res < 0) res -= e_num;

  return res;
}


// true iff move ok

bool Board::make_move(const Move &mv)
{
  bool move_ok = false;

  assert(mv.is_valid());

  if (mv.is_pass()) {
    if (no_moves()) {
      // pass OK, toggle color
      turn_color = Color::opponent(turn_color);
      return true;
    }
    return false;
  }
  
  if (squares[mv.sq_index] != Board::EMPTY) return false;

  SqCont turn_cont, opp_cont;
  
  if (turn_color == Color::BLACK) { turn_cont = BLACK; opp_cont = WHITE; }
  else                            { turn_cont = WHITE; opp_cont = BLACK; }
  
  FORT (i, 8) {

    if (squares[mv.sq_index + d[i]] == opp_cont) {

      // scan line

      int pos = mv.sq_index + d[i];

      while (squares[pos] == opp_cont) pos += d[i];

      if (squares[pos] == turn_cont) {

	move_ok = true;

	// flip discs

	while (squares[pos] != EMPTY) {
	  squares[pos] = turn_cont;
	  pos -= d[i];
	}
      }
    }
  }

  if (move_ok) {

    // place new disc and toggle color
      
    squares[mv.sq_index] = turn_cont;
    turn_color = Color::opponent(turn_color);
  }
  
  return move_ok;
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
      else if (co == EMPTY) os << " -";
      else                  os << "  ";
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

#if NICOLET_RANDOM

// Alternate algorithm to get a random set-up. 
// We try to generate a sequence of random moves
// starting from a 5 discs random position
// Stephane Nicolet, 4/12/2002
void Board::random_setup_2(int ra)
{
  assert((BoardType::board_width(type) & 1) == 0);
  
  // only use that method for more than 10 discs
  
  assert((ra >= 10));
  

  // compute all corners (>= 4 border neighbors)

  bool corners[BoardType::MAX_BOARD_SIZE];

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    int nb = 0;
    if (squares[i] == EMPTY) {
      FORT (j, 8) {
	if (squares[i+d[j]] == BORDER) nb++;
      }
    }
    corners[i] = nb >= 4;
  }

  // compute all squares adjacent to corners (+corners -> squares to avoid)

  bool avoid[BoardType::MAX_BOARD_SIZE];

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    avoid[i] = false;
    if (squares[i] == EMPTY) {
      if (corners[i])
	avoid[i] = true;
      else {
	FORT (j, 8) {
	  if (corners[i+d[j]]) { avoid[i] = true; break; }
	}
      }
    } else avoid[i] = true;
  }
  
  int   number_of_tries_left = 5;
  bool  solution_found = false;
  
  while ((number_of_tries_left > 0) && !solution_found) {
  
    int	number_of_random_moves = ra - 5;
    int	n;
    int moves[BoardType::MAX_BOARD_SIZE];
  	
    // first start with 5 random discs at the center
    random_setup(5);
  	
    // let's hope we'll find a legal sequence from this initial position
    solution_found = true;  
    
    for (n = 1 ; n <= number_of_random_moves ; n++) {
  		
      // compute all legal moves, except moves touching corners
  		
      int number_of_legal_moves = 0;
  		
      FORT (k, (sint4)BoardType::MAX_BOARD_SIZE) {
        Move mv(k);
        if ((!avoid[k]) && move_is_legal(mv)) {
	  moves[number_of_legal_moves] = k;
	  ++number_of_legal_moves;
        }
      }
   	  
      if (number_of_legal_moves == 0) {

	// one of the players passes, we could handle this
	// but it's more likely that it's an early wipe-out,
	// so it's better to try again from scratch...
  	  	
	solution_found = false;
	--number_of_tries_left;
	break; 
  			
      } else {
  		
	// play one legal move randomly  		
 
	int p = ::ra.num(number_of_legal_moves);
	Move mv(moves[p]);
	make_move(mv);
  			
      }
    }
	
    // avoid some lopsided positions

    if (solution_found) {
      int count_black = 0;
      int count_white;
      int min_count = max(ra / 4, 3);
		
      FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
	if(squares[i] == BLACK) count_black++;
      }
      count_white = ra - count_black;
		  
      if (count_black <= min_count || count_white <= min_count) {
	solution_found = false;
	--number_of_tries_left;
      }	
    }

    // clear the board if setup failed
	
    if(!solution_found) {
      FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
	if (squares[i] != BORDER) squares[i] = EMPTY;
      }
      turn_color = BLACK;
    }
  }  
  
  if (!solution_found) {
    random_setup(ra);  // give up, use Mic's algorithm instead
  }
}


// true iff move is legal. Doesn't flip the discs or change color to move.

bool Board::move_is_legal(const Move &mv)
{

  assert(mv.is_valid());

  if (mv.is_pass()) {
    if (no_moves()) {
      // pass OK, toggle color
      turn_color = Color::opponent(turn_color);
      return true;
    }
    return false;
  }
  
  if (squares[mv.sq_index] != Board::EMPTY) return false;

  SqCont turn_cont, opp_cont;
  
  if (turn_color == Color::BLACK) { turn_cont = BLACK; opp_cont = WHITE; }
  else                            { turn_cont = WHITE; opp_cont = BLACK; }
  
  FORT (i, 8) {

    if (squares[mv.sq_index + d[i]] == opp_cont) {

      // scan line

      int pos = mv.sq_index + d[i];

      while (squares[pos] == opp_cont) pos += d[i];

      if (squares[pos] == turn_cont) 
      	return true;
		}
	}
  
  return false;
}
#endif
