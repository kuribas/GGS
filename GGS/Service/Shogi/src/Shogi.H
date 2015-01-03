/*
  (c) Michael Buro, mic@research.nj.nec.com
  NEC Research Institute
  4 Independence Way
  Princeton, NJ 08540, USA

  (c) Shogivar extensions by Kristof Bastiaensen, kristof@vleeuwen.org

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

#ifndef Shogi_H
#define Shogi_H

#include "RegularBoardGame.H"
#include "ShogiImpl.H"
#include <vector>

class Shogi : public RegularBoardGame {

 private:

   Board *pos;
   sint4 start_color;
  
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

   Shogi() { pos = NULL; }
   ~Shogi() {}
};

#endif
