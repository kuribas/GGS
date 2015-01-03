// $Id: Global.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Michael Buro, mic@research.nj.nec.com
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


#ifndef Global_H
#define Global_H

// Othello classes
// 9.99 by Michael Buro

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "CPP.H"
#include "System.H"

#ifdef SERVICE
#ifndef NO_VC_CON
#include "Actors.H"
#include "IO_FILE.H"
#define errstr vc_con
#define ABORT  ::abort()
#endif
#else
#define errstr cerr
#define ABORT  ::abort()
#endif

#if 0
#if EOM == endl
#undef EOM
#define EOM '\n'
#endif
#endif

#define FOREVER    for (;;)
#define FOR(i, n)  for (i = 0; i < (n); i++)
#define FORS(i, n)  for (sint4 i = 0; i < (n); i++)
#define FORU(i, n)  for (uint4 i = 0; i < (n); i++)

#if defined(__GNUC__)
#define FORT(i, n)  for (typeof(n) i = 0; i < (n); i++)
#else
#define FORT(i, n)  for (int i = 0; i < (n); i++)
#endif


#define STR2STRING(os, s) s = os.str()


#if defined(__GNUC__)

#define ERR(s) {\
  errstr << std::endl << __FILE__ << " " << __FUNCTION__ << "() (line " << __LINE__ << "): "\
  << s << std::endl; ABORT;\
}

#define ERR2(s1, s2) {\
  errstr << std::endl << __FILE__ << " " << __FUNCTION__ << "() (line " << __LINE__ << "): "\
  << s1 << " " << s2 << std::endl; ABORT;\
}

#else

#define ERR(s) {\
  errstr << std::endl << __FILE__  << " (line " << __LINE__ << "): "\
  << s << std::endl; ABORT;\
}

#define ERR2(s1, s2) {\
  errstr << std::endl << __FILE__  << " (line " << __LINE__ << "): "\
  << s1 << " " << s2 << std::endl; ABORT;\
}
#endif


inline int my_round(double x) { return int(rint(x)); }

// ostream &MyForm( ostream& os, const char* fmt, ... );

class Rand {

public:

  static bool use_time_seed;
  static sint4 num(sint4 limit);  // return random 0 <= x < limit

};

extern Rand ra;


class Color
{
public:
  
  // do not change
  enum { BLACK = 0, WHITE = 1, UNDEF = 2 };

  static sint4 opponent(sint4 col);
  static void write(std::ostream &os, sint4 col);
  static sint4 read(std::istream &is);
};

#endif
