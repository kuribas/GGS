// $Id: TIME_Client.H 160 2007-06-22 15:21:10Z mburo $
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
//: TIME_Client.H (bof) (c) Igor Durdanovic

#ifndef TIME_Client_H
#define TIME_Client_H

#include "CPP.H"
#include <iostream>

class TIME_Client
{
public:
  virtual ~TIME_Client() { te_cancel(); }
  
  /*   */ uint4         te_create( sint4 Mssg, uint4 Time, bool ABS = false );
  /*   */ void          te_cancel( sint4 Mssg );
  /*   */ void          te_cancel();
  virtual void          te_handle( sint4 Mssg, uint4 Time ) = 0;
  virtual std::ostream& te_print ( std::ostream& os ) const = 0;

protected:
  uint4 clock;
};

#endif

//: TIME_Client.H (eof) (c) Igor
