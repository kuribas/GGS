// $Id: Phutball.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Warren D. Smith, Igor Durdanovic, igord@research.nj.nec.com
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
//: Phutball.C (bof) (c) Igor Durdanovic, Warren D. Smith

#include "Phutball.H"
#include <String.H>
#include <iomanip>
#include <cstdlib>
#include <ctype.h>

void RegularBoardGame::gdb() {}

RegularBoardGame* RegularBoardGame::new_game() { return new Phutball; }

const bool RegularBoardGame::HAS_RAND         = true; 
const bool RegularBoardGame::HAS_RAND_TYPE    = true; 
const bool RegularBoardGame::HAS_KOMI         = false;
const bool RegularBoardGame::HAS_ANTI         = false;
const bool RegularBoardGame::HAS_SYNCHRO      = true;
const int  RegularBoardGame::RESULT_INCREMENT = 1;

const char* const RegularBoardGame::GAME_NAME     = "Phutball";
const char* const RegularBoardGame::LOGIN_SERVICE = "/phb";
const char* const RegularBoardGame::LOGIN_SYSTEM  = " p.h.u.t.b.a.l.l ";

const char* const RegularBoardGame::DEFAULT_CLOCK = "15:00//"; // ???

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";


// UP moves first
int Phutball::blacks_result( bool ) const {
  if (mode == black_WON) return  1;
  if (mode == white_WON) return -1;
  return 0; // should not happen
}

double RegularBoardGame::score( double r )
{
  return r;
  //    (0.5 * RegularBoardGame::blacks_result()) / 
  //     RegularBoardGame::max_result() + 0.5;
}

bool RegularBoardGame::read_board_type( istream &is, int &board_type )
{
  int H, W;
  is >> H; if ( is.fail() ) return false;
  char c; is.read( &c, sizeof(c) );
  if ( is.fail() || c != 'x' ) { W = H; if ( ! is.fail() ) is.putback( c ); }
  else { is >> W; if ( is.fail() ) return false; }

  // for now, am requiring H,W to both be odd.
  // maybe later can permit even H and/or even W.
  if( H < 3 || H > Phutball::max_h ||
      W < 3 || W > Phutball::max_w || 
      (H&1)==0 || (W&1)==0 ) return false;
  
  board_type = H * 100 + W;

  return true;
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

// no need for HASH

void Phutball::init_size( int H, int W )
{
  // note H,W must be odd as matters stand now
  assert( H >= 3 && H <= max_h &&
          W >= 3 && W <= max_w && 
          (H&1)!=0 && (W&1)!=0 ); 

  width  = W+2; // note extra dummies
  height = H+2;

  turn = UP;
  
  dir8[0] = -1;        // left
  dir8[1] = width;     // up
  dir8[2] =  1;        // right
  dir8[3] = -dir8[1];  // down

  dir8[4] = dir8[0]+dir8[1]; //left-up
  dir8[5] = dir8[2]+dir8[1]; //right-up
  dir8[6] = dir8[0]+dir8[3]; //left-down
  dir8[7] = dir8[2]+dir8[3]; //right-down

  board.erase( board.begin(), board.end() );
  
  board.reserve( height * width );

  for ( int x = 0; x < width; ++x ) board.push_back( LO_FRAME );
  for ( int y = 0; y < H; ++y ) {
    board.push_back( SIDE_FRAME );
    for ( int x = 0; x < W; ++x ) board.push_back( EMPTY );
    board.push_back( SIDE_FRAME );
  }
  for ( int x = 0; x < width; ++x ) board.push_back( UP_FRAME );
}

void Phutball::init_pos( int board_type, int rand_type )
{
  mode = PLAYING;
  
  int H = board_type / 100;
  int W = board_type % 100;

  init_size( H, W );
  int x = (width-1)/2;
  int y = (height-1)/2;
  int p;

  ballX = x; ballY = y;
  int centerpoint = y*width+x;
  board[centerpoint] = BALL;
  for ( int i = 0; i < rand_type; ++i ) {
    y = rand() % H + 1;
    x = rand() % W + 1;
    p = y * width + x;
    if ( board[p] != EMPTY ) { --i; continue; }
    board[p] = STONE;
  }
}

char Phutball::ccolor( COLOR c )
{
  switch ( c ) {
  case EMPTY        : return '.';
  case STONE        : return 'X';
  case BALL         : return 'O';
  case MARKED_STONE : return 'x'; //should not happen
  case SIDE_FRAME   : return '|';
  case UP_FRAME     : return 'U';
  case LO_FRAME     : return 'L';
  default    : abort();
  }
  return '?';
}

Phutball::COLOR Phutball::color( char c )
{
  switch ( c ) {
  case '.' : return EMPTY;
  case 'X' : return STONE;
  case 'x' : return MARKED_STONE; //should not happen
  case 'O' : return BALL;
  case '|' : return SIDE_FRAME;
  case 'U' : return UP_FRAME;
  case 'L' : return LO_FRAME;
  default  : abort(); 
  }
  return EMPTY;
}

void Phutball::write_pos_txt( ostream& os ) const
{
  switch ( turn ) {
  case UP   : os << "-> U" << EOL; break;
  case DOWN : os << "-> D" << EOL; break;
  default    : abort();
  }

  os << "  "; for ( int x = 0; x < width-2; ++x ) os << ' ' << char( 'A'+x ); os << EOL;
  for ( int y = 1; y < height-1; ++y ) {
    os << setw(2) << height-1-y;
    for ( int x = 1; x < width-1; ++x ) {
      const_iterator p = board.begin() + (height-1-y) * width + x;
      os << ' ' << ccolor( *p );
    }
    os << ' ' << setw(-2) << height-1-y << EOL;
  }
  os << "  "; for ( int x = 0; x < width-2; ++x ) os << ' ' << char( 'A'+x ); os << EOL;
}

void Phutball::write_pos_ggf(ostream &os, bool one_line ) const
{
  int H = height - 2;
  int W = width - 2;
  os << ( H == W ? H : H * 100 + W );
  os << ( one_line ? ' ' : EOL );
  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      os << ccolor( board[ y * width + x ] );
    }
    os << ( one_line ? ' ' : EOL );
  }
  os << char( turn );
}

bool Phutball::read_pos_ggf( istream &is )
{
  mode = PLAYING;
  
  int H; is >> H; if ( is.fail() ) return false;
  int W = H; if ( W > 100 ) { H /= 100; W %= 100; }
  
  init_size( H, W );

  char c;

  is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL

  COLOR thing;  
  for ( int y = 1; y < height-1; ++y ) {
    for ( int x = 1; x < width-1; ++x ) {
      is.read( &c, sizeof(c) ); if ( is.fail() ) return false;
      thing = color( c );
      if(thing==BALL){ ballX = x; ballY = y; }
      board[ y * width + x ] = thing;
    }
    is.read( &c, sizeof(c) ); if ( is.fail() ) return false; // EOL
  }

  is.read( &c, sizeof(c) ); if ( is.fail() ) return false;
  /**/ if ( c == 'U' ) turn = UP;
  else if ( c == 'L' ) turn = DOWN;
  else return false;

  return true;
}

RegularBoardGame::TURN Phutball::color_to_move() const
{
  switch ( turn ) {
  case UP   : return RegularBoardGame::BLACK;
  case DOWN : return RegularBoardGame::WHITE;
  default   : abort();
  }
}

int Phutball::symmetry( int s, int y, int x ) const
{
  switch ( s ) {
  case 1 : return y                  * width + ( width - 1 - x );
  case 2 : return ( height - 1 - y ) * width + x;
  case 3 : return ( height - 1 - y ) * width + ( width - 1 - x );
  }
  return y * width + x;
}


int max(int a, int b)
{
  if(a<b) return b;
  return a;
}

int abs(int a)
{
  if(a<0) return -a;
  return a;
}

int Phutball::dir_ident( int x, int y, int x2, int y2 )
{
  int s=0;
  if(x2>x){
    s += 1; // right
  }else if(x2<x){
    s -= 1; // left
  }
  if(y2>y){
    s += width; // up
  }else if(y2<y){
    s -= width; // down
  }
  return s;
}

int Phutball::win_dir_ident( int loc )
{
  int s,loc2,ud;
  COLOR goal;
  ud = width; goal = UP_FRAME;
  if(turn == DOWN){ ud = -width; goal = LO_FRAME; }
  
  for(s = ud-1; s <= ud+1; s++){
    for(loc2 = loc+s; board[loc2] == STONE ; loc2 += s){
      if( board[loc2+s] == goal ) return s;
    }
  }
  return 0; // no valid winning jump direction found
}

void Phutball::unmark_stones()
{
   for(int i=height*width-1; i>0; i--){
      if( board[i] == MARKED_STONE ) board[i] = STONE;
   }
}

bool Phutball::parse_move( ostream& err, const string& inp, MOVE& move )
{
  if ( inp.empty() ) {
    err << "ERR: empty move string.";
    return false;
  }

  if ( inp.size() == 1 && inp[0] == '*' ) {
    move.i = -1;
    move.y = -1;
    move.x = -1;
    return true;
  }
  
  unsigned int i = 0;
  move.x = toupper(inp[i]) - 'A' + 1;

  if ( move.x < 1 || move.x > width -2 ) {
    err << "ERR: x coordinate not within board range.";
    return false;
  }

  move.y = 0;
  for ( ; ++i < inp.size(); ) {
    if (! isdigit(inp[i]) ) {
      err << "ERR: non-digit in y move coord " << inp << " .";
      return false;
    }
    move.y = move.y*10 + (inp[i] - '0');
  }

  if ( move.y < 1 || move.y > height-2 ) {
    err << "ERR: y coordinate not within board range.";
    return false;
  }

  move.i = move.y * width + move.x;

  return true;
}

bool Phutball::parse_moves( ostream& err, const string& inp, vector<MOVE>& moves )
{
  moves.erase( moves.begin(), moves.end() );
  vector< String > Moves;

  String Inp(inp);
  String::parse( Inp, Moves, '-' );

  vector< String >::iterator it = Moves.begin();  
  vector< String >::iterator hi = Moves.end();
  for ( ; it != hi ; ++it ) {
    MOVE move;
    bool ok = parse_move( err, *it, move );
    if (! ok ) return false;
    moves.push_back( move );
  }

  if ( moves.empty() ) {
    err << "ERR: no move coords?!";
    return false;
  }

  if ( moves[0].i < 0 ) {
    err << "ERR: * can not be the first move coord.";
    return false;
  }

  for ( unsigned int i = 0; i < moves.size() - 1; ++i ) {
    if ( moves[i].i < 0 ) {
      err << "ERR: * can be specified only as the last move coord.";
      return false;
    }
  }

  return true;
}

bool Phutball::legal_move( ostream& err, const string& s )
{
  if ( ballY <= 1 || ballY >= height-2 ) { err << "ERR: game is finished?"; return false; }
  
  vector<MOVE> moves;
  bool ok = parse_moves( err, s, moves );
  if (! ok ) return false;

  int i = 0;
  
  if ( board[moves[i].i] == STONE || board[moves[i].i] == MARKED_STONE  ) {
    err << "ERR: this point is occupied by a stone.";
    return false;
  }

  if ( board[moves[i].i] == EMPTY  ) {
    if ( moves.size() != 1 ) {
      err << "ERR: too many move coords.";
      return false;
    }
    return true;
  }

  if ( board[moves[i].i] == BALL  ) {
    if ( moves.size() < 2 ){ 
      err << "ERR: not enough move coords for a jump move."; 
      return false; 
    }

    int len = moves.back().i < 0 ? moves.size() - 1 : moves.size();
    for ( int i = 1; i < len; ++i ) {
      if(  board[moves[i].i] != EMPTY && board[moves[i].i] != BALL ) {
	err << "ERR: tried to jump to non-empty vertex.";
	unmark_stones();
	return false;
      }
      int di = dir_ident( moves[i-1].x, moves[i-1].y, moves[i].x, moves[i].y );
      int Linfdist = moves[i].max_dist( moves[i-1] );
      if (Linfdist <= 1){
	err << "ERR: tried to jump 1 step (too short).";
	unmark_stones();
	return false;
      }
      for ( int j=1; j < Linfdist; j++){
	if( board[moves[i-1].i + j*di ] != STONE ){
	  err << "ERR: tried to jump over non-stone.";
	  unmark_stones();
	  return false;
	}
	board[moves[i-1].i+j*di] = MARKED_STONE;
      }
      if( moves[i-1].i+Linfdist*di != moves[i].i){
	err << "ERR: jumploc bogus.";
	unmark_stones();
	return false;
      }
    }
    if ( moves.back().i < 0 ) {
      int di = win_dir_ident( moves[moves.size()-2].i );
      unmark_stones(); 
      if (di==0){
	err << "ERR: said -* but no winning jump over goal line exists.";
	unmark_stones();
	return false;
      }
      return (di!=0);
    }
    if( (turn == UP   && moves.back().y == 1) ||
	(turn == DOWN && moves.back().y == height-2) ) {
      err << "ERR: not allowed to suicidally end jump on goal line.";
      unmark_stones();
      return false;
    }

    unmark_stones();
    return true;
  }
  assert( "can not come here" == 0 );
  return true;
}

void Phutball::do_move( const string& s )
{
  vector<MOVE> moves;
  bool ok = parse_moves( vc_con, s, moves );
  assert( ok );

  int i = 0;
  
  if ( board[moves[i].i] == EMPTY  ) {
    board[moves[i].i] = STONE;
    turn = PLAYER(UP + DOWN - turn);
    return;
  }

  if ( board[moves[i].i] == BALL  ) {
    int len = moves.back().i < 0 ? moves.size() - 1 : moves.size();
    if ( moves.back().i < 0 ) {
      mode = (turn == UP) ? black_WON : white_WON;
      int di = win_dir_ident( moves[moves.size()-2].i );
      board[moves[moves.size()-2].i] = BALL;
      for ( int j=1; ; j++){
	int loc = moves[moves.size()-2].i+j*di;
	if ( board[ loc ] != STONE ) break;
	board[ loc ] = BALL;
      }
    } else {
      if ( turn == DOWN  && moves.back().y == 1        ) mode = white_WON;
      if ( turn == UP    && moves.back().y == height-2 ) mode = black_WON;
      if ( mode != PLAYING ) board[moves.back().i] = BALL;
    }
    for ( int i = 1; i < len; ++i ) {
      int di = dir_ident( moves[i-1].x, moves[i-1].y, moves[i].x, moves[i].y );
      int Linfdist = moves[i].max_dist( moves[i-1] );
      if ( mode != PLAYING ) board[moves[i-1].i] = BALL;
      for ( int j=1; j < Linfdist; j++){
	board[moves[i-1].i+j*di] = ( mode == PLAYING ) ? EMPTY : BALL;
      }
    }
    if ( mode == PLAYING ) {
      board[moves[0].i    ] = EMPTY;
      board[moves.back().i] = BALL;
    }
  }
  turn = PLAYER(UP + DOWN - turn);
}

//: Phutball.C (eof) (c) Igor Durdanovic, Warren D. Smith
