// $Id: CheckersImpl.C 160 2007-06-22 15:21:10Z mburo $
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

#include "CheckersImpl.H"
#include "RegularBoardGame.H"
#include <algorithm>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
  //  strerr << (sint4)t << endl;
  
  if (t == TYPE_6)  return 6;
  if (t == TYPE_8)  return 8;
  if (t == TYPE_10) return 10;
  return -1; // illegal
}

sint4 BoardType::board_squares(sint4 t)
{
  //  strerr << (sint4)t << endl;
  
  if (t == TYPE_6)  return 36;
  if (t == TYPE_8)  return 64;
  if (t == TYPE_10) return 100;
  return -1; // illegal
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


Move::Move(sint4 sq1)
{
  ms.push_back(sq1);
}


// parse ascii-move [e.g. A1-C3-E5]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  ms.erase(ms.begin(), ms.end());

  vector<String> vec;
  String::parse(s, vec, '-');

  if (vec.size() < 2) {
    os << "illegal coords; e.g. a1-c3";
    return false;
  }
    
  FORT (i, vec.size()) {

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

    ms.push_back(xy_to_ind(x, y));
  }

  return true;
}


bool Move::is_valid() const
{
  if (ms.size() < 2) return false;

  FORT (i, ms.size()) {
    if (ms[i] < 0 || ms[i] >= BoardType::MAX_BOARD_SIZE)
      return false;
  }

  return true;
}


void Move::write(ostream &os) const
{
  if (ms.size() < 2) {

    os << "PA";

  } else {

    sint4 x, y;

    FORT (i, ms.size()) {

      ind_to_xy(ms[i], x, y);
      Form(os, "%c%d", 'A'+x, y+1);
      if (i < ms.size()-1) os << "-";
    }
  }

}

void Move::get_coords(MoveSeq &move_seq) const
{
  move_seq = ms;
}

void Move::set_coords(const MoveSeq &move_seq)
{
  ms = move_seq;
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
  int w = BoardType::board_width(type);
  
  assert((w & 1) == 0);
  int u = w/2-1;

  FORT (y, u) {
    FORT (x, u+1) {
      squares[Move::xy_to_ind(2*x+(y & 1), y)] = BLACK;
      squares[Move::xy_to_ind(2*x+((y & 1)^1), w-1-y)] = WHITE;        
    }
  }

  turn_color = Color::BLACK;
}


void Board::random_setup(int /*ra*/)
{
  regular_setup();
  
  int w = BoardType::board_width(type);;

  // promote three random checkers for each side

  FORT (i, 3) {

    FORT (j, 2) {

      sint4 x, y;
      sint4 col = (j > 0) ? Color::BLACK : Color::WHITE;
      
      do {
        x = ::ra.num(w);
        y = ::ra.num(w);
      } while (squares[Move::xy_to_ind(x, y)] != col);

      if (j > 0) squares[Move::xy_to_ind(x, y)] = Board::BLACK_KING;
      else       squares[Move::xy_to_ind(x, y)] = Board::WHITE_KING;
    }
  }
  
  turn_color = Color::BLACK;
}


// ra < 0 => regular setup, otherwise random_setup

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

  if (ra < 0) regular_setup(); else random_setup(ra);
}


sint4 Board::get_to_move() const
{
  return turn_color;
}


void Board::toggle_to_move()
{
  turn_color = Color::opponent(turn_color);
} 


bool Board::moves_available(bool only_jumps) const
{
  SqCont own_checker, own_king;
  SqCont opp_checker, opp_king;
  sint4 dir;
  
  if (turn_color == BLACK) {
    own_checker = BLACK; own_king = BLACK_KING; dir = BoardType::DX;
    opp_checker = WHITE; opp_king = WHITE_KING;
  } else {
    own_checker = WHITE; own_king = WHITE_KING; dir = -BoardType::DX;
    opp_checker = BLACK; opp_king = BLACK_KING;
  }

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == own_checker || squares[i] == own_king) {
      
      // checker jump possible?

      if ((squares[i+dir+1] == opp_checker || squares[i+dir+1] == opp_king) &&
          squares[i+2*(dir+1)] == EMPTY) return true;

      if ((squares[i+dir-1] == opp_checker || squares[i+dir-1] == opp_king) &&
          squares[i+2*(dir-1)] == EMPTY) return true;

      // checker move possible?
      
      if (!only_jumps && (squares[i+dir+1] == EMPTY ||
                          squares[i+dir-1] == EMPTY)) return true;
      
      if (squares[i] == own_king) {

        // king jump possible?
        
        if ((squares[i-dir+1] == opp_checker || squares[i-dir+1] == opp_king) &&
            squares[i+2*(-dir+1)] == EMPTY) return true;
        
        if ((squares[i-dir-1] == opp_checker || squares[i-dir-1] == opp_king) &&
            squares[i+2*(-dir-1)] == EMPTY) return true;

        // king move possible?
      
        if (!only_jumps && (squares[i-dir+1] == EMPTY ||
                            squares[i-dir-1] == EMPTY)) return true;
      }
    }
  }
  return false;
}



bool Board::is_finished() const
{
  return !moves_available();
}


sint4 Board::sq_num(Board::SqCont cont) const
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

  sint4 bc = sq_num(BLACK) + sq_num(BLACK_KING);
  sint4 wc = sq_num(WHITE) + sq_num(WHITE_KING);  

  int res = bc - wc;

  if (res == 0)

    // equal material => side to move loses by 1
    
    if (turn_color == Color::BLACK) res = -1;
    else                            res = +1;

  return res;
}


// true iff move ok

bool Board::make_move(const Move &mv)
{
  sint4 last_sq, w, x, y;
  
  sint4 dist[4] = { BoardType::DX+1, -(BoardType::DX+1),
                    BoardType::DX-1, -(BoardType::DX-1) };

  assert(mv.is_valid());

  Move::MoveSeq ms;

  mv.get_coords(ms);

  if (ms.size() < 2) return false;
  
  SqCont own_checker, own_king;
  SqCont opp_checker, opp_king;
  sint4 dir;
  
  if (turn_color == BLACK) {

    own_checker = BLACK; own_king = BLACK_KING; dir = BoardType::DX;
    opp_checker = WHITE; opp_king = WHITE_KING;
    
  } else {

    own_checker = WHITE; own_king = WHITE_KING; dir = -BoardType::DX;
    opp_checker = BLACK; opp_king = BLACK_KING;
    
  }

  if (squares[ms[0]] != own_checker &&
      squares[ms[0]] != own_king) return false;

  assert(moves_available());

  Board copy = *this;
  
  bool must_jump = moves_available(true);

  if (must_jump) {

    // move must be sequence of jumps

    for (uint4 i=1; i < ms.size(); i++) {

      if (squares[ms[i]] != EMPTY) goto error;

      if (squares[ms[i-1]] == own_king) {

        sint4 j;
        
        FOR (j, 4)
          if (ms[i] == ms[i-1] + 2*dist[j]) break;

        if (j >= 4) goto error;
        
        if (squares[ms[i-1]+dist[j]] != opp_checker &&
            squares[ms[i-1]+dist[j]] != opp_king) goto error;   

      } else {

        sint4 e = 0;
        
        if      (ms[i] == ms[i-1] + 2*(dir+1)) e = dir+1;
        else if (ms[i] == ms[i-1] + 2*(dir-1)) e = dir-1;

        if (!e) goto error;
        
        if (squares[ms[i-1]+e] != opp_checker &&
            squares[ms[i-1]+e] != opp_king) goto error; 
      }

      squares[(ms[i]+ms[i-1])/2] = EMPTY;
      squares[ms[i]] = squares[ms[i-1]];
      squares[ms[i-1]] = EMPTY;
    }

    // jumps OK
    
  } else {

    // no jump

    if (ms.size() != 2) goto error;
    if (squares[ms[1]] != EMPTY) goto error;
    
    if (squares[ms[0]] == own_king) {

      if (ms[1] != ms[0]+dir+1 && ms[1] != ms[0]+dir-1 &&
          ms[1] != ms[0]-dir+1 && ms[1] != ms[0]-dir-1)
        goto error;

    } else {
    
      if (ms[1] != ms[0]+dir+1 && ms[1] != ms[0]+dir-1)
        goto error;
    }

    squares[ms[1]] = squares[ms[0]];
    squares[ms[0]] = EMPTY;
  }

  // Promotion
  
  last_sq = ms.back();
  w = BoardType::board_width(type);
  Move::ind_to_xy(last_sq, x, y);
  
  if (turn_color == Color::BLACK && y == w-1) squares[last_sq] = BLACK_KING;
  if (turn_color == Color::WHITE && y == 0)   squares[last_sq] = WHITE_KING;
  
  toggle_to_move();
  return true;

 error:;

  *this = copy;
  return false;
}


void Board::write_ggf(ostream &os, bool one_line) const
{
  sint4 w = BoardType::board_width(type);

  os << type;
  if (!one_line) os << EOL; else os << " ";

  FORT (y, w) {
    FORT (x, w) {
      switch (squares[Move::xy_to_ind(x,y)]) {
      case BLACK:       os << "b"; break;
      case BLACK_KING:  os << "B"; break;       
      case WHITE:       os << "w"; break;
      case WHITE_KING:  os << "W"; break;       
      case EMPTY:       os << "-"; break;
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
        case 'b': squares[ind] = BLACK;      break;
        case 'B': squares[ind] = BLACK_KING; break;       
        case 'w': squares[ind] = WHITE;      break;
        case 'W': squares[ind] = WHITE_KING; break;       
        case '-': case ' ': squares[ind] = EMPTY;  break;
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
      switch (squares[Move::xy_to_ind(x,y)]) {
      case BLACK:       os << " b"; break;
      case BLACK_KING:  os << " B"; break;    
      case WHITE:       os << " w"; break;
      case WHITE_KING:  os << " W"; break;    
      case EMPTY:       if ((x+y) & 1) os << " -"; else os << "  "; break;
      default: os << "  ";
      }
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

