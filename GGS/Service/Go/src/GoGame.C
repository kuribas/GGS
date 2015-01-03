// $Id: GoGame.C 160 2007-06-22 15:21:10Z mburo $
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
//: GoGame.C (bof) (c) Igor Durdanovic

#include "GoGame.H"
#include <iomanip>
#include <cstdlib>
#include <ctype.h>

void RegularBoardGame::gdb() {}

using namespace std;

RegularBoardGame* RegularBoardGame::new_game() { return new GoGame; }

const bool RegularBoardGame::HAS_RAND         = true;
const bool RegularBoardGame::HAS_RAND_TYPE    = true;
const bool RegularBoardGame::HAS_KOMI         = true;
const bool RegularBoardGame::HAS_ANTI         = false;
const bool RegularBoardGame::HAS_SYNCHRO      = true;
const int  RegularBoardGame::RESULT_INCREMENT = 1;

const char* const RegularBoardGame::GAME_NAME     = "Go";
const char* const RegularBoardGame::LOGIN_SERVICE = "/go";
const char* const RegularBoardGame::LOGIN_SYSTEM  = " g.o ";

const char* const RegularBoardGame::DEFAULT_CLOCK = "15:00//2:00";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";

double RegularBoardGame::score( double r )
{
  const double K = 0.75; // +2 -> 0.8, -2 -> 0.2

  double a = fabs(r);
  
  return 0.5 + r * K * 0.5 / ( 1.0 + a * K  );
}

bool RegularBoardGame::read_board_type( istream &is, int &board_type )
{
  int H, W;
  is >> H; if ( is.fail() ) return false;
  char c; is.read( &c, sizeof(c) );
  if ( is.fail() || c != 'x' ) {
    if ( H > 100 ) {
      W = H % 100;
      H = H / 100;
    } else {
      W = H;
    }
    if ( ! is.fail() ) is.putback( c );
  }
  else { is >> W; if ( is.fail() ) return false; }

  if ( H > W ) { int t = H; H = W; W = t; }

  if ( W > GoGame::max_w ) return false;
  if ( H > GoGame::max_h ) return false;
  
  board_type = H * 100 + W;

  return true;
}

bool RegularBoardGame::read_rand_type( istream &is, int board_type, int &rand_type )
{
  int R; is >> R; if ( is.fail() ) return false;

  int H = board_type / 100;
  int W = board_type % 100;
  if ( R*2 > H*W ) return false;

  rand_type = R;
  return true;
}

GoGame::HASH GoGame::hash[ max_h * max_w + 1 ];

GoGame::HASH::HASH()
{
  for ( int i = 0; i < 3; ++i ) c[i] = random();
}

void GoGame::init_size( int H, int W )
{
  assert( H >= 3 && H <= max_h && W >= 3 && W <= max_w ); 

  width  = W+2;
  height = H+2;

  turn = BLACK;
  seki = score = live[0] = live[1] = dead[0] = dead[1] = area[0] = area[1] = 0;
  
  dir4[0] = -1;        // left
  dir4[1] = width;     // up
  dir4[2] =  1;        // right
  dir4[3] = - dir4[1]; // down
  pass  = 0;
  state = PLAY;
  
  board.erase( board.begin(), board.end() );
  sig  .erase( sig  .begin(), sig  .end() );
  
  board.reserve( height * width );
  sig  .reserve( height * width );

  for ( int x = 0; x < width; ++x ) board.push_back( FRAME );
  for ( int y = 0; y < H; ++y ) {
    board.push_back( FRAME );
    for ( int x = 0; x < W; ++x ) board.push_back( EMPTY );
    board.push_back( FRAME );
  }
  for ( int x = 0; x < width; ++x ) board.push_back( FRAME );
}

void GoGame::init_pos( int board_type, int rand_type )
{
  int H = board_type / 100;
  int W = board_type % 100;

  init_size( H, W );

  for ( int i = 0; i < rand_type; ++i ) {
    int y = rand() % H + 1;
    int x = rand() % W + 1;
    int p = y * width + x;
    if ( board[p].color != EMPTY ) { --i; continue; }
    board[p].color = 1 + (i & 1);
    ++live[ board[p].color - 1];
  }
}

char GoGame::ccolor( COLOR c )
{
  switch ( c ) {
  case EMPTY : return '.';
  case BLACK : return 'X';
  case WHITE : return 'O';
  default    : abort();
  }
  return 0;
}

char GoGame::eccolor( COLOR c )
{
  switch ( c ) {
  case BLACK : return 'x';
  case WHITE : return 'o';
  case EMPTY :
  case (BLACK|WHITE) : return '.';
  default    : abort();
  }
  return 0;
}

char GoGame::mccolor( COLOR c )
{
  switch ( c ) {
  case BLACK : return 'x';
  case WHITE : return 'o';
  default    : abort();
  }
  return 0;
}

GoGame::COLOR GoGame::color( char c )
{
  switch ( c ) {
  case '.' : return EMPTY;
  case 'X' : return BLACK;
  case 'O' : return WHITE;
  default  : abort();
  }
  return FRAME;
}

void GoGame::write_pos_txt( ostream& os ) const
{
  if ( state == END ) {
    Form( os, "T+P(S): X:[%d+%d(%d)=%d(%d)] - O[%d+%d(%d)=%d(%d)] = %d(%d), seki:%d",
	  area[0],dead[1],live[0],area[0]+dead[1],area[0]+live[0],
	  area[1],dead[0],live[1],area[1]+dead[0],area[1]+live[1],
	  area[0]+dead[1]-(area[1]+dead[0]),
	  area[0]+live[0]-(area[1]+live[1]),
	  seki ) << EOL;
  } else {
    os << "prisoners: " << dead[1] << "(X), " << dead[0] << "(0)" << EOL;
  }
  os << EOL;
  os << "  "; for ( int x = 0; x < width-2; ++x ) os << ' ' << char( 'A'+x ); os << EOL;
  for ( int y = 1; y < height-1; ++y ) {
    os << setw(2) << height-1-y;
    for ( int x = 1; x < width-1; ++x ) {
      const_iterator p = board.begin() + (height-1-y) * width + x;
      int group = p->group;
      if ( state == MARK && p->color != EMPTY &&
	   (mark[0]( group ) != 0 || mark[1]( group ) != 0) ) {
	os << ' ' << mccolor( COLOR( p->color ) );
      } else if ( state == END && p->color == EMPTY ) {
	os << ' ' << eccolor( COLOR( p->area ) );
      } else {
	os << ' ' << ccolor( COLOR( p->color ) );
      }
    }
    os << ' ' << setw(-2) << height-1-y << EOL;
  }
  os << "  "; for ( int x = 0; x < width-2; ++x ) os << ' ' << char( 'A'+x ); os << EOL;
  os << EOL;
  switch ( turn ) {
  case BLACK : os << "X "; break;
  case WHITE : os << "O "; break;
  default    : abort();
  }

  switch ( state ) {
  case PLAY : os << "to PLAY"; break;
  case MARK : os << "to MARK"; break;
  case END  : os << "to END"; break;
  default   : abort();
  }
  os << EOL;
}

void GoGame::write_pos_ggf(ostream &os, bool one_line ) const
{
  int H = height - 2;
  int W = width - 2;
  os << ( H == W ? H : H * 100 + W );
  os << ( one_line ? ' ' : EOL );
  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      os << ccolor( COLOR( board[ y * width + x ].color ) );
    }
    os << ( one_line ? ' ' : EOL );
  }
  os << GoGame::ccolor( turn );
}

bool GoGame::read_pos_ggf( istream &is )
{
  int H; is >> H; if ( is.fail() ) return false;
  int W = H; if ( W > 100 ) { H /= 100; W %= 100; }
  
  init_size( H, W );

  char c;

  is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL
  
  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      is.read( &c, sizeof(c) ); if ( is.fail() ) return false;
      board[ y * width + x ].color = color( c );
    }
    is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL
  }

  is.read( &c, sizeof(c) ); if ( is.fail() ) return false;
  turn = color( c );

  return true;
}

RegularBoardGame::TURN GoGame::color_to_move() const
{
  switch ( turn ) {
  case BLACK : return RegularBoardGame::BLACK;
  case WHITE : return RegularBoardGame::WHITE;
  default    : abort();
  }
}

void GoGame::erase( iterator p, int c )
{
  ++dead[c-1]; p->color = EMPTY; p->group = 0;

  for ( int d = 0; d < 4; ++d ){
    iterator q = p + dir4[d];
    if ( q->color == c ) erase( q, c );
  }
}

bool GoGame::kill( iterator p, int c )
{
  bool any = false;

  for ( int d = 0; d < 4; ++d ) {
    iterator q = p + dir4[d];
    if ( q->color == c && is_dead( q, c ) ) {
      erase( q, c );
      any = true;
    }
  }

  return any;
}

bool GoGame::is_deadR( iterator it, int c )
{
  int dirset = 0;
  bool  dead = true;
  
  for ( int d = 0; d < 4; ++d ) { // note what dirs are left
    iterator cit = it + dir4[d];
    if ( cit->flag ) continue;
    if ( cit->color == EMPTY ) { dead = false; goto CLEANUP; }
    if ( cit->color != c ) continue;
    dirset |= (1<<d);
    cit->flag = true;
  }
  
  for ( int d = 0; d < 4; ++d ) { // go in each dir4
    if ( dirset & (1<<d) ) {
      if (! is_deadR( it + dir4[d], c ) ) { // clean up when done
	dead = false;
	goto CLEANUP;
      }
    }
  }

CLEANUP:
  for ( int d = 0; d < 4; ++d ) if ( dirset & (1<<d) ) (it+dir4[d])->flag = false;
  
  return dead;
}

bool GoGame::is_dead( iterator p, int c )
{
  p->flag = true;

  bool b = is_deadR( p, c );

  p->flag = false;

  return b;
}

int GoGame::symmetry( int s, int y, int x ) const
{
  switch ( s ) {
  case 1 : return y                  * width + ( width - 1 - x );
  case 2 : return ( height - 1 - y ) * width + x;
  case 3 : return ( height - 1 - y ) * width + ( width - 1 - x );
    // width == height
  case 4 : return x                  * width + y;
  case 5 : return x                  * width + ( width - 1 - y );
  case 6 : return ( height - 1 - x ) * width + y;
  case 7 : return ( height - 1 - x ) * width + ( width - 1 - y );
  }
  return y * width + x;
}

int GoGame::gene_sig( int s ) const
{
  int u = hash[max_h*max_w][turn];

  for ( int y = 1; y < height-1; ++y )
    for ( int x = 1; x < width-1; ++x ) {
      int i = y * width + x;
      u ^= hash[i][ board[ symmetry(s, y, x) ].color ];
    }

  return u;
}

int GoGame::gene_sig() const
{
  int u = gene_sig( 0 );

  int No = width == height ? 8 : 4;
  for ( int s = 1; s < No; ++s ) {
    int n = gene_sig( s ); if ( u < n ) u = n;
  }

  return u;
}

void GoGame::fillE( iterator p, int c )
{
  if ( p->color != EMPTY  ) return;
  if ( (p->area & c) == c ) return;

  p->area |= c;
  
  for ( int d = 0; d < 4; ++d ) fillE( p + dir4[d], c );
}

void GoGame::fill( iterator p )
{
  if ( p->flag ) return; p->flag = true;
  
  for ( int d = 0; d < 4; ++d ) {
    iterator q = p + dir4[d];
    if ( q->color == EMPTY ) fillE( q, p->color );
    if ( q->color == p->color ) fill( q );
  }
}

void GoGame::calc_score()
{
  SET< int > groups;

  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      iterator p = board.begin() + y * width + x;
      if ( p->color == EMPTY ) continue;
      int g = p->group;
      if ( groups( g ) != 0 ) continue;
      groups += g;
      fill( p );
    }
  }

  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      iterator p = board.begin() + y * width + x;
      if ( p->color == EMPTY ) {
	if ( p->area & BLACK ) {
	  if ( p->area & WHITE ) ++seki; else ++area[0];
	} else if ( p->area & WHITE ) ++area[1]; else ++seki;
      } else {
	++live[ p->color-1 ];
      }
    }
  }

  assert( live[0] + area[0] + live[1] + area[1] + seki == (width-2)*(height-2) );
  
#if JAPAN_SCORE
  score = area[0] + dead[1] - area[1] - dead[0];
#else
  score = area[0] + live[0] - area[1] - live[1];
#endif
}

void GoGame::markg( iterator p, int g )
{
  p->group = g;

  for ( int d = 0; d < 4; ++d ){
    iterator q = p + dir4[d];
    if ( q->color == p->color && q->group == 0 ) markg( q, g );
  }
}

bool GoGame::mark_group()
{
  int grp_idx = 1;
  
  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      int p = y * width + x;
      if ( board[p].color != EMPTY &&
	   board[p].group == 0 ) markg( board.begin() + p, grp_idx++ );
    }
  }

  return grp_idx > 1;
}

bool GoGame::legal_mark_dead( ostream& err, iterator p )
{
  if ( p->color == EMPTY  ) {
    err << "ERR: no group found at this point.";
    return false;
  }

  return true;
}

void GoGame::do_mark_dead( iterator p )
{
  if ( p->color == EMPTY  ) return;

  COLOR opponent = COLOR( BLACK + WHITE - turn );

  int group = p->group;
  
  int* my = mark[ turn - 1 ]( group );

  if ( my == 0 ) { // mot yet marked by me
    int* op = mark[ opponent - 1 ]( group );

    if ( op != 0 ) { // opponnet has already marked it as dead
      erase( p, p->color );
      mark[ opponent-1 ] -= group;
    } else {
      mark[ turn - 1 ] += group;
    }
  } else { // cancel my marking
    mark[ turn - 1 ] -= group;
  }

  turn = opponent;

  pass = 0;
}

bool GoGame::legal_move( ostream& err, const string& s )
{
  if ( state == END ) { err << "ERR: game is finished?"; return false; }
  
  if ( s.size() < 2 ) { err << "ERR: move too short?"; return false; }

  COLOR opponent = COLOR( BLACK + WHITE - turn );

  if ( s == "pa" || s == "PA" ) return true;

  int x = toupper(s[0]) - 'A' + 1;
  int y = atoi( s.c_str() + 1 );
  
  if ( y < 1 || y > height-2 || x < 1 || x > width -2 ) {
    err << "ERR: coordinates(" << s << ") not within board range.";
    return false;
  }

  int i = y * width + x;

  if ( state == MARK ) return legal_mark_dead( err, board.begin() + i );
  
  if ( board[i].color != EMPTY  ) {
    err << "ERR: this(" << s << ") point is not empty.";
    return false;
  }

  COLOR player   = turn;
  
  iterator p = board.begin() + i;

  vector< FIELD > backup( board );
  
  p->color = player;
  int old_dead[2]; old_dead[0] = dead[0]; old_dead[1] = dead[1];
  if (! kill( p, opponent ) && is_dead( p, player ) ) {
    p->color = EMPTY;
    dead[0] = old_dead[0]; dead[1] = old_dead[1];
    err << "ERR: suicide is not allowed";
    return false;
  }

  dead[0] = old_dead[0]; dead[1] = old_dead[1];
  
  turn = opponent;

  int u = gene_sig();

#if SUPER_KO
  for ( int i = sig.size(); --i >= 0; )
    if ( u == sig[i] ) {
#else      
      if ( sig.size() > 0 && u = sig.back() ) {
#endif    
	turn  = player;
	board = backup;
#if SUPER_KO
	err << "ERR: super-ko repetition of pos:" << i;
#else
	err << "ERR: ko repetition of last pos.";
#endif
	return false;
#if SUPER_KO
      }
#else
    }
#endif

  turn  = player;
  board = backup;
  
  return true;
}

void GoGame::do_move( const string& s )
{
  if ( state == END ) return;
  if ( s.size() < 2 ) return;

  COLOR opponent = COLOR( BLACK + WHITE - turn );

  if ( s == "pa" || s == "PA" ) {
    turn = opponent;
    if ( ++pass >= 2 ) {
      switch ( state ) {
      case PLAY : state = MARK; pass = 0; if ( mark_group() ) break;
      case MARK : state = END;  pass = 0; calc_score(); return;
      default   : abort();
      }
    }
    return;
  }

  int x = toupper(s[0]) - 'A' + 1;
  int y = atoi( s.c_str() + 1 );
  if ( y < 1 || y > height-2 || x < 1 || x > width -2 ) return;
  int i = y * width + x;
  if ( state == MARK ) { do_mark_dead( board.begin() + i ); return; }
  if ( board[i].color != EMPTY  ) return;
  COLOR player   = turn;
  iterator p = board.begin() + i;
  vector< FIELD > backup( board );
  p->color = player;
  if (! kill( p, opponent ) && is_dead( p, player ) ) {
    p->color = EMPTY;
    return;
  }

  turn = opponent;

  int u = gene_sig();

#if SUPER_KO
  for ( int i = sig.size(); --i >= 0; )
    if ( u == sig[i] ) {
#else      
      if ( sig.size() > 0 && u = sig.back() ) {
#endif    
	turn  = player;
	board = backup;
	return;
#if SUPER_KO
      }
#else
    }
#endif

  sig.push_back( u );
  pass = 0;
}

//: GoGame.C (eof) (c) Igor Durdanovic
