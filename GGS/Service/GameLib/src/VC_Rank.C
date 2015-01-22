// $Id: VC_Rank.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Rank.C (bof) (c) Igor Durdanovic

#include "Client.H"
#include "IO_FILE.H"
#include "VAR_Client.H"
#include "VAR_Service.H"
#include "VC_Rank.H"
#include "VT100.H"
#include <algorithm>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const real8 mul_tdev = 3.5; // was : 1720.0 / 350.0 ~ 4.9

using namespace std;

bool lt_rank( const Rank& R1, const Rank& R2 )
{
  TSTAT;
  
  real8 r1 = R1.rank - R1.tdev * mul_tdev;
  real8 r2 = R2.rank - R2.tdev * mul_tdev;
  if ( r1 == r2 ) return R1.name < R2.name;
  return r1 > r2;
}

bool eq_rank( const Rank& R1, const Rank& R2 )
{
  TSTAT;
  
  real8 r1 = R1.rank - R1.tdev * mul_tdev;
  real8 r2 = R2.rank - R2.tdev * mul_tdev;
  return r1 == r2 && R1.name == R2.name;
}

bool lt_name( const Rank& R1, const Rank& R2 )
{
  TSTAT;
  
  return R1.name < R2.name;
}

bool eq_name( const Rank& R1, const Rank& R2 )
{
  TSTAT;
  
  return R1.name == R2.name;
}

//

ostream& Rank::print( ostream& os ) const
{
  TSTAT;
  
  os << setw(-8) << name << ' ' << reinterpret_cast<const Rating&>(*this);

  return os;
}

ostream& Rank::save ( ostream& os ) const
{
  TSTAT;
  
  name.save( os );
  Rating::save( os );
  
  return os;
}

istream& Rank::load ( istream& is )
{
  TSTAT;
  
  name.load( is );
  Rating::load( is );

  tdev = Rating::Rdev();

  return is;
}

//

ccptr VC_Rank::tag = "VC_Rank";

VC_Rank::VC_Rank( const String& ID )
  : tid(ID), avg(0), dev(0)
{
  te_create( 0, 10 ); // schedule first re-sorting
}

VC_Rank::VC_Rank( const VC_Rank& R )
  : TIME_Client(),
    tid( R.tid ),
    avg( R.avg ),
    dev( R.dev ),
    by_name( R.by_name ),
    by_rank( R.by_rank )
{
  te_create( 0, 10 ); // schedule first re-sorting
}

VC_Rank& VC_Rank::operator=( const VC_Rank& R )
{
  te_cancel();
  
  tid = R.tid;
  avg = R.avg;
  dev = R.dev;
  by_name = R.by_name;
  by_rank = R.by_rank;

  te_create( 0, 10 ); // schedule first re-sorting

  return *this;
}

VC_Rank::~VC_Rank()
{
}

void VC_Rank::sort_check() const
{
  TSTAT;
  
  vc_log << VCTIME << " sort check" << endl;
  for ( uint4 i = 1; i < by_name.size(); ++i ) {
    bool ok1 = by_name[i-1].name < by_name[i].name;
    bool ok2 = lt_name( by_name[i-1], by_name[i] );
    if (! ok1 || ok1 != ok2 ) {
      vc_con << "sort by name failed at " << i-1 << ' ' << i << endl;
      vc_con << by_name[i-1].name << ' ' << by_name[i-1].rank << '@' << by_name[i-1].tdev << " < "
	     << by_name[i  ].name << ' ' << by_name[i  ].rank << '@' << by_name[i  ].tdev << endl;
    }
  }
  for ( uint4 i = 1; i < by_rank.size(); ++i ) {
    bool ok1 =
      by_rank[i-1].rank - by_rank[i-1].tdev * mul_tdev >
      by_rank[i  ].rank - by_rank[i  ].tdev * mul_tdev; 
    bool ok2 = lt_rank( by_rank[i-1], by_rank[i] );
    if (! ok1 || ok1 != ok2 ) {
      vc_con << "sort by rank failed at " << i-1 << ' ' << i << endl;
      vc_con << by_rank[i-1].name << ' ' << by_rank[i-1].rank << '@' << by_rank[i-1].tdev << " < "
	     << by_rank[i  ].name << ' ' << by_rank[i  ].rank << '@' << by_rank[i  ].tdev << endl;
    }
  }
}

void VC_Rank::te_handle( sint4 /*mssg*/, uint4 /*time*/)
{
  TSTAT;
  
  VEC<Rank>::iterator it = by_rank.begin();
  VEC<Rank>::iterator hi = by_rank.end();
  for ( ; it != hi; ++it ) (*it).tdev = (*it).Rdev();
  by_name = by_rank;
  
  sort( by_rank.begin(), by_rank.end(), lt_rank );
  sort( by_name.begin(), by_name.end(), lt_name );

  vc_var.modified();
  
  te_create( 0, time_rank );
}

ostream& VC_Rank::te_print ( ostream& os ) const
{
  TSTAT;
  
  os << "VC_Rank( " << tid << " )";

  return os;
}

void VC_Rank::top( Client& P, const String& /*ot*/, sint4 No, sint4 Lo ) const
{
  TSTAT;
  
  Lo--;
  if ( Lo < 0 ) Lo = 0;
  if ( Lo      > sint4(by_rank.size()) ) Lo = by_rank.size();
  if ( Lo + No > sint4(by_rank.size()) ) No = by_rank.size() - Lo;

  int w = 10 - strlen(RegularBoardGame::LOGIN_SERVICE);
  P.tell().vt100( vt_start, "top" ) << setw(w) << String("");
  Form( P, "%6.1f@%5.1f      Inactive       AScore    Win   Draw   Loss %d",
	avg, dev, by_rank.size() );

  print( P, P.vt100(), P.id(), Lo, No ) << EOM;
}

void VC_Rank::rank( Client& P, const String& /*ot*/, const String& name ) const
{
  TSTAT;
  
  int w = 9 - sizeof(RegularBoardGame::LOGIN_SERVICE);
  P.tell().vt100( vt_start, "rank" ) << setw(w) << String("");
  Form( P, "%6.1f@%5.1f      Inactive       AScore    Win   Draw   Loss %d",
	avg, dev, by_rank.size() );

  Rank R( name );
  VEC<Rank>::const_iterator it;
  it = lower_bound( by_name.begin(), by_name.end(), R, lt_name );
  if ( it == by_name.end() || (*it).name != R.name ) { P << EOM; return; }
  
  R = *it;
  it = lower_bound( by_rank.begin(), by_rank.end(), R, lt_rank );
  if ( it == by_rank.end() || (*it).name != R.name ) { P << EOM; return; }
  
  print( P, P.vt100(), name, it - by_rank.begin() - 8, 16 ) << EOM;
}

void VC_Rank::update( const String& name, const Rating& r )
{
  TSTAT;
  
  Rank R( name );
  VEC<Rank>::iterator itn;
  VEC<Rank>::iterator itr;

  itn = lower_bound( by_name.begin(), by_name.end(), R, lt_name );
  if ( itn != by_name.end() && (*itn).name == name ) {
    R = *itn;
    itr = lower_bound( by_rank.begin(), by_rank.end(), R, lt_rank );
    if ( itr == by_rank.end() || (*itr).name != name ) {
      vc_con << VCFL
	     << "Inconsistent sets for '" << name << "'" << EOM;
      return;
    }
  
    avg *= by_rank.size();
    avg -= (*itn).rank;
    by_name.erase( itn );
    by_rank.erase( itr );
    if ( by_rank.size() > 0 ) avg /= by_rank.size();
  }

  reinterpret_cast<Rating&>( R ) = r; R.tdev = R.Rdev();
  
  itn = lower_bound( by_name.begin(), by_name.end(), R, lt_name );
  itr = lower_bound( by_rank.begin(), by_rank.end(), R, lt_rank );
  avg *= by_rank.size();
  by_name.insert( itn, R );
  by_rank.insert( itr, R );
  avg += r.Rank();
  avg /= by_rank.size();
  
  {
    VEC<Rank>::const_iterator it = by_rank.begin();
    VEC<Rank>::const_iterator hi = by_rank.end();
    dev = 0; for ( ; it != hi; ++it ) { real8 d = (*it).rank - avg; dev += d*d; }
    if ( by_rank.size() > 0 ) dev /= by_rank.size(); dev = sqrt(dev);
  }
}
  
void VC_Rank::erase()
{
  TSTAT;
  
  by_name.erase();
  by_rank.erase();
}

ostream& VC_Rank::print( ostream& os, bool vt100, const String& Name, sint4 Lo, sint4 No ) const
{
  TSTAT;
  
  if ( Lo < 0 ) Lo = 0;
  if ( No > 64) No = 64;
  if ( Lo      > sint4(by_rank.size()) ) Lo = by_rank.size();
  if ( Lo + No > sint4(by_rank.size()) ) No = by_rank.size() - Lo;

  VEC<Rank>::const_iterator it = by_rank.begin() + Lo;
  VEC<Rank>::const_iterator hi = it + No;
  for ( ; it != hi; ++it) {
    os << EOL;
    bool eq = (*it).name == Name;
    if ( eq && vt100 ) os << vt_self;
    os << setw(5) << it - by_rank.begin() + 1 << ' ' << *it << ( eq ? " <=" : "" );
    if ( eq && vt100 ) os << vt_reset;
  }

  return os;
}

ostream& VC_Rank::save ( ostream& os ) const
{
  TSTAT;
  
  tag_save( tag, os );

  tid.save( os );
  by_rank.save( os );
  
  return os;
}
  
istream& VC_Rank::load ( istream& is )
{
  TSTAT;
  
  if (! tag_chck( tag, is ) ) { vc_con << VCFL; System::exit(-1); }

  erase();
  
  tid.load( is );
  by_rank.load( is );
  by_name = by_rank;
  
  sort( by_name.begin(), by_name.end(), lt_name );
  sort( by_rank.begin(), by_rank.end(), lt_rank );

  {
    VEC<Rank>::const_iterator it = by_rank.begin();
    VEC<Rank>::const_iterator hi = by_rank.end();
    avg = 0; for ( ; it != hi; ++it ) avg += (*it).rank;
    if ( by_rank.size() > 0 ) avg /= by_rank.size();
  }
  {
    VEC<Rank>::const_iterator it = by_rank.begin();
    VEC<Rank>::const_iterator hi = by_rank.end();
    dev = 0; for ( ; it != hi; ++it ) { real8 d = (*it).rank - avg; dev += d*d; }
    if ( by_rank.size() > 0 ) dev /= by_rank.size(); dev = sqrt(dev);
  }
  
  return is;
}

//: VC_Rank.C (eof) (c) Igor
