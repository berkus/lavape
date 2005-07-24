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
#ifdef INTERPRETER
#include "Check.h"
#else
#include "ExecView.h"
#include "Q6_null.xpm"
#endif
#include "wx_obj.h"
#include "LavaBaseDoc.h"
#include "Convert.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "DIO.h"


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

LavaObjectPtr SynObject::Evaluate (CheckData &ckd, LavaVariablePtr,unsigned) {
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

bool SynObject::Execute (CheckData &ckd, LavaVariablePtr,unsigned) {
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
    if (!declHandle)
      return false;
    if (ckd.document->GetOperatorID(declHandle,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
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
  else if (parentObject->primaryToken == elsif_T
  || parentObject->primaryToken == ifx_T)
    return false;
  else if (parentObject->IsBinaryOp())
    return false;
  else if (parentObject->primaryToken == Minus_T)
    return false;
  else if (parentObject->primaryToken == attach_T)
    return false;
  else if (parentObject->primaryToken == clone_T)
    return false;
  else if (parentObject->primaryToken == connect_T)
    return false;
  else if (parentObject->primaryToken == disconnect_T)
    return false;
  else if (parentObject->primaryToken == copy_T)
    return false;
  else if (parentObject->primaryToken == qua_T)
    return false;
  else if (parentObject->primaryToken == quant_T)
    return false;
  else if (parentObject->primaryToken == item_T)
    return false;
  else if (parentObject->primaryToken == intIntv_T)
    return false;
  else if (parentObject->IsFuncInvocation())
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
    if (!declHandle)
      return false;
    if (ckd.document->GetOperatorID(declHandle,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
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
  else if (parentObject->primaryToken == elsif_T
  || parentObject->primaryToken == ifx_T)
    return false;
  else if (parentObject->IsBinaryOp())
    return false;
  else if (parentObject->primaryToken == Minus_T)
    return false;
  else if (parentObject->primaryToken == attach_T)
    return false;
  else if (parentObject->primaryToken == clone_T)
    return false;
  else if (parentObject->primaryToken == connect_T)
    return false;
  else if (parentObject->primaryToken == disconnect_T)
    return false;
  else if (parentObject->primaryToken == copy_T)
    return false;
  else if (parentObject->primaryToken == qua_T)
    return false;
  else if (parentObject->primaryToken == quant_T)
    return false;
  else if (parentObject->primaryToken == item_T)
    return false;
  else if (parentObject->primaryToken == intIntv_T)
    return false;
  else if (parentObject->IsFuncInvocation())
    return false;

  return true;
}

bool SynObject::NullAdmissible (CheckData &ckd) {
  LavaDECL *targetDecl, *declHandle;
  SynObject *targetObj, *synObj;
  Assignment *assig;
  IfExpression *ifx;
  MultipleOp *multOpExp;
  BinaryOp *binOpEx;
  Disconnect *disconnStm;
  Connect *connectStm;
  CHE *chpFormIn;
  Category cat;
  TID targetTid, tidOperatorFunc;
  SynFlags ctxFlags;

  ckd.tempCtx = ckd.lpc;
  if (parentObject->primaryToken == parameter_T) {
    targetDecl = ckd.document->IDTable.GetDECL(((Parameter*)parentObject)->formParmID,ckd.inINCL);
    if (targetDecl && !targetDecl->TypeFlags.Contains(isOptional))
      return false;
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
      if (targetDecl && !targetDecl->TypeFlags.Contains(isOptional))
        return false;
    }
  }
  else if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    targetObj = (SynObject*)assig->targetObj.ptr;
    return targetObj->IsOptional(ckd);
  }
  else if (parentObject->primaryToken == ObjRef_T
    && parentObject->parentObject->primaryToken == connect_T) {
    connectStm = (Connect*)parentObject->parentObject;
    if (parentObject->whereInParent == (address)&connectStm->signalSender.ptr)
      return false;
  }
  else if (parentObject->primaryToken == connect_T) {
    connectStm = (Connect*)parentObject;
    if (whereInParent == (address)&connectStm->signalSender.ptr)
      return false;
  }
  else if (parentObject->primaryToken == disconnect_T) {
    disconnStm = (Disconnect*)parentObject;
    if (whereInParent == (address)&disconnStm->signalSender.ptr
    && ((SynObject*)disconnStm->signalReceiver.ptr)->primaryToken == nil_T)
      return false;
    else if (whereInParent == (address)&disconnStm->signalReceiver.ptr
    && ((SynObject*)disconnStm->signalSender.ptr)->primaryToken == nil_T)
      return false;
    else
      return true;
  }
  else if (parentObject->primaryToken == ObjRef_T
  && parentObject->parentObject->primaryToken == disconnect_T) {
    disconnStm = (Disconnect*)parentObject->parentObject;
    synObj = parentObject;
    if (synObj->whereInParent == (address)&disconnStm->signalSender.ptr
    && ((SynObject*)disconnStm->signalReceiver.ptr)->primaryToken == nil_T)
      return false;
    else if (synObj->whereInParent == (address)&disconnStm->signalReceiver.ptr
    && ((SynObject*)disconnStm->signalSender.ptr)->primaryToken == nil_T)
      return false;
    else
      return true;
  }
  else if (IsFuncHandle())
    return false;
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
      && parentObject->primaryToken == assignFX_T))
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
  && !IsAssigTarget()
  && !(parentObject->primaryToken == ObjRef_T && parentObject->flags.Contains(isDisabled)))
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
    || parentObject->primaryToken == run_T
    || (primaryToken == assignFS_T
        && (parentObject->primaryToken == connect_T
            || parentObject->primaryToken == signal_T)))
      return false;
    else
      return true;
  switch (primaryToken) {
  case FuncPH_T:
  case FuncRef_T:
  case FuncDisabled_T:
    if (parentObject->IsSelfVar()
    || parentObject->primaryToken == assignFX_T
    || (parentObject->primaryToken == assignFS_T
        && (parentObject->parentObject->primaryToken == connect_T
            || parentObject->parentObject->primaryToken == signal_T))
    || parentObject->primaryToken == connect_T
    || parentObject->primaryToken == disconnect_T
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

ROContext SynObject::ReadOnlyContext () {
  SynObject *obj=this, *parent=parentObject;

  while (parent) {
    if (parent->IsReadOnlyClause(obj))
      return roClause;
    obj = parent;
    parent = parent->parentObject;
  }

  if (obj->primaryToken == invariant_T
  || obj->primaryToken == require_T
  || obj->primaryToken == ensure_T)
    return assertion;

  if (((SelfVar*)obj)->execDECL->ParentDECL->TypeFlags.Contains(isConst))
    return roExec;
  else
    return noROContext;
}
/*
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
*/
bool SynObject::InOldExpression () {
  SynObject *parent;

  for (parent=parentObject;
       parent;
       parent = parent->parentObject)
    if (parent->primaryToken == old_T)
      return true;
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

bool Reference::IsExecutable() {
  if (type == FuncPH_T)
    return true;
  else
    return false;
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

bool MultipleOp::IsReadOnlyClause(SynObject *) {
  switch (primaryToken) {
  //case or_T:
  case xor_T:
    return true;
  default:
    return false;
  }
}

bool QuantStmOrExp::IsReadOnlyClause(SynObject *synObj) {
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

bool IfThen::IsReadOnlyClause(SynObject *synObj) {
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

bool IfxThen::IsReadOnlyClause(SynObject *synObj) {
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

#ifdef INTERPRETER
QString SynObject::whatsThisText() { return QString::null; }
#else
QString SynObject::whatsThisText() {
  switch (primaryToken) {
  case CrtblPH_T:
    break;
  case Exp_T:
    if (parentObject->primaryToken == fail_T)
      return QString(QObject::tr("<p>This is a placeholder for an <b>optional executable <font color=red><i>Lava</i></font> expression</b> in a fail statement."
      "<br><br>Press the Delete key to remove it.</p>"));
    else
      return QString(QObject::tr("<p>This is a placeholder for an <b>executable <font color=red><i>Lava</i></font> expression</b></p>"));
  case ExpDisabled_T:
    return QString(QObject::tr("<p>This is a disabled placeholder for an executable <font color=red><i><b>Lava</b></i></font> expression</p>"));
  case FuncPH_T:
    if (((FuncExpression*)parentObject)->handle.ptr)
      return QString(QObject::tr("<p>This is a <b>placeholder for a function reference</b>."
      "<br><br>Select a function from the function combo-boxes at the top of the exec window.</p>"));
    else
      return QString(QObject::tr("<p>This is a <b>placeholder for a reference to a static function</b>."
      "<br><br>First select an interface containing the desired static function from the type combo-boxes "
      "at the top of the exec window.<br><br>Then select a static function from the function combo-box "
      "which appears to the right of the type combo-boxes.</p>"));
  case FuncRef_T:
    if (((FuncExpression*)parentObject)->handle.ptr)
      return QString(QObject::tr("<p>This is a <b>function reference</b>."
      "<br><br>To replace it, select a function from the function combo-boxes at the top of the exec window.</p>"));
    else
      return QString(QObject::tr("<p>This is a <b>reference to a static function</b>."
      "<br><br>To replace it, first select an interface containing the desired static function from the type combo-boxes "
      "at the top of the exec window.<br><br>Then select a static function from the function combo-box "
      "which appears to the right of the type combo-boxes.</p>"));
  case FuncDisabled_T:
    return QString(QObject::tr("<p>This is a <b>disabled placeholder for a function reference</b>."
      "<br><br>Insert a call expression first to enable it.</p>"));
  case ObjPH_T:
    if (parentObject->primaryToken == parameter_T)
      return QString(QObject::tr("<p>This is a placeholder for an output parameter. Output parameters are always optional."
        "<br><br>Press the Delete key to remove it.</p>"));
    else
      return QString(QObject::tr("<p>This is a placeholder for a local or member variable</p>"));
  case ObjPHOpt_T:
    break;
  case ObjDisabled_T:
    return QString(QObject::tr("<p>This is a placeholder for a disabled local or member variable</p>"));
  case SetPH_T:
    break;
  case SetAttribPH_T:
    break;
  case Stm_T:
    return QString(QObject::tr("<p>This is a placeholder for an executable <font color=red><i><b>Lava</b></i></font> statement</p>"));
  case TypePH_T:
    if (parentObject->primaryToken == quant_T
    && !parentObject->parentObject->IsDeclare())
      return QString(QObject::tr("<p>This is a placeholder for an optional type reference."
      "<br><br>Press the Delete key to remove it.</p>"));
    else
      return QString(QObject::tr("<p>This is a placeholder for a type reference</p>"));
  case TypeRef_T:
    if (parentObject->primaryToken == quant_T
    && !parentObject->parentObject->IsDeclare())
      return QString(QObject::tr("<p>This is an optional type reference."
      "<br><br>Press the Delete key twice to remove it.</p>"));
    else
      return QString(QObject::tr("<p>This is a type reference</p>"));
  case VarPH_T:
    return QString(QObject::tr("<p>This is a placeholder for a <b>new</b> local variable to be declared here</p>"));
  case const_T:
    return QString(QObject::tr("<p>This is a constant</p>"));
  case nil_T:
    if (parentObject->primaryToken == disconnect_T
    || parentObject->primaryToken == connect_T) {
      QPixmap nullIcon = QPixmap(Q6_null);
      QMimeSourceFactory::defaultFactory()->setPixmap( "nullIcon", Q6_null );
      if (replacedType == Exp_T)
        return QString(QObject::tr("<p>This is a \"wildcard\", meaning \"any object\"; "
        "produced by the <img src=\"nullIcon\"> (\"undefined\") tool button</p>"));
      else
        return QString(QObject::tr("<p>This is a \"wildcard\", meaning \"any signal or callback, respectively\"; "
        "produced by the <img src=\"nullIcon\"> (\"undefined\") tool button</p>"));
    }
    else
      return QString(QObject::tr("<p>This is a \"null/nil/nothing/undefined\" constant</p>"));
  default:
    return QString(QObject::tr("<p>\"What's this\" help is not yet available for this syntactic element</p>"));
  }
  return QString(QObject::tr("<p>\"What's this\" help is not yet available for this syntactic element</p>"));
}
#endif

#ifdef INTERPRETER
void SynObject::whatNext() {}
#else
void SynObject::whatNext() {
  switch (primaryToken) {
  case CrtblPH_T:
    break;
  case Exp_T:
    QWhatsThis::display(QString(QObject::tr("<p>Insert an executable <font color=red><i><b>Lava</b></i></font> expression "
      "by clicking one of the enabled expression buttons on the keyword toolbar "
      "or on the operations toolbar. "
      "<a href=\"../whatNext/Expr_wn.htm\">More...</a></p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case ExpDisabled_T:
//    return QString(QObject::tr("<p>This is a disabled placeholder for an executable Lava expression</p>"));
    break;
  case ExpOpt_T:
    break;
  case FuncPH_T:
    if (((FuncExpression*)parentObject)->handle.ptr)
      QWhatsThis::display(QString(QObject::tr("Select a function from the function combo-boxes at the top of the exec window.</p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    else
      QWhatsThis::display(QString(QObject::tr("<p>This is a <b>placeholder for a reference to a static function</b>."
      "<br><br>First select an interface containing the desired static function from the type combo-boxes "
      "at the top of the exec window.<br><br>Then select a static function from the function combo-box "
      "which appears to the right of the type combo-boxes.</p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case FuncDisabled_T:
    QWhatsThis::display(QString(QObject::tr("<p>This is a <b>disabled placeholder for a function reference</b>."
      "<br><br>Insert a call expression first to enable it.</p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case TDOD_T:
    QWhatsThis::display(QString(QObject::tr("<p>Select a member variable from the \"Members\" "
      "combo-box at the top of this window (if a member of the required type exists!). "
      "<a href=\"../whatNext/Var_wn.htm\">More...</a></p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case ObjPH_T:
    QWhatsThis::display(QString(QObject::tr("<p>Select a variable from the \"Variables\" "
      "combo-box at the top of this window. ")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case ObjPHOpt_T:
    break;
  case ObjDisabled_T:
    break;
  case SetPH_T:
    break;
  case SetAttribPH_T:
    break;
  case Stm_T:
    QWhatsThis::display(QString(QObject::tr("<p>Insert an executable <font color=red><i><b>Lava</b></i></font> statement "
      "by clicking one of the enabled statement buttons on the keyword toolbar "
      "or on the operations toolbar. "
      "<a href=\"../whatNext/Stm_wn.htm\">More...</a></p>")),execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case TypePH_T:
  case TypeRef_T:
    QWhatsThis::display(QString(QObject::tr("<p>Select a type from the \"Types\" or "
      "\"Basic types\" combo-boxes at the top of this window. "
      "<a href=\"../whatNext/Type_wn.htm\">More...</a></p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case VarPH_T:
    QWhatsThis::display(QString(QObject::tr("<p>Enter a variable name  "
      "or insert another such placeholder; "
      "<a href=\"../whatNext/VarPH_wn.htm\">details...</a></p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  case VarName_T:
    QWhatsThis::display(QString(QObject::tr("<p>Edit this variable name  "
      "or insert another <font color=red>&lt;varName&gt;</font> placeholder; "
      "<a href=\"../whatNext/VarName_wn.htm\">details...</a></p>")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    break;
  default:
    if (((CExecView*)execView)->text->currentSynObj->StatementSelected(((CExecView*)execView)->text->currentSelection))
      QWhatsThis::display(QString(QObject::tr("<p>Insert an executable <font color=red><i><b>Lava</b></i></font> statement "
      "before or after the selected statement "
      "by clicking one of the enabled statement buttons on the keyword toolbar "
        "or on the operations toolbar. "
        "<a href=\"../whatNext/Statement_wn.htm\">More...</a></p>")),execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    else if (((CExecView*)execView)->text->currentSynObj->ExpressionSelected(((CExecView*)execView)->text->currentSelection))
      QWhatsThis::display(QString(QObject::tr("<p>Insert an executable <font color=red><i><b>Lava</b></i></font> expression "
        "by clicking one of the enabled expression buttons on the keyword toolbar "
        "or on the operations toolbar. "
        "<a href=\"../whatNext/Expression_wn.htm\">More...</a></p>")),execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
    else
      QWhatsThis::display(QString(QObject::tr("<p>No specific \"What next?\" help available for this selection")),
      execView->mapToGlobal(startToken->data.rect.topLeft()+QPoint(70,18)),execView);
  }
}
#endif
