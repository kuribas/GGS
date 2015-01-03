// $Id: VC_Request.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, igord@research.nj.nec.com
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
//: VC_Request.H (bof) (c) Igor Durdanovic

#ifndef VC_Request_H
#define VC_Request_H

#include "Array_N.H"
#include "String.H"
#include "SET_id.H"
#include "TIME_Client.H"
#include "GAME_Clock.H"
#include "Game.H"
#include "MatchType.H"

class Client;
class VC_Match;

// match <type> [my-time [opp-time]] <opponent>|(-pts +pts)
// match -load <counter>

class Request  // content holder
{
public:
  /* */ bool    synchro() const { return t.is_synchro_game(); }
  const String& savedid() const { return counter; }
  /* */ bool      saved() const { return !counter.empty(); }
  /* */ sint4    stored() const;
  /* */ bool      rated() const { return is_rated; }
  /* */ bool       rand() const { return t.is_rand_game(); }
  /* */ bool       komi() const { return t.is_komi_game(); }
  /* */ bool       anti() const { return t.is_anti_game(); }
  /* */ sint4     discs() const { return t.get_rand_type(); }
  /* */ sint4      size() const { return t.get_board_type(); }
  const MatchType& type() const { return t; }
  const String&      td() const { return t_d; }
  const String&     tid() const { return t_id; }
  /* */ GAME_Clock& clk( uint4 i ) /* */ { return c[i]; }
  const GAME_Clock& clk( uint4 i ) const { return c[i]; }
  /* */ char      color( uint4 i ) const;
  const String&    name( uint4 i ) const;
  /* */ real8    rating( uint4 i ) const;
  /* */ real8    ratdev( uint4 i ) const;
  
  bool   parse( Client* P1, const String& arg, bool NoError );
  bool tdparse( Client* TD, const String& arg );
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is, bool Ghost );

public:
  sint4       refcnt; // reference counter ... needed for (s)history
  String     counter;
  bool       is_rated;
  Client*         p1; // players
  mutable Client* p2; // 0 -> global with rating range
  String          n1; // will be set when match starts!
  String          n2; // p1,p2 may not be available in load()
  Array_N< GAME_Clock, 4 > c; // clocks
  MatchType        t; // actual game type
  String         t_d; // tournament director -- must be saved
  String        t_id; // tournament id       -- won't be saved
};

inline std::ostream& operator << ( std::ostream& os, const Request& R ) { return R.print(os); }


class VC_Request // VC wrapper
{
public:
   VC_Request( const Request& R );
  ~VC_Request();

  const String& id() const { return sid; }
    
  static VC_Match*   parse( Client* P1, const String& arg, bool NoError );
  static VC_Match* tdparse( Client* TD, const String& arg );

  void notify( std::ostringstream* ostr, bool* rbool ) const;
  
  std::ostream& print( std::ostream& os ) const;

private:
  sint4   rid;
  String  sid;
  Request req;
  
  static SET_id<64> id_cnt;

  static bool global_find_client           ( /* */    Request& r );
  friend bool local_duplicated             ( const VC_Request& R1, const Request& r2 );
  friend bool globl_duplicated             ( const VC_Request& R1, const Request& r2 );
  friend bool local_recv_eq_this_local_send( const VC_Request& R1, const Request& r2 );
  friend bool globl_send_eq_this_local_send( const VC_Request& R1, const Request& r2 );
  friend bool local_recv_eq_this_globl_send( const VC_Request& R1, const Request& r2 );
  friend bool globl_send_eq_this_globl_send( const VC_Request& R1, const Request& r2 );
  
  friend class EXE_Service;
};

inline bool operator <  ( const VC_Request& A, const VC_Request& B )
{
  return A.id() <  B.id();
}
inline bool operator == ( const VC_Request& A, const VC_Request& B )
{
  return A.id() == B.id();
}
inline bool operator <  ( const VC_Request& A, const String&     B )
{
  return A.id() <  B;
}
inline bool operator == ( const VC_Request& A, const String&     B )
{
  return A.id() == B;
}
inline std::ostream& operator << ( std::ostream& os, const VC_Request& R )
{
  return R.print(os);
}

#endif

//: VC_Request.H (eof) (c) Igor
