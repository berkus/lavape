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

#include "ConstrUpdate.h"
#include "qstring.h"
#include "Constructs.h"
#include "Syntax.h"
#include "SynIDTable.h"
#include "ExecView.h"
#include "PEBaseDoc.h"
#include "NESTANY.h"
//#include "stdafx.h"

#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()

SynObject *replacedObj, *toBeDrawn;
bool multipleUpdates, insMult=false;


bool CConstrUpdate::ChangeConstraint(CLavaPEHint *hint, wxDocument *doc, bool undo) {
  SynObject **ptr, *parent, *oldParent;
  CHAINX *chx;
  CHE *che, *newChe;
  SynObject *synObj, *oldObj, *minusOpd;
  MultipleOp *multOp;
  SET flags;
  unsigned long flagBits;
  TComment *pCmt, *oldCmt;
  TToken newToken;
  MinusOp *newMinusOp;
  FuncExpression *funcExpr;
  TID tid;

  if (toBeDrawn
  && !insMult
  && !(undo && (InsertMode)hint->CommandData3 == InsMult)
  && !(!undo && (InsertMode)hint->CommandData3 == DelMult))
    multipleUpdates = true;
  insMult = false;

  if (undo)
    switch ((InsertMode)hint->CommandData3) {
    case InsNested:
      goto insNested;

    case InsMult:
      oldObj = (SynObject*)hint->CommandData2;
      multOp = (MultipleOp*)hint->CommandData4;
      synObj = (SynObject*)((CHE*)multOp->operands.first)->data;
      chx = multOp->containingChain;
      if (chx) {
        che = (CHE*)multOp->whereInParent;
        che->data = synObj;
      }
      else {
        ptr = (SynObject**)multOp->whereInParent;
        *ptr = synObj;
      }

      ((CHE*)multOp->operands.first)->data = 0;
      // to prevent synObj to be deleted when this hint is deleted

      synObj->parentObject = multOp->parentObject;
      synObj->whereInParent = multOp->whereInParent;
      synObj->containingChain = multOp->containingChain;
      if (oldObj->IsThrow()) {
        replacedObj = oldObj;
        toBeDrawn = oldObj;
      }
      else
        if (synObj->primaryToken == Minus_T) {
          replacedObj = multOp->parentObject;
          toBeDrawn = synObj->parentObject;
        }
        else {
          replacedObj = multOp;
          toBeDrawn = synObj;
        }
      hint->CommandData3 = DelMult;
      break;

    case InsChain:
      chx = (CHAINX*)hint->CommandData5;
      che = (CHE*)hint->CommandData6;
      //  CommandData4 contains the actual data
      oldObj = (SynObject*)hint->CommandData2;
      replacedObj = oldObj;
      toBeDrawn = oldObj;
      if (che)
        che = (CHE*)che->successor;
      else
        che = (CHE*)chx->first;
      chx->Uncouple(che);
      ((SynObject*)che->data)->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,oldObj,onDeleteID,(address)che,chx);
      hint->CommandData3 = DelChain;
      hint->CommandData4 = (DWORD)che;
      break;

    case DelNested:
      synObj = (SynObject*)hint->CommandData4;
      chx = synObj->containingChain;
      if (chx) {
        che = (CHE*)synObj->whereInParent;
        che->data = synObj;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onUndoDeleteID,(address)che,chx);
      }
      else {
        ptr = (SynObject**)synObj->whereInParent;
        *ptr = synObj;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onUndoDeleteID,(address)ptr,0);
      }
      //synObj->flags.EXCL(subtreeRemoved);

      toBeDrawn = synObj->parentObject;
      replacedObj = synObj->parentObject;
      hint->CommandData3 = ReDelNested;
      break;

    case ReDelNested:
      synObj = (SynObject*)hint->CommandData4;
      chx = synObj->containingChain;
      if (chx) {
        che = (CHE*)synObj->whereInParent;
        che->data = 0;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onDeleteID,(address)che,chx);
      }
      else {
        ptr = (SynObject**)synObj->whereInParent;
        *ptr = 0;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onDeleteID,(address)ptr,0);
      }
      //synObj->flags.INCL(subtreeRemoved);

      toBeDrawn = synObj->parentObject;
      replacedObj = synObj->parentObject;
      hint->CommandData3 = DelNested;
      break;

    case DelMult:
      multOp = (MultipleOp*)hint->CommandData4;
      chx = (CHAINX*)hint->CommandData5;
      oldObj = (SynObject*)hint->CommandData2;
      if (chx) {
        che = (CHE*)hint->CommandData6;
        synObj = (SynObject*)che->data;
        che->data = multOp;
      }
      else {
        ptr = (SynObject**)hint->CommandData6;
        synObj = *ptr;
        *ptr = multOp;
      }
      //multOp->flags.EXCL(removed);
      ((CHE*)multOp->operands.first)->data = synObj;
      multOp->parentObject = oldObj;
      multOp->whereInParent = (address)hint->CommandData6;
      multOp->containingChain = chx;
      synObj->parentObject = multOp;
      synObj->whereInParent = (address)multOp->operands.first;
      synObj->containingChain = &multOp->operands;
      if (oldObj->IsThrow()) {
        replacedObj = oldObj;
        toBeDrawn = oldObj;
      }
      else {
        replacedObj = synObj;
        toBeDrawn = multOp;
      }
      hint->CommandData3 = InsMult;
      break;

    case DelChain:
      synObj = (SynObject*)hint->CommandData2;  // parent
      chx = (CHAINX*)hint->CommandData5;
      che = (CHE*)hint->CommandData6;
      newChe = (CHE*)hint->CommandData4;
      chx->Insert(che,newChe);
      hint->CommandData3 = InsChain;
      ((SynObject*)newChe->data)->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj,onUndoDeleteID,(address)newChe,chx);
      if (!replacedObj) {
        replacedObj = synObj;
        toBeDrawn = synObj;
      }
      break;

    case InsFlags:
      synObj = (SynObject*)hint->CommandData2;
      flagBits = hint->CommandData4;
      flags = SET(flagBits);
      if (flags.Contains(newLine)) {
        parent = synObj->parentObject;
        oldParent = synObj;
        while (parent->startToken == synObj->startToken) {
          oldParent = parent;
          parent = parent->parentObject;
        }
        replacedObj = parent;
        toBeDrawn = parent;
      }
      else if (flags.Contains(ignoreSynObj)
      && synObj->primaryToken == ObjPH_T
      && synObj->parentObject->parentObject->IsFuncInvocation()) {
        replacedObj = synObj->parentObject->parentObject;
        toBeDrawn = synObj->parentObject->parentObject;
      }
      else {
        replacedObj = synObj;
        toBeDrawn = synObj;
      }
      synObj->flags -= flags;
      hint->CommandData3 = DelFlags;
      break;

    case PlusMinus:
      goto plusMinus;

    case DelFlags:
      synObj = (SynObject*)hint->CommandData2;
      flagBits = hint->CommandData4;
      flags = SET(flagBits);
      if (flags.Contains(newLine)) {
        parent = synObj->parentObject;
        oldParent = synObj;
        while (parent->startToken == synObj->startToken) {
          oldParent = parent;
          parent = parent->parentObject;
        }
        replacedObj = parent;
        toBeDrawn = parent;
      }
      else if (flags.Contains(ignoreSynObj)
      && synObj->primaryToken == ObjPH_T
      && synObj->parentObject->parentObject->IsFuncInvocation()) {
        replacedObj = synObj->parentObject->parentObject;
        toBeDrawn = synObj->parentObject->parentObject;
      }
      else {
        replacedObj = synObj;
        toBeDrawn = synObj;
      }
      synObj->flags += flags;
      hint->CommandData3 = InsFlags;
      break;

    case ChgComment:
      synObj = (SynObject*)hint->CommandData2;
      pCmt = (TComment*)hint->CommandData4;
      oldCmt = synObj->comment.ptr;
      if (pCmt && pCmt->str.l)
        synObj->comment.ptr = pCmt;
      else {
        synObj->comment.ptr = 0;
        if (pCmt) delete pCmt;
      }
      hint->CommandData4 = (DWORD)oldCmt;
      replacedObj = synObj;
      toBeDrawn = synObj;
      break;

    case ChgOp:
      synObj = (SynObject*)hint->CommandData2;
      newToken = (TToken)hint->CommandData4;
      hint->CommandData4 = (DWORD)synObj->primaryToken;
      synObj->primaryToken = newToken;
      replacedObj = synObj;
      toBeDrawn = synObj;
      break;
    }
  else
    switch ((InsertMode)hint->CommandData3) {
    case InsNested:
insNested:
      chx = (CHAINX*)hint->CommandData5;
      synObj = (SynObject*)hint->CommandData4;
      if (chx) {
        che = (CHE*)hint->CommandData6;
        oldObj = (SynObject*)che->data;
        hint->CommandData4 = (DWORD)oldObj;
        che->data = synObj;
      }
      else {
        ptr = (SynObject**)hint->CommandData6;
        oldObj = *ptr;
        hint->CommandData4 = (DWORD)oldObj;
        *ptr = synObj;
      }
      if (synObj) {
        synObj->parentObject = (SynObject*)hint->CommandData2;
        synObj->whereInParent = (address)hint->CommandData6;
        synObj->containingChain = chx;
        //synObj->flags.EXCL(subtreeRemoved);
      }
      if (oldObj) {
        if (oldObj->parentObject->IsThrow()
        || oldObj->parentObject->primaryToken == assign_T
        || oldObj->parentObject->primaryToken == clone_T
        || oldObj->parentObject->primaryToken == run_T
        || oldObj->primaryToken == Minus_T
        || (synObj && synObj->primaryToken == Minus_T)) {
          replacedObj = oldObj->parentObject;
          toBeDrawn = replacedObj;
        }
        else if (synObj
        && (synObj->parentObject->primaryToken != quant_T
            || synObj->type != TypePH_T)) {
          replacedObj = oldObj;
          toBeDrawn = synObj;
        }
        else {
          replacedObj = oldObj->parentObject;
          toBeDrawn = replacedObj;
        }

        if (oldObj->IsPlaceHolder() && oldObj->comment.ptr)
          synObj->comment.ptr = (TComment*)oldObj->comment.ptr->Clone();

        switch (oldObj->primaryToken) {
        case VarName_T:
          if (synObj->primaryToken == VarName_T)
            synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onChange,(chx?(address)che:(address)ptr),chx);
          else if (synObj->primaryToken == VarPH_T)
            oldObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onDeleteID,(chx?(address)che:(address)ptr),chx);
          break;
        case VarPH_T:
          if (synObj->primaryToken == VarName_T)
            if (undo)
              synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onUndoDeleteID,(chx?(address)che:(address)ptr),chx);
            else
              synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onNewID,(chx?(address)che:(address)ptr),chx);
          break;
        case ObjPH_T:
        case ObjRef_T:
          if (oldObj->parentObject->IsFuncInvocation()) {
            funcExpr = (FuncExpression*)oldObj->parentObject;

            if (oldObj->whereInParent == (address)&funcExpr->handle.ptr)
              if (oldObj->IsPlaceHolder()) {
                ((SynObject*)funcExpr->function.ptr)->type = FuncPH_T;
                ((SynObject*)funcExpr->function.ptr)->primaryToken = FuncPH_T;
                ((SynObject*)funcExpr->function.ptr)->replacedType = FuncPH_T;
                replacedObj = oldObj->parentObject;
                toBeDrawn = oldObj->parentObject;
              }
              else {
                replacedObj = oldObj->parentObject;
                toBeDrawn = oldObj->parentObject;
              }
          }
          break;
        default:
          oldObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,oldObj->parentObject,onDeleteID,(chx?(address)che:(address)ptr),chx);
          if (synObj)
            if (undo)
              synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onUndoDeleteID,(chx?(address)che:(address)ptr),chx);
            else
              synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onNewID,(chx?(address)che:(address)ptr),chx);
        }
      }
      else {
        replacedObj = (*ptr)->parentObject;
        toBeDrawn = replacedObj;
      }
      break;

    case InsMult:
      insMult = true;
      multOp = (MultipleOp*)hint->CommandData4;
      chx = (CHAINX*)hint->CommandData5;
      oldObj = (SynObject*)hint->CommandData2;
      if (chx) {
        che = (CHE*)hint->CommandData6;
        synObj = (SynObject*)che->data;
        che->data = multOp;
      }
      else {
        ptr = (SynObject**)hint->CommandData6;
        synObj = *ptr;
        *ptr = multOp;
      }
      //multOp->flags.EXCL(removed);
      ((CHE*)multOp->operands.first)->data = synObj;
      multOp->parentObject = oldObj;
      multOp->whereInParent = (address)hint->CommandData6;
      multOp->containingChain = chx;
      synObj->parentObject = multOp;
      synObj->whereInParent = (address)multOp->operands.first;
      synObj->containingChain = &multOp->operands;
      if (oldObj->IsThrow()) {
        replacedObj = oldObj;
        toBeDrawn = oldObj;
      }
      else {
        if (synObj->primaryToken == Minus_T) {
          replacedObj = oldObj;
          toBeDrawn = multOp->parentObject;
        }
        else {
          replacedObj = synObj;
          toBeDrawn = multOp;
        }
      }
      break;

    case InsChain:
      chx = (CHAINX*)hint->CommandData5;
      che = (CHE*)hint->CommandData6;
      oldObj = (SynObject*)hint->CommandData2;
      if (!replacedObj) {
        replacedObj = oldObj;
        toBeDrawn = oldObj;
      }
      newChe = (CHE*)hint->CommandData4;
      synObj = (SynObject*)newChe->data;
      synObj->parentObject = oldObj;
      synObj->containingChain = chx;
      synObj->whereInParent = (address)newChe;
      chx->Insert(che,newChe);
      synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onNewID,(address)newChe,chx);
      break;

    case DelNested:
      synObj = (SynObject*)hint->CommandData4;
      chx = synObj->containingChain;
      if (chx) {
        che = (CHE*)synObj->whereInParent;
        che->data = 0;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onDeleteID,(address)che,chx);
      }
      else {
        ptr = (SynObject**)synObj->whereInParent;
        *ptr = 0;
        synObj->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj->parentObject,onDeleteID,(address)ptr,0);
      }
      //synObj->flags.INCL(subtreeRemoved);

      toBeDrawn = synObj->parentObject;
      replacedObj = toBeDrawn;
      break;

    case DelMult:
      multOp = (MultipleOp*)hint->CommandData4;
      synObj = (SynObject*)((CHE*)multOp->operands.first)->data;
      chx = multOp->containingChain;
      if (chx) {
        che = (CHE*)multOp->whereInParent;
        che->data = synObj;
      }
      else {
        ptr = (SynObject**)multOp->whereInParent;
        *ptr = synObj;
      }
      //multOp->flags.INCL(removed);

      ((CHE*)multOp->operands.first)->data = 0;
      // to prevent synObj to be deleted when this hint is deleted

      synObj->parentObject = multOp->parentObject;
      synObj->whereInParent = multOp->whereInParent;
      synObj->containingChain = multOp->containingChain;
      toBeDrawn = synObj;
      replacedObj = multOp;
      break;

    case DelChain:
      che = (CHE*)hint->CommandData4;
      chx = (CHAINX*)hint->CommandData5;
      synObj = (SynObject*)hint->CommandData2; // parent
      replacedObj = synObj;
      toBeDrawn = synObj;
      hint->CommandData6 = (DWORD)che->predecessor;
      ((SynObject*)che->data)->MakeTable((address)&((CPEBaseDoc*)doc)->IDTable,0,synObj,onDeleteID,(address)che,chx);
      chx->Uncouple(che);
      break;

    case InsFlags:
      synObj = (SynObject*)hint->CommandData2;
      flagBits = hint->CommandData4;
      flags = SET(flagBits);
      if (flags.Contains(newLine)) {
        parent = synObj->parentObject;
        oldParent = synObj;
        while (parent->startToken == synObj->startToken) {
          oldParent = parent;
          parent = parent->parentObject;
        }
        replacedObj = parent;
        toBeDrawn = parent;
      }
      else if (flags.Contains(ignoreSynObj)
      && synObj->primaryToken == ObjPH_T
      && synObj->parentObject->parentObject->IsFuncInvocation()) {
        replacedObj = synObj->parentObject->parentObject;
        toBeDrawn = synObj->parentObject->parentObject;
      }
      else {
        replacedObj = synObj;
        toBeDrawn = synObj;
      }
      synObj->flags += flags;
      break;

    case PlusMinus:
plusMinus:
      synObj = (SynObject*)hint->CommandData4;
      chx = synObj->containingChain;
      if (chx) {
        che = (CHE*)synObj->whereInParent;
        if (synObj->primaryToken == Minus_T) {
          //synObj->flags.INCL(removed);
          minusOpd = (SynObject*)((MinusOp*)synObj)->operand.ptr;
          che->data = minusOpd;
          minusOpd->parentObject = synObj->parentObject;
          minusOpd->whereInParent = synObj->whereInParent;
          minusOpd->containingChain = chx;
          ((MinusOp*)synObj)->operand.ptr = 0;
          hint->CommandData5 = (DWORD)synObj;
          oldObj = minusOpd->parentObject;
          hint->CommandData4 = (DWORD)minusOpd;
        }
        else {
          if (hint->CommandData5)
            newMinusOp = (MinusOp*)hint->CommandData5;
          else
            newMinusOp = new MinusOpV(true);
          newMinusOp->operand.ptr = synObj;
          che->data = newMinusOp;
          newMinusOp->parentObject = synObj->parentObject;
          synObj->parentObject = newMinusOp;
          newMinusOp->whereInParent = synObj->whereInParent;
          synObj->whereInParent = (address)&((MinusOp*)synObj)->operand.ptr;
          synObj->containingChain = 0;
          newMinusOp->containingChain = chx;
          oldObj = newMinusOp->parentObject;
          hint->CommandData4 = (DWORD)newMinusOp;
          hint->CommandData5 = (DWORD)0;
        }
      }
      else {
        ptr = (SynObject**)synObj->whereInParent;
        if (synObj->primaryToken == Minus_T) {
          //synObj->flags.INCL(removed);
          minusOpd = (SynObject*)((MinusOp*)synObj)->operand.ptr;
          *ptr = minusOpd;
          minusOpd->parentObject = synObj->parentObject;
          minusOpd->whereInParent = synObj->whereInParent;
          minusOpd->containingChain = chx;
          ((MinusOp*)synObj)->operand.ptr = 0;
          hint->CommandData5 = (DWORD)synObj;
          oldObj = minusOpd->parentObject;
          hint->CommandData4 = (DWORD)minusOpd;
        }
        else {
          if (hint->CommandData5)
            newMinusOp = (MinusOp*)hint->CommandData5;
          else
            newMinusOp = new MinusOpV(true);
          newMinusOp->operand.ptr = synObj;
          *ptr = newMinusOp;
          newMinusOp->parentObject = synObj->parentObject;
          synObj->parentObject = newMinusOp;
          newMinusOp->whereInParent = synObj->whereInParent;
          synObj->whereInParent = (address)&((MinusOp*)synObj)->operand.ptr;
          synObj->containingChain = 0;
          newMinusOp->containingChain = 0;
          oldObj = newMinusOp->parentObject;
          hint->CommandData4 = (DWORD)newMinusOp;
          hint->CommandData5 = (DWORD)0;
        }
      }
      replacedObj = oldObj;
      toBeDrawn = oldObj;
      break;

    case DelFlags:
      synObj = (SynObject*)hint->CommandData2;
      flagBits = hint->CommandData4;
      flags = SET(flagBits);
      if (flags.Contains(newLine)) {
        parent = synObj->parentObject;
        oldParent = synObj;
        while (parent->startToken == synObj->startToken) {
          oldParent = parent;
          parent = parent->parentObject;
        }
        replacedObj = parent;
        toBeDrawn = parent;
      }
      else if (flags.Contains(ignoreSynObj)
      && synObj->primaryToken == ObjPH_T
      && synObj->parentObject->parentObject->IsFuncInvocation()) {
        replacedObj = synObj->parentObject->parentObject;
        toBeDrawn = synObj->parentObject->parentObject;
      }
      else {
        replacedObj = synObj;
        toBeDrawn = synObj;
      }
      synObj->flags -= flags;
      break;

    case ChgComment:
      synObj = (SynObject*)hint->CommandData2;
      pCmt = (TComment*)hint->CommandData4;
      oldCmt = synObj->comment.ptr;
      if (pCmt && pCmt->str.l)
        synObj->comment.ptr = pCmt;
      else {
        synObj->comment.ptr = 0;
        if (pCmt) delete pCmt;
      }
      hint->CommandData4 = (DWORD)oldCmt;
      replacedObj = synObj;
      toBeDrawn = synObj;
      break;

    case ChgOp:
      synObj = (SynObject*)hint->CommandData2;
      newToken = (TToken)hint->CommandData4;
      hint->CommandData4 = (DWORD)synObj->primaryToken;
      synObj->primaryToken = newToken;
      replacedObj = synObj;
      toBeDrawn = synObj;
      break;
    }

  return true;
}


void CConstrUpdate::DeleteHint(CLavaPEHint *hint) {
  CHE *che;
  TComment *pCmt;
  SynObject *synObj;

  switch ((InsertMode)hint->CommandData3) {
  case InsFlags:
  case DelFlags:
  case DelNested:
  case ReDelNested:
  case InsChain:
  case InsMult:
  case ChgOp:
//  case ToggleArrows:
    break;
  case PlusMinus:
    if (hint->CommandData5)
      delete (SynObject*)hint->CommandData5;
    break;
  case DelChain:
    if (!hint->FirstLast.Contains(impliedExecHint)) {
      che = (CHE*)hint->CommandData4;
      delete che;
    }
    break;
  case ChgComment:
    pCmt = (TComment*)hint->CommandData4;
    delete pCmt;
    break;
    synObj = (SynObject*)hint->CommandData4;
    if (synObj->primaryToken == Minus_T
    && !((MinusOp*)synObj)->operand.ptr)
      delete (SynObject*)hint->CommandData4;
    break;
  default:
    delete (SynObject*)hint->CommandData4;
  }
}


void CConstrUpdate::MakeExec(CPEBaseDoc*, LavaDECL *myDECL) {
  SelfVarV *selfVar;
  LavaDECL *decl, *declName;
  SynObject *body, *execName;
  QString funcName;

  selfVar = new SelfVarV;
  selfVar->type = implementation_T;
  selfVar->containingChain = 0;
  selfVar->parentObject = 0;
  selfVar->stackPos = 0;
  selfVar->execDECL = myDECL;
  selfVar->oldFormParms = 0;
  myDECL->Exec.ptr = selfVar;

  decl = myDECL->ParentDECL;
  declName = decl;
  switch (decl->DeclType) {
  case Function:
    if (decl->TypeFlags.Contains(defaultInitializer))
      selfVar->primaryToken = dftInitializer_T;
    else if (decl->TypeFlags.Contains(isInitializer))
      selfVar->primaryToken = initializer_T;
    else
      selfVar->primaryToken = function_T;
    decl = decl->ParentDECL;
    break;
  case Initiator:
    selfVar->primaryToken = initiator_T;
    break;
  case Interface:
    selfVar->primaryToken = constraint_T;
  }
  if (selfVar->primaryToken == dftInitializer_T
  || selfVar->primaryToken == initializer_T)
    body = new SucceedStatementV();
  else
    body = new SynObjectV(Stm_T);
  body->containingChain = 0;
  body->parentObject = selfVar;
  body->whereInParent = (address)&selfVar->body.ptr;
  selfVar->body.ptr = body;

  funcName = declName->LocalName.c;
  execName = new ReferenceV(FuncPH_T,TID(-1,0),funcName);
    // the final nID is determined later on MakeTable
  execName->containingChain = 0;
  execName->parentObject = selfVar;
  execName->whereInParent = (address)&selfVar->execName.ptr;
  execName->flags.INCL(inExecHdr);
  selfVar->execName.ptr = execName;
  switch (decl->DeclType) {
  case Impl:
  case Interface:
    selfVar->varName = STRING("self");
    break;
  default: ;
  }
  myDECL->TreeFlags.INCL(leftArrows);
  myDECL->TreeFlags.INCL(ShowExecComments);
}


void CConstrUpdate::MakeSetGet(CPEBaseDoc *myDoc, LavaDECL *myDECL, TID &prop, TID &parm) {
  SelfVarV *selfVar;
  SynObject *synObj;
  AssignmentV *assigStm;
  LavaDECL *decl;
  QString funcName, refName;
  TDODC from, to;
  TDODV *tdod;
  ObjReference *objRef;
  CHE *newChe, *chp;
  bool isSetCase;

  if (myDECL->Exec.ptr) return;

  selfVar = new SelfVarV;
  selfVar->type = implementation_T;
  selfVar->containingChain = 0;
  selfVar->parentObject = 0;
  selfVar->primaryToken = function_T;
  myDECL->Exec.ptr = selfVar;

  decl = myDECL->ParentDECL;
  isSetCase = decl->TypeFlags.Contains(isPropSet);
  funcName = decl->FullName.c;
  synObj = new ReferenceV(FuncPH_T,TID(decl->OwnID,0),funcName);
  synObj->containingChain = 0;
  synObj->parentObject = selfVar;
  synObj->whereInParent = (address)&selfVar->execName.ptr;
  synObj->flags.INCL(inExecHdr);
  selfVar->execName.ptr = synObj;
  selfVar->varName = STRING("self");
  decl = decl->ParentDECL;
  selfVar->typeID = TID(decl->OwnID,0);

  assigStm = new AssignmentV(true);
  assigStm->containingChain = 0;
  assigStm->parentObject = selfVar;
  assigStm->whereInParent = (address)&selfVar->body.ptr;
  selfVar->body.ptr = assigStm;
  selfVar->MakeTable((address)&myDoc->IDTable,0,(SynObjectBase*)myDECL,onNewID,0,0);

  if (isSetCase) {
    tdod = new TDODV(true);
    tdod->ID = parm;
    newChe = new CHE(tdod);
    from.Append(newChe);

    tdod = new TDODV(true);
    tdod->ID = selfVar->varID;
    newChe = new CHE(tdod);
    to.Append(newChe);
    tdod = new TDODV(true);
    tdod->ID = prop;
    newChe = new CHE(tdod);
    to.Append(newChe);
  }
  else {
    tdod = new TDODV(true);
    tdod->ID = parm;
    newChe = new CHE(tdod);
    to.Append(newChe);

    tdod = new TDODV(true);
    tdod->ID = selfVar->varID;
    newChe = new CHE(tdod);
    from.Append(newChe);
    tdod = new TDODV(true);
    tdod->ID = prop;
    newChe = new CHE(tdod);
    from.Append(newChe);
  }
  assigStm->exprValue.ptr = new ObjReferenceV(from,refName);
  assigStm->targetObj.ptr = new ObjReferenceV(to,refName);

  objRef = (ObjReference*)assigStm->exprValue.ptr;
  objRef->parentObject = assigStm;
  for (chp = (CHE*)objRef->refIDs.first; chp; chp = (CHE*)chp->successor)
    ((SynObject*)chp->data)->parentObject = objRef;
  if (isSetCase)
    objRef->flags.INCL(isInputVar);
  else {
    objRef->flags.INCL(isSelfVar);
    objRef->flags.INCL(isMemberVar);
  }

  objRef = (ObjReference*)assigStm->targetObj.ptr;
  objRef->parentObject = assigStm;
  objRef->whereInParent = (address)&assigStm->targetObj.ptr;
  for (chp = (CHE*)objRef->refIDs.first; chp; chp = (CHE*)chp->successor)
    ((SynObject*)chp->data)->parentObject = objRef;
  if (isSetCase) {
    objRef->flags.INCL(isSelfVar);
    objRef->flags.INCL(isMemberVar);
  }
  else
    objRef->flags.INCL(isOutputVar);

  myDECL->TreeFlags.INCL(leftArrows);
  myDECL->TreeFlags.INCL(ShowExecComments);
}

