// $Id: CPP.C 9037 2010-07-06 04:05:44Z mburo $
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
//: CPP.C (bof) (c) Igor Durdanovic

#include "CPP.H"
#include "String.H"
#include "System.H"
#include <cassert>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

static char *alloc_sprintf(const char *fmt, va_list ap)
{
  static int     size = 1;
  static char*   p = (char*) System::malloc( sizeof(*p) );
  static va_list ap2;

  for (;;) {
    /* Try to print in the allocated space. */
    va_copy(ap2, ap);
    int n = vsnprintf (p, size, fmt, ap2);
    va_end(ap2);
    
    /* If that worked, return the string. */
    if (n > -1 && n < size) return p;
    
    /* Else try again with more space. */
    size *= 2;  /* twice the old size */
    p = (char*) System::realloc( p, size );
  }
}

#if 0
static char *alloc_sprintf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *msg = alloc_sprintf(fmt, ap);
  va_end(ap);
  return msg;
}
#endif

ostream& Form(ostream &os, const char *fmt, va_list ap)
{
  return os << alloc_sprintf( fmt, ap );
}

ostream& Form( ostream& os, const char* fmt, ... )
{
  TSTAT;
  
#if 1
  va_list ap;
  va_start(ap, fmt);
  Form(os, fmt, ap);
  va_end(ap);
  return os;
#else
  va_list ap;
  va_start( ap, fmt );
  os.vform( fmt, ap );
  va_end(ap);
  return os;
#endif  
}

//

bool sint4_parse( const String& s, bool Sign, sint4& n )
{
  TSTAT;
  
  if ( s.empty() ) return false;

  String::const_iterator it = s.begin();
  String::const_iterator hi = s.end();

  bool sign = !Sign;
  for ( ; it != hi; ++it ) {
    if ( ( *it == '+' || *it == '-' ) && ! sign ) { sign = true; continue; }
    if (! isdigit( *it ) ) return false;
  }

  n = atoi( s.c_str() );

  return true;
}

bool sint8_parse( const String& s, bool Sign, sint8& n )
{
  TSTAT;
  
  sint4 n4;
  bool ok = sint4_parse( s, Sign, n4 );
  if ( ok ) n = n4;
  return ok;
}

//

class CPP { public: CPP (); };

CPP::CPP()
{
  assert( sizeof(sint1) == 1 );
  assert( sizeof(uint1) == 1 );
  assert( sizeof(sint2) == 2 );
  assert( sizeof(uint2) == 2 );
  assert( sizeof(sint4) == 4 );
  assert( sizeof(uint4) == 4 );
  assert( sizeof(sint8) == 8 );
  assert( sizeof(uint8) == 8 );
  assert( sizeof(real4) == 4 );
  assert( sizeof(real8) == 8 );
  assert( sizeof(realC) == 12 );
}

static CPP _cpp_types; // check basic types

//: CPP.C (eof) (c) Igor
