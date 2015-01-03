// $Id: Message.H 160 2007-06-22 15:21:10Z mburo $
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
//: Message.H (bof) (c) Igor Durdanovic

#ifndef Message_H
#define Message_H

#include "String.H"

class Message
{
private:
  std::ostream&  err( bool vt, std::ostream& os, ccptr prefix );
  
public:
  std::ostream& _001( bool vt, std::ostream& os ); // login
  std::ostream& _002( bool vt, std::ostream& os ); // login syntax
  // free _003
  std::ostream& _004( bool vt, std::ostream& os ); // your password
  std::ostream& _005( bool vt, std::ostream& os ); // password wrong
  std::ostream& _006( bool vt, std::ostream& os ); // VC disconnected
  
  std::ostream& _010( bool vt, std::ostream& os, const String& str ); // command unknown
  std::ostream& _011( bool vt, std::ostream& os ); // max string length
  std::ostream& _012( bool vt, std::ostream& os, uint4 max_no ); // max. # definitions
  std::ostream& _013( bool vt, std::ostream& os, const String& str ); // cmd [+|-] syntax
  std::ostream& _014( bool vt, std::ostream& os, const String& str ); // cmd <num> syntax
  std::ostream& _015( bool vt, std::ostream& os ); // passw syntax
  std::ostream& _016( bool vt, std::ostream& os, const String& str ); // user not found
  std::ostream& _017( bool vt, std::ostream& os ); // _admin command
  std::ostream& _018( bool vt, std::ostream& os ); // tell syntax
  std::ostream& _019( bool vt, std::ostream& os, const String& str ); // tell .chann restricted
  std::ostream& _020( bool vt, std::ostream& os, const String& str ); // bug, .chann not found
  std::ostream& _021( bool vt, std::ostream& os ); // tell _group restricted
  std::ostream& _022( bool vt, std::ostream& os, const String& str ); // bug, _group not found
  std::ostream& _023( bool vt, std::ostream& os ); // quit failed
  std::ostream& _024( bool vt, std::ostream& os, const String& str ); // you are disconnected
  std::ostream& _025( bool vt, std::ostream& os, const String& str ); // err, .chann not found
  std::ostream& _026( bool vt, std::ostream& os, const String& str ); // .chann name
  std::ostream& _027( bool vt, std::ostream& os ); // max .channs
  std::ostream& _028( bool vt, std::ostream& os, const String& str ); // err, _group not found
  std::ostream& _029( bool vt, std::ostream& os, const String& str ); // VC group
  std::ostream& _030( bool vt, std::ostream& os, const String& str ); // _group name
  std::ostream& _031( bool vt, std::ostream& os, const String& str ); // user unregistered
  std::ostream& _032( bool vt, std::ostream& os, const String& str ); // improper alias name
  std::ostream& _033( bool vt, std::ostream& os ); // bell syntax
  std::ostream& _034( bool vt, std::ostream& os ); // xtell syntax
  std::ostream& _035( bool vt, std::ostream& os ); // name syntax
  std::ostream& _036( bool vt, std::ostream& os ); // email syntax
  std::ostream& _037( bool vt, std::ostream& os ); // name not set
  std::ostream& _038( bool vt, std::ostream& os ); // uptime syntax
  std::ostream& _039( bool vt, std::ostream& os ); // who syntax
  std::ostream& _040( bool vt, std::ostream& os ); // root command
  std::ostream& _041( bool vt, std::ostream& os ); // _admin group
  std::ostream& _042( bool vt, std::ostream& os, const String& str ); // user is ignoring you
  std::ostream& _043( bool vt, std::ostream& os ); // shistory syntax
  std::ostream& _044( bool vt, std::ostream& os, const String& host, const String& port ); // mux not found
  std::ostream& _045( bool vt, std::ostream& os, const String& host, const String& port ); // mux is active
  std::ostream& _046( bool vt, std::ostream& os ); // mux port not specified
  std::ostream& _047( bool vt, std::ostream& os, const String& str ); // method undefined
  std::ostream& _048( bool vt, std::ostream& os ); // verbose syntax
  std::ostream& _049( bool vt, std::ostream& os ); // aliasing commands
  std::ostream& _050( bool vt, std::ostream& os ); // chann syntax
  std::ostream& _051( bool vt, std::ostream& os ); // ignore syntax
  std::ostream& _052( bool vt, std::ostream& os ); // max ignoreds
  std::ostream& _053( bool vt, std::ostream& os ); // notify syntax
  std::ostream& _054( bool vt, std::ostream& os, const String& str ); // notifyee not found
  std::ostream& _055( bool vt, std::ostream& os, const String& str ); // ignoree not found
  std::ostream& _056( bool vt, std::ostream& os ); // max notifys
  std::ostream& _057( bool vt, std::ostream& os ); // pack syntax
  std::ostream& _058( bool vt, std::ostream& os ); // passw not set
  std::ostream& _059( bool vt, std::ostream& os, const String& str ); // improper var name
  std::ostream& _060( bool vt, std::ostream& os, const String& str ); // histo <h|d|m|y>
};

#endif

//: Message.H (eof) (c) Igor
