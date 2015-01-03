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
#include "VAR_Service.H"
#include "EXE_Service.H"
#include "Message.H"
#include "Client.H"
#include "Game.H"

const sint4  root_level = 3; // this must coresspond to Central definitions
const sint4 admin_level = 2;
const sint4  user_level = 1;
const sint4 unreg_level = 0;

sint4  time_log  = 24*60*60;
sint4  time_db   = 24*60*60;
sint4  time_stat =       60;
sint4  time_rank =    10*60;
sint4  time_save =    10*60;

const String file_con     = "log/" + std::string(RegularBoardGame::GAME_NAME) + ".con";
const String file_log     = "log/" + std::string(RegularBoardGame::GAME_NAME) + ".log";
const String file_db      = "db/"  + std::string(RegularBoardGame::GAME_NAME) + ".db";
const String file_save    = "db/"  + std::string(RegularBoardGame::GAME_NAME) + ".save";
const String file_archive = "db/"  + std::string(RegularBoardGame::GAME_NAME) + ".game";

const String group_td = "_td";

const String login_system  = RegularBoardGame::LOGIN_SYSTEM;
const String login_service = RegularBoardGame::LOGIN_SERVICE;
String passw_service; // set in main

const uint4 max_ignore = 64;
const uint4 max_notify = 64;
const uint4 max_track  = 64;
const uint4 max_str_len = 1 << 10; // 1K

const uint4 file_max_buf_len = 1 << 14; // 16K
const uint4 user_max_buf_len = 1 << 12; // 4K
const uint4 serv_max_buf_len = 1 << 20; // 1M

const String str_empty( "" );
const String str_plus ( "+" );
const String str_minus( "-" );
const String str_pct  ( "%" );
const String str_star ( "*" );
const String str_quest( "?" );

const uint4 warn_blocks = 1024 * 8;
const uint4 exit_blocks = 1024 * 2;
const uint4 warn_inodes = 1024 * 8;
const uint4 exit_inodes = 1024 * 2;
const uint4 warn_memory = 1024 * 1024 * 4;
const uint4 exit_memory = 1024 * 1024;
/* */ bool  svc_ready   = true; 
/* */ bool  tstat_ready = false;

bool vc_con_ready  = false; // destructors are called for static
bool vc_sig_ready  = false; // objects even if they were not created!
bool vc_time_ready = false;
bool vc_db_ready   = false;
bool vc_save_ready = false;
bool vc_ios_ready  = false;
bool vc_log_ready  = false;
bool vc_var_ready  = false;
bool vc_exe_ready  = false;
bool vc_stat_ready = false;

TSTAT_Server    vc_tstat;
IO_FILE         vc_con ( file_con.c_str(), true,  false );
Signal          vc_sig;               // needs vc_con !!
TIME_Server     vc_time;              // needs vc_con, vc_log
DB_Server       vc_db  ( file_db.c_str()  , IO_Buffer::NONE ); // needs vc_con, vc_log
DB_Server       vc_save( file_save.c_str(), IO_Buffer::NONE ); // needs vc_con, vc_log
IO_Server       vc_ios;               // needs vc_sig, vc_con; vc_time, vc_log, vc_var
IO_FILE         vc_log( file_log.c_str(), false, false, false, time_log ); // needs vc_ios
VAR_Service     vc_var;
EXE_Service     vc_exe;
Stat            vc_stat;
Message         vc_mssg;               // needs nothing

//: Actors.C (eof) (c) Igor
