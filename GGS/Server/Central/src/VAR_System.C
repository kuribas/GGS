// $Id: VAR_System.C 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_System.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "Actors.H"
#include "DB_Server.H"
#include "EXE_Client.H"
#include "SET_PTR_Client.H"
#include "IO_FILE.H"
#include "VAR_System.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

VAR_Client VAR_System::svar;

using namespace std;

VAR_System::VAR_System()
  : history( 32 ),
    dirty( false )
{
  System::gethostname( host );
  svar.make( str_empty, str_empty, str_empty, str_empty );
  vc_con << VCTIME << "  VAR_System()" << endl;

  if ( this == &vc_var ) vc_var_ready = true;
}

VAR_System::~VAR_System()
{
  if ( this == &vc_var && ! vc_var_ready ) return;

  modified(); // always save ?!
  save();
  vc_con << VCTIME << " ~VAR_System()" << endl;
}

void VAR_System::init()
{
  TSTAT;
  
  erase();

  groups += Group( group_client );
  groups += Group( group_admin  );
  groups += Group( group_service );

  alias = Alias( "A    salias" );
  alias = Alias( "H    shistory" );
  alias = Alias( "a    alias" );
  alias = Alias( "c    chann" );
  alias = Alias( "f    finger" );
  alias = Alias( "g    group" );
  alias = Alias( "h    history" );
  alias = Alias( "i    ignore" );
  alias = Alias( "m    mux" );
  alias = Alias( "n    notify" );
  alias = Alias( "q    quit" );
  alias = Alias( "t    tell" );
  alias = Alias( "to   $t /os" );
  alias = Alias( "tom  $to ask  $otype" );
  alias = Alias( "top  $to play $match" );
  alias = Alias( "tor  $to rank $otype" );
  alias = Alias( "tot  $to top  $otype" );
  alias = Alias( "tow  $to who  $otype" );
  alias = Alias( "u    uptime" );
  alias = Alias( "w    who" );

  dirty = true;
}

void VAR_System::erase()
{
  TSTAT;
  
  alias.  erase();
  groups. erase();
  history.erase();

  host.   erase();
  channs. erase();
  ignore. erase();
  notify. erase();
  star_notify.erase();

  dirty = false;
}

void VAR_System::save( ostream& os ) const
{
  TSTAT;
   
  alias          .save( os );
  SET_String Groups; groups.vec( Groups ); Groups.save( os );
  history        .save( os );
  histo_users    .save( os );
  histo_inp_bytes.save( os );
  histo_out_bytes.save( os );
}

void VAR_System::save() const
{
  TSTAT;
  
  svar.save();

  if (! dirty ) return;

  ostringstream os; save( os );

  vc_db.put( login_system, os.str() );

  dirty = false;
}

void VAR_System::load()
{
  TSTAT;
  
  erase();

  String sis;
  CRatio ok = vc_db.get( login_system, sis );
  if ( ok.Txt() == 0 ) { vc_con << VCFL; System::exit(1); }
  stringstream is(sis);

  alias . load( is );
  SET_String Groups; Groups.load( is );
  SET_String::const_iterator it = Groups.begin();
  SET_String::const_iterator hi = Groups.end();
  for ( ; it != hi; ++it ) groups += Group( *it );
  groups += Group( group_client );
  groups += Group( group_admin  );
  groups += Group( group_service );

  history        .load( is );
  histo_users    .load( is );
  histo_inp_bytes.load( is );
  histo_out_bytes.load( is );
  dirty = false;
}

void VAR_System::add( IO_TCP_Client* Who )
{
  TSTAT;
  
  groups.add( Who->var.groups, Who->var.login, false );
  channs.add( Who->var.channs, Who->var.login, true  );
  ignore.add( Who->var.ignore, Who->var.login, true  );
  notify.add( Who->var.notify, Who->var.login, true  );
  if ( Who->var.star_notify ) star_notify += Who->id();
     
  Who->var.time = System::clock();
  Who->var.last = System::clock();
  Who->var.sock = Who->io_desc();
  if ( Who->io_mux() == 0 || ! Who->io_full() ) {
    System::getsockhost( Who->var.host_name, Who->var.host_ip, Who->var.port, Who->io_desc() );
  } else {
    Who->var.host_name = Who->host_name;
    Who->var.host_ip   = Who->host_ip;
    Who->var.port      = Who->port;
  }
  clients += Who;
}

void VAR_System::del( IO_TCP_Client* Who )
{
  TSTAT;
  
  groups.del( Who->var.groups, Who->var.login, false );
  channs.del( Who->var.channs, Who->var.login, true  );
  ignore.del( Who->var.ignore, Who->var.login, true  );
  notify.del( Who->var.notify, Who->var.login, true  );
  if ( Who->var.star_notify ) star_notify -= Who->id();

  History H( Who->var.login, 
	     Who->var.admin(), Who->var.time, System::clock(), // Who->var.last, 
	     Who->var.host_ip, Who->var.host_name );

  Who->var.history.push( H );
  Who->var.last = System::clock();
  Who->var.modified();

  if ( Who->state == IO_TCP_Client::on_line ) {
    clients -= Who->var.login;
    history.push( H );
  }
  
  modified();
}

//

bool VAR_System::login_ok( const String& Login )
{
  TSTAT;
  
  if ( Login.size() < 2 ) return false;
  if ( Login.size() > 8 ) return false;
  if ( Login[0] == '.' || Login[0] == '_' ) return false;
  for ( uint4 i = 0; i < Login.size(); i++ ) {
    if ( Login[i] == ',' || isspace(Login[i]) || !isprint(Login[i]) ) return false;
  }
  return true;
}

bool VAR_System::name_ok( const String& Name )
{
  TSTAT;
  
  if ( Name.size() < 2 ) return false;
  if ( Name.size() > 8 ) return false;
  for ( uint4 i = 0; i < Name.size(); i++ )
    if ( Name[i] == ',' || !isprint(Name[i]) ) return false;
  return true;
}

VAR_Client* VAR_System::var( const String& user )
{
  TSTAT;
  
  IO_TCP_Client* ip = client( user ); if ( ip != 0 ) return &(ip->var);
  svar.load( user );
  if (! svar.name.empty() ) return &svar;

  return 0;
}

IO_TCP_Client* VAR_System::client( const String& user )
{
  TSTAT;
  
  return clients( user );
}

//: VAR_System.C (eof) (c) Igor
