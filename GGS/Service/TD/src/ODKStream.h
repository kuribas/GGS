// $Id: ODKStream.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.


#ifndef ODK_ODKSTREAM_H
#define ODK_ODKSTREAM_H

#include "ggsstream.h"
#include "OsObjects.h"
#include "AmsObjects.h"
#include "SG.h"

extern string sMyLogin;

#include <iostream>
using namespace std;

class CTRM {
public:
  CTRM() {};
  CTRM(const string& idtd);
  u4 iTrn, iRound, iMatch;

  void In(istream& is);
};

inline istream& operator>>(istream& is, CTRM& trm) { trm.In(is); return is; }

class tdstream: public ggsstream {
public:
  tdstream();

  virtual void Handle           (const CMsg& msg);
  virtual void HandleLogin      ();
  virtual void HandleTell       (const CMsgGGSTell& msg);
  virtual void HandleUnknown    (const CMsgGGSUnknown& msg);

  // general commands
  virtual void HandleCmdHelp    (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdRules   (const string& sFrom, const string& sCmd, istream& is);

  // Director commands
  virtual void HandleCmdNew     (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdCancel  (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdBan     (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdUnban   (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdForce   (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdBreak   (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdDelay   (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdStop    (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdUnstop  (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdOpen    (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdClose   (const string& sFrom, const string& sCmd, istream& is);

  // player commands
  virtual void HandleCmdTourneys(const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdJoin    (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdWithdraw(const string& sFrom, const string& sCmd, istream& is);

  // user commands
  virtual void HandleCmdFinger          (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdSchedulePlayer  (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdScheduleRound   (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdScheduleTournament(const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdRankings        (const string& sFrom, const string& sCmd, istream& is);

  // server commands
  virtual void HandleCmdIdle    ();
  virtual void HandleCmdBackup  (const string& sFrom, const string& sCmd, istream& is);
  virtual void HandleCmdRestore (const string& sFrom, const string& sCmd, istream& is);

  virtual CSGBase* CreateService(const string& sLogin);

  bool IsSuperuser(const string& sLogin) const;

protected:
  bool fInitTranslator;
  map<string,string> translate;

  void InitTranslator();
  void Translate(string& sCmd);
};

enum {
  kNoErr=0, kErrUnknownCommand, 

  kErrTournamentNotStarted, kErrTournamentStarted, 
  kErrPlayerNotInTournament, kErrBadNewCmd, kErrBadNewMinutes, kErrBadStyle,
  kErrMustBeDirector, kErrBadService,
  
  kErrMissingLogin, kErrUnregistered, kErrNotBanned, kErrBanned,

  kErrNoTournaments,kErrMissingTournamentId, kErrBadTournamentId,
  kErrMissingRoundId, kErrBadRoundId, kErrBadTRM,
  kErrMissingMinutes, kErrBadBreakMinutes,

  kErrAlreadyInTournament, kErrNoSchedule,
  kErrUnscheduledRound, kErrTournamentEnded,

  kErrNotCurrentRound, kErrCantForceBye, kErrBadNRounds, kErrOpen,
  kErrUserNotPresent // Igor    
};

void ErrOut(int err, const string& sLogin, const string& sCmd);
ostream& OKOut(const string& sLogin, const string& sCmd);

class CServiceOsTD : public CSG<COsRules> {
public:
  CServiceOsTD(tdstream* pgs);

  virtual void HandleMatchDelta (const TMsgMatchDelta& msg);
  virtual void HandleTDStart    (const TMsgTDStart& msg);

  map<string, CTRM> idmToTrm;
};

class CServiceAmsTD : public CSG<CAmsRules> {
public:
  CServiceAmsTD(tdstream* pgs);

  virtual void HandleMatchDelta (const TMsgMatchDelta& msg);
  virtual void HandleTDStart    (const TMsgTDStart& msg);

  map<string, CTRM> idmToTrm;
};

#endif // ODK_GGSSTREAM_H
