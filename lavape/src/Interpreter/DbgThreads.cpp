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


#ifdef __GNUC__
#pragma implementation
#endif

#include "prelude.h"
#include "sflsock.h"
#include "mdiframes.h"
#include "DbgThreads.h"
#include "ASN1File.h"
#include "CDPpp.h"
#include "LavaAppBase.h"
#include "LavaThread.h"
#include "LavaBaseDoc.h"
#include "DumpView.h"
#include "LavaProgram.h"
#include "LavaBaseStringInit.h"
#include "Tokens.h"
#include "qclipboard.h"
#include "qfiledialog.h"
#include "qfontdialog.h"
#include "qmessagebox.h"
#include <signal.h>
#include "qassistantclient.h"
#include "qprocess.h"
#include <stdlib.h>

#ifndef WIN32
#include <locale.h>
#endif

CLavaDebugThread::CLavaDebugThread() {
  dbgStopData=0; 
  varAction=0; 
  listenSocket = 0;
  debugOn = false;
  pContThreadEvent = new CEventEx();
}

CLavaDebugThread::~CLavaDebugThread() 
{
  if (dbgStopData) delete dbgStopData;
  dbgStopData = 0;
  if (varAction) delete varAction;
  varAction = 0;
  //(*pContThreadEvent)--;
  delete pContThreadEvent;
}


void CLavaDebugThread::run() {
  fd_set read_fds;  //, write_fds, error_fds;
  int nReady;
  ASN1InSock *get_cid;
  ASN1OutSock *put_cid;

	QString lavapePath, buf;
  sockaddr_in sa;
  socklen_t sz_sa = sizeof(sockaddr_in); 
  QProcess lavape;
  DebugMessage mSend, mReceive;

  CThreadData *td = new CThreadData(this);
	threadStg.setLocalData(td);
  
  if (debugOn) {
    workSocket = connect_TCP(remoteIPAddress,remotePort);
    (*pContThreadEvent)++;
  }
  else { //PMDump

#ifdef WIN32
    lavapePath = ExeDir + "/LavaPE.exe";
#else
    lavapePath = ExeDir + "/LavaPE";
#endif
	
    sock_init();
    if (!listenSocket)
      listenSocket = passive_TCP("0",20);

    memset(&sa,0,sizeof(sockaddr_in));
    int rc = getsockname((int)listenSocket,(struct sockaddr*)&sa,&sz_sa);
    QString host_addr = "127.0.0.1";
	  QStringList args;
	  args << lavapePath << doc->GetFilename() << host_addr << QString("%1").arg(ntohs(sa.sin_port));
	  lavape.setArguments(args);
	  if (!lavape.launch(buf)) {
      QMessageBox::critical(qApp->mainWidget(),qApp->name(),ERR_LavaPEStartFailed.arg(errno),QMessageBox::Ok,0,0);
		  return;
	  }

    FD_ZERO(&read_fds);
    FD_SET(listenSocket,&read_fds);

    while (true) {
      nReady = sock_select(FD_SETSIZE,&read_fds,NULL,NULL,NULL);
      if (nReady == -1 && errno == EINTR) continue;
      if (FD_ISSET(listenSocket,&read_fds)) {
        workSocket = accept_socket(listenSocket);
        break;
      }
    }
  }

  ASN1OutSock* out_cid = new ASN1OutSock (workSocket);
  if (!out_cid->Done) {
    delete out_cid;
    qApp->exit(1);
  }
  put_cid = out_cid;

  ASN1InSock* in_cid = new ASN1InSock (workSocket);
  if (!in_cid->Done) {
    delete in_cid;
    qApp->exit(1);
  }
  get_cid = in_cid;
  if (!debugOn) {
    varAction->run();
    mSend.SetData(Dbg_StopData, dbgStopData);
    CDPDebugMessage(PUT, put_cid, (address)&mSend);
    put_cid->flush();
    doc->debugOn = true;
  }
  while (true) {
   	CDPDebugMessage(GET,get_cid,(address)&mReceive);
    if (get_cid->Done) {
      
      switch (mReceive.Command) {
      case Dbg_StackRq:
        dbgStopData->ObjectChain.Destroy();
        dbgStopData->ActStackLevel = mReceive.CallStackLevel;
        varAction->run();
        mSend.SetData(Dbg_Stack, dbgStopData);
        break;
      case Dbg_MemberDataRq:
        mSend.SetData(ObjDebugItem::openObj((CHE*)dbgStopData->ObjectChain.first, (CHEint*)mReceive.ObjNr.ptr->first));
        mReceive.ObjNr.Destroy();
        break;
      case Dbg_Continue:
        //set breakpoints
        actContType = ((DebugContData*)mReceive.ContinueData.ptr)->ContType;
        if (actContType == dbg_RunTo) {
          //use ((DebugContData*)mReceive.ContinueData.ptr)->RunToPoint.ptr
        }

        dbgStopData->ActStackLevel = 0;  //reset dbgStopData
        dbgStopData->StackChain.Destroy();
        dbgStopData->ObjectChain.Destroy();
        //wxTheApp->m_appWindow->setActiveWindow();
        //wxTheApp->m_appWindow->raise();
        (*pContExecEvent)--;    //continue ExecuteLava
        if (pContThreadEvent->available())
          (*pContThreadEvent)++;
        (*pContThreadEvent)++;  //DebugThread wait for next stop with new dbgStopData
        varAction->run();
        mSend.SetData(Dbg_StopData, dbgStopData);
        break;
      default:;
      }
        
      CDPDebugMessage(PUT, put_cid, (address)&mSend);
      put_cid->flush();
    }
    else {
      delete dbgStopData;
      delete varAction;
      dbgStopData=0;
      varAction=0;
      debugOn = false;
      doc->debugOn = false;
      (*pContExecEvent)--;
      break;
    }
  }
}


void CLavaDebugThread::initData(CLavaBaseDoc* d) {
  if (!dbgStopData) {
    doc = d;
    dbgStopData = new DebugStopData;
    varAction = new LocalDebugVar(dbgStopData, doc);
  }
}


bool LocalDebugVar::Action(CheckData &ckd, VarName *vn, TID&)
{
  QString label = QString(vn->varName.c);
  if (cheAct->data.Stack[vn->stackPos])
    label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[vn->stackPos])[0]->classDECL->FullName.c);
  ObjDebugItem* item = new ObjDebugItem(0, doc, (LavaObjectPtr)cheAct->data.Stack[vn->stackPos], label, false, false, false);
  CHE* che = new CHE(item);
  dbgStopData->ObjectChain.Append(che);
  return false;
}

void LocalDebugVar::run()
{
  CheckData ckd;
  ckd.document = doc;
  cheAct = (CHEStackData*) dbgStopData->StackChain.GetNth(dbgStopData->ActStackLevel+1);
  CheckLocalScope (ckd, (SynObject*)cheAct->data.SynObj);
}

