// $Id: Chess.H 160 2007-06-22 15:21:10Z mburo $
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

#ifndef Chess_H
#define Chess_H

#include "RegularBoardGame.H"
#include "ChessImpl.H"
#include <vector>

class Chess : public RegularBoardGame {

private:

  enum { DRAW_IF_50_REVERSIBLE_MOVES = 1 };
  enum { DRAW_IF_POSITION_REPEATED   = 1 };
  
  Board pos;
  std::vector<uint4> check_sums;
  sint4 start_color;
  
  void init_check_sums();
  bool draw_by_repetition() const;
  bool draw_by_lack_of_progress() const;
  
public:

  void init_pos(sint4 board_type, sint4 rand_type);
  bool read_pos_ggf(std::istream &is);
  void write_pos_ggf(std::ostream &os, bool one_line=true) const;
  void write_pos_txt(std::ostream &os) const;
  bool legal_move(std::ostream &err, const std::string &move_str);
  void do_move(const std::string &move_str);
  TURN color_to_move() const;
  bool game_finished() const;
  sint4 blacks_result( bool Anti ) const;
  sint4 max_result() const;  

  ~Chess() {}
};

#endif
