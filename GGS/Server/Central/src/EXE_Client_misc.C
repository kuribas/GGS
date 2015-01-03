// $Id: EXE_Client_misc.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client_misc.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "Actors.H"
#include "Signal.H"
#include "EXE_Client.H"
#include "VAR_System.H"
#include "Message.H"
#include "VT100.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

// when a user logs in he receives notify from everybody
void EXE_Client::recv_notify( IO_TCP_Client& who )
{
  if ( who.var.groups( group_service ) != 0 ) { recv_service_notify( who ); return; }
  if ( who.var.star_notify )                  { recv_star_notify   ( who ); return; }

  TSTAT;
  
  SET_String::const_iterator it = who.var.notify.begin();
  SET_String::const_iterator hi = who.var.notify.end();
  bool any = false;
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it ); 
    if (    0 == ip ) continue;
    if ( &who == ip ) continue;
    if (  who.ignore( ip ) ) continue;
    if (! any ) who.interrupt();
    who << ": + " << ip->id() << ' ' << ip->var.admin() << EOL;
    if (! any ) who.sync();
    any = true;
  }
}

// when a user logs in, notify is sent to others
void EXE_Client::send_notify( const IO_TCP_Client& who, bool Arrived )
{
  send_star_notify( who, Arrived );
  
  Group* gp = vc_var.notify( who.id() ); if (! gp ) return;

  TSTAT;
  
  ostringstream os;
  os << ": " << ( Arrived ? "+ " : "- " ) << who.id() << ' ' << who.var.admin() << EOL;

  SET_String::iterator it = gp->obj.begin();
  SET_String::iterator hi = gp->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it ); 
    if ( ip == 0 ) continue;
    if ( ip->var.star_notify ) continue; // will receive it through *
    if ( ip->ignore( &who ) ) continue;
    ip->interrupt();
    ip->bell( ip->var.bell.notify() );
    ip->text( os );
    ip->ready();
    ip->sync();
  }
}


void EXE_Client::recv_star_notify( IO_TCP_Client& who ) // sends list of connected users
{
  TSTAT;
  
  if ( vc_var.clients.size() == 0 ) return;

  who.interrupt();
  
  SET_PTR_Client::const_iterator it = vc_var.clients.begin();
  SET_PTR_Client::const_iterator hi = vc_var.clients.end();
  for ( ; it != hi; ++it ) {
    const IO_TCP_Client* ip = &**it;
    if (  who.ignore( ip ) ) continue;
    who << ": + " << ip->id() << ' ' << ip->var.admin() << EOL;
  }

  who.ready();
  who.sync();
}

void EXE_Client::recv_service_notify( IO_TCP_Client& who ) // sends list of connected users
{
  TSTAT;
  
  if ( vc_var.clients.size() == 0 ) return;

  who.interrupt();
  
  SET_PTR_Client::const_iterator it = vc_var.clients.begin();
  SET_PTR_Client::const_iterator hi = vc_var.clients.end();
  for ( ; it != hi; ++it ) {
    const IO_TCP_Client* ip = &**it;
    if ( who.ignore( ip ) ) continue;
    who << ": + " << ip->id() << ' ' << (*it)->var.admin() << EOL;
  }

  who.ready();
  who.sync();
}

void EXE_Client::send_star_notify( const IO_TCP_Client& who, bool Arrived )
{
  TSTAT;
  
  send_service_notify( who, Arrived );
  
  ostringstream os;
  os << ": " << ( Arrived ? "+ " : "- " ) << who.id() << ' ' << who.var.admin();

  SET_String::iterator it = vc_var.star_notify.begin();
  SET_String::iterator hi = vc_var.star_notify.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0 ) continue;
    if ( ip == &who ) continue;
    if ( ip->ignore( &who ) ) continue;
    if ( ip->var.groups( group_service ) ) continue; // service are separate
    ip->interrupt();
    ip->bell( ip->var.bell.notify() );
    ip->text( os ) << EOL;
    ip->ready();
    ip->sync();
  }
}

void EXE_Client::send_service_notify( const IO_TCP_Client& who, bool Arrived )
{
  TSTAT;
  
  Group* G = vc_var.groups( group_service );
  if ( G == 0 ) return;

  ostringstream os;
  os << ": " << ( Arrived ? "+ " : "- " ) << who.id() << ' ' << who.var.admin();

  SET_String::iterator it = G->obj.begin();
  SET_String::iterator hi = G->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0 ) continue;
    if ( ip == &who ) continue;
    if ( ip->ignore( &who ) ) continue;
    ip->interrupt();
    ip->bell( ip->var.bell.notify() );
    ip->text( os ) << EOL;
    ip->ready();
    ip->sync();
  }
}

//

bool EXE_Client::add_group( IO_TCP_Client& who, const String& user, const String& group )
{
  TSTAT;
  
  Group*      gp = vc_var.groups( group ); if ( gp == 0 ) return false;
  VAR_Client* vp = vc_var.var   ( user );  if ( vp == 0 ) return false;

  if ( who.var.admin() <= vp->admin() && vp->login != who.id() ) {
    vc_mssg._041( who.var.vt100, who );
    return false;
  }

  vp->groups += group;
  vp->modified();

  if ( vc_var.client( vp->login ) == 0 ) vp->save();

  gp->obj += vp->login;

  return true;
}

bool EXE_Client::del_group( IO_TCP_Client& who, const String& user, const String& group )
{
  TSTAT;
  
  Group*      gp = vc_var.groups( group ); if ( gp == 0 ) return false;
  VAR_Client* vp = vc_var.var   ( user );  if ( vp == 0 ) return false;
  
  if ( who.var.admin() <= vp->admin() && vp->login != who.id() ) {
    vc_mssg._041( who.var.vt100, who );
    return false;
  }

  vp->groups -= group;
  vp->modified();

  if ( vc_var.client( vp->login ) == 0 ) vp->save();

  gp->obj -= vp->login;

  return true;
}

//: EXE_Client_misc.C (eof) (c) Igor
