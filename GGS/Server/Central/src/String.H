// $Id: String.H 160 2007-06-22 15:21:10Z mburo $
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
//: String.H (bof) (c) Igor Durdanovic

#ifndef String_H
#define String_H

#include "CPP.H"
#include <string>
#include <vector>
#include <sstream>

#if defined(SERVER) || defined(SERVICE)
class SET_String;
#endif
 
class String
  : public std::string
{
public:
  String() {}
  String( const std::string&   s ) : std::string(s) {}
  String( ccptr                s ) : std::string(s) {}
  String( std::ostringstream& os ) : std::string(os.str()) {}
  String( const std::string& s, sint4 p, sint4 l ) : std::string(s,p,l) {}
  String( ccptr              s,          sint4 l ) : std::string(s,  l) {}

  String& operator = ( ccptr s ) { std::string::operator = ( s ); return *this; }
  
  String& pack(); // removes leading and trailing spaces

  uint4 find_space( uint4 pos ) const;

  static void parse( const String& Text, String& Name, String& Rest ); // isspace
  static void parse( const String& Text, String& Name, String& Rest, char Sep );
  
  static void parse( const String& Text, std::vector<String>& Names ); // isspace
  static void parse( const String& Text, std::vector<String>& Names, char Sep );

#if defined(SERVER) || defined(SERVICE)
  static void parse( const String& Text, SET_String& Names ); // isspace
  static void parse( const String& Text, SET_String& Names, char Sep );
#endif
  
  String& replace( char p, char  s ); // replaces p(attern) with s(ubstitution)
  String& replace( char p, const String& s );

  String& replace( size_type i, size_type n, const String& s ) {
    std::string::replace( i, n, s ); return *this;
  }

  void form(const char* fmt, ... );
  
  static std::ostream& qprint( std::ostream& os, ccptr p, uint4 l );
  
  std::ostream&  print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
private:
  //std::string::find;
};


 // save string
void tag_save( ccptr tag, std::ostream& os);
// load string and check if it is same as this
bool tag_chck (ccptr tag, std::istream& is );

inline std::ostream& operator << ( std::ostream& os, const String& V ) { return V.print( os ); }

inline bool operator <  ( const String& a, const String& b )
{
  return reinterpret_cast< const std::string& >( a ) <  reinterpret_cast< const std::string& >( b );
}
inline bool operator == ( const String& a, const String& b )
{
  return reinterpret_cast< const std::string& >( a ) == reinterpret_cast< const std::string& >( b );
}

inline bool operator <  ( const String& a, const char* b )
{
  return reinterpret_cast< const std::string& >( a ) <  b;
}
inline bool operator == ( const String& a, const char* b )
{
  return reinterpret_cast< const std::string& >( a ) == b;
}

#endif

//: String.H (eof) (c) Igor
