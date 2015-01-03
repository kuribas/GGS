// $Id: Alias.C 9037 2010-07-06 04:05:44Z mburo $
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
//: Alias.C (bof) (c) Igor Durdanovic

#include "Alias.H"
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

bool Alias::alias_name_ok( const String& s )
{
  TSTAT;
  
  if ( s.empty()    ) return false;
  if ( s.size() > 8 ) return false;
  String::const_iterator it = s.begin();
  String::const_iterator hi = s.end();
  for ( ; it != hi; ++it ) if (! isprint(*it) ) return false;
  return true;
}

bool Alias::var_name_ok( const String& s )
{
  TSTAT;
  
  if ( s.empty()    ) return false;
  if ( s.size() > 8 ) return false;
  sint4 i4; bool ok = sint4_parse( s, false, i4 ); if ( ok ) return false;
  if ( s == "-" ) return false;
  String::const_iterator it = s.begin();
  String::const_iterator hi = s.end();
  for ( ; it != hi; ++it ) if (! isprint(*it) ) return false;
  return true;
}

ostream& Alias::print( ostream& os ) const
{
  TSTAT;
  
  const String& name = *this;
  Form( os << EOL, "%-8s %s", name.c_str(), obj.c_str() );
  return os;
}

ostream& Alias::print( ostream& os, const String& Prefix ) const
{
  TSTAT;
  
  const String& name = *this;
  if ( name.size() < Prefix.size() ) return os;
  if ( strncmp( name.data(), Prefix.data(), Prefix.size() ) != 0 ) return os;
  Form( os << EOL, "%-8s %s", name.c_str(), obj.c_str() );
  return os;
}

//: Alias.C (eof) (c) Igor
