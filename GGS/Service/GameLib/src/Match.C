// $Id: Match.C 160 2007-06-22 15:21:10Z mburo $
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

#include "Global.H"
#include <cstdio>
#include <vector>
#include <algorithm>
#include "Match.H"
#include "RegularBoardGame.H"


#define CONV 0

using namespace std;

void Match::delete_this()
{
  delete this;
#if 0
  char* p1 = (char*) this;
  char* p2 = p1 + sizeof(*this);
  for ( ; p1 != p2; ++p1 ) {
    char c = *p1;
  }
#endif  
  //  memset(this, 0xff, sizeof(*this));
}


Match::Match(const Request &r) : VC_Match(r)
{
  bool syn = RegularBoardGame::HAS_SYNCHRO && type().is_synchro_game();

  if (saved()) {

    // a saved match
    
    istringstream iss(match_str);

    int n;

    iss >> n;

#if CONV
    if (!iss) { ERR("istringstream read error 2"); return; }
#else
    if (!iss) { n = 1; }
#endif

    FORT (i, n) {
      Game *p = new Game(*this, i);
      if (!p->read_ggf(iss)) {
	ERR("istringstream read error 2"); return;
      }

      p->start_to_move_clock(false);
      gp.push_back(p);
    }

#if 0
    FORU (i, gp.size()) {
      if (type() != gp[i]->type()) ERR("different game types");
    }
#endif
    
    if ((syn && gp.size() != 2) || (!syn && gp.size() != 1)) ERR("wrong game #");

    if (syn) {
      
      // synchronization required? (true => not required)
    
      if (gp[0]->is_finished()) sinf[0].move_sent = true;
      if (gp[1]->is_finished()) sinf[1].move_sent = true;

    }
    
  } else {

    // new match

    sint4 color0 = 0;
    ostringstream oss;
    String board_string;

    // generate starting position

    {
      RegularBoardGame *rbg = RegularBoardGame::new_game();
      rbg->init_pos(type().get_board_type(), type().get_rand_type());
      rbg->write_pos_ggf(oss);
      STR2STRING(oss, board_string);
      delete rbg;
    }
    
    if (!type().is_komi_game() && !type().is_rand_game()) {
    
      sint4 b_ind;
      sint4 lc0;
      
      switch (type().get_pref_color()) {
      
      case Color::UNDEF:
      
	lc0 = last_color(0);

	if (!rated()) lc0 = '?';  // unrated => random colors

	lc0 = '?';  // last_color is broken (synchro...)
	
	switch (lc0) {
	case 'B' : b_ind = 1; break;
	case 'W' : b_ind = 0; break;
	default  : b_ind = ::ra.num(1000) >= 500; break;
	}

	color0 = (b_ind == 0) ? Color::BLACK : Color::WHITE;
	break;
      
      case Color::BLACK:
	color0 = Color::BLACK;
	break;
      
      case Color::WHITE:
	color0 = Color::WHITE;
	break;
      
      default:
	ERR2("undefined color", type().get_pref_color());
	break;
      }
    
    } else if (type().is_komi_game()) {

      color0 = Color::UNDEF;
    
    } else if (type().is_rand_game()) {

      if (!type().is_synchro_game()) ERR("match type 'r' not supported");

      color0 = Color::BLACK;
    
    }

    Game *ng = new Game(*this, 0, color0, board_string);
    gp.push_back(ng);

    if (syn) {
      ng = new Game(*this, 1, Color::opponent(color0), board_string);
      gp.push_back(ng);
    }
 
  }
    
  // create game start message
  // /os: + match .9 2544 lynx 2505 kitty s8r20 R

  {
    String msg;
    ostringstream oss;

    oss << "+ match " << id() << " ";
    Form(oss, "%.0f ", rating(0)); oss << name(0); oss << " ";
    Form(oss, "%.0f ", rating(1)); oss << name(1); oss << " ";
    oss << type() << " ";
    if (!tid().empty()) oss << "T ";
    else if (rated()) oss << "R "; else oss << "U ";
    STR2STRING(oss, msg);
    last_msg[0] = last_msg[1] = msg;
  }

  cb_start();
}

Match::~Match()
{
  check_magic();

  if (!is_finished() && !is_interrupted()) {

    // "disaster"

    if      (dtor(0)) mpinf[0].adjourned = true;
    else if (dtor(1)) mpinf[1].adjourned = true;
    else {

      // service shutdown?!

      mpinf[0].adjourned = true;
      mpinf[1].adjourned = true;
    }
  }

  real4 r0 = 0;  // result for player 0

  if (is_finished()) {

    r0 = gp[0]->result0();
    
    if (type().is_synchro_game()) {
      
      r0 += gp[1]->result0();
      r0 *= 0.5;
      
    }
  }

  sint4 nsi = not_in_sync_id();

  if (nsi >= 0) {

    // out of sync

    // errstr << "undo last move" << endl;
    gp[nsi]->undo_last_move();
  }

  // archive/adjourn first -- Igor
  
  if (rated() && (!type().is_komi_game() || gp[0]->komi_defined)) {

    ostringstream oss;

    oss << gp.size() << " ";
    gp[0]->write_ggf(oss, true);
    if (type().is_synchro_game()) gp[1]->write_ggf(oss, true);

    String game_log(oss);
    String game_info;
    
    if (gp[0]->komi_defined) {
      game_info = type().to_string_with_komi(gp[0]->komi);
    } else {
      game_info = type().to_string();
    }

    if (is_finished()) {

#if 0
      if      (resigned())        game_info += ":r";
      else if (matching_offers()) game_info += ":s";
      else if (timeout())         game_info += ":t";
#endif
      
      real4 s0 = score(r0);
      cb_archive(game_log, game_info, r0, s0);

    } else {

      if (!is_aborted()) {
	game_info += ":l";

	//errstr << "cb_adjourn: " << game_log << " " << game_info << endl;
	
        cb_adjourn(game_log, game_info);
      }
    }
  }

  // create game end messages
  // /os: - match .9 2544 lynx 2505 kitty s8r20 R +30.0 .stored_id

  {
    ostringstream oss;
    oss << "- match " << id() << " ";
    Form(oss, "%.0f ", rating(0)); oss << name(0); oss << " ";
    Form(oss, "%.0f ", rating(1)); oss << name(1); oss << " ";
    oss << type() << " ";
    if (!tid().empty()) oss << "T ";
    else if (rated()) oss << "R "; else oss << "U ";

    if (is_finished()) {

      Form(oss, "%+.2f ", r0);

#if 0      
      if      (resigned())        oss << "resigned";
      else if (matching_offers()) oss << "mutual score";
      else if (timeout())         oss << "timeout";
#endif
      
    } else {

      if (is_aborted())

	oss << "aborted";

      else { 

	oss << name(!mpinf[0].adjourned) << " left";

      }
    }

    // print global counter -- Igor    
    if (rated() && !is_aborted()) oss << ' ' << counter();

    String msg;
    STR2STRING(oss, msg);
    last_msg[0] = last_msg[1] = msg;
    cb_end();
  }

  // finally remove games
  
  FORT (i, gp.size()) delete gp[i];
}


// called by EXE_Service

void Match::mssg_last(ostream &os, bool client) const
{
  os << last_msg[int(client)];
}


bool Match::is_finished() const
{
  if (type().is_synchro_game()) {

    return gp[0]->is_finished() && gp[1]->is_finished();

  } else {

    return gp[0]->is_finished();

  }
}

bool Match::is_interrupted() const
{
  return is_aborted() || is_adjourned();
}

bool Match::is_aborted() const
{
  return mpinf[0].aborted && mpinf[1].aborted;
}

bool Match::is_adjourned() const
{
  return mpinf[0].adjourned || mpinf[1].adjourned;
}


// id= .match-id[.game-id], return -1 if there is no game-id

sint4 Match::game_id(const String &id)
{
  vector<String> s;
  String::parse(id, s, '.');

  if (s.size() == 2)
    return atoi(s[1].c_str());
  else
    return -1;

  return 0;
}


String Match::match_id(const String &id)
{
  String a, b;

  String::parse(id, a, b, '.');
  return String(".") + a;
}


String Match::err_string(const String &id) const {
  return "error " + id + " ";
}

sint4 Match::name_index(const String &pl) const
{
  if      (name(0) == pl) return 0;
  else if (name(1) == pl) return 1;

  ERR("undef player");
  return 0;  
}


bool Match::play(ostream &os,
		 const String &id,
		 const String &pl,
		 const String &mv_info_time)
{
  check_magic();

  msgs.clear();

  sint4 gid = game_id(id);

  if (type().is_synchro_game() ^ gid >= 0) {
      
    os << err_string(id);
    os << "game id needed (only) in synchro games";
    return false;
  }
    
  if (type().is_synchro_game() && gid >= 2) {
    os << err_string(id);
    os << "game id must be .0 or .1 in synchro games";
    return false;
  }
    
  if (!type().is_synchro_game()) {
      
    // regular & komi games
      
    bool ret = gp[0]->play(os, pl, mv_info_time);
    if (!ret) return false;
    send_to_all(msgs);
      
    if (gp[0]->is_finished()) delete_this();
    return true;
  }
    
  // synchro game
    
  if (!gp[gid]->player_clock_running(pl)) {
    os << err_string(id) << "wrong game!";
    return false;
  }
    
  bool ret = gp[gid]->play(os, pl, mv_info_time);
  if (!ret) return false;  // invalid move

  // move ok

  sinf[gid].move_sent = true;
  sinf[gid].player = pl;

  if (gp[gid]->is_finished()) {

    // game end message
    
    ostringstream oss;
    oss << "end " << gp[gid]->id();
    oss << " ( " << name(0) << " vs. " << name(1);
    Form(oss, " ) %+.2f ", gp[gid]->result0());

    String msg;
    STR2STRING(oss, msg);

    msgs.append(msg);
  }

  sinf[gid].msgs = msgs;
  send_to_name(pl, msgs);  // move + game end messages to last player
  
  if (sinf[0].move_sent && sinf[1].move_sent) {

    // synchro: start active clocks

    if (!gp[0]->is_finished()) gp[0]->start_to_move_clock(false);
    if (!gp[1]->is_finished()) gp[1]->start_to_move_clock(false);
    
    sinf[0].move_sent = gp[0]->is_finished();
    sinf[1].move_sent = gp[1]->is_finished();
    
    // both moves sent -> send messages to others

    send_to_all_but_name(sinf[0].player, sinf[0].msgs);
    sinf[0].msgs.clear();
    send_to_all_but_name(sinf[1].player, sinf[1].msgs);
    sinf[1].msgs.clear();
    
    if (is_finished()) { delete_this(); return true; }
  };

  return true;
}


// player has issued undo, true iff undo allowed

bool Match::undo(ostream &os, const String &id, const String &pl)
{
  check_magic();

  if (type().is_synchro_game()) {
    os << err_string(id) << "no undo in synchro games";
    return false;
  }

  return gp[0]->undo(os, pl);
}

// player has offered a value

bool Match::score(ostream &os, const String &id,
		  const String &pl, const String& value)
{
  check_magic();

  if (type().is_synchro_game()) {
    os << err_string(id) << "no score in synchro games";
    return false;
  }

  if (gp[0]->score(os, pl, value)) {

    if (gp[0]->is_finished()) delete_this();
    return true;

  } else return false;
}

bool Match::resign(ostream &/*os*/, const String &/*id*/, const String &pl)
{
  check_magic();
  
  FORT (i, 1+type().is_synchro_game()) gp[i]->resign(pl);

  if (type().is_synchro_game()) {

    // game end messages

    FORT (i, 2) {

      Messages m;
      String msg;
      
      ostringstream oss;
      oss << "end " << gp[i]->id();
      oss << " ( " << name(0) << " vs. " << name(1);
      Form(oss, " ) %+.2f ", gp[i]->result0());
      STR2STRING(oss, msg);
      
      m.append(msg);
      send_to_all(m);
    }
  }
  
  delete_this();
  return true;
}
 
void Match::adjourn(const String &pl)
{
  check_magic();
  
  mpinf[name_index(pl)].adjourned = true;

  if (type().is_synchro_game()) {
    
    // game end message to *.0 and *.1

    FORT (i, 2) {
      ostringstream oss;
      oss << "end " << gp[i]->id();
      oss << " ( " << name(0) << " vs. " << name(1)
	  << " ) player " << pl << " left";
      String msg;
      STR2STRING(oss, msg);
      send_to_all(msg);
    }
  }

  delete_this();
}

void Match::abort(const String &pl)
{
  check_magic();
  
  sint4 ind;
  
  if (name(0) == name(1)) {

    // selfplay => abort

    mpinf[0].aborted = mpinf[1].aborted = true;
    goto do_it;
  }

  ind = name_index(pl);
  mpinf[ind].aborted = true;

  if (is_aborted()) {

  do_it:
    
    // both players aborted => game/match end

    if (type().is_synchro_game()) {
    
      // game end message to *.0 and *.1
      
      FORT (i, 2) {
	ostringstream oss;
	oss << "end " << gp[i]->id();
	oss << " ( " << name(0) << " vs. " << name(1)
	    << " ) aborted";
	String msg;
	STR2STRING(oss, msg);
	send_to_all(msg);
      }
    }
    
    delete_this();
    return;
    
  } else {

    FORT (i, 1+type().is_synchro_game()) {
      send_to_all("abort " + gp[i]->id() + " " + pl + " is asking");
    }
  }  
}


void Match::call_join(const String &n) const
{
  bool omit[2]; // omit last move in game i?
  omit[0] = omit[1] = false;

  sint4 nsi = not_in_sync_id();

#if 0  
  cout << "-------------------" << endl;
  cout << nsi << endl;
  cout << n << endl;
  if (nsi >= 0) {
    cout << gp[nsi]->previous_name_to_move() << endl;
    gp[nsi]->write_infos();
  }
#endif
  
  if (nsi >= 0) { // game nsi longer

    // omit last move in longer game if n=observer or n=opponent

    if ((n != name(0) && n != name(1)) ||
	 (gp[nsi]->previous_name_to_move() != n)) {

      omit[nsi] = true;
    }
  }

  FORT (i, 1+type().is_synchro_game()) {
    ostringstream oss0, oss1;

    gp[i]->mssg_join(oss0, false, omit[i]);
    STR2STRING(oss0, last_msg[0]);
    
    gp[i]->mssg_join(oss1, true, omit[i]);  
    STR2STRING(oss1, last_msg[1]);   
    
    cb_update_to_name(n);
  }
}

void Match::send_to_all(const Messages &m)
{
  FORT (i, m.msgs.size()) {
    last_msg[0] = m.msgs[i].m0;
    last_msg[1] = m.msgs[i].m1;
    cb_update_to_all();
  }
}

void Match::send_to_all(const String &s)
{
  last_msg[0] = last_msg[1] = s;
  cb_update_to_all();
}

void Match::send_to_name(const String &n, const Messages &m)
{
  FORT (i, m.msgs.size()) {
    last_msg[0] = m.msgs[i].m0;
    last_msg[1] = m.msgs[i].m1;
    cb_update_to_name(n);
  }  
}

void Match::send_to_all_but_name(const String &n, const Messages &m)
{
  FORT (i, m.msgs.size()) {
    last_msg[0] = m.msgs[i].m0;
    last_msg[1] = m.msgs[i].m1;
    cb_update_to_all_but_name(n);
  }
}


// return game id i (0/1) with the maximum number of moves if move#
// difference is 1 in synchro games;
// -1 otherwise

sint4 Match::not_in_sync_id() const
{
  if (!type().is_synchro_game() || is_finished()) return -1;

  uint4 n0 = gp[0]->move_num();
  uint4 n1 = gp[1]->move_num();    
  
  if (::abs(n0-n1) != 1) return -1;

  return n0 < n1;
}


void Match::Messages::append(const String &s)
{
  Messages::Msg msg;
  msg.m0 = msg.m1 = s;
  msgs.push_back(msg);
}

void Match::Messages::append(const String &m0, const String &m1)
{
  Messages::Msg msg;
  msg.m0 = m0;
  msg.m1 = m1;  
  msgs.push_back(msg);
}

void Match::Messages::append(const Messages &msgs)
{
  FORT (i, msgs.msgs.size()) {
    append(msgs.msgs[i].m0, msgs.msgs[i].m1);
  }
}

real8 Match::score(real8 result) { return RegularBoardGame::score(result); }


// game time event handler

void Match::te_handle(sint4 mssg, uint4 /*time*/) {

  check_magic();
  
  sint4 gid = mssg >> 1;
  sint4 pid = mssg & 1;

  // errstr << "TIME_EVENT " << gid << " " << pid << " " << time << endl;

  // check for timeouts
  
  msgs.clear();
  gp[gid]->te_handle(pid);
  send_to_all(msgs);

  if (type().is_synchro_game() && gp[gid]->is_finished()) {

    // synchro match game end message
      
    ostringstream oss;
    oss << "end " << gp[gid]->id();
    oss << " ( " << name(0) << " vs. " << name(1);
    Form(oss, " ) %+.2f ", gp[gid]->result0());
    
    String msg;
    STR2STRING(oss, msg);
    send_to_all(msg);
  }

  if (is_finished()) { delete_this(); return; }

  if (type().is_synchro_game() && gp[gid]->is_finished()) {

    // one game remaining
    // start pid's clock in the other game if no clock is running

    if (!gp[1-gid]->clock(0).running() && !gp[1-gid]->clock(1).running())
      gp[1-gid]->start_to_move_clock(false);

    // handle pending synchro messages

    sinf[gid].move_sent = true;
  
    if (sinf[0].move_sent && sinf[1].move_sent) {
      
      sinf[0].move_sent = gp[0]->is_finished();
      sinf[1].move_sent = gp[1]->is_finished();
      
      // both moves sent -> send messages to others
      
      send_to_all_but_name(sinf[0].player, sinf[0].msgs);
      sinf[0].msgs.clear();
      send_to_all_but_name(sinf[1].player, sinf[1].msgs);
      sinf[1].msgs.clear();
    }
  };

}

