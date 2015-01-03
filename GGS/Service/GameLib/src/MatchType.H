// $Id: MatchType.H 160 2007-06-22 15:21:10Z mburo $
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

#ifndef MatchType_H
#define MatchType_H

#include "CPP.H"
#include "String.H"
#include "Game.H"


class MatchType
{

private:

  // update save/load if data members change!!!

  sint4 board_type;
  sint4 rand_type; // >= 0 <=> is rand game
  bool  komi_game;
  bool  anti_game;
  bool  synchro_game;
  sint4 pref_color;     // color request of issuing player
                        // UNDEF => random colors


  // to be adjusted if datamembers change (mt excluded)
  
  bool equal_type(const MatchType &t) const {
    if (board_type != t.board_type) return false;
    if (rand_type != t.rand_type) return false;
    if (komi_game  != t.komi_game)  return false;
    return anti_game == t.anti_game;
  }

  String to_string(bool with_komi, real4 komi) const;

public:

  String td_data() const;

  void init() {
    board_type = -1;
    rand_type = -1;    
    komi_game = false; 
    anti_game = false; 
    synchro_game = false;   
    pref_color = Color::UNDEF;
  }
  
  MatchType() { init(); }
  
  ~MatchType() {}
  
  bool parse(std::ostream &erros, std::istream &is);
  String key() const;  // return game type key (rating, files etc)

  String to_string() const;
  String to_string_with_komi(real4 komi) const;

  sint4 get_board_type() const { return board_type; }
  sint4 get_rand_type()  const { return rand_type; }
  sint4 get_pref_color() const { return pref_color; }
  bool  is_synchro_game()const;
  bool  is_rand_game()   const;
  bool  is_anti_game()   const;
  bool  is_komi_game()   const;

  static bool normalize_key(String &s);

  // 100% equal
  
  bool is_equal(const MatchType &t) const;

  // equal type & preferred colors match
  
  bool is_matching(const MatchType &t) const;

  bool players_ok(std::ostream &os, const String &p1, const String &p2) const
  {
    if (is_synchro_game() && p1 == p2) {
      os << "different players in synchro games";
      return false;
    }

    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const MatchType &gt);

  // update these functions if data members change!!!

  std::ostream &save(std::ostream &os) const
  {
    return os.write(ccptr(this), sizeof(*this));
  }
  std::istream &load(std::istream &is)
  {
    return is.read(cptr(this), sizeof(*this));
  }
};

#endif
