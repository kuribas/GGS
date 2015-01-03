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
#include "FLAG_Bell.H"
#include "FLAG_Verb.H"
#include "SET_Alias.H"
#include "SET_String.H"
#include "VC_History.H"
#include <sstream>

class VAR_Client
{
public:
   VAR_Client() : vt100(false), hear(true), history( 8 ), dirty(false) {}
  ~VAR_Client() { save(); }

  String     login;
  String     passw;
  String     name;
  String     info;
  String     email;
  sint4      time;
  sint4      last;
  String     host_ip;
  String     host_name;
  sint4      sock;
  sint4      port;
  SET_Alias  alias;
  SET_String groups;
  SET_String channs;
  SET_String notify;
  SET_String ignore;
  bool       star_notify;
  bool       notify_groups;
  bool       notify_channs;
  bool       notify_notify;
  bool       notify_ignore;
  FLAG_Bell bell;
  bool       vt100;
  bool       hear;
  FLAG_Verb  verbose;
  VC_History history;
  SET_Alias  vars;
  String     prefix;
  
  void init();

  void make( const String& Login, 
	     const String& Passw, 
	     const String& Name,  
	     const String& Email );

  void modified() { dirty = true; }

  sint4 admin() const;

  std::ostream& print( std::ostream& os, const VAR_Client& P ) const;

  void erase();

  void save( std::ostream& os ) const;
  void save() const;
  void load( const String& Login );

  void ios(); // convert from IOS
  
protected:
  mutable bool   dirty;
  mutable CRatio dblen;

  friend class IO_TCP_VAR_Client;
};

#endif

//: VAR_Client.H (eof) (c) Igor
