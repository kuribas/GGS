// $Id: odkstream.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.


#include "types.h"

#include "ODKStream.h"
#include "GGSMessage.h"
#include "SGMessages.h"
#include "GetMove.h"
#include "Tournament.h"

#include <iostream>
#include <sstream>
using namespace std;
#include <time.h>

extern tdstream tds;

ostream& OKOut(const string& sLogin, const string& sCmd) {
  return tds << "t " << sLogin << " " << sCmd << ": ";
}

void ErrOut(int err, const string& sLogin, const string& sCmd) {
  tds << "t " << sLogin << " ERR " << err << ": " << sCmd << ": ";
  switch(err) {
  case kErrTournamentNotStarted:
    tds << "can't issue command before tournament starts";
    break;
  case kErrTournamentStarted:
    tds << "can't issue command after tournament starts";
    break;
  case kErrPlayerNotInTournament:
    tds << "player is not in the tournament";
    break;
  case kErrBadNewCmd:
    tds << "format new s|r <service> <match type> <clock> <# rounds> <minutes to start>";
    break;
  case kErrBadService:
    tds << "service does not exist or is not installed in " << sMyLogin;
    break;
  case kErrBadNewMinutes:
    tds << "minutes to start must be between 0 and 30";
    break;
  case kErrBadStyle:
    tds << "style must be r (round robin) or s (swiss)";
    break;
  case kErrMustBeDirector:
    tds << "command restricted to tournament director";
    break;
  case kErrMissingLogin:
    tds << "missing login";
    break;
  case kErrUnregistered:
    tds << "you need to be registered";
    break;
  case kErrNotBanned:
    tds << "player had not been banned";
    break;
  case kErrBanned:
    tds << "player has been banned";
    break;
  case kErrNoTournaments:
    tds << "no tournaments have been scheduled";
    break;
  case kErrMissingTournamentId:
    tds << "need tournament id";
    break;
  case kErrBadTournamentId:
    tds << "tournament id does not exist";
    break;
  case kErrMissingRoundId:
    tds << "need round id";
    break;
  case kErrBadRoundId:
    tds << "round does not exist";
    break;
  case kErrMissingMinutes:
    tds << "need number of minutes";
    break;
  case kErrBadBreakMinutes:
    tds << "number of minutes must be between 0 and 60";
    break;
  case kErrAlreadyInTournament:
    tds << "you are already in the tournament";
    break;
  case kErrNoSchedule:
    tds << "tournament has not been scheduled yet";
    break;
  case kErrUnknownCommand:
    tds << "unknown command - type help or ? for help";
    break;
  case kErrUnscheduledRound:
    tds << "that round of the tournament has not been scheduled yet";
    break;
  case kErrTournamentEnded:
    tds << "the tournament is over";
    break;
  case kErrNotCurrentRound:
    tds << "scores can be set only for the current round";
    break;
  case kErrBadTRM:
    tds << "tournament/round/match combination does not exist";
    break;
  case kErrCantForceBye:
    tds << "can't force the result of a bye game";
    break;
  case kErrBadNRounds:
    tds << "tournaments must have at least 1 round";
    break;
  case kErrOpen:
    tds << "tournament is closed";
    break;
  case kErrUserNotPresent:
    tds << "User is not present!";
    break;
  default:
    tds << "unknown error, please report this bug";
    QSSERT(0);
    break;
  }
  tds << "\n";
}

tdstream::tdstream() {
  fInitTranslator=false;
}

extern const char* sChannel;
CTournaments trns;

CSGBase* tdstream::CreateService(const string& sServiceLogin) {
  CSGBase* pos=NULL;
  if (!HasService(sServiceLogin)) {
    if (sServiceLogin=="/os") {
      pos=new CServiceOsTD(this);
      if (pos)
        loginToPService[sServiceLogin]=pos;
    }
    else if (sServiceLogin=="/ams") {
      pos=new CServiceAmsTD(this);
      if (pos)
        loginToPService[sServiceLogin]=pos;
    }
  }
  return pos;
}

void tdstream::Handle(const CMsg& msg) {
  cout << msg.sRawText << "\n";
}

void tdstream::HandleLogin() {
  BaseLogin();
  (*this) << "mso\n"
          << "repeat 10 t " << sMyLogin << " idle\n"
          << "c + " << sChannel << "\n"
          << "notify + *\n";
  flush();
}

void tdstream::HandleUnknown(const CMsgGGSUnknown& msg) {
  cout << "Unknown GGS message: \n";
  Handle(msg);
}

////////////////////////
// Commands
////////////////////////

void tdstream::InitTranslator() {
  if (fInitTranslator)
    return;
  fInitTranslator=true;

  translate["?"]="help";
  translate["f"]="finger";
  translate["j"]="join";
  translate["t"]="tourneys";
  translate["r"]="rankings";
}

void tdstream::Translate(string& sCmd) {
  InitTranslator();
  map<string,string>::iterator i=translate.find(sCmd);
  if (i!=translate.end())
    sCmd=(*i).second;
}

void tdstream::HandleTell(const CMsgGGSTell& msg) {
  cout << msg.sFrom << " " << msg.sText << "\n";
  // command from me
  if (IsSuperuser(msg.sFrom) && msg.sText=="quit") {
    Logout();
  }
  // command from self
  else if (msg.sFrom==sMyLogin) {
    if (msg.sText=="idle")
      HandleCmdIdle();
    else {
      cout << "unknown message from self: " << msg.sText << "\n";
    }
  }
  // command from someone else, but not from a channel or group
  else if (msg.sFrom[0]!='.' && msg.sFrom[0]!='_') {
    string sText(msg.sText);
    istringstream is(sText);
    string sCmd;

    is >> sCmd;
    Translate(sCmd);

    // director commands
    if    (sCmd == "new")
      HandleCmdNew    (msg.sFrom, sCmd, is);
    else if (sCmd == "break")
      HandleCmdBreak    (msg.sFrom, sCmd, is);
    else if (sCmd == "delay")
      HandleCmdDelay    (msg.sFrom, sCmd, is);
    else if (sCmd == "stop")
      HandleCmdStop   (msg.sFrom, sCmd, is);
    else if (sCmd == "unstop")
      HandleCmdUnstop   (msg.sFrom, sCmd, is);
    else if (sCmd == "cancel")
      HandleCmdCancel   (msg.sFrom, sCmd, is);
    else if (sCmd == "ban")
      HandleCmdBan    (msg.sFrom, sCmd, is);
    else if (sCmd == "unban")
      HandleCmdUnban    (msg.sFrom, sCmd, is);
    else if (sCmd == "force")
      HandleCmdForce    (msg.sFrom, sCmd, is);
    else if( sCmd == "open")
      HandleCmdOpen     (msg.sFrom, sCmd, is);
    else if( sCmd == "close")
      HandleCmdClose    (msg.sFrom, sCmd, is);

    // player commands
    else if (sCmd == "tourneys")
      HandleCmdTourneys   (msg.sFrom, sCmd, is);
    else if (sCmd == "join")
      HandleCmdJoin   (msg.sFrom, sCmd, is);
    else if (sCmd == "withdraw")
      HandleCmdWithdraw (msg.sFrom, sCmd, is);

    // user commands
    else if (sCmd == "finger")
      HandleCmdFinger (msg.sFrom, sCmd, is);
    else if (sCmd == "rankings")
      HandleCmdRankings (msg.sFrom, sCmd, is);
    else if (sCmd == "sp")
      HandleCmdSchedulePlayer(msg.sFrom, sCmd, is);
    else if (sCmd == "sr")
      HandleCmdScheduleRound(msg.sFrom, sCmd, is);
    else if (sCmd == "st")
      HandleCmdScheduleTournament(msg.sFrom, sCmd, is);

    // superuser commands
    else if (sCmd == "backup")
      HandleCmdBackup (msg.sFrom, sCmd, is);
    else if (sCmd == "restore")
      HandleCmdRestore(msg.sFrom, sCmd, is);

    // General commands
    else if (sCmd == "help")
      HandleCmdHelp(msg.sFrom, sCmd, is);
    else if (sCmd == "rules")
      HandleCmdRules(msg.sFrom, sCmd, is);
    else {
      ErrOut(kErrUnknownCommand, msg.sFrom, sCmd);
    }
  }
  flush();
}

void tdstream::HandleCmdHelp(const string& sFrom, const string& sCmd, istream& /*is*/) { 
  OKOut(sFrom, sCmd)
    << "\\\\Tournament Director Program"
    << "\\"
    << "\\General Information:"
    << "\\help or ?       - this help file"
    << "\\rules           - special tournament rules"
    << "\\"
    << "\\Player commands:"
    << "\\[t]ourneys      - list tourneys"
    << "\\[j]oin <id>     - join a tourney                  [b]"
    << "\\withdraw <id>   - remove yourself from a tourney  [b]"
    << "\\"
    << "\\User commands:"
    << "\\[f]inger <id>    - tourney info"
    << "\\[r]ankings <id>  - display rankings in a tourney  [a]"
    << "\\sp <id> <login>  - display schedule for a player  [a]"
    << "\\sr <id> <round>  - display schedule for a round   [a]"
    << "\\st <id>          - display schedule for a tourney [a]"
    << "\\"
    << "\\Director commands:"
    << "\\new s|r <service> <match type> <clock> <rounds> <minutes until start> - create a new tournament"
    << "\\break <id> <minutes>  - set between-round break time"
    << "\\delay <id> <minutes>  - delay tournament start    [b]"
    << "\\open <id>        - joining is allowed"
    << "\\close <id>       - joining is not allowed"
    << "\\stop <id>        - rounds will not start"
    << "\\unstop <id>      - rounds will start"
    << "\\cancel <id>      - cancel a tourney               [b]"
    << "\\ban <id> <login> - ban someone from a tourney     [b]"
    << "\\unban <id> <login> - unban someone from a tourney [b]"
    << "\\force <idt.round.match> <score> - set the score of a match  [a]"
    << "\\"
    << "\\Superuser commands:"
    << "\\backup           - save tournaments to disk"
    << "\\restore          - restore tournaments from disk"
    << "\\"
    << "\\[a] - only available after the tourney starts"
    << "\\[b] - only available before the tourney starts"
    << "\n";
}

void tdstream::HandleCmdRules(const string& sFrom, const string& sCmd, istream& is) {
  string sChapter;

  is >> sChapter;
  if (sChapter.empty()) {
    OKOut(sFrom, sCmd)
      << "\\\\Tournament Rules"
      << "\\"
      << "\\The following special rules apply to tournament games:"
      << "\\"
      << "\\All tournament games are rated, so you must be registered"
      << "\\to play."
      << "\\" 
      << "\\Once you sign up for a tournament, your tournament games will"
      << "\\be automatically started."
      << "\\"
      << "\\Your clock ticks and new games will be started EVEN IF YOU DISCONNECT."
      << "\\When you reconnect you may continue playing."
      << "\\\\"
      << "\\More rules: rules swiss, rules tiebreak\n";
  }
  else if (sChapter=="tiebreak") {
    OKOut(sFrom, sCmd)
      << "\\\\ Tiebreaks"
      << "\\"
      << "\\Ties are resolved by comparing the break fraction (BF),"
      << "\\which is based on opponent strength and victory amount"
      << "\\"
      << "\\Define a player's 'winning %' as"
      << "\\    (wins + draws/2)/(wins+draws+losses), not counting byes"
      << "\\"
      << "\\Define a game's Victory Fraction as"
      << "\\     [(match result/maximum match result)+1]/2"
      << "\\"
      << "\\BF = [average(opponent's winning % + game Victory Fraction)]/2"
      << "\\"
      << "\\Othello players: 2*BF*64*(number of rounds)=Brightwell Quotient"
      << "\\  (if there are no byes or withdrawals)"
      << "\n";
  }
  else if (sChapter=="swiss") {
    OKOut(sFrom, sCmd)
      << "\\\\ Swiss Tournaments"
      << "\\"
      << "\\'Swiss' is a method of scheduling tournaments."
      << "\\Players with similar records are scheduled to play each other."
      << "\\This is likely to be more fun since your opponent will be about"
      << "\\your strength, at least after the first couple of rounds."
      << "\\"
      << "\\Players can join and withdraw from Swiss tournaments while the"
      << "\\tournament is ongoing; joins and withdraws take place as soon"
      << "\\as the next round begins. You can even withdraw and rejoin later."
      << "\\"
      << "\\As of this writing the algorithm follows these rules, in order:"
      << "\\1.Avoid pairing the same players twice"
      << "\\2.Match nearby opponents"
      << "\\3.Give white to player with more net black games (black-white),"
      << "\\     choose color randomly if both have same net black games."
      << "\n";
  }
}

//////////////////////////////
// Director commands
//////////////////////////////

void tdstream::HandleCmdNew(const string& sFrom, const string& sCmd, istream& is) {

  string smt;
  CSGClock ck;
  int nRounds;
  int nMinutesToStart;
  string sLoginService;
  char cStyle;

  // check message format
  if (!(is >> cStyle >> sLoginService >> smt >> ck >> nRounds >> nMinutesToStart)) {
    ErrOut(kErrBadNewCmd, sFrom, sCmd);
  }
  
  else if (PService(sLoginService)==NULL) {
    ErrOut(kErrBadService, sFrom, sCmd);
  }

  // can't clog the server with tons of tournaments
  else if (nMinutesToStart>30 || nMinutesToStart<1) {
    ErrOut(kErrBadNewMinutes, sFrom, sCmd);
  }

  else if (nRounds<=0) {
    ErrOut(kErrBadNRounds, sFrom, sCmd);
  }

  else if (cStyle!='s' && cStyle!='r') {
    ErrOut(kErrBadStyle, sFrom, sCmd);
  }

  // create tournament
  else {
    CTournament* pTrn;
    if (cStyle=='r')
      pTrn=new CTournamentRoundRobin(sFrom, sLoginService, ck, smt, nRounds, nMinutesToStart);
    else
      pTrn=new CTournamentSwiss(sFrom, sLoginService, ck, smt, nRounds, nMinutesToStart);

    if (pTrn) {
      trns.InsertTournament(pTrn);
      OKOut(sFrom, sCmd) << pTrn->Name() << " created\n";
      extern bool fTesting;

      (*this) << "t " << (fTesting?sFrom:string(".chat")) << ' '
              << "\\---------------- " << sMyLogin << " --------------\\"
              << pTrn->Name() << " starting in " << nMinutesToStart << " minutes"
              << "\\(" << pTrn->Description() << ")"
              << "\\to play: tell " << sMyLogin << " join " << pTrn->Idt()
              << "\\for tournament messages: chann + " << sChannel 
              << "\\---------------- " << sMyLogin << " --------------"
              << '\n';
    }
  }
}

void tdstream::HandleCmdBreak(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  u4 nBreak;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd);
  if (!ptrn) {
  }
  else if (ptrn->Ended())
    ErrOut(kErrTournamentEnded, sFrom, sCmd);
  else if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else if (!(is >> nBreak))
    ErrOut(kErrMissingMinutes, sFrom, sCmd);
  else if (nBreak>60)
    ErrOut(kErrBadBreakMinutes, sFrom, sCmd);
  else
    ptrn->HandleCmdBreak(nBreak, sFrom, sCmd);
}

void tdstream::HandleCmdDelay(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  u4 nMinutes;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd);
  if (!ptrn) {
  }
  else if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else if (!(is >> nMinutes))
    ErrOut(kErrMissingMinutes, sFrom, sCmd);
  else if (nMinutes>60)
    ErrOut(kErrBadBreakMinutes, sFrom, sCmd);
  else
    ptrn->HandleCmdDelay(nMinutes, sFrom, sCmd);
}

void tdstream::HandleCmdStop(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd);
  if (!ptrn) {
  }
  else if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else
    ptrn->HandleCmdStop(sFrom, sCmd);
}

void tdstream::HandleCmdUnstop(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd);
  if (!ptrn) {
  }
  else if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else
    ptrn->HandleCmdUnstop(sFrom, sCmd);
}

void tdstream::HandleCmdCancel(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kUnstarted);
  if (!ptrn)
    return;

  if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);

  else {
    OKOut(sFrom, sCmd) << "cancelled tournament " << idt << "\n";
    ptrn->HandleCmdCancel();
    trns.erase(idt);
  }
}

void tdstream::HandleCmdOpen(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (!ptrn)
    return;

  if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else {
    ptrn->HandleCmdOpen(sFrom);
  }
}

void tdstream::HandleCmdClose(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (!ptrn)
    return;

  if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else {
    ptrn->HandleCmdClose(sFrom);
  }
}

void tdstream::HandleCmdBan(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (!ptrn)
    return;

  if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);
  else {
    string sPlayer;
    if (!(is >> sPlayer))
      ErrOut(kErrMissingLogin, sFrom, sCmd);
    else
      ptrn->HandleCmdBan(sPlayer, sFrom, sCmd);
  }
}

void tdstream::HandleCmdUnban(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (ptrn==NULL) {
  }

  else if (!(ptrn->IsDirector(sFrom)))
    ErrOut(kErrMustBeDirector, sFrom, sCmd);

  else {
    string sPlayer;

    if (!(is >> sPlayer))
      ErrOut(kErrMissingLogin, sFrom, sCmd);

    else 
      ptrn->HandleCmdUnban(sPlayer, sFrom, sCmd);
  }
}

void tdstream::HandleCmdForce(const string& sFrom, const string& sCmd, istream& is) {
  CTRM trm;
  CSGResult result;

  if (is >> trm >> result) {
    map<u4, CTournament*>::iterator i=trns.find(trm.iTrn);
    if (i!=trns.end()) {
      /*
        if (trm.iRound==(*i).second->NRoundsPlayed())
        trns[trm.iTrn]->SetResult(trm, result);
        else
        ErrOut(kErrNotCurrentRound, sFrom, sCmd);*/
      trns[trm.iTrn]->SetResult(trm, result);
    }
    else {
      ErrOut(kErrBadTRM, sFrom, sCmd);
    }
  }
}

///////////////////////////////////
// Player commands
///////////////////////////////////

void tdstream::HandleCmdTourneys(const string& sFrom,const string& sCmd,  istream& /*is*/) {
  OKOut(sFrom, sCmd) << trns.size() << "\\";
  trns.List();
  (*this) << "\n";
}

void tdstream::HandleCmdJoin(const string& sFrom, const string& sCmd, istream& is) {
  // test for registered first so unregistered users get the most appropriate
  //  error message
  if (loginToLevel[sFrom]==0) {
    ErrOut(kErrUnregistered, sFrom, sCmd);
    return;
  }
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (!ptrn)
    return;

  string sPlayer;

  if (IsSuperuser(sFrom))
    is >> sPlayer;
  if (sPlayer.empty())
    sPlayer=sFrom;

  map<string, int>::iterator i=loginToLevel.find(sPlayer);
  if ( i == loginToLevel.end() ) {
    ErrOut(kErrUserNotPresent, sFrom, sCmd );
    return;
  }
      
  ptrn->HandleCmdJoin(sPlayer, sFrom, sCmd);
}

void tdstream::HandleCmdWithdraw(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kJoinable);
  if (!ptrn)
    return;

  string sPlayer;

  if (IsSuperuser(sFrom))
    is >> sPlayer;
  if (sPlayer.empty())
    sPlayer=sFrom;

  ptrn->HandleCmdWithdraw(sPlayer, sFrom, sCmd);
}

///////////////////////////////////
// user commands
///////////////////////////////////

void tdstream::HandleCmdFinger(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kAny);
  if (!ptrn)
    return;

  OKOut(sFrom, sCmd) << idt << "\\";
  ptrn->HandleCmdFinger(sFrom, sCmd);
  (*this) << "\n";
}

void tdstream::HandleCmdRankings(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kStarted);
  if (!ptrn)
    return;


  ptrn->HandleCmdRankings(sFrom, sCmd);
  (*this) << "\n";
}

void tdstream::HandleCmdSchedulePlayer(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kStarted);
  if (!ptrn)
    return;

  string sPlayer;
  if (!(is >> sPlayer))
    sPlayer=sFrom;
  ptrn->HandleCmdSchedulePlayer(sPlayer, sFrom, sCmd);
  (*this) << "\n";
}

void tdstream::HandleCmdScheduleRound(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kStarted);
  if (!ptrn)
    return;

  int iRound;
  if (!(is >> iRound)) {
    ErrOut(kErrMissingRoundId, sFrom, sCmd);
  }
  else {
    iRound--;
    ptrn->HandleCmdScheduleRound(iRound, sFrom, sCmd);
  }
}

void tdstream::HandleCmdScheduleTournament(const string& sFrom, const string& sCmd, istream& is) {
  u4 idt;
  CTournament* ptrn=trns.FindTournament(is, idt, sFrom, sCmd, CTournaments::kStarted);
  if (!ptrn)
    return;

  ptrn->HandleCmdScheduleTournament(sFrom, sCmd);
  (*this) << "\n";
}

///////////////////////////////////
// System commands
///////////////////////////////////

void tdstream::HandleCmdIdle() {
  CTournaments::iterator i,j;
  CTournament* ptrn;

  for (i=trns.begin(); i!=trns.end(); ) {
    ptrn=((*i).second);
    j=i;
    i++;
    ptrn->DoIdle();
    if (ptrn->Dead())
      trns.erase(j);
  }
}

void tdstream::HandleCmdBackup(const string& sFrom, const string& /*sCmd*/, istream& /*is*/) {
  if (IsSuperuser(sFrom))
    trns.Backup();
}

void tdstream::HandleCmdRestore(const string& sFrom, const string& /*sCmd*/, istream& /*is*/) {
  if (IsSuperuser(sFrom))
    trns.Restore();
}

bool tdstream::IsSuperuser(const string& sLogin) const {
#if 1
  // Igor: reuse GGS admin levels
  map<string, int>::const_iterator i=loginToLevel.find(sLogin);
  if ( i == loginToLevel.end() ) return false;
  return ( i->second >=2 );
#else 
  return sLogin=="n2" || sLogin=="mic" || sLogin=="igor" || sLogin=="dan" || sLogin=="romano";
#endif  
}

//////////////////////////////////////
// /os specific messages
//////////////////////////////////////

CServiceOsTD::CServiceOsTD(tdstream* apgs) : CSG<COsRules>(apgs) {
  (*pgs) << "t /os notify + *\n" << flush;
}

void CServiceOsTD::HandleMatchDelta (const TMsgMatchDelta& msg)
{
  if (!msg.fPlus) {
    map<string, CTRM>::iterator i = idmToTrm.find(msg.match.idm);
    if (i!=idmToTrm.end()) {
      CTRM& trm=(*i).second;
      trns[trm.iTrn]->SetResult(trm, msg.result);
      idmToTrm.erase(i);
    }
  }
}


void CServiceOsTD::HandleTDStart(const TMsgTDStart& msg)
{
  idmToTrm[msg.idm] = CTRM(msg.idtd);
}

//////////////////////////////////////
// /ams specific messages
//////////////////////////////////////

CServiceAmsTD::CServiceAmsTD(tdstream* apgs) : CSG<CAmsRules>(apgs) {
  (*pgs) << "t /ams notify + *\n" << flush;
}

void CServiceAmsTD::HandleMatchDelta  (const TMsgMatchDelta& msg){
  if (!msg.fPlus) {
    map<string, CTRM>::iterator i = idmToTrm.find(msg.match.idm);
    if (i!=idmToTrm.end()) {
      CTRM& trm=(*i).second;
      trns[trm.iTrn]->SetResult(trm, msg.result);
      idmToTrm.erase(i);
    }
  }
}


void CServiceAmsTD::HandleTDStart(const TMsgTDStart& msg) {
  idmToTrm[msg.idm] = CTRM(msg.idtd);
}
