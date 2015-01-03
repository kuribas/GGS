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

#include "COMMA.H"
#include "System.H"
#include "Actors.H"
#include "Client.H"
#include "DB_Server.H"
#include "VAR_Client.H"
#include "VT100.H"
#include <sstream>
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

VAR_Client::~VAR_Client()
{
  save();
}

void VAR_Client::make( const String& Login, sint4 Admin  )
{
  TSTAT;
  
  login = Login;
  admin = Admin;
  tell_prefix = "tell ";
  tell_prefix += login;
  tell_prefix += ' ';
}

ostream& VAR_Client::print_who( ostream& os, Client& Who, const String& OT )
{
  TSTAT;
  
  if ( login == Who.var.login && Who.var.vt100 ) os << vt_self;
  os << setw(-8) << login;
  if ( ghost ) {
    os << " x ";
  } else {
    if ( open > play.size() ) os << " + "; else os << " - ";
  }
  real8 r1 = rating( OT ).Rank(), lo_r1 = r1, mi_r1 = r1, hi_r1 = r1;
  real8 d1 = rating( OT ).Rdev(), lo_d1 = d1, mi_d1 = d1, hi_d1 = d1;
  Form( os, "%6.1f@%5.1f", r1, d1 );
  if ( login != Who.id() ) {
    real8 r2 = Who.rank( OT ), lo_r2 = r2, mi_r2 = r2, hi_r2 = r2;
    real8 d2 = Who.rdev( OT ), lo_d2 = d2, mi_d2 = d2, hi_d2 = d2;
    VC_Rating::rate( 0.0, lo_r1, lo_d1, hi_r2, hi_d2 );
    VC_Rating::rate( 0.5, mi_r1, mi_d1, mi_r2, mi_d2 );
    VC_Rating::rate( 1.0, hi_r1, hi_d1, lo_r2, lo_d2 );
    Form( os, " -> %+7.1f %+7.1f %+7.1f @%5.1f", hi_r2-r2, mi_r2-r2, lo_r2-r2, lo_d2 );
  } else {
    if ( play.size() != 0 ) os << "                                   ";
  }
  os << ' ' << play;
  if ( login == Who.id() && Who.var.vt100 ) os << vt_reset;
  return os;
}

ostream& VAR_Client::print_finger( ostream& os ) const
{
  TSTAT;
  
  os << "login  : " << login << EOL;
  dblen.print( os << "dblen  : ", "%5.1f = ", 1, 1 ) << EOL;
  os << "level  : " << admin << EOL
     << "open   : " << open  << EOL
     << "client : " << ( client ? '+' : '-' ) << EOL
     << "trust  : " << ( trust  ? '+' : '-' ) << EOL
     << "rated  : " << ( rated  ? '+' : '-' ) << EOL
     << "bell   : " << bell << EOL
     << "vt100  : " << ( vt100  ? '+' : '-' ) << EOL
     << "hear   : " << ( hear   ? '+' : '-' ) << EOL
     << "play   : " << play  << EOL
     << "stored (" << ( notify_stored  ? '+' : '-' ) << ") : " << stored.size() << EOL
     << "notify (" << ( notify_notify  ? '+' : '-' ) << ") : " << ( star_notify ? "* " : "" ) << notify << EOL
     << "track  (" << ( notify_track   ? '+' : '-' ) << ") : " << ( star_track  ? "* " : "" ) << track << EOL
     << "ignore (" << ( notify_ignore  ? '+' : '-' ) << ") : " << ignore << EOL
     << "request(" << ( notify_request ? '+' : '-' ) << ") : " << send << " : " << recv << EOL
     << "watch  (" << ( notify_watch   ? '+' : '-' ) << ") : " << watch << EOL
     << "accept : " << aformula << EOL
     << "decline: " << dformula << EOL
     << "Type  Rating@StDev      Inactive       AScore    Win   Draw   Loss" << EOL
     << rating;
  return os;
}

void VAR_Client::erase()
{
  TSTAT;
  
  login.erase();
  admin = unreg_level;
  client= false;
  open  = 0;
  trust = false;
  rated = false;
  bell  = FLAG_Bell();
  vt100 = false;
  hear  = true;
  notify_stored  = false;
  notify_track   = false;
  notify_ignore  = false;
  notify_notify  = false;
  notify_watch   = false;
  notify_request = false;
  star_notify    = false;
  star_track     = false;
  track . erase();
  ignore. erase();
  notify. erase();
  send  . erase();
  recv  . erase();
  play  . erase();
  watch . erase();
  rating. erase();
  stored. erase();
  history.erase();
  aformula.erase();
  dformula.erase();
}

void VAR_Client::save( ostream& os ) const
{
  TSTAT;
  
  login.save( os );
  os.write( ccptr(&admin), sizeof(admin) );
  os.write( ccptr(&client),sizeof(client) );
  os.write( ccptr(&open),  sizeof(open) );
  os.write( ccptr(&trust), sizeof(trust) );
  os.write( ccptr(&rated), sizeof(rated) );
  os.write( ccptr(&vt100), sizeof(vt100) );
  os.write( ccptr(&hear),  sizeof(hear) );
  os.write( ccptr(&notify_stored), sizeof(notify_stored) );
  os.write( ccptr(&notify_track),  sizeof(notify_track) );
  os.write( ccptr(&notify_ignore), sizeof(notify_ignore) );
  os.write( ccptr(&notify_notify), sizeof(notify_notify) );
  os.write( ccptr(&notify_watch),  sizeof(notify_watch) );
  os.write( ccptr(&notify_request),sizeof(notify_request) );
  os.write( ccptr(&star_notify),   sizeof(star_notify) );
  os.write( ccptr(&star_track),    sizeof(star_track) );
  bell    .save( os );
  track   .save( os );
  ignore  .save( os );
  notify  .save( os );
  rating  .save( os );
  stored  .save( os );
  history .save( os );
  aformula.save( os );
  dformula.save( os );
}

void VAR_Client::save() const
{
  TSTAT;
  
  if ( admin == unreg_level ) dirty = false;

  if (! dirty ) return;

  ostringstream os; save( os );

  dblen = vc_db.put( login, os.str() );

  dirty = false;
}

bool VAR_Client::load( const String& Login )
{
  TSTAT;
  
  erase();

  String sis;
  CRatio ok = vc_db.get( Login, sis );
  if ( ok.Txt() == 0 ) return false;
  stringstream is(sis); 
  dblen = ok;

  login.load( is );
  is.read( cptr(&admin), sizeof(admin) );
  is.read( cptr(&client),sizeof(client) );
  is.read( cptr(&open),  sizeof(open) );
  is.read( cptr(&trust), sizeof(trust) );
  is.read( cptr(&rated), sizeof(rated) );
  is.read( cptr(&vt100), sizeof(vt100) );
  is.read( cptr(&hear),  sizeof(hear) );
  is.read( cptr(&notify_stored), sizeof(notify_stored) );
  is.read( cptr(&notify_track),  sizeof(notify_track) );
  is.read( cptr(&notify_ignore), sizeof(notify_ignore) );
  is.read( cptr(&notify_notify), sizeof(notify_notify) );
  is.read( cptr(&notify_watch),  sizeof(notify_watch) );
  is.read( cptr(&notify_request),sizeof(notify_request) );
  is.read( cptr(&star_notify),   sizeof(star_notify) );
  is.read( cptr(&star_track),    sizeof(star_track) );
  bell    .load( is );
  track   .load( is );
  ignore  .load( is );
  notify  .load( is );
  rating  .load( is );
  stored  .load( is );
  history .load( is );
  aformula.load( is );
  dformula.load( is );
  
  tell_prefix = "tell ";
  tell_prefix += login;
  tell_prefix += ' ';
  
  dirty = false;

  return true;
}

//: VAR_Client.C (eof) (c) Igor
