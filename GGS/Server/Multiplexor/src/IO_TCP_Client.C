// $Id: IO_TCP_Client.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_TCP_Client.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "Stat.H"
#include "IO_FILE.H"
#include "IO_Server.H"
#include "IO_TCP_Mux.H"
#include "IO_TCP_Client.H"
#include <cstdio>
#include <cstring>

const uint4 user_send_size = 1 << 11; //   2K
const uint4 user_recv_size = 1 << 10; //   1K

const uint4 user_send_buff = user_send_size * 4;  //  8K
const uint4 user_recv_buff = user_recv_size * 2;  //  2K

using namespace std;

IO_TCP_Client::IO_TCP_Client( sint4 Sock, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m ) 
  : IO_TCP( false, Sock, Mux, Full, m ) 
{
  if ( mux == 0 || ! full ) {
    sint4 val;
    System::setsockopt( desc, SOL_SOCKET, SO_KEEPALIVE, &(val = 1), sizeof(val) );
  }
  
  // send message to mux
  if ( mux != 0 && ! full ) {
    String host_name;
    String host_ip;
    sint4  host_port;
    System::getsockhost( host_name, host_ip, host_port, desc );
    char mssg[4096];
    sprintf( mssg, "%d %s %s", host_port, host_ip.c_str(), host_name.c_str() );
    
    uint2 d = desc;
    uint2 m = IO::CONNECT;
    uint4 l = strlen(mssg);
    mux->text( ccptr(&d), sizeof(d) );
    mux->text( ccptr(&m), sizeof(m) );
    mux->text( ccptr(&l), sizeof(l) );
    mux->text( mssg, l );
  }
  
  ++vc_stat.n_user;
  vc_log << VCTIME << ' ';
  Form( vc_log, "IO_TCP_Client( %3d, %p, %d, %s )",
	desc, mux, full, send_buff.method_str() ) << endl;
}

IO_TCP_Client::~IO_TCP_Client()
{
  vc_log << VCTIME << ' ';
  Form( vc_log, "~IO_TCP_Client( %3d, %p, %d, %s )",
	desc, mux, full, send_buff.method_str() ) << endl;

  // send message to mux
  if ( mux != 0 && ! full ) {
    uint2 d = desc;
    uint2 m = IO::DISCONNECT;
    uint4 l = 0;
    mux->text( ccptr(&d), sizeof(d) );
    mux->text( ccptr(&m), sizeof(m) );
    mux->text( ccptr(&l), sizeof(l) );
  }
  
  close( false );
}

void IO_TCP_Client::close( bool Err )
{
  if ( desc < 0 ) return;

  --vc_stat.n_user;

  IO_TCP::close( Err );
}

void IO_TCP_Client::time( sint4 /*Mssg*/, uint4 /*Time*/, bool /*Set*/ )
{
}

bool IO_TCP_Client::work( IO_Buffer& B )
{
  if (! ( mux && ! full ) ) return false;

  uint2 d = desc;
  uint2 m = IO::MESSAGE;
  uint4 l = B.size( false );
  mux->text( ccptr(&d), sizeof(d) );
  mux->text( ccptr(&m), sizeof(m) );
  mux->text( ccptr(&l), sizeof(l) );
  mux->text( B.buff( false ), l );
  B.remove( false, l );
  
  return true;
}

void IO_TCP_Client::adjust()
{
  send_buff.max = user_send_buff;
  recv_buff.max = user_recv_buff;
  excp_buff.max = user_recv_buff;
  send_size = user_send_size;
  recv_size = user_recv_size;
#if 0
  if ( mux == 0 || ! full ) {
    sint4 val;
    System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF, &(val = send_size), sizeof(val) );
    System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF, &(val = recv_size), sizeof(val) );
  }
#endif
}

//: IO_TCP_Client.C (eof) (c) Igor
