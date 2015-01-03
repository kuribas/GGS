// $Id: GAME_History.C 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_History.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "IO_FILE.H"
#include "CRatio.H"
#include "DB_Server.H"
#include "GAME_History.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& History::print( ostream& os ) const
{
  TSTAT;
  
  os << setw(-8)  << counter << ' '
     << setw(-20) << String( System::stime( date ) ) << ' ';
  Form( os, "%4.0f ", rank1 ) << setw(-8) << name1 << ' ';
  Form( os, "%4.0f ", rank2 ) << setw(-8) << name2 << ' ';
  Form( os, "%+6.1f", result ) << ' ';
  os << info;

  return os;
}

ostream& History::save ( ostream& os ) const
{
  TSTAT;
  
  info.   save( os );
  counter.save( os );
  name1.  save( os );
  name2.  save( os );
  os.write( ccptr(&rank1),  sizeof(rank1) );
  os.write( ccptr(&rank2),  sizeof(rank2) );
  os.write( ccptr(&result), sizeof(result) );
  os.write( ccptr(&date),   sizeof(date) );

  return os;
}

istream& History::load ( istream& is )
{
  TSTAT;
  
  info.   load( is );
  counter.load( is );
  name1.  load( is );
  name2.  load( is );
  is.read( cptr(&rank1),  sizeof(rank1) );
  is.read( cptr(&rank2),  sizeof(rank2) );
  is.read( cptr(&result), sizeof(result) );
  is.read( cptr(&date),   sizeof(date) );

  return is;
}

//

const String GAME_History::tag( "GAME_History" );

bool GAME_History::operator() ( const String& Counter ) const
{
  TSTAT;
  
  VEC<History>::const_iterator it = vec.begin();
  VEC<History>::const_iterator hi = vec.end();
  for ( ; it != hi; ++it ) if ( (*it).counter == Counter ) return true;
  return false;
}

String GAME_History::add( const String& Info,
			  const String& Counter,
			  /* */ real8   Rank1,
			  const String& Name1,
			  /* */ real8   Rank2,
			  const String& Name2,
			  /* */ real8   Result,
			  /* */ sint4   Date )
{
  TSTAT;
  
  String last;
  while ( vec.size() >= len ) { last = vec[0].counter; vec.erase( vec.begin() ); }

  vec.push_back( History( Info, Counter, Rank1, Name1, Rank2, Name2, Result, Date ) );

  return last;
}
  
ostream& GAME_History::print( ostream& os ) const
{
  TSTAT;
  
  VEC<History>::const_iterator it = vec.begin();
  VEC<History>::const_iterator hi = vec.end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << EOL << *it; }
  return os;
}

ostream& GAME_History::save ( ostream& os ) const
{
  TSTAT;

  tag.tag_save( os );
  
  os.write( ccptr(&len), sizeof(len) );
  vec.save( os );

  return os;
}

istream& GAME_History::load ( istream& is )
{
  TSTAT;
  
  if (! tag.tag_chck( is ) ) { vc_con << VCFL; System::exit(-1); }

  uint4 Len;
  is.read( cptr(&Len), sizeof(Len) );
  if ( len != Len ) {
    vc_con << VCFL;
    Form( vc_con, "Len(%d) != len(%d)", Len, len ) << endl;
  }

  vec.load( is );
  while ( vec.size() > len ) vec.erase( vec.begin() );

  // perform sanity check on DB
  for ( int i = vec.size(); --i >=0 ; ) {
    String data;
    CRatio ok = vc_save.get( vec[i].counter, data );
    if ( ok.Txt() == 0 ) {
      vec[i].print( vc_log << VCFL ) << " not found." << endl;
      vec.erase( vec.begin() + i );
    }
  }

  return is;
}

//: GAME_History.C (eof) (c) Igor
