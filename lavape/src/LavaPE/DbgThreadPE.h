// LavaPE.h : main header file for the LavaPE application
//

#ifndef __DbgThread
#define __DbgThread


#include "LavaAppBase.h"
#include "prelude.h"
#include "sflsock.h"
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
  CLavaPEDebugThread() 
    {dbgRequest=0; get_cid=0; put_cid=0; interpreterWaits = false; myDoc=0;}
  QString remoteIPAddress, remotePort;
  sock_t listenSocket, workSocket;
  ASN1InSock *get_cid;
  ASN1OutSock *put_cid;
  DbgMessage* dbgRequest;
  DbgMessages dbgReceived;
  CHAINANY brkPnts;
  bool interpreterWaits;
  bool startedFromLava;
  void clearBrkPnts();
  void adjustBrkPnts();
  void removeBrkPoints(CLavaBaseDoc* closedDoc);
  void restoreBrkPoints(CLavaBaseDoc* openedDoc);
  ~CLavaPEDebugThread() {reset(true);}

protected:
  void checkBrkPnts1();
  void checkBrkPnts2();
  void reset(bool final);
	void run();
}; 

#endif
