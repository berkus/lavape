
#ifdef __GNUC__
#pragma interface
#endif

#ifndef __CLavaThread
#define __CLavaThread

#include "SynIDTable.h"
#include "qsemaphore.h"
#include "qthread.h"
#include "qthreadstorage.h"


class CLavaBaseDoc;
class CSectionDesc;
class CRuntimeException;

class LAVABASE_DLL CEventEx : public QSemaphore
{
public:
  CEventEx() : QSemaphore(1) {
		lastException = 0; 
		ex = 0; 
		(*this)++;
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

	void run();
}; 

class LAVABASE_DLL CThreadData {
public:
	CThreadData(CLavaThread *thr);
	
	CLavaThread *threadPtr;
};

template class LAVABASE_DLL QThreadStorage<class CThreadData *>;

extern LAVABASE_DLL QThreadStorage<CThreadData*> threadStg;

typedef QPtrList<CLavaThread> CThreadList;


#endif
