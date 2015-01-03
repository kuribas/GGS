// $Id: GoGame.H 9037 2010-07-06 04:05:44Z mburo $
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
//: GoGame.H (bof) (c) Igor Durdanovic

#ifndef GoGame_H
#define GoGame_H

#include <iostream>
#include <string>
#include <vector>
#include "RegularBoardGame.H"
#include "SET.H"

using namespace::std;

#define SUPER_KO 1
#define JAPAN_SCORE 1

class GoGame
  : public RegularBoardGame
{
public:
  void init_pos( int board_type, int rand_type );

  void write_pos_txt( ostream &os ) const;
  void write_pos_ggf( ostream &os, bool one_line=false ) const;
  bool  read_pos_ggf( istream &is );

  TURN color_to_move() const;

  bool legal_move( ostream &err, const string &move_str );
  void    do_move(               const string &move_str );

  bool game_finished() const { return state == END; }

  int  blacks_result( bool /*Anti*/ ) const { return score; }
  int     max_result() const { return (width-2)*(height-2); }

  enum SIZE  { max_h = 26, max_w = 26 };
  
private:
  enum COLOR { EMPTY = 0, BLACK = 1, WHITE = 2, FRAME = 4 };
  enum STATE { PLAY, MARK, END };
  
  struct FIELD {
    FIELD( int C ) : flag(false), color(C), group(0), area(EMPTY) {}

    mutable bool flag  :  1; // flag for recursive algos.
    /*   */ int  color :  7; // color of the point
    /*   */ int  group : 16; // group number ( when marking dead groups )
    /*   */ int  area  :  8; // color of the empty point
  };
  struct HASH {
    HASH();

    int operator[]( int i ) const { return c[i]; }
  private:
    int c[3]; // color signature
  };
  
  vector< FIELD > board;
  vector< int   > sig;     // signatures for each position
  SET   < int   > mark[2]; // both players have to mark dead groups 
  
  int   width, height;
  COLOR turn;
  int   live[2]; // scoring
  int   dead[2]; 
  int   area[2];
  int   dir4[4];
  int   seki;
  int   score;
  int   pass;
  STATE state;

  static HASH hash[ max_h * max_w + 1 ];
  
  //
  
  typedef vector< FIELD >::      iterator       iterator;
  typedef vector< FIELD >::const_iterator const_iterator;

  void init_size( int board_type, int rand_type );
  void calc_score();
  bool mark_group();
  bool legal_mark_dead( ostream& err, iterator p );
  void    do_mark_dead(               iterator p );
  
  void markg( iterator p, int g );
  void erase( iterator p, int c );
  bool kill ( iterator p, int c );
  void fillE( iterator p, int c );
  void fill ( iterator p );

  bool is_deadR( iterator p, int c);
  bool is_dead ( iterator p, int c);

  int symmetry( int s, int y, int x ) const;
  int gene_sig( int s ) const;
  int gene_sig() const;

  static char eccolor( COLOR c );
  static char mccolor( COLOR c );
  static char  ccolor( COLOR c );
  static COLOR  color( char  c );
};

#endif

//: GoGame.H (eof) (c) Igor Durdanovic
