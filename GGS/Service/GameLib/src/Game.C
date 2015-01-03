// $Id: Game.C 160 2007-06-22 15:21:10Z mburo $
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


#include <cstdio>
#include <vector>
#include <algorithm>
#include "String.H"
#include "Game.H"
#include "Match.H"

#define DBG  0
#define TEST 0

using namespace std;

const sint4 TRUST_PERIOD = 4;    // seconds

const real4 MoveInfo::EVAL_EPS = 0.01; // adjust eval related %.xf!!!

real4 MoveInfo::eval_round(real4 v)
{
  return round(v/EVAL_EPS)*EVAL_EPS;
}

real4 MoveInfo::eval_round_half(real4 v)
{
  return round(v/(0.5*EVAL_EPS))*(0.5*EVAL_EPS);
}

void MoveInfo::write(ostream &os) const {

  os << move_str;
  if (eval != 0 || time >= 5000) {
    os << "/";
    if (eval != 0) Form( os, "%.2f", eval);
    if (time >= 5000) Form( os, "/%.2f", time*1E-6);
  }

}

bool MoveInfo::parse(ostream &err, const String &args) {

  String s, rest, rest2;

  move_str = "";
  eval = 0;
  time = 0;

  String::parse(args, s, rest, '/');
  
  if (s.length() <= 0) {
    err << "illegal move string; must be <move>[/<eval>[/<time>]]";
    return false;
  }

  move_str = s;
  
  if (rest != "") {

    // read eval
    
    String::parse(rest, s, rest2, '/');
    if (! s.empty() ) {
      istringstream is(s);
      is >> eval;
      if (!is) { 
        err << "move eval expected";
        return false;
      }

      if (fabs(eval) >= 1000) {
        err << "move eval out of range";
        return false;
      }

      // round eval to EVAL_EPS
      eval = MoveInfo::eval_round(eval);
    }

    if (rest2 != "") {

      // read time
      
      istringstream is(rest2);
      real4 time_sec;
      is >> time_sec;
      if (!is || time_sec < 0) { 
        err << "move time >= 0 expected";
        return false;
      }

      if (fabs(time_sec) >= 100000) {
        err << "move time out of range";
        return false;
      }

      time = sint8(time_sec * 1000000);
    }
  }
  
  return true;
}


sint4 Game::save_pos_info(String &ggf, String &txt)
{
  // ggf
  
  ostringstream os1;
  rbg->write_pos_ggf(os1);
  STR2STRING(os1, ggf);

  // txt
  
  ostringstream os2;
  rbg->write_pos_txt(os2);
  STR2STRING(os2, txt);

  return rbg->color_to_move();
}


Game::Game(Match &m, sint4 id) : match(m), game_id(id) {
  rbg = RegularBoardGame::new_game();
  reset();
}


// stored game

Game::Game(Match &m, sint4 id, istream &is) : match(m), game_id(id)
{
  rbg = RegularBoardGame::new_game();
  read_ggf(is);
  start_to_move_clock(false);
}



// new game

Game::Game(Match &m, sint4 id, sint4 color0, const String &pos0) :
  match(m), game_id(id)
{
  rbg = RegularBoardGame::new_game();
  reset(); // ???

  istringstream is(pos0);
  if (!rbg->read_pos_ggf(is)) ERR("board corrupt");

  start_pos_color_to_move = save_pos_info(start_pos_ggf, start_pos_txt);
  
  gpinf[0].color = color0;

  if (color0 == Color::UNDEF) gpinf[1].color = color0;
  else 
    gpinf[1].color = (color0 == Color::BLACK) ? Color::WHITE : Color::BLACK;

  if (match.type().is_komi_game()) {

    start_both_clocks(false);
    
  } else {

    start_to_move_clock(false);
  }
}


GAME_Clock &Game::clock(uint4 i) {
  if ((game_id & 1) == 0) return match.clock0(i);
  else                    return match.clock1(i);
}


const GAME_Clock &Game::clock(uint4 i) const {
  if ((game_id & 1) == 0) return match.clock0(i);
  else                    return match.clock1(i);
}


String Game::id() const {
  String s;
  
  if (match.type().is_synchro_game())
    s.form("%s.%d", match.id().c_str(), game_id);
  else
    s = match.id();
  
  return s;
}


String Game::err_string()
{
  return String("error ")+id()+" ";
}

// ggf functions

static bool skip_until(istream &is, char u)
{
  char c;

  do { is >> c; } while (is && c != u);
  return is;
}


static bool read_until(istream &is, char u, String &s)
{
  char c;

  s = "";
  
  do { 
    c = is.get();
    s += c; 
  } while (is && c != u);

  if (is) s.erase(s.length()-1);
  
#if 0  
  // delete trailing ' ' 

  while (i >= 0)
    if (s[i] == ' ') s[i--] = 0; else break;

  // delete preceeding ' ' 

  i = 0;
  while (s[i] == ' ') i++;

  int j = 0;

  while (s[i]) s[j++] = s[i++];
  s[j] = 0;

#if DBG
  strerr << "read: " << s << endl;
#endif

#endif
  
  return is;
}


static bool next_token(istream &is, String &s)
{
  char c;

  // skip white space and ';'

  do { c = is.get(); } while (is && (c == ' ' || c == '\n' || c == ';' || c == '\t'));

  if (!is) return false;
  
  if (c == ')') {  // end of game
    s += c;
    return true;
  }

  is.putback(c);                         

  do { 

    c = is.get();

#if DBG
    errstr << "<" << c << ">" << endl;
#endif
    
    s += toupper(c);
    if (s.size() > 3) return false;
    
  } while (is && c != '[');

  if (is) s.erase(s.length()-1);
  return is;
}


bool Game::read_ggf(istream &is)
{
  char c;
  bool type_read = false;
  
  // search "(;" 

  do {
    if (!skip_until(is, '(')) return false;
    is >> c;
    if (c != ';') is.putback(c);
  } while (c != ';');

  reset();

  gpinf[0].color = Color::BLACK;
  gpinf[1].color = Color::WHITE;

  FOREVER {

    String s;
    float f;

    if (!is) return false;
    if (!next_token(is, s)) {
      ERR("no more tokens\a");
      return false;
    }

#if DBG
    errstr << "token: " << s << endl;
#endif

    if      (s == ")") break; // end of game
    else if (s == "GM" ||     // ZAP
	     s == "US" || 
	     s == "CP" || 
	     s == "COPYRIGHT" ||
	     s == "GN" || 
	     s == "C"  || 
	     s == "BL" || 
	     s == "WL" || 
	     s == "NB" || 
	     s == "NW" || 
	     s == "EV" || 
	     s == "LT") { skip_until(is, ']'); } 

    else if (s == "PB") {           // black name

      if (!read_until(is, ']', s)) {
	ERR("] not found after PB\a");
	return false;
      }

      gpinf[0].name = s;

    } else if (s == "PW") {         // white name

      if (!read_until(is, ']', s)) {
	ERR("] not found after PW\a");
	return false;
      }

      gpinf[1].name = s;

    } else if (s == "RB") {         // black rating

      if (!read_until(is, ']', s)) return false;
      // gpinfo[0].rating = atof(s.c_str());

    } else if (s == "RW") {         // white rating

      if (!read_until(is, ']', s)) return false;
      // gpinfo[1].rating = atof(s.c_str());
      
    } else if (s == "PC") {         // place

      if (!read_until(is, ']', s)) {
        ERR("] not found after PC\a");
        return false;
      }

      // ... place = s;
      
    } else if (s == "DT") {         // date

      if (!read_until(is, ']', s)) {
        ERR("] not found after DT\a");
        return false;
      }

      // ...

    } else if (s == "TY") {         // type

      if (!read_until(is, ']', s)) {
        ERR("] not found after TY\a");
        return false;
      }

#if 0
      // not needed, already stored in match
      istringstream iss(s);
      ostringstream oss;

      if (!match.type().parse(oss, iss)) { ERR("type error"); return false; }
#endif
      
      type_read = true;
      
    } else if (s == "TI") {         // both times

      if (!read_until(is, ']', s)) {
        ERR("no ] after TI\a");
        return false;
      }
      
      // ...

    } else if (s == "TB") {         // b time

      if (!read_until(is, ']', s)) {
        ERR("no ] after TB\a");
        return false;
      }
      
      // ...

    } else if (s == "TW") {         // w time

      if (!read_until(is, ']', s)) {
        ERR("no ] after TW\a");
        return false;
      }
      
      // ...

    } else if (s == "KB") {         // black komi move
      
      if (!read_until(is, ']', s)) {
	ERR("no ] after KB");
	return false;
      }

      ostringstream oss;
      
      if (!gpinf[0].komi_move_info.parse(oss, s)) {
        ERR("KB move corrupt\a");
        return false;
      }

    } else if (s == "KW") {         // white komi move
      
      if (!read_until(is, ']', s)) {
	ERR("no ] after KW");
	return false;
      }
      
      ostringstream oss;

      if (!gpinf[1].komi_move_info.parse(oss, s)) {
        ERR("KW move corrupt\a");
        return false;
      }

    } else if (s == "KM") {         // komi

      if (!read_until(is, ']', s)) return false;
      if (sscanf(s.c_str(), "%f", &f) == 1) {
        komi = f;
	komi_defined = true;
      } else {
        ERR("syntax error: KM\a");
        return false;
      }

    } else if (s == "RE") {         // result

      if (!read_until(is, ']', s)) {
        ERR("no ] after RE\a");
	return false;
      }

      // ...
      
    } else if (s == "BO") {         // start board

      if (!read_until(is, ']', s)) {
        ERR("no ] after BO\a");
	return false;
      }

      istringstream is(s);

      // errstr << "bo=" << s << " " << s.length() << endl;

      if (!rbg->read_pos_ggf(is)) {
	ERR("ggf start_pos corrupt");
	return false;
      }

      start_pos_color_to_move = save_pos_info(start_pos_ggf, start_pos_txt);

      gpinf[0].komi_move_info.set_to_move(start_pos_color_to_move);
      gpinf[1].komi_move_info.set_to_move(start_pos_color_to_move);      
      
    } else if (s == "B" || s == "W") {         // move

      MoveInfo mi;

      if (!read_until(is, ']', s)) {
        ERR("no ] after B/W\a");
	return false;
      }

      ostringstream oss;
      
      if (!mi.parse(oss, s)) {
        ERR("B/W move corrupt 1\a");
        return false;
      }

      if (s == "B")
	mi.set_to_move(Color::BLACK);
      else
	mi.set_to_move(Color::WHITE);
      
      minfos.push_back(mi);

    } else {

      ERR2("unknown GGF command: \a", s);
      return false;
      
    }
  }

  if (!type_read) { ERR("typeless game"); return false; }
  
  if (gpinf[0].name != match.name(0)) {

    // wrong player order
    
    GamePlayerInfo pi;
    pi = gpinf[0]; gpinf[0] = gpinf[1]; gpinf[1] = pi;
  }

  replay();
  return true;
}

#if 0
static String time_convert(time_t seconds)
{
  String s;
  ostringstream oss;

  struct tm ti = *localtime(&seconds);

  Form(oss, "%d-%d-%d %02d:%02d %s",
	  ti.tm_year + 1900, ti.tm_mon+1, ti.tm_mday,
	  ti.tm_hour, ti.tm_min, tzname[0]
	 );

  STR2STRING(oss, s);
  return s;
}
#endif

void Game::write_ggf(ostream &os, bool one_line) const
{
  sint4 indb = 0;
  sint4 indw;
  
  if (!match.type().is_komi_game() || komi_defined) {
    indb = color_index(Color::BLACK);
  }

  indw = 1-indb;
  String my_eol("");

  if (!one_line) my_eol += EOL;

  os << "(;GM[" << RegularBoardGame::GAME_NAME << "]PC[GGS"
     << RegularBoardGame::LOGIN_SERVICE << "]DT["
#if 1    
     << System::ggftime(System::clock()) << "]" // Igor for Welty
#else
     << time_convert(time(0)) << "]"
#endif    
     << my_eol;
  os << "PB[" << match.name(indb) << "]";
  os << "PW[" << match.name(indw) << "]" << my_eol;
  os << "RB[" << match.rating(indb) << "]";
  os << "RW[" << match.rating(indw) << "]" << my_eol;

  ostringstream os1, os2;
  
  clock(indb).print(os1, true);
  clock(indw).print(os2, true);

  String s1(os1), s2(os2);

  if (s1 != s2) {
    os << "TB[" << s1 << "]";
    os << "TW[" << s2 << "]" << my_eol;
  } else {
    os << "TI[" << s1 << "]" << my_eol;
  }
  
  os << "TY[" << match.type() << "]";

  if (gpinf[indb].komi_move_info.is_valid()) {
    os << "KB["; gpinf[indb].komi_move_info.write(os); os<< "]";
  }
  if (gpinf[indw].komi_move_info.is_valid()) {
    os << "KW["; gpinf[indw].komi_move_info.write(os); os<< "]";
  }
  if (komi_defined) {
    Form(os, "KM[%.3f]", komi);
  }
  os << "RE[";
  if (!is_finished()) os << "?";
  else {
    Form(os, "%+.3f", blacks_result());
    if      (resigned())        os << ":r";
    else if (matching_offers()) os << ":s";
    else if (timeout())         os << ":t";
  }
  os << "]" << my_eol;

  // starting position

  os << "BO[" << start_pos_ggf << "]" << my_eol;
  
  int n = 0;

  FORT (i, minfos.size()) {
    if (minfos[i].get_to_move() == Color::BLACK) os << "B"; else os << "W";
    os << "["; minfos[i].write(os); os << "]";
    ++n;
    if (n >= 4) { n = 0; if (i < minfos.size()-1) os << my_eol; }
  }
  if (n) os << my_eol;
  os << ";)";
}
    

void Game::write_join_update_header(ostream &os) const
{
  os << id() << " " << match.type() << " K";
  if (komi_defined) os << komi; else os << "?";
  os << EOL;
}

void Game::write_clocks(ostream &os, bool setting, bool prev) const
{
  sint4 ind = (gpinf[1].color == Color::BLACK);
  
  Form( os, "%-8s (%6.1f ", match.name(ind).c_str(), match.rating(ind));
  Color::write(os, gpinf[ind].color);
  os << ") "; 

  if (prev)
    prev_clock(ind).print(os, setting) << EOL;
  else
    clock(ind).print(os, setting) << EOL;
  
  Form(os, "%-8s (%6.1f ", match.name(1-ind).c_str(), match.rating(1-ind));
  Color::write(os, gpinf[1-ind].color);
  os << ") ";

  if (prev)
    prev_clock(1-ind).print(os, setting) << EOL;
  else
    clock(1-ind).print(os, setting) << EOL;  
}


void Game::write_update(ostream &os, bool use_prev_info) const
{   
  // last move
	
  Form(os, "%3d: ", minfos.size());
  if (minfos.size() > 0) {
    minfos[minfos.size()-1].write(os);
  } else {
    os << "PASS"; // pass if no move played
  }
  os << EOL;
	
  // clocks + current position

  write_clocks(os, false, use_prev_info);
  os << EOL;
    
  if (use_prev_info) {

    os << prev_pos_txt << EOL;
    
  } else {
    
    rbg->write_pos_txt(os);
    os << EOL;
    
  }
}


// last move omitted in synchro games if games are not in sync

void Game::mssg_join(ostream &os, bool client, bool omit_last_move) const
{
  bool omitted = false;
  MoveInfo last_move;

  if (omit_last_move && minfos.size() > 0) {
    last_move = minfos[minfos.size()-1];
    const_cast< vector<MoveInfo>& >(minfos).pop_back(); // still const in the end
    omitted = true;
  }

  os << "join ";

  if (client) {

    os << id() << " ";
    write_ggf(os);

  } else {

    write_join_update_header(os);
    
    os << minfos.size() << " move(s)" << EOL;
    
    if (minfos.size() > 0) {
      
      // start position if a move was played
      
      write_clocks(os, true, omitted);
      os << EOL;
      os << start_pos_txt;
      
      FORT (i, minfos.size()-1) {
        Form(os, "%3d: ", i+1);
        minfos[i].write(os);
        os << EOL;
      }
    }

    // last move

    write_update(os, omitted);
  }

  // restore move list
  
  if (omitted) const_cast< vector<MoveInfo>& >(minfos).push_back(last_move);
}

sint4 Game::index_to_move() const
{
  return color_index(rbg->color_to_move());
}


const String &Game::name_to_move() const
{
  return match.name(index_to_move());
}

const String &Game::previous_name_to_move() const
{
  static String s0("");
  
  if (minfos.size() < 1) return s0;

  MoveInfo last_move = minfos[minfos.size()-1];
  return match.name(color_index(last_move.get_to_move()));
}

sint4 Game::color_index(sint4 color) const
{
  if      (gpinf[0].color == color) return 0;
  else if (gpinf[1].color == color) return 1;

  ERR("undef color");
  return 0;
}


bool Game::undo(ostream &os, const String &pl)
{
  if (minfos.size() <= 0) { os << err_string() + "nothing to undo"; return false; }

  if (komi_defined && minfos.size() < 2) {
    os << err_string() + "no undo of komi moves"; return false;
  }

  if (match.name(0) == match.name(1)) {

    // selfplay => undo

    gpinf[0].undone = gpinf[1].undone = true;

  } else {
  
    sint4 ind = match.name_index(pl);
    gpinf[ind].undone = true;
  }

  if (!gpinf[0].undone || !gpinf[1].undone) {

    match.send_to_all("undo " + id() + " " + pl + " is asking");
    
  } else {
    
    // undo last move and replay whole game

    cancel_time_event(0);
    cancel_time_event(1);    
    
    minfos.pop_back();
    replay();

    // renew join message

    Match::Messages msgs;
    String s0;
    ostringstream oss0;
    mssg_join(oss0, false);
    STR2STRING(oss0, s0);

    String s1;
    ostringstream oss1;
    mssg_join(oss1, true);
    STR2STRING(oss1, s1);

    msgs.append(s0, s1);

    match.send_to_all(msgs);
    
    start_to_move_clock(false);
  }

  return true;
}

void Game::reset_offers_and_undos()
{
  gpinf[0].reset_offer_and_undo();
  gpinf[1].reset_offer_and_undo();
}

bool Game::matching_offers() const
{
  if (!gpinf[0].offered || !gpinf[1].offered) return false;

  return gpinf[0].offered_value <= - gpinf[1].offered_value;
}


bool Game::resigned() const
{
  return gpinf[0].resigned || gpinf[1].resigned;
}
 

void Game::resign(const String &player)
{
  gpinf[match.name_index(player)].resigned = true;
}


bool Game::score(ostream &os, const String &pl, const String& score)
{
  sint4 ind = match.name_index(pl);

  gpinf[ind].offered = true;

  if (sscanf(score.c_str(), "%lf", &gpinf[ind].offered_value) != 1) {
    os << err_string() << "score <value>";
    return false;
  }

  if (match.name(0) == match.name(1)) {
    gpinf[1-ind].offered = true;
    gpinf[1-ind].offered_value = gpinf[ind].offered_value = 0.0;
  }

  // matching offers => game end

  if (!matching_offers()) {
    match.send_to_all(String("score ") + id() + String(" ") + pl + String(" is offering"));
  }

  cancel_time_event(0);
  cancel_time_event(1);    
  return true;
}


bool Game::is_finished() const
{
  return
    rbg->game_finished() ||
    resigned() ||
    matching_offers() ||
    gpinf[0].timeout2 || gpinf[1].timeout2;
}


bool Game::timeout() const
{
  // check timeout1 integrity
  
  if (gpinf[0].timeout1 < 0 || gpinf[0].timeout1 > 2 ||
      gpinf[1].timeout1 < 0 || gpinf[1].timeout1 > 2 ||
      abs(gpinf[0].timeout1-gpinf[1].timeout1) > 1) {

    String s;

    s.form("timeout1 values corrupted %d %d\n",
	   gpinf[0].timeout1, gpinf[1].timeout1);
    ERR(s);
  }

  return
    gpinf[0].timeout1 || gpinf[1].timeout1 ||
    gpinf[0].timeout2 || gpinf[1].timeout2;
}


bool Game::play(ostream &os,
		const String &pl,
		MoveInfo &mi,
		bool replay)
{
  bool komi_game = match.type().is_komi_game();
  bool prev_komi_defined = komi_defined;
  int pi = -1, oi = -1;

  reset_offers_and_undos();

  ostringstream oss;

  if (!rbg->legal_move(oss, mi.move_str)) {
    oss.flush();
    String errmsg;
    STR2STRING(oss, errmsg);
#if DBG
    errstr << "ERR: " << errmsg << endl;
#endif
    os << err_string() << errmsg; return false;
  }

  // set color-to-move in mi

  mi.set_to_move(rbg->color_to_move());
  
  // save previous state

  save_pos_info(prev_pos_ggf, prev_pos_txt);

  prev_clocks[0] = clock(0);
  prev_clocks[1] = clock(1);
  
  if (komi_game && !komi_defined) {

    // in initial position => collect komi moves

    if (pl == match.name(0) && pl == match.name(1)) {

      // selfplay => fill komi move slots
	
      if (gpinf[0].komi_move_info.is_valid()) pi = 1; else pi = 0;
      gpinf[pi].komi_move_info = mi;
	
    } else {
      
      if (pl == match.name(0)) {
	if (gpinf[0].komi_move_info.is_valid()) {
	  os << err_string()+"you already sent the first move"; return false;
	}
	gpinf[0].komi_move_info = mi;
	pi = 0;

      } else if (pl == match.name(1)) {

	if (gpinf[1].komi_move_info.is_valid()) {
	  os << err_string()+"you already sent the first move"; return false;
	}
	gpinf[1].komi_move_info = mi;
	pi = 1;

      } else {

	os << pl + " not playing";
	ERR(pl + " not playing");
	return false;

      }
    }

    if (gpinf[0].komi_move_info.is_valid() && gpinf[1].komi_move_info.is_valid()) {

      // received both komi moves => compute komi value and assign colors

      komi = (gpinf[0].komi_move_info.eval + gpinf[1].komi_move_info.eval) * 0.5;
      
      // round komi to 0.5*MoveInfo::EVAL_EPS;

      komi = MoveInfo::eval_round_half(komi);

      if (gpinf[0].komi_move_info.eval > komi) {

	gpinf[0].color = rbg->color_to_move();

      } else if (gpinf[0].komi_move_info.eval < komi) {

	gpinf[0].color = Color::opponent(rbg->color_to_move());
	  
      } else {

	if (gpinf[0].color == Color::UNDEF) {

	  // assign random colors in non-replay mode

	  gpinf[0].color = (::ra.num(1000) >= 500) ? Color::BLACK : Color::WHITE;
	}
      }

      gpinf[1].color = Color::opponent(gpinf[0].color);
      if (rbg->color_to_move() == RegularBoardGame::WHITE) komi = -komi;
      komi_defined = true;
    }
  }

  bool regular_move = !komi_game || prev_komi_defined;
  
  if (regular_move) {
  
    // check whether "name" is to move
  
    if (name_to_move() != pl) { os << err_string()+"not your turn"; return false; }

    // check whether move is legal

    pi = index_to_move();
    oi = 1 - pi;

    ostringstream oss;
    
    rbg->do_move(mi.move_str);

    if (!replay) {
      if (!clock(pi).running() || clock(oi).running()) {
	ERR("wrong clock is running or both clocks are running");
      }
    }


  } else {

    // one of the first two moves in komi games

    oi = 1-pi;

  }

  if (pi < 0 || oi < 0 || oi != 1-pi) {
    ERR("pi not set");
  }

  // adjust player clock
  
  if (replay || match.trusted(pi)) {

    if (!replay) {

      // check time: if less than wallclock-TRUST_PERIOD => take wallclock

      sint8 elapsed = clock(pi).elapsed_since_start();
      
      if (mi.time < elapsed - TRUST_PERIOD * uSec) {
 
	ostringstream oss;
	oss << "trust-violation " << id() << " " << pl << " (";
	Color::write(oss, gpinf[pi].color);
	oss << ") delta= " << TRUST_PERIOD << " + ";
	Form(oss, "%.1f secs", 
	     real4(elapsed - TRUST_PERIOD * uSec - mi.time)/uSec + 0.1);
	
	String msg;
	STR2STRING(oss, msg);
	match.msgs.append(msg);

	if ( elapsed < 0 ) { // Igor: gracefully handle problems
	  errstr << VCFL << "elapsed(" << elapsed << ')' << endl;
	  elapsed = 1;
	}
	mi.time = elapsed;
      }

    }

#if TEST
    errstr << "stop clock " << pi << " " << mi.time/uSec << endl
	   << "c0r=" << clock(0).running() << " "
           << "c1r=" << clock(1).running() << endl;
#endif

    // trusted | replay -> take time stored in move
    
    if (clock(pi).stop(mi.time) < 0)
      errstr << VCFL << "stop result < 0" << endl;
    
    if (!replay) cancel_time_event(pi);
    
  } else {

    // untrusted -> wall clock time
    
    sint8 elapsed = clock(pi).stop(-1);
    if (elapsed < 0) {
      errstr << VCFL << "stop result < 0" << endl;
      elapsed = 1;
    }
    
#if TEST
    errstr << "UNTRUSTED ELAPSED= " << elapsed/uSec << endl
	   << "to1=" << clock(0).soft_timeout() << " "
           << "to2=" << clock(1).hard_timeout() << endl;
#endif

    cancel_time_event(pi);
    
#if TEST
    errstr << "stop clock " << pi << endl;
#endif

    mi.time = elapsed;
  }

  if (!regular_move) {

    // overwrite komi move (now having correct time)

    gpinf[pi].komi_move_info = mi;
  }

  
  if (!komi_game || komi_defined) {

    // update board with actual move

    // in case that komi just got defined take
    // the according first move

    if (!prev_komi_defined && komi_defined) {

      mi = gpinf[index_to_move()].komi_move_info;
      // color to move should be set in mi!
      rbg->do_move(mi.move_str);
    }

    // append move to list
  
    minfos.push_back(mi);

    if (!replay) {
    
      if (komi_game && komi_defined && !prev_komi_defined) {
	
	// renew join message (updated color/time info)
	// when both komi moves just have been sent
	
	String s0;
	ostringstream oss0;
	mssg_join(oss0, 0);
	STR2STRING(oss0, s0);

	String s1;
	ostringstream oss1;
	mssg_join(oss1, 1);
	STR2STRING(oss1, s1);
	match.msgs.append(s0, s1);
	
      } else {
      
	// create update messages

	String s0, s1;
	
	{
	  // mode 0
	  
	  ostringstream oss;
	  
	  oss << "update ";
	  write_join_update_header(oss);
	  write_update(oss);
	  STR2STRING(oss, s0);
	}
	
	{
	  // mode 1
	  
	  ostringstream oss;
	  
	  oss << "update " << id() << " ";
	  
	  // last move 
	  
	  if (minfos.size() > 0) minfos.back().write(oss);
	  else { os << "PASS"; }
	  
	  STR2STRING(oss, s1);
	}

	match.msgs.append(s0, s1);

	{
	  string msg;
	  rbg->server_comment(msg);
	  if (!msg.empty()) {
	    msg = "comment " + id() + " " + msg;
	    match.msgs.append(msg);
	  }
	}
      }
    }
  }

  handle_timeouts(pi, replay);
  
  if (is_finished()) {
    cancel_time_event(0);
    cancel_time_event(1);    
    return true;
  }

  // don't start clock if first komi game move or
  // !replay synchro game move

  if ((komi_game && !komi_defined) ||
      (match.type().is_synchro_game() && !replay)) {}
  else start_to_move_clock(replay);
    
  if (komi_game && !komi_defined && match.name(0) == match.name(1)) {
    return play(os, pl, mi, replay);
  }

  return true;
}

// detect timeouts and send messages only once

void Game::handle_timeouts(sint4 pi, bool replay)
{
#if DBG  
  errstr << "to1=" << clock(pi).soft_timeout() << endl;
  errstr << "to2=" << clock(pi).hard_timeout() << endl;  
#endif
  
  if (!gpinf[pi].timeout1 && clock(pi).soft_timeout()) {
    
    // timeout

    sint4 oi = 1-pi;
    gpinf[pi].timeout1 = gpinf[oi].timeout1 + 1;
      
    if (!replay) {
      
      // timeout1 message

      ostringstream oss;
      oss << "timeout " << id() << " " << match.name(pi) << " (";
      Color::write(oss, gpinf[pi].color);
      oss << ")";
      String msg;
      STR2STRING(oss, msg);
      match.msgs.append(msg);

      // create next time event / moved (was called in replay mode) !!!
      create_time_event(pi);
    }
  }

#if 0
  // synchro: soft => hard
  // what's the value of soft-timed-out games in synchro mode?
  //  if ((clock(pi).soft_timeout() && match.type().is_synchro_game()) ||
#endif
  
  if (clock(pi).hard_timeout()) {
      
    // fatal timeout => game finished
    
    if (gpinf[pi].timeout2)
      errstr << VCFL << "second timeout2" << endl;
      
    gpinf[pi].timeout2 = true;
    
    if (!replay) {
      
      // timeout2 message
      
      ostringstream oss;
      oss << "fatal-timeout " << id() << " " << match.name(pi) << " (";
      Color::write(oss, gpinf[pi].color);
      oss << ")";
      String msg;
      STR2STRING(oss, msg);
      match.msgs.append(msg);

#if DBG      
      errstr << "to3=" << clock(pi).hard_timeout() << endl;
#endif
    }

    cancel_time_event(pi);
  }
}



void Game::start_to_move_clock(bool replay)
{
#if TEST
  errstr << "start clock (" << index_to_move() << ")" << " " << replay << endl;
#endif

  int i = index_to_move();
  clock(i).start();

  if (!replay) create_time_event(i); // no time events in replay mode
}

void Game::start_both_clocks(bool replay)
{
#if TEST
  errstr << "start both clocks" << endl;
#endif
    
  clock(0).start();
  clock(1).start();

  if (!replay) {
    create_time_event(0);
    create_time_event(1);
  }
}

bool Game::play(ostream &os, const String &pl, const String &move_info_string)
{
  MoveInfo mi;
  ostringstream oss;

  // move syntactically correct?

  if (!mi.parse(oss, move_info_string)) {
    os << err_string();
    mi.parse(os, move_info_string);
    return false;
  }
  
  return play(os, pl, mi);
}


// assumes names,colors,clocks and komi-moves set

void Game::replay()
{
  // save relevant info before resetting

  MoveInfo km0 = gpinf[0].komi_move_info;
  MoveInfo km1 = gpinf[1].komi_move_info;
  int c0 = gpinf[0].color;
  int c1 = gpinf[1].color;
  
  // save move list and erase original
  
  vector<MoveInfo> mvs;

  mvs.insert(mvs.end(), minfos.begin(), minfos.end());
  minfos.erase(minfos.begin(), minfos.end());

  istringstream iss(start_pos_ggf);
  if (!rbg->read_pos_ggf(iss)) ERR("start_pos_ggf corrupt");

  reset();

  gpinf[0].color = c0;
  gpinf[1].color = c1;
  
  clock(0).reset();
  clock(1).reset();
  
  if (match.type().is_komi_game()) {

    start_both_clocks(true);
  
  } else {

    start_to_move_clock(true);
    
  }    

  // replay all komi moves

  if (km0.is_valid()) {
    ostringstream os;
    os << "km0="; km0.write(os); os << endl;
    gpinf[0].komi_move_info.write(os); os << endl;
    gpinf[1].komi_move_info.write(os); os << endl;

#if DBG    
    errstr << String(os) << " " << match.name(0) << endl;
#endif
    
    if (!play(os, match.name(0), km0, true)) {
      os << "km1="; km1.write(os); os << endl;
      gpinf[0].komi_move_info.write(os); os << endl;
      gpinf[1].komi_move_info.write(os); os << endl;
      ERR2("illegal komi move in replay 0", String(os));    
    }
  }

  if (km1.is_valid() && match.name(0) != match.name(1)) {
    ostringstream os;
    os << "km1="; km1.write(os); os << endl;
    gpinf[0].komi_move_info.write(os); os << endl;
    gpinf[1].komi_move_info.write(os); os << endl;

#if DBG
    errstr << String(os) << " " << match.name(1) << endl;
#endif
    
    if (!play(os, match.name(1), km1, true)) {
      os << "km0="; km0.write(os); os << endl;
      gpinf[0].komi_move_info.write(os); os << endl;
      gpinf[1].komi_move_info.write(os); os << endl;
      ERR2("illegal komi move in replay 1", String(os));    
    }
  }

  // replay remaining moves

  FORT (i, mvs.size()) {

    if (i == 0 && match.type().is_komi_game()) {
      // first move in list is komi move
      continue;
    }
    
    String pl = name_to_move();
    ostringstream os;
    mvs[i].write(os); os << " ";
    if (!play(os, pl, mvs[i], true)) {
      rbg->write_pos_txt(errstr);
      ERR2("illegal move in replay", String(os));
    }
  }

}


void Game::undo_last_move()
{
  if (minfos.size() > 0) {
    minfos.pop_back();
    replay();
  }
}


real8 Game::blacks_result() const
{
  real8 vb, mr;
  sint4 ri;
  
  mr = rbg->max_result();
  ri = RegularBoardGame::RESULT_INCREMENT;

  if (ri < 1) {
    ERR("result increment < 1");
    return 0;
  }
  
  if (!match.type().is_komi_game() && komi != 0.0) {
    ERR("komi != 0 in non komi game");
    return 0;
  }

  if (!is_finished()) {
    ERR("game not finished");
    return 0;
  }

  if (gpinf[0].timeout2 || gpinf[1].timeout2) {

    // fatal timeout => player loss maximal
    
    sint4 col;
    
    if (gpinf[0].timeout2) col = gpinf[0].color;
    else                   col = gpinf[1].color;

    if (col == Color::BLACK) vb = -mr - komi;
    else                     vb = +mr - komi;

  } else if (gpinf[0].resigned || gpinf[1].resigned) {

    // resign => player loss maximal
    
    sint4 col;
    
    if (gpinf[0].resigned) col = gpinf[0].color;
    else                   col = gpinf[1].color;

    if (col == Color::BLACK) vb = -mr - komi;
    else                     vb = +mr - komi;

  } else if (matching_offers()) {

    // offers matched => take average
    
    vb = (gpinf[color_index(Color::BLACK)].offered_value -
          gpinf[color_index(Color::WHITE)].offered_value) * 0.5 - komi;
      
  } else {

    // regular game ending

    sint4 ddb = rbg->blacks_result( match.type().is_anti_game() );
  
    if (match.type().is_anti_game()) ddb = -ddb;
    
    vb = ddb - komi;

    if (timeout()) {

      // one or two timeouts => first timedout player loses
      
      sint4 col;

      if (gpinf[0].timeout1 == 1) col = gpinf[0].color;
      else                        col = gpinf[1].color;

      if (col == Color::BLACK) {
      
        // black timedout first => if black didn't lose assign minimal black loss
        
        sint4 mldd = sint4(floor(komi/ri))*ri;
        while (mldd >= komi) mldd -= ri;
        real8 vminlossb = mldd - komi;
        
        vb = min(vb, vminlossb);
        
      } else {

        // white timedout first => if white didn't lose assign minimal white loss
        
        sint4 mwdd = sint4(ceil(komi/ri))*ri;
        while (mwdd <= komi) mwdd += ri;
        real8 vminwinb = mwdd - komi;
        
        vb = max(vb, vminwinb);
      }
    }
  }

  vb = MoveInfo::eval_round_half(vb);
  return vb;
}


real8 Game::result0() const
{
  real4 r0 = blacks_result();
  if (gpinf[0].color == Color::WHITE) r0 = -r0;
  return r0;
}



bool Game::player_clock_running(const String &pl) const
{
  return clock(match.name_index(pl)).running();
}


void Game::te_handle(sint4 player_index)
{
  errstr << "TIME_EVENT player " << player_index << endl;

  if (!clock(player_index).running()) 
    errstr << VCFL << "clock not running";
    
  clock(player_index).event();
  
  handle_timeouts(player_index, false);
}


void Game::create_time_event(sint4 player_index)
{
  sint4 t = clock(player_index).time_left()/uSec + TRUST_PERIOD + 1;
  match.te_create(game_id*2+player_index, t);
#if DBG
  errstr << "create te player " << player_index
	 << " t=" << (t/60) << ":" << (t % 60) << endl;
  errstr << "time left= " << clock(player_index).time_left()/uSec << endl;
#endif  
}


void Game::cancel_time_event(sint4 player_index)
{
  match.te_cancel(2*game_id + player_index);
#if DBG  
  errstr << "cancel te player " << player_index << endl;
#endif  
}

void Game::write_infos() {    
  cout << minfos.size() << endl;
  FORU (i, minfos.size()) {
    cout << minfos[i].move_str << " " << minfos[i].color_to_move << endl;
  }
}

