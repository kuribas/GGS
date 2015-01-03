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
#include "DB_Server.H"
#include "IO_Server.H"
#include "IO_TCP_Server.H"
#include "IO_TCP_Mux.H"
#include "IO_FILE.H"
#include "VAR_Client.H"
#include "VAR_System.H"
#include <unistd.h>
#include <fstream>

enum { IP_PROTO = 0, TCP_PROTO = 6 };

using namespace std;

void db_print( ostream& os, const String& key )
{
  if ( key == login_system ) return;
  VAR_Client vc;
  vc.load( key );
  vc.print( os, vc ) << endl << endl;
}

void db_format( DB_Server& db, const String& key )
{
  ostringstream os;
	       
  if ( key == login_system ) {
    VAR_System vs;
    vs.load();
    vs.save( os );
  } else {
    VAR_Client vc;
    vc.load( key );
    vc.save( os );
  }

  db.put( key, os.str() );
}

int main( int ac, char ** av )
{
#if 1
  System::file_close( 0 ); // close stdin
  System::file_close( 1 ); // close stdout
  System::file_close( 2 ); // close stderr
#endif
  
  char* gsa_home = getenv( "GSAHOME" ); if ( gsa_home == 0 ) System::exit(-1);
  string svc_dir(gsa_home);  svc_dir += "/Server/Central";
  System::chdir( svc_dir.c_str() );

  sint4 Port = 5000; 

  if ( ac > 1 ) {
    if ( av[1] == string( "-db_init" ) ) {
      vc_var.init();
      VAR_Client vc; 
      vc.init();
      return 0;
    } else if ( av[1] == string( "-db_print" ) ) {
      ofstream ofs( "Server.db.print" ); if (! ofs ) { vc_con << VCFL; System::exit(-1); }
      vc_db.print( ofs, &db_print );
      return 0;
    } else if ( av[1] == string( "-db_format" ) ) {
      DB_Server db( "Server.db", IO_Buffer::NONE );
      vc_db.format( db, &db_format );
      return 0;
    } else if ( av[1] == string( "-db_pack" ) ) {
      vc_db.pack();
      return 0;
    } else if ( av[1] == string( "-db_ios" ) ) {
      vc_var.load();
      VAR_Client vc;
      vc.ios();
      return 0;
    }
    Port = atoi( av[1] );
  }

  int desc = -1;
  
  // raw text
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc,  IO_Buffer::NONE  );

  // bzip2 compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::BZIP2 );

  // gzip compression
  desc = System::sock_open( AF_INET, SOCK_STREAM, IP_PROTO );
  /*  */ System::sock_bind( desc,  AF_INET, Port++ );
  new IO_TCP_Server( desc, IO_Buffer::GZIP );

  vc_var.load();
  vc_ios.loop();
  vc_ios.down(); // must kill dynamic clients before static destructors

  return 0;
}

//: main.C (eof) (c) Igor
