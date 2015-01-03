// $Id: SET_PTR_Request.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_PTR_Request.C (bof) (c) Igor Durdanovic

#include "VC_Request.H"
#include "SET_String.H"
#include "SET_PTR_Request.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

/* */ VC_Request* SET_PTR_Request::find ( const Request& B, COMPARE f )
{
  TSTAT;
  
  vector< VC_Request* > requests;  // all requests
  {
    iterator it = begin();
    iterator hi = end();
    for ( ; it != hi; ++it ) { requests.push_back( &**it ); }
  }
  random_shuffle( requests.begin(), requests.end() );
  
  vector< VC_Request* >::iterator it = requests.begin();
  vector< VC_Request* >::iterator hi = requests.end();
  for ( ; it != hi; ++it ) if ( f( **it, B ) ) return *it;
  return 0;
}

const VC_Request* SET_PTR_Request::find ( const Request& B, COMPARE f ) const
{
  TSTAT;
  
  vector< const VC_Request* > requests;  // all requests
  {
    const_iterator it = begin();
    const_iterator hi = end();
    for ( ; it != hi; ++it ) { requests.push_back( &**it ); }
  }
  random_shuffle( requests.begin(), requests.end() );
  
  vector< const VC_Request* >::iterator it = requests.begin();
  vector< const VC_Request* >::iterator hi = requests.end();
  for ( ; it != hi; ++it ) if ( f( **it, B ) ) return *it;
  return 0;
}

ostream& SET_PTR_Request::print( ostream& os ) const
{
  TSTAT;
  
  const_iterator it = begin();
  const_iterator hi = end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << EOL << *it; }
  return os;
}

void SET_PTR_Request::erase( const SET_String& Set )
{
  TSTAT;
  
  SET_PTR_Request rm;
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) { VC_Request* R = (*this)( *it ); if ( R ) rm += R; }
  rm.erase( true );
}

//: SET_PTR_Request.C (eof) (c) Igor
