// $Id: VC_History.H 160 2007-06-22 15:21:10Z mburo $
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
//: VC_History.H (bof) (c) Igor Durdanovic

#ifndef VC_History_H
#define VC_History_H

#include "VEC.H"
#include "String.H"
#include <iostream>

class VAR_Client;

class History
{
private:  
  String login;
  sint4  admin;
  sint4  arrived;
  sint4  departed;
  String host_ip;
  String host_name;

public:
  enum HMODE { SYSTEM, CLIENT };

  History() : admin(0), arrived(0), departed(0) {}
  History( const String& Login, 
	   sint4 Admin, sint4 Arrived, sint4 Departed,
	   const String& Host_IP, const String& Host_Name ) 
    :
    login( Login ),
    admin( Admin ), arrived( Arrived ), departed( Departed ),
    host_ip( Host_IP ),
    host_name( Host_Name ) {}

  std::ostream& print( std::ostream& os, HMODE M, const VAR_Client& VP ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  void erase();
};

class VC_History
{
public:
  VC_History( uint4 Len ) : len( Len ) {}

  void  erase() { vec.erase(); }
  uint4 size () const { return vec.size(); }

  VC_History& push( const History& H );

  std::ostream& print( std::ostream& os, History::HMODE M, const VAR_Client& VP ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

private:
  static const String tag;
  
  uint4        len;
  VEC<History> vec;
};

#endif

//: VC_History.H (eof) (c) Igor
