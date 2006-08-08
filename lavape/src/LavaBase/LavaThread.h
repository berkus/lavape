#ifndef __CLavaThread
#define __CLavaThread

#include "SynIDTable.h"
#include "qthread.h"
#include "qthreadstorage.h"
//Added by qt3to4:
#include <QList>
#include <QSemaphore>
#include <QMutex>


class CLavaBaseDoc;
class CSectionDesc;
class CRuntimeException;

class LAVABASE_DLL CSemaphore : public QSemaphore
{
public:
  CSemaphore() : QSemaphore(1) {
		lastException = 0; 
		ex = 0; 
	}
  CSectionDesc **lastException;
  CRuntimeException* ex;
};

class LAVABASE_DLL CLavaThread : public QThread
{
public:
  CLavaThread() {
    mySemaphore.acquire();
  }
  CLavaThread(CLavaBaseDoc *d);

	CLavaBaseDoc *myDoc;

  virtual void suspend() {
    mySemaphore.acquire();
  }

  virtual void resume() {
    mySemaphore.release();
  }

  CSemaphore mySemaphore;


  //virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  //virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
  void run(){};
}; 

class LAVABASE_DLL CLavaDbgBase : public QObject {
public:
  CLavaDbgBase(){ isRunning = false; };
  CLavaDbgBase(CLavaBaseDoc *d);

	CLavaBaseDoc *myDoc;
  bool isRunning;

  virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
  Q_OBJECT
}; 

typedef QList<CLavaThread*> CThreadList;


#endif
