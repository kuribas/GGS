// $Id: ggsstream.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#ifndef GDK_GGSSTREAM_H
#define GDK_GGSSTREAM_H


#include "sockbuf.h"
#include <set>
#include <map>
using namespace std;

class CMsg;
class CSGBase;

#include "GGSMessage.h"
#include "SGBase.h"

class ggsstream : public iostream {
public:
  // construction/destruction
  ggsstream();
  virtual ~ggsstream();

  // Connection, disconnection
  virtual int Connect(const string& sServer, int nPort, bool logging=false);
  virtual int Disconnect();

  // login, logout
  virtual int Login(const char* sLogin, const char* sPassword);
  virtual int Logout();
  virtual const string& GetLogin() const;
  virtual int GetLevel() const;

  // turn stream data into messages
  virtual void Process();

  // Information
  virtual bool IsConnected() const;
  virtual bool IsLoggedIn() const;

  // return an error description
  static const char* ErrText(int err);
  enum { kErrUnknown=0x8200, kErrBadPassword,
	 kErrLoggedIn, kErrLoggedOut, kErrConnected, kErrNotConnected,
	 kErrNoStreambuf, kErrUserCancelled };

  // Handle messages relating to persistent data
  void BaseLogin		();
  void BaseUserDelta	(const CMsgGGSUserDelta& msg);
  void BaseDisconnect	();

  // Handle messages
  virtual void Handle				(const CMsg& msg);
  virtual void HandleAlias		(const CMsgGGSAlias& msg);
  virtual void HandleDisconnect	();
  virtual void HandleErr			(const CMsgGGSErr& msg);
  virtual void HandleFinger		(const CMsgGGSFinger& msg);
  virtual void HandleHelp			(const CMsgGGSHelp& msg);
  virtual void HandleLogin		();
  virtual void HandleTell			(const CMsgGGSTell& msg);
  virtual void HandleUnknown		(const CMsgGGSUnknown& msg);
  virtual void HandleUserDelta	(const CMsgGGSUserDelta& msg);
  virtual void HandleWho			(const CMsgGGSWho& msg);

  // persistent 
  map<string,int> loginToLevel;

  virtual void DirectMessage(string& sMsg);
  CSGBase* PService(const string& sUserLogin);
  bool HasService(const string& sUserLogin) const;

protected:
  virtual CSGBase* CreateService(const string& sUserLogin) = 0;
  virtual void DestroyService(const string& sUserLogin);
  virtual void DestroyAllServices();

  virtual int await(const char* sAwait);

  // turn stream data into messages
  virtual void ProcessLine(string& sLine);
  virtual void ProcessMessage();

  // parse messages
  virtual void Parse(const string& sMsg, const string& sFrom, istream& is);

  // post messages
  virtual void Post(string& sMsg);

  bool fLoggedIn;
  string sLogin;

  map<string, CSGBase*> loginToPService;

private:
  string sMsg;
  sockbuf *psockbuf;
};

#endif	//GDK_GGSSTREAM_H
