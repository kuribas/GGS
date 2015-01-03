// $Id: Tournament.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#ifndef H_TOURNAMENT
#define H_TOURNAMENT

#include <vector>
#include <map>
#include <iostream>
#include <string>
using namespace std;

#include "SGObjects.h"
#include "ODKStream.h"

class CTournament;

class CScheduleMatch {
 public:
  CScheduleMatch();

  int idps[2];	// player ids
  CSGResult result;

  bool operator<(const CScheduleMatch& b) const { return this<&b; }
  bool operator==(const CScheduleMatch& b) const { return this==&b; }

  void OutputSchedule(const CTournament& trn, u4 iRound, u4 iMatch) const;

  bool Active() const;


  void Out(ostream& os) const;
  void In(istream& is);
};

inline ostream& operator<<(ostream& os, const CScheduleMatch& sm) { sm.Out(os); return os; }
inline istream& operator>>(istream& is, CScheduleMatch& sm) { sm.In(is); return is; }

class CScheduleRound : public vector<CScheduleMatch> {
 public:
  bool operator<(const CScheduleRound& b) const { return this<&b; }
  bool operator==(const CScheduleRound& b) const { return this==&b; }

  void OutputSchedule(const CTournament& trn, u4 iRound) const;

  bool Active() const;

  void Out(ostream& os) const;
  void In(istream& is);
};

inline ostream& operator<<(ostream& os, const CScheduleRound& sr) { sr.Out(os); return os; }
inline istream& operator>>(istream& is, CScheduleRound& sr) { sr.In(is); return is; }

class CSchedule : public vector<CScheduleRound> {
 public:
  void Out(ostream& os) const;
  void In(istream& is);
};

inline ostream& operator<<(ostream& os, const CSchedule& sch) { sch.Out(os); return os; }
inline istream& operator>>(istream& is, CSchedule& sch) { sch.In(is); return is; }

class CTDRanking {
 public:
  CTDRanking();

  int idp;
  u4 nResult[3];
  u4 nByes;
  double discs;
  double fractionTotal;
  double bq;

  u4 Demipoints() const;
  u4 DemipointsNoByes() const;
  u4 NPlayed() const;
  double WinPct2() const;
  double WinFraction2() const;

  bool operator<(const CTDRanking&  b) const;
  bool operator==(const CTDRanking&  b) const;
};

class CPlayer {
 public:
  string sLogin;
  bool fWithdrawn;

  bool Bye() const;
  bool Active() const;

  bool operator<(const CPlayer&  b) const { return sLogin<b.sLogin; };
  bool operator==(const CPlayer&  b) const { return sLogin==b.sLogin; };

  void Out(ostream& os) const;
  void OutIfActive(ostream& os, bool fSwing) const;
  void In(istream& is);
};

inline ostream& operator<<(ostream& os, const CPlayer& pl) { pl.Out(os); return os; }
inline istream& operator>>(istream& is, CPlayer& pl) { pl.In(is); return is; }

class CPlayerList : public vector<CPlayer> {
 public:
  int idpSwing;

  CPlayerList();

  u4 Join(const string& sLogin);
  u4 Withdraw(const string& sLogin);
  int AddSwing(const string& sLogin);
  void RemoveWithdrawn();

  bool IsActive(const string& sLogin) const;
  int LoginToIdp(const string& sLogin) const;
  const string& IdpToLogin(int idp) const;

  int IdpBye() const;

  u4 NActive() const;
  u4 NActiveNoSwing() const;

  void Out(ostream& os) const;
  void OutActive(ostream& os) const;
  void In(istream& is);
};

inline ostream& operator<<(ostream& os, const CPlayerList& pl) { pl.Out(os); return os; }
inline istream& operator>>(istream& is, CPlayerList& pl) { pl.In(is); return is; }

class CTournament {
 public:
  CTournament() :fStopped(0) {};	// used when reading from stream, also makes VC happy
  CTournament(const string& sLoginDirector, const string& sLoginService,
              const CSGClock& ck,
              const string& smt, u4 nRounds, u4 nMinutesToStart);

  // Director commands
  virtual void HandleCmdCancel();
  virtual void HandleCmdBan(const string& sPlayer, const string& sFrom, const string& sCmd);
  virtual void HandleCmdUnban(const string& sPlayer, const string& sFrom, const string& sCmd);
  virtual void HandleCmdBreak(u4 nMinutes, const string& sFrom, const string& sCmd);
  virtual void HandleCmdDelay(u4 nMinutes, const string& sFrom, const string& sCmd);
  virtual void HandleCmdStop(const string& sFrom, const string& sCmd);
  virtual void HandleCmdUnstop(const string& sFrom, const string& sCmd);
  virtual void HandleCmdOpen(const string& sFrom);
  virtual void HandleCmdClose(const string& sFrom);

  // Player commands
  virtual void HandleCmdJoin(const string& sPlayer, const string& sFrom, const string& sCmd);
  virtual void HandleCmdWithdraw( const string& sPlayer, const string& sFrom, const string& sCmd);

  // user info
  virtual void HandleCmdFinger(const string& sFrom, const string& sCmd) const;
  virtual void HandleCmdSchedulePlayer(const string& sLogin, const string& sFrom, const string& sCmd) const;
  virtual void HandleCmdScheduleRound(u4 iRound, const string& sFrom, const string& sCmd) const;
  virtual void HandleCmdScheduleTournament(const string& sFrom, const string& sCmd) const;
  virtual void HandleCmdRankings(const string& sFrom, const string& sCmd) const;

  // server commands
  virtual void DoIdle();
  virtual u4 SetResult(const CTRM& trm, const CSGResult& result);

  // Info
  virtual string Name() const;
  virtual u4 Idt() const;
  virtual void SetIdt(u4);
  virtual string Description() const;

  virtual bool Started() const;
  virtual bool Ended() const;
  virtual bool Dead() const;

  virtual u4 NRoundsPlayed() const;

  virtual bool IsDirector(const string& sLogin) const;
  bool CheckSchedule(const string& sFrom, const string& sCmd);

  // I/O
  virtual void OutHeadline(ostream& os) const;
  static void OutHeader();
  virtual void Out(ostream& os) const;
  virtual void In(istream& is);
  static CTournament* CreateFromStream(istream& is);

  // operators
  bool operator<(const CTournament& b) const { return idt < b.idt; }
  bool operator==(const CTournament& b) const { return idt == b.idt; }

  // available commands
  virtual u4 ErrDelay(const string& sFrom) const;
  virtual u4 ErrJoin() const;

  CPlayerList pl;

 protected:
  virtual void Start();
  virtual void RoundStart();
  virtual void MatchStart(u4 iMatch);
  virtual void RoundEnd();
  virtual void TournamentEnd();

  virtual string MatchId(u4 iRound, u4 iMatch) const;
  virtual bool PlayerActive(int idp) const;

  virtual bool StartOK();
  virtual void RandomizePlayers();

  virtual void SetTBreak(int t);
  virtual void SetTNextRoundStart(int t);
  virtual void GetTDType();

  virtual vector<CTDRanking> CalculateRankings() const;
  void AddSwingPlayer();

  string smt, smtOut, smtDescription;
  double dMaxResult;
  CSGClock ck;
  string sLoginDirector;
  string sLoginService;
  CSchedule rounds;
  u4 idt;
  set<string> sBanned;
  bool fDead, fStopped;
  string sWinner;
  static size_t max_score_width;
  bool fOpen;


  // timing
  bool fBreak;
  time_t tNextRound;
  u4 nRoundsPlayed, nRounds;
  u4 tBreak;
  u4 iWarningLevel;

  static u4 WarningLevel(int t);
  static int tWarnings[6];

  virtual string Style() const;

 public:
  virtual ~CTournament() {}
};

inline ostream& operator<<(ostream& os, const CTournament& trn) { trn.Out(os); return os; }
inline istream& operator>>(istream& is, CTournament& trn) { trn.In(is); return is; }

class CTournamentSwiss: public CTournament {
 public:
  CTournamentSwiss(const string& sLoginDirector, const string& sLoginService,
                   const CSGClock& ck, const string& smt, u4 nRounds, u4 nMinutesToStart);
  CTournamentSwiss() {};	// for serialization
 protected:
  virtual string Style() const;

  virtual void RoundStart();
  void ScheduleNextRound();

  virtual u4 ErrJoin() const;
 private:
  u4 ScheduleSub(vector<bool>& fMatched, u4* nPreplayed, int iLastTop,
                 u4 nMatches, u4 beta, u4& nNodes);
  u4 ScheduleSubMatch(vector<bool>& fMatched, u4* nPreplayed, int iTop,
                      u4 iOpp, u4 nMatches, u4 beta, u4& nNodes);
  void AssignColors();

  u4* CalcPreplayed(const vector<int>& idpToRank, u4 nPlaying);
  void CalcNetBlacks(vector<int>& netblacks) const;

 public:
  virtual ~CTournamentSwiss() {};
};

class CTournamentRoundRobin: public CTournament {
 public:
  CTournamentRoundRobin(const string& sLoginDirector, const string& sLoginService,
                        const CSGClock& ck, const string& smt, u4 nRounds, u4 nMinutesToStart);
  CTournamentRoundRobin() {};	// serialization

  virtual u4 ErrJoin() const;

 protected:
  virtual string Style() const;

  virtual void RoundStart();
  virtual void ScheduleTournament();

  void RoundMatchToPlayers(u4 iRound, u4 iMatch, int& iBlack, int& iWhite) const;
  u4 PlayerRoundToMatch(int i, u4 iRound) const;

 public:
  virtual ~CTournamentRoundRobin() {};
};

class CTournaments : public map<u4, CTournament*> {
 public:
  void List() const;
  CTournaments();
  ~CTournaments();

  enum {kAny, kUnstarted, kStarted, kJoinable};

  map<u4, CTournament*>::iterator erase(map<u4, CTournament*>::iterator i) {
    map<u4, CTournament*>::iterator j;
    j=i;
    j++;
    delete (*i).second;
    map<u4, CTournament*>::erase(i);
    return j;
  }
  
  u4 erase(u4 idt);

  CTournament* FindTournament( istream& is, u4& idt, const string& sFrom,
                               const string& sCmd, u4 fFlags=kAny);

  u4 InsertTournament(CTournament* pTrn);

  void Out(ostream& os) const;
  void In(istream& is);

  void Backup() const;
  void Restore();

  u4 nIdts;
};


#endif // H_TOURNAMENT

