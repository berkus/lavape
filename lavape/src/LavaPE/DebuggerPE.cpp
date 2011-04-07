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


#include "LavaPE_all.h"
#include "DebuggerPE.h"
#include "LavaPE.h"
#include "LavaBaseDoc.h"
#include "SynIDTable.h"
#include "ExecView.h"
#include "Constructs.h"

#include <errno.h>
//#include <prelude.h>
#include <QTcpSocket>
#include "MACROS.h"
#include "ASN1File.h"
//Added by qt3to4:

#pragma hdrstop

#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#include <process.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif


void CLavaPEDebugger::start() {
	QString interpreterPath, lavaFile = myDoc->GetFilename();
  quint16 locPort;

#ifdef WIN32
    interpreterPath = ExeDir + "/Lava.exe";
#elif __Darwin
    interpreterPath = ExeDir + "/Lava.app/Contents/MacOS/Lava";
#else
    interpreterPath = ExeDir + "/Lava";
#endif

  if (myDoc->debugOn) {
    if (!listenSocket) {
      listenSocket = new QTcpServer(this);
      connect(listenSocket,SIGNAL(newConnection()),SLOT(connectToClient()));
      listenSocket->listen();
    }
    locPort = listenSocket->serverPort();
    QString host_addr = "127.0.0.1";
    QStringList args;
	  args << lavaFile << host_addr << QString("%1").arg(locPort);
    if (!QProcess::startDetached(interpreterPath,args)) {
      QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),ERR_LavaStartFailed);
		  return;
	  }
  }
  else {
    workSocket = new QTcpSocket(this);
    connect(workSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(error(QAbstractSocket::SocketError)));
    connect(workSocket,SIGNAL(connected()),SLOT(connected()));
    workSocket->connectToHost(remoteIPAddress,remotePort);
  }
}

void CLavaPEDebugger::connectToClient() {
  workSocket = listenSocket->nextPendingConnection();
  connect(workSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(error(QAbstractSocket::SocketError)));
  connected();
}

void CLavaPEDebugger::connected() {
  isConnected = true;
  sendPending = false;

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

  dbgRequest = 0;
  if (myDoc->debugOn) {
    dbgRequest = new DbgMessage(Dbg_Continue);
    checkBrkPnts1();
    checkBrkPnts0();
    checkBrkPnts2();
    if (!dbgRequest->ContData.ptr)
      dbgRequest->ContData.ptr = new DbgContData;
    ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first = brkPnts.first;
    ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.last = brkPnts.last;
    if (dbgRequest->ContData.ptr)
      ((DbgContData*)dbgRequest->ContData.ptr)->ContType = dbg_Cont;
    CDPDbgMessage(PUT, put_cid, dbgRequest,false);
    put_cid->flush();
    if (put_cid->Done) {
      ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first = 0;
      ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.last = 0;
      sendPending = false;

    }
    else {
      stop(otherError);
      return;
    }
    startedFromLava = false;
  }
  else {
    startedFromLava = true;
    myDoc->debugOn = true;
    ((CPEBaseDoc*)myDoc)->changeNothing = true;
  }
  connect(workSocket,SIGNAL(readyRead()),SLOT(receive()));
  wxTheApp->selectionChanged = true;

}

void CLavaPEDebugger::receive() {
  if (!get_cid->bytesAvailable())
    return;
  wxTheApp->selectionChanged = true; // to enforce update of debugging buttons
  sendPending = true;
  ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->DebugPage->setEnabled(true);
  if (dbgReceived.lastReceived)
    delete dbgReceived.lastReceived;
  dbgReceived.lastReceived = dbgReceived.newReceived;
  dbgReceived.newReceived = new DbgMessage;
  CDPDbgMessage(GET, get_cid, dbgReceived.newReceived,false);
  if (get_cid->Done) {
    //errBeforeStarted = dbgReceived.newReceived->DbgData.ptr && !((DbgStopData*)dbgReceived.newReceived->DbgData.ptr)->StackChain.first;
    if (((DbgStopData*)dbgReceived.newReceived->DbgData.ptr)->stopReason == Stop_SynError)
      synErrReported = true;
    if (!dbgReceived.newReceived->DbgData.ptr || ((DbgStopData*)dbgReceived.newReceived->DbgData.ptr)->stopReason != Stop_Start)
	    QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebug,(void*)&dbgReceived));
    return;
  }
  else
    stop();
}

void CLavaPEDebugger::send() {
  if (dbgRequest->Command == Dbg_Continue)
    checkBrkPnts1();
  sendPending = false;
  ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->DebugPage->setEnabled(false);
  CDPDbgMessage(PUT, put_cid, dbgRequest,false);
  put_cid->flush();
  if (!put_cid->Done)
    stop();

  if (dbgRequest->Command == Dbg_Continue) {
    checkBrkPnts2();
    if (dbgReceived.newReceived)
		  ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->removeExecStackPos((DbgStopData*)dbgReceived.newReceived->DbgData.ptr, myDoc);
  }
  wxTheApp->selectionChanged = true;
}

void CLavaPEDebugger::error(QAbstractSocket::SocketError socketError) {
  if (socketError == QAbstractSocket::RemoteHostClosedError)
    stop(disconnected);
  else {
    qDebug() << "+++ socket error: " << workSocket->errorString();
    stop(otherError); // other error
  }
}

void CLavaPEDebugger::stop(DbgExitReason reason) {
  if (!isConnected)
    return;
  isConnected = false;
  sendPending = true;
  synErrReported = false;

  if (dbgReceived.newReceived) {
    delete dbgReceived.newReceived;
    dbgReceived.newReceived = 0;
  }
  if (dbgRequest) {
    delete dbgRequest;
    dbgRequest = 0;
  }
  if (get_cid)
    delete get_cid;
  get_cid= 0;
  if (put_cid)
    delete put_cid;
  put_cid= 0;

  if (reason != disconnected)
    workSocket->disconnectFromHost();
  workSocket = 0;

  if (myDoc) {
    myDoc->debugOn = false;
    ((CPEBaseDoc*)myDoc)->changeNothing = false;
  }
  wxTheApp->selectionChanged = true; // to enforce button updates
  if (startedFromLava/* || errBeforeStarted*/)
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->OnFileExit();//qApp->exit(0);
  else
    QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebug,(void*)0));
}


void CLavaPEDebugger::checkBrkPnts0()
{
  CHEProgPoint *chePP;
  chePP = (CHEProgPoint*)brkPnts.first;
  while (chePP) {
    if (myDoc->IDTable.GetDECL(chePP->data.FuncID))
      chePP->data.Skipped = false;
    else
      chePP->data.Skipped = true;
    chePP = (CHEProgPoint*)chePP->successor;
  }
}

void CLavaPEDebugger::checkBrkPnts1()
{
  CHEProgPoint *chePPnew, *rmPP;
  ProgPoint *runToPP;

  if (LBaseData->ContData) {
    chePPnew = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
    while (chePPnew) {
      if (chePPnew->data.FuncDoc) {
        chePPnew->data.FuncDocName = ((CHESimpleSyntax*)((CLavaBaseDoc*)chePPnew->data.FuncDoc)->IDTable.mySynDef->SynDefTree.first)->data.SyntaxName;
        chePPnew->data.FuncDocDir = ((CLavaBaseDoc*)chePPnew->data.FuncDoc)->IDTable.DocDir;
      }
      chePPnew->data.FuncID.nINCL = myDoc->IDTable.GetINCL(chePPnew->data.FuncDocName, chePPnew->data.FuncDocDir);
      if (chePPnew->data.FuncID.nINCL < 0) {
        rmPP = chePPnew;
        chePPnew = (CHEProgPoint*)chePPnew->successor;
        LBaseData->ContData->BrkPnts.Remove(rmPP->predecessor);
      }
      else
        chePPnew = (CHEProgPoint*)chePPnew->successor;
    }
    runToPP = LBaseData->ContData->RunToPnt.ptr;
    if (runToPP) {
      if (runToPP->FuncDoc) {
        runToPP->FuncDocName = ((CHESimpleSyntax*)((CLavaBaseDoc*)runToPP->FuncDoc)->IDTable.mySynDef->SynDefTree.first)->data.SyntaxName;
        runToPP->FuncDocDir = ((CLavaBaseDoc*)runToPP->FuncDoc)->IDTable.DocDir;
      }
      runToPP->FuncID.nINCL = myDoc->IDTable.GetINCL(runToPP->FuncDocName,runToPP->FuncDocDir);
      if (runToPP->FuncID.nINCL < 0) {
        LBaseData->ContData->RunToPnt.Destroy();
        LBaseData->ContData->ContType = dbg_Step;
      }
    }
    dbgRequest->ContData.ptr = LBaseData->ContData;
  }
  else
    dbgRequest->ContData.ptr = 0;
  LBaseData->ContData = 0;
}


void CLavaPEDebugger::checkBrkPnts2()
{
  CHEProgPoint *chePP, *chePPnew, *rmPP;

  if (!dbgRequest->ContData.ptr)
    return;
  chePPnew = (CHEProgPoint*)((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first;
  while (chePPnew) {
    if (!chePPnew->data.Activate) {
      chePP = (CHEProgPoint*)brkPnts.first;
      while (chePP && ((chePP->data.SynObjID != chePPnew->data.SynObjID)
             || (chePP->data.FuncID != chePPnew->data.FuncID)
             || (chePP->data.ExecType != chePPnew->data.ExecType)))
        chePP = (CHEProgPoint*)chePP->successor;
      if (chePP)
        brkPnts.Remove(chePP->predecessor);
      rmPP = chePPnew;
      chePPnew = (CHEProgPoint*)chePPnew->successor;
      ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.Remove(rmPP->predecessor);
    }
    else
      chePPnew = (CHEProgPoint*)chePPnew->successor;
  }
  if (brkPnts.last)
    brkPnts.last->successor = ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first;
  else
    brkPnts.first = ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first;
  if (((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.last)
    brkPnts.last =  ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.last;

  ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.first = 0;
  ((DbgContData*)dbgRequest->ContData.ptr)->BrkPnts.last = 0;
}

void CLavaPEDebugger::adjustBrkPnts()
{
  adjustBrkPnts(&brkPnts);
  if (LBaseData->ContData && LBaseData->ContData->BrkPnts.first)
    adjustBrkPnts(&LBaseData->ContData->BrkPnts);
}

void CLavaPEDebugger::adjustBrkPnts(CHAINANY* brkPntsChain)
//remember: this function is called only from main thread
{
  CHEProgPoint *chePP, *rmPP;
  bool doMess = false;
  LavaDECL *funcDecl, *execDecl;
  CSearchData sData;

  chePP = (CHEProgPoint*)brkPntsChain->first;
  while (chePP) {
    chePP->data.FuncID.nINCL = myDoc->IDTable.GetINCL(chePP->data.FuncDocName, chePP->data.FuncDocDir);
    if (chePP->data.FuncID.nINCL < 0) {
      doMess = doMess || chePP->data.FuncDoc;
      if (chePP->data.FuncDoc) {
        funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
        execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
        if (execDecl) {
          sData.synObjectID = chePP->data.SynObjID;
          sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
          sData.nextFreeID = 0;
          // sData.finished = false;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
          if (sData.synObj)
            ((SynObject*)sData.synObj)->workFlags.EXCL(isBrkPnt);
        }
      }
      rmPP = chePP;
      chePP = (CHEProgPoint*)chePP->successor;
      brkPntsChain->Remove(rmPP->predecessor);
      if (doMess)
        QMessageBox::information(wxTheApp->m_appWindow, qApp->applicationName(),"One or more breakpoints cannot not be set and have been removed");
    }
    else {
      if (!myDoc->IDTable.GetDECL(chePP->data.FuncID))
        chePP->data.Skipped = true;
      else {
        chePP->data.Skipped = false;
        if (/*chePP->data.Activate &&*/ chePP->data.FuncDoc) {
          funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
          execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
          if (execDecl) {
            sData.synObjectID = chePP->data.SynObjID;
            sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
            sData.nextFreeID = 0;
            // sData.finished = false;
            ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
            if (chePP->data.Activate)
              ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
            else
              ((SynObject*)sData.synObj)->workFlags.EXCL(isBrkPnt);
            //chePP->data.SynObj = sData.synObj;
          }
        }
      }
      chePP = (CHEProgPoint*)chePP->successor;
    }
  }
}

void CLavaPEDebugger::checkAndSetBrkPnts(CLavaBaseDoc* updatedDoc)
//remember: this function is called only from main thread, after local move
{
  CHEProgPoint *chePP;
  LavaDECL *funcDecl, *execDecl;
  CSearchData sData;

  chePP = (CHEProgPoint*)brkPnts.first;
  while (chePP) {
    if (chePP->data.Activate && ((CLavaBaseDoc*)chePP->data.FuncDoc == updatedDoc)) {
      funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
      execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
      if (execDecl) {
        sData.synObjectID = chePP->data.SynObjID;
        sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
        sData.nextFreeID = 0;
        // sData.finished = false;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
        if (sData.synObj)
          ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
        //chePP->data.SynObj = sData.synObj;
      }
    }
    chePP = (CHEProgPoint*)chePP->successor;
  }
  if (LBaseData->ContData)
  chePP = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
  while (chePP) {
    if (chePP->data.Activate && ((CLavaBaseDoc*)chePP->data.FuncDoc == updatedDoc)) {
      funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
      execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
      if (execDecl) {
        sData.synObjectID = chePP->data.SynObjID;
        sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
        sData.nextFreeID = 0;
        // sData.finished = false;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
        if (sData.synObj)
          ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
        //chePP->data.SynObj = sData.synObj;
      }
    }
    chePP = (CHEProgPoint*)chePP->successor;
  }
}

void CLavaPEDebugger::clearBrkPnts()
//remember: this function is called only from main thread
{
  int docPos, viewPos;
  CHEProgPoint *chePP;
  CLavaBaseView *view;
  wxDocument* doc;
  LavaDECL *funcDecl, *execDecl;
  CSearchData sData;

  if (LBaseData->ContData) {
    for (chePP = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
         chePP; chePP = (CHEProgPoint*)chePP->successor)
      if (chePP->data.FuncDoc) {
        funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
        execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
        if (execDecl) {
          sData.synObjectID = chePP->data.SynObjID;
          sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
          sData.nextFreeID = 0;
          // sData.finished = false;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
          ((SynObject*)sData.synObj)->workFlags.EXCL(isBrkPnt);
        }
      }
  }
  for (chePP = (CHEProgPoint*)brkPnts.first;
       chePP; chePP = (CHEProgPoint*)chePP->successor)
    if (chePP->data.FuncDoc) {
      funcDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable.GetDECL(0, chePP->data.FuncID.nID);
      execDecl = ((CLavaBaseDoc*)chePP->data.FuncDoc)->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
      if (execDecl) {
        sData.synObjectID = chePP->data.SynObjID;
        sData.doc = (CLavaBaseDoc*)chePP->data.FuncDoc;
        sData.nextFreeID = 0;
        // sData.finished = false;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&((CLavaBaseDoc*)chePP->data.FuncDoc)->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
        ((SynObject*)sData.synObj)->workFlags.EXCL(isBrkPnt);
      }
    }
  for (docPos = 0; docPos < wxTheApp->m_docManager->m_docs.size(); docPos++) {
    doc = wxTheApp->m_docManager->m_docs[docPos];
    for (viewPos = 0; viewPos < doc->m_documentViews.size(); viewPos++) {
      view = (CLavaBaseView*)doc->m_documentViews[viewPos];
      if (view->inherits("CExecView"))
        ((CExecView*)view)->redCtl->update();
    }
  }
  if (LBaseData->ContData)
    LBaseData->ContData->BrkPnts.Destroy();
  brkPnts.Destroy();
}

void CLavaPEDebugger::cleanBrkPoints(CLavaBaseDoc* closedDoc)
//remember: this function is called only from main thread
// after closing a document
{
  CHEProgPoint *chePP;
  if (LBaseData->ContData) {
    for (chePP = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
         chePP; chePP = (CHEProgPoint*)chePP->successor) {
           if (closedDoc == (CLavaBaseDoc*)chePP->data.FuncDoc) {
             //chePP->data.SynObj = 0;
             chePP->data.FuncDoc = 0;
           }
    }
  }
  for (chePP = (CHEProgPoint*)brkPnts.first;
       chePP; chePP = (CHEProgPoint*)chePP->successor) {
    if (closedDoc == (CLavaBaseDoc*)chePP->data.FuncDoc) {
      //chePP->data.SynObj = 0;
      chePP->data.FuncDoc = 0;
    }
  }
}


void CLavaPEDebugger::restoreBrkPoints(CLavaBaseDoc* openedDoc)
//remember: this function is called only from main thread
{
  LavaDECL *funcDecl, *execDecl;
  CheckData ckd;
  CSearchData sData;
  CHEProgPoint *chePP;

  ckd.document = myDoc;
  if (LBaseData->ContData) {
    for (chePP = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
         chePP; chePP = (CHEProgPoint*)chePP->successor) {
      if (!chePP->data.FuncDoc && SameFile(openedDoc->IDTable.IDTab[0]->FileName, openedDoc->IDTable.DocDir,
        chePP->data.FuncDocName, chePP->data.FuncDocDir)) {
        funcDecl = openedDoc->IDTable.GetDECL(0, chePP->data.FuncID.nID);
        execDecl = openedDoc->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
        if (execDecl) {
          sData.synObjectID = chePP->data.SynObjID;
          sData.doc = openedDoc;
          sData.nextFreeID = 0;
          // sData.finished = false;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&openedDoc->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
          //chePP->data.SynObj = sData.synObj;
          ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
          chePP->data.FuncDoc = (address)openedDoc;
        }
      }
    }
  }
  for (chePP = (CHEProgPoint*)brkPnts.first;
       chePP; chePP = (CHEProgPoint*)chePP->successor) {
    if (!chePP->data.FuncDoc && SameFile(openedDoc->IDTable.IDTab[0]->FileName, openedDoc->IDTable.DocDir,
          chePP->data.FuncDocName, chePP->data.FuncDocDir)) {
      funcDecl = openedDoc->IDTable.GetDECL(0, chePP->data.FuncID.nID);
      execDecl = openedDoc->GetExecDECL(funcDecl, chePP->data.ExecType, false,false);
      if (execDecl) {
        sData.synObjectID = chePP->data.SynObjID;
        sData.doc = openedDoc;
        sData.nextFreeID = 0;
        // sData.finished = false;
        ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&openedDoc->IDTable, 0, (SynObjectBase*)execDecl, onGetAddress, 0,0, (address)&sData);
        //chePP->data.SynObj = sData.synObj;
        ((SynObject*)sData.synObj)->workFlags.INCL(isBrkPnt);
        chePP->data.FuncDoc = (address)openedDoc;
      }
    }
  }
}


bool CLavaPEDebugger::checkExecBrkPnts(unsigned synObjIDold, unsigned synObjIDnew, int funcnID, TDeclType execType, CLavaBaseDoc* funcDoc)
//remember: this function is called only from main thread
{
  CHEProgPoint *chePP;
  chePP = (CHEProgPoint*)brkPnts.first;
  while (chePP &&
      (((CLavaBaseDoc*)chePP->data.FuncDoc != funcDoc)
      || (chePP->data.FuncID.nID != funcnID) || (chePP->data.ExecType != execType)
      || (chePP->data.SynObjID != synObjIDold)))
    chePP = (CHEProgPoint*)chePP->successor;
  if (chePP) {
    chePP->data.SynObjID = synObjIDnew;
    chePP->data.Skipped = false;
    return true;
  }
  else {
    if (LBaseData->ContData) {
      chePP = (CHEProgPoint*)(CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
      while (chePP &&
            (((CLavaBaseDoc*)chePP->data.FuncDoc != funcDoc)
            || (chePP->data.FuncID.nID != funcnID) || (chePP->data.ExecType != execType)
            || (chePP->data.SynObjID != synObjIDold)))
        chePP = (CHEProgPoint*)chePP->successor;
    }
    if (chePP) {
      chePP->data.SynObjID = synObjIDnew;
      chePP->data.Skipped = false;
      return true;
    }
  }
  return false;
}
