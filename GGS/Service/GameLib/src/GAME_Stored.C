// $Id: GAME_Stored.C 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_Stored.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "IO_FILE.H"
#include "CRatio.H"
#include "DB_Server.H"
#include "GAME_Stored.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& Stored::print( ostream& os ) const
{
  TSTAT;
  
  os << setw(-8)  << counter << ' '
     << setw(-20) << String( System::stime( date ) ) << ' '
     << setw(-8)  << name1 << ' '
     << setw(-8)  << name2 << ' '
     << info;

  return os;
}

ostream& Stored::save ( ostream& os ) const
{
  TSTAT;
  
  info.   save( os );
  counter.save( os );
  name1.  save( os );
  name2.  save( os );
  os.write( ccptr(&date), sizeof(date) );

  return os;
}

istream& Stored::load ( istream& is )
{
  TSTAT;
  
  info.   load( is );
  counter.load( is );
  name1.  load( is );
  name2.  load( is );
  is.read( cptr(&date), sizeof(date) );

  return is;
}

//

ccptr GAME_Stored::tag = "GAME_Stored";

bool GAME_Stored::operator[] ( const String& C ) const
{
  TSTAT;
  
  VEC<Stored>::const_iterator it = vec.begin();
  VEC<Stored>::const_iterator hi = vec.end();
  for ( ; it != hi; ++it ) if ( (*it).counter == C ) return true;

  return false;
}

#if 0
sint4 GAME_Stored::count ( const String& P ) const
{
  TSTAT;
  
  sint4 cnt = 0;

  VEC<Stored>::const_iterator it = vec.begin();
  VEC<Stored>::const_iterator hi = vec.end();
  for ( ; it != hi; ++it ) if ( (*it).name2 == P || (*it).name1 == P ) cnt++;

  return cnt;
}
#endif

sint4 GAME_Stored::count ( const String& P1, const String& P2 ) const
{
  TSTAT;

  sint4 cnt = 0;

  VEC<Stored>::const_iterator it = vec.begin();
  VEC<Stored>::const_iterator hi = vec.end();
  for ( ; it != hi; ++it ) {
    if ( ( (*it).name1 == P1 && (*it).name2 == P2 ) ||
	 ( (*it).name2 == P1 && (*it).name1 == P2 ) ) cnt++;
  }

  return cnt;
}

void GAME_Stored::erase( const String& Counter )
{
  TSTAT;
  
  VEC<Stored>::iterator it = vec.begin();
  VEC<Stored>::iterator hi = vec.end();
  for ( ; it != hi; ++it ) if ( (*it).counter == Counter ) { vec.erase( it ); break; }
}

void GAME_Stored::add( const String& Info,
		       const String& Counter,
		       const String& Name1,
		       const String& Name2 )
{
  TSTAT;
  
  vec.push_back( Stored( Info, Counter, Name1, Name2, System::clock() ) );
}
  
ostream& GAME_Stored::print( ostream& os ) const
{
  TSTAT;
  
  VEC<Stored>::const_iterator it = vec.begin();
  VEC<Stored>::const_iterator hi = vec.end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << EOL << *it; }
  return os;
}

ostream& GAME_Stored::save ( ostream& os ) const
{
  TSTAT;

  tag_save( tag, os );
  
  vec.save( os );

  return os;
}

istream& GAME_Stored::load ( istream& is )
{
  TSTAT;
  
  if (! tag_chck( tag, is ) ) { vc_con << VCFL; System::exit(-1); }

  vec.load( is );

  // perform sanity check on DB
  for ( int i = vec.size(); --i >=0 ; ) {
    String data;
    CRatio ok = vc_save.get( vec[i].counter, data );
    if ( ok.Txt() == 0 ) {
      vec[i].print( vc_log << VCFL ) << " not found:" << vec[i].counter << endl;
      vec.erase( vec.begin() + i );
    }
  }
  
  return is;
}

//: GAME_Stored.C (eof) (c) Igor
