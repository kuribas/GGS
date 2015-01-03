// $Id: Amazons.C 160 2007-06-22 15:21:10Z mburo $
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

#include "Amazons.H"

using namespace std;

bool Amazons::read_pos_ggf(istream &is) { return pos.read_ggf(is); }

void Amazons::write_pos_ggf(ostream &os, bool one_line) const {
  pos.write_ggf(os, one_line);
}

void Amazons::write_pos_txt(ostream &os) const { pos.write(os); }

void Amazons::init_pos(sint4 board_type, sint4 rand_type) {
  pos.init(board_type, rand_type);
}

RegularBoardGame::TURN Amazons::color_to_move() const
{
  return RegularBoardGame::TURN(pos.get_to_move());
}

bool Amazons::legal_move(ostream &err, const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs)) { err << "corrupt move"; return false; }

  Board q = pos;
  
  bool ok = q.make_move(mv);
  if (!ok) err << "illegal move " + mvs;
  return ok;
}

void Amazons::do_move(const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs) || !pos.make_move(mv)) ERR("corrupt move");
}


bool Amazons::game_finished() const { return pos.is_finished(); }

sint4 Amazons::blacks_result( bool ) const { return pos.blacks_result(); }

sint4 Amazons::max_result() const { return BoardType::board_squares(pos.get_type()); }


void Amazons::server_comment(string &msg) { pos.comment(msg); }


// [-oo,+oo] -> [0,1]
// +1 -> 0.8
 
real8 RegularBoardGame::score(real8 result) {

  const real8 K = 0.75;

  result *= 2;
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

RegularBoardGame *RegularBoardGame::new_game() { return new Amazons(); }



const int RegularBoardGame::RESULT_INCREMENT = 1;

const bool   RegularBoardGame::HAS_RAND      = true;
const bool   RegularBoardGame::HAS_RAND_TYPE = false; 
const bool   RegularBoardGame::HAS_KOMI      = true;      
const bool   RegularBoardGame::HAS_ANTI      = false;      
const bool   RegularBoardGame::HAS_SYNCHRO   = true;   

const char * const RegularBoardGame::GAME_NAME     = "Amazons";

const char * const RegularBoardGame::LOGIN_SERVICE = "/ams";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " a.m.a.z.o.n.s ";

const char * const RegularBoardGame::DEFAULT_CLOCK = "15:00/0/0";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";

void RegularBoardGame::gdb() {}

