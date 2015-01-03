// $Id: sockbuf.C 9037 2010-07-06 04:05:44Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#include "types.h"
#include "sockbuf.h"
#include <fstream>
#include <iomanip>
#include <cassert>
#include <cstring>

using namespace std;

sockbuf::sockbuf(bool logging) {
  // log incoming data to file?
  fplog=NULL;
  loglast=kLogNone;
  if (logging) {
    fplog=new ofstream("recv.txt");
  }

  // create get buffer
  setg(buf,buf+nBufSize, buf+nBufSize);
  setp(buf+nBufSize,buf+2*nBufSize);
  err=0;

  fConnected=false;
}

sockbuf::~sockbuf() {
  if (fConnected)
    disconnect();

#ifdef _WIN32
  WSACleanup();
#endif

  if (fplog) {
    if (fplog->is_open()) fplog->close();
    delete fplog;
    fplog = NULL;
  }
}

#ifndef _WIN32
#define HOSTENT        struct hostent
#define PROTOENT       struct protoent
#define SOCKADDR_IN    struct sockaddr_in
#define SOCKADDR       struct sockaddr
#define SOCKET_ERROR   (-1)
#define closesocket(x) close(x)

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

int sockbuf::connect(const string& sServer, int nPort) {
	HOSTENT *hostent;
	PROTOENT *protoent;
	SOCKADDR_IN sa;
	char *sProtocol="tcp";

	if (err)
		return err;

	if (fConnected) {
		QSSERT(0);
		return kErrAlreadyConnected;
	}

#ifdef _WIN32
	// Windows sockets initialization
	int wsaerr;
	WSADATA wsadata;

	wsaerr=WSAStartup(MAKEWORD(1,1), &wsadata);
	if (wsaerr || wsadata.wVersion!=MAKEWORD(1,1))
		return kErrCantStartup;
#endif

	// get connection location and type
	if (!(hostent=gethostbyname(sServer.c_str())))
		return kErrNoHost;
	if (!(protoent=getprotobyname(sProtocol)))
		return kErrNoProtocol;

	sa.sin_family=AF_INET;
	sa.sin_port=htons(nPort);

#ifndef _WIN32
	bcopy( hostent->h_addr, (&sa.sin_addr), hostent->h_length );
#else	
	sa.sin_addr.S_un.S_addr=*(u4*)hostent->h_addr_list[0];
#endif
	
	// get socket
	if (!(sock=socket(AF_INET, SOCK_STREAM, protoent->p_proto)))
		return kErrNoSocket;
	
	// connect
	if (::connect(sock,(SOCKADDR*)&sa,sizeof(sa))) {
		closesocket(sock);
		return kErrCantConnect;
	}

	fConnected=true;
	return 0;
}

int sockbuf::disconnect() {
	if (fConnected) {

		closesocket(sock);
		fConnected=false;
		return 0;
	}
	else {
		return kErrNotConnected;
	}
}

bool sockbuf::IsConnected() const {
	return fConnected;
}

int sockbuf::underflow() {
	char* p0;
	int nGetSize;

	if (!fConnected || err || eback()==NULL)
		return EOF;

	p0=eback();
	nGetSize=nBufSize;

	int nrecv=recv(sock, p0, nGetSize,0);
	if (nrecv==SOCKET_ERROR) {
#ifdef _WIN32
		int wsaerr=WSAGetLastError();
		if (wsaerr!=WSAEMSGSIZE) {
			switch(wsaerr) {
			case WSAECONNRESET:
				err=kErrConnectionReset;
				break;
			// we get a WSAENOTSOCK error when the connection has been closed
			//	by our program, since it deleted the socket
			case WSAENOTSOCK:		// win NT, 98
			case WSAECONNABORTED:	// win XP
				err=kErrUnknown;
				break;
			default:
				err=kErrUnknown;
				QSSERT(0);
			}
			return EOF;
		}
#else	// not windows, don't know the error codes
		err=kErrUnknown;
		return EOF;
#endif
	}
	if (nrecv==0) {
		// connection closed
		err=kErrConnectionClosed;
		return EOF;
	}
	else {
		if (gptr())
			setg(p0, p0, p0+nrecv);
		//else
		//	setg(unbuf, unbuf-1, unbuf+1);
		if (fplog) {
			if (loglast!=kLogRecv) {
				loglast=kLogRecv;
				fplog->write("[recv]",6);
			}
			fplog->write(p0, nrecv);
			*fplog << flush;
		}
		return *p0;
	}
}

int sockbuf::overflow(int c) {
	if (!fConnected || err)
		return EOF;

	int nSend=pptr()-pbase();
	int nSent=send(sock, pbase(), nSend,0);
	bool fOK=nSend==nSent;

	QSSERT(fOK);
	if (fplog) {
		if (loglast!=kLogSend) {
			loglast=kLogSend;
			fplog->write("[send]",6);
		}
		fplog->write(pbase(), pptr()-pbase());
	}
	if (fOK && c!=EOF) {
		char cc=char(c);
		nSent=send(sock, &cc, 1, 0);
		fOK=nSent==1;
		if (fplog) {
			fplog->write(&cc,1);
		}
	}
	if (fplog)
		fplog->flush();

	// clear put area
	setp(pbase(),epptr());

	return fOK?0:EOF;
}

int sockbuf::sync() {
	return overflow(EOF);
}

int sockbuf::Err() const {
	return err;
}
