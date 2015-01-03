// $Id: HexGame.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, Warren D. Smith {igord,wds}@research.nj.nec.com
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
//: HexGame.H (bof) (c) Igor Durdanovic

#ifndef HexGame_H
#define HexGame_H

#include <iostream>
#include <string>
#include <vector>
#include "RegularBoardGame.H"
#include "SET.H"

using namespace::std;

class HexGame
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

  bool game_finished() const { return finished; }

  int  blacks_result( bool /*Anti*/ ) const { return score; }

  // score = +100000*E if win with fraction E, 0<E<1, empty hexes.
  // Best score possible is if size^2 - 2*size + 1 hexes are empty
  // out of size*size total hexes.
  int     max_result() const { 
    return (100000*(1+(size-2)*size))/(size*size);
  }

  enum { min_s = 3, max_s = 26 };
  
  enum COLOR { EMPTY   = 0,
	       EMPTY_1 = 1,
	       EMPTY_2 = 2,
	       BLACK   = 3,
	       BLACK_T = 4, 
	       BLACK_B = 5,
	       WHITE   = 6,	       
	       WHITE_L = 7,
	       WHITE_R = 8 };
  
private:
  vector< COLOR > board;
  
  int   size, nodes;
  COLOR turn;
  bool  finished;
  int   score;
  int   empty_count;
  int   dir6[6];
  //
  
  typedef vector< COLOR >::      iterator       iterator;
  typedef vector< COLOR >::const_iterator const_iterator;

  bool connected_rec( iterator it,           COLOR C, COLOR EC );
  bool connected    ( iterator it, int Step, COLOR C, COLOR EC );
  
  void init_size( int Size );
  void init_size( int board_type, int rand_type );
  void calc_score();
  bool mark_group();
  bool mark_dead( ostream& err, iterator p );
};

#endif

//: HexGame.H (eof) (c) Igor Durdanovic
