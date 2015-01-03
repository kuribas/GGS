// $Id: RegularBoardGame.H 160 2007-06-22 15:21:10Z mburo $
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

#ifndef RegularBoardGame_H
#define RegularBoardGame_H

#include <iostream>
#include <string>
#include "Global.H"

// newline character
#define EOL '\\'

class RegularBoardGame {

public:

  // values returned by color_to_move()
  enum TURN { BLACK = Color::BLACK, WHITE = Color::WHITE };
  
  // POS = current position
  
  // init POS w.r.t. board_type and rand_type (<0 => no rand)
  virtual void init_pos(int board_type, int rand_type) = 0;

  // init POS from ggf representation
  virtual bool read_pos_ggf(std::istream &is) = 0;

  // color to move in POS (BLACK | WHITE)
  virtual TURN color_to_move() const = 0;

  // write POS in ggf format, use only one line if flag is set
  virtual void write_pos_ggf(std::ostream &os, bool one_line=true) const = 0;

  // write POS in (verbose) text format
  virtual void write_pos_txt(std::ostream &os) const = 0;

  // false iff move_str is illegal in POS (write error message to err),
  virtual bool legal_move( std::ostream &err, const std::string &move_str) = 0;

  // make legal move in POS
  virtual void do_move(const std::string &move_str) = 0;

  // true iff game is finished
  virtual bool game_finished() const = 0;

  // game result for black
  virtual int blacks_result( bool Anti ) const = 0;

  // maximum result in position POS
  virtual int max_result() const = 0;

  // messages sent after updates
  virtual void server_comment( std::string &msg ) { msg.erase(); }
  
  virtual ~RegularBoardGame() {}


  // static functions and data:
  
  // adjusted score for rating update: [-inf,+inf] -> [0,1]
  static double score(double v);

  // read board type, true iff board type is OK, store it as integer
  static bool read_board_type(std::istream &is, int &board_type);

  // read rand type, true iff rand_type is OK for board_type, store it as integer
  static bool read_rand_type(std::istream &is, int board_type, int &rand_type);

  // generates the actual game instance
  static RegularBoardGame *new_game();


  // fundamental game properties. game supports ...
  static const bool HAS_RAND;        // random starting positions
  static const bool HAS_RAND_TYPE;   // parameterized rand mode
  static const bool HAS_KOMI;        // "fair" games using a komi value
  static const bool HAS_ANTI;        // anti mode (negated result)
  static const bool HAS_SYNCHRO;     // synchronized 2-game matches

  static const char* const DEFAULT_CLOCK; // e.g. othello: "15:00/0/2:00"
  
  // result increment (e.g. 2 (Othello), 1 (Amazons))
  static const int RESULT_INCREMENT;  

  // full game name (e.g. "Othello", "Amazons")
  static const char * const GAME_NAME;

  // service login info (login starts with '/')
  static const char * const LOGIN_SERVICE;  // e.g. "/os"
  // static const char * const PASSW_SERVICE; // set in main
  
  // begins with space (e.g. " o.t.h.e.l.l.o ")
  static const char * const LOGIN_SYSTEM;

  // color names
  static const char * const FIRST_COLOR;
  static const char * const SECOND_COLOR;
  
  // dummy function called by main (useful for setting a breakpoint)
  static void gdb();
};

#endif

