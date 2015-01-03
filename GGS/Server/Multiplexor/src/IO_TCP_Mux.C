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
#include <cstring>

const uint4 mux_send_size = 1 << 14; //   16K
const uint4 mux_recv_size = 1 << 14; //   16K
	    
const uint4 mux_send_buff = mux_send_size * 64; //  1M
const uint4 mux_recv_buff = mux_recv_size * 64; //  1M

using namespace std;

IO_TCP_Mux::IO_TCP_Mux( sint4 Sock, IO_Buffer::Method m ) 
  : IO_TCP( true, Sock, 0, 0, m ) 
{
  send_buff.max = mux_send_buff;
  recv_buff.max = mux_recv_buff;
  excp_buff.max = mux_recv_buff;
  send_size = mux_send_size;
  recv_size = mux_recv_size;

  sint4 val;

  System::setsockopt( desc, SOL_SOCKET, SO_KEEPALIVE, &(val = 1), sizeof(val) );
#if 0
  System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF, &(val = send_size), sizeof(val) );
  System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF, &(val = recv_size), sizeof(val) );
#endif
  vc_mux = this;

  vc_log << VCTIME << ' ';
  Form( vc_log,"IO_TCP_Mux( %3d, %s )", desc, send_buff.method_str() ) << endl;
}

IO_TCP_Mux::~IO_TCP_Mux()
{
  vc_log << VCTIME << ' ';
  Form( vc_log, "~IO_TCP_Mux( %3d, %s )", desc, send_buff.method_str() ) << endl;

  close( false );

  vc_mux = 0;  // ready for another connection
}

void IO_TCP_Mux::close( bool Err )
{
  if ( desc < 0 ) return;

  IO_TCP::close( Err );

  vc_ios.free_mux_clients( this ); 
}

void IO_TCP_Mux::send_ping()
{
  uint2 d = CMD_PING;
  uint2 m = IO::COMMAND;
  uint4 l = 0;
  text( ccptr(&d), sizeof(d) );
  text( ccptr(&m), sizeof(m) );
  text( ccptr(&l), sizeof(l) );
  sync();
}

void IO_TCP_Mux::recv_ping()
{
  send_ping();
}

bool IO_TCP_Mux::work( IO_Buffer& B )
{
  for ( ;; ) {
    if ( B.size( false ) < 8 ) break; // insufficient data
    uint2 d; memcpy( &d, B.buff( false ),     sizeof(d) );
    uint2 m; memcpy( &m, B.buff( false ) + 2, sizeof(m) );
    uint4 l; memcpy( &l, B.buff( false ) + 4, sizeof(l) );
    if ( B.size( false ) < 8 + l ) break; // insufficient data
    B.remove( false, 8 ); // skip header
#if LOG > 2
    vc_log << VCTIME << ' ';
    Form( vc_log, "MSSG: d(%d) m(%s) l(%d)", sint4(d), mssg_str(MSSG(m)), l ) << endl;
#endif
    switch ( m ) {
    case IO::DISCONNECT :
      if ( vc_ios[d] != 0 ) vc_ios.free_client( vc_ios[d], true );
      break;
    case IO::MESSAGE    :
      if ( vc_ios[d] != 0 ) {
	vc_ios[d]->text( B.buff( false ), l );
	vc_ios[d]->sync();
      }
      B.remove( false, l );
      break;
    case IO::COMMAND :
      switch ( d ) {
      case CMD_PING : recv_ping(); break;
      default :
	vc_log << VCTIME << ' ';
	Form( vc_log, "MSSG: d(%d) m(%s) l(%d)", sint4(d), mssg_str(MSSG(m)), l ) << endl;
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

//: IO_TCP_Mux.C (eof) (c) Igor
