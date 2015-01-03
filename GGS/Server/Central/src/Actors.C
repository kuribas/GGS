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
#include "IO_FILE.H"
#include "IO_Server.H"
#include "DB_Server.H"
#include "TIME_Server.H"
#include "TSTAT_Server.H"
#include "EXE_Client.H"
#include "VAR_System.H"
#include "Message.H"
#include "SET_PTR_Mux.H"

const String Central_root_name  = "Igor Durdanovic";
const String Central_root_email = "igord@research.nj.nec.com";

const sint4  root_level = 3;
const sint4 admin_level = 2;
const sint4  user_level = 1;
const sint4 unreg_level = 0;

sint4  time_log   = 24*60*60;
sint4  time_db    = 24*60*60;
sint4  time_stat  =       60;
sint4  time_logon =     2*60;
ccptrc file_con   = "log/Central.con";
ccptrc file_log   = "log/Central.log";
ccptrc file_db    =  "db/Central.db";

const String login_system  = " s.y.s.t.e.m ";
const String login_root    = "root";

const String group_service = "_service"; // all service providers
const String group_client  = "_client";  // all clients
const String group_admin   = "_admin";   // admin group
const String chann_help    = ".help";    // help channel
const String chann_chat    = ".chat";    // chat channel

const String var_lrecv   = "_recv";      // who sent the last message we received
const String var_lsend   = "_send";      // to whom we sent the last message
const String var_prefix  = "_pfix";      // prefix all commands with ..
const String var_rsec    = "_rsec";      // repeat after sec
const String var_rcmd    = "_rcmd";      // repeat cmd 
const String var_ready   = "_ready";     // READY prompt
const String var_alert   = "_alert";     // ALERT prompt

const uint4 max_alias  = 64;     // max no of aliases   per user
const uint4 max_vars   = 64;     // max no of variables per user
const uint4 max_chann  = 64;     // max no of channels  per user
const uint4 max_ignore = 64;     // max no of ignoreds  per user
const uint4 max_notify = 64;     // max no of notifys   per user
const uint4 max_str_len = 1 << 10; // 1K

const String str_empty( "" );
const String str_plus ( "+" );
const String str_minus( "-" );
const String str_pct  ( "%" );
const String str_star ( "*" );
const String str_quest( "?" );
const String str_ready( "READY\r\n" );
const String str_alert( "ALERT\r\n" );

const uint4 warn_blocks = 1024 * 8;
const uint4 exit_blocks = 1024 * 2;
const uint4 warn_inodes = 1024 * 8;
const uint4 exit_inodes = 1024 * 2;
const uint4 warn_memory = 1024 * 1024 * 4;
const uint4 exit_memory = 1024 * 1024;
/* */ bool  srv_ready   = true;
/* */ bool  tstat_ready = false;
/* */ uint4 vc_trace    = 0;

bool vc_con_ready  = false; // destructors are called for static
bool vc_sig_ready  = false; // objects even if they were not created!
bool vc_time_ready = false;
bool vc_db_ready   = false;
bool vc_ios_ready  = false;
bool vc_log_ready  = false;
bool vc_var_ready  = false;
bool vc_exe_ready  = false;
bool vc_stat_ready = false;

TSTAT_Server   vc_tstat;
IO_FILE        vc_con( file_con, true,  false );
Signal         vc_sig;            // needs vc_con !!
TIME_Server    vc_time;           // needs vc_con, vc_log
DB_Server      vc_db ( file_db, IO_Buffer::NONE ); // needs vc_con, vc_log, vc_time
IO_Server      vc_ios;            // needs vc_sig, vc_con, vc_time, vc_mux
IO_FILE        vc_log( file_log, false, false, false, time_log ); // needs vc_ios
VAR_System     vc_var;            // needs vc_con, vc_db
EXE_Client     vc_exe;
Stat           vc_stat;           // needs vc_time
Message        vc_mssg;           // needs nothing
SET_PTR_Mux    vc_mux;            // needs nothing

//: Actors.C (eof) (c) Igor Durdanovic
