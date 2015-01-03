// $Id: GDKStream.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved

#ifndef GDK_GDKSTREAM_H
#define GDK_GDKSTREAM_H

#include "ggsstream.h"
#include "SG.h"
#include "SGMessages.h"

class CGDKStream: public ggsstream {
public:
  virtual void Handle	    (const CMsg& msg);
  virtual void HandleLogin  ();
  virtual void HandleTell   (const CMsgGGSTell& msg);
  virtual void HandleUnknown(const CMsgGGSUnknown& msg);

  virtual CSGBase* CreateService(const string& sUserLogin);

};

template <class TRules>
class CServiceGDK : public CSG<TRules> {
public:
  CServiceGDK(ggsstream* apgs);

  virtual void HandleJoin   (const typename CSG<TRules>::TMsgJoin& msg);
  virtual void HandleUnknown(const typename CSG<TRules>::TMsgUnknown& msg);
  virtual void HandleUpdate (const typename CSG<TRules>::TMsgUpdate& msg);
  
  virtual void MakeMoveIfNeeded(const string& idg);
};

#include "GDKStream_T.h"

#endif // GDK_GDKSTREAM_H
