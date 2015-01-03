// $Id: IO_FILE.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_FILE.H (bof) (c) Igor Durdanovic

#ifndef IO_FILE_H
#define IO_FILE_H

#include "IO.H"
#include "TIME_Client.H"

class IO_FILE
  : public IO,
    protected TIME_Client
{
public:
   IO_FILE( sint4 Desc, bool Sync, bool Alloc );
   IO_FILE( ccptr Path, bool Sync, bool Alloc, bool Append = false, sint4 Time = -1 );
  ~IO_FILE();

  virtual bool send();

  virtual void close( bool Err );

  virtual bool clog();

  virtual void          te_handle( sint4 Mssg, uint4 Time );
  virtual std::ostream& te_print( std::ostream& os ) const;

private:
  virtual bool recv() { return true; }
  virtual bool excp() { return true; }

  void ready() const;
  void open ();
  
  /* ctor */  IO_FILE( const IO_FILE& Client );
  IO_FILE& operator= ( const IO_FILE& Client );

  bool  app;
  ccptr path; // keep the base name
  sint4 time; // reopen time
};

#endif

//: IO_FILE.H (eof) (c) Igor
