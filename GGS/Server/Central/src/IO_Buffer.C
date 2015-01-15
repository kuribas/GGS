// $Id: IO_Buffer.C 160 2007-06-22 15:21:10Z mburo $
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
//: IO_Buffer.C (bof) (c) Igor Durdanovic

#include "CPP.H"
#include "Actors.H"
#include "System.H"
#include "Stat.H"
#include "IO_FILE.H"
#include "String.H"
#include <cstring>
#include <cstdio>
extern "C" { 
#include <bzlib.h> 
#include <zlib.h>
}

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

const char* IO_Buffer::MethodStr[] = { "none", "bzip2", "gzip" };

IO_Buffer::IO_Buffer( Method m, uint4 Max, IO* Device )
  : max(Max),
    _Tarr(0), _Tlen(0),
    _Zarr(0), _Zptr(0), _Zlen(0), 
    _method(m),
    _device(Device)
{ 
  setp( _Tarr, _Tarr + _Tlen ); 
}

IO_Buffer::~IO_Buffer()
{
  System::free( _Tarr );
  setp( 0, 0 ); // clean streambuf pointers !!

  System::free( _Zarr );
  _Zarr = 0;
  _Zptr = 0;
}

char* IO_Buffer::last( bool IO )
{
  TSTAT;
  
  if ( IO && _method != NONE ) return _Zptr;
  return pptr();
}

char* IO_Buffer::buff( bool IO )
{
  TSTAT;
  
  if ( IO && _method != NONE ) return _Zarr;
  return _Tarr;
}

uint4 IO_Buffer::size( bool IO )
{
  TSTAT;
  
  if ( IO && _method != NONE ) return _Zptr - _Zarr;
  return pptr() - _Tarr;
}

void IO_Buffer::extend( bool IO, uint4 len )
{
  TSTAT;
  
  if ( IO && _method != NONE ) Zextend( len ); else Textend( len );
}

void IO_Buffer::update( bool IO, uint4 len )
{
  TSTAT;
  
  if ( IO && _method != NONE ) Zupdate( len ); else Tupdate( len );
}

//

void IO_Buffer::append( bool IO, ccptr mssg, uint4 len )
{
  TSTAT;
  
  if ( IO && _method != NONE ) Zappend( mssg, len ); else Tappend( mssg, len );
}

void IO_Buffer::remove( bool IO, uint4 len )
{
  TSTAT;
  
  if ( IO && _method != NONE ) Zremove( len ); else Tremove( len );
}

//

char* IO_Buffer::find( char c )
{
  TSTAT;
  
  char* it = _Tarr;
  char* hi = pptr();
  for ( ; it != hi; ++it ) if ( *it == c ) return it;
  return 0;
}

void IO_Buffer::Tappend( ostringstream& os )
{
  TSTAT;

  const string& s = os.str();
  Tappend( s.data(), (uint4)s.size() );
}

void IO_Buffer::Tappend( ccptr file, bool /*EOL_eq_EOM*/ )
{
  TSTAT;
  
  sint4 len = System::file_size( file );

  if ( len == 0 ) return;

  sint4 fd = System::file_open( file, O_RDONLY, 0 );

  if ( fd < 0 ) return; // couldn't open file, no free fd-s ?

  char* buf = cptr( System::malloc( len ) );
  int   cnt = 0;
  for ( int n = 0; cnt != len && n < 8 ; n++ ) { // try at most n times
    int no = System::file_read( fd, buf + cnt, len - cnt );
    cnt += no;
  }
  String line( buf, cnt );
  line.replace( NEW_LINE, EOL );

  System::free( buf );
  System::file_close( fd );

  append( false, line.data(), line.size() );
}

int IO_Buffer::sync()
{
  TSTAT;
  
  if ( _device ) return _device->sync();
  else return 0;
}

int IO_Buffer::overflow( int c )
{
  TSTAT;
  
  uint4 pos = pptr() - _Tarr;
  _Tlen += 128;
  _Tarr = (char*) System::realloc( _Tarr, _Tlen );
  _Tarr[ pos ] = c;
  setp( _Tarr, _Tarr + _Tlen );
  pbump( pos + 1 );
  return 0;
}

//

void IO_Buffer::Tappend( ccptr mssg, uint4 len )
{
  TSTAT;
  
  Textend( len );
  memcpy( pptr(), mssg, len );
  Tupdate( len );
}

void IO_Buffer::Tremove( uint4 len )
{
  TSTAT;
  
  uint4 pos = pptr() - _Tarr;
  memmove( _Tarr, _Tarr + len, pos - len );
  setp( _Tarr, _Tarr + _Tlen );
  pbump( pos - len );  
}

void IO_Buffer::Textend( uint4 len )
{
  TSTAT;
  
  uint4 pos = pptr() - _Tarr;
  if ( pos + len > _Tlen ) {
    _Tlen = pos + len;
    _Tarr = (char*) System::realloc( _Tarr, _Tlen );
    setp( _Tarr, _Tarr + _Tlen );
    pbump( pos );
  }
}

void IO_Buffer::Tupdate( uint4 len )
{
  TSTAT;
  
  pbump( len );
}

void IO_Buffer::Zappend( ccptr mssg, uint4 len )
{
  TSTAT;
  
  Zextend( len );
  memcpy( _Zptr, mssg, len );
  Zupdate( len );
}

void IO_Buffer::Zremove( uint4 len )
{
  TSTAT;
  
  uint4 pos = _Zptr - _Zarr;
  memmove( _Zarr, _Zarr + len, pos - len );
  _Zptr -= len;
}

void IO_Buffer::Zextend( uint4 len )
{
  TSTAT;
  
  uint4 pos = _Zptr - _Zarr;
  if ( pos + len > _Zlen ) {
    _Zlen = pos + len;
    _Zarr = (char*) System::realloc( _Zarr, _Zlen );
    _Zptr = _Zarr + pos;
  }
}

void IO_Buffer::Zupdate( uint4 len )
{
  TSTAT;
  
  _Zptr += len;
}

//

void IO_Buffer::compress( bool Stat )
{
  TSTAT;
  
  uint4 dsize = pptr() - _Tarr; if ( dsize == 0 ) return;

  switch ( _method ) {
  case NONE  : return;
  case BZIP2 : while(   bzip2( Stat ) ); return; // _Tarr might be compressed in chunks
  case GZIP  : while( deflate( Stat ) ); return;
  default    : 
    vc_con << VCFL << "method(" << int(_method) << ")" << endl;
    System::exit(-1);
  }
}

void IO_Buffer::decompress( bool Stat )
{
  TSTAT;
  
  uint4 csize = _Zptr - _Zarr;
  if ( csize == 0 ) return;

  switch ( _method ) {
  case NONE  : return;
  case BZIP2 : while ( bunzip2( Stat ) ); return; // several blocks can be in _Zarr !!
  case GZIP  : while ( inflate( Stat ) ); return;
  default    : 
    vc_con << VCFL << "method(" << int(_method) << ")" << endl;
    System::exit(-1);
  }
}

// bzip2

bool IO_Buffer::bzip2( bool Stat )
{
  TSTAT;
  
  const uint4 bz_max = 59000; // fits into 2 bytes
  uint4 dsize = size( false );
  uint4 Dsize = ( dsize > bz_max ? bz_max : dsize );
  uint4 Csize = Dsize * 11/10 + 50 + sizeof(Csize) + sizeof(Dsize);
  uint4 Bsize = Csize;

  Zextend( Bsize );

  uint2 Csize2 = Csize;
  uint2 Dsize2 = Dsize;

  char* Zptr = _Zptr + sizeof(Csize2) + sizeof(Dsize2);

  const int bz_100k = 1;
  const int bz_verb = 0;
  const int bz_work = 30;
  int ok = BZ2_bzBuffToBuffCompress( Zptr, &Csize, _Tarr, Dsize, bz_100k, bz_verb, bz_work );

  if ( ok != BZ_OK ) {
    vc_con << VCFL;
    Form( vc_con, "bzip2( %p, %d, %p, %d ) = %d", Zptr, Csize, _Tarr, Dsize, ok ) << endl;
    System::exit(-1);
  }

  Csize2 = Csize;
  Dsize2 = Dsize;

  Bsize = Csize + sizeof(Csize2) + sizeof(Dsize2); // actual block size

  memcpy( _Zptr, &Csize2, sizeof(Csize2) );
  Zptr = _Zptr + sizeof(Csize2);
  memcpy( Zptr, &Dsize2, sizeof(Dsize2) );

  Tremove( Dsize );
  Zupdate( Bsize );

  if ( Stat ) vc_stat.io += CRatio( Dsize, Bsize );

  return size(false) != 0;
}

bool IO_Buffer::bunzip2( bool Stat )
{
  TSTAT;
  
  uint2 Csize2;
  uint2 Dsize2;
  uint4 Bsize = sizeof(Csize2) + sizeof(Dsize2);
  uint4 zsize = size( true );
  if ( zsize < Bsize ) return false; // insufficient data

  memcpy( &Csize2, _Zarr,                  sizeof(Csize2) );
  memcpy( &Dsize2, _Zarr + sizeof(Csize2), sizeof(Dsize2) );
  Bsize += Csize2;

  if ( zsize < Bsize ) return false; // incomplete block

  uint4 Csize = Csize2;
  uint4 Dsize = Dsize2;

  if ( Dsize > 65536 ) { // something is wrong, block can not be larger then 64K
    vc_con << VCFL << "Csize(" << Csize << ") Dsize(" << Dsize << ")" << endl;
    Zremove( Bsize ); // try to recover by skipping this block
    return true;
  }

  Textend( Dsize ); // enlarge buffer

  char* Zptr = _Zarr + sizeof(Csize2) + sizeof(Dsize2);
  int   ok   = BZ2_bzBuffToBuffDecompress( pptr(), &Dsize, Zptr, Csize, 0, 0 );

  if ( ok != BZ_OK ) { // we have a problem here ..
    vc_con << VCFL;
    Form( vc_con, "bunzip2( %p, %d, %p, %d ) = %d", pptr(), Dsize, _Zarr, Csize, ok ) << endl;
    System::exit(-1);
  }

  Tupdate( Dsize );
  Zremove( Bsize );

  if ( Stat ) vc_stat.io += CRatio( Dsize, Bsize );

  return true;
}

// gzip

bool IO_Buffer::deflate( bool Stat )
{
  TSTAT;
  
  const uint4 lzw_max = 64000; // fits into 2 bytes
  uint4 dsize = size( false );
  uint4 Dsize = ( dsize > lzw_max ? lzw_max : dsize );
  uint4 Csize = Dsize * 101/100 + 12 + sizeof(Csize) + sizeof(Dsize);
  uint4 Bsize = Csize;

  Zextend( Bsize );

  uint2 Csize2 = Csize;
  uint2 Dsize2 = Dsize;

  char* Zptr = _Zptr + sizeof(Csize2) + sizeof(Dsize2);

  const int lzw_level = 9;
  int ok = compress2( (Bytef*)Zptr, (uLongf*)&Csize, (const Bytef*)_Tarr, (uLong)Dsize, lzw_level );

  if ( ok != Z_OK ) {
    vc_con << VCFL;
    Form( vc_con, "compress2( %p, %d, %p, %d ) = %d", Zptr, Csize, _Tarr, Dsize, ok ) << endl;
    System::exit(-1);
  }

  Csize2 = Csize;
  Dsize2 = Dsize;

  Bsize = Csize + sizeof(Csize2) + sizeof(Dsize2); // actual block size

  memcpy( _Zptr, &Csize2, sizeof(Csize2) );
  Zptr = _Zptr + sizeof(Csize2);
  memcpy( Zptr, &Dsize2, sizeof(Dsize2) );

  Tremove( Dsize );
  Zupdate( Bsize );

  if ( Stat ) vc_stat.io += CRatio( Dsize, Bsize );

  return size(false) != 0;
}

bool IO_Buffer::inflate( bool Stat )
{
  TSTAT;
  
  uint2 Csize2;
  uint2 Dsize2;
  uint4 Bsize = sizeof(Csize2) + sizeof(Dsize2);
  uint4 zsize = size( true );
  if ( zsize < Bsize ) return false; // insufficient data

  memcpy( &Csize2, _Zarr,                  sizeof(Csize2) );
  memcpy( &Dsize2, _Zarr + sizeof(Csize2), sizeof(Dsize2) );
  Bsize += Csize2;

  if ( zsize < Bsize ) return false; // incomplete block

  uint4 Csize = Csize2;
  uint4 Dsize = Dsize2;

  if ( Dsize > 65536 ) { // something is wrong, block can not be larger then 64K
    vc_con << VCFL << "Csize(" << Csize << ") Dsize(" << Dsize << ")" << endl;
    Zremove( Bsize ); // try to recover by skipping this block
    return true;
  }

  Textend( Dsize ); // enlarge buffer

  char* Zptr = _Zarr + sizeof(Csize2) + sizeof(Dsize2);
  int   ok   = uncompress( (Bytef*) pptr(), (uLongf*) &Dsize, (const Bytef*) Zptr, (uLong) Csize );

  if ( ok != BZ_OK ) {
    vc_con << VCFL;
    Form( vc_con, "uncompress( %p, %d, %p, %d ) = %d", pptr(), Dsize, _Zarr, Csize, ok ) << endl;
    System::exit(-1);
  }

  Tupdate( Dsize );
  Zremove( Bsize );

  if ( Stat ) vc_stat.io += CRatio( Dsize, Bsize );

  return true;
}

//: IO_Buffer.C (eof) (c) Igor
