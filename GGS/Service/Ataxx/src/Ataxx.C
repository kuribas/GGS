// $Id: Ataxx.C 160 2007-06-22 15:21:10Z mburo $
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

#include "Ataxx.H"

const int MAX_RESULT = 1000;

using namespace std;

void Ataxx::init_check_sums() {
  check_sums.erase(check_sums.begin(), check_sums.end());
  check_sums.push_back(pos.adler32());
  start_color = pos.get_to_move();
}

bool Ataxx::read_pos_ggf(istream &is) {
  bool ok = pos.read_ggf(is);
  if (!ok) return false;
  init_check_sums();
  return true;
}

void Ataxx::write_pos_ggf(ostream &os, bool one_line) const {
  pos.write_ggf(os, one_line);
}

void Ataxx::write_pos_txt(ostream &os) const { pos.write(os); }

void Ataxx::init_pos(sint4 board_type, sint4 rand_type) {
  pos.init(board_type, rand_type);
  init_check_sums();
}

RegularBoardGame::TURN Ataxx::color_to_move() const
{
  return RegularBoardGame::TURN( pos.get_to_move() );
}

bool Ataxx::legal_move(ostream &err, const string &mvs) {

  Move mv;
  if (!mv.parse(err, mvs)) return false;

  bool ok = pos.legal_move(mv);
  if (!ok) { err << "illegal move"; }
  return ok;
}


void Ataxx::do_move(const string &mvs) {

  Move mv;
  ostringstream os;

  if (!mv.parse(os, mvs)) ERR("illegal move in do_move");
  pos.play_move(mv);
  check_sums.push_back(pos.adler32());
}


sint4 Ataxx::blacks_result( bool ) const {

  assert(game_finished());

  return pos.blacks_result();
}

sint4 Ataxx::max_result() const {
  return BoardType::board_squares(pos.get_type());
}


bool Ataxx::end_by_lack_of_progress() const {

  if (END_IF_N_REVERSIBLE_MOVES <= 0) return false;
  return pos.get_rev_move_num() >= END_IF_N_REVERSIBLE_MOVES;
}


bool Ataxx::end_by_repetition() const {

  if (END_IF_POSITION_REPEATED_N_TIMES <= 0) return false;

  sint4 n = check_sums.size();
  if (n < 2) return false;
  uint4 last = check_sums.back();

  sint4 reps = 0;

  // errstr << "LAST= " << last << endl;
  
  for (sint4 i=n-3; i >= 0; i -= 2) {
    // errstr << "PREV= " << check_sums[i] << endl;    
    if (check_sums[i] == last) {
      ++reps;
      if (reps >= END_IF_POSITION_REPEATED_N_TIMES) return true;
    }
  }

  return false;
}


bool Ataxx::game_finished() const 
{
  //errstr << "!prog= " << draw_by_lack_of_progress() << endl;
  //errstr << "pos.finished= " << pos.is_finished() << endl;

  return
    end_by_lack_of_progress() ||
    end_by_repetition()       ||
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

RegularBoardGame *RegularBoardGame::new_game() { return new Ataxx(); }

const int RegularBoardGame::RESULT_INCREMENT = 2;

const bool   RegularBoardGame::HAS_RAND      = true;
const bool   RegularBoardGame::HAS_RAND_TYPE = true; 
const bool   RegularBoardGame::HAS_KOMI      = true;      
const bool   RegularBoardGame::HAS_ANTI      = true;      
const bool   RegularBoardGame::HAS_SYNCHRO   = true;   

const char * const RegularBoardGame::GAME_NAME = "Ataxx";

const char * const RegularBoardGame::LOGIN_SERVICE = "/ax";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " a.t.a.x.x ";

const char * const RegularBoardGame::DEFAULT_CLOCK = "0:30:00/0:00/0:00";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";

void RegularBoardGame::gdb() {}
