// $Id: VC_SEQ.H 160 2007-06-22 15:21:10Z mburo $
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
//: VC_SEQ.H (bof) (c) Igor Durdanovic

#ifndef VC_SEQ_H
#define VC_SEQ_H

#include "SET_Group.H"

class IO_TCP_Client;

class VC_SEQ
{ 
public:
  enum TYPE { CHANN, GROUP };
  
  VC_SEQ( const String& Name, TYPE T );
  ~VC_SEQ() {}

  bool ok() const { return g != 0; }
  
  void text( const IO_TCP_Client* from,
	     const String&        name,
	     std::ostringstream&  osML,
	     std::ostringstream&  os1L );

private:
  TYPE   t;
  Group* g;
};

#endif

//: VC_SEQ.H (eof) (c) Igor
