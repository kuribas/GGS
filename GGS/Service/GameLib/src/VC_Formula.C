// $Id: VC_Formula.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Formula.H (bof) (c) Igor Durdanovic

#include "Client.H"
#include "VC_Request.H"
#include "VC_Formula.H"
#include "Game.H"
#include "Match.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

// do not close cout in main.C when debugging
#define DEBUG_EVAL 0
#define DEBUG_PARSE 0

#if DEBUG_PARSE

#define TRACE_E( msg, t, s )							\
cout << msg << Set(t) << '"' << s << '"' << endl

#define TRACE_O( msg, exp, t, s )						\
cout << msg << Set(exp->type()) << '.' << Set(t) << '"' << s << '"' << endl

#else

#define TRACE_E( msg, t, s )
#define TRACE_O( msg, exp, t, s )

#endif

using namespace std;
using namespace VC_Exp;

VC_Formula::~VC_Formula() { delete e; }

VC_Formula::VC_Formula( const VC_Formula& F ) : e(0)
{
  ostringstream os; parse( os, F.s );
}

bool VC_Formula::parse( ostream& os, const String& S )
{
  TSTAT;
  
  String ps(S), cs(S);
  ps.pack();
  cs.pack();
  if ( ps.empty() ) {
    delete e; e = 0;
    s.erase();
    return true;
  }
  Exp* pe = Exp::parse( os, ps );
  if ( pe != 0    ) {
    delete e; e = pe; s = cs;
#if DEBUG_PARSE    
    e->print(cout) << endl;
#endif    
    return true;
  }
  return false;
}

bool VC_Formula::eval ( const Client* C, const Request& R ) const
{
  TSTAT;

#if DEBUG_EVAL
  cout << __PRETTY_FUNCTION__ << ' ' << C->id()
       << " (" << R.p1->id() << ',' << R.p2->id() << ')' << endl;
#endif  
  if ( e == 0 ) return false;
  return e->eval( C, R ).b;
}

ostream& VC_Formula::print( ostream& os ) const
{
  TSTAT;
  
  os << s;

  return os;
}

void VC_Formula::erase()
{
  TSTAT;
  
  delete e; e = 0;
  s.erase();
}

ostream& VC_Formula::save ( ostream& os ) const
{
  TSTAT;
  
  s.save( os );

  return os;
}

istream& VC_Formula::load ( istream& is )
{
  TSTAT;
  
  delete e;

  s.load( is );
  if ( s.empty() ) {
    e = 0;
  } else {
    String ps(s);
    ostringstream os;
    e = Exp::parse(os, ps);
    if ( e == 0 ) {
      vc_con << VCFL << "can not reparse '" << s << "'" << endl;
      vc_con.text( os ) << endl;
      s.erase();
    }
  }

  return is;
}

Exp* parse_E1( uint4 T, ostream& os, String& S );
Exp* parse_E2( uint4 T, ostream& os, String& S );
Exp* parse_E3( uint4 T, ostream& os, String& S );
Exp* parse_E4( uint4 T, ostream& os, String& S );
Exp* parse_E5( uint4 T, ostream& os, String& S );
Exp* parse_E1_O1_E1( uint4 T, ostream& os, String& S, Exp* a1 );
Exp* parse_E2_O2_E2( uint4 T, ostream& os, String& S, Exp* a1 );
Exp* parse_E3_O3_E3( uint4 T, ostream& os, String& S, Exp* a1 );
Exp* parse_E4_O4_E4( uint4 T, ostream& os, String& S, Exp* a1 );

ccptr Val::type( TYPE T )
{
  TSTAT;
  
  switch ( T ) {
  case COLOR : return "COLOR";
  case BOOL  : return "BOOL";
  case INT   : return "INT";
  case REAL  : return "REAL";
  default    : break;
  }
  return "?";
}

ostream& Set::print( ostream& os ) const
{
  TSTAT;
  
  os << '{';
  bool any = false;
  if ( Val::BOOL & t ) {                     os << Val::type(Val::BOOL); any = true; }
  if ( Val::INT  & t ) { if (any) os << ','; os << Val::type(Val::INT);  any = true; }
  if ( Val::REAL & t ) { if (any) os << ','; os << Val::type(Val::REAL); any = true; }
  if ( Val::COLOR& t ) { if (any) os << ','; os << Val::type(Val::COLOR);any = true; }
  os << '}';
  return os;
}

ostream& Val::print( ostream& os ) const
{
  TSTAT;
  
  switch ( t ) {
  case BOOL  : os << ( b ? "BOOL(true)" : "BOOL(false)" ); break;
  case INT   : os <<   "INT(" << i << ")"; break;
  case REAL  : os <<  "REAL(" << r << ")"; break;
  case COLOR : os << "COLOR(" << c << ")"; break;
  default    : os << "?" << int(t);
  }
  return os;
}

Exp* Exp::parse( ostream& os, String& S )
{
  TSTAT;
  
  Exp* e = parse_E1( Val::BOOL, os, S );
  if (! S.empty() && e != 0 ) { os << "E0 unparsed: " << S; delete e; return 0; }
  return e;
}

//

static bool token_bool( ostream& /*os*/, String& Stream, bool& b )
{
  TSTAT;
  
  if ( Stream[0] == 'T' || Stream[0] == 'F' ) {
    b = Stream[0] == 'T';
    Stream.erase( 0, 1 );
    Stream.pack();
    return true;
  }
  return false;
}

static bool token_sint4( ostream& /*os*/, String& Stream, sint4& i )
{
  TSTAT;
  
  i = 0;
  uint4 k = 0;
  for ( ; k < Stream.size(); ++k ) {
    if ( Stream[k] >= '0' && Stream[k] <= '9' ) {
      i = i * 10 + int(Stream[k] - '0');
      continue;
    }
    break;
  }
  if ( k > 0 ) { Stream.erase( 0, k ); Stream.pack(); return true; }
  return false;
}

static bool token_real8( ostream& /*os*/, String& Stream, real8& r )
{
  TSTAT;
  
  real8  i = 0;
  real8  f = 0;
  real8 ef = 0.1;
  bool  i_mode = true;
  uint4 k = 0;
  for ( ; k < Stream.size(); ++k ) {
    if ( Stream[k] >= '0' && Stream[k] <= '9' ) {
      if ( i_mode ) i = i * 10.0 + real8(Stream[k] - '0');
      else        { f = f +   ef * real8(Stream[k] - '0'); ef *= 0.1; }
      continue;
    }
    if ( Stream[k] == '.' && i_mode ) { i_mode = false; continue; }
    break;
  }
  if ( k > 0 && ! i_mode ) { Stream.erase( 0, k ); Stream.pack(); r = i + f; return true; }
  return false;
}

static bool token_color( ostream& /*os*/, String& Stream, char& c )
{
  TSTAT;
  
  if ( Stream[0] == 'B' || Stream[0] == 'W' || Stream[0] == '?' ) {
    c = Stream[0];
    Stream.erase( 0, 1 );
    Stream.pack();
    return true;
  }
  return false;
}

static bool token( const String& Keyword, String& Stream )
{
  TSTAT;
  
  if ( Stream.size() < Keyword.size() ) return false;
  for ( uint4 i = 0; i < Keyword.size(); ++i )
    if ( Stream[i] != Keyword[i] ) return false;

  for ( uint4 i = Keyword.size(); i < Stream.size(); ++i ) {
    if ( isspace(Stream[i]) ) continue;
    Stream.erase( 0, i );
    return true;
  }
  
  Stream.erase( 0, Stream.size() );
  return true;
}

//

#define TYPE_CHECK( t, msg, exp, s )						\
if ( ( exp->type() & t ) == 0 ) {						\
  os << msg << Set(exp->type()) << "!=" << Set(t) << '"' << s << '"';		\
  delete exp;									\
  return 0;									\
}

#define EXP2_CHECK( exp1, exp2 ) if ( exp2 == 0 ) { delete exp1; return 0; }

Exp* parse_E1( uint4 T, ostream& os, String& S )
{
  TSTAT;
  
  TRACE_E( "E1", T, S );
  Exp* a1 = parse_E2( Val::ALL, os, S ); if ( a1 == 0 ) return 0;
  return parse_E1_O1_E1( T, os, S, a1 );
}

Exp* parse_E2( uint4 T, ostream& os, String& S )
{
  TSTAT;
  
  TRACE_E( "E2", T, S );
  Exp* a1 = parse_E3( Val::ALL, os, S ); if ( a1 == 0 ) return 0; 
  return parse_E2_O2_E2( T, os, S, a1 );
}

Exp* parse_E3( uint4 T, ostream& os, String& S )
{
  TSTAT;
  
  TRACE_E( "E3", T, S );
  Exp* a1 = parse_E4( Val::ALL, os, S ); if ( a1 == 0 ) return 0; 
  return parse_E3_O3_E3( T, os, S, a1 );
}

Exp* parse_E4( uint4 T, ostream& os, String& S )
{
  TSTAT;
  
  TRACE_E( "E4", T, S );
  Exp* a1 = parse_E5( Val::ALL, os, S ); if ( a1 == 0 ) return 0; 
  return parse_E4_O4_E4( T, os, S, a1 );
}

Exp* parse_E5( uint4 T, ostream& os, String& S )
{
  TSTAT;
  
  TRACE_E( "E5", T, S );
  if ( (T & Val::BOOL) && token( "synchro", S ) ) return new SYNCHRO;
  if ( (T & Val::BOOL) && token( "saved", S ) ) return new SAVED;
  if ( (T & Val::INT ) && token( "stored",S ) ) return new STORED;
  if ( (T & Val::BOOL) && token( "rated", S ) ) return new RATED;
  if ( (T & Val::BOOL) && token( "rand",  S ) ) return new RAND;
  if ( (T & Val::BOOL) && token( "komi",  S ) ) return new KOMI;
  if ( (T & Val::BOOL) && token( "anti",  S ) ) return new ANTI;
  if ( (T & Val::INT)  && token( "discs", S ) ) return new DISCS;
  if ( (T & Val::INT)  && token( "size",  S ) ) return new SIZE;

  if ( (T & Val::INT)  && token( "mc",  S ) ) return new MC;
  if ( (T & Val::INT)  && token( "oc",  S ) ) return new OC;

  if ( (T & Val::INT)  && token( "mt1", S ) ) return new MT1;
  if ( (T & Val::INT)  && token( "mm1", S ) ) return new MM1;
  if ( (T & Val::BOOL) && token( "ml1", S ) ) return new ML1;
  if ( (T & Val::INT)  && token( "mt2", S ) ) return new MT2;
  if ( (T & Val::INT)  && token( "mm2", S ) ) return new MM2;
  if ( (T & Val::BOOL) && token( "ma2", S ) ) return new MA2;
  if ( (T & Val::INT)  && token( "mt3", S ) ) return new MT3;
  if ( (T & Val::INT)  && token( "mm3", S ) ) return new MM3;
  if ( (T & Val::BOOL) && token( "ma3", S ) ) return new MA3;

  if ( (T & Val::INT)  && token( "ot1", S ) ) return new OT1;
  if ( (T & Val::INT)  && token( "om1", S ) ) return new OM1;
  if ( (T & Val::BOOL) && token( "ol1", S ) ) return new OL1;
  if ( (T & Val::INT)  && token( "ot2", S ) ) return new OT2;
  if ( (T & Val::INT)  && token( "om2", S ) ) return new OM2;
  if ( (T & Val::BOOL) && token( "oa2", S ) ) return new OA2;
  if ( (T & Val::INT)  && token( "ot3", S ) ) return new OT3;
  if ( (T & Val::INT)  && token( "om3", S ) ) return new OM3;
  if ( (T & Val::BOOL) && token( "oa3", S ) ) return new OA3;

  if ( (T & Val::REAL) && token( "mr",  S ) ) return new MR;
  if ( (T & Val::REAL) && token( "or",  S ) ) return new OR;
  if ( (T & Val::REAL) && token( "md(", S ) ) {
    Exp* a1 = parse_E3( Val::REAL, os, S ); if ( a1 == 0 ) return 0;
    if (! token( ")", S ) ) { os << "')' expected: " << S; delete a1; return 0; }
    return new MD( a1 );
  }
  if ( (T & Val::INT) && token( "rnd(", S ) ) {
    Exp* a1 = parse_E3( Val::INT, os, S ); if ( a1 == 0 ) return 0;
    if (! token( ")", S ) ) { os << "')' expected: " << S; delete a1; return 0; }
    return new RND( a1 );
  }
  if ( (T & Val::REAL) && token( "od(", S ) ) {
    Exp* a1 = parse_E3( Val::REAL, os, S ); if ( a1 == 0 ) return 0;
    if (! token( ")", S ) ) { os << "')' expected: " << S; delete a1; return 0; }
    return new OD( a1 );
  }
  if ( (T & Val::NUM) && token( "+", S ) ) {
    Exp* a1 = parse_E5( Val::NUM, os, S ); if ( a1 == 0 ) return 0;
    return a1;
  }
  if ( (T & Val::NUM) && token( "-", S ) ) {
    Exp* a1 = parse_E5( Val::NUM, os, S ); if ( a1 == 0 ) return 0;
    return new _neg( a1 );
  }
  if ( (T & Val::BOOL) && token( "!", S ) ) {
    Exp* a1 = parse_E5( Val::BOOL, os, S ); if ( a1 == 0 ) return 0;
    return new _neg( a1 );
  }
  if ( (T & Val::INT) && token( "playing(", S ) ) {
    Exp* a1 = parse_E3( Val::BOOL, os, S ); if ( a1 == 0 ) return 0;
    if (! token( ")", S ) ) { os << "')' expected: " << S; delete a1; return 0; }
    return new PLAYING( a1 );
  }
  if ( token( "(", S ) ) {
    Exp* a1 = parse_E1( T, os, S ); if ( a1 == 0 ) return 0;
    if (! token( ")", S ) ) { os << "')' expected: " << S; delete a1; return 0; }
    return a1;
  }
  bool   b; if ( (T & Val::BOOL) && token_bool  ( os, S, b ) ) return new VExp(Val(b));
  real8  r; if ( (T & Val::REAL) && token_real8 ( os, S, r ) ) return new VExp(Val(r));
  sint4  i; if ( (T & Val::INT ) && token_sint4 ( os, S, i ) ) return new VExp(Val(i));
  char   c; if ( (T & Val::COLOR)&& token_color ( os, S, c ) ) return new VExp(Val(c));
  os << "E5.?: " << S; return 0;
}

Exp* parse_E1_O1_E1( uint4 T, ostream& os, String& S, Exp* a1 ) 
{
  TSTAT;
  
  TRACE_O( "O1", a1, T, S );
  if (! S.empty() ) {  
    if ( (T & Val::BOOL) && token( "|", S ) ) {
      TYPE_CHECK( Val::BOOL, "O1.|", a1, S );
      Exp* a2 = parse_E1( Val::BOOL, os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E1_O1_E1( Val::BOOL, os, S, new _or( a1, a2 ) );
    }
    if ( (T & Val::BOOL) && token( "&", S ) ) {
      TYPE_CHECK( Val::BOOL, "O1.&", a1, S );
      Exp* a2 = parse_E1( Val::BOOL, os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E1_O1_E1( Val::BOOL, os, S, new _and( a1, a2 ) );
    }
  }
  TYPE_CHECK( T, "O1.e", a1, S ); 
  return a1;
}

Exp* parse_E2_O2_E2( uint4 T, ostream& os, String& S, Exp* a1 )
{
  TSTAT;
  
  TRACE_O( "O2", a1, T, S );
  if (! S.empty() ) {
  
    if ( (T & Val::BOOL) && token( "==", S ) ) {
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _eq( a1, a2 );
    }
    if ( (T & Val::BOOL) && token( "!=", S ) ) {
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _ne( a1, a2 );
    }
    if ( (T & Val::BOOL) && token( "<=", S ) ) {
      TYPE_CHECK( Val::NUM, "O2.<=", a1, S );
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _le( a1, a2 );
    }
    if ( (T & Val::BOOL) && token( ">=", S ) ) {
      TYPE_CHECK( Val::NUM, "O2.>=", a1, S );
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _ge( a1, a2 );
    }
    if ( (T & Val::BOOL) && token( "<", S ) ) {
      TYPE_CHECK( Val::NUM, "O2.<", a1, S );
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _lt( a1, a2 );
    }
    if ( (T & Val::BOOL) && token( ">", S ) ) {
      TYPE_CHECK( Val::NUM, "O2.>", a1, S );
      Exp* a2 = parse_E2( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return new _gt( a1, a2 );
    }
  }
  TYPE_CHECK( T, "O2.e", a1, S );  
  return a1;
}

Exp* parse_E3_O3_E3( uint4 T, ostream& os, String& S, Exp* a1 )
{
  TSTAT;
  
  TRACE_O( "O3", a1, T, S );
  if (! S.empty() ) {
    if ( (T & Val::NUM) && token( "+", S ) ) {
      TYPE_CHECK( Val::NUM, "O3.+", a1, S );
      Exp* a2 = parse_E3( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E3_O3_E3( T, os, S, new _add( a1, a2 ) );
    }
    if ( (T & Val::NUM) && token( "-", S ) ) {
      TYPE_CHECK( Val::NUM, "O3.-", a1, S );
      Exp* a2 = parse_E3( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E3_O3_E3( T, os, S, new _sub( a1, a2 ) );
    }
  }
  TYPE_CHECK( T, "O3.e", a1, S );
  return a1;
}

Exp* parse_E4_O4_E4( uint4 T, ostream& os, String& S, Exp* a1 )
{
  TSTAT;
  
  TRACE_O( "O4", a1, T, S );
  if (! S.empty() ) { 
    if ( (T & Val::NUM) && token( "*", S ) ) {
      TYPE_CHECK( Val::NUM, "O4.*", a1, S );
      Exp* a2 = parse_E3( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E4_O4_E4( T, os, S, new _mul( a1, a2 ) );
    }
    if ( (T & Val::NUM) && token( "/", S ) ) {
      TYPE_CHECK( Val::NUM, "O4./", a1, S );
      Exp* a2 = parse_E3( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E4_O4_E4( T, os, S, new _div( a1, a2 ) );
    }
    if ( (T & Val::INT) && token( "%", S ) ) {
      TYPE_CHECK( Val::INT, "O4./", a1, S );
      Exp* a2 = parse_E3( a1->type(), os, S );
      EXP2_CHECK( a1, a2 );
      return parse_E4_O4_E4( T, os, S, new _mod( a1, a2 ) );
    }
  }
  TYPE_CHECK( T, "O4.e", a1, S );
  return a1;
}

Val SYNCHRO ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.synchro() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif  
  return v;
}
Val SAVED ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.saved() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val STORED::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.stored());
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val RATED ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.rated()   );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val RAND  ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.rand()  );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val KOMI  ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.komi()  );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val ANTI  ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.anti()  );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val DISCS ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.discs() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val SIZE  ::eval( const Client* /*C*/, const Request& R ) const
{
  TSTAT;
  
  Val v( R.size()  );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MC::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.color(0) : R.color(1) );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MT1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ini() : R.clk(1).Ini() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MM1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ini_no() : R.clk(1).Ini_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val ML1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ini_loss() : R.clk(1).Ini_loss() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MT2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Inc() : R.clk(1).Inc() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MM2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Inc_no() : R.clk(1).Inc_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MA2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Inc_add() : R.clk(1).Inc_add() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MT3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ext() : R.clk(1).Ext() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MM3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ext_no() : R.clk(1).Ext_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MA3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.clk(0).Ext_add() : R.clk(1).Ext_add() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MR::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(0) ? R.rating(0) : R.rating(1) );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val MD::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  if (! R.rated() ) return Val( 0.0 );
  real8 mr = C->id() == R.name(0) ? R.rating(0) : R.rating(1);
  real8 md = C->id() == R.name(0) ? R.ratdev(0) : R.ratdev(1);
  real8 Or = C->id() == R.name(1) ? R.rating(0) : R.rating(1);
  real8 od = C->id() == R.name(1) ? R.ratdev(0) : R.ratdev(1);
  Val result = a1->eval( C, R );
  real8 nresult = Match::score( result.r );
  real8 nr = mr;
  VC_Rating::rate( nresult, nr, md, Or, od );
  Val v( nr - mr );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OC::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.color(0) : R.color(1) );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OT1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ini() : R.clk(1).Ini() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OM1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ini_no() : R.clk(1).Ini_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OL1::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ini_loss() : R.clk(1).Ini_loss() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OT2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Inc() : R.clk(1).Inc() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OM2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Inc_no() : R.clk(1).Inc_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OA2::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Inc_add() : R.clk(1).Inc_add() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OT3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ext() : R.clk(1).Ext() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OM3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ext_no() : R.clk(1).Ext_no() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OA3::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.clk(0).Ext_add() : R.clk(1).Ext_add() );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OR::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( C->id() == R.name(1) ? R.rating(0) : R.rating(1) );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val OD::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  if (! R.rated() ) return Val( 0.0 );
  real8 mr = C->id() == R.name(1) ? R.rating(0) : R.rating(1);
  real8 md = C->id() == R.name(1) ? R.ratdev(0) : R.ratdev(1);
  real8 Or = C->id() == R.name(0) ? R.rating(0) : R.rating(1);
  real8 od = C->id() == R.name(0) ? R.ratdev(0) : R.ratdev(1);
  Val result = a1->eval( C, R );
  real8 nresult = Match::score( result.r );
  real8 nr = mr;
  VC_Rating::rate( nresult, nr, md, Or, od );
  Val v( nr - mr );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val RND::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val result = a1->eval( C, R );
  Val v ( sint4(0) );
  if ( result.i < 0 ) {
    v.i = - random() % (-result.i);
  } else if ( result.i > 0 ) {
    v.i = random() % result.i;
  }
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _or::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( a1->eval( C, R ).b || a2->eval( C, R ).b );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _and::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v( a1->eval( C, R ).b && a2->eval( C, R ).b );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _eq::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  /**/ if ( a1->type() == Val::BOOL ) v = Val( a1->eval( C, R ).b == a2->eval( C, R ).b );
  else if ( a1->type() == Val::INT  ) v = Val( a1->eval( C, R ).i == a2->eval( C, R ).i );
  else if ( a1->type() == Val::REAL ) v = Val( a1->eval( C, R ).r == a2->eval( C, R ).r );
  else if ( a1->type() == Val::COLOR) v = Val( a1->eval( C, R ).c == a2->eval( C, R ).c );
  else {
    vc_con << VCFL << int(a1->type()) << ' ' << int(a2->type()) << endl;
    return Val(0);
  }
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _ne::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  /**/ if ( a1->type() == Val::BOOL ) v = Val( a1->eval( C, R ).b != a2->eval( C, R ).b );
  else if ( a1->type() == Val::INT  ) v = Val( a1->eval( C, R ).i != a2->eval( C, R ).i );
  else if ( a1->type() == Val::REAL ) v = Val( a1->eval( C, R ).r != a2->eval( C, R ).r );
  else if ( a1->type() == Val::COLOR) v = Val( a1->eval( C, R ).c != a2->eval( C, R ).c );
  else {
    vc_con << VCFL << int(a1->type()) << ' ' << int(a2->type()) << endl;
    return Val(0);
  }
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _lt::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i < a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r < a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _le::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i <= a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r <= a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _gt::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i > a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r > a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _ge::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i >= a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r >= a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _neg::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  /**/ if ( a1->type() == Val::INT  ) v = Val( 0   - a1->eval( C, R ).i );
  else if ( a1->type() == Val::REAL ) v = Val( 0.0 - a1->eval( C, R ).r );
  else v = Val( ! a1->eval( C, R ).b );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _add::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i + a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r + a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _sub::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i - a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r - a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _mul::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  )
    v = Val( a1->eval( C, R ).i * a2->eval( C, R ).i );
  else
    v = Val( a1->eval( C, R ).r * a2->eval( C, R ).r );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _div::eval( const Client* C, const Request& R ) const
{
  TSTAT;

  Val v(false);
  if ( a1->type() == Val::INT  ){
    sint4 d = a2->eval( C, R ).i;
    if ( d == 0 )
      v = Val( 0 );
    else
      v = Val( a1->eval( C, R ).i / d ); 
  } else {
    real8 d = a2->eval( C, R ).r;
    if ( d == 0.0 )
      v = Val( 0.0 );
    else
      v = Val( a1->eval( C, R ).r / d );
  }
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}
Val _mod::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v(false);
  if ( a1->type() == Val::INT  ){
    sint4 d = a2->eval( C, R ).i;
    if ( d == 0 )
      v =Val( 0 );
    else
      v = Val( a1->eval( C, R ).i % d ); 
  } else 
    v = Val( 0 );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif
  return v;
}

Val PLAYING ::eval( const Client* C, const Request& R ) const
{
  TSTAT;
  
  Val v1 = a1->eval( C, R );
  Val v( C->playing( v1.b ) );
#if DEBUG_EVAL
  v.print( cout << __PRETTY_FUNCTION__ << ' ' ) << endl;
#endif  
  return v;
}

//: VC_Formula.C (eof) (c) Igor
