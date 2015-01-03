// $Id: VAR_Client.H 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Client.H (bof) (c) Igor Durdanovic

#ifndef VAR_Client_H
#define VAR_Client_H

#include "CRatio.H"
#include "String.H"
#include "FLAG_Bell.H"
#include "SET_String.H"
#include "SET_Rating.H"
#include "GAME_Stored.H"
#include "GAME_History.H"
#include "VC_Formula.H"

class Client;

class VAR_Client
{ 
public:
   VAR_Client() : ghost(true), history(32), dirty(false) { erase(); }
  ~VAR_Client();

  void modified() { dirty = true; }
  void erase();
  void make( const String& Login, sint4 Admin );
  void save ( std::ostream& os ) const;
  void save () const;
  bool load ( const String& Login );

  std::ostream& print_who   ( std::ostream& os, Client& Who, const String& OT );
  std::ostream& print_finger( std::ostream& os ) const;

  static void ios();
  static void sgf();

  bool            ghost; // the user is not here!!
  String          login;
  sint4           admin;
  bool           client;
  uint4            open; // open for how many matches at once
  bool            trust;
  bool            rated;
  FLAG_Bell        bell;
  bool            vt100;
  bool             hear;
  bool    notify_stored;
  bool    notify_track;
  bool    notify_ignore;
  bool    notify_notify;
  bool    notify_watch;
  bool    notify_request; // notify about global requests
  bool      star_notify;
  bool      star_track;
  SET_String      track;
  SET_String     ignore;
  SET_String     notify;
  SET_Rating     rating;
  GAME_Stored    stored;
  GAME_History  history;
  VC_Formula   aformula; // auto accept
  VC_Formula   dformula; // auto decline

  mutable bool    dirty; // temporary variables
  mutable CRatio  dblen;
  String    tell_prefix; 
  SET_String       send;
  SET_String       recv;
  SET_String       play;
  SET_String      watch;
};

inline bool operator <  ( const VAR_Client& p, const VAR_Client& q )
{
  return p.login <  q.login;
}
inline bool operator == ( const VAR_Client& p, const VAR_Client& q )
{
  return p.login == q.login;
}
inline bool operator <  ( const VAR_Client& p, const String&     q )
{
  return p.login <   q;
}
inline bool operator == ( const VAR_Client& p, const String&     q )
{
  return p.login ==  q;
}

#endif

//: VAR_Client.H (eof) (c) Igor
