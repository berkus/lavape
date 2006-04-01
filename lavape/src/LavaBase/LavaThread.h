#ifndef __CLavaThread
#define __CLavaThread

#include "SynIDTable.h"
#include "qthread.h"
#include "qthreadstorage.h"
//Added by qt3to4:
#include <QList>
#include <QSemaphore>


class CLavaBaseDoc;
class CSectionDesc;
class CRuntimeException;

class LAVABASE_DLL CEventEx : public QSemaphore
{
public:
  CEventEx() : QSemaphore(1) {
		lastException = 0; 
		ex = 0; 
		acquire();
	}
  CSectionDesc **lastException;
  CRuntimeException* ex;
};

class CThreadData;

class LAVABASE_DLL CLavaThread : public QThread
{
public:
  CLavaThread() {   pContExecEvent = new CEventEx(); }
  CLavaThread(unsigned (*fp)(CLavaBaseDoc *d),CLavaBaseDoc *d);
  ~CLavaThread();

	unsigned (*exec)(CLavaBaseDoc *d);
	CLavaBaseDoc *myDoc;

  CEventEx *pContExecEvent;

	static CLavaThread *currentThread();
  virtual bool checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc) {return false;}
  virtual void checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc) {}
	void run();
}; 

class LAVABASE_DLL CThreadData {
public:
	CThreadData(CLavaThread *thr);
	
	CLavaThread *threadPtr;
};

//template class LAVABASE_DLL QThreadStorage<class CThreadData *>;

extern LAVABASE_DLL QThreadStorage<CThreadData*>* threadStg();

typedef QList<CLavaThread*> CThreadList;


#endif
