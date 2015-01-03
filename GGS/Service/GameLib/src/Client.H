// $Id: Client.H 160 2007-06-22 15:21:10Z mburo $
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
//: Client.H (bof) (c) Igor Durdanovic

#ifndef Client_H
#define Client_H

#include "IO_Buffer.H"
#include "VAR_Client.H"
#include <sstream>

class Client
  : public std::ostream
{ 
public:
   Client( bool Ghost = false );
  ~Client();

  void alive2ghost();
  void ghost2alive();
  
  void ghost2void (); // kills ghost if not needed any more
  
  Client& text( std::ostringstream& os );
  Client& mssg( const String& file );
  
  Client& tell(); // generates "tell <client>
  Client& bell( bool On ); 
  
  const String& id() const { return var.login; }
  bool       vt100() const { return var.vt100; }
  Client&    vt100( const String& prefix, ccptr         mssg );
  Client&    vt100( const String& prefix, const String& mssg );
  Client&    vt100( const String& prefix, std::ostringstream& mssg );
  bool        dtor() const { return dtor_; }

  bool ignore( const Client* whom ) const;

  bool ghost() const { return var.ghost; }
  bool in_tournament() const;
  
  void clean_requests(); // remove all requests related to Client
  void clean_matches ( bool All ); // remove all (non-tour) matches
  void clean_watches (); // remove all watched games
  
  /* */ real8 rank( const String& key ) { return var.rating( key ).Rank(); }
  /* */ real8 rdev( const String& key ) { return var.rating( key ).Rdev(); }
    
  sint4 admin() const { return var.admin; }

  sint4 playing( bool rand ) const;
  
  VAR_Client var;
  
private:
  IO_Buffer send_buff;
  bool      dtor_;
  
  friend class VAR_Service;
};

inline bool operator <  ( const Client& p1, const Client& p2 ) { return p1.id() <  p2.id(); }
inline bool operator == ( const Client& p1, const Client& p2 ) { return p1.id() == p2.id(); }

inline bool operator <  ( const Client& p1, const String& p2 ) { return p1.id() <  p2; }
inline bool operator == ( const Client& p1, const String& p2 ) { return p1.id() == p2; }

#endif

//: Client.H (eof) (c) Igor
