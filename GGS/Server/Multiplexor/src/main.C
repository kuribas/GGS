// $Id: main.C 160 2007-06-22 15:21:10Z mburo $
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
//: main.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_Server.H"
#include "IO_TCP_Server.H"
#include <unistd.h>
#include <stdlib.h>
#include <string>

enum { IP_PROTO = 0, TCP_PROTO = 6 };

using namespace std;

int main( int ac, char ** av )
{
#if 1
  System::file_close( 0 ); // close stdin
  System::file_close( 2 ); // close stderr
#endif

  char* gsa_home = getenv( "GSAHOME" ); if ( gsa_home == 0 ) System::exit(-1);
  string svc_dir(gsa_home);  svc_dir += "/Server/Multiplexor";
  System::chdir( svc_dir.c_str() );

  sint4 Port = 5000; if ( ac > 1 ) Port = atoi( av[1] );

  int desc = -1;

  
  // clients ports
  
  // raw text
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc,  IO_Buffer::NONE, IO_TCP_Server::CLIENT );

  // bzip2 compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::BZIP2, IO_TCP_Server::CLIENT );

  // lzw compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::GZIP, IO_TCP_Server::CLIENT );


  // server ports
  
  // raw text
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::NONE, IO_TCP_Server::SERVER );

  // bzip2 compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::BZIP2, IO_TCP_Server::SERVER );

  // lzw compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::GZIP, IO_TCP_Server::SERVER );

  
  vc_ios.loop();
  vc_ios.down(); // must kill dynamic clients before static destructors

  return 0;
}

//: main.C (eof) (c) Igor
