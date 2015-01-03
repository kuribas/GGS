// $Id: VAR_Client_sgf.C 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Client_sgf.C (bof) (c) Igor Durdanovic

#include "CPP.H"
#include "Actors.H"
#include "DB_Server.H"
#include "Client.H"
#include "VAR_Client.H"
#include "VAR_Service.H"
#include <cstdio>
#include "Game.H"
#include "Match.H"

using namespace std;

void VAR_Client::sgf()
{
  FILE* f = fopen( "sgf.game", "r" ); if ( f == 0 ) abort();
  char m[65536];

  SET_PTR< VAR_Client > Set;
    
  for ( int num = 0, err = 0;; ++num ) {
    if ( ( num % 1000 ) == 0 )
      vc_con << System::dtime( System::clock() )
	     << setw(8) << num << setw(8) << err << endl;
    
    if ( feof(f) ) break;
    fgets(m,65535,f);
    String game(m);
    
    String::size_type it = 0;
    String::size_type hi = 0;

    it = game.find("DT[", 0); if ( it == String::npos ) abort(); it += 3;
    hi = game.find("]",  it); if ( hi == String::npos ) abort();
    String dt( game.data() + it, hi - it );
    sint4 t = atoi( dt.c_str() ); if ( t == 0 ) t = 759616000;

    it = game.find("PB[", 0); if ( it == String::npos ) abort(); it += 3;
    hi = game.find("]",  it); if ( hi == String::npos ) abort();
    String pb( game.data() + it, hi - it );
    
    it = game.find("PW[", 0); if ( it == String::npos ) abort(); it += 3;
    hi = game.find("]",  it); if ( hi == String::npos ) abort();
    String pw( game.data() + it, hi - it );
    
    it = game.find("TY[", 0); if ( it == String::npos ) abort(); it += 3;
    hi = game.find("]",  it); if ( hi == String::npos ) abort();
    String ty( game.data() + it, hi - it );

    it = game.find("RE[", 0); if ( it == String::npos ) abort(); it += 3;
    hi = game.find("]",  it); if ( hi == String::npos ) abort();
    String re( game.data() + it, hi - it );
    real8 r = atof( re.c_str() );
    real8 nr = Match::score( r );

    MatchType gt;
    {
      ostringstream os;
      istringstream is(ty);
      bool ok = gt.parse( os, is ); if (! ok ) abort();
    }
    VAR_Client* vcb;
    VAR_Client* vcw;
    
    vcb = Set( pb );
    if ( vcb == 0 ) {
      vcb = vc_var.var( pb );
      if ( vcb == 0 ) { vcb = new VAR_Client; vcb->make( pb, 1 ); }
      else vcb = new VAR_Client( *vcb );
      Set += vcb;
    }
    vcw = Set( pw );
    if ( vcw == 0 ) {
      vcw = vc_var.var( pw );
      if ( vcw == 0 ) { vcw = new VAR_Client; vcw->make( pw, 1 ); }
      else vcw = new VAR_Client( *vcw );
      Set += vcw;
    }

    VC_Rating& Rb = vcb->rating( gt.key() );
    VC_Rating& Rw = vcw->rating( gt.key() );
    if ( Rb.date == 0 ) Rb.date = t;
    if ( Rw.date == 0 ) Rw.date = t;

#if 0
    vc_con << pb << ' ' << Rb << ' ' << pw << ' ' << Rw << endl;
#endif

    Rating::rate( r, nr, Rb, Rw, t );
    vc_var.rank( gt.key() ).update( pb, Rb );
    vc_var.rank( gt.key() ).update( pw, Rw );
#if 0
    vc_con << pb << ' ' << Rb << ' ' << pw << ' ' << Rw << endl;
#endif
  }
  
  SET_PTR< VAR_Client >::iterator it = Set.begin();
  SET_PTR< VAR_Client >::iterator hi = Set.end();
  for ( ; it != hi; ++it ) { (*it)->modified(); (*it)->save(); }
  
  vc_var.modified();  // async event, we have to save manually
  vc_var.save();
}

//: VAR_Client_sgf.C (eof) (c) Igor
