// $Id: HexGame.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, igord@research.nj.nec.com
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
//: HexGame.C (bof) (c) Igor Durdanovic

#include "HexGame.H"
#include <iomanip>
#include <cstdlib>
#include <ctype.h>

void RegularBoardGame::gdb() {}

RegularBoardGame* RegularBoardGame::new_game() { return new HexGame; }

const bool RegularBoardGame::HAS_RAND         = true;//added wds
const bool RegularBoardGame::HAS_RAND_TYPE    = true;//added wds
const bool RegularBoardGame::HAS_KOMI         = true;
const bool RegularBoardGame::HAS_ANTI         = false;
const bool RegularBoardGame::HAS_SYNCHRO      = true;
const int  RegularBoardGame::RESULT_INCREMENT = 1;

const char* const RegularBoardGame::GAME_NAME     = "Hex";
const char* const RegularBoardGame::LOGIN_SERVICE = "/hex";
const char* const RegularBoardGame::LOGIN_SYSTEM  = " h.e.x ";

const char* const RegularBoardGame::DEFAULT_CLOCK = "10:00//";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";


double RegularBoardGame::score( double r ) //maps into [0,1]
{
  const double K = 0.75; // +2 -> 0.8, -2 -> 0.2

  double a = fabs(r);
  
  return 0.5 + r * K * 0.5 / ( 1.0 + a * K  );

  //  return  (0.5*blacks_result())/max_result() + 0.5; 
}

bool RegularBoardGame::read_board_type( istream &is, int &board_type )
{
  is >> board_type; if ( is.fail() ) return false;
  
  return ( board_type >= HexGame::min_s && board_type <= HexGame::max_s );
}


bool RegularBoardGame::read_rand_type( istream &is, int board_type, int &rand_type )
{
  int R; is >> R; if ( is.fail() ) return false;

  int H = board_type / 100;
  int W = board_type % 100;
  if ( R > H*W ) return false;

  rand_type = R;
  return true;
}


void HexGame::init_size( int Size )
{
  assert( Size >= min_s && Size <= max_s );

  size = Size;
  nodes = (size+2); nodes *= nodes;
  empty_count = size*size;
  finished = false;
  score = 0;
  turn = BLACK;

  //   y  x
  dir6[0] = -1;     
  dir6[1] =  1;   
  dir6[2] =   size + 2;
  dir6[3] = - size - 2; 
  dir6[4] =   size + 1;
  dir6[5] = - size - 1;
  
  board.erase( board.begin(), board.end() );
  
  board.reserve( nodes );

  board.push_back( EMPTY_1 );
  for ( int x = 0; x < size ; ++x ) board.push_back( BLACK_T );
  board.push_back( EMPTY_2 );
  
  for ( int y = 0; y < size; ++y ) {
    board.push_back( WHITE_L );
    for ( int x = 1; x < size+1; ++x ) board.push_back( EMPTY );
    board.push_back( WHITE_R );
  }
  board.push_back( EMPTY_1 );
  for ( int x = 0; x < size ; ++x ) board.push_back( BLACK_B );
  board.push_back( EMPTY_2 );
}

void HexGame::init_pos( int board_type, int rand_type )
{
  init_size( board_type );

  for ( int i = 0; i < rand_type; ++i ) {
    int y = rand() % size + 1;
    int x = rand() % size + 1;
    int p = y * (size+2) + x;

    if ( board[p] != EMPTY_2 && board[p] != EMPTY_1 ) { --i; continue; }
    board[p] = turn;
    empty_count--;
    turn = ( turn == BLACK ? WHITE : BLACK );
  } 
}

void HexGame::write_pos_txt( ostream& os ) const
{
  switch ( turn ) {
  case BLACK : os << "-> X" << EOL; break;
  case WHITE : os << "-> O" << EOL; break;
  default    : abort();
  }

  os << "    "; for ( int x = 0; x < size; ++x ) os << ' ' << char('A' + x ); os << EOL;
  int  tab   = 1;
  int  y     = 0;
  for ( int i = 0; i < nodes; ++i ) {
    switch( board[i] ) {
    case EMPTY    : os << " ."; break;
    case BLACK    : os << " X"; break;
    case WHITE    : os << " O"; break;
    case BLACK_T  :
    case BLACK_B  : os << " x"; break;
    case EMPTY_1  :
      for ( int j = 0; j < tab; ++j ) os << ' ';
      os << "    "; break;
    case EMPTY_2  : os << "  "; os << EOL; ++tab; break;
    case WHITE_L  :
      for ( int j = 0; j < tab; ++j ) os << ' ';
      os << setw(2) << ++y << " o"; break;
    case WHITE_R  : os << " o " << setw(-2) << y << EOL; ++tab; break;
    default : abort();
    }
  }
  os << "    ";
  for ( int j = 0; j < tab; ++j ) os << ' ';
  for ( int x = 0; x < size; ++x ) os << ' ' << char('A' + x ); os << EOL;
}

void HexGame::write_pos_ggf(ostream &os, bool one_line ) const
{
  os << size;
  os << ( one_line ? ' ' : EOL );

  for ( int y = 0; y < size; ++y ) {
    for ( int x = 0; x < size; ++x ) {
      int i = (y+1)*(size+2)+x+1;
      switch( board[i] ) {
      case EMPTY : os << '.'; break;
      case BLACK : os << 'X'; break;
      case WHITE : os << 'O'; break;
      default : abort();
      }
    }
    os << (one_line ? ' ' : EOL );
  }
  if ( turn == BLACK ) os << 'X';
  else if ( turn == WHITE ) os << 'O';
  else abort();
}

bool HexGame::read_pos_ggf( istream &is )
{
  int S; is >> S; if ( is.fail() ) return false;
  char c; is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL
  
  init_size( S );
  empty_count = 0;
  
  for ( int y = 0; y < size; ++y ) {
    for ( int x = 0; x < size; ++x ) {
      is >> c; if ( is.fail() ) return false;
      int i = (y+1)*(size+2)+x+1;
      switch( c ) {
      case '.' : board[i] = EMPTY;   empty_count++;  break;
      case 'X' : board[i] = BLACK;   break;
      case 'O' : board[i] = WHITE;   break;
      default : abort();
      }
    }
    is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL
  }

  is.read( &c, sizeof(c) ); if ( is.fail() ) return false;
  if ( c == 'X' ) turn = BLACK;
  else if ( c == 'O' ) turn = WHITE;
  else abort();
  
  return true;
}

RegularBoardGame::TURN HexGame::color_to_move() const
{
  switch ( turn ) {
  case BLACK : return RegularBoardGame::BLACK;
  case WHITE : return RegularBoardGame::WHITE;
  default    : abort();
  }
}

struct Undo
{
public:
  void set( vector<HexGame::COLOR>::iterator I, HexGame::COLOR C ) {
    Bak b; b.i = I; b.c = *I; *I = C;
    vec.push_back(b);
  }
  ~Undo() {
    while (! vec.empty() ) {
      *vec.back().i = vec.back().c;
      vec.pop_back();
    }
  }

private:
  struct Bak {
    vector<HexGame::COLOR>::iterator i;
    HexGame::COLOR                   c;
  };
  vector<Bak> vec;
};

bool HexGame::connected_rec( iterator it, COLOR C, COLOR EC )
{
  int  d  = 0;
  Undo undo;
  
  for ( ; d < 6; ++d ) {
    iterator nit = it + dir6[d];
    if ( *nit == EC ) return true;
    if ( *nit !=  C ) continue;
    undo.set( nit, EMPTY );
    assert(*nit == EMPTY); 
    if ( connected_rec( nit, C, EC ) ) return true;
  }
  return false;
}

bool HexGame::connected( iterator it, int Step, COLOR C, COLOR EC )
{
  iterator hi = it + size*Step;
  Undo undo;
  
  for ( ; it != hi; it += Step ) {
    if ( *it != C ) continue;
    undo.set( it, EMPTY );
    assert(*it == EMPTY); 
    if ( connected_rec( it, C, EC ) ) return true;
  }
  return false;
}

bool HexGame::legal_move( ostream& err, const string& s )
{
  int x = toupper(s[0]) - 'A' + 1;
  int y = atoi( s.c_str() + 1 );

  if ( y < 1 || y > size || x < 1 || x > size ) {
    err << "ERR: coordinates not within board range.";
    return false;
  }

  int i = y * (size+2) + x;

  if ( board[i] != EMPTY ) {
    err << "ERR: this point is not empty.";
    return false;
  }

  return true;
}

void HexGame::do_move( const string& s )
{
  int x = toupper(s[0]) - 'A' + 1;
  int y = atoi( s.c_str() + 1 );

  if ( y < 1 || y > size || x < 1 || x > size ) return;

  int i = y * (size+2) + x;

  if ( board[i] != EMPTY ) return;

  board[i] = turn;  empty_count--;

  if ( turn == BLACK ) {
    if ( (finished = connected( board.begin() + size + 3,      1, BLACK, BLACK_B )) )
      // get 100000*E points for win with fraction E of hexes empty:
      score =  (100000 * empty_count) / (size*size) ;
  }
  else {
    if ( (finished = connected( board.begin() + size + 3, size+2, WHITE, WHITE_R )) )
      score = -(100000 * empty_count) / (size*size) ;
  }
    
  turn = ( turn == BLACK ? WHITE : BLACK );
}

//: HexGame.C (eof) (c) Igor Durdanovic
