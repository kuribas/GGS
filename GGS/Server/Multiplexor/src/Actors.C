// $Id: Actors.C 160 2007-06-22 15:21:10Z mburo $
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
//: Actors.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "Signal.H"
#include "Stat.H"
#include "IO_Server.H"
#include "IO_FILE.H"
#include "TIME_Server.H"
#include "IO_TCP_Mux.H"

sint4  time_log  = 24*60*60;
sint4  time_stat =       60;
ccptrc file_log  = "log/Multiplexer.log";

const uint4 file_max_buf_len = 1 << 14; // 16K
const uint4 user_max_buf_len = 1 << 12; // 4K
const uint4 serv_max_buf_len = 1 << 20; // 1M

const String empty( "" );

bool vc_con_ready  = false;
bool vc_sig_ready  = false;
bool vc_time_ready = false;
bool vc_ios_ready  = false;
bool vc_log_ready  = false;
bool vc_stat_ready = false;

IO_FILE        vc_con( 1, true,  false );
Signal         vc_sig;            // needs vc_con !!
TIME_Server    vc_time;           // needs vc_con, vc_log
IO_Server      vc_ios;            // needs vc_sig, vc_con; vc_time, vc_log, vc_var
IO_FILE        vc_log( file_log, false, false, false, time_log ); // needs vc_ios
Stat           vc_stat;           // needs vc_time
IO_TCP_Mux*    vc_mux( 0 );

//: Actors.C (eof) (c) Igor
