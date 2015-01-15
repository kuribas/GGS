// $Id: EXE_Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client.C (bof) (c) Igor Durdanovic

#include "HHMMSS.H"
#include "USEC.H"
#include "System.H"
#include "Actors.H"
#include "Signal.H"
#include "Stat.H"
#include "String.H"
#include "DB_Server.H"
#include "TIME_Server.H"
#include "IO_FILE.H"
#include "IO_TCP_Client.H"
#include "IO_TCP_Mux.H"
#include "Histogram_HDMY.H"
#include "VAR_System.H"
#include "VAR_Client.H"
#include "Message.H"
#include "VC_SEQ.H"
#include "SET_PTR_Mux.H"
#include "EXE_Client.H"
#include "VT100.H"
#include <iomanip>

#include "TSTAT_Server.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

const sint4 recursion_level = 4;

EXE_Client::EXE_Client() 
{
  add( ":",        &EXE_Client::dummy );
  add( ";",        &EXE_Client::dummy );

  add( "alias",    &EXE_Client::alias );
  add( "var",      &EXE_Client::var );
  add( "bell",     &EXE_Client::bell );
  add( "vt100",    &EXE_Client::vt100 );
  add( "hear",     &EXE_Client::hear );
  add( "verbose",  &EXE_Client::verbose );
  add( "info",     &EXE_Client::info );
  add( "passw",    &EXE_Client::passw );

  add( "who",      &EXE_Client::who );
  add( "finger" ,  &EXE_Client::finger );
  add( "history",  &EXE_Client::history );

  add( "echo",     &EXE_Client::echo );
  add( "tell",     &EXE_Client::tell );
  add( "quit",     &EXE_Client::quit );

  add( "chann",    &EXE_Client::chann );
  add( "group",    &EXE_Client::group );
  add( "notify" ,  &EXE_Client::notify );
  add( "ignore" ,  &EXE_Client::ignore );

  add( "repeat" ,  &EXE_Client::repeat );

  add( "help",     &EXE_Client::help );

  add( "name",     &EXE_Client::name );
  add( "email",    &EXE_Client::email );

  add( "stat_user",&EXE_Client::histo_usr );
  add( "stat_inp", &EXE_Client::histo_inp );
  add( "stat_out", &EXE_Client::histo_out );

  add( "uptime",   &EXE_Client::uptime );
  add( "salias",   &EXE_Client::salias );
  add( "shistory", &EXE_Client::shistory );
  add( "down",     &EXE_Client::down );
  add( "mux",      &EXE_Client::mux );
  add( "pack",     &EXE_Client::pack );
  add( "exec",     &EXE_Client::exec );
  add( "tstat",    &EXE_Client::tstat );
  add( "trace",    &EXE_Client::trace );
  add( "tevent",   &EXE_Client::tevent );

  vc_con << VCTIME << "  EXE_Client( " << cmds.size() << " )" << endl;

  if ( this == &vc_exe ) vc_exe_ready = true;
}

EXE_Client::~EXE_Client() 
{
  if ( this == &vc_exe && ! vc_exe_ready ) return;

  vc_con << VCTIME << " ~EXE_Client( " << cmds.size() << " )" << endl;
}

void EXE_Client::add( ccptr Name, Command Proc )
{
  cmds += EXE_Cmd( Name, Proc );
}

void EXE_Client::shell_execute( IO_TCP_Client& who, String& line, bool Prefix )
{
  TSTAT;
  
  if ( Prefix ) {
    String _prefix = who.var.vars.def( var_prefix );
    if (! _prefix.empty() ) {
      shell_replace( who, _prefix, recursion_level );
      who.var.alias.replace_arg( _prefix, str_empty );
      line = _prefix + ' ' + line;
    }
  }

  String orig( line );
  String cmd;
  String arg;
  String::parse( line, cmd, arg );
  if ( cmd != "alias" && cmd != "salias" ) {
    who.var.vars.replace_var( line );
    String::parse( line, cmd, arg );
  }

  if (! cmd.empty() ) {
    if ( vc_trace > 0 && ( vc_trace > 1 || who.var.groups( group_service ) == 0 ) )
      vc_log << who.id() << ' ' << cmd << ' ' << arg << endl;

    const EXE_Cmd* p = cmds( cmd );

    if ( p ) (this->*(p->obj))( who, arg ); else vc_mssg._010( who.vt100(), who, cmd );
    
    if ( who.var.verbose.ack() )
      who.vt100( vt_start, ": +ack: " ) << ' ' << orig << " => " << line << EOM;
  }
  who.ready();
}

void EXE_Client::shell_replace( IO_TCP_Client& who, String& line, sint4 no_rec )
{
  TSTAT;
  
  if ( no_rec <= 0 ) return;
  
  String cmd;
  String arg;
  String::parse( line, cmd, arg );
  bool ok = who.var.alias.replace_alias( cmd, &vc_var.alias );
  if (! ok ) return;

  line = cmd + ' ' + arg;
  shell_replace( who, line, no_rec - 1 );
}

void EXE_Client::shell_expand ( IO_TCP_Client& who, String& line,
				sint4& no_cmd, sint4 no_rec, bool Prefix )
{
  TSTAT;
  
  String cmd;
  String arg;
  String::parse( line, cmd, arg );
  if ( cmd == ":" || cmd == ";" ) {
    shell_expand( who, arg, no_cmd, no_rec, false );
    return;
  }
  if ( cmd == "tell" ) {
    shell_execute( who, line, false ); --no_cmd;
    return;
  }
  bool ok = who.var.alias.replace_alias( cmd, &vc_var.alias );
  if (! ok ) {
    shell_execute( who, line, Prefix ); --no_cmd;
    return;
  }

  VEC<String> stack; String::parse( cmd, stack, ';' );
  
  for ( uint4 i = 0; i < stack.size() && no_cmd > 0; ++i ) {
    who.var.alias.replace_arg( stack[i], arg );
    if ( no_rec > 0 ) {
      shell_expand ( who, stack[i], no_cmd, no_rec - 1, Prefix );
    } else {
      shell_execute( who, stack[i], Prefix ); --no_cmd;
    }
  }
}

void EXE_Client::operator () ( IO_TCP_Client& who, String& text )
{
  TSTAT;
  
  sint4 no_cmd = 64;
  shell_expand( who, text, no_cmd, recursion_level, true );
  
  who.sync();       // send whatever was written in the buffer
  who.var.save();   // save player's vars
  vc_var.save();    // save system's vars
}

// alias [[?]pattern [replacement]] | [!login]

void EXE_Client::alias( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "alias" )
      << ' ' << who.var.alias.save_size()
      << '/' << who.var.alias.size() << who.var.alias << EOM;
    return; 
  }
  if ( arg[0] == '?' ) {
    String Prefix( arg.data() + 1, arg.size() - 1 );
    who << ": ";
    who.vt100( vt_start, "alias" );
    who.var.alias.print( who, Prefix );
    who << EOM;
    return; 
  }
  if ( arg[0] == '!' && arg.size() > 2 ) {
    String name( arg.data()+1, arg.size() - 1 );
    VAR_Client* V = vc_var.var( name );
    if ( V == 0 ) { vc_mssg._016( who.vt100(), who, name ); return; }
    who << ": ";
    who.vt100( vt_start, "alias" )
      << ' ' << name
      << ' ' << V->alias.save_size()
      << '/' << V->alias.size() << V->alias << EOM;
    return; 
  }

  String name;
  String rest;
  String::parse( arg, name, rest );
  if (! rest.empty() && ! Alias::alias_name_ok( name ) )
    { vc_mssg._032( who.vt100(), who, name ); return; }
    
  Alias A( name, rest );
  const NAMED<Command>* p = cmds( A );
  if (! rest.empty() && p != 0 )
    { vc_mssg._049( who.vt100(), who ); return; }
  if ( rest.size() > max_str_len ) { vc_mssg._011( who.vt100(), who ); return; }

  who.var.alias = A;
  who.var.modified();

  if ( who.var.alias.save_size() <= max_alias     ) return;
  if ( who.var.alias.size()      <= max_alias * 2 ) return;
  who.var.alias -= A;
  vc_mssg._012( who.vt100(), who, max_alias );
}

// var [[?]name [definition]]

void EXE_Client::var( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "var" ) << ' ' << who.var.vars.size() << who.var.vars << EOM;
    return; 
  }
  if ( arg[0] == '?' && arg.size() > 2 ) {
    String Prefix( arg.data() + 1, arg.size() - 1 );
    who << ": ";
    who.vt100( vt_start, "var" );
    who.var.vars.print( who, Prefix );
    who << EOM;
    return; 
  }
  if ( arg[0] == '!' && arg.size() > 2 ) {
    String name( arg.data()+1, arg.size() - 1 );
    VAR_Client* V = vc_var.var( name );
    if ( V == 0 ) { vc_mssg._016( who.vt100(), who, name ); return; }
    who << ": ";
    who.vt100( vt_start, "var" )
      << ' ' << name
      << ' ' << V->vars.size() << V->vars << EOM;
    return; 
  }

  String name;
  String rest;
  String::parse( arg, name, rest );
  if (! Alias::var_name_ok( name ) ) { vc_mssg._059( who.vt100(), who, name ); return; }
    
  Alias A( name, rest );
  if ( A.def().size() > max_str_len ) { vc_mssg._011( who.vt100(), who ); return; }

  who.var.vars = A;
  who.var.modified();

  if ( who.var.vars.size() <= max_vars ) return;
  who.var.vars -= A;
  vc_mssg._012( who.vt100(), who, max_vars );
}

// bell (+|-)(t|tc|tg|n|nc|ng|ni|nn)

void EXE_Client::bell( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  bool ok = !arg.empty() && who.var.bell.parse( arg );
  if ( ok ) who.var.modified();
  else vc_mssg._033( who.vt100(), who );
}

// verbose (+|-)(news|ack|help|faq)

void EXE_Client::verbose( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  bool ok = !arg.empty() && who.var.verbose.parse( arg );
  if ( ok ) who.var.modified();
  else vc_mssg._048( who.vt100(), who );
}

// vt100 [+|-]

void EXE_Client::vt100( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  bool vt100 = false;
  /**/ if ( arg.empty()) vt100 = ! who.vt100();
  else if ( arg == "+" ) vt100 = true;
  else if ( arg == "-" ) vt100 = false;
  else { vc_mssg._013( who.vt100(), who, "vt100" ); return; }

  who.var.vt100 = vt100;
  who.var.modified();
}

// hear [+|-]

void EXE_Client::hear( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  bool hear = false;
  /**/ if ( arg.empty()) hear = ! who.var.hear;
  else if ( arg == "+" ) hear = true;
  else if ( arg == "-" ) hear = false;
  else { vc_mssg._013( who.vt100(), who, "hear" ); return; }

  who.var.hear = hear;
  who.var.modified();
}

// info <info>

void EXE_Client::info( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.size() > max_str_len ) { vc_mssg._011( who.vt100(), who ); return; }

  who.var.info = arg;
  who.var.info.replace( NEW_LINE, "\ninfo   : " ); // to make parsing easier ..
  who.var.modified();
}

// passw <new> <old> [login]

void EXE_Client::passw( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  VEC<String> alist;
  String::parse( arg, alist );
  if ( alist.size() < 2 || alist.size() > 3 ) { vc_mssg._015( who.vt100(), who ); return; }

  if ( alist.size() == 3 ) {
    VAR_Client* vp = vc_var.var( alist[2] );
    if ( vp == 0 ) { vc_mssg._016( who.vt100(), who, alist[2] ); return; }

    if ( who.id()       != vp->login   &&
	 who.var.admin() < vp->admin() &&
	 who.var.admin() < admin_level ) {
      vc_mssg._017( who.vt100(), who );
      return;
    }

    if ( vp->passw != alist[1] ) { vc_mssg._005( who.vt100(), who ); return; }

    vp->passw = alist[0];
    vp->modified();

    return;
  }

  if ( who.var.passw != alist[1] ) { vc_mssg._005( who.vt100(), who ); return; }

  who.var.passw = alist[0];
  who.var.modified();
}

// who

void EXE_Client::who( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if (! arg.empty() ) { vc_mssg._039( who.vt100(), who ); return; }

  who << ": ";
  who.vt100( vt_start, "who" )
    << setw(5) << vc_var.clients.size()
    << "        idle      online         ip-addr hostname"
    << EOL;

  SET_PTR_Client::const_iterator it = vc_var.clients.begin();
  SET_PTR_Client::const_iterator hi = vc_var.clients.end();
  for ( ; it != hi; ++it ) {
    bool me = (*it)->id() == who.id();
    bool vt100 =  me && who.vt100();
    if ( vt100 ) who << vt_self;
    Form( who, "%-8s %c", (*it)->id().c_str(), (*it)->var.admin() < user_level ? '-' : '+' );
    who << setw(12) << HHMMSS( System::clock() - (*it)->var.last )
	<< setw(12) << HHMMSS( System::clock() - (*it)->var.time );
    if ( &who == &**it ||
	 who.var.admin() > (*it)->var.admin() && who.var.admin() > user_level ) {
      who << setw(16) << (*it)->var.host_ip << ' ' << (*it)->var.host_name;
    }
    if ( me ) who << " <=";
    if ( vt100 ) who << vt_reset;
    who << EOL;
  }
}

// finger [login1 login2 ..]

void EXE_Client::finger( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "finger" ) << EOL;
    who.var.print( who, who.var ) << EOM;
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    VAR_Client* vp = vc_var.var( *it );
    if ( vp == 0 ) { Err.push_back( *it ); continue; }
    who << ": ";
    who.vt100( vt_start, "finger" ) << EOL;
    vp->print( who, who.var );
    who << EOL;
  }
  if (! Err.empty() ) {
    who << ": ";
    who.vt100( vt_start, "finger" ) << ' ';
    who.vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}


// history [login1 login2 ..]

void EXE_Client::history( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "history" ) << ' ' << who.id() << ' ' << who.var.history.size() << EOL;
    if ( who.var.history.size() > 0 )
      who.var.history.print( who, History::CLIENT, who.var ) << EOM;
    return;
  }

  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    VAR_Client* vp = vc_var.var( *it );
    if ( vp == 0 ) { Err += *it; continue; }
    who << ": ";
    who.vt100( vt_start, "history" ) << ' ' << vp->login << ' ' << vp->history.size() << EOL;
    if ( vp->history.size() > 0 ) {
      vp->history.print( who, History::CLIENT, who.var ) << EOL;
    }
  }
  if (! Err.empty() ) {
    who << ": ";
    who.vt100( vt_start, "history" ) << ' ';
    who.vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}

// tell <who1,who2,..> [mssg] ; <who> ::= <.chann> | <_group> | <login>

void EXE_Client::tell( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  String to_seq;
  String msg;
  String::parse( arg, to_seq, msg, ' ' );

  if ( to_seq.empty() ) { vc_mssg._018( who.vt100(), who ); return; }

  IO_TCP_Client* C = vc_var.client( to_seq );
  bool clobber = ( C == 0 || C->var.groups( group_service ) == 0 );

  if ( clobber ) {
    who.var.vars = Alias( var_lsend + ' ' + to_seq );
    tell_send( who, msg );
  } else {
    String old_lsend = who.var.vars.def( var_lsend );
    who.var.vars = Alias( var_lsend + ' ' + to_seq );
    tell_send( who, msg );
    who.var.vars = Alias( var_lsend + ' ' + old_lsend );
  }
}

void EXE_Client::tell_send( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  const String& _lsend = who.var.vars.def( var_lsend );
  if ( _lsend.empty() ) { vc_mssg._018( who.vt100(), who ); return; }

  SET_String Set; String::parse( _lsend, Set, ',' );
  
  String mssgML( arg );
  String mssg1L( arg );
  mssgML.replace( NEW_LINE, FIX_LINE );
  mssg1L.replace( NEW_LINE, ONE_LINE );
  ostringstream osML; osML << who.id() << ": " << mssgML << EOL;
  ostringstream os1L; os1L << who.id() << ": " << mssg1L << EOL;

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    if ( (*it)[0] == '.' ) {
      if (! who.var.channs( *it ) ) { vc_mssg._019( who.vt100(), who, *it ); continue; }
      VC_SEQ seq( *it, VC_SEQ::CHANN );
      if (! seq.ok() ) { vc_mssg._020( who.vt100(), who, *it ); continue; }
      seq.text( &who, *it, osML, os1L );
      continue;
    }

    if ( (*it)[0] == '_' ) {
      if ( who.var.admin() < admin_level ) { vc_mssg._021( who.vt100(), who ); continue; }
      VC_SEQ seq( *it, VC_SEQ::GROUP );
      if (! seq.ok() ) { vc_mssg._022( who.vt100(), who, *it ); continue; }
      seq.text( &who, *it, osML, os1L );
      continue;
    }

    IO_TCP_Client* P = vc_var.client( *it );
    if ( P == 0 ) { vc_mssg._016( who.vt100(), who, *it ); continue; }
    // if ( P == &who ) continue; // talking to yourself silly
    if ( P->ignore( &who ) ) { vc_mssg._042( who.vt100(), who, P->id() ); continue; } // ignore
    
    P->interrupt();
    if ( who.var.groups( group_service ) == 0 ) P->bell( P->var.bell.tell() );
    P->text( P->var.groups( group_service ) != 0 ? os1L : osML );
    P->ready();
    P->sync();
    if ( who.var.groups( group_service ) == 0 ) P->var.vars = Alias( var_lrecv, who.id() );
  }
}

// echo [mssg]

void EXE_Client::echo( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  String mssgML( arg );
  String mssg1L( arg );

  mssgML.replace( NEW_LINE, FIX_LINE );
  mssg1L.replace( NEW_LINE, ONE_LINE );

  ostringstream osML; osML << ": " << mssgML << EOL;
  ostringstream os1L; os1L << ": " << mssg1L << EOL;

  who.interrupt();
  who.text( who.var.groups( group_service ) != 0 ? os1L : osML );
  who.ready();
  who.sync();
}

// quit [login] [passw]

void EXE_Client::quit( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  String user;
  String pass;
  String::parse( arg, user, pass );

  if ( user.empty() ) {
    vc_mssg._006( who.vt100(), who );
    who.te_cancel();
    who.te_create( 0, 0 );
    return;
  }

  IO_TCP_Client* ip = vc_var.client( user );
  if ( ip == 0 ) { vc_mssg._016( who.vt100(), who, user ); return; }

  bool ok = who.id() == ip->id();
  ok = ok || ( who.var.admin() > ip->var.admin() && who.var.admin() > user_level );
  ok = ok || ( pass == ip->var.passw );
  if ( ! ok ) { vc_mssg._023( who.vt100(), who ); return; }

  if ( ip->var.groups( group_service ) == 0 ) {
    vc_mssg._024( ip->var.vt100, *ip, who.id() ); 
    vc_mssg._006( false, *ip );
    ip->sync();
  }

  ip->te_cancel();
  ip->te_create( 0, 0 );
}

// repeat [seconds [command]]

void EXE_Client::repeat( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) { who.te_cancel( 1 ); return; }

  if ( who.var.admin() < user_level ) { vc_mssg._031( who.vt100(), who, "you" ); return; }
  
  String str;
  String cmd;
  String::parse( arg, str, cmd );

  sint4 sec = atoi( str.c_str() );
  if ( sec <= 0 ) { who.te_cancel( 1 ); return; }
  who.var.vars = Alias( var_rsec + ' ' + str );
  if ( cmd.empty() ) {
    if ( who.var.vars.def( var_rcmd ).empty() ) { who.te_cancel( 1 ); return; }
  } else {
    who.var.vars = Alias( var_rcmd + ' ' + cmd );
  }
	who.te_cancel( 1 );
  who.te_create( 1, sec );
}

// help <file> .. <file>

void EXE_Client::help( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  SET_String Set; String::parse( arg, Set );
  SET_String Err;
  
  if ( Set.size() == 0 ) {
    who << ": ";
    who.vt100( vt_start, "help" ) << " help" << EOL;
    who.mssg( "msg/help" );
    return;
  }

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();

  for ( ; it != hi ; ++it ) {
    String File = "msg/" + *it;
    if (!System::file_legal( File.c_str() ) ) { Err += *it; continue; }
    if ( System::file_exist( File.c_str() ) ) {
      who << ": ";
      who.vt100( vt_start, "help" ) << ' ' << *it << EOL;
      who.mssg( File );
      continue;
    }
    Err += *it;
  }
  if (! Err.empty() ) {
    who << ": ";
    who.vt100( vt_start, "help" ) << ' ';
    who.vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
  }
}

// name <user> [name]

void EXE_Client::name( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  String user;
  String name;
  String::parse( arg, user, name );
  if ( user.empty() ) { vc_mssg._035( who.vt100(), who ); return; }

  VAR_Client* vp = vc_var.var( user );
  if ( vp == 0 ) { vc_mssg._016( who.vt100(), who, user ); return; }

  if ( vp->passw.empty() ) { vc_mssg._058( who.vt100(), who ); return; }

  if ( ( who.var.admin() <  admin_level ) ||
       ( who.var.admin() <  vp->admin() ) ||
       ( who.var.admin() == vp->admin() && who.id() != vp->login ) ) {
    vc_mssg._017( who.vt100(), who ); 
    return;
  }

  vp->name = name;
  vp->modified();
}

// email <login> [email]

void EXE_Client::email( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  String user;
  String email;
  String::parse( arg, user, email );
  if ( user.empty() ) { vc_mssg._036( who.vt100(), who ); return; }

  VAR_Client* vp = vc_var.var( user );
  if ( vp == 0 ) { vc_mssg._016( who.vt100(), who, user ); return; }

  if ( vp->name.empty() ) { vc_mssg._037( who.vt100(), who ); return; }

  if ( ( who.var.admin() <  admin_level ) ||
       ( who.var.admin() <  vp->admin() ) ||
       ( who.var.admin() == vp->admin() && who.id() != vp->login ) ) {
    vc_mssg._017( who.vt100(), who ); 
    return;
  }
  if (! email.empty() || who.var.admin() == root_level ) {
    vp->email = email;
    vp->modified();
    IO_TCP_Client* ip = vc_var.client( vp->login );
    if ( ip ) send_service_notify( *ip, true ); // notify only if client present
  }
}

// histo_X <h|d|m|y>

void EXE_Client::histo_inp( IO_TCP_Client& who, const String& arg )
{
  TSTAT;

  histo( who, arg, vc_var.histo_inp_bytes, "#input " );
}

void EXE_Client::histo_out( IO_TCP_Client& who, const String& arg )
{
  TSTAT;

  histo( who, arg, vc_var.histo_out_bytes, "#output " );
}

void EXE_Client::histo_usr( IO_TCP_Client& who, const String& arg )
{
  TSTAT;

  histo( who, arg, vc_var.histo_users, "#user " );
}

void EXE_Client::histo( IO_TCP_Client& who, const String& arg, const Histogram_HDMY& H, ccptr Title )
{
  TSTAT;

  if ( arg.empty() || ( arg != "h" && arg != "d" && arg != "m" && arg != "y" ) ) {
    vc_mssg._060( who.vt100(), who, arg );
    return;
  }

  who << ": ";
  who.vt100( vt_start, Title );
  
  if ( arg == "h" ) {
    who.vt100( vt_start, "stat[hour]" );
    H.print( who, Histogram_HDMY::HOURLY );
    return;
  }
  if ( arg == "d" ) {
    who.vt100( vt_start, "stat[day]" );
    H.print( who, Histogram_HDMY::DAILY );
    return;
  }
  if ( arg == "m" ) {
    who.vt100( vt_start, "stat[month]" );
    H.print( who, Histogram_HDMY::MONTHLY );
    return;
  }
  if ( arg == "y" ) {
    who.vt100( vt_start, "stat[year]" );
    H.print( who, Histogram_HDMY::YEARLY );
    return;
  }
}
  
// uptime [-sys]

void EXE_Client::uptime( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() || arg == "-sys" ) {
    who << ": ";
    who.vt100( vt_start, "uptime" ) << EOL;
    who << vc_stat;
    
    if (! arg.empty() ) {
      struct rlimit rlim;
      struct rusage ruse;
      who << EOL;
      who << "RESOURCE LIMITS :        soft / hard" << EOL;
      System::rlimit_get( RLIMIT_CPU, rlim );
      Form( who, "cpu seconds      : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_FSIZE, rlim );
      Form( who, "file  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_DATA, rlim );
      Form( who, "data  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_STACK, rlim );
      Form( who, "stack size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_CORE, rlim );
      Form( who, "core  size       : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_RSS, rlim );
      Form( who, "resident memory  : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      System::rlimit_get( RLIMIT_NPROC, rlim );
      Form( who, "# of processes   : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
#if defined(glibc)
      System::rlimit_get( RLIMIT_OFILE, rlim );
      Form( who, "# of open files  : %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
#endif
      System::rlimit_get( RLIMIT_MEMLOCK, rlim );
      Form( who, "# of memory locks: %10d / %10d", rlim.rlim_cur, rlim.rlim_max ) << EOL;
      who << "RESOURCE USAGE :" << EOL;
      System::rusage_get( ruse );
      Form( who, "time used user / system               : %10d / %10d",
	    sint8(ruse.ru_utime.tv_sec * uSec) + ruse.ru_utime.tv_usec,
	    sint8(ruse.ru_stime.tv_sec * uSec) + ruse.ru_stime.tv_usec ) << EOL;
      Form( who, "memory max resident / integral shared : %10d / %10d", ruse.ru_maxrss, ruse.ru_ixrss ) << EOL;
      Form( who, "unshared data / stack                 : %10d / %10d", ruse.ru_idrss, ruse.ru_isrss ) << EOL;
      Form( who, "page reclaims / faults                : %10d / %10d", ruse.ru_minflt, ruse.ru_majflt ) << EOL;
      Form( who, "swaps                                 : %10d",  ruse.ru_nswap ) << EOL;
      Form( who, "block operations input / output       : %10d / %10d", ruse.ru_inblock, ruse.ru_oublock ) << EOL;
      Form( who, "messages sent / received              : %10d / %10d", ruse.ru_msgsnd, ruse.ru_msgrcv ) << EOL;
      Form( who, "signals received                      : %10d", ruse.ru_nsignals ) << EOL;
      Form( who, "context switches voluntary / invol.   : %10d / %10d", ruse.ru_nvcsw, ruse.ru_nivcsw );
    }

    who << EOL;
    return;
  }

  vc_mssg._038( who.vt100(), who ); 
}

// salias [[?]pattern] [replacement]

void EXE_Client::salias( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "system alias" ) << ' ' << vc_var.alias.size() << vc_var.alias << EOL;
    return;
  }
  if ( arg[0] == '?' ) {
    String Prefix( arg.data() + 1, arg.size() - 1 );
    who << ": ";
    who.vt100( vt_start, "system alias" );
    vc_var.alias.print( who, Prefix );
    who << EOM;
    return; 
  }

  String name;
  String rest;
  String::parse( arg, name, rest );
  if (! rest.empty() && ! Alias::alias_name_ok( name ) )
    { vc_mssg._032( who.vt100(), who, name ); return; }
    
  Alias A( name, rest );
  const NAMED<Command>* p = cmds( A );
  if (! rest.empty() && p != 0 )
    { vc_mssg._049( who.vt100(), who ); return; }
  if ( who.var.admin() < admin_level ) { vc_mssg._017( who.vt100(), who ); return; }

  vc_var.alias = A;
  vc_var.modified();
}

// shistory

void EXE_Client::shistory( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if (! arg.empty() ) { vc_mssg._043( who.vt100(), who ); return; }

  who << ": ";
  who.vt100( vt_start, "system history" ) << ' ' << vc_var.history.size() << EOL;
  vc_var.history.print( who, History::SYSTEM, who.var ) << EOL;
}

// down

void EXE_Client::down( IO_TCP_Client& who, const String& /*arg*/)
{
  TSTAT;
  
  if ( who.id() != login_root ) {
    vc_mssg._040( who.vt100(), who ); 
    return;
  }

  vc_sig.send( SIGINT );
}

// mux [host port [method]]

void EXE_Client::mux( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( arg.empty() ) {
    who << ": ";
    who.vt100( vt_start, "mux" ) << ' ' << vc_mux.size() << EOL << vc_mux;
    return;
  }

#if 1  
  if ( who.var.admin() < admin_level ) { vc_mssg._017( who.vt100(), who ); return; }
#else  
  if ( who.id() != login_root )        { vc_mssg._040( who.vt100(), who ); return; }
#endif
  
  String tmp;
  String host;
  String::parse( arg, host, tmp );
  if ( tmp.size() == 0 ) { vc_mssg._046( who.vt100(), who ); return; }
  String sport;
  String method;
  String::parse( tmp, sport, method );

  String key = host + '@' + sport;
  
  IO_TCP_Mux* m = vc_mux( key );
  
  if ( method.size() == 0 ) {
    if ( m == 0 ) {
      vc_mssg._044( who.vt100(), who, host, sport ); 
      return;
    }
    m->te_cancel();
    m->te_create( IO_TCP_Mux::TE_KILL, 0 );
    return;
  }

  sint4 port = atoi( sport.c_str() ) + 3; // go to server ports
  
  IO_Buffer::Method cm = IO_Buffer::NONE;
  /**/ if ( method == "gzip"  ) { cm = IO_Buffer::GZIP;  port += 2; }
  else if ( method == "bzip2" ) { cm = IO_Buffer::BZIP2; port += 1; }
  else if ( method != "none"  ) {
    vc_mssg._047( who.vt100(), who, method ); 
    return;
  }
  
  if ( m == 0 ) {
    IO_TCP_Mux::connect( host.c_str(), port, cm, key );
    return;
  }
  
  vc_mssg._045( who.vt100(), who, host, sport ); 
}

// pack

void EXE_Client::pack( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
  if ( arg.empty() ) { vc_db.te_create( 0, 1 ); return; }
  vc_mssg._057( who.vt100(), who ); 
}

// stat

EXE_Client::EXE_Cmd::EXE_Cmd( const String& Name, Command Cmd )
  : NAMED<Command>( Name, Cmd )
{
}

ostream& EXE_Client::EXE_Cmd::print( ostream& os ) const
{
  const String& str = *this;
  os << setw(-9) << str;
#if 0
  Command       cmd = obj;
  Form( os, "{%d, %2d, {%08p} }",
	cmd.__delta,
	cmd.__index,
	cmd.__pfn_or_delta2.__pfn,
	cmd.__pfn_or_delta2.__delta2 );
#endif  
  return os;
}

ostream& EXE_Client::EXE_Cmds::print( ostream& os ) const
{
  TSTAT;
  
  SET< EXE_Cmd >::const_iterator it = begin();
  SET< EXE_Cmd >::const_iterator hi = end();
  for ( ; it != hi; ++it ) os << EOL << *it;
  return os;
}

// tstat [+|-]

void EXE_Client::tstat( IO_TCP_Client& who, const String& arg )
{
  TSTAT;

  if (! arg.empty() ) {
    if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
    /**/ if ( arg == "+" ) { tstat_ready = true;  vc_tstat.erase(); }
    else if ( arg == "-" ) { tstat_ready = false; vc_tstat.erase(); }
    else vc_mssg._013( who.vt100(), who, "tstat" );
    return;
  }
  who << ": "; who.vt100( vt_start, "tstat" ) << ' ' << vc_tstat << EOM;
}

// trace 0|1|2

void EXE_Client::trace( IO_TCP_Client& who, const String& arg )
{
  TSTAT;

  if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }

  if ( arg.empty() ) {
    who << ": "; who.vt100( vt_start, "trace" ) << ' ' << vc_trace << EOM;
    return;
  }
  vc_trace = atoi( arg.c_str() );
}

// tevent

void EXE_Client::tevent( IO_TCP_Client& who, const String& /*arg*/ )
{
  TSTAT;
  
  who << ": "; who.vt100( vt_start, "tevent" ) << ' ' << vc_time << EOM;
}

// exec <user> <cmds>

void EXE_Client::exec( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
  String usr;
  String cmd;
  String::parse( arg, usr, cmd );
  if ( usr.empty() || cmd.empty() ) { vc_mssg._034( who.vt100(), who ); return; }
  IO_TCP_Client* C = vc_var.client( usr );
  if ( C == 0 ) { vc_mssg._016( who.vt100(), who, usr ); return; }
  (*this)( *C, cmd );
}

// : <cmd> [args]
// ; <cmd> [args]

void EXE_Client::dummy( IO_TCP_Client& /*who*/, const String& /*arg*/ )
{
  TSTAT;
  
}

//: EXE_Client.C (eof) (c) Igor
