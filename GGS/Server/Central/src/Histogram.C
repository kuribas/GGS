// $Id: Histogram.C 160 2007-06-22 15:21:10Z mburo $
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
//: Histogram.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "COMMA.H"
#include "Histogram.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

ccptr Histogram::tag = "Histogram";

using namespace std;

Histogram::Histogram( int N )
  : curr_i(0)
{
  TSTAT;
  
  arr.reserve( N );
  for ( int i = N; --i >= 0; ) arr.push_back( MinMax<sint8>() );
}

ostream& Histogram::print( ostream& os, ccptr* Name ) const
{
  TSTAT;
  
  if (! minmax.ok() ) return os << EOM;

  int dx = minmax.max() - minmax.min() + 1;
  
  real8 sx = 75.0/dx;

  os << EOL;
  os << "   |" << setw(-6) << COMMA(minmax.min());
  for ( int i = 0; i < (79-4-6-6-1); ++i ) os << '-';
  os << setw(6) << COMMA(minmax.max()) << '|';
  
  vector< MinMax<sint8> >::const_iterator it = arr.begin();
  vector< MinMax<sint8> >::const_iterator hi = arr.end();
  for ( ; it != hi; ++it ) {
    if (! (*it).ok() ) continue;
    os << EOL;
    int y = (*it).idx();
    if ( Name != 0 ) os << setw(3) << Name[ y ]; else os << setw(3) << y;
    os << ' ';
    int x1lo = (*it).min();
    int x2lo = (*it).cur();
    int x2hi = x2lo + 1;
    int x3hi = (*it).max() +1;
    x1lo = int((x1lo - minmax.min()) * sx);
    x2lo = int((x2lo - minmax.min()) * sx);
    x2hi = int((x2hi - minmax.min()) * sx);
    if ( x2lo == x2hi ) {
      if ( x2lo > 0 ) --x2lo; else ++x2hi;
    }
    x3hi = int((x3hi - minmax.min()) * sx);
    for ( int i = 0;  i < x1lo; ++i ) os << ' ';
    for ( int i = x1lo; i < x2lo; ++i ) os << '=';
    for ( int i = x2lo; i < x2hi; ++i ) os << '#';
    for ( int i = x2hi; i < x3hi; ++i ) os << '=';
  }
  os << EOM;

  return os;
}

void Histogram::update( int i, sint8 Val )
{
  TSTAT;
  
  if ( minmax.idx() != i ) ++curr_i;
  if ( curr_i == arr.size() ) {
    minmax.reset(); // redo min max boundaries during shift
    vector< MinMax<sint8> >::iterator to = arr.begin();
    vector< MinMax<sint8> >::iterator it = to+1;
    vector< MinMax<sint8> >::iterator hi = arr.end();
    for ( ; it != hi; ++it, ++to ) {
      *to = *it;
      minmax.update( (*to).idx(), (*to).min() );
      minmax.update( (*to).idx(), (*to).max() );
    }
    arr[--curr_i].reset();
  }

  arr[curr_i].update( i, Val );
  minmax.     update( i, Val );
}

ostream& Histogram::save ( ostream& os ) const
{
  TSTAT;

  tag_save( tag, os );
  
  arr.   save( os );
  minmax.save( os );
  os.write( ccptr(&curr_i), sizeof(curr_i) );

  return os;
}

istream& Histogram::load ( istream& is )
{
  TSTAT;

  if (! tag_chck( tag, is ) ) { vc_con << VCFL; System::exit(-1); }

  size_t len = arr.size();
  arr.   load( is );
  while ( arr.size() > len ) arr.erase( arr.begin() );
  while ( arr.size() < len ) arr.push_back( MinMax<sint8>() );
  
  minmax.load( is );
  
  is.read( cptr(&curr_i), sizeof(curr_i) );
  if ( curr_i >= arr.size() ) curr_i = arr.size() - 1;

  return is;
}
  
//: Histogram.C (eof) (c) Igor
