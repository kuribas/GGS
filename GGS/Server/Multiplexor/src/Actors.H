// $Id: Actors.H 160 2007-06-22 15:21:10Z mburo $
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
//: Actors.H (bof) (c) Igor Durdanovic

#ifndef Actors_H
#define Actors_H

#include "CPP.H"

class Signal;
class String;
class Stat;
class IO;
class IO_Server;
class IO_FILE;
class TIME_Server;
class MSG_english;
class IO_TCP_Mux;

extern sint4  time_log;
extern sint4  time_stat;
extern ccptrc file_log;

extern const String empty;

extern const uint4 user_max_buf_len;
extern const uint4 file_max_buf_len;
extern const uint4 serv_max_buf_len;

extern bool vc_con_ready ;
extern bool vc_sig_ready ;
extern bool vc_time_ready;
extern bool vc_ios_ready ;
extern bool vc_log_ready ;
extern bool vc_stat_ready;

extern IO_FILE         vc_con;
extern Signal          vc_sig;
extern TIME_Server     vc_time;
extern IO_Server       vc_ios;
extern IO_FILE         vc_log;
extern Stat            vc_stat;
extern IO_TCP_Mux*     vc_mux;

#endif

//: Actors.H (eof) (c) Igor
