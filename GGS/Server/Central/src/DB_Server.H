// $Id: DB_Server.H 160 2007-06-22 15:21:10Z mburo $
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
//: DB_Server.H (bof) (c) Igor Durdanovic

#ifndef DB_Server_H
#define DB_Server_H

#include "CRatio.H"
#include "IO_Buffer.H"
#include "TIME_Client.H"
#include <gdbm.h>

class String;
class DB_Server;

typedef void (*DB_PRINT)  ( std::ostream& os, const String& key );
typedef void (*DB_FORMAT) ( DB_Server&    db, const String& key );

class DB_Key_Handler // true generic DB iterating interface
{                    // TODO: rewrite obsoletes using new interface
public:
  virtual void handle( const String& key ) = 0; // call-back for each key
  virtual ~DB_Key_Handler() {}
};

class DB_Server
  : public TIME_Client
{
public:
  DB_Server( ccptr Name, IO_Buffer::Method Method );
  virtual ~DB_Server();

  CRatio get( const String& Key, /* */ String& Val ); // how many bytes read
  CRatio put( const String& Key, const String& Val ); // how many bytes wrote
  
  void  del( const String& Key );
	   
  void pack();
  void print  ( std::ostream&   os, DB_PRINT  db_print  ); // obsolete
  void format ( DB_Server& db, DB_FORMAT db_format  );     // obsolete
  void iterate( DB_Key_Handler& kh );                      // read only!

  virtual void          te_handle( sint4 Mssg, uint4 Time ); // schedule pack
  virtual std::ostream& te_print ( std::ostream& os ) const;
  
private:
  static void   String2datum( const String& S, datum&  D ); // will share
  static String datum2String( const datum&  D ); // will deallocate

  CRatio put( const String& Key, const std::ostringstream& Val );
  CRatio put( const datum& dKey, const datum& dVal );

  void   compress( const datum& Din, datum& Dout );
  void decompress( const datum& Din, datum& Dout );
		 
  void close();
  void open_get();
  void open_put();

  ccptr    name;
  bool     open;
  bool     read;
  GDBM_FILE dbf;
  
  IO_Buffer::Method method;
};

#endif

//: DB_Server.H (eof) (c) Igor
