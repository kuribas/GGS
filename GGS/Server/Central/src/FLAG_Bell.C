// $Id: FLAG_Bell.C 9037 2010-07-06 04:05:44Z mburo $
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
//: FLAG_Bell.C (bof) (c) Igor Durdanovic

#include "FLAG_Bell.H"
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ccptr FLAG_Bell::name[ FLAG_Bell::len ] = { "t", "tc", "tg", "n", "nc", "ng", "ni", "nn" };

bool FLAG_Bell::parse( const String& S ) // (+|-)(t|tc|tg|n|nc|ng|ni|nn)
{
  TSTAT;
  
  String parm;
  String temp;
  String rest( S );
  for ( ; !rest.empty(); ) {
    String::parse( rest, parm, temp ); rest = temp;
    for ( uint4 i = 0, m = 1; i < len; ++i, m *= 2 ) {
      if ( strcmp( name[i], parm.c_str() + 1 ) == 0 ) {
	/**/ if ( parm[0] == '+' ) val = val |  m;
	else if ( parm[0] == '-' ) val = val & ~m;
	else return false;
	goto NEXT;
      }
    }
    return false;
  NEXT: ;
  }
  
  return true;
}

ostream& FLAG_Bell::print( ostream& os ) const
{
  TSTAT;
  
  for ( uint4 i = 0, m = 1; i < len; ++i, m *= 2 ) {
    if ( i != 0 ) os << ' ';
    os << ( ( val & m ) == 0 ? '-' : '+' ) << name[i];
  }
  return os;
}

ostream& FLAG_Bell::save ( ostream& os ) const
{
  TSTAT;
  
  os.write( ccptr(&val), sizeof(val) );
  return os;
}

istream& FLAG_Bell::load ( istream& is )
{
  TSTAT;
  
  is.read( cptr(&val), sizeof(val) );
  return is;
}

//: FLAG_Bell.C (eof) (c) Igor
