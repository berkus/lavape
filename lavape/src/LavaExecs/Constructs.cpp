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
#include "wx_obj.h"
#include "Check.h"
#include "LavaBaseDoc.h"
#include "Convert.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "DIO.h"
//#include "stdafx.h"


#define ADJUST(nnn,decl) \
  nnn.nINCL = ckd.document->IDTable.IDTab[decl->inINCL]->nINCLTrans[nnn.nINCL].nINCL
// for Sniff parser
#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()
// for Sniff parser


TToken OperandType (TToken token) {
  // only relevant for operators
  switch (token) {
  case Semicolon_T:
  case or_T:
  case xor_T:
  case and_T:
  case andThen_T:
    return Stm_T;

  case Equal_T:
  case NotEqual_T:
  case LessThan_T:
  case GreaterThan_T:
  case LessEqual_T:
  case GreaterEqual_T:
  case Plus_T:
  case Minus_T:
  case Mult_T:
  case Slash_T:
  case Percent_T:
  case BitOr_T:
  case BitXor_T:
  case BitAnd_T:
  case Lshift_T:
  case Rshift_T:
    return Exp_T;

  default:
  return Stm_T;
  }
}

TToken PlaceHolderType (TToken token) {
  // only relevant for operators
  switch (token) {
  case Semicolon_T:
  case or_T:
  case xor_T:
  case and_T:
  case andThen_T:
  case Equal_T:
  case NotEqual_T:
  case LessThan_T:
  case GreaterThan_T:
  case LessEqual_T:
  case GreaterEqual_T:
    return Stm_T;

  case Plus_T:
  case Minus_T:
  case Mult_T:
  case Slash_T:
  case Percent_T:
  case BitOr_T:
  case BitXor_T:
  case BitAnd_T:
  case Lshift_T:
  case Rshift_T:
    return Exp_T;

  default:
  return Stm_T;
  }
}


void SynObject::NLincIndent (CProgTextBase &t) {
#ifdef EXECVIEW
  currentIndent++;
  t.NewLine();
#endif
}

void SynObject::NLdecIndent (CProgTextBase &t) {
#ifdef EXECVIEW
  if (currentIndent >= 1) {
    currentIndent--;
  }
  t.NewLine();
#endif
}


SynObject::SynObject () {
  type = Stm_T;
  replacedType = type;
  primaryToken = Stm_T;
  primaryTokenNode = 0;
  startToken = 0;
  parentObject = 0;
  whereInParent = 0;
  containingChain = 0;
  oldError = 0;
  lastError = 0;
  synObjectID = 0;
  currentIndent = 0;
	startPos = -1;
  errorChanged = false;
}

LavaObjectPtr SynObject::Evaluate (CheckData &ckd, LavaVariablePtr) {
#ifdef INTERPRETER
  DString dFileName=ckd.document->GetAbsSynFileName();
  QString cFileName = dFileName.c;
  QString cExecName = ckd.myDECL->ParentDECL->FullName.c;
  QString cClassName = GetWxClassInfo()->className;
  QString msg = cFileName + " / " + cExecName + ": " + cClassName + " not yet implemented!";
  critical(qApp->mainWidget(),qApp->name(),msg,QMessageBox::Ok,0,0);
//  AfxMessageBox(msg,MB_OK|MB_ICONSTOP);
  ckd.document->throwError = true;
//!!!  LavaEnd(ckd.document, false);
#endif
  return 0;
}

bool SynObject::Execute (CheckData &ckd, LavaVariablePtr) {
#ifdef INTERPRETER
  DString dFileName=ckd.document->GetAbsSynFileName();
  QString cFileName = dFileName.c;
  QString cExecName = ckd.myDECL->ParentDECL->FullName.c;
  QString cClassName = GetWxClassInfo()->className;
  QString msg = cFileName + " / " + cExecName + ": " + cClassName + " not yet implemented!";
  critical(qApp->mainWidget(),qApp->name(),msg,QMessageBox::Ok,0,0);
//  AfxMessageBox(msg,MB_OK|MB_ICONSTOP);
  ckd.document->throwError = true;
//!!!  LavaEnd(ckd.document, false);
#endif
  return false;
}

bool SynObject::BoolAdmissibleOnly (CheckData &ckd) {
  LavaDECL *targetDecl, *declHandle;
  Assignment *assig;
  MultipleOp *multOpExp;
  ObjReference *handle;
  FuncExpression *funcExpr;
  CHE *chpFormIn;
  Category cat;
  SynFlags ctxFlags;
  TID targetTid, tidOperatorFunc,
      tidBool=TID(ckd.document->IDTable.BasicTypesID[B_Bool],ckd.document->isStd?0:1);

  ckd.tempCtx = ckd.lpc;
  if (parentObject->primaryToken == parameter_T) {
    targetDecl = ckd.document->IDTable.GetDECL(((Parameter*)parentObject)->formParmID,ckd.inINCL);
    if (parentObject->parentObject->IsFuncInvocation()) {
      funcExpr = (FuncExpression*)parentObject->parentObject;
      handle = (ObjReference*)funcExpr->handle.ptr;
      if (handle)
        handle->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
      else
        ckd.tempCtx = ckd.lpc;
    }
    else
      ckd.tempCtx = ckd.lpc;
    targetDecl = ckd.document->GetFinalMVType(targetDecl->RefID,targetDecl->inINCL,ckd.tempCtx,cat, &ckd);
    if (!targetDecl)
      return false;
    if (!ckd.document->IDTable.IsAn(targetDecl,tidBool,0))
      return false;
  }
  else if (parentObject->IsMultOp()) {
    multOpExp = (MultipleOp*)parentObject;
//    if ((CHE*)whereInParent == (CHE*)multOpExp->operands.first) return true;

    ((SynObject*)((CHE*)multOpExp->operands.first)->data)->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
    declHandle = ckd.document->GetTypeAndContext(declHandle,ckd.tempCtx);
    if (declHandle
    && ckd.document->GetOperatorID(declHandle,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
      chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
      targetTid = ((LavaDECL*)chpFormIn->data)->RefID;
      ADJUST(targetTid,((LavaDECL*)chpFormIn->data));
      targetDecl = ckd.document->GetFinalMTypeAndContext(targetTid,0,ckd.tempCtx, &ckd);
      if (!targetDecl)
        return false;
      if (!ckd.document->IDTable.IsAn(targetDecl,tidBool,0))
        return false;
    }
  }
  else if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    ((SynObject*)assig->targetObj.ptr)->ExprGetFVType(ckd,targetDecl,cat,ctxFlags);
    targetDecl = ckd.document->GetType(targetDecl);
    if (!targetDecl)
      return false;
    if (!ckd.document->IDTable.IsAn(targetDecl,tidBool,0))
      return false;

  }
  else if (parentObject->IsBinaryOp())
    return false;
  else if (parentObject->primaryToken == attach_T)
    return false;
  else if (parentObject->primaryToken == qua_T)
    return false;
  else if (parentObject->primaryToken == quant_T)
    return false;
  else if (parentObject->primaryToken == item_T)
    return false;
  else if (parentObject->primaryToken == intIntv_T)
    return false;

  return true;
}

bool SynObject::EnumAdmissibleOnly (CheckData &ckd) {
  LavaDECL *targetDecl, *declHandle;
  Assignment *assig;
  MultipleOp *multOpExp;
  ObjReference *handle;
  FuncExpression *funcExpr;
  CHE *chpFormIn;
  Category cat;
  SynFlags ctxFlags;
  TID targetTid, tidOperatorFunc,
      tidEnum=TID(ckd.document->IDTable.BasicTypesID[Enumeration],ckd.document->isStd?0:1);

  ckd.tempCtx = ckd.lpc;
  if (parentObject->primaryToken == parameter_T) {
    targetDecl = ckd.document->IDTable.GetDECL(((Parameter*)parentObject)->formParmID,ckd.inINCL);
    if (parentObject->parentObject->IsFuncInvocation()) {
      funcExpr = (FuncExpression*)parentObject->parentObject;
      handle = (ObjReference*)funcExpr->handle.ptr;
      if (handle)
        handle->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
      else
        ckd.tempCtx = ckd.lpc;
    }
    else
      ckd.tempCtx = ckd.lpc;
    targetDecl = ckd.document->GetFinalMVType(targetDecl->RefID,targetDecl->inINCL,ckd.tempCtx,cat, &ckd);
    if (!targetDecl)
      return false;
    if (!ckd.document->IDTable.IsAn(targetDecl,tidEnum,0))
      return false;
  }
  else if (parentObject->IsMultOp()) {
    multOpExp = (MultipleOp*)parentObject;
//    if ((CHE*)whereInParent == (CHE*)multOpExp->operands.first) return true;

    ((SynObject*)((CHE*)multOpExp->operands.first)->data)->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
    declHandle = ckd.document->GetTypeAndContext(declHandle,ckd.tempCtx);
    if (declHandle
    && ckd.document->GetOperatorID(declHandle,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
      chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
      targetTid = ((LavaDECL*)chpFormIn->data)->RefID;
      ADJUST(targetTid,((LavaDECL*)chpFormIn->data));
      targetDecl = ckd.document->GetFinalMTypeAndContext(targetTid,0,ckd.tempCtx, &ckd);
      if (!targetDecl)
        return false;
      if (!ckd.document->IDTable.IsAn(targetDecl,tidEnum,0))
        return false;
    }
  }
  else if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    ((SynObject*)assig->targetObj.ptr)->ExprGetFVType(ckd,targetDecl,cat,ctxFlags);
    targetDecl = ckd.document->GetType(targetDecl);
    if (!targetDecl)
      return false;
    if (!ckd.document->IDTable.IsAn(targetDecl,tidEnum,0))
      return false;

  }
  else if (parentObject->IsBinaryOp())
    return false;
  else if (parentObject->primaryToken == attach_T)
    return false;
  else if (parentObject->primaryToken == qua_T)
    return false;
  else if (parentObject->primaryToken == quant_T)
    return false;
  else if (parentObject->primaryToken == item_T)
    return false;
  else if (parentObject->primaryToken == intIntv_T)
    return false;

  return true;
}

bool SynObject::NullAdmissible (CheckData &ckd) {
  LavaDECL *targetDecl, *declHandle;
  SynObject *targetObj;
  Assignment *assig;
  IfExpression *ifx;
  MultipleOp *multOpExp;
  BinaryOp *binOpEx;
  CHE *chpFormIn;
  Category cat;
  TID targetTid, tidOperatorFunc;
  SynFlags ctxFlags;

  ckd.tempCtx = ckd.lpc;
  if (parentObject->primaryToken == parameter_T) {
    targetDecl = ckd.document->IDTable.GetDECL(((Parameter*)parentObject)->formParmID,ckd.inINCL);
    if (targetDecl && !targetDecl->TypeFlags.Contains(isOptional)) {
      return false;
    }
  }
  else if (parentObject->primaryToken == fail_T)
    return true;
  else if (parentObject->IsMultOp()) {
    multOpExp = (MultipleOp*)parentObject;
    if ((CHE*)whereInParent == (CHE*)multOpExp->operands.first) return false;

    ((SynObject*)((CHE*)multOpExp->operands.first)->data)->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
    declHandle = ckd.document->GetTypeAndContext(declHandle,ckd.tempCtx);
    if (declHandle
    && ckd.document->GetOperatorID(declHandle,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
      chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
      targetDecl = (LavaDECL*)chpFormIn->data;
      if (targetDecl && !targetDecl->TypeFlags.Contains(isOptional))
        return false;
    }
  }
  else if (parentObject->IsBinaryOp()) {
    binOpEx = (BinaryOp*)parentObject;
    if (whereInParent == (address)&binOpEx->operand1.ptr) return false;

    ((SynObject*)binOpEx->operand1.ptr)->ExprGetFVType(ckd,declHandle,cat,ctxFlags);
    declHandle = ckd.document->GetTypeAndContext(declHandle,ckd.tempCtx);
    if (declHandle
    && ckd.document->GetOperatorID(declHandle,(TOperator)(binOpEx->primaryToken-not_T),tidOperatorFunc)) {
      chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
      targetDecl = (LavaDECL*)chpFormIn->data;
      if (targetDecl && !targetDecl->TypeFlags.Contains(isOptional)) {
        return false;
      }
    }
  }
  else if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    targetObj = (SynObject*)assig->targetObj.ptr;
    return targetObj->IsOptional(ckd);
  }
  else if (parentObject->primaryToken == elsif_T) {
    ifx = (IfExpression*)parentObject->parentObject;
    return ifx->NullAdmissible(ckd);
  }
  else if (parentObject->primaryToken == ifx_T) { // else part
    ifx = (IfExpression*)parentObject;
    return ifx->NullAdmissible(ckd);
  }
  return true;
}

bool SynObject::IsExpression () {
  ObjReference *parent;

  if (type == Exp_T
  || type == enumConst_T
  || primaryToken == ObjPH_T
  || primaryToken == ObjRef_T
  || (type == FuncPH_T
      && (parentObject->primaryToken == assignFX_T
          || parentObject->primaryToken == callback_T)))
    return true;
  if (primaryToken != TDOD_T) return false;
  parent = (ObjReference*)parentObject;
  if ((CHE*)whereInParent == parent->refIDs.first)
    return true;
  else
    return false;
}

bool SynObject::ExpressionSelected (CHETokenNode *currentSelection) {
  SynObject *optClause;

  if (IsExpression()
  && !currentSelection->data.OptionalClauseToken(optClause)
  && !IsAssigTarget())
    return true;
  else
    return false;
}

bool SynObject::StatementSelected (CHETokenNode *currentSelection) {
  SynObject *optClause;

  if (IsStatement()
  && !currentSelection->data.OptionalClauseToken(optClause))
    return true;
  else
    return false;
}

bool SynObject::SameExec (LavaDECL *decl) {
  SynObject *parent=this;

  while (parent->parentObject) parent = parent->parentObject;
  if (((SelfVar*)parent)->execDECL == decl)
    return true;
  else
    return false;
}

bool SynObject::IsStatement () {

  if (type == Stm_T)
    if ((parentObject->primaryToken == new_T
        && whereInParent != (address)&((NewExpression*)parentObject)->butStatement.ptr)
    || parentObject->primaryToken == run_T)
      return false;
    else
      return true;
  switch (primaryToken) {
  case FuncPH_T:
  case FuncRef_T:
  case FuncDisabled_T:
    if (parentObject->IsSelfVar()
    || parentObject->primaryToken == assignFX_T
    || parentObject->primaryToken == callback_T
    || parentObject->parentObject->primaryToken == new_T
    || parentObject->parentObject->primaryToken == run_T
    )
      return false;
    else
      return true;
  default:
    return false;
  }
}

bool TokenNode::OptionalClauseToken (SynObject *&optClause) {
  switch (token) {
  case but_T:
    if (synObject->primaryToken == new_T)
      optClause = (SynObject*)((NewExpression*)synObject)->butStatement.ptr;
    else
      optClause = (SynObject*)((CloneExpression*)synObject)->butStatement.ptr;
    return true;
  case oid_T:
    if (synObject->primaryToken == new_T)
      optClause = (SynObject*)((NewExpression*)synObject)->url.ptr;
    else
      optClause = (SynObject*)((AttachObject*)synObject)->url.ptr;
    return true;
  case where_T:
    if (synObject->primaryToken == foreach_T
    && ((Exists*)synObject)->updateStatement.ptr) {
      optClause = (SynObject*)((QuantStmOrExp*)synObject)->statement.ptr;
      return true;
    }
    break;
  case else_T:
    if (synObject->primaryToken == if_T)
      optClause = (SynObject*)((IfStatement*)synObject)->elsePart.ptr;
    else if (synObject->primaryToken == ifx_T)
      return false;
    else if (synObject->primaryToken == switch_T)
      optClause = (SynObject*)((SwitchStatement*)synObject)->elsePart.ptr;
    else if (synObject->primaryToken == type_T)
      optClause = (SynObject*)((TypeSwitchStatement*)synObject)->elsePart.ptr;
    return true;
  case do_T:
    if (synObject->primaryToken == exists_T) {
      optClause = (SynObject*)((Exists*)synObject)->updateStatement.ptr;
      return true;
    }
    else if (synObject->primaryToken == foreach_T
    && ((Exists*)synObject)->statement.ptr) {
      optClause = (SynObject*)((Exists*)synObject)->updateStatement.ptr;
      return true;
    }
    break;
  default: ;
  }
  return false;
}

bool SynObject::HasOptionalParts () 
{
  switch (primaryToken) {
  case attach_T:
  case clone_T:
  case exists_T:
  case fail_T:
  case foreach_T:
  case if_T:
  case new_T:
  case switch_T:
  case transaction_T:
  case type_T:
    return true;
  case assignFS_T:
    if (((FuncStatement*)this)->outputs.first)
      return true;
    else
      return false;

  case quant_T:
    if (parentObject->primaryToken == declare_T)
      return false;
    else
      return true;
  default:
    return false;
  }
}

bool SynObject::IsArrayObj () {
  SynObject *synObj=(primaryToken == TDOD_T ? parentObject : this);

  if (synObj->parentObject->primaryToken == arrayAtIndex_T
  && synObj->whereInParent == (address)&((ArrayAtIndex*)synObj->parentObject)->arrayObj.ptr)
    return true;
  return false;
}

bool SynObject::IsFuncHandle () {
  SynObject *synObj=(primaryToken == TDOD_T ? parentObject : this);

  if (synObj->parentObject->IsFuncInvocation()
  && synObj->whereInParent == (address)&((FuncExpression*)synObj->parentObject)->handle.ptr)
    return true;
  return false;
}

bool SynObject::IsOutputParam () {
  SynObject *synObj=(primaryToken == TDOD_T ? parentObject : this);
  FuncStatement *funcStm;
  Parameter *parm;

  if (synObj->parentObject
  && synObj->parentObject->primaryToken == parameter_T
  && synObj->parentObject->parentObject->primaryToken == assignFS_T) {
    parm = (Parameter*)synObj->parentObject;
    funcStm = (FuncStatement*)parm->parentObject;
    if (parm->containingChain == (CHAINX*)&funcStm->outputs)
      return true;
  }
  return false;
}

bool SynObject::IsAssigTarget () {
  SynObject *synObj=(primaryToken == TDOD_T ? parentObject : this);

  if (synObj->parentObject->primaryToken == assign_T
  && synObj->whereInParent == (address)&((Assignment*)synObj->parentObject)->targetObj.ptr)
    return true;
  else if (synObj->parentObject->primaryToken == copy_T
  && synObj->whereInParent == (address)&((CopyStatement*)synObj->parentObject)->ontoObj.ptr)
    return true;
  else if (synObj->IsOutputParam())
    return true;
  else
    return false;
}

bool SynObject::IsIfClause () {
  CHAINX *chx;

  if (primaryToken != elsif_T) return false;
  chx = containingChain;
  if ((CHE*)whereInParent == chx->first)
    return true;
  else
    return false;
}

bool SynObject::IsObjRef () {
  ObjReference *parent;

  if (primaryToken != TDOD_T) return false;
  parent = (ObjReference*)parentObject;
  if ((CHE*)whereInParent == parent->refIDs.first)
    return true;
  else
    return false;
}

bool SynObject::InHiddenIniClause (CheckData &ckd, SynObject *&typeRef) {
  SynObject *obj=this, *parent= parentObject;

  while (parent) {
    if (parent->primaryToken == new_T
    && obj == ((NewExpression*)parent)->initializerCall.ptr
    && !obj->startToken) {
      typeRef =  (SynObject*)((NewExpression*)parent)->objType.ptr;
      return true;
    }
    obj = parent;
    parent = parent->parentObject;
  }

  return false;
}

bool SynObject::InReadOnlyContext () {
  SynObject *obj=this, *parent=parentObject;
  bool roExec=false;

  while (parent) {
    if (parent->IsReadOnlyClause(obj,roExec))
      return true;
    obj = parent;
    parent = parent->parentObject;
  }
  return false;
}

bool SynObject::InReadOnlyClause () {
  SynObject *obj=this, *parent=parentObject;
  bool roExec=false;

  while (parent) {
    if (parent->IsReadOnlyClause(obj,roExec)
    && !roExec)
      return true;
    obj = parent;
    parent = parent->parentObject;
  }
  return false;
}

FormParm::FormParm (bool) {
  type = Exp_T;
  primaryToken = FormParm_T;
  replacedType = type;
}

FormParms::FormParms (SynObject *selfVar) {
  type = FormParms_T;
  primaryToken = FormParms_T;
  replacedType = type;
  parentObject = selfVar;
}

Reference::Reference (TToken token,TID id,const char *name) {
  type = token;
  replacedType = type;
  switch (token) {
  case FuncPH_T:
    primaryToken = FuncRef_T;
    break;
  case SetPH_T:
  case TypePH_T:
    primaryToken = TypeRef_T;
    break;
  case CrtblPH_T:
  case Callee_T:
  case CompObj_T:
    primaryToken = CrtblRef_T;
    break;
  }
  refID = id;
  refName = STRING(name);
}

TDOD::TDOD () {
  type = TDOD_T;
  replacedType = type;
  primaryToken = TDOD_T;
  fieldDecl = 0;
}

ObjReference::ObjReference (TDODC &id,const char *name) {
  type = ObjPH_T;
  primaryToken = ObjRef_T;
  replacedType = type;
  refIDs = id;
  refName = STRING(name);
}

void MultipleOp::MultipleOpInit (TToken primToken) {
  primaryToken = primToken;
  type = PlaceHolderType(primaryToken);
  replacedType = type;
  operands.Append(NewCHE());
}

bool MultipleOp::IsReadOnlyClause(SynObject *, bool &roExec) {
  roExec = false;

  switch (primaryToken) {
  //case or_T:
  case xor_T:
    return true;
  default:
    return false;
  }
}

bool QuantStmOrExp::IsReadOnlyClause(SynObject *synObj, bool &roExec) {
  roExec = false;

  if (IsDeclare())
    return false;
  if (synObj->whereInParent == (address)&statement.ptr)
    return true;
  else
    return false;
}

bool IfStatement::NestedOptClause (SynObject *optClause) {
  if (optClause->whereInParent == (address)&elsePart.ptr)
    return true;
  else
    return false;
}

bool IfThen::IsRepeatableClause (CHAINX *&chx) {
  chx = &((IfStatement*)parentObject)->ifThens;
  return true;
}

bool IfThen::IsReadOnlyClause(SynObject *synObj, bool &roExec) {
  roExec = false;

  if (synObj->whereInParent == (address)&ifCondition.ptr)
    return true;
  else
    return false;
}

IfExpression::IfExpression () {
  isOpt=true;
  targetDecl=0;
}

bool IfxThen::IsRepeatableClause (CHAINX *&chx) {
  chx = &((IfExpression*)parentObject)->ifThens;
  return true;
}

bool IfxThen::IsReadOnlyClause(SynObject *synObj, bool &roExec) {
  roExec = false;

  if (synObj->whereInParent == (address)&ifCondition.ptr)
    return true;
  else
    return false;
}

bool SwitchStatement::NestedOptClause (SynObject *optClause) {
  if (optClause->whereInParent == (address)&elsePart.ptr)
    return true;
  else
    return false;
}

bool Branch::IsRepeatableClause (CHAINX *&chx) {
  chx = &((SwitchStatement*)parentObject)->branches;
  return true;
}

bool CatchClause::IsRepeatableClause (CHAINX *&chx) {
  chx = &((TryStatement*)parentObject)->catchClauses;
  return true;
}

bool TypeSwitchStatement::NestedOptClause (SynObject *optClause) {
  if (optClause->whereInParent == (address)&elsePart.ptr)
    return true;
  else
    return false;
}

bool TypeBranch::IsRepeatableClause (CHAINX *&chx) {
  chx = &((TypeSwitchStatement*)parentObject)->branches;
  return true;
}

bool Quantifier::IsRepeatableClause (CHAINX *&chx) {
  chx = &((QuantStmOrExp*)parentObject)->quantifiers;
  return true;
}

bool AttachObject::NestedOptClause (SynObject *optClause) {
  if (/*optClause->whereInParent == (address)&orgunit.ptr
  || */optClause->whereInParent == (address)&url.ptr)
    return true;
  else
    return false;
}

bool NewExpression::NestedOptClause (SynObject *optClause) {
  if (optClause->whereInParent == (address)&butStatement.ptr
  || optClause->whereInParent == (address)&url.ptr)
    return true;
  else
    return false;
}

bool CloneExpression::NestedOptClause (SynObject *optClause) {
  if (optClause->whereInParent == (address)&butStatement.ptr)
    return true;
  else
    return false;
}

bool Exists::NestedOptClause (SynObject *optClause) {
  if (primaryToken == exists_T)
    if (optClause->whereInParent == (address)&updateStatement.ptr)
      return true;
    else
      return false;
  else // foreach
    if (optClause->whereInParent == (address)&statement.ptr
    && updateStatement.ptr)
      return true;
    else if (optClause->whereInParent == (address)&updateStatement.ptr
    && statement.ptr)
      return true;
    else
      return false;
}
