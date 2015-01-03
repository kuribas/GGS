// $Id: VC_Match.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Match.C (bof) (c) Igor Durdanovic

#include "CRatio.H"
#include "Actors.H"
#include "Client.H"
#include "IO_FILE.H"
#include "Stat.H"
#include "DB_Server.H"
#include "EXE_Service.H"
#include "VAR_Service.H"
#include "VC_Match.H"
#include "VC_SEQ.H"
#include "SET_PTR_Match.H"
#include <fstream>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

SET_id<64> VC_Match::id_cnt;

using namespace std;

VC_Match::VC_Match( const Request& R )
  : mid( id_cnt.get() ), req(R)
{
  TSTAT;

  {
    ostringstream os; os << '.' << mid;
    sid.append( os.str() );
  }

  if ( saved() ) {
    String sdata;
    CRatio ok = vc_save.get( req.savedid(), sdata );
    if ( ok.Txt() != 0 ) {
      data.write( sdata.data(), sdata.size() );
      req.load( data, false );
      match_str.load(data);
      
      mcnt = atoi( req.savedid().c_str() + 1 );
      scnt = req.savedid();
      req.p1->var.stored.erase( scnt );
      req.p2->var.stored.erase( scnt );
      vc_save.del( scnt );
    } else { // we are in big trouble, match has disappeared!!
      vc_con << VCFL << "Match: " << req.counter << " disappeared?!" << endl;
    }
  } else {
    mcnt = vc_var.match_cnt++;
    vc_var.modified(); // counter changed!
    // vc_var.save();  // asynchronous save!
    
    ostringstream os; os << '.' << mcnt;
    scnt.append( os.str() );
  }

  vc_var.matches += this;

  req.n1 = req.p1->id();
  req.n2 = req.p2->id();
  
  req.p1->var.play += sid;
  req.p2->var.play += sid;

  vc_var.watch += Group( sid ); // add game group into watch

  for ( int i = 0; i < 4; ++i ) req.c[i].reset();

  ++vc_stat.n_match;
  vc_var.histo_matches  .update( vc_stat.n_match.  current() );
  vc_var.modified();
}

VC_Match::~VC_Match()
{
  TSTAT;

  --vc_stat.n_match;
  vc_var.histo_matches  .update( vc_stat.n_match.  current() );
  vc_var.modified();

  vc_var.matches -= sid;

  req.p1->var.play -= sid;
  if ( req.p1 != req.p2 ) req.p2->var.play -= sid;

  clean_observers();
  vc_var.watch -= sid; // del game group from watch

  if ( req.p1 ) req.p1->ghost2void(); // get rid of ghosts if not needed any more
  if ( req.p2 ) req.p2->ghost2void();

  id_cnt.put( mid );
}

ostream& VC_Match::te_print( ostream& os ) const
{
  TSTAT;

  assert( sid[0] == '.' );
  os << "VC_Match( " << sid << " )";

  return os;
}

void VC_Match::clean_requests() const
{
  TSTAT;
  
  if ( req.p1->var.open <= req.p1->var.play.size() ) req.p1->clean_requests();
  if ( req.p1 == req.p2 ) return;
  if ( req.p2->var.open <= req.p2->var.play.size() ) req.p2->clean_requests();
}

void VC_Match::clean_observers() const
{
  TSTAT;
  
  Group* G = vc_var.watch( sid );
  if ( G == 0 ) { vc_con << VCFL; return; } // can not happen
  SET_String::const_iterator it = G->obj.begin();
  SET_String::const_iterator hi = G->obj.end();
  for ( ; it != hi; ++it ) {
    Client* C = vc_var.clients( *it );
    if ( C == 0 ) { vc_con << VCFL << "M(" << sid << ") " << *it << endl; continue; }
    C->var.watch -= sid;
  }
}

const String& VC_Match::name( uint4 i ) const
{
  TSTAT;
  
  return i == 0 ? req.n1 : req.n2;
}

char VC_Match::last_color( uint4 i ) const
{
  TSTAT;
  
  const Client* p1 = ( i == 0 ? req.p1 : req.p2 );
  const Client* p2 = ( i == 0 ? req.p2 : req.p1 );
  VEC<History>::const_reverse_iterator it = p1->var.history.rbegin();
  VEC<History>::const_reverse_iterator hi = p1->var.history.rend();
  for ( ; it != hi; ++it ) {
    if ( (*it).Name1() == p1->id() && (*it).Name2() == p2->id() ) return 'B';
    if ( (*it).Name1() == p2->id() && (*it).Name2() == p1->id() ) return 'W';
  }
  it = p2->var.history.rbegin();
  hi = p2->var.history.rend();
  for ( ; it != hi; ++it ) {
    if ( (*it).Name1() == p1->id() && (*it).Name2() == p2->id() ) return 'B';
    if ( (*it).Name1() == p2->id() && (*it).Name2() == p1->id() ) return 'W';
  }
  it = vc_var.history.rbegin();
  hi = vc_var.history.rend();
  for ( ; it != hi; ++it ) {
    if ( (*it).Name1() == p1->id() && (*it).Name2() == p2->id() ) return 'B';
    if ( (*it).Name1() == p2->id() && (*it).Name2() == p1->id() ) return 'W';
  }
  return '?';
}

real8 VC_Match::rating( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? req.p1 : req.p2 ) -> var.rating( req.t.key() ).Rank();
}

real8 VC_Match::ratdev( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? req.p1 : req.p2 ) -> var.rating( req.t.key() ).Rdev();
}

bool VC_Match::trusted( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? req.p1 : req.p2 ) -> var.trust;
}

bool VC_Match::dtor( uint4 i ) const
{
  TSTAT;
  
  return ( i == 0 ? req.p1 : req.p2 ) -> dtor();
}

void VC_Match::cb_start  () const
{
  TSTAT;
  
  vc_exe.match_start  ( *this );
}

void VC_Match::cb_update_to_all () const
{
  TSTAT;
  
  vc_exe.match_update_to_all ( *this );
}

void VC_Match::cb_update_to_name ( const String& Name ) const
{
  TSTAT;
  
  vc_exe.match_update_to_name ( *this, Name );
}

void VC_Match::cb_update_to_all_but_name ( const String& Name ) const
{
  TSTAT;
  
  vc_exe.match_update_to_all_but_name ( *this, Name );
}

void VC_Match::cb_end    () const
{
  TSTAT;
  
  vc_exe.match_end    ( *this );
}

void VC_Match::cb_adjourn( const String& Record, const String& Info )
{
  TSTAT;
  
  ostringstream   os;
  req.refcnt = 2; // goes into two lists, it really doesn't matter here ..
  req.save   ( os );
  Record.save( os );
  vc_save.put( scnt, os.str() );

  req.p1->var.stored.add( Info, scnt, req.p1->id(), req.p2->id() );
  req.p2->var.stored.add( Info, scnt, req.p1->id(), req.p2->id() );
  req.p1->var.modified();
  req.p2->var.modified();
  req.p1->var.save();
  req.p2->var.save();

  Form( vc_log, "%10s %8s vs %8s stored",
	scnt.c_str(),
	req.p1->var.login.c_str(), 
	req.p2->var.login.c_str() ) << endl;
}

void VC_Match::refcnt( const String& key )
{
  TSTAT;
  
  if (! key.empty() ) {
    String sis;
    CRatio ok = vc_save.get( key, sis );
    if ( ok.Txt() != 0 ) { 
      stringstream is(sis);
      Request R;
      String  S;
      R.load( is, false );
      S.load( is );
      vc_log << VCTIME << " R[" << key << "] " << R.n1 << ' ' << R.n2 << " = " << R.refcnt << endl;
      if ( --R.refcnt == 0 ) vc_save.del( key ); // ok no more references
    else {
	ostringstream os;
	R.save( os );
	S.save( os );
	vc_save.put( key, os.str() );
      }
    } else {
      vc_con << VCFL << "stored(" << key << ")" << endl;
    }
  }
}

bool rating_update_bell( const VC_Match& M, const Client* P )
{
  TSTAT;
  
  return P->var.play( M.id() ) ? P->var.bell.play() : P->var.bell.watch();
}

void VC_Match::cb_archive( const String& Record, const String& Info,
			   real8 Result,
			   real8 NormResult )
{
  TSTAT;
  
  ostringstream   os;
  req.refcnt = 3; // going into 3 different lists
  req.save   ( os );
  Record.save( os );
  vc_save.put( scnt, os.str() );
  {
    ofstream ofs( file_archive.c_str(), ios::app );
    if (! ofs ) { vc_con << VCFL << "ofstream(" << file_archive << ")" << endl; }
    else ofs << Record << endl;
  }

  const String& N1 = req.p1->id();
  const String& N2 = req.p2->id();
  
  VC_Rating& R1 = req.p1->var.rating( req.t.key() );
  VC_Rating& R2 = req.p2->var.rating( req.t.key() );
  real8 r1 = R1.Rank();
  real8 d1 = R1.Rdev();
  real8 r2 = R2.Rank();
  real8 d2 = R2.Rdev();
  sint4 t  = System::clock();

  String key, key1, key2;
  key  = vc_var.     history.add( Info, scnt, r1, N1, r2, N2,  Result, t );
  key1 = req.p1->var.history.add( Info, scnt, r1, N1, r2, N2,  Result, t );
  key2 = req.p2->var.history.add( Info, scnt, r1, N1, r2, N2,  Result, t );
  
  VC_Rating::rate( Result, NormResult, R1, R2, t );
  
  vc_var.rank( req.t.key() ).update( req.p1->id(), R1 );
  vc_var.rank( req.t.key() ).update( req.p2->id(), R2 );
  /**/ vc_var.modified();  // async event, we have to save manually
  req.p1->var.modified();
  req.p2->var.modified();
  //   vc_var.save();      // asynchronous save !!
  req.p1->var.save();
  req.p2->var.save();

  refcnt( key ); // update reference counts
  refcnt( key1 );
  refcnt( key2 );

  {
    ostringstream os;
    os << "rating_update " << id() << EOL;
    Form( os << setw(-8) << req.p1->id(), " %7.2f @ %6.2f  %+7.2f -> %7.2f @ %6.2f",
	  r1, d1, R1.Rank() - r1, R1.Rank(), R1.Rdev() ) << EOL;
    Form( os << setw(-8) << req.p2->id(), " %7.2f @ %6.2f  %+7.2f -> %7.2f @ %6.2f",
	  r2, d2, R2.Rank() - r2, R2.Rank(), R2.Rdev() );

    SET_String vec;
    vec += req.p1->id();
    vec += req.p2->id();
    Group* G = vc_var.watch( id() ); if ( G != 0 ) vec.add( G->obj ); // impossible
    VC_SEQ seq( vec ); seq.text( 0, os, *this, rating_update_bell );
  }
}

ostream& VC_Match::print( ostream& os ) const
{
  TSTAT;

  os << setw(4) << id() << ' ';
  Form( os, "%4.0f %-8s ", req.p1->rank( req.t.key() ), req.p1->id().c_str() );
  Form( os, "%4.0f %-8s ", req.p2->rank( req.t.key() ), req.p2->id().c_str() );
  os << setw(8) << req.type() << ' ';
  os << ( req.rated() ? ( !req.td().empty() ? " T " : " R " ) : " U " );
  const Group *G = vc_var.watch( id() );
  if ( G ) os << G->obj.size(); else os << '0';

  return os;
}

//: VC_Match.C (eof) (c) Igor
