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
  ~CLavaThread();

	CLavaBaseDoc *myDoc;

  virtual void suspend() {
    mySemaphore.acquire();
  }

  virtual void resume() {
    mySemaphore.release();
  }

  CSemaphore mySemaphore;


//	static CLavaThread *currentThread();
  virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
  void run(){};
}; 

class LAVABASE_DLL CThreadData {
public:
	CThreadData(CLavaThread *thr);
	
	CLavaThread *threadPtr;
};

//template class LAVABASE_DLL QThreadStorage<class CThreadData *>;

//extern LAVABASE_DLL QThreadStorage<CThreadData*>* threadStg();

typedef QList<CLavaThread*> CThreadList;


#endif
