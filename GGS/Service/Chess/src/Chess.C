// $Id: Chess.C 160 2007-06-22 15:21:10Z mburo $
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

#include "Chess.H"

const int MAX_RESULT = 1000;

using namespace std;

void Chess::init_check_sums() {
  check_sums.erase(check_sums.begin(), check_sums.end());
  check_sums.push_back(pos.adler32());
  start_color = pos.get_to_move();
}

bool Chess::read_pos_ggf(istream &is) {
  bool ok = pos.read_ggf(is);
  if (!ok) return false;
  init_check_sums();
  return true;
}

void Chess::write_pos_ggf(ostream &os, bool one_line) const {
  pos.write_ggf(os, one_line);
}

void Chess::write_pos_txt(ostream &os) const { pos.write(os); }

void Chess::init_pos(sint4 board_type, sint4 rand_type) {
  pos.init(board_type, rand_type);
  init_check_sums();
}

RegularBoardGame::TURN Chess::color_to_move() const
{
  return RegularBoardGame::TURN( pos.get_to_move() );
}

bool Chess::legal_move(ostream &err, const string &mvs) {

  Move mv;
  if (!mv.parse(err, mvs)) return false;

  bool ok = pos.legal_move(mv);
  if (!ok) { err << "illegal move"; }
  return ok;
}


void Chess::do_move(const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs)) ERR("illegal move in do_move");
  pos.do_move(mv);
  check_sums.push_back(pos.adler32());
}


sint4 Chess::blacks_result( bool ) const {

  assert(game_finished());
  
  if (pos.is_finished()) {

    sint4 rb = pos.blacks_result();

    if (rb == 0) return 0;

    sint4 move_num = (check_sums.size() - 1)/2;

    //errstr << move_num << " " << check_sums.size() << " " << endl;
    
    if (start_color != pos.get_to_move()) ++move_num;

    //errstr << move_num << endl;
    
    sint4 val = MAX_RESULT - move_num;

    if (rb > 0) return val; else return -val;

  }
  return 0; // draw
}

sint4 Chess::max_result() const { return MAX_RESULT; }



bool Chess::draw_by_lack_of_progress() const {

  if (!DRAW_IF_50_REVERSIBLE_MOVES) return false;
  return pos.get_rev_move_num() >= 50;
}


bool Chess::draw_by_repetition() const {

  if (!DRAW_IF_POSITION_REPEATED) return false;

  sint4 n = check_sums.size();
  if (n < 2) return false;
  uint4 last = check_sums.back();

  sint4 reps = 0;

  // errstr << "LAST= " << last << endl;
  
  for (sint4 i=n-3; i >= 0; i -= 2) {
    // errstr << "PREV= " << check_sums[i] << endl;    
    if (check_sums[i] == last) { ++reps; if (reps >= 3) return true; }
  }

  return false;
}


bool Chess::game_finished() const 
{
  //errstr << "!prog= " << draw_by_lack_of_progress() << endl;
  errstr << "rep= " << draw_by_repetition() << endl;
  //errstr << "pos.finished= " << pos.is_finished() << endl;

  return
    (!pos.sufficient_material(BLACK) && !pos.sufficient_material(WHITE)) ||
    draw_by_lack_of_progress() ||
    draw_by_repetition()       ||
    pos.is_finished();
}


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

RegularBoardGame *RegularBoardGame::new_game() { return new Chess(); }

const int RegularBoardGame::RESULT_INCREMENT = 1;

const bool   RegularBoardGame::HAS_RAND      = true;
const bool   RegularBoardGame::HAS_RAND_TYPE = false; 
const bool   RegularBoardGame::HAS_KOMI      = true;      
const bool   RegularBoardGame::HAS_ANTI      = false;      
const bool   RegularBoardGame::HAS_SYNCHRO   = true;   

const char * const RegularBoardGame::GAME_NAME = "Chess";

const char * const RegularBoardGame::LOGIN_SERVICE = "/cs";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " c.h.e.s.s ";


//const char * const RegularBoardGame::DEFAULT_CLOCK = "2:00:00,40/0:00/30:00";
//const char * const RegularBoardGame::DEFAULT_CLOCK = "10:00,N2//10:00,N2";
const char * const RegularBoardGame::DEFAULT_CLOCK = "1:00:00/0:00/0:00";

const char * const RegularBoardGame::FIRST_COLOR  = "white";
const char * const RegularBoardGame::SECOND_COLOR = "black";

void RegularBoardGame::gdb() {}
