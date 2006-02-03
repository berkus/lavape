#ifndef __DBTHREAD
#define __DBTHREAD

#include "Constructs.h"
#include "LavaAppBase.h"       // Hauptsymbole
#include "LavaBaseDoc.h"       // Hauptsymbole
#include "Syntax.h"       // Hauptsymbole
#include "docview.h"
#include "qstring.h"
#include "q3process.h"
#include <QTcpSocket>
#include <QTcpServer>

class LocalDebugVar : public VarAction {
public:
  LocalDebugVar(DbgStopData* data, CLavaBaseDoc* d)
  {dbgStopData = data; doc = d; }
  void run();
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
  DbgStopData* dbgStopData;
  CHEStackData* cheAct;
};

class LAVAEXECS_DLL CLavaDebugThread : public CLavaThread
{
public:
  CLavaDebugThread();
  ~CLavaDebugThread();

  void initData(CLavaBaseDoc* d);
  QTcpServer *listenSocket;
  QTcpSocket *workSocket;
  QString remoteIPAddress;
  quint16 remotePort;

  DbgStopData* dbgStopData;
  LocalDebugVar *varAction;
  bool debugOn;
  bool debugOff;
  CEventEx *pContDebugEvent;
  DbgContType actContType;
  CHAINANY /*ProgPoint*/ brkPnts;
  DbgMessage mSend, mReceive;

protected:
	void run();
  void setBrkPnts();
  void addCalleeParams();
}; 

#endif
