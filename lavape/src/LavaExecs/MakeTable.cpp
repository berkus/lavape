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


#include "Constructs.h"
#ifndef INTERPRETER
#include "ExecView.h"
#endif
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
//#include "stdafx.h"


#ifdef INTERPRETER
#define ENTRY \
  parentObject = (SynObject*)parent; \
  whereInParent = where; \
  containingChain = chxp; \
  if (searchData) synObjectID = ++((CSearchData*)searchData)->nextFreeID;
#else
#define ENTRY \
  if (searchData) synObjectID = ++((CSearchData*)searchData)->nextFreeID; \
  if (update == onSelect) { \
    if (synObjectID == ((CSearchData*)searchData)->synObjectID) { \
      ((CExecView*)((CSearchData*)searchData)->execView)->Select(this); \
      return; \
    } \
  } \
  parentObject = (SynObject*)parent; \
  whereInParent = where; \
  containingChain = chxp;
#endif
//      ((CExecView*)((CSearchData*)searchData)->execView)->Select(this); 
//  if (update == onSearch) synObjectID = ++((CSearchData*)searchData)->nextFreeID; 

#define MTBL(WHERE) ((SynObject*)WHERE)->MakeTable(table,inINCL,(SynObjectBase*)this,update,(address)&WHERE,0,searchData)

#define MTBLCHE(CHEP,CHXP) ((SynObject*)CHEP->data)->MakeTable(table,inINCL,(SynObjectBase*)this,update,(address)CHEP,CHXP,searchData)


static int inINCL;


void SynObject::MakeTable (address,int,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  ENTRY
}

void EnumConst::MakeTable (address table,int,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  ENTRY

#ifndef INTERPRETER
  switch (update) {
  case onSearch:
    if (refID == ((CSearchData*)searchData)->findRefs.refTid
    && Id == ((CSearchData*)searchData)->findRefs.enumID) {
      ((CSearchData*)searchData)->synObjectID = synObjectID;
      ((CSearchData*)searchData)->constructNesting = DString(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecBarText(*(CSearchData*)searchData);
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

void Parameter::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(formParmID);
  MTBL (parameter.ptr);
}

void FormParms::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY

  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&inputs);
  }
  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&outputs);
  }
}

void FormParm::MakeTable (address table,int,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(formParmID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(formParmID);

  MTBL (formParm.ptr);
}

void Reference::MakeTable (address table,int,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  ENTRY
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
      ((CSearchData*)searchData)->constructNesting = DString(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecBarText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void TDOD::MakeTable (address,int,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  ENTRY
  switch (update) {
  case onDeleteID:
    LBaseData->Browser->LastBrowseContext->RemoveSynObj(this);
    break;
#ifndef INTERPRETER
  case onSearch:
    if (ID == ((CSearchData*)searchData)->findRefs.refTid
    && accessTypeOK(((CSearchData*)searchData)->findRefs.FindRefFlags)) {
      ((CSearchData*)searchData)->synObjectID = synObjectID;
      ((CSearchData*)searchData)->constructNesting = DString(LocationOfConstruct());
      ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecBarText(*(CSearchData*)searchData);
    }
    break;
#endif
  default: ;
  }
}

void ObjReference::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData) {
  CHE *chp;

  ENTRY
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


void MultipleOp::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);

  for (chp = (CHE*)operands.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&operands);
  }
}

void BaseInit::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (baseItf.ptr);
  if (initializerCall.ptr)
    MTBL (initializerCall.ptr);
}

void SelfVar::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address,CHAINX*,address searchData)
{
  CHE *chp;
  LavaDECL *decl, *declName;
  CSearchData sData;
  if (searchData)
    sData = *(CSearchData*)searchData;

  if (update == onSelect)
    ((CSearchData*)searchData)->execView = myView;
  else if (update == onSearch && inINCL && !checked) {
    MakeTable(table,inINCL,parent,onAddID,(address)this,0,(address)&sData);
    CheckData ckd;
    ckd.myDECL = (LavaDECL*)parent; 
    ckd.inINCL = inINCL;
    ckd.document = (CLavaBaseDoc*)((CSearchData*)searchData)->doc;
    Check(ckd); // only to insert identifiers of references
    checked = true;
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
        VarName::MakeTable (table,inINCL,parent,update,(address)&execDECL->Exec.ptr,0,searchData);
      if (execDECL->ParentDECL->DeclType == Interface) //invariant
        typeID = TID(execDECL->ParentDECL->OwnID,0);
      else // function/initializer
        if (execDECL->ParentDECL->ParentDECL)
          typeID = TID(execDECL->ParentDECL->ParentDECL->OwnID,0);
    }
    else
      VarName::MakeTable (table,inINCL,parent,update,(address)&execDECL->Exec.ptr,0,searchData);

  whereInParent = 0;
  containingChain = 0;
  parentObject = 0;
  concernExecs = false;
  execDECL = (LavaDECL*)parent;


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

  if (formParms.ptr)
    MTBL (formParms.ptr);

  for (chp = (CHE*)baseInitCalls.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&baseInitCalls);
  }

  MTBL (body.ptr);
}

void HandleOp::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (operand.ptr);
}

void FailStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if ((SynObject*)exception.ptr)
    MTBL (exception.ptr);
}

void UnaryOp::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);

  if ((SynObject*)operand.ptr) // in MinusOp ptr==0 possible
    MTBL (operand.ptr);
}

void LogicalNot::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (operand.ptr);
}

void EvalExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (operand.ptr);
}

void InSetStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY

  MTBL (operand1.ptr);
  MTBL (operand2.ptr);
}

void BinaryOp::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);

  MTBL (operand1.ptr);
  MTBL (operand2.ptr);
}

void VarName::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
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
        ((CSearchData*)searchData)->constructNesting = DString(LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecBarText(*(CSearchData*)searchData);
      }
    }
    else { //  search symbol by name
      if (!IsSelfVar()
      && varName.StringMatch(((CSearchData*)searchData)->findRefs.searchName,
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(matchCase),
            ((CSearchData*)searchData)->findRefs.FindRefFlags.Contains(wholeWord))) {
        ((CSearchData*)searchData)->synObjectID = synObjectID;
        ((CSearchData*)searchData)->constructNesting = DString(LocationOfConstruct());
        ((CPEBaseDoc*)((CSearchData*)searchData)->doc)->SetExecBarText(*(CSearchData*)searchData);
      }
    }
    break;
#endif
  default: ;
  }
}

void Assignment::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (targetObj.ptr);
  MTBL (exprValue.ptr);
}

void ArrayAtIndex::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (update == onCopy)
    ((TIDTable*)table)->ChangeFromTab(opFunctionID);
  else if (update == onMove)
    ((TIDTable*)table)->ChangeRefToClipID(opFunctionID);

  MTBL (arrayObj.ptr);
  MTBL (arrayIndex.ptr);
}

void FuncExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&inputs);
  }

  if (handle.ptr)
    MTBL (handle.ptr);
  if (function.ptr)
    MTBL (function.ptr);
}

void FuncStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  FuncExpression::MakeTable(table,inINCL,parent,update,where,chxp,searchData);

  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&outputs);
  }
}

void Callback::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (callbackType.ptr);
  MTBL (callback.ptr);
  MTBL (onEvent.ptr);
}

void IfThen::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (ifCondition.ptr);
  MTBL (thenPart.ptr);
}

void IfStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&ifThens);
  }
  if (elsePart.ptr)
    MTBL (elsePart.ptr);
}

void IfxThen::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (ifCondition.ptr);
  MTBL (thenPart.ptr);
}

void IfExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&ifThens);
  }
  if (elsePart.ptr)
    MTBL (elsePart.ptr);
}

void TypeBranch::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (exprType.ptr);
  MTBL (varName.ptr);

  if (thenPart.ptr)
    MTBL (thenPart.ptr);
}

void Branch::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  for ( chp = (CHE*)caseLabels.first;
        chp;
        chp = (CHE*)chp->successor)
    MTBLCHE (chp,&caseLabels);

  if (thenPart.ptr)
    MTBL (thenPart.ptr);
}

void SwitchStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  MTBL (caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&branches);
  }
  if (elsePart.ptr)
    MTBL (elsePart.ptr);
}

void CatchClause::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (catchClause.ptr)
    MTBL (catchClause.ptr);
}

void TryStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  MTBL (tryStatement.ptr);
  for (chp = (CHE*)catchClauses.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&catchClauses);
  }
}

void TypeSwitchStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  MTBL (caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&branches);
  }
  if (elsePart.ptr)
    MTBL (elsePart.ptr);
}

void AssertStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (statement.ptr);
}

void ThrowStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (error.ptr)
    MTBL (error.ptr);
}

void AttachObject::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  if (objType.ptr)
    MTBL (objType.ptr);
  if (itf.ptr) // if: because of Run/NewExpression
    MTBL (itf.ptr);
/*  if (orgunit.ptr)
    MTBL (orgunit.ptr);*/
  if (url.ptr)
    MTBL (url.ptr);
}

void Run::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  AttachObject::MakeTable(table,inINCL,parent,update,where,chxp,searchData);

  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&inputs);
  }
  if (varName.ptr)
    MTBL (varName.ptr);
  if (initializerCall.ptr)
    MTBL (initializerCall.ptr);
}

void QueryItf::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (itf.ptr);
  MTBL (givenObj.ptr);
}

void GetUUID::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (itf.ptr);
}

void IntegerInterval::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (from.ptr);
  MTBL (to.ptr);
}

void Quantifier::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  if (elemType.ptr)
    MTBL (elemType.ptr);
  for (chp = (CHE*)quantVars.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&quantVars);
  }
  if (set.ptr)
    MTBL (set.ptr);
}

void QuantStmOrExp::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor) {
    MTBLCHE (chp,&quantifiers);
  }

  if (statement.ptr)
    MTBL (statement.ptr);

  if (IsExists()
  && ((Exists*)this)->updateStatement.ptr)
    MTBL (((Exists*)this)->updateStatement.ptr);
}

void SelectExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  QuantStmOrExp::MakeTable(table,inINCL,parent,update,where,chxp,searchData);

  MTBL (addObject.ptr);
  MTBL (resultSet.ptr);
}

void NewExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  AttachObject::MakeTable(table,inINCL,parent,update,where,chxp,searchData);

  if (varName.ptr)
    MTBL (varName.ptr);
  if (initializerCall.ptr)
    MTBL (initializerCall.ptr);
  if (butStatement.ptr)
    MTBL (butStatement.ptr);
}

void CloneExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (varName.ptr);
  MTBL (fromObj.ptr);
  if (butStatement.ptr)
    MTBL (butStatement.ptr);
}

void CopyStatement::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (fromObj.ptr);
  MTBL (ontoObj.ptr);
}

void EnumItem::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (itemNo.ptr);
  MTBL (enumType.ptr);
}

void ExtendExpression::MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData)
{
  ENTRY
  MTBL (extendObj.ptr);
  MTBL (extendType.ptr);
}
