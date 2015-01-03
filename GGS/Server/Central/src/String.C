// $Id: String.C 160 2007-06-22 15:21:10Z mburo $
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
//: String.C (bof) (c) Igor Durdanovic

#include "String.H"
#include <iostream>
#include <iomanip>
#include <cstdarg>

#if defined(SERVER) || defined(SERVICE)
#include "SET_String.H"
#endif

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

uint4 String::find_space( uint4 pos ) const
{
  TSTAT;
  
  const_iterator it = begin() + pos;
  const_iterator hi = end();
  if ( it == hi ) return pos;
  for ( ; it != hi; ++it ) if ( isspace( *it ) ) break;
  return it - begin();
}

void String::parse( const String& Text, String& Name, String& Rest )
{
  TSTAT;
  
  Name.erase();
  Rest.erase();

  String::const_iterator it1 = Text.begin();
  String::const_iterator it2;
  while ( it1 != Text.end() &&  isspace(*it1) ) ++it1; // position at the first non-space
  it2 = it1;
  while ( it2 != Text.end() && !isspace(*it2) ) ++it2; // position at the next space
  for ( ; it1 != it2; ++it1 ) Name += *it1;            // copy to Name

  it1 = it2;
  while ( it1 != Text.end() &&  isspace(*it1) ) ++it1; // position at the first non-space
  it2 = Text.end();
  if ( it1 == it2 ) return;
  --it2;
  while ( it2 != it1 && isspace(*it2) ) --it2;         // position at the last non-space
  ++it2;
  for ( ; it1 != it2; ++it1 ) Rest += *it1;            // copy to Rest 
}

void String::parse( const String& Text, String& Name, String& Rest, char Sep )
{
  TSTAT;
  
  Name.erase();
  Rest.erase();

  String::const_iterator it1 = Text.begin();
  String::const_iterator it2 = it1;
  while ( it2 != Text.end() && *it2 != Sep ) ++it2;    // position at the first Sep
  for ( ; it1 != it2; ++it1 ) Name += *it1;            // copy to Name

  if ( it1 != Text.end() ) ++it1;
  it2 = Text.end();
  for ( ; it1 != it2; ++it1 ) Rest += *it1;            // copy to Rest 
}

void String::parse( const String& Text, vector<String>& Names )
{
  TSTAT;
  
  String rest( Text );
  String name;
  String tmp;

  Names.erase( Names.begin(), Names.end() );

  for ( ;; ) {
    String::parse( rest, name, tmp ); rest = tmp;
    if ( name.empty() ) break;
    Names.push_back( name );
  }
}

void String::parse( const String& Text, vector<String>& Names, char Sep )
{
  TSTAT;
  
  String rest( Text );
  String name;
  String tmp;

  Names.erase( Names.begin(), Names.end() );

  for ( ;; ) {
    String::parse( rest, name, tmp, Sep ); rest = tmp;
    if ( name.empty() ) { if ( rest.empty() ) break; else continue; }
    Names.push_back( name );
  }
}

#if defined(SERVER) || defined(SERVICE)
void String::parse( const String& Text, SET_String& Names )
{
  TSTAT;
  
  String rest( Text );
  String name;
  String tmp;

  Names.erase();

  for ( ;; ) {
    String::parse( rest, name, tmp ); rest = tmp;
    if ( name.empty() ) break;
    Names += name;
  }
}

void String::parse( const String& Text, SET_String& Names, char Sep )
{
  TSTAT;
  
  String rest( Text );
  String name;
  String tmp;

  Names.erase();

  for ( ;; ) {
    String::parse( rest, name, tmp, Sep ); rest = tmp;
    if ( name.empty() ) { if ( rest.empty() ) break; else continue; }
    Names += name;
  }
}
#endif

String& String::replace( char p, char s )
{
  TSTAT;
  
  String::iterator it = begin();
  String::iterator hi = end();
  for ( ; it != hi; ++it ) if ( *it == p ) *it = s;
  return *this;
}

String& String::replace( char p, const String& s )
{
  TSTAT;
  
  String::iterator it = begin();
  String::iterator hi = end();
  uint4 sL = s.size() - 1;
  for ( ; it != hi; ++it ) if ( *it == p ) {
    int i = it - begin() + sL;
    string::replace( it, it+1, s );
    it = begin() + i; // must refresh iterators
    hi = end();
  }
  return *this;
}

String& String::pack()
{
  TSTAT;
  
  {
    String::iterator it = begin();
    String::iterator hi = end();
    for ( ; it != hi && isspace(*it); ++it );
    erase( begin(), it );
  }
  {
    String::reverse_iterator it = rbegin();
    String::reverse_iterator hi = rend();
    for ( ; it != hi && isspace(*it); ++it );
    erase(size() - (it - rbegin()), it - rbegin() );
  }
  
  return *this;
}

void String::form(const char* fmt, ... )
{
  erase();

  ostringstream oss;
  
  va_list ap;
  va_start(ap, fmt);

  Form( oss, fmt, ap );

  va_end(ap);

  append(oss.str());
}

ostream& qprint_chr( ostream& os, char c )
{
  switch ( c ) {
  case '\\' : return os << '\\' << '\\';
  case '\'' : return os << '\\' << '\'';
  case '"'  : return os << '\\' << '"';
  case '\a' : return os << '\\' << 'a';
  case '\n' : return os << '\\' << 'n';
  case '\r' : return os << '\\' << 'r';
  case '\t' : return os << '\\' << 't';
  case '\v' : return os << '\\' << 'v';
  default   : {
    if ( isprint( c ) ) return os << c;
    uint1 d0 = uint1(c)%16;
    uint1 d1 = uint1(c)/16;
    os << '\\' << 'x'
       << (d1<10 ? char(d1+'0') : char(d1-10+'A'))
       << (d0<10 ? char(d0+'0') : char(d0-10+'A'));
  }
  }
  return os;
}

ostream& String::qprint( ostream& os, ccptr p, uint4 l )
{
  if ( l > 2000 ) return os;
  for ( uint4 i = 0; i < l; ++i ) {
    qprint_chr( os, p[i] ) << flush;
  }
  return os;
}

ostream& String::print( ostream& os ) const
{
  TSTAT;
  
  const string& s = *this;
  sint4 w = os.width();  os << setw(0);
  sint4 W = w < 0 ? -w : w;
  sint4 d = W - sint4(s.size());
  if ( d > 0 && w > 0 ) for ( int i = 0; i < d; i++ ) os << ' ';
  os << s;
  if ( d > 0 && w < 0 ) for ( int i = 0; i < d; i++ ) os << ' ';
  os << setw(0);
  return os;
}

ostream& String::save( ostream& os ) const
{
  TSTAT;
  
  sint4 n = size();
  os.write( ccptr(&n), sizeof(n) );
  os.write( data(), n );
  return os;
}

istream& String::load( istream& is )
{
  TSTAT;
  
  erase();
  sint4 n = 0;
  is.read( cptr(&n), sizeof(n) );
  char* buf = new char[n];
  is.read( buf, n );
  append( buf, n );
  delete [] buf;
  return is;
}

void String::tag_save( ostream& os ) const
{
  TSTAT;

  save( os );
}

bool String::tag_chck( istream& is ) const
{
  TSTAT;

  String tmp;

  tmp.load( is );

  return tmp == *this;
}

//: String.C (eof) (c) Igor
