/*
    (c) Michael Buro, mic@research.nj.nec.com
    NEC Research Institute
    4 Independence Way
    Princeton, NJ 08540, USA

    (c) Shogivar extensions by Kristof Bastiaensen, kristof.bastiaensen@vleeuwen.org

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

#include "Shogi.H"
#include "String.H"

#define MAX_VARIANT 10
const int MAX_RESULT = 1;

using namespace std;

bool Shogi::read_pos_ggf(istream &is)
{
   sint4 type;
   if (!RegularBoardGame::read_board_type(is, type))
      return false;

   if(pos) { delete pos; pos = NULL; }

   pos = Board::new_board(type);
   if (!pos) return false;

   if (pos->read_pos(is))
      return true;

   delete pos;
   pos = NULL;
   return false;
}

void Shogi::write_pos_ggf(ostream &os, bool one_line) const {
   os << pos->get_type()->num;
   os << (one_line ? ' ' : EOL);
   pos->write_ggf(os, one_line);
}

void Shogi::write_pos_txt(ostream &os) const {
   os << "Variant: " << pos->get_type()->name << EOL;
   pos->write(os); }

void Shogi::init_pos(sint4 board_type, sint4 rand_type) {
   if(pos) delete pos;
   pos = Board::new_board(board_type);
   pos->init_setup();
}

RegularBoardGame::TURN Shogi::color_to_move() const
{
   return RegularBoardGame::TURN( pos->get_to_move() );
}

bool Shogi::legal_move(ostream &err, const string &mvs)
{
   Board::Move mv(pos->get_type());
   if (!mv.parse(mvs)) {
       err << "parse error in move";
       return false; }

   bool ok = pos->one_legal_move(mv);
   if (!ok) { err << "illegal move"; }
   return ok;
}


void Shogi::do_move(const string &mvs)
{
   Board::Move mv(pos->get_type());

   if (!mv.parse(mvs) || !pos->update_move(mv))
      ERR("illegal move in do_move");
   pos->do_move(mv);
}


sint4 Shogi::blacks_result( bool ) const
{
   assert(game_finished());

   return (pos->get_result());
}

sint4 Shogi::max_result() const { return MAX_RESULT; }

bool Shogi::game_finished() const 
{
   return pos->is_finished();
}


real8 RegularBoardGame::score(real8 result)
{
   return result;
}

bool RegularBoardGame::read_rand_type(istream &is, sint4 type, sint4 &bt)
{
   return false;
}

bool RegularBoardGame::read_board_type(istream &is, sint4 &bt)
{
   is >> bt;

   if(bt < 0 || bt > MAX_VARIANT) return false;
   if (!is) return false;

   return true;
}

RegularBoardGame *RegularBoardGame::new_game() { return new Shogi(); }

const int RegularBoardGame::RESULT_INCREMENT = 1;

const bool   RegularBoardGame::HAS_RAND      = false;
const bool   RegularBoardGame::HAS_RAND_TYPE = false; 
const bool   RegularBoardGame::HAS_KOMI      = false;
const bool   RegularBoardGame::HAS_ANTI      = false;
const bool   RegularBoardGame::HAS_SYNCHRO   = true;

const char * const RegularBoardGame::GAME_NAME = "Shogi";

const char * const RegularBoardGame::LOGIN_SERVICE = "/shs";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " s.h.o.g.i ";


//const char * const RegularBoardGame::DEFAULT_CLOCK = "2:00:00,40/0:00/30:00";
//const char * const RegularBoardGame::DEFAULT_CLOCK = "10:00,N2//10:00,N2";
const char * const RegularBoardGame::DEFAULT_CLOCK = "1:00:00/0:00/0:00";

const char * const RegularBoardGame::FIRST_COLOR  = "black";
const char * const RegularBoardGame::SECOND_COLOR = "white";

void RegularBoardGame::gdb() {}
