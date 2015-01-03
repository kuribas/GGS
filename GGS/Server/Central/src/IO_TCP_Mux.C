// $Id: IO_TCP_Mux.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_TCP_Mux.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "Stat.H"
#include "IO_FILE.H"
#include "IO_Server.H"
#include "IO_TCP_Mux.H"
#include "SET_PTR_Mux.H"
#include "VAR_System.H"
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const uint4 mux_send_size = 1 << 14; //   16K
const uint4 mux_recv_size = 1 << 14; //   16K
	    
const uint4 mux_send_buff = mux_send_size * 64; //  1M
const uint4 mux_recv_buff = mux_recv_size * 64; //  1M

using namespace std;

IO_TCP_Mux::IO_TCP_Mux( sint4 Sock, IO_Buffer::Method m, const String& Key ) 
  : IO_TCP( true, Sock, 0, false, m ), key(Key)
{
  TSTAT;
  
  send_buff.max = mux_send_buff;
  recv_buff.max = mux_recv_buff;
  excp_buff.max = mux_recv_buff;
  send_size = mux_send_size;
  recv_size = mux_recv_size;

  sint4 val;

  System::setsockopt( desc, SOL_SOCKET, SO_KEEPALIVE, &(val = 0), sizeof(val) );
#if 0
  System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF, &(val = send_size), sizeof(val) );
  System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF, &(val = recv_size), sizeof(val) );
#endif
  System::getsockhost( host_name, host_ip, host_port, desc );

  struct rlimit rlim;

  System::rlimit_get( RLIMIT_NOFILE, rlim ); rlim.rlim_cur = rlim.rlim_max;
  System::rlimit_set( RLIMIT_NOFILE, rlim );
  System::rlimit_get( RLIMIT_NOFILE, rlim );

  vec.reserve( rlim.rlim_cur );

  for ( sint4 i = rlim.rlim_cur; --i >= 0; ) vec.push_back( 0 );

  vc_mux += this;

  Form( vc_log << VCTIME, "  IO_TCP_Mux( %3d, %s ) [%d]",
	desc, send_buff.method_str(), vec.size() ) << endl;

  te_ping();
}

IO_TCP_Mux::~IO_TCP_Mux()
{
  TSTAT;

  te_cancel();
  
  Form( vc_log << VCTIME, " ~IO_TCP_Mux( %3d, %s ) [%d]",
	desc, send_buff.method_str(), vec.size() ) << endl;
  
  vc_mux -= (*this)();
  close( false );
}

IO_TCP_Mux* IO_TCP_Mux::connect( ccptr Host, sint4 Port, IO_Buffer::Method m, String Key )
{
  TSTAT;
  
  sint4 desc = System::sock_connect( Host, Port, false );

  if ( desc < 0 ) return 0;

  return new IO_TCP_Mux( desc, m, Key );
}

void IO_TCP_Mux::close( bool Err )
{
  TSTAT;
  
  if ( desc < 0 ) return;

  vector<IO_TCP_Client*>::reverse_iterator it = vec.rbegin();
  vector<IO_TCP_Client*>::reverse_iterator hi = vec.rend();
  for ( ; it != hi; ++it ) {
    if ( (*it) == 0 ) continue;
    delete *it;
    (*it) = 0;
  }
  IO_TCP::close( Err );
}

void IO_TCP_Mux::te_ping()
{
  te_cancel( TE_PING );
  te_create( TE_PING, 120 );
}

void IO_TCP_Mux::te_no_ping()
{
  te_cancel( TE_NO_PING );
  te_create( TE_NO_PING, 15 );
}

void IO_TCP_Mux::send_ping()
{
  uint2 d = CMD_PING;
  uint2 m = IO::COMMAND;
  uint4 l = 0;
  text( ccptr(&d), sizeof(d) ); // MUX protocol header = 8 bytes
  text( ccptr(&m), sizeof(m) );
  text( ccptr(&l), sizeof(l) );
  sync();

  te_ping();    // (re)schedule ping
  te_no_ping(); // (re)schedule kill if mux does not answer
}

void IO_TCP_Mux::recv_ping()
{
  te_cancel( TE_NO_PING );
}

void IO_TCP_Mux::te_handle( sint4 Mssg, uint4 /*Time*/ )
{
  TSTAT;

  switch ( Mssg ) {
  case TE_PING    : send_ping(); break;
  case TE_NO_PING : // there was no ping, proceed with kill
  default         : if ( io_alloc() ) delete this; else close( true );
  }
}

ostream& IO_TCP_Mux::te_print ( ostream& os ) const
{
  TSTAT;
  
  Form( os, "IO_TCP_Mux( %3d, %s ) [%d]", desc, send_buff.method_str(), vec.size() );
  return os;
}

bool IO_TCP_Mux::work( IO_Buffer& B )
{
  TSTAT;
  
  for ( ;; ) {
    if ( B.size( false ) < 8 ) break; // insufficient data
    uint2 d; memcpy( &d, B.buff( false ),     sizeof(d) );
    uint2 m; memcpy( &m, B.buff( false ) + 2, sizeof(m) );
    uint4 l; memcpy( &l, B.buff( false ) + 4, sizeof(l) );
    if ( B.size( false ) < 8 + l ) break; // insufficient data
    B.remove( false, 8 ); // skip header
#if LOG > 2
    vc_log << VCTIME << ' ';
    Form( vc_log, "MSSG: d(%d) m(%s) l(%d)", sint4(d), IO::mssg_str(MSSG(m)), l ) << endl;
#endif
    switch ( m ) {
    case IO::CONNECT    :
      if ( vec[d] != 0 ) delete vec[d];
      {
	vec[d] = new IO_TCP_Client( d, this, true, IO_Buffer::NONE );
	String mssg( B.buff( false ), l );
	String port;
	String host;
	String::parse( mssg, port, host );
	vec[d]->port = atoi( port.c_str() );
	String::parse( host, vec[d]->host_ip, vec[d]->host_name );
	B.remove( false, l );
      }
      break;
    case IO::DISCONNECT :
      if ( vec[d] != 0 ) delete vec[d];
      break;
    case IO::MESSAGE    :
      if ( vec[d] != 0 ) {
	vec[d]->recv_buff.append( false, B.buff(false), l );
	if (! vec[d]->work( vec[d]->recv_buff ) ) delete vec[d];
      }
      B.remove( false, l );
      break;
    case IO::COMMAND :
      switch ( d ) {
      case CMD_PING : recv_ping(); break;
      default :
	vc_log << VCTIME << ' ';
	Form( vc_log, "MSSG: d(%d) m(%s) l(%d)", sint4(d), IO::mssg_str(MSSG(m)), l ) << endl;
      }
      B.remove( false, l );
      break;
    default :
      vc_log << VCFL;
      Form( vc_log, "d(%d) m(%s) l(%d)", sint4(d), mssg_str(MSSG(m)), l ) << endl;
      return false;
    }
  }
  return true;
}

void IO_TCP_Mux::remove( sint4 Desc )
{
  TSTAT;
  
  vec[Desc] = 0;

  uint2 d = Desc;
  uint2 m = IO::DISCONNECT;
  uint4 l = 0;
  text( ccptr(&d), sizeof(d) ); // MUX protocol header = 8 bytes
  text( ccptr(&m), sizeof(m) );
  text( ccptr(&l), sizeof(l) );
  sync();
}

ostream& IO_TCP_Mux::print( ostream& os ) const
{
  TSTAT;
  
  Form( os, "%-20s : %5s %-15s %-20s #%d",
	key.c_str(),
	send_buff.method_str(),
	host_ip.c_str(),
	host_name.c_str(),
	desc);
  return os;
}

//: IO_TCP_Mux.C (eof) (c) Igor
