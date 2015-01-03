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

#include <iostream>
#include "String.H"

class Message
{
private:
  std::ostream&  err( bool vt, std::ostream& os, ccptr prefix );
  
public:
  std::ostream& _010( bool vt, std::ostream& os, const String& str ); // command unknown
  std::ostream& _011( bool vt, std::ostream& os, const String& str ); // VC command
  std::ostream& _012( bool vt, std::ostream& os, const String& str ); // syntax cmd [+|-]
  std::ostream& _013( bool vt, std::ostream& os ); // admin command
  std::ostream& _014( bool vt, std::ostream& os ); // syntax who
  std::ostream& _015( bool vt, std::ostream& os ); // syntax uptime
  std::ostream& _016( bool vt, std::ostream& os, const String& str ); // user unknown
  std::ostream& _017( bool vt, std::ostream& os ); // you don't fit formula
  std::ostream& _018( bool vt, std::ostream& os ); // play syntax
  std::ostream& _019( bool vt, std::ostream& os ); // duplicted match 
  std::ostream& _020( bool vt, std::ostream& os ); // syntax bell
  std::ostream& _021( bool vt, std::ostream& os ); // syntax accept 
  std::ostream& _022( bool vt, std::ostream& os, const String& str ); // request id unknown
  std::ostream& _023( bool vt, std::ostream& os, const String& cmd ); // cmd <match>
  std::ostream& _024( bool vt, std::ostream& os ); // opp not open
  std::ostream& _025( bool vt, std::ostream& os, const String& str, const String& num ); // cmd <num> syntax
  std::ostream& _026( bool vt, std::ostream& os ); // you not open
  std::ostream& _027( bool vt, std::ostream& os ); // aliasing alias
  std::ostream& _028( bool vt, std::ostream& os, const String& str ); // user unregistered
  std::ostream& _029( bool vt, std::ostream& os ); // score syntax
  std::ostream& _030( bool vt, std::ostream& os ); // accepting own match request
  std::ostream& _031( bool vt, std::ostream& os, const String& str ); // histogram <h|d|m|y>
  std::ostream& _032( bool vt, std::ostream& os, const String& str ); // archived not found
  std::ostream& _033( bool vt, std::ostream& os, const String& str ); // archived not yours
  std::ostream& _034( bool vt, std::ostream& os, const String& str ); // archived no opponent
  std::ostream& _035( bool vt, std::ostream& os ); // syntax top
  std::ostream& _036( bool vt, std::ostream& os ); // syntax rank
  std::ostream& _037( bool vt, std::ostream& os, const String& str ); // match <id> not found
  std::ostream& _038( bool vt, std::ostream& os ); // syntax assess
  std::ostream& _039( bool vt, std::ostream& os ); // opp is ignoring you
  std::ostream& _040( bool vt, std::ostream& os ); // you are ignoring opp
  std::ostream& _041( bool vt, std::ostream& os ); // your stored is full
  std::ostream& _042( bool vt, std::ostream& os ); // opp  stored is full
  std::ostream& _043( bool vt, std::ostream& os ); // syntax shistory
  std::ostream& _044( bool vt, std::ostream& os ); // syntax tell
  std::ostream& _045( bool vt, std::ostream& os, const String& str ); // illegal rating key
  std::ostream& _046( bool vt, std::ostream& os ); // you are not registered
  std::ostream& _047( bool vt, std::ostream& os ); // rated mismatch
  std::ostream& _048( bool vt, std::ostream& os ); // ignore syntax
  std::ostream& _049( bool vt, std::ostream& os ); // max ignores
  std::ostream& _050( bool vt, std::ostream& os, const String& str ); // ignore not found
  std::ostream& _051( bool vt, std::ostream& os ); // notify syntax
  std::ostream& _052( bool vt, std::ostream& os ); // max notify
  std::ostream& _053( bool vt, std::ostream& os, const String& str ); // notify not found
  std::ostream& _054( bool vt, std::ostream& os ); // track syntax
  std::ostream& _055( bool vt, std::ostream& os ); // max track
  std::ostream& _056( bool vt, std::ostream& os, const String& str ); // track not found
  std::ostream& _057( bool vt, std::ostream& os ); // watch syntax
  std::ostream& _058( bool vt, std::ostream& os, const String& str ); // group name
  std::ostream& _059( bool vt, std::ostream& os, const String& str ); // watch not found
  std::ostream& _060( bool vt, std::ostream& os ); // root command
  std::ostream& _061( bool vt, std::ostream& os ); // pack syntax
  std::ostream& _062( bool vt, std::ostream& os ); // td command
  std::ostream& _063( bool vt, std::ostream& os ); // group syntax
  std::ostream& _064( bool vt, std::ostream& os, const String& str ); // group not found
  std::ostream& _065( bool vt, std::ostream& os, const String& str ); // td - player(s) absent
  std::ostream& _066( bool vt, std::ostream& os, const String& str ); // td - start syntax
  std::ostream& _067( bool vt, std::ostream& os, const String& str ); // td - archived match missing
  std::ostream& _068( bool vt, std::ostream& os, const String& str ); // td - clock syntax
  std::ostream& _069( bool vt, std::ostream& os, const String& str ); // td - match type
  std::ostream& _070( bool vt, std::ostream& os, const String& str ); // td - self play
  std::ostream& _071( bool vt, std::ostream& os, const String& str ); // td - unregistered player(s)
  std::ostream& _072( bool vt, std::ostream& os ); // tdtype syntax
  std::ostream& _073( bool vt, std::ostream& os ); // adjourn on T game
  std::ostream& _074( bool vt, std::ostream& os ); // abort on T game
  std::ostream& _075( bool vt, std::ostream& os, const String& str ); // td - not T game
  std::ostream& _076( bool vt, std::ostream& os, const String& str ); // td - not TD game
  std::ostream& _077( bool vt, std::ostream& os, const String& str ); // ask on td game
  std::ostream& _078( bool vt, std::ostream& os ); // ask ""
};

#endif

//: Message.H (eof) (c) Igor Durdanovic
