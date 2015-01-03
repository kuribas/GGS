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

class String;
class Signal;
class Stat;
class IO_FILE;
class IO_Server;
class DB_Server;
class TIME_Server;
class TSTAT_Server;
class SEQ_TCP_Client;
class EXE_Client;
class VAR_System;
class Message;
class SET_PTR_Mux;

extern const String Central_root_name;
extern const String Central_root_email;

extern const sint4  root_level;
extern const sint4 admin_level;
extern const sint4  user_level;
extern const sint4 unreg_level;

extern const String login_system;
extern const String login_root;

extern const String group_client;
extern const String group_admin;
extern const String group_service;
extern const String chann_help;
extern const String chann_chat;

extern const String var_lrecv;
extern const String var_lsend;
extern const String var_prefix;
extern const String var_rsec;
extern const String var_rcmd;
extern const String var_ready;
extern const String var_alert;

extern const uint4 max_alias;
extern const uint4 max_vars;
extern const uint4 max_chann;
extern const uint4 max_ignore;
extern const uint4 max_notify;
extern const uint4 max_str_len;

extern const String str_empty;
extern const String str_plus;
extern const String str_minus;
extern const String str_pct;
extern const String str_star;
extern const String str_quest;
extern const String str_ready;
extern const String str_alert;

extern ccptrc file_log;
extern sint4  time_log;
extern sint4  time_db;
extern sint4  time_stat;
extern sint4  time_logon;

extern const uint4 warn_blocks; // warn, disk space reaching low levels
extern const uint4 exit_blocks; // exit, no disk space left
extern const uint4 warn_inodes; // warn, file system reaching low levels
extern const uint4 exit_inodes; // exit, no file system resources left
extern const uint4 warn_memory; // warn if memory gets tight
extern const uint4 exit_memory; // exit if memory gets to tight
extern /* */ bool  srv_ready;   // "READY" | "ALERT"
extern       bool  tstat_ready; // collect TSTAT
extern       uint4 vc_trace;

extern bool vc_con_ready;
extern bool vc_sig_ready;
extern bool vc_time_ready;
extern bool vc_db_ready;
extern bool vc_ios_ready;
extern bool vc_log_ready;
extern bool vc_var_ready;
extern bool vc_exe_ready;
extern bool vc_stat_ready;

extern TSTAT_Server   vc_tstat;
extern IO_FILE        vc_con;
extern Signal         vc_sig;
extern TIME_Server    vc_time;
extern DB_Server      vc_db;
extern VAR_System     vc_var;
extern IO_Server      vc_ios;
extern IO_FILE        vc_log;
extern EXE_Client     vc_exe;
extern Stat           vc_stat;
extern Message        vc_mssg;
extern SET_PTR_Mux    vc_mux;

#endif

//: Actors.H (eof) (c) Igor
