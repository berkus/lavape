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
#include "DbgThread.h"
#include "ASN1File.h"
#include "CDPpp.h"
#include "Syntax.h"
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

#define CHECKIMPL(FUNCDECL) \
  classDecl = FUNCDECL->ParentDECL; \
  if (classDecl->DeclType == Impl) \
    classDecl = doc->IDTable.GetDECL(((CHETID*)classDecl->Supports.first)->data, classDecl->inINCL); \
  if (classDecl->DeclType == Interface) \
    doc->CheckImpl(ckd, classDecl); 




bool debugStep=false,
  debugStepFunc=false,
  debugStepInto=false,
  debugStepOut=false;

LavaVariablePtr stepOutStack, currentStack;

CLavaDebugThread::CLavaDebugThread() {
  dbgStopData=0; 
  varAction=0; 
  listenSocket = 0;
  debugOn = false;
  pContDebugEvent = new CEventEx();
  if (pContDebugEvent->available())
    (*pContDebugEvent)++;
  if (pContExecEvent->available())
    (*pContExecEvent)++;
}


void CLavaDebugThread::initData(CLavaBaseDoc* d) {
  if (!dbgStopData) {
    doc = d;
    dbgStopData = new DbgStopData;
    varAction = new LocalDebugVar(dbgStopData, doc);
  }
}


CLavaDebugThread::~CLavaDebugThread() 
{
  if (dbgStopData) delete dbgStopData;
  dbgStopData = 0;
  if (varAction) delete varAction;
  varAction = 0;
  (*pContDebugEvent)--; 
  wait();
  delete pContDebugEvent;
}


void CLavaDebugThread::run() {

  QProcess lavape;
  fd_set read_fds;
  int nReady;
  ASN1InSock *get_cid;
  ASN1OutSock *put_cid;
	QString lavapePath, buf;
  sockaddr_in sa;
  socklen_t sz_sa = sizeof(sockaddr_in); 
  DDItemData * oid;
  bool fin = false;


  CThreadData *td = new CThreadData(this);
	threadStg.setLocalData(td);
  
  if (debugOn) 
    workSocket = connect_TCP(remoteIPAddress,remotePort);
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

  put_cid = new ASN1OutSock (workSocket);
  if (!put_cid->Done) {
    delete put_cid;
    qApp->exit(1);
  }
//  put_cid = out_cid;

  get_cid = new ASN1InSock (workSocket);
  if (!get_cid->Done) {
    delete get_cid;
    qApp->exit(1);
  }
//  get_cid = in_cid;
  if (debugOn) 
    (*pContDebugEvent)++;  //DebugThread wait until ExecuteLava has finished initialisation
  else {
    varAction->run();
    mSend.SetSendData(Dbg_StopData, dbgStopData);
    CDPDbgMessage(PUT, put_cid, (address)&mSend);
    put_cid->flush();
    doc->debugOn = true;
  }
  while (true) {
   	CDPDbgMessage(GET,get_cid,(address)&mReceive);
    if (get_cid->Done) {
      
      switch (mReceive.Command) {
      case Dbg_Exit: 
        fin = true;
        break;
      case Dbg_StackRq:
        dbgStopData->ObjectChain.Destroy();
        dbgStopData->ActStackLevel = mReceive.CallStackLevel;
        varAction->run();
        mSend.SetSendData(Dbg_Stack, dbgStopData);
        break;
      case Dbg_MemberDataRq:
        oid = DebugItem::openObj((CHE*)dbgStopData->ObjectChain.first, (CHEint*)mReceive.ObjNr.ptr->first);
        mSend.SetSendData(oid);
        mReceive.ObjNr.Destroy();
        break;
      case Dbg_Continue:
        setBrkPnts();

        dbgStopData->ActStackLevel = 0;  //reset dbgStopData
        dbgStopData->StackChain.Destroy();
        dbgStopData->ObjectChain.Destroy();
//        qApp->mainWidget()->setActiveWindow();
//        qApp->mainWidget()->raise();
//         qDebug("Dbg_Continue, available=%d",pContExecEvent->available());
        (*pContExecEvent)--;    //continue ExecuteLava
        (*pContDebugEvent)++;  //DebugThread wait for next stop with new dbgStopData
        if (!varAction) {
          fin = true;
          break;
        }
        if (dbgStopData->StackChain.first) {      
          varAction->run();
          mSend.SetSendData(Dbg_StopData, dbgStopData);
        }
        else 
          fin = true;
        break;
      default:;
      }
      if (fin)
        break;
      CDPDbgMessage(PUT, put_cid, (address)&mSend);
      if (!put_cid->Done) 
        break;
      put_cid->flush();
    }
    else
      break;
  }
  delete dbgStopData;
  delete varAction;
  mSend.Destroy();
  mReceive.Destroy();
  brkPnts.Destroy();
  dbgStopData=0;
  varAction=0;
  delete put_cid;
  delete get_cid;
  debugOn = false;
  if (doc)
    if (doc->startedFromLavaPE) 
      qApp->quit();
    else 
      doc->Close();
}

void CLavaDebugThread::setBrkPnts()
{
  CHEProgPoint *chePP, *chePPnew;
  LavaDECL *funcDecl, *execDecl, *classDecl;
  CSearchData sData;
  CheckData ckd;

  ckd.document = doc;

  if (mReceive.ContData.ptr) {
    actContType = ((DbgContData*)mReceive.ContData.ptr)->ContType;
    if (((DbgContData*)mReceive.ContData.ptr)->ClearBrkPnts) {
      for (chePP = (CHEProgPoint*)brkPnts.first;
                 chePP; chePP = (CHEProgPoint*)chePP->successor) 
        ((SynObject*)chePP->data.SynObj)->workFlags.EXCL(isBrkPnt);
      brkPnts.Destroy();
    }
    for (chePPnew = (CHEProgPoint*)((DbgContData*)mReceive.ContData.ptr)->BrkPnts.first;
         chePPnew; chePPnew = (CHEProgPoint*)chePPnew->successor) {
      if (chePPnew->data.Activate) {
        funcDecl = doc->IDTable.GetDECL(chePPnew->data.FuncID);
        execDecl = doc->GetExecDECL(funcDecl, chePPnew->data.ExecType, false,false);
        if (execDecl) {
          CHECKIMPL(funcDecl)
          sData.synObjectID = chePPnew->data.SynObjID;
          sData.doc = doc;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&doc->IDTable, 0, (SynObjectBase*)execDecl, onSetBrkPnt, 0,0, (address)&sData);
          chePPnew->data.SynObj = sData.synObj;
        }
      }
      else {
        chePP = (CHEProgPoint*)brkPnts.first;
        while (chePP && (chePP->data.SynObjID != chePPnew->data.SynObjID))
          chePP = (CHEProgPoint*)chePP->successor;
        if (chePP) {
          ((SynObject*)chePP->data.SynObj)->workFlags.EXCL(isBrkPnt);
          brkPnts.Remove(chePP->predecessor);
        }
      }
    }
    if (brkPnts.last)
      brkPnts.last->successor = ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.first;
    else   
      brkPnts.first = ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.first;
    brkPnts.last =  ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.last;
    ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.first = 0;
    ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.last = 0;
    if (actContType == dbg_RunTo) {
      funcDecl = doc->IDTable.GetDECL(((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->FuncID);
      execDecl = doc->GetExecDECL(funcDecl, ((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->ExecType, false,false);
      if (execDecl) {
        CHECKIMPL(funcDecl)
        sData.synObjectID = ((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->SynObjID;
        sData.doc = doc;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&doc->IDTable, 0, (SynObjectBase*)execDecl, onSetRunToPnt, 0,0, (address)&sData);
      }
    }
    else if (actContType == dbg_Step)
      debugStep = true;
    else if (actContType == dbg_StepFunc)
      debugStepFunc = true;
    else if (actContType == dbg_StepInto)
      debugStepInto = true;
    else if (actContType == dbg_StepOut) {
      debugStepOut = true;
      stepOutStack = currentStack;
    }
  }
  else
    actContType = dbg_Cont;

}

bool LocalDebugVar::Action(CheckData &ckd, VarName *vn, TID&)
{
  QString label = QString(vn->varName.c);
  if (cheAct->data.Stack[vn->stackPos])
    label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[vn->stackPos])[0]->classDECL->FullName.c);
  DebugItem* item = new DebugItem(new DDMakeClass, 0, doc, (LavaObjectPtr)cheAct->data.Stack[vn->stackPos], label, false, false, false);
  CHE* che = new CHE(item);
  dbgStopData->ObjectChain.Prepend(che);
  return false;
}

void LocalDebugVar::run()
{
  CheckData ckd;
  LavaDECL *func;
  CHE *cheIO, *cheItem, *afterElem = (CHE*)dbgStopData->ObjectChain.first;
  DebugItem *item;
  int ii, stpos = SFH+1;
  QString label;

  ckd.document = doc;
  cheAct = (CHEStackData*) dbgStopData->StackChain.GetNth(dbgStopData->ActStackLevel+1);
  CheckLocalScope (ckd, (SynObject*)cheAct->data.SynObj);
  if (cheAct->data.ExecType == ExecDef) {
    func = doc->IDTable.GetDECL(cheAct->data.FuncID);
    if (func->DeclType == Function) {
      cheIO = (CHE*)func->NestedDecls.first;
      for (ii = func->nInput + func->nOutput; ii; ii--) {
        label = ((LavaDECL*)cheIO->data)->FullName.c;
        if (cheAct->data.Stack[stpos])
          label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[stpos])[0]->classDECL->FullName.c);
        item = new DebugItem(new DDMakeClass, 0, doc, (LavaObjectPtr)cheAct->data.Stack[stpos], label, false, false, false);
        cheItem = new CHE(item);
        dbgStopData->ObjectChain.Insert(afterElem, cheItem);
        afterElem = cheItem;
        stpos++;
        cheIO = (CHE*)cheIO->successor;
      }
    }
  }
}

