// $Id: Match.H 160 2007-06-22 15:21:10Z mburo $
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


#ifndef Match_H
#define Match_H

// generic game classes
// 9.99 by Michael Buro

#include "VC_Match.H"
#include "Array_N.H"
#include "Magic.H"
#include <vector>

class Request;
class Game;

// handles single game and synchro match

class Match : public VC_Match, public Magic
{
public:

  class Messages {

  public:

    struct Msg { String m0, m1; };
    
    std::vector<Msg> msgs;
    
    void clear() { msgs.erase(msgs.begin(), msgs.end()); }
    void append(const String &s);
    void append(const String &s0, const String &s1);
    void append(const Messages &msgs);
    
    Messages() { clear(); }
  };

private:

  class MatchPlayerInfo {
  public:
    
    bool adjourned;
    bool aborted;
    bool undone;

    void reset() {
      adjourned = false;
      aborted = false;
      undone = false;
    }
    
    MatchPlayerInfo() { reset(); }
  };


  class SynchroInfo {

  public:
    
    bool move_sent;
    String player;
    Messages msgs;

    SynchroInfo() { move_sent = false; }
  };
  
  Array_N< MatchPlayerInfo, 2 > mpinf;
  
public:
  
  std::vector<Game*> gp;
  Array_N< SynchroInfo, 2 > sinf;  // for game with index i (synchro)

  void send_to_all(const String &s);
  void send_to_all(const Messages &msgs);
  void send_to_name(const String &name, const Messages &msgs);
  void send_to_all_but_name(const String &name, const Messages &msgs);

  Match(const Request &r);
  virtual ~Match();
  
  Messages msgs;       // Game writes here
  mutable Array_N< String, 2 > last_msg;  // VC_Match reads here

  String err_string(const String &id) const;
  sint4 name_index(const String &pl) const;

  bool is_finished() const;
  bool is_interrupted() const;
  bool is_aborted() const;
  bool is_adjourned() const;
  sint4 game_id(const String &id);
  String match_id(const String &id);
  sint4 not_in_sync_id() const;

  void delete_this();
  
  // ++++ virtual functions called by EXE ++++

  // extracts info from move_info_time and tries to make move for player name
  bool play(std::ostream &os, const String &id, const String &pl,
	    const String &move_info_time);

  // player has issued undo, true iff undo allowed
  bool undo(std::ostream &os, const String &id, const String &pl);

  // player has offered a value
  bool score(std::ostream &os, const String &id, const String &pl, const String& value);

  bool resign(std::ostream &os, const String &id, const String &pl);

  // player has broken game
  void adjourn(const String &pl);

  // player has aborted
  void abort(const String &pl);

  void mssg_last(std::ostream &os, bool client) const;
  void call_join(const String &name) const;

  void te_handle(sint4 mssg, uint4 time);
  // void te_print(std::ostream &os) const { os << "Match " << id() << endl; };
  
  
  // ++++++++++++++++++++++++++++

  // game dependent; defined in <game>Score.C vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  
  real8 result() const;         // returns komi adjusted game result for black
  static real8 score(real8 r);  // normalizes score -> [0,1]

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  bool player_clock_running(const String &pl) const; 

};

#endif
