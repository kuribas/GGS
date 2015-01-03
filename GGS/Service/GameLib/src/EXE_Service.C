// $Id: EXE_Service.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service.C (bof) (c) Igor Durdanovic

#include "CRatio.H"
#include "System.H"
#include "Actors.H"
#include "Signal.H"
#include "Stat.H"
#include "DB_Server.H"
#include "TIME_Server.H"
#include "Histogram_HDMY.H"
#include "IO_FILE.H"
#include "IO_TCP_Service.H"
#include "Client.H"
#include "EXE_Service.H"
#include "VAR_Service.H"
#include "SET_PTR_Request.H"
#include "SET_PTR_Match.H"
#include "VC_SEQ.H"
#include "Message.H"
#include "VT100.H"
#include <iomanip>
#include "Match.H"
#include "TSTAT_Server.H"
#include "TSTAT_Client.H"
#include <sstream>

using namespace std;

EXE_Service::EXE_Service()
{
  add( "+",           &EXE_Service::add );
  add( "-",           &EXE_Service::del );

  add( "top",         &EXE_Service::top );
  add( "rank",        &EXE_Service::rank );

  add( "who",         &EXE_Service::who );
  add( "finger",      &EXE_Service::finger );
  add( "assess",      &EXE_Service::assess );

  add( "group",       &EXE_Service::group );
  add( "tdtype",      &EXE_Service::tdtype );
  add( "tdstart",     &EXE_Service::tdstart );
  add( "tdbreak",     &EXE_Service::tdadjourn );
  add( "tdabort",     &EXE_Service::tdabort );

  add( "ask",         &EXE_Service::ask );
  add( "continue",    &EXE_Service::cont );
  add( "request",     &EXE_Service::request );
  add( "accept",      &EXE_Service::accept );
  add( "decline",     &EXE_Service::decline );
  add( "cancel",      &EXE_Service::cancel );

  add( "aform",       &EXE_Service::aform );
  add( "dform",       &EXE_Service::dform );

  add( "play",        &EXE_Service::play );
  add( "undo",        &EXE_Service::undo );
  add( "abort",       &EXE_Service::abort );
  add( "break",       &EXE_Service::adjourn );
  add( "score",       &EXE_Service::score );
  add( "resign",      &EXE_Service::resign );
  add( "moves",       &EXE_Service::moves );

  add( "tell",        &EXE_Service::tell );
  
  add( "look",        &EXE_Service::look );
  add( "history",     &EXE_Service::history );
  add( "stored",      &EXE_Service::stored );

  add( "help",        &EXE_Service::help );

  add( "open",        &EXE_Service::open );
  add( "client",      &EXE_Service::client );

  add( "bell",        &EXE_Service::bell );
  add( "vt100",       &EXE_Service::vt100 );
  add( "trust",       &EXE_Service::trust );
  add( "hear",        &EXE_Service::hear );
  add( "rated",       &EXE_Service::rated );

  add( "watch",       &EXE_Service::watch );
  add( "match",       &EXE_Service::match );
  add( "ignore",      &EXE_Service::ignore );
  add( "notify",      &EXE_Service::notify );
  add( "track",       &EXE_Service::track );

  add( "stat_user",   &EXE_Service::histo_usr );
  add( "stat_request",&EXE_Service::histo_req );
  add( "stat_match",  &EXE_Service::histo_mat );
  add( "stat_inp",    &EXE_Service::histo_inp );
  add( "stat_out",    &EXE_Service::histo_out );

  add( "uptime",      &EXE_Service::uptime );
  add( "salias",      &EXE_Service::salias );
  add( "shistory",    &EXE_Service::shistory );
  add( "pack",        &EXE_Service::pack );
  add( "del_stored",  &EXE_Service::del_stored );
  add( "tstat",       &EXE_Service::tstat );
  add( "tevent",      &EXE_Service::tevent );

  vc_con << VCTIME << "  EXE_Service( " << cmds.size() << " )" << endl;

  if ( this == &vc_exe ) vc_exe_ready = true;
}

EXE_Service::~EXE_Service()
{
  if ( this == &vc_exe && ! vc_exe_ready ) return;

  vc_con << VCTIME << " ~EXE_Service( " << cmds.size() << " )" << endl; 
}

void EXE_Service::add( ccptr Name, Command Proc )
{
  cmds += EXE_Cmd( Name, Proc );
}

void EXE_Service::shell_execute( Client* P, String& line )
{
  TSTAT;
  
  String cmd;
  String arg;
  String::parse( line, cmd, arg );
  
  const EXE_Cmd* p = cmds( cmd );

  if ( p ) (this->*(p->obj))( P, arg );
  else { if ( P ) vc_mssg._010( P->vt100(), P->tell(), cmd ) << EOM; }
}

void EXE_Service::shell_expand ( Client* P, String& line, sint4& no_cmd, sint4 no_rec )
{
  TSTAT;
  
  String cmd;
  String arg;
  String::parse( line, cmd, arg );
  bool ok = vc_var.alias.replace_alias( cmd, 0 );
  if (! ok ) {
    line = cmd + ' ' + arg;
    shell_execute( P, line ); --no_cmd;
    return;
  }

  VEC<String> stack; String::parse( cmd, stack, ';' );
  
  for ( uint4 i = 0; i < stack.size() && no_cmd > 0; ++i ) {
    vc_var.alias.replace_arg( stack[i], arg );
    if ( no_rec > 0 ) {
      shell_expand ( P, stack[i], no_cmd, no_rec - 1 );
    } else {
      shell_execute( P, stack[i] ); --no_cmd;
    }
  }
}

void EXE_Service::operator () ( Client* P, String& text )
{
  TSTAT;
  
  if ( text.empty() ) return;
  sint4 no_cmd = 8; shell_expand( P, text, no_cmd, 3 );
  
  if ( P ) P->var.save(); // sync player's  vars
  // vc_var.save();       // sync service's vars // asynchronous save
}


void EXE_Service::notify_stored( Client* P )
{
  TSTAT;
  
  VEC<Stored>::const_iterator it = P->var.stored.begin();
  VEC<Stored>::const_iterator hi = P->var.stored.end();
  SET_String Me;
  SET_String Op;
  for ( ; it != hi; ++it ) {
    Client* C = 0; // no ghosts
    /**/ if ( (*it).Name1() == P->id() ) C = vc_var.client( (*it).Name2(), false, false );
    else if ( (*it).Name2() == P->id() ) C = vc_var.client( (*it).Name1(), false, false );
    if ( C != 0 ) {
      if ( P->var.notify_stored ) Me += C->id();
      if ( C->var.notify_stored ) Op += P->id();
    }
  }
  if (! Me.empty() ) {
    P->tell();
    SET_String::const_iterator it = Me.begin();
    SET_String::const_iterator hi = Me.end();
    for ( bool first = true; it != hi; ++it ) {
      if (! first ) *P << EOL;
      *P << "+ " << *it << " stored";
    }
    *P << EOM;
  }
  if (! Op.empty() ) {
    SET_String::const_iterator it = Op.begin();
    SET_String::const_iterator hi = Op.end();
    for ( ; it != hi; ++it ) {
      Client* C = vc_var.client( *it, false, false );
      if ( C == 0 ) continue;
      C->tell().bell( C->var.bell.notify_stored() ) << "+ " << P->id() << " stored" << EOM;
    }
  }
}

// + <user> <level|_group|.chann>

void EXE_Service::add( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P != 0 ) { vc_mssg._011( P->vt100(), P->tell(), " + " ) << EOM; return; }
  String login;
  String group;
  String::parse( arg, login, group );
  if ( group.size() != 1 && '0' > group[0] && group[0] > '3' ) return; // not for us
  
  Client* pp = vc_var.client( login, false, true ); // return ghost if we have one
  if ( pp == 0 ) {
    Client* np = new Client;
    bool ok = np->var.load( login );
    if (! ok ) np->var.make( login, unreg_level );
    np->var.ghost = false;
    np->var.admin = atoi( group.c_str() );
    np->var.modified();
    np->var.save();
    vc_var.add( np );
    notify_stored( np );
    return;
  }

  pp->var.admin = atoi( group.c_str() );
  pp->var.modified();
  pp->var.save();
  
  if ( pp->var.ghost ) pp->ghost2alive();
}

// - <user> [_group|.chann]

void EXE_Service::del( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P != 0 ) { vc_mssg._011( P->vt100(), P->tell(), "-" ) << EOM; return; }
  String login;
  String group;
  String::parse( arg, login, group );
  if ( group.size() != 1 && '0' > group[0] && group[0] > '3' ) return; // not for us

  Client* O = vc_var.clients ( login );
  if ( O == 0 ) return;

  if ( O->in_tournament() ) {
    O->alive2ghost();
  } else {
    vc_var.del( O );
    delete O;
  }
}


// top <type> [[From] Num]

void EXE_Service::top( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> vec; String::parse( arg, vec );

  if ( vec.size() < 1 || vec.size() > 3 ) {
    vc_mssg._035( P->vt100(), P->tell() ) << EOM; return;
  }
  
  String RatingType = vec[0];
  if (! MatchType::normalize_key( RatingType ) ) {
    vc_mssg._045( P->vt100(), P->tell(), RatingType ) << EOM; return;
  }

  if ( vec.size() == 1 ) { vc_var.rank( RatingType ).top( *P, RatingType ); return; }
  if ( vec.size() == 2 ) {
    sint4 num; if (! sint4_parse( vec[1], false, num ) ) {
      vc_mssg._035( P->vt100(), P->tell() ) << EOM; return;
    }
    vc_var.rank( RatingType ).top( *P, RatingType, sint4(num) ); return;
  }
  if ( vec.size() == 3 ) {
    sint4 from;
    if (! sint4_parse( vec[1], false, from ) ) {
      vc_mssg._035( P->vt100(), P->tell() ) << EOM; return;
    }
    sint4 num;
    if (! sint4_parse( vec[2], false, num ) ) {
      vc_mssg._035( P->vt100(), P->tell() ) << EOM; return;
    }
    vc_var.rank( RatingType ).top( *P, RatingType, sint4(num), sint4(from) ); return;
  }
}

// rank <type> [login]

void EXE_Service::rank( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> vec; String::parse( arg, vec );

  if ( vec.size() != 1 && vec.size() != 2 ) {
    vc_mssg._036( P->vt100(), P->tell() ) << EOM; return;
  }

  String RatingType = vec[0];
  if (! MatchType::normalize_key( RatingType ) ) {
    vc_mssg._045( P->vt100(), P->tell(), RatingType ) << EOM; return;
  }

  if ( vec.size() == 1 ) {
    vc_var.rank( RatingType ).rank( *P, RatingType, P->id() );
    return;
  }
  if ( vec.size() == 2 ) {
    VAR_Client* VC = vc_var.var( vec[1] );
    if ( VC == 0 ) { vc_mssg._016( P->vt100(), P->tell(), vec[1] ) << EOM; return; }
    vc_var.rank( RatingType ).rank( *P, RatingType, vec[1] );
    return;
  }
}


// who <type> [a] [-pts +pts]

class WHO_cmp
{
private:
  const String& ot;
  
public:
  WHO_cmp( const String& OT ) : ot(OT) {}
  bool operator() ( SET_PTR_Client::iterator p1,
		    SET_PTR_Client::iterator p2 )
  {
    const real8 mul_tdev = 1720.0 / 350.0;

    real8 r1 = (*p1)->rank( ot ) - (*p1)->rdev( ot ) * mul_tdev;
    real8 r2 = (*p2)->rank( ot ) - (*p2)->rdev( ot ) * mul_tdev;
    return r1 < r2;
  }
};

void EXE_Service::who( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> vec; String::parse( arg, vec );

  if ( vec.size() == 0 || vec.size() > 4 ) {
    vc_mssg._014( P->vt100(), P->tell() ) << EOM; return;
  }
  
  String RatingType = vec[0];
  if (! MatchType::normalize_key( RatingType ) ) {
    vc_mssg._045( P->vt100(), P->tell(), RatingType ) << EOM; return;
  }
  
  bool Open = true;
  if ( vec.size() == 2 || vec.size() == 4 ) {
    if ( vec[1] == "a" ) Open = false;
    else { vc_mssg._014( P->vt100(), P->tell() ) << EOM; return; }
  }
  bool Range = vec.size() > 2;
  real8 lo_pts = -9999;
  real8 hi_pts = +9999;
  if ( Range ) {
    lo_pts = atof( vec[vec.size()-2].c_str() );
    hi_pts = atof( vec[vec.size()-1].c_str() );
  }
  
  vector< SET_PTR_Client::iterator > v;
  {
    SET_PTR_Client::iterator it = vc_var.clients.begin();
    SET_PTR_Client::iterator hi = vc_var.clients.end();
    for ( ; it != hi; ++it ) {
      if ( Open && ( (*it)->var.open <= (*it)->var.play.size() || (*it)->ghost() ) ) continue;
      if ( Range ) {
	real8 r1 =     P->rank( RatingType ), lo_r1 = r1, hi_r1 = r1;
	real8 d1 =     P->rdev( RatingType ), lo_d1 = d1, hi_d1 = d1;
	real8 r2 = (*it)->rank( RatingType ), lo_r2 = r2, hi_r2 = r2;
	real8 d2 = (*it)->rdev( RatingType ), lo_d2 = d2, hi_d2 = d2;
	VC_Rating::rate( 0.0, lo_r1, lo_d1, hi_r2, hi_d2 ); lo_r1 -= r1;
	VC_Rating::rate( 1.0, hi_r1, hi_d1, lo_r2, lo_d2 ); hi_r1 -= r1;
	if ( lo_r1 < lo_pts ) continue;
	if ( hi_r1 > hi_pts ) continue;
      }
      v.push_back( it );
    }
  }
  sort( v.begin(), v.end(), WHO_cmp( RatingType ) );
  
  vector< SET_PTR_Client::iterator >::const_iterator it = v.begin();
  vector< SET_PTR_Client::iterator >::const_iterator hi = v.end();
  sint4 w = 9 - strlen(RegularBoardGame::LOGIN_SERVICE); if ( w < 0 ) w = 0;
  P->tell().vt100( vt_start, "who" )
    << ' ' << setw(5) << vc_var.clients.size()
    << setw(w) << String("")
    << "change:    win    draw    loss         match(es)";
  for ( ; it != hi; ++it ) { (**it)->var.print_who( *P << EOL, *P, RatingType ); }
  *P << EOM;
  return;
}


// finger [ <login> .. <login> ]

void EXE_Service::finger( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "finger" ) << ' ' << P->id() << EOL;
    P->var.print_finger( *P ) << EOM;
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  bool users = false;
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    VAR_Client* vc = vc_var.var( *it );
    if ( vc == 0 ) { Err += *it; continue; }
    if ( users ) *P << EOL; users = true;
    P->tell().vt100( vt_start, "finger" ) << ' ' << vc->login << EOL;
    vc->print_finger( *P ) << EOM;
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "finger" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}

static void cut_match_id( String& s )
{
  String::size_type pos = s.find( '.', 1 );
  if ( pos != String::npos ) s.erase( pos, s.size() - pos );
}

// assess [.match] <score>
// assess <type> <login> <score>
// assess <type> <login> <login> <score>
// assess * <login> <score>
// assess * <login> <login> <score>

void EXE_Service::assess( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> vec; String::parse( arg, vec );

  if ( vec.size() == 1 && vec[0][0] != '.' && P->var.play.size() == 1 )
    vec.insert( vec.begin(), P->var.play[0] );
  
  if ( vec.size() < 2 || vec.size() > 4 ) {
    vc_mssg._038( P->vt100(), P->tell() ) << EOM; return;
  }

  String p1;
  String p2;
  real8  r1;
  real8  d1;
  real8  r2;
  real8  d2;
  real8  result;
  real8  score;
  String RatingType;
  
  switch ( vec.size() ) {
  case 2 : { // <.match> <score>
    String sid(vec[0]); cut_match_id( sid );
    
    VC_Match* M = vc_var.matches( sid );
    if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), arg ) << EOM; return; }
    RatingType = M->type().key();
    p1 = M->name(0);
    r1 = M->rating(0);
    d1 = M->ratdev(0);
    p2 = M->name(1);
    r2 = M->rating(1);
    d2 = M->ratdev(1);
    result = atof( vec[1].c_str() );
  } break;
  case 3 : {
    if ( vec[0] == str_star ) { star_assess( P, vec[1], vec[2] ); return; } // * <login> <score>
    RatingType = vec[0];
    if (! MatchType::normalize_key( RatingType ) ) {
      vc_mssg._045( P->vt100(), P->tell(), RatingType ) << EOM; return;
    }
    p1 = P->id();
    r1 = P->rank( RatingType );
    d1 = P->rdev( RatingType );
    VAR_Client* V = vc_var.var( vec[1] );
    if ( V == 0 ) { vc_mssg._016( P->vt100(), P->tell(), vec[1] ) << EOM; return; }
    p2 = V->login;
    r2 = V->rating( RatingType ).Rank();    
    d2 = V->rating( RatingType ).Rdev();    
    result = atof( vec[2].c_str() );
  } break;
  case 4 : {
    if ( vec[0] == str_star ) { star_assess( P, vec[1], vec[2], vec[3] ); return; } // * <login> <login> <score>
    RatingType = vec[0];
    if (! MatchType::normalize_key( RatingType ) ) {
      vc_mssg._045( P->vt100(), P->tell(), RatingType ) << EOM; return;
    }
    VAR_Client* V = vc_var.var( vec[1] );
    if ( V == 0 ) { vc_mssg._016( P->vt100(), P->tell(), vec[1] ) << EOM; return; }
    p1 = V->login;
    r1 = V->rating( RatingType ).Rank();    
    d1 = V->rating( RatingType ).Rdev();    
    V = vc_var.var( vec[2] );
    if ( V == 0 ) { vc_mssg._016( P->vt100(), P->tell(), vec[2] ) << EOM; return; }
    p2 = V->login;
    r2 = V->rating( RatingType ).Rank();    
    d2 = V->rating( RatingType ).Rdev();    
    result = atof( vec[3].c_str() );
  } break;
  default : vc_con << VCFL; return;
  }
  score = Match::score( result );
  real8 new_r1 = r1;
  real8 new_d1 = d1;
  real8 new_r2 = r2;
  real8 new_d2 = d2;
  VC_Rating::rate( score, new_r1, new_d1, new_r2, new_d2 );
  P->tell().vt100( vt_start, "assess" ) << ' ' << RatingType << ' ';
  Form( *P, "%+6.2f -> %6.4f", result, score ) << EOL;
  Form( *P << setw(-8) << p1, " %7.2f@%6.2f %+8.2f -> %7.2f@%6.2f",
	r1, d1, new_r1 - r1, new_r1, new_d1 ) << EOL;
  Form( *P << setw(-8) << p2, " %7.2f@%6.2f %+8.2f -> %7.2f@%6.2f",
	r2, d2, new_r2 - r2, new_r2, new_d2 ) << EOM;
}

void EXE_Service::star_assess( Client* P, const String& Login2, const String& Score )
{
  VAR_Client* V2 = vc_var.var( Login2 );
  if ( V2 == 0 ) { vc_mssg._016( P->vt100(), P->tell(), Login2 ) << EOM; return; }

  SET_String Types; // collect all existing rating types
  {
    SET_Rating::iterator it = V2->rating.begin();
    SET_Rating::iterator hi = V2->rating.end();
    for ( ; it != hi; ++it ) Types += (*it).id();
  }
  {
    SET_Rating::iterator it = P->var.rating.begin();
    SET_Rating::iterator hi = P->var.rating.end();
    for ( ; it != hi; ++it ) Types += (*it).id();
  }
  // call assess for all of them
  {
    SET_String::iterator it = Types.begin();
    SET_String::iterator hi = Types.end();
    for ( ; it != hi; ++it ) {
      String arg = *it + ' ' + Login2 + ' ' + Score;
      assess( P, arg );
    }
  }
}

void EXE_Service::star_assess( Client* P, const String& Login1, const String& Login2, const String& Score )
{
  VAR_Client* V1 = vc_var.var( Login1 );
  if ( V1 == 0 ) { vc_mssg._016( P->vt100(), P->tell(), Login1 ) << EOM; return; }

  VAR_Client* V2 = vc_var.var( Login2 );
  if ( V2 == 0 ) { vc_mssg._016( P->vt100(), P->tell(), Login2 ) << EOM; return; }

  SET_String Types; // collect all existing rating types
  {
    SET_Rating::iterator it = V2->rating.begin();
    SET_Rating::iterator hi = V2->rating.end();
    for ( ; it != hi; ++it ) Types += (*it).id();
  }
  {
    SET_Rating::iterator it = V1->rating.begin();
    SET_Rating::iterator hi = V1->rating.end();
    for ( ; it != hi; ++it ) Types += (*it).id();
  }
  // call assess for all of them
  {
    SET_String::iterator it = Types.begin();
    SET_String::iterator hi = Types.end();
    for ( ; it != hi; ++it ) {
      String arg = *it + ' ' + Login1 + ' ' + Login2 + ' ' + Score;
      assess( P, arg );
    }
  }
}

// tdtype <td.id> <type>

void EXE_Service::tdtype( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  Group* gp = vc_var.groups( group_td );

  if ( gp == 0 || gp->obj( P->id() ) == 0 ) { 
    vc_mssg._062( P->vt100(), P->tell() ) << EOM;
    return;
 }

  if ( arg.empty()) {
    vc_mssg._072( P->vt100(), P->tell() ) << EOM;
  }

  String tid;
  String type;
  String::parse( arg, tid, type );
  MatchType t;
  ostringstream os; // <type>
  istringstream iarg( type );
  if (!t.parse( os, iarg ) ) {
    vc_mssg._069( P->vt100(), P->tell(), tid );
    P->text( os ) << EOM;
    return;
  }

  P->tell().vt100( vt_start, "tdtype " ) << tid << ' ' << t.td_data() << EOM;

#if 0
  P->tell().vt100( vt_start, "tdtype " ) << tid << ' ' << t.to_string() << ' ';
  if ( t.is_synchro_game() || t.is_komi_game() ) {
    *P << "colors can not be chosen.";
  } else if ( t.get_pref_color() == Color::UNDEF ) {
    *P << "colors can be chosen.";
  } else {
    *P << "colors are preset.";
  }
  *P << EOM;
#endif  
}

// tdstart <td.id> <type> <login1> <time1> <login2> <time2> 
// tdstart <td.id> <stored.id> 

void EXE_Service::tdstart( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  Group* gp = vc_var.groups( group_td );

  if ( gp == 0 || gp->obj( P->id() ) == 0 ) { 
    vc_mssg._062( P->vt100(), P->tell() ) << EOM;
    return;
 }

  VC_Match* M = VC_Request::tdparse( P, arg );
  if ( M != 0 ) {
    P->tell().vt100( vt_start, "tdstart " ) << M->tid() << ' ' << M->id() << EOM;
  }
}

// tdadjourn <match.id>

void EXE_Service::tdadjourn( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  Group* gp = vc_var.groups( group_td );

  if ( gp == 0 || gp->obj( P->id() ) == 0 ) { 
    vc_mssg._062( P->vt100(), P->tell() ) << EOM;
    return;
  }

  String mid( arg ); cut_match_id( mid );

  VC_Match* M = vc_var.matches( mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), mid ) << EOM; return; }
  if ( M->tid().empty()   ) { vc_mssg._075( P->vt100(), P->tell(), mid ) << EOM; return; }
  if ( M->td() != P->id() ) { vc_mssg._076( P->vt100(), P->tell(), mid ) << EOM; return; }
  
  M->adjourn( M->name(0) );
}

// tdabort <game.id> .. <game.id>

void EXE_Service::tdabort( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  Group* gp = vc_var.groups( group_td );

  if ( gp == 0 || gp->obj( P->id() ) == 0 ) { 
    vc_mssg._062( P->vt100(), P->tell() ) << EOM;
    return;
  }

  String mid( arg ); cut_match_id( mid );

  VC_Match* M = vc_var.matches( mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), mid ) << EOM; return; }
  if ( M->tid().empty()   ) { vc_mssg._075( P->vt100(), P->tell(), mid ) << EOM; return; }
  if ( M->td() != P->id() ) { vc_mssg._076( P->vt100(), P->tell(), mid ) << EOM; return; }
  
  M->abort( M->name(0) );
  M->abort( M->name(1) );
}

// ask <.counter>
// ask [_type] [my-clock [opp-clock]] <opp>
// ask [_type] [my-clock [opp-clock]]

void EXE_Service::ask( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  if ( arg.empty() ) {
    vc_mssg._078( P->vt100(), P->tell() ) << EOM;
    return;
  }
  if ( P->var.open > P->var.play.size() ) {
    VC_Match* M = VC_Request::parse( P, arg, false );
    if ( M != 0 ) M->clean_requests(); // check open < #play for each player
    return;
  }
  
  vc_mssg._026( P->vt100(), P->tell() ) << EOM;
}

// continue

void EXE_Service::cont( Client* P, const String& /*arg*/ )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<Stored>::const_iterator it = P->var.stored.begin();
  VEC<Stored>::const_iterator hi = P->var.stored.end();
  for ( ; it != hi; ++it ) {
    if ( P->var.open <= P->var.play.size() ) return;
    VC_Match* M = VC_Request::parse( P, (*it).Counter(), true );
    if ( M != 0 ) M->clean_requests();
  }
}

// accept <id>

void EXE_Service::accept( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) { vc_mssg._021( P->vt100(), P->tell() ) << EOM; return; }

  VC_Request*   R = vc_var.local ( arg );
  if ( R == 0 ) R = vc_var.global( arg );
  if ( R == 0 ) {
    vc_mssg._022( P->vt100(), P->tell(), arg ) << EOM; return;
  } else {
    Client* P2 = R->req.p2;

    if ( R->req.p1 == P ) {
      vc_mssg._030( P->vt100(), P->tell() ) << EOM; return;
    }

    /*  */ if ( R->req.p2 == 0 ) {
      R->req.p2 = P;
      bool dp1 = R->req.p1->var.dformula.eval( R->req.p1, R->req );
      if ( dp1 ) {
	R->req.p2 = 0;
	vc_mssg._017( P->vt100(), P->tell() ) << EOM; return;
      }
    } else if ( R->req.p2 != P ) {
      vc_mssg._022( P->vt100(), P->tell(), arg ) << EOM; return;
    }
    VC_Match* M = new Match( R->req );
    R->req.p2 = P2;
    delete R;
    M->clean_requests();
  }
}

// decline <id>|<player> .. <id>|<player>

void EXE_Service::decline( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    if ( (*it)[0] == '.' ) { // we have <.id>
      if ( P->var.recv( *it ) == 0 ) { Err += *it; continue; }
      VC_Request*   R = vc_var.local ( *it );
      if ( R == 0 ) R = vc_var.global( *it );
      if ( R == 0 ) { vc_con << VCFL << "request(" << *it << ")" << endl; continue; }
      delete R;
    } else {
      Client* C = vc_var.client( *it, false, false );
      if ( C == 0 ) { Err += *it; continue; }
      SET_String::const_iterator rit = P->var.recv.begin();
      SET_String::const_iterator rhi = P->var.recv.end();
      for ( ; rit != rhi; ++rit ) {
	VC_Request* R = vc_var.local( *rit );
	if ( R == 0 ) { vc_con << VCFL << "request(" << *rit << ")" << endl; continue; }
	if ( R->req.p1 != C ) continue;
	delete R;
      }
    }
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "decline" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}

// cancel <id>|<player> .. <id>|<player>

void EXE_Service::cancel( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  SET_String Set; String::parse( arg, Set );
  SET_String Err;

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    if ( (*it)[0] == '.' ) { // we have <.id>
      if ( P->var.send( *it ) == 0 ) { Err += *it; continue; }
      VC_Request*   R = vc_var.local ( *it );
      if ( R == 0 ) R = vc_var.global( *it );
      if ( R == 0 ) { vc_con << VCFL << "request(" << *it << ")" << endl; continue; }
      delete R;
    } else {
      Client* C = vc_var.client( *it, false, false );
      if ( C == 0 ) { Err += *it; continue; }
      SET_String::const_iterator rit = C->var.recv.begin();
      SET_String::const_iterator rhi = C->var.recv.end();
      for ( ; rit != rhi; ++rit ) {
	VC_Request* R = vc_var.local( *rit );
	if ( R == 0 ) { vc_con << VCFL << "request(" << *rit << ")" << endl; continue; }
	if ( R->req.p1 != C ) continue;
	if ( R->req.p1 != P ) continue;
	delete R;
      }
    }
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "cancel" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}


// aform <formula>

void EXE_Service::aform( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  ostringstream os;
  bool ok = P->var.aformula.parse( os, arg );
  if ( ! ok ) {
    P->tell();
    P->vt100( vt_start, "aform" ) << ' ';
    P->vt100( vt_error, "ERR" ) << ' ';
    P->text(os) << EOM;
  }
  else P->var.modified();
}


// dform <formula>

void EXE_Service::dform( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  ostringstream os;
  bool ok = P->var.dformula.parse( os, arg );
  if ( ! ok ) {
    P->tell();
    P->vt100( vt_start, "dform" ) << ' ';
    P->vt100( vt_error, "ERR" ) << ' ';
    P->text(os) << EOM;
  }
  else P->var.modified();
}


// play [.match] <move>

void EXE_Service::play( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) { vc_mssg._018( P->vt100(), P->tell() ) << EOM; return; }

  VC_Match* M = 0;
  String mssg;
  String mgid( arg );
  if ( mgid[0] != '.' && P->var.play.size() == 1 ) {
    M = vc_var.matches( P->var.play[0] );
    mssg = arg;
    if ( M == 0 ) { vc_log << VCFL; return; }
    mgid = M->id();
  } else {
    String rst;
    String::parse( arg, mgid, mssg );
    String sid(mgid); cut_match_id( sid );

    const String* mid = P->var.play( sid );
    if ( mid ) M = vc_var.matches( *mid );
    if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), sid ) << EOM; return; }
  }

  ostringstream os;
  bool ok = M->play( os, mgid, P->id(), mssg );
  if (! ok ) P->tell().text( os ) << EOM;
}

// undo [.match]

void EXE_Service::undo( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  if ( list.size() == 0 && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );
  
  if ( list.size() != 1 ) { vc_mssg._023( P->vt100(), P->tell(), "undo" ) << EOM; return; }

  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );
  
  const String* mid = P->var.play( sid );
  VC_Match* M = mid == 0 ? 0 : vc_var.matches( *mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), arg ) << EOM; return; }

  ostringstream os;
  bool ok = M->undo( os, mgid, P->id() );
  if (! ok ) P->tell().text( os ) << EOM;
}

// abort [.match]

void EXE_Service::abort( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  if ( list.size() == 0 && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );

  if ( list.size() != 1 ) {vc_mssg._023( P->vt100(), P->tell(), "abort" ) << EOM; return;}
  
  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );

  const String* mid = P->var.play( sid );
  VC_Match* M = mid == 0 ? 0 : vc_var.matches( *mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), arg ) << EOM; return; }
  if (! M->tid().empty() ) { vc_mssg._074( P->vt100(), P->tell() ) << EOM; return; }

  M->abort( P->id() );
}

// break [.match]

void EXE_Service::adjourn( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  if ( list.size() == 0 && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );

  if ( list.size() != 1 ) {
    vc_mssg._023( P->vt100(), P->tell(), "break" ) << EOM;
    return;
  }
  
  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );

  const String* mid = P->var.play( sid );
  VC_Match* M = mid == 0 ? 0 : vc_var.matches( *mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), arg ) << EOM; return; }
  if (! M->tid().empty() ) { vc_mssg._073( P->vt100(), P->tell() ) << EOM; return; }
  
  M->adjourn( P->id() );
}

// resign [.match]

void EXE_Service::resign( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  if ( list.size() == 0 && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );

  if ( list.size() != 1 ) {
    vc_mssg._023( P->vt100(), P->tell(), "resign" ) << EOM;
    return;
  }
  
  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );

  const String* mid = P->var.play( sid );
  VC_Match* M = mid == 0 ? 0 : vc_var.matches( *mid );
  if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), arg ) << EOM; return; }

  ostringstream os;
  bool ok = M->resign( os, mgid, P->id() );
  if (! ok ) P->tell().text( os ) << EOM;
}

// score [.match] <result>

void EXE_Service::score( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );
  
  if ( list.size() == 1 && list[0][0]!= '.' && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );

  if ( list.size() != 2 ) {
    vc_mssg._029( P->vt100(), P->tell() ) << EOM;
    return;
  }
  
  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );

  const String* mid = P->var.play( sid );
  VC_Match* M = mid == 0 ? 0 : vc_var.matches( *mid );
  if ( M == 0 ) {
    vc_mssg._037( P->vt100(), P->tell(), list[0] ) << EOM;
    return;
  }

  ostringstream os;
  bool ok = M->score( os, mgid, P->id(), list[1] );
  if (! ok ) P->tell().text( os ) << EOM;
}

// moves [.match]

void EXE_Service::moves( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  if ( list.size() == 0 && P->var.play.size() == 1 )
    list.insert( list.begin(), P->var.play[0] );

  if ( list.size() != 1 ) {
    vc_mssg._023( P->vt100(), P->tell(), "moves" ) << EOM;
    return;
  }
  
  String mgid( list[0] );
  String sid(mgid); cut_match_id( sid );

  VC_Match* M = vc_var.matches( sid );
  if ( M == 0 ) {
    vc_mssg._037( P->vt100(), P->tell(), list[0] ) << EOM;
    return;
  }

  M->call_join( P->id() );
}


// tell [a|o|p] [.match] <mssg>

bool tell_bell( char C, const Client* P )
{
  /**/ if ( C == 'A' ) return P->var.bell.tell_all();
  else if ( C == 'O' ) return P->var.bell.tell_observers();
  else if ( C == 'P' ) return P->var.bell.tell_players();
  return false;
}

void EXE_Service::tell( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) { vc_mssg._044( P->vt100(), P->tell() ) << EOM; return; }

  String mssg = arg;
  String prm, rst;
  String::parse( arg, prm, rst );
  
  bool o = false;
  bool p = false;
  
  /**/ if ( prm == "o" ) { o = true; mssg = rst; String::parse( mssg, prm, rst ); }
  else if ( prm == "p" ) { p = true; mssg = rst; String::parse( mssg, prm, rst ); }
  else if ( prm == "a" ) {           mssg = rst; String::parse( mssg, prm, rst ); }
  if ( prm.empty() ) { vc_mssg._044( P->vt100(), P->tell() ) << EOM; return; }

  String mgid( prm );
  VC_Match* M = 0;
  if ( mgid[0] != '.' && P->var.play.size() == 1 ) {
    M = vc_var.matches( P->var.play[0] );
    if ( M == 0 ) { vc_log << VCFL; return; }
    mgid = M->id();
  } else {
    String sid(mgid); cut_match_id( sid );

    M = vc_var.matches( sid );
    if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), prm ) << EOM; return; }
    mssg = rst;
  }

  ostringstream os;
  char c = 'A'; if ( o ) c = 'O'; else if ( p ) c = 'P';
  Form( os << mgid, " %c %4.0f ", c, P->rank( M->req.t.key() ) ) << P->id() << ": " << rst;

  SET_String vec;
  if (! o ) {
    vec += M->req.p1->id();
    vec += M->req.p2->id();
  }
  if (! p ) {
    Group* G = vc_var.watch( M->id() );
    if ( G != 0 ) vec.add( G->obj ); // impossible
  }
  { // remove everybody that is ignoring P
    const Group* ip = vc_var.ignore( P->id() );
    if ( ip != 0 ) {
      SET_String c;
      c.cross( vec, ip->obj );
      vec.del( c );
    }
  }
  VC_SEQ seq( vec ); seq.text( P, os, c, tell_bell );
}


#if 0

// old

void EXE_Service::tell( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) { vc_mssg._044( P->vt100(), P->tell() ) << EOM; return; }

  String mssg = arg;
  String prm, rst;
  String::parse( arg, prm, rst );
  
  bool o = false;
  bool p = false;
  
  /**/ if ( prm == "o" ) { o = true; mssg = rst; String::parse( mssg, prm, rst ); }
  else if ( prm == "p" ) { p = true; mssg = rst; String::parse( mssg, prm, rst ); }
  else if ( prm == "a" ) {           mssg = rst; String::parse( mssg, prm, rst ); }
  if ( prm.empty() ) { vc_mssg._044( P->vt100(), P->tell() ) << EOM; return; }

  String mgid( prm );
  VC_Match* M = 0;
  if ( mgid[0] != '.' && P->var.play.size() == 1 ) {
    M = vc_var.matches( P->var.play[0] );
    if ( M == 0 ) { vc_log << VCFL; return; }
    mgid = M->id();
  } else {
    String sid(mgid); cut_match_id( sid );

    M = vc_var.matches( sid );
    if ( M == 0 ) { vc_mssg._037( P->vt100(), P->tell(), prm ) << EOM; return; }
    mssg = rst;
  }

  ostringstream os;
  char c = 'A'; if ( o ) c = 'O'; else if ( p ) c = 'P';
  Form( os << mgid, " %c %4.0f ", c, P->rank( M->req.t.key() ) ) << P->id() << ": " << rst;

  SET_String vec;
  if (! o ) {
    vec += M->req.p1->id();
    vec += M->req.p2->id();
  }
  if (! p ) {
    Group* G = vc_var.watch( M->id() );
    if ( G != 0 ) vec.add( G->obj ); // impossible
  }
  VC_SEQ seq( vec ); seq.text( P, os, c, tell_bell );
}

#endif


// look <.counter>

void EXE_Service::look( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) { vc_mssg._032( P->vt100(), P->tell(), arg ) << EOM; return; }

  String sis;
  CRatio ok = vc_save.get( arg, sis );
  if ( ok.Txt() == 0 ) { vc_mssg._032( P->vt100(), P->tell(), arg ) << EOM; return; }
  stringstream is(sis);
  Request req;
  String  rec;
  req.load( is, false );
  rec.load( is );

  P->tell().vt100( vt_start, "look" ) << EOL << rec << EOM;
}

// history <login> .. <login>

void EXE_Service::history( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "history" )
      << ' ' << P->var.history.size() << ' ' << P->id() << EOL
      << P->var.history << EOM;
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    VAR_Client* V = vc_var.var( *it );
    if ( V == 0 ) { Err += *it; continue; }
    P->tell().vt100( vt_start, "history" )
      << ' ' << V->history.size() << ' ' << *it << EOL
      << V->history << EOM;
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "history" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}

// stored [+|-] | ( [login] .. [login] )

void EXE_Service::stored( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "stored" )
      << ' ' << P->var.stored.size() << ' ' << P->id() << EOL
      << P->var.stored << EOM;
    return;
  }

  /**/ if ( arg == "+" ) { P->var.notify_stored = true;  P->var.modified(); return; }
  else if ( arg == "-" ) { P->var.notify_stored = false; P->var.modified(); return; }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    VAR_Client* V = vc_var.var( *it );
    if ( V == 0 ) { Err += *it; continue; }
    P->tell().vt100( vt_start, "stored" )
      << ' ' << V->stored.size() << ' ' << *it << EOL
      << V->stored << EOM;
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "stored" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}


// help [file] .. [file]

void EXE_Service::help( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  if ( Set.empty() ) {
    P->tell().vt100( vt_start, "help" ) << " help" << EOL;
    P->mssg( "msg/help" ) << EOM;
    return;
  }

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi ; ++it ) {
    String File = "msg/" + *it;
    if (!System::file_legal( File.c_str() ) ) { Err += *it; continue; }
    if ( System::file_exist( File.c_str() ) ) {
      P->tell().vt100( vt_start, "help" ) << ' ' << *it << EOL;
      P->mssg( File ) << EOM;
      continue;
    }
    Err += *it;
  }
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "help" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}


// open <n>

void EXE_Service::open( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  sint4 o;
  if (! sint4_parse( arg, false, o ) ) {
    vc_mssg._025( P->vt100(), P->tell(), "open", arg ) << EOM; return;
  }

  P->var.open = sint4(o);
  P->var.modified();

  if ( P->var.open <= P->var.play.size() ) P->clean_requests();
}

// client [+|-]

void EXE_Service::client( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  bool client = false;
  /**/ if ( arg.empty()) client = ! P->var.client;
  else if ( arg == "+" ) client = true;
  else if ( arg == "-" ) client = false;
  else { vc_mssg._012( P->vt100(), P->tell(), "client" ) << EOM; return; }

  P->var.client = client;
  P->var.modified();
}

// bell (+|-)(r|p|w|n|ns|nn|nt|ni|nr|nw|ta|to|tp)

void EXE_Service::bell( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  bool ok = !arg.empty() && P->var.bell.parse( arg );
  if ( ok ) P->var.modified();
  else vc_mssg._020( P->vt100(), P->tell() ) << EOM;
}

// vt100 [+|-]

void EXE_Service::vt100( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  bool vt100 = false;
  /**/ if ( arg.empty()) vt100 = ! P->var.vt100;
  else if ( arg == "+" ) vt100 = true;
  else if ( arg == "-" ) vt100 = false;
  else { vc_mssg._012( P->vt100(), P->tell(), "vt100" ) << EOM; return; }

  P->var.vt100 = vt100;
  P->var.modified();
}

// hear [+|-]

void EXE_Service::hear( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  bool hear = false;
  /**/ if ( arg.empty()) hear = ! P->var.hear;
  else if ( arg == "+" ) hear = true;
  else if ( arg == "-" ) hear = false;
  else { vc_mssg._012( P->vt100(), P->tell(), "hear" ) << EOM; return; }

  P->var.hear = hear;
  P->var.modified();
}

// trust [+|-]

void EXE_Service::trust( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  bool trust = false;
  /**/ if ( arg.empty()) trust = ! P->var.trust;
  else if ( arg == "+" ) trust = true;
  else if ( arg == "-" ) trust = false;
  else { vc_mssg._012( P->vt100(), P->tell(), "trust" ) << EOM; return; }

  P->var.trust = trust;
  P->var.modified();
}

// rated [+|-]

void EXE_Service::rated( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( P->var.admin < user_level ) {
    vc_mssg._046( P->vt100(), P->tell() ) << EOM; return; 
  }

  bool rated = false;
  /**/ if ( arg.empty()) rated = ! P->var.rated;
  else if ( arg == "+" ) rated = true;
  else if ( arg == "-" ) rated = false;
  else { vc_mssg._012( P->vt100(), P->tell(), "rated" ) << EOM; return; }

  P->var.rated = rated;
  P->var.modified();
}


// request
// request [+|-] 
// request [ <login>|<.id> ...]

void EXE_Service::request_glob( VecReqPtr& A, Client* M, Client* O )
{
  SET_PTR_Request::const_iterator it = vc_var.global.begin();
  SET_PTR_Request::const_iterator hi = vc_var.global.end();
  for ( ; it != hi; ++it ) {
    if ( (*it)->req.p1 != O && O != 0 ) continue;
    if ( (*it)->req.p1 == M ) continue;
    (*it)->req.p2 = M;
    bool aa = (*it)->req.p1->var.aformula.eval( (*it)->req.p1, (*it)->req );
    bool ad = (*it)->req.p2->var.dformula.eval( (*it)->req.p2, (*it)->req );
    if ( aa && ! ad ) A.push_back( &**it );
    (*it)->req.p2 = 0;
  }
}

void EXE_Service::request_recv( VecReqPtr& A, Client* M, Client* O, const SET_String& S )
{
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) {
    VC_Request* R = vc_var.local( *it );
    if ( R == 0 ) { vc_con << VCFL; continue; }
    if ( R->req.p1 != O && O != 0 ) continue;
    if ( R->req.p2 != M ) continue;
    A.push_back( R );
  }
}

void EXE_Service::request_send( VecReqPtr& A, Client* M, Client* O, const SET_String& S )
{
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) {
    VC_Request*   R = vc_var.local ( *it );
    if ( R == 0 ) R = vc_var.global( *it );
    if ( R == 0 ) { vc_con << VCFL; continue; }
    if ( R->req.p1 != M ) continue;
    if ( R->req.p1 != O && O != 0 ) continue;
    A.push_back( R );
  }
}

void EXE_Service::request_print( VecReqPtr& A, Client* M )
{
  VecReqPtr::const_iterator it = A.begin();
  VecReqPtr::const_iterator hi = A.end();
  for ( ; it != hi; ++it ) *M << EOL << **it;
}

void EXE_Service::request( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg == "+" ) {
    P->var.notify_request = true;
    P->var.modified();
    return;
  }
  if ( arg == "-" ) {
    P->var.notify_request = false;
    P->var.modified();
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  VecReqPtr glob;
  VecReqPtr recv;
  VecReqPtr send;

  if (! Set.empty() ) {
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( (*it)[0] != '.' ) { // we have a login
	Client* C = vc_var.client( *it, false, false );
	if ( C == 0 ) { Err += *it; continue; }
	request_glob( glob, P, C );
	request_recv( recv, P, C, P->var.recv ); 
	request_send( send, P, C, P->var.send ); 
      } else {                 // we have an .id
	if ( P->var.send( *it ) != 0 ) {
	  VC_Request*   R = vc_var.local ( *it );
	  if ( R == 0 ) R = vc_var.global( *it );
	  if ( R != 0 ) send.push_back( R );
	  continue;
	}
	if ( P->var.recv( *it ) != 0 ) {
	  VC_Request* R = vc_var.local ( *it );
	  if ( R != 0 ) recv.push_back( R );
	  continue;
	}
	VC_Request* R = vc_var.global ( *it );
	if ( R != 0 ) glob.push_back( R );
	else Err += *it;
      }
    }
  } else {
    request_glob( glob, P, 0 );
    request_recv( recv, P, 0, P->var.recv );
    request_send( send, P, 0, P->var.send );
  }

  glob.unique();
  recv.unique();
  send.unique();
  
  P->tell().vt100( vt_start, "request" ) << ' ';
  Form( *P, "glob(%d/%d) recv(%d/%d) send(%d/%d)",
	glob.size(), vc_var.global.size(),
	recv.size(), P->var.recv.size(),
	send.size(), P->var.send.size() );
  if ( glob.size() ) { *P << EOL << "glob " << glob.size(); request_print( glob, P ); }
  if ( recv.size() ) { *P << EOL << "recv " << recv.size(); request_print( recv, P ); }
  if ( send.size() ) { *P << EOL << "sent " << send.size(); request_print( send, P ); }

  if (! Err.empty() ) {
    *P << EOL;
    P->vt100( vt_error, "ERR" ) << " not found: " << Err;
  }
  *P << EOM;
}

// match 
// match <login>|<.id> .. <login>|<.id> 

void EXE_Service::match( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "match" )
      << ' ' << vc_var.matches.size()
      << '/' << vc_var.matches.size();
    SET_PTR_Match::const_iterator it = vc_var.matches.begin();
    SET_PTR_Match::const_iterator hi = vc_var.matches.end();
    for ( ; it != hi; ++it ) (*it)->print( *P << EOL );
    *P << EOM;
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  SET_String vec;

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    if ( (*it)[0] != '.' ) { // we have a login
      Client* C = vc_var.client( *it, false, true );
      if ( C == 0 ) { Err += *it; continue; }
      vec.add( C->var.play );
    } else {
      VC_Match* M = vc_var.matches( *it );
      if ( M == 0 ) { Err += *it; continue; }
      vec += M->id();
    }
  }

  P->tell().vt100( vt_start, "match" )
    << ' ' << vec.size() << '/' << vc_var.matches.size();
  for ( SET_String::const_iterator it = vec.begin(), hi = vec.end(); it != hi; ++it ) {
    VC_Match* M = vc_var.matches( *it );
    if ( M == 0 ) { vc_con << VCFL << "match(" << *it << ")" << endl; continue; }
    M->print( *P << EOL );
  }

  if (! Err.empty() ) {
    *P << EOL;
    P->vt100( vt_error, "ERR" ) << " not found: " << Err;
  }
  *P << EOM;
}

//

void EXE_Service::histo_inp( Client* P, const String& arg )
{
  TSTAT;

  histo( P, arg, vc_var.histo_inp_bytes, "#input " );
}

void EXE_Service::histo_out( Client* P, const String& arg )
{
  TSTAT;

  histo( P, arg, vc_var.histo_out_bytes, "#output " );
}

void EXE_Service::histo_usr( Client* P, const String& arg )
{
  TSTAT;

  histo( P, arg, vc_var.histo_users, "#user " );
}

void EXE_Service::histo_req( Client* P, const String& arg )
{
  TSTAT;

  histo( P, arg, vc_var.histo_requests, "#request " );
}

void EXE_Service::histo_mat( Client* P, const String& arg )
{
  TSTAT;

  histo( P, arg, vc_var.histo_matches, "#match " );
}

void EXE_Service::histo( Client* P, const String& arg, const Histogram_HDMY& H, ccptr Title )
{
  TSTAT;

  if ( arg.empty() || ( arg != "h" && arg != "d" && arg != "m" && arg != "y" ) ) {
    vc_mssg._031( P->vt100(), P->tell(), arg ) << EOM;
    return;
  }
  
  P->tell();
  P->vt100( vt_start, Title );
  
  if ( arg == "h" ) {
    P->vt100( vt_start, "stat[hour]" );
    H.print( *P, Histogram_HDMY::HOURLY );
    return;
  }
  if ( arg == "d" ) {
    P->vt100( vt_start, "stat[day]" );
    H.print( *P, Histogram_HDMY::DAILY );
    return;
  }
  if ( arg == "m" ) {
    P->vt100( vt_start, "stat[month]" );
    H.print( *P, Histogram_HDMY::MONTHLY );
    return;
  }
  if ( arg == "y" ) {
    P->vt100( vt_start, "stat[year]" );
    H.print( *P, Histogram_HDMY::YEARLY );
    return;
  }
}

//

// uptime

void EXE_Service::uptime( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  P->tell();

  if ( arg.size() == 0 || arg == "-sys" ) {
    P->vt100( vt_start, "uptime" ) << EOL << vc_stat;

    if ( arg.size() != 0 ) {
      struct rlimit rlim;
      struct rusage ruse;
      *P << EOL;
      *P << "RESOURCE LIMITS :        soft / hard" << EOL;
      System::rlimit_get( RLIMIT_CPU, rlim );
      Form( *P, "cpu seconds      : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_FSIZE, rlim );
      Form( *P, "file  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_DATA, rlim );
      Form( *P, "data  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_STACK, rlim );
      Form( *P, "stack size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_CORE, rlim );
      Form( *P, "core  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_RSS, rlim );
      Form( *P, "resident memory  : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_NPROC, rlim );
      Form( *P, "# of processes   : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
#if defined(glibc)
      System::rlimit_get( RLIMIT_OFILE, rlim );
      Form( *P, "# of open files  : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
#endif
      System::rlimit_get( RLIMIT_MEMLOCK, rlim );
      Form( *P, "# of memory locks: %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      *P << "RESOURCE USAGE :" << EOL;
      System::rusage_get( ruse );
      Form( *P, "time used user / system               : %10d / %10d",
	    sint8(ruse.ru_utime.tv_sec * uSec) + ruse.ru_utime.tv_usec,
	    sint8(ruse.ru_stime.tv_sec * uSec) + ruse.ru_stime.tv_usec) << EOL;
      Form( *P, "memory max resident / integral shared : %10d / %10d", ruse.ru_maxrss, ruse.ru_ixrss ) << EOL;
      Form( *P, "unshared data / stack                 : %10d / %10d", ruse.ru_idrss, ruse.ru_isrss ) << EOL;
      Form( *P, "page reclaims / faults                : %10d / %10d", ruse.ru_minflt, ruse.ru_majflt ) << EOL;
      Form( *P, "swaps                                 : %10d",  ruse.ru_nswap ) << EOL;
      Form( *P, "block operations input / output       : %10d / %10d", ruse.ru_inblock, ruse.ru_oublock ) << EOL;
      Form( *P, "messages sent / received              : %10d / %10d", ruse.ru_msgsnd, ruse.ru_msgrcv ) << EOL;
      Form( *P, "signals received                      : %10d", ruse.ru_nsignals ) << EOL;
      Form( *P, "context switches voluntary / invol.   : %10d / %10d", ruse.ru_nvcsw, ruse.ru_nivcsw );
    }

    *P << EOM;

    return;
  }

  vc_mssg._015( P->vt100(), *P ) << EOM;
}

// salias [pattern [replacement]]

void EXE_Service::salias( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "system alias" )
      << ' '
      << vc_var.alias.size()
      << vc_var.alias << EOM;
    return;
  }
  if ( arg[0] == '?' ) {
    String Prefix( arg.data() + 1, arg.size() - 1 );
    P->tell().vt100( vt_start, "system alias" );
    vc_var.alias.print( *P, Prefix ) << EOM;
    return; 
  }

  if ( P->admin() >= admin_level ) {
    Alias A( arg );
    if ( A == "alias" ) { vc_mssg._027( P->vt100(), P->tell() ) << EOM; return; }
    vc_var.alias = A;
    vc_var.modified();
    return;
  }

  vc_mssg._013( P->vt100(), P->tell() ) << EOM;
}

// shistory

void EXE_Service::shistory( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( arg.empty() ) {
    P->tell().vt100( vt_start, "system history" ) << ' ' << vc_var.history.size() << EOL
						  << vc_var.history << EOM;
    return;
  }

  vc_mssg._043( P->vt100(), P->tell() ) << EOM;
}

// pack

void EXE_Service::pack( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  if ( P->admin() != root_level ) { vc_mssg._060( P->vt100(), P->tell() ) << EOM; return; }
  if ( arg.empty() ) {
    vc_db.  te_cancel(); vc_db.  te_create( 0, 1 );
    vc_save.te_cancel(); vc_save.te_create( 0, 1 );
    return;
  }
  vc_mssg._061( P->vt100(), P->tell() ) << EOM;
}

void EXE_Service::del_stored( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  if ( P->admin() != root_level ) { vc_mssg._060( P->vt100(), P->tell() ) << EOM; return; }
  if ( arg.empty() ) return;
  String User, Count;
  String::parse( arg, User, Count );
  VAR_Client* vc = vc_var.var( User );
  if ( vc == 0 ) {
    P->tell().vt100( vt_start, "del_stored" ) << ' ';
    P->vt100( vt_error, "ERR" ) << " not found: " << User << EOM;
    return;
  }
  vc->stored.erase( Count );
  vc->modified();
  vc->save();
  P->tell().vt100( vt_start, "del_stored" )
    << ' '
    << "'" << User  << "'" << ' '
    << "'" << Count << "'" << EOM;
  return;
}


// stat

EXE_Service::EXE_Cmd::EXE_Cmd( const String& Name, Command Cmd )
  : NAMED<Command>( Name, Cmd )
{
}

ostream& EXE_Service::EXE_Cmd::print( ostream& os ) const
{
  const String& str = *this;
  os << setw(-9) << str;
#if 0
  Command       cmd = obj;
  Form( os, "{%d, %2d, {%08p} }",
	cmd.__delta,
	cmd.__index,
	cmd.__pfn_or_delta2.__pfn,
	cmd.__pfn_or_delta2.__delta2 );
#endif  
  return os;
}

ostream& EXE_Service::EXE_Cmds::print( ostream& os ) const
{
  SET< EXE_Cmd >::const_iterator it = begin();
  SET< EXE_Cmd >::const_iterator hi = end();
  for ( ; it != hi; ++it ) os << EOL << *it;
  return os;
}

// tstat [+|-]

void EXE_Service::tstat( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  
  if (! arg.empty() ) {
    if ( P->admin() < root_level ) { vc_mssg._060( P->vt100(), *P ); return; }
    /**/ if ( arg == "+" ) { tstat_ready = true;  vc_tstat.erase(); }
    else if ( arg == "-" ) { tstat_ready = false; vc_tstat.erase(); }
    else vc_mssg._012( P->vt100(), *P, "tstat" );
    return;
  }
  P->tell().vt100( vt_start, "tstat" ) << ' ' << vc_tstat << EOM;
}

// tevent

void EXE_Service::tevent( Client* P, const String& /*arg*/ )
{
  TSTAT;
  
  if ( P == 0 ) return;
  
  P->tell().vt100( vt_start, "tevent" ) << ' ' << vc_time << EOM;
}

//: EXE_Service.C (eof) (c) Igor
