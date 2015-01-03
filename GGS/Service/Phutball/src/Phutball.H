// $Id: Phutball.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, Waaren D. SMith igord@research.nj.nec.com
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
//: Phutball.H (bof) (c) Igor Durdanovic, Warren D. Smith

#ifndef Phutball_H
#define Phutball_H

#include <iostream>
#include <string>
#include <vector>
#include "RegularBoardGame.H"
#include "SET.H"

using namespace::std;

class Phutball
  : public RegularBoardGame
{
public:
  // init POS w.r.t. board_type and rand_type (<0 => no rand)
  virtual void init_pos(int board_type, int rand_type) ;

  // init POS from ggf representation
  virtual bool read_pos_ggf(istream &is) ;

  // color to move in POS (BLACK | WHITE)
  virtual TURN color_to_move() const ;

  // write POS in ggf format, use only one line if flag is set
  virtual void write_pos_ggf(ostream &os, bool one_line=true) const ;

  // write POS in (verbose) text format
  virtual void write_pos_txt(ostream &os) const ;

  // false iff move_str is illegal in POS (write error message to err),
  virtual bool legal_move(ostream &err, const string &move_str) ;

  // make legal move in POS
  virtual void do_move(const string &move_str) ;

  // true iff game is finished
  virtual bool game_finished() const { return mode != PLAYING; }

  // game result for black 
  // GGS idiocy insists on calling players BLACK & WHITE which Phutball 
  // players call UP & DOWN respectively
  virtual int blacks_result( bool Anti ) const;

  // maximum result in position POS
  virtual int max_result() const { return 1; }

  virtual ~Phutball() {}

  enum BOARDSIZEPARAM { max_h = 25, max_w = 25 }; //??? 26x26 max

private:
  enum COLOR { EMPTY = 0, STONE = 1, BALL = 2, MARKED_STONE = 3,
     UP_FRAME = 4, LO_FRAME = 5, SIDE_FRAME = 6  };

  enum PLAYER { UP = 'U', DOWN = 'L' };
  
  vector< COLOR > board;
  typedef vector< COLOR >::      iterator       iterator;
  typedef vector< COLOR >::const_iterator const_iterator;

  struct MOVE {
    int i;
    int y;
    int x;
    int max_dist( const MOVE& m ) { return max( abs(x-m.x), abs(y-m.y) ); }
  };
  bool parse_move( ostream& err, const string& inp, MOVE& move );
  bool parse_moves( ostream& err, const string& inp, vector<MOVE>& moves );

  enum MODE2 { PLAYING, black_WON, white_WON };

  MODE2  mode;
  int    width, height;
  int    ballY, ballX;
  PLAYER turn;
  int    score;
  int    dir8[8];
  
  void init_size( int board_type, int rand_type ); //???
  int dir_ident( int x, int y, int x2, int y2 );
  int win_dir_ident( int loc );
  void unmark_stones();
  
  int symmetry( int s, int y, int x ) const;

  static char  ccolor( COLOR c );
  static COLOR  color( char  c );
};

#endif

//: Phutball.H (eof) (c) Igor Durdanovic, Warren D. Smith
