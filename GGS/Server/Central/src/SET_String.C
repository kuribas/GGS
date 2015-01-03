// $Id: SET_String.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_String.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "SET_Group.H"
#include "SET_String.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

const String SET_String::tag( "SET_String" );

SET_String& SET_String::add( const SET_String& Set )
{
  TSTAT;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) (*this) += *it;
  return *this;
}

SET_String& SET_String::del( const SET_String& Set )
{
  TSTAT;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) (*this) -= *it;
  return *this;
}

ostream& SET_String::print( ostream& os, bool Hide, bool NewLine ) const
{
  TSTAT;
  
  static String hide( ".." );

  const_iterator it = begin();
  const_iterator hi = end();
  bool any = false;

  if ( it != hi ) {
    for ( ; it != hi; ++it ) {
      const String& str = *it;
      if ( Hide && str.find( hide, 0 ) == 0 ) continue; // hide ..channs
      if ( any ) os << ' ';
      os << str;
      any = true;
    }
  }

  if ( any && NewLine ) os << EOL;

  return os;
}

ostream& SET_String::save( ostream& os ) const
{
  TSTAT;

  tag.tag_save( os );

  return SET<String>::save( os );
}

istream& SET_String::load( istream& is )
{
  TSTAT;

  if (! tag.tag_chck( is ) ) { vc_con << VCFL; System::exit(-1); }

  return SET<String>::load( is );
}

//: SET_String.C (eof) (c) Igor
