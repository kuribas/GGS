// $Id: MatchType.C 160 2007-06-22 15:21:10Z mburo $
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

#include "MatchType.H"
#include "RegularBoardGame.H"

using namespace std;

bool MatchType::is_synchro_game() const {
  return RegularBoardGame::HAS_SYNCHRO && synchro_game;
}  

bool MatchType::is_rand_game() const {
  return RegularBoardGame::HAS_RAND && rand_type >= 0;
}

bool MatchType::is_anti_game() const {
  return RegularBoardGame::HAS_ANTI && anti_game;
}

bool MatchType::is_komi_game() const {
  return RegularBoardGame::HAS_KOMI && komi_game;
}

bool MatchType::parse(ostream &erros, istream &is)
{
  init();

  char c = is.get();
  if (is.eof()) return true;

  if (toupper(c) == 'S') {
    synchro_game = true;
  } else {
    is.putback(c);
  }

  if (!RegularBoardGame::read_board_type(is, board_type)) {
    erros << "illegal board type";
    return false;
  }

  do {

    c = is.get();
    
    switch (toupper(c)) {

    case 'K':
      
      if (!RegularBoardGame::HAS_KOMI) {
	erros << "komi games not supported";
	return false;
      }
      
      if (pref_color != Color::UNDEF) {
	erros << "no preferred color in komi games"; return false;
      }
      komi_game = true;
      break;

    case 'R':

      if (!RegularBoardGame::HAS_RAND) {
	erros << "rand games not supported";
	return false;
      }
      
      if (pref_color != Color::UNDEF) {
	erros << "no preferred color in rand games"; return false;
      }

      rand_type = 0;
      
      if (RegularBoardGame::HAS_RAND_TYPE) {
	if (!RegularBoardGame::read_rand_type(is, board_type, rand_type)) {
	  erros << "missing or illegal rand-type";
	  return false;
	}
      }
      break;

    case 'A':

      if (!RegularBoardGame::HAS_ANTI) {
	erros << "anti games not supported";
	return false;
      }
      
      anti_game = true;
      break;

    case 'B':
    case 'W':
      if (is_komi_game() || is_rand_game()) {
	erros << "no preferred color in komi or rand games"; return false; 
      }
      if (pref_color != Color::UNDEF) {
	erros << "only one color preference allowed"; return false;
      }

      if (toupper(c) == 'B')
	pref_color = Color::BLACK;
      else
	pref_color = Color::WHITE;
      break;
      
    default:
      break;
    }
  } while (is);

  if (!is.eof() && !isspace(c)) {
    erros << "illegal game type";
    return false;
  }

  if (is_synchro_game() && is_komi_game()) {
    erros << "synchro+komi not supported";
    return false;
  }

  if (is_rand_game() && !is_komi_game() && !is_synchro_game()) {
    erros << "random games must be komi or synchro";
    return false;
  }
  
  return true;
}


ostream &operator<<(ostream &os, const MatchType &gt)
{
  return os << gt.to_string();
}

String MatchType::to_string() const { return to_string(false, 0); } 

String MatchType::to_string_with_komi(real4 komi) const
{
  return to_string(true, komi);
}

String MatchType::to_string(bool with_komi, real4 komi) const
{
  ostringstream os;

  if (RegularBoardGame::HAS_SYNCHRO && is_synchro_game()) { os << "s"; }
  os << board_type;
  if (RegularBoardGame::HAS_KOMI && is_komi_game()) { os << "k"; if (with_komi) Form(os, "%+.2f", komi); }
  if (RegularBoardGame::HAS_RAND && is_rand_game()) {
    os << "r";
    if (RegularBoardGame::HAS_RAND_TYPE)  os << rand_type;
  }
  if (get_pref_color() == Color::BLACK) os << "b";
  if (get_pref_color() == Color::WHITE) os << "w";  
  if (RegularBoardGame::HAS_ANTI && is_anti_game()) os << "a";

  String ret;
  STR2STRING(os, ret);
  return ret;
}


String MatchType::key() const
{
  ostringstream os;

  os << board_type;
  String s(os);

  if (RegularBoardGame::HAS_RAND && is_rand_game()) s += "r";
  if (RegularBoardGame::HAS_ANTI && is_anti_game()) s += "a";
  return s;
}


bool MatchType::normalize_key(String &s)
{
  ostringstream os;
  istringstream is(s);
  sint4 bt;

  if (!RegularBoardGame::read_board_type(is, bt)) return false;

  os << bt;
  char c = 'x';
  bool got_r = false, got_a = false;
  
  while (is) {
    c = is.get();
    if (!is || is.eof()) break;
    
    switch (toupper(c)) {

    case 'R':
      got_r = true;
      break;

    case 'A':
      got_a = true;
      break;

    default:
      return false;
    }
  }

  if (!is.eof() && !isspace(c)) return false;

  if (got_r) os << "r";
  if (got_a) os << "a";

  STR2STRING(os, s);
  return true;
}


bool MatchType::is_equal(const MatchType &t) const
{
  if (synchro_game != t.synchro_game) return false;
  if (!equal_type(t)) return false;
  return pref_color == t.pref_color;
}


bool MatchType::is_matching(const MatchType &t) const
{
  if (!equal_type(t)) return false;
  
  if (pref_color == Color::UNDEF) 
    return t.pref_color == Color::UNDEF;
  
  return Color::opponent(pref_color) == t.pref_color;
}


String MatchType::td_data() const
{
  String s;

  s = to_string() + ' ' + key() + ' ';

  if (!is_synchro_game() && !is_komi_game()) {
    
    s += to_string() + "b ";
    s += string("") + RegularBoardGame::FIRST_COLOR + string(" ")
      + RegularBoardGame::SECOND_COLOR;

  } else {

    s += to_string() + " player1 player2";

  }

  return s;
}
