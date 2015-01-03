// $Id: SET_Rank.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Rank.C (bof) (c) Igor Durdanovic

#include "String.H"
#include "SET_Rank.H"
#include "MatchType.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const String SET_Rank::tag( "SET_Rank" );

using namespace std;

/* */ VC_Rank& SET_Rank::operator () ( const String& K )
{
  TSTAT;
  
  VC_Rank* R = SET<VC_Rank>::operator() ( K );
  if ( R == 0 ) R = ( (*this) += VC_Rank( K ) );
  return *R;
}

ostream& SET_Rank::save( ostream& os ) const
{
  TSTAT;
  
  tag.tag_save( os );
  
  sint4 n = size();
  os.write( ccptr(&n), sizeof(n) );
  const_iterator it = begin();
  const_iterator hi = end();
  for ( ; it != hi; ++it ) (*it).save( os );
  return os;
}

istream& SET_Rank::load( istream& is )
{
  TSTAT;

  if (! tag.tag_chck( is ) ) { vc_con << VCFL; System::exit(-1); }
  
  erase();
  sint4 n = 0;
  is.read( cptr(&n), sizeof(n) );
  for ( VC_Rank o ; --n >= 0 ; ) {
    o.load( is );

    if ( o.size() == 0 ) continue;

    String type = o.id();
    if (! MatchType::normalize_key( type ) ) continue;

    (*this) += o;
  }
  
  return is;
}

//: SET_Rank.C (eof) (c) Igor
