// $Id: IO_TCP_Server.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_TCP_Server.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "Actors.H"
#include "IO_Server.H"
#include "IO_FILE.H"
#include "IO_TCP_Client.H"
#include "IO_TCP_Mux.H"
#include "IO_TCP_Server.H"
#include <cstring>

const char* IO_TCP_Server::ConnectionName[] = { "client", "server" };

using namespace std;

IO_TCP_Server::IO_TCP_Server( sint4 desc, IO_Buffer::Method m, Connection c ) 
  : IO_TCP( true, desc, 0, 0, IO_Buffer::NONE ), 
    method( m ),
    connection( c )
{
  System::sock_listen( desc, 5 );

  vc_log << VCTIME << ' ';
  Form( vc_log, "IO_TCP_Server( %3d %5s %6s )",
	desc, IO_Buffer::method_str(method), io_connection_str() ) << endl;
}

IO_TCP_Server::~IO_TCP_Server()
{
  vc_log << VCTIME << ' ';
  Form( vc_log, "~IO_TCP_Server( %3d %5s %6s )",
	desc, IO_Buffer::method_str(method), io_connection_str() ) << endl;
  
  close( false );
}

bool IO_TCP_Server::recv()
{
  sint4 client = System::sock_accept( desc );

  if ( client < 0 ) return true;
  if ( client+1 >= vc_ios.last() ) {
    vc_log << VCFL << "recv( " << client << " / " << vc_ios.last() << " )" << endl;

    System::sock_shutdown( client, 2 );
    System::sock_close   ( client );
    return true;
  }

  switch ( connection ) {
  case CLIENT :
    if ( vc_mux == 0 ) {
      //      vc_log << VCFL << "recv( " << client << ") server not ready" << endl;
      System::sock_shutdown( client, 2 );
      System::sock_close   ( client );
    } else {
      new IO_TCP_Client( client, vc_mux, false, method );
    }
    break;
  case SERVER :
    if ( vc_mux != 0 ) {
      vc_log << VCFL
	     << "recv( " << client << ") server already connected" << endl;
      System::sock_shutdown( client, 2 );
      System::sock_close   ( client );
    } else {
      new IO_TCP_Mux( client, method );
    }
    break;
  default :
    vc_log << VCFL
	   << "recv( " << client << ") connection(" << int(connection) << ")" << endl;
    System::exit( -1 ); // exit gracefully
  }
  
  return true;
}

//: IO_TCP_Server.C (eof) (c) Igor
