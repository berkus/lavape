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
#include "ExecView.h"
#include "Check.h"
#include "Convert.h"
#include "DIO.h"
#include "CHAINANY.h"
#include "LavaAppBase.h"
#include "qstring.h"

#pragma hdrstop


#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()

#define SELF this

#define ENTRY \
  bool ignoreInherited=ignored, \
       ignore = SELF->flags.Contains(ignoreSynObj) || ignored; \
  t.ignored = ignore; \
  SELF->currentIndent = 0; \
  if (t.htmlGen) \
		SELF->startPosHTML = t.posInLine; \
	else \
	  SELF->startPos = -1; \
  if (ignore) { SELF->errorChain.Destroy(); SELF->oldError = 0; } \
  SELF->parentObject = t.currentSynObj; \
  SELF->whereInParent = where; \
  SELF->containingChain = chxp; \
  t.currentSynObj = SELF; \
  if (t.selectAt == t.currentSynObj) {\
    if (!t.newSelection) \
      t.plhSearch = true; \
  } \
  if (SELF->flags.Contains(newLine)) \
    t.SynObjNewLine(); \
  if (!t.htmlGen) { \
    SELF->primaryTokenNode = 0; \
    SELF->startToken = 0; \
  } \
  if (SELF->comment.ptr && (t.htmlGen || t.showComments)) {\
    if (!SELF->comment.ptr->trailing) {\
      t.Insert(Comment_T); \
			if (!t.htmlGen) \
				SELF->startToken = t.currentToken; \
			if (SELF->comment.ptr->inLine) {\
        t.Blank(); \
				if (t.htmlGen) \
					SELF->startPosHTML = t.posInLine; \
			} \
      else \
        t.NewLine(); \
    } \
  }

#define EXIT \
  if (SELF->comment.ptr && t.showComments) {\
    if (SELF->comment.ptr->trailing) {\
      if (SELF->comment.ptr->inLine) \
        t.Blank(); \
      else \
        t.NewLine(); \
      t.Insert(Comment_T); \
      SELF->endToken = t.currentToken; \
    } \
  } \
  if (!t.htmlGen) { \
    SELF->endToken = t.currentToken; \
    if (t.selectAfter == t.currentSynObj) {\
      t.selectAfter = 0; \
      t.plhSearch = false; \
    } \
  } \
  t.currentSynObj = SELF->parentObject; \
  t.ignored = ignoreInherited;


#define DRAW(WHERE) ((SynObject*)WHERE)->Draw(t,(address)&WHERE,0,ignore)

#define DRAWCHE(CHEP,CHXP) ((SynObject*)CHEP->data)->Draw(t,(address)CHEP,CHXP,ignore)

#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()

#define ADJUST4(nnn) \
  nnn.nINCL = t.document->IDTable.IDTab[t.ckd.inINCL]->nINCLTrans[nnn.nINCL].nINCL


static bool ifPart = true;

static inline void code (const char *text)
/* put constant text code */
{
  IO.WriteString(text);
}


static inline void codeNl (const char *text)
/* put constant text code and then new line */
{
  IO.WriteString(text);
  IO.WriteLn();
}


SynObjectV::SynObjectV () {
}

SynObjectV::SynObjectV (TToken t) {
  primaryToken = t;
  type = t;
  replacedType = type;
  switch (t) {
  case FuncDisabled_T:
  case ObjDisabled_T:
  case CrtblDisabled_T:
  case ExpDisabled_T:
    flags.INCL(isDisabled);
  }
}

void SynObjectV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (primaryToken == TypePH_T
  && parentObject->primaryToken == quant_T
  && !parentObject->parentObject->IsDeclare())
    t.Insert(TypePHopt_T);
  else if (primaryToken == Exp_T
  && parentObject->primaryToken == fail_T)
    t.Insert(ExpOpt_T);
  else if (primaryToken == ObjPH_T
    && parentObject->primaryToken == parameter_T) {
    if (!flags.Contains(ignoreSynObj))
      t.Insert(ObjPHOpt_T);
  }
  else
    t.Insert(primaryToken);
  EXIT
}

TDODV::TDODV (bool) {
  type = TDOD_T;
  replacedType = type;
  primaryToken = TDOD_T;
}

void TDODV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (!name.l)
    UpdateReference(t.ckd);
  t.Insert(primaryToken,true,flags.Contains(isOptionalExpr));
  EXIT
}

VarNameV::VarNameV (const char *vn) {
  type = VarPH_T;
  primaryToken = VarName_T;
  replacedType = type;
  varName = STRING(vn);
}

void VarNameV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  else if (flags.Contains(isAnyCatX))
    t.Insert(Mult_T);
  if (flags.Contains(isOptionalExpr))
    t.Insert(primaryToken,true,true);
  else
    t.Insert(primaryToken,true);
  EXIT
}

FormParmV::FormParmV (bool) {
  type = Exp_T;
  primaryToken = FormParm_T;
  replacedType = type;
}

void FormParmV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  DRAW(parmType.ptr);
  primaryTokenNode = startToken;
  t.Blank();
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  else if (flags.Contains(isAnyCatX))
    t.Insert(Mult_T);
  DRAW(formParm.ptr);

  EXIT
}

FormParmsV::FormParmsV (SynObject *selfVar) {
  type = FormParms_T;
  primaryToken = FormParms_T;
  replacedType = type;
  parentObject = selfVar;
}

void FormParmsV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *chp;

  ENTRY

  if (inputs.first) {
    t.Insert(inputs_T);
    t.Insert(Colon_T);
    NLincIndent(t);
    for (chp = (CHE*)inputs.first; chp; chp = (CHE*)chp->successor) {
      t.Blank();
      DRAWCHE(chp,&inputs);
      if (chp->successor) {
        t.Insert(Comma_T);
        t.NewLine();
      }
    }
  }

  if (outputs.first) {
    if (inputs.first) {
      NLdecIndent(t);
      t.Insert(outputs_T);
    }
    else
      t.Insert(outputs_T,true);
    t.Insert(Colon_T);
    NLincIndent(t);
    for (chp = (CHE*)outputs.first; chp; chp = (CHE*)chp->successor) {
      t.Blank();
      DRAWCHE(chp,&outputs);
      if (chp->successor) {
        t.Insert(Comma_T);
        t.NewLine();
      }
    }
  }

  EXIT
}

BaseInitV::BaseInitV (address,SelfVar *selfVar,LavaDECL *formBase) {
  FuncStatementV *funcStm;
  ObjReferenceV *objRef;
  TDODV *tdod;
  TDODC tdodc;
  TID baseTid(formBase->OwnID,formBase->inINCL);
  CHE *newChe;
  DString baseName;

  type = initializing_T;
  replacedType = type;
  primaryToken = initializing_T;
  if (formBase->DeclType == VirtualType)
    baseName = "<" + formBase->FullName + ">";
  else
    baseName = formBase->FullName;

  baseItf.ptr = new ReferenceV(TypePH_T,baseTid,baseName.c);

  tdod = new TDODV(true);
  tdod->name = STRING("self");
  tdod->ID = selfVar->varID;
  newChe = new CHE(tdod);
  tdodc.Append(newChe);
  objRef = new ObjReferenceV(tdodc,"self");
  tdod->parentObject = objRef;
  objRef->flags.INCL(isDisabled);
  objRef->flags.INCL(isSelfVar);

  if (formBase->DeclType == VirtualType) {
    self.ptr = objRef;
    baseItf2.ptr = baseItf.ptr->Clone();
    initializerCall.ptr = 0;
  }
  else {
    funcStm = new FuncStatementV(objRef);
    funcStm->flags.INCL(staticCall);
    funcStm->flags.INCL(isIniCallOrHandle);
    funcStm->handle.ptr = objRef;
    objRef->parentObject = funcStm;
    initializerCall.ptr = funcStm;
  }

}

void BaseInitV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(primaryToken,true);
  t.Blank();
  t.Insert(base_T);
  t.Blank();
  DRAW(baseItf.ptr);
  t.Insert(Colon_T);
  NLincIndent(t);
  if (initializerCall.ptr)
    DRAW(initializerCall.ptr);
  else {
    DRAW(self.ptr);
    t.Insert(Period_T);
    DRAW(baseItf2.ptr);
    t.Insert(Colon_T);
    t.Insert(Colon_T);
    t.Insert(Lparenth_T);
    t.Insert(dftInitializer_T);
    t.Insert(Rparenth_T);
  }
  EXIT
}

void SelfVarV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  LavaDECL *funcDecl=execDECL->ParentDECL;
  CHE *chp;
  bool first=true;

  ENTRY

  if (execDECL->DeclType == Require
  || execDECL->DeclType == Ensure) {
    DRAW(execName.ptr);
    t.Blank();
    if (execDECL->ParentDECL->ParentDECL->DeclType == Impl) {
      t.Insert(implementation_T);
      t.Blank();
    }
    t.Insert(primaryToken,true);
  }
  else {
    if (primaryToken == invariant_T
    && execDECL->ParentDECL->DeclType == Impl) {
      t.Insert(implementation_T);
      t.Blank();
    }
    t.Insert(primaryToken,true);
    t.Blank();
    DRAW(execName.ptr);
  }

  if (funcDecl->TypeFlags.Contains(isTransaction)
  || funcDecl->TypeFlags.Contains(execConcurrent)) {
    t.Blank();
    t.Insert(Lparenth_T);
    if (funcDecl->TypeFlags.Contains(execConcurrent))
      t.Insert(concurrent_T);
    if (funcDecl->TypeFlags.Contains(isTransaction)) {
      if (funcDecl->TypeFlags.Contains(execConcurrent))
        t.Blank();
      t.Insert(transaction_T);
    }
    t.Insert(Rparenth_T);
  }

  t.NewLine();
  if (formParms.ptr) {
    DRAW(formParms.ptr);
    t.NewLine();
  }
  if (baseInitCalls.first) {
    t.NewLine();
    for (chp = (CHE*)baseInitCalls.first; chp; chp = (CHE*)chp->successor) {
      if (first)
        first = false;
      else
        NLdecIndent(t);
      DRAWCHE(chp,&baseInitCalls);
    }
    t.NewLine();
  }
  t.NewLine();
  DRAW(body.ptr);
  EXIT
}

ReferenceV::ReferenceV (TToken token,TID id,const char *name) {
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

void ReferenceV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  if (!refName.l)
    UpdateReference(t.ckd);
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  if (flags.Contains(isSubstitutable))
    t.Insert(Lbrace_T);
  //if (flags.Contains(isStateObjectX))
  //  t.Insert(Tilde_T);
  //else if (flags.Contains(isSameAsSelf))
  //  t.Insert(Equal_T);
  //else if (flags.Contains(isAnyCatX))
  //  t.Insert(Mult_T);
  t.Insert(primaryToken,true);
  if (flags.Contains(isSubstitutable))
    t.Insert(Rbrace_T);

  EXIT
}

ObjReferenceV::ObjReferenceV (TDODC &id,const char *name) {
  type = ObjPH_T;
  primaryToken = ObjRef_T;
  replacedType = type;
  refIDs = id;
  refName = STRING(name);
}

void ObjReferenceV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *che;
  bool isFirst=true;

  ENTRY
  if (!refName.l)
    UpdateReference(t.ckd);

  for (che = (CHE*)refIDs.first;
       che;
       che = (CHE*)che->successor) {
    if (!isFirst)
      t.Insert(Period_T);
    else
      isFirst = false;
    DRAWCHE(che,&refIDs);
  }
  primaryTokenNode = ((SynObject*)((CHE*)refIDs.first)->data)->primaryTokenNode;
  EXIT
}

EnumConstV::EnumConstV (TToken,TID &tid,QString &name) {
  type = Exp_T;
  primaryToken = enumConst_T;
  replacedType = type;
  refID = tid;
  Id = STRING(qPrintable(name));
}

void EnumConstV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  t.Insert(primaryToken,true);
  EXIT
}

ConstantV::ConstantV (const char *c) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Const_T;
  str = STRING(c);
}

void ConstantV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  t.Insert(primaryToken,true);
  EXIT
}

BoolConstV::BoolConstV (bool val) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Boolean_T;
  boolValue = val;
}

void BoolConstV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  if (boolValue)
    t.Insert(true_T,true);
  else
    t.Insert(false_T,true);
  EXIT
}

NullConstV::NullConstV () {
  type = Exp_T;
  replacedType = type;
  primaryToken = nil_T;
}

void NullConstV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
    if (parentObject->primaryToken == connect_T
    || parentObject->primaryToken == disconnect_T)
      t.Insert(Mult_T,true);
    else
      t.Insert(nil_T,true);
  EXIT
}

SucceedStatementV::SucceedStatementV () {
  type = Stm_T;
  replacedType = type;
  primaryToken = succeed_T;
}

void SucceedStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(succeed_T,true);
  EXIT
}

FailStatementV::FailStatementV () {
  type = Stm_T;
  replacedType = type;
  primaryToken = fail_T;
  exception.ptr = new SynObjectV(ExpOpt_T);
}

void FailStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  if (exception.ptr) {
    t.Insert(throw_T,true);
    t.Blank();
    DRAW(exception.ptr);
  }
  else
    t.Insert(fail_T,true);
  EXIT
}

OldExpressionV::OldExpressionV () {
  type = Exp_T;
  replacedType = type;
  primaryToken = old_T;
  paramExpr.ptr = new SynObjectV(Exp_T);
}

void OldExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  bool parenth=IsFuncHandle();

  if (parenth)
    t.Insert(Lparenth_T);

  t.Insert(old_T,true);
  t.Blank();
  DRAW(paramExpr.ptr);

  if (parenth)
    t.Insert(Rparenth_T);

  EXIT
}

MinusOpV::MinusOpV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Minus_T;
  //operand.ptr = new SynObjectV(Exp_T);
}

void MinusOpV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (
    parentObject->IsUnaryOp()
    || parentObject->IsBinaryOp()
    || parentObject->IsInSetStm()
    || IsFuncHandle()
    || (parentObject->IsMultOp()
        && parentObject->primaryToken != Plus_T));
  if (parenth)
    t.Insert(Lparenth_T);
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}


LogicalNotV::LogicalNotV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = not_T;
  operand.ptr = new SynObjectV(Stm_T);
}

void LogicalNotV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(operand.ptr);
  EXIT
}

InSetStatementV::InSetStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = in_T;
  operand1.ptr = new SynObjectV(Exp_T);
  operand2.ptr = new SynObjectV(Exp_T);
}

void InSetStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {

  ENTRY
  DRAW(operand1.ptr);
  t.Blank();
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(operand2.ptr);
  EXIT
}

BinaryOpV::BinaryOpV (TToken primToken) {
  type = PlaceHolderType(primToken);
  replacedType = type;
  primaryToken = primToken;
  operand1.ptr = new SynObjectV(Exp_T);
  if (primaryToken == in_T)
    operand2.ptr = new SynObjectV(SetPH_T);
  else
    operand2.ptr = new SynObjectV(Exp_T);
}

void BinaryOpV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (
       ( (parentObject->IsBinaryOp() || parentObject->IsMultOp())
            && priority[primaryToken] <= priority[parentObject->primaryToken]
            && primaryToken != in_T
         )
       || parentObject->IsUnaryOp()
       || parentObject->IsInSetStm()
       || IsFuncHandle()
       || parentObject->IsThrow()
       || primaryToken == Lshift_T
       || primaryToken == Rshift_T
       );
  if (parenth)
    t.Insert(Lparenth_T);
  DRAW(operand1.ptr);
  t.Blank();
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(operand2.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}


MultipleOpV::MultipleOpV (TToken primToken) {
  MultipleOpInit(primToken);
}

void Draw (MultipleOp *self,CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *opdPtr, *opdPtr2, *pred=0;
  bool parenth, isFirst=true;
  SynObject *opd1, *opd2, *synObj;

#undef SELF
#define SELF self
  ENTRY
  parenth = (
          (self->parentObject->IsBinaryOp() && self->parentObject->IsExpression())
       || self->parentObject->IsMultOp()
       || self->parentObject->IsUnaryOp()
       || self->parentObject->IsInSetStm()
       || self->InOldExpression()
       || self->parentObject->primaryToken == qua_T
       || self->IsFuncHandle()
       || self->parentObject->IsThrow()
       );
  if (parenth) {
    t.Insert(Lparenth_T);
    t.Blank();
  }

  opdPtr = (CHE*)self->operands.first;
  opd1 = (SynObject*)opdPtr->data;
  opdPtr2 = (CHE*)opdPtr->successor;
  opd2 = (SynObject*)opdPtr2->data;

  for (; opdPtr;
       opdPtr = (CHE*)opdPtr->successor) {
    synObj = (SynObject*)opdPtr->data;
    if (pred) {
      if (self->primaryToken == Semicolon_T) {
        t.Insert(self->primaryToken,isFirst);
        t.NewLine();
      }
      else
        if (((SynObject*)((CHE*)self->operands.first)->data)->type == Stm_T) {
          t.Blank();
          t.Insert(self->primaryToken,isFirst);
					t.NewLine();
        }
        else {
          t.Blank();
          if (self->primaryToken != Plus_T
          || synObj->primaryToken != Minus_T) {
            t.Insert(self->primaryToken,isFirst);
            t.Blank();
          }
          else if (!self->primaryTokenNode)
            self->primaryTokenNode = ((SynObject*)pred->data)->primaryTokenNode;
        }
      if (isFirst && opd1->primaryToken == ignore_T)
				t.NewLine();

      isFirst = false;
    }
    DRAWCHE(opdPtr,&self->operands);
    pred = opdPtr;
  }

  if (parenth) {
    t.Blank();
    t.Insert(Rparenth_T);
  }
  EXIT
#undef SELF
#define SELF this
}

InvertOpV::InvertOpV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Invert_T;
  operand.ptr = new SynObjectV(Exp_T);
}

void InvertOpV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (parentObject
    && ( ( (parentObject->IsUnaryOp()
           || parentObject->IsBinaryOp()
           || parentObject->IsInSetStm()
           || parentObject->IsMultOp()
           || IsFuncHandle()
           )
         && priority[primaryToken] <= priority[parentObject->primaryToken]
         )
       ));
  if (parenth)
    t.Insert(Lparenth_T);
  t.Insert(primaryToken,true);
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

HandleOpV::HandleOpV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Handle_T;
  operand.ptr = new SynObjectV(Exp_T);
}

void HandleOpV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (parentObject
    && ( ( (parentObject->IsUnaryOp()
           || parentObject->IsBinaryOp()
           || parentObject->IsInSetStm()
           || parentObject->IsMultOp()
           )
         && priority[primaryToken] <= priority[parentObject->primaryToken]
         )
       ));
  if (parenth)
    t.Insert(Lparenth_T);
  t.Insert(primaryToken,true);
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

OrdOpV::OrdOpV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = Ord_T;
  operand.ptr = new SynObjectV(Exp_T);
}

void OrdOpV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (parentObject
    && ( ( (parentObject->IsUnaryOp()
           || parentObject->IsBinaryOp()
           || parentObject->IsInSetStm()
           || parentObject->IsMultOp()
           )
         && priority[primaryToken] <= priority[parentObject->primaryToken]
         )
       ));
  if (parenth)
    t.Insert(Lparenth_T);
  t.Insert(primaryToken,true);
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

EvalExpressionV::EvalExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = EvalExpr_T;
  operand.ptr = new SynObjectV(Stm_T);
}

void EvalExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (((SynObject*)operand.ptr)->IsUnaryOp()
        || ((SynObject*)operand.ptr)->IsInSetStm()
        || IsFuncHandle()
        || ((SynObject*)operand.ptr)->IsMultOp()
        || ((SynObject*)operand.ptr)->IsFuncInvocation());
  if (parenth)
    t.Insert(Lparenth_T);
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  t.Insert(primaryToken,true);
  EXIT
}

EvalStatementV::EvalStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = EvalStm_T;
  operand.ptr = new SynObjectV(Exp_T);
}

void EvalStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = !(((SynObject*)operand.ptr)->IsConstant()
        || ((SynObject*)operand.ptr)->IsPlaceHolder()
        || ((SynObject*)operand.ptr)->primaryToken == ObjRef_T);
  if (parenth)
    t.Insert(Lparenth_T);
  DRAW(operand.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  t.Insert(primaryToken,true);
  EXIT
}

AssertStatementV::AssertStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = assert_T;
  statement.ptr = new SynObjectV(Stm_T);
}

void AssertStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(primaryToken,true);
  NLincIndent(t);
  DRAW(statement.ptr);
  NLdecIndent(t);
  t.Insert(ENDassert_T);
  EXIT
}

IgnoreStatementV::IgnoreStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = ignore_T;
  igVars.Append(NewCHE(new SynObjectV(ObjPH_T)));
}

void IgnoreStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true;
  CHE *varPtr;

  ENTRY
  t.Insert(primaryToken,true);
  t.Blank();

  for (varPtr = (CHE*)igVars.first;
       varPtr;
       varPtr = (CHE*)varPtr->successor) {
    if (!isFirst) {
      t.Insert(Comma_T,false);
      t.Blank();
    }
    else
      isFirst = false;
    DRAWCHE(varPtr,&igVars);
  }
  EXIT
}

AttachObjectV::AttachObjectV (bool) {
  type = Exp_T;
  //type = attach_T;
  replacedType = type;
  primaryToken = attach_T;
  objType.ptr = new SynObjectV(CompObj_T);
  itf.ptr = new SynObjectV(TypePH_T);
//  orgunit.ptr = new SynObjectV(Exp_T);
  url.ptr = new SynObjectV(Exp_T);
}

AttachObjectV::AttachObjectV (Reference *ref) {
  type = attach_T;
  replacedType = type;
  primaryToken = attach_T;
  objType.ptr = ref;
  itf.ptr = new SynObjectV(TypePH_T);
//  orgunit.ptr = new SynObjectV(Exp_T);
  url.ptr = new SynObjectV(Exp_T);
}

void AttachObjectV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (flags.Contains(isStateObjectX))
    t.Insert(Tilde_T);
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(objType.ptr);
  if (itf.ptr) {
    t.Insert(Comma_T);
    t.NewLine();
    t.Insert(itf_T);
    t.Blank();
    DRAW(itf.ptr);
  }
/*  if (orgunit.ptr) {
    t.Insert(Comma_T);
    t.NewLine();
    t.Insert(loc_T,false,true);
    t.Blank();
    DRAW(orgunit.ptr);
  }*/
  if (url.ptr) {
    t.Insert(Comma_T);
    t.NewLine();
    t.Insert(oid_T,false,true);
    t.Blank();
    DRAW(url.ptr);
  }
  EXIT
}

RunV::RunV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = run_T;
  initiator.ptr = new SynObjectV(Callee_T);
}

RunV::RunV (Reference *ref) {
  type = Stm_T;
  replacedType = type;
  primaryToken = run_T;
  initiator.ptr = ref;
}

void RunV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true;
  CHE *paramPtr;

  ENTRY

  t.Insert(primaryToken,true);
  t.Blank();
  if (t.leftArrows) {
    DRAW(initiator.ptr);
    if (!primaryTokenNode)
      primaryTokenNode = ((SynObject*)initiator.ptr)->primaryTokenNode;

    if (inputs.first
    && flags.Contains(inputArrow)) {
      t.Blank();
      t.Insert(Larrow_T);
      t.Blank();
    }

    if ((inputs.first
        && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Lparenth_T);

    for (paramPtr = (CHE*)inputs.first;
         paramPtr;
         paramPtr = (CHE*)paramPtr->successor) {
      if (!isFirst)
        t.Insert(Comma_T,false);
      else
        isFirst = false;
      DRAWCHE(paramPtr,&inputs);
    }
    if ((inputs.first
        && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Rparenth_T);
  }
  else {
    if (flags.Contains(inputArrow)) {
      if (inputs.first != inputs.last)
        t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T,false);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      if (inputs.first != inputs.last)
        t.Insert(Rparenth_T);

      if (inputs.first) {
        t.Blank();
        t.Insert(Rarrow_T);
        t.Blank();
      }

      DRAW(initiator.ptr);
      if (!primaryTokenNode)
        primaryTokenNode = ((SynObject*)initiator.ptr)->primaryTokenNode;
    }
    else {
      DRAW(initiator.ptr);
      if (!primaryTokenNode)
        primaryTokenNode = ((SynObject*)initiator.ptr)->primaryTokenNode;
      if (inputs.first)
        t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T,false);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      if (inputs.first)
        t.Insert(Rparenth_T);
    }
  }
  EXIT
}

QueryItfV::QueryItfV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = itf_T;
  itf.ptr = new SynObjectV(TypePH_T);
  givenObj.ptr = new SynObjectV(Exp_T);
}

void QueryItfV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(itf_T,true);
  t.Blank();
  DRAW(itf.ptr);
  t.Blank();
  t.Insert(of_T,true);
  t.Blank();
  DRAW(givenObj.ptr);
  EXIT
}

GetUUIDV::GetUUIDV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = uuid_T;
  itf.ptr = new SynObjectV(TypePH_T);
}

void GetUUIDV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(uuid_T,true);
  t.Blank();
  t.Insert(of_T,true);
  t.Blank();
  DRAW(itf.ptr);
  EXIT
}

NewExpressionV::NewExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = new_T;
  objType.ptr = new SynObjectV(CrtblPH_T);
  errorInInitializer = false;
  butStatement.ptr = new SynObjectV(Stm_T);
}

NewExpressionV::NewExpressionV (Reference *ref, bool withItf, bool withLoc) {
  type = Exp_T;
  replacedType = type;
  primaryToken = new_T;
  objType.ptr = ref;
  errorInInitializer = false;
  varName.ptr = new VarNameV("temp");
  if (withItf)
    itf.ptr = new SynObjectV(TypePH_T);
  if (withLoc) {
    url.ptr = new SynObjectV(Exp_T);
  }
  if (!withItf)
    butStatement.ptr = new SynObjectV(Stm_T);
}

NewExpressionV::NewExpressionV (FuncStatement *ref, bool withItf, bool withLoc) {
  type = Exp_T;
  replacedType = type;
  primaryToken = new_T;
  varName.ptr = new VarNameV("temp");
  initializerCall.ptr = ref;
  ref->flags.INCL(isIniCallOrHandle);
  if (withItf)
    itf.ptr = new SynObjectV(TypePH_T);
  if (withLoc) {
    url.ptr = new SynObjectV(Exp_T);
  }
  if (!withItf)
    butStatement.ptr = new SynObjectV(Stm_T);
}

void NewExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  LavaDECL *iniDecl=initializerCall.ptr && !ignore?
      ((FuncStatement*)initializerCall.ptr)->funcDecl
      :0;
  bool isFuncHandle=IsFuncHandle(), showTemp;

  showTemp = !ignore
  && (butStatement.ptr
    || !iniDecl
    || !iniDecl->TypeFlags.Contains(defaultInitializer)
    || errorInInitializer);

  if (isFuncHandle)
    t.Insert(Lparenth_T);

  if (flags.Contains(isStateObjectX) && !showTemp)
    t.Insert(Tilde_T);

  if (initializerCall.ptr) {
    t.Insert(primaryToken,true);
    t.Blank();
    DRAW(objType.ptr);
    if (!((SynObject*)((FuncStatement*)initializerCall.ptr)->function.ptr)->IsPlaceHolder() && !ignore)
      iniDecl = t.document->IDTable.GetDECL(((Reference*)((FuncStatement*)initializerCall.ptr)->function.ptr)->refID,t.ckd.inINCL);
    if (showTemp) {
      t.Blank();
      if (flags.Contains(isStateObjectX))
        t.Insert(Tilde_T);
      DRAW(varName.ptr);
    }
    if (!ignore
    && (!iniDecl
      || !iniDecl->TypeFlags.Contains(defaultInitializer)
      || errorInInitializer)) {
      NLincIndent(t);
      DRAW(initializerCall.ptr);
      NLdecIndent(t);
    }
    else {
      ((SynObject*)initializerCall.ptr)->startToken = 0;
      if (butStatement.ptr)
        t.NewLine();
    }

    if (butStatement.ptr) {
      t.Insert(but_T,false,true);
      NLincIndent(t);
      DRAW(butStatement.ptr);
      NLdecIndent(t);
    }
    if (butStatement.ptr
    || (iniDecl && !iniDecl->TypeFlags.Contains(defaultInitializer))
    || errorInInitializer)
      t.Insert(ENDnew_T);
  }
  else {
    t.Insert(primaryToken,true);
    t.Blank();
    DRAW(objType.ptr);
    if (butStatement.ptr && varName.ptr) {
      t.Blank();
      DRAW(varName.ptr);
    }

    if (itf.ptr) {
      t.Insert(Comma_T);
      t.NewLine();
      t.Insert(itf_T);
      t.Blank();
      DRAW(itf.ptr);
    }
    if (url.ptr) {
      t.Insert(Comma_T);
      t.NewLine();
      t.Insert(oid_T,false,true);
      t.Blank();
      DRAW(url.ptr);
    }
    if (butStatement.ptr) {
      t.NewLine();
      t.Insert(but_T,false,true);
      NLincIndent(t);
      DRAW(butStatement.ptr);
      NLdecIndent(t);
      t.Insert(ENDnew_T);
    }
  }

  if (isFuncHandle)
    t.Insert(Rparenth_T);

  EXIT
}

CloneExpressionV::CloneExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = clone_T;
  varName.ptr = new VarNameV("temp");
  fromObj.ptr = new SynObjectV(Exp_T);
  butStatement.ptr = new SynObjectV(Stm_T);
}

void CloneExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (!butStatement.ptr
  && parentObject->IsOperation())
    t.Insert(Lparenth_T);
  //if (flags.Contains(isStateObjectX))
  //  t.Insert(Tilde_T);
  //else if (flags.Contains(isSameAsSelf))
  //  t.Insert(Equal_T);
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(fromObj.ptr);
  if (butStatement.ptr) {
    t.Blank();
    t.Insert(as_T);
    t.Blank();
    DRAW(varName.ptr);
    t.NewLine();
    t.Insert(but_T,false,true);
    NLincIndent(t);
    DRAW(butStatement.ptr);
    NLdecIndent(t);
    t.Insert(ENDclone_T);
  }
  else if (parentObject->IsOperation())
    t.Insert(Rparenth_T);
  EXIT
}

CopyStatementV::CopyStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = copy_T;
  fromObj.ptr = new SynObjectV(Exp_T);
  ontoObj.ptr = new SynObjectV(ObjPH_T);
}

void CopyStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(primaryToken,true);
  t.Blank();
  if (t.leftArrows) {
    DRAW(ontoObj.ptr);
    t.Blank();
    t.Insert(Larrow_T);
    t.Blank();
    DRAW(fromObj.ptr);
  }
  else {
    DRAW(fromObj.ptr);
    t.Blank();
    t.Insert(Rarrow_T);
    t.Blank();
    DRAW(ontoObj.ptr);
  }
  EXIT
}

EnumItemV::EnumItemV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = item_T;
  itemNo.ptr = new SynObjectV(Exp_T);
  enumType.ptr = new SynObjectV(TypePH_T);
}

void EnumItemV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (
    parentObject->IsUnaryOp()
    || parentObject->IsBinaryOp()
    || parentObject->IsInSetStm()
    || IsFuncHandle()
    || parentObject->IsMultOp());
  if (parenth)
    t.Insert(Lparenth_T);
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(itemNo.ptr);
  t.Blank();
  t.Insert(of_T);
  t.Blank();
  DRAW(enumType.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

ExtendExpressionV::ExtendExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = qua_T;
  extendObj.ptr = new SynObjectV(Exp_T);
  extendType.ptr = new SynObjectV(TypePH_T);
}

void ExtendExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool parenth;

  ENTRY
  parenth = (
    parentObject->IsUnaryOp()
    || parentObject->IsBinaryOp()
    || parentObject->IsInSetStm()
    || IsFuncHandle()
    || parentObject->IsMultOp());
  if (parenth)
    t.Insert(Lparenth_T);
  DRAW(extendObj.ptr);
  t.Blank();
  DRAW(extendType.ptr);
  if (!primaryTokenNode)
    primaryTokenNode = ((SynObject*)extendType.ptr)->primaryTokenNode;
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

SelectExpressionV::SelectExpressionV (bool) {
  Quantifier *quant;

  type = Exp_T;
  replacedType = type;
  primaryToken = select_T;
  quant = new QuantifierV(true);
  quantifiers.Append(NewCHE(quant));
  primaryClause.ptr = new SynObjectV(Stm_T);
  addObject.ptr = new SynObjectV(Exp_T);
  resultSet.ptr = new SynObjectV(Exp_T);
}

void SelectExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *quantPtr;
  bool isFirst=true;

  ENTRY
  t.Insert(select_T,true);
  NLincIndent(t);
  for (quantPtr = (CHE*)quantifiers.first;
       quantPtr;
       quantPtr = (CHE*)quantPtr->successor) {
    if (!isFirst) {
      t.Insert(Semicolon_T,false);
      t.NewLine();
    }
    else
      isFirst = false;
    DRAWCHE(quantPtr,&quantifiers);
  }
  NLdecIndent(t);

  t.Insert(where_T);
  NLincIndent(t);
  DRAW(primaryClause.ptr);
  NLdecIndent(t);

  t.Insert(add_T);
  NLincIndent(t);
  DRAW(addObject.ptr);
  NLdecIndent(t);
  t.Insert(to_T);
  NLincIndent(t);
  DRAW(resultSet.ptr);
  NLdecIndent(t);
  t.Insert(ENDselect_T);
  EXIT
}

ExistsV::ExistsV (bool withUpd) {
  Quantifier *quant;

  type = Stm_T;
  replacedType = type;
  primaryToken = exists_T;
  quant = new QuantifierV(true);
  quantifiers.Append(NewCHE(quant));
  primaryClause.ptr = new SynObjectV(Stm_T);
  if (withUpd)
    secondaryClause.ptr = new SynObjectV(Stm_T);
}

SynObject *ExistsV::InsertOptionals () {
  SynObject *insObj;

  if (!secondaryClause.ptr) {
    insObj = new SynObjectV(Stm_T);
    insObj->parentObject = this;
    insObj->whereInParent = (address)&secondaryClause.ptr;
    return insObj;
  }
  else
    return 0;
}

void ExistsV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *quantPtr;
  bool isFirst=true, endex=true;

  ENTRY
  t.Insert(primaryToken,true);
  NLincIndent(t);
  for (quantPtr = (CHE*)quantifiers.first;
       quantPtr;
       quantPtr = (CHE*)quantPtr->successor) {
    if (!isFirst) {
      t.Insert(Semicolon_T,false);
      t.NewLine();
    }
    else
      isFirst = false;
    DRAWCHE(quantPtr,&quantifiers);
  }

  NLdecIndent(t);
  if (secondaryClause.ptr)
    t.Insert(where_T);
  else
    t.Insert(with_T);
  NLincIndent(t);
  DRAW(primaryClause.ptr);

  if (secondaryClause.ptr) {
    NLdecIndent(t);
    t.Insert(do_T,false,true);
    NLincIndent(t);
    DRAW(secondaryClause.ptr);
  }
  NLdecIndent(t);
  t.Insert(ENDexists_T);
  EXIT
}

DeclareV::DeclareV (bool) {
  Quantifier *quant;
  //FuncStatementV *funcStm=new FuncStatementV(true,false,false);
  SynObject *stm;

  type = Stm_T;
  replacedType = type;
  primaryToken = declare_T;
  quant = new QuantifierV(false);
  quantifiers.Append(NewCHE(quant));
  //secondaryClause.ptr = funcStm;
  //funcStm->parentObject = this;
  //funcStm->whereInParent = (address)&secondaryClause.ptr;
  //funcStm->containingChain = 0;
  //funcStm->flags.INCL(isIniCallOrHandle);
  //funcStm->flags.INCL(staticCall);
  //((SynObject*)funcStm->handle.ptr)->primaryToken = ExpDisabled_T;
  //((SynObject*)funcStm->handle.ptr)->type = ExpDisabled_T;
  //((SynObject*)funcStm->handle.ptr)->replacedType = ExpDisabled_T;
  //((SynObject*)funcStm->handle.ptr)->flags.INCL(isDisabled);
  stm = new SynObjectV(Stm_T);
  stm->parentObject = this;
  primaryClause.ptr = stm;
}

void DeclareV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *quantPtr;
  bool isFirst=true;

  ENTRY
  t.Insert(primaryToken,true);
  NLincIndent(t);
  for (quantPtr = (CHE*)quantifiers.first;
       quantPtr;
       quantPtr = (CHE*)quantPtr->successor) {
    if (!isFirst) {
      t.Insert(Semicolon_T,false);
      t.NewLine();
    }
    else
      isFirst = false;
    DRAWCHE(quantPtr,&quantifiers);
  }

  NLdecIndent(t);
  if (secondaryClause.ptr) {
    t.Insert(initialize_T,false,true);
    NLincIndent(t);
    DRAW(secondaryClause.ptr);
    NLdecIndent(t);
  }
  t.Insert(do_T);
  NLincIndent(t);
  DRAW(primaryClause.ptr);

  NLdecIndent(t);
  t.Insert(ENDdeclare_T);
  EXIT
}

ForeachV::ForeachV (bool withUpd) {
  QuantifierV *quant;

  type = Stm_T;
  replacedType = type;
  primaryToken = foreach_T;
  quant = new QuantifierV(true);
  quantifiers.Append(NewCHE(quant));
  primaryClause.ptr = new SynObjectV(Stm_T);
  if (withUpd)
    secondaryClause.ptr = new SynObjectV(Stm_T);
}

void ForeachV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *quantPtr;
  bool isFirst=true;

  ENTRY
  t.Insert(primaryToken,true);
  NLincIndent(t);
  for (quantPtr = (CHE*)quantifiers.first;
       quantPtr;
       quantPtr = (CHE*)quantPtr->successor) {
    if (!isFirst) {
      t.Insert(Semicolon_T,false);
      t.NewLine();
    }
    else
      isFirst = false;
    DRAWCHE(quantPtr,&quantifiers);
  }

  if (primaryClause.ptr) {
    NLdecIndent(t);
    if (secondaryClause.ptr)
      t.Insert(where_T,false,true);
    else
      t.Insert(holds_T);
    NLincIndent(t);
    DRAW(primaryClause.ptr);
  }

  if (secondaryClause.ptr) {
    NLdecIndent(t);
    if (primaryClause.ptr)
      t.Insert(do_T,false,true);
    else
      t.Insert(do_T,false,false);
    NLincIndent(t);
    DRAW(secondaryClause.ptr);
  }

  NLdecIndent(t);
  t.Insert(ENDforeach_T);
  EXIT
}

IntegerIntervalV::IntegerIntervalV (bool) {
  type = intIntv_T;
  replacedType = type;
  primaryToken = intIntv_T;
  from.ptr = new SynObjectV(Exp_T);
  to.ptr = new SynObjectV(Exp_T);
}

void IntegerIntervalV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(Lbracket_T);
  DRAW(from.ptr);
  t.Blank();
  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(to.ptr);
  t.Insert(Rbracket_T);
  EXIT
}

QuantifierV::QuantifierV (bool withSet) {
  SynObject *newVar=new SynObjectV(VarPH_T);
  CHE *che=NewCHE(newVar);

  newVar->parentObject = this;
  newVar->whereInParent = (address)che;
  newVar->containingChain = (CHAINX*)&quantVars;
  type = quant_T;
  replacedType = type;
  primaryToken = quant_T;
  elemType.ptr = new SynObjectV(TypePH_T);
  quantVars.Append(che);
  if (withSet)
    set.ptr = new SynObjectV(SetPH_T);
}

SynObject *QuantifierV::InsertOptionals () {
  SynObject *insObj;

  if (!elemType.ptr) {
    insObj = new SynObjectV(TypePH_T);
    insObj->parentObject = this;
    insObj->whereInParent = (address)&elemType.ptr;
    return insObj;
  }
  else
    return 0;
}

void QuantifierV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true;
  CHE *varPtr;

  ENTRY
  if (elemType.ptr) {
    DRAW(elemType.ptr);
    if (!primaryTokenNode)
      primaryTokenNode = ((SynObject*)elemType.ptr)->primaryTokenNode;
    t.Blank();
  }

  for (varPtr = (CHE*)quantVars.first;
       varPtr;
       varPtr = (CHE*)varPtr->successor) {
    if (!isFirst) {
      t.Insert(Comma_T,false);
      t.Blank();
    }
    else
      isFirst = false;
    DRAWCHE(varPtr,&quantVars);
  }

  if (set.ptr) {
    t.Blank();
    if (parentObject->primaryToken == select_T)
      t.Insert(from_T,true);
    else
      t.Insert(in_T,true);
    t.Blank();
    DRAW(set.ptr);
  }

  EXIT
}

IfStatementV::IfStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = if_T;
  ifThens.Append(NewCHE(new IfThenV(true)));
  ifThens.Append(NewCHE(new IfThenV(true)));
  elsePart.ptr = new SynObjectV(Stm_T);
}

void IfStatementV::Insert2Optionals (SynObject *&elseP, SynObject *&branch, CHAINX *&chx) {
  if (!elsePart.ptr) {
    elseP = new SynObjectV(Stm_T);
    elseP->parentObject = this;
    elseP->whereInParent = (address)&elsePart.ptr;
  }

  if (ifThens.first == ifThens.last) {
    branch = new IfThenV(true);
    chx = &ifThens;
  }
}

void IfStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *ifThenPtr;
  ROContext roCtx=ReadOnlyContext();
  bool parenth;

  ENTRY

  if (!ifThens.first->successor
  && !elsePart.ptr
  && (roCtx == assertion
      || roCtx == roClause)) {
    parenth = 
      ((SynObject*)((IfThen*)((CHE*)ifThens.first)->data)->ifCondition.ptr)->IsMultOp();
    if (parenth)
      t.Insert(Lparenth_T);
    DRAW(((IfThen*)((CHE*)ifThens.first)->data)->ifCondition.ptr);
    if (parenth)
      t.Insert(Rparenth_T);

    t.Blank();
    t.Insert(implies_T,true);
    t.Blank();

    parenth = 
      ((SynObject*)((IfThen*)((CHE*)ifThens.first)->data)->thenPart.ptr)->IsMultOp();
    if (parenth)
      t.Insert(Lparenth_T);
    DRAW(((IfThen*)((CHE*)ifThens.first)->data)->thenPart.ptr);
    if (parenth)
      t.Insert(Rparenth_T);
  }
  else {
    ifPart = true;
    for (ifThenPtr = (CHE*)ifThens.first;
         ifThenPtr;
         ifThenPtr = (CHE*)ifThenPtr->successor) {
      DRAWCHE(ifThenPtr,&ifThens);
      t.NewLine();
    }
    primaryTokenNode = ((SynObject*)((CHE*)ifThens.first)->data)->primaryTokenNode;

    if (elsePart.ptr) {
      t.Insert(else_T,false,true);
      NLincIndent(t);
      DRAW(elsePart.ptr);
      NLdecIndent(t);
    }
    t.Insert(ENDif_T);
  }
  EXIT
}

IfThenV::IfThenV (bool) {
  type = elsif_T;
  replacedType = type;
  primaryToken = elsif_T;
  ifCondition.ptr = new SynObjectV(Stm_T);
  thenPart.ptr = new SynObjectV(Stm_T);
}

void IfThenV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (ifPart)
    t.Insert(if_T,true);
  else
    t.Insert(elsif_T,true,true);
  ifPart = false;
  NLincIndent(t);
  DRAW(ifCondition.ptr);
  NLdecIndent(t);
  t.Insert(then_T);
	thenToken = t.currentToken;
  NLincIndent(t);
  DRAW(thenPart.ptr);
  EXIT
}

IfdefStatementV::IfdefStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = ifdef_T;
  ifCondition.Append(NewCHE(new SynObjectV(ObjPH_T)));
  thenPart.ptr = new SynObjectV(Stm_T);
  elsePart.ptr = new SynObjectV(Stm_T);
}

void IfdefStatementV::Insert2Optionals (SynObject *&thenP,SynObject *&elseP) {
  if (!thenPart.ptr) {
    thenP = new SynObjectV(Stm_T);
    thenP->parentObject = this;
    thenP->whereInParent = (address)&thenPart.ptr;
  }
  if (!elsePart.ptr) {
    elseP = new SynObjectV(Stm_T);
    elseP->parentObject = this;
    elseP->whereInParent = (address)&elsePart.ptr;
  }
}

void IfdefStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  CHE *objRef;
  bool isFirst=true;

  t.Insert(ifdef_T,true);
  NLincIndent(t);
  for (objRef = (CHE*)ifCondition.first;
       objRef;
       objRef = (CHE*)objRef->successor) {
    if (!isFirst) {
      t.Insert(Comma_T,false);
      t.Blank();
    }
    else
      isFirst = false;
    DRAWCHE(objRef,&ifCondition);
  }
  NLdecIndent(t);
  if (thenPart.ptr) {
    t.Insert(then_T,false,true);
    NLincIndent(t);
    DRAW(thenPart.ptr);
    NLdecIndent(t);
  }

  if (elsePart.ptr) {
    t.Insert(else_T,false,true);
    NLincIndent(t);
    DRAW(elsePart.ptr);
    NLdecIndent(t);
  }
  t.Insert(ENDifdef_T);
  EXIT
}

IfExpressionV::IfExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = ifx_T;
  ifThens.Append(NewCHE(new IfxThenV(true)));
  ifThens.Append(NewCHE(new IfxThenV(true)));
  elsePart.ptr = new SynObjectV(Exp_T);
}

void IfExpressionV::InsertBranch (SynObject *&branch, CHAINX *&chx) {
  if (ifThens.first == ifThens.last) {
    branch = new IfxThenV(true);
    chx = &ifThens;
  }
}

void IfExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *ifThenPtr;

  ENTRY
  ifPart = true;
  for (ifThenPtr = (CHE*)ifThens.first;
       ifThenPtr;
       ifThenPtr = (CHE*)ifThenPtr->successor) {
    DRAWCHE(ifThenPtr,&ifThens);
    t.NewLine();
  }
  primaryTokenNode = ((SynObject*)((CHE*)ifThens.first)->data)->primaryTokenNode;

  t.Insert(else_T);
  NLincIndent(t);
  DRAW(elsePart.ptr);
  NLdecIndent(t);
  t.Insert(ENDifx_T);
  EXIT
}

ElseExpressionV::ElseExpressionV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = elseX_T;
  expr1.ptr = new SynObjectV(Exp_T);
  expr2.ptr = new SynObjectV(Exp_T);
}

void ElseExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  bool parenth=parentObject->IsUnaryOp() || parentObject->IsBinaryOp() || parentObject->IsMultOp();

  if (parenth)
    t.Insert(Lparenth_T);
  DRAW(expr1.ptr);
  t.Blank();
  t.Insert(elseX_T,true);
  t.Blank();
  DRAW(expr2.ptr);
  if (parenth)
    t.Insert(Rparenth_T);
  EXIT
}

IfxThenV::IfxThenV (bool) {
  type = elsif_T;
  replacedType = type;
  primaryToken = elsif_T;
  ifCondition.ptr = new SynObjectV(Stm_T);
  thenPart.ptr = new SynObjectV(Exp_T);
}

void IfxThenV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  if (ifPart)
    t.Insert(ifx_T,true);
  else
    t.Insert(elsif_T,true,true);
  ifPart = false;
  NLincIndent(t);
  DRAW(ifCondition.ptr);
  NLdecIndent(t);
  t.Insert(then_T);
	thenToken = t.currentToken;
  NLincIndent(t);
  DRAW(thenPart.ptr);
  EXIT
}

SwitchStatementV::SwitchStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = switch_T;
  caseExpression.ptr = new SynObjectV(Exp_T);
  branches.Append(NewCHE(new BranchV(true)));
  elsePart.ptr = new SynObjectV(Stm_T);
}

void SwitchStatementV::Insert2Optionals (SynObject *&elseP, SynObject*&, CHAINX*&) {
  if (!elsePart.ptr) {
    elseP = new SynObjectV(Stm_T);
    elseP->parentObject = this;
    elseP->whereInParent = (address)&elsePart.ptr;
  }
}

void SwitchStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *branchPtr;

  ENTRY
  t.Insert(switch_T,true);
  t.Blank();
  DRAW(caseExpression.ptr);
  t.NewLine();
  for (branchPtr = (CHE*)branches.first;
       branchPtr;
       branchPtr = (CHE*)branchPtr->successor) {
    DRAWCHE(branchPtr,&branches);
    t.NewLine();
  }

  if (elsePart.ptr) {
    t.Insert(else_T,false,true);
    NLincIndent(t);
    DRAW(elsePart.ptr);
    NLdecIndent(t);
  }
  t.Insert(ENDswitch_T);
  EXIT
}

BranchV::BranchV (bool) {
  type = case_T;
  replacedType = type;
  primaryToken = case_T;
  caseLabels.Append(NewCHE(new SynObjectV(Exp_T)));
  thenPart.ptr = new SynObjectV(Stm_T);
}

void BranchV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true;
  CHE *labelPtr;

  ENTRY
  t.Insert(case_T,true,true);
  t.Blank();
  for (labelPtr = (CHE*)caseLabels.first;
       labelPtr;
       labelPtr = (CHE*)labelPtr->successor) {
    if (!isFirst) {
      t.Insert(Comma_T,false);
      t.Blank();
    }
    else
      isFirst = false;
    DRAWCHE(labelPtr,&caseLabels);
  }
  t.Insert(Colon_T);
  NLincIndent(t);
  DRAW(thenPart.ptr);
  EXIT
}

TryStatementV::TryStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = try_T;
  tryStatement.ptr = new SynObjectV(Stm_T);
  catchClauses.Append(NewCHE(new CatchClauseV(true)));
//  elsePart.ptr = new SynObjectV(Stm_T);
}

void TryStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *branchPtr;

  ENTRY
  t.Insert(try_T,true);
  NLincIndent(t);
  DRAW(tryStatement.ptr);
  NLdecIndent(t);
  for (branchPtr = (CHE*)catchClauses.first;
       branchPtr;
       branchPtr = (CHE*)branchPtr->successor) {
    DRAWCHE(branchPtr,&catchClauses);
    t.NewLine();
  }

/*  if (elsePart.ptr) {
    t.Insert(else_T,false,true);
    NLincIndent(t);
    DRAW(elsePart.ptr);
    NLdecIndent(t);
  }*/

  t.Insert(ENDtry_T);
  EXIT
}

void TryStatementV::InsertBranch (SynObject *&branch, CHAINX *&chx) {
    branch = new CatchClauseV(true);
    chx = &catchClauses;
}

CatchClauseV::CatchClauseV (bool) {
  type = catch_T;
  replacedType = type;
  primaryToken = catch_T;
  exprType.ptr = new SynObjectV(TypePH_T);
  varName.ptr = new SynObjectV(VarPH_T);
  catchClause.ptr = new SynObjectV(Stm_T);
}

void CatchClauseV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(catch_T,true);
  t.Blank();
  DRAW(exprType.ptr);
  t.Blank();
  DRAW(varName.ptr);
  NLincIndent(t);
  DRAW(catchClause.ptr);
  EXIT
}

TypeSwitchStatementV::TypeSwitchStatementV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = type_T;
  caseExpression.ptr = new SynObjectV(Exp_T);
  branches.Append(NewCHE(new TypeBranchV(true)));
  elsePart.ptr = new SynObjectV(Stm_T);
}

void TypeSwitchStatementV::Insert2Optionals (SynObject *&elseP, SynObject*&, CHAINX*&) {
  if (!elsePart.ptr) {
    elseP = new SynObjectV(Stm_T);
    elseP->parentObject = this;
    elseP->whereInParent = (address)&elsePart.ptr;
  }
}

void TypeSwitchStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  CHE *branchPtr;

  ENTRY
  t.Insert(type_T,true);
  t.Blank();
  t.Insert(of_T,true);
  t.Blank();
  DRAW(caseExpression.ptr);
  t.NewLine();
  for (branchPtr = (CHE*)branches.first;
       branchPtr;
       branchPtr = (CHE*)branchPtr->successor) {
    DRAWCHE(branchPtr,&branches);
    t.NewLine();
  }

  if (elsePart.ptr) {
    t.Insert(else_T,false,true);
    NLincIndent(t);
    DRAW(elsePart.ptr);
    NLdecIndent(t);
  }
  t.Insert(ENDtypeof_T);
  EXIT
}

TypeBranchV::TypeBranchV (bool) {
  type = caseType_T;
  replacedType = type;
  primaryToken = caseType_T;
  exprType.ptr = new SynObjectV(TypePH_T);
  varName.ptr = new SynObjectV(VarPH_T);
  thenPart.ptr = new SynObjectV(Stm_T);
}

void TypeBranchV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY
  t.Insert(caseType_T,true);
  t.Blank();
  DRAW(exprType.ptr);
  t.Blank();
  DRAW(varName.ptr);
  t.Insert(Colon_T);
  NLincIndent(t);
  DRAW(thenPart.ptr);
  EXIT
}

AssignmentV::AssignmentV () {
  type = Stm_T;
  replacedType = type;
  primaryToken = assign_T;
}

AssignmentV::AssignmentV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = assign_T;
  exprValue.ptr = new SynObjectV(Exp_T);
  targetObj.ptr = new SynObjectV(ObjPH_T);
}

void AssignmentV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  t.Insert(primaryToken,true);
  t.Blank();
  if (t.leftArrows) {
    DRAW(targetObj.ptr);
    t.Blank();
    t.Insert(Larrow_T);
    t.Blank();
    DRAW(exprValue.ptr);
  }
  else {
    DRAW(exprValue.ptr);
    t.Blank();
    t.Insert(Rarrow_T);
    t.Blank();
    DRAW(targetObj.ptr);
  }

  EXIT
}

ArrayAtIndexV::ArrayAtIndexV (bool) {
  type = Exp_T;
  replacedType = type;
  primaryToken = arrayAtIndex_T;
  arrayObj.ptr = new SynObjectV(Exp_T);
  arrayIndex.ptr = new SynObjectV(Exp_T);
  setCase = false;
}

void ArrayAtIndexV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  DRAW(arrayObj.ptr);
  t.Insert(Lbracket_T,true);
  DRAW(arrayIndex.ptr);
  t.Insert(Rbracket_T);

  EXIT
}

ParameterV::ParameterV (SynObject *param) {
  type = Exp_T;
  replacedType = type;
  primaryToken = parameter_T;
  parameter.ptr = param;
}

void ParameterV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  if (t.parmNames) {
    t.Insert(primaryToken,true);
    t.Insert(Colon_T);
  }
  DRAW(parameter.ptr);
  if (!primaryTokenNode)
    primaryTokenNode = ((SynObject*)parameter.ptr)->primaryTokenNode;

  EXIT
}

FuncExpressionV::FuncExpressionV (bool funcDisabled, bool staticFunc) {
  type = Exp_T;
  replacedType = type;
  primaryToken = assignFX_T;
  if (!staticFunc)
    handle.ptr = new SynObjectV(Exp_T);
  if (funcDisabled)
    function.ptr = new SynObjectV(FuncDisabled_T);
  else
    function.ptr = new SynObjectV(FuncPH_T);
  //inputs.Append(NewCHE(new ParameterV(new SynObjectV(ExpDisabled_T))));
}

FuncExpressionV::FuncExpressionV (ObjReference *ref) {
  type = Exp_T;
  replacedType = type;
  primaryToken = assignFX_T;
  handle.ptr = ref;
  function.ptr = new SynObjectV(FuncPH_T);
  //inputs.Append(NewCHE(new ParameterV(new SynObjectV(ExpDisabled_T))));
}

FuncExpressionV::FuncExpressionV (Reference *ref) {
  type = Exp_T;
  replacedType = type;
  primaryToken = assignFX_T;
  function.ptr = ref;
}

void FuncExpressionV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true/*, parenth*/;
  CHE *paramPtr;

  ENTRY

  if (t.leftArrows) {
    if (handle.ptr) {
      DRAW(handle.ptr);
      t.Insert(Period_T);
    }
    DRAW(function.ptr);
    if (!primaryTokenNode)
      primaryTokenNode = t.currentToken;

    if (inputs.first && flags.Contains(inputArrow)) {
      t.Blank();
      t.Insert(Larrow_T);
      t.Blank();
    }

    if ((inputs.first && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Lparenth_T);

    for (paramPtr = (CHE*)inputs.first;
         paramPtr;
         paramPtr = (CHE*)paramPtr->successor) {
      if (!isFirst)
        t.Insert(Comma_T);
      else
        isFirst = false;
      DRAWCHE(paramPtr,&inputs);
    }
    if ((inputs.first
        && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Rparenth_T);
  }
  else {
    if (flags.Contains(inputArrow)) {
      if (inputs.first != inputs.last)
        t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      if (inputs.first != inputs.last)
        t.Insert(Rparenth_T);

      if (inputs.first) {
        t.Blank();
        t.Insert(Rarrow_T);
        t.Blank();
      }

      if (handle.ptr) {
        DRAW(handle.ptr);
        t.Insert(Period_T);
      }
      DRAW(function.ptr);
      if (!primaryTokenNode)
        primaryTokenNode = t.currentToken;
    }
    else {
      if (handle.ptr) {
        DRAW(handle.ptr);
        t.Insert(Period_T);
      }
      DRAW(function.ptr);
      if (!primaryTokenNode)
        primaryTokenNode = t.currentToken;

      t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      t.Insert(Rparenth_T);
    }
  }

  EXIT
}

FuncStatementV::FuncStatementV (bool funcDisabled, bool out, bool staticFunc) {
  type = Stm_T;
  replacedType = type;
  primaryToken = assignFS_T;
  if (!staticFunc)
    handle.ptr = new SynObjectV(Exp_T);
  if (funcDisabled)
    function.ptr = new SynObjectV(FuncDisabled_T);
  else
    function.ptr = new SynObjectV(FuncPH_T);
//  inputs.Append(NewCHE(new ParameterV(new SynObjectV(ExpDisabled_T))));
  if (out)
    outputs.Append(NewCHE(new ParameterV(new SynObjectV(ObjDisabled_T))));
}

FuncStatementV::FuncStatementV (ObjReference *ref, bool out) {
  type = Stm_T;
  replacedType = type;
  primaryToken = assignFS_T;
  handle.ptr = ref;
  function.ptr = new SynObjectV(FuncPH_T);
  if (out)
    outputs.Append(NewCHE(new ParameterV(new SynObjectV(ObjDisabled_T))));
}

FuncStatementV::FuncStatementV () {
  type = Stm_T;
  replacedType = type;
  primaryToken = assignFS_T;
  function.ptr = new SynObjectV(FuncPH_T);
}

FuncStatementV::FuncStatementV (Reference *ref) {
  type = Stm_T;
  replacedType = type;
  primaryToken = assignFS_T;
  function.ptr = ref;
}

void FuncStatementV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  bool isFirst=true, visibleParms=false;
  CHE *paramPtr;

  ROContext roCtx=ReadOnlyContext();
  bool drawCallKeywd = (roCtx != assertion) 
    && (roCtx != roClause) 
    && (parentObject->primaryToken != connect_T)
    && (parentObject->primaryToken != signal_T);

  ENTRY

  for (paramPtr = (CHE*)outputs.first;
       paramPtr;
       paramPtr = (CHE*)paramPtr->successor) {
    if (!((SynObject*)((Parameter*)paramPtr->data)->parameter.ptr)->flags.Contains(ignoreSynObj))
      visibleParms = true;
  }

  if (drawCallKeywd) {
    t.Insert(assignFS_T,true);
    t.Blank();
  }
  if (t.leftArrows) {
    if (visibleParms) {
      if (outputs.first != outputs.last)
        t.Insert(Lparenth_T);
      isFirst=true;
      for (paramPtr = (CHE*)outputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T);
        else
          isFirst = false;
        if (paramPtr->data)
          DRAWCHE(paramPtr,&outputs);
      }
      if (outputs.first != outputs.last)
        t.Insert(Rparenth_T);

      if (outputs.first) {
        t.Blank();
        t.Insert(Larrow_T);
        t.Blank();
      }
    }

    if (handle.ptr) {
      DRAW(handle.ptr);
      t.Insert(Period_T);
    }
    DRAW(function.ptr);
    if (!primaryTokenNode)
      primaryTokenNode = t.currentToken;

    if (inputs.first
    && flags.Contains(inputArrow)) {
      t.Blank();
      t.Insert(Larrow_T);
      t.Blank();
    }

    if ((inputs.first
        && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Lparenth_T);
    isFirst=true;

    for (paramPtr = (CHE*)inputs.first;
         paramPtr;
         paramPtr = (CHE*)paramPtr->successor) {
      if (!isFirst)
        t.Insert(Comma_T,false);
      else
        isFirst = false;
      DRAWCHE(paramPtr,&inputs);
    }
    if ((inputs.first
        && !flags.Contains(inputArrow))
    || inputs.first != inputs.last)
      t.Insert(Rparenth_T);
  }
  else {
    if (flags.Contains(inputArrow)) {
      if (inputs.first != inputs.last)
        t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T,false);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      if (inputs.first != inputs.last)
        t.Insert(Rparenth_T);

      if (inputs.first) {
        t.Blank();
        t.Insert(Rarrow_T);
        t.Blank();
      }

      if (handle.ptr) {
        DRAW(handle.ptr);
        t.Insert(Period_T);
      }
      DRAW(function.ptr);
      if (!outputs.first)
        primaryTokenNode = t.currentToken;
    }
    else {
      if (handle.ptr) {
        DRAW(handle.ptr);
        t.Insert(Period_T);
      }
      DRAW(function.ptr);
      if (!outputs.first)
        primaryTokenNode = t.currentToken;

      if (inputs.first)
        t.Insert(Lparenth_T);
      for (paramPtr = (CHE*)inputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T,false);
        else
          isFirst = false;
        DRAWCHE(paramPtr,&inputs);
      }
      if (inputs.first)
        t.Insert(Rparenth_T);
    }

    if (visibleParms) {
      if (outputs.first) {
        t.Blank();
        t.Insert(Rarrow_T);
        t.Blank();
      }

      if (outputs.first != outputs.last)
        t.Insert(Lparenth_T);
      isFirst=true;
      for (paramPtr = (CHE*)outputs.first;
           paramPtr;
           paramPtr = (CHE*)paramPtr->successor) {
        if (!isFirst)
          t.Insert(Comma_T,false);
        else
          isFirst = false;
        if (paramPtr->data)
          DRAWCHE(paramPtr,&outputs);
      }
      if (outputs.first != outputs.last)
        t.Insert(Rparenth_T);
    }
  }

  EXIT
}

ConnectV::ConnectV (bool senderClass) {
  type = Stm_T;
  replacedType = type;
  primaryToken = connect_T;
  if (senderClass)
    signalSenderClass.ptr = new SynObjectV(TypePH_T);
  else
    signalSender.ptr = new SynObjectV(Exp_T);
  signalFunction.ptr = new SynObjectV(FuncDisabled_T);
  callback.ptr = new FuncStatementV(true);
}

void ConnectV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  t.Insert(primaryToken,true);
  t.Blank();
  if (signalSender.ptr) {
    DRAW(signalSender.ptr);
    t.Insert(Period_T);
  }
  else {
    DRAW(signalSenderClass.ptr);
//    t.Blank();
    t.Insert(Colon_T);
    t.Insert(Colon_T);
//    t.Blank();
  }
  DRAW(signalFunction.ptr);
  t.Blank();
  t.Insert(to_T);
  t.Blank();
  DRAW(callback.ptr);
  EXIT
}

DisconnectV::DisconnectV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = disconnect_T;
  signalSender.ptr = new SynObjectV(Exp_T);
  signalFunction.ptr = new SynObjectV(FuncDisabled_T);
  signalReceiver.ptr = new SynObjectV(Exp_T);
  callbackFunction.ptr = new SynObjectV(FuncDisabled_T);
}

void DisconnectV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(signalSender.ptr);
//  t.Blank();
  t.Insert(Period_T);
//  t.Blank();
  DRAW(signalFunction.ptr);
  t.Blank();
  t.Insert(from_T);
  t.Blank();
  DRAW(signalReceiver.ptr);
  t.Insert(Period_T);
  DRAW(callbackFunction.ptr);

  EXIT
}

SignalV::SignalV (bool) {
  type = Stm_T;
  replacedType = type;
  primaryToken = signal_T;
}


void SignalV::Draw (CProgTextBase &t,address where,CHAINX *chxp,bool ignored) {
  ENTRY

  t.Insert(primaryToken,true);
  t.Blank();
  DRAW(sCall.ptr);

  EXIT
}


void DrawExec (DString file,bool singleFile,CLavaBaseDoc *doc,LavaDECL *myDECL,SynObject *newObj) {
  SET flags;
  bool mod=true;
  QString selText;
  CProgHTML text;

//  Tokens_INIT();
  text.htmlGen = true;
  text.singleFile = singleFile;
  text.showComments = true;
  if (myDECL->TreeFlags.Contains(leftArrows))
    text.leftArrows = true;
  else
    text.leftArrows = false;
  if (myDECL->TreeFlags.Contains(parmNames))
    text.parmNames = true;
  else
    text.parmNames = false;

  text.document = doc;
  text.ckd.document = doc;
//  text.ckd.currentSynObj = 0;
  text.ckd.myDECL = myDECL;

  text.currentSynObj = newObj->parentObject;
  if (!singleFile) {
    codeNl("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"");
    codeNl("    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
    codeNl("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
    codeNl("<head>");
    codeNl("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"/>");
    codeNl("<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\"/>");
    codeNl("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\"/>");
    codeNl("<meta name=\"GENERATOR\" content=\"Lava HTML Generator\"/>");
    code("<title>");
    code(file.c);
    codeNl("</title>");
    codeNl("<link rel=\"STYLESHEET\" type=\"text/css\" href=\"../LavaIcons/LavaStyles.css\"/>");
    codeNl("</head>");
    codeNl("<body><pre>");
  }
  UpdateParameters(text.ckd);
  newObj->Check(text.ckd);
  newObj->Draw(text,newObj->whereInParent,0,false);
  if (!singleFile)
    codeNl("</pre></body></html>");
  text.insertedChars = 0;
}
