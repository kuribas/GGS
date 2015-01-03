// $Id: Signal.C 160 2007-06-22 15:21:10Z mburo $
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
//: Signal.C (bof) (c) Igor Durdanovic

#include <sys/wait.h>
#include <cstring>

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "Signal.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

#define MAX_SIG 32
// #define MAX_SIG NSIG

sigset_t Signal::sig_set;

bool     Signal::sig_exit = false;
bool     Signal::sig_pipe = false;
bool     Signal::sig_alrm = false;

using namespace std;

Signal::Signal()
{
  sint4 ok = sigfillset( &sig_set );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "sigfillset( ... ) = " << ok << endl << VCER; 
#endif

    System::exit(errno);
  }

  struct sigaction sigact;

  for ( sint4 i = 1; i < MAX_SIG; ++i ) {
    switch ( i ) { // skip panic signals ..
    case SIGILL    :
    case SIGABRT   :
    case SIGFPE    :
    case SIGSEGV   :
    case SIGSTKFLT : continue;
    default        : break;
    }
    
    sigact.sa_handler   = handler;
    sigact.sa_mask      = sig_set;
    sigact.sa_flags     = SA_NOCLDSTOP;

    if ( i == SIGKILL ) continue;
    if ( i == SIGSTOP ) continue;

    ok = sigaction( i, &sigact, 0 );

    if ( ok != 0 ){
#ifdef LOG_ERR
      vc_con << VCFL << "sigaction( " << i << ", ... ) = " << ok << endl << VCER; 
#endif
    }
  }

  vc_con << VCTIME << "  Signal( " << MAX_SIG << " )" << endl;

  if ( this == &vc_sig ) vc_sig_ready = true;
}

Signal::~Signal()
{
  if ( this == &vc_sig && ! vc_sig_ready ) return;

  struct sigaction sigact;

  for ( sint4 i = 1; i < MAX_SIG; ++i ) {
    sigact.sa_handler   = SIG_DFL;

    if ( i == SIGKILL ) continue;
    if ( i == SIGSTOP ) continue;

    sint4 ok = sigaction( i, &sigact, 0 );

    if ( ok != 0 ){
#ifdef LOG_ERR
      vc_con << VCFL << "sigaction( " << i << ", ... ) = " << ok << endl << VCER; 
#endif
    }
  }

  vc_con << VCTIME << " ~Signal( " << MAX_SIG << " )" << endl; 
}

void Signal::handler_EXIT()
{
  sig_exit = true;
}

void Signal::handler_PANIC()
{
  System::exit(-1);
}

void Signal::handler_SIGPIPE()
{
  TSTAT;
  
  sig_pipe = true;
}

void Signal::handler_SIGALRM()
{
  TSTAT;
  
  sig_alrm = true;
}

void Signal::handler_SIGCHLD()
{
  TSTAT;
  
  sint4 status;

  wait3( &status, WNOHANG | WUNTRACED, 0 );
}

void Signal::handler( sint4 SigNo )
{
  TSTAT;
  
  if ( SigNo != SIGALRM ) {
    vc_log << VCTIME << " Signal::handler( " 
	   << SigNo << ' ' << sys_siglist[ SigNo ] <<  " )" << endl;
  }

  switch ( SigNo ) {
  case SIGHUP     : break;
  case SIGINT     : handler_EXIT();    break;
  case SIGQUIT    : break;
  case SIGILL     : handler_PANIC();   break;
  case SIGTRAP    : break;
  case SIGABRT    : handler_PANIC();   break;
  case SIGBUS     : 
  case SIGFPE     : handler_PANIC();   break;
  case SIGKILL    : break;
  case SIGUSR1    : break;
  case SIGSEGV    : handler_PANIC();   break;
  case SIGUSR2    : break;
  case SIGPIPE    : handler_SIGPIPE(); break;
  case SIGALRM    : handler_SIGALRM(); break;
  case SIGTERM    : handler_EXIT();    break;
  case SIGSTKFLT  : handler_PANIC();   break;
  case SIGCHLD    : handler_SIGCHLD(); break;
  case SIGCONT    : break;
  case SIGSTOP    : break;
  case SIGTSTP    : break;
  case SIGTTIN    : break;
  case SIGTTOU    : break;
  case SIGURG     : break;
  case SIGXCPU    : break;
  case SIGXFSZ    : break;
  case SIGVTALRM  : break;
  case SIGPROF    : break;
  case SIGWINCH   : break;
  case SIGIO      : break;
  case SIGPWR     : break;
  case SIGUNUSED  : break;
  default         : 
    vc_con << VCFL << "SigNo:" << SigNo << endl; 
    break;
  }
}

void Signal::send( sint4 Sig )
{
  TSTAT;
  
  errno = 0;
  sint4 pid = getpid();
  sint4 res = kill( pid, Sig );
  if ( res != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << " kill( " << pid << ", " << Sig << " ) = " << res << endl << VCER;
#endif
    System::exit( errno );
  }
}

//: Signal.C (eof) (c) Igor
