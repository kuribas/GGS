// $Id: Game.H 160 2007-06-22 15:21:10Z mburo $
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


#ifndef Game_H
#define Game_H

#include "Global.H"
#include "RegularBoardGame.H"
#include "GAME_Clock.H"
#include "String.H"
#include "Array_N.H"

class Match;

class MoveInfo {

public:

  static const real4 EVAL_EPS; // eval accuracy
  String move_str;
  real4 eval;
  sint8 time;
  sint4 color_to_move;
  
  void init() { move_str = ""; eval = time = 0; color_to_move = Color::BLACK; }

  MoveInfo() { init(); }

  bool parse(std::ostream &err, const String &s);

  bool is_valid() const { return move_str != ""; }

  void write(std::ostream &os) const;
  
  static real4 eval_round(real4 v);
  static real4 eval_round_half(real4 v);

  void set_to_move(sint4 c) { color_to_move = c; }
  sint4 get_to_move() const { return color_to_move; }
};


class GamePlayerInfo
{
public:
    
  String      name;
  sint4       color;
  sint4       timeout1;  // first timeout order matters => sint4
  bool        timeout2;  // second timeout (fatal)
  bool        offered;   // player offered a value (result=value+komi (black))
  real8       offered_value; 
  bool        undone;
  bool        resigned;
  MoveInfo    komi_move_info;
  
  void reset() {
    color     = Color::UNDEF;
    timeout1  = 0;
    timeout2  = false;
    resigned  = false;
    reset_offer_and_undo();
    komi_move_info.init();
  }

  void reset_offer_and_undo() {
    offered  = false;
    offered_value = 0;
    undone = false;
  }
    
  GamePlayerInfo() { reset(); }

};


// single game

class RegularBoardGame;

class Game
{

private:

  mutable RegularBoardGame *rbg;

  Match        &match;   // to access match info
  sint4        game_id;  // 0 or 1 to access clocks
  Array_N< GamePlayerInfo, 2 > gpinf;

  bool         komi_defined;
  real4        komi;

  std::vector<MoveInfo> minfos;

  // startpos info
  
  String       start_pos_ggf, start_pos_txt;
  sint4        start_pos_color_to_move;

  sint4 save_pos_info(String &, String &);
  
  // previous game info used in write_update()

  String       prev_pos_ggf, prev_pos_txt;
  Array_N< GAME_Clock, 2 > prev_clocks;
  GAME_Clock   &prev_clock(uint4 i) { return prev_clocks[i]; }
  const GAME_Clock &prev_clock(uint4 i) const { return prev_clocks[i]; }  
  
  const String &name_to_move() const;
  const String &previous_name_to_move() const;
  sint4 color_index(sint4 color) const;
  sint4 name_index(const String &pl) const;  // maybe ambiguous!
  sint4 index_to_move() const;

  void reset() {
    gpinf[0].reset();
    gpinf[1].reset();
    komi_defined = false;
    komi = 0;
  }

  // access clocks in match req structure
  
  GAME_Clock &clock(uint4 i);
  const GAME_Clock &clock(uint4 i) const;

  String id() const;

  void start_to_move_clock(bool replay);
  void start_both_clocks(bool replay);
  
  void write_join_update_header(std::ostream &os) const;
  void write_clocks(std::ostream &os, bool setting, bool prev = false) const;
  void write_update(std::ostream &os, bool prev = false) const;

  bool play(std::ostream &os, const String &pl, MoveInfo &mi, bool replay = false);
  void replay(); // replay move list

  uint4 move_num() const { return minfos.size(); }
  void undo_last_move();
  
  void reset_offers_and_undos();

  bool is_finished() const;
  bool is_interrupted() const;
  bool matching_offers() const;
  bool resigned() const;
  bool timeout() const;  

  void te_handle(sint4 player_index);
  void create_time_event(sint4 player_index);
  void cancel_time_event(sint4 player_index);

  void handle_timeouts(sint4 player_index, bool replay);
  
  String err_string();

public:
  
  // create new game or continue adjourned game (dependent on p_is == 0)

  Game(Match &m, sint4 id);
  Game(Match &m, sint4 id, std::istream &is);
  Game(Match &m, sint4 id, sint4 color0, const String &pos0);

  bool player_clock_running(const String &name) const;
    
  // extracts info from move_info_time and tries to make move for player name
  bool play(std::ostream &os, const String &pl, const String &move_info_time);

  // player has issued undo, true iff undo allowed
  bool undo(std::ostream &os, const String &pl);

  // player has offered a value
  bool score(std::ostream &os, const String &pl, const String& value);

  // player has resigned
  void resign(const String &player);
  
  void write_ggf(std::ostream &os, bool one_line=false) const;
  bool read_ggf(std::istream &is);

  void mssg_join(std::ostream &os, bool client, bool omit_last_move=false) const;

  ~Game() {}

  real8 blacks_result() const; // returns komi adjusted game result for black
  real8 result0() const;       // result for player 0

  void write_infos();
  
  friend class Match;
};

#endif
