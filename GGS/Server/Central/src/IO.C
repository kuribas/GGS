// $Id: IO.C 160 2007-06-22 15:21:10Z mburo $
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
//: IO.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_Server.H"
#include "IO.H"
#include "IO_FILE.H"
#if defined(SERVER) || defined(MULTIPLEXER)
#include "IO_TCP_Mux.H"
#endif
#include "String.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

char* IO::MssgName[] = { "connect", "disconnect", "message" };

using namespace std;

IO::IO( sint4 Desc, IO_TCP_Mux* Mux, bool Full,
	uint4 send_size, uint4 recv_size, uint4 excp_size, 
	bool Sync, bool Alloc, 
	IO_Buffer::Method m ) 
  : ostream(&send_buff), 
    send_buff(m,send_size, this),
    recv_buff(m,recv_size, 0),
    excp_buff(m,excp_size, 0),
    desc(Desc), mux(Mux), full(Full), direct(Sync), alloc(Alloc)
{
}

int IO::sync()
{
  TSTAT;
  
  if ( this == &vc_con && ! vc_con_ready ) {
    send_buff.remove( false, send_buff.size( false ) );
    return 0;
  }
  if ( this == &vc_log && ! vc_log_ready ) {
    send_buff.remove( false, send_buff.size( false ) );
    return 0;
  }
  if ( send_buff.size( false ) == 0 && send_buff.size( true  ) == 0 ) return 0;
#if defined(SERVER) || defined(MULTIPLEXER)
  if ( mux != 0 && full ) {
    send_buff.compress();
    uint2 d = io_desc();
    uint2 m = MESSAGE;
    uint4 l = send_buff.size( true );
    ccptr p = send_buff.buff( true );
    mux->text( ccptr(&d), sizeof(d) ); // MUX protocol header = 8 bytes
    mux->text( ccptr(&m), sizeof(m) );
    mux->text( ccptr(&l), sizeof(l) );
    mux->text( p, l );
    send_buff.remove( true, l );
    return 0;
  }
#endif
  if ( io_sync() ) send(); else vc_ios.add_send( *this );
  return 0;
}


IO& IO::text( ccptr mssg, uint4 len )
{
  TSTAT;
  
  send_buff.append( false, mssg, len );
  return *this;
}

IO& IO::text( ostringstream& os )
{
  TSTAT;

  send_buff.Tappend( os );
  return *this;
}

IO& IO::mssg( const String& file )
{
  TSTAT;
  
  send_buff.Tappend( file.c_str(), true );
  return *this;
}

//: IO.C (eof) (c) Igor
