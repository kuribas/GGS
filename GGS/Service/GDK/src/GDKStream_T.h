// $Id: GDKStream_T.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#include <cmath>

template <class TRules>
void CServiceGDK<TRules>::HandleJoin(const typename CSG<TRules>::TMsgJoin& msg) {
  BaseJoin(msg);
  MakeMoveIfNeeded(msg.idg);
}

template <class TRules>
CServiceGDK<TRules>::CServiceGDK(ggsstream* apgs) : CSG<TRules>(apgs)  {
  (*this->pgs) << "ts trust +\n"
	       << "tell " << this->sLogin << " open 1\n";
  this->pgs->flush();
}

template <class TRules>
void CServiceGDK<TRules>::HandleUnknown(const typename CSG<TRules>::TMsgUnknown& msg) {
  cout << "Unknown " << this->sLogin << " message: ";
  Handle(msg);
}

template <class TRules>
void CServiceGDK<TRules>::HandleUpdate(const typename CSG<TRules>::TMsgUpdate& msg) {
	BaseUpdate(msg);
	MakeMoveIfNeeded(msg.idg);
}

// helper function for join and update messages
template <class TRules>
void CServiceGDK<TRules>::MakeMoveIfNeeded(const string& idg) {
  typename CSG<TRules>::TGame* pgame=dynamic_cast<typename CSG<TRules>::TGame*>(CSG<TRules>::PGame(idg));
  QSSERT(pgame);
  if (pgame!=NULL) {
    bool fMyMove=pgame->ToMove(this->pgs->GetLogin());
    
    if (fMyMove) {
      CSGMoveListItem mli;

      mli.dEval=0;	// user might forget to set this

			// we check both clock() time and time() time.
			//	clock() time is more accurate but the counter may roll, so
			//	we return clock() time only if the two times are close.
      clock_t clockStart=clock();
      time_t tStart=time(NULL);
      GetMove(*pgame, mli);
      double tTime=time(NULL)-tStart;
      clock_t clockEnd=clock();
      double tClock=double(clockEnd-clockStart)/CLOCKS_PER_SEC;
      if (fabs(tClock-tTime)<2)
	mli.tElapsed=tClock;
      else
	mli.tElapsed=tTime;
      CSG<TRules>::ViewerUpdate(idg, mli);
    }
  }
  else
    QSSERT(0);
}
