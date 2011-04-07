#ifndef __CLavaThread
#define __CLavaThread

#include "SynIDTable.h"
#include "Syntax.h"
#include "qthread.h"
#include "qthreadstorage.h"
//Added by qt3to4:
#include <QList>
#include <QSemaphore>
#include <QMutex>


class CLavaBaseDoc;
class CSectionDesc;
class CRuntimeException;

class LAVABASE_DLL CExecAbort
{
};

class LAVABASE_DLL CSemaphore : public QSemaphore
{
public:
  CSemaphore() : QSemaphore(1) {
		lastException = 0; 
		ex = 0;
    synError = 0;
	}
  CSectionDesc **lastException;
  CRuntimeException* ex;
  SynErr* synError;
};

class LAVABASE_DLL CLavaThread : public QThread {
public:
  CLavaThread() {
    abort = false;
    waitingForUI = false;
    handler_Call = false;
    ldocEnd = false;
    mySemaphore.acquire();
  }
  CLavaThread(CLavaBaseDoc *d);

  CLavaBaseDoc *myDoc;
  bool abort, ldocEnd, waitingForUI, handler_Call;

  virtual void suspend() {
    mySemaphore.acquire();
    if (abort) {
      abort = false;
      throw CExecAbort();
    }
  }

  virtual void resume() {
    if ((!waitingForUI || abort) && !mySemaphore.available())
      mySemaphore.release();
  }

  CSemaphore mySemaphore;


  //virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  //virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
  void run(){};
}; 

enum DbgExitReason {
  normalEnd,
  disconnected,
  otherError
};

class LAVABASE_DLL CLavaDbgBase : public QObject {
public:
  CLavaDbgBase(){ isConnected = false; sendPending = true; myDoc = 0; synErrReported = false;};
  CLavaDbgBase(CLavaBaseDoc *d);

	CLavaBaseDoc *myDoc;
  bool isConnected, sendPending, synErrReported;

  virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
  Q_OBJECT
}; 

typedef QList<CLavaThread*> CThreadList;


#endif
