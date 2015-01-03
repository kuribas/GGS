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
  os << ": ";
  if ( vt ) os << vt_error;
  os << prefix;
  if ( vt ) os << vt_reset;
  if ( prefix[0] != 0 ) os << ' ';
  return os;
}

ostream& Message::_001( bool /*vt*/, ostream& os )
{
  return err( false, os, "" ) << "Enter login (yours, or one you'd like to use)." << EOL;
}

ostream& Message::_002( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "Login (2-8 chars) can not start with '.' or '_' chars and can not contain ',' or '\\' chars." << EOL;
}

ostream& Message::_004( bool /*vt*/, ostream& os )
{
  return err( false, os, "" ) << "Enter your password." << EOL;
}

ostream& Message::_005( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "Password is incorrect." << EOL;
}

ostream& Message::_006( bool /*vt*/, ostream& os )
{
  return err( false, os, "" ) << "Thank you for using GGS." << EOL;
}

//

ostream& Message::_010( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "command '" << str << "' not recognized." << EOL;
}

ostream& Message::_011( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "string longer than " << max_str_len << " chars." << EOL;
}

ostream& Message::_012( bool vt, ostream& os, uint4 max_no )
{
  return err( vt, os, "ERR" ) << "max. " << max_no << " definitions." << EOL;
}

ostream& Message::_013( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << str << " [+|-]." << EOL;
}

ostream& Message::_014( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << str << " <num>." << EOL;
}

ostream& Message::_015( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "passw <new> <old> [login]." << EOL;
}

ostream& Message::_016( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "user '" << str << "' not found." << EOL;
}

ostream& Message::_017( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "command restricted to _admin group." << EOL;
}

ostream& Message::_018( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "tell <who1,who2,..> [mssg] ; who = .chann | _group | user." << EOL;
}

ostream& Message::_019( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "you must listen to '" << str << "' if you want to talk on it." << EOL;
}

ostream& Message::_020( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << ": BUG: channel '" << str << "' not found !?" << EOL;
}

ostream& Message::_021( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "only admins can talk to a _group." << EOL;
}

ostream& Message::_022( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << ": BUG: group '" << str << "' not found !?" << EOL;
}

ostream& Message::_023( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "incorrect password or insufficient admin level." << EOL;
}

ostream& Message::_024( bool /*vt*/, ostream& os, const String& str )
{
  return err( false, os, "" ) << "User " << str << " disconnected you." << EOL;
}

ostream& Message::_025( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "channel '" << str << "' not found." << EOL;
}

ostream& Message::_026( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper channel name." << EOL;
}

ostream& Message::_027( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_chann << " channels." << EOL;
}

ostream& Message::_028( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "group '" << str << "' not found." << EOL;
}

ostream& Message::_029( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is an internal group." << EOL;
}

ostream& Message::_030( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper group name." << EOL;
}

ostream& Message::_031( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is|are not a registered user." << EOL;
}

ostream& Message::_032( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper alias name." << EOL;
}

ostream& Message::_033( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "bell (+|-)(t|tc|tg|n|nc|ng|ni|nn)." << EOL;
}

ostream& Message::_034( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "exec <user> <command>." << EOL;
}

ostream& Message::_035( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "name <user> [name]." << EOL;
}

ostream& Message::_036( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "email <login> [email]." << EOL;
}

ostream& Message::_037( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "name must be set first." << EOL;
}

ostream& Message::_038( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "uptime [-sys]." << EOL;
}

ostream& Message::_039( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "who." << EOL;
}

ostream& Message::_040( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "command restricted to root." << EOL;
}

ostream& Message::_041( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "only root can alter _admin group." << EOL;
}

ostream& Message::_042( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' will not hear you." << EOL;
}

ostream& Message::_043( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "shistory." << EOL;
}

ostream& Message::_044( bool vt, ostream& os, const String& host, const String& port )
{
  return err( vt, os, "ERR" ) << "multiplexer '" << host << ' ' << port << "' not connected." << EOL;
}

ostream& Message::_045( bool vt, ostream& os, const String& host, const String& port )
{
  return err( vt, os, "ERR" ) << "multiplexer '" << host << ' ' << port << "' already active." << EOL;
}

ostream& Message::_046( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "port missing in multiplexer specification." << EOL;
}

ostream& Message::_047( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "compression's method '" << str << "' unknown." << EOL;
}

ostream& Message::_048( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "verbose (+|-)(news|ack|help|faq)." << EOL;
}

ostream& Message::_049( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "you can not use command names as alias names." << EOL;
}

ostream& Message::_050( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "chann [*|%] | [[+|-] <chann> .. <chann>]." << EOL;
}

ostream& Message::_051( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "ignore [%] | [[+|-] <user> .. <user>]." << EOL;
}

ostream& Message::_052( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_ignore << " ignores." << EOL;
}

ostream& Message::_053( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "notify [*|%] | [[+|-] <user> .. <user>]." << EOL;
}

ostream& Message::_054( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "notify '" << str << "' not found." << EOL;
}

ostream& Message::_055( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "ignore '" << str << "' not found." << EOL;
}

ostream& Message::_056( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "max " << max_notify << " notifys." << EOL;
}

ostream& Message::_057( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "pack." << EOL;
}

ostream& Message::_058( bool vt, ostream& os )
{
  return err( vt, os, "ERR" ) << "passw must be set first." << EOL;
}

ostream& Message::_059( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper variable name." << EOL;
}

ostream& Message::_060( bool vt, ostream& os, const String& str )
{
  return err( vt, os, "ERR" ) << "'" << str << "' is not a proper histogram option: h|d|m|y." << EOL;
}

//: Message.C (eof) (c) Igor
