// $Id: VAR_Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Client.C (bof) (c) Igor Durdanovic

#include "HHMMSS.H"
#include "System.H"
#include "Actors.H"
#include "DB_Server.H"
#include "SET_Group.H"
#include "VAR_System.H"
#include "VAR_Client.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void VAR_Client::erase()
{
  TSTAT;
  
  login    .erase();
  passw    .erase();
  name     .erase();
  info     .erase();
  email    .erase();
  time = 0;
  last = 0;
  host_ip  .erase();
  host_name.erase();
  sock = -1;
  port = -1;
  alias    .erase();
  groups   .erase();
  channs   .erase();
  notify   .erase();
  ignore   .erase();
  star_notify   = false;
  notify_groups = false;
  notify_channs = false;
  notify_notify = false;
  notify_ignore = false;
  bell    = FLAG_Bell();
  vt100   = false;
  hear    = true;
  verbose = FLAG_Verb();
  history.erase();
  vars   .erase();
  prefix .erase();
}

void VAR_Client::save( ostream& os ) const
{
  TSTAT;
  
  login    .save( os );
  passw    .save( os );
  name     .save( os );
  info     .save( os );
  email    .save( os );
  os.write( ccptr(&time), sizeof(time) );
  os.write( ccptr(&last), sizeof(last) );
  host_ip  .save( os );
  host_name.save( os );
  os.write( ccptr(&sock), sizeof(sock) );
  os.write( ccptr(&port), sizeof(port) );
  alias    .save( os );
  groups   .save( os );
  channs   .save( os );
  notify   .save( os );
  ignore   .save( os );
  bell     .save( os );
  os.write( ccptr(&star_notify),    sizeof(star_notify) );
  os.write( ccptr(&notify_groups),  sizeof(notify_groups) );
  os.write( ccptr(&notify_channs),  sizeof(notify_channs) );
  os.write( ccptr(&notify_notify),  sizeof(notify_notify) );
  os.write( ccptr(&notify_ignore),  sizeof(notify_ignore) );
  os.write( ccptr(&vt100),  sizeof(vt100) );
  os.write( ccptr(&hear),   sizeof(hear) );
  verbose  .save( os );
  history  .save( os );
  vars     .save( os );
}

void VAR_Client::save() const
{
  TSTAT;
  
  if ( name.empty() ) dirty = false;

  if ( name.empty() && email.empty() && ! login.empty() ) vc_db.del( login );
    
  if (! dirty ) return;

  ostringstream os;
  save( os );
  dblen = vc_db.put( login, os.str() );
  dirty = false;
}

void VAR_Client::load( const String& Login )
{
  TSTAT;
  
  erase();

  String sis;
  CRatio ok = vc_db.get( Login, sis );
  if ( ok.Txt() == 0 ) return;
  stringstream is(sis); 

  dblen = ok;
  
  login    .load( is );
  passw    .load( is );
  name     .load( is );
  info     .load( is );
  email    .load( is );
  is.read( cptr(&time), sizeof(time) );
  is.read( cptr(&last), sizeof(last) );
  host_ip  .load( is );
  host_name.load( is );
  is.read( cptr(&sock), sizeof(sock) );
  is.read( cptr(&port), sizeof(port) );
  alias    .load( is );
  groups   .load( is );
  channs   .load( is );
  notify   .load( is );
  ignore   .load( is );
  bell     .load( is );
  is.read( cptr(&star_notify),    sizeof(star_notify) );
  is.read( cptr(&notify_groups),  sizeof(notify_groups) );
  is.read( cptr(&notify_channs),  sizeof(notify_channs) );
  is.read( cptr(&notify_notify),  sizeof(notify_notify) );
  is.read( cptr(&notify_ignore),  sizeof(notify_ignore) );
  is.read( cptr(&vt100),  sizeof(vt100) );
  is.read( cptr(&hear),   sizeof(hear) );
  verbose  .load( is );
  history  .load( is );
  vars     .load( is );

  dirty = false;
}

void VAR_Client::init()
{
  TSTAT;
  
  make( login_root, 
	login_root, 
	Central_root_name,
	Central_root_email );

  groups += group_admin;
}

void VAR_Client::make( const String& Login,
		       const String& Passw, 
		       const String& Name,  
		       const String& Email ) 
{
  TSTAT;
  
  erase();
  
  login   = Login;
  passw   = Passw;
  name    = Name;
  email   = Email;

  groups += group_client;
  channs += chann_help;
  channs += chann_chat;

  dirty = true;
}

sint4 VAR_Client::admin() const
{
  TSTAT;
  
  if ( login == login_root   ) return  root_level;
  if ( groups( group_admin ) ) return admin_level;
  if (! email.empty()        ) return  user_level;
  return unreg_level;
}

ostream& VAR_Client::print( ostream& os, const VAR_Client& P ) const
{
  TSTAT;
  
  bool ok = (P.admin() > admin() && P.admin() > user_level) || &P == this;

  os << "login  : " << login << EOL;
  dblen.print( os << "dblen  : ", "%5.1f = ", 1, 1 ) << EOL;
  if ( ok ) os << "passw  : " << passw << EOL;

  os << "name   : " << name  << EOL;
  os << "info   : " << info  << EOL;
  if ( ok )
  os << "email  : " << email << EOL;

  if ( vc_var.client( login ) != 0 )
    os << "since  : " << System::dtime( time ) << EOL; 
  else
    os << "last   : " << System::dtime( last ) << EOL;

  if ( vc_var.client( login ) != 0 ) {
    os << "idle   : " 
       << setw(8) << HHMMSS( System::clock() - last )
       << ", on line : " 
       << setw(8) << HHMMSS( System::clock() - time ) << EOL;
  } else 
    os << "after  : " << setw(8) << HHMMSS( last - time ) << EOL;

  if ( ok ) {
    os << "host   : " << host_ip << ' ' << host_name << EOL;
    os << "sock   : " << sock << " @ " << port;
    IO_TCP_Client* ip = vc_var.client( login );
    if ( ip ) {
      os << " S(" << ip->send_buff.size(false) << '/' << ip->send_buff.size(true) << ')';
      os << " R(" << ip->recv_buff.size(false) << '/' << ip->recv_buff.size(true) << ')';
      os << " E(" << ip->excp_buff.size(false) << '/' << ip->excp_buff.size(true) << ')';
    }
    os << EOL;
  }

  os << "bell   : " << bell << EOL
     << "verbose: " << verbose << EOL
     << "vt100  : " << ( vt100 ? '+' : '-' ) << EOL
     << "hear   : " << ( hear  ? '+' : '-' ) << EOL;

  os << "groups(" << (notify_groups ? '+' : '-') << ") : " << groups << EOL;
  os << "channs(" << (notify_channs ? '+' : '-') << ") : "; channs.print( os, !ok, false ); os << EOL;
  os << "notify(" << (notify_notify ? '+' : '-') << ") : " << ( star_notify ? "* " : "" ) << notify << EOL;
  os << "ignore(" << (notify_ignore ? '+' : '-') << ") : " << ignore;

  return os;
}

//: VAR_Client.C (eof) (c) Igor
