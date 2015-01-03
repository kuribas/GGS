// $Id: Message.C 160 2007-06-22 15:21:10Z mburo $
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
//: Message.C (bof) (c) Igor Durdanovic

#include "Message.H"
#include "Actors.H"
#include "VT100.H"

using namespace std;

ostream& Message::err( bool vt, ostream& os, ccptr prefix )
{
  if ( vt ) os << vt_error;
  os << prefix;
  if ( vt ) os << vt_reset;
  return os << ' ';
}

ostream& Message::_010( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "command '" << str << "' not recognized.";
}

ostream& Message::_011( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "command '" << str << "' is VC restricted.";
}

ostream& Message::_012( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "" << str << " [+|-].";
}

ostream& Message::_013( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "command restricted to _admin group.";
}

ostream& Message::_014( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "who <type> [a] [-pts +pts].";
}

ostream& Message::_015( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "uptime [-sys].";
}

ostream& Message::_016( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "user '" << str << "' not found.";
}

ostream& Message::_017( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "your request doesn't fit the opponent's formula.";
}

ostream& Message::_018( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "play <match> <move> [eval] [time].";
}

ostream& Message::_019( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "match with same parameters is already pending.";
}

ostream& Message::_020( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "bell (+|-)(r|p|w|n|ns|nn|nt|ni|nr|nw|ta|to|tp).";
}

ostream& Message::_021( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "accept <id>.";
}

ostream& Message::_022( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "request '" << str << "' not found.";
}

ostream& Message::_023( bool vt, ostream& os, const String& cmd )
{
  return err( vt, os, "ERR" ) << cmd << " <match>.";
}

ostream& Message::_024( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "Player is not accepting new matches.";
}

ostream& Message::_025( bool vt, ostream& os, const String& cmd, const String& num )
{
  return err( vt, os, "ERR" ) << cmd << " <n != " << num << ">.";
}

ostream& Message::_026( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "Your open value is too low for new matches.";
}

ostream& Message::_027( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you can not use 'alias' as a name for an alias.";
}

ostream& Message::_028( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a registered user.";
}

ostream& Message::_029( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "score <id> <your score>.";
}

ostream& Message::_030( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you cannot accept your own match request.";
}

ostream& Message::_031( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper histogram option: h|d|m|y.";
}

ostream& Message::_032( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "no archived match '" << str << "' found.";
}

ostream& Message::_033( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "archived match '" << str << "' is not yours.";
}

ostream& Message::_034( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "no opponent present for archived match '" << str << "'.";
}

ostream& Message::_035( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "top <type> [num]";
}

ostream& Message::_036( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "rank <type> [login]";
}

ostream& Message::_037( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "match '" << str << "' not found.";
}

ostream& Message::_038( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "assess <.id> | (<type> <login> [login]) <score>.";
}

ostream& Message::_039( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "the opponent is ignoring you.";
}

ostream& Message::_040( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you are ignoring the opponent.";
}

ostream& Message::_041( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you have too many stored matches.";
}

ostream& Message::_042( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "the opponent has too many stored matches.";
}

ostream& Message::_043( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "shistory.";
}

ostream& Message::_044( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "tell [o] <.id> [mssg].";
}

ostream& Message::_045( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "rating type '" <<  str << "' unknown.";
}

ostream& Message::_046( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you are not registered.";
}

ostream& Message::_047( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "rated variable mismatch.";
}

ostream& Message::_048( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "ignore [%] | [[+|-] <user> .. <user>].";
}

ostream& Message::_049( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_ignore << " ignores.";
}

ostream& Message::_050( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "ignore " << str << " not found.";
}

ostream& Message::_051( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "notify [*|%] | [[+|-] <user> .. <user>].";
}

ostream& Message::_052( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_notify << " notify.";
}

ostream& Message::_053( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "notify " << str << " not found.";
}

ostream& Message::_054( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "track [*|%] | [[+|-] <user> .. <user>].";
}

ostream& Message::_055( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_track << " track.";
}

ostream& Message::_056( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "track " << str << " not found.";
}

ostream& Message::_057( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "watch [*|%] | [[+|-] <user> .. <user>].";
}

ostream& Message::_058( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper group name.";
}

ostream& Message::_059( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "watch " << str << " not found.";
}

ostream& Message::_060( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "command restricted to root.";
}

ostream& Message::_061( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "pack syntax.";
}

ostream& Message::_062( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "command restricted to _td group.";
}

ostream& Message::_063( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "group syntax error.";
}

ostream& Message::_064( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "group '" << str << "' not found.";
}

ostream& Message::_065( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " can not start, player(s) is/are absent!?";
}

ostream& Message::_066( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " tdstart syntax.";
}

ostream& Message::_067( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " archived match not found.";
}

ostream& Message::_068( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " clock syntax.";
}

ostream& Message::_069( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " match type; ";
}

ostream& Message::_070( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " self playing.";
}

ostream& Message::_071( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "TDERR" ) << str << " player(s) is/are not registered.";
}

ostream& Message::_072( bool vt, ostream& os )
{
  return err( vt, os, "TDERR" ) << "tdtype <td.id> <type>.";
}

ostream& Message::_073( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you can not adjourn tournament game.";
}

ostream& Message::_074( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you can not abort tournament game.";
}

ostream& Message::_075( bool vt, ostream& os, const String& str  )
{
  return err( vt, os, "TDERR" ) << "match " << str << " is not tournament match.";
}

ostream& Message::_076( bool vt, ostream& os, const String& str  )
{
  return err( vt, os, "TDERR" ) << "match " << str << " is not under your juridisction.";
}

ostream& Message::_077( bool vt, ostream& os, const String& str  )
{
  return err( vt, os, "ERR" ) << str << " is a tournament match.";
}

ostream& Message::_078( bool vt, ostream& os  )
{
  return err( vt, os, "ERR" ) << "ask <.stored> | <type> <myclock> [ oppclock [ opponent ] ].";
}

//: Message.C (eof) (c) Igor Durdanovic
