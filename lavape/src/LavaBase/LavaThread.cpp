/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "LavaBase_all.h"
#include "MACROS.h"

#include "LavaThread.h"
#include "LavaBaseDoc.h"

#pragma hdrstop


static QThreadStorage<CThreadData*> myThreadStg;

QThreadStorage<CThreadData*>* threadStg() {
  return &myThreadStg;
}


CLavaThread::CLavaThread(unsigned (*fp)(CLavaBaseDoc *), CLavaBaseDoc* docu)
{
  exec = fp;
	myDoc = docu;
  pContExecEvent = new CEventEx();

  if (!myDoc->ThreadList) {
		myDoc->ThreadList = new CThreadList;
//    myDoc->ThreadList->setAutoDelete(true);
  }
}

CLavaThread::~CLavaThread()
{
  terminate(); //(*pContExecEvent)--;
  wait();
  delete pContExecEvent;
}

CThreadData::CThreadData(CLavaThread *thr)
{
  threadPtr = thr;
}

CLavaThread *CLavaThread::currentThread() {
	return threadStg()->localData()->threadPtr; 
}

void CLavaThread::run() {
  CThreadData *td = new CThreadData(this);
	threadStg()->setLocalData(td);
  myDoc->ThreadList->append(this);
	(*exec)(myDoc);
}
