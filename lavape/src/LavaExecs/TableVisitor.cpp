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
#include "PEBaseDoc.h"

#pragma hdrstop

#ifndef INTERPRETER
#include "ExecView.h"
#endif
#include "Constructs.h"


void TableVisitor::Eval (SynObject *obj,SynObject *parent,address where,CHAINX *chxp) {
#ifdef INTERPRETER
  obj->parentObject = parent;
  obj->whereInParent = where;
  obj->containingChain = chxp;
  if (update == onSetSynOID) obj->synObjectID = ++((CSearchData*)searchData)->nextFreeID;
  if (update == onGetAddress
  && obj->synObjectID == ((CSearchData*)searchData)->synObjectID) {
    ((CSearchData*)searchData)->synObj = obj;
    //((CSearchData*)searchData)->finished = true;
    finished = true;
    return;
  } 
#else
  unsigned oldSynObjectID;
  if (update == onSetSynOID) {
    oldSynObjectID = obj->synObjectID;
    obj->synObjectID = ++((CSearchData*)searchData)->nextFreeID;
    if (obj->workFlags.Contains(isBrkPnt))
      if (!LBaseData->debugger->checkExecBrkPnts(oldSynObjectID,obj->synObjectID,
         ((CSearchData*)searchData)->execDECL->ParentDECL->OwnID,
         ((CSearchData*)searchData)->execDECL->DeclType,
         (CLavaBaseDoc*)((CSearchData*)searchData)->doc))
        obj->workFlags.EXCL(isBrkPnt); }
  else if (update == onSelect) { 
    if (obj->synObjectID == ((CSearchData*)searchData)->synObjectID) { 
      if (((CSearchData*)searchData)->debugStop) { 
        ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->stopReason = ((CSearchData*)searchData)->stopReason;
        if (((CSearchData*)searchData)->innermostStop)
          if (obj->primaryToken == assignFS_T)
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->debugStopToken = obj->startToken;
          else
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->debugStopToken = obj->primaryTokenNode;
        else
          if (obj->primaryToken == assignFS_T)
            if (obj->startToken)
              ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = obj->startToken; 
            else /*new with hidden dft ini call */
              ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = obj->parentObject->primaryTokenNode;
          else
            ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->callerStopToken = obj->primaryTokenNode;
        ((CExecView*)((CSearchData*)searchData)->execView)->autoScroll = true;
        ((CExecView*)((CSearchData*)searchData)->execView)->redCtl->update();
      }
      else {
        //((CExecView*)((CSearchData*)searchData)->execView)->forcePrimTokenSelect = true;
        //((CExecView*)((CSearchData*)searchData)->execView)->text->selectAt = obj;
        //((CExecView*)((CSearchData*)searchData)->execView)->text->currentSynObj = obj;
        //((CExecView*)((CSearchData*)searchData)->execView)->text->currentSelection = obj->primaryTokenNode;
        ((CExecView*)((CSearchData*)searchData)->execView)->SetSelectAt(obj);
      }
      //((CSearchData*)searchData)->finished = true;
      finished = true;
      return;
    }
  }
  else if (update == onGetAddress) {
    obj->synObjectID = ++((CSearchData*)searchData)->nextFreeID;
    if (obj->synObjectID == ((CSearchData*)searchData)->synObjectID) {
      ((CSearchData*)searchData)->synObj = obj;
      //((CSearchData*)searchData)->finished = true;
      finished = true;
      return; }
  }
  obj->parentObject = parent;
  obj->whereInParent = where;
  obj->containingChain = chxp;
#endif
}

void TableVisitor::VisitEnumConst (EnumConst *obj,SynObject *parent,address where,CHAINX *chxp) {
  
#ifndef INTERPRETER
  switch (update) {
  case onSearch:
    if (obj->refID == ((CSearchData*)searchData)->findRefs.refTid
    && obj->Id == ((CSearchData*)searchData)->findRefs.enumID) {
      ((CSearchData*)searchData)->synObjectID = obj->synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(obj->LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
  case onCopy:
    ((TIDTable*)table)->ChangeFromTab(obj->refID);
    break;
  case onMove:
    ((TIDTable*)table)->ChangeRefToClipID(obj->refID);
    break;
  }
#endif
}

void TableVisitor::VisitParameter (Parameter *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->formParmID);
}

void TableVisitor::VisitFormParm (FormParm *obj,SynObject *parent,address where,CHAINX *chxp) {
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->formParmID);
}

void TableVisitor::VisitReference (Reference *obj,SynObject *parent,address where,CHAINX *chxp) {
  switch (update) {
  case onCopy:
    if (obj->parentObject->parentObject)
      ((TIDTable*)table)->ChangeFromTab(obj->refID);
    break;
  case onMove:
    ((TIDTable*)table)->ChangeRefToClipID(obj->refID);
    break;
  case onDeleteID:
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(obj);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (obj->refID == ((CSearchData*)searchData)->findRefs.refTid
    && ((CSearchData*)searchData)->findRefs.enumID.l == 0) {
      ((CSearchData*)searchData)->synObjectID = obj->synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(obj->LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitTDOD (TDOD *obj,SynObject *parent,address where,CHAINX *chxp) {
  switch (update) {
  case onDeleteID:
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(obj);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (obj->ID == ((CSearchData*)searchData)->findRefs.refTid
    && obj->accessTypeOK(((CSearchData*)searchData)->findRefs.FindRefFlags)) {
      ((CSearchData*)searchData)->synObjectID = obj->synObjectID;
      ((CSearchData*)searchData)->constructNesting = qPrintable(obj->LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitObjReference (ObjReference *obj,SynObject *parent,address where,CHAINX *chxp) {
  CHE *chp;

  switch (update) {
  case onAddID:
  case onNewID:
    for ( chp = (CHE*)obj->refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TDOD*)chp->data)->parentObject = obj;
    }
    break;
  case onCopy:
    for ( chp = (CHE*)obj->refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TIDTable*)table)->ChangeFromTab(((TDOD*)chp->data)->ID);
    }
    break;
  case onMove:
    for ( chp = (CHE*)obj->refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TIDTable*)table)->ChangeRefToClipID(((TDOD*)chp->data)->ID);
      ((TDOD*)chp->data)->parentObject = obj;
    }
    break;
  default:
    for ( chp = (CHE*)obj->refIDs.first;
          chp;
          chp = (CHE*)chp->successor) {
      ((TDOD*)chp->data)->parentObject = obj;
    }
  }
}

void TableVisitor::VisitMultipleOp (MultipleOp *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->opFunctionID);
}

void TableVisitor::VisitSelfVar (SelfVar *obj,SynObject *parent,address where,CHAINX *chxp)
{
  LavaDECL *decl, *declName;
  CSearchData sData;

  if (searchData)
    sData = *(CSearchData*)searchData;

  obj->execDECL = (LavaDECL*)parent;

  if (!obj->checked
  && ((update == onSearch && inINCL)
      || update == onGetAddress
      || update == onSetSynOID)) {
    if (update == onSearch) {
      TableVisitor vis(table,inINCL,onAddID,(address)&sData);
      obj->Accept(vis,parent,where,chxp);
    }
    CheckData ckd;
    ckd.myDECL = obj->execDECL; 
    ckd.inINCL = obj->execDECL->inINCL;
    ckd.document = (CLavaBaseDoc*)((CSearchData*)searchData)->doc;
    //UpdateParameters(ckd);
    //obj->Check(ckd); // only to insert identifiers of references and formal parameters
  }

  switch (update) {
  case onTypeID:
    decl = obj->execDECL->ParentDECL;
    if (decl->DeclType == Function) {
      decl = decl->ParentDECL;
      if (decl)
        switch (decl->DeclType) {
        case Impl:
        case Interface:
          obj->typeID = TID(decl->OwnID,0);
          break;
        default: ;
        }
    }
    return;
  default: ;
  }
  if (obj->primaryToken != initiator_T) // function/initializer or invariant
    if (update == onCopy || update == onMove) {
      if (update == onMove) {
        TableVisitor vis(table,inINCL,update,searchData);
        vis.VisitVarName(obj);
      }
      if (obj->execDECL->ParentDECL->DeclType == Interface) //invariant
        obj->typeID = TID(obj->execDECL->ParentDECL->OwnID,0);
      else // function/initializer
        if (obj->execDECL->ParentDECL->ParentDECL)
          obj->typeID = TID(obj->execDECL->ParentDECL->ParentDECL->OwnID,0);
    }
    else {
      TableVisitor vis(table,inINCL,update,searchData);
      vis.VisitVarName(obj);
    }
  obj->whereInParent = 0;
  obj->containingChain = 0;
  obj->parentObject = 0;
  obj->concernExecs = false;

  ((SynObject*)obj->execName.ptr)->parentObject = obj;
  if (update == onNewID) {
    decl = obj->execDECL->ParentDECL;
    declName = decl;
    ((Reference*)obj->execName.ptr)->refID = TID(declName->OwnID,0);
    if (decl->DeclType == Function)
      decl = decl->ParentDECL;
    if (decl)
      switch (decl->DeclType) {
      case Impl:
      case Interface:
        obj->typeID = TID(decl->OwnID,0);
        break;
      default: ;
      }
  }
}

void TableVisitor::VisitUnaryOp (UnaryOp *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->opFunctionID);
}

void TableVisitor::VisitBinaryOp (BinaryOp *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->opFunctionID);
}

void TableVisitor::VisitSynObject (SynObject *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (obj->primaryToken != VarPH_T || obj->parentObject->primaryToken != quant_T)
    return;

  Quantifier *quant=(Quantifier*)obj->parentObject;
  Declare *dclStm=(Declare*)quant->parentObject;

  if (update == onAddID) {
    currIniCall = 0;
    currIniCallChp = 0;
    if (dclStm->IsDeclare()
    && dclStm->secondaryClause.ptr
    && obj->whereInParent == quant->quantVars.first
    && quant->whereInParent == dclStm->quantifiers.first) {
      //currIniOrder = 1;
      if (((SynObject*)dclStm->secondaryClause.ptr)->IsFuncInvocation()) {
        currIniCall = (FuncStatement*)dclStm->secondaryClause.ptr;
        currIniCallChp = 0;
      }
      else {
        currIniCallChp = (CHE*)((SemicolonOp*)dclStm->secondaryClause.ptr)->operands.first;
        currIniCall = (FuncStatement*)currIniCallChp->data;
      }
    }

    obj->iniCall = currIniCall;
    if (currIniCall)
      currIniCall->varName = obj;
    if (currIniCallChp) {
      currIniCallChp = (CHE*)currIniCallChp->successor;
      if (currIniCallChp)
        currIniCall = (FuncStatement*)currIniCallChp->data;
    }
  }
}

void TableVisitor::VisitVarName (VarName *obj,SynObject *parent,address where,CHAINX *chxp)
{
  Quantifier *quant=(Quantifier*)obj->parentObject;
  Declare *dclStm=0;

  if (update == onAddID) {
    if (quant && quant->primaryToken == quant_T)
      dclStm = (Declare*)quant->parentObject;
    if (dclStm && dclStm->IsDeclare()
    && dclStm->secondaryClause.ptr) {
      if (obj->whereInParent == quant->quantVars.first
      && quant->whereInParent == dclStm->quantifiers.first) {
        //currIniOrder = 1;
        if (((SynObject*)dclStm->secondaryClause.ptr)->IsFuncInvocation()) {
          currIniCall = (FuncStatement*)dclStm->secondaryClause.ptr;
          currIniCallChp = 0;
        }
        else {
          currIniCallChp = (CHE*)((SemicolonOp*)dclStm->secondaryClause.ptr)->operands.first;
          currIniCall = (FuncStatement*)currIniCallChp->data;
        }
      }
      obj->iniCall = currIniCall;
      currIniCall->varName = obj;
      //obj->iniOrder = currIniOrder++;
      if (currIniCallChp) {
        currIniCallChp = (CHE*)currIniCallChp->successor;
        if (currIniCallChp)
          currIniCall = (FuncStatement*)currIniCallChp->data;
      }
    }
  }

  switch (update) {
  case onUndoDeleteID:
    if (obj->varID.nID != -1)
      ((TIDTable*)table)->UndoDeleteLocalID(obj->varID.nID);
    break;
  case onDeleteID:
    if (obj->varID.nID != -1)
      ((TIDTable*)table)->DeleteID(obj->varID.nID);
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(obj);
    break;
  case onAddID:
    ((TIDTable*)table)->AddLocalID((DWORD)obj,obj->varID.nID,inINCL);
    break;
  case onNewID:
    if (obj->varID.nINCL == -1) // set in ExecView or below to avoid duplicate assignment of an ID
      obj->varID.nINCL = 0;
    else
     ((TIDTable*)table)->NewLocalID((DWORD)obj,obj->varID.nID);
    break;
  case onConstrCopy:
     ((TIDTable*)table)->NewLocalID((DWORD)obj,obj->varID.nID,true);
      obj->varID.nINCL = -1;  // cf. case onNewID
    break;
  case onChange:
  case onUndoChange:
  //case onMove:
    ((TIDTable*)table)->ChangeLocalID((DWORD)obj,obj->varID.nID);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (!((CSearchData*)searchData)->findRefs.index) {
      if (obj->varID == ((CSearchData*)searchData)->findRefs.refTid) {
        ((CSearchData*)searchData)->synObjectID = obj->synObjectID;
        ((CSearchData*)searchData)->constructNesting = qPrintable(obj->LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
      }
    }
    else { //  search symbol by name
      if (!obj->IsSelfVar()
      && obj->varName.StringMatch(((CSearchData*)searchData)->findRefs.searchName,
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(matchCase),
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(wholeWord))) {
        ((CSearchData*)searchData)->synObjectID = obj->synObjectID;
        ((CSearchData*)searchData)->constructNesting = qPrintable(obj->LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecFindText(*(CSearchData*)searchData);
      }
    }
    break;
#endif
  default: ;
  }
}

void TableVisitor::VisitArrayAtIndex (ArrayAtIndex *obj,SynObject *parent,address where,CHAINX *chxp)
{
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(obj->opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(obj->opFunctionID);
}
