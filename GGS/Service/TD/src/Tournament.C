// $Id: Tournament.C 9037 2010-07-06 04:05:44Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//  All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#include "Tournament.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <stdio.h>
using namespace std;

extern tdstream tds;

bool fTesting = false;
#if 0
const char* sChannel = ".test";
#else
const char* sChannel = ".tourney";
#endif

ostream& TimeOut(int t) {
  stringstream stream;
  if (t<0) {
    t=0;
  }
  stream << t/60 << ':';
  t%=60;
  if (t<10)
    stream << 0;
  stream << t;
  tds << setw(5) << stream.str();
  return tds;
}


/////////////////////////////////////////
// CTRM class
/////////////////////////////////////////

CTRM::CTRM(const string& idtd) {
  string id(idtd);

  istringstream is(idtd);
  In(is);
}

void CTRM::In(istream& is) {
  char c;
  is >> iTrn >> c >> iRound >> c >> iMatch;

  iRound--;
  iMatch--;
}

/////////////////////////////////////////
// CTDRanking class
/////////////////////////////////////////

CTDRanking::CTDRanking() {
  nResult[0]=nResult[1]=nResult[2]=nByes=0;
  fractionTotal=bq=discs=0;
}

u4 CTDRanking::DemipointsNoByes() const {
  return nResult[2]+nResult[2]+nResult[1];
}

u4 CTDRanking::Demipoints() const {
  u4 nWins=nResult[2]+nByes;
  return nWins+nWins+nResult[1];
}

u4 CTDRanking::NPlayed() const {
  return nResult[2]+nResult[1]+nResult[0];
}

double CTDRanking::WinPct2() const {
  return double(DemipointsNoByes())/NPlayed();
}

double CTDRanking::WinFraction2() const {
  return 1+fractionTotal/NPlayed();
}

bool CTDRanking::operator<(const CTDRanking& b) const {
  if (Demipoints()!=b.Demipoints())
    return Demipoints()>b.Demipoints();
  else
    return bq>b.bq;
}

bool CTDRanking::operator==(const CTDRanking& b) const {
  return Demipoints()==b.Demipoints() && bq==b.bq;
}

/////////////////////////////////////////
// CScheduleMatch class
/////////////////////////////////////////

CScheduleMatch::CScheduleMatch() {
  result.status = CSGResult::kUnstarted;
}

bool CScheduleMatch::Active() const {
  return result.status==COsResult::kUnfinished;
}

void CScheduleMatch::OutputSchedule(const CTournament& trn, u4 iRound, u4 iMatch) const {
  string sp0=trn.pl.IdpToLogin(idps[0]);
  string sp1=trn.pl.IdpToLogin(idps[1]);

  stringstream stream;
  stream << iRound+1 << '.' << iMatch+1;
  tds << "\\" << setw(string("10.10").size()) << stream.str() << "  ";
  if (sp0.empty())
    tds << setw(8) << sp1 << " rests";
  else if (sp1.empty())
    tds << setw(8) << sp0 << " rests";
  else
    tds << setw(8) << sp1 << ' ' << setw(8) << sp0 << ' ' << setw(string("+196.00").size()) << setprecision(1) << fixed << showpos << result << noshowpos;
}

void CScheduleMatch::Out(ostream& os) const {
  os << idps[0] << " "
     << idps[1] << " "
     << result << " ";
}

void CScheduleMatch::In(istream& is) {
  is >> idps[0] >> idps[1] >> result;
}

/////////////////////////////////////////
// CScheduleRound class
/////////////////////////////////////////

bool CScheduleRound::Active() const {
  for ( const_iterator pm=begin(); pm!=end(); pm++) {
    if (pm->Active())
      return true;
  }

  return false;

}

void CScheduleRound::OutputSchedule(const CTournament& trn, u4 iRound) const {
  tds << "\\round " << setw(3) << iRound+1 << ":";
  u4 i;
  for (i=0; i<size(); i++)
    begin()[i].OutputSchedule(trn, iRound, i);
}

void CScheduleRound::Out(ostream& os) const {
  os << "#Matches: " << size() << " ";
  const_iterator i;
  for (i=begin(); i!=end(); i++)
    os << (*i) << "    ";
  os << "\n";
}

void CScheduleRound::In(istream& is) {
  u4 n;
  string s;
  CScheduleMatch sm;
  is >> s >> n; QSSERT(s=="#Matches:");
  while (n--) {
    is >> sm;
    push_back(sm);
  }
}

/////////////////////////////////////////
// CSchedule class
/////////////////////////////////////////

void CSchedule::Out(ostream& os) const {
  os << "#RoundsScheduled: " << size() << "\n";
  const_iterator i;
  for (i=begin(); i!=end(); i++)
    os << (*i);
}

void CSchedule::In(istream& is) {
  u4 n;
  string s;

  CScheduleRound sr;

  is >> s >> n; QSSERT(s=="#RoundsScheduled:");
  while (n--) {
    sr.clear();
    is >> sr;
    push_back(sr);
  }
}

/////////////////////////////////////////
// CPlayer class
/////////////////////////////////////////

bool CPlayer::Active() const {
  return !fWithdrawn;
}

bool CPlayer::Bye() const {
  return sLogin.empty();
}

void CPlayer::Out(ostream& os) const {
  os << fWithdrawn << " ";
  if (Bye())
    os << "(bye)";
  else
    os << sLogin;
}

void CPlayer::OutIfActive(ostream& os, bool fSwing) const {
  if (fWithdrawn)
    return;
  if (Bye())
    os << "(bye)";
  else if (fSwing)
    os << '(' << sLogin << ')';
  else
    os << sLogin;
  os << " ";
}

void CPlayer::In(istream& is) {
  is >> fWithdrawn;
  is >> sLogin;
  if (sLogin=="(bye)")
    sLogin="";
}

/////////////////////////////////////////
// CPlayerList class
/////////////////////////////////////////

CPlayerList::CPlayerList() {
  idpSwing=-1;
}

u4 CPlayerList::Join(const string& sLogin) {
  int idp=LoginToIdp(sLogin);
  if (idp<0) {
    CPlayer p;
    p.sLogin=sLogin;
    p.fWithdrawn=false;
    push_back(p);
    return 0;
  }
  else if ((*this)[idp].fWithdrawn) {
    (*this)[idp].fWithdrawn=false;
    return 0;
  }
  else
    return kErrAlreadyInTournament;
}

u4 CPlayerList::Withdraw(const string& sLogin) {
  int idp=LoginToIdp(sLogin);
  if (idp<0 || (*this)[idp].fWithdrawn) {
    return kErrPlayerNotInTournament;
  }
  else  {
    (*this)[idp].fWithdrawn=true;
    return 0;
  }
}

int CPlayerList::AddSwing(const string& sLogin) {
  // no if already have swing player
  if (idpSwing>=0)
    return -1;

  // if player is not bye, check to see that he's online and registered
  if (!sLogin.empty()) {
      
    // no if not here or not registered
    map<string, int>::iterator i=tds.loginToLevel.find(sLogin);
    if (i==tds.loginToLevel.end())
      return -1;
    if ((*i).second<=0)
      return -1;
  }

  // no if already in tournament or other join error
  if (Join(sLogin))
    return -1;
  
  idpSwing=LoginToIdp(sLogin);
  return idpSwing;
}

void CPlayerList::RemoveWithdrawn() {
  for (int i = size(); --i >= 0; ) {
    if ((*this)[i].fWithdrawn)
      erase(begin()+i);
  }
}

bool CPlayerList::IsActive(const string& sLogin) const {
  int idp=LoginToIdp(sLogin);
  return !(idp<0 || (*this)[idp].fWithdrawn);
}

int CPlayerList::LoginToIdp(const string& sLogin) const {
  u4 idp;

  for (idp=0; idp<size(); idp++) {
    if ((*this)[idp].sLogin==sLogin)
      return int(idp);
  }
  return -1;
}

const string& CPlayerList::IdpToLogin(int idp) const {
  return (*this)[idp].sLogin;
}

int CPlayerList::IdpBye() const {
  if (idpSwing<0 || !(*this)[idpSwing].Bye())
    return -1;
  else 
    return idpSwing;
}

u4 CPlayerList::NActive() const {
  u4 idp;
  int nActive;

  nActive=0;
  for (idp=0; idp<size(); idp++) {
    if (!((*this)[idp].fWithdrawn))
      nActive++;
  }
  return nActive;

}

void CPlayerList::Out(ostream& os) const {
  os << "#Players: " << size() << " ";
  os << "idpSwing: " << idpSwing << " ";
  const_iterator i;
  for (i=begin(); i!=end(); i++)
    os << *i << " ";
}

void CPlayerList::OutActive(ostream& os) const {
  os << NActive() << " ";
  for ( const_iterator i=begin(); i!=end(); i++) {
    (*i).OutIfActive(os, (i-begin()) == idpSwing);
  }
}

void CPlayerList::In(istream& is) {
  int n;
  CPlayer p;
  string s;

  is >> s >> n; QSSERT(s=="#Players:");
  is >> s >> idpSwing; QSSERT(s=="idpSwing:");
  while (n--) {
    is >> p;
    push_back(p);
  }
}

/////////////////////////////////////////
// Tournaments class
/////////////////////////////////////////

size_t CTournament::max_score_width = 0;

CTournaments::CTournaments() : nIdts(1) {}

CTournaments::~CTournaments() {
  CTournaments::iterator i;
  for (i=begin() ; i!=end() ; i++)
    delete (*i).second;
}

// find a tournament, print an error message and return NULL if not possible
CTournament* CTournaments::FindTournament(istream& is, u4& idt,
                                          const string& sFrom, const string& sCmd, u4 fFlags) {
  CTournament* pTrn=NULL;

  if (empty()) {
    ErrOut(kErrNoTournaments, sFrom, sCmd);
  }
  else {
    CTournaments::iterator i;
    if (!(is >> idt)) {
      if (size()==1) {
        i=begin();
        idt=(*i).first;
        is.clear();
      }
      else {
        ErrOut(kErrMissingTournamentId, sFrom, sCmd);
        tds.flush();
        return pTrn;
      }
    }
    else
      i=find(idt);

    if (i==end())
      ErrOut(kErrBadTournamentId, sFrom, sCmd);
    else
      pTrn= (*i).second;
  }

  if (pTrn) {
    int err;

    // check that tournament is proper
    switch(fFlags) {
    case kUnstarted:
      if (pTrn->Started()) {
        ErrOut(kErrTournamentStarted, sFrom, sCmd);
        pTrn=NULL;
      }
      break;
    case kStarted:
      if (!pTrn->Started()) {
        ErrOut(kErrTournamentNotStarted, sFrom, sCmd);
        pTrn=NULL;
      }
      break;
    case kJoinable:
      err=pTrn->ErrJoin();
      if (err) {
        ErrOut(err, sFrom, sCmd);
        pTrn=NULL;
      }
      break;
    }

  }
  tds.flush();
  return pTrn;
}

u4 CTournaments::InsertTournament(CTournament* pTrn) {
  u4 idt=nIdts++;

  pTrn->SetIdt(idt);
  (*this)[idt]=pTrn;
  return idt;
}

u4 CTournaments::erase(u4 idt) {
  CTournaments::iterator i;
  u4 n=0;

  i=lower_bound(idt);
  while (i!=end() && (*i).first==idt) {
    i=erase(i);
    n++;
  }

  return n;
}

void CTournaments::List() const {
  CTournament::OutHeader();
  CTournaments::const_iterator i;
  for (i=begin(); i!=end(); i++) {
    tds << '\\';
    (*(*i).second).OutHeadline(tds);
  }
  tds << "\n" << flush;
}

void CTournaments::Out(ostream& os) const {
  const_iterator i;
  os << "#Tournaments: " << size() << "\n\n";
  for (i=begin(); i!=end(); i++) {
    //  os << (*i).first << " " << *((*i).second) << "\n\n";
    os << *((*i).second) << "\n\n";
  }
}

void CTournaments::In(istream& is) {
  int n;
  CTournament* pTrn;
  string s;

  clear();
  nIdts=1;
  QSSERT(size()==0);
  is >> s >> n;
  QSSERT(s=="#Tournaments:");
  while (n--) {
    //is >> iTrn;
    pTrn=CTournament::CreateFromStream(is);
    InsertTournament(pTrn);
  }
}

static const char* fnBackup="backup.trns";

void CTournaments::Backup() const {
  ofstream ofs(fnBackup);
  Out(ofs);
}

void CTournaments::Restore() {
  ifstream ifs(fnBackup);
  In(ifs);
}

/////////////////////////////////////////
// Tournament class
/////////////////////////////////////////

CTournament::CTournament(const string& asLoginDirector, const string& asLoginService,
                         const CSGClock& ack,
                         const string& asmt, u4 anRounds, u4 anMinutesToStart) {
  fBreak=true;
  tBreak=60;
  fDead=false;
  nRoundsPlayed=0;
  fStopped=0;
  fOpen = true;

  sLoginDirector=asLoginDirector;
  sLoginService=asLoginService;
  ck=ack;
  smt=asmt;
  nRounds=anRounds;
  SetTNextRoundStart(anMinutesToStart*60);
  GetTDType();
}

void CTournament::GetTDType() {
  CSGBase* pService=tds.PService(sLoginService);
  if (!pService)
    throw CError(kErrBadService, sLoginService+" is not available or not installed in " + sMyLogin);
  pService->TDType(smt, smtOut, smtDescription, dMaxResult);
}

/////////////////////////////////////////
// Tournament class : director commands
/////////////////////////////////////////

void CTournament::HandleCmdBan(const string& sPlayer, const string& sFrom, const string& sCmd) {
  if (Started())
    ErrOut(kErrTournamentStarted, sFrom, sCmd);
  else {
    sBanned.insert(sPlayer);
    tds << "t " << sFrom << " banned " << sPlayer << " from " << Name() << "\n";

    // if player is in tournament, remove him and tell him
    int idp=pl.LoginToIdp(sPlayer);
    if (idp>=0)
      HandleCmdWithdraw(sPlayer, sFrom, sCmd);
  }
  tds.flush();
}

void CTournament::HandleCmdBreak(u4 nMinutes, const string& sFrom, const string& /*sCmd*/) {
  tds << "t " << sFrom << " between-round break set to " << nMinutes << " minutes\n";

  SetTBreak(nMinutes*60);
}

u4 CTournament::ErrDelay(const string& /*sFrom*/) const {
  if (Started())
    return kErrTournamentStarted;
  else
    return 0;
}

void CTournament::HandleCmdDelay(u4 nMinutes, const string& sFrom, const string& /*sCmd*/) {
  tds << "t " << sFrom << " delay tournament start for " << nMinutes << " minutes\n";
  SetTNextRoundStart(nMinutes*60+tNextRound-time(NULL));
}

void CTournament::HandleCmdStop(const string& sFrom, const string& /*sCmd*/) {
  tds << "t " << sFrom << "," << sChannel << " tournament " << idt
      << " stopped (new rounds will not start until the director unstops the tournament)\n";
  fStopped=true;
}

void CTournament::HandleCmdUnstop(const string& sFrom, const string& /*sCmd*/) {
  tds << "t " << sFrom << "," << sChannel << " tournament " << idt << " unstopped\n";
  fStopped=false;
}

void CTournament::HandleCmdUnban(const string& sPlayer, const string& sFrom, const string& sCmd) {
  if (Started())
    ErrOut(kErrTournamentStarted, sFrom, sCmd);
  else {
    if (sBanned.erase(sPlayer))
      OKOut(sFrom, sCmd) <<  "unbanned " << sPlayer << " from "<< Name() << "\n";
    else
      ErrOut(kErrNotBanned, sFrom, sCmd) ;
  }
  tds.flush();
}

void CTournament::HandleCmdCancel() {
  for (CPlayerList::iterator pp=pl.begin(); pp!=pl.end(); pp++)
    tds << "t " << pp->sLogin << " " << Name() << " is cancelled.\n";
  tds << "t " << sChannel << " " << Name() << " is cancelled\n";
  tds.flush();
}

void CTournament::HandleCmdOpen(const string& sFrom) {
  tds << "t " << sFrom << "," << sChannel << " tournament " << idt
      << " is open (joining the tournament is possible)\n";
  fOpen = true;
}

void CTournament::HandleCmdClose(const string& sFrom) {
  tds << "t " << sFrom << "," << sChannel << " tournament " << idt
      << " is closed (joining the tournament is not possible)\n";
  fOpen = false;
}

/////////////////////////////////////////
// Tournament class : player commands
/////////////////////////////////////////

u4 CTournament::ErrJoin() const {
  QSSERT(0);
  return 0;
}

void CTournament::HandleCmdJoin(const string& sPlayer, const string& sFrom, const string& sCmd) {
  int err;

  if (sBanned.find(sPlayer)!=sBanned.end())
    err=kErrBanned;
  else if( fOpen )
    err=pl.Join(sPlayer);
  else
    err=kErrOpen;

  if (err)
    ErrOut(err, sFrom, sCmd);
  else {
    tds << "t " << sFrom << "," << sChannel << " " << sCmd << ": " << sPlayer << " joins " << Name() << "\n";
    if (sFrom!=sPlayer)
      tds << "t " << sPlayer << "," << sChannel << " " << sCmd << ": " << sPlayer << " joins " << Name() << "\n";
  }
}

void CTournament::HandleCmdWithdraw(const string& sPlayer, const string& sFrom, const string& sCmd) {
  int err=pl.Withdraw(sPlayer);
  if (err)
    ErrOut(err, sFrom, sCmd);
  else {
    tds << "t " << sFrom << " " << sCmd << ": " << sPlayer << " withdrawn from " << Name() << "\n";
    if (sFrom!=sPlayer)
      tds << "t " << sPlayer << " " << sCmd << ": " << sPlayer << " withdrawn from " << Name() << "\n";
  }
}

/////////////////////////////////////////
// Tournament class : user commands
/////////////////////////////////////////

void CTournament::HandleCmdFinger(const string& sFrom, const string& sCmd) const {
  OKOut(sFrom, sCmd)
    << "\\id        : " << idt
    << "\\joining   : " << (fOpen?"open":"closed")
    << "\\director  : " << sLoginDirector
    << "\\service   : " << sLoginService
    << "\\rounds    : " << nRoundsPlayed << "/" << nRounds
    << "\\style     : " << Style()
    << "\\clock     : " << ck
    << "\\type      : " << smt
    << "\\breaks    : ";
  TimeOut(tBreak);

  tds << "\\players   : ";
  pl.OutActive(tds);

  if (IsDirector(sFrom)) {
    tds << "\\banned    : " << sBanned.size();
    set<string>::iterator i;
    for (i=sBanned.begin(); i!=sBanned.end(); i++)
      tds << " " << *i;
  }
  if (fStopped)
    tds << "\\stopped";

  if (!fBreak)
    tds << "\\round " << nRoundsPlayed+1 << " is being played ";
  else if (nRoundsPlayed==nRounds)
    tds << "\\tournament has finished";
  else {
    tds << "\\round " << nRoundsPlayed+1 << " begins in ";
    TimeOut(tNextRound-time(NULL));
  }
}

void CTournament::HandleCmdSchedulePlayer(const string& sPlayer, const string& sFrom, const string& sCmd) const {
  u4 iRound, iMatch;

  // find player
  int idp=pl.LoginToIdp(sPlayer);

  if (idp==-1) {
    ErrOut(kErrPlayerNotInTournament, sFrom, sCmd);
    tds.flush();
    return;
  }

  OKOut(sFrom, sCmd) << "schedule for player " << sPlayer << " in " << Name() <<  ":\\";
  for (iRound=0; iRound<rounds.size(); iRound++) {
    const CScheduleRound& round=rounds[iRound];
    for (iMatch=0; iMatch<round.size() ; iMatch++) {
      const CScheduleMatch& match=round[iMatch];
      if (match.idps[0]==idp || match.idps[1]==idp)
        match.OutputSchedule(*this, iRound, iMatch);
    }
  }
  tds << "\n";
}

void CTournament::HandleCmdScheduleRound(u4 iRound, const string& sFrom, const string& sCmd) const {
  if (!Started()) {
    ErrOut(kErrNoSchedule, sFrom, sCmd);
  }

  if (iRound>=nRounds) {
    ErrOut(kErrBadRoundId, sFrom, sCmd);
    return;
  }
  if (iRound>=rounds.size()) {
    ErrOut(kErrUnscheduledRound, sFrom, sCmd);
    return;
  }

  OKOut(sFrom, sCmd) << "schedule for round " << iRound+1 << " in " << Name() << ":\\";
  rounds[iRound].OutputSchedule(*this, iRound);
  tds << "\n";
}

void CTournament::HandleCmdScheduleTournament(const string& sFrom, const string& sCmd) const {
  if (!Started()) {
    ErrOut(kErrNoSchedule,sFrom, sCmd);
    return;
  }

  OKOut(sFrom, sCmd) << "schedule for " << Name() <<  ":\\";
  u4 iRound;
  for (iRound=0; iRound<rounds.size(); iRound++)
    rounds[iRound].OutputSchedule(*this, iRound);
  tds << "\n";
}

void CTournament::HandleCmdRankings(const string& sFrom, const string& sCmd) const {
  vector<CTDRanking> rankings=CalculateRankings();

  int precision=tds.precision();
  ios_base::fmtflags flags=tds.setf(ios::fixed, ios::floatfield);
  u4 i;

  OKOut(sFrom, sCmd) << Name() << "\\";
  for (i=0; i<rankings.size(); i++) {
    const CTDRanking& rk=rankings[i];
    if (pl[rk.idp].Bye())
      continue;
    double bq=nRoundsPlayed?rk.bq*0.25:0;

    tds.precision(1);
    tds << setw(4) << rk.Demipoints()*0.5;
    tds << " ("
        << setw(2) << rk.nResult[2]+rk.nByes << " "
        << setw(2) << rk.nResult[1] << " "
        << setw(2) << rk.nResult[0]
        << ") { "
        << setw(max_score_width) << setprecision(2) << std::fixed << rk.discs/max(nRoundsPlayed,u4(1))
        << " } "
        << setw(8) << pl[rk.idp].sLogin.c_str();
    tds.precision(4);
    tds << " [" << bq << "]"
        << "\\";

  }
  tds.precision(precision);
  tds.flags(flags);
}

/////////////////////////////////////////
// Tournament class : server commands
/////////////////////////////////////////

void CTournament::DoIdle() {
  if (!fBreak  || nRoundsPlayed==nRounds || fStopped)
    return;

  time_t tDelta=tNextRound-time(NULL);
  if (tDelta<=0) {
    if (nRoundsPlayed==0) {
      if (StartOK())
        Start();
      else {
        fDead=true; // will be deleted
        HandleCmdCancel();
      }
    }
    else 
      RoundStart();
  }
  else if (nRoundsPlayed==0) {
    u4 iWarningLevelNew=WarningLevel(tDelta);
    if (iWarningLevelNew<iWarningLevel) {
      iWarningLevel=iWarningLevelNew;
      tds << "t " << sChannel << " " << Name() << " starts in ";
      TimeOut(tWarnings[iWarningLevel]) << "\n" << flush;
    }
  }
}

u4 CTournament::SetResult(const CTRM& trm, const CSGResult& result) {
  if (rounds[trm.iRound][trm.iMatch].result.status==CSGResult::kBye)
    return kErrCantForceBye;
  
  rounds[trm.iRound][trm.iMatch].result=result;
  
  // check to see if round is over
  if ((trm.iRound==nRoundsPlayed) && !rounds[trm.iRound].Active())
    RoundEnd();
  return 0;
}

/////////////////////////////////////////
// Tournament class : information
/////////////////////////////////////////

bool CTournament::Started() const {
  return !(fBreak && nRoundsPlayed==0);
}

bool CTournament::Ended() const {
  return nRoundsPlayed==nRounds;
}

bool CTournament::Dead() const {
  return fDead;
}

u4 CTournament::Idt() const {
  return idt;
}

void CTournament::SetIdt(u4 aidt) {
  idt=aidt;
}

u4 CTournament::NRoundsPlayed() const {
  return nRoundsPlayed;
}

string CTournament::Description() const {
  ostringstream os;

  os << smtDescription << " clock: " << ck << ", "
     << nRounds << " rounds " << Style();
  string sResult(os.str());

  return sResult;
}

bool CTournament::IsDirector(const string& sLogin) const {
  return sLogin==sLoginDirector;
}

/////////////////////////////////////////
// Tournament class : output functions
/////////////////////////////////////////

void CTournament::OutHeader() {
  tds << setw(5) << "id" << " "
      << setw(8) << "director"  << " "
      << setw(5) << "game" << " "
      << setw(3) << "#pl" << " "
      << setw(3+1+3) << "round" << " ";
  tds << "start";
  tds << " "
      << setw(17) << "clock" << " "
      << setw(10) << "type";
}

void CTournament::OutHeadline(ostream& tds) const {
  tds << setw(5) << idt << " "
      << setw(8) << sLoginDirector  << " "
      << setw(5) << sLoginService << " "
      << setw(3) << pl.NActive() << " "
      << setw(3) << nRoundsPlayed << "/" << setw(3) << nRounds << " ";
  if (!fBreak)
    tds << " now ";
  else if (nRoundsPlayed==nRounds)
    tds << " done";
  else
    TimeOut(tNextRound-time(NULL));
  stringstream stream;
  stream << ck;
  tds << " "
      << setw(17) << stream.str() << " "
      << setw(10) << smt;
  if (!sWinner.empty())
    tds << "   Winner: " << sWinner;
}

void CTournament::Out(ostream& os) const {
  os << "Style: " << Style() << "\n"
     << "Open: " << fOpen << "\n"
     << "Clock: " << ck << "\n"
     << "fBreak: " << fBreak << "\n"
     << "fDead: " << fDead << "\n"
     << "fStopped: " << fStopped << "\n"
     << "idt: " << idt << "\n"
     << "iWarningLevel: " << iWarningLevel << "\n"
     << "MatchType: " << smt << "\n"
     << "#Rounds: " << nRounds << "\n"
     << "#RoundsPlayed: " << nRoundsPlayed << "\n"
     << pl << "\n"
     << rounds;

  os << "Banned: " << sBanned.size() << " ";
  set<string>::iterator i;
  for (i=sBanned.begin(); i!=sBanned.end(); i++)
    os << *i << " ";
  os << "\n";

  os << "Director: " << sLoginDirector << "\n"
     << "Service: " << sLoginService << "\n"
     << "tBreak: " << tBreak << "\n"
     << "tNextRound: " << tNextRound << "\n";
}

void CTournament::In(istream& is) {
  int n;
  string s;
  is >> s >> fOpen; QSSERT(s=="Open:");
  is >> s >> ck; QSSERT(s=="Clock:");
  is >> s >> fBreak; QSSERT(s=="fBreak:");
  is >> s >> fDead; QSSERT(s=="fDead:");
  is >> s >> fStopped; QSSERT(s=="fStopped:");
  is >> s >> idt; QSSERT(s=="idt:");
  is >> s >> iWarningLevel; QSSERT(s=="iWarningLevel:");
  is >> s >> smt; QSSERT(s=="MatchType:");
  is >> s >> nRounds; QSSERT(s=="#Rounds:");
  is >> s >> nRoundsPlayed; QSSERT(s=="#RoundsPlayed:");
  is >> pl;
  is >> rounds;
  is >> s >> n; QSSERT(s=="Banned:");
  while (n--) {
    is >> s;
    sBanned.insert(s);
  }
  is >> s >> sLoginDirector;  QSSERT(s=="Director:");
  is >> s >> sLoginService; QSSERT(s=="Service:");
  is >> s >> tBreak;      QSSERT(s=="tBreak:");
  is >> s >> tNextRound;    QSSERT(s=="tNextRound:");

  GetTDType();

  if (nRounds==nRoundsPlayed)
    sWinner=pl[CalculateRankings()[0].idp].sLogin;
}

CTournament* CTournament::CreateFromStream(istream& is) {
  string s, style;
  CTournament* trn;

  trn=NULL;
  is >> s; QSSERT(s=="Style:");
  is >> style;
  if (style=="swiss")
    trn=new CTournamentSwiss;
  else if (style=="rrobin")
    trn=new CTournamentRoundRobin;
  
  if (trn)
    trn->In(is);

  return trn;
}


string CTournament::Name() const {
  stringstream ss;
  ss << "tournament " << idt;
  return ss.str();
}

/////////////////////////////////////////
// Tournament class : internal functions
/////////////////////////////////////////

string CTournament::Style() const {
  QSSERT(0);  // no pure virtual functions in MS VC if using STL
  return "";
}

vector<CTDRanking> CTournament::CalculateRankings() const {
  vector<CTDRanking> rankings;
  u4 nPlayers=pl.size();
  rankings.resize(nPlayers);
  u4 iRound, iMatch,iBlack, iWhite;


  for (iBlack=0; iBlack<nPlayers; iBlack++)
    rankings[iBlack].idp=iBlack;

  // calc points
  for (iRound=0; iRound<rounds.size(); iRound++) {
    const CScheduleRound& round=rounds[iRound];
    for (iMatch=0; iMatch<round.size(); iMatch++) {
      const CScheduleMatch& match=round[iMatch];
      COsResult result=match.result;
      iBlack=match.idps[1];
      iWhite=match.idps[0];
      QSSERT(iBlack<nPlayers);
      QSSERT(iWhite<nPlayers);

      switch(result.status) {
      case COsResult::kUnstarted:
      case COsResult::kUnfinished:
        break;
      case COsResult::kBye: // ignore to calc bq
        if (!pl[iBlack].Bye())
          rankings[iBlack].nByes++;
        if (!pl[iWhite].Bye())
          rankings[iWhite].nByes++;
        break;
      default:
        int iBlackPoints;
        if (result.dResult<0)
          iBlackPoints=0;
        else if (result.dResult>0)
          iBlackPoints=2;
        else
          iBlackPoints=1;
        rankings[iBlack].nResult[iBlackPoints]++;
        rankings[iWhite].nResult[2-iBlackPoints]++;
        double dWinFraction=(result.dResult)/dMaxResult;
        rankings[iBlack].fractionTotal+=dWinFraction;
        rankings[iWhite].fractionTotal-=dWinFraction;
        rankings[iBlack].discs+=result.dResult;
        rankings[iWhite].discs-=result.dResult;
        stringstream stream;
        if( result.dResult>0 )
          stream << setprecision(2) << std::fixed << -result.dResult;
        else
          stream << setprecision(2) << std::fixed << result.dResult;
        max_score_width = max(max_score_width, stream.str().size());
      }
    }
  }

  // calc BQ
  for (iRound=0; iRound<rounds.size(); iRound++) {
    for (iMatch=0; iMatch<rounds[iRound].size(); iMatch++) {
      const CScheduleMatch& match=rounds[iRound][iMatch];
      COsResult result=match.result;
      iBlack=match.idps[1];
      iWhite=match.idps[0];

      if (result.HasScore()) {
        double dWinFraction=(result.dResult)/dMaxResult;
        rankings[iBlack].bq+=1+dWinFraction+
          rankings[iWhite].WinPct2();
        rankings[iWhite].bq+=1-dWinFraction+
          rankings[iBlack].WinPct2();
      }
    }
  }

  /*
  // eliminate bye player
  if (idpSwing>=0 && sPlayers[idpSwing].empty())
  rankings.erase(rankings.begin()+idpSwing);
  */

  // average bq
  for (iBlack=0; iBlack<rankings.size(); iBlack++) {
    u4 nPlayed=rankings[iBlack].NPlayed();
    if (nPlayed)
      rankings[iBlack].bq/=nPlayed;
    else
      rankings[iBlack].bq=0;
  }

  sort(rankings.begin(), rankings.end());
  return rankings;
}

void CTournament::AddSwingPlayer() {
  int iSwing=-1;

  if (sLoginService=="/os") {
    COsMatchType mt(smt);
    if (mt.bt.nWidth==8 && mt.bt.nHeight==8 && mt.fAnti==false)
      iSwing = pl.AddSwing("ant");
  }

  if (iSwing<0)
    pl.AddSwing("");
}

bool CTournament::PlayerActive(int /*idp*/) const {
  return true;
}

// return random number in [0,1)
double rand01()
{
  return ((double)rand()/((double)(RAND_MAX)+1.0));
}


void CTournament::RandomizePlayers() {
  u4 i,j,n;
  CPlayer temp;

  n=pl.size();

  // if there's a swing player, put him last and don't randomize him
  //  so we know idpSwing
  if (pl.idpSwing>=0) {
    n--;
    temp=pl[n];
    pl[n]=pl[pl.idpSwing];
    pl[pl.idpSwing]=temp;
    pl.idpSwing=n;
  }

  // randomize remaining players
  for (i=n-1; i>0; i--) {
    // was: j=rand()*(i+1)/(1+RAND_MAX);

    j = (int) ((i+1)*rand01()); // 0..i
    QSSERT(j<=i);
    temp=pl[j];
    pl[j]=pl[i];
    pl[i]=temp;
  }
}

void CTournament::SetTBreak(int t) {
  tBreak=t;
  if (fBreak && nRoundsPlayed)
    SetTNextRoundStart(t);
}

void CTournament::SetTNextRoundStart(int tWait) {
  tNextRound=tWait+time(NULL);
  if (nRoundsPlayed) {
    tds << "t " << sChannel << " " << Name();
    if (nRoundsPlayed==0)
      tds << " will start in ";
    else 
      tds << " round " << nRoundsPlayed+1 << " starts in ";
    TimeOut(tWait) << "\n";
    tds.flush();
  }
  iWarningLevel=WarningLevel(tWait);
}

bool CTournament::StartOK() {
  return pl.NActive()>=2;
}
  
int CTournament::tWarnings[6] = { 10, 60, 2*60, 5*60, 15*60, 30*60 };

u4 CTournament::WarningLevel(int t) {
  return lower_bound(tWarnings, tWarnings+6, t)-tWarnings;
}

// before a tournament starts, sPlayers is in alphabetical order
//  after it starts, sPlayers is in a random order and has bye ("") if
//  there were an odd number of players

void CTournament::Start() {
  fBreak=false;

  // start first round
  RoundStart();
}

string CTournament::MatchId(u4 iRound, u4 iMatch) const {
  ostringstream tds;
  tds << idt << "." << iRound+1 << "." << iMatch+1;
  string sResult = tds.str();
  return sResult;
}

void CTournament::MatchStart(u4 iMatch) {
  u4 iBlack, iWhite;

  CScheduleRound& round=rounds[nRoundsPlayed];
  CScheduleMatch& match=round[iMatch];

  iBlack=match.idps[1];
  iWhite=match.idps[0];

  //RoundMatchToPlayers(nRoundsPlayed, iMatch, iBlack, iWhite);

  const string& sBlack=pl[iBlack].sLogin;
  const string& sWhite=pl[iWhite].sLogin;

  // new code, too bad if you're not here
  if (sBlack.empty() || sWhite.empty())
    match.result.status=COsResult::kBye;
  else {
    match.result.status=COsResult::kUnfinished;
    tds << "t " << sLoginService << " tdstart " 
        << MatchId(nRoundsPlayed, iMatch) << " " 
        << smtOut << " "
        << sBlack << " " << ck << " "
        << sWhite << " " << ck << " "
        << "\n";
  }
}

extern CTournaments trns;

void CTournament::RoundStart() {
  u4 iMatch;
  u4 nMatches=rounds[nRoundsPlayed].size();
  fBreak=false;

  tds << "t " << sChannel << " starting round " << nRoundsPlayed+1 << " of " << Name() << "\n";
  
  for (iMatch=0; iMatch<nMatches; iMatch++)
    MatchStart(iMatch);
  tds.flush();

  trns.Backup();
}

void CTournament::RoundEnd() {
  fBreak=true;

  tds << "t " << sChannel << " ending round " << nRoundsPlayed+1 << " of " << Name() << "\\";
  
  rounds[nRoundsPlayed].OutputSchedule(*this, nRoundsPlayed);
  tds << "\n";
  tds.flush();

  nRoundsPlayed++;

  if (nRoundsPlayed==nRounds)
    TournamentEnd();
  else 
    SetTNextRoundStart(tBreak);
  tds.flush();

  trns.Backup();
}

void CTournament::TournamentEnd() {
  tds << "t " << sChannel << " " << Name() << " is over. "
      << "\\tell " << sMyLogin << " r " << idt << " for final rankings, or"
      << "\\tell " << sMyLogin << " st " << idt << " for game-by-game results\n";
  if (!fTesting) {
    sWinner=pl[CalculateRankings()[0].idp].sLogin;
    tds << "t .chat Congratulations to " << sWinner
        << ", winner of tournament " << idt << "\n";
  }
}

////////////////////////////////////////////
// CTournamentRoundRobin
////////////////////////////////////////////

CTournamentRoundRobin::CTournamentRoundRobin(const string& asLoginDirector, const string& asLoginService,
                                             const CSGClock& ack, const string& asmt, u4 anRounds,
                                             u4 anMinutesToStart) : CTournament(asLoginDirector,
                                                                                asLoginService, ack, asmt, anRounds, anMinutesToStart) {
}

string CTournamentRoundRobin::Style() const {
  return "rrobin";
}

u4 CTournamentRoundRobin::ErrJoin() const {
  if (Started())
    return kErrTournamentStarted;
  else
    return 0;
}

void CTournamentRoundRobin::RoundStart() {
  if (nRoundsPlayed==0)
    ScheduleTournament();
  CTournament::RoundStart();
}

void CTournamentRoundRobin::ScheduleTournament() {
  // randomize player order
  RandomizePlayers();
  pl.RemoveWithdrawn();

  // add swing player
  if (pl.NActive()&1)
    AddSwingPlayer();

  // calc # of rounds
  u4 N = pl.NActive()-1;
  if (N*2 < nRounds)
    nRounds=N*2;

  // create results struct
  CScheduleRound round;
  u4 nMatches=(pl.NActive()+1)/2;
  round.resize(nMatches); 
  rounds.resize(nRounds, round);

  // set players
  u4 iRound, iMatch;
  for (iRound=0; iRound<nRounds; iRound++) {
    for (iMatch=0; iMatch<nMatches; iMatch++) {
      CScheduleMatch& match=rounds[iRound][iMatch];
      RoundMatchToPlayers(iRound, iMatch, match.idps[1], match.idps[0]);
      if (pl[match.idps[0]].Bye() || pl[match.idps[1]].Bye())
        match.result.status=CSGResult::kBye;
      else
        match.result.status=CSGResult::kUnstarted;
    }
  }
}

u4 CTournamentRoundRobin::PlayerRoundToMatch(int iPlayer, u4 iRound) const {
  QSSERT(iPlayer>=0);
  u4 i=(u4)iPlayer;
  u4 r,result,N;
  N=pl.size()-1;

  r=iRound;
  if (r>=N)
    r-=N; // reverse matches
  if (iRound&1)
    r+=N;
  r/=2;

  if (i==N || i==r)
    return 0;
  if (r>i)
    result= r-i;
  else
    result=i-r;
  if (result>N/2)
    result-=N/2;

  return result;
}

void CTournamentRoundRobin::RoundMatchToPlayers(u4 iRound, u4 iMatch, int& iBlack, int& iWhite) const {
  int N=pl.size()-1;
  int p1,p2,r;
  bool fReverseMatches = int(iRound)>=N;
  // midpoint, games are between players r+iMatch and r-iMatch (mod N)
  r=iRound;
  if (fReverseMatches)
    r-=N;
  if (r&1)
    r+=N;
  r/=2;

  // special match
  if (iMatch==0) {
    p1=N;
    p2=r;
  }
  // other matches
  else {
    p1=r-iMatch;
    if (p1<0)
      p1+=N;
    p2=r+iMatch;
    if (p2>=N)
      p2-=N;
  }

  // calc color
  if ((p1<p2)^((p1+p2)&1)^(fReverseMatches)) {
    iBlack=p1;
    iWhite=p2;
  }
  else {
    iBlack=p2;
    iWhite=p1;
  }
}

////////////////////////////////////////////
// CTournamentSwiss
////////////////////////////////////////////

CTournamentSwiss::CTournamentSwiss(const string& asLoginDirector, const string& asLoginService,
                                   const CSGClock& ack, const string& asmt, u4 anRounds,
                                   u4 anMinutesToStart) : CTournament(asLoginDirector,
                                                                      asLoginService, ack, asmt, anRounds, anMinutesToStart) {
}

string CTournamentSwiss::Style() const {
  return "swiss";
}

u4 CTournamentSwiss::ErrJoin() const {
  if (Ended())
    return kErrTournamentEnded;
  else
    return 0;
}

void CTournamentSwiss::RoundStart() {
  if (nRoundsPlayed==0) {
    AddSwingPlayer();
    RandomizePlayers();
  }

  ScheduleNextRound();
  CTournament::RoundStart();
}

void CTournamentSwiss::ScheduleNextRound() {
  u4 i, nPlaying=pl.NActive(), nTotal=pl.size();
  vector<bool> fMatched;
  vector<int>  rankToIdp;
  u4 nNodes=0;
  
  // don't use swing player if he makes an odd number of players
  bool fNoSwing=nPlaying&1;
  if (fNoSwing)
    nPlaying--;

  // initialize fMatched and rankToIdp
  fMatched.resize(nPlaying, false);
  rankToIdp.resize(nPlaying);


  if (nRoundsPlayed) {
    vector<CTDRanking> rankings = CalculateRankings();
    nPlaying=0;
    for (i=0; i<nTotal; i++) {
      int idp=rankings[i].idp;
      if (pl[idp].Active() && !(fNoSwing && idp==pl.idpSwing)) {
        rankToIdp[nPlaying++]=idp;
      }
    }
  }
  else {
    nPlaying=0;
    for (i=0; i<nTotal; i++) {
      int idp=i;
      if (pl[idp].Active() && !(fNoSwing && idp==pl.idpSwing)) {
        rankToIdp[nPlaying++]=idp;
      }
    }
  }

  QSSERT((nPlaying&1)==0);

  // create results struct
  u4 nMatches=nPlaying/2;
  rounds.resize(nRoundsPlayed+1);
  rounds[nRoundsPlayed].resize(nMatches); 

  // init idpToRank
  vector<int> idpToRank;
  idpToRank.resize(pl.size(),-1);
  u4 rank;
  for (rank=0; rank<nPlaying; rank++)
    idpToRank[rankToIdp[rank]]=rank;

  u4 *nPreplayed = CalcPreplayed(idpToRank, nPlaying);

  u4 beta;
  for (beta=1; beta<=ScheduleSub(fMatched, nPreplayed, -1, 0, beta, nNodes) ; beta*=2)
    ;

  cout << "NNodes: " << nNodes << "\n";

  // fix schedules so they are in terms of idp instead of rank
  CScheduleRound& round=rounds[nRoundsPlayed];
  u4 iMatch;
  for (iMatch=0; iMatch<round.size(); iMatch++) {
    CScheduleMatch& match=round[iMatch];
    match.idps[0]=rankToIdp[match.idps[0]];
    match.idps[1]=rankToIdp[match.idps[1]];
  }

  delete [] nPreplayed;

  AssignColors();
}

// find a subtournament with fewer than beta preplays. Return number of preplays
u4 CTournamentSwiss::ScheduleSub(vector<bool>& fMatched, u4* nPreplayed,
                                 int irLastTop,
                                 u4 nMatches, u4 beta, u4& nNodes) {
  u4 n=fMatched.size();
  u4 irTop, irNext;

  // find top player
  for (irTop=irLastTop+1; irTop<n && fMatched[irTop]; irTop++)
    ;
  
  // all players scheduled
  if (irTop==n)
    return 0;
  
  // find next player
  for (irNext=irTop+1; irNext<n && fMatched[irNext]; irNext++)
    ;
  
  // we shouldn't have an odd number of players
  if (irNext==n) {
    QSSERT(0);
    return 0;
  }


  u4 irOpp,cost;

  // try players with no preplays first
  for (irOpp=irNext; irOpp<n; irOpp++) {
    if (fMatched[irOpp])
      continue;
    if (!nPreplayed[n*irTop+irOpp]) {
      cost=ScheduleSubMatch(fMatched, nPreplayed, irTop, irOpp, nMatches, beta, nNodes);
      if (cost<beta)
        return cost;
    }
  }

  // try players with preplays next
  for (irOpp=irNext; irOpp<n; irOpp++) {
    if (fMatched[irOpp])
      continue;
    u4 npp=nPreplayed[n*irTop+irOpp];
    if (npp && npp<beta) {
      cost=npp+ScheduleSubMatch(fMatched, nPreplayed, irTop, irOpp, nMatches, beta-npp, nNodes);
      if (cost<beta)
        return cost;
    }
  }

  return beta;
}

// add a match and schedule subtournament. beta has already been charged for the
//  penalty for this match
u4 CTournamentSwiss::ScheduleSubMatch(vector<bool>& fMatched, u4* nPreplayed, int irTop,
                                      u4 irOpp, u4 nMatches, u4 beta, u4& nNodes) {
  nNodes++;

  // make move.
  fMatched[irTop]=true;
  fMatched[irOpp]=true;

  CScheduleMatch& match=rounds[nRoundsPlayed][nMatches];
  match.idps[0]=irTop;
  match.idps[1]=irOpp;

  u4 cost=ScheduleSub(fMatched, nPreplayed, irTop, nMatches+1, beta, nNodes);

  //undo move
  fMatched[irOpp]=false;
  fMatched[irTop]=false;

  return cost;
}

void CTournamentSwiss::AssignColors() {
  vector<int> netblacks;
  bool fSwap;
  int nb0, nb1, temp;
  u4 iMatch;
  int idpBye=pl.IdpBye();

  CalcNetBlacks(netblacks);

  CScheduleRound& round=rounds[nRoundsPlayed];
  for (iMatch=0; iMatch<round.size();iMatch++) {
    CScheduleMatch& match=round[iMatch];
    if (match.idps[0]==idpBye || match.idps[1]==idpBye)
      continue;
    nb0=netblacks[match.idps[0]];
    nb1=netblacks[match.idps[1]];
    if (nb1==nb0)
      fSwap= rand01() >= 0.5;
    else
      fSwap=nb1>nb0;

    if (fSwap) {
      temp=match.idps[0];
      match.idps[0]=match.idps[1];
      match.idps[1]=temp;
    }
  }
}

void CTournamentSwiss::CalcNetBlacks(vector<int>& netblacks) const {
  // calculate previous netblacks
  u4 iRound, iMatch;
  netblacks.resize(pl.size(),0);
  for (iRound=0; iRound<nRoundsPlayed; iRound++) {
    const CScheduleRound& round=rounds[iRound];
    for (iMatch=0; iMatch<round.size(); iMatch++) {
      const CScheduleMatch& match=round[iMatch];
      if (match.result.HasScore()) {
        netblacks[match.idps[0]]--;
        netblacks[match.idps[1]]++;
      }
    }
  }

}

u4* CTournamentSwiss::CalcPreplayed(const vector<int>& idpToRank, u4 nPlaying) {
  u4 iRound, iMatch;
  u4 n=nPlaying;
  
  u4* nPreplayed=new u4[n*n];

  // clear
  u4 i,j;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      nPreplayed[n*i+j]=0;
    }
  }

  // count
  for (iRound=0; iRound<nRoundsPlayed; iRound++) {
    CScheduleRound& round=rounds[iRound];
    for (iMatch=0; iMatch<round.size(); iMatch++) {
      int* idps=round[iMatch].idps;
      int rank0=idpToRank[idps[0]];
      int rank1=idpToRank[idps[1]];

      if (rank0>=0 && rank1>=0) {
        nPreplayed[n*rank0+rank1]++;
        nPreplayed[n*rank1+rank0]++;
      }
    }
  }

  return nPreplayed;
}
