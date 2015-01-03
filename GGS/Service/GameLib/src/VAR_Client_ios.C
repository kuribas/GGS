// $Id: VAR_Client_ios.C 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Client_ios.C (bof) (c) Igor Durdanovic

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

void VAR_Client::ios()
{
  FILE* f = fopen( "ios.game", "r" ); if ( f == 0 ) abort();
  char m[512];

  SET_PTR< VAR_Client > Set;
    
  for ( int num = 0, err = 0;; ++num ) {
    if ( ( num % 1000 ) == 0 )
      vc_con << System::dtime( System::clock() ) << setw(8) << num << setw(8) << err << endl;
    if ( feof(f) ) break;
    fgets(m,512,f);
    sint4 t = atoi( m ); if ( t == 0 ) t = 759616000;
    if ( strlen(m) < 80 ) { ++err; continue; }
    if ( m[11] == 'r'   ) { ++err; continue; }
    if ( m[25] != '('   ) { ++err; continue; }
    if ( m[37] != ')'   ) { ++err; continue; }
    if ( m[51] != '('   ) { ++err; continue; }
    if ( m[63] != ')'   ) { ++err; continue; }
    if ( m[65] != '+' && m[65] != 'K' ) { ++err; continue; }
    char  p1[16]; sscanf( m+13, "%s", p1 );
    char  p2[16]; sscanf( m+39, "%s", p2 );
    sint4 D1 = atoi( m+22 );
    sint4 D2 = atoi( m+48 );
    real8 komi;
    bool  rand;
    if ( m[65] == 'K' ) {
      komi = atof( m+66 );
      int b = 0;
      for ( int i = 73; m[i] != 0; i++ ) if ( m[i] == '#' ) ++b;
      rand = ( b > 4 );
    }
    else {
      komi = 0.0;
      rand = false;
    }
    if ( D1 + D2 < 64 ) {
      /**/ if ( D1 > D2 ) D1 = 64 - D2;
      else if ( D1 < D2 ) D2 = 64 - D1; else D1 = D2 = 32;
    }
    if ( D1 + D2 != 64 ) { ++err; continue; }

    real8 result = (D1 - D2) - komi;
    real8 normresult = Match::score( result );
    
    String key = rand ? "8r" : "8"; MatchType::normalize_key( key );

    VAR_Client* vc1;
    VAR_Client* vc2;
    
    vc1 = Set( p1 );
    if ( vc1 == 0 ) {
      vc1 = vc_var.var( p1 );
      if ( vc1 == 0 ) { vc1 = new VAR_Client; vc1->make( p1, 1 ); }
      else vc1 = new VAR_Client( *vc1 );
      Set += vc1;
    }
    vc2 = Set( p2 );
    if ( vc2 == 0 ) {
      vc2 = vc_var.var( p2 );
      if ( vc2 == 0 ) { vc2 = new VAR_Client; vc2->make( p2, 1 ); }
      else vc2 = new VAR_Client( *vc2 );
      Set += vc2;
    }

    VC_Rating& R1 = vc1->rating( key );
    VC_Rating& R2 = vc2->rating( key );
    if ( R1.date == 0 ) R1.date = t;
    if ( R2.date == 0 ) R2.date = t;

#if 0    
    vc_con << setw(-8) << String(p1) << ' ' << R1 << EOM;
    vc_con << setw(-8) << String(p2) << ' ' << R2 << EOM;
#endif
    
    Rating::rate( result, normresult, R1, R2, t );
    vc_var.rank( key ).update( p1, R1 );
    vc_var.rank( key ).update( p2, R2 );

#if 0
    vc_con << result << ' ' << t << EOM;
    vc_con << setw(-8) << String(p1) << ' ' << R1 << EOM;
    vc_con << setw(-8) << String(p2) << ' ' << R2 << EOM;
    vc_con << EOM;
#endif
  }
  
  SET_PTR< VAR_Client >::iterator it = Set.begin();
  SET_PTR< VAR_Client >::iterator hi = Set.end();
  for ( ; it != hi; ++it ) { (*it)->modified(); (*it)->save(); }
  
  vc_var.modified();  // async event, we have to save manually
  vc_var.save();
}

//: VAR_Client_ios.C (eof) (c) Igor
