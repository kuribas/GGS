// $Id: VC_Request.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Request.C (bof) (c) Igor Durdanovic

#include "CRatio.H"
#include "Actors.H"
#include "Stat.H"
#include "Client.H"
#include "DB_Server.H"
#include "VAR_Service.H"
#include "EXE_Service.H"
#include "VC_Request.H"
#include "SET_PTR_Request.H"
#include "Histogram_HDMY.H"
#include "Message.H"
#include "VT100.H"
#include <sstream>
#include <iomanip>
#include "Match.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

#define MAX_STORED 64

using namespace std;

bool local_duplicated( const VC_Request& R1, const Request& r2 );
bool globl_duplicated( const VC_Request& R1, const Request& r2 );
bool local_recv_eq_this_local_send( const VC_Request& R1, const Request& r2 );
bool globl_send_eq_this_local_send( const VC_Request& G1, const Request& l2 );
bool local_recv_eq_this_globl_send( const VC_Request& L1, const Request& g2 );
bool globl_send_eq_this_globl_send( const VC_Request& G1, const Request& g2 );

  
bool Request::parse( Client* P1, const String& s, bool NoError )
{
  TSTAT;

  t_d.erase();
  t_id.erase();
    
  p1 = P1;
  p2 = 0;

  is_rated  = p1->var.rated;
  
  String arg, tmp, rst = s;
  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg[0] == '.' ) { // we have load request
    if (! p1->var.stored[ arg ] ) {
      if (! NoError ) { vc_mssg._032( P1->vt100(), P1->tell(), arg ) << EOM; }
      return false; 
    }
    String sdata;
    CRatio ok = vc_save.get( arg, sdata );
    if ( ok.Txt() == 0 ) {
      if (! NoError ) { vc_mssg._032( P1->vt100(), P1->tell(), arg ) << EOM; }
      return false;
    }
    stringstream data(sdata);
    load( data, false );
    if ( p1 != P1 && p2 != P1 ) {
      if (! NoError ) { vc_mssg._033( P1->vt100(), P1->tell(), arg ) << EOM; }
      return false;
    }
    if (! t_d.empty() ) {
      if (! NoError ) { vc_mssg._077( P1->vt100(), P1->tell(), arg ) << EOM; }
      return false;
    }
    if ( p1 ==  0 || p2 ==  0 || p1->ghost() || p2->ghost() ) {
      if (! NoError ) { vc_mssg._034( P1->vt100(), P1->tell(), arg ) << EOM; }
      return false;
    }
    counter = arg;
    if ( p1 != P1 ) { Client* t = p1; p1 = p2; p2 = t; }
    return true;
  }
  
  ostringstream os;
  istringstream iarg( arg );
  if (!t.parse( os, iarg ) ) {
    if (! NoError ) { P1->tell().vt100( vt_error, os ) << EOM; }
    return false;
  }

  String::parse( rst, arg, tmp );
  if ( c[0].parse( arg ) ) {
    rst = tmp;
    String::parse( rst, arg, tmp ); 
    if ( !arg.empty() && c[1].parse( arg ) ) rst = tmp; else c[1] = c[0];
    c[2] = c[0];
    c[3] = c[1];
  }

  if ( rst.empty() ) return true; // global
  
  p2 = vc_var.client( rst, false, false );
  if ( p2 == 0 ) {
    if (! NoError ) { vc_mssg._016( P1->vt100(), P1->tell(), rst ) << EOM; }
    return false;
  }
  
  if ( p1 == p2 ) is_rated = false;

  {
    ostringstream os;

    if (!t.players_ok(os, p1->id(), p2->id())) {
      if (! NoError ) { P1->tell().vt100( vt_error, os ) << EOM; }
      return false;
    }
  }
  
  return true;
}

// <td.id> <type> <login1> <time1> <login2> <time2>
// <td.id> <stored.id>

bool Request::tdparse( Client* TD, const String& s )
{
  TSTAT;

  t_d = TD->id();
  t_id.erase();
  
  if ( s.empty() ) { vc_mssg._066( TD->vt100(), TD->tell(), s ) << EOM; return false; }
  
  p1 = 0;
  p2 = 0;

  is_rated = true;

  String arg, tmp, rst = s;
  String::parse( rst, t_id, tmp ); rst = tmp;

  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg.empty() ) { vc_mssg._066( TD->vt100(), TD->tell(), t_id ) << EOM; return false; }
  
  if ( arg[0] == '.' ) { // we have a load request
    String sdata;
    CRatio ok = vc_save.get( arg, sdata );
    if ( ok.Txt() == 0 ) {
      vc_mssg._067( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
    }
    stringstream data(sdata);
    load( data, true ); // will make ghosts if needed
    if ( p1 ==  0 || p2 ==  0 ) {
      vc_mssg._065( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
    }
    if ( TD->id() != t_d ) { // not this TD!!
      vc_mssg._076( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
    }
    counter = arg;
    return true;
  }
  
  ostringstream os; // <type>
  istringstream iarg( arg );
  if (!t.parse( os, iarg ) ) {
    vc_mssg._069( TD->vt100(), TD->tell(), t_id );
    TD->text( os ) << EOM;
    return false;
  }

  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg.empty() ) {
    vc_mssg._066( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  p1 = vc_var.client( arg, true, true ); // make ghost if necessary

  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg.empty() ) {
    vc_mssg._066( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  if (! c[0].parse( arg ) ) {
    vc_mssg._068( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }

  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg.empty() ) {
    vc_mssg._066( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  p2 = vc_var.client( arg, true, true ); // make ghost if necessary

  String::parse( rst, arg, tmp ); rst = tmp;
  if ( arg.empty() ) {
    vc_mssg._066( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  if (! c[1].parse( arg ) ) {
    vc_mssg._068( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }

  if ( p1 ==  0 || p2 ==  0 ) {
    vc_mssg._065( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  if ( p1 == p2 ) {
    vc_mssg._070( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  if ( p1->admin() < user_level || p2->admin() < user_level ) {
    vc_mssg._071( TD->vt100(), TD->tell(), t_id ) << EOM; return false;
  }
  
  {
    ostringstream os;
    if (!t.players_ok(os, p1->id(), p2->id())) {
      vc_mssg._069( TD->vt100(), TD->tell(), t_id );
      TD->text( os ) << EOM;
      return false;
    }
  }
  
  c[2] = c[0];
  c[3] = c[1];

  return true;
}

const String& Request::name  ( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? p1 : p2 ) -> id();
}

/* */ real8   Request::rating( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? p1 : p2 ) -> var.rating( t.key() ).Rank();
}

/* */ real8   Request::ratdev( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? p1 : p2 ) -> var.rating( t.key() ).Rdev();
}

sint4 Request::stored() const
{
  TSTAT;

  return p1->var.stored.count( p1->id(), p2->id() );
  
  // was:  return p1->var.stored.count( p2->id() );
}

char Request::color( uint4 i ) const
{
  TSTAT;
  
  sint4 C = t.get_pref_color();
  if ( i == 0 ) {
    switch ( C ) {
    case Color::BLACK : return 'B';
    case Color::WHITE : return 'W';
    default           : return '?';
    }
  } else {
    switch ( C ) {
    case Color::BLACK : return 'W';
    case Color::WHITE : return 'B';
    default           : return '?';
    }
  }
}

ostream& Request::print( ostream& os ) const
{
  TSTAT;
  
  Form( os, "%6.1f %-8s ", p1->rank( t.key() ), p1->id().c_str() );
  c[0].print( os, true ) << ' ';
  if ( counter.empty() ) {
    os << setw(8) << t << ( is_rated ? ( !t_d.empty() ? " T " : " R " ) : " U " );
  } else {
    os << setw(8) << t << " S ";
  }
  if ( p2 ) Form( os, "%6.1f %-8s", p2->rank( t.key() ), p2->id().c_str() );
  else os << "*      *       ";
  if (! (c[0] == c[1]) ) c[1].print( os << ' ', true );
  if (! counter.empty() ) os << ' ' << counter;
  return os;
}

ostream& Request::save( ostream& os ) const
{
  TSTAT;
  
  os.write( ccptr(&refcnt), sizeof(refcnt) );
  n1.save( os );
  n2.save( os );
  for ( int i = 0; i < 4; ++i ) c[i].save( os );
  t.save( os );
  t_d.save( os );
  return os;
}

istream& Request::load( istream& is, bool Make_Ghost )
{
  TSTAT;
  
  is.read( cptr(&refcnt), sizeof(refcnt) );
  is_rated  = true;
  n1.load( is );
  n2.load( is );
  p1 = vc_var.client( n1, Make_Ghost, Make_Ghost );
  p2 = vc_var.client( n2, Make_Ghost, Make_Ghost );
  for ( int i = 0; i < 4; ++i ) c[i].load( is );
  t.load( is );
  t_d.load( is );
  return is;
}

//

SET_id<64> VC_Request::id_cnt;

VC_Request::VC_Request( const Request& R )
  : rid( id_cnt.get() ), req(R)
{
  TSTAT;
  
  {
    ostringstream os; os << '.' << rid;
    sid.append( os.str() );
  }
  
  if ( req.p2 ) { vc_var.local += this; } else { vc_var.global += this; }

  ostringstream ro[2]; // bell
  bool       rb[2] = { false, false };
  for ( int i = 0; i < 2; ++i ) ro[i] << "tell ";
  
  /*         */{ req.p1->var.send += sid; bool b = req.p1->var.bell.request(); if ( rb[b] ) ro[b] << ','; else rb[b] = true; ro[b] << req.p1->id(); }
  if ( req.p2 ){ req.p2->var.recv += sid; bool b = req.p2->var.bell.request(); if ( rb[b] ) ro[b] << ','; else rb[b] = true; ro[b] << req.p2->id(); }

  if (!req.p2 ) notify( ro, rb );

  print( ro[0] << " + "   ); 
  print( ro[1] << " + " );

  if ( vc_var.connected() ) {
    for ( int i = 0; i < 2; ++i ) if ( rb[i] ) vc_var.socket->text( ro[i] ) << EOM;
  }
  
  ++vc_stat.n_request;
  vc_var.histo_requests.update( vc_stat.n_request.current() );
  vc_var.modified();
}

VC_Request::~VC_Request()
{
  TSTAT;
  
  --vc_stat.n_request;
  vc_var.histo_requests.update( vc_stat.n_request.current() );
  vc_var.modified();

  ostringstream ro[2]; // bell
  bool       rb[2] = { false, false };
  for ( int i = 0; i < 2; ++i ) ro[i] << "tell ";
  
  if (!req.p2 ) notify( ro, rb );

  /*         */{ req.p1->var.send -= sid; bool b = req.p1->var.bell.request(); if ( rb[b] ) ro[b] << ','; else rb[b] = true; ro[b] << req.p1->id(); }
  if ( req.p2 ){ req.p2->var.recv -= sid; bool b = req.p2->var.bell.request(); if ( rb[b] ) ro[b] << ','; else rb[b] = true; ro[b] << req.p2->id(); }

  print( ro[0] << " - "   ); 
  print( ro[1] << " - " );

  if ( vc_var.connected() ) {
    for ( int i = 0; i < 2; ++i )
      if ( rb[i] ) vc_var.socket->text( ro[i] ) << EOM;
  }
  
  if ( req.p2 ) { vc_var.local -= sid; } else { vc_var.global -= sid; }

  id_cnt.put( rid );
}

void VC_Request::notify( ostringstream* ro, bool* rb ) const
{
  TSTAT;
  
  SET_PTR_Client::iterator it = vc_var.clients.begin();
  SET_PTR_Client::iterator hi = vc_var.clients.end();
  for ( ; it != hi; ++it ) {
    if ( req.p1 == &**it ) continue;
    if ( req.p1->ignore( &**it ) ) continue;
    if ( (*it)->ignore( req.p1 ) ) continue;
    if ( req.is_rated != (*it)->var.rated ) continue;
    if ( (*it)->var.open <= (*it)->var.play.size() ) continue;
    if ( (*it)->var.notify_request ) {
      req.p2 = &**it;
      bool df1 = req.p1->var.dformula.eval( req.p1, req ); // they do not decline
      bool df2 = req.p2->var.dformula.eval( req.p2, req );
      req.p2 = 0;
      if ( ! df1 && ! df2 ) {
	bool b = (*it)->var.bell.notify_request();
	if ( rb[b] ) ro[b] << ','; else rb[b] = true;
	ro[b] << (*it)->id();
      }
    }
  }
}

VC_Match* VC_Request::parse( Client* P1, const String& arg, bool NoError )
{
  TSTAT;
  
  Request R;

  if ( R.parse( P1, arg, NoError ) ) {
    if ( R.rated() && R.p1->var.stored.size() >= MAX_STORED && R.counter.empty() ) {
      if (! NoError ) { vc_mssg._041( P1->vt100(), P1->tell() ) << EOM; }
      return 0;
    }
    
    if ( R.p2 != 0 ) {
      if ( R.rated() && R.p2->var.stored.size() >= MAX_STORED && R.counter.empty() ) {
	if (! NoError ) { vc_mssg._042( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }
      
      if ( R.p2->ignore( R.p1 ) ) { // check for ignore
	if (! NoError ) { vc_mssg._039( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }
      if ( R.p1->ignore( R.p2 ) ) {
	if (! NoError ) { vc_mssg._040( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }
      
      if ( R.p1->var.rated && !R.p2->var.rated ) {
	if (! NoError ) { vc_mssg._047( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }
      
      if ( R.p2->var.open <= R.p2->var.play.size() ) { // opp not open
	if (! NoError ) { vc_mssg._024( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }

      bool ad = R.p2->var.dformula.eval( R.p2, R ); // check auto-decline
      if ( ad ) {
	if (! NoError ) { vc_mssg._017( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }

      bool aa = R.p2->var.aformula.eval( R.p2, R ); // check auto-accept
      if ( aa ) {
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	VC_Match* M = new Match( R );
	return M;
      }

      if ( vc_var.local.find( R, local_duplicated ) != 0 ) {
	if (! NoError ) { vc_mssg._019( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }
      
      VC_Request* p = vc_var.local.find( R, local_recv_eq_this_local_send );
      if ( p != 0 ) { // we have received similar request (from P2)
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	VC_Match* M = new Match( p->req );
	delete p;
	return M;
      }
      
      p = vc_var.global.find( R, globl_send_eq_this_local_send );
      if ( p != 0 ) { // there is a similar global request (from P2)
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	p->req.p2 = P1;
	VC_Match* M = new Match( p->req );
	p->req.p2 = 0;
	delete p;
	return M;
      }

      if ( R.p1 == R.p2 ) { // self playing ...
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	VC_Match* M = new Match( R );
	return M;
      }
    }

    if ( R.p2 == 0 ) {
      if ( vc_var.global.find( R, globl_duplicated ) != 0 ) {
	if (! NoError ) { vc_mssg._019( P1->vt100(), P1->tell() ) << EOM; }
	return 0;
      }

      VC_Request* p = vc_var.global.find( R, globl_send_eq_this_globl_send );
      if ( p != 0 ) { // there is a similar global request
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	p->req.p2 = P1;
	VC_Match* M = new Match( p->req );
	p->req.p2 = 0;
	delete p;
	return M;
      }
      
      p = vc_var.local.find( R, local_recv_eq_this_globl_send );
      if ( p != 0 ) { // we have received a similar request (from P2)
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	VC_Match* M = new Match( p->req );
	delete p;
	return M;
      }

      bool any = global_find_client( R );
      if ( any ) { // we have found a user that listens to global and fits formula
	++vc_stat.n_request;
	vc_var.histo_requests.update( vc_stat.n_request.current() );
	vc_var.modified();
	--vc_stat.n_request;
	VC_Match* M = new Match( R );
	delete p;
	return M;
      }
    }
    
    new VC_Request( R );
  }

  return 0;
}

VC_Match* VC_Request::tdparse( Client* TD, const String& arg )
{
  TSTAT;
  
  Request R;

  if ( R.tdparse( TD, arg ) ) { // start new match ..
    ++vc_stat.n_request;
    vc_var.histo_requests.update( vc_stat.n_request.current() );
    vc_var.modified();
    --vc_stat.n_request;
    VC_Match* M = new Match( R );
    return M;
  } else { // get rid of ghost players
    if ( R.p1 ) R.p1->ghost2void();
    if ( R.p2 ) R.p2->ghost2void();
  }

  return 0;
}

ostream& VC_Request::print( ostream& os ) const
{
  TSTAT;
  
  return os << setw(4) << id() << ' ' << req;
}

//

bool VC_Request::global_find_client( Request& r )
{
  TSTAT;

  vector< Client* > clients;  // all clients
  {
    SET_PTR_Client::iterator it = vc_var.clients.begin();
    SET_PTR_Client::iterator hi = vc_var.clients.end();
    for ( ; it != hi; ++it ) { clients.push_back( &**it ); }
  }
  random_shuffle( clients.begin(), clients.end() );
  
  vector< Client* >::iterator it = clients.begin();
  vector< Client* >::iterator hi = clients.end();
  for ( ; it != hi; ++it ) {
    if ( r.p1 == *it ) continue;
    if ( r.p1->ignore( *it ) ) continue;
    if ( (*it)->ignore( r.p1 ) ) continue;
    if ( r.rated() != (*it)->var.rated ) continue;
    if ( (*it)->var.open <= (*it)->var.play.size() ) continue;
    r.p2 = *it;
    bool df1 = r.p1->var.dformula.eval( r.p1, r ); // sender    implicitely accepts
    bool af2 = r.p2->var.aformula.eval( r.p2, r ); // recepient explicitely accepts
    bool df2 = r.p2->var.dformula.eval( r.p2, r ); // and declines
    if ( ! df1 && af2 && ! df2 ) return true;
    r.p2 = 0;
  }
  return false;
}

bool local_duplicated( const VC_Request& R1, const Request& r2 )
{
  TSTAT;
  
  const Request& r1 = R1.req;
  
  return
    r1.type().is_equal( r2.type() ) &&
    r1.rated() == r2.rated() &&
    r1.p1    == r2.p1 &&
    r1.p2    == r2.p2 &&
    r1.c[0]    == r2.c[0] &&
    r1.c[1]    == r2.c[1];
}

bool globl_duplicated( const VC_Request& R1, const Request& r2 )
{
  TSTAT;
  
  const Request& r1 = R1.req;
  
  return
    r1.type().is_equal( r2.type() ) &&
    r1.rated() == r2.rated() &&
    r1.p1    == r2.p1 &&
    r1.c[0]    == r2.c[0] &&
    r1.c[1]    == r2.c[1];
}

bool local_recv_eq_this_local_send( const VC_Request& R1, const Request& r2 )
{
  TSTAT;
  
  const Request& r1 = R1.req;

  return
    r1.counter == r2.counter &&
    r1.type().is_matching( r2.type() ) &&
    r1.rated() == r2.rated() &&
    r1.p2    == r2.p1 &&
    r1.p1    == r2.p2 &&
    r1.c[0]  == r2.c[1] &&
    r1.c[1]  == r2.c[0];
}

/* g.p1: A  l.p1: B
   g.p2: B* l.p2: A
   A.aform = T (implicit, it has sent the request!)
   A.dform = ? (explicit, global)
   B.aform = T (implicit, it has sent the request!)
   B.dform = T (implicit, it has sent the request!)
 */
bool globl_send_eq_this_local_send( const VC_Request& G1, const Request& l2 )
{
  TSTAT;
  
  const Request& g1 = G1.req;

  if ( g1.p1      != l2.p2 ||
       g1.rated() != l2.rated() ||
      !g1.type().is_matching( l2.t ) ) return false;
  
  g1.p2 = l2.p1;
  bool g1p1_d = g1.p1->var.dformula.eval( g1.p1, g1 );
  g1.p2 = 0;
  
  return
    ! g1p1_d &&
    g1.c[0] == l2.c[1] &&
    g1.c[1] == l2.c[0];
} 

/* l.p1: A  g.p1: B
   l.p2: B  g.p2: A*
   A.aform = T (implicit, it has sent the request!)
   A.dform = T (implicit, it has sent the request!)
   B.aform = T (implicit, it has sent the request!)
   B.dform = F (explicit, global)
 */
bool local_recv_eq_this_globl_send( const VC_Request& L1, const Request& g2 )
{
  TSTAT;
  
  const Request& l1 = L1.req;

  if ( l1.p2      != g2.p1    || 
       l1.rated() != g2.rated() ||
      !l1.type().is_matching( g2.t ) ) return false;
  
  g2.p2 = l1.p1;
  bool g2p1_d = g2.p1->var.dformula.eval( g2.p1, g2 );
  g2.p2 = 0;

  return
    ! g2p1_d &&
    l1.c[0] == g2.c[1] &&
    l1.c[1] == g2.c[0];
} 

/* g.p1: A  g.p1: B
   g.p2: B* g.p2: A*
   A.aform = T (implicit, it has sent the request!)
   A.dform = F (explicit, global)
   B.aform = T (implicit, it has sent the request!)
   B.dform = F (explicit, global)
 */
bool globl_send_eq_this_globl_send( const VC_Request& G1, const Request& g2 )
{
  TSTAT;
  
  const Request& g1 = G1.req;

  if ( g1.p1      == g2.p1    ||
       g1.rated() != g2.rated() ||
       !g1.t.is_matching( g2.t ) ) return false;

  if ( g1.p1->ignore( g2.p1 ) || g2.p1->ignore( g1.p1 ) ) return false;
  
  g1.p2 = g2.p1;
  bool g1p1_d = g1.p1->var.dformula.eval( g1.p1, g1 ); 
  g1.p2 = 0;

  g2.p2 = g1.p1;
  bool g2p1_d = g2.p1->var.dformula.eval( g2.p1, g2 );
  g2.p2 = 0;
  
  return
    ! g1p1_d &&
    ! g2p1_d &&
    g1.c[0] == g2.c[1] &&
    g1.c[1] == g2.c[0];
}

//: VC_Request.C (eof) (c) Igor
