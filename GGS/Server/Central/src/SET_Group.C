// $Id: SET_Group.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Group.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "SET_Group.H"
#include <sstream>
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void SET_Group::vec( vector<String>& S ) const
{
  TSTAT;
  
  S.erase( S.begin(), S.end() );
  const_iterator it = begin();
  const_iterator hi = end();
  for ( ; it != hi; ++it ) S.push_back( *it );
}

bool SET_Group::add( const String& Name, const String& Login, bool Create )
{
  TSTAT;
  
  Group* gp = (*this)( Name );
  if ( gp == 0 ) {
    if (! Create ) return false;
    gp = (*this) += Group( Name );
  }
  gp->obj += Login;
  return true;
}

bool SET_Group::del( const String& Name, const String& Login, bool Remove )
{
  TSTAT;
  
  Group* gp = (*this)( Name );
  if ( gp == 0 ) return false;
  gp->obj -= Login;
  if ( Remove && gp->obj.size() == 0 ) (*this) -= Name;
  return true;
}

void SET_Group::add( const vector<String>& Names, const String& Login, bool Create )
{
  TSTAT;
  
  vector<String>::const_iterator it = Names.begin();
  vector<String>::const_iterator hi = Names.end();
  for ( ; it != hi; ++it )
    if (! add( *it, Login, Create ) )
      vc_log << VCFL << "Login:" << Login << " Group:" << *it << " not added." << endl;
}

void SET_Group::del( const vector<String>& Names, const String& Login, bool Remove )
{
  TSTAT;
  
  vector<String>::const_iterator it = Names.begin();
  vector<String>::const_iterator hi = Names.end();
  for ( ; it != hi; ++it )
    if (! del( *it, Login, Remove ) )
      vc_log << VCFL << "Login:" << Login << " Group:" << *it << " not removed." << endl;
}

ostream& SET_Group::print( ostream& os, bool Hide, bool NewLine ) const
{
  TSTAT;
  
  static const String hide( ".." );

  const_iterator it = begin();
  const_iterator hi = end();
  bool any = false;

  if ( it != hi ) {
    for ( ; it != hi; ++it ) {
      const String& str = *it;
      if ( Hide && str.find( hide, 0 ) == 0 ) continue; // hide ..channs
      if ( any ) os << ' ';
      os << str << '(' << (*it).obj.size() << ')';
      any = true;
    }
  }

  if ( NewLine && any ) os << EOL;

  return os;
}

//: SET_Group.C (eof) (c) Igor
