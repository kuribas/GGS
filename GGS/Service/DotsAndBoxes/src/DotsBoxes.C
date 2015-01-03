// $Id: DotsBoxes.C 160 2007-06-22 15:21:10Z mburo $
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

#include "DotsBoxes.H"

using namespace std;

bool DotsBoxes::read_pos_ggf(istream &is) { return pos.read_ggf(is); }

void DotsBoxes::write_pos_ggf(ostream &os, bool one_line) const {
  pos.write_ggf(os, one_line);
}

void DotsBoxes::write_pos_txt(ostream &os) const { pos.write(os); }

void DotsBoxes::init_pos(sint4 board_type, sint4 rand_type) {
  pos.init(board_type, rand_type);
}

RegularBoardGame::TURN DotsBoxes::color_to_move() const
{
  return RegularBoardGame::TURN( pos.get_to_move() );
}

bool DotsBoxes::legal_move(ostream &err, const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs)) { err << "corrupt move"; return false; }

  Board q = pos;
  
  bool ok = q.make_move(mv);
  if (!ok) err << "illegal move";
  return ok;
}

void DotsBoxes::do_move(const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs) || !pos.make_move(mv)) ERR("corrupt move");
}

bool DotsBoxes::game_finished() const { return pos.is_finished(); }

sint4 DotsBoxes::blacks_result(bool) const { return pos.blacks_result(); }

sint4 DotsBoxes::max_result() const {
  sint4 w = BoardType::board_width(pos.get_type());
  return (w-1)*(w-1);
}


// static functions

real8 RegularBoardGame::score(real8 result) {

  const real8 K = 0.75;
  
  real8 a = fabs(result);
  
  return 0.5 + result * K * 0.5 / ( 1.0 + a * K  );
}

bool RegularBoardGame::read_board_type(istream &is, sint4 &bt) {
  is >> bt;
  if (!is) return false;
  return BoardType::board_width(bt) > 0;
}

bool RegularBoardGame::read_rand_type(istream &is, sint4 bt, sint4 &rt) {
  is >> rt;
  if (!is) return false;
  return BoardType::legal_rand_type(bt, rt);
}



RegularBoardGame *RegularBoardGame::new_game() { return new DotsBoxes(); }

const int RegularBoardGame::RESULT_INCREMENT = 2;

const bool   RegularBoardGame::HAS_RAND      = true;
const bool   RegularBoardGame::HAS_RAND_TYPE = true; 
const bool   RegularBoardGame::HAS_KOMI      = true;      
const bool   RegularBoardGame::HAS_ANTI      = false;      
const bool   RegularBoardGame::HAS_SYNCHRO   = true;   

const char * const RegularBoardGame::GAME_NAME = "DotsAndBoxes";

const char * const RegularBoardGame::LOGIN_SERVICE = "/db";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " d.o.t.s ";

const char * const RegularBoardGame::DEFAULT_CLOCK = "15:00//0:00";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";
 
void RegularBoardGame::gdb() {}
