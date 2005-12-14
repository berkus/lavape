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


#include "Check.h"
#include "qstring.h"
#include "Constructs.h"

#include "PEBaseDoc.h"
#ifndef INTERPRETER
#include "ExecUpdate.h"
#include "ExecView.h"
#endif

#include "LavaBaseStringInit.h"
#include "Convert.h"
#include "MACROS.h"


#define ENTRY \
  if (!execView) execView=ckd.execView; \
  bool ok=true; \
  oldError = (CHE*)errorChain.first; \
  errorChain.Destroy(); lastError = 0; \
  if (flags.Contains(ignoreSynObj)) return true;

#define EXIT {return ok;}

/*#define EXIT \
  {ckd.currentSynObj = parentObject; \
  return ok;}*/

#define ERROREXIT {return false;}

/*#define ERROREXIT \
  {ckd.currentSynObj = parentObject; \
  return false;}*/

#define IsPH(PTR) (!PTR || ((SynObject*)PTR)->IsPlaceHolder())
#define NoPH(PTR) (PTR && !((SynObject*)PTR)->IsPlaceHolder())

#define ADJUST(nnn,decl) \
  nnn.nINCL = ckd.document->IDTable.IDTab[decl->inINCL]->nINCLTrans[nnn.nINCL].nINCL

#define ADJUST3(nnn,decl) \
  nnn.nINCL = IDTable->IDTab[decl->inINCL]->nINCLTrans[nnn.nINCL].nINCL

#define ADJUST4(nnn) \
  nnn.nINCL = ckd.document->IDTable.IDTab[ckd.inINCL]->nINCLTrans[nnn.nINCL].nINCL

#define OPERATOR (primaryToken==Ord_T?OP_fis:(TOperator)(primaryToken-not_T))

#ifndef INTERPRETER
CExecUpdate execUpdate;


static void PutInsChainHint (CheckData &ckd,SynObject *func,CHAINX *chx,CHE *newChe,CHE *pred) {
  CLavaPEHint *hint;

  hint = new CLavaPEHint(
    CPECommand_Exec,
    ckd.document,
    SET(impliedExecHint,-1),
    (DWORD)ckd.myDECL,
    (DWORD)func,
    (DWORD)InsChain,
    (DWORD)newChe,
    (DWORD)chx,
    (DWORD)pred);
  if (ckd.concernExecs) {
    ((CPEBaseDoc*)ckd.document)->UndoMem.AddToMem(hint);
    ((CPEBaseDoc*)ckd.document)->UpdateDoc(0,false,hint);
  }
  else
    execUpdate.ChangeExec(hint,ckd.document,false);
}

static void PutDelChainHint (CheckData &ckd,SynObject *func,CHAINX *chx,CHE *che) {
  CLavaPEHint *hint;

  hint = new CLavaPEHint(
    CPECommand_Exec,
    ckd.document,
    SET(impliedExecHint,-1),
    (DWORD)ckd.myDECL,
    (DWORD)func,
    (DWORD)DelChain,
    (DWORD)che,
    (DWORD)chx,
    (DWORD)che->predecessor);
  if (ckd.concernExecs) {
    ((CPEBaseDoc*)ckd.document)->UndoMem.AddToMem(hint);
    ((CPEBaseDoc*)ckd.document)->UpdateDoc(0,false,hint);
  }
  else
    execUpdate.ChangeExec(hint,ckd.document,false);
}
#endif


QString SynObject::LocationOfConstruct () {
  SynObject *synObj=this;
  QString cSynObjName, varName, path;
  bool first = true;
  CHE *chp;

  do {
    if (synObj->primaryToken == VarName_T)
      cSynObjName = QString(((VarName*)synObj)->varName.c);
    else if (synObj->primaryToken == TDOD_T) {
      cSynObjName = QString(((TDOD*)synObj)->name.c);
      chp = (CHE*)((CHE*)synObj->whereInParent)->predecessor;
      while (chp) {
        synObj = (SynObject*)chp->data;
        cSynObjName = QString(((TDOD*)synObj)->name.c) + "." + cSynObjName;
        chp = (CHE*)chp->predecessor;
      }
      synObj = synObj->parentObject;
    }
    else if (synObj->primaryToken == ObjRef_T)
      cSynObjName = QString(((ObjReference*)synObj)->refName.c);
    else if (synObj->primaryToken == TypeRef_T
    || synObj->primaryToken == CrtblRef_T)
      cSynObjName = QString(((Reference*)synObj)->refName.c);
    else if (synObj->primaryToken == Boolean_T)
      cSynObjName = ((BoolConst*)synObj)->boolValue?QString("true"):QString("false");
    else if (synObj->primaryToken == Const_T)
      cSynObjName = QString(((Constant*)synObj)->str.c);
    else if (synObj->primaryToken == enumConst_T)
      cSynObjName = QString(((EnumConst*)synObj)->Id.c);
    else if (synObj->primaryToken == nil_T)
      cSynObjName = QString("nil");
    else if (synObj->primaryToken == FuncRef_T) {
      if (synObj->parentObject->type == implementation_T)
        cSynObjName = QString("(name of function/main program/invariant)");
    }
    else if (synObj->IsFuncInvocation())
      if (((SynObject*)((FuncExpression*)synObj)->function.ptr)->primaryToken == FuncPH_T
      || ((SynObject*)((FuncExpression*)synObj)->function.ptr)->primaryToken == FuncDisabled_T)
        cSynObjName = QString("<func>()");
      else
        cSynObjName =
          QString(((Reference*)((FuncExpression*)synObj)->function.ptr)->refName.c) + "()";
    else if (synObj->primaryToken == FormParm_T) {
      if (((ObjReference*)((FormParm*)synObj)->formParm.ptr)->flags.Contains(isInputVar))
        cSynObjName = QString(TOKENSTR[inputs_T]);
      else
        cSynObjName = QString(TOKENSTR[outputs_T]);
      synObj = synObj->parentObject;
    }
    else if (synObj->primaryToken != FuncPH_T)
      cSynObjName = QString(TOKENSTR[synObj->primaryToken]);

    if (synObj->parentObject->primaryToken == parameter_T
    || synObj->parentObject->IsUnaryOp()
    || ((synObj->parentObject->IsMultOp()
        || synObj->parentObject->IsBinaryOp())
       && synObj->parentObject->type != Stm_T))
      cSynObjName = QString("(") + cSynObjName + ")";

    if (first/* && synObj->primaryToken != FuncRef_T*/) {
      path = cSynObjName;
      first = false;
    }
    else if (path.length())
      path = cSynObjName + " | " + path;
    else
      path = cSynObjName;

    synObj = synObj->parentObject;
    if (synObj->primaryToken == parameter_T)
      synObj = synObj->parentObject;
  } while (synObj->parentObject);

  return path;
}


void SynObject::SetError(CheckData &ckd,QString *errorCode,char *textParam)
{
  SynObject *synObj=this;
  DString dFileName = ckd.document->GetAbsSynFileName();
  QString cFileName(dFileName.c), cExecName;
  QString msg, msgText;

  if (IsFuncInvocation())
    synObj = (SynObject*)((FuncExpression*)synObj)->function.ptr;
  else if (synObj->primaryToken == parameter_T)
    synObj = (SynObject*)((Parameter*)synObj)->parameter.ptr;
  else if (synObj->primaryToken == qua_T)
    synObj = (SynObject*)((ExtendExpression*)synObj)->extendType.ptr;
  else if (synObj->primaryToken == ifx_T)
    synObj = (SynObject*)((CHE*)((IfExpression*)synObj)->ifThens.first)->data;

  if (synObj->primaryToken == ObjRef_T)
    synObj = (SynObject*)((CHE*)((ObjReference*)synObj)->refIDs.last)->data;

#ifdef INTERPRETER
  msgText = *errorCode;
  if (textParam)
    msgText = QString(textParam) + msgText;

  if (ckd.myDECL->ParentDECL->DeclType == Interface)
    cExecName = QString("invariant of ") + ckd.myDECL->ParentDECL->FullName.c;
  else
    cExecName = ckd.myDECL->ParentDECL->FullName.c;
  if (synObj->type == implementation_T)
    msg = msgText + " in " + cFileName + "::" + cExecName;
  else
    msg = msgText + "\n\nFile: " + cFileName + "\n\nPath to erroneous construct:\n\n" + cExecName + " | " + synObj->LocationOfConstruct();

  if (SetLavaException(ckd, check_ex, msg))
    throw CUserException();
  else
    throw CNotSupportedException();
#else
  if (((SelfVar*)ckd.selfVar)->execView)
    new CLavaError(&synObj->errorChain,errorCode,new DString(textParam));
  ckd.nErrors++;
  synObj->lastError = errorCode;
#endif
}


bool RefTable::assigned (CheckData &ckd,
                         CObjRefTable &oRefTbl,
                         CHE *che) {
  CHECVarDesc *cheTbl;
  CVarDesc *varDesc;
  TID tid;

  tid = ((TDOD*)che->data)->ID;
  ADJUST4(tid);
  for (cheTbl = (CHECVarDesc*)oRefTbl.first; cheTbl; cheTbl = (CHECVarDesc*)cheTbl->successor)
    if (cheTbl->data.varID == tid) break;

  if (cheTbl) { // found: end or recursion
    varDesc = &cheTbl->data;
    if (che->successor)
      return assigned(ckd,varDesc->subTree,(CHE*)che->successor);
    else
      return varDesc->writeAccess;
  }
  else // not found
    return false;
}


void RefTable::findObjRef (CheckData &ckd,
                           CObjRefTable &oRefTbl,
                           CVarDesc *oldVarDesc,
                           CVarDesc *&newVarDesc,
                           CHE *che) {
  CHECVarDesc *cheTbl;
  CVarDesc *varDesc;
  CObjRefTable *ort;
  ObjReference *objRef=(ObjReference*)((TDOD*)che->data)->parentObject;
  CWriteAccess *newWA;
  CHE *newCHEWA;
  TID tid;
  CHE *chp;

  for (cheTbl = (CHECVarDesc*)oRefTbl.first; cheTbl; cheTbl = (CHECVarDesc*)cheTbl->successor) {
    tid = ((TDOD*)che->data)->ID;
    ADJUST4(tid);
    if (cheTbl->data.varID == tid) break;
  }

  if (cheTbl) { // found: end or recursion
    varDesc = &cheTbl->data;
    if (che->successor)
      findObjRef(ckd,varDesc->subTree,varDesc,newVarDesc,(CHE*)che->successor);
    else if (!ckd.iniCheck) {
      newWA = new CWriteAccess;
      newWA->objRef = objRef;
      newWA->varDesc = newVarDesc = &cheTbl->data;
      newCHEWA = new CHE(newWA);
      ((RefTable*)ckd.refTable)->refTableEntries.Append(newCHEWA);
    }
    else
      newVarDesc = varDesc;
  }
  else { // not found
    ort = &oRefTbl;
    for (chp=che; chp; chp = (CHE*)chp->successor) {
      cheTbl = new CHECVarDesc;
      cheTbl->data.varID = ((TDOD*)chp->data)->ID;
      ADJUST4(cheTbl->data.varID);
      cheTbl->data.parent = oldVarDesc;
      cheTbl->data.writeAccess = false;
      if (!chp->successor && !ckd.iniCheck) {
        newWA = new CWriteAccess;
        newWA->objRef = objRef;
        newWA->varDesc = newVarDesc = &cheTbl->data;
        newCHEWA = new CHE(newWA);
        ((RefTable*)ckd.refTable)->refTableEntries.Append(newCHEWA);
      }
      ort->Append(cheTbl);
      ort = &cheTbl->data.subTree;
      oldVarDesc = &cheTbl->data;
    }
  }
}

bool CWriteAccess::Contains (ObjReference *objRef) {
// checks if the statement of the write access (assignment or func. stmt.)
// contains objRef. If yes, then this write acc. sets the value of objRef
// after the read acc. to objRef. In other words: objRef counts as still undefined
  SynObject *stm, *myStm=this->objRef->parentObject;

  for (stm = objRef->parentObject; stm; stm = stm->parentObject)
    if (stm == myStm) return true;
  return false;
}

QString *RefTable::findMatchingAccess (
      CheckData &ckd,
      CHE *&chp,
      CVarDesc *refEntry,
      bool nestedCall,
      bool &isAssigned,
      ObjReference *objRef,
      bool backward) {

  QString *errorCode=ckd.iniCheck?&ERR_NotYetInitialized:0;
  unsigned totalBranches=1, iniBranches=0;
  bool alreadyIni=false, iniInBrStm;
  CWriteAccess *writeAcc;
  CVarDesc *currVarDesc;

  do {
    if (((CRefEntry*)chp->data)->IsWriteAccess()) {
      writeAcc = (CWriteAccess*)chp->data;
      if (writeAcc->varDesc->writeAccess) {
        if (ckd.iniCheck) {
          if (writeAcc->varDesc == refEntry
          && !writeAcc->Contains(objRef))
            if (nestedCall) {
              if (!alreadyIni) {
                alreadyIni = true;
                iniBranches++;
              }
            }
            else
              return 0;
        }
        else {
          if (writeAcc->varDesc == refEntry) {
            objRef->conflictingAssig = writeAcc->objRef;
            return &ERR_SingleAssViol;
          }
          else {
            for (currVarDesc = writeAcc->varDesc->parent;
                 currVarDesc;
                 currVarDesc = currVarDesc->parent)
              if (currVarDesc == refEntry) {
                objRef->conflictingAssig = writeAcc->objRef;
                return &ERR_PrevDescAssig;
              }
          }
        }
      }
    }
    else if (((CRefEntry*)chp->data)->IsEndBranchStm()) {
      if (chp->predecessor && ((CRefEntry*)((CHE*)chp->predecessor)->data)->IsFailSucceed())
        totalBranches--;
        
      while (chp->predecessor && ((CRefEntry*)((CHE*)chp->predecessor)->data)->IsFailSucceed())
        chp = ((CBranch*)chp->data)->precedingBranch;

      if (((CRefEntry*)chp->data)->IsBranchStm())
        continue;
      if (backward)
        chp = (CHE*)chp->predecessor;
      else
        chp = (CHE*)chp->successor;
      if (errorCode = findMatchingAccess(ckd,chp,refEntry,true,iniInBrStm,objRef))
        return errorCode;
      if (ckd.iniCheck)
        if (iniInBrStm)
          if (nestedCall) {
            if (!alreadyIni) {
              alreadyIni = true;
              iniBranches++;
              if (iniBranches < totalBranches)
                return &ERR_NotYetInitialized;
            }
          }
          else {
            isAssigned = true;
            return 0;
          }
    }
    else if (((CRefEntry*)chp->data)->IsBranchStm()) {
      if (nestedCall) {
        if (ckd.iniCheck)
          isAssigned = (iniBranches == totalBranches);
        return 0;
      }
    }
    else if (((CRefEntry*)chp->data)->IsBranch()) {
      if (nestedCall) {
        if (((CRefEntry*)((CHE*)chp->predecessor)->data)->IsFailSucceed()) {
          chp = ((CBranch*)chp->data)->precedingBranch;
          continue;
        }
        else if (ckd.iniCheck) {
          totalBranches++;
          alreadyIni = false;
        }
      }
      else
        chp = ((CBranch*)chp->data)->branchStm;
    }
    if (backward)
      chp = (CHE*)chp->predecessor;
    else
      chp = (CHE*)chp->successor;
  } while (chp);

  if (ckd.iniCheck && !alreadyIni)
    return &ERR_NotYetInitialized;
  else
    return 0;
}

QString *RefTable::AssignCheck (CheckData &ckd, ObjReference *objRef) {
  QString *errorCode=0;
  CVarDesc *newEntry;
  CHE *chp;
  bool isAssigned;

  ckd.iniCheck = false;
  findObjRef(ckd,objRefTable,0,newEntry,(CHE*)objRef->refIDs.first);
  chp = (CHE*)refTableEntries.last->predecessor;
  if (chp)
    errorCode = findMatchingAccess(ckd,chp,newEntry,false,isAssigned,objRef);
  newEntry->writeAccess = true;
  return errorCode;
}

QString *RefTable::ReadCheck (CheckData &ckd, ObjReference *objRef) {
  QString *errorCode=0;
  CVarDesc *entryFound=0;
  CHE *chp;
  bool isAssigned;

  ckd.iniCheck = true;
  chp = (CHE*)refTableEntries.last;
  if (chp)
    findObjRef(ckd,objRefTable,0,entryFound,(CHE*)objRef->refIDs.first);
  if (!entryFound)
    errorCode = &ERR_NotYetInitialized;
  else
    errorCode = findMatchingAccess(ckd,chp,entryFound,false,isAssigned,objRef);
  return errorCode;
}


void RefTable::NewBranchStm (CHE *&branchStm,CHE *&precedingBranch) {
  CBranchStm *newBr=new CBranchStm;

  branchStm = new CHE(newBr);
  precedingBranch = branchStm;
  newBr->branchStm = branchStm;
  refTableEntries.Append(branchStm);
}

void RefTable::NewBranch (CHE *branchStm,CHE *&precedingBranch) {
  precedingBranch = new CHE(new CBranch(branchStm,precedingBranch));
  refTableEntries.Append(precedingBranch);
}

void RefTable::EndBranchStm (CHE *branchStm,CHE *precedingBranch) {
  precedingBranch = new CHE(new CEndBranchStm(branchStm,precedingBranch));
  refTableEntries.Append(precedingBranch);
}

void RefTable::EndOfExec () {
  refTableEntries.Destroy();
}


bool SelfVar::IsEmptyExec() {
  return ((SynObject*)body.ptr)->IsPlaceHolder();
}

bool sameType(CheckData &ckd, const TID &t1, const TID &t2)
{
  TID tbase2, tid1, tid2;
  LavaDECL  *decl1=ckd.document->IDTable.GetDECL(t1),
                  *decl2=ckd.document->IDTable.GetDECL(t2);

  if (!decl1 || !decl2)
    return true;

  if (decl1 == decl2)
    return true;
  else
    return false;
}

bool compatibleContext(CheckData &ckd, LavaDECL* decl2, const CContext &context1, const CContext &context2)
{ // compatibleContext returns false if the result of compatibleTypes
  // must be false for types decl1 and decl2
  // although decl1 and decl2 are equal or decl1 is c-derived from decl2
  CContext con;
  ckd.document->IDTable.GetPattern(decl2, con);
//  if (!decl2->WorkFlags.Contains(isPartOfPattern))
  if (!con.oContext || (con.oContext == decl2))
    return true;
  if (!context2.oContext && (decl2->DeclType == VirtualType)
      || (context1.oContext != context2.oContext)
      || (context1.ContextFlags.Contains(selfoContext) != context2.ContextFlags.Contains(selfoContext))
         && (context1.ContextFlags.Contains(staticContext) != context2.ContextFlags.Contains(staticContext)))
    return false;
  if (context2.iContext
      && ckd.document->IDTable.IsAn(context2.iContext,
                                    TID(con.oContext->OwnID, con.oContext->inINCL),0)) {
    if ((context1.iContext != context2.iContext)
       || (context1.ContextFlags.Contains(selfiContext) != context2.ContextFlags.Contains(selfiContext))
         && (context1.ContextFlags.Contains(staticContext) != context2.ContextFlags.Contains(staticContext)))
      return false;
  }
  return true;
}


bool compatibleTypes(CheckData &ckd, LavaDECL *decl1, const CContext &context1, LavaDECL *decl2, const CContext &context2)
{
  //decl1 right hand side
  if (!decl1 || !decl2)
    return true;
  SynFlags ctxFlags1 = context1.ContextFlags;
  SynFlags ctxFlags2 = context2.ContextFlags;
  if (decl1->TypeFlags.Contains(substitutable))
    ctxFlags1 = SET(multiContext,-1);
  if (decl2->TypeFlags.Contains(substitutable))
    ctxFlags2 = SET(multiContext,-1);
  ckd.errorCode = &ERR_IncompatibleType;
  if (ctxFlags2.Contains(undefContext) && context1.oContext)
    return false;
  if (ctxFlags1.Contains(multiContext) && !ctxFlags2.Contains(multiContext))
    return false;
  if ((decl2->OwnID == ckd.document->IDTable.BasicTypesID[B_Object]) && ((ckd.document->isStd?0:1) == decl2->inINCL))
    return true;
  if ((decl1->DeclType != VirtualType) && (decl2->DeclType != VirtualType)) {
//    ckd.errorCode = &ERR_IncompatibleType;
    if (ctxFlags2.Contains(multiContext)
      && ckd.document->IDTable.IsAn(TID(decl1->OwnID, decl1->inINCL),0,TID(decl2->OwnID, decl2->inINCL),0))
      return true;
    if ((decl1 != decl2) && !ckd.document->IsCDerivation(decl1,decl2,&ckd))
      return false;
    return compatibleContext(ckd, decl2, context1, context2);
  }
  else
    if ((decl1->DeclType == VirtualType) && (decl2->DeclType == VirtualType)) {
      ckd.errorCode = &ERR_IncompWithTargetVT;
      if (decl1 == decl2)
        return compatibleContext(ckd, decl2, context1, context2);
      else
        return false;
    }
    else {
      if (decl2->DeclType == VirtualType) {
        ckd.errorCode = &ERR_IncompatibleType;
        if (!ctxFlags2.Contains(staticContext) && !ctxFlags2.Contains(multiContext))
          return false;
        if (decl2->TypeFlags.Contains(isAbstract))
          return compatibleContext(ckd, decl2, context1, context2);
        decl2 = ckd.document->IDTable.GetDECL(decl2->RefID,decl2->inINCL);
      }
      else {  //(decl1->DeclType == VirtualType)
        ckd.errorCode = &ERR_IncompWithTargetVT;
        if (!ctxFlags1.Contains(staticContext) && !ctxFlags2.Contains(multiContext))
          return false;
        if (decl1->TypeFlags.Contains(isAbstract)) {
          ckd.errorCode = &ERR_IncompAbstractVT;
          return false;
        }
        decl1 = ckd.document->IDTable.GetDECL(decl1->RefID,decl1->inINCL);
      }
      if ((decl1 != decl2)  && !ckd.document->IsCDerivation(decl1,decl2,&ckd))
        return false;
      if (ctxFlags2.Contains(multiContext)
        && ckd.document->IDTable.IsAn(TID(decl1->OwnID, decl1->inINCL),0,TID(decl2->OwnID, decl2->inINCL),0))
        return true;
      else
        return !decl2 || compatibleContext(ckd, decl2, context1, context2);
    }
}

bool compatibleInput(CheckData &ckd, CHE *actParm, CHE *formParm, const CContext &callCtx, Category callObjCat)
{
  SynObject *actSynObj=(SynObject*)actParm->data;
  SynObject *parm=(actSynObj->primaryToken==parameter_T?
    (SynObject*)((Parameter*)actSynObj)->parameter.ptr : actSynObj);
  LavaDECL *actTypeDecl, *actDecl, *formDecl, *formTypeDecl;
  bool ok=true;
  Category actCat, formCat;
  CContext callContext=callCtx;
  SynFlags ctxFlags;

  parm->ExprGetFVType(ckd,actTypeDecl,actCat,ctxFlags);
  if (actTypeDecl == (LavaDECL*)-1)
    if (parm->NullAdmissible(ckd))
      return true;
    else { // "nothing" admissible?
      parm->SetError(ckd,&ERR_NotOptional);
      return false;
    }
  actDecl = ckd.document->GetType(actTypeDecl);
  formDecl = (LavaDECL*)formParm->data;
  ckd.document->MemberTypeContext(formDecl, callContext,&ckd);
  callContext = callCtx;
  formTypeDecl = ckd.document->GetFinalMVType(formDecl->RefID,formDecl->inINCL,callContext,formCat,&ckd);
  callContext = callCtx;
  if (formCat == unknownCategory
  && formDecl->TypeFlags.Contains(trueObjCat))
    if (formDecl->TypeFlags.Contains(stateObject))
      formCat = stateObj;
    else if (formDecl->TypeFlags.Contains(sameAsSelf))
      formCat = sameAsSelfObj;
    else
      formCat = valueObj;
  if (NoPH(parm))
    ((Expression*)parm)->targetCat = formCat;
  if (actDecl == 0 && !parm->IsIfStmExpr())
    return false; // but actTypeDecl == -1 ("nothing") is ok!

  if (formDecl->TypeFlags.Contains(substitutable))
    callContext.ContextFlags = SET(multiContext,-1);
  if (ctxFlags.bits) // of actParm???
    ckd.tempCtx.ContextFlags = ctxFlags;
  if (parm->IsIfStmExpr()) {
    ((CondExpression*)parm)->targetDecl = formTypeDecl;
    ((CondExpression*)parm)->targetCtx = ckd.tempCtx;
    ((CondExpression*)parm)->targetCat = formCat;
    ((CondExpression*)parm)->callObjCat = callObjCat;
    ok &= parm->Check(ckd);
  }
  else {
    if (!compatibleTypes(ckd,actTypeDecl,ckd.tempCtx,formTypeDecl,callContext)) {
      parm->SetError(ckd,ckd.errorCode);
      ok = false;
    }

    if (actCat != unknownCategory
    && ((formCat == sameAsSelfObj && actCat != callObjCat)
        || (formCat != sameAsSelfObj && actCat != formCat))) {
      parm->SetError(ckd,&ERR_IncompatibleCategory);
      ok = false;
    }
  }
  return ok;
}

bool compatibleOutput(CheckData &ckd, CHE *actParm, CHE *formParm, const CContext &callCtx, Category callObjCat)
{
  SynObject *actSynObj=(SynObject*)actParm->data;
  TID formTID;
  LavaDECL *actTypeDecl, *formDecl, *formTypeDecl;
  bool ok=true;
  Category actCat, formCat;
  CContext callContext=callCtx;
  SynFlags ctxFlags;

  actSynObj->ExprGetFVType(ckd,actTypeDecl,actCat,ctxFlags);
  if (actTypeDecl == (LavaDECL*)-1) // null always admissible as output target
    return true;
  if (!actTypeDecl)
    return false;
  formDecl = (LavaDECL*)formParm->data;
  ckd.document->MemberTypeContext(formDecl, callContext,&ckd);
  formTID = formDecl->RefID;
  formTypeDecl = ckd.document->GetFinalMVType(formTID,formDecl->inINCL,callContext,formCat,&ckd);
  if (formDecl->TypeFlags.Contains(substitutable))
    callContext.ContextFlags = SET(multiContext,-1);
  if (ctxFlags.bits)
    ckd.tempCtx.ContextFlags = ctxFlags;
  if (!compatibleTypes(ckd,formTypeDecl,callContext,actTypeDecl,ckd.tempCtx)) {
    if (actSynObj->primaryToken == parameter_T)
      ((SynObject*)((Parameter*)actSynObj)->parameter.ptr)->SetError(ckd,ckd.errorCode);
    else
      actSynObj->SetError(ckd,ckd.errorCode);
    ok = false;
  }

  if (formCat == unknownCategory
  && formDecl->TypeFlags.Contains(trueObjCat))
    if (formDecl->TypeFlags.Contains(stateObject))
      formCat = stateObj;
    else if (formDecl->TypeFlags.Contains(sameAsSelf))
      formCat = sameAsSelfObj;
    else
      formCat = valueObj;

  if (actCat != unknownCategory
  && ((formCat == sameAsSelfObj && actCat != callObjCat)
      || (formCat != sameAsSelfObj && actCat != formCat))) {
    if (actSynObj->primaryToken == parameter_T)
      ((SynObject*)((Parameter*)actSynObj)->parameter.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
    else
      actSynObj->SetError(ckd,&ERR_IncompatibleCategory);
    ok = false;
  }
  return ok;
}


bool slotFunction(CheckData &ckd, LavaDECL *callbackDecl, const CContext &callbackCtx, LavaDECL *signalDecl, const CContext &signalCtx) {


  CHE *chSlot, *chSignal;
  LavaDECL *formParmTypeSlot, *formParmTypeSignal;
  Category cat, catS;
  CContext sigCtx;
  unsigned iArg=1;

  if (GetFirstOutput(callbackDecl)) {
    ckd.errorCode = &ERR_SlotHasOutput;
    return false;
  }
  chSlot = GetFirstInput(callbackDecl);
  chSignal = GetFirstInput(signalDecl);
  while (chSlot && chSignal 
        && (((LavaDECL*)chSlot->data)->DeclType == IAttr)
        && (((LavaDECL*)chSignal->data)->DeclType == IAttr)) {
    ckd.tempCtx = callbackCtx;
    sigCtx = signalCtx;
    ckd.document->MemberTypeContext((LavaDECL*)chSlot->data, ckd.tempCtx,&ckd);
    ckd.document->MemberTypeContext((LavaDECL*)chSignal->data, sigCtx,&ckd);
    formParmTypeSlot = ckd.document->GetFinalMVType(((LavaDECL*)chSlot->data)->RefID,((LavaDECL*)chSlot->data)->inINCL,ckd.tempCtx,cat,&ckd);
    formParmTypeSignal = ckd.document->GetFinalMVType(((LavaDECL*)chSignal->data)->RefID,((LavaDECL*)chSignal->data)->inINCL,sigCtx,catS,&ckd);
    if (((LavaDECL*)chSlot->data)->TypeFlags.Contains(substitutable))
      ckd.tempCtx.ContextFlags = SET(multiContext,-1);  //?? stimmt der Context
    if (((LavaDECL*)chSignal->data)->TypeFlags.Contains(substitutable))
      sigCtx.ContextFlags = SET(multiContext,-1);  //?? stimmt der Context
    if (!compatibleTypes(ckd,formParmTypeSignal,sigCtx,formParmTypeSlot,ckd.tempCtx)) {
      ckd.errorCode = &ERR_SlotWrongArg;
      ckd.iArg = iArg;
      return false;
    }
    chSlot = (CHE*)chSlot->successor;
    chSignal = (CHE*)chSignal->successor;
    iArg++;
    if (chSlot && ((LavaDECL*)chSlot->data)->DeclType != IAttr) 
      chSlot = 0;
    if (chSignal && ((LavaDECL*)chSignal->data)->DeclType != IAttr) 
      chSignal = 0;
  }
  if (chSignal) {
    ckd.errorCode = &ERR_SlotTooFewArgs;
    return false;
  }
  else if (chSlot) {
    ckd.errorCode = &ERR_SlotTooManyArgs;
    return false;
  }
  else
    return true;
}

unsigned GetNumInputs(TIDTable *idt, const TID &tid)
{
  LavaDECL *decl;

  decl = idt->GetDECL(tid);
  if (decl)
    return decl->nInput;
  else
    return 0;
}

CHE* GetFirstInput(LavaDECL *funcDecl)
{
  CHE *chpParam;

  if (!funcDecl) return 0;
  chpParam = (CHE*)funcDecl->NestedDecls.first;
  if (chpParam && ((LavaDECL*)chpParam->data)->DeclType == IAttr)
    return chpParam;
  else
    return 0;
}

CHE* GetFirstInput(TIDTable *idt, const TID &tid)
{
  CHE *chpParam;
  LavaDECL *decl;

  decl = idt->GetDECL(tid);
  if (!decl) return 0;
  chpParam = (CHE*)decl->NestedDecls.first;
  if (chpParam && ((LavaDECL*)chpParam->data)->DeclType == IAttr)
    return chpParam;
  else
    return 0;
}

unsigned GetNumOutputs(TIDTable *idt, const TID &tid)
{
  LavaDECL *decl;

  decl = idt->GetDECL(tid);
  if (decl)
    return decl->nOutput;
  else
    return 0;
}


CHE *GetFirstOutput(LavaDECL *funcDecl)
{
  CHE *chpParam;

  if (!funcDecl) return 0;
  chpParam = (CHE*)funcDecl->NestedDecls.first;
  while (chpParam && ((LavaDECL*)chpParam->data)->DeclType == IAttr) {
    chpParam = (CHE*)chpParam->successor;
  }
  if (chpParam && ((LavaDECL*)chpParam->data)->DeclType == OAttr)
    return chpParam;
  else
    return 0;
}


CHE *GetFirstOutput(TIDTable *idt, const TID &tid)
{
  CHE *chpParam;
  LavaDECL *decl;

  decl = idt->GetDECL(tid);
  if (!decl) return 0;
  chpParam = (CHE*)decl->NestedDecls.first;
  while (chpParam && ((LavaDECL*)chpParam->data)->DeclType == IAttr) {
    chpParam = (CHE*)chpParam->successor;
  }
  if (chpParam && ((LavaDECL*)chpParam->data)->DeclType == OAttr)
    return chpParam;
  else
    return 0;
}

bool SynObject::UpdateReference (CheckData &ckd) {
  bool ok=true;
  ObjReference *objRef;
//  SynObject *synObj;
  DWORD dw;
  LavaDECL *startDecl=0, *decl, *oldDecl=0, *declVID, *startDeclV;
  TIDType idtype;
  Category cat;
  CHE *che;
  TDOD *tdod;
  TID tdodID, tid;
  DString nINCLstring, nIDstring, strINCL, strID;
  SynFlags ctxFlags;
  CHEEnumSelId* enumsel;
  unsigned iItem=0;

  switch (primaryToken) {

  case ObjRef_T:
    objRef = (ObjReference*)this;
    objRef->flags.EXCL(brokenRef);
    che = (CHE*)objRef->refIDs.first;
    tdod = (TDOD*)che->data;
    tdod->execView=ckd.execView;
    tdodID = tdod->ID;
    ADJUST4(tdodID);
    ((TDOD*)che->data)->oldError = (CHE*)((TDOD*)che->data)->errorChain.first;
    ((TDOD*)che->data)->errorChain.Destroy();
    ((TDOD*)che->data)->lastError = 0;
    objRef->myFinalVType = 0;
    dw = ckd.document->IDTable.GetVar(tdodID,idtype);
    if (!dw) {
//#ifndef INTERPRETER
      if (!objRef->refName.l) {
        Convert.IntToString(tdodID.nINCL, strINCL);
        Convert.IntToString(tdodID.nID, strID);
        objRef->refName = "("+strID+","+strINCL+")";
        ((TDOD*)che->data)->name = objRef->refName;
      }
//#endif
      ((TDOD*)che->data)->SetError(ckd,&ERR_Broken_ref);
      ok = false;
      objRef->flags.INCL(brokenRef);
    }
    else if (idtype == localID) {
      ((TDOD*)che->data)->whereInParent = (address)che;
//#ifndef INTERPRETER
      objRef->refName = ((VarName*)dw)->varName;
      ((TDOD*)che->data)->name = objRef->refName;
//#endif
      if (objRef->OutOfScope(ckd)) {
        if (objRef->flags.Contains(isSelfVar)
        && ckd.myDECL->ParentDECL->DeclType == Function
        && !ckd.myDECL->ParentDECL->TypeFlags.Contains(isStatic))
          ((TDOD*)che->data)->ID = ((SelfVar*)ckd.selfVar)->varID;
        else if (!objRef->flags.Contains(isSelfVar)) {
          LocalVarByNameSearch lvbns(((TDOD*)che->data)->name);
          lvbns.CheckLocalScope(ckd,objRef);
          if (lvbns.found)
            ((TDOD*)che->data)->ID = lvbns.varName->varID;
		      else
			      ((SynObject*)((CHE*)objRef->refIDs.first)->data)->SetError(ckd,&ERR_OutOfScope);
        }
        else {
          ok = false;
          objRef->flags.INCL(brokenRef);
          if(objRef->flags.Contains(isSelfVar)
		      && ckd.myDECL->ParentDECL->TypeFlags.Contains(isStatic))
            ((SynObject*)((CHE*)objRef->refIDs.first)->data)->SetError(ckd,&ERR_SelfInStatic);
		      else
			      ((SynObject*)((CHE*)objRef->refIDs.first)->data)->SetError(ckd,&ERR_OutOfScope);
        }
      }
      if (ok) {
        if (((VarName*)dw)->flags.Contains(isOptionalExpr))
          ((TDOD*)che->data)->flags.INCL(isOptionalExpr);
        else
          ((TDOD*)che->data)->flags.EXCL(isOptionalExpr);
        ((TDOD*)che->data)->parentObject = objRef;
        ((VarName*)dw)->ExprGetFVType(ckd,startDeclV,cat,ctxFlags);
        if (objRef->flags.Contains(isSelfVar))
          startDecl = ckd.selfTypeDECL; // static self-type
        else
          startDecl = startDeclV; // final virtual self-type
        if (!startDecl) {
          ((TDOD*)che->data)->SetError(ckd,&ERR_UndefType);
          ok = false;
          objRef->flags.INCL(brokenRef);
          objRef->myFinalVType = 0;
          break;
        }
        ((TDOD*)che->data)->context = ckd.tempCtx;
        if (!che->successor) {
          objRef->myContext = ckd.tempCtx;
          if (ctxFlags.Contains(multiContext))
            objRef->flags.INCL(isSubstitutable);
          objRef->myFinalVType = startDeclV;
          objRef->myCategory = cat;
          if (((VarName*)dw)->flags.Contains(isOptionalExpr))
            objRef->flags.INCL(isOptionalExpr);
          else
            objRef->flags.EXCL(isOptionalExpr);
        }
        che = (CHE*)che->successor;
        if (che)
          if (!VerifyObj(ckd,che,objRef->refName,objRef,startDecl)) {
            objRef->flags.INCL(brokenRef);
            ok = false;
          }
      }
    }
    else { // global var.
      decl = *(LavaDECL**)dw;
      if ((decl->DeclType == IAttr || decl->DeclType == OAttr)
      && decl->ParentDECL != ckd.myDECL->ParentDECL
      && !tdod->ReplaceWithLocalParm(ckd,ckd.myDECL->ParentDECL,decl->DeclType)) {
        tdod->SetError(ckd,&ERR_FuncParmOutOfScope);
        ERROREXIT
      }
      if (decl->DeclType == IAttr) {
        //parmDecl = ckd.document->IDTable.GetDECL(tdod->ID,ckd.inINCL);
        decl->WorkFlags.INCL(isReferenced);
      }
      objRef->refName.Reset(0);
      if (decl->TypeFlags.Contains(isOptional)) {
        objRef->flags.INCL(isOptionalExpr);
        ((TDOD*)che->data)->flags.INCL(isOptional);
      }
      else {
        objRef->flags.EXCL(isOptionalExpr);
        ((TDOD*)che->data)->flags.EXCL(isOptionalExpr);
      }
      if (!VerifyObj(ckd,che,objRef->refName,objRef)) {
        objRef->flags.INCL(brokenRef);
        ok = false;
      }
     }

#ifndef INTERPRETER
    if (!objRef->flags.Contains(brokenRef)) {
      che = (CHE*)objRef->refIDs.last;
      tdodID = ((TDOD*)che->data)->ID;
      ADJUST4(tdodID);
      dw = ckd.document->IDTable.GetVar(tdodID,idtype);
      if (idtype == globalID) {
        decl = *(LavaDECL**)dw;
        if (decl && decl->DeclType == Attr
        && ((TDOD*)((CHE*)((CHE*)objRef->refIDs.last)->predecessor)->data)->IsStateObject(ckd))
          objRef->flags.INCL(isStateObjMember);
        else
          objRef->flags.EXCL(isStateObjMember);
      }
    }

#else
    if (idtype == localID)
      objRef->stackPos = ((VarName*)dw)->stackPos;
    else {
      decl = *(LavaDECL**)dw;
      if (decl->DeclType == IAttr)
        objRef->stackPos = decl->nInput+1+SFH;
      else // output parameter
        objRef->stackPos = decl->nInput+1+SFH;
    }
  objRef->finalType = ckd.document->GetType(objRef->myFinalVType);
#endif
    break;

  case TypeRef_T:
  case CrtblRef_T:
  case FuncRef_T:
  case enumConst_T:
    if (primaryToken == enumConst_T) {
      decl = ckd.document->IDTable.GetDECL(((EnumConst*)this)->refID,ckd.inINCL);
#ifdef INTERPRETER
      ((EnumConst*)this)->refDecl = decl;
      ((EnumConst*)this)->finalType = ckd.document->GetType(decl);
#endif
    }
    else {
      decl = ckd.document->IDTable.GetDECL(((Reference*)this)->refID,ckd.inINCL);
      ((Reference*)this)->refDecl = decl;
    }
    if (decl) {
      if ((primaryToken == TypeRef_T || primaryToken == CrtblRef_T)
      && parentObject->primaryToken != initializing_T) {
        if (decl->DeclType != VirtualType && ckd.document->TypeForMem(ckd.myDECL->ParentDECL,decl,&ckd)) {
          SetError(ckd,&ERR_UseVT);
          ok = false;
        }
      }
      if (primaryToken == FuncRef_T && parentObject && parentObject->IsFuncInvocation())
        if (parentObject->parentObject)  //to exclude execName of a invariant
          if (((FuncExpression*)parentObject)->flags.Contains(staticCall)
          || !((FuncExpression*)parentObject)->handle.ptr) {
            if (((FuncExpression*)parentObject)->vtypeID.nID != -1) {
              declVID = ckd.document->IDTable.GetDECL(((FuncExpression*)parentObject)->vtypeID,ckd.inINCL);
              if (declVID)
                ((Reference*)this)->refName = "<" + declVID->FullName + ">::" + decl->LocalName;
              else {
                Convert.CardToString(((FuncExpression*)parentObject)->vtypeID.nINCL,nINCLstring);
                Convert.CardToString(((FuncExpression*)parentObject)->vtypeID.nID,nIDstring);
                ((Reference*)this)->refName
                  = "<(" + nIDstring + "," + nINCLstring + ")>::" + decl->LocalName;
                SetError(ckd,&ERR_Broken_ref);
                ok = false;
              }
            }
            else
              ((Reference*)this)->refName = decl->FullName;
          }
          else
            ((Reference*)this)->refName = decl->LocalName;
        else { // execName
          ((Reference*)this)->refName = decl->LocalName;
        }
      else if (primaryToken == enumConst_T) {
        for (enumsel =
          (CHEEnumSelId*)((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Items.first;
          enumsel;
          enumsel = (CHEEnumSelId*)enumsel->successor) {
          if (enumsel->data.Id == ((EnumConst*)this)->Id) {
#ifdef INTERPRETER
            ((EnumConst*)this)->enumItem = iItem;
#endif
            break;
          }
          else
            iItem++;
        }
        if (enumsel) break;

#ifdef INTERPRETER
        SetError(ckd,&ERR_Broken_ref);
        ok = false;
#else
        if (ckd.hint
        && ckd.hint->com == CPECommand_Change
        && ckd.hint->CommandData5
        && *(LavaDECL**)ckd.hint->CommandData4 == decl) {
          if (ckd.concernExecs
          || ckd.undoRedo != 1 ) // do/redo
            if (((EnumConst*)this)->Id == *((DString*)ckd.hint->CommandData6))
              ((EnumConst*)this)->Id = *((DString*)ckd.hint->CommandData5);
            else {
              SetError(ckd,&ERR_Broken_ref);
              ok = false;
            }
          else // undo
            if (((EnumConst*)this)->Id == *((DString*)ckd.hint->CommandData5))
              ((EnumConst*)this)->Id = *((DString*)ckd.hint->CommandData6);
            else {
              SetError(ckd,&ERR_Broken_ref);
              ok = false;
            }
        }
        else {
          SetError(ckd,&ERR_Broken_ref);
          ok = false;
        }
#endif
      }
      else if (decl->DeclType == VirtualType)
        ((Reference*)this)->refName = "<" + decl->FullName + ">";
      else if (parentObject->IsFuncInvocation())
        ((Reference*)this)->refName = decl->FullName;
      else // [dis]connect
        if ((whereInParent == (address)&((Disconnect*)parentObject)->signalFunction.ptr
             && (!((Disconnect*)parentObject)->signalSender.ptr
                 || ((SynObject*)((Disconnect*)parentObject)->signalSender.ptr)->primaryToken == nil_T))
            || (whereInParent == (address)&((Disconnect*)parentObject)->callbackFunction.ptr
               && ((SynObject*)((Disconnect*)parentObject)->signalReceiver.ptr)->primaryToken == nil_T))
          ((Reference*)this)->refName = decl->FullName;
        else
          ((Reference*)this)->refName = decl->LocalName;
    }
    else {
      if (primaryToken == FuncRef_T
      && !parentObject->parentObject) {
        ((Reference*)this)->refID = TID(((SelfVar*)parentObject)->execDECL->OwnID,0);
        ((Reference*)this)->refName = ((SelfVar*)parentObject)->execDECL->FullName;
        break;
      }
      if (!((Reference*)this)->refName.l) {
        Convert.CardToString(((Reference*)this)->refID.nINCL,nINCLstring);
        Convert.CardToString(((Reference*)this)->refID.nID,nIDstring);
        ((Reference*)this)->refName = "(" + nIDstring + "," + nINCLstring + ")";
      }
      SetError(ckd,&ERR_Broken_ref);
      ok = false;
    }
    break;

  case TDOD_T:
    decl = ckd.document->IDTable.GetDECL(((TDOD*)this)->ID,ckd.inINCL);
    if (decl) {
      ((TDOD*)this)->name = decl->LocalName;
      if (decl->TypeFlags.Contains(isOptional))
        ((TDOD*)this)->flags.INCL(isOptionalExpr);
      else
        ((TDOD*)this)->flags.EXCL(isOptionalExpr);
    }
    else {
      if (!((TDOD*)this)->name.l) {
        Convert.CardToString(((TDOD*)this)->ID.nINCL,nINCLstring);
        Convert.CardToString(((TDOD*)this)->ID.nID,nIDstring);
        ((TDOD*)this)->name = "(" + nIDstring + "," + nINCLstring + ")";
      }
      SetError(ckd,&ERR_Broken_ref);
      ok = false;
    }
    break;

#ifdef INTERPRETER
  case ObjPH_T:
  case ObjDisabled_T:
  case SetPH_T:
  case SetAttribPH_T:
  case TypePH_T:
  case FuncPH_T:
  case FuncDisabled_T:
    SetError(ckd,&ERR_Placeholder);
    ok = false;
    break;
#endif

  default: ;
  }

  return ok;
}


bool SynObject::Check (CheckData &ckd)
{
  ENTRY
#ifdef INTERPRETER
  SetError(ckd,&ERR_Placeholder);
#endif
//  if (!flags.Contains(isDisabled))
    ckd.nPlaceholders++;
  ERROREXIT
}

void MultipleOp::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  TID tidOperatorFunc;
  LavaDECL *declOutparm1;
  CHE *chpOutparm1;
#ifndef INTERPRETER
  LavaDECL *declOp1;
  Category cat1;
#endif

  decl = 0;
  cat = unknownCategory;

#ifdef INTERPRETER
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    return;
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc);
#else
  ((SynObject*)((CHE*)operands.first)->data)->ExprGetFVType(ckd,declOp1,cat1,ctxFlags);
  declOp1 = ckd.document->GetType(declOp1);
  if (declOp1)
    if (!ckd.document->GetOperatorID(declOp1,OPERATOR,tidOperatorFunc))
      return;
    else {
#endif
      chpOutparm1 = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
      if (chpOutparm1) {
        declOutparm1 = (LavaDECL*)chpOutparm1->data;
        if (!declOutparm1)
          return;
        if (declOutparm1->TypeFlags.Contains(isOptional))
          flags.INCL(isOptionalExpr);
        else
          flags.EXCL(isOptionalExpr);
        ckd.tempCtx = callCtx;
        if (ctxFlags.bits)
          ckd.tempCtx.ContextFlags = ctxFlags;
        decl = ckd.document->GetFinalMVType(declOutparm1->RefID,declOutparm1->inINCL,ckd.tempCtx,cat,&ckd);
        if (declOutparm1->TypeFlags.Contains(substitutable))
          ctxFlags = SET(multiContext,-1);
        else
          ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
        if (cat == unknownCategory
        && declOutparm1->TypeFlags.Contains(trueObjCat))
          if (declOutparm1->TypeFlags.Contains(stateObject))
            cat = stateObj;
          else if (declOutparm1->TypeFlags.Contains(sameAsSelf))
            cat = sameAsSelfObj;
          else
            cat = valueObj;
      }
#ifndef INTERPRETER
    }
#else
  finalType = ckd.document->GetType(decl);
#endif
}

bool MultipleOp::Check (CheckData &ckd)
{
  CHE *chp, *chpActIn, *chpFormIn, *chpFormOut, *branchStm, *precedingBranch;
  SynObject *opd, *opd1;
  TID tidOp1, tidOperatorFunc, tidOutparm1;
  LavaDECL *declOp1, *formInParmDecl;
  Category cat, callObjCat;
  SET oldFlags=ckd.flags;
  SynFlags ctxFlags;
  bool isStm, opd1ok=true;
#ifdef INTERPRETER
  LavaDECL *actDecl, *formDecl;
  TID actTid, formTid;
#endif

  ENTRY

  isStm = IsStatement();

  if (primaryToken == or_T)
    ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);

  if (primaryToken == xor_T)
    ckd.flags.INCL(InXor);
  for (chp = (CHE*)operands.first;
       chp;
       chp = (CHE*)chp->successor) {
    if (primaryToken == or_T) {
      if (chp != (CHE*)operands.first)
        ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    }
    opd = (SynObject*)chp->data;
    if (isStm || !opd->IsIfStmExpr()) {
      ok &= opd->Check(ckd);
      if (chp == (CHE*)operands.first)
        opd1ok = ok;
    }

  }

  if (primaryToken == or_T) {
    ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);
    ckd.flags = oldFlags;
  }

  if (isStm) return ok;

  opd1 = (SynObject*)((CHE*)operands.first)->data;

  if (IsPH(opd1))
    ERROREXIT

/*  if (opd1->IsIfStmExpr()) {
    opd1->SetError(ckd,&ERR_IfxForbidden);
    ERROREXIT
  }*/

  if (!opd1ok)
    ERROREXIT

  ckd.tempCtx = ckd.lpc;
  opd1->ExprGetFVType(ckd,declOp1,cat,ctxFlags);
  if (!declOp1) {
    opd1->SetError(ckd,&ERR_UndefType);
    ERROREXIT
  }
  if (declOp1->TypeFlags.Contains(isAbstract)) {
    SetError(ckd,&ERR_OperatorUndefined);
    ERROREXIT
  }
  declOp1 = ckd.document->GetTypeAndContext(declOp1,ckd.tempCtx);
  callCtx = ckd.tempCtx;
  ckd.document->NextContext(declOp1, callCtx);
  callObjCat = cat;

#ifdef INTERPRETER
  if (!declOp1)
    SetError(ckd,&ERR_UndefType);
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    SetError(ckd,&ERR_OperatorUndefined);
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc);
  if (!funcDecl)
    SetError(ckd,&ERR_OperatorUndefined);
  funcSectionNumber = ckd.document->GetMemsSectionNumber(ckd, declOp1,funcDecl);
#else
  if (!declOp1)
    ERROREXIT
  if (!ckd.document->GetOperatorID(declOp1,OPERATOR,tidOperatorFunc)) {
    SetError(ckd,&ERR_OperatorUndefined);
    ERROREXIT
  }
  else
    opFunctionID = tidOperatorFunc;
#endif

  chpActIn = (CHE*)operands.first;
  chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);

  chpFormOut = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
  if (chpFormOut)
    if (((LavaDECL*)chpFormOut->data)->TypeFlags.Contains(isOptional))
      flags.INCL(isOptionalExpr);
    else
      flags.EXCL(isOptionalExpr);

#ifdef INTERPRETER
  formDecl = (LavaDECL*)chpFormIn->data;
  formVType = ckd.document->IDTable.GetDECL(formDecl->RefID,formDecl->inINCL);
  vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, formVType, isOuter);
  ckd.tempCtx = callCtx;
  if (ctxFlags.bits)
    ckd.tempCtx.ContextFlags = ctxFlags;
  formDecl = ckd.document->GetFinalMTypeAndContext(formDecl->RefID,formDecl->inINCL,ckd.tempCtx,&ckd);
#endif
  CContext callContext = callCtx;
  if (ctxFlags.bits)
    callContext.ContextFlags = ctxFlags;
  while (chpActIn) {
  //if (!((SynObject*)chpActIn->data)->IsIfStmExpr()) {
      if (chpActIn->predecessor)
        compatibleInput(ckd,chpActIn,chpFormIn,callContext,callObjCat);
      opd = (SynObject*)chpActIn->data;
      formInParmDecl = (LavaDECL*)chpFormIn->data;
      if (opd->IsOptional(ckd)
      && !formInParmDecl->TypeFlags.Contains(isOptional)
      && !opd->IsDefChecked(ckd)) {
        ((SynObject*)opd)->SetError(ckd,&ERR_NotOptional);
        ok = false;
      }
#ifdef INTERPRETER
      ((SynObject*)chpActIn->data)->ExprGetFVType(ckd,actDecl,cat,ctxFlags);
      actDecl = ckd.document->GetType(actDecl);
      ((Expression*)chpActIn->data)->sectionNumber = ckd.document->GetSectionNumber(ckd, actDecl,formDecl);
#endif
  //}
    chpActIn = (CHE*)chpActIn->successor;
  }
  EXIT
}

VarAction::VarAction () {
  inIgnore = false;
  inExecHeader = false;
  inFormParms = false;
  inBaseInits = false;
  inParameter = false;
  inForeach = false;
  tempNo = 0;
}

void VarAction::CheckLocalScope (CheckData &ckd, SynObject *synObj)
{
  CHE *chp, *varPtr;
  Quantifier *quant, *stopQuant=0;
  VarName *varName;
  Reference *elemTypePtr;
  SynObject *synObjOrig=synObj;
  SelfVar *selfVar;
  TID tid, typeID;
  LavaDECL *decl;
  bool inForeachNew;
  Category cat;
  SynFlags ctxFlags;

  ckd.criticalScope=false;
  if (synObj->primaryToken == ObjRef_T
  && synObj->parentObject->primaryToken == quant_T)
    stopQuant = (Quantifier*)synObj->parentObject;

  if (synObj->parentObject && synObj->parentObject->primaryToken == Handle_T)
    ckd.handleOpd = true;
  else
    ckd.handleOpd=false;

  for ( ; synObj; synObj = synObj->parentObject) {
    inForeachNew = false;
    if (synObj->primaryToken == parameter_T
    && ((FuncExpression*)synObj->parentObject)->handle.ptr
    && ((SynObject*)((FuncExpression*)synObj->parentObject)->handle.ptr)->flags.Contains(isDisabled)
    && synObj->containingChain == (CHAINX*)&((FuncExpression*)synObj->parentObject)->inputs)
      // initializer input parameters must not reference "temp"
      ckd.criticalScope = false;//true;

    if (synObj->flags.Contains(inExecHdr))
      inExecHeader = true;
    if (synObj->flags.Contains(ignoreSynObj))
      inIgnore = true;

    switch (synObj->primaryToken) {
    case quant_T:
      ckd.criticalScope = false;
      break;
    case foreach_T:
      inForeach = true;
      inForeachNew = true;
    case declare_T:
    case exists_T:
    case select_T:
      if (synObj == synObjOrig)
        break;
      ckd.inQuant = (synObj->primaryToken != declare_T);
      for ( chp = (CHE*)((QuantStmOrExp*)synObj)->quantifiers.last;
            chp;
            chp = (CHE*)chp->predecessor) {
        quant = (Quantifier*)chp->data;
        if (quant == stopQuant) {
          stopQuant = 0;
          break;
        }
        for ( varPtr = (CHE*)quant->quantVars.last;
              varPtr;
              varPtr = (CHE*)varPtr->predecessor) {
          if (((SynObject*)varPtr->data)->primaryToken == VarName_T) {
            varName = (VarName*)varPtr->data;
            if (quant->elemType.ptr) {
              if (((SynObject*)quant->elemType.ptr)->primaryToken == TypeRef_T) {
                elemTypePtr = (Reference*)quant->elemType.ptr;
                typeID = elemTypePtr->refID;
                ADJUST4(typeID);
              }
              else
                typeID.nID = -1;
              if (Action(ckd,varName,typeID)) {
                if (inForeachNew)
                  inForeach = false;
                return;
              }
            }
            else if (NoPH(quant->set.ptr)) {
              ((SynObject*)quant->set.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
              if (decl)
                if (((SynObject*)quant->set.ptr)->primaryToken == intIntv_T) {
									tid = OWNID(decl);
                  if (Action(ckd,varName,tid)) {
                    if (inForeachNew)
                      inForeach = false;
                    return;
                  }
                }
                else if (((SynObject*)quant->set.ptr)->primaryToken == TypeRef_T) { // enumeration type
									tid = OWNID(decl);
									if (Action(ckd,varName,tid)) {
                    if (inForeachNew)
                      inForeach = false;
                    return;
                  }
                }
                else {
                  doc->IDTable.GetParamID(decl,tid,isSet);
                  if (Action(ckd,varName,tid)) {
                    if (inForeachNew)
                      inForeach = false;
                    return;
                  }
                }
            }
          }
        }
      }
      break;
    case caseType_T:
      if (((SynObject*)((TypeBranch*)synObj)->varName.ptr)->primaryToken == VarName_T) {
        varName = (VarName*)((TypeBranch*)synObj)->varName.ptr;
        if (((SynObject*)((TypeBranch*)synObj)->exprType.ptr)->primaryToken == TypeRef_T) {
          elemTypePtr = (Reference*)((TypeBranch*)synObj)->exprType.ptr;
          typeID = elemTypePtr->refID;
          ADJUST4(typeID);
          if (Action(ckd,varName,typeID)) return;
        }
      }
      break;
    case catch_T:
      if (((SynObject*)((CatchClause*)synObj)->varName.ptr)->primaryToken == VarName_T) {
        varName = (VarName*)((CatchClause*)synObj)->varName.ptr;
        if (((SynObject*)((CatchClause*)synObj)->exprType.ptr)->primaryToken == TypeRef_T) {
          elemTypePtr = (Reference*)((CatchClause*)synObj)->exprType.ptr;
          typeID = elemTypePtr->refID;
          ADJUST4(typeID);
          if (Action(ckd,varName,typeID)) return;
        }
      }
      break;
    case new_T:
      if (!((NewExpression*)synObj)->itf.ptr) {
        tempNo++;
        varName = (VarName*)((NewExpression*)synObj)->varName.ptr;
        elemTypePtr = (Reference*)((NewExpression*)synObj)->objType.ptr;
        if (NoPH(varName)) {
          typeID = elemTypePtr->refID;
          ADJUST4(typeID);
          if (Action(ckd,varName,typeID)) return;
        }
      }
      ckd.criticalScope = false;
      break;
    case clone_T:
      tempNo++;
      if (((CloneExpression*)synObj)->butStatement.ptr) {
        varName = (VarName*)((CloneExpression*)synObj)->varName.ptr;
        ((ObjReference*)((CloneExpression*)synObj)->fromObj.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
        decl = ckd.document->GetType(decl);
        if (decl) {
					tid = OWNID(decl);
          if (Action(ckd,varName,tid)) return;
				}
      }
      break;
    case FormParms_T:
      inFormParms = true;
      break;
    case initializing_T:
      inBaseInits = true;
      break;
    case parameter_T:
      inParameter = true;
      break;
    case function_T:
    case initializer_T:
    case dftInitializer_T:
    case invariant_T:
    case require_T:
    case ensure_T:
      selfVar = (SelfVar*)synObj;
      if (!selfVar->execDECL->ParentDECL->TypeFlags.Contains(isStatic)) {
        typeID = selfVar->typeID;
        ADJUST4(typeID);
        if (Action(ckd,selfVar,typeID)) return;
      }
      break;
    default: ;
    }
  }
}


//#ifndef INTERPRETER

void UpdateParameters (CheckData &ckd) {
  FormParms *fp;
  SelfVar *selfVar=(SelfVar*)ckd.myDECL->Exec.ptr;
  LavaDECL *decl=ckd.myDECL->ParentDECL;

  if (ckd.myDECL->DeclType == Require
  || ckd.myDECL->DeclType == Ensure
  || decl->DeclType == Interface)
    return;

  if (decl->DeclType == Function)
    if (decl->TypeFlags.Contains(defaultInitializer))
      selfVar->primaryToken = dftInitializer_T;
    else if (decl->TypeFlags.Contains(isInitializer))
      selfVar->primaryToken = initializer_T;
    else
      selfVar->primaryToken = function_T;

  selfVar->oldFormParms = (FormParms*)selfVar->formParms.ptr;
#ifdef INTERPRETER
  fp = new FormParms(selfVar);
#else
  fp = new FormParmsV(selfVar);
#endif
  selfVar->formParms.ptr = fp;
//  ckd.currentSynObj = selfVar;
  ((FormParms*)selfVar->formParms.ptr)->Check(ckd);
  if (!fp->inputs.first && !fp->outputs.first) {
    delete fp;
    selfVar->formParms.ptr = 0;
  }
}

static void harmonize (CheckData &ckd, CHAINX &chain, CHE *parmDef, CHE *&parmRef, ExecFlags ioFlag) {
  LavaDECL *decl=(LavaDECL*)parmDef->data;
  TID parmDefTid(decl->OwnID,0);
  FormParm *newParmRef;
  TDOD *tdod;
  TDODC tdodc;
  ObjReference *objRef;

//  ADJUST(parmDefTid,decl);

#ifdef INTERPRETER
  newParmRef = new FormParm(true);
  newParmRef->parmType.ptr = new Reference(TypePH_T,decl->RefID,"");
#else
  newParmRef = new FormParmV(true);
  newParmRef->parmType.ptr = new ReferenceV(TypePH_T,decl->RefID,"");
#endif
  if (decl->TypeFlags.Contains(substitutable))
    ((SynObject*)newParmRef->parmType.ptr)->flags.INCL(isSubstitutable);
  if (decl->TypeFlags.Contains(stateObject))
    ((SynObject*)newParmRef->parmType.ptr)->flags.INCL(isVariable);
  else if (decl->TypeFlags.Contains(sameAsSelf))
    ((SynObject*)newParmRef->parmType.ptr)->flags.INCL(isSameAsSelf);
#ifdef INTERPRETER
  tdod = new TDOD;
#else
  tdod = new TDODV(true);
#endif
  tdod->ID = parmDefTid;
  tdodc.Append(new CHE(tdod));
#ifdef INTERPRETER
  newParmRef->formParm.ptr = objRef = new ObjReference(tdodc,"");
#else
  newParmRef->formParm.ptr = objRef = new ObjReferenceV(tdodc,"");
#endif
  ((TDOD*)((CHE*)objRef->refIDs.first)->data)->parentObject = objRef;
  objRef->parentObject = newParmRef;
  objRef->flags.INCL(ioFlag);
  objRef->UpdateReference(ckd);
  newParmRef->formParmID = parmDefTid;
  parmRef = new CHE(newParmRef);
  chain.Append(parmRef);
}
//#endif

bool FormParms::Check (CheckData &ckd)
{
  SelfVar *selfVar=(SelfVar*)parentObject;
  CHE *chpParmRef, *chpParmDef;
//#ifndef INTERPRETER
  TDOD *tdod;
  LavaDECL *parmDecl;
//#endif
  TID rID=((Reference*)selfVar->execName.ptr)->refID;

  ENTRY

  ADJUST4(rID);
//#ifndef INTERPRETER
  chpParmDef = GetFirstInput(&ckd.document->IDTable,rID);
  while (chpParmDef && ((LavaDECL*)chpParmDef->data)->DeclType == IAttr) {
    // locate old parm. and reposition it if necessary:
    harmonize(ckd,inputs,chpParmDef,chpParmRef,isInputVar);
    tdod = (TDOD*)((CHE*)((ObjReference*)((FormParm*)chpParmRef->data)->formParm.ptr)->refIDs.first)->data;
    parmDecl = ckd.document->IDTable.GetDECL(tdod->ID,ckd.inINCL);
    parmDecl->WorkFlags.EXCL(isReferenced);
    chpParmDef = (CHE*)chpParmDef->successor;
  }
//#endif

  chpParmDef = GetFirstOutput(&ckd.document->IDTable,rID);
  if (chpParmDef && ((LavaDECL*)chpParmDef->data)->DeclType == OAttr) {
    while (chpParmDef && ((LavaDECL*)chpParmDef->data)->DeclType == OAttr) {
      // locate old parm. and reposition it if necessary:
      harmonize(ckd,outputs,chpParmDef,chpParmRef,isOutputVar);
      chpParmDef = (CHE*)chpParmDef->successor;
    }
  }
  EXIT
}

bool BaseInit::Check (CheckData &ckd)
{
#ifndef INTERPRETER
  ObjReferenceV *objRef;
  TDODV *tdod;
  TDODC tdodc;
  CHE *newChe;
#endif

  ENTRY
  ((SynObject*)baseItf.ptr)->Check(ckd);
  if (initializerCall.ptr)
    ((SynObject*)initializerCall.ptr)->Check(ckd);
  else if (self.ptr) {
#ifndef INTERPRETER
  ((Reference*)baseItf2.ptr)->refName = ((Reference*)baseItf.ptr)->refName;
#endif
  }
  else {
#ifndef INTERPRETER
    tdod = new TDODV(true);
    tdod->name = STRING("self");
    tdod->ID = ((SelfVar*)ckd.selfVar)->varID;
    newChe = new CHE(tdod);
    tdodc.Append(newChe);
    objRef = new ObjReferenceV(tdodc,"self");
    tdod->parentObject = objRef;
    objRef->flags.INCL(isDisabled);
    objRef->flags.INCL(isSelfVar);
    self.ptr = objRef;
    baseItf2.ptr = baseItf.ptr->Clone();
#endif
  }
  EXIT
}

void SelfVar::ExprGetFVType (CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags)
{
  cat = unknownCategory;
  ctxFlags.bits = 0;
  decl = selfType;
  if (!decl) return;

  ckd.tempCtx = selfCtx;
  if (decl->DeclType == VirtualType
  && decl->TypeFlags.Contains(definesObjCat)
  && decl->TypeFlags.Contains(trueObjCat))
    if (decl->TypeFlags.Contains(stateObject))
      cat = stateObj;
    else
      cat = valueObj;
}

static void reposBaseInits (CheckData &ckd,SelfVar *selfVar,CHAINX *chain,LavaDECL *formBase,CHE *&actBase) {
  TID formTid(formBase->OwnID,formBase->inINCL), actRefID;
#ifndef INTERPRETER
  CHE *currActParm, *pred=(CHE*)chain->last;
  BaseInitV *baseInit;
#endif

  if (actBase) {
    actRefID = ((Reference*)((BaseInit*)actBase->data)->baseItf.ptr)->refID;
    ADJUST4(actRefID);
    if (actRefID == formTid)
      return;
  }

#ifdef INTERPRETER
  if (actBase)
    ((Reference*)((BaseInit*)actBase->data)->baseItf.ptr)->SetError(ckd,&ERR_BaseInit);
  else if (chain->last)
    ((SynObject*)((CHE*)((CHE*)chain->last)->predecessor)->data)->SetError(ckd,&ERR_MissingBaseInits);
  else
    selfVar->SetError(ckd,&ERR_MissingBaseInits);
#else
  if (actBase) {
    currActParm = (CHE*)actBase->successor;
    while (currActParm
    && ((Reference*)((BaseInit*)currActParm->data)->baseItf.ptr)->refID != formTid)
      currActParm = (CHE*)currActParm->successor;
    if (currActParm) {
      PutDelChainHint(ckd,selfVar,chain,currActParm);
      PutInsChainHint(ckd,selfVar,chain,currActParm,(CHE*)actBase->predecessor);
      actBase = currActParm;
      ((Parameter*)actBase->data)->Check(ckd);
      return;
    }
    else
      pred = (CHE*)actBase->predecessor;
  }

  baseInit = new BaseInitV((address)&ckd.document->IDTable,selfVar,formBase);
  actBase = NewCHE(baseInit);
  PutInsChainHint(ckd,selfVar,chain,actBase,pred);
#endif
}

bool SelfVar::BaseInitsCheck (CheckData &ckd)
{
  CHE *chpActBase;
  CHETID *chpFormBase;
  LavaDECL *implDECL, *itfDECL, *formBaseDECL;
  SynObject *opd;
  bool ok=true;

  implDECL = ckd.myDECL->ParentDECL->ParentDECL;
  itfDECL = ckd.document->IDTable.GetDECL(((CHETID*)implDECL->Supports.first)->data,implDECL->inINCL);
  if (!itfDECL) {
    SetError(ckd,&ERR_ImplUndefItf);
    return false;
  }
  chpActBase = (CHE*)baseInitCalls.first;
  chpFormBase = (CHETID*)itfDECL->Supports.first;
  for (;
       chpFormBase;
       chpFormBase = (CHETID*)chpFormBase->successor) {
//      if (chpFormBase->data.nINCL != 1 && !ckd.document->isStd && (itfDECL->inINCL != 1)) {
    formBaseDECL = ckd.document->IDTable.GetDECL(chpFormBase->data,itfDECL->inINCL);
    if (!formBaseDECL) {
      SetError(ckd,&ERR_NoBaseType);
      return false;
    }
    if ((formBaseDECL->fromBType == NonBasic) || (formBaseDECL->fromBType == B_Exception)) {
      if (chpActBase) {
        opd = (SynObject*)chpActBase->data;
        opd->Check(ckd);
      }
      reposBaseInits(ckd,this,&baseInitCalls,formBaseDECL,chpActBase);
      if (formBaseDECL->DeclType == VirtualType)
        ((BaseInit*)chpActBase->data)->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, ckd.lpc, formBaseDECL, ((BaseInit*)chpActBase->data)->isOuter);
      chpActBase = (CHE*)chpActBase->successor;
    }
  }
  if (chpActBase) {
#ifdef INTERPRETER
    SetError(ckd,&ERR_RedundantParms);
    return false;
  }
#else
    for ( ;
         chpActBase;
         chpActBase = (CHE*)chpActBase->successor)  // delete remainder of parameter chain
      PutDelChainHint(ckd,this,&baseInitCalls,chpActBase);
    return true;
  }
#endif
  return true;
}

bool SelfVar::InitCheck (CheckData &ckd, bool inSelfCheck) {
  LavaDECL *implDECL, *itfDECL;
  TDOD *tdod1, *tdod2;
  TDODC tdodc;
  CHE *chp;
  DString varName;
  QString *rc;
  bool ok=true;

  implDECL = ckd.myDECL->ParentDECL->ParentDECL;
  itfDECL = ckd.document->IDTable.GetDECL(((CHETID*)implDECL->Supports.first)->data,implDECL->inINCL);

  if (!itfDECL) {
    SetError(ckd,&ERR_ImplUndefItf);
    return false;
  }

  tdod1 = new TDOD();
  tdod1->ID.nID = ((SelfVar*)ckd.selfVar)->varID.nID;
  tdodc.Append(new CHE(tdod1));
  tdod2 = new TDOD();
  tdodc.Append(new CHE(tdod2));
  ObjReference selfMember(tdodc,"self");
  selfMember.flags.INCL(isSelfVar);

  for (chp = (CHE*)itfDECL->NestedDecls.first; chp; chp = (CHE*)chp->successor)
    if (((LavaDECL*)chp->data)->DeclType == Attr
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isOptional)
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isPlaceholder)
    && !((LavaDECL*)chp->data)->SecondTFlags.Contains(overrides)) {
      ((TDOD*)((CHE*)selfMember.refIDs.last)->data)->ID = TID(((LavaDECL*)chp->data)->OwnID, ((CHETID*)implDECL->Supports.first)->data.nINCL);
      if (rc = ((RefTable*)ckd.refTable)->ReadCheck(ckd,&selfMember)) {
        varName = DString("self.") + ((LavaDECL*)chp->data)->LocalName;
        if (inSelfCheck)
          SetError(ckd,&ERR_MissingInitialization,varName.c);
        else if (ckd.succeed)
          ((SynObject*)ckd.succeed)->SetError(ckd,&ERR_NotYetInitialized,varName.c);
        ok = false;
      }
    }

  for (chp = (CHE*)implDECL->NestedDecls.first; chp; chp = (CHE*)chp->successor)
    if (((LavaDECL*)chp->data)->DeclType == Attr
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isOptional)
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isPlaceholder)) {
      ((TDOD*)((CHE*)selfMember.refIDs.last)->data)->ID = TID(((LavaDECL*)chp->data)->OwnID,0);
      if (rc = ((RefTable*)ckd.refTable)->ReadCheck(ckd,&selfMember)) {
        varName = DString("self.") + ((LavaDECL*)chp->data)->LocalName;
        if (inSelfCheck)
          SetError(ckd,&ERR_MissingInitialization,varName.c);
        else if (ckd.succeed)
          ((SynObject*)ckd.succeed)->SetError(ckd,&ERR_NotYetInitialized,varName.c);
        ok = false;
      }
    }

  return ok;
}

bool SelfVar::InputCheck (CheckData &ckd) {
  bool ok=true;
  CHE *chpFormIn;
  LavaDECL *decl;
  ObjReference *objRef;

  if (formParms.ptr) {
    for (chpFormIn = (CHE*)((FormParms*)formParms.ptr)->inputs.first;
         chpFormIn;
         chpFormIn = (CHE*)chpFormIn->successor) {
      decl = ckd.document->IDTable.GetDECL(((FormParm*)chpFormIn->data)->formParmID,ckd.inINCL);
      if (!decl->TypeFlags.Contains(isOptional)
      && !decl->WorkFlags.Contains(isReferenced)) {
        objRef = (ObjReference*)((FormParm*)chpFormIn->data)->formParm.ptr;
        objRef->SetError(ckd,&ERR_NotReferenced,objRef->refName.c);
        ok = false;
      }
    }
  }
  return ok;
}

bool SelfVar::OutputCheck (CheckData &ckd) {
  bool ok=true;
  CHE *chpFormOut;
  LavaDECL *decl;

  if (formParms.ptr) {
    for (chpFormOut = (CHE*)((FormParms*)formParms.ptr)->outputs.first;
         chpFormOut;
         chpFormOut = (CHE*)chpFormOut->successor) {
      decl = ckd.document->IDTable.GetDECL(((FormParm*)chpFormOut->data)->formParmID,ckd.inINCL);
      if (!decl->TypeFlags.Contains(isOptional))
        ok &= ((ObjReference*)((FormParm*)chpFormOut->data)->formParm.ptr)->Check(ckd);
    }
  }
  return ok;
}

bool SelfVar::Check (CheckData &ckd)
{
  RefTable refTable;
#ifdef INTERPRETER
  TID itfTID;

#else
  if (ckd.concernExecs)
    concernExecs = true;
  else if (concernExecs) {
    concernExecs = false;
    checked = true;
    return ((SynObject*)execName.ptr)->Check(ckd);
  }
#endif

  ENTRY

  ckd.selfVar = this;

  ok &= ((SynObject*)execName.ptr)->Check(ckd);

  ckd.inINCL = ckd.myDECL->inINCL;

  ckd.document->NextContext(execDECL, ckd.lpc);

  if (execDECL->ParentDECL->DeclType == Function) {
    ckd.selfTypeDECL = execDECL->ParentDECL->ParentDECL;
    ckd.lpc.ContextFlags.INCL(selfiContext);
    ckd.lpc.ContextFlags.INCL(selfoContext);
    if (execDECL->ParentDECL->TypeFlags.Contains(forceOverride))
      ckd.lpc.ContextFlags.INCL(staticContext);
    else
      ckd.lpc.ContextFlags.EXCL(staticContext);
  }
  else {
    ckd.selfTypeDECL = execDECL->ParentDECL;
    ckd.lpc.ContextFlags.INCL(staticContext);
    ckd.lpc.ContextFlags.EXCL(selfiContext);
    ckd.lpc.ContextFlags.EXCL(selfoContext);
  }

  ckd.document->IDTable.virtualSelf(ckd.selfTypeDECL,selfType);
  selfCtx = ckd.lpc;
  ckd.refTable = &refTable;


#ifdef INTERPRETER
  inINCL = ckd.inINCL;
  stackPos = SFH;
  switch (execDECL->ParentDECL->DeclType) {
  case Function:
    nInputs = ckd.myDECL->ParentDECL->nInput;
    nOutputs = ckd.myDECL->ParentDECL->nOutput;
    funcDECL = execDECL->ParentDECL;
    if (execDECL->DeclType == ExecDef)
      isFuncBody = true;
    if (funcDECL->Supports.first
        && !(funcDECL->TypeFlags.Contains(isPropGet)
             || funcDECL->TypeFlags.Contains(isPropSet)))
      funcDECL = ckd.document->IDTable.GetDECL(((CHETID*)funcDECL->Supports.first)->data,funcDECL->inINCL);
    if (execDECL->ParentDECL->ParentDECL->DeclType == Interface)
      itfDECL = execDECL->ParentDECL->ParentDECL;
    else {
      itfTID = ((CHETID*)execDECL->ParentDECL->ParentDECL->Supports.first)->data;
      itfDECL = ckd.document->IDTable.GetDECL(itfTID, execDECL->inINCL);
    }
    break;
  case Initiator:
    nInputs = ckd.myDECL->ParentDECL->nInput;
    nOutputs = 0;
    break;
  case Interface:
  case Impl:
    nInputs = 0;
    nOutputs = 0;
    isInvariant = true;
    if (execDECL->ParentDECL->DeclType == Interface)
      itfDECL = execDECL->ParentDECL;
    else {
      itfTID = ((CHETID*)execDECL->ParentDECL->Supports.first)->data;
      itfDECL = ckd.document->IDTable.GetDECL(itfTID, execDECL->inINCL);
    }
    break;
  }
  nParams = nInputs + nOutputs + 1;
  ckd.currentStackLevel = nParams + SFH;
  ckd.stackFrameSize = nParams + SFH;
#endif

  if (execDECL->DeclType == ExecDef
  && execDECL->ParentDECL->DeclType != Impl) {
//#ifndef INTERPRETER
    UpdateParameters(ckd);
    // must be done before BaseInitsCheck since it resets the "isReferenced" WorkFlag of
    // input parameters
//#endif

    if (execDECL->ParentDECL->TypeFlags.Contains(isInitializer))
      ok &= BaseInitsCheck(ckd);
    else
      baseInitCalls.Destroy();
  }

//################################################################
  ok &= ((SynObject*)body.ptr)->Check(ckd);
//################################################################

  if (primaryToken == invariant_T
       || primaryToken == require_T
       || primaryToken == ensure_T)
    if (NoPH(body.ptr))
      ckd.myDECL->WorkFlags.INCL(nonEmptyInvariant);
//    else
//      ckd.myDECL->WorkFlags.EXCL(nonEmptyInvariant);

  if (execDECL->DeclType == ExecDef
  && execDECL->ParentDECL->DeclType != Impl) {
    if (execDECL->ParentDECL->TypeFlags.Contains(isInitializer))
      ok &= InitCheck(ckd);

    ok &= InputCheck(ckd); // check if all non-optional input parameters have been referenced
    if (execDECL->ParentDECL->DeclType == Function)
      ok &= OutputCheck(ckd); // check if all non-optional output parameters have been set
  }

#ifdef INTERPRETER
  if (isFuncBody)
    stackFrameSize = (((AssertionData*)funcDECL->Exec.ptr)->PrepareAssertions(ckd,this));
  else {
    if (isInvariant
    && LBaseData->m_checkInvariants)
      stackFrameSize = ((InvarData*)itfDECL->Exec.ptr)->PrepareInvariants(ckd,this);
    else
      stackFrameSize = ckd.stackFrameSize;
  }
#else
  if (ckd.nErrors+ckd.nPlaceholders) {
    if (!execDECL->DECLError2.first)
      new CLavaError(&execDECL->DECLError2,&ERR_ExecError);
  }
  else if (execDECL->DECLError2.first)
    execDECL->DECLError2.Destroy();

  ((CPEBaseDoc*)ckd.document)->SetExecItemImage(
    execDECL,
    IsEmptyExec(),
    ckd.nErrors+ckd.nPlaceholders != 0);
  if (ckd.execView) {
    ((CExecView*)ckd.execView)->nErrors = ckd.nErrors;
    ((CExecView*)ckd.execView)->nPlaceholders = ckd.nPlaceholders;
  }
#endif

  ((RefTable*)ckd.refTable)->EndOfExec();
  checked = true;
  EXIT
}

void HandleOp::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  cat = valueObj;
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[B_Che]);
  ckd.tempCtx = CContext(0,0);
  ckd.tempCtx.ContextFlags.INCL(staticContext);
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool HandleOp::Check (CheckData &ckd)
{
  ObjReference *opd;
  TID varTID;
  TIDType idType;
  DWORD dw;
  VarName *varName;

  ENTRY
  ok &= ((SynObject*)operand.ptr)->Check(ckd);

  if (((SynObject*)operand.ptr)->primaryToken == ObjRef_T) {
    opd = (ObjReference*)operand.ptr;
    if (opd->refIDs.first != opd->refIDs.last) {
      SetError(ckd,&ERR_HandleOpParm);
      ERROREXIT
    }
    varTID = ((TDOD*)((CHE*)opd->refIDs.first)->data)->ID;
    dw = ckd.document->IDTable.GetVar(varTID,idType,ckd.inINCL);
    if (idType != localID) {
      SetError(ckd,&ERR_HandleOpParm);
      ERROREXIT
    }
    varName = (VarName*)dw;
    if (varName->parentObject->parentObject->primaryToken == declare_T) {
      SetError(ckd,&ERR_HandleOpParm);
      ERROREXIT
    }
  }
  else if (((SynObject*)operand.ptr)->primaryToken != Exp_T) {
    SetError(ckd,&ERR_HandleOpParm);
    ERROREXIT
  }
  EXIT
}

bool SucceedStatement::Check (CheckData &ckd)
{
  MultipleOp *multOp;
  CHE *che;
  SynObject *parent;
  bool existsSucc=false;

  ENTRY

  if (parentObject->IsMultOp()) {
    multOp = (MultipleOp*)parentObject;
    che = (CHE*)whereInParent;
    if (che->successor) {
      SetError(ckd,&ERR_SuccFailNotLast);
      ok = false;
    }
    for (parent = parentObject;
         parent && parent->parentObject;
         parent = parent->parentObject)
      if (parent->parentObject->IsMultOp()) {
        multOp = (MultipleOp*)parent->parentObject;
        che = (CHE*)parent->whereInParent;
        if (che->successor) {
          existsSucc = true;
          break;
        }
      }

    if (!existsSucc) {
      SetError(ckd,&ERR_SucceedNeedless);
      ok = false;
    }
  }


  if (parentObject->primaryToken == not_T) {
    SetError(ckd,&ERR_SuccFailNotNot);
    ok = false;
  }

  ((RefTable*)ckd.refTable)->refTableEntries.Append(new CHE(new CFailSucceed));

  ckd.succeed = this;
  if (ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer))
    ((SelfVar*)ckd.selfVar)->InitCheck(ckd,false);
  if (ckd.myDECL->ParentDECL->DeclType == Function)
    ((SelfVar*)ckd.selfVar)->OutputCheck(ckd); // check if all mandatory output parameters have been set
  ckd.succeed = 0;

  EXIT
}

bool FailStatement::Check (CheckData &ckd)
{
  MultipleOp *multOp;
  CHE *che;
  LavaDECL *declError, *funcDecl, *declExceptType;
  CHETID *chpTID;
  Category cat;
  SynFlags ctxFlags;

  ENTRY

  if (parentObject->IsMultOp()) {
    multOp = (MultipleOp*)parentObject;
    che = (CHE*)whereInParent;
    if (che->successor) {
      SetError(ckd,&ERR_SuccFailNotLast);
      ok = false;
    }
  }

  if (parentObject->primaryToken == not_T) {
    SetError(ckd,&ERR_SuccFailNotNot);
    ok = false;
  }

  ((RefTable*)ckd.refTable)->refTableEntries.Append(new CHE(new CFailSucceed));

  if (!exception.ptr) {
    if (ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer)
    || (ckd.myDECL->ParentDECL->DeclType == Function
        && ((SelfVar*)ckd.selfVar)->formParms.ptr
        && ((FormParms*)((SelfVar*)ckd.selfVar)->formParms.ptr)->outputs.first))
      SetError(ckd,&ERR_MustThrowExc);
    EXIT
  }

  ok &= ((SynObject*)exception.ptr)->Check(ckd);

  if (IsPH(exception.ptr))
    ERROREXIT

  if (ckd.myDECL->ParentDECL->DeclType == Initiator) {
    ((SynObject*)exception.ptr)->SetError(ckd,&ERR_ThrowInInitiator);
    ERROREXIT
  }
  if (ckd.myDECL->ParentDECL->TypeFlags.Contains(execIndependent)) {
    ((SynObject*)exception.ptr)->SetError(ckd,&ERR_InadmissibleThrow);
    ERROREXIT
  }

  ((SynObject*)exception.ptr)->ExprGetFVType(ckd,declError,cat,ctxFlags);
  if (!declError)
    ERROREXIT

  if (/*cat != unknownCategory &&*/ cat != valueObj) {
    ((SynObject*)exception.ptr)->SetError(ckd,&ERR_InadmissibleCategory);
    ok = false;
  }
  if (!declError->SecondTFlags.Contains(isException)) {
    ((SynObject*)exception.ptr)->SetError(ckd,&ERR_FailNoException);
    ERROREXIT
  }

  CContext errCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    errCtx.ContextFlags = ctxFlags;
  funcDecl = ckd.myDECL->ParentDECL;
  if (ckd.myDECL->ParentDECL->DeclType == Function && funcDecl->Supports.first) {
    funcDecl = ckd.document->IDTable.GetDECL(((CHETID*)funcDecl->Supports.first)->data,funcDecl->inINCL);
    // funcDecl in interface
  }
  for ( chpTID = (CHETID*)funcDecl->Inherits.first;
        chpTID;
        chpTID = (CHETID*)chpTID->successor) {
    ckd.tempCtx = ckd.lpc;
    declExceptType = ckd.document->GetFinalMVType(chpTID->data,ckd.inINCL,ckd.tempCtx,cat,&ckd);
    if (compatibleTypes(ckd,declError,errCtx,declExceptType,ckd.tempCtx))
      EXIT
  }
  ((SynObject*)exception.ptr)->SetError(ckd,&ERR_InadmissibleThrow);
  ERROREXIT
}

bool OldExpression::Check (CheckData &ckd)
{
  ENTRY
  ok &= ((SynObject*)paramExpr.ptr)->Check(ckd);
#ifdef INTERPRETER
  ((AssertionData*)((SelfVar*)ckd.selfVar)->funcDECL->Exec.ptr)->oldExpressions.append(this);
#endif
  EXIT
}

void OldExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((ObjReference*)paramExpr.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
}

void UnaryOp::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  TID tidOperatorFunc;
  LavaDECL *declOutparm1;
#ifndef INTERPRETER
  LavaDECL *declOp1;
#endif
  CHE *chpOutparm1;

  decl = 0;
  cat = unknownCategory;

#ifdef INTERPRETER
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    return;
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc);
#else
  ((SynObject*)operand.ptr)->ExprGetFVType(ckd,declOp1,cat,ctxFlags);
  declOp1 = ckd.document->GetType(declOp1);
  if (declOp1)
    if (!ckd.document->GetOperatorID(declOp1,OPERATOR,tidOperatorFunc))
      return;
    else {
#endif
      chpOutparm1 = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
      if (chpOutparm1) {
        declOutparm1 = (LavaDECL*)chpOutparm1->data;
        if (!declOutparm1)
          return;
        if (declOutparm1->TypeFlags.Contains(isOptional))
          flags.INCL(isOptionalExpr);
        else
          flags.EXCL(isOptionalExpr);
#ifdef INTERPRETER
        formVType = ckd.document->IDTable.GetDECL(declOutparm1->RefID,formVType->inINCL);
        vSectionNumber = ckd.document->GetVTSectionNumber(ckd, ckd.tempCtx, formVType, isOuter);
#endif
        ckd.tempCtx = callCtx;
        if (ctxFlags.bits)
          ckd.tempCtx.ContextFlags = ctxFlags;
        decl = ckd.document->GetFinalMVType(declOutparm1->RefID,declOutparm1->inINCL,ckd.tempCtx,cat,&ckd);
        if (declOutparm1->TypeFlags.Contains(substitutable))
          ctxFlags = SET(multiContext,-1);
        else
          ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
        if (cat == unknownCategory
        && declOutparm1->TypeFlags.Contains(trueObjCat))
          if (declOutparm1->TypeFlags.Contains(stateObject))
            cat = stateObj;
          else if (declOutparm1->TypeFlags.Contains(sameAsSelf))
            cat = sameAsSelfObj;
          else
            cat = valueObj;
      }
#ifndef INTERPRETER
    }
#else
  finalType = ckd.document->GetType(decl);
#endif
}

bool UnaryOp::Check (CheckData &ckd)
{
  TID tidOperatorFunc;
  LavaDECL *declOp1;
  Category cat;
  SynFlags ctxFlags;
  CHE *chpFormOut;
  SynObject *opd;
#ifdef INTERPRETER
  LavaDECL *formDecl;
#endif

  ENTRY

  opd = (SynObject*)operand.ptr;
/*  if (opd->primaryToken == ifx_T) {
    opd->SetError(ckd,&ERR_IfxForbidden);
    ok = false;
  }*/

  ok &= opd->Check(ckd);

  ckd.tempCtx = ckd.lpc;
  opd->ExprGetFVType(ckd,declOp1,cat,ctxFlags);
  if (!declOp1)
    ERROREXIT
  if (declOp1->TypeFlags.Contains(isAbstract)) {
    SetError(ckd,&ERR_OperatorUndefined);
    ERROREXIT
  }

  if (opd->IsOptional(ckd)
  && (opd->primaryToken != ObjRef_T || !opd->IsDefChecked(ckd))) {
    opd->SetError(ckd,&ERR_NotOptional);
    ok = false;
  }
  declOp1 = ckd.document->GetTypeAndContext(declOp1,ckd.tempCtx);
  callCtx = ckd.tempCtx;
  ckd.document->NextContext(declOp1, callCtx);

  tidOperatorFunc = opFunctionID;
  chpFormOut = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
  if (chpFormOut)
    if (((LavaDECL*)chpFormOut->data)->TypeFlags.Contains(isOptional))
      flags.INCL(isOptionalExpr);
    else
      flags.EXCL(isOptionalExpr);

  if (declOp1) {
#ifdef INTERPRETER
    if (tidOperatorFunc.nID == -1)
      SetError(ckd,&ERR_UnaryOpUndefined);
    funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc,ckd.inINCL);
    if (!funcDecl)
      SetError(ckd,&ERR_UnaryOpUndefined);
    formDecl = (LavaDECL*)chpFormOut->data;
    formVType = ckd.document->IDTable.GetDECL(formDecl->RefID,formDecl->inINCL);
    vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, formVType, isOuter);
    funcSectionNumber = ckd.document->GetMemsSectionNumber(ckd, declOp1,funcDecl);
#else
    if (!ckd.document->GetOperatorID(declOp1,OPERATOR,tidOperatorFunc)) {
      SetError(ckd,&ERR_UnaryOpUndefined);
      ERROREXIT
    }
    else
      opFunctionID = tidOperatorFunc;
#endif
  }
  EXIT
}

bool LogicalNot::Check (CheckData &ckd)
{
  SET oldFlags=ckd.flags;

  ENTRY
  ckd.flags.INCL(InLogNot);
  ok &= ((SynObject*)operand.ptr)->Check(ckd);
  ckd.flags = oldFlags;
  EXIT
}

bool InSetStatement::Check (CheckData &ckd)
{
  Expression *opd1, *opd2;
  TID tidSetEl;
  LavaDECL *declOp1, *declOp2, *declSet, *declSetEl;
  Category cat1, cat2;
  SynFlags ctxFlags;
  CContext tCtx;

  ENTRY
  ok &= ((SynObject*)operand1.ptr)->Check(ckd);
  ok &= ((SynObject*)operand2.ptr)->Check(ckd);

  opd1 = (Expression*)operand1.ptr;
  opd2 = (Expression*)operand2.ptr;

  opd1->ExprGetFVType(ckd,declOp1,cat1,ctxFlags);
  if (declOp1 == (LavaDECL*)-1) {
    opd1->SetError(ckd,&ERR_IncompatibleType);
    ERROREXIT
  }
  tCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    tCtx.ContextFlags = ctxFlags;
  opd2->ExprGetFVType(ckd,declOp2,cat2,ctxFlags);
  if (declOp2 == (LavaDECL*)-1) {
    opd2->SetError(ckd,&ERR_IncompatibleType);
    ERROREXIT
  }
  if (ctxFlags.bits)
    ckd.tempCtx.ContextFlags = ctxFlags;

  if (opd2->primaryToken == intIntv_T) {
    if (!declOp1)
      EXIT
    if (!compatibleTypes(ckd,declOp2,ckd.tempCtx,declOp1,tCtx)) {
      opd1->SetError(ckd,ckd.errorCode);
      ok = false;
    }
    if (cat1 != unknownCategory
    && cat2 != unknownCategory
    && cat1 != cat2) {
      opd1->SetError(ckd,&ERR_IncompatibleCategory);
      ok = false;
    }
  }
  else {
    declSet = ckd.document->GetType(declOp2);
    if (!declSet)
      ERROREXIT
    if (declSet->SecondTFlags.Contains(isSet)) {
      ckd.document->IDTable.GetParamID(declSet,tidSetEl,isSet);
      if (tidSetEl.nID == -1) {
        opd2->SetError(ckd,&ERR_UndefinedSetElemType);
        ok = false;
      }
      else {
        if (!declOp1)
          EXIT
        declSetEl = ckd.document->GetFinalMVType(tidSetEl,0,ckd.tempCtx,cat2,&ckd); // Context??
        if (!compatibleTypes(ckd,declOp1,tCtx,declSetEl,ckd.tempCtx)) {
          opd1->SetError(ckd,ckd.errorCode);
          ok = false;
        }
        if (declSetEl
        && cat1 != unknownCategory
        && cat2 != unknownCategory
        && cat1 != cat2) {
          opd1->SetError(ckd,&ERR_IncompatibleCategory);
          ok = false;
        }
        if (NoPH(opd1))
          ((Expression*)opd1)->targetCat = cat2;
      }
    }
    else {
      opd2->SetError(ckd,&ERR_IsntSet);
      ok = false;
    }
  }

  EXIT
}

void BinaryOp::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  TID tidOp1, tidOperatorFunc, tidOutparm1;
  LavaDECL *declOutparm1;
#ifndef INTERPRETER
  LavaDECL *declOp1;
#endif
  CHE *chpOutparm1;
//  SynFlags ctxFlags;

  decl = 0;
  cat = unknownCategory;

#ifdef INTERPRETER
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    return;
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc);
#else
  ((SynObject*)operand1.ptr)->ExprGetFVType(ckd,declOp1,cat,ctxFlags);
  declOp1 = ckd.document->GetType(declOp1);
  if (declOp1)
    if (!ckd.document->GetOperatorID(declOp1,OPERATOR,tidOperatorFunc))
      return;
    else {
#endif
      chpOutparm1 = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
      if (chpOutparm1) {
        declOutparm1 = (LavaDECL*)chpOutparm1->data;
        if (!declOutparm1)
          return;
        if (declOutparm1->TypeFlags.Contains(isOptional))
          flags.INCL(isOptionalExpr);
        else
          flags.EXCL(isOptionalExpr);
        ckd.tempCtx = callCtx;
        if (ctxFlags.bits)
          ckd.tempCtx.ContextFlags = ctxFlags;
        decl = ckd.document->GetFinalMVType(declOutparm1->RefID,declOutparm1->inINCL,ckd.tempCtx,cat,&ckd);
        if (declOutparm1->TypeFlags.Contains(substitutable))
          ctxFlags = SET(multiContext,-1);
        else
          ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
        if (cat == unknownCategory
        && declOutparm1->TypeFlags.Contains(trueObjCat))
          if (declOutparm1->TypeFlags.Contains(stateObject))
            cat = stateObj;
          else if (declOutparm1->TypeFlags.Contains(sameAsSelf))
            cat = sameAsSelfObj;
          else
            cat = valueObj;
      }
#ifndef INTERPRETER
    }
#else
  finalType = ckd.document->GetType(decl);
#endif
}

bool BinaryOp::Check (CheckData &ckd)
{
  Expression *opd1, *opd2;
  TID tidSetEl, tidOperatorFunc, formTid;
  LavaDECL *declFuncClass, *declOp1, *declOp2, *finalDeclOp2,
    *formParmDecl, *formTypeDecl;
  CContext op2Ctx;
  CHE *chpFormIn, *chpFormOut;
  SynFlags ctxFlags1, ctxFlags2;
  Category cat1, cat2, formCat2, callObjCat;
  bool opd1ok, opd2IsNull=false;

  ENTRY

  opd1 = (Expression*)operand1.ptr;
  opd2 = (Expression*)operand2.ptr;

/*  if (opd1->IsIfStmExpr()) {
    opd1->SetError(ckd,&ERR_IfxForbidden);
    ERROREXIT
  }*/

  ok &= opd1ok = opd1->Check(ckd);
  if (opd1->IsOptional(ckd)
  && (primaryToken != Equal_T && primaryToken != NotEqual_T)
  && (opd1->primaryToken != ObjRef_T || !opd1->IsDefChecked(ckd))) {
    ((SynObject*)opd1)->SetError(ckd,&ERR_NotOptional);
    ok = false;
  }

  if (!opd2->IsIfStmExpr())
    ok &= opd2->Check(ckd);

  opd1->ExprGetFVType(ckd,declOp1,cat1,ctxFlags1);
  callCtx = ckd.tempCtx;
  opd2->ExprGetFVType(ckd,declOp2,cat2,ctxFlags2);
  if (declOp2 == (LavaDECL*)-1)
    if (opd2->NullAdmissible(ckd)) // "nothing" admissible?
      opd2IsNull = true;
    else {
      opd2->SetError(ckd,&ERR_NotOptional);
      ERROREXIT
    }
  if (!opd2IsNull && ctxFlags2.bits)
    ckd.tempCtx.ContextFlags = ctxFlags2;
  declFuncClass = declOp1;
  if (IsPH(opd1))
    ERROREXIT
  else if (!declFuncClass) {
    if (opd1ok)
      opd1->SetError(ckd,&ERR_UndefType);
    ERROREXIT
  }
  declFuncClass = ckd.document->GetTypeAndContext(declFuncClass,callCtx);
  ckd.document->NextContext(declFuncClass, callCtx);
  callObjCat = cat1;

#ifdef INTERPRETER
  if (!declFuncClass)
    opd1->SetError(ckd,&ERR_UndefType);
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    SetError(ckd,&ERR_OperatorUndefined);
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc);
  if (!funcDecl)
    SetError(ckd,&ERR_OperatorUndefined);
  funcSectionNumber = ckd.document->GetMemsSectionNumber(ckd, declFuncClass,funcDecl);
#else
  if (!declFuncClass)
    ERROREXIT
  if (!ckd.document->GetOperatorID(declFuncClass,OPERATOR,tidOperatorFunc)) {
    SetError(ckd,&ERR_OperatorUndefined);
    ERROREXIT
  }
  else
    opFunctionID = tidOperatorFunc;
#endif

  if (opd2IsNull || IsPH(opd2) || !declOp2)
    EXIT
  chpFormIn = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
  formParmDecl = (LavaDECL*)chpFormIn->data;
  if (!formParmDecl)
    ERROREXIT
  formTid = formParmDecl->RefID;
  CContext ctx = callCtx;
  if (ctxFlags1.bits)
    ctx.ContextFlags = ctxFlags1;
  formTypeDecl = ckd.document->GetFinalMVType(formTid,formParmDecl->inINCL,ctx,formCat2,&ckd); // Context??
  if (formParmDecl->TypeFlags.Contains(substitutable))
    ctx.ContextFlags = SET(multiContext,-1); //?? stimmt der Context
  if (formCat2 == unknownCategory
  && formParmDecl->TypeFlags.Contains(trueObjCat))
    if (formParmDecl->TypeFlags.Contains(stateObject))
      formCat2 = stateObj;
    else if (formParmDecl->TypeFlags.Contains(sameAsSelf))
      formCat2 = sameAsSelfObj;
    else
      formCat2 = valueObj;

  if (opd2->IsIfStmExpr()) {
    ((CondExpression*)opd2)->targetDecl = formTypeDecl;
    ((CondExpression*)opd2)->targetCtx = ctx;
    ((CondExpression*)opd2)->targetCat = formCat2;
    ((CondExpression*)opd2)->callObjCat = callObjCat;
    ok &= ((SynObject*)opd2)->Check(ckd);
  }
  else {
    if ((primaryToken == Equal_T || primaryToken == NotEqual_T)
    && ((declFuncClass->inINCL == 1 && !ckd.document->isStd)
        || (declFuncClass->inINCL == 0 && ckd.document->isStd))) {
      finalDeclOp2 = ckd.document->GetTypeAndContext(declOp2,op2Ctx);
      if (declFuncClass != finalDeclOp2) {
        opd2->SetError(ckd,&ERR_IncompatibleType);
        ok = false;
      }
    }
    else {
      if (!compatibleTypes(ckd,declOp2,ckd.tempCtx,formTypeDecl,ctx)) {
        opd2->SetError(ckd,ckd.errorCode);
        ok = false;
      }
    }
    if (cat2 != unknownCategory
    && ((formCat2 == sameAsSelfObj && cat2 != callObjCat)
        || (formCat2 != sameAsSelfObj && cat2 != formCat2))) {
      opd2->SetError(ckd,&ERR_IncompatibleCategory);
      ok = false;
    }
  }

  chpFormOut = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
  if (chpFormOut)
    if (((LavaDECL*)chpFormOut->data)->TypeFlags.Contains(isOptional))
      flags.INCL(isOptionalExpr);
    else
      flags.EXCL(isOptionalExpr);

  if (opd2->IsOptional(ckd)
  && !formParmDecl->TypeFlags.Contains(isOptional)
  && !opd2->IsDefChecked(ckd)) {
    ((SynObject*)opd2)->SetError(ckd,&ERR_NotOptional);
    ok = false;
  }

#ifdef INTERPRETER
  opd2->formVType = ckd.document->IDTable.GetDECL(formParmDecl->RefID, formParmDecl->inINCL);
  opd2->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, opd2->formVType, opd2->isOuter);
  formTypeDecl = ckd.document->GetType(formTypeDecl);
  declOp2 = ckd.document->GetType(declOp2);
  opd2->sectionNumber = ckd.document->GetSectionNumber(ckd, declOp2,formTypeDecl);
  if (chpFormOut) {
    formVType = (LavaDECL*)chpFormOut->data;
    formVType = ckd.document->IDTable.GetDECL(formVType->RefID, formVType->inINCL);
    vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, formVType, isOuter);
  }
#endif
  EXIT
}

void EvalExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  cat = valueObj;
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[B_Bool]);
  ckd.tempCtx = CContext(0,0);
  ckd.tempCtx.ContextFlags.INCL(staticContext);
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = decl;
#endif
}

bool EvalExpression::Check (CheckData &ckd)
{
  ENTRY

  ok &= ((SynObject*)operand.ptr)->Check(ckd);

  EXIT
}

bool EvalStatement::Check (CheckData &ckd)
{
  Category cat;
  LavaDECL *declOpd, *boolDecl;
  CContext nullCtx;
  SynFlags ctxFlags;

  ENTRY

  ok = ((SynObject*)operand.ptr)->Check(ckd);
  ((SynObject*)operand.ptr)->ExprGetFVType(ckd,declOpd,cat,ctxFlags);
  if (!declOpd)
    ERROREXIT
  boolDecl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[B_Bool]);
  declOpd = ckd.document->GetType(declOpd);
  if (!ckd.document->IDTable.IsAn(TID(declOpd->OwnID, declOpd->inINCL),0,TID(boolDecl->OwnID, boolDecl->inINCL),0)) {
    ((SynObject*)operand.ptr)->SetError(ckd,ckd.errorCode);
    ok = false;
  }

  EXIT
}

void Reference::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ckd.tempCtx = ckd.lpc;
  cat = unknownCategory;
  decl = ckd.document->GetFinalMVType(refID,ckd.inINCL,ckd.tempCtx,cat,&ckd);
  if (cat == unknownCategory)
    if (flags.Contains(isVariable))
      cat = stateObj;
    else if (flags.Contains(isSameAsSelf))
      cat = sameAsSelfObj;
    else
      cat = valueObj;
  ctxFlags.bits = 0;
  if (flags.Contains(isSubstitutable))
    ctxFlags.INCL(multiContext);
}

bool Reference::Check (CheckData &ckd)
{
  ENTRY
  ok &= UpdateReference(ckd);
  EXIT
}

void ObjReference::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = myFinalVType;
  if (decl)
    cat = myCategory;
  else
    cat = unknownCategory;
  ckd.tempCtx = myContext;
  ctxFlags.bits = 0;
  if (flags.Contains(isSubstitutable))
    ctxFlags.INCL(multiContext);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

VarRefContext ObjReference::Context () {
  Assignment *assig;
  CopyStatement *copyStm;
  FuncStatement *funcStm;
  FuncExpression *funcExpr;

  if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    if (whereInParent == (address)&assig->targetObj.ptr)
      return assignmentTarget;
  }
  else if (parentObject->primaryToken == copy_T) {
    copyStm = (CopyStatement*)parentObject;
    if (whereInParent == (address)&copyStm->ontoObj.ptr)
      return copyTarget;
  }
  else if (parentObject->primaryToken == parameter_T
  && parentObject->parentObject->primaryToken == assignFS_T) {
    funcStm = (FuncStatement*)parentObject->parentObject;
    if (parentObject->containingChain == &funcStm->outputs)
      return assignmentTarget;
  }
  else if (parentObject->IsFuncInvocation()) {
    funcExpr = (FuncExpression*)parentObject;
    if (whereInParent == (address)&funcExpr->handle.ptr)
      return funcHandle;
  }
  else if (parentObject->primaryToken == arrayAtIndex_T
    && whereInParent == (address)&((ArrayAtIndex*)parentObject)->arrayObj.ptr) {
    if (parentObject->parentObject->primaryToken == assign_T) {
      assig = (Assignment*)parentObject->parentObject;
      if (parentObject->whereInParent == (address)&assig->targetObj.ptr)
        return arrayTarget;
      else
        return funcHandle;
    }
    else if (parentObject->parentObject->primaryToken == parameter_T
    && parentObject->parentObject->parentObject->primaryToken == assignFS_T) {
      funcStm = (FuncStatement*)parentObject->parentObject->parentObject;
      if (parentObject->parentObject->containingChain == &funcStm->outputs)
        return arrayTarget;
      else
        return funcHandle;
    }
  }

  return inputParam;
}

bool ObjReference::InConstituent (CheckData &ckd) {
  LavaDECL *decl;
  CHE *chp;

  for (chp = (CHE*)((CHE*)refIDs.last)->predecessor; chp; chp = (CHE*)chp->predecessor) {
    decl = ckd.document->IDTable.GetDECL(((TDOD*)chp->data)->ID,ckd.inINCL);
    if (!decl) return false;
    if (!decl->TypeFlags.Contains(constituent))
      return false;
  }
  return true;
}

bool SynObject::IsDefChecked(CheckData &ckd) {
  SynObject *synObj, *parent;

  if (primaryToken != ObjRef_T)
    return false;

  synObj = this;
  for (parent=synObj->parentObject; parent; parent=parent->parentObject) {
    if (parent->parentObject
    && parent->parentObject->primaryToken == ifdef_T
    && parent->whereInParent == (address)&((IfdefStatement*)parent->parentObject)->thenPart.ptr)
      if (((IfdefStatement*)parent->parentObject)->Checks(*(ObjReference*)synObj))
        return true;
  }
  return false;
}

bool ObjReference::operator== (ObjReference &objRef) {
  CHE *chp, *chp2;

  chp = (CHE*)refIDs.first;
  chp2 = (CHE*)objRef.refIDs.first;
  do {
    if(((TDOD*)chp->data)->ID != ((TDOD*)chp2->data)->ID)
      return false;
    chp = (CHE*)chp->successor;
    chp2 = (CHE*)chp2->successor;
  } while (chp && chp2);
  if (!chp && !chp2)
    return true;

  return false;
}

bool SynObject::InInitializer (CheckData &ckd) {
  if (ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer))
    return true;
  else
    return false;
}

bool ObjReference::IsOptional (CheckData &ckd) {
  return ((TDOD*)((CHE*)refIDs.last)->data)->IsOptional(ckd);
}

bool ObjReference::OutOfScope (CheckData &ckd) {
  TID startID=((TDOD*)((CHE*)refIDs.first)->data)->ID;
  ADJUST4(startID);
  LocalVarSearch lvs(ckd.document,&startID);

  lvs.CheckLocalScope(ckd,this);
  if (lvs.inForeach)
    flags.INCL(isInForeach);
  else
    flags.EXCL(isInForeach);
  if (lvs.found) return false;
  else return true;
}

bool ObjReference::AssignCheck (CheckData &ckd,VarRefContext vrc) {
  LavaDECL *decl, *iniItfDecl;
  Expression *fromExpr;
  CHE *secondChe;
  DWORD dw;
  TIDType idtype;
  TID iniItfTid;
  Category cat;
  SynFlags ctxFlags;
  QString *rc;
  bool ok=true;
  ROContext roContext=ReadOnlyContext();

  if (flags.Contains(isLocalVar)
  && flags.Contains(isInForeach)) {
    ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_AssigInForeach);
    return false;
  }

  if (roContext == roClause) {
    SetError(ckd,&ERR_AssignInROClause);
    return false;
  }

  if (refIDs.first == refIDs.last) { // single target variable
    if (roContext == assertion
    && !flags.Contains(isDeclareVar)) {
      SetError(ckd,&ERR_AssignInPrePostInv);
      return false;
    }
    if (roContext == roExec
    && !flags.Contains(isDeclareVar)
    && !flags.Contains(isOutputVar)) {
      SetError(ckd,&ERR_AssignInROClause);
      return false;
    }
    if (flags.Contains(isSelfVar)) {
      if (InInitializer(ckd) && vrc == copyTarget) {
        fromExpr = (Expression*)((CopyStatement*)parentObject)->fromObj.ptr;
        fromExpr->ExprGetFVType(ckd,decl,cat,ctxFlags);
        decl = ckd.document->GetType(decl);
        iniItfTid = ((CHETID*)ckd.selfTypeDECL->Supports.first)->data;
        iniItfDecl = ckd.document->IDTable.GetDECL(iniItfTid,ckd.inINCL);
        if (decl
        && !ckd.document->IsCDerivation(decl,iniItfDecl,&ckd)) {
          SetError(ckd,&ERR_CopyToSelf);
          return false;
        }
      }
      else if (parentObject->primaryToken != copy_T) {
        SetError(ckd,&ERR_AssigToSelf);
        return false;
      }
    }
    else if (flags.Contains(isInputVar)) {
      SetError(ckd,&ERR_AssigToInput);
      return false;
    }
    else if (!flags.Contains(isDeclareVar)
    && !flags.Contains(isOutputVar)) {
      if (!flags.Contains(isTempVar)
        || vrc != copyTarget) {
        SetError(ckd,&ERR_AssigToLocal);
        return false;
      }
    }
  }
  else { // path a.b. ...
    dw = ckd.document->IDTable.GetVar(((TDOD*)((CHE*)refIDs.last)->data)->ID,idtype,ckd.inINCL);
    decl = *(LavaDECL**)dw;
    if (roContext == assertion
    && !flags.Contains(isDeclareVar)) {
      SetError(ckd,&ERR_AssignInPrePostInv);
      return false;
    }
    if (roContext != noROContext
    && !(flags.Contains(isSelfVar)
         && InInitializer(ckd)
         && refIDs.first->successor == refIDs.last)
    && !(ckd.flags.Contains(InBut)
         && flags.Contains(isTempVar)
         && refIDs.first->successor == refIDs.last)) {
      SetError(ckd,&ERR_AssignInQuery);
      return false;
    }
    else
      if (flags.Contains(isStateObjMember)) {
        if (decl->TypeFlags.Contains(isConst)) {
          if (!flags.Contains(isSelfVar)
          || !InInitializer(ckd)) {
            SetError(ckd,&ERR_AssigToRdOnly);
            return false;
          }
        }
        else if (flags.Contains(isDeclareVar)
        || flags.Contains(isOutputVar)) {
          secondChe = (CHE*)((CHE*)refIDs.first)->successor;
          ((CHE*)refIDs.first)->successor = 0;
          if (rc = ((RefTable*)ckd.refTable)->ReadCheck(ckd,this)) {
            ((TDOD*)((CHE*)refIDs.first)->data)->SetError(ckd,rc,((TDOD*)((CHE*)refIDs.first)->data)->name.c);
            ((CHE*)refIDs.first)->successor = secondChe;
            return false;
          }
          else
            ((CHE*)refIDs.first)->successor = secondChe;
        }
      }
      // !isStateObjMember:
      else if (((CHE*)refIDs.first)->successor == refIDs.last) {
        if (!flags.Contains(isTempVar))
          if (flags.Contains(isSelfVar)) {
            if (ckd.myDECL->ParentDECL->TypeFlags.Contains(isConst)
            && !InInitializer(ckd)) { // assignment to self.mem in read-only function != initializer
              ((SynObject*)((CHE*)refIDs.last)->data)->SetError(ckd,&ERR_RdOnlyFunc);
              return false;
            }
          }
          else {
            ((SynObject*)((CHE*)refIDs.last)->data)->SetError(ckd,&ERR_AssigToFrozen);
            return false;
          }
      }
      else {
        ((SynObject*)((CHE*)refIDs.last)->data)->SetError(ckd,&ERR_AssigToFrozen);
        return false;
      }
  }

  // finally prevent single-assignment violations
  if  (rc = ((RefTable*)ckd.refTable)->AssignCheck(ckd,this)) {
    SetError(ckd,rc);
    return false;
  }
  return ok;
}

bool ObjReference::CopyCheck (CheckData &ckd) {
//  LavaDECL *decl;
//  Category cat;
  QString *rc;
  SynFlags ctxFlags;

  if (flags.Contains(isTempVar)
  && refIDs.first == refIDs.last)
    return true;

  if (flags.Contains(isSelfVar)
  && InInitializer(ckd)
  && refIDs.first == refIDs.last)
    return true;

  // finally prevent single-assignment violations
  if  (rc = ((RefTable*)ckd.refTable)->AssignCheck(ckd,this)) {
    SetError(ckd,rc);
    return false;
  }

/*  ExprGetFVType(ckd,decl,cat,ctxFlags);
  if (cat == stateObj)
    return true;

  SetError(ckd,&ERR_WrongCopyTarget);
  return false;*/
  return true;
}

LocalVarSearch::LocalVarSearch (CLavaBaseDoc *d, TID *tid) {
  doc = d;
  toBeFound = tid;
  found = false;
}

bool LocalVarSearch::Action (CheckData &ckd, VarName *varName, TID&) {
  TID varID=varName->varID;

  ADJUST4(varID);
  if (varID == *toBeFound)
    if (ckd.criticalScope) {
      ckd.criticalScope = false;
      return false;
    }
    else {
      found = true;
      return true;
    }
  else
    return false;
}

LocalVarByNameSearch::LocalVarByNameSearch (STRING name) {
  toBeFound = name;
  found = false;
}

bool LocalVarByNameSearch::Action (CheckData &ckd, VarName *vName, TID&) {
  STRING name=vName->varName;

  if (name == toBeFound)
    if (ckd.criticalScope) {
      ckd.criticalScope = false;
      return false;
    }
    else {
      found = true;
      varName = vName;
      return true;
    }
  else
    return false;
}

bool ObjReference::ReadCheck (CheckData &ckd) {
  QString *rc=0;
  CHE *secondChe;
  bool ok=true;
  Expression *expr;
  FuncExpression *funcExpr;
  Operation *op;

//  if (flags.Contains(isLocalVar))
//    OutOfScope(ckd);

  if (flags.Contains(isSelfVar)
  && refIDs.first == refIDs.last
  && ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer)
  && parentObject->primaryToken == parameter_T) {
    expr = (Expression*)parentObject->parentObject;
    if (expr->IsFuncInvocation()) {
      funcExpr = (FuncExpression*)expr;
      if (!funcExpr->funcDecl)
        return ok;
      if (!funcExpr->funcDecl->TypeFlags.Contains(isInitializer)) {
        ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_SelfUnfinished);
        ok = false;
      }
    }
    else if (expr->IsMultOp() || expr->IsBinaryOp()) {
      op = (Operation*)expr;
      if (!op->funcDecl)
        return ok;
      if (!op->funcDecl->TypeFlags.Contains(isInitializer)) {
        ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_SelfUnfinished);
        ok = false;
      }
    }
  }
  else if (flags.Contains(isTempVar)
  && refIDs.first == refIDs.last
  && parentObject->primaryToken == parameter_T) {
    expr = (Expression*)parentObject->parentObject;
    if (expr->IsFuncInvocation()) {
      funcExpr = (FuncExpression*)expr;
      if (!funcExpr->funcDecl)
        return ok;
      if (!funcExpr->funcDecl->TypeFlags.Contains(isInitializer)) {
        ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_TempUnfinished);
        ok = false;
      }
    }
    else if (expr->IsMultOp() || expr->IsBinaryOp()) {
      op = (Operation*)expr;
      if (!op->funcDecl)
        return ok;
      if (!op->funcDecl->TypeFlags.Contains(isInitializer)) {
        ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_TempUnfinished);
        ok = false;
      }
    }
  }

  if (flags.Contains(isDeclareVar)
  || (flags.Contains(isOutputVar) && ckd.myDECL->DeclType != Ensure)) {
    secondChe = (CHE*)((CHE*)refIDs.first)->successor;
    ((CHE*)refIDs.first)->successor = 0;
    if (rc = ((RefTable*)ckd.refTable)->ReadCheck(ckd,this)) {
      ((TDOD*)((CHE*)refIDs.first)->data)->SetError(ckd,rc,((TDOD*)((CHE*)refIDs.first)->data)->name.c);
      ((CHE*)refIDs.first)->successor = secondChe;
      return false;
    }
    ((CHE*)refIDs.first)->successor = secondChe;
  }
  else if (flags.Contains(isSelfVar)
  && (CHE*)refIDs.first->successor == refIDs.last
  && ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer)
  && !Inherited(ckd)) {
    if (rc = ((RefTable*)ckd.refTable)->ReadCheck(ckd,this)) {
      SetError(ckd,rc,refName.c);
      return false;
    }
  }

  return ok;
}

bool ObjReference::Inherited (CheckData &ckd) {
  LavaDECL *implDECL, *itfDECL;
  TID myTid=((TDOD*)((CHE*)refIDs.last)->data)->ID, memberTid;
  CHE *chp;
  bool ok=true;

  implDECL = ckd.myDECL->ParentDECL->ParentDECL;
  itfDECL = ckd.document->IDTable.GetDECL(((CHETID*)implDECL->Supports.first)->data,implDECL->inINCL);

  if (!itfDECL) return true;

  ADJUST4(myTid);
  for (chp = (CHE*)itfDECL->NestedDecls.first; chp; chp = (CHE*)chp->successor)
    if (((LavaDECL*)chp->data)->DeclType == Attr
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isOptional)
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isPlaceholder)) {
      memberTid = TID(((LavaDECL*)chp->data)->OwnID, itfDECL->inINCL);
      if (memberTid == myTid) return false;
    }

  for (chp = (CHE*)implDECL->NestedDecls.first; chp; chp = (CHE*)chp->successor)
    if (((LavaDECL*)chp->data)->DeclType == Attr
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isOptional)
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isPlaceholder)) {
      memberTid = TID(((LavaDECL*)chp->data)->OwnID, itfDECL->inINCL);
      if (memberTid == myTid) return false;
    }

  return true;
}

bool ObjReference::ArrayTargetCheck (CheckData &ckd) {
  if (((TDOD*)((CHE*)refIDs.last)->data)->IsStateObject(ckd))
    return true;
  if (ReadOnlyContext() == roClause) {
    SetError(ckd,&ERR_AssignInROClause);
    ERROREXIT
  }
  if (!((SynObject*)((ArrayAtIndex*)parentObject)->arrayObj.ptr)->flags.Contains(isTempVar))
    if (InInitializer(ckd)) {
      if (!((SynObject*)((ArrayAtIndex*)parentObject)->arrayObj.ptr)->flags.Contains(isSelfVar)) {
        SetError(ckd,&ERR_ImmutableArray);
        ERROREXIT
      }
    }
    else {
      SetError(ckd,&ERR_ImmutableArray);
      ERROREXIT
    }
  return true;
}

bool ObjReference::CallCheck (CheckData &ckd) {
  FuncExpression *funcExpr=(FuncExpression*)parentObject;
  TID tid;
  LavaDECL *decl;
  bool ok=true;

  ok = ReadCheck(ckd);

  if (!parentObject->IsFuncInvocation())
    return ok;

  if (refIDs.first == refIDs.last)
    if (flags.Contains(isTempVar))
      return true;
    else if (flags.Contains(isSelfVar)) {
      if (parentObject->parentObject->primaryToken != initializing_T
      && ckd.myDECL->ParentDECL->TypeFlags.Contains(isInitializer)
      && !((SelfVar*)ckd.selfVar)->InitCheck(ckd,false)) {
        ((SynObject*)((CHE*)refIDs.first)->data)->SetError(ckd,&ERR_SelfUnfinishedCallObj);
        return false;
      }
//      else
//        return ok;
    }

  if (((Reference*)funcExpr->function.ptr)->IsPlaceHolder())
    return ok;

  decl = ckd.document->IDTable.GetDECL(((Reference*)funcExpr->function.ptr)->refID,ckd.inINCL);

  if (!decl|| flags.Contains(brokenRef))
    return ok;

  if (!decl->TypeFlags.Contains(isConst))
    if (ReadOnlyContext() != noROContext
    && (!flags.Contains(isDeclareVar)
        || refIDs.first != refIDs.last)) {
      funcExpr->SetError(ckd,&ERR_NonROCallInROClause);
      return false;
    }
    else if (!((TDOD*)((CHE*)refIDs.last)->data)->IsStateObject(ckd)
    && !flags.Contains(isSelfVar)
    && !flags.Contains(isTempVar)) {
      SetError(ckd,&ERR_ImmutableCallObj);
      return false;
    }

  return ok;
}

bool ObjReference::Check (CheckData &ckd) {
  bool ok1=true;

  if (parentObject->primaryToken == FormParm_T) {
    if (((RefTable*)ckd.refTable)->ReadCheck(ckd,this)) {
      if (ckd.succeed) {
        SetError(ckd,&ERR_MissingInitialization,refName.c);
        ((SynObject*)ckd.succeed)->SetError(ckd,&ERR_NotYetInitialized,refName.c);
      }
      else if (!((TDOD*)((CHE*)refIDs.last)->data)->errorChain.first)
        SetError(ckd,&ERR_MissingInitialization,refName.c);
      ok1 = false;
    }
  }
  else {
    ENTRY
    ok1 = ok;

    ok1 &= UpdateReference(ckd);

    if (flags.Contains(brokenRef))
      ERROREXIT

    VarRefContext vrc=Context();
    switch (vrc) {
    case assignmentTarget:
    case copyTarget:
      ok1 &= AssignCheck(ckd,vrc);
      break;

    case inputParam:
      ok1 &= ReadCheck(ckd);
      break;

    case funcHandle:
      ok1 &= CallCheck(ckd);
      break;

    case arrayTarget:
      ok1 &= ArrayTargetCheck(ckd);
      break;

    }
  }

  if (InOldExpression()
  && ((flags.Contains(isLocalVar) && !flags.Contains(isSelfVar))
      || flags.Contains(isOutputVar))) {
    SetError(ckd,&ERR_LocalOrOutputVarInOld);
    ok1 = false;
  }

  return ok1;
}

bool TDOD::IsStateObject (CheckData &ckd)
{
  DWORD dw;
  TIDType idtype;
  LavaDECL *decl;
  Category cat;
  SynFlags ctxFlags;

  dw = ckd.document->IDTable.GetVar(ID,idtype,ckd.inINCL);
  if (idtype == globalID)
    return (*(LavaDECL**)dw)->TypeFlags.Contains(stateObject);
  else {
    ((VarName*)dw)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    return cat == stateObj;
  }
}

bool TDOD::accessTypeOK (SynFlags accessFlags)
{
  if (accessFlags.Contains(readRefs) && accessFlags.Contains(writeRefs))
    return true;
  if (parentObject->parentObject->primaryToken == FormParm_T)
    return false;
  switch (parentObject->parentObject->primaryToken) {
  case assign_T:
    if((parentObject->whereInParent == (address)&((Assignment*)parentObject->parentObject)->targetObj.ptr
        && accessFlags.Contains(writeRefs))
    || (parentObject->whereInParent != (address)&((Assignment*)parentObject->parentObject)->targetObj.ptr
        && accessFlags.Contains(readRefs)))
      return true;
    else
      return false;
  case parameter_T:
    if (parentObject->parentObject->parentObject->primaryToken == assignFS_T)
      if ((parentObject->parentObject->containingChain
            == &((FuncStatement*)parentObject->parentObject->parentObject)->outputs
          && accessFlags.Contains(writeRefs))
      || (parentObject->parentObject->containingChain
            == &((FuncStatement*)parentObject->parentObject->parentObject)->inputs
          && accessFlags.Contains(readRefs)))
        return true;
      else
        return false;
    else
      return false;
 default:
    if (accessFlags.Contains(readRefs))
      return true;
    else
      return false;
  }
  return true;
}

bool TDOD::ReplaceWithLocalParm(CheckData &ckd, LavaDECL *funcDecl,TDeclType declType)
{
  CHE *parm=(CHE*)funcDecl->NestedDecls.first;

  if (!ckd.execView || ckd.inInitialUpdate || !parm)
    return false;

  if (declType == OAttr)
    for (; ((LavaDECL*)parm->data)->DeclType == IAttr; parm = (CHE*)parm->successor);
  if (!parm || ((LavaDECL*)parm->data)->DeclType != declType)
    return false;
  for (; parm && ((LavaDECL*)parm->data)->DeclType == declType; parm = (CHE*)parm->successor) {
    if (name == ((LavaDECL*)parm->data)->LocalName) {
      ID = TID(((LavaDECL*)parm->data)->OwnID,((LavaDECL*)parm->data)->inINCL);
      return true;
    }
  }
  return false;
}

void VarName::ExprGetFVType (CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags)
{
  Quantifier *quant;
  TID tidSetEl;

  decl = 0;
  cat = unknownCategory;

  ckd.tempCtx = ckd.lpc;
  switch (parentObject->primaryToken) {
  case quant_T:
    quant = (Quantifier*)parentObject;
    if (quant->elemType.ptr
    && ((SynObject*)quant->elemType.ptr)->primaryToken == TypeRef_T) {
      decl = ckd.document->GetFinalMVType(((Reference*)quant->elemType.ptr)->refID,ckd.inINCL,ckd.tempCtx,cat,&ckd);
      if (cat == unknownCategory)
        if (((Reference*)quant->elemType.ptr)->flags.Contains(isVariable))
          cat = stateObj;
        else if (((Reference*)quant->elemType.ptr)->flags.Contains(isSameAsSelf))
          cat = sameAsSelfObj;
        else
          cat = valueObj;
      if (((Reference*)quant->elemType.ptr)->flags.Contains(isSubstitutable))
        ctxFlags = SET(multiContext,-1);
      else
        ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
    }
    else if (!quant->elemType.ptr
    && NoPH(quant->set.ptr)) {
      ((SynObject*)quant->set.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
      if (((SynObject*)quant->set.ptr)->primaryToken == intIntv_T
      || (decl && decl->SecondTFlags.Contains(isEnum)))
        return;
      ckd.document->IDTable.GetParamID(decl,tidSetEl,isSet); // Context/cat??
      if (ctxFlags.bits)
        ckd.tempCtx.ContextFlags = ctxFlags;
      decl = ckd.document->GetFinalMVType(tidSetEl,0,ckd.tempCtx,cat,&ckd);
			if (!ckd.tempCtx.ContextFlags.Contains(multiContext))
        ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);

    }
    break;
  case caseType_T:
    ((SynObject*)((TypeBranch*)parentObject)->exprType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  case catch_T:
    ((SynObject*)((CatchClause*)parentObject)->exprType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  case run_T:
    ((SynObject*)((Run*)parentObject)->initiator.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  case new_T:
    ((SynObject*)((NewExpression*)parentObject)->objType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  case clone_T:
    ((SynObject*)((CloneExpression*)parentObject)->fromObj.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  case initializing_T:
    ((SynObject*)((BaseInit*)parentObject)->baseItf.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
    break;
  default: ;
  }
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

Reference *VarName::TypeRef ()
{
  SynObject *type=0;

  if (!parentObject) return 0; // SelfVar
  switch (parentObject->primaryToken) {
  case quant_T:
    type = (SynObject*)((Quantifier*)parentObject)->elemType.ptr;
    break;
  case caseType_T:
    type = (SynObject*)((TypeBranch*)parentObject)->exprType.ptr;
    break;
  case new_T:
    type = (SynObject*)((NewExpression*)parentObject)->objType.ptr;
    break;
  default: ;
  }
  return (Reference*)type;
}

bool VarName::Check (CheckData &ckd)
{
  ENTRY
  if (primaryToken == VarName_T) {
  }
#ifdef INTERPRETER
  else
    SetError(ckd,&ERR_Placeholder);
#endif
  EXIT
}

void Constant::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[constType]);
  if (flags.Contains(isVariable))
    cat = stateObj;
  else if (flags.Contains(isSameAsSelf))
    cat = sameAsSelfObj;
  else
    cat = valueObj;
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = decl;
#endif
}

bool Constant::Check (CheckData &ckd) {
  ENTRY
#ifdef INTERPRETER
  typeDECL = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[constType]);
#endif
  EXIT
}

void BoolConst::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[B_Bool]);
  if (flags.Contains(isVariable))
    cat = stateObj;
  else if (flags.Contains(isSameAsSelf))
    cat = sameAsSelfObj;
  else
    cat = valueObj;
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = decl;
#endif
}

bool BoolConst::Check (CheckData &ckd) {
  ENTRY
#ifdef INTERPRETER
  typeDECL = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[B_Bool]);
#endif
  EXIT
}

void NullConst::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = (LavaDECL*)-1;
  cat = unknownCategory;
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = decl;
#endif
}

bool NullConst::Check (CheckData &ckd) {
  ENTRY
  flags.INCL(isOptionalExpr);
  EXIT
}

void EnumConst::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = ckd.document->IDTable.GetDECL(refID,ckd.inINCL);
  if (flags.Contains(isVariable))
    cat = stateObj;
  else if (flags.Contains(isSameAsSelf))
    cat = sameAsSelfObj;
  else
    cat = valueObj;
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool EnumConst::Check (CheckData &ckd) {
  ENTRY
  ok &= UpdateReference(ckd);
  EXIT
}

bool Assignment::Check (CheckData &ckd)
{
  LavaDECL *declSource, *declTarget, *targetDecl;
  Category catSource, catTarget;
  CContext sourceCtx, targetCtx;
  SynFlags ctxFlags;
  SynObject *targObj=(SynObject*)targetObj.ptr;
#ifdef INTERPRETER
  TIDType idtype;
  DWORD dw;
  CHE *pred;
  ObjReference *objRef;
#endif

  ENTRY

  if (ReadOnlyContext() == roClause) {
    SetError(ckd,&ERR_AssignInROClause);
    ERROREXIT
  }

  ok &= targObj->Check(ckd);
  // important for NullAdmissible check on exprValue:
  // check tarObj first (so targetObj/isOptionalExpr flag is updated)

  ckd.tempCtx = ckd.lpc;
  targObj->ExprGetFVType(ckd,declTarget,catTarget,ctxFlags);
  targetCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    targetCtx.ContextFlags = ctxFlags;
  if (((SynObject*)exprValue.ptr)->IsIfStmExpr()) {
    targetDecl = ckd.document->GetType(declTarget);
    ((CondExpression*)exprValue.ptr)->targetDecl = targetDecl;
    ((CondExpression*)exprValue.ptr)->targetCtx = targetCtx;
    ((CondExpression*)exprValue.ptr)->targetCat = catTarget;
    ((CondExpression*)exprValue.ptr)->callObjCat = catTarget;
  }

  ok &= ((SynObject*)exprValue.ptr)->Check(ckd);
  if (!ok)
    ERROREXIT

  if (((SynObject*)exprValue.ptr)->IsOptional(ckd)
  && !targObj->IsOptional(ckd)
  && !((SynObject*)exprValue.ptr)->IsDefChecked(ckd)) {
    ((SynObject*)exprValue.ptr)->SetError(ckd,&ERR_NotOptional);
    ok = false;
  }

  if (!((SynObject*)exprValue.ptr)->IsIfStmExpr()) {
    ckd.tempCtx = ckd.lpc;
    ((SynObject*)exprValue.ptr)->ExprGetFVType(ckd,declSource,catSource,ctxFlags);
    if (declSource != (LavaDECL*)-1) {
      if (!declSource || !declTarget)
        ERROREXIT

      sourceCtx = ckd.tempCtx;
      if (declTarget->TypeFlags.Contains(substitutable))
        targetCtx.ContextFlags = SET(multiContext,-1);
      if (ctxFlags.bits)
        sourceCtx.ContextFlags = ctxFlags;
      if (!compatibleTypes(ckd,declSource,sourceCtx,declTarget,targetCtx)) {
        ((SynObject*)exprValue.ptr)->SetError(ckd,ckd.errorCode);
        ok = false;
      }
      if (catSource != unknownCategory
      && catTarget != unknownCategory
      && catSource != catTarget) {
        ((SynObject*)exprValue.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
        ok = false;
      }
      if (NoPH(exprValue.ptr))
      ((Expression*)exprValue.ptr)->targetCat = catTarget;
    }
  }

#ifdef INTERPRETER
  declTarget = ckd.document->GetType(declTarget);
  if (!((SynObject*)exprValue.ptr)->IsIfStmExpr() && declSource != (LavaDECL*)-1) {
    declSource = ckd.document->GetType(declSource);
    ((Expression*)exprValue.ptr)->sectionNumber = ckd.document->GetSectionNumber(ckd, declSource,declTarget);
  }
  if (targObj->primaryToken == arrayAtIndex_T)
    kindOfTarget = arrayElem;
  else {
    dw = ckd.document->IDTable.GetVar(((TDOD*)((CHE*)((ObjReference*)targObj)->refIDs.last)->data)->ID,idtype,ckd.inINCL);
    switch (idtype) {
    case localID:
      kindOfTarget = field;
      break;
    default:
      declTarget = *(LavaDECL**)dw;
      if (declTarget->TypeFlags.Contains(hasSetGet))
        kindOfTarget = property;
      else
        kindOfTarget = field;
      if (declTarget->DeclType == Attr
      && !declTarget->TypeFlags.Contains(constituent)
      && !declTarget->TypeFlags.Contains(acquaintance))
        targObj->flags.INCL(isReverseLink);
    }
  }
  if (targObj->primaryToken == ObjRef_T) {
    objRef = (ObjReference*)targObj;
    pred = (CHE*)objRef->refIDs.last->predecessor;
    if (pred
    && (pred == (CHE*)objRef->refIDs.first)
    && objRef->flags.Contains(isReverseLink)
    && objRef->flags.Contains(isSelfVar))
      objRef->flags.INCL(unfinishedAllowed);
    // an unfinished object may be assigned only to a reverse link self.revLink
    // within an initializer
  }
#endif
  EXIT
}

void ArrayAtIndex::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  TID tidOp1, tidOperatorFunc, tidOutparm1;
  LavaDECL *declOutparm1, *declInparm2;
#ifndef INTERPRETER
  LavaDECL *declOp1;
#endif
  CHE *chpOutparm1, *chpInparm2;

  decl = 0;
  cat = unknownCategory;

#ifdef INTERPRETER
  tidOperatorFunc = opFunctionID;
  ADJUST4(tidOperatorFunc);
  if (tidOperatorFunc.nID == -1)
    return;
  funcDecl = ckd.document->IDTable.GetDECL(tidOperatorFunc,ckd.inINCL);
#else
  ((SynObject*)arrayObj.ptr)->ExprGetFVType(ckd,declOp1,cat,ctxFlags);
  declOp1 = ckd.document->GetType(declOp1);
  if (declOp1)
    if (setCase) {
      if (!ckd.document->GetOperatorID(declOp1,OP_arraySet,tidOperatorFunc))
        return;
    }
    else {
      if (!ckd.document->GetOperatorID(declOp1,OP_arrayGet,tidOperatorFunc))
        return;
    }
#endif
  chpOutparm1 = GetFirstOutput(&ckd.document->IDTable,tidOperatorFunc);
  ckd.tempCtx = callCtx;
  if (ctxFlags.bits)
    ckd.tempCtx.ContextFlags = ctxFlags;
  if (chpOutparm1) {
    declOutparm1 = (LavaDECL*)chpOutparm1->data;
    if (!declOutparm1)
      return;
    decl = ckd.document->GetFinalMVType(declOutparm1->RefID,declOutparm1->inINCL,ckd.tempCtx,cat,&ckd);
    if (declOutparm1->TypeFlags.Contains(substitutable))
      ctxFlags = SET(multiContext,-1);
    else
      ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
    if (cat == unknownCategory
    && declOutparm1->TypeFlags.Contains(trueObjCat))
      if (declOutparm1->TypeFlags.Contains(stateObject))
        cat = stateObj;
      else if (declOutparm1->TypeFlags.Contains(sameAsSelf))
        cat = sameAsSelfObj;
      else
        cat = valueObj;
  }
  else {
    chpInparm2 = GetFirstInput(&ckd.document->IDTable,tidOperatorFunc);
    if (!chpInparm2)
      return;
    chpInparm2 = (CHE*)chpInparm2->successor;
    declInparm2 = (LavaDECL*)chpInparm2->data;
    if (!declInparm2)
      return;
    decl = ckd.document->GetFinalMVType(declInparm2->RefID,declInparm2->inINCL,ckd.tempCtx,cat,&ckd);
    if (declInparm2->TypeFlags.Contains(substitutable))
      ctxFlags = SET(multiContext,-1);
    else
      ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
    if (cat == unknownCategory
    && declInparm2->TypeFlags.Contains(trueObjCat))
      if (declInparm2->TypeFlags.Contains(stateObject))
        cat = stateObj;
      else if (declInparm2->TypeFlags.Contains(sameAsSelf))
        cat = sameAsSelfObj;
      else
        cat = valueObj;
  }
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool ArrayAtIndex::Check (CheckData &ckd)
{
  Expression *opd2;
  TID opFuncID;
  LavaDECL  *objTypeDecl, *indexTypeDecl, *declOp2, *formDecl;
  CHE *chpFormIO;
  Category actCat, formCat, callObjCat;
  CContext xCtx1, xCtx2;
  SynFlags ctxFlags;
  Assignment *assig;
  FuncStatement *funcStm;

  ENTRY
  ok &= ((SynObject*)arrayIndex.ptr)->Check(ckd);

  setCase = false;
  if (parentObject->primaryToken == arrayAtIndex_T) {
    setCase = ((ArrayAtIndex*)parentObject)->setCase;
    if (setCase && parentObject->parentObject->primaryToken != arrayAtIndex_T)
      parentObject->SetError(ckd,&ERR_AssigToMultiArray);
  }
  else if (parentObject->primaryToken == assign_T) {
    assig = (Assignment*)parentObject;
    if (whereInParent == (address)&assig->targetObj.ptr)
      setCase = true;
  }
  else if (parentObject->primaryToken == parameter_T
  && parentObject->parentObject->primaryToken == assignFS_T) {
    funcStm = (FuncStatement*)parentObject->parentObject;
    if (parentObject->containingChain == &funcStm->outputs)
      setCase = true;
  }

  ok &= ((SynObject*)arrayObj.ptr)->Check(ckd);
  if (((SynObject*)arrayObj.ptr)->primaryToken != Exp_T
  && ((SynObject*)arrayObj.ptr)->primaryToken != ObjRef_T
  && ((SynObject*)arrayObj.ptr)->primaryToken != arrayAtIndex_T) {
    ok = false;
    ((SynObject*)arrayObj.ptr)->SetError(ckd,&ERR_FaultyArrayObj);
  }

  ((SynObject*)arrayObj.ptr)->ExprGetFVType(ckd,objTypeDecl,callObjCat,ctxFlags);
  objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);
  callCtx = ckd.tempCtx;
  xCtx1 = callCtx;
  if (ctxFlags.bits)
    xCtx1.ContextFlags = ctxFlags;
#ifdef INTERPRETER
  if (!objTypeDecl)
    ((SynObject*)arrayObj.ptr)->SetError(ckd,&ERR_UndefType);
  funcDecl = ckd.document->IDTable.GetDECL(opFunctionID,ckd.inINCL);
  if (!funcDecl)
    ((SynObject*)arrayObj.ptr)->SetError(ckd,&ERR_ArrOperatorUndefined);
  funcSectionNumber = ckd.document->GetMemsSectionNumber(ckd, objTypeDecl,funcDecl);
#else
  if (!objTypeDecl)
    ERROREXIT

  if (setCase) {
    if (!ckd.document->GetOperatorID(objTypeDecl,OP_arraySet,opFunctionID)) {
      SetError(ckd,&ERR_ArrOperatorUndefined);
      ERROREXIT
    }
  }
  else {
    if (!ckd.document->GetOperatorID(objTypeDecl,OP_arrayGet,opFunctionID)) {
      SetError(ckd,&ERR_ArrOperatorUndefined);
      ERROREXIT
    }
  }
#endif
  opd2 = (Expression*)arrayIndex.ptr;
  opd2->ExprGetFVType(ckd,indexTypeDecl,actCat,ctxFlags);
  xCtx2 = ckd.tempCtx;
  if (ctxFlags.bits)
    xCtx2.ContextFlags = ctxFlags;
  opFuncID = opFunctionID;
  ADJUST4(opFuncID);

  chpFormIO = GetFirstInput(&ckd.document->IDTable,opFuncID);
  declOp2 = (LavaDECL*)chpFormIO->data;
  if (!declOp2)
    ERROREXIT
  formDecl = ckd.document->GetFinalMVType(declOp2->RefID,declOp2->inINCL,xCtx1,formCat,&ckd);
  if (!compatibleTypes(ckd,indexTypeDecl,xCtx2,formDecl,xCtx1)) {
    opd2->SetError(ckd,ckd.errorCode);
    ERROREXIT
  }
  if (formCat == unknownCategory
  && declOp2->TypeFlags.Contains(trueObjCat))
    if (declOp2->TypeFlags.Contains(stateObject))
      formCat = stateObj;
    else if (declOp2->TypeFlags.Contains(sameAsSelf))
      formCat = sameAsSelfObj;
    else
      formCat = valueObj;

  if (actCat != unknownCategory
  && ((formCat == sameAsSelfObj && actCat != callObjCat)
  || (formCat != sameAsSelfObj && actCat != formCat))) {
    opd2->SetError(ckd,&ERR_IncompatibleCategory);
    ok = false;
  }
  if (NoPH(arrayIndex.ptr))
    ((Expression*)arrayIndex.ptr)->targetCat = formCat;

#ifdef INTERPRETER
  opd2->formVType = ckd.document->IDTable.GetDECL(declOp2->RefID,declOp2->inINCL);
  opd2->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, opd2->formVType, opd2->isOuter);
  formDecl = ckd.document->GetType(formDecl);
  indexTypeDecl = ckd.document->GetType(indexTypeDecl);
  opd2->sectionNumber = ckd.document->GetSectionNumber(ckd, indexTypeDecl,formDecl);
  if (setCase)
    chpFormIO = (CHE*)chpFormIO->successor;
  else
    chpFormIO = GetFirstOutput(&ckd.document->IDTable,opFuncID);
  formVType = (LavaDECL*)chpFormIO->data;
  ckd.tempCtx = callCtx;
  formVType = ckd.document->IDTable.GetDECL(formVType->RefID,formVType->inINCL);
  vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, formVType, isOuter);
#endif

  EXIT
}

void Parameter::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)parameter.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  if (decl && decl != (LavaDECL*)-1)
    finalType = ckd.document->GetType(decl);
  else
    finalType = 0;
#endif
}

bool Parameter::Check (CheckData &ckd)
{
  ENTRY
  if (ckd.execView)
    primaryTokenNode = ((SynObject*)parameter.ptr)->primaryTokenNode;
  ok = ((SynObject*)parameter.ptr)->Check(ckd);
  EXIT
}

static void reposition(CheckData &ckd,SynObject *func,bool isInput,CHAINX *chain,CHE *formParm,CHE *&actParm) {
  LavaDECL *decl=(LavaDECL*)formParm->data;
  TID formTid(decl->OwnID,0), actTid;
#ifndef INTERPRETER
  CHE *currActParm, *pred=(CHE*)chain->last;
  SynObject *placeHdr;
  ParameterV *parm;
#endif

  ADJUST(formTid,decl);
  if (actParm) {
    actTid = ((Parameter*)actParm->data)->formParmID;
    ADJUST4(actTid);
    if (actTid == formTid)
      return;
  }
#ifdef INTERPRETER
  if (actParm)
    ((SynObject*)actParm->data)->SetError(ckd,&ERR_Parameter);
  else
    func->SetError(ckd,&ERR_MissingParms);
#else
  if (actParm) {
    currActParm = (CHE*)actParm->successor;
    while (currActParm && ((ParameterV*)currActParm->data)->formParmID != formTid) {
      currActParm = (CHE*)currActParm->successor;
    }
    if (currActParm) {
      PutDelChainHint(ckd,func,chain,currActParm);
      PutInsChainHint(ckd,func,chain,currActParm,(CHE*)actParm->predecessor);
      actParm = currActParm;
      ((Parameter*)actParm->data)->Check(ckd);
      return;
    }
    else {
      if (isInput)
        placeHdr = new SynObjectV(Exp_T);
      else
        placeHdr = new SynObjectV(ObjPH_T);
      parm = new ParameterV(placeHdr);
      parm->formParmID = formTid;
      pred = (CHE*)actParm->predecessor;
      actParm = NewCHE(parm);
      PutInsChainHint(ckd,func,chain,actParm,pred);
    }
  }
  else {
    if (isInput)
      placeHdr = new SynObjectV(Exp_T);
    else
      placeHdr = new SynObjectV(ObjPH_T);
    parm = new ParameterV(placeHdr);
    parm->formParmID = formTid;
    actParm = NewCHE(parm);
    PutInsChainHint(ckd,func,chain,actParm,pred);
  }
#endif
}

void FuncExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  CHE *chpFormOut;
  TID tid2, funcTid;
  LavaDECL *callDecl, *outParmDecl;
  SynFlags callSynFlags;

  decl = 0;
  cat = unknownCategory;
  if (IsPH(function.ptr))
    return;

  funcTid = ((Reference*)function.ptr)->refID;
  ADJUST4(funcTid);
  chpFormOut = GetFirstOutput(&ckd.document->IDTable,funcTid);
  if (!chpFormOut)
    return;
  outParmDecl = (LavaDECL*)chpFormOut->data;
  ckd.tempCtx = callCtx;
  if (myCtxFlags.bits)
    ckd.tempCtx.ContextFlags = myCtxFlags;
  decl = ckd.document->GetFinalMVType(outParmDecl->RefID,outParmDecl->inINCL,ckd.tempCtx,cat,&ckd);
  if (cat == unknownCategory)
    if(outParmDecl->TypeFlags.Contains(stateObject))
      cat = stateObj;
    else if (outParmDecl->TypeFlags.Contains(sameAsSelf))
      ((SynObject*)handle.ptr)->ExprGetFVType(ckd,callDecl,cat,callSynFlags);
    else
      cat = valueObj;
  if (( (LavaDECL*)chpFormOut->data)->TypeFlags.Contains(substitutable))
    ctxFlags = SET(multiContext,-1);
  else
    ctxFlags = ckd.tempCtx.ContextFlags * SET(undefContext,-1);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool FuncExpression::Check (CheckData &ckd)
{
  CHE *chpActIn, *chpFormIn, *chpFormOut;
  SynObject *opd, *actParm;
  TID objTypeTid, selfTid, funcItfTid, funcTid;
  LavaDECL *funcItf, *funcImpl=0, *implItfDecl, *formInParmDecl;
  Expression *callExpr;
  Category cat;
  SynFlags ctxFlags;
  bool privateFunction=false;
#ifdef INTERPRETER
  unsigned nInputs, nOutputs;
  TID vTid;
  LavaDECL *actDecl;
#endif

  ENTRY

//  ((SynObject*)function.ptr)->Check(ckd);

  callExpr = (Expression*)handle.ptr;
  if (callExpr) {
    ok &= callExpr->Check(ckd);
    if (callExpr->IsOptional(ckd)
    && (callExpr->primaryToken != ObjRef_T || !callExpr->IsDefChecked(ckd))) {
      ((SynObject*)callExpr)->SetError(ckd,&ERR_NotOptional);
      ok = false;
    }
  }
#ifndef INTERPRETER
  if (callExpr) {
    if (!ok) {
      if (((SynObject*)function.ptr)->primaryToken == FuncPH_T) {
        ((SynObject*)function.ptr)->primaryToken = FuncDisabled_T;
        ((SynObject*)function.ptr)->flags.INCL(isDisabled);
      }
      ERROREXIT
    }
    else if (((SynObject*)function.ptr)->primaryToken == FuncDisabled_T) {
      ((SynObject*)function.ptr)->primaryToken = FuncPH_T;
      ((SynObject*)function.ptr)->flags.EXCL(isDisabled);
    }
  }
#endif
  if (callExpr) {
    ckd.tempCtx = ckd.lpc;
    ((Expression*)handle.ptr)->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
    objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);

#ifndef INTERPRETER
    if (!objTypeDecl) {
      if (!((Expression*)handle.ptr)->flags.Contains(brokenRef)
      && !IsPH(handle.ptr))
        ((SynObject*)handle.ptr)->SetError(ckd,&ERR_CallExprUndefType);
      if (((SynObject*)function.ptr)->primaryToken == FuncPH_T)
        ((SynObject*)function.ptr)->primaryToken = FuncDisabled_T;
    }
    else if (((SynObject*)function.ptr)->primaryToken == FuncDisabled_T) {
      ((SynObject*)function.ptr)->primaryToken = FuncPH_T;
      ((SynObject*)function.ptr)->flags.EXCL(isDisabled);
    }
#endif

    if (objTypeDecl) {
      callCtx = ckd.tempCtx;
      ckd.document->NextContext(objTypeDecl, callCtx);
      callObjCat = cat;
      if (callExpr->flags.Contains(isSelfVar)
      && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
        // Implementation required for self, rather than Interface
        objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

      if (objTypeDecl)
        objTypeTid = OWNID(objTypeDecl);
    }

    ok = ((SynObject*)function.ptr)->Check(ckd);
    if (!ok)
      ERROREXIT

    funcTid = ((Reference*)function.ptr)->refID;
    ADJUST4(funcTid);
    if (objTypeTid.nID != -1) {
      funcDecl = ckd.document->IDTable.GetDECL(funcTid);
      if (!funcDecl)
        ERROREXIT
      if (primaryToken == signal_T
        && !funcDecl->SecondTFlags.Contains(isLavaSignal)) {
        ((SynObject*)function.ptr)->SetError(ckd,&ERR_NoSignal);
        ok = false;
      }
      if (flags.Contains(staticCall)
      && !funcDecl->TypeFlags.Contains(isInitializer)
      && !(callExpr->flags.Contains(isSelfVar)
          && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)) {
        ((SynObject*)function.ptr)->SetError(ckd,&ERR_NoInitializer);
        ok = false;
      }
      if (funcDecl->TypeFlags.Contains(isStatic)) {
        ((SynObject*)handle.ptr)->SetError(ckd,&ERR_CallObjInStatic);
        ok = false;
      }
      if (funcDecl->TypeFlags.Contains(isInitializer)
      && !flags.Contains(staticCall)) {
        ((SynObject*)function.ptr)->SetError(ckd,&ERR_ExplicitInitializerCall);
        ok = false;
      }
      funcItf = funcDecl->ParentDECL;
      if (funcItf->DeclType == Impl) {
        funcImpl = funcItf;
        funcItf = ckd.document->IDTable.GetDECL(((CHETID*)funcItf->Supports.first)->data, funcItf->inINCL);
      }
      funcItfTid = OWNID(funcItf);
      if (!ckd.document->IDTable.IsAn(objTypeTid,0,funcItfTid,0)) {
        ((SynObject*)function.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
        ok = false;
      }
      if (funcDecl->TypeFlags.Contains(isProtected))
        if (ckd.selfTypeDECL->DeclType == Initiator) {
          ((SynObject*)function.ptr)->SetError(ckd,&ERR_Protected);
          ok = false;
        }
        else {
          implItfDecl = ckd.document->IDTable.GetDECL(((CHETID*)ckd.selfTypeDECL->Supports.first)->data,ckd.inINCL);
          if (!ckd.document->IDTable.IsAn(OWNID(objTypeDecl),0,OWNID(implItfDecl),0)) {
            ((SynObject*)function.ptr)->SetError(ckd,&ERR_Protected);
            ok = false;
          }
        }
      else if (funcImpl) {
        if (funcImpl != ckd.selfTypeDECL) {
          ((SynObject*)function.ptr)->SetError(ckd,&ERR_Private);
          ok = false;
        }
      }
    }
  }
  else { //static function
    ok = ((SynObject*)function.ptr)->Check(ckd);
    if (!ok)
      ERROREXIT
//    if (IsPH(function.ptr))
//      ERROREXIT

    callObjCat = unknownCategory;
    funcTid = ((Reference*)function.ptr)->refID;
    ADJUST4(funcTid);
    funcDecl = ckd.document->IDTable.GetDECL(funcTid);
    if (!funcDecl)
      ERROREXIT
    if (!funcDecl->TypeFlags.Contains(isStatic)) {
      ((SynObject*)function.ptr)->SetError(ckd,&ERR_CallExprRequired);
      ERROREXIT
    }
    objTypeDecl = funcDecl->ParentDECL;
    ckd.tempCtx = ckd.lpc;
    ckd.document->NextContext(objTypeDecl,ckd.tempCtx);
    callCtx = ckd.tempCtx;
  }

#ifdef INTERPRETER
  if (callExpr) {
    funcSectionNumber = ckd.document->GetMemsSectionNumber(ckd, objTypeDecl,funcDecl);
    if (flags.Contains(staticCall) && (vtypeID.nID > -1)) {
      vTid = vtypeID;
      ADJUST4(vTid);
      vBaseType = ckd.document->IDTable.GetDECL(vTid);
      vBaseSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, vBaseType, vBaseIsOuter);
    }
    else
      vBaseType = 0;
  }
  else
    ckd.document->CheckImpl(ckd, objTypeDecl);
  nOutputs = ((Reference*)function.ptr)->refDecl->nOutput;
#endif

  chpFormIn = GetFirstInput(&ckd.document->IDTable,funcTid);
  chpFormOut = GetFirstOutput(&ckd.document->IDTable,funcTid);
  if (parentObject->primaryToken == assignFX_T) {
    if (!chpFormOut
    || (chpFormOut->successor
        && ((LavaDECL*)((CHE*)chpFormOut->successor)->data)->DeclType == OAttr)) {
      ((SynObject*)function.ptr)->SetError(ckd,&ERR_NotSingleOutput);
      ok = false;
    }
  }
  if (chpFormOut)
    if (((LavaDECL*)chpFormOut->data)->TypeFlags.Contains(isOptional))
      flags.INCL(isOptionalExpr);
    else
      flags.EXCL(isOptionalExpr);

  CContext callContext = callCtx;
  if (myCtxFlags.bits)
    callContext.ContextFlags = myCtxFlags;
  if (parentObject->primaryToken != connect_T) {
    chpActIn = (CHE*)inputs.first;
    while (chpFormIn) {
      // locate act. parm. and reposition it if necessary:
      reposition(ckd,this,true,&inputs,chpFormIn,chpActIn);
      opd = (SynObject*)chpActIn->data;
      // check act. parm.:
      actParm =
        (opd->primaryToken==parameter_T?(SynObject*)((Parameter*)opd)->parameter.ptr : opd);
      if (!actParm->IsIfStmExpr())
        ok/*rc*/ &= opd->Check(ckd);
      // check act.parm/form.parm. type compatibility:
      ok &= compatibleInput(ckd,chpActIn,chpFormIn,callContext,callObjCat);
  //  parm = (SynObject*)((Parameter*)chpActIn->data)->parameter.ptr;
      formInParmDecl = (LavaDECL*)chpFormIn->data;
      if (opd->IsOptional(ckd)
      && !formInParmDecl->TypeFlags.Contains(isOptional)
      && !opd->IsDefChecked(ckd)) {
        ((SynObject*)opd)->SetError(ckd,&ERR_NotOptional);
        ok = false;
      } 
#ifdef INTERPRETER
      ((SynObject*)chpActIn->data)->ExprGetFVType(ckd,actDecl,cat,ctxFlags);
      ckd.tempCtx = callContext;
      ((Expression*)chpActIn->data)->formVType = ckd.document->IDTable.GetDECL(formInParmDecl->RefID,formInParmDecl->inINCL);
      ((Expression*)chpActIn->data)->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, ((Expression*)chpActIn->data)->formVType, ((Expression*)chpActIn->data)->isOuter);
      formInParmDecl = ckd.document->GetFinalMTypeAndContext(formInParmDecl->RefID,formInParmDecl->inINCL,ckd.tempCtx,&ckd);
      if (actDecl != (LavaDECL*)-1) { // "nothing"?
        actDecl = ckd.document->GetType(actDecl);
        ((Expression*)chpActIn->data)->sectionNumber = ckd.document->GetSectionNumber(ckd, actDecl,formInParmDecl);
      }
      if (callExpr && callExpr->flags.Contains(isDisabled)) // initializer call
        opd->flags.INCL(unfinishedAllowed);
#endif
      if (chpActIn)
        chpActIn = (CHE*)chpActIn->successor;
      chpFormIn = (CHE*)chpFormIn->successor;
      if (chpFormIn && ((LavaDECL*)chpFormIn->data)->DeclType != IAttr)
        chpFormIn = 0;
    }
    if (chpActIn)
#ifdef INTERPRETER
      SetError(ckd,&ERR_RedundantParms);
#else
    for ( ;
          chpActIn;
          chpActIn = (CHE*)chpActIn->successor)  // delete remainder of parameter chain
      PutDelChainHint(ckd,this,&inputs,chpActIn);
#endif
  }
#ifdef INTERPRETER
  nInputs = ((Reference*)function.ptr)->refDecl->nInput;
  nParams = nInputs + nOutputs + 1;
  if (nOutputs && (primaryToken == assignFX_T)) {
    chpFormOut = GetFirstOutput(&ckd.document->IDTable,funcTid);
    formVType = (LavaDECL*)chpFormOut->data;
    ckd.tempCtx = callContext;
    formVType = ckd.document->IDTable.GetDECL(formVType->RefID,formVType->inINCL);
    vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, formVType, isOuter);
  }
#endif

  EXIT
}

bool FuncStatement::Check (CheckData &ckd)
{
  CHE *chpActOut, *chpFormOut, *oldError1;
  Parameter *opd;
  SynFlags ctxFlags;
	bool visibleParms=false;
  TID funcTid;
  LavaDECL *formOutParmDecl;
#ifdef INTERPRETER
  Category cat;
  LavaDECL *actDecl, *formTypeDecl, *declTarget;
  SynObject *targetObj;
  DWORD dw;
  TIDType idtype;
#else
//  SynObjectV *objPHobj;
//  NullConst *nilObj;
#endif

  ENTRY

  oldError1 = oldError;
  ok &= FuncExpression::Check(ckd);
  oldError = oldError1;
  if (!function.ptr || IsPH(function.ptr))
    ERROREXIT

  CContext callContext = callCtx;
  if (myCtxFlags.bits)
    callContext.ContextFlags = myCtxFlags;
  funcTid = ((Reference*)function.ptr)->refID;
  ADJUST4(funcTid);
  chpFormOut = GetFirstOutput(&ckd.document->IDTable,funcTid);
  chpActOut = (CHE*)outputs.first;
  while (chpFormOut) {
    reposition(ckd,this,false,&outputs,chpFormOut,chpActOut);
    if (!((SynObject*)((Parameter*)chpActOut->data)->parameter.ptr)->flags.Contains(ignoreSynObj)) {
      // locate act. parm. and reposition it if necessary:
      opd = (Parameter*)chpActOut->data;
      bool rc = opd->Check(ckd);
      if (rc) {
        ok &= rc;
      // check act.parm/form.parm. type compatibility:
        ok &= compatibleOutput(ckd,chpActOut,chpFormOut,callContext,callObjCat);
        formOutParmDecl = (LavaDECL*)chpFormOut->data;
        if (formOutParmDecl->TypeFlags.Contains(isOptional)
        && !opd->IsOptional(ckd)) {
          ((SynObject*)opd)->SetError(ckd,&ERR_NotOptional);
          ok = false;
        } 
#ifdef INTERPRETER
        targetObj = (SynObject*)((Parameter*)chpActOut->data)->parameter.ptr;
        targetObj->ExprGetFVType(ckd,actDecl,cat,ctxFlags);
        if (actDecl != (LavaDECL*)-1) {
          actDecl = ckd.document->GetType(actDecl);
          formTypeDecl = (LavaDECL*)chpFormOut->data;
          ckd.tempCtx = callContext;
          ((Expression*)chpActOut->data)->formVType = ckd.document->IDTable.GetDECL(formTypeDecl->RefID,formTypeDecl->inINCL);
          ((Expression*)chpActOut->data)->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, callCtx, ((Expression*)chpActOut->data)->formVType, ((Expression*)chpActOut->data)->isOuter);
          formTypeDecl = ckd.document->GetFinalMTypeAndContext(formTypeDecl->RefID,formTypeDecl->inINCL,ckd.tempCtx,&ckd);
          ((Expression*)chpActOut->data)->sectionNumber = ckd.document->GetSectionNumber(ckd, formTypeDecl,actDecl);

          if (targetObj->primaryToken == arrayAtIndex_T)
            ((Parameter*)chpActOut->data)->kindOfTarget = arrayElem;
          else {
            if (targetObj->primaryToken != ObjPH_T) {
              dw = ckd.document->IDTable.GetVar(((TDOD*)((CHE*)((ObjReference*)targetObj)->refIDs.last)->data)->ID,idtype,ckd.inINCL);
              switch (idtype) {
              case localID:
                ((Parameter*)chpActOut->data)->kindOfTarget = field;
                break;
              default:
                ObjReference *parm = (ObjReference*)((Parameter*)chpActOut->data)->parameter.ptr;
                declTarget = *(LavaDECL**)dw;
                if (declTarget->TypeFlags.Contains(hasSetGet))
                  ((Parameter*)chpActOut->data)->kindOfTarget = property;
                else
                  ((Parameter*)chpActOut->data)->kindOfTarget = field;
                if (!declTarget->TypeFlags.Contains(constituent)
                && !declTarget->TypeFlags.Contains(acquaintance))
                  ((Parameter*)chpActOut->data)->flags.INCL(isReverseLink);
              }
            }
          }
        }
#endif
      }
    }
    if (chpActOut)
      chpActOut = (CHE*)chpActOut->successor;
    chpFormOut = (CHE*)chpFormOut->successor;
    if (chpFormOut && ((LavaDECL*)chpFormOut->data)->DeclType != OAttr)
      chpFormOut = 0;
  }
  if (chpActOut)
#ifdef INTERPRETER
    SetError(ckd,&ERR_RedundantParms);
#else
  for ( ;
        chpActOut;
        chpActOut = (CHE*)chpActOut->successor)  // delete remainder of parameter chain
    PutDelChainHint(ckd,this,&outputs,chpActOut);
#endif

  EXIT
}

static QString slotWrongArg;

bool Connect::Check (CheckData &ckd)
{
  TID objTypeTid, senderClassTid, selfTid, funcItfTid, funcTid;
  LavaDECL *sigDecl, *callbackDecl, *senderClass, *funcItf, *funcImpl=0;
  Expression *callExpr;
  LavaDECL *objTypeDecl=0;
  Category cat;
  CContext sigCtx, callbackCtx;
  SynFlags myCtxFlags;
  bool senderClassOK=false;

  ENTRY

  callExpr = (Expression*)signalSender.ptr;
  if (callExpr) {
    ok &= ((SynObject*)signalSender.ptr)->Check(ckd);
    ckd.tempCtx = ckd.lpc;
    callExpr->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
    objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);
  }
  else {
    senderClassOK = ((Reference*)signalSenderClass.ptr)->Check(ckd);
    ok &= senderClassOK;
    if (senderClassOK)
      senderClass = ((Reference*)signalSenderClass.ptr)->refDecl;
  }

  if (!objTypeDecl && !senderClassOK) {
    if (callExpr
    && !IsPH(callExpr)
    && !callExpr->flags.Contains(brokenRef))
      callExpr->SetError(ckd,&ERR_CallExprUndefType);
#ifndef INTERPRETER
    if (((SynObject*)signalFunction.ptr)->primaryToken == FuncPH_T) {
      ((SynObject*)signalFunction.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)signalFunction.ptr)->flags.INCL(isDisabled);
    }
#endif
  }
#ifndef INTERPRETER
  else {
    if (((SynObject*)signalFunction.ptr)->primaryToken == FuncDisabled_T) {
      ((SynObject*)signalFunction.ptr)->primaryToken = FuncPH_T;
      ((SynObject*)signalFunction.ptr)->flags.EXCL(isDisabled);
    }
  }
#endif

  if (objTypeDecl) {
    sigCtx = ckd.tempCtx;
    ckd.document->NextContext(objTypeDecl, sigCtx);
    if (callExpr->flags.Contains(isSelfVar)
    && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
      // Implementation required for self, rather than Interface
      objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

    if (objTypeDecl)
      objTypeTid = OWNID(objTypeDecl);
  }


// en|disable callback specification:
  ok &= ((SynObject*)signalFunction.ptr)->Check(ckd);
#ifndef INTERPRETER
  if (!ok) {
    if (((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->IsPlaceHolder()) {
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.INCL(isDisabled);
    }
    ERROREXIT
  }
  else
    ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.EXCL(isDisabled);
#endif

  funcTid = ((Reference*)signalFunction.ptr)->refID;
  ADJUST4(funcTid);
  sigDecl = ckd.document->IDTable.GetDECL(funcTid);
  if (!sigDecl) {
    ((SynObject*)signalFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
#ifndef INTERPRETER
    if (((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->IsPlaceHolder()) {
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.INCL(isDisabled);
    }
#endif
    ERROREXIT
  }
  else {
    if (!sigDecl->SecondTFlags.Contains(isLavaSignal)) {
      ((Reference*)signalFunction.ptr)->SetError(ckd,&ERR_NoSignal);
#ifndef INTERPRETER
      if (((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->IsPlaceHolder()) {
        ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
        ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.INCL(isDisabled);
      }
#endif
      ERROREXIT
    }
    if (objTypeTid.nID == -1) {
      funcItf = sigDecl->ParentDECL;
      if (funcItf->DeclType == Impl)
        funcItf = ckd.document->IDTable.GetDECL(((CHETID*)funcItf->Supports.first)->data, funcItf->inINCL);
      funcItfTid = OWNID(funcItf);
      senderClassTid = OWNID(senderClass);
      if (!ckd.document->IDTable.IsAn(senderClassTid,0,funcItfTid,0)) {
        ((SynObject*)signalFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
#ifndef INTERPRETER
        if (((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->IsPlaceHolder()) {
          ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
          ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.INCL(isDisabled);
        }
#endif
        ok = false;
      }
    }
    else {
      funcItf = sigDecl->ParentDECL;
      if (funcItf->DeclType == Impl)
        funcItf = ckd.document->IDTable.GetDECL(((CHETID*)funcItf->Supports.first)->data, funcItf->inINCL);
      funcItfTid = OWNID(funcItf);
      if (!ckd.document->IDTable.IsAn(objTypeTid,0,funcItfTid,0)) {
        ((SynObject*)signalFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
#ifndef INTERPRETER
        if (((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->IsPlaceHolder()) {
          ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
          ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->flags.INCL(isDisabled);
        }
#endif
        ok = false;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////////////

  callExpr = (Expression*)((FuncStatement*)callback.ptr)->handle.ptr;
  ok &= callExpr->Check(ckd);
  ckd.tempCtx = ckd.lpc;
  callExpr->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
  objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);

  ok &= ((SynObject*)callback.ptr)->Check(ckd);
  if (!ok || !objTypeDecl)
    ERROREXIT

  callbackCtx = ckd.tempCtx;
  ckd.document->NextContext(objTypeDecl, callbackCtx);
  if (callExpr->flags.Contains(isSelfVar)
  && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
    // Implementation required for self, rather than Interface
    objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

  if (objTypeDecl)
    objTypeTid = OWNID(objTypeDecl);
  funcTid = ((Reference*)((FuncStatement*)callback.ptr)->function.ptr)->refID;
  ADJUST4(funcTid);
  if (objTypeTid.nID != -1)
    callbackDecl = ckd.document->IDTable.GetDECL(funcTid);

///////////////////////////////////////////////////////////////////////////////////

// check whether slot is compatible with signal

  ok &= slotFunction(ckd,callbackDecl,callbackCtx,sigDecl,sigCtx);
  if (!ok) {
    if (ckd.errorCode == &ERR_SlotWrongArg) {
      slotWrongArg = ERR_SlotWrongArg.arg(ckd.iArg);
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->SetError(ckd,&slotWrongArg);
    }
    else
      ((SynObject*)((FuncStatement*)callback.ptr)->function.ptr)->SetError(ckd,ckd.errorCode);
    ERROREXIT
  }

  EXIT
}

bool Disconnect::Check (CheckData &ckd)
{
  TID objTypeTid, selfTid, funcItfTid, funcTid;
  LavaDECL *sigDecl, *callbackDecl, *funcItf;
  Expression *callExpr;
  LavaDECL *objTypeDecl;
  Category cat;
  CContext sigCtx, callbackCtx;
  SynFlags myCtxFlags;

  ENTRY

  ok &= ((SynObject*)signalSender.ptr)->Check(ckd);

  callExpr = (Expression*)signalSender.ptr;
  ckd.tempCtx = ckd.lpc;
  callExpr->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
  objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);

  if (!objTypeDecl) {
    if (callExpr->primaryToken != nil_T
    && !callExpr->flags.Contains(brokenRef)
    && !IsPH(callExpr))
      callExpr->SetError(ckd,&ERR_CallExprUndefType);
    if (callExpr->primaryToken == nil_T) {
      if (((SynObject*)signalFunction.ptr)->primaryToken == FuncDisabled_T) {
        ((SynObject*)signalFunction.ptr)->primaryToken = FuncPH_T;
        ((SynObject*)signalFunction.ptr)->flags.EXCL(isDisabled);
      }
    }
    else if (((SynObject*)signalFunction.ptr)->primaryToken == FuncPH_T) {
      ((SynObject*)signalFunction.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)signalFunction.ptr)->flags.INCL(isDisabled);
    }
  }
  else if (((SynObject*)signalFunction.ptr)->primaryToken == FuncDisabled_T) {
    ((SynObject*)signalFunction.ptr)->primaryToken = FuncPH_T;
    ((SynObject*)signalFunction.ptr)->flags.EXCL(isDisabled);
  }

  if (objTypeDecl) {
    sigCtx = ckd.tempCtx;
    ckd.document->NextContext(objTypeDecl, sigCtx);
    if (callExpr->flags.Contains(isSelfVar)
    && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
      // Implementation required for self, rather than Interface
      objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

    if (objTypeDecl)
      objTypeTid = OWNID(objTypeDecl);
  }


// en|disable callback specification:

  ok &= ((SynObject*)signalFunction.ptr)->Check(ckd);
  if (!ok) {
    if (((SynObject*)callbackFunction.ptr)->IsPlaceHolder())
      ((SynObject*)callbackFunction.ptr)->primaryToken = FuncDisabled_T;
    ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
    ERROREXIT
  }
  else
    ((SynObject*)callbackFunction.ptr)->flags.EXCL(isDisabled);


  if (((SynObject*)signalFunction.ptr)->primaryToken != nil_T) {
    funcTid = ((Reference*)signalFunction.ptr)->refID;
    ADJUST4(funcTid);
  }
  sigDecl = ckd.document->IDTable.GetDECL(funcTid);
  if (((SynObject*)signalFunction.ptr)->primaryToken != nil_T && !sigDecl) {
    ((SynObject*)signalFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
    if (((SynObject*)callbackFunction.ptr)->IsPlaceHolder())
      ((SynObject*)callbackFunction.ptr)->primaryToken = FuncDisabled_T;
    ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
    ERROREXIT
  }
  if (sigDecl) {
    if (!sigDecl->SecondTFlags.Contains(isLavaSignal)) {
      ((Reference*)signalFunction.ptr)->SetError(ckd,&ERR_NoSignal);
      if (((SynObject*)callbackFunction.ptr)->IsPlaceHolder())
        ((SynObject*)callbackFunction.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
      ERROREXIT
    }
    if (objTypeTid.nID != -1) {
      funcItf = sigDecl->ParentDECL;
      if (funcItf->DeclType == Impl) {
        funcItf = ckd.document->IDTable.GetDECL(((CHETID*)funcItf->Supports.first)->data, funcItf->inINCL);
      }
      funcItfTid = OWNID(funcItf);
      if (!ckd.document->IDTable.IsAn(objTypeTid,0,funcItfTid,0)) {
        ((SynObject*)signalFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
        if (((SynObject*)callbackFunction.ptr)->IsPlaceHolder())
          ((SynObject*)callbackFunction.ptr)->primaryToken = FuncDisabled_T;
        ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
        ok = false;
      }
    }
  }


////////////////////////////////////////////////////////////////////////////////////////

  ok &= ((SynObject*)signalReceiver.ptr)->Check(ckd);

  callExpr = (Expression*)signalReceiver.ptr;
  ckd.tempCtx = ckd.lpc;
  callExpr->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
  objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);

  if (!objTypeDecl) {
    if (callExpr->primaryToken != nil_T
    && !callExpr->flags.Contains(brokenRef)
    && !IsPH(callExpr))
      callExpr->SetError(ckd,&ERR_CallExprUndefType);
    if (callExpr->primaryToken != nil_T
    && ((SynObject*)callbackFunction.ptr)->primaryToken == FuncPH_T) {
      ((SynObject*)callbackFunction.ptr)->primaryToken = FuncDisabled_T;
      ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
    }
  }
  else if (((SynObject*)callbackFunction.ptr)->primaryToken == FuncDisabled_T) {
    ((SynObject*)callbackFunction.ptr)->primaryToken = FuncPH_T;
    ((SynObject*)callbackFunction.ptr)->flags.EXCL(isDisabled);
  }

  if (objTypeDecl) {
    callbackCtx = ckd.tempCtx;
    ckd.document->NextContext(objTypeDecl, callbackCtx);
    if (callExpr->flags.Contains(isSelfVar)
    && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
      // Implementation required for self, rather than Interface
      objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

    if (objTypeDecl)
      objTypeTid = OWNID(objTypeDecl);
  }
  else
    objTypeTid.nID = -1;

  ok &= ((SynObject*)callbackFunction.ptr)->Check(ckd);
  if (!ok)
    ERROREXIT

  if (((SynObject*)callbackFunction.ptr)->primaryToken != nil_T) {
    funcTid = ((Reference*)callbackFunction.ptr)->refID;
    ADJUST4(funcTid);
  }
  else
    funcTid.nID = -1;
  if (objTypeTid.nID != -1) {
    callbackDecl = ckd.document->IDTable.GetDECL(funcTid);
    if (((SynObject*)callbackFunction.ptr)->primaryToken != nil_T && !callbackDecl) {
      ((SynObject*)callbackFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
      ((SynObject*)callbackFunction.ptr)->flags.INCL(isDisabled);
      ERROREXIT
    }
    else
      ((SynObject*)callbackFunction.ptr)->flags.EXCL(isDisabled);

    if (callbackDecl) {
      if (callbackDecl->TypeFlags.Contains(isInitializer)) {
        ((Reference*)callbackFunction.ptr)->SetError(ckd,&ERR_InitializerForbidden);
        ok = false;
      }
      funcItf = callbackDecl->ParentDECL;
      if (funcItf->DeclType == Impl) {
        funcItf = ckd.document->IDTable.GetDECL(((CHETID*)funcItf->Supports.first)->data, funcItf->inINCL);
      }
      funcItfTid = OWNID(funcItf);
      if (!ckd.document->IDTable.IsAn(objTypeTid,0,funcItfTid,0)) {
        ((SynObject*)callbackFunction.ptr)->SetError(ckd,&ERR_MissingFuncDecl);
        ok = false;
      }
    }
  }

  if (!ok)
    ERROREXIT

///////////////////////////////////////////////////////////////////////////////////

// check whether slot is compatible with signal

  if (((SynObject*)signalFunction.ptr)->primaryToken != nil_T
  && ((SynObject*)callbackFunction.ptr)->primaryToken != nil_T) {
    ok &= slotFunction(ckd,callbackDecl,callbackCtx,sigDecl,sigCtx);
    if (!ok) {
      if (ckd.errorCode == &ERR_SlotWrongArg) {
        slotWrongArg = ERR_SlotWrongArg.arg(ckd.iArg);
        ((SynObject*)callbackFunction.ptr)->SetError(ckd,&slotWrongArg);
      }
      else
        ((SynObject*)callbackFunction.ptr)->SetError(ckd,ckd.errorCode);
      ERROREXIT
    }
  }

  EXIT
}

bool Signal::Check (CheckData &ckd)
{
  TID objTypeTid, selfTid, funcItfTid, funcTid;
  LavaDECL *sigDecl;
  Expression *callExpr;
  LavaDECL *objTypeDecl;
  Category cat;
  CContext sigCtx, callbackCtx;
  SynFlags myCtxFlags;

  ENTRY

  ok &= ((FuncStatement*)sCall.ptr)->Check(ckd);
  if (!ok)
    ERROREXIT

  callExpr = (Expression*)((FuncStatement*)sCall.ptr)->handle.ptr;
  ckd.tempCtx = ckd.lpc;
  callExpr->ExprGetFVType(ckd,objTypeDecl,cat,myCtxFlags);
  objTypeDecl = ckd.document->GetTypeAndContext(objTypeDecl,ckd.tempCtx);

  if (!objTypeDecl) {
    if (!callExpr->flags.Contains(brokenRef)
    && !IsPH(callExpr))
      callExpr->SetError(ckd,&ERR_CallExprUndefType);
    if (((SynObject*)((FuncStatement*)sCall.ptr)->function.ptr)->primaryToken == FuncPH_T)
      ((SynObject*)((FuncStatement*)sCall.ptr)->function.ptr)->primaryToken = FuncDisabled_T;
  }
  else if (((SynObject*)((FuncStatement*)sCall.ptr)->function.ptr)->primaryToken == FuncDisabled_T) {
    ((SynObject*)((FuncStatement*)sCall.ptr)->function.ptr)->primaryToken = FuncPH_T;
    ((SynObject*)((FuncStatement*)sCall.ptr)->function.ptr)->flags.EXCL(isDisabled);
  }
  if (objTypeDecl) {
    sigCtx = ckd.tempCtx;
    ckd.document->NextContext(objTypeDecl, sigCtx);
    if (callExpr->flags.Contains(isSelfVar)
    && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
      // Implementation required for self, rather than Interface
      objTypeDecl = ckd.document->IDTable.GetDECL(((SelfVar*)ckd.selfVar)->typeID,ckd.inINCL);

    if (objTypeDecl)
      objTypeTid = OWNID(objTypeDecl);
  }

  funcTid = ((Reference*)((FuncStatement*)sCall.ptr)->function.ptr)->refID;
  ADJUST4(funcTid);
  if (objTypeTid.nID != -1) {
    sigDecl = ckd.document->IDTable.GetDECL(funcTid);
    if (!sigDecl)
      ERROREXIT
    if (!sigDecl->SecondTFlags.Contains(isLavaSignal)) {
      ((Reference*)((FuncStatement*)sCall.ptr)->function.ptr)->SetError(ckd,&ERR_NoSignal);
      ok = false;
    }
  }

  EXIT
}

bool IfThen::Check (CheckData &ckd)
{
  SET oldFlags=ckd.flags;

  ENTRY
  ckd.flags.INCL(InIfCond);
  ok &= ((SynObject*)ifCondition.ptr)->Check(ckd);
  ckd.flags = oldFlags;
  ok &= ((SynObject*)thenPart.ptr)->Check(ckd);
  EXIT
}

bool IfStatement::Check (CheckData &ckd)
{
  CHE *chp, *branchStm, *precedingBranch;
  SynObject *opd;

  ENTRY

  ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);

  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor) {
    if (chp != (CHE*)ifThens.first)
      ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    opd = (SynObject*)chp->data;
    ok &= opd->Check(ckd);
  }

  ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
  if (elsePart.ptr)
    ok &= ((SynObject*)elsePart.ptr)->Check(ckd);

  ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);

  EXIT
}

bool IfxThen::Check (CheckData &ckd)
{
  SET oldFlags=ckd.flags;

  ENTRY
  ckd.flags += SET(InIfCond,InBranch,-1);
  ok &= ((SynObject*)ifCondition.ptr)->Check(ckd);
  ckd.flags = oldFlags;
  ok &= ((SynObject*)thenPart.ptr)->Check(ckd);
  EXIT
}

bool IfdefStatement::Check (CheckData &ckd)
{
  CHE *chp, *branchStm, *precedingBranch;
  ObjReference *opd;

  ENTRY

  ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);

  for (chp = (CHE*)ifCondition.first;
       chp;
       chp = (CHE*)chp->successor) {
    opd = (ObjReference*)chp->data;
    ok &= opd->Check(ckd);
    if (!IsPH(opd) && !opd->flags.Contains(isOptionalExpr)) {
      ((SynObject*)((CHE*)opd->refIDs.last)->data)->SetError(ckd,&ERR_SuperfluousMandatory);
      ok = false;
    }
  }
  if (thenPart.ptr)
    ok &= ((SynObject*)thenPart.ptr)->Check(ckd);

  ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
  if (elsePart.ptr)
    ok &= ((SynObject*)elsePart.ptr)->Check(ckd);

  ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);

  EXIT
}

bool IfdefStatement::Checks (ObjReference &objRef) {
  CHE *chp;

  for (chp=(CHE*)ifCondition.first; chp; chp=(CHE*)chp->successor)
    if (((SynObject*)chp->data)->primaryToken == ObjRef_T && *(ObjReference*)chp->data == objRef)
      return true;

  return false;
}

void IfExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags)
{
  decl = 0;
  /*if (!parentObject->IsBinaryOp()
  || whereInParent != (address)&((BinaryOp*)parentObject)->operand1.ptr)
    NullAdmissible(ckd);*/
#ifdef INTERPRETER
  finalType = 0;
#endif
}

bool IfExpression::Check (CheckData &ckd)
{
  CHE *chp, *chpFirst;
  IfxThen *opd;
  LavaDECL *currentBranchType;
  Category cat;
  bool first=true;
  SynFlags ctxFlags;
  CContext sourceCtx;

  ENTRY

/*  if (parentObject->primaryToken != assign_T
  && parentObject->primaryToken != elsif_T
  && parentObject->primaryToken != parameter_T
  && !parentObject->IsBinaryOp()
  && !parentObject->IsMultOp()) {
    SetError(ckd,&ERR_IfxForbidden);
    ERROREXIT
  }*/

  if (!targetDecl)
    return true;

  flags.EXCL(isOptionalExpr);
  chpFirst = (CHE*)ifThens.first;
  for (chp = chpFirst;
       chp;
       chp = (CHE*)chp->successor) {
    opd = (IfxThen*)chp->data;
    if (((SynObject*)opd->thenPart.ptr)->IsIfStmExpr()) {
      ((CondExpression*)opd->thenPart.ptr)->targetDecl = targetDecl;
      ((CondExpression*)opd->thenPart.ptr)->targetCtx = targetCtx;
      ((CondExpression*)opd->thenPart.ptr)->targetCat = targetCat;
      ((CondExpression*)opd->thenPart.ptr)->callObjCat = callObjCat;
    }
    ok &= opd->Check(ckd);
    if (((SynObject*)opd->thenPart.ptr)->IsOptional(ckd))
      flags.INCL(isOptionalExpr);
    if (!((SynObject*)opd->thenPart.ptr)->IsIfStmExpr()) {
      ckd.tempCtx = ckd.lpc;
      ((SynObject*)opd->thenPart.ptr)->ExprGetFVType(ckd,currentBranchType,cat,ctxFlags);
      if (currentBranchType == (LavaDECL*)(-1)) {
        if (((SynObject*)opd->thenPart.ptr)->NullAdmissible(ckd)) // "nothing" admissible?
          continue;
        else {
          ((SynObject*)opd->thenPart.ptr)->SetError(ckd,&ERR_NotOptional);
          ok = false;
          continue;
        }
      }
      sourceCtx = ckd.tempCtx;
      if (ctxFlags.bits)
        sourceCtx.ContextFlags = ctxFlags;
      if (!compatibleTypes(ckd,currentBranchType,sourceCtx,targetDecl,targetCtx)) {
        ((SynObject*)opd->thenPart.ptr)->SetError(ckd,ckd.errorCode);
        ok = false;
      }
      if (cat != unknownCategory
      && ((targetCat == sameAsSelfObj && cat != callObjCat)
          || (targetCat != sameAsSelfObj && cat != targetCat))) {
        ((SynObject*)opd->thenPart.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
        ok = false;
      }
      if (NoPH(opd->thenPart.ptr))
        ((Expression*)opd->thenPart.ptr)->targetCat = targetCat;
    }
  }

  if (elsePart.ptr) {
    if (((SynObject*)elsePart.ptr)->IsIfStmExpr()) {
      ((CondExpression*)elsePart.ptr)->targetDecl = targetDecl;
      ((CondExpression*)elsePart.ptr)->targetCtx = targetCtx;
      ((CondExpression*)elsePart.ptr)->targetCat = targetCat;
      ((CondExpression*)elsePart.ptr)->callObjCat = callObjCat;
    }
    ok &= ((SynObject*)elsePart.ptr)->Check(ckd);
    if (!((SynObject*)elsePart.ptr)->IsIfStmExpr()) {
      ckd.tempCtx = ckd.lpc;
      ((SynObject*)elsePart.ptr)->ExprGetFVType(ckd,currentBranchType,cat,ctxFlags);
      if (currentBranchType == (LavaDECL*)(-1)) {
        if (((SynObject*)elsePart.ptr)->NullAdmissible(ckd)) // "nothing" admissible?
          return true;
        else {
          ((SynObject*)elsePart.ptr)->SetError(ckd,&ERR_NotOptional);
          ERROREXIT
        }
      }
      if (((SynObject*)elsePart.ptr)->IsOptional(ckd))
        flags.INCL(isOptionalExpr);
      sourceCtx = ckd.tempCtx;
      if (ctxFlags.bits)
        sourceCtx.ContextFlags = ctxFlags;
      if (!compatibleTypes(ckd,currentBranchType,sourceCtx,targetDecl,targetCtx)) {
        ((SynObject*)elsePart.ptr)->SetError(ckd,ckd.errorCode);
        ok = false;
      }
      if (cat != unknownCategory
      && ((targetCat == sameAsSelfObj && cat != callObjCat)
          || (targetCat != sameAsSelfObj && cat != targetCat))) {
        ((SynObject*)elsePart.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
        ok = false;
      }
      if (NoPH(elsePart.ptr))
        ((Expression*)elsePart.ptr)->targetCat = targetCat;
    }
  }

  EXIT
}

void ElseExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags)
{
  decl = 0;
  /*if (!parentObject->IsBinaryOp()
  || whereInParent != (address)&((BinaryOp*)parentObject)->operand1.ptr)
    NullAdmissible(ckd);*/
#ifdef INTERPRETER
  finalType = 0;
#endif
}

bool ElseExpression::Check (CheckData &ckd)
{
  Expression *opd1, *opd2;
  LavaDECL *currentBranchType;
  Category cat;
  SynFlags ctxFlags;
  CContext sourceCtx;

  ENTRY

  opd1 = (Expression*)expr1.ptr;
  opd2 = (Expression*)expr2.ptr;
  ok &= opd1->Check(ckd);
  ok &= opd2->Check(ckd);
  if (IsPH(opd1) || !opd1->IsOptional(ckd) || IsPH(opd2) || !opd2->IsOptional(ckd))
    flags.EXCL(isOptionalExpr);
  else
    flags.INCL(isOptionalExpr);

  if (!targetDecl)
    return true;

  if (opd1->IsIfStmExpr()) {
    ((CondExpression*)opd1)->targetDecl = targetDecl;
    ((CondExpression*)opd1)->targetCtx = targetCtx;
    ((CondExpression*)opd1)->targetCat = targetCat;
    ((CondExpression*)opd1)->callObjCat = callObjCat;
  }
  if (!opd1->IsIfStmExpr()) {
    ckd.tempCtx = ckd.lpc;
    opd1->ExprGetFVType(ckd,currentBranchType,cat,ctxFlags);
    if (currentBranchType == (LavaDECL*)(-1)) {
      if (opd1->NullAdmissible(ckd)) // "nothing" admissible?
        return true;
      else {
        opd1->SetError(ckd,&ERR_NotOptional);
        ERROREXIT
      }
    }
    sourceCtx = ckd.tempCtx;
    if (ctxFlags.bits)
      sourceCtx.ContextFlags = ctxFlags;
    if (!compatibleTypes(ckd,currentBranchType,sourceCtx,targetDecl,targetCtx)) {
      opd1->SetError(ckd,ckd.errorCode);
      ok = false;
    }
    if (cat != unknownCategory
    && ((targetCat == sameAsSelfObj && cat != callObjCat)
        || (targetCat != sameAsSelfObj && cat != targetCat))) {
      opd1->SetError(ckd,&ERR_IncompatibleCategory);
      ok = false;
    }
    if (NoPH(opd1))
      opd1->targetCat = targetCat;
  }

  if (opd2->IsIfStmExpr()) {
    ((CondExpression*)opd2)->targetDecl = targetDecl;
    ((CondExpression*)opd2)->targetCtx = targetCtx;
    ((CondExpression*)opd2)->targetCat = targetCat;
    ((CondExpression*)opd2)->callObjCat = callObjCat;
  }
  if (!opd2->IsIfStmExpr()) {
    ckd.tempCtx = ckd.lpc;
    opd2->ExprGetFVType(ckd,currentBranchType,cat,ctxFlags);
    if (currentBranchType == (LavaDECL*)(-1)) {
      if (opd2->NullAdmissible(ckd)) // "nothing" admissible?
        return true;
      else {
        opd2->SetError(ckd,&ERR_NotOptional);
        ERROREXIT
      }
    }
    sourceCtx = ckd.tempCtx;
    if (ctxFlags.bits)
      sourceCtx.ContextFlags = ctxFlags;
    if (!compatibleTypes(ckd,currentBranchType,sourceCtx,targetDecl,targetCtx)) {
      opd2->SetError(ckd,ckd.errorCode);
      ok = false;
    }
    if (cat != unknownCategory
    && ((targetCat == sameAsSelfObj && cat != callObjCat)
        || (targetCat != sameAsSelfObj && cat != targetCat))) {
      opd2->SetError(ckd,&ERR_IncompatibleCategory);
      ok = false;
    }
    if (NoPH(opd1))
      opd2->targetCat = targetCat;
  }

  if (ok && !opd1->IsOptional(ckd)) {
    opd1->SetError(ckd,&ERR_ElseExprObsolete);
    ERROREXIT
  }

  EXIT
}

bool TypeBranch::Check (CheckData &ckd)
{
  LavaDECL *declBranchType, *declSwitchExpression=((TypeSwitchStatement*)parentObject)->declSwitchExpression;
  Category catBranchType, catSwitchExpression=((TypeSwitchStatement*)parentObject)->catSwitchExpression;
  CContext context;
  SynFlags ctxFlags;
  CContext exCtx, swCtx;

  ENTRY
  swCtx = ckd.tempCtx;
  ok &= ((SynObject*)exprType.ptr)->Check(ckd);
  ok &= ((SynObject*)varName.ptr)->Check(ckd);
  ((SynObject*)exprType.ptr)->ExprGetFVType(ckd,declBranchType,catBranchType,ctxFlags);

  exCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    exCtx.ContextFlags = ctxFlags;
  if (!compatibleTypes(ckd,declBranchType,exCtx,declSwitchExpression,swCtx)) {
    ((SynObject*)exprType.ptr)->SetError(ckd,ckd.errorCode);
    ok = false;
  }
#ifdef INTERPRETER
  ((VarName*)varName.ptr)->stackPos = ckd.currentStackLevel;
  typeDecl = ckd.document->GetType(declBranchType);
  declSwitchExpression = ckd.document->GetType(declSwitchExpression);
#endif
  if (catBranchType != unknownCategory
  && catSwitchExpression != unknownCategory
  && catBranchType != catSwitchExpression) {
    ((SynObject*)exprType.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
    ok = false;
  }

  if (thenPart.ptr)
    ok &= ((SynObject*)thenPart.ptr)->Check(ckd);
  EXIT
}

bool Branch::Check (CheckData &ckd)
{
  CHE *chp;
  SynObject *caseLabel;
  LavaDECL *caseLabelDecl, *caseExprDecl;
  Category catCaseLabel, catCaseExpr;
  SynFlags ctxFlags;

  ENTRY
  for (chp = (CHE*)caseLabels.first;
       chp;
       chp = (CHE*)chp->successor) {
    caseLabel = (SynObject*)chp->data;
    ok &= caseLabel->Check(ckd);
    if (ok) {
      caseLabel->ExprGetFVType(ckd,caseLabelDecl,catCaseLabel,ctxFlags);
      CContext caseLabelCtx = ckd.tempCtx;
      if (ctxFlags.bits)
        caseLabelCtx.ContextFlags = ctxFlags;
      ((SynObject*)((SwitchStatement*)parentObject)->caseExpression.ptr)->ExprGetFVType(ckd,caseExprDecl,catCaseExpr,ctxFlags);
      if (ctxFlags.bits)
        ckd.tempCtx.ContextFlags = ctxFlags;
      if (!compatibleTypes(ckd,caseLabelDecl,caseLabelCtx,caseExprDecl,ckd.tempCtx)) {
        caseLabel->SetError(ckd,ckd.errorCode);
        ok = false;
      }
      if (catCaseLabel != unknownCategory
      && catCaseExpr != unknownCategory
      && catCaseLabel != catCaseExpr) {
        caseLabel->SetError(ckd,&ERR_IncompatibleCategory);
        ok = false;
      }
    }
  }
  if (thenPart.ptr)
    ok &= ((SynObject*)thenPart.ptr)->Check(ckd);
  EXIT
}

bool SwitchStatement::Check (CheckData &ckd)
{
  CHE *chp, *branchStm, *precedingBranch;
  SynObject *opd;

  ENTRY

  ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);

  ((Expression*)caseExpression.ptr)->Check(ckd);
  CContext swCtx = ckd.tempCtx;

  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor) {
    if (chp != (CHE*)branches.first)
      ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    opd = (SynObject*)chp->data;
    ckd.tempCtx = swCtx;
    ok &= opd->Check(ckd);
  }
  if (elsePart.ptr) {
    ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    ok &= ((SynObject*)elsePart.ptr)->Check(ckd);
  }

  ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);

  EXIT
}

bool CatchClause::Check (CheckData &ckd)
{
  ENTRY
  LavaDECL *declBranchType;
  Category catBranchType;
  CContext context;
  SynFlags ctxFlags;

  ok &= ((SynObject*)exprType.ptr)->Check(ckd);
  ok &= ((SynObject*)varName.ptr)->Check(ckd);
  ((SynObject*)exprType.ptr)->ExprGetFVType(ckd,declBranchType,catBranchType,ctxFlags);

  if (!declBranchType)
    ERROREXIT

  if (!declBranchType->SecondTFlags.Contains(isException)) {
    ((SynObject*)exprType.ptr)->SetError(ckd,&ERR_FailNoException);
    ERROREXIT
  }
#ifdef INTERPRETER
  ((VarName*)varName.ptr)->stackPos = ckd.currentStackLevel;
  typeDecl = ckd.document->GetType(declBranchType);
//  declSwitchExpression = ckd.document->GetType(declSwitchExpression);
#endif
  if (catBranchType != valueObj) {
    ((SynObject*)exprType.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
    ok = false;
  }

  if (catchClause.ptr)
    ok &= ((SynObject*)catchClause.ptr)->Check(ckd);
  EXIT
}

bool TryStatement::Check (CheckData &ckd)
{
  CHE *chp, *branchStm, *precedingBranch;
  SynObject *opd;

  ENTRY

  ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);
#ifdef INTERPRETER
  ckd.currentStackLevel++;
#endif

  ((Expression*)tryStatement.ptr)->Check(ckd);

  ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
  for (chp = (CHE*)catchClauses.first;
       chp;
       chp = (CHE*)chp->successor) {
    if (chp != (CHE*)catchClauses.first)
      ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    opd = (SynObject*)chp->data;
    ok &= opd->Check(ckd);
  }

  ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);

#ifdef INTERPRETER
  if (ckd.currentStackLevel > ckd.stackFrameSize)
    ckd.stackFrameSize = ckd.currentStackLevel;
  ckd.currentStackLevel--;
#endif
  EXIT
}

bool TypeSwitchStatement::Check (CheckData &ckd)
{
  CHE *chp, *branchStm, *precedingBranch;
  SynObject *opd;
  SynFlags ctxFlags;

  ENTRY

  ((RefTable*)ckd.refTable)->NewBranchStm(branchStm,precedingBranch);

#ifdef INTERPRETER
  ckd.currentStackLevel++;
#endif
  ((SynObject*)caseExpression.ptr)->Check(ckd);
  ((SynObject*)caseExpression.ptr)->ExprGetFVType(ckd,declSwitchExpression,catSwitchExpression,ctxFlags);
  CContext swCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    swCtx.ContextFlags = ctxFlags;
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor) {
    if (chp != (CHE*)branches.first)
      ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
    opd = (SynObject*)chp->data;
    ckd.tempCtx = swCtx;
    ok &= opd->Check(ckd);
  }

  ((RefTable*)ckd.refTable)->NewBranch(branchStm,precedingBranch);
  if (elsePart.ptr)
    ok &= ((SynObject*)elsePart.ptr)->Check(ckd);

  ((RefTable*)ckd.refTable)->EndBranchStm(branchStm,precedingBranch);

#ifdef INTERPRETER
  if (ckd.currentStackLevel > ckd.stackFrameSize)
    ckd.stackFrameSize = ckd.currentStackLevel;
  ckd.currentStackLevel--;
#endif
  EXIT
}

bool AssertStatement::Check (CheckData &ckd)
{
  ENTRY
  ok &= ((SynObject*)statement.ptr)->Check(ckd);
  EXIT
}

void AttachObject::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)itf.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool AttachObject::Check (CheckData &ckd)
{
  SynFlags ctxFlags;
  LavaDECL *objDECL, *urlDECL, *stringDECL;
  CHETID *supp;
  Category cat;

  ENTRY
  ok &= ((SynObject*)objType.ptr)->Check(ckd);
  if (itf.ptr) {
    ok &= ((SynObject*)itf.ptr)->Check(ckd);
    if (NoPH(objType.ptr)) {
      objDECL = ckd.document->IDTable.GetDECL(((Reference*)objType.ptr)->refID,ckd.inINCL);
      if (objDECL) {
        if (!IsPH(itf.ptr)) {
          for (supp = (CHETID*)objDECL->Supports.first; supp; supp = (CHETID*)supp->successor) {
            if (ckd.document->IDTable.EQEQ(((Reference*)itf.ptr)->refID,ckd.inINCL,
                supp->data,((Reference*)objType.ptr)->refID.nINCL)) break;
          }
          if (!supp) {
            ((SynObject*)itf.ptr)->SetError(ckd,&ERR_ItfNotInCOS);
            ok = false;
          }
        }

        if ((objDECL->nOutput == PROT_LAVA) || (objDECL->nOutput == PROT_NATIVE)) {
          if (!objDECL->Items.first) {
            ((SynObject*)objType.ptr)->SetError(ckd,&ERR_OIDrequired);
            ok = false;
          }
          if (!url.ptr) {
            ((SynObject*)objType.ptr)->SetError(ckd,&ERR_URLrequired);
            ok = false;
          }
          else {
            ok &= ((SynObject*)url.ptr)->Check(ckd);
            ((SynObject*)url.ptr)->ExprGetFVType(ckd,urlDECL,cat,ctxFlags);
            urlDECL = ckd.document->GetType(urlDECL);
            stringDECL = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[VLString]);
            if (urlDECL
            && !ckd.document->IsCDerivation(urlDECL,stringDECL,&ckd)) {
              ((SynObject*)url.ptr)->SetError(ckd,&ERR_URLmustBeString);
              return false;
            }
          }
        }
        else {
#ifdef INTERPRETER
          ((SynObject*)objType.ptr)->SetError(ckd,&ERR_NotYetImplemented);
          ok = false;
#endif
        }
      }
    }
  }

#ifdef INTERPRETER
  ExprGetFVType(ckd,typeDECL,attachCat,ctxFlags);
  typeDECL = ckd.document->GetType(typeDECL);
#endif

  EXIT
}

bool Run::Check (CheckData &ckd)
{
  CHE *chpActIn, *chpFormIn;
  SynObject *opd;
  TID tidInitiator;
  LavaDECL *decl;
  CContext callCtx;
  Category callObjCat;
#ifdef INTERPRETER
//  CHE *chpExec;
  unsigned oldStackLevel=ckd.currentStackLevel;
  Category cat;
  SynFlags ctxFlags;
#endif

  ENTRY
  ok &= ((SynObject*)initiator.ptr)->Check(ckd);

  if (IsPH(initiator.ptr))
    ERROREXIT

  chpActIn = (CHE*)inputs.first;
  tidInitiator = ((Reference*)initiator.ptr)->refID;
  ADJUST4(tidInitiator);
  decl = ckd.document->IDTable.GetDECL(tidInitiator);
  if (!decl->TypeFlags.Contains(isConst) && ReadOnlyContext() != noROContext) {
    SetError(ckd,&ERR_NonROCallInROClause);
    ERROREXIT
  }

  chpFormIn = GetFirstInput(&ckd.document->IDTable,tidInitiator);
  callCtx = ckd.lpc;
  callObjCat = unknownCategory;
  while (chpFormIn) {
    // locate act. parm. and reposition it if necessary:
    reposition(ckd,this,true,&inputs,chpFormIn,chpActIn);
    // check act. parm.:
    opd = (SynObject*)chpActIn->data;
    ok &= opd->Check(ckd);
    // check act.parm/form.parm. type compatibility:
    ok &= compatibleInput(ckd,chpActIn,chpFormIn,callCtx,callObjCat);
    if (chpActIn)
      chpActIn = (CHE*)chpActIn->successor;
    chpFormIn = (CHE*)chpFormIn->successor;
    if (chpFormIn && ((LavaDECL*)chpFormIn->data)->DeclType != IAttr)
      chpFormIn = 0;
  }
  if (chpActIn)
#ifdef INTERPRETER
    SetError(ckd,&ERR_RedundantParms);
#else
  for ( ;
        chpActIn;
        chpActIn = (CHE*)chpActIn->successor)  // delete remainder of parameter chain
    PutDelChainHint(ckd,this,&inputs,chpActIn);
#endif

#ifdef INTERPRETER
  ((SynObject*)initiator.ptr)->ExprGetFVType(ckd,typeDECL,cat,ctxFlags);
//    isStateObj = (cat == stateObj ? true : false);
  typeDECL = ckd.document->GetType(typeDECL);
  nParams = typeDECL->nInput;
#endif

  EXIT
}

void QueryItf::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)itf.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool QueryItf::Check (CheckData &ckd) {
  LavaDECL *declObj, *declItf;
  Category cat;
  SynFlags ctxFlags;

  ENTRY

  ok &= ((SynObject*)itf.ptr)->Check(ckd);
  ok &= ((SynObject*)givenObj.ptr)->Check(ckd);
  if (((SynObject*)itf.ptr)->IsPlaceHolder()
  || ((SynObject*)givenObj.ptr)->IsPlaceHolder())
    ERROREXIT

  ((SynObject*)givenObj.ptr)->ExprGetFVType(ckd,declObj,cat,ctxFlags);
  declObj = ckd.document->GetType(declObj);
  if (!declObj) {
    ((SynObject*)givenObj.ptr)->SetError(ckd,&ERR_UndefType);
    ok = false;
  }
  else if (!declObj->TypeFlags.Contains(isComponent)) {
    ((SynObject*)givenObj.ptr)->SetError(ckd,&ERR_NoCompObjItf);
    ok = false;
  }

  ((SynObject*)itf.ptr)->ExprGetFVType(ckd,declItf,cat,ctxFlags);
  declItf = ckd.document->GetType(declItf);
  if (!declItf) {
    ((SynObject*)itf.ptr)->SetError(ckd,&ERR_UndefType);
    ok = false;
  }
  else if (!declItf->TypeFlags.Contains(isComponent)) {
    ((SynObject*)givenObj.ptr)->SetError(ckd,&ERR_NoCompObjItf);
    ok = false;
  }

  EXIT
}

void GetUUID::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[VLString]);
  if (flags.Contains(isVariable))
    cat = stateObj;
  else if (flags.Contains(isSameAsSelf))
    cat = sameAsSelfObj;
  else
    cat = valueObj;
  ctxFlags.bits = 0;
#ifdef INTERPRETER
  finalType = decl;
#endif
}

bool GetUUID::Check (CheckData &ckd) {
  ENTRY
  ok &= ((SynObject*)itf.ptr)->Check(ckd);
  EXIT
}

void IntegerInterval::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  decl = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[Integer]);
  if (flags.Contains(isVariable))
    cat = stateObj;
  else if (flags.Contains(isSameAsSelf))
    cat = sameAsSelfObj;
  else
    cat = valueObj;
  ctxFlags.bits = 0;
}

bool IntegerInterval::Check (CheckData &ckd)
{
  TID tidInt;
  LavaDECL *declExpr;
  Category cat;
  SynFlags ctxFlags;

  ENTRY
  ok &= ((SynObject*)from.ptr)->Check(ckd);
  ok &= ((SynObject*)to.ptr)->Check(ckd);
  tidInt.nID = ckd.document->IDTable.BasicTypesID[Integer];
  tidInt.nINCL = (ckd.document->isStd?0:1);
  ((SynObject*)from.ptr)->ExprGetFVType(ckd,declExpr,cat,ctxFlags);
  if (declExpr == (LavaDECL*)-1) {
    ((SynObject*)from.ptr)->SetError(ckd,&ERR_IncompatibleType);
    ok = false;
  }
  else {
    declExpr = ckd.document->GetType(declExpr);
    if (declExpr && !sameType(ckd,OWNID(declExpr),tidInt)) {
      ((SynObject*)from.ptr)->SetError(ckd,&ERR_IncompatibleType);
      ok = false;
    }
  }
  ((SynObject*)to.ptr)->ExprGetFVType(ckd,declExpr,cat,ctxFlags);
  if (declExpr == (LavaDECL*)-1) {
    ((SynObject*)to.ptr)->SetError(ckd,&ERR_IncompatibleType);
    ok = false;
  }
  else {
    declExpr = ckd.document->GetType(declExpr);
    if (declExpr && !sameType(ckd,OWNID(declExpr),tidInt)) {
      ((SynObject*)to.ptr)->SetError(ckd,&ERR_IncompatibleType);
      ok = false;
    }
  }
  EXIT
}

bool Quantifier::Check (CheckData &ckd)
{
  CHE *chp;
  VarName *opd;
  TID tidElemType;
  LavaDECL *declSetType;
  Category elemCat, typeCat;
  SynFlags ctxFlags;
#ifdef INTERPRETER
  unsigned nQuantVars=0;
  bool isSetQuant=NoPH(set.ptr)
    && !((SynObject*)set.ptr)->IsIntIntv()
    && ((SynObject*)set.ptr)->primaryToken != TypeRef_T;
#endif

  ENTRY
  if (elemType.ptr)
    ok &= ((SynObject*)elemType.ptr)->Check(ckd);

  if (set.ptr)
    ((SynObject*)set.ptr)->Check(ckd);

  for (chp = (CHE*)quantVars.first;
       chp;
       chp = (CHE*)chp->successor) {
    opd = (VarName*)chp->data;
    ok &= opd->Check(ckd);

#ifdef INTERPRETER
    if (isSetQuant) {
      opd->stackPos = ckd.currentStackLevel + nQuantVars;
      nQuantVars += 2;
    }
    else
      opd->stackPos = ckd.currentStackLevel + nQuantVars++;
#endif
  }

#ifdef INTERPRETER
  ckd.currentStackLevel += nQuantVars;
  ((QuantStmOrExp*)parentObject)->nQuantVars += nQuantVars;
#endif

  if (NoPH(set.ptr)) {
    ((SynObject*)set.ptr)->ExprGetFVType(ckd,declSetType,elemCat,ctxFlags);
    declSetType = ckd.document->GetType(declSetType);
    if (((SynObject*)set.ptr)->primaryToken == intIntv_T
    || (declSetType && declSetType->SecondTFlags.Contains(isEnum))) {
      if (elemType.ptr) {
        ((SynObject*)elemType.ptr)->SetError(ckd,&ERR_EnumIntvType);
        ok = false;
      }
    }
    else if (declSetType)
      if (!declSetType->SecondTFlags.Contains(isSet)) {
        ((SynObject*)set.ptr)->SetError(ckd,&ERR_IsntSet);
        ok = false;
      }
      else {
        ckd.document->IDTable.GetParamID(declSetType,tidElemType,isSet); // set elem. type
        if (ctxFlags.bits)
          ckd.tempCtx.ContextFlags = ctxFlags;
        setElemDecl = ckd.document->GetFinalMVType(tidElemType,0,ckd.tempCtx,elemCat,&ckd);
        if (NoPH(elemType.ptr)) {
          CContext elCtx = ckd.tempCtx;
          ((Reference*)elemType.ptr)->ExprGetFVType(ckd,typeDecl,typeCat,ctxFlags);
          if (ctxFlags.bits)
            ckd.tempCtx.ContextFlags = ctxFlags;
          if (typeCat == unknownCategory)
            if (((Reference*)elemType.ptr)->flags.Contains(isVariable))
              typeCat = stateObj;
            else if (((Reference*)elemType.ptr)->flags.Contains(isSameAsSelf))
              typeCat = sameAsSelfObj;
            else
              typeCat = valueObj;

          if (!compatibleTypes(ckd,typeDecl,ckd.tempCtx,setElemDecl,elCtx)) {
            ((SynObject*)elemType.ptr)->SetError(ckd,ckd.errorCode);
            ok = false;
          }
#ifdef INTERPRETER
          typeDecl = ckd.document->GetType(typeDecl);
        }
        setElemDecl = ckd.document->GetType(setElemDecl);
#else
        }
#endif
      }
    else
      ERROREXIT
  }

  EXIT
}

bool QuantStmOrExp::InitCheck (CheckData &ckd)
{
  CHE *chp, *varPtr;
  CVarDesc *entryFound=0;
  ObjReference *objRef;
  Quantifier *quant;
  TDOD *tdod;
  TDODC refIDs;
  bool ok=true;

  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor) {
    quant = (Quantifier*)chp->data;
    for (varPtr = (CHE*)quant->quantVars.first;
         varPtr;
         varPtr = (CHE*)varPtr->successor) {
      if (!((SynObject*)varPtr->data)->IsPlaceHolder()) {
        tdod = new TDOD;
        tdod->ID.nID = ((VarName*)varPtr->data)->varID.nID;
        refIDs.Append(new CHE(tdod));
        objRef = new ObjReference(refIDs,((VarName*)varPtr->data)->varName.c);
        if (!((RefTable*)ckd.refTable)->assigned(ckd,((RefTable*)ckd.refTable)->objRefTable,(CHE*)objRef->refIDs.first)) {
          ((VarName*)varPtr->data)->SetError(ckd,&ERR_Unused);
          ok = false;
        }
        delete objRef;
        refIDs.Destroy();
      }
    }
  }
  return ok;
}

bool QuantStmOrExp::Check (CheckData &ckd)
{
  CHE *chp;
  SynObject *opd;
  SET oldFlags=ckd.flags;
#ifdef INTERPRETER
  unsigned oldStackLevel=ckd.currentStackLevel;
  nQuantVars = 0;
#endif

  ENTRY
/*  if ((IsDeclare()
      || ((Exists*)this)->updateStatement.ptr)
  && InReadOnlyContext()) {
    SetError(ckd,&ERR_AssignInQuery);
    ok = false;
  }*/

  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor) {
    opd = (SynObject*)chp->data;
    ok &= opd->Check(ckd);
  }

  if (statement.ptr)
    ok &= ((SynObject*)statement.ptr)->Check(ckd);
  if (primaryToken == foreach_T)
    ckd.flags.INCL(InForEach);
  if (IsExists() && ((Exists*)this)->updateStatement.ptr)
    ok &= ((SynObject*)((Exists*)this)->updateStatement.ptr)->Check(ckd);
  if (IsDeclare())
    ok &= InitCheck(ckd);

#ifdef INTERPRETER
  if (primaryToken != select_T) {
    if (ckd.currentStackLevel > ckd.stackFrameSize)
      ckd.stackFrameSize = ckd.currentStackLevel;
    ckd.currentStackLevel = oldStackLevel;
  }
#endif
  ckd.flags = oldFlags;
  EXIT
}

void SelectExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)resultSet.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool SelectExpression::Check (CheckData &ckd)
{
  TID setElemTid;
  LavaDECL *setElemDecl, *setTypeDecl, *addObjDecl;
  CContext elCtx, addCtx;
  CHE *oldError1;
  //SynObject *opd;
  Category setCat, setElCat, addCat;
  SynFlags ctxFlags;
#ifdef INTERPRETER
  unsigned oldStackLevel=ckd.currentStackLevel;
#endif

  ENTRY
  oldError1 = oldError;
//  ckd.currentSynObj = parentObject;
  ok &= QuantStmOrExp::Check(ckd);
//  ckd.currentSynObj = this;
  oldError = oldError1;
  ok &= ((SynObject*)addObject.ptr)->Check(ckd);
  ok &= ((SynObject*)resultSet.ptr)->Check(ckd);

  if (IsPH(resultSet.ptr))
    ERROREXIT
  ((SynObject*)resultSet.ptr)->ExprGetFVType(ckd,setTypeDecl,setCat,ctxFlags);
  setTypeDecl = ckd.document->GetType(setTypeDecl);
  if (!setTypeDecl) {
    ((SynObject*)resultSet.ptr)->SetError(ckd,&ERR_UndefType);
    ERROREXIT
  }
  if (!setTypeDecl->SecondTFlags.Contains(isSet)) {
    ((SynObject*)resultSet.ptr)->SetError(ckd,&ERR_IsntSet);
    ERROREXIT
  }
  if (!ckd.document->IDTable.GetParamID(setTypeDecl,setElemTid,isSet)) {
    ((SynObject*)resultSet.ptr)->SetError(ckd,&ERR_UndefinedSetElemType);
    ERROREXIT
  }
  ckd.tempCtx = ckd.lpc;
  setElemDecl = ckd.document->GetFinalMVType(setElemTid,0,ckd.tempCtx,setElCat,&ckd);
  elCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    elCtx.ContextFlags = ctxFlags;
  setElemDecl = ckd.document->GetType(setElemDecl);
  if (!setElemDecl) {
    ((SynObject*)resultSet.ptr)->SetError(ckd,&ERR_UndefinedSetElemType);
    ERROREXIT
  }
  if (IsPH(addObject.ptr))
    ERROREXIT
  ((SynObject*)addObject.ptr)->ExprGetFVType(ckd,addObjDecl,addCat,ctxFlags);
  addCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    addCtx.ContextFlags = ctxFlags;
  addObjDecl = ckd.document->GetType(addObjDecl);
  if (!addObjDecl) {
    ((SynObject*)addObject.ptr)->SetError(ckd,&ERR_UndefType);
    ERROREXIT
  }
  if (!compatibleTypes(ckd,addObjDecl,addCtx,setElemDecl,elCtx)) {
    ((SynObject*)addObject.ptr)->SetError(ckd,ckd.errorCode);
    ok = false;
  }

  if (addCat != unknownCategory && addCat != setElCat) {
    ((SynObject*)addObject.ptr)->SetError(ckd,&ERR_IncompatibleCategory);
    ok = false;
  }
  if (NoPH(addObject.ptr))
    ((Expression*)addObject.ptr)->targetCat = setElCat;
  if (setCat == valueObj
  && ((SynObject*)resultSet.ptr)->primaryToken != new_T) {
    ((SynObject*)addObject.ptr)->SetError(ckd,&ERR_SetIsFrozenValObj);
    ok = false;
  }

#ifdef INTERPRETER
  if (ckd.currentStackLevel > ckd.stackFrameSize)
    ckd.stackFrameSize = ckd.currentStackLevel;
  ckd.currentStackLevel = oldStackLevel;
#endif
  EXIT
}

void NewExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  FuncStatement *funcStm;
  TID funcTid;
  LavaDECL *funcDecl;

  ctxFlags.bits = 0;
  if (itf.ptr)
    ((SynObject*)itf.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
  else if (initializerCall.ptr) {
    funcStm = (FuncStatement*)initializerCall.ptr;
    if (((SynObject*)funcStm->function.ptr)->IsPlaceHolder()) {
      decl = 0;
      return;
    }
    funcTid = ((Reference*)funcStm->function.ptr)->refID;
    funcDecl = ckd.document->IDTable.GetDECL(funcTid,ckd.inINCL);
    ((SynObject*)objType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags); // for cat only
    if (funcDecl)
      decl = funcDecl->ParentDECL;
    else
      decl = 0;
  }
  else
    ((SynObject*)objType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool NewExpression::Check (CheckData &ckd)
{
#ifdef INTERPRETER
  TID tidType;
  unsigned oldStackLevel=ckd.currentStackLevel;
#endif
  Category cat;
  SynFlags ctxFlags;
  LavaDECL *objDECL, *urlDECL, *stringDECL;
  CHETID *supp;
  bool ok1;
  SET oldFlags=ckd.flags;

  ENTRY
  if (objType.ptr) {
    ok &= ((SynObject*)objType.ptr)->Check(ckd);
/*    if (IsPH(objType.ptr))
      ERROREXIT*/
  }

  if (varName.ptr) {
#ifdef INTERPRETER
    ((VarName*)varName.ptr)->stackPos = ckd.currentStackLevel++;
#endif
    ok &= ((SynObject*)varName.ptr)->Check(ckd);
  }
  if (initializerCall.ptr) {
    ok1 = ((SynObject*)initializerCall.ptr)->Check(ckd);
#ifndef INTERPRETER
    if (!ok1) {
      errorInInitializer = true;
      multipleUpdates = true;
    }
    else
      errorInInitializer = false;
#endif
    ok &= ok1;
  }

#ifdef INTERPRETER
  if (initializerCall.ptr)
    ((ObjReference*)((FuncStatement*)initializerCall.ptr)->handle.ptr)->stackPos = oldStackLevel;
  if (itf.ptr) {
    ((SynObject*)itf.ptr)->ExprGetFVType(ckd,typeDECL,attachCat,ctxFlags);
    typeDECL = ckd.document->GetType(typeDECL);
  }
  else {
    if (ckd.currentStackLevel > ckd.stackFrameSize)
      ckd.stackFrameSize = ckd.currentStackLevel;
    ((SynObject*)objType.ptr)->ExprGetFVType(ckd,typeDECL,cat,ctxFlags);
    typeDECL = ckd.document->GetType(typeDECL);
    if (typeDECL->NestedDecls.last) {
      execDECL = (LavaDECL*)((CHE*)typeDECL->NestedDecls.last)->data;
      if (execDECL->DeclType != ExecDef)
        execDECL = 0;
    }
    else
      execDECL = 0;
  }
#endif

  if (itf.ptr) {
    ok &= ((SynObject*)itf.ptr)->Check(ckd);
    if (NoPH(objType.ptr)) {
      objDECL = ckd.document->IDTable.GetDECL(((Reference*)objType.ptr)->refID,ckd.inINCL);
      if (objDECL) {
        if (!IsPH(itf.ptr)) {
          for (supp = (CHETID*)objDECL->Supports.first; supp; supp = (CHETID*)supp->successor) {
            if (ckd.document->IDTable.EQEQ(((Reference*)itf.ptr)->refID,ckd.inINCL,
                supp->data,((Reference*)objType.ptr)->refID.nINCL)) break;
          }
          if (!supp) {
            ((SynObject*)itf.ptr)->SetError(ckd,&ERR_ItfNotInCOS);
            ok = false;
          }
        }

        if (objDECL->nOutput == PROT_LAVA) {
          if (!objDECL->Items.first) {
            ((SynObject*)objType.ptr)->SetError(ckd,&ERR_OIDrequired);
            ok = false;
          }
          if (!url.ptr) {
            ((SynObject*)objType.ptr)->SetError(ckd,&ERR_URLrequired);
            ok = false;
          }
          else {
            ok &= ((SynObject*)url.ptr)->Check(ckd);
            ((SynObject*)url.ptr)->ExprGetFVType(ckd,urlDECL,cat,ctxFlags);
            urlDECL = ckd.document->GetType(urlDECL);
            stringDECL = ckd.document->IDTable.GetDECL(ckd.document->isStd?0:1,ckd.document->IDTable.BasicTypesID[VLString]);
            if (urlDECL
            && !ckd.document->IsCDerivation(urlDECL,stringDECL,&ckd)) {
              ((SynObject*)url.ptr)->SetError(ckd,&ERR_URLmustBeString);
              return false;
            }
          }
        }
        else {
#ifdef INTERPRETER
          ((SynObject*)objType.ptr)->SetError(ckd,&ERR_NotYetImplemented);
          ok = false;
#endif
        }
      }
    }
  }

  if (butStatement.ptr) {
    ckd.flags.INCL(InBut);
    ok &= ((SynObject*)butStatement.ptr)->Check(ckd);
    ckd.flags = oldFlags;
  }

#ifdef INTERPRETER
  if (ckd.currentStackLevel > ckd.stackFrameSize)
    ckd.stackFrameSize = ckd.currentStackLevel;
  ckd.currentStackLevel = oldStackLevel;
#endif

  EXIT
}

void CloneExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)fromObj.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool CloneExpression::Check (CheckData &ckd)
{
  SET oldFlags=ckd.flags;
#ifdef INTERPRETER
  TID tidType;
  unsigned oldStackLevel=ckd.currentStackLevel;
  LavaDECL *typeDECL;
  Category cat;
  SynFlags ctxFlags;
#endif

  ENTRY
  ok &= ((SynObject*)fromObj.ptr)->Check(ckd);
  if (varName.ptr)
    ok &= ((SynObject*)varName.ptr)->Check(ckd);

#ifdef INTERPRETER
  ((VarName*)varName.ptr)->stackPos = ckd.currentStackLevel;
  ckd.currentStackLevel++;
  if (ckd.currentStackLevel > ckd.stackFrameSize)
    ckd.stackFrameSize = ckd.currentStackLevel;
  ((SynObject*)fromObj.ptr)->ExprGetFVType(ckd,typeDECL,cat,ctxFlags);
  typeDECL = ckd.document->GetType(typeDECL);
  if (typeDECL->NestedDecls.last) {
    execDECL = (LavaDECL*)((CHE*)typeDECL->NestedDecls.last)->data;
    if (execDECL->DeclType != ExecDef)
      execDECL = 0;
  }
  else
    execDECL = 0;
#endif

  if (butStatement.ptr) {
    ckd.flags.INCL(InBut);
    ok &= ((SynObject*)butStatement.ptr)->Check(ckd);
    ckd.flags = oldFlags;
  }

#ifdef INTERPRETER
  ckd.currentStackLevel = oldStackLevel;
#endif
  EXIT
}

bool CopyStatement::Check (CheckData &ckd)
{
  LavaDECL *fromTypeDecl, *ontoTypeDecl;
  Category cat;
  SynFlags ctxFlags;
  SynObject *source=(SynObject*)fromObj.ptr, *target=(SynObject*)ontoObj.ptr;
#ifdef INTERPRETER
  DWORD dw;
  TIDType idtype;
#endif

  ENTRY

  if (ReadOnlyContext() == roClause) {
    SetError(ckd,&ERR_AssignInROClause);
    ERROREXIT
  }

  ok &= source->Check(ckd);
  ok &= target->Check(ckd);
  if (!ok)
    ERROREXIT

  source->ExprGetFVType(ckd,fromTypeDecl,cat,ctxFlags);
  CContext fromCtx = ckd.tempCtx;
  if (ctxFlags.bits)
    fromCtx.ContextFlags = ctxFlags;
  target->ExprGetFVType(ckd,ontoTypeDecl,cat,ctxFlags);
  if (ctxFlags.bits)
    ckd.tempCtx.ContextFlags = ctxFlags;
  ontoTypeDecl = ckd.document->GetType(ontoTypeDecl);
  if (target->IsOptional(ckd)
  && ontoTypeDecl
  && ontoTypeDecl->inINCL != 1
  && !ontoTypeDecl->HasDefaultInitializer()) {
    target->SetError(ckd,&ERR_HasNoDftIni);
    ok = false;
  }
  if (source->IsOptional(ckd)
  && !target->IsOptional(ckd)
  && !source->IsDefChecked(ckd)) {
    source->SetError(ckd,&ERR_NotOptional);
    ok = false;
  }

#ifdef INTERPRETER
  this->ontoTypeDecl = ontoTypeDecl;
  fromTypeDecl = ckd.document->GetType(fromTypeDecl);
  if (target->primaryToken == arrayAtIndex_T)
    kindOfTarget = arrayElem;
  else {
    dw = ckd.document->IDTable.GetVar(((TDOD*)((CHE*)((ObjReference*)target)->refIDs.last)->data)->ID,idtype,ckd.inINCL);
    switch (idtype) {
    case localID:
      kindOfTarget = field;
      break;
    default:
      ontoTypeDecl = *(LavaDECL**)dw;
      if (ontoTypeDecl->TypeFlags.Contains(hasSetGet))
        kindOfTarget = property;
      else
        kindOfTarget = field;
      if (!ontoTypeDecl->TypeFlags.Contains(constituent)
        && !ontoTypeDecl->TypeFlags.Contains(acquaintance))
        target->flags.INCL(isReverseLink);
    }
  }
#endif
  EXIT
}

void EnumItem::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)enumType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool EnumItem::Check (CheckData &ckd)
{
  LavaDECL *declExpr;
  Category cat;
  SynFlags ctxFlags;
  TID tidInt;

  ENTRY
  ok &= ((SynObject*)itemNo.ptr)->Check(ckd);
  ok &= ((SynObject*)enumType.ptr)->Check(ckd);

  tidInt.nID = ckd.document->IDTable.BasicTypesID[Integer];
  tidInt.nINCL = (ckd.document->isStd?0:1);
  ((SynObject*)itemNo.ptr)->ExprGetFVType(ckd,declExpr,cat,ctxFlags);
  declExpr = ckd.document->GetType(declExpr);
  if (declExpr && !sameType(ckd,OWNID(declExpr),tidInt)) {
    ((SynObject*)itemNo.ptr)->SetError(ckd,&ERR_IncompatibleType);
    ok = false;
  }

  ((SynObject*)enumType.ptr)->ExprGetFVType(ckd,declExpr,cat,ctxFlags);
  declExpr = ckd.document->GetType(declExpr);
  if (declExpr && !declExpr->SecondTFlags.Contains(isEnum)) {
    ((SynObject*)enumType.ptr)->SetError(ckd,&ERR_IsntEnum);
    ok = false;
  }
  EXIT
}

void ExtendExpression::ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) {
  ((SynObject*)extendType.ptr)->ExprGetFVType(ckd,decl,cat,ctxFlags);
#ifdef INTERPRETER
  finalType = ckd.document->GetType(decl);
#endif
}

bool ExtendExpression::Check (CheckData &ckd)
{
  LavaDECL *declObj, *declType;
  Category cat;
  SynFlags ctxFlags;
  CContext ctxObj, ctxType;

  ENTRY
  ok &= ((SynObject*)extendObj.ptr)->Check(ckd);
  ok &= ((SynObject*)extendType.ptr)->Check(ckd);

  ckd.tempCtx = ckd.lpc;
  ((SynObject*)extendObj.ptr)->ExprGetFVType(ckd,declObj,cat,ctxFlags);
  //declObj = ckd.document->GetType(declObj);
  ctxObj = ckd.tempCtx;
  if (ctxFlags.bits)
    ctxObj.ContextFlags = ctxFlags;
  ((SynObject*)extendType.ptr)->ExprGetFVType(ckd,declType,cat,ctxFlags);
  ctxType = ckd.tempCtx;
  if (ctxFlags.bits)
    ctxType.ContextFlags = ctxFlags;
  if (!compatibleTypes(ckd,declType,ctxType,declObj,ctxObj)) {
    ((SynObject*)extendType.ptr)->SetError(ckd,ckd.errorCode);
    ok = false;
  }
  declType = ckd.document->GetType(declType);
  if (declType
  && declType->inINCL != 1
  && !declType->HasDefaultInitializer()) {
    ((SynObject*)extendType.ptr)->SetError(ckd,&ERR_HasNoDftIni);
    ok = false;
  }
#ifdef INTERPRETER
  extendTypeDecl = declType;
#endif
  EXIT
}

// ==================================================================================

static int FieldInType(CheckData &ckd, LavaDECL *typeDECL, LavaDECL *fieldDECL)
{
  int rc;
  if (!typeDECL || (typeDECL->DeclDescType != StructDesc))
    return -1;
  if (fieldDECL->ParentDECL == typeDECL)
    return 1;
  if (fieldDECL->ParentDECL->DeclType == Impl)
    if ((fieldDECL->ParentDECL == ckd.selfTypeDECL) && ckd.selfTypeDECL->Supports.first
        && (OWNID(typeDECL) == ((CHETID*)ckd.selfTypeDECL->Supports.first)->data))
      return 1;
    else
      return 0;
  CHETID *cheSup = (CHETID*)typeDECL->Supports.first;
  LavaDECL* inDECL;
  while (cheSup ) {
    inDECL = ckd.document->IDTable.GetDECL(cheSup->data, typeDECL->inINCL); //GetFinalDef(pID, typeDECL->inINCL);
    switch (rc = FieldInType(ckd, inDECL, fieldDECL)) {
    case 0:
    case 1:
      return rc;
    default:
      cheSup = (CHETID*)cheSup->successor;
    }
  }
  return -1;
}


bool VerifyObj(CheckData &ckd, CHE* DODs, DString& name, ObjReference *parent, LavaDECL *startDecl)
// startDecl is from final static real type
{
  DWORD vv;
  LavaDECL *fieldDECL = 0, *typeDECL = 0, *vTypeDECL = 0, *implItfDecl, *parentTypeDecl=startDecl;
  TIDType itype;
  DString strINCL, strID;
  bool ok=true, setErr = false;
  CHE *cheo = DODs;
  TDOD* dod = (TDOD*)cheo->data;
  int fldInType=-2;

  if (startDecl) {
    typeDECL = startDecl;
    typeDECL = ckd.document->GetTypeAndContext(typeDECL, ckd.tempCtx);
    if (!typeDECL && (startDecl->RefID.nID >= 0))
      ok = false;
  }
  else {
    ckd.tempCtx = ckd.lpc;
    dod->context = ckd.tempCtx;
    fieldDECL = ckd.document->IDTable.GetDECL(dod->ID,ckd.inINCL);
    if (fieldDECL->TypeFlags.Contains(isOptional))
      dod->flags.INCL(isOptionalExpr);
    else
      dod->flags.EXCL(isOptionalExpr);
    if (fieldDECL && ((fieldDECL->DeclType == IAttr) || (fieldDECL->DeclType == OAttr))) {
      dod->fieldDecl = fieldDECL;
      ckd.document->MemberTypeContext(fieldDECL, ckd.tempCtx,&ckd);
      vTypeDECL = ckd.document->GetFinalMVType(fieldDECL->RefID,fieldDECL->inINCL,ckd.tempCtx, parent->myCategory,&ckd);
      if (!cheo->successor) // 22.5.01
        parent->myContext = ckd.tempCtx;
      typeDECL = ckd.document->GetTypeAndContext(vTypeDECL, ckd.tempCtx);
      if (!typeDECL && (!vTypeDECL || (vTypeDECL->RefID.nID >= 0) || cheo->successor)) {
        dod->SetError(ckd,&ERR_UndefType);
        ok = false;
      }
#ifdef INTERPRETER
      dod->vType = ckd.document->IDTable.GetDECL(fieldDECL->RefID, fieldDECL->inINCL);
      dod->eType = typeDECL;
      dod->isProperty = fieldDECL->TypeFlags.Contains(hasSetGet);
#endif
      if (!cheo->successor) {
        parent->myFinalVType = vTypeDECL;
        if (fieldDECL->TypeFlags.Contains(substitutable))
          parent->flags.INCL(isSubstitutable);
        if (parent->myCategory == unknownCategory
            && fieldDECL->TypeFlags.Contains(trueObjCat))
          if (fieldDECL->TypeFlags.Contains(stateObject))
            parent->myCategory = stateObj;
          else
            parent->myCategory = valueObj;
      }
//#ifndef INTERPRETER
      name += fieldDECL->LocalName;
      dod->name = fieldDECL->LocalName;
//#endif
      dod->parentObject = parent;
      cheo = (CHE*)cheo->successor;
    }
  }
  while (cheo) {
    dod = (TDOD*)cheo->data;
    dod->execView=ckd.execView;
    dod->oldError = (CHE*)dod->errorChain.first;
    dod->errorChain.Destroy();
    dod->lastError = 0;
    dod->parentObject = parent;
    dod->whereInParent = (address)cheo;
    dod->context = ckd.tempCtx;
    //das Flag vom vorigen Feld muss hier genommen werden
    if (fieldDECL && fieldDECL->TypeFlags.Contains(substitutable)
       || vTypeDECL && vTypeDECL->TypeFlags.Contains(substitutable))
      ckd.tempCtx.ContextFlags = SET(multiContext,-1);

    if (cheo && !setErr) {
      vv = ckd.document->IDTable.GetVar(dod->ID, itype,ckd.inINCL);
      if (itype == globalID) {
        fieldDECL = *(LavaDECL**)vv;
        if (fieldDECL->TypeFlags.Contains(isOptional))
          dod->flags.INCL(isOptionalExpr);
        else
          dod->flags.EXCL(isOptionalExpr);
        if (fieldDECL->DeclType == Attr) {
          if (typeDECL)
            fldInType = FieldInType(ckd, typeDECL, fieldDECL);
          if (!typeDECL || (typeDECL && (fldInType == 1 || fldInType == 0))) {
//#ifndef INTERPRETER
            if (name.l)
              name += pkt;
            name += fieldDECL->LocalName;
//#endif
            dod->name = fieldDECL->LocalName;
            if (fldInType == 0)
              setErr = true;
          }
          else
            setErr = true;
          dod->fieldDecl = fieldDECL;
          if (fieldDECL->TypeFlags.Contains(isProtected))
            if (ckd.selfTypeDECL->DeclType == Initiator) {
              dod->SetError(ckd,&ERR_Protected);
              ok = false;
            }
            else {
              implItfDecl = ckd.document->IDTable.GetDECL(((CHETID*)ckd.selfTypeDECL->Supports.first)->data,ckd.inINCL);
              if (!ckd.document->IDTable.IsAn(OWNID(parentTypeDecl),0,OWNID(implItfDecl),0)) {
                dod->SetError(ckd,&ERR_Protected);
                ok = false;
              }
            }
#ifdef INTERPRETER
          dod->sectionNumber = ckd.document->GetMemsSectionNumber(ckd, typeDECL, fieldDECL);
#endif
          ckd.document->MemberTypeContext(fieldDECL, ckd.tempCtx,&ckd);
          vTypeDECL = ckd.document->GetFinalMVType(fieldDECL->RefID, fieldDECL->inINCL, ckd.tempCtx, parent->myCategory,&ckd);
          if (!cheo->successor) //22.5.01
            parent->myContext = ckd.tempCtx;
          typeDECL = ckd.document->GetTypeAndContext(vTypeDECL, ckd.tempCtx);
          if (!typeDECL && (!vTypeDECL || (vTypeDECL->RefID.nID >= 0) || cheo->successor)) {
            dod->SetError(ckd,&ERR_UndefType);
            ok = false;
          }
#ifdef INTERPRETER
          dod->vType = ckd.document->IDTable.GetDECL(fieldDECL->RefID, fieldDECL->inINCL);
          dod->eType = typeDECL;
          dod->isProperty = fieldDECL->TypeFlags.Contains(hasSetGet);
          dod->vSectionNumber = ckd.document->GetVTSectionNumber(ckd, dod->context, dod->vType, dod->isOuter);
#endif
          if (!cheo->successor) {
            parent->myFinalVType = vTypeDECL;
            if (fieldDECL->TypeFlags.Contains(substitutable))
              parent->flags.INCL(isSubstitutable);
            if (parent->myCategory == unknownCategory
                && fieldDECL->TypeFlags.Contains(trueObjCat))
              if (fieldDECL->TypeFlags.Contains(stateObject))
                parent->myCategory = stateObj;
              else
                parent->myCategory = valueObj;
          }
        }//if Attr
        else  //not Attr
          setErr = true;
      }
      else  //not globalID
        setErr = true;
    }
    if (setErr) {
//#ifndef INTERPRETER
      if (!dod->name.l) {
        Convert.IntToString(dod->ID.nID, strID);
        Convert.IntToString(dod->ID.nINCL, strINCL);
        dod->name = "(" + strID + "," + strINCL + ")";
        if (name.l)
          name = name + ".(" + strID + "," + strINCL + ")";
        else
          name = "(" + strID + "," + strINCL + ")";
      }
//#endif
      switch (fldInType) {
      case 0:
        dod->SetError(ckd,&ERR_Private);
        break;
      default:
        dod->SetError(ckd,&ERR_Broken_ref);
      }
#ifdef INTERPRETER
      return false;
#else
      ok = false;
      setErr = false;
#endif
    }
    parentTypeDecl = typeDECL;
    cheo = (CHE*)cheo->successor;
  }//while
  return ok;
}


#ifdef INTERPRETER
AssertionData::AssertionData(CheckData &ckd, LavaDECL *funcDECL) {
  CHETID *chp;
  LavaDECL *baseFuncDECL;

  requireDECL = 0;
  ensureDECL = 0;
  requireDECLimpl = 0;
  ensureDECLimpl = 0;
  maxFrameSize = 0;
  stackFrameSize = 0;
  nTotalOldExpr = 0;
  this->funcDECL = funcDECL;
  hasOrInheritsPreconditions = false;
  hasOrInheritsPostconditions = false;
  hasOrInheritsInvariants = false;

  if (funcDECL->ParentDECL->DeclType == Interface
  && funcDECL->Supports.first) { // private functions don't need overridden-table
    for (chp=(CHETID*)funcDECL->Supports.first;
         chp;
         chp=(CHETID*)chp->successor) {
      baseFuncDECL = ckd.document->IDTable.GetDECL(chp->data, funcDECL->inINCL);
      if (ckd.document->IsCDerivation(funcDECL->ParentDECL,baseFuncDECL->ParentDECL,&ckd))
        // only for classical class derivations!
        overridden.append((AssertionData*)baseFuncDECL->Exec.ptr);
    }
  }
}

unsigned AssertionData::PrepareAssertions(CheckData &ckd, SelfVar *selfVar) {
  LavaDECL *implFuncDECL, *itfFuncDECL=0;
  TID itfFuncTID;
  AssertionData *adp;
  unsigned i=0;

  nOwnOldExpr=oldExpressions.count();
  nOvrOldExpr=0;
  maxFrameSize=ckd.stackFrameSize;

  implFuncDECL = selfVar->execDECL->ParentDECL;
  if (((LavaDECL*)((CHE*)implFuncDECL->ParentDECL->NestedDecls.last)->data)->DeclType == ExecDef) {
    hasOrInheritsInvariants = true;
    invariantDECLimpl = (LavaDECL*)((CHE*)implFuncDECL->ParentDECL->NestedDecls.last)->data;
  }

  requireDECLimpl = ((CLavaBaseDoc*)ckd.document)->GetExecDECL(implFuncDECL,Require,false,false);
  if (requireDECLimpl) {
    hasOrInheritsPreconditions = true;
    maxFrameSize = lmax(maxFrameSize,((SelfVar*)requireDECLimpl->Exec.ptr)->stackFrameSize);
  }
  ensureDECLimpl = ((CLavaBaseDoc*)ckd.document)->GetExecDECL(implFuncDECL,Ensure,false,false);
  if (ensureDECLimpl) {
    hasOrInheritsPostconditions = true;
    maxFrameSize = lmax(maxFrameSize,((SelfVar*)ensureDECLimpl->Exec.ptr)->stackFrameSize);
  }

  if (implFuncDECL->Supports.first) {
    itfFuncTID = ((CHETID*)implFuncDECL->Supports.first)->data;
    itfFuncDECL = ckd.document->IDTable.GetDECL(itfFuncTID, implFuncDECL->inINCL);
  }

  if (!itfFuncDECL || (itfFuncDECL->DeclType != Function)) {
    stackFrameSize = maxFrameSize + nOwnOldExpr + nOvrOldExpr;
    return stackFrameSize;
  }

  if (((LavaDECL*)((CHE*)itfFuncDECL->ParentDECL->NestedDecls.last)->data)->DeclType == ExecDef) {
    hasOrInheritsInvariants = true;
    invariantDECL = (LavaDECL*)((CHE*)itfFuncDECL->ParentDECL->NestedDecls.last)->data;
  }

  requireDECL = ((CLavaBaseDoc*)ckd.document)->GetExecDECL(itfFuncDECL,Require,false,false);
  if (requireDECL) {
    hasOrInheritsPreconditions = true;
    maxFrameSize = lmax(maxFrameSize,((SelfVar*)requireDECL->Exec.ptr)->stackFrameSize);
  }
  ensureDECL = ((CLavaBaseDoc*)ckd.document)->GetExecDECL(itfFuncDECL,Ensure,false,false);
  if (ensureDECL) {
    hasOrInheritsPostconditions = true;
    maxFrameSize = lmax(maxFrameSize,((SelfVar*)ensureDECL->Exec.ptr)->stackFrameSize);
  }

  if (itfFuncDECL->TypeFlags.Contains(isInitializer)) {
    stackFrameSize = maxFrameSize + nOwnOldExpr + nOvrOldExpr;
    return stackFrameSize;
  }

  for (adp=overridden.first();
       adp;
       adp=overridden.next()) {
    maxFrameSize = lmax(maxFrameSize,adp->maxFrameSize);
    nOvrOldExpr += adp->nTotalOldExpr;
    if (adp->hasOrInheritsPreconditions)
      hasOrInheritsPreconditions = true;
    if (adp->hasOrInheritsPostconditions)
      hasOrInheritsPostconditions = true;
    if (adp->hasOrInheritsInvariants)
      hasOrInheritsInvariants = true;
  }

  nTotalOldExpr = nOwnOldExpr + nOvrOldExpr;
  stackFrameSize = maxFrameSize+nTotalOldExpr;
  return stackFrameSize;
}


InvarData::InvarData(CheckData &ckd, LavaDECL *itfDECL) {
  CHETID *chp;
  LavaDECL *baseItfDECL;

  invariantDECL = 0;
  invariantDECLimpl = 0;
  this->itfDECL = itfDECL;
  stackFrameSize = 0;
  hasOrInheritsInvariants = false;

  if (itfDECL->Supports.first) { // private functions don't need overridden-table
    for (chp=(CHETID*)itfDECL->Supports.first;
         chp;
         chp=(CHETID*)chp->successor) {
      baseItfDECL = ckd.document->IDTable.GetDECL(chp->data, itfDECL->inINCL);
      if (ckd.document->IsCDerivation(itfDECL,baseItfDECL,&ckd))
        // only for classical class derivations!
        overridden.append((InvarData*)baseItfDECL->Exec.ptr);
    }
  }
}


unsigned InvarData::PrepareInvariants(CheckData &ckd, SelfVar *selfVar) {
  LavaDECL *implDECL;
  TID itfTID;
  InvarData *idp;
  unsigned i=0;

  maxFrameSize=ckd.stackFrameSize;

  if (itfDECL->DeclType == Interface) {
    if (itfDECL->NestedDecls.last
      && ((LavaDECL*)((CHE*)itfDECL->NestedDecls.last)->data)->DeclType == ExecDef) {
      invariantDECL = (LavaDECL*)((CHE*)itfDECL->NestedDecls.last)->data;
      hasOrInheritsInvariants = true;
    }
    implDECL = itfDECL->RuntimeDECL;
    if (implDECL->NestedDecls.last
      && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclType == ExecDef) {
      invariantDECLimpl = (LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data;
      hasOrInheritsInvariants = true;
    }
  }
  else { // Impl 
    if (itfDECL->NestedDecls.last
      && ((LavaDECL*)((CHE*)itfDECL->NestedDecls.last)->data)->DeclType == ExecDef) {
      invariantDECLimpl = (LavaDECL*)((CHE*)itfDECL->NestedDecls.last)->data;
      hasOrInheritsInvariants = true;
    }
  }

  for (idp=overridden.first();
       idp;
       idp=overridden.next()) {
    maxFrameSize = lmax(maxFrameSize,idp->stackFrameSize);
    if (idp->hasOrInheritsInvariants)
      hasOrInheritsInvariants = true;
  }

  stackFrameSize = maxFrameSize;
  return stackFrameSize;
}

#endif

