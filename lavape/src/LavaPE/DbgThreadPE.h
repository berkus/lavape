#ifndef __DbgThread
#define __DbgThread


#include "LavaAppBase.h"
//#include "prelude.h"
#include <QTcpSocket>
#include <QTcpServer>
#include "ASN1File.h"
#include "CHAINANY.h"
#include "Syntax.h"
#include "LavaThread.h"
#include "LavaBaseDoc.h"

#include "qobject.h"
#include "qstring.h"

class DbgMessages 
{
public:
  DbgMessages() {lastReceived = 0; newReceived = 0;}
  DbgMessage *lastReceived;
  DbgMessage *newReceived;
};

class CLavaPEDebugThread : public CLavaThread
{
public:
  CLavaPEDebugThread() {
    dbgRequest=0;
    get_cid=0;
    put_cid=0;
    interpreterWaits = false;
    myDoc=0;
    listenSocket = 0;
    workSocket = 0;
  }
  QString remoteIPAddress;
  quint16 remotePort;
  QTcpServer *listenSocket;
  QTcpSocket *workSocket;
  ASN1InSock *get_cid;
  ASN1OutSock *put_cid;
  DbgMessage* dbgRequest;
  DbgMessages dbgReceived;
  CHAINANY brkPnts;
  bool interpreterWaits;
  bool startedFromLava;
  void clearBrkPnts();
  void adjustBrkPnts();
  void adjustBrkPnts(CHAINANY* brkPntsChain);
  void cleanBrkPoints(CLavaBaseDoc* closedDoc);
  void restoreBrkPoints(CLavaBaseDoc* openedDoc);
  virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc);
  virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc);
  ~CLavaPEDebugThread() {reset(true);}
  void reset(bool final);

protected:
  void checkBrkPnts0();
  void checkBrkPnts1();
  void checkBrkPnts2();
	void run();
}; 

#endif
