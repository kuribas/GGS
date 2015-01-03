// $Id: main.C 9037 2010-07-06 04:05:44Z mburo $
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

#include "System.H"
#include "Actors.H"
#include "IO_Server.H"
#include "DB_Server.H"
#include "Client.H"
#include "VAR_Client.H"
#include "VAR_Service.H"
#include "Game.H"
#include <unistd.h>
#include <fstream>

#define COMPRESS 0

using namespace std;

void db_print( ostream& os, const String& key )
{
  if ( key == login_system ) return;
  VAR_Client vc;
  vc.load( key );
  ostringstream dat;
  vc.print_finger( dat );
  String str( dat );
  str.replace( EOL, NEW_LINE );
  os << str << endl << endl;
}

void save_print( ostream& os, const String& key )
{
  Request   req;
  String    rec;
  String    sdat;
  vc_save.get( key, sdat );
  stringstream dat(sdat);
  req.load( dat, false );
  rec.load( dat );
  os << key << ' ' << req.refcnt << ' ' << rec << endl << endl;
}

void save_format( DB_Server& db, const String& key )
{
  Request   req;
  String    rec;
  String    sdat;
  vc_save.get( key, sdat );
  stringstream dat(sdat);
  req.load( dat, false );
  rec.load( dat );
  if ( req.refcnt <= 0 ) return;
  ostringstream os;
  req.save( os );
  rec.save( os );
  db.put( key, os.str() );
}

void db_format( DB_Server& db, const String& key )
{
  ostringstream os;

  if ( key == login_system ) {
    VAR_Service vs;
    vs.load();
    //    vs.history.erase();
    vs.save( os );
  } else {
    VAR_Client vc;
    vc.load( key );
    //    vc.stored.erase();
    //    vc.history.erase();
    vc.save( os );
  }

  db.put( key, os.str() );
}


void Error(const string &s)
{
  cerr << "error: " << s << endl;
  exit(-1);
}

class Rebuild_Rank : public DB_Key_Handler
{
public:
  void handle( const String& key )
  {
    if ( key == login_system ) return;
    VAR_Client vc;
    vc.load( key );
    for ( SET_Rating::iterator it = vc.rating.begin(), hi = vc.rating.end(); it !=hi ; ++it ) {
      vc_var.rank( it->id() ).update( vc.login, *it );
    }
  }
};

void rebuild_rank()
{
  vc_var.rank.erase();
  Rebuild_Rank rr;
  vc_db.iterate( rr );
  vc_var.save();
}

int main(int argc, char **argv)
{
#if 0
  System::file_close( 0 ); // close stdin
  System::file_close( 1 ); // close stdout
  System::file_close( 2 ); // close stderr
#endif

  RegularBoardGame::gdb();

  char* gsa_home = getenv( "GSAHOME" ); if ( gsa_home == 0 ) System::exit(-1);
 
  string svc_dir(gsa_home);  svc_dir += "/Service/" + string(RegularBoardGame::GAME_NAME); 
  System::chdir( svc_dir.c_str() );

  string server   = "localhost";
  int    port     = 5000;
  string login    = RegularBoardGame::LOGIN_SERVICE;
  string password = "";

  if (argc < 2) {

  error:
    cout
      << "usage: " << argv[0] << "-db_init | -db_print | -db_format | -db_save_print | -db_save_format |"
      << "                             -db_pack | -db_ios | -db_sgf " << endl 
      << "            or  [-s server] [-p port] [-l login] -pw password" << endl;
    exit(-1);
  }

  for (int argi=1; argi < argc; ++argi) {

    string opt = argv[argi];

    if (opt == "-l") {

      if (argi == argc-1) goto error;
      login = argv[argi+1];
      if (login == "") Error("empty login");
      argi++;

    } else if (opt == "-pw") {

      if (argi == argc-1) goto error;
      password = argv[argi+1];
      if (password == "") Error("empty password");
      argi++;

    } else if (opt == "-s") {

      if (argi == argc-1) goto error;
      server = argv[argi+1];
      if (server == "") Error("empty server");
      argi++;

    } else if (opt == "-p") {

      if (argi == argc-1) goto error;
      if (sscanf(argv[argi+1], "%d", &port) != 1 || port < 1024) Error("port?");
      argi++;

    } else if (opt == "-db_init") {      // db options

      vc_var.init();
      vc_var.save();
      return 0;
      
    } else if (opt == "-db_print") {

      ofstream ofs( "Service.db.print" ); if (! ofs ) { vc_con << VCFL; System::exit(-1); }
      vc_db.print( ofs, &db_print );
      return 0;
      
    } else if (opt == "-db_format") {

      DB_Server db( "Service.db", IO_Buffer::NONE );
      vc_db.format( db, &db_format );
      return 0;
      
    } else if (opt == "-db_save_print") {

      ofstream ofs( "Service.save.print" ); if (! ofs ) { vc_con << VCFL; System::exit(-1); }
      vc_save.print( ofs, &save_print );
      return 0;

    } else if (opt == "-db_save_format") {
      
      DB_Server db( "Service.save", IO_Buffer::NONE );
      vc_save.format( db, &save_format );
      return 0;

    } else if (opt == "-db_pack") {
      
      vc_db.  pack();
      vc_save.pack();
      return 0;

    } else if (opt == "-db_ios") {

      vc_var.load();
      VAR_Client::ios();
      return 0;

    } else if (opt == "-db_sgf") {

      vc_var.load();
      VAR_Client::sgf();
      return 0;

    } else goto error;
  }

  if (password == "") goto error;

  passw_service = password;
  
  vc_var.load();

  // vc_var.set_match_cnt(800000); // uncomment and set if match_cnt is corrupted
  // rebuild_rank(); // can be removed ater rank is rebuilt
  
#if   COMPRESS==0
  vc_var.connect( server.c_str(), port+0, IO_Buffer::NONE );
#elif COMPRESS==1
  vc_var.connect( server.c_str(), port+1, IO_Buffer::BZIP2 );
#elif COMPRESS==2
  vc_var.connect( server.c_str(), port+2, IO_Buffer::GZIP );
#else
#error COMPRESS
#endif

  vc_ios.loop();
  vc_ios.down(); // kill rest of the sockets

  return 0;
}

//: main.C (eof) (c) Igor
