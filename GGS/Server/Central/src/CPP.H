// $Id: CPP.H 160 2007-06-22 15:21:10Z mburo $
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
//: CPP.H (bof) (c) Igor Durdanovic

#ifndef CPP_H
#define CPP_H

#include <iostream>
#include <cstdarg>

#define NEW_LINE  '\n'
#define FIX_LINE  "\n|"
#define ONE_LINE  "\\"
#define EOM '\r' << endl

#if   defined(SERVER) || defined(MULTIPLEXER)
#define EOL "\r\n"
#elif defined(SERVICE)
#define EOL '\\'
#else
#error what application?
#endif

typedef   signed char        sint1;
typedef unsigned char        uint1;
typedef   signed short       sint2;
typedef unsigned short       uint2;
typedef   signed int         sint4;
typedef unsigned int         uint4;
typedef               float  real4;
typedef               double real8;
typedef          long double realC;
typedef   signed long long   sint8;
typedef unsigned long long   uint8;

typedef void* vptr;
typedef char* cptr;

typedef const void* cvptr;
typedef const char* ccptr;

typedef const void* const cvptrc;
typedef const char* const ccptrc;


const sint1 min_sint1 = 0x80;
const sint1 max_sint1 = 0x7F;
const uint1 min_uint1 = 0x00;
const uint1 max_uint1 = 0xFF;
const sint2 min_sint2 = 0x8000;
const sint2 max_sint2 = 0x7FFF;
const uint2 min_uint2 = 0x0000;
const uint2 max_uint2 = 0xFFFF;
const sint4 min_sint4 = 0x80000000;
const sint4 max_sint4 = 0x7FFFFFFF;
const uint4 min_uint4 = 0x00000000;
const uint4 max_uint4 = 0xFFFFFFFF;
const sint8 min_sint8 = 0x8000000000000000LL;
const sint8 max_sint8 = 0x7FFFFFFFFFFFFFFFLL;
const uint8 min_uint8 = 0x0000000000000000LL;
const uint8 max_uint8 = 0xFFFFFFFFFFFFFFFFLL;

const sint8 uSec = 1000000;

#if !defined(MULTIPLEXER)
#define TSTAT TSTAT_Client _tc( __FUNCTION__, __FILE__, __LINE__ )
#define TSTAT_OK
#else
#define TSTAT
#endif

#if defined(__GNUC__)

#define VCFL VCTIME << ' ' << __FILE__ << ' ' << __LINE__ << ' ' <<__FUNCTION__ << endl

#else

#define VCFL VCTIME << ' ' << __FILE__ << ' ' << __LINE__ << endl

template <class T> bool operator != ( const T& a, const T& b ) { return ! ( a == b ); }

#endif

#define VCER "SYSERR[" << errno << "]: " << strerror( errno ) << endl
#define VCTIME System::clock() << ' ' << System::dtime( System::clock() )

//


class String;

std::ostream& Form( std::ostream& os, ccptr fmt, ... );
std::ostream& Form( std::ostream &os, ccptr fmt, va_list ap );

bool sint4_parse( const String& s, bool Sign, sint4& n );
bool sint8_parse( const String& s, bool Sign, sint8& n );

#endif

//: CPP.H (eof) (c) Igor
