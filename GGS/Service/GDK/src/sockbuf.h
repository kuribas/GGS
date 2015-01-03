// $Id: sockbuf.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#ifndef GDK_SOCKBUF_H
#define GDK_SOCKBUF_H

#include "types.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#ifdef _WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

class sockbuf : public streambuf {
public:
  // construction/destruction
  sockbuf(bool logging=false);
  virtual ~sockbuf();

  // overrides
  virtual int underflow();
  virtual int overflow(int c=EOF);
  virtual int sync();

  // errors
  enum { kErrUnknown=0x8600, kErrCantStartup, kErrNoHost, kErrNoProtocol, kErrNoSocket,
	 kErrCantConnect, kErrConnectionReset, kErrConnectionClosed,
	 kErrNotConnected, kErrAlreadyConnected };
  int Err() const;

  bool IsConnected() const;

  int connect(const string& sServer, int nPort);
  int disconnect();

protected:
  enum { nBufSize=1024 };

  bool fConnected;

#ifdef _WIN32
  SOCKET sock;
#else
  int sock;
#endif
  ofstream *fplog;
  enum {kLogNone, kLogRecv, kLogSend} loglast;
  char buf[nBufSize*2];
  int err;
};

#endif // GDK_SOCKBUF_H
