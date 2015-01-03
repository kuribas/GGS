// $Id: VAR_System.H 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_System.H (bof) (c) Igor Durdanovic

#ifndef VAR_System_H
#define VAR_System_H

#include "SET_Group.H"
#include "SET_PTR_Client.H"
#include "VAR_Client.H"
#include "VC_History.H"
#include "Histogram_HDMY.H"

#include <sstream>

class VAR_System
{ 
public:
   VAR_System();
  ~VAR_System();

  void init();

  void add( IO_TCP_Client* Who ); // updates system when
  void del( IO_TCP_Client* Who ); // user (dis)connects

  void modified() { dirty = true; }

  void erase();
  void load();
  void save( std::ostream& os ) const;
  void save() const;

  static bool login_ok( const String& Login );
  static bool  name_ok( const String& Name );

  IO_TCP_Client* client( const String& user );
  VAR_Client*       var( const String& user );

protected:
  SET_Alias      alias;  // permanent
  SET_Group      groups;
  VC_History     history;
  Histogram_HDMY histo_users;
  Histogram_HDMY histo_inp_bytes;
  Histogram_HDMY histo_out_bytes;

  SET_PTR_Client clients;   // temporary
  String            host;
  SET_Group       channs;
  SET_Group       ignore;
  SET_Group       notify;
  SET_String star_notify;
  mutable bool    dirty;
  
private:
  static VAR_Client svar;

  friend class EXE_Client;
  friend class IO_TCP_Client;
  friend class VC_SEQ;
  friend class Stat;
};

#endif

//: VAR_System.H (eof) (c) Igor
