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


#include "mdiframes.h"
#include "Debugger.h"
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
//#include "qfiledialog.h"
#include "qfontdialog.h"
#include "qmessagebox.h"
//Added by qt3to4:
#include <signal.h>
#include "QtAssistant/qassistantclient.h"
#include <stdlib.h>

#ifndef WIN32
#include <locale.h>
#endif

#pragma hdrstop


#define CHECKIMPL(FUNCDECL) \
  if (FUNCDECL->DeclType == Function) \
    classDECL = FUNCDECL->ParentDECL; \
  else \
    classDECL = FUNCDECL; \
  if (classDECL->DeclType == Impl) \
    classDECL = myDoc->IDTable.GetDECL(((CHETID*)classDECL->Supports.first)->data, classDECL->inINCL); \
  if (classDECL->DeclType == Interface) \
    myDoc->CheckImpl(ckd, classDECL);



CLavaDebugger::CLavaDebugger() {
  dbgStopData=0;
  varAction=0;
  listenSocket = 0;
  workSocket = 0;
  m_execThread = 0;
  startedFromLavaPE = false;
}


void CLavaDebugger::initData(CLavaBaseDoc* d, CLavaThread *execThr) {
  if (!dbgStopData) {
    myDoc = d;
    m_execThread = execThr;
    dbgStopData = new DbgStopData;
    varAction = new LocalDebugVar(dbgStopData, myDoc);
  }
}

void CLavaDebugger::resetData()
{
  myDoc = 0;
  m_execThread = 0;
  if (dbgStopData) {
    delete dbgStopData;
    dbgStopData = 0;
  }
  if (varAction) {
    delete varAction;
    varAction = 0;
  }
  mSend.Destroy();
  mReceive.Destroy();
}


CLavaDebugger::~CLavaDebugger()
{
   resetData();
  /*if (dbgStopData) {
    delete dbgStopData;
    dbgStopData = 0;
  }
  if (varAction) {
    delete varAction;
    varAction = 0;
  }
  mSend.Destroy();
  mReceive.Destroy();*/
}

void CLavaDebugger::customEvent(QEvent *ev) {
  switch (ev->type()) {
  case UEV_Start:
     start();
     break;
  case UEV_Send:
     send();
     break;
  case UEV_Stop:
     stop(normalEnd);
     break;
  default: ;
  }
}

void CLavaDebugger::start() {
	QString lavapePath, buf;
  quint16 locPort;

  if (startedFromLavaPE) {
    workSocket = new QTcpSocket(this);
    connect(workSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(error(QAbstractSocket::SocketError)));
    connect(workSocket,SIGNAL(readyRead()),SLOT(receive()));
    workSocket->connectToHost(remoteIPAddress,remotePort);
    connected();
  }

  else { //PMDump
#ifdef WIN32
    lavapePath = ExeDir + "/LavaPE.exe";
#else
    lavapePath = ExeDir + "/LavaPE";
#endif

    if (!listenSocket) {
      listenSocket = new QTcpServer(this);
      connect(listenSocket,SIGNAL(newConnection()),SLOT(connectToClient()));
      listenSocket->listen();
    }

    locPort = listenSocket->serverPort();
    QString host_addr = "127.0.0.1";
	  QStringList args;
    if (((CLavaProgram*)myDoc)->LcomFileName.isEmpty())
	    args << myDoc->GetFilename() << host_addr << QString("%1").arg(locPort);
    else
	    args << ((CLavaProgram*)myDoc)->LcomFileName << host_addr << QString("%1").arg(locPort);
    if (!QProcess::startDetached(lavapePath,args)) {
      QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),ERR_LavaPEStartFailed,QMessageBox::Ok,0,0);
		  return;
	  }
    //if (!((CLavaProgram*)myDoc)->corruptSyntax) 
      myDoc->openForDebugging = true;
  }
  //if (!((CLavaProgram*)myDoc)->corruptSyntax) 
    myDoc->debugOn = true;
}

void CLavaDebugger::connectToClient() {
  workSocket = listenSocket->nextPendingConnection();
  connect(workSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(error(QAbstractSocket::SocketError)));
  connect(workSocket,SIGNAL(readyRead()),SLOT(receive()));
  connected();
}

void CLavaDebugger::connected() {
  isConnected = true;

  put_cid = new ASN1OutSock (workSocket);
  if (!put_cid->Done) {
    stop(otherError);
    return;
  }

  get_cid = new ASN1InSock (workSocket);
  if (!get_cid->Done) {
    stop(otherError);
    return;
  }

  if (!startedFromLavaPE) {
    if ((dbgStopData->stopReason != Stop_Start) && dbgStopData->StackChain.first) {
      varAction->run();
      addCalleeParams();
    }
    mSend.SetSendData(Dbg_StopData, dbgStopData);
    CDPDbgMessage0(PUT, put_cid, &mSend,false);
    put_cid->flush();
    if (!put_cid->Done) {
      stop(otherError);
      return;
    }
  }
}

void CLavaDebugger::receive() {
  DDItemData *oid;

  if (!get_cid->bytesAvailable())
    return;

 	CDPDbgMessage0(GET,get_cid,&mReceive,false);
  if (get_cid->Done) {
    switch (mReceive.Command) {
    case Dbg_Exit:
      stop(otherError); // is treated like an error
      break;
    case Dbg_StackRq:
      dbgStopData->ObjectChain.Destroy();
      dbgStopData->ParamChain.Destroy();
      dbgStopData->ActStackLevel = mReceive.CallStackLevel;
      if (dbgStopData->StackChain.first) {
        varAction->run();
        if (!dbgStopData->ActStackLevel)
          addCalleeParams();
      }
      mSend.SetSendData(Dbg_Stack, dbgStopData);
      send();
      break;
    case Dbg_MemberDataRq:
      if (mReceive.fromParams)
        oid = DebugItem::openObj((CHE*)dbgStopData->ParamChain.first, (CHEint*)mReceive.ObjNr.ptr->first);
      else
        oid = DebugItem::openObj((CHE*)dbgStopData->ObjectChain.first, (CHEint*)mReceive.ObjNr.ptr->first);
      mSend.SetSendData(oid);
      mReceive.ObjNr.Destroy();
      send();
      break;
    case Dbg_Continue:
      if (m_execThread) {
        if (varAction) {
          setBrkPnts();
          if (dbgStopData) {
            dbgStopData->ActStackLevel = 0;  //reset dbgStopData
            dbgStopData->CalleeStack = 0;
            dbgStopData->StackChain.Destroy();
            dbgStopData->ObjectChain.Destroy();
            dbgStopData->ParamChain.Destroy();
          }
        }
        m_execThread->resume();    //continue ExecuteLava
      }
      break;
    default:;
    }
  }
  else
    stop(otherError);
}

void CLavaDebugger::send() {
  if (mReceive.Command == Dbg_Continue) {
    if (!varAction) {
      stop(otherError);
      return;
    }
    if (dbgStopData->StackChain.first) {
      varAction->run();
      addCalleeParams();
      mSend.SetSendData(Dbg_StopData, dbgStopData);
    }
    else  {
      stop(otherError);
      return;
    }
  }

  CDPDbgMessage0(PUT, put_cid, &mSend,false);
  put_cid->flush();
  if (!put_cid->Done )//|| ((CLavaProgram*)myDoc)->corruptSyntax)
    stop(otherError);
}

void CLavaDebugger::error(QAbstractSocket::SocketError socketError) {
  if (socketError == QAbstractSocket::RemoteHostClosedError)
    stop(disconnected);
  else {
    qDebug() << "+++ socket error: " << workSocket->errorString();
    stop(otherError); // other error
  }
}

void CLavaDebugger::stop(DbgExitReason reason) {
  if (!isConnected)
    return;
  isConnected = false;

  if (dbgStopData) {
    delete dbgStopData;
    dbgStopData = 0;
  }
  if (varAction) {
    delete varAction;
    varAction=0;
  }

  mSend.Destroy();
  mReceive.Destroy();
  brkPnts.Destroy();

  if (get_cid)
    delete get_cid;
  get_cid= 0;
  if (put_cid)
    delete put_cid;
  put_cid= 0;

  if (reason != disconnected)
    workSocket->disconnectFromHost();
  workSocket = 0;
  if (myDoc && (reason == disconnected) && myDoc->openForDebugging && !((CLavaProgram*)myDoc)->corruptSyntax) {
    myDoc->debugOn = false;
    myDoc->openForDebugging = false;
    myDoc = 0;
    if (m_execThread)
      m_execThread->resume();    //continue ExecuteLava
    m_execThread = 0;
    return;
  }
  else
    if (((CLavaProgram*)myDoc)->corruptSyntax) {
      ((CLavaProgram*)myDoc)->corruptSyntax = false;
      delete myDoc;
      //((CLavaProgram*)myDoc)->CreateFailed();
      myDoc = 0;
    }
  if ((reason != normalEnd) && m_execThread) {
    m_execThread->abort = true;
    m_execThread->resume();
  }
  else if (startedFromLavaPE) {
    if (reason == normalEnd)
      qApp->exit(0);
    else
      qApp->exit(1);
  }
}

void CLavaDebugger::setBrkPnts()
{
  CHEProgPoint *chePP, *chePPnew, *rmPP;
  LavaDECL *funcDecl, *execDecl, *classDECL;
  CSearchData sData;
  CheckData ckd;

  ckd.document = myDoc;

  if (mReceive.ContData.ptr) {
    actContType = ((DbgContData*)mReceive.ContData.ptr)->ContType;
    if (((DbgContData*)mReceive.ContData.ptr)->ClearBrkPnts) {
      for (chePP = (CHEProgPoint*)brkPnts.first;
                 chePP; chePP = (CHEProgPoint*)chePP->successor)
        ((SynObject*)chePP->data.SynObj)->workFlags.EXCL(isBrkPnt);
      brkPnts.Destroy();
    }
    chePPnew = (CHEProgPoint*)((DbgContData*)mReceive.ContData.ptr)->BrkPnts.first;
    while(chePPnew) {
      if (chePPnew->data.Activate && !chePPnew->data.Skipped) {
        funcDecl = myDoc->IDTable.GetDECL(chePPnew->data.FuncID);
        execDecl = myDoc->GetExecDECL(funcDecl, chePPnew->data.ExecType, false,false);
        if (execDecl) {
          CHECKIMPL(funcDecl)
          sData.synObjectID = chePPnew->data.SynObjID;
          sData.doc = myDoc;
          sData.nextFreeID = 0;
          // sData.finished = false;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
          ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
          chePPnew->data.SynObj = sData.synObj;
          chePPnew = (CHEProgPoint*)chePPnew->successor;
        }
        else {
          rmPP = chePPnew;
          chePPnew = (CHEProgPoint*)rmPP->successor;
          ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.Remove(rmPP->predecessor);
        }
      }
      else {
        if (chePPnew->data.Skipped) {
          rmPP = chePPnew;
          chePPnew = (CHEProgPoint*)rmPP->successor;
          ((DbgContData*)mReceive.ContData.ptr)->BrkPnts.Remove(rmPP->predecessor);
        }
        else {
          chePP = (CHEProgPoint*)brkPnts.first;
          while (chePP && ((chePP->data.SynObjID != chePPnew->data.SynObjID)
            || (chePP->data.FuncID != chePPnew->data.FuncID)
            || (chePP->data.ExecType != chePPnew->data.ExecType)))
            chePP = (CHEProgPoint*)chePP->successor;
          if (chePP) {
            ((SynObject*)chePP->data.SynObj)->workFlags.EXCL(isBrkPnt);
            brkPnts.Remove(chePP->predecessor);
          }
          chePPnew = (CHEProgPoint*)chePPnew->successor;
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
      funcDecl = myDoc->IDTable.GetDECL(((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->FuncID);
      execDecl = myDoc->GetExecDECL(funcDecl, ((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->ExecType, false,false);
      if (execDecl) {
        CHECKIMPL(funcDecl)
        sData.synObjectID = ((DbgContData*)mReceive.ContData.ptr)->RunToPnt.ptr->SynObjID;
        sData.doc = myDoc;
        sData.nextFreeID = 0;
        // sData.finished = false;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
        LBaseData->tempBrkPoint = sData.synObj;
        ((SynObject*)sData.synObj)->workFlags.INCL(isTempPoint);
        nextDebugStep = noStep;
      }
    }
    else if (actContType == dbg_Step)
      nextDebugStep = nextStm;
    else if (actContType == dbg_StepFunc)
      nextDebugStep = nextFunc;
    else if (actContType == dbg_StepInto)
      nextDebugStep = stepInto;
    else if (actContType == dbg_StepOut) {
      nextDebugStep = stepOut;
      stepOutStackDepth = currentStackDepth;
    }
  }
  else {
    actContType = dbg_Cont;
    nextDebugStep = noStep;
  }
}

void CLavaDebugger::addCalleeParams()
{
  if (!dbgStopData->CalleeStack || !dbgStopData->CalleeFunc)
    return;

  CHE *cheItem, *cheIO;
  DebugItem *item;
  QString label;
  int highPos, ii, stpos = SFH+1;

  if (dbgStopData->stopReason == Stop_NextFunc
  || dbgStopData->stopReason == Stop_NextOp)
    highPos = dbgStopData->CalleeFunc->nInput;
  else if (dbgStopData->stopReason == Stop_StepOut
  || dbgStopData->stopReason == Stop_Exception) {
    highPos =dbgStopData->CalleeFunc->nInput + dbgStopData->CalleeFunc->nOutput;
  }
  else
    return;

  if (dbgStopData->CalleeFunc->DeclType == Function) {
    if (!dbgStopData->CalleeFunc->TypeFlags.Contains(isStatic)) {
      label = QString(dbgStopData->CalleeFunc->FullName.c);
      label += "::self";
      if (dbgStopData->CalleeStack[SFH])
        label = label + QString(" (") + QString(((LavaObjectPtr)dbgStopData->CalleeStack[SFH])[0]->classDECL->FullName.c);
      item = new DebugItem(new DDMakeClass, 0, myDoc, (LavaObjectPtr)dbgStopData->CalleeStack[SFH], label, false, false, false);
      cheItem = new CHE(item);
      dbgStopData->ParamChain.Append(cheItem);
    }
    cheIO = (CHE*)dbgStopData->CalleeFunc->NestedDecls.first;
    for (ii = highPos; ii; ii--) {
      label = ((LavaDECL*)cheIO->data)->FullName.c;
      if (dbgStopData->CalleeStack[stpos])
        label = label + QString(" (") + QString(((LavaObjectPtr)dbgStopData->CalleeStack[stpos])[0]->classDECL->FullName.c);
      item = new DebugItem(new DDMakeClass, 0, myDoc, (LavaObjectPtr)dbgStopData->CalleeStack[stpos], label, false, false, false);
      cheItem = new CHE(item);
      dbgStopData->ParamChain.Append(cheItem);
      stpos++;
      cheIO = (CHE*)cheIO->successor;
    }
  }
}

bool LocalDebugVar::Action(CheckData &ckd, VarName *vn, TID& typeID)
{
  LavaDECL *typeDecl;
  QString label = QString(vn->varName.c);
  if (cheAct->data.Stack[vn->stackPos])
    label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[vn->stackPos])[0]->classDECL->FullName.c);
  else {
    typeDecl = doc->IDTable.GetDECL(typeID);
    label = label + QString(" (") + QString(typeDecl->FullName.c) + QString(")");
  }
  DebugItem* item = new DebugItem(new DDMakeClass, 0, doc, (LavaObjectPtr)cheAct->data.Stack[vn->stackPos], label, false, false, false);
  CHE* che = new CHE(item);
  dbgStopData->ObjectChain.Prepend(che);
  return false;
}

void LocalDebugVar::run()
{
  CheckData ckd;
  LavaDECL *func;
  CHE *cheIO, *cheItem, *afterElem;
  DebugItem *item;
  int ii, stpos = SFH+1;
  QString label;

  ckd.document = doc;
  cheAct = (CHEStackData*) dbgStopData->StackChain.GetNth(dbgStopData->ActStackLevel+1);
  ckd.inINCL = cheAct->data.FuncID.nINCL;
  CheckLocalScope (ckd, (SynObject*)cheAct->data.SynObj);
  if (cheAct->data.ExecType == ExecDef) {
    func = doc->IDTable.GetDECL(cheAct->data.FuncID);
    if (func->DeclType == Function) {
      if (func->TypeFlags.Contains(isStatic))
        afterElem = 0;
      else
        afterElem = (CHE*)dbgStopData->ObjectChain.first;
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
      if (dbgStopData->ActStackLevel > 0) {
        cheAct = (CHEStackData*)cheAct->predecessor;
        if (cheAct->data.ExecType == ExecDef) {
          func = doc->IDTable.GetDECL(cheAct->data.FuncID);
          if (func->DeclType == Function) {
            if (!func->TypeFlags.Contains(isStatic)) {
              label = QString(func->FullName.c);
              label += "::self";
              if (cheAct->data.Stack[SFH])
                label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[SFH])[0]->classDECL->FullName.c);
              item = new DebugItem(new DDMakeClass, 0, doc, (LavaObjectPtr)cheAct->data.Stack[SFH], label, false, false, false);
              cheItem = new CHE(item);
              dbgStopData->ParamChain.Append(cheItem);
            }
            stpos = SFH+1;
            cheIO = (CHE*)func->NestedDecls.first;
            for (ii = func->nInput; ii; ii--) {
              label = ((LavaDECL*)cheIO->data)->FullName.c;
              if (cheAct->data.Stack[stpos])
                label = label + QString(" (") + QString(((LavaObjectPtr)cheAct->data.Stack[stpos])[0]->classDECL->FullName.c);
              item = new DebugItem(new DDMakeClass, 0, doc, (LavaObjectPtr)cheAct->data.Stack[stpos], label, false, false, false);
              cheItem = new CHE(item);
              dbgStopData->ParamChain.Append(cheItem);
              stpos++;
              cheIO = (CHE*)cheIO->successor;
            }
          }
        }
      }
    }
  }
}

