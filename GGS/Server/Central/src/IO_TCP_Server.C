// $Id: IO_TCP_Server.C 160 2007-06-22 15:21:10Z mburo $
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
#include "IO_TCP_Server.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

IO_TCP_Server::IO_TCP_Server( sint4 desc, IO_Buffer::Method m ) 
  : IO_TCP( true, desc, 0, 0, IO_Buffer::NONE ), 
    method( m )
{
  System::sock_listen( desc, 5 );

  Form( vc_log << VCTIME, "  IO_TCP_Server( %3d %5s )", desc, IO_Buffer::method_str(method) ) << endl;
}

IO_TCP_Server::~IO_TCP_Server()
{
  Form( vc_log << VCTIME, " ~IO_TCP_Server( %3d %5s )", desc, IO_Buffer::method_str(method) ) << endl;
  close( false );
}

bool IO_TCP_Server::recv()
{
  TSTAT;
  
  sint4 client = System::sock_accept( desc );

  if ( client < 0 ) return true;
  if ( vc_ios.size() + 2 >= vc_ios.last() ) { // keep 2 fds in reserve
    vc_con << VCFL
	   << "recv( " << client << ' ' << vc_ios.size() << " / " << vc_ios.last() << " )"
	   << endl;
    System::sock_shutdown( client, 2 );
    System::sock_close   ( client );
    return true;
  }

  new IO_TCP_Client( client, 0, 0, method );
  
  return true;
}

//: IO_TCP_Server.C (eof) (c) Igor
