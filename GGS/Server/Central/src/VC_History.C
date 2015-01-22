// $Id: VC_History.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_History.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "HHMMSS.H"
#include "System.H"
#include "IO_FILE.H"
#include "IO_TCP_Client.H"
#include "VC_History.H"
#include "VT100.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& History::print( ostream& os, HMODE M, const VAR_Client& VP ) const
{
  TSTAT;
  
  bool vt100 = VP.login == login && VP.vt100;
  if ( vt100 ) os << vt_self;
  os << System::dtime( departed ) << ' ' 
     << setw(10) << HHMMSS( departed - arrived );
  if ( M == SYSTEM ) Form( os, " %-8s", login.c_str() );
  if ( VP.login == login || VP.admin() > admin && VP.admin() > user_level )
    os << setw(16) << host_ip << ' ' << host_name;
  if ( vt100 ) os << vt_reset;
  return os;
}

void History::erase()
{
  TSTAT;
  
  login.erase();
  arrived = 0;
  departed = 0;
  host_ip.  erase();
  host_name.erase();
}

ostream& History::save ( ostream& os ) const
{
  TSTAT;
  
  login.save( os );
  os.write( ccptr(&admin),    sizeof(admin) );
  os.write( ccptr(&arrived),  sizeof(arrived) );
  os.write( ccptr(&departed), sizeof(departed) );
  host_ip.  save( os );
  host_name.save( os );
  return os;
}

istream& History::load ( istream& is )
{
  TSTAT;
  
  erase();
  login.load( is );
  is.read( cptr(&admin),    sizeof(admin) );
  is.read( cptr(&arrived),  sizeof(arrived) );
  is.read( cptr(&departed), sizeof(departed) );
  host_ip.  load( is );
  host_name.load( is );
  return is;
}

//

const char *VC_History::tag = "VC_History";

VC_History& VC_History::push( const History& H )
{
  TSTAT;
  
  if ( vec.size() >= len ) vec.erase( vec.begin() );
  vec.push_back( H );
  return *this;
}

ostream& VC_History::print( ostream& os, History::HMODE M, const VAR_Client& VP ) const
{
  TSTAT;
  
  VEC<History>::const_iterator it = vec.begin();
  VEC<History>::const_iterator hi = vec.end();
  if ( it != hi ) {
    (*it).print( os, M, VP );
    for ( ; ++it != hi; ) (*it).print( os << EOL, M, VP );
  }
  return os;
}

ostream& VC_History::save ( ostream& os ) const
{
  TSTAT;

  tag_save( tag, os );
  
  os.write( ccptr(&len), sizeof(len) );
  vec.save( os );

  return os;
}

istream& VC_History::load ( istream& is )
{
  TSTAT;
  
  if (! tag_chck( tag, is ) ) { vc_con << VCFL; System::exit(-1); }

  uint4 Len = len;
  is.read( cptr(&len), sizeof(len) );
  vec.load( is );
  if ( len != Len ) {
    vc_con << VCFL;
    Form( vc_con, "Len(%d) != len(%d)", Len, len ) << endl;
    len = Len;
    while ( vec.size() > len ) vec.erase( vec.begin() );
  }

  return is;
}

//: VC_History.C (eof) (c) Igor
