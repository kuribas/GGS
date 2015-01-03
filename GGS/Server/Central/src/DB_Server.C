// $Id: DB_Server.C 9037 2010-07-06 04:05:44Z mburo $
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
//: DB_Server.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "String.H"
#include "Actors.H"
#include "Stat.H"
#include "IO.H"
#include "IO_FILE.H"
#include "DB_Server.H"
#include <cstdlib>
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

DB_Server::DB_Server( ccptr Name, IO_Buffer::Method Method ) 
  : name(Name), open(false), read(false), method(Method)
{
  vc_con << VCTIME << "  DB_Server( " << name << ", " << IO_Buffer::method_str( method ) << " )" << endl;

  open_put();

  te_create( 0, 10 ); // schedule first pack()
#if defined(SERVER)
  if ( this == &vc_db ) vc_db_ready = true;
#endif
#if defined(SERVICE)
  if ( this == &vc_db   ) vc_db_ready   = true;
  if ( this == &vc_save ) vc_save_ready = true;
#endif
}

DB_Server::~DB_Server() 
{
#if defined(SERVER)
  if ( this == &vc_db && ! vc_db_ready ) return;
#endif
#if defined(SERVICE)
  if ( this == &vc_db   && ! vc_db_ready   ) return;
  if ( this == &vc_save && ! vc_save_ready ) return;
#endif
  close();
  vc_con << VCTIME << " ~DB_Server( " << name << ", " << IO_Buffer::method_str( method ) << " )" << endl;
}

void DB_Server::te_handle( sint4 /*Mssg*/, uint4 /*Time*/ )
{
  TSTAT;

  te_cancel();
  
  pack();

  te_create( 0, time_db );
}

ostream& DB_Server::te_print( ostream& os ) const
{
  TSTAT;
  
  os << "DB_Server( " << name << ", " << IO_Buffer::method_str( method ) << " )";
  return os;
}

String DB_Server::datum2String( const datum& D ) 
{
  TSTAT;
  
  if ( D.dsize == 0 ) return str_empty;

  ostringstream ss;
  ss.write( D.dptr, D.dsize ); free( D.dptr );
  return ss.str();
  
  String S( D.dptr, D.dsize ); free( D.dptr );
  
  return S;
}

void DB_Server::String2datum( const String& S, datum& D ) 
{
  TSTAT;
  
  D.dsize = S.size();
  D.dptr  = (cptr) S.data();
}

void DB_Server::close() 
{
  TSTAT;
  
  if ( open ) {
    open = false;
    gdbm_close( dbf );
  }
}

void DB_Server::open_put() 
{
  TSTAT;
  
  if (  read ) close();
  if (! open ) {
    sint4 blok = 1024;
    sint4 mode = GDBM_WRCREAT;
    sint4 perm = S_IRUSR | S_IWUSR | S_IRGRP;
    dbf = gdbm_open( (cptr) name, blok, mode, perm, 0 );
    if ( dbf == 0 ) { 
      vc_con << VCFL
	     << "gdbm_open( " << name << ", " << blok << ", " << mode << ", " << perm << ", 0 )" << endl
	     << gdbm_strerror( gdbm_errno ) << endl;
      System::exit(-1);
    }
  }
  open = true;
  read = false;
}

void DB_Server::open_get() 
{
  TSTAT;
  
  if (! read ) close();
  if (! open ) {
    sint4 blok = 1024;
    sint4 mode = GDBM_READER;
    sint4 perm = S_IRUSR | S_IWUSR;
    dbf = gdbm_open( (cptr) name, blok, mode, perm, 0 );
    if ( dbf == 0 ) { 
      vc_con << VCFL
	     << "gdbm_open( " << name << ", " << blok << ", " << mode << ", " << perm << ", 0 )" << endl
	     << gdbm_strerror( gdbm_errno ) << endl;
      System::exit(-1);
    }
  }
  open = true;
  read = true;
}

CRatio DB_Server::get( const String& Key, String& ss ) 
{
  TSTAT;
  
  if ( Key.empty() ) { vc_con << VCFL; return 0; }

  open_get(); if (! open ) return false;

  datum dKey; String2datum( Key, dKey );
  datum dVal = gdbm_fetch( dbf, dKey );

  if ( dVal.dsize == 0 ) return false;

  sint4 txt = dVal.dsize;
  sint4 com = txt;
  
  if ( method != IO_Buffer::NONE ) {
    datum cVal; decompress( dVal, cVal ); free( dVal.dptr ); txt = cVal.dsize;
    ss = datum2String( cVal );
  } else {
    ss = datum2String( dVal );
  }

  return CRatio( txt, com );
}

CRatio DB_Server::put( const String& Key, const String& Val ) 
{
  TSTAT;
  
  datum dKey; String2datum( Key, dKey );
  datum dVal; String2datum( Val, dVal );
  return put( dKey, dVal );
}

CRatio DB_Server::put( const datum& Key, const datum& Val ) 
{
  TSTAT;
  
  if ( Key.dsize == 0 ) { vc_con << VCFL; return CRatio(0,0); }

  open_put(); if (! open ) return CRatio(0,0);

  sint4 flag = GDBM_REPLACE;
  sint4 ok   = 0;

  sint4 txt = Val.dsize;
  sint4 com = txt;
  
  if ( method != IO_Buffer::NONE ) {
    datum cVal; compress( Val, cVal ); com = cVal.dsize;
    ok = gdbm_store( dbf, Key, cVal, flag );
    free( cVal.dptr ); // we have to do free
  } else {
    ok = gdbm_store( dbf, Key, Val, flag );
  }
  
  if ( ok != 0 ) {
    vc_con << VCFL
	   << "gdbm_store( " << String(Key.dptr, Key.dsize) << ", " << String(Val.dptr, Val.dsize) << ", " << flag << " ) = " << ok << endl
	   << gdbm_strerror( gdbm_errno ) << endl;
    System::exit(-1);
  }
  
  return CRatio( txt, com );
}

void DB_Server::compress( const datum& Din, datum& Dout )
{
  TSTAT;
  
  IO_Buffer buf( method, 1 << 20, 0 );
  buf.append( false, Din.dptr, Din.dsize );
  buf.compress( false );
  Dout.dsize = buf.size( true );
  Dout.dptr  = (cptr) malloc( Dout.dsize );
  memcpy( Dout.dptr, buf.buff( true ), Dout.dsize );

  vc_stat.db += CRatio( Din.dsize, Dout.dsize );
}

void DB_Server::decompress( const datum& Din, datum& Dout )
{
  TSTAT;
  
  IO_Buffer buf( method, 1 << 20, 0 );
  buf.append( true, Din.dptr, Din.dsize );
  buf.decompress( false );
  Dout.dsize = buf.size( false );
  Dout.dptr  = (cptr) malloc( Dout.dsize );
  memcpy( Dout.dptr, buf.buff( false ), Dout.dsize );

  vc_stat.db += CRatio( Dout.dsize, Din.dsize );
}

void DB_Server::del( const String& Key ) 
{
  TSTAT;
  
  open_put();
  datum dKey;
  String2datum( Key, dKey );
  sint4 ok = gdbm_delete( dbf, dKey );
  if ( ok != 0 && gdbm_errno != GDBM_ITEM_NOT_FOUND ) {
    vc_con << VCFL
	   << "gdbm_delete( " << Key << " ) = " << ok << endl
	   << gdbm_strerror( gdbm_errno ) << endl;
    System::exit(-1);
  }
}

void DB_Server::print( ostream& os, DB_PRINT db_print )
{
  TSTAT;
  
  open_get();
  
  datum fKey = gdbm_firstkey( dbf );
  for ( ; fKey.dptr != 0; ) {
    datum nKey = gdbm_nextkey ( dbf, fKey );
    String Key( datum2String( fKey ) );
    db_print( os, Key );
    fKey = nKey;
  }
}

void DB_Server::format( DB_Server& db, DB_FORMAT db_format )
{
  TSTAT;
  
  open_get();
  
  datum fKey = gdbm_firstkey( dbf );
  for ( ; fKey.dptr != 0; ) {
    datum nKey = gdbm_nextkey ( dbf, fKey );
    String Key( datum2String( fKey ) );
    db_format( db, Key );
    fKey = nKey;
  }
}

void DB_Server::iterate( DB_Key_Handler& kh )
{
  TSTAT;
  
  open_get();
  
  datum fKey = gdbm_firstkey( dbf );
  for ( ; fKey.dptr != 0; ) {
    datum nKey = gdbm_nextkey ( dbf, fKey );
    String Key( datum2String( fKey ) );
    kh.handle( Key );
    fKey = nKey;
  }
}

void DB_Server::pack()
{
  TSTAT;
  
  vc_log << VCTIME << " DB(" << name << ").pack()" << endl; 

  open_put();
  sint4 ok = gdbm_reorganize( dbf );
  if ( ok != 0 ) {
    vc_con << VCFL
	   << "gdbm_reorganize( ... ) = " << ok << endl
	   << gdbm_strerror( gdbm_errno ) << endl;
    System::exit(-1);
  }
}

//: DB_Server.C (eof) (c) Igor
