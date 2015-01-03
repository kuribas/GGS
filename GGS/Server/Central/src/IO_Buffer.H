// $Id: IO_Buffer.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_Buffer.H (bof) (c) Igor Durdanovic

#ifndef IO_Buffer_H
#define IO_Buffer_H

#include "CPP.H"
#include <sstream>

class IO;

class IO_Buffer
  : public std::streambuf
{
public:
  uint4  max;

  enum Method { NONE = 0, BZIP2, GZIP };

  IO_Buffer( Method m, uint4 Max, IO* Device ); // device knows how to sync()
  ~IO_Buffer();

  bool  full( bool IO ) { return size(IO) > max; }

  char* find( char c );

  char* last( bool IO );
  char* buff( bool IO );
  uint4 size( bool IO );

  void append( bool IO, ccptr mssg, uint4 len );
  void remove( bool IO, uint4 len );

  void extend( bool IO, uint4 len ); // makes realloc, but doesn't move ptr (size)
  void update( bool IO, uint4 len ); // updates ptr (size)

  void   compress( bool Stat = true ); // _Tarr -> _Zarr and cleans _Tarr
  void decompress( bool Stat = true ); // _Zarr -> _Tarr and cleans _Zarr

  void Tappend( std::ostringstream& os ); // always on txt buffer
  void Tappend( ccptr file, bool EOL_eq_EOM ); // always on txt buffer

  /*  */ Method method()     const     { return _method; }
  /*  */ ccptr  method_str() const     { return MethodStr[ _method ]; }
  static ccptr  method_str( Method m ) { return MethodStr[ m ]; }
  
protected:
  char* _Tarr; // resizable array
  uint4 _Tlen; // allocated size 

  int sync();
  int overflow( int c );

private:
  static char* MethodStr[];

  char* _Zarr; // format: Clen : byte2, Dlen : byte2, array : byte*
  char* _Zptr; // pointer to end of used space
  uint4 _Zlen; // allocated size
  
  Method _method;
  IO*    _device;

  bool   bzip2( bool Stat );
  bool bunzip2( bool Stat );

  bool deflate( bool stat );
  bool inflate( bool stat );

  void Tappend( ccptr mssg, uint4 len );
  void Tremove( uint4 len );

  void Textend( uint4 len ); // makes realloc, but doesn't move ptr (size)
  void Tupdate( uint4 len ); // updates ptr

  void Zappend( ccptr mssg, uint4 len );
  void Zremove( uint4 len );

  void Zextend( uint4 len ); // makes realloc, but doesn't move ptr (size)
  void Zupdate( uint4 len ); // updates ptr
};

#endif

//: IO_Buffer.H (eof) (c) Igor
