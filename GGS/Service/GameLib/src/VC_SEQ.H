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
#include "VC_Match.H"

class Client;
class VC_SEQ
{ 
public:
   VC_SEQ( const SET_String& Seq ) : Set(Seq) {}
  ~VC_SEQ() {}

  typedef bool (*BellFun1)( const VC_Match& M, const Client* C );
  typedef bool (*BellFun2)( char C,            const Client* P );
  
  void mssg( const VC_Match& M, BellFun1 BF, VC_Match::MssgFun MF );
  void call( const VC_Match& M, /*        */ VC_Match::CallFun CF );

  void text( const Client* from, std::ostringstream& os, char C, BellFun2 BF );
  void text( const Client* from, std::ostringstream& os, const VC_Match& M, BellFun1 BF );

private:
  const SET_String& Set;
};

#endif

//: VC_SEQ.H (eof) (c) Igor
