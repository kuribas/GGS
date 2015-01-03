// $Id: SET_Alias.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Alias.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "SET_Alias.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

const String SET_Alias::tag( "SET_Alias" );

bool SET_Alias::replace_alias( String& Name, const SET_Alias* Next ) const
{
  TSTAT;
  
  const Alias* a = (*this)( Name );
  if ( a == 0 && Next != 0 ) a = (*Next)( Name );
  if ( a == 0 ) return false;

  Name = a->def();
  return true;
}

void SET_Alias::replace_var( String& Text ) const
{
  TSTAT;
  
  for ( String::size_type pos = 0;; ) {
    if ( Text.size() > max_str_len ) break;
    String::size_type it = Text.find( '$', pos );
    if ( it == String::npos ) break;
    String::size_type hi = Text.find_space( it + 1 );
    pos = hi;
    String Name( Text.data() + it + 1, hi - it - 1 );
    if (! Alias::var_name_ok( Name ) ) continue;
    if ( it > 0 && Text[it-1] == '\'' ) { // it is a quote
      Text.erase( it-1, 1 );
      continue;
    }
    const String& a = def( Name );
    Text.replace( it, hi - it, a );
    pos = it + a.size();
  }
}

void SET_Alias::replace_arg( String& Text, const String& CArg )
{
  TSTAT;
  
  String Arg( CArg );
  
  VEC<String> args;
  bool args_ok = false;

  for ( String::size_type pos = 0;; ) {
    if ( Text.size() > max_str_len ) break;
    String::size_type it = Text.find( '$', pos );
    if ( it == String::npos ) break;
    String::size_type hi = Text.find_space( it+1 ); pos = hi;
    if ( it > 0 && Text[it-1] == '\'' ) { // it is a quote
      Text.erase( it-1, 1 );
      continue;
    }
    String Name( Text.data() + it + 1, hi - it - 1 );
    if ( Name.empty() ) { // $*
      Text.replace( it, hi - it, Arg );
      pos = it + Arg.size();
      continue;
    }
    if ( Name == "-" ) { // shift
      if (! Arg.empty() ) { String h,t; String::parse( Arg, h, t ); Arg = t; }
      if ( args_ok ) if (! args.empty() ) args.erase( args.begin() );
      Text.replace( it, hi - it + 1, str_empty );
      pos = it;
      continue;
    }

    sint4 sidx;
    bool ok = sint4_parse( Name, false, sidx );
    if (! ok ) continue;
    uint4 uidx = sidx;
    if (! args_ok ) { String::parse( Arg, args ); args_ok = true; }
    if ( uidx >= args.size() ) { // $<out of range>
      Text.replace( it, hi - it, str_empty );
      pos = it;
    } else {
      Text.replace( it, hi - it, args[uidx] );
      pos = it + args[uidx].size();
    }
  }
}

void SET_Alias::erase()
{
  TSTAT;
  
  _size = 0;
  SET<Alias>::erase();
}

Alias* SET_Alias::operator += ( const Alias& A )
{
  TSTAT;
  
  if ( A[0] != '_' ) ++_size;
  return SET<Alias>::operator+=( A );
}

void   SET_Alias::operator -= ( const Alias& A )
{
  TSTAT;
  
  if ( A[0] != '_' ) --_size;
  SET<Alias>::operator-=( A );
}
  
SET_Alias& SET_Alias::operator = ( const Alias& a )
{
  TSTAT;
  
  Alias *p = (*this)( a );

  if ( p == 0 ) { 
    if (! a.def().empty() )   (*this) += a; return *this; }
    if (  a.def().empty() ) { (*this) -= a; return *this; }
  p->def( a.def() );
  return *this;
}

const String& SET_Alias::def ( const String& s ) const
{
  TSTAT;
  
  const Alias *p = (*this)( s );

  return p == 0 ? str_empty : p->def();
}

ostream& SET_Alias::print( ostream& os ) const
{
  TSTAT;
  
  const_iterator it = begin();
  const_iterator hi = end();

  for ( ; it != hi; ++it ) (*it).print( os );
  return os;
}

ostream& SET_Alias::print( ostream& os, const String& Prefix ) const
{
  TSTAT;
  
  const_iterator it = begin();
  const_iterator hi = end();

  for ( ; it != hi; ++it ) (*it).print( os, Prefix );
  return os;
}

ostream& SET_Alias::save( ostream& os ) const
{
  TSTAT;

  tag.tag_save( os );
  
  os.write( ccptr(&_size), sizeof(_size) );
  const_iterator it = begin();
  const_iterator hi = end();
  for ( ; it != hi; ++it ) if ( (*it)[0] != '_' ) (*it).save( os );
  return os;
}

istream& SET_Alias::load( istream& is )
{
  TSTAT;

  if (! tag.tag_chck( is ) ) { vc_con << VCFL; System::exit(-1); }
  
  erase();
  sint4 n;
  is.read( cptr(&n), sizeof(n) );
  for ( Alias A; --n >= 0 ; ) { A.load( is ); (*this) += A; }
  return is;
}

//: SET_Alias.C (eof) (c) Igor
