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


#include "LavaAppBase.h"
#include "LavaThread.h"
#include "LavaProgram.h"
#include "SynIDTable.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "DbgThread.h"
#include "Constructs.h"
#include "BAdapter.h"
#include "LavaGUIView.h"
#include "mdiframes.h"


#include "qstring.h"
#include "qmessagebox.h"
//Added by qt3to4:

#ifdef WIN32
#include <shlobj.h>
#include <float.h>
#else
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>
#ifndef __SunOS
#include <fenv.h>
#endif
#endif

#pragma hdrstop


#define ADJUST(nnn,decl) \
  nnn.nINCL = IDTable.IDTab[decl->inINCL]->nINCLTrans[nnn.nINCL].nINCL


CLavaProgram::CLavaProgram()
{
}

CLavaProgram::~CLavaProgram()
{
}


bool CLavaProgram::LoadSyntax(const QString& fn, SynDef*& sntx, bool reDef, bool putErr)
{
  int readResult; 
  QString *errCode; 
  LavaDECL *errDECL=0;
  bool errEx;
  QString /* *toINCL,*/ str;
  CheckData ckd;

  PathName = qPrintable(fn);
  readResult = ReadSynDef(fn, sntx);
  if (readResult < 0)
    sntx = 0;
//  toINCL = 0;
  if (sntx) {
    PathName = qPrintable(fn);
    isStd = SameFile(PathName, StdLava.ascii());
    CalcNames(fn);
    sntx->IDTable = (address)&IDTable;
    hasIncludes = FALSE;
    AddSyntax(sntx, fn, errEx);  //Add include files/patterns
    if (errEx)   //error allready indicated
      return false;
    if (errCode = IDTable.SetImplDECLs(errDECL)) {
      LavaError(ckd, false, errDECL, errCode);
      return false;
    }
    else
      return true;
  }
  else {
    if (readResult < 0) {
      if (reDef || putErr) {
        str = QString("Cannot read lava component file '") + fn;
        if (!putErr) {
          str += ", corrupt lava program syntax";
          str += "\n  ";
          str += "\nPerhaps different lava versions?"; 
          str += "\n  ";
        }
      }
      else {
        str = QString("Cannot read file '") + fn + ", corrupt lava program syntax";
        str += "\n  ";
        str += "\nPerhaps different lava versions?"; 
        str += "\n  ";
      }
      if (putErr || reDef) 
//        AfxMessageBox(str.c, MB_OK+MB_ICONSTOP);
        critical(wxTheApp->m_appWindow,qApp->name(),tr(str),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    }
    else {
      if (reDef) {
        str = QString("Lava component file '") + fn + "' not found";
        str += "\n  ";
        str += "\nDo you want to retry loading the lava document using another file name (*.lcom)?"; 
        str += "\n  ";
//        if (AfxMessageBox(str.c, MB_RETRYCANCEL|MB_ICONQUESTION) == IDRETRY) 
				if (question(wxTheApp->m_appWindow,qApp->name(),tr(str),QMessageBox::Retry,QMessageBox::Cancel,0)==QMessageBox::Retry)
          return SelectLcom(false);
      }
      else {
        if (putErr) {
          str = QString("File '") + fn + "' not found";
//          AfxMessageBox(str.c, MB_OK+MB_ICONSTOP);
          critical(wxTheApp->m_appWindow,qApp->name(),tr(str),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        }
      }
    }
    return false;
  }
}


bool CLavaProgram::OnOpenProgram(const QString lpszPathName, bool imiExec, bool reDef, bool putErr) 
{
  bool synOk=true;
  QString fName=PathName.c;

  if (!mySynDef) 
    if (lpszPathName.length())
      synOk = LoadSyntax(lpszPathName, mySynDef, reDef, putErr); //mySynDef from lava file
    else
      synOk = LoadSyntax(fName, mySynDef, reDef, putErr); //mySynDef from lava file
  if (!synOk || !mySynDef)
    return false;
//  wxDocManager::GetDocumentManager()->AddFileToHistory(fName);
  if (imiExec) {
    QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaStart, (void*)this));

  }
  return true;
}

void CLavaProgram::InitBAdapter()
{
  CheckData ckd;
  DECLTab[B_Object ]     = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Object ], 0);
  if (!DECLTab[B_Object ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Bitset ]       = IDTable.GetDECL(1, IDTable.BasicTypesID[Bitset ], 0);
  if (!DECLTab[Bitset ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Bool ]       = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Bool ], 0);
  if (!DECLTab[B_Bool ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Char ]         = IDTable.GetDECL(1, IDTable.BasicTypesID[Char ], 0);
  if (!DECLTab[Char ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Integer ]      = IDTable.GetDECL(1, IDTable.BasicTypesID[Integer ], 0);
  if (!DECLTab[Integer ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Float ]        = IDTable.GetDECL(1, IDTable.BasicTypesID[Float ], 0);
  if (!DECLTab[Float ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Double ]       = IDTable.GetDECL(1, IDTable.BasicTypesID[Double ], 0);
  if (!DECLTab[Double ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[VLString ]     = IDTable.GetDECL(1, IDTable.BasicTypesID[VLString ], 0);
  if (!DECLTab[VLString ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[Enumeration ]  = IDTable.GetDECL(1, IDTable.BasicTypesID[Enumeration ], 0);
  if (!DECLTab[Enumeration ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Set ]        = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Set ], 0);
  if (!DECLTab[B_Set ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Chain ]      = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Chain ], 0);
  if (!DECLTab[B_Chain ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Che ]        = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Che ], 0);
  if (!DECLTab[B_Che ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Array ]      = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Array ], 0);
  if (!DECLTab[B_Array ])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[ComponentObj ]         = IDTable.GetDECL(1, IDTable.BasicTypesID[ComponentObj], 0);
  if (!DECLTab[ComponentObj])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_Exception ]         = IDTable.GetDECL(1, IDTable.BasicTypesID[B_Exception], 0);
  if (!DECLTab[B_Exception])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_HWException ]         = IDTable.GetDECL(1, IDTable.BasicTypesID[B_HWException], 0);
  if (!DECLTab[B_HWException])
    LavaError(ckd, false, 0, &ERR_inStd);
  DECLTab[B_RTException ]         = IDTable.GetDECL(1, IDTable.BasicTypesID[B_RTException], 0);
  if (!DECLTab[B_RTException])
    LavaError(ckd, false, 0, &ERR_inStd);
  MakeStdAdapter();
}

bool CLavaProgram::CheckNamesp(CheckData& ckd, LavaDECL* nspDECL)
{
  if (nspDECL->WorkFlags.Contains(runTimeOK))
    return true;
  if (!CheckVElems(ckd, nspDECL, true, 0, 0))
    return false;
  if (!MakeSectionTable(ckd, nspDECL))
    return false;
  nspDECL->WorkFlags.INCL(runTimeOK);
  return true;
}

bool CLavaProgram::CheckContext(CheckData& ckd, const CContext& con)
{
  if (con.oContext) 
    if (con.oContext->DeclType == Package)
      CheckNamesp(ckd, con.oContext);
    else
      CheckImpl(ckd, con.oContext);
  if (ckd.exceptionThrown)
    return false;
  if (con.iContext) 
    if (con.iContext->DeclType == Package)
      CheckNamesp(ckd, con.iContext);
    else
      CheckImpl(ckd, con.iContext);
  if (ckd.exceptionThrown)
    return false;
  return true;
}


bool CLavaProgram::CheckImpl(CheckData& ckd, LavaDECL* classDECL, LavaDECL* specDECL)
{
  LavaDECL *classElDecl, *implElDecl, *implDECL = classDECL->RuntimeDECL, *execDECL;;
  CHE *cheImplEl, *cheClass=0;
  bool toImpl;
  TAdapterFunc* funcAdapter=0;
  CContext con;
  CheckData ckdl;
  CSearchData sData;
  CHE* elChe, *execChe;

  if (!classDECL)
    return false;

  ckdl.document = this;

  if (implDECL) {
    if (implDECL->WorkFlags.Contains(runTimeOK))
      return true;
  }
  else {
    if (classDECL->WorkFlags.Contains(implRequired)) {
      LavaError(ckd, true, classDECL, &ERR_NoClassImpl);
      return false;
    }
      //read implementation syntax
    else {
      classDECL->RuntimeDECL = classDECL;
      implDECL = classDECL;
    }
  }

  if (classDECL->TypeFlags.Contains(isNative) && (classDECL->DeclType != CompObjSpec)) {
    funcAdapter = GetAdapterTable(ckd, classDECL, specDECL);
    if (!funcAdapter
      && (!classDECL->TypeFlags.Contains(isComponent) || specDECL && (specDECL->DeclType == CompObjSpec))) {
      LavaError(ckd, true, classDECL, &ERR_NoNativeImpl); //native implementation not found
      return false;
    }
  }
  IDTable.GetPattern(classDECL, con);
  if (!CheckVElems(ckd, classDECL, false, (DWORD)funcAdapter, specDECL))
    return false;
  implDECL->WorkFlags.INCL(runTimeOK); //prevents circle calls of CheckImpl
  if (!CheckContext(ckd, con))
    return false;
  implDECL->WorkFlags.EXCL(runTimeOK);
  implDECL = classDECL->RuntimeDECL;

  if (LBaseData->m_checkInvariants)
    classDECL->Exec.ptr = new InvarData(ckd,classDECL);

  if (classDECL->NestedDecls.last) {
    execDECL = (LavaDECL*)((CHE*)classDECL->NestedDecls.last)->data;
    if (execDECL->DeclDescType == ExecDesc) {
      ckdl.myDECL = execDECL;
      ckdl.currentStackLevel = 0;
      ckdl.stackFrameSize = 0;
      ckdl.inINCL = execDECL->inINCL;
      try {
        ((SynObject*)execDECL->Exec.ptr)->Check (ckdl);
        sData.doc = ckdl.document;
        sData.nextFreeID = 0;
        sData.finished = false;
        ((SynObject*)execDECL->Exec.ptr)->MakeTable((address)&ckdl.document->IDTable, 0, (SynObjectBase*)ckdl.myDECL, onSetSynOID, 0,0, (address)&sData);
      }
      catch(CUserException) {
      }
      if (ckdl.exceptionThrown) {
        if (ckd.lastException)
          DEC_FWD_CNT(ckd, ckd.lastException);
        ckd.lastException = ckdl.lastException;
        ckd.exceptionThrown = true;
        return false;
      }
    }
  }
  if ((implDECL != classDECL) && implDECL->NestedDecls.last) {
    execDECL = (LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data;
    if (execDECL->DeclDescType == ExecDesc) {
      ckdl.myDECL = execDECL;
      ckdl.currentStackLevel = 0;
      ckdl.stackFrameSize = 0;
      ckdl.inINCL = execDECL->inINCL;
      try {
        ((SynObject*)execDECL->Exec.ptr)->Check (ckdl);
        sData.doc = ckdl.document;
        sData.nextFreeID = 0;
        sData.finished = false;
        ((SynObject*)execDECL->Exec.ptr)->MakeTable((address)&ckdl.document->IDTable, 0, (SynObjectBase*)ckdl.myDECL, onSetSynOID, 0,0, (address)&sData);
      }
      catch(CUserException) {
      }
      if (ckdl.exceptionThrown) {
        if (ckd.lastException)
          DEC_FWD_CNT(ckd, ckd.lastException);
        ckd.lastException = ckdl.lastException;
        ckd.exceptionThrown = true;
        return false;
      }
    }
  }
  
  cheClass = (CHE*)classDECL->NestedDecls.first;
  int nNative = LAH;  //length of adapter table header
  while (cheClass) {
    classElDecl = (LavaDECL*)cheClass->data;
    toImpl = classElDecl && (classElDecl->DeclType == Function)
             && !classElDecl->TypeFlags.Contains(isAbstract)
             && !classElDecl->TypeFlags.Contains(isNative);
    if (classElDecl->TypeFlags.Contains(isNative)) {
      if (classElDecl->TypeFlags.Contains(isGUI))
        if (classElDecl->TypeFlags.Contains(isGUIEdit))
          classElDecl->RuntimeDECL = (LavaDECL*)&ShowFuncEdit;
        else
          classElDecl->RuntimeDECL = (LavaDECL*)&ShowFuncFillIn;
      else
        if (funcAdapter)
          classElDecl->RuntimeDECL = (LavaDECL*)funcAdapter[nNative];
        /*
        else {
          LavaError(ckd, true, classElDecl, &ERR_NoNativeImpl); //native implementation not found
          return false;
        }
        */
      nNative++;
    }
    if (toImpl) {
      cheImplEl = (CHE*)implDECL->NestedDecls.first;
      if (cheImplEl)
        implElDecl = (LavaDECL*)cheImplEl->data;
      while (cheImplEl && ((implElDecl->DeclType != Function) 
             || !implElDecl->SecondTFlags.Contains(funcImpl)
             || !IDTable.EQEQ(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL, TID(classElDecl->OwnID, classElDecl->inINCL), 0) )) {
        cheImplEl = (CHE*)cheImplEl->successor;
        if (cheImplEl)
          implElDecl = (LavaDECL*)cheImplEl->data;
      }
      if (cheImplEl) { 
        if (!CheckFuncImpl(ckd, implElDecl, classElDecl))
          return false;
        implElDecl->WorkFlags.INCL(checkmark);
      }
      else {
        LavaError(ckd, true, classElDecl, &ERR_NoFuncImpl);
        return false;
      }
    }
    cheClass = (CHE*)cheClass->successor;
  }
  if (! CheckSetAndGets(ckd, implDECL, classDECL))
    return false;
  if (implDECL->DeclType == Impl) { 
    for (cheImplEl = (CHE*)implDECL->NestedDecls.first; cheImplEl; cheImplEl = (CHE*)cheImplEl->successor) {
      implElDecl = (LavaDECL*)cheImplEl->data;
      if (implElDecl->WorkFlags.Contains(checkmark)) 
        implElDecl->WorkFlags.EXCL(checkmark);
      else 
        if ((implElDecl->DeclType == Function) 
          && implElDecl->SecondTFlags.Contains(funcImpl)) {
          if (implElDecl->TypeFlags.Contains(isPropGet) || implElDecl->TypeFlags.Contains(isPropSet)) {
            classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
            if (!classElDecl || !classElDecl->TypeFlags.Contains(hasSetGet))
              new CLavaError(&implElDecl->DECLError1, &ERR_NoSetGetMember);
          }
          else
            LavaError(ckd, true, implElDecl, &ERR_MissingItfFuncDecl);
          return false;
        }
      if ((implElDecl->DeclType == Function) 
           && !implElDecl->TypeFlags.Contains(isAbstract)
           && !implElDecl->TypeFlags.Contains(isNative)) {
        implElDecl->RuntimeDECL = (LavaDECL*)((CHE*)implElDecl->NestedDecls.last)->data;
        if (implElDecl->RuntimeDECL->DeclType != ExecDef) {
          LavaError(ckd, true, classDECL, &ERR_MissingExec);
          return false;
        }
      }
    }
  }
  if (implDECL->TypeFlags.Contains(isGUI)) {
    for (cheImplEl = (CHE*)implDECL->NestedDecls.first;
         cheImplEl && (((LavaDECL*)cheImplEl->data)->DeclType != FormDef);
         cheImplEl = (CHE*)cheImplEl->successor);
    if (!cheImplEl) {
      LavaError(ckd, true, implDECL, &ERR_CorruptForm);
      return false;
    }
    if (!CheckForm(ckd, (LavaDECL*)cheImplEl->data))
      return false;
  }
  if (!MakeSectionTable(ckd, classDECL))
    return false;
  // remember: all function exec checks must be done after construction of 
  // the section table
  CSectionDesc* secTab = (CSectionDesc*)implDECL->SectionTabPtr;
  for (elChe = (CHE*)classDECL->NestedDecls.first; elChe; elChe = (CHE*)elChe->successor) {
    classElDecl = (LavaDECL*)elChe->data;
    if (classElDecl->DeclType == Function) {
      classElDecl->Exec.ptr = new AssertionData(ckdl,classElDecl);
      for (execChe = (CHE*)classElDecl->NestedDecls.last;
           execChe && (((LavaDECL*)execChe->data)->DeclDescType == ExecDesc);
           execChe = (CHE*)execChe->predecessor) {
        execDECL = (LavaDECL*)execChe->data;
        ckdl.myDECL = execDECL;
        ckdl.currentStackLevel = 0;
        ckdl.stackFrameSize = 0;
        ckdl.inINCL = execDECL->inINCL;
        implDECL->WorkFlags.INCL(runTimeOK);
        try {
          ((SynObject*)execDECL->Exec.ptr)->Check (ckdl);
          sData.doc = ckdl.document;
          sData.nextFreeID = 0;
          sData.finished = false;
          ((SynObject*)execDECL->Exec.ptr)->MakeTable((address)&ckdl.document->IDTable, 0, (SynObjectBase*)ckdl.myDECL, onSetSynOID, 0,0, (address)&sData);
        }
        catch(CUserException) {
        }
        if (ckdl.exceptionThrown) {
          if (ckd.lastException)
            DEC_FWD_CNT(ckd, ckd.lastException);
          ckd.lastException = ckdl.lastException;
          ckd.exceptionThrown = true;
          return false;
        }
        implDECL->WorkFlags.EXCL(runTimeOK);
      }
    }
  }
  if (implDECL != classDECL) {
    for (elChe = (CHE*)implDECL->NestedDecls.first; elChe; elChe = (CHE*)elChe->successor) {
      implElDecl = (LavaDECL*)elChe->data;
      if (implElDecl->DeclType == Function) {
        if (!implElDecl->Supports.first
        || implElDecl->TypeFlags.Contains(isPropGet)
        || implElDecl->TypeFlags.Contains(isPropSet))
          implElDecl->Exec.ptr = new AssertionData(ckdl,implElDecl);
        for (execChe = (CHE*)implElDecl->NestedDecls.first;
             execChe;
             execChe = (CHE*)execChe->successor) {
          if (((LavaDECL*)execChe->data)->DeclDescType == ExecDesc) {
            execDECL = (LavaDECL*)execChe->data;
            ckdl.myDECL = execDECL;
            ckdl.currentStackLevel = 0;
            ckdl.stackFrameSize = 0;
            ckdl.inINCL = execDECL->inINCL;
            implDECL->WorkFlags.INCL(runTimeOK);
            try {
              ((SynObject*)execDECL->Exec.ptr)->Check (ckdl);
              sData.nextFreeID = 0;
              sData.doc = ckdl.document;
              sData.nextFreeID = 0;
              sData.finished = false;
              ((SynObject*)execDECL->Exec.ptr)->MakeTable((address)&ckdl.document->IDTable, 0, (SynObjectBase*)ckdl.myDECL, onSetSynOID, 0,0, (address)&sData);
            }
            catch(CUserException) {
            }
            if (ckdl.exceptionThrown) {
              if (ckd.lastException)
                DEC_FWD_CNT(ckd, ckd.lastException);
              ckd.lastException = ckdl.lastException;
              ckd.exceptionThrown = true;
              return false;
            }
            implDECL->WorkFlags.EXCL(runTimeOK);
            if ((execDECL->DeclType == ExecDef) && !implElDecl->TypeFlags.Contains(isStatic)
               && !implElDecl->TypeFlags.Contains(isPropGet)
               && !implElDecl->TypeFlags.Contains(isPropSet)) {
                secTab[execDECL->SectionInfo2].funcDesc[execDECL->SectionInfo1].stackFrameSize
                                          = ((SelfVar*)execDECL->Exec.ptr)->stackFrameSize;
                if (implElDecl->TypeFlags.Contains(defaultInitializer))
                  secTab[execDECL->SectionInfo2].funcDesc[0].stackFrameSize
                                       = ((SelfVar*)execDECL->Exec.ptr)->stackFrameSize;
            }
          }
        }
      }
    }
  }
  implDECL->WorkFlags.INCL(runTimeOK);
  return true;
}


bool CLavaProgram::CheckFuncImpl(CheckData& ckd, LavaDECL* funcDECL, LavaDECL* classFuncDECL)
{
  CHE *cheimplIOEl,  *checlassIOEl;
  LavaDECL *implIOEl, *classIOEl;
  QString *err;

  funcDECL->TypeFlags.EXCL(isAbstract);
  funcDECL->TypeFlags.EXCL(isNative);
  if (classFuncDECL->TypeFlags.Contains(isConst) != funcDECL->TypeFlags.Contains(isConst)) {
    LavaError(ckd, true, funcDECL, &ERR_funcImpl);
    return false;
  }
  cheimplIOEl = (CHE*)funcDECL->NestedDecls.first;
  if (!cheimplIOEl) {
    LavaError(ckd, true, funcDECL, &ERR_funcImpl);
    return false;
  }
  checlassIOEl = (CHE*)classFuncDECL->NestedDecls.first;
  while (cheimplIOEl && checlassIOEl
    && (((LavaDECL*)cheimplIOEl->data)->DeclDescType != ExecDesc)
    && (((LavaDECL*)checlassIOEl->data)->DeclDescType != ExecDesc) ) {
    implIOEl = (LavaDECL*)cheimplIOEl->data;
    classIOEl = (LavaDECL*)checlassIOEl->data;
    if ((implIOEl->DeclType != classIOEl->DeclType)
        || !IDTable.EQEQ(((CHETID*)implIOEl->Supports.first)->data, implIOEl->inINCL, TID(classIOEl->OwnID, classIOEl->inINCL), 0) 
        || !IDTable.EQEQ(implIOEl->RefID, implIOEl->inINCL, classIOEl->RefID, classIOEl->inINCL)) {
      LavaError(ckd, true, funcDECL, &ERR_funcImpl);
      return false;
    }
    cheimplIOEl = (CHE*)cheimplIOEl->successor;
    checlassIOEl = (CHE*)checlassIOEl->successor;
  }
  if ( checlassIOEl && (((LavaDECL*)checlassIOEl->data)->DeclDescType != ExecDesc)
     || !cheimplIOEl
     || cheimplIOEl && (((LavaDECL*)cheimplIOEl->data)->DeclDescType != ExecDesc)
     || (((LavaDECL*)((CHE*)funcDECL->NestedDecls.last)->data)->DeclType != ExecDef) ) {
    LavaError(ckd, true, funcDECL, &ERR_funcImpl);
    return false;
  }
  if (classFuncDECL->Inherits.first) {
    err = CheckException(classFuncDECL, &ckd);
    if (err) {
      LavaError(ckd, true, classFuncDECL, err);
      return false;
    }
  }
  classFuncDECL->RuntimeDECL = (LavaDECL*)((CHE*)funcDECL->NestedDecls.last)->data; //cheimplIOEl->data;
  return true;
}

bool CLavaProgram::CheckSetAndGets(CheckData& ckd, LavaDECL* implDECL, LavaDECL* classDECL)
{
  LavaDECL *ifaceElDecl, *getImpl;
  CHE *cheI=0, *cheImpl;
  bool toImpl;
  CHETVElem *El;
  El = (CHETVElem*)classDECL->VElems.VElems.first;
  cheImpl = (CHE*)implDECL->NestedDecls.first;
  if (!El) {
    cheI = (CHE*)classDECL->NestedDecls.first;
    if (!cheI) {
      cheI = cheImpl;
      cheImpl = 0;
    }
  }
  while (El || cheI) { 
    if (El) {
      ifaceElDecl = IDTable.GetDECL(El->data.VTEl);
      toImpl = ifaceElDecl && (ifaceElDecl->DeclType == Attr) 
               && ifaceElDecl->TypeFlags.Contains(isAbstract)
               && ifaceElDecl->TypeFlags.Contains(hasSetGet);
    }
    else {
      ifaceElDecl = (LavaDECL*)cheI->data;
      toImpl = ifaceElDecl && (ifaceElDecl->DeclType == Attr)
               && ifaceElDecl->TypeFlags.Contains(hasSetGet)
               && !ifaceElDecl->TypeFlags.Contains(isAbstract)
               && !ifaceElDecl->TypeFlags.Contains(isNative);
    }
    if (toImpl) {
      if (!CheckOneSetGet(ckd, isPropGet, implDECL, ifaceElDecl))
        return false;
      getImpl = ifaceElDecl->RelatedDECL;
      if (!CheckOneSetGet(ckd, isPropSet, implDECL, ifaceElDecl))
        return false;
      ifaceElDecl->RelatedDECL->RelatedDECL = getImpl; //propDecl->setExec->getExec
    }
    if (El) {
      El = (CHETVElem*)El->successor;
      if (!El) {
        cheI = (CHE*)classDECL->NestedDecls.first;
        if (!cheI) {
          cheI = cheImpl;
          cheImpl = 0;
        }
      }
    }
    else {
      cheI = (CHE*)cheI->successor;
      if (!cheI) {
        cheI = cheImpl;
        cheImpl = 0;
      }
    }
  }//all properties
  return true;
}

bool CLavaProgram::CheckOneSetGet(CheckData& ckd, TypeFlag setgetflag, LavaDECL* implDECL,
                                        LavaDECL* propDecl)
{
  CHE *cheio, *cheSetGet = (CHE*)implDECL->NestedDecls.first;
  if (!cheSetGet) {
    LavaError(ckd, true, propDecl, &ERR_SetGetError);
    return false;
  }
  LavaDECL *setGet = (LavaDECL*)cheSetGet->data;
  while (cheSetGet && ( (setGet->DeclType != Function) 
         || !setGet->TypeFlags.Contains(setgetflag)
         || !IDTable.EQEQ(((CHETID*)setGet->Supports.first)->data, setGet->inINCL, TID(propDecl->OwnID, propDecl->inINCL), 0))) {
    cheSetGet = (CHE*)cheSetGet->successor;
    if (cheSetGet)
      setGet = (LavaDECL*)cheSetGet->data;
  }
  if (!cheSetGet) {
    LavaError(ckd, true, propDecl, &ERR_SetGetError);
    return false;
  }
  TDeclType dt;
  if (setgetflag == isPropGet)
    dt = OAttr;
  else
    dt = IAttr; 
  LavaDECL* ioDECL;
  bool ok = false;
  for (cheio = (CHE*)setGet->NestedDecls.first; cheio && !ok; cheio = (CHE*)cheio->successor) {
    ioDECL = (LavaDECL*)cheio->data;
    ok = (ioDECL->DeclType == dt) && IDTable.EQEQ(ioDECL->RefID, ioDECL->inINCL, propDecl->RefID, propDecl->inINCL);
  }
  if (ok) {
    propDecl->RelatedDECL = (LavaDECL*)((CHE*)setGet->NestedDecls.last)->data;
    setGet->WorkFlags.INCL(checkmark);
  }
  else {
    LavaError(ckd, true, propDecl, &ERR_SetGetError);
    return false;
  }
  return true;
}

bool CLavaProgram::CheckFuncInOut(CheckData& ckd, LavaDECL* funcDECL)
{
  LavaDECL *IODECL, *OverFunc, *IOType;
  CHE *cheOverIO, *cheIO;
  CHETID* cheID;
  SynFlags typeFlags;
  bool catErr;
  QString *errCode;

  for (cheIO = (CHE*)funcDECL->NestedDecls.first; cheIO; cheIO = (CHE*)cheIO->successor) {
    IODECL = (LavaDECL*)cheIO->data;
    if (IODECL->DeclDescType != ExecDesc) {
      IOType = IDTable.GetDECL(IODECL->RefID, IODECL->inINCL);
      if (!IOType) {
        LavaError(ckd, true, IODECL, &ERR_NoRefType);
        return false;
      }
      if (errCode = TypeForMem(IODECL, IOType, &ckd)) {
        LavaError(ckd, true, IODECL, errCode, IOType);
        return false;
      }
    }
  }
  for (cheID = (CHETID*)funcDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    OverFunc = IDTable.GetDECL(cheID->data, funcDECL->inINCL);
    if (!OverFunc) {
      LavaError(ckd, true, funcDECL, &ERR_NoOverridden);
      return false;
    }
    if (OverFunc->TypeFlags.Contains(isStatic)) {
      LavaError(ckd, true, funcDECL, &ERR_OverriddenStatic);
      return false;
    }
    if (OverFunc->TypeFlags.Contains(isConst))
      funcDECL->TypeFlags.INCL(isConst);
    if (!OverFunc->TypeFlags.Contains(isProtected))
      funcDECL->TypeFlags.EXCL(isProtected);
    cheOverIO = (CHE*)OverFunc->NestedDecls.first;
    cheIO = (CHE*)funcDECL->NestedDecls.first;
    while (cheOverIO && cheIO
      && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)
      && (((LavaDECL*)cheOverIO->data)->DeclDescType != ExecDesc)) {
      IODECL = (LavaDECL*)cheIO->data;
      if ((IODECL->DeclType != ((LavaDECL*)cheOverIO->data)->DeclType) 
        || !OverriddenMatch(IODECL, false, &ckd)) {
        LavaError(ckd, true, IODECL, &ERR_IOParams);
        return false;
      }
      typeFlags = IODECL->TypeFlags;
      GetCategoryFlags(IODECL, catErr);
      if (catErr || (typeFlags != IODECL->TypeFlags)) {
        LavaError(ckd, true, IODECL, &ERR_OverriddenDiffs);
        return false;
      }
      cheIO = (CHE*)cheIO->successor;
      cheOverIO = (CHE*)cheOverIO->successor;
    }
    if (cheOverIO && (((LavaDECL*)cheOverIO->data)->DeclDescType != ExecDesc)
      || cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
      LavaError(ckd, true, IODECL, &ERR_IOParams);
      return false;
    }
  }
  return true;
}

bool CLavaProgram::CheckForm(CheckData& ckd, LavaDECL* formDECL, int)
{
  LavaDECL *classElDecl, *formElDecl, *classDECL;
  CHE *cheformEl, *checlassEl;
  TID tid;
  QString *err;

  if (formDECL->WorkFlags.Contains(runTimeOK))
    return true;
  classDECL = IDTable.GetDECL(formDECL->RefID, formDECL->inINCL);
  if (!classDECL) {
    LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm);
    return false;
  }
  if (!classDECL->usableIn(formDECL)) {
    LavaError(ckd, true, formDECL->ParentDECL, &ERR_InvisibleType, classDECL);
    return false;
  }
  formDECL->RelatedDECL = classDECL;
  formDECL->ParentDECL->RuntimeDECL = IDTable.GetDECL(((CHETID*)formDECL->ParentDECL->Supports.first)->data, formDECL->inINCL);
  if (!formDECL->ParentDECL->RuntimeDECL) {
    LavaError(ckd, true, formDECL->ParentDECL, &ERR_NoClassImpl); 
    return false;
  }
  formDECL->ParentDECL->RelatedDECL = classDECL;
  formDECL->ParentDECL->RuntimeDECL->RelatedDECL = classDECL;
  for (; classDECL && (classDECL->DeclType == VirtualType); classDECL = IDTable.GetDECL(classDECL->RefID, classDECL->inINCL));
  if (!classDECL || (classDECL->DeclDescType != formDECL->DeclDescType)) {
    LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
    return false;
  }
  if (formDECL->DeclDescType == EnumType) {
    return CheckMenu(ckd, formDECL, classDECL);
  }
  if (formDECL->DeclDescType == StructDesc) {
    checlassEl = (CHE*)classDECL->NestedDecls.first;
    if (checlassEl)
      classElDecl = (LavaDECL*)checlassEl->data;
    cheformEl = (CHE*)formDECL->NestedDecls.first;
    while (cheformEl) {
      formElDecl = (LavaDECL*)cheformEl->data;
      if (formElDecl->DeclType == FormText) {
        cheformEl = (CHE*)cheformEl->successor;
      }
      else {
        if (!formElDecl->Supports.first) {
          LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
          return false;
        }
        tid = ((CHETID*)formElDecl->Supports.first)->data;
        ADJUST(tid, formElDecl);
        while (checlassEl  
            && (TID(classElDecl->OwnID, classElDecl->inINCL) != tid)) {
            checlassEl = (CHE*)checlassEl->successor;
            if (checlassEl) 
              classElDecl = (LavaDECL*)checlassEl->data;
        }
        if (checlassEl
          && ((classElDecl->DeclType == Attr) && classElDecl->TypeFlags.Contains(constituent)
              || (classElDecl->DeclType == VirtualType) && classElDecl->SecondTFlags.Contains(isSet) 
              || (classElDecl->DeclType == VirtualType) && classElDecl->SecondTFlags.Contains(isArray) )) {
          formElDecl->RelatedDECL = classElDecl;
          formElDecl->RuntimeDECL = IDTable.GetDECL(formElDecl->RefID, formElDecl->inINCL);
          err = CheckFormEl(formElDecl, classElDecl);
          if (err) {
            LavaError(ckd, true, formElDecl, err, formElDecl->RuntimeDECL);
            return false;
          }
          cheformEl = (CHE*)cheformEl->successor;
        }
        else {
          LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
          return false;
        }
      }
    }
  }
  formDECL->WorkFlags.INCL(runTimeOK);
  return true;
}

bool CLavaProgram::CheckMenu(CheckData& ckd, LavaDECL* formDECL, LavaDECL* classDECL)
{
  LavaDECL* inElClass = &((TEnumDescription*)classDECL->EnumDesc.ptr)->EnumField;
  LavaDECL* inElForm = &((TEnumDescription*)formDECL->EnumDesc.ptr)->EnumField;
  CHE* inDefElform = (CHE*)((TEnumDescription*)formDECL->EnumDesc.ptr)->MenuTree.NestedDecls.first;
  CHEEnumSelId* enumselClass = (CHEEnumSelId*)inElClass->Items.first;
  CHEEnumSelId* enumselForm = (CHEEnumSelId*)inElForm->Items.first;
  while (enumselClass) { 
    if (inDefElform && (((LavaDECL*)inDefElform->data)->LocalName == enumselClass->data.Id)) {
      inDefElform = (CHE*)inDefElform->successor;
      enumselClass = (CHEEnumSelId*)enumselClass->successor;
      enumselForm = (CHEEnumSelId*)enumselForm->successor;
    }
    else {
      if (inDefElform && ((LavaDECL*)inDefElform->data)->LocalName.l) {
        if (enumselForm->data.SelectionCode.l) {
          LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
          return false;
        }
        enumselClass = (CHEEnumSelId*)enumselClass->successor;
        enumselForm = (CHEEnumSelId*)enumselForm->successor;
      }
      else {
        if (inDefElform)
          inDefElform = (CHE*)inDefElform->successor;
        else {
          if (!enumselForm
              || (enumselForm->data.Id != enumselClass->data.Id)
              || enumselForm->data.SelectionCode.l) {
            LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
            return false;
          }
          enumselClass = (CHEEnumSelId*)enumselClass->successor;
          enumselForm = (CHEEnumSelId*)enumselForm->successor;
        }
      }
    }
  }
  while (inDefElform) {
    if (((LavaDECL*)inDefElform->data)->LocalName.l) {
      LavaError(ckd, true, formDECL->ParentDECL, &ERR_CorruptForm2);
      return false;
    }
    else
      inDefElform = (CHE*)inDefElform->successor;
  }
  return true;
}


bool CLavaProgram::AddVElems(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseclassDECL)
{
  CHETVElem *El, *lastEl, *ElC;
  CHETVElem* addElem = (CHETVElem*)baseclassDECL->VElems.VElems.first;
  TID IDClss;
  while (addElem) {
    for (El = (CHETVElem*)classDECL->VElems.VElems.first;
         El && (El->data.VTClss != addElem->data.VTClss);
         El = (CHETVElem*)El->successor);
    if (!El) {
      //it is from a new class, add all entries from this class
      IDClss = addElem->data.VTClss;
      while (addElem && (addElem->data.VTClss == IDClss)) {
        El = new CHETVElem;
        El->data = addElem->data;
        classDECL->VElems.VElems.Append(El);
        addElem = (CHETVElem*)addElem->successor;
      }
    }
    else {
      ElC = El;
      for (lastEl = El; lastEl && (lastEl->data.VTClss == addElem->data.VTClss); 
           lastEl = (CHETVElem*)lastEl->successor);
      //is it or is an extension allready in the table?
      while (El && !IDTable.IsAn(El->data.VTEl, 0, addElem->data.VTEl, 0))
        if (El == lastEl)
          El = 0;
        else
          El = (CHETVElem*)El->successor;
      if (!El) {
        El = ElC;
        //is it an extension of an element in the table?
        while (El && !IDTable.IsAnc(addElem->data.VTEl, 0, El->data.VTEl, 0))
          if (El == lastEl)
            El = 0;
          else
            El = (CHETVElem*)El->successor;
        if (El) {
          El->data.VTEl = addElem->data.VTEl; 
          El->data.op = addElem->data.op;
          El->data.ElDECL = addElem->data.ElDECL;
        }
        else {
          //find the position in the table
          El = IDTable.FindSamePosInVT(addElem, ElC, lastEl);
          if (El) { //same pos in VT
            if (!IDTable.Overrides(El->data.VTEl, 0, addElem->data.VTEl, 0, classDECL)) {
              if (IDTable.Overrides(addElem->data.VTEl, 0, El->data.VTEl, 0, classDECL)) {
                El->data.VTEl = addElem->data.VTEl; 
                El->data.TypeFlags = addElem->data.TypeFlags;
                El->data.op = addElem->data.op;
                El->data.ok = addElem->data.ok;
              }
              else {
                LavaError(ckd, true, addElem->data.ElDECL, &ERR_NoOverridden); //mehrdeutig
                return false;
              }
            }
            //else El overrides addElem
          }//same pos in VT
          else {
            LavaError(ckd, true, addElem->data.ElDECL, &ERR_NoOverridden);
            return false;
          }
        }
      }
      addElem = (CHETVElem*)addElem->successor;
    }
  }
  return true;
}


int CLavaProgram::GetMemsSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* memDECL, bool putErr)
{
  if (!classDECL)
    classDECL = IDTable.GetDECL(memDECL->RefID, memDECL->inINCL);
  return GetSectionNumber(ckd, classDECL, memDECL->ParentDECL, putErr);
}

int CLavaProgram::GetSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL, bool putErr) 
{
  if (classDECL) {
    if (classDECL->DeclType == Impl )
      classDECL = IDTable.GetDECL(((CHETID*)classDECL->Supports.first)->data, classDECL->inINCL);//!
  }
  else
    return 0;
  if (baseDECL && (baseDECL->DeclType == Impl) ) {
    baseDECL = IDTable.GetDECL(((CHETID*)baseDECL->Supports.first)->data, baseDECL->inINCL);//!
    if (!baseDECL) {
      LavaError(ckd, true, classDECL, &ERR_Broken_ref);
      return -1;
    }
  }
  if (!classDECL) {
    LavaError(ckd, true, classDECL, &ERR_Broken_ref);
    return -1;
  }
  if (!AllocSectionTable(ckd, classDECL))
    return -1;
  int nsec, prim_nsec = 0;
  int nSections = ((CSectionDesc*)classDECL->SectionTabPtr)[0].nSections;
  for (nsec = 0; 
       (nsec < nSections) 
      && ( ((CSectionDesc*)classDECL->SectionTabPtr)[nsec].classDECL != baseDECL); 
      nsec++)
    if (((CSectionDesc*)classDECL->SectionTabPtr)[nsec].SectionFlags.Contains(SectPrimary))
      prim_nsec++;

  if (nsec < nSections)
    return nsec;
  else 
    if (!baseDECL)
      return prim_nsec-1;
    else {
      if (putErr) {
        LavaError(ckd, true, classDECL, &ERR_NotBaseIF, baseDECL);
      }
      return -1;
    }
}

int CLavaProgram::getSectionNumber(CheckData& ckd, LavaDECL *classDECL, LavaDECL* baseDECL) 
{
  if (!classDECL ) {
    LavaError(ckd, true, classDECL, &ERR_InVTofBaseIF);
    return -1;
  }
  if (!AllocSectionTable(ckd, classDECL))
    return -1;
  int nsec;
  int nSections = ((CSectionDesc*)classDECL->SectionTabPtr)[0].nSections;
  for (nsec = 0; 
       (nsec < nSections) 
      && ( ((CSectionDesc*)classDECL->SectionTabPtr)[nsec].classDECL != baseDECL); 
      nsec++);
  if (nsec < nSections)
    return nsec;
  else 
    LavaError(ckd, true, classDECL, &ERR_NotBaseIF, baseDECL);
  return -1;
}

int CLavaProgram::GetSectionNumber(CheckData& ckd, const TID& classID, const TID& baseclassID)
{
  LavaDECL* classDECL = IDTable.GetDECL(classID);
  LavaDECL* baseDECL = IDTable.GetDECL(baseclassID);
  return GetSectionNumber(ckd, classDECL, baseDECL);
}

int CLavaProgram::GetVTSectionNumber(CheckData& ckd, const CContext& context, LavaDECL* paramDECL, bool& outer)
{
  if (paramDECL->DeclType != VirtualType) {
    outer = false;
    return 0;
  }
  int nr = GetSectionNumber(ckd, context.oContext, paramDECL->ParentDECL, false);
  if (nr >= 0) {
    outer = true;
    return nr;
  }
  else {
    outer = false;
    return GetSectionNumber(ckd, context.iContext, paramDECL->ParentDECL);
  }
}


LavaObjectPtr CLavaProgram::CastVInObj(CheckData& ckd, LavaObjectPtr obj, const CContext& callCtx, LavaDECL* ET_casted, LavaDECL* VT_f, int v0, bool outer )
{
  if (VT_f->DeclType != VirtualType)
    return 0;
  LavaObjectPtr objectCasted = (LavaObjectPtr)obj;
  int secN_call, secN_casted;
  LavaDECL *fvalCasted;
  CContext castedCtx(((CSectionDesc*)ET_casted->SectionTabPtr)->innerContext, ((CSectionDesc*)ET_casted->SectionTabPtr)->outerContext);
  CSectionDesc *casted_tab, *call_tab;
  if (!CheckContext(ckd, castedCtx))
    return 0;
  if (!CheckContext(ckd, callCtx))
    return 0;
  secN_call = v0 + VT_f->SectionInfo2; /*v0 = GetVTSectionNumber(callCtx, ET_call, outer)*/
  if (outer && callCtx.oContext || !outer && !callCtx.iContext) {
    call_tab = (CSectionDesc*)callCtx.oContext->SectionTabPtr;
    if (!castedCtx.oContext) {
      casted_tab = (CSectionDesc*)castedCtx.iContext->SectionTabPtr;
      secN_casted = getSectionNumber(ckd, castedCtx.iContext, call_tab[secN_call].classDECL);
      if (ckd.exceptionThrown)
        return 0;
      fvalCasted = casted_tab[secN_casted].vtypeDesc[VT_f->SectionInfo1].fValue;
    }
    else {
      casted_tab = (CSectionDesc*)castedCtx.oContext->SectionTabPtr;
      secN_casted = getSectionNumber(ckd, castedCtx.oContext, call_tab[secN_call].classDECL);
      if (ckd.exceptionThrown)
        return 0;
      fvalCasted = casted_tab[secN_casted].vtypeDesc[VT_f->SectionInfo1].fValue;
    }
  }
  else {
    call_tab = (CSectionDesc*)callCtx.iContext->SectionTabPtr;
    if (!castedCtx.iContext) {
      casted_tab = (CSectionDesc*)castedCtx.oContext->SectionTabPtr;
      secN_casted = getSectionNumber(ckd, castedCtx.oContext, call_tab[secN_call].classDECL);
      if (ckd.exceptionThrown)
        return 0;
      fvalCasted = casted_tab[secN_casted].vtypeDesc[VT_f->SectionInfo1].fValue;  
    }
    else {
      casted_tab = (CSectionDesc*)castedCtx.iContext->SectionTabPtr;
      secN_casted = getSectionNumber(ckd, castedCtx.iContext, call_tab[secN_call].classDECL);
      if (ckd.exceptionThrown)
        return 0;
      fvalCasted = casted_tab[secN_casted].vtypeDesc[VT_f->SectionInfo1].fValue;  
    }
  }

  if (!fvalCasted)
    fvalCasted = DECLTab[B_Object];
  objectCasted = objectCasted - objectCasted[0][0].sectionOffset;
  secN_casted = getSectionNumber(ckd, objectCasted[0][0].classDECL, fvalCasted);
  if (ckd.exceptionThrown)
    return 0;
  return objectCasted + objectCasted[0][secN_casted].sectionOffset;
}


int CLavaProgram::GetVStatCallSecN(CheckData& ckd, LavaObjectPtr lowObj, const CContext& lowCtx, LavaDECL* vTypeLow, int vsecLow, bool outer )
{
  if (vTypeLow->DeclType != VirtualType)
    return 0;
  int vsecHigh;
  CSectionDesc *lowConTab, *highConTab;
  LavaDECL* vValueHigh;
  LavaObjectPtr highObj = lowObj - (*lowObj)->sectionOffset;
  if (outer)
    lowConTab = (CSectionDesc*)lowCtx.oContext->SectionTabPtr;
  else 
    lowConTab = (CSectionDesc*)lowCtx.iContext->SectionTabPtr;
  if (outer)
    if (!lowCtx.iContext && (*highObj)[0].innerContext)
      highConTab = (CSectionDesc*)(*highObj)[0].innerContext->SectionTabPtr;
    else
      highConTab = (CSectionDesc*)(*highObj)[0].outerContext->SectionTabPtr;
  else
    highConTab = (CSectionDesc*)(*highObj)[0].innerContext->SectionTabPtr;
  vsecHigh = getSectionNumber(ckd, highConTab->classDECL, lowConTab[vsecLow].classDECL);
  if (ckd.exceptionThrown)
    return 0;
  vValueHigh = highConTab[vsecHigh].vtypeDesc[vTypeLow->SectionInfo1].fValue;
  return getSectionNumber(ckd, (*highObj)[0].classDECL, vValueHigh);
}

LavaObjectPtr CLavaProgram::CastVOutObj(CheckData& ckd, LavaObjectPtr obj, const CContext& callCtx, LavaDECL* VT_f, int v0, bool outer )
{
  if (VT_f->DeclType != VirtualType)
    return 0;
  LavaObjectPtr objectCasted = (LavaObjectPtr)obj;
  int secN_call, secN_casted;
  LavaDECL *fvalCall;
  CSectionDesc *call_tab;
  if (!CheckContext(ckd, callCtx))
    return 0;
  secN_call = v0 + VT_f->SectionInfo2; /*v0 = GetVTSectionNumber(callCtx, ET_call, outer)*/
  if (outer && callCtx.oContext || !outer && !callCtx.iContext) {
    call_tab = (CSectionDesc*)callCtx.oContext->SectionTabPtr;
    fvalCall = call_tab[secN_call].vtypeDesc[VT_f->SectionInfo1].fValue;  
  }
  else {
    call_tab = (CSectionDesc*)callCtx.iContext->SectionTabPtr;
    fvalCall = call_tab[secN_call].vtypeDesc[VT_f->SectionInfo1].fValue;  
  }

  if (!fvalCall)
    fvalCall = DECLTab[B_Object];
  objectCasted = objectCasted - objectCasted[0][0].sectionOffset;
  secN_casted = getSectionNumber(ckd, objectCasted[0][0].classDECL, fvalCall);
  if (ckd.exceptionThrown)
    return 0;
  return objectCasted + objectCasted[0][secN_casted].sectionOffset;
}

LavaObjectPtr CLavaProgram::CastVObj(CheckData& ckd, LavaObjectPtr obj, LavaDECL* eType)
{
  if (!obj)
    return 0;
  if (!eType)
    eType = DECLTab[B_Object];
  obj = obj - obj[0][0].sectionOffset;
  int nn = getSectionNumber(ckd, obj[0][0].classDECL, eType);
  if (ckd.exceptionThrown)
    return 0;
  return obj + obj[0][nn].sectionOffset;
}

bool CLavaProgram::MakeFormVT(LavaDECL *decl, CheckData* pckd)
{
  LavaDECL *classDECL, **pbaseFDECL, *baseFDECL;
  CHETVElem *El, *Elbase;
  if (!decl)
    return false;
  if (decl->WorkFlags.Contains(formVTOK))
    return true;
  CHETID* cheID = (CHETID*)decl->Supports.first; //id of basic gui interface
  if (decl->DeclType == FormDef) {  
    while (cheID) {
      baseFDECL = IDTable.GetDECL(cheID->data, decl->inINCL);
      if (!baseFDECL) {
        LavaError(*pckd, true, decl, &ERR_Broken_ref);
        return false;
      }
      if (!baseFDECL->TypeFlags.Contains(isGUI)) {
        LavaError(*pckd, true, decl, &ERR_NoExtForm);
        return false;
      }
      if (!baseFDECL->usableIn(decl)) {
        LavaError(*pckd, true, decl, &ERR_InvisibleType, baseFDECL);
        return false;
      }
      pbaseFDECL = GetFormpDECL(baseFDECL->RuntimeDECL);
      if (pbaseFDECL && *pbaseFDECL) {
        baseFDECL = *pbaseFDECL;
        if (!MakeFormVT(baseFDECL, pckd))
          return false;
        for (Elbase = (CHETVElem*)baseFDECL->VElems.VElems.first; Elbase; Elbase = (CHETVElem*)Elbase->successor) {
          El = (CHETVElem*)decl->VElems.VElems.first;
          while (El && (El->data.VTEl != Elbase->data.VTEl))
            El = (CHETVElem*)El->successor;
          if (!El) {
            El = new CHETVElem;
            El->data.VTClss = Elbase->data.VTClss;
            El->data.VTEl = Elbase->data.VTEl; 
            decl->VElems.VElems.Append(El);
          }
        }
      }
      cheID = (CHETID*)cheID->successor;
    }
  }
  if (decl->DeclType == FormDef)
    classDECL = IDTable.GetDECL(decl->RefID, decl->inINCL);
  else
    classDECL = decl;
  if (!classDECL) {
    LavaError(*pckd, true, decl, &ERR_Broken_ref);
    return false;
  }
  El = new CHETVElem;
  El->data.VTClss = TID( decl->OwnID, decl->inINCL);
  El->data.VTEl = TID( classDECL->OwnID, classDECL->inINCL); 
  decl->VElems.VElems.Append(El);
  decl->WorkFlags.INCL(formVTOK);
  return true;
}


bool CLavaProgram::AddVBase(CheckData& ckd, LavaDECL* decl, LavaDECL* conDECL )
{
  LavaDECL *IFace;
  CHETVElem *El;
  if (!decl)
    return false;
  CHETID *cheID = (CHETID*)decl->Supports.first;
  while (cheID) { 
    IFace = IDTable.GetDECL(cheID->data, decl->inINCL);
    if (IFace && (IFace->DeclType == VirtualType)) {
      IFace = IDTable.GetFinalBasicType(cheID->data, decl->inINCL, conDECL);
      if (!MakeVElems(IFace, &ckd))
        return false;
      if (!AddVElems(ckd, conDECL, IFace))
        return false;
      if (IFace->fromBType == NonBasic) {
        for (El = (CHETVElem*)IFace->VElems.VElems.first;
             El && ((IFace != El->data.ClssDECL)
                    || (El->data.ElDECL->DeclType != Function)
                    || !El->data.ElDECL->TypeFlags.Contains(defaultInitializer));
             El = (CHETVElem*)El->successor);
        if (!El || (El->data.ElDECL->DeclType != Function)
          || !El->data.ElDECL->TypeFlags.Contains(defaultInitializer)) {
          LavaError(ckd, true, conDECL, &ERR_VBaseWithoutDefaultIni);
          return false;
        }
      }
    }
    if (!AddVBase(ckd, IFace, conDECL))
      return false;
    cheID = (CHETID*)cheID->successor;
  }
  return true;
}

bool CLavaProgram::MakeVElems(LavaDECL *classDECL, CheckData* pckd)
{
  //use of classDECL->VElems.UpdateNo:
  //   = 0, before MakeVElems,
  //   = 1, check complete,
  //   = -1, in calculation of VT, prevents circle calls of MakeVElems in erroneous lava programs
  //   = -2, in checks after calculation of VT,

  if (!classDECL || (classDECL->DeclType == VirtualType))
    return false;
  if (classDECL->WorkFlags.Contains(runTimeOK))  //=CheckImpl, MakeVElems, MakeSectionTable ok
    return true;
  if (classDECL->VElems.UpdateNo == -1) 
    return false;  //prevents circle calls of MakeVElems
  if (classDECL->VElems.UpdateNo == 1)
    return true;
  if (classDECL->VElems.UpdateNo == -2)
    return true;

  CHETID *cheID, *cheIDA;
  LavaDECL *baseDECL = 0, *ElDECL;
  CHETVElem *El;
  SynFlags typeFlags;
  bool inTab, catErr, hasEnum = false;;
  QString *errID;
  CHE* cheDecl;
  TID declID = TID(-1,0);
  classDECL->VElems.UpdateNo = -1;
  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {//!
    baseDECL = IDTable.GetFinalBasicType(cheID->data, classDECL->inINCL, classDECL);
    if (!baseDECL) {
      LavaError(*pckd, true, classDECL, &ERR_Broken_ref);
      return false;
    }
    if (!MakeVElems(baseDECL, pckd) || !AddVElems(*pckd, classDECL, baseDECL))
      return false;
  }  
  if (!AddVBase(*pckd, classDECL, classDECL))
    return false;
  for (cheDecl = (CHE*)classDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
    ElDECL = (LavaDECL*)cheDecl->data;
    if ( ( ( ElDECL->DeclType == VirtualType)
         || ( ElDECL->DeclType == Function)
         || ( ElDECL->DeclType == Attr)
         )
        && ElDECL->Supports.first) {
      //check and replace the extensions
      El = (CHETVElem*)classDECL->VElems.VElems.first;
      cheID = (CHETID*)ElDECL->Supports.first;
      inTab = false;
      while (cheID) {
        while (El && !inTab && !IDTable.EQEQ(El->data.VTEl, 0, cheID->data, ElDECL->inINCL)) {
          cheIDA = (CHETID*)El->data.Ambgs.first;
          while (cheIDA && !IDTable.EQEQ(cheIDA->data, 0, cheID->data, ElDECL->inINCL))
            cheIDA = (CHETID*)cheIDA->successor;
          inTab = cheIDA != 0;
          if (!inTab)
            El = (CHETVElem*)El->successor;
        }
        inTab = El != 0;
        if (!inTab) {
          El = (CHETVElem*)classDECL->VElems.VElems.first;
          cheID = (CHETID*)cheID->successor;
        }
        else
          cheID = 0;
      }
      if (El) {
        if (ElDECL->DeclType == Function) {
          if (!CheckFuncInOut(*pckd, ElDECL))
            return false;
        }
        else {
          if (!OverriddenMatch(ElDECL, false, pckd)) {
            LavaError(*pckd, true, ElDECL, &ERR_OverriddenDiffs);
            return false;
          }
          if ((ElDECL->DeclType == VirtualType) && (ElDECL->RefID.nID >= 0)
            && !IDTable.CheckValOfVirtual(ElDECL, false)) {
            LavaError(*pckd, true, classDECL, &ERR_InvalidValOfVT);
            return false;
          }
          if (baseDECL && (ElDECL->DeclType == Attr)) {
            if (!baseDECL->TypeFlags.Contains(isConst))
              ElDECL->TypeFlags.EXCL(isConst);
            if (!baseDECL->TypeFlags.Contains(isProtected))
              ElDECL->TypeFlags.EXCL(isProtected);
          }
          typeFlags = ElDECL->TypeFlags;
          GetCategoryFlags(ElDECL, catErr);
          if (catErr || (typeFlags != ElDECL->TypeFlags)) {
            LavaError(*pckd, true, classDECL, &ERR_OverriddenDiffs);
            return false;
          }
        }
        El->data.VTEl = TID(ElDECL->OwnID, ElDECL->inINCL);
        El->data.op = ElDECL->op;
        El->data.Ambgs.Destroy();
        El->data.ElDECL = ElDECL;
        if ((ElDECL->DeclType != Function) && !ElDECL->TypeFlags.Contains(isAbstract)) {
          ElDECL->RuntimeDECL = IDTable.GetDECL(ElDECL->RefID, ElDECL->inINCL);
          if (!ElDECL->RuntimeDECL) {
            LavaError(*pckd, true, ElDECL, &ERR_Broken_ref);
            return false;
          }
          if ((ElDECL->DeclType == VirtualType) 
            && (errID = TestValOfVirtual(ElDECL, ElDECL->RuntimeDECL))) {
            LavaError(*pckd, true, classDECL, errID);
            return false;
          }
          else
            if ((ElDECL->DeclType == Attr) 
              && (errID = TypeForMem(ElDECL, ElDECL->RuntimeDECL, pckd))) {
              LavaError(*pckd, true, ElDECL, errID, ElDECL->RuntimeDECL);
              return false;
            }
        }
      }
      else  {//the overridden object was not found
        LavaError(*pckd, true, ElDECL, &ERR_NoOverridden);
        return false;
      }
    }
  }
  for (El = (CHETVElem*)classDECL->VElems.VElems.first;
       El && !El->data.Ambgs.first;
       El = (CHETVElem*)El->successor);
  if (El) {
    LavaError(*pckd, true, classDECL, &ERR_AmbgInVT);
    return false;
  }
  TCTVElem vElems;
  for (cheDecl = (CHE*)classDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
    ElDECL = (LavaDECL*)cheDecl->data;
    if ( (  ( ElDECL->DeclType == VirtualType) 
         || ( ElDECL->DeclType == Function) && !ElDECL->TypeFlags.Contains(isStatic)
         || ( ElDECL->DeclType == Attr))
         && !ElDECL->Supports.first) {
      El = new CHETVElem;
      El->data.VTClss = TID(classDECL->OwnID, classDECL->inINCL);
      El->data.VTEl = TID(ElDECL->OwnID, classDECL->inINCL);
      El->data.op =ElDECL->op;
      El->data.ElDECL = ElDECL;
      El->data.ClssDECL = classDECL;
      vElems.Append(El);
      if ((ElDECL->DeclType != Function) && !ElDECL->TypeFlags.Contains(isAbstract)) {
        ElDECL->RuntimeDECL = IDTable.GetDECL(ElDECL->RefID, ElDECL->inINCL);
        if (!ElDECL->RuntimeDECL) {
          LavaError(*pckd, true, ElDECL, &ERR_Broken_ref);
          return false;
        }
      }
    }
  }
  if (!AttachPrivatElems(*pckd, classDECL))
    return false;
  if (vElems.first) {
    vElems.last->successor = classDECL->VElems.VElems.first;
    if (classDECL->VElems.VElems.first)
      classDECL->VElems.VElems.first->predecessor = vElems.last;
    else
      classDECL->VElems.VElems.last = vElems.last;
    classDECL->VElems.VElems.first = vElems.first;
  }
  vElems.first = 0;
  vElems.last = 0;

  bool isCreatable = !classDECL->TypeFlags.Contains(isAbstract);
  for (El = (CHETVElem*)classDECL->VElems.VElems.first;
       El && El->data.ok;
       El = (CHETVElem*)El->successor) {
    ElDECL = IDTable.GetDECL(El->data.VTEl);
    if (isCreatable) {
      if (ElDECL->TypeFlags.Contains(isAbstract)) {
        LavaError(*pckd, true, classDECL, &ERR_AbstractInherited, ElDECL);
        return false;
      }
    }
    if (ElDECL->TypeFlags.Contains(forceOverride)
      && (ElDECL->ParentDECL != classDECL)) {
      LavaError(*pckd, true, classDECL, &ERR_ForceOver, ElDECL);
      return false;
    }
    if (declID != El->data.VTClss) {
      declID = El->data.VTClss;
      baseDECL = IDTable.GetDECL(declID);
      if (baseDECL->DeclDescType == EnumType)
        if (hasEnum) {
          LavaError(*pckd, true, classDECL, &ERR_OneEnum, baseDECL);
          return false;
        }
        else
          hasEnum = true;
    }
  }
  //now make checks that uses virtual table

  classDECL->VElems.UpdateNo = -2;
  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {//!
    baseDECL = IDTable.GetDECL(cheID->data, classDECL->inINCL);
    if (errID = ExtensionAllowed(classDECL, baseDECL, pckd)) {
      LavaError(*pckd, true, classDECL, errID, baseDECL);
      return false;
    }
  }  

  for (cheDecl = (CHE*)classDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
    ElDECL = (LavaDECL*)cheDecl->data;
    if (ElDECL->DeclType == Function) {
      if (!CheckFuncInOut(*pckd, ElDECL))
        return false;
    }
    else
      if (ElDECL->DeclType == VirtualType) {
        if (!ElDECL->TypeFlags.Contains(isAbstract)
          && (errID = TestValOfVirtual(ElDECL, ElDECL->RuntimeDECL))) {
          LavaError(*pckd, true, ElDECL->RuntimeDECL, errID);
          return false;
        }
      }
      else 
        if ((ElDECL->DeclType == Attr)
          && (errID = TypeForMem(ElDECL, ElDECL->RuntimeDECL, pckd))) {
          LavaError(*pckd, true, ElDECL, errID, ElDECL->RuntimeDECL);
          return false;
        }
  }

  classDECL->VElems.UpdateNo = 1;
  return true;
}

bool CLavaProgram::AttachPrivatElems(CheckData& ckd, LavaDECL* classDECL)
{
  // attach the private attr and function
  TCTVElem vElems;
  CHETVElem *El;
  CHE* cheDecl;
  QString *errID;
  LavaDECL* ElDECL;
  if (classDECL->RuntimeDECL && (classDECL != classDECL->RuntimeDECL)) {
    for (cheDecl = (CHE*)classDECL->RuntimeDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
      ElDECL = (LavaDECL*)cheDecl->data;
      if ( ( (ElDECL->DeclType == Function) && !ElDECL->TypeFlags.Contains(isStatic)
          || ( ElDECL->DeclType == Attr))
          && !ElDECL->Supports.first) {
        El = new CHETVElem;
        El->data.VTClss = TID(classDECL->OwnID, classDECL->inINCL);
        El->data.VTEl = TID(ElDECL->OwnID, classDECL->inINCL);
        El->data.op = ElDECL->op;
        vElems.Append(El);
        El->data.ElDECL = ElDECL;
        El->data.ClssDECL = classDECL;
        if (ElDECL->DeclType == Function) {
          if (!CheckFuncInOut(ckd, ElDECL))
            return false;
        }
        else {
          ElDECL->RuntimeDECL = IDTable.GetDECL(ElDECL->RefID, ElDECL->inINCL);
          if (!ElDECL->RuntimeDECL) {
            LavaError(ckd, true, ElDECL, &ERR_Broken_ref); 
            return false;
          }
          if (errID = TypeForMem(ElDECL, ElDECL->RuntimeDECL, &ckd)) {
            LavaError(ckd, true, ElDECL, errID, ElDECL->RuntimeDECL);
            return false;
          }
        }
      }
    }  
  }
  for (El = (CHETVElem*)classDECL->VElems.VElems.first; 
       El && El->successor && (classDECL == ((CHETVElem*)El->successor)->data.ClssDECL);
       El = (CHETVElem*)El->successor);
  if (vElems.first) {
    if (El) {
      if (El->data.ClssDECL == classDECL) {
        vElems.last->successor = El->successor;
        El->successor = vElems.first;
        if (classDECL->VElems.VElems.last == El) 
          classDECL->VElems.VElems.last = vElems.last;
      }
      else {
        vElems.last->successor = classDECL->VElems.VElems.first; 
        classDECL->VElems.VElems.first = vElems.first;
      }

    }
    else {
      classDECL->VElems.VElems.first = vElems.first;
      classDECL->VElems.VElems.last = vElems.last;
    }
  }
  vElems.first = 0;
  vElems.last = 0;
  return true;
}

bool CLavaProgram::CheckVElems(CheckData& ckd, LavaDECL *classDECL, bool fromNameSp, DWORD fAdapt, LavaDECL* specDECL)
{
  if (classDECL->WorkFlags.Contains(runTimeOK))  //=CheckImpl, MakeVElems, MakeSectionTable ok
    return true;
  CSectionDesc *secTab;
  LavaDECL *implDECL, *ElDECL;
  int ii, ifrom;
  CHE *cheDecl;
  TAdapterFunc* funcAdapter;

  if (!MakeVElems(classDECL, &ckd) )//|| !AttachPrivatElems(ckd, classDECL))
    return false;
  if (!classDECL->SectionTabPtr)
    if (!AllocSectionTable(ckd, classDECL))
      return false;
  if (classDECL->DeclType == Package) {
    if (fromNameSp)
      ifrom = 1;
    else 
      ifrom = 0;
    for (ii = ifrom;  ii < classDECL->nSection; ii++) {
      if (!CheckNamesp(ckd, ((CSectionDesc*)classDECL->SectionTabPtr)[ii].classDECL))
        return false;
    }
  }
  else
    for (ii = 1;  ii < classDECL->nSection; ii++) {
      if (!CheckImpl(ckd, ((CSectionDesc*)classDECL->SectionTabPtr)[ii].classDECL, specDECL))
        return false;
    }
  implDECL = classDECL->RuntimeDECL;
  if (!implDECL)
    if (classDECL->WorkFlags.Contains(implRequired)) {
      LavaError(ckd, true, classDECL, &ERR_NoClassImpl);
      return false;
    }
    else
      classDECL->RuntimeDECL = classDECL;
  secTab = (CSectionDesc*)classDECL->SectionTabPtr;
  /*
  if (!secTab) {
    classDECL->SectionTabPtr = new CSectionDesc[nSection];
    secTab = (CSectionDesc*)classDECL->SectionTabPtr;
    secTab[0].nSections = classDECL->nSection;
    secTab[0].classDECL = classDECL;
  }
  */
  if (implDECL)
    secTab[0].implDECL = implDECL;
  else
    secTab[0].implDECL = classDECL;

  cheDecl = (CHE*)classDECL->NestedDecls.first;
  classDECL->SectionInfo1 = 1;  //remember: the 0-position in the virtual function table
                                //is reserved for the default constructor
  funcAdapter = (TAdapterFunc*)fAdapt;
  if (classDECL->TypeFlags.Contains(isNative) && funcAdapter)
    classDECL->SectionInfo2 = (unsigned)*funcAdapter[0];
  else
    classDECL->SectionInfo2 = 0;
  classDECL->SectionInfo3 = 0;
  for (cheDecl = (CHE*)classDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
    ElDECL = (LavaDECL*)cheDecl->data;
    if ( (  ( ElDECL->DeclType == VirtualType) 
         || ( ElDECL->DeclType == Function) && !ElDECL->TypeFlags.Contains(isStatic)
         || ( ElDECL->DeclType == Attr))
         && !ElDECL->Supports.first) {
      if (ElDECL->DeclType == Function)
        classDECL->SectionInfo1++;
      else
        if (ElDECL->DeclType == VirtualType) 
          classDECL->SectionInfo3++;
        else 
          classDECL->SectionInfo2++;
    }
  }
  //now attach the private attr and function
  if (classDECL->RuntimeDECL && (classDECL != classDECL->RuntimeDECL)) {
    for (cheDecl = (CHE*)classDECL->RuntimeDECL->NestedDecls.first; cheDecl; cheDecl = (CHE*)cheDecl->successor) {
      ElDECL = (LavaDECL*)cheDecl->data;
      if ( (  (ElDECL->DeclType == Function)
           || (ElDECL->DeclType == Attr))
          && !ElDECL->Supports.first) 
        if (ElDECL->DeclType == Function)
          classDECL->SectionInfo1++;
        else 
          classDECL->SectionInfo2++;
    }  
  }
  return true;
}

bool CLavaProgram::nVirtualBase(CheckData& ckd, LavaDECL *classDECL, LavaDECL* conDECL)
{
  CHETID *cheID;
  LavaDECL *baseDECL, *baseDECLMap;

  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    baseDECL = IDTable.GetFinalDef(cheID->data, classDECL->inINCL);
    baseDECLMap = IDTable.GetFinalBasicType(cheID->data, classDECL->inINCL, conDECL);
    if (baseDECL != baseDECLMap) {
      if (!baseDECLMap) {
        LavaError(ckd, true, classDECL, &ERR_Broken_ref);
        return false;
      }
      if (!baseDECLMap->usableIn(classDECL)) {
        LavaError(ckd, true, classDECL, &ERR_InvisibleBasictype, baseDECLMap);
        return false;
      }
      baseDECL = baseDECLMap;
      if (!AllocSectionTable(ckd, baseDECL))
        return false;
      conDECL->nSection += baseDECL->nSection;
    }
    if (!nVirtualBase(ckd, baseDECL, conDECL))
      return false;
  }
  return true;
}

bool CLavaProgram::AddVirtualBase(CheckData& ckd, LavaDECL *classDECL, LavaDECL* conDECL, int posSect)
{
  int ii, posS;
  CHETID *cheID;
  LavaDECL *baseDECL, *baseDECLMap, *vbaseDECL;
  CSectionDesc *secTab, *baseTab;

  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    vbaseDECL = IDTable.GetDECL(cheID->data, classDECL->inINCL);
    if (vbaseDECL && (vbaseDECL->DeclType == VirtualType)) {
      baseDECL = IDTable.GetFinalDef(cheID->data, classDECL->inINCL);
      baseDECLMap = IDTable.GetFinalBasicType(cheID->data, classDECL->inINCL, conDECL);
      if (baseDECL != baseDECLMap) {
        baseDECL = baseDECLMap;
        if (!AddVirtualBase(ckd, baseDECL, conDECL, posSect))
          return false;
        if (!baseDECL) {
          LavaError(ckd, true, classDECL, &ERR_Broken_ref);
          return false;
        }
        if (!baseDECL->usableIn(classDECL)) {
          LavaError(ckd, true, classDECL, &ERR_InvisibleBasictype, baseDECL);
          return false;
        }
        baseTab = (CSectionDesc*)baseDECL->SectionTabPtr;
        secTab = (CSectionDesc*)conDECL->SectionTabPtr;
        posS = posSect;
        for (ii = 0; ii < baseTab[0].nSections; ii++) {
          secTab[posS] = baseTab[ii];
          secTab[posS].funcDesc = 0;
          secTab[posS].attrDesc = 0;
          secTab[posS].vtypeDesc = 0; 
          secTab[posS].outerContext = 0; 
          secTab[posS].innerContext = 0; 
          posS++;
        }     
        posSect = posS;
      }
    }
    else 
      if (!AddVirtualBase(ckd, vbaseDECL, conDECL, posSect))
        return false;
  }
  return true;
}


bool CLavaProgram::AllocSectionTable(CheckData& ckd, LavaDECL *classDECL)
{
  if (classDECL->SectionTabPtr)
    return true;;
  CHETID *cheID;
  LavaDECL *baseDECL;
  CSectionDesc *secTab, *baseTab;
  int ii, inINCL, posSect;

  classDECL->nSection = 1;
  inINCL = classDECL->inINCL;
  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    baseDECL = IDTable.GetFinalDef(cheID->data, inINCL);
    if (!baseDECL) {
      LavaError(ckd, true, classDECL, &ERR_Broken_ref);
      return false;
    }
  if (!baseDECL->usableIn(classDECL)) {
      LavaError(ckd, true, classDECL, &ERR_InvisibleBasictype, baseDECL);
      return false;
    }
    if (!AllocSectionTable(ckd, baseDECL))
      return false;
    classDECL->nSection = classDECL->nSection + baseDECL->nSection;
    if (!nVirtualBase(ckd, baseDECL, classDECL))
      return false;
  }
  classDECL->SectionTabPtr = new CSectionDesc[classDECL->nSection];
  secTab = (CSectionDesc*)classDECL->SectionTabPtr;
  secTab[0].nSections = classDECL->nSection;
  secTab[0].classDECL = classDECL;
  secTab[0].sectionOffset = 0;
  secTab[0].SectionFlags.INCL(SectPrimary);
  posSect = 1;
  for (cheID = (CHETID*)classDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    baseDECL = IDTable.GetFinalDef(cheID->data, inINCL);
    baseTab = (CSectionDesc*)baseDECL->SectionTabPtr;
    for (ii = 0; ii < baseTab[0].nSections; ii++) {
      secTab[posSect] = baseTab[ii];
      secTab[posSect].funcDesc = 0;
      secTab[posSect].attrDesc = 0;
      secTab[posSect].vtypeDesc = 0; 
      secTab[posSect].outerContext = 0; 
      secTab[posSect].innerContext = 0; 
      posSect++;
    }
    if (!AddVirtualBase(ckd, baseDECL, classDECL, posSect))
      return false;
  }
  
  for (posSect = 1; posSect < secTab[0].nSections; posSect++) {
    for (ii = 0;
         (ii < posSect) && (secTab[ii].classDECL != secTab[posSect].classDECL);
         ii++);
    if (ii == posSect) 
      secTab[posSect].SectionFlags.INCL(SectPrimary);
    else
      secTab[posSect].SectionFlags.EXCL(SectPrimary);
  }
  return true;
}

bool CLavaProgram::MakeSectionTable(CheckData& ckd, LavaDECL* classDECL)
{
  if (classDECL->WorkFlags.Contains(runTimeOK))
    return true;

  int ii, posSect = 1, inii,
      posAttr = 0, posFunc = 0, posParam = 0,
      posAttrSect = 0, posFuncSect = 0, posParamSect = 0, nNative,
      offset=0, inINCL,
      totalInfo1, totalInfo2, totalInfo3;
  CSectionDesc *secTab, *baseTab, *valueTab;
  LavaDECL *ElDECL, *ElImpl, *parentDECL, *implDECL;
  CContext con;
  CHETVElem *El;
  TID  TIDDef, TIDCl;
  unsigned dist;
  bool outerParam;
  TAdapterFunc* funcAdapter=0;
  CVTypeDesc *pd;
  CVFuncDesc *fd;    
  CVAttrDesc *ad;    

  secTab = (CSectionDesc*)classDECL->SectionTabPtr;
  implDECL = classDECL->RuntimeDECL;
  inINCL = classDECL->inINCL;
  totalInfo1 = classDECL->SectionInfo1;
  totalInfo2 = classDECL->SectionInfo2; 
  totalInfo3 = classDECL->SectionInfo3; 

  //fill in section table and calculate length of function-, attr- and param table

  for (posSect = 0; posSect < classDECL->nSection; posSect++) {
    baseTab = (CSectionDesc*)secTab[posSect].classDECL->SectionTabPtr;
    secTab[posSect] = baseTab[0];
    for (inii = 1; 
         (inii < posSect)  && (secTab[inii].implDECL != baseTab[0].implDECL);
         inii++);
    if (inii == posSect) {
      totalInfo1 += baseTab[0].classDECL->SectionInfo1;
      totalInfo2 += baseTab[0].classDECL->SectionInfo2;
      totalInfo3 += baseTab[0].classDECL->SectionInfo3;
    }
  }
  if (totalInfo1)
    secTab[0].funcDesc = new CVFuncDescX [totalInfo1];
  else
    secTab[0].funcDesc = 0;
  if (totalInfo2) 
    secTab[0].attrDesc = new CVAttrDesc [totalInfo2];
  else
    secTab[0].attrDesc = 0;
  if (totalInfo3) 
    secTab[0].vtypeDesc = new CVTypeDesc [totalInfo3];
  else
    secTab[0].vtypeDesc = 0;
  IDTable.GetPattern(classDECL, con);
//  if (con.iContext)
    secTab[0].innerContext = con.iContext; //((CSectionDesc*)con.iContext->SectionTabPtr)->vtypeDesc;
//  if (con.oContext)
    secTab[0].outerContext = con.oContext; //((CSectionDesc*)con.oContext->SectionTabPtr)->vtypeDesc;
  // fill in function and attr tab
  El = (CHETVElem*)classDECL->VElems.VElems.last;
  if (El) 
    TIDCl = El->data.VTClss;
  posSect = 0;
  for (posSect = 0; posSect < secTab[0].nSections; posSect++) {
    //new section?
    for (ii = 0;
         (ii < posSect) && (secTab[ii].implDECL != secTab[posSect].implDECL);
         ii++);
    if (ii == posSect) { //not dummy section
      funcAdapter = 0;
      secTab[posSect].sectionOffset = offset;
      secTab[posSect].SectionFlags.INCL(SectPrimary);
      if (posSect > 0) {
        secTab[posSect].funcDesc = 0;
        secTab[posSect].attrDesc = 0;
        secTab[posSect].vtypeDesc = 0;
      }
      if (secTab[posSect].classDECL->TypeFlags.Contains(isNative)) {
        funcAdapter = GetAdapterTable(ckd, secTab[posSect].classDECL, secTab[0].classDECL);
        //if (!funcAdapter)
        //  return false;
        if (funcAdapter)
          dist =(unsigned)funcAdapter[0];
        else
          dist = 0;
        posAttr += dist; //positions for the native object
        offset += dist;
        nNative = LAH;
      }
      posAttrSect = posAttr;
      posFuncSect = posFunc;
      posParamSect = posParam;
      secTab[0].funcDesc[posFunc].funcExec = 0;
      posFunc++; //the position of default constructor
      for (El = (CHETVElem*)classDECL->VElems.VElems.first;
           El && (secTab[posSect].implDECL != El->data.ClssDECL)
              && (secTab[posSect].implDECL != El->data.ClssDECL->RuntimeDECL);
           El = (CHETVElem*)El->successor); 
      if (El) {
        //section not empty
        TIDCl = El->data.VTClss;
        while (El &&  (TIDCl == El->data.VTClss)) {
          ElDECL = El->data.ElDECL;
          parentDECL = ElDECL->ParentDECL;
          if (parentDECL->RuntimeDECL)
            ElImpl = parentDECL->RuntimeDECL;
          else
            ElImpl = parentDECL;
          if (ElDECL->DeclType == Function) {
            if (!secTab[posSect].funcDesc)
              secTab[posSect].funcDesc = &secTab[0].funcDesc[posFuncSect];
            fd = &secTab[0].funcDesc[posFunc];
            fd->isNative = ElDECL->TypeFlags.Contains(isNative);
            if (fd->isNative) {
              if ( funcAdapter) {
                fd->funcPtr = funcAdapter[nNative];
                nNative++;
              }
              else
                fd->funcPtr = (TAdapterFunc)ElDECL->RuntimeDECL;
              fd->stackFrameSize = ElDECL->nInput + ElDECL->nOutput + 1 + SFH;
              // +1 for self, +2 for pointer synObj and to parent stack frame
            }
            else {
              fd->funcExec = ElDECL->RuntimeDECL;
              //remember: the stackFrameSize must be set after the check of the exec has been done
              //this sets only the right stackFrameSize for functions of a base class
              if (fd->funcExec)
                fd->stackFrameSize = ((SelfVar*)ElDECL->RuntimeDECL->Exec.ptr)->stackFrameSize;
              else
                fd->stackFrameSize = 0;
            }
            if (ElDECL->TypeFlags.Contains(isInitializer) && ElDECL->TypeFlags.Contains(defaultInitializer))
              secTab[0].funcDesc[posFuncSect] = secTab[0].funcDesc[posFunc];
            if (secTab[posSect].implDECL != ElImpl) {
              for (ii = 0;
                   (ii < secTab[0].nSections)
                    && (secTab[ii].implDECL != ElImpl);
                   ii++);
              fd->delta = ii;
//              fd->delta = ii - posSect;
            }
            else
              fd->delta = posSect;
            if (implDECL == ElImpl) { 
              //is the function part of the implementation of this section table
              ElDECL->SectionInfo1 = posFunc-posFuncSect;
              ElDECL->SectionInfo2 = posSect;
              if (!ElDECL->TypeFlags.Contains(isNative) && ElDECL->RuntimeDECL) {
                ElDECL->RuntimeDECL->SectionInfo1 = ElDECL->SectionInfo1;
                ElDECL->RuntimeDECL->SectionInfo2 = ElDECL->SectionInfo2;
              }
            }
            posFunc++;
          }
          else {//Attr
            if (ElDECL->DeclType == Attr) {
              if (!secTab[posSect].attrDesc) 
                secTab[posSect].attrDesc = &secTab[0].attrDesc[posAttrSect];
              ad = &secTab[0].attrDesc[posAttr];
              ad->attrDECL = ElDECL;
              ad->setExec = ElDECL->RelatedDECL;
              if (ElDECL->RuntimeDECL && ElDECL->RelatedDECL)
                ad->getExec = ElDECL->RelatedDECL->RelatedDECL;
              else
                ad->getExec = 0;
              if (secTab[posSect].implDECL != ElImpl) {
                for (ii = 0;
                     (ii < secTab[0].nSections) && (secTab[ii].implDECL != ElImpl);
                     ii++);
                ad->delta = ii;
              }
              else
                ad->delta = posSect;
              if (implDECL == ElImpl) {
                //is this field/property part of the implementation of this section table
                ElDECL->SectionInfo1 = posAttr-posAttrSect;
                ElDECL->SectionInfo2 = posSect;
                if (ad->setExec) {
                  ad->setExec->SectionInfo1 = ElDECL->SectionInfo1;
                  ad->setExec->SectionInfo2 = ElDECL->SectionInfo2;
                }
                if (ad->getExec) {
                  ad->getExec->SectionInfo1 = ElDECL->SectionInfo1;
                  ad->getExec->SectionInfo2 = ElDECL->SectionInfo2;
                }
              }
              posAttr++;
              offset++;
            }
            else { //VirtualType
              if (!secTab[posSect].vtypeDesc) 
                secTab[posSect].vtypeDesc = &secTab[0].vtypeDesc[posParamSect];
              pd = &secTab[0].vtypeDesc[posParam];
              pd->paramDECL = ElDECL;
              if (ElDECL->TypeFlags.Contains(constituent))
                secTab[posSect].SectionFlags.INCL(ElemsConstituent);
              if (ElDECL->TypeFlags.Contains(acquaintance))
                secTab[posSect].SectionFlags.INCL(ElemsAcquaintance);
              if (ElDECL->SecondTFlags.Contains(isChain))
                secTab[posSect].SectionFlags.INCL(SectChainType);
              pd->isparamRef = (ElDECL->RuntimeDECL && (ElDECL->RuntimeDECL->DeclType == VirtualType));
              for (ii = 0;
                   (ii < secTab[0].nSections) && (secTab[ii].implDECL != ElImpl);
                   ii++);
//              pd->delta = ii - posSect;
              pd->delta = ii;
              if (implDECL == ElImpl) {
                //is this param part of the implementation of this section table
                ElDECL->SectionInfo1 = posParam-posParamSect;
                ElDECL->SectionInfo2 = posSect;
              }
              if (!pd->isparamRef) 
                pd->fValue = ElDECL->RuntimeDECL;
              posParam++;
            }
          }
          El = (CHETVElem*)El->successor; 
        }
      }
      offset += LSH; //CSectionDesc pointer + object section flag
    }//not dummy section
  }

  //set the vtypeDesc values
  for (posSect = 0; posSect < secTab[0].nSections; posSect++) {
    if (secTab[posSect].SectionFlags.Contains(SectPrimary))
      for (ii = 0; ii < secTab[posSect].classDECL->SectionInfo3; ii++) {
        pd = &secTab[posSect].vtypeDesc[ii];
        if (pd->isparamRef) {
          ElDECL = pd->paramDECL->RuntimeDECL;
          posParamSect = GetMemsSectionNumber(ckd, con.iContext, ElDECL, false);
          if (ckd.exceptionThrown)
            return false;
          outerParam = (posParamSect < 0);
          while ((ElDECL->DeclType == VirtualType) && !outerParam) {
            ElDECL = ElDECL->RuntimeDECL;
            posParamSect = GetMemsSectionNumber(ckd, con.iContext, ElDECL, false);
            if (ckd.exceptionThrown)
              return false;
            outerParam = (posParamSect < 0);
          }
          if (outerParam) 
            posParamSect = GetMemsSectionNumber(ckd, con.oContext, ElDECL);
          if (posParamSect < 0) {
            LavaError(ckd, true, ElDECL, &ERR_VTOutOfScope);
            return false;
          }
          if (ckd.exceptionThrown)
            return false;
          posParamSect += ElDECL->SectionInfo2;
          posParam = ElDECL->SectionInfo1;
          if (outerParam)
            valueTab = (CSectionDesc*)con.oContext->SectionTabPtr;
          else 
            valueTab = (CSectionDesc*)classDECL->SectionTabPtr;
          pd->fValue = valueTab[posParamSect].vtypeDesc[posParam].fValue;
        }
      }
  }
  //fill the dummy sections
  for (ii = 2; ii < secTab[0].nSections; ii++) {
    posSect = 1;
    while ((posSect < ii) && (secTab[posSect].implDECL != secTab[ii].implDECL))
      posSect++;
    if (posSect < ii) {
      secTab[ii] = secTab[posSect];
      secTab[ii].SectionFlags.EXCL(SectPrimary);
    }
  }
  if (implDECL) {
    implDECL->SectionTabPtr = classDECL->SectionTabPtr;
    implDECL->SectionInfo1 = classDECL->SectionInfo1;
    implDECL->SectionInfo2 = classDECL->SectionInfo2;
    implDECL->SectionInfo3 = classDECL->SectionInfo3;
    implDECL->nSection = classDECL->nSection;
  }
  return true;
}


void CLavaProgram::LavaError(CheckData& ckd, bool setLavaEx, LavaDECL *decl, QString *nresourceID, LavaDECL* refDECL)
{
  QString msg, cWhere;
  int code;
  if (ckd.exceptionThrown && setLavaEx)
    return;
  msg = *nresourceID;
  DString dFileName = GetAbsSynFileName();
  QString cFileName = QString(dFileName.c);

  if (decl)
    cWhere = QString(decl->FullName.c);
  else
    cWhere = QString("??");
  QString cWhat;
  msg = cFileName + " / " + cWhere + ": " + msg;
  if (refDECL && ((nresourceID == &ERR_InvisibleBasictype)
                  || (nresourceID == &ERR_AbstractInherited)
                  || (nresourceID == &ERR_ForceOver)
                  || (nresourceID == &ERR_InvisibleType))) {
    cWhat = QString(refDECL->FullName.c);
    msg = msg + " (" + cWhat + ")";
  }

  if (nresourceID == &ERR_RunTimeException)
    code = check_ex;
  else if (nresourceID == &ERR_DLLError)
    code = DLLError_ex;
  else if (nresourceID == &ERR_AllocObjectFailed)
    code = memory_ex;
  else if (nresourceID == &ERR_ldocNotOpened)
    code = ldocNotOpened_ex;
  else if (nresourceID == &ERR_CompoNotFound)
    code = CompoNotFound_ex;
  else if (nresourceID == &ERR_ldocIncompatible)
    code = ldocIncompatible_ex;
  else if (nresourceID == &ERR_IncompatibleCategory)
    code = IncompatibleCategory_ex;
  else if (nresourceID == &ERR_NoPicture)
    code = NoPicture_ex;
  else
    code = check_ex;

  if (setLavaEx) {
    if (!SetLavaException(ckd, code, msg))
      throw CRuntimeException(code, &msg);
  }
  else
    throw CRuntimeException(code, &msg);
  /*
  critical(wxTheApp->m_appWindow,qApp->name(),tr(msg),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  throwError = true;
  LavaEnd(true);
  */
}


void CLavaProgram::HCatch(CheckData& ckd)
{
  CVFuncDesc *fDesc;
  LavaObjectPtr exception, newStackFrame[SFH+1];
  if (IDTable.catchfuncDecl) {
    exception = ckd.lastException;
		newStackFrame[0] = 0;
		newStackFrame[1] = 0;
		newStackFrame[2] = 0;
    newStackFrame[SFH] = exception;
    ckd.exceptionThrown = false;
    fDesc = &(exception[0][0].funcDesc[IDTable.catchfuncDecl->SectionInfo1]);
    fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ckd, newStackFrame);
    DEC_FWD_CNT(ckd, exception);
  }
}

#ifdef WIN32
void trans_func( unsigned u, _EXCEPTION_POINTERS* pExp )
{
  _clearfp();
  throw CHWException(u);
}

void sigEnable() {
  _set_se_translator(trans_func);/*
//  _control87(0,_EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID); // enable floating-point exceptions

// Get the default control word.
   int cw = _controlfp( 0,0 );

   // Set the exception masks OFF, turn exceptions on.
   cw &=~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL|EM_INEXACT);

   // Set the control word.
   _controlfp( cw, MCW_EM );*/
}

#else
void signalHandler(int sig_number, siginfo_t *info
#ifdef __FreeBSD
, void *
#endif
)
{
	hwException = CHWException(sig_number,info);
  longjmp(contOnHWexception,1);
}

void sigEnable() {
  int rc;
  sigset_t sigs;
  struct sigaction sa, sa_old;

  rc = sigfillset(&sigs);
  rc = pthread_sigmask(SIG_UNBLOCK,&sigs,0);
//  feenableexcept(FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID);
#ifdef __FreeBSD
  sa.sa_sigaction = (void(*)(int, struct __siginfo *, void *))signalHandler;
#else
#ifdef __SunOS
  sa.sa_sigaction = (void(*)(int,  siginfo_t *, void *))signalHandler;
#else
#ifdef __Darwin
  sa.__sigaction_u.__sa_sigaction = (void(*)(int,struct __siginfo*,void*))signalHandler;
#else
  sa.sa_handler = (sighandler_t)signalHandler;
#endif
#endif
#endif
  sa.sa_mask = sigs;
  sa.sa_flags = SA_SIGINFO;
//	rc = sigaction(SIGFPE,&sa,&sa_old);
	rc = sigaction(SIGABRT,&sa,&sa_old);
	rc = sigaction(SIGILL,&sa,&sa_old);
	rc = sigaction(SIGINT,&sa,&sa_old);
	rc = sigaction(SIGSEGV,&sa,&sa_old);
	rc = sigaction(SIGTERM,&sa,&sa_old);
}
#endif


void CLavaExecThread::run() {
  CThreadData *td = new CThreadData(this);
//	threadStg()->setLocalData(td);
  myDoc->ThreadList->append(this);
	ExecuteLava(myDoc);
}

unsigned CLavaExecThread::ExecuteLava(CLavaBaseDoc *doc)
{
  CheckData ckd;
  CSearchData sData;
  LavaVariablePtr newStackFrame=0;
  QString msg="Normal end of application";
  unsigned frameSize, pos, newOldExprLevel;
  bool ok;

#ifdef WIN32
  unsigned frameSizeBytes;

  CoInitialize(0);
#endif
  ((CLavaDebugThread*)LBaseData->debugThread)->myExecThread = this;
  QThread::setTerminationEnabled();
  ckd.document = (CLavaProgram*)doc;
  LavaDECL* topDECL = (LavaDECL*)((CHESimpleSyntax*)ckd.document->mySynDef->SynDefTree.first)->data.TopDef.ptr;
  CHE* che;
  for (che = (CHE*)topDECL->NestedDecls.first;
       che && (((LavaDECL*)che->data)->DeclType == VirtualType);
       che = (CHE*) che->successor);
  if (che && (((LavaDECL*)che->data)->DeclType == Initiator)
          && ((LavaDECL*)che->data)->NestedDecls.last) {
    ((CLavaProgram*)ckd.document)->InitBAdapter();
    topDECL = (LavaDECL*)((CHE*)((LavaDECL*)che->data)->NestedDecls.last)->data;
    if (topDECL && (topDECL->DeclType == ExecDef) && topDECL->Exec.ptr) {
      sigEnable();

      try {
#ifndef WIN32
        if (setjmp(contOnHWexception)) throw hwException;
#endif
        ckd.myDECL = topDECL;
        ok = ((SynObject*)topDECL->Exec.ptr)->Check(ckd);
        if (!ok) {
          critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr("Please open this program in LavaPE and remove all static errors first!"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
          goto stop;
        }
        sData.nextFreeID = 0;
        sData.doc = ckd.document;
        sData.nextFreeID = 0;
        sData.finished = false;
        ((SynObject*)topDECL->Exec.ptr)->MakeTable((address)&doc->IDTable, 0, (SynObjectBase*)ckd.myDECL, onSetSynOID, 0,0, (address)&sData);
        topDECL->WorkFlags.INCL(runTimeOK);

        frameSize = ((SelfVar*)topDECL->Exec.ptr)->stackFrameSize;
        newOldExprLevel = frameSize - 1;
#ifdef WIN32
        frameSizeBytes = frameSize<<2;
        __asm {
          sub esp, frameSizeBytes
          mov newStackFrame, esp
        }
#else
				newStackFrame = new LavaObjectPtr[frameSize];
#endif
        for (pos=0;pos<frameSize;pos++)
          newStackFrame[pos] = 0;
        if (LBaseData->debugOn) {
          doc->debugOn = true;
          ((CLavaDebugThread*)LBaseData->debugThread)->pContDebugEvent->release(); 
          //debug thread continue, now initialisation is finished 
          ((CLavaDebugThread*)LBaseData->debugThread)->pContExecEvent->acquire(); 
          //execution thread wait until debug thread has received first message from LavaPE
        }
        if (!((SelfVarX*)topDECL->Exec.ptr)->Execute(ckd,newStackFrame,newOldExprLevel)) {
          if (!ckd.exceptionThrown)
            ((SelfVarX*)topDECL->Exec.ptr)->SetRTError(ckd, &ERR_ExecutionFailed,newStackFrame);

          if (newStackFrame) {
#ifdef WIN32
            __asm {
              add esp, frameSizeBytes
            }
#else
						delete [] newStackFrame;
#endif
          }
          ((CLavaProgram*)ckd.document)->HCatch(ckd);
stop:     ckd.document->throwError = false;
          CLavaPEHint* hint = new CLavaPEHint(CPECommand_LavaEnd, ckd.document, (const unsigned long)3,(const unsigned long)QThread::currentThread());
          QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaEnd,(void*)hint));
//          ckd.document->LavaEnd(true);
          return 0;
        }
      }
      catch (CHWException ex) {
        critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr(ex.message),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        ckd.document->throwError = false;
        LavaEnd(ckd.document, true);
        return 0;
      }
      catch (CRuntimeException ex) {
        critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr(ex.message),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        ckd.document->throwError = false;
        LavaEnd(ckd.document, true);
        return 0;
      }
      catch(CUserException) {
        if (ckd.lastException)
          ((CLavaProgram*)ckd.document)->HCatch(ckd);
        ckd.document->throwError = false;
        LavaEnd(ckd.document, true);
        return 0;
      }
      catch(CException) {
        // For other exception types, notify user here.
        critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr("Unknown exception during check or execution of Lava program"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        if (ckd.document->throwError) {
          CLavaPEHint *hint =  new CLavaPEHint(CPECommand_LavaEnd, ckd.document, (const unsigned long)3,(const unsigned long)QThread::currentThread());
					QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaEnd,(void*)hint));
        }
        return 0;
      }
      catch(int) {
        // For stack overflow, notify user here.
        critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr("Stack overflow!"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        if (ckd.document->throwError) {
          CLavaPEHint *hint =  new CLavaPEHint(CPECommand_LavaEnd, ckd.document, (const unsigned long)3,(const unsigned long)QThread::currentThread());
					QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaEnd,(void*)hint));
        }
        return 0;
      }
      if (newStackFrame) {
#ifdef WIN32
        __asm {
          add esp, frameSizeBytes
        }
#else
        delete [] newStackFrame;
#endif
      }
      information(wxTheApp->m_appWindow,qApp->name(),QApplication::tr(msg),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      CLavaPEHint *hint =  new CLavaPEHint(CPECommand_LavaEnd, ckd.document, (const unsigned long)3,(const unsigned long)QThread::currentThread());
      QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaEnd,(void*)hint));
      return 1;
    }
    else {
      critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr("No initiator in the first declaration position"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      ckd.document->throwError = false; //07.05.2002
      LavaEnd(ckd.document, true);  //07.05.2002
      return 0;
    }
  }
  else {
    critical(wxTheApp->m_appWindow,qApp->name(),QApplication::tr("No initiator in the first declaration position"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    ckd.document->throwError = false;  //07.05.2002
    LavaEnd(ckd.document, true);  //07.05.2002
    return 0;
  }
}

CRuntimeException* showFunc(CheckData& ckd, LavaVariablePtr stack, bool frozen, bool fromFillIn)
{
  CRuntimeException* ex=0;
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();
  CLavaPEHint* hint =  new CLavaPEHint(CPECommand_OpenFormView, ckd.document, (const unsigned long)3, (DWORD)&stack[SFH], (DWORD)&stack[SFH+1], (DWORD)&stack[SFH+2], (DWORD)frozen, (DWORD)currentThread, (DWORD)fromFillIn);
  if (currentThread != wxTheApp->mainThread) {
    currentThread->pContExecEvent->lastException = 0;
	  QApplication::postEvent(LBaseData->theApp, new CustomEvent(UEV_LavaShow,(void*)hint));
    currentThread->pContExecEvent->acquire();
    if (currentThread->pContExecEvent->lastException) {
      if (ckd.lastException)
        DEC_FWD_CNT(ckd, ckd.lastException);
      ckd.lastException = currentThread->pContExecEvent->lastException;
      currentThread->pContExecEvent->lastException = 0;
      ckd.exceptionThrown = true;
    }
    else 
      if (currentThread->pContExecEvent->ex) {
        ex = currentThread->pContExecEvent->ex;
        currentThread->pContExecEvent->ex = 0;
      }
  }
  else {
    ((CLavaBaseDoc*)hint->fromDoc)->LavaDialog = new LavaGUIDialog(wxTheApp->m_appWindow, hint); 
    if (((QDialog*)((CLavaBaseDoc*)hint->fromDoc)->LavaDialog)->exec() != QDialog::Accepted)
      ex = new CRuntimeException(RunTimeException_ex, &ERR_CanceledForm);
  }

  if (ckd.document->throwError) {
    ckd.document->throwError = false;
		throw CUserException();
  }
  return ex;
}

bool ShowFuncEdit(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr newStackFrame[SFH+3];
  newStackFrame[0] = stack[0];
  newStackFrame[1] = stack[1];
  newStackFrame[2] = (LavaObjectPtr)((unsigned)stack[2] & ~2);
  newStackFrame[SFH] = stack[SFH];
  newStackFrame[SFH+1] = stack[SFH+1];
  newStackFrame[SFH+2] = 0;
  CRuntimeException *ex = CopyObject(ckd, &newStackFrame[SFH+1], &newStackFrame[SFH+2], ((SynFlags*)(newStackFrame[SFH+1]+1))->Contains(stateObjFlag), newStackFrame[SFH][0][0].classDECL->RelatedDECL);
  if (ckd.exceptionThrown)
    return false;
  if (ex)
    throw *ex;
  ex = showFunc(ckd, newStackFrame, !((SynFlags*)(newStackFrame[SFH+2]+1))->Contains(stateObjFlag), false);
  if (ckd.exceptionThrown) {
    if (newStackFrame[SFH+2])
      DFC(newStackFrame[SFH+2]);
    return false;
  }
  else {
    if (ex)
      throw *ex;
    UpdateObject(ckd, newStackFrame[SFH+1], &newStackFrame[SFH+2]);
    if (newStackFrame[SFH+2])
      DFC(newStackFrame[SFH+2]);
    return true;
  }
}

bool ShowFuncFillIn(CheckData& ckd, LavaVariablePtr stack)
{
  CRuntimeException *ex;

  if (stack[SFH+2])
    DFC( stack[SFH+2]);
  stack[SFH+2] = AllocateObject(ckd, stack[SFH][0][0].classDECL->RelatedDECL, false);
  if (!stack[SFH+2]) {
    if (ckd.exceptionThrown)
      return false;
    else
      throw CRuntimeException(memory_ex, &ERR_AllocObjectFailed);
  }
  if (stack[SFH+1]) {
    ex = CopyObject(ckd, &stack[SFH+1], &stack[SFH+2], false, stack[SFH][0][0].classDECL->RelatedDECL);
    if (ckd.exceptionThrown)
      return false;
    if (ex)
      throw *ex;
  }
  ex = showFunc(ckd, stack, false, true);
  if (ckd.exceptionThrown)
    return false;
  else {
    if (ex)
      throw *ex;
    ((SynFlags*)(stack[SFH+2]+1))->INCL(finished);
    return true;
  }
}
