// $Id: SnortImpl.C 160 2007-06-22 15:21:10Z mburo $
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

#include "Snort.H"
#include "SnortImpl.H"
#include "RegularBoardGame.H"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <time.h>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
  assert(t>1);
  assert(t<=TYPE_MAX);

  return t;
}

sint4 BoardType::board_squares(sint4 t)
{

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
  index = UNDEF;
}


// parse ascii-move [e.g. "A1-B1" ]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  index = UNDEF;

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
  
  index = xy_to_ind(x, y);
  
  char c;
  iss >> c;

  if (iss) { os << "illegal move '" << c << "'"; return false; }

  return true;
}


bool Move::is_valid() const
{
  return index != UNDEF;
}


void Move::write(ostream &os) const
{
  // assert(is_valid());

  if (index == UNDEF) {

    os << "?";

  } else {

    int x, y;
    ind_to_xy(index, x, y);
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


void Board::random_setup(int ra)
{
  regular_setup();
  
  // block ra squares

    if (type == 7)
    {
       squares[Move::xy_to_ind(1,6)] = BLOCKED;
       squares[Move::xy_to_ind(3,2)] = BLOCKED;
       squares[Move::xy_to_ind(6,4)] = BLOCKED;
    }
    else if (type == 15)
    {
       squares[Move::xy_to_ind(2, 5)] = BLOCKED;
       squares[Move::xy_to_ind(12, 3)] = BLOCKED;
       squares[Move::xy_to_ind(7, 7)] = BLOCKED;
   
    }
    else
    {
      FORS (i, ra) {

        int rs;

        do {
          rs = rand() % BoardType::MAX_BOARD_SIZE;
        } while (squares[rs] != EMPTY);

        squares[rs] = BLOCKED;
      }
  }
}


// ra == 0 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  //score_message_sent = false;
  
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

bool Board::is_finished() const
{
  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) 
  {
    if ((squares[i] == EMPTY)
        || ((squares[i] == NEARBLACK) && (get_to_move() == Color::BLACK))
        || ((squares[i] == NEARWHITE) && (get_to_move() == Color::WHITE))
        )
        return false;
  }
  return true;
}

sint4 Board::count(Board::SqCont cont) const
{
  sint4 n = 0;

  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) {
    if (squares[i] == cont) n++;
  }

  return n;
}

const int kEmptyVal = 3;
const int kOppVal = 1;

int Board::eval(int move) const
{
  bool blackToMove = (get_to_move() == Color::BLACK);
   int eval = 0;
   SqCont state = squares[move];
   if (state == EMPTY)
    eval +=5; // prefer empty over near.
        
  // check neighbor points state.
  for (int i=0; i<4; ++i)
  {
    sint4 nb = move+d[i];
    SqCont nbstate = squares[nb];
    if (nbstate == EMPTY)
        eval += kEmptyVal;
    else if
         (((nbstate == NEARBLACK) && ! blackToMove)
        || ((nbstate == NEARWHITE) && blackToMove) )
       eval += kOppVal;
    
  }  
    return eval;
}

void Board::make_good_move()
{
    int best = -1; int bestEval = -1;
  bool blackToMove = (get_to_move() == Color::BLACK);
    
    for (int i = 0; i < (sint4)BoardType::MAX_BOARD_SIZE; i++)
    {
       SqCont state = squares[i];
       if ((state == EMPTY)
        || ((state == NEARBLACK) && blackToMove)
        || ((state == NEARWHITE) && ! blackToMove))
       {    int v = eval(i);
            if (v > bestEval)
            {
                best = i; bestEval = v;
            }
       }
    }
    Move m;
    m.index = best;
    
    bool OK = make_move(m);
    assert(OK);
}

void Board::make_random_move()
{
    int moves[BoardType::MAX_BOARD_SIZE]; int nuMoves = 0;
  bool blackToMove = (get_to_move() == Color::BLACK);
    
    for (int i = 0; i < (sint4)BoardType::MAX_BOARD_SIZE; i++)
    {
       SqCont state = squares[i];
       if ((state == EMPTY)
        || ((state == NEARBLACK) && blackToMove)
        || ((state == NEARWHITE) && ! blackToMove))
       {    moves[nuMoves] = i;
            ++nuMoves;
       }
    }
    cout << "\nthere were " << nuMoves << " moves for " 
         << (blackToMove? 'B' : 'W');
    Move m;
    m.index = moves[rand() % nuMoves];
    
    bool OK = make_move(m);
    assert(OK);
}

sint4 Board::blacks_result() const
{
  assert(is_finished());

 // return count(BLACK) - count(WHITE) + count(NEARBLACK) - count(NEARWHITE);
  bool blackToMove = (get_to_move() == Color::BLACK);
    int delta = blackToMove ? -1 : 1;
  return delta + count(NEARBLACK) - count(NEARWHITE);
}


// true iff move ok

bool Board::make_move(const Move &mv)
{
  if (!mv.is_valid()) return false;

  sint4 p = mv.index;
  
  SqCont state = squares[p];
  
  bool valid = (state == EMPTY)
        || ((state == NEARBLACK) && (get_to_move() == Color::BLACK))
        || ((state == NEARWHITE) && (get_to_move() == Color::WHITE));
  if (! valid)
      return false;

  bool blackToMove = (get_to_move() == Color::BLACK);
  squares[p] = blackToMove ? BLACK : WHITE;

  // update neighbor points state.
  for (int i=0; i<4; ++i)
  {
    sint4 nb = p+d[i];
    SqCont nbstate = squares[nb];
    if (nbstate == EMPTY)
        squares[nb] = blackToMove ? NEARBLACK : NEARWHITE;
    else if (nbstate == NEARBLACK && ! blackToMove)
        squares[nb] = BLOCKED;
    else if (nbstate == NEARWHITE && blackToMove)
        squares[nb] = BLOCKED;
    
    // make sure the NEAR... states are set up consistently.
    if (blackToMove)
        assert(nbstate != WHITE);
    else
        assert(nbstate != BLACK);
  }  

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
      case BLACK:     os << "*"; break;
      case WHITE:     os << "O"; break;
      case EMPTY:     os << "-"; break;
      case NEARBLACK: os << " b"; break;
      case NEARWHITE: os << " w"; break;
      case BLOCKED:      os << "+"; break;	
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
	case 'b': squares[ind] = NEARBLACK;  break;	  
	case 'w': squares[ind] = NEARWHITE;  break;	  
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
      if      (co == BLACK)     os << " *";
      else if (co == WHITE)     os << " O";
      else if (co == EMPTY)     os << " -";
      else if (co == NEARBLACK) os << " b";
      else if (co == NEARWHITE) os << " w";
      else if (co == BLOCKED)   os << " +";
      else                      os << "  ";
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



#if 0 // test game-specific

void main2()
{

/*    ofstream ofs( "test" );
    ofs << "hello" << endl;
    ofs << "hello" << endl;

    cout << "hello" << endl;
    cout << "hello" << endl;
*/
    Snort* s = new Snort;
    assert(s);
    srand(time(0));
   
    //for(int i=0;;++i)
    {
    s->init_pos(15,3);
    s->write_pos_txt(cout);
    
        while (! s->game_finished())
        {
            if (s->color_to_move() == Color::BLACK)
                s->make_good_move(); // make_random_move
            else
                s->make_good_move();
            //s->write_pos_txt(cout);
        }
    
    //cout << "\ngame " << i << " over, result = " << s->blacks_result();
    }
    /*
    s->write_pos_txt(cout);
    cout << "\ntoPlay: " << s->color_to_move();
    s->do_move("B3");
    s->write_pos_txt(cout);
    s->do_move("A5");
    s->write_pos_txt(cout);
    s->do_move("B6");
    s->write_pos_txt(cout);
    */
/**/
//    exit(1);
}
#endif
