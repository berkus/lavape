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


#include "Constructs.h"
#ifndef INTERPRETER
#include "ExecView.h"
#endif
#include "LavaAppBase.h"
#include "PEBaseDoc.h"

#pragma hdrstop


//static int inINCL;


void TableVisitor::Eval (SynObject *synObj) {
#ifdef INTERPRETER
  synObj->parentObject = parent;
  synObj->whereInParent = where;
  synObj->containingChain = chxp;
  if (update == onSetSynOID) synObjectID = ++((CSearchData*)searchData)->nextFreeID;
  if (update == onGetAddress
  && synObjectID == ((CSearchData*)searchData)->synObjectID) {
    ((CSearchData*)searchData)->synObj = synObj;
    ((CSearchData*)searchData)->finished = true;
    return;
  } 
#else
  unsigned oldSynObjectID;
  if (update == onSetSynOID) {
    oldSynObjectID = synObj->synObjectID;
    synObj->synObjectID = ++((CSearchData*)searchData)->nextFreeID;
    if (synObj->workFlags.Contains(isBrkPnt))
      if (!LBaseData->debugger->checkExecBrkPnts(oldSynObjectID,synObj->synObjectID,
         ((CSearchData*)searchData)->execDECL->ParentDECL->OwnID,
         ((CSearchData*)searchData)->execDECL->DeclType,
         (CLavaBaseDoc*)((CSearchData*)searchData)->doc))
        synObj->workFlags.EXCL(isBrkPnt); }
  else if (update == onSelect) { 
    if (synObj->synObjectID == ((CSearchData*)searchData)->synObjectID) { 
      if (((CSearchData*)searchData)->debugStop) { 
        ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->stopReason = ((CSearchData*)searchData)->stopReason;
        if (((CSearchData*)searchData)->innermostStop)
          if (synObj->primaryToken == assignFS_T)
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->debugStopToken = synObj->startToken;
          else
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->debugStopToken = synObj->primaryTokenNode;
        else
          if (synObj->primaryToken == assignFS_T)
            if (synObj->startToken)
              ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = synObj->startToken; 
            else /*new with hidden dft ini call */
              ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = synObj->parentObject->primaryTokenNode;
          else
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = synObj->primaryTokenNode;
        ((CExecView*)((CSearchData*)searchData)->execView)->autoScroll = true;
        ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->update();
      }
      else
        ((CExecView*)((CSearchData*)searchData)->execView)->Select(synObj);
      ((CSearchData*)searchData)->finished = true;
      finished = true;
      return;
    }
  }
  else if (update == onGetAddress) {
    synObj->synObjectID = ++((CSearchData*)searchData)->nextFreeID;
    if (synObj->synObjectID == ((CSearchData*)searchData)->synObjectID) {
      ((CSearchData*)searchData)->synObj = synObj;
      ((CSearchData*)searchData)->finished = true;
      finished = true;
      return; }
  }
  synObj->parentObject = parent;
  synObj->whereInParent = where;
  synObj->containingChain = chxp;
#endif
}

void TableVisitor::VisitEnumConst (EnumConst *obj) {
  
#ifndef INTERPRETER
  switch (update) {
  case onSearch:
    if (refID == ((CSearchData*)searchData)->findRefs.refTid
    && Id == ((CSearchData*)searchData)->findRefs.enumID) {
      ((CSearchData*)searchData)->synObjectID = synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
  case onCopy:
    ((TIDTable*)table)->ChangeFromTab(refID);
    break;
  case onMove:
    ((TIDTable*)table)->ChangeRefToClipID(refID);
    break;
  }
#endif
}

void TableVisitor::VisitParameter (Parameter *obj)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(formParmID);
}

void TableVisitor::VisitFormParm (FormParm *obj) {
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(formParmID);
}

void TableVisitor::VisitReference (Reference *obj) {
  switch (update) {
  case onCopy:
    ((TIDTable*)table)->ChangeFromTab(refID);
    break;
  case onMove:
    ((TIDTable*)table)->ChangeRefToClipID(refID);
    break;
  case onDeleteID:
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(this);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (refID == ((CSearchData*)searchData)->findRefs.refTid
    && ((CSearchData*)searchData)->findRefs.enumID.l == 0) {
      ((CSearchData*)searchData)->synObjectID = synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitTDOD (TDOD *obj) {
  switch (update) {
  case onDeleteID:
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(this);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (ID == ((CSearchData*)searchData)->findRefs.refTid
    && accessTypeOK(((CSearchData*)searchData)->findRefs.FindRefFlags)) {
      ((CSearchData*)searchData)->synObjectID = synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitObjReference (ObjReference *obj) {
  CHE *chp;

  switch (update) {
  case onAddID:
  case onNewID:
    for ( chp = (CHE*)refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TDOD*)chp->data)->parentObject = this;
      MTBLCHE (chp,&refIDs);
    }
    break;
  case onCopy:
    for ( chp = (CHE*)refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TIDTable*)table)->ChangeFromTab(((TDOD*)chp->data)->ID);
      MTBLCHE (chp,&refIDs);
    }
    break;
  case onMove:
    for ( chp = (CHE*)refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TIDTable*)table)->ChangeRefToClipID(((TDOD*)chp->data)->ID);
      ((TDOD*)chp->data)->parentObject = this;
      MTBLCHE (chp,&refIDs);
    }
    break;
  default:
    for ( chp = (CHE*)refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TDOD*)chp->data)->parentObject = this;
      MTBLCHE (chp,&refIDs);
    }
  }
}


void TableVisitor::VisitMultipleOp (MultipleOp *obj)
{
  CHE *chp;

  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);
}

void TableVisitor::VisitSelfVar (SelfVar *obj)
{
  CHE *chp;
  LavaDECL *decl, *declName;
  CSearchData sData;
  if (searchData)
    sData = *(CSearchData*)searchData;

  execDECL = (LavaDECL*)parent;

  if (!checked
  && ((update == onSearch && inINCL)
      || update == onGetAddress
      || update == onSetSynOID)) {
    if (update == onSearch)
      MakeTable(table,inINCL,parent,onAddID,(address)this,0,(address)&sData);
    CheckData ckd;
    ckd.myDECL = execDECL; 
    ckd.inINCL = inINCL;
    ckd.document = (CLavaBaseDoc*)((CSearchData*)searchData)->doc;
    Check(ckd); // only to insert identifiers of references and formal parameters
  }

  if (update != onCopy) // since execName is recalculated below on onNewID
    MTBL (execName.ptr);
  switch (update) {
  case onTypeID:
    decl = execDECL->ParentDECL;
    if (decl->DeclType == Function) {
      decl = decl->ParentDECL;
      if (decl)
        switch (decl->DeclType) {
        case Impl:
        case Interface:
          typeID = TID(decl->OwnID,0);
          break;
        default: ;
        }
    }
    return;
  default: ;
  }
  if (primaryToken != initiator_T) // function/initializer or invariant
    if (update == onCopy
      || update == onMove) {
      if (update == onMove)
        TableVisitor::VisitVarName (table,inINCL,parent,update,(address)&execDECL->Exec.ptr,0,searchData);
      if (execDECL->ParentDECL->DeclType == Interface) //invariant
        typeID = TID(execDECL->ParentDECL->OwnID,0);
      else // function/initializer
        if (execDECL->ParentDECL->ParentDECL)
          typeID = TID(execDECL->ParentDECL->ParentDECL->OwnID,0);
    }
    else
      TableVisitor::VisitVarName (table,inINCL,parent,update,(address)&execDECL->Exec.ptr,0,searchData);

  whereInParent = 0;
  containingChain = 0;
  parentObject = 0;
  concernExecs = false;


  ((SynObject*)execName.ptr)->parentObject = this;
  if (update == onNewID) {
    decl = execDECL->ParentDECL;
    declName = decl;
    ((Reference*)execName.ptr)->refID = TID(declName->OwnID,0);
    if (decl->DeclType == Function)
      decl = decl->ParentDECL;
    if (decl)
      switch (decl->DeclType) {
      case Impl:
      case Interface:
        typeID = TID(decl->OwnID,0);
        break;
      default: ;
      }
  }
}

void TableVisitor::VisitUnaryOp (UnaryOp *obj)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);

  if ((SynObject*)operand.ptr) // in MinusOp ptr==0 possible
    MTBL (operand.ptr);
}

void TableVisitor::VisitBinaryOp (BinaryOp *obj)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);
}

void TableVisitor::VisitVarName (VarName *obj)
{
  switch (update) {
  case onUndoDeleteID:
    if (varID.nID != -1)
      ((TIDTable*)table)->UndoDeleteLocalID(varID.nID);
    break;
  case onDeleteID:
    if (varID.nID != -1)
      ((TIDTable*)table)->DeleteID(varID.nID);
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(this);
    break;
  case onAddID:
    ((TIDTable*)table)->AddLocalID((DWORD)this,varID.nID,inINCL);
    break;
  case onNewID:
    if (varID.nINCL == -1) // set in ExecView or below to avoid duplicate assignment of an ID
      varID.nINCL = 0;
    else
     ((TIDTable*)table)->NewLocalID((DWORD)this,varID.nID);
    break;
  case onConstrCopy:
     ((TIDTable*)table)->NewLocalID((DWORD)this,varID.nID,true);
      varID.nINCL = -1;  // cf. case onNewID
    break;
  case onChange:
  case onUndoChange:
  //case onMove:
    ((TIDTable*)table)->ChangeLocalID((DWORD)this,varID.nID);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (!((CSearchData*)searchData)->findRefs.index) {
      if (varID == ((CSearchData*)searchData)->findRefs.refTid) {
        ((CSearchData*)searchData)->synObjectID = synObjectID;
        ((CSearchData*)searchData)->constructNesting = qPrintable(LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
      }
    }
    else { //  search symbol by name
      if (!IsSelfVar()
      && varName.StringMatch(((CSearchData*)searchData)->findRefs.searchName,
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(matchCase),
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(wholeWord))) {
        ((CSearchData*)searchData)->synObjectID = synObjectID;
        ((CSearchData*)searchData)->constructNesting = qPrintable(LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
      }
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitArrayAtIndex (ArrayAtIndex *obj)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);
}
