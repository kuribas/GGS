// $Id: GDKStream.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved

#include "types.h"

#include "GDKStream.h"
#include "GetMove.h"
#include "OsObjects.h"
#include "AmsObjects.h"
#include "CksObjects.h"

#include <iostream>
using namespace std;
#include <time.h>

void CGDKStream::Handle(const CMsg& msg) {
	cout << msg.sRawText << "\n";
}

void CGDKStream::HandleLogin() {
	BaseLogin();
	(*this) << "mso\n"
		<< "msck\n"
		<< "ts open 1\n"
		<< "ta 8 n2\n";
	flush();
}

void CGDKStream::HandleTell(const CMsgGGSTell& msg) {
	cout << msg.sFrom << " " << msg.sText << "\n";
	if (msg.sFrom=="n2") {
		if (msg.sText=="quit")
			Logout();
		else {
			(*this) << msg.sText << "\n";
			flush();
		}
	}
}

void CGDKStream::HandleUnknown(const CMsgGGSUnknown& msg) {
	cout << "Unknown GGS message: \n";
	Handle(msg);
}

CSGBase* CGDKStream::CreateService(const string& sUserLogin) {
	CSGBase* pService=NULL;
	if (!HasService(sUserLogin)) {
		if (sUserLogin=="/os")
			pService=new CServiceGDK<COsRules>(this);
		if (sUserLogin=="/ams")
			pService=new CServiceGDK<CAmsRules>(this);
		if (sUserLogin=="/cks")
			pService=new CServiceGDK<CCksRules>(this);
		if (pService)
			loginToPService[sUserLogin]=pService;
	}
	return pService;
}
