// DbThreads.h : 
//

#ifndef __DBTHREAD
#define __DBTHREAD

#include "Constructs.h"
#include "LavaAppBase.h"       // Hauptsymbole
#include "LavaBaseDoc.h"       // Hauptsymbole
#include "Syntax.h"       // Hauptsymbole
#include "docview.h"
#include "qstring.h"
#include "qprocess.h"
#include "prelude.h"
#include "sflsock.h"


class LocalDebugVar : public VarAction {
public:
  LocalDebugVar(DebugStopData* data, CLavaBaseDoc* d)
  {dbgStopData = data; doc = d; }
  void run();
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
  DebugStopData* dbgStopData;
  CHEStackData* cheAct;
};

class LAVAEXECS_DLL CLavaDebugThread : public CLavaThread
{
public:
  CLavaDebugThread();

  void initData(CLavaBaseDoc* d);
  sock_t listenSocket, workSocket;
  QString remoteIPAddress, remotePort;

  DebugStopData* dbgStopData;
  LocalDebugVar *varAction;
  bool debugOn;
  CEventEx *pContThreadEvent;
  DbgContType actContType;

  ~CLavaDebugThread();
protected:
	void run();
}; 

#endif
