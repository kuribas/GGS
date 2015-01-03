// $Id: VAR_Service.C 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Service.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "Actors.H"
#include "Client.H"
#include "IO_FILE.H"
#include "DB_Server.H"
#include "VAR_Service.H"
#include <sstream>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

VAR_Client VAR_Service::svar;

using namespace std;

VAR_Service::VAR_Service()
  : socket(0), match_cnt(0), history(32), dirty(false)
{
  svar.make( str_empty, unreg_level );
  
  if ( this == &vc_var ) vc_var_ready = true;

  vc_log << VCTIME << " VAR_Service()" << endl;
  
  te_create( 0, 10 ); // schedule first save
}

VAR_Service::~VAR_Service()
{
  if ( this == &vc_var && ! vc_var_ready ) return;

  matches.erase( true );
  global .erase( true );
  local  .erase( true );
  clients.erase( true );
  
  vc_log << VCTIME << " ~VAR_Service()" << endl;

  modified(); // awlays save
  save();

  vc_var_ready = false;
}

void VAR_Service::te_handle( sint4 /*Mssg*/, uint4 /*Time*/ )
{
  TSTAT;
  
  save();

  te_create( 0, time_save );
}

ostream& VAR_Service::te_print( ostream& os ) const
{
  TSTAT;
  
  os << "VAR_Service()";
  
  return os;
}

bool VAR_Service::ready( const String& S )
{
  TSTAT;
  
  static String _ready( "READY" );
  static String _alert( "ALERT" );

  return ( S == _ready || S == _alert );
}

void VAR_Service::connect( ccptr Host, sint4 Port, IO_Buffer::Method m )
{
  sint4 desc = System::sock_connect( Host, Port, false );

  if ( desc < 0 ) { vc_con << VCFL; System::exit(-1); }
  
  socket = new IO_TCP_Service( desc, m );
}

Client* VAR_Service::client( const String& Login, bool Mak_Ghost, bool Ret_Ghost )
{
  TSTAT;
  
  if ( Login.empty() ) return 0;
  Client* C = clients( Login );
  if ( C != 0 && C->var.ghost && !Ret_Ghost ) return 0;
  if ( C == 0 && Mak_Ghost ) {
    C = new Client;
    bool ok = C->var.load( Login );
    if (! ok ) { delete C; return 0; } // no unregistered ghosts
    C->var.modified();
    C->var.save();
    C->var.ghost = true;
    add( C );
  }
  return C;
}

VAR_Client* VAR_Service::var( const String& Login )
{
  TSTAT;
  
  if ( Login.empty() ) return 0;
  
  Client* pp = client( Login, false, true ); // use ghost if we have one
  if ( pp != 0 ) return &(pp->var);

  svar.load( Login );
  if ( svar.admin == unreg_level ) return 0;
  return &svar;
}

void VAR_Service::add( Client* Who )
{
  TSTAT;

  if (! Who->var.ghost ) {
    if ( Who->var.star_notify ) star_notify += Who->id();
    if ( Who->var.star_track  ) star_track  += Who->id();
  
    track .add( Who->var.track,  Who->id(), true  );
    ignore.add( Who->var.ignore, Who->id(), true  );
    notify.add( Who->var.notify, Who->id(), true  );
  }
  clients += Who;
}

void VAR_Service::del( Client* Who )
{
  TSTAT;
  
  if ( Who->var.star_notify ) star_notify -= Who->id();
  if ( Who->var.star_track  ) star_track  -= Who->id();
  
  track .del( Who->var.track,  Who->id(), true  );
  ignore.del( Who->var.ignore, Who->id(), true  );
  notify.del( Who->var.notify, Who->id(), true  );

  clients -= Who->id();
}

void VAR_Service::sync()
{
  TSTAT;
  
  if (! connected() ) {
    vc_con << VCFL << endl;
    System::exit(-1);
  }

  SET_PTR_Client::iterator it = clients.begin();
  SET_PTR_Client::iterator hi = clients.end();
  for ( ; it != hi; ++it ) {
    IO_Buffer& sb = (*it)->send_buff;
    if ( sb.size( false ) == 0 ) continue;
    socket->text( sb.buff( false ), sb.size( false ) );
    sb.remove( false, sb.size( false ) );
  }
  
  socket->sync();
}

void VAR_Service::init()
{
  TSTAT;
  
  erase();

  alias = Alias( "A salias $" );
  alias = Alias( "H shistory" );
  alias = Alias( "a accept $" );
  alias = Alias( "b break $" );
  alias = Alias( "c cancel $" );
  alias = Alias( "d decline $" );
  alias = Alias( "f finger $" );
  alias = Alias( "h history $" );
  alias = Alias( "i ignore $" );
  alias = Alias( "l look $" );
  alias = Alias( "m match $" );
  alias = Alias( "n notify $" );
  alias = Alias( "o open $" );
  alias = Alias( "r request $" );
  alias = Alias( "s stored $" );
  alias = Alias( "t track $" );
  alias = Alias( "u uptime $" );
  alias = Alias( "w watch $" );

  dirty = true;
}

void VAR_Service::erase()
{
  TSTAT;
  
  match_cnt = 0;
  alias.  erase();
  rank.   erase();
  history.erase();
  groups. erase();
}

void VAR_Service::save( ostream& os ) const
{
  TSTAT;
  
  os.write( ccptr(&match_cnt), sizeof(match_cnt) );
  alias.  save( os );
  rank.   save( os );
  history.save( os );
  groups. save( os );
  histo_users    .save( os );
  histo_requests .save( os );
  histo_matches  .save( os );
  histo_inp_bytes.save( os );
  histo_out_bytes.save( os );
}

void VAR_Service::save() const
{
  TSTAT;
  
  svar.save();

  if (! dirty ) return; // always save!

  ostringstream os; save( os );

  vc_db.put( login_system, os.str() );

  dirty = false;
}

void VAR_Service::load()
{
  TSTAT;

  erase();

  String sis;
  CRatio ok = vc_db.get( login_system, sis );
  if ( ok.Txt() == 0 ) { vc_con << VCFL; System::exit(1); }
  stringstream is(sis);

  is.read( cptr(&match_cnt), sizeof(match_cnt) );
  alias.  load( is );
  rank.   load( is );
  history.load( is );
  groups. load( is );  
  histo_users    .load( is );
  histo_requests .load( is );
  histo_matches  .load( is );
  histo_inp_bytes.load( is );
  histo_out_bytes.load( is );
  
  dirty = false;
}

bool VAR_Service::name_ok( const String& Name )
{
  TSTAT;
  
  if ( Name.size() < 2 ) return false;
  if ( Name.size() > 8 ) return false;
  for ( uint4 i = 0; i < Name.size(); i++ )
    if ( Name[i] == ',' || !isprint(Name[i]) ) return false;
  return true;
}


//: VAR_Service.C (eof) (c) Igor
