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
#include "Check.h"
#include "BAdapter.h"
#include "LavaBaseDoc.h"
#include "LavaThread.h"
#include "LavaBaseStringInit.h"
#include "LavaProgram.h"
#include "qmap.h"
#include "qstring.h"
#include <stdlib.h>
#include <errno.h>
#ifndef WIN32
#include <setjmp.h>
#endif
 
#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()

#define SUCCEED {if (ckd.exceptionThrown || ckd.immediateReturn) return false; else return true;}

#define RETURN(OK) {if (!(OK) || ckd.exceptionThrown) return false; else return true;}

#ifdef WIN32
#define FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; unsigned last=(FRAMESIZE>>2)-1;\
  NEWSTACK[last] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[last-1] = (LavaObjectPtr)(SynObject*)this;\
  try { RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
  catch (CHWException*) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    throw;\
  }\
  catch (CRuntimeException* ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (ex->SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (caught) {\
  __asm {\
    sub esp, FRAMESIZE}\
  }\
}
#else
#define FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; unsigned last=(FRAMESIZE>>2)-1;\
  NEWSTACK[last] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[last-1] = (LavaObjectPtr)(SynObject*)this;\
  try {\
    if (!setjmp(contOnHWexception)) {\
      RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
    else\
      throw hwException;\
  }\
  catch (CHWException*) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    throw;\
  }\
  catch (CRuntimeException* ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (ex->SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
}
#endif


#ifdef WIN32
#define TRY_FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; unsigned last=(FRAMESIZE>>2)-1;\
  NEWSTACK[last] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[last-1] = (LavaObjectPtr)(SynObject*)this;\
  try { RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
  catch (CHWException* ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (ex->SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (CRuntimeException* ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (ex->SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (caught) {\
  __asm {\
    sub esp, FRAMESIZE}\
  }\
}
#else
#define TRY_FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; unsigned last=(FRAMESIZE>>2)-1;\
  NEWSTACK[last] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[last-1] = (LavaObjectPtr)(SynObject*)this;\
  try { \
    if (!setjmp(contOnHWexception)) {\
      RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
    else\
      throw hwException;\
  }\
  catch (CHWException* ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (!ex->SetLavaException(CKD))\
      throw;\
  }\
  catch (CRuntimeException* ex) {\
    ckd.selfVar = mySelfVar;\
    RESULT = false;\
    CKD.callStack = CallStack(CKD,stackFrame);\
    if (!ex->SetLavaException(CKD))\
      throw;\
  }\
}
#endif


QString SynObject::CallStack(CheckData &ckd,LavaVariablePtr crashStack) {
  SynObject *synObj=this, *oldSynObj;
  CHE *chp;
  LavaVariablePtr stack=crashStack;
  QString cFileName, cSynObjName, msg, path;
  unsigned last, inINCL;

  if (synObj->primaryToken == parameter_T)
    synObj = (SynObject*)((Parameter*)synObj)->parameter.ptr;

  msg = QString("Call stack:\n");

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
    else if (synObj->primaryToken == TypeRef_T)
      cSynObjName = QString(((Reference*)synObj)->refName.c);
    else if (synObj->primaryToken == Boolean_T)
      cSynObjName = ((BoolConst*)synObj)->boolValue?QString("true"):QString("false");
    else if (synObj->primaryToken == Const_T)
      cSynObjName = QString(((Constant*)synObj)->str.c);
    else if (synObj->primaryToken == enumConst_T)
      cSynObjName = QString(((EnumConst*)synObj)->Id.c);
    else if (synObj->primaryToken == nil_T)
      cSynObjName = QString("nil");
    else if (synObj->IsFuncInvocation()) {
      path = "| " + QString(((Reference*)((FuncExpression*)synObj)->function.ptr)->refName.c)
             + "() " + path;
      synObj = synObj->parentObject;
      continue;
    }
    else if (synObj->primaryToken == arrayAtIndex_T)
      cSynObjName = QString(((ObjReference*)((ArrayAtIndex*)synObj)->arrayObj.ptr)->refName.c)
                    + "[]";
    else if (synObj->type == implementation_T) { // exec SelfVar
      oldSynObj = synObj;
      inINCL = ((SelfVar*)synObj)->inINCL;
      cFileName = QString(ckd.document->IDTable.IDTab[inINCL]->FileName.c);
      last = (((SelfVar*)synObj)->stackFrameSize>>2)-1;
      synObj = (SynObject*)(stack[last-1]);
      stack = (LavaVariablePtr)stack[last];
      switch (oldSynObj->primaryToken) {
      case constraint_T:
        path = QString("\n| invariant of ")
               + ((SelfVar*)oldSynObj)->execDECL->FullName.c
               + ", file " + cFileName + "\n" + path;
        continue;
      case function_T:
      case initializer_T:
      case dftInitializer_T:
        path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
               + "(), file " + cFileName + "\n" + path;
        break;
      default: // initiator
        path = "\ninitiator "
               + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
               + ", file "
               + cFileName + "\n" + path;
      }
      if (stack) {
        synObj = synObj->parentObject;
        continue;
      }
      else
        break;
    }
    else
      cSynObjName = QString(TOKENSTR[synObj->primaryToken]);

    if (stack) {
      if (synObj->parentObject->primaryToken == parameter_T
      || synObj->parentObject->IsUnaryOp()
      || ((synObj->parentObject->IsMultOp()
          || synObj->parentObject->IsBinaryOp())
         && synObj->parentObject->type != Stm_T))
        cSynObjName = QString("(") + cSynObjName + ")";
      path = "| " + cSynObjName + " " + path;
      if (synObj->parentObject->primaryToken == parameter_T)
        synObj = synObj->parentObject;
      synObj = synObj->parentObject;
    }
    else
      path = cSynObjName + path;
  } while (stack);
  return msg + path;
}

void SynObject::SetRTError(CheckData &ckd,QString *errorCode,LavaVariablePtr stackFrame,const char *textParam) {
  SynObject *synObj=this;
  QString msgText, varName;
  msgText = *errorCode;
  bool thr = false;
  int code;

  if (ckd.exceptionThrown)
    return;

  if (errorCode == &ERR_NullCallObject)
    code = NullCallObject_ex;
  else if (errorCode == &ERR_NullMandatory)
    code = NullMandatory_ex;
  else if(errorCode == &ERR_NullParent)
    code = NullParent_ex;
  else if(errorCode == &ERR_NullException)
    code = NullException_ex;
  else if(errorCode == &ERR_AssertionViolation)
    code = AssertionViolation_ex;
  else if(errorCode == &ERR_IntegerRange)
    code = IntegerRange_ex;
  else if(errorCode == &ERR_EnumOrdLow)
    code = EnumOrdLow_ex;
  else if(errorCode == &ERR_EnumOrdHigh)
    code = EnumOrdHigh_ex;
  else if(errorCode == &ERR_AssigToFrozen)
    code = AssigToFrozen_ex;
  else if(errorCode == &ERR_ZombieAccess)
    code = ZombieAccess_ex;
  else if(errorCode == &ERR_UnfinishedObject)
    code = UnfinishedObject_ex;
  else if(errorCode == &ERR_ExecutionFailed)
    code = ExecutionFailed_ex;
  else if(errorCode == &ERR_AllocObjectFailed)
    code = memory_ex;
  else if(errorCode == &ERR_OutFunctionFailed)
    code = OutFunctionFailed_ex;
  else
    code = RunTimeException_ex;
    //thr = true;

  ckd.callStack = CallStack(ckd,stackFrame);

  if (textParam)
    msgText = QString(textParam) + ": " + msgText;
  else if (errorCode == &ERR_AssertionViolation
  && comment.ptr)
    msgText = msgText + ": " + comment.ptr->str.c;

  if (!SetLavaException(ckd, code, msgText)) {
    CRuntimeException* ex = new CRuntimeException(code, &msgText);
    throw ex;
  }
//!!!  if (thr)
//    throw (new CUserException);

  ckd.exceptionThrown = true;
}


bool SelfVarX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  int oldStackLevel=ckd.currentStackLevel, secN;
  unsigned iOldExpr=0;
  bool ok;
  CHE *baseInit;
  LavaObjectPtr callObj, obj=0;
  LavaVariablePtr newStackFrame;
  CVFuncDesc *fDesc;
  unsigned frameSize, inINCL=ckd.inINCL;
  BaseInit* baseInitDat;
  SynObjectBase *mySelfVar=this;
  CRuntimeException *ex;
  OldExpression *oep;

  ckd.currentStackLevel = nParams;
  ckd.selfVar = this;

  for (oep = oldExpressions.first();
       oep;
       oep = oldExpressions.next()) {
    oep->iOldExpr = iOldExpr;
    obj = ((SynObject*)oep->paramExpr.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return false;
    stackFrame[(stackFrameSize>>2)-iOldExpr-1] = 0;
    ex = CopyObject(ckd,&obj,&stackFrame[(stackFrameSize>>2)-iOldExpr-1],((SynFlags*)(obj+1))->Contains(stateObjFlag),obj[0][0].classDECL);
    ckd.selfVar = mySelfVar;
    ok = !ex && !ckd.exceptionThrown;
    if (ex) {
      ex->message = ex->message + CallStack(ckd,stackFrame);
      if (ex->SetLavaException(ckd)) 
        delete ex;
    }
    if (obj)
      DFC(obj);
    if (!ok)
      return false;
    ((SynFlags*)(stackFrame[(stackFrameSize>>2)-iOldExpr-1]+1))->INCL(finished);
    iOldExpr++;
  }

  for (baseInit = (CHE*)baseInitCalls.first;
       baseInit;
       baseInit = (CHE*)baseInit->successor) {
    baseInitDat = (BaseInit*)baseInit->data;
    if (baseInitDat->initializerCall.ptr) {
      ok = ((SynObject*)baseInitDat->initializerCall.ptr)->Execute(ckd,stackFrame);
      if (ckd.exceptionThrown)
        return false;
    }
    else {
      secN = ((CLavaProgram*)ckd.document)->GetVStatCallSecN(ckd, stackFrame[0], selfCtx,
                                               ((Reference*)baseInitDat->baseItf.ptr)->refDecl,
                                               baseInitDat->vSectionNumber,
                                               baseInitDat->isOuter);
      if (ckd.exceptionThrown)
        goto ret;
      callObj = stackFrame[0] - (*stackFrame[0])[0].sectionOffset;
      fDesc = &(*callObj)[secN].funcDesc[0];
      frameSize = fDesc->stackFrameSize;
      if (frameSize) {
#ifdef WIN32
        __asm {
          sub esp, frameSize
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
        newStackFrame[0] = callObj + (*callObj)[secN].sectionOffset;
        if (fDesc->isNative) {
          TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
        }
        else
          FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)
#ifdef WIN32
        __asm {
          add esp, frameSize
        }
#else
        delete [] newStackFrame;
#endif
        if (ckd.exceptionThrown)
          goto ret;
      }
    }
  }
  ok = ((SynObject*)body.ptr)->Execute(ckd,stackFrame);
  if (!ok && nOutputs)
    SetRTError(ckd,&ERR_OutFunctionFailed,stackFrame);
ret:
  ckd.currentStackLevel = oldStackLevel;
  ckd.immediateReturn = false;
  RETURN(ok)
}

bool SemicolonOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame);
    if (!ok || ckd.exceptionThrown)
      return false;
    else if (ckd.immediateReturn)
      return true;
  }
  SUCCEED
}

bool OrOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame);
    if (ckd.exceptionThrown || ckd.immediateReturn)
      return ok; // ok is true in succeed case!
    else if (ok)
      return true;
  }

  return false;
}
/*
bool OrExOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  bool ok, firstTime=true;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      if (opd->successor) {
        ckd.exceptionThrown = false;
        firstTime = false;
      }
      else
        return false;
    }
    else {
      if (ckd.lastException && !firstTime)
        DropException(ckd,0);
      return ok;
    }
  }

  return false;
}
*/
bool XorOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  unsigned nTrue=0;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame);
    if (ckd.exceptionThrown || ckd.immediateReturn)
      return false;
    else if (ok)
      nTrue++;
  }
  if (nTrue == 1)
    return true;
  else
    return false;
}

bool FailStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr object=0, lastEx=ckd.lastException;
  int ii;
  QString callStack;

  if (lastEx)
    DFC(lastEx);
  if (exception.ptr) {
    object = ((Expression*)exception.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return false;
    if (!object) {
      ((SynObject*)exception.ptr)->SetRTError(ckd,&ERR_NullException,stackFrame,"fail statement: exception object =0");
      return false;
    }
    ((SynFlags*)(object+1))->INCL(finished);
    for (ii = 0; (ii < object[0][0].nSections) && (object[0][ii].classDECL != ckd.document->DECLTab[B_Exception]); ii++);
    ckd.callStack = CallStack(ckd,stackFrame);
    ckd.lastException = object + object[0][ii].sectionOffset;
    ckd.exceptionThrown = true;
  }
  else
    ckd.immediateReturn = true;
  return false;
}

LavaObjectPtr OldExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  return stackFrame[(((SelfVar*)ckd.selfVar)->stackFrameSize>>2)-iOldExpr-1];
}

LavaObjectPtr UnaryOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  Expression *opd;
  LavaObjectPtr object;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize;

  if (flags.Contains(ignoreSynObj)) return 0;

  opd = (Expression*)operand.ptr;
  object = opd->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return object;
  if (!object) {
    SetRTError(ckd,&ERR_NullCallObject,stackFrame,"UnaryOpX::Evaluate");
    return (LavaObjectPtr)-1;
  }
  funcSect = &(*object)[funcSectionNumber + funcDecl->SectionInfo2];
  fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
  frameSize = fDesc->stackFrameSize;
#ifdef WIN32
  __asm {
    sub esp, frameSize
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  newStackFrame[0] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
//                            + funcSect[fDesc->delta].sectionOffset;
  newStackFrame[1] = 0;
  if (fDesc->isNative) 
    TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
  else 
    FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)

  DFC(object);
  object = newStackFrame[1];
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
	delete [] newStackFrame;
#endif
  if (!ok)
    return (LavaObjectPtr)-1;;
  return object;
}

bool BinaryOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  Expression *opd, *opd2;
  LavaObjectPtr object, object2;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize;

  if (flags.Contains(ignoreSynObj)) return true;

  opd = (Expression*)operand1.ptr;
  object = opd->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown) {
    if (object)
      DFC(object); 
    return false;
  }
  opd2 = (Expression*)operand2.ptr;
  object2 = opd2->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown) {
    if (object2)
      DFC(object2); 
    return false;
  }
  if (!object)
    if ((primaryToken != Equal_T && primaryToken != NotEqual_T) || object2) {
      SetRTError(ckd,&ERR_NullCallObject,stackFrame,"BinaryOpX::Execute");
      if (object2)
        DFC(object2); 
      return false;
    }
    else
      return (primaryToken == Equal_T);

  funcSect = &(*object)[funcSectionNumber + funcDecl->SectionInfo2];
  fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
  frameSize = fDesc->stackFrameSize;
#ifdef WIN32
  __asm {
    sub esp, frameSize
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  newStackFrame[0] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
  if (object2) {
    if (opd2->formVType->DeclType == VirtualType) {
      newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object2, callCtx, newStackFrame[0][0][0].classDECL, opd2->formVType, opd2->vSectionNumber, opd2->isOuter);
      if (ckd.exceptionThrown)
        goto ret;
    }
    else
      newStackFrame[1] = CASTOBJECT(object2, opd2->sectionNumber);
  }
  else
    newStackFrame[1] = 0;

  if (fDesc->isNative) 
    TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
  else 
    FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)

  if (newStackFrame[1])
    DFC(newStackFrame[1]);  // release input parameter
ret:
  DFC(newStackFrame[0]);  // release self
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
  delete [] newStackFrame;
#endif
  RETURN(ok)
}

LavaObjectPtr MultipleOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  LavaObjectPtr object, object2;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize;

  opd = (CHE*)operands.first;
  object = ((Expression*)opd->data)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return object;
  if (!object) {
    SetRTError(ckd,&ERR_NullCallObject,stackFrame,"MultipleOpX::Evaluate");
    return (LavaObjectPtr)-1;
  }
  funcSect = &(*object)[funcSectionNumber + funcDecl->SectionInfo2];
  fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
  frameSize = fDesc->stackFrameSize;
#ifdef WIN32
  __asm {
    sub esp, frameSize
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  newStackFrame[0] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
  opd = (CHE*)opd->successor;
  for (; opd; opd = (CHE*)opd->successor) {
    object2 = ((Expression*)opd->data)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      object = (LavaObjectPtr)-1;
      goto ret;
    }
    if (object2) {
      if (formVType->DeclType == VirtualType) {
        newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object2, callCtx, newStackFrame[0][0][0].classDECL, formVType, vSectionNumber, isOuter);
        if (ckd.exceptionThrown) {
          ok = false;
          object = (LavaObjectPtr)-1;
          goto ret0;
        }
      }
      else
        newStackFrame[1] = CASTOBJECT(object2,((Expression*)opd->data)->sectionNumber);
    }
    else
      newStackFrame[1] = 0;
    newStackFrame[2] = 0;

    if (fDesc->isNative) 
      TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)

    if (newStackFrame[1])
      DFC(newStackFrame[1]);  // release input parameter
ret0:
    DFC(newStackFrame[0]);  // release self
    if (!ok)
      goto ret;
    object = newStackFrame[2];
    if (!object) {
      SetRTError(ckd,&ERR_NullCallObject,stackFrame,"MultipleOpX::Evaluate3");
      goto ret;
    }
    newStackFrame[0] = object; // no cast???
  }
  if (formVType->DeclType == VirtualType) {
    object = ((CLavaProgram*)ckd.document)->CastVOutObj(ckd, object, callCtx, formVType, vSectionNumber, isOuter);
    if (ckd.exceptionThrown) {
      object = (LavaObjectPtr)-1;
      goto ret0;
    }
  }
ret:
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
  delete [] newStackFrame;
#endif
  return object;
}

LavaObjectPtr EvalExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr trueObj;
  
  trueObj = AllocateObject(ckd,ckd.document->DECLTab[B_Bool],flags.Contains(isVariable));
  if (!trueObj) {
    if (!ckd.exceptionThrown)
      SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
    return (LavaObjectPtr)-1;
  }
  ((SynFlags*)(trueObj+1))->INCL(finished);
  *(bool*)(trueObj+LSH) = ((SynObject*)operand.ptr)->Execute(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  return trueObj;
}

bool EvalStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr trueObj, object;
  bool ok;

  trueObj = AllocateObject(ckd,ckd.document->DECLTab[B_Bool],flags.Contains(isVariable));
  if (!trueObj) {
    if (!ckd.exceptionThrown)
      SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
    return false;
  }
  ((SynFlags*)(trueObj+1))->INCL(finished);
  *(bool*)(trueObj+LSH) = true;
  object = ((SynObject*)operand.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown) {
    DFC(trueObj);
    return false;
  }
  ok = EqualObjects(ckd,object,trueObj,0);
  DFC(trueObj);
  DFC(object);
  RETURN(ok);
}

bool InSetStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr setObj, setElem, fromObj, toObj;
  IntegerInterval *intv;
  int fromInt, toInt, secN, ii;
  bool ok;

  setElem = ((SynObject*)operand1.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return false;
  if (!setElem) {
    ((SynObject*)operand1.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
    return false;
  }
  if (((SynObject*)operand2.ptr)->IsIntIntv()) {
    intv = (IntegerInterval*)operand2.ptr;
    fromObj = ((SynObject*)intv->from.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      return false;
    }
    if (!fromObj) {
      ((SynObject*)intv->from.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      DFC(setElem);
      return false;
    }
    toObj = ((SynObject*)intv->to.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      DFC(fromObj);
      return false;
    }
    if (!toObj) {
      ((SynObject*)intv->to.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      DFC(setElem);
      DFC(fromObj);
      return false;
    }
    secN = ckd.document->GetSectionNumber(ckd, setElem[0][0].classDECL, ckd.document->DECLTab[Integer]);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      DFC(fromObj);
      return false;
    }
    setElem = CASTOBJECT(setElem,secN);
    ii = *(int*)(setElem+LSH);
    secN = ckd.document->GetSectionNumber(ckd, fromObj[0][0].classDECL, ckd.document->DECLTab[Integer]);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      DFC(fromObj);
      return false;
    }
    fromObj = CASTOBJECT(fromObj,secN);
    fromInt = *(int*)(fromObj+LSH);
    secN = ckd.document->GetSectionNumber(ckd, toObj[0][0].classDECL, ckd.document->DECLTab[Integer]);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      DFC(fromObj);
      return false;
    }
    toObj = CASTOBJECT(toObj,secN);
    toInt = *(int*)(toObj+LSH);
    DFC(setElem);
    DFC(fromObj);
    DFC(toObj);
    if (fromInt <= toInt)
      if (ii >= fromInt && ii <= toInt)
        return true;
      else
        return false;
    else
      if (ii <= fromInt && ii >= toInt)
        return true;
      else
        return false;
  }
  else {
    setObj = ((SynObject*)operand2.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      return false;
    }
    if (!setObj) {
      ((SynObject*)operand2.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      DFC(setElem);
      return false;
    }
    ok = HSetContains(setObj,setElem);
    DFC(setElem);
    DFC(setObj);
    RETURN(ok)
  }
}


bool AssertStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {

  if (flags.Contains(ignoreSynObj)) return true;

//  if (debugMode) {
  if (!((SynObject*)statement.ptr)->Execute(ckd,stackFrame)) {
    SetRTError(ckd,&ERR_AssertionViolation,stackFrame);
    return false;
  }
  else
    SUCCEED
/*  }
  else
    SUCCEED*/
}

bool IfStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)ifThens.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)((IfThenX*)opd->data)->ifCondition.ptr)->Execute(ckd,stackFrame);
    if (ckd.exceptionThrown || ckd.immediateReturn)
      return false;
    else if (ok)
      RETURN(((SynObject*)((IfThenX*)opd->data)->thenPart.ptr)->Execute(ckd,stackFrame))
  }

  if (elsePart.ptr)
    RETURN(((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame))
  else
    return true;
}

bool TryStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  ok = ((SynObject*)tryStatement.ptr)->Execute(ckd,stackFrame);
  if (!ckd.exceptionThrown)
    return ok;

  for ( opd=(CHE*)catchClauses.first;
        opd;
        opd = (CHE*)opd->successor) {
    ckd.exceptionThrown = false;
    ok = ((SynObject*)((CatchClauseX*)opd->data)->catchClause.ptr)->Execute(ckd,stackFrame);
    if (!ckd.exceptionThrown) {
      DropException(ckd,0);
			return ok;
    }
  }
	return false;
}
 
LavaObjectPtr IfExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *opd;
  LavaObjectPtr result=(LavaObjectPtr)-1;

  for ( opd=(CHE*)ifThens.first;
        opd;
        opd = (CHE*)opd->successor)
    if (((Expression*)((IfxThenX*)opd->data)->ifCondition.ptr)->Execute(ckd,stackFrame)) {
      if (ckd.exceptionThrown || ckd.immediateReturn)
        return (LavaObjectPtr)-1;
      result = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->Evaluate (ckd,stackFrame);
      if (ckd.exceptionThrown || ckd.immediateReturn)
        return (LavaObjectPtr)-1;
      if (((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->IsIfStmExpr())
        sectionNumber = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->sectionNumber;
      else if (((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->primaryToken != nil_T)
        sectionNumber = ckd.document->GetSectionNumber(ckd, ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->finalType,targetDecl);
      return result;
    }
  result = ((Expression*)elsePart.ptr)->Evaluate (ckd,stackFrame);
  if (ckd.exceptionThrown || ckd.immediateReturn)
    return (LavaObjectPtr)-1;
  if (((Expression*)elsePart.ptr)->IsIfStmExpr())
    sectionNumber = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->sectionNumber;
  else if (((Expression*)elsePart.ptr)->primaryToken != nil_T)
    sectionNumber = ckd.document->GetSectionNumber(ckd, ((Expression*)elsePart.ptr)->finalType,targetDecl);
  return result;
}

bool SwitchStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *branchp, *caseLabelp;
  BranchX *branch;
  SynObject *caseLabel;
  LavaObjectPtr caseExpr, caseLab;
  bool ok;

  if (flags.Contains(ignoreSynObj)) return true;

  caseExpr = ((Expression*)caseExpression.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown) {
    DFC(caseExpr);
    return false;
  }

  for ( branchp=(CHE*)branches.first;
        branchp;
        branchp = (CHE*)branchp->successor) {
    branch = (BranchX*)branchp->data;
    for ( caseLabelp=(CHE*)branch->caseLabels.first;
          caseLabelp;
          caseLabelp = (CHE*)caseLabelp->successor) {
      caseLabel = (SynObject*)caseLabelp->data;
      caseLab = caseLabel->Evaluate(ckd,stackFrame);
      if (ckd.exceptionThrown) {
        DFC(caseExpr);
        return false;
      }
      ok = EqualObjects(ckd,caseLab,caseExpr,0);
      if (ok) {
        DFC(caseExpr);
        ok = ((SynObject*)branch->thenPart.ptr)->Execute(ckd,stackFrame);
        if (ckd.exceptionThrown)
          return false;
        RETURN(ok)
      }
      DFC(caseLab);
    }
  }

  DFC(caseExpr);
  DFC(caseLab);
  if (elsePart.ptr)
    RETURN(((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame))
   else {
    SetRTError(ckd,&ERR_SwitchMissingElse,stackFrame);
    return false;
  }
}

bool TypeSwitchStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *branchp;
  TypeBranch *branch;
  LavaObjectPtr caseExpr, caseExprCasted;
  bool ok=true;
  int secN;

  if (flags.Contains(ignoreSynObj)) return true;

  ckd.currentStackLevel++;
  caseExpr =((Expression*)caseExpression.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown) {
    ckd.currentStackLevel--;
    return false;
  }
  if (!caseExpr) {
    ((Expression*)caseExpression.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
    ckd.currentStackLevel--;
    return false;
  }

  for ( branchp=(CHE*)branches.first;
        branchp;
        branchp = (CHE*)branchp->successor) {
    branch = (TypeBranch*)branchp->data;
    caseExprCasted = caseExpr-caseExpr[0][0].sectionOffset;
    secN = ckd.document->GetSectionNumber(ckd, caseExprCasted[0][0].classDECL, branch->typeDecl, false);
    if (ckd.exceptionThrown) {
      DFC(caseExpr);
      ckd.currentStackLevel--;
      return false;
    }
    if ( secN >= 0) {
      caseExprCasted = CASTOBJECT(caseExprCasted,secN);
      stackFrame[((VarName*)branch->varName.ptr)->stackPos] = caseExprCasted;
      ok = ((SynObject*)branch->thenPart.ptr)->Execute(ckd,stackFrame);
      DFC(caseExpr);
      ckd.currentStackLevel--;
      RETURN(ok)
    }
  }

  DFC(caseExpr);
  if (elsePart.ptr)
    ok = ((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame);

  ckd.currentStackLevel--;
  RETURN(ok)
}

bool DeclareX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  unsigned i;
  bool result;
  unsigned stackLevel = ckd.currentStackLevel;
  
  if (flags.Contains(ignoreSynObj)) return true;

  for ( i = stackLevel;
        i < stackLevel+nQuantVars;
        i++)
    stackFrame[i] = 0;
  ckd.currentStackLevel += nQuantVars;
  result = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame);
  for ( i = stackLevel;
        i < stackLevel+nQuantVars;
        i++)
    if (stackFrame[i])
      DFC(stackFrame[i]);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(result);
}

static bool Enumerate (SynObject *callObj, CheckData &ckd, LavaVariablePtr stackFrame, 
                       CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0) {
  CHAINX *setChain;
  CHE *cheElem, *cheElemSucc;
  Quantifier *quant;
  IntegerInterval *intv;
  CContext context;
  LavaObjectPtr setObj, elemObj, fromObj, toObj, enumBaseObj;
  LavaDECL *enumDecl;
  TEnumDescription *enumDesc;
  CHEEnumSelId *cheItem;
  DString *enumId;
  int fromInt, toInt, index, secN, ii;
  bool ok=true;

  quant = (Quantifier*)cheQuant->data;
  if (((SynObject*)quant->set.ptr)->IsIntIntv()) { // Integer interval
    intv = (IntegerInterval*)quant->set.ptr;
    fromObj = ((SynObject*)intv->from.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return false;
    if (!fromObj) {
      ((SynObject*)intv->from.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      return false;
    }
    secN = ckd.document->GetSectionNumber(ckd, fromObj[0][0].classDECL, ckd.document->DECLTab[Integer]);
    if (ckd.exceptionThrown) {
      DFC(fromObj);
      return false;
    }
    fromObj = CASTOBJECT(fromObj,secN);
    fromInt = *(int*)(fromObj+LSH);
    DFC(fromObj);
    toObj = ((SynObject*)intv->to.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return false;
    if (!toObj) {
      ((SynObject*)intv->to.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      return false;
    }
    secN = ckd.document->GetSectionNumber(ckd, toObj[0][0].classDECL, ckd.document->DECLTab[Integer]);
    if (ckd.exceptionThrown) {
      DFC(toObj);
      return false;
    }
    toObj = CASTOBJECT(toObj,secN);
    toInt = *(int*)(toObj+LSH);
    DFC(toObj);
    if (fromInt <= toInt)
      for (index = fromInt; index <= toInt; index++) {
        elemObj = AllocateObject(ckd,ckd.document->DECLTab[Integer],false);
        if (!elemObj) {
          if (!ckd.exceptionThrown)
            ((VarName*)cheVar->data)->SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
          break;
        }
        *(int*)(elemObj+LSH) = index;
        stackFrame[((VarName*)cheVar->data)->stackPos] = elemObj; // set element
        ok = callObj->Recursion(ckd,stackFrame,cheQuant,cheVar,rSet);
        DFC(elemObj);
        if (!ok) break;
      }
    else
      for (index = fromInt; index >= toInt; index--) {
        elemObj = AllocateObject(ckd,ckd.document->DECLTab[Integer],false);
        if (!elemObj) {
          if (!ckd.exceptionThrown)
            ((VarName*)cheVar->data)->SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
          break;
        }
        *(int*)(elemObj+LSH) = index;
        stackFrame[((VarName*)cheVar->data)->stackPos] = elemObj; // set element
        ok = callObj->Recursion(ckd,stackFrame,cheQuant,cheVar,rSet);
        DFC(elemObj);
        if (!ok) break;
      }
  }
  else if (((SynObject*)quant->set.ptr)->primaryToken == TypeRef_T) { // Enumeration type
    enumDecl = ((Reference*)quant->set.ptr)->refDecl;
    enumDesc = (TEnumDescription*)enumDecl->EnumDesc.ptr;
    ii = 0;
    for (cheItem = (CHEEnumSelId*)enumDesc->EnumField.Items.first;
         cheItem;
         cheItem = (CHEEnumSelId*)cheItem->successor) {
      enumId = &cheItem->data.Id;
      elemObj = AllocateObject(ckd,enumDecl,false);
      if (!elemObj) {
        if (!ckd.exceptionThrown)
          ((VarName*)cheVar->data)->SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
        break;
      }
      enumBaseObj = CastEnumType(ckd, elemObj);
      if (ckd.exceptionThrown) {
        DFC(elemObj);
        break;
      }
      ((SynFlags*)(enumBaseObj+1))->INCL(finished);
      *(int*)(enumBaseObj+LSH) = ii++;
      NewQString((QString*)(enumBaseObj+LSH+1),enumId->c);
      stackFrame[((VarName*)cheVar->data)->stackPos] = elemObj; // set element
      ok = callObj->Recursion(ckd,stackFrame,cheQuant,cheVar,rSet);
      DFC(elemObj);
      if (!ok) break;
    }
  }
  else { // Set type
    setObj = CastSetType(ckd, stackFrame[((ObjReference*)quant->set.ptr)->stackPos]);
    if (ckd.exceptionThrown)
      return false;
    if (!setObj) {
      ((ObjReference*)quant->set.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      return false;
    }
    setChain = (CHAINX*)(setObj+LSH);

    for (cheElem = (CHE*)setChain->first; cheElem && ok; cheElem = cheElemSucc) {
      cheElemSucc = (CHE*)cheElem->successor; // since cheElem might be deleted in the loop
      elemObj = (LavaObjectPtr)cheElem->data;
      if (quant->elemType.ptr) {
        elemObj = elemObj - elemObj[0][0].sectionOffset;
        secN = ckd.document->GetSectionNumber(ckd, elemObj[0][0].classDECL, quant->typeDecl, false);
        if (ckd.exceptionThrown)
          return false;
        if ( secN >= 0) 
          elemObj = CASTOBJECT(elemObj,secN);
        else
          continue;
      }
      else {
        elemObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, elemObj, quant->setElemDecl);
        if (ckd.exceptionThrown)
          return false;
      }

      stackFrame[((VarName*)cheVar->data)->stackPos] = elemObj; // set element
      stackFrame[((VarName*)cheVar->data)->stackPos + 1] = (LavaObjectPtr)cheElem-LSH-1; //set element handle

      ok = callObj->Recursion(ckd,stackFrame,cheQuant,cheVar,rSet);
      if (!ok) break;
    }
  }

  return ok;
}

bool ExistsX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame,
                         CHE *cheQuant, CHE *cheVar, LavaObjectPtr) {
  Quantifier *quant;
//  LavaObjectPtr setObj;
//  CHAINX *setChain;
  CHE /**cheElem,*/ *cheVar2;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,cheQuant,(CHE*)cheVar->successor);
  else {
    if (cheQuant->successor) {
      cheQuant = (CHE*)cheQuant->successor;
      quant = (Quantifier*)cheQuant->data;
      cheVar2 = (CHE*)quant->quantVars.first;
/*      setObj = CastSetType(ckd, stackFrame[((ObjReference*)quant->set.ptr)->stackPos]);
      if (ckd.exceptionThrown)
        return false;
      setChain = (CHAINX*)(setObj+LSH);
      cheElem = (CHE*)setChain->first;*/
      ok = Enumerate(this,ckd,stackFrame,cheQuant,cheVar2);
    }
    else
      if (updateStatement.ptr) {
        if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame))
          ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame);
      }
      else
        ok = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame);
  }
  return ok;
}

bool ExistsX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  //unsigned i;
  unsigned stackLevel = ckd.currentStackLevel;
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  bool ok;
  
  if (flags.Contains(ignoreSynObj)) return true;

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  ok = Enumerate(this,ckd,stackFrame,cheQuant,cheVar);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(ok)
}

bool ForeachX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame,
                          CHE *cheQuant, CHE *cheVar, LavaObjectPtr) {
  Quantifier *quant;
  CHE *cheVar2;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,cheQuant,(CHE*)cheVar->successor);
  else {
    if (cheQuant->successor) {
      cheQuant = (CHE*)cheQuant->successor;
      quant = (Quantifier*)cheQuant->data;
      cheVar2 = (CHE*)quant->quantVars.first;
      ok = Enumerate(this,ckd,stackFrame,cheQuant,cheVar2);
    }
    else
      if (statement.ptr)
        if (updateStatement.ptr) {
          if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame))
            ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame);
        }
        else
          ok = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame);
      else
        ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame);
  }
  return ok;
}

bool ForeachX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  //unsigned i;
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  bool ok;
  
  if (flags.Contains(ignoreSynObj)) return true;

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  ok = Enumerate(this,ckd,stackFrame,cheQuant,cheVar);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(ok)
}

bool SelectExpressionX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame,
                                   CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet) {
  Quantifier *quant;
  CHE *cheVar2;
  LavaObjectPtr resultObj;
  LavaVariablePtr newStackFrame;
  unsigned frameSize;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,cheQuant,(CHE*)cheVar->successor,rSet);
  else {
    if (cheQuant->successor) {
      cheQuant = (CHE*)cheQuant->successor;
      quant = (Quantifier*)cheQuant->data;
      cheVar2 = (CHE*)quant->quantVars.first;
      ok = Enumerate(this,ckd,stackFrame,cheQuant,cheVar2,rSet);
    }
    else
      if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame)) { // add resultObj to rSet
        resultObj = ((Expression*)addObject.ptr)->Evaluate(ckd,stackFrame);
        frameSize = 8;
#ifdef WIN32
        __asm {
          sub esp, frameSize
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
        newStackFrame[0] = CastSetType(ckd,rSet);
        newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVObj(ckd, resultObj,0);
        SetAdd(ckd, newStackFrame);
        DFC(resultObj); // release new object
        // release stackFrame frame
#ifdef WIN32
        __asm {
          add esp, frameSize
        }
#else
				delete [] newStackFrame;
#endif
      }
  }
  return ok;
}

LavaObjectPtr SelectExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  //unsigned i;
  unsigned stackLevel = ckd.currentStackLevel;
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  LavaObjectPtr rSet=0;
  
  /*for ( i = stackLevel;
        i < stackLevel+nQuantVars;
        i++)
    stackFrame[i] = 0;*/

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  rSet =((Expression*)resultSet.ptr)->Evaluate(ckd,stackFrame);

  Enumerate(this,ckd,stackFrame,cheQuant,cheVar,rSet);
  ckd.currentStackLevel -= nQuantVars;
  return rSet;
}

bool ObjReferenceX::assign (SynObject *source,
                    LavaObjectPtr object,
                    TargetType targetType,
                    unsigned sectionNumber,
                    CheckData &ckd,
                    LavaVariablePtr stackFrame) {

  LavaVariablePtr var, newStackFrame;
  LavaObjectPtr arrayObj, indexObj, propertyInfo;
  SynObject *arrayRef;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=1, frameSize;
  ArrayAtIndex *aai;
  LavaDECL *setExec;
  Expression *indexExpr;
  TDOD* lastDOD;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok=true;

  if (flags.Contains(isUnsafeMandatory) && !object) {
    SetRTError(ckd,&ERR_NullMandatory,stackFrame);
    return false;
  }

  switch (targetType) {
  case field:
fieldCase:
    if (!(var = GetMemberVarPtr(ckd,stackFrame,stackPos))) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_NullMandatory,stackFrame,"ObjReferenceX::GetMemberVarPtr");
      return false;
    }

    if (*var)
      if (flags.Contains(isReverseLink))
        DRC(*var)
      else
        DFC(*var)

    if (object) {
      if (!((SynFlags*)(object-(*object)->sectionOffset+1))->Contains(finished)
      && !flags.Contains(unfinishedAllowed)) {
        if (source)
          source->SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        else
          SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        DFC(object);
        return false;
      } // an unfinished object may be assigned only to a reverse link self.revLink
        // within an initializer

      if (refIDs.first != refIDs.last) {
        lastDOD = (TDOD*)((CHE*)refIDs.last)->data;
        if (lastDOD->vType->DeclType == VirtualType)
          *var = ((CLavaProgram*)ckd.document)->CastVObj(ckd, object, lastDOD->eType);
        else
          *var = CASTOBJECT(object, sectionNumber);
      }
      else
        *var = CASTOBJECT(object, sectionNumber);
      if (flags.Contains(isReverseLink)) {
        IRC(object);
        DFC(object); // in this order!
      }
    }
    else
      *var = 0;
    break;
 
  case property: // call set_property function
    propertyInfo = GetPropertyInfo(ckd,stackFrame,stackPos,setExec);
    if (setExec && setExec->Exec.ptr == ckd.selfVar) goto fieldCase;
    if (!propertyInfo) {
      SetRTError(ckd,&ERR_NullCallObject,stackFrame,"assign/set function");
      return false;
    }
    frameSize = ((SelfVar*)setExec->Exec.ptr)->stackFrameSize;
#ifdef WIN32
    __asm {
      sub esp, frameSize
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
    newStackFrame[0] = propertyInfo; // set self
    IFC(newStackFrame[0]); 
    if (object) {
      if (refIDs.first != refIDs.last) {
        lastDOD = (TDOD*)((CHE*)refIDs.last)->data;
        if (lastDOD->vType->DeclType == VirtualType) {
          ckd.tempCtx = lastDOD->context;
          ckd.document->GetTypeAndContext(lastDOD->vType, ckd.tempCtx);
          newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, ckd.tempCtx, newStackFrame[0][0][0].classDECL, lastDOD->vType, lastDOD->vSectionNumber, lastDOD->isOuter);
        }
        else
          newStackFrame[1] = CASTOBJECT(object, sectionNumber);
      }
      else
        newStackFrame[1] = CASTOBJECT(object, sectionNumber);
      IFC(newStackFrame[1]);  // set input parameter
    }
    else
      newStackFrame[1] = 0;

    ok = ((SynObject*)setExec->Exec.ptr)->Execute(ckd,newStackFrame);
    ckd.selfVar = mySelfVar;

    DFC(newStackFrame[0]);  // release self
    if (newStackFrame[1])
      DFC(newStackFrame[1]);  // release input parameter
#ifdef WIN32
    __asm {
      add esp, frameSize
    }
#else
    delete [] newStackFrame;
#endif
    break;

  case arrayElem: // call operator[]<- function
    aai = (ArrayAtIndex*)this;
    arrayRef = (SynObject*)aai->arrayObj.ptr;
    arrayObj = arrayRef->Evaluate(ckd,stackFrame);
    if (!arrayObj) {
      arrayRef->SetRTError(ckd,&ERR_NullCallObject,stackFrame,"assign array element, null-array");
      return false;
    }
    funcSect = &(*arrayObj)[aai->funcSectionNumber + aai->funcDecl->SectionInfo2];
    fDesc = &funcSect->funcDesc[aai->funcDecl->SectionInfo1];
#ifdef WIN32
    __asm {
      sub esp, 20
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[5];
#endif
    newStackFrame[0] = arrayObj - (*arrayObj)->sectionOffset
                       + (*(arrayObj - (*arrayObj)->sectionOffset))[fDesc->delta].sectionOffset;
    //IFC(newStackFrame[0]);  // set self
    indexExpr = (Expression*)aai->arrayIndex.ptr;
    indexObj = indexExpr->Evaluate(ckd,stackFrame);
    if (indexObj) {
      if (indexExpr->formVType->DeclType == VirtualType)
        newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, indexObj, aai->callCtx, newStackFrame[0][0][0].classDECL, indexExpr->formVType, indexExpr->vSectionNumber, indexExpr->isOuter);
      else
        newStackFrame[1] = CASTOBJECT(indexObj, indexExpr->sectionNumber);
    }
    else {
      source->SetRTError(ckd,&ERR_NullMandatory,stackFrame,"assign array element, null-index");
      goto ret0;
    }
    if (object)
      newStackFrame[2] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, aai->callCtx, newStackFrame[0][0][0].classDECL, aai->formVType, aai->vSectionNumber, aai->isOuter);
    else
      newStackFrame[2] = 0;

    if (fDesc->isNative) 
      TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,20,ok)
    else 
      FCALL(ckd,((SynObject*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,20,ok)

    if (newStackFrame[2])
      DFC(newStackFrame[2]);  // release input parameter 2
    DFC(newStackFrame[1]);  // release input parameter 1
ret0: DFC(newStackFrame[0]);  // release self

#ifdef WIN32
    __asm {
      add esp, 20
    }
#else
    delete [] newStackFrame;
#endif
    break;
  }

  return ok;
}

bool AssignmentX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr object;

  if (flags.Contains(ignoreSynObj)) return true;

  object = ((Expression*)exprValue.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return false;

  RETURN(((ObjReferenceX*)targetObj.ptr)->assign((Expression*)exprValue.ptr,
         object,
         kindOfTarget,
         ((Expression*)exprValue.ptr)->sectionNumber,
         ckd,
         stackFrame))
}

LavaObjectPtr ArrayAtIndexX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  Expression *array, *expr;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;

  array = (Expression*)arrayObj.ptr;
  callObj = array->Evaluate(ckd,stackFrame);
  if (!callObj) {
    array->SetRTError(ckd,&ERR_NullCallObject,stackFrame,"ArrayAtIndexX::Evaluate");
    return (LavaObjectPtr)-1;
  }
  funcSect = &(*callObj)[funcSectionNumber + funcDecl->SectionInfo2];
  fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
#ifdef WIN32
  __asm {
    sub esp, 20
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[5];
#endif
  newStackFrame[0] = callObj - (*callObj)->sectionOffset
                       + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
//                            + funcSect[fDesc->delta].sectionOffset;
  expr = (Expression*)arrayIndex.ptr;
  object = expr->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    goto ret;
  if (object) {
    if (expr->formVType->DeclType == VirtualType)
      newStackFrame[1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx/*callObj[0][0].classDECL*/, newStackFrame[0][0][0].classDECL, expr->formVType, expr->vSectionNumber, expr->isOuter);
    else
      newStackFrame[1] = CASTOBJECT(object, expr->sectionNumber);
  }
  else
    newStackFrame[1] = 0;
  newStackFrame[2] = 0;
  if (fDesc->isNative) 
    TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,20,ok)
  else 
    FCALL(ckd,((SynObject*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,20,ok)
  if (ok) {
    object = newStackFrame[2];
    if (object)
      object = ((CLavaProgram*)ckd.document)->CastVOutObj(ckd, object, callCtx, formVType, vSectionNumber, isOuter);
  }
ret:
  DFC(callObj); // release call object
  DFC(newStackFrame[1]); // release input parameter (= array index)
#ifdef WIN32
  __asm { // not earlier, since newStackFrame[1] is still used in the preceding statement
    add esp, 20
  }
#else
	delete [] newStackFrame;
#endif
  return object;
}

static unsigned char map(unsigned char inChar) {
  switch (inChar) {
  case '\\':
  case '?':
  case '\'':
  case '\"':
    return inChar;
  case 'a':
    return '\x07';
  case 'b':
    return '\x08';
  case 't':
    return '\x09';
  case 'n':
    return '\x0A';
  case 'v':
    return '\x0B';
  case 'f':
    return '\x0C';
  case 'r':
    return '\x0D';
  default:
    return '\0';
  }
}

LavaObjectPtr ConstantX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame)
{
  int i, k=-1;
  DString bitset;

  if (value)
    IFC(value)
  else {
    value = AllocateObject(ckd,typeDECL,flags.Contains(isVariable));
    if (!value) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
      return (LavaObjectPtr)-1;
    }

    ((SynFlags*)(value+1))->INCL(finished);

    switch (constType) {
    case Char:
      if (str[1] != '\\')
        *(char*)(value+LSH) = str[1];
      else
        *(char*)(value+LSH) = map(str[2]);
      break;
    case Integer:
      *(int*)(value+LSH) = strtol(str.c,0,0);
      if (errno == ERANGE) {
        SetRTError(ckd,&ERR_IntegerRange,stackFrame,"ConstantX::Evaluate");
        IFC(value); // for permanent ref from Constant
        allocatedObjects--;
        return (LavaObjectPtr)-1;
      }
      break;
    case Bitset:
      bitset = str;
      bitset.Replace("X",1);
      *(int*)(value+LSH) = strtoul(bitset.c,0,0);
      if (errno == ERANGE) {
        SetRTError(ckd,&ERR_IntegerRange,stackFrame,"ConstantX::Evaluate");
        IFC(value); // for permanent ref from Constant
        allocatedObjects--;
        return (LavaObjectPtr)-1;
      }
      break;
    case Float:
      *(float*)(value+LSH) = (float)strtod(str.c,0); //(float)atof(str.c);
      break;
    case Double:
      *(double*)(value+LSH) = strtod(str.c,0); //atof(str.c);
      break;
    case VLString:
      DString str1(str.l-1);
      for (i=1; i<str.l; i++) {
        if (str[i] == '"') break;
        if (str[i] != '\\')
          str1[++k] = str[i];
        else
          str1[++k] = map(str[i++ + 1]);
      }
      str1.l = k+1;
      str1[str1.l] = '\0';
      NewQString((QString*)(value+LSH), str1.c);
      break;
    }
    IFC(value); // for permanent ref from Constant
    allocatedObjects--;
    // is released only when syntax is released
  }
  return value; 
}

ConstantX::~ConstantX () {
  CheckData ckd;
  LavaVariablePtr newStackFrame;

  if (!value) return;
  switch (constType) {
  case VLString:
#ifdef WIN32
    __asm {
      sub esp, 8
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[2];
#endif
    newStackFrame[0] = value;
    StringDecFunc(ckd, newStackFrame);
#ifdef WIN32
    __asm {
      add esp, 8
    }
#else
		delete [] newStackFrame;
#endif
    break;
  }
  delete [](value-LOH);
}

LavaObjectPtr BoolConstX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame)
{
  if (value)
    IFC(value)
  else {
    value = AllocateObject(ckd,typeDECL,flags.Contains(isVariable));
    if (!value) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
      return (LavaObjectPtr)-1;
    }
    ((SynFlags*)(value+1))->INCL(finished);
    *(bool*)(value+LSH) = boolValue;
    IFC(value); // for permanent ref from BoolConst
    // is released only when syntax is released
    allocatedObjects--;
  }
  return value; 
}

BoolConstX::~BoolConstX () {
  if (value)
    delete [] (value-LOH);
}

LavaObjectPtr EnumConstX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame)
{
  if (value)
    IFC(value)
  else {
    value = AllocateObject(ckd,refDecl,flags.Contains(isVariable));
    if (!value) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
      return (LavaObjectPtr)-1;
    }
    ((SynFlags*)(value+1))->INCL(finished);
    enumBaseObj = CastEnumType(ckd, value);
    if (ckd.exceptionThrown)
      return (LavaObjectPtr)-1;
    *(int*)(enumBaseObj+LSH) = enumItem;
    NewQString((QString*)(enumBaseObj+LSH+1),Id.c);
    IFC(value); // for permanent ref from EnumConst
    // is released only when syntax is released
    allocatedObjects--;
  }
  return value; 
}

EnumConstX::~EnumConstX () {
  CheckData ckd;
  LavaObjectPtr newStackFrame[1];

  if (value) {
    newStackFrame[0] = enumBaseObj+1; // +1 since the QString value follows after the int value
    StringDecFunc(ckd,newStackFrame);
    delete [] (value-LOH);
  }
}

LavaObjectPtr ObjReferenceX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr object=GetMemberObjPtr(ckd,stackFrame,stackPos);
  if (object)
    IFC(object);  // set "output parameter"
  return object;
}

LavaObjectPtr CloneExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr obj, copyOfObj=0;
  CRuntimeException *ex;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;

  obj = ((ObjReferenceX*)fromObj.ptr)->Evaluate(ckd,stackFrame);  
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  ex = CopyObject(ckd,&obj,&copyOfObj,obj?((SynFlags*)(obj+1))->Contains(stateObjFlag):true/*ckd.stateObj*/);
  ckd.selfVar = mySelfVar;
  if (ex) {
    ex->message = ex->message + CallStack(ckd,stackFrame);
    if (ex->SetLavaException(ckd)) 
      delete ex;
  }
  if (obj)
    DFC(obj);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  
  stackFrame[((VarName*)varName.ptr)->stackPos] = copyOfObj;

  if (butStatement.ptr) {
    ((SynObject*)butStatement.ptr)->Execute(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return (LavaObjectPtr)-1;
  }

  if (execDECL) {
    ok = ((SelfVar*)execDECL->Exec.ptr)->Execute(ckd,stackFrame);
    ckd.selfVar = mySelfVar;
    if (!ok) {
      SetRTError(ckd,&ERR_AssertionViolation,stackFrame);
      return (LavaObjectPtr)-1;
    }
  }
  ((SynFlags*)(copyOfObj+1))->INCL(finished);
  return copyOfObj;
}

bool CopyStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr obj, copyOfObj;
  bool ok = true, nullTarget;
  CRuntimeException *ex;
  SynObjectBase *mySelfVar=ckd.selfVar;

  if (flags.Contains(ignoreSynObj)) return true;

  obj = ((ExpressionX*)fromObj.ptr)->Evaluate(ckd,stackFrame);  
  if (ckd.exceptionThrown)
    return false;
  copyOfObj = ((ObjReferenceX*)ontoObj.ptr)->Evaluate(ckd,stackFrame);  
  if (ckd.exceptionThrown) {
    if (obj)
      DFC(obj);
    return false;
  }
  nullTarget = copyOfObj?false:true;
  if (((SynObject*)ontoObj.ptr)->flags.Contains(isUnsafeMandatory) && !copyOfObj) {
    ((SynObject*)ontoObj.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
    if (obj)
      DFC(obj);
    return false;
  }
  if (nullTarget)
    ex = CopyObject(ckd,&obj,&copyOfObj,((ObjReferenceX*)ontoObj.ptr)->myCategory == stateObj,ontoTypeDecl);
  else
    ex = CopyObject(ckd,&obj,&copyOfObj,((ObjReferenceX*)ontoObj.ptr)->myCategory == stateObj);
  ckd.selfVar = mySelfVar;
  ok = !ex && !ckd.exceptionThrown;
  if (ex) {
    ex->message = ex->message + CallStack(ckd,stackFrame);
    if (ex->SetLavaException(ckd)) 
      delete ex;
  }
  if (obj)
    DFC(obj);
  if (!ok) {
    if (copyOfObj)
      DFC(copyOfObj);
    return false;
  }
  if (nullTarget) {
    ((SynFlags*)(copyOfObj+1))->INCL(finished);
    ok = ((ObjReferenceX*)ontoObj.ptr)->assign((ExpressionX*)fromObj.ptr,
           copyOfObj,
           kindOfTarget,
           0, // copyOfObj is already casted on the target type
           ckd,
           stackFrame);
  }
  else if (copyOfObj)
    DFC(copyOfObj);
  RETURN(ok)
}

LavaObjectPtr EnumItemX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr value, index, enumBaseObj;
  int indexInt, secN, ii;
  LavaDECL *enumDecl;
  TEnumDescription *enumDesc;
  CHEEnumSelId *cheItem;

  enumDecl = ((Reference*)enumType.ptr)->refDecl;
  value = AllocateObject(ckd,enumDecl,flags.Contains(isVariable));
  if (!value) {
    if (!ckd.exceptionThrown)
      SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
    return (LavaObjectPtr)-1;
  }
  enumBaseObj = CastEnumType(ckd, value);
  if (ckd.exceptionThrown) {
    DFC(value);
    return (LavaObjectPtr)-1;
  }
  ((SynFlags*)(enumBaseObj+1))->INCL(finished);
  index = ((Expression*)itemNo.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  secN = ckd.document->GetSectionNumber(ckd, index[0][0].classDECL, ckd.document->DECLTab[Integer]);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  index = CASTOBJECT(index,secN);
  indexInt = *(int*)(index+LSH);
  DFC(index);
  if (indexInt < 0) {
    SetRTError(ckd,&ERR_EnumOrdLow,stackFrame,"EnumItemX::Evaluate_2");
    return (LavaObjectPtr)-1;
  }
  *(int*)(enumBaseObj+LSH) = indexInt;
  enumDesc = (TEnumDescription*)enumDecl->EnumDesc.ptr;
  ii = 0;
  for (cheItem = (CHEEnumSelId*)enumDesc->EnumField.Items.first;
       cheItem && ii<indexInt;
       cheItem = (CHEEnumSelId*)cheItem->successor) ii++;
  if (ii < indexInt) {
    SetRTError(ckd,&ERR_EnumOrdHigh,stackFrame,"EnumItemX::Evaluate_3");
    return (LavaObjectPtr)-1;
  }
  NewQString((QString*)(enumBaseObj+LSH+1),cheItem->data.Id.c);

  return value;
}

LavaObjectPtr ExtendExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr obj, copyOfObj=0;
  SynObjectBase *mySelfVar=ckd.selfVar;
  CRuntimeException *ex;

  if (flags.Contains(ignoreSynObj)) return 0;

  obj = ((ObjReferenceX*)extendObj.ptr)->Evaluate(ckd,stackFrame);  
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  ex = CopyObject(ckd,&obj,&copyOfObj,false,extendTypeDecl);
  ckd.selfVar = mySelfVar;
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  if (ex) {
    ex->message = ex->message + CallStack(ckd,stackFrame);
    if (ex->SetLavaException(ckd)) 
      delete ex;
    return (LavaObjectPtr)-1;
  }
  if (obj)
    DRC(obj);
  if (copyOfObj)
    ((SynFlags*)(copyOfObj+1))->INCL(finished);
//  if (!ok)
//    return (LavaObjectPtr)-1;
  return copyOfObj;
}

LavaObjectPtr AttachObjectX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaObjectPtr urlObj, rtObj, urlStr;
  LavaDECL *cosDecl;
  int secn;

  cosDecl = ((Reference*)objType.ptr)->refDecl;
  urlObj = ((Expression*)url.ptr)->Evaluate(ckd,stackFrame);
  if (ckd.exceptionThrown)
    return false;
  rtObj = AttachLavaObject(ckd,urlObj,cosDecl,typeDECL,attachCat == stateObj);
  if (!rtObj && !ckd.exceptionThrown) {
    secn = ckd.document->GetSectionNumber(ckd, (*urlObj)->classDECL, ckd.document->DECLTab[VLString]);
    urlStr = (LavaObjectPtr)(urlObj - (*urlObj)->sectionOffset + (*urlObj)[secn].sectionOffset);
    SetRTError(ckd,&ERR_ldocNotOpened,stackFrame, *(QString*)(urlStr + LSH));
  }
  return rtObj;
}


LavaObjectPtr NewExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  LavaVariablePtr newStackFrame;
  unsigned frameSize, oldStackLevel = ckd.currentStackLevel;
  LavaObjectPtr object, urlObj;
  CVFuncDesc *fDesc;
  bool ok=true;
  LavaDECL *cosDecl;
  SynObjectBase *mySelfVar=ckd.selfVar;

  if (itf.ptr) {
    cosDecl = ((Reference*)objType.ptr)->refDecl;
    urlObj = ((Expression*)url.ptr)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return (LavaObjectPtr)-1;
    object = CreateObject(ckd, urlObj, cosDecl, typeDECL, attachCat == stateObj);
    ckd.selfVar = mySelfVar;
    if (!object) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
      return (LavaObjectPtr)-1;
    }
  }
  else {
    object = AllocateObject(ckd,typeDECL,((Reference*)objType.ptr)->flags.Contains(isVariable));
    if (!object) {
      if (!ckd.exceptionThrown)
        SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
      return (LavaObjectPtr)-1;
    }
  
    ckd.currentStackLevel++;
    stackFrame[((VarName*)varName.ptr)->stackPos] = object;

    if (initializerCall.ptr) {
      stackFrame[((ObjReference*)((FuncStatement*)initializerCall.ptr)->handle.ptr)->stackPos]
        = object;
      if (!((SynObject*)initializerCall.ptr)->Execute(ckd,stackFrame)) {
        ckd.currentStackLevel = oldStackLevel;
        DFC(object);
        return (LavaObjectPtr)-1;
      }
    }
    else { // call default initializer
      fDesc = object[0]->funcDesc;
      if (fDesc->funcExec) {
        frameSize = fDesc->stackFrameSize;
#ifdef WIN32
        __asm {
          sub esp, frameSize
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
        newStackFrame[0] = object;
        if (fDesc->isNative) {
          TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
          if (ckd.exceptionThrown) {
            ckd.currentStackLevel = oldStackLevel;
            DFC(object);
            return (LavaObjectPtr)-1;
          }
        }
        else {
          FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)
          if (ckd.exceptionThrown) {
            ckd.currentStackLevel = oldStackLevel;
            DFC(object);
            return (LavaObjectPtr)-1;
          }
        }

#ifdef WIN32
        __asm {
          add esp, frameSize
        }
#else
        delete [] newStackFrame;
#endif
      }
    }

    if (butStatement.ptr) {
      ok = ((SynObject*)butStatement.ptr)->Execute(ckd,stackFrame);
      if (!ok) {
        ckd.currentStackLevel = oldStackLevel;
        DFC(object);
        return (LavaObjectPtr)-1;
      }
    }
    if (execDECL) { // invoke invariant if it exists
      unsigned frameSize=((SelfVar*)execDECL->Exec.ptr)->stackFrameSize,
               last=(frameSize>>2)-1;
      LavaVariablePtr newStackFrame;
#ifdef WIN32
      __asm {
        sub esp, frameSize
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
      newStackFrame[last] = (LavaObjectPtr)stackFrame; \
      newStackFrame[last-1] = (LavaObjectPtr)this;\
      ok = ((SelfVar*)execDECL->Exec.ptr)->Execute(ckd,newStackFrame);
      ckd.selfVar = mySelfVar;
#ifdef WIN32
      __asm {
        add esp, frameSize
      }
#else
			delete [] newStackFrame;
#endif
      if (!ok) {
        if (!ckd.exceptionThrown)
          SetRTError(ckd,&ERR_AssertionViolation,stackFrame);
        DFC(object);
        ckd.currentStackLevel = oldStackLevel;
        return (LavaObjectPtr)-1;
      }
    }

    ((SynFlags*)(object+1))->INCL(finished);
  }

  ckd.currentStackLevel = oldStackLevel;
  return object;
}

LavaObjectPtr ParameterX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  return ((Expression*)parameter.ptr)->Evaluate(ckd,stackFrame);
}

LavaObjectPtr HandleOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  ObjReference *opd=(ObjReference*)operand.ptr;
  LavaObjectPtr handle=stackFrame[opd->stackPos + 1];

  IFC(handle);
  return handle;
}

bool FuncStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE* che;
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj=0;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=1, nInputs=0, frameSize;
  bool ok;
  Expression *objRef, *inActParm;
  Parameter *outActParm;
  SelfVar *staticExec;
  SynObjectBase *mySelfVar=ckd.selfVar;
  LavaDECL *funcSectClass;
  int secN;

  if (flags.Contains(ignoreSynObj)) return true;

  objRef = (ExpressionX*)handle.ptr;
  if (objRef) {
    callObj = objRef->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return false;
    if (!callObj) {
      objRef->SetRTError(ckd,&ERR_NullCallObject,stackFrame,"FuncStatementX::Execute");
      return false;
    }
    if (flags.Contains(staticCall)) {
      if (vBaseType) {
        funcSectClass = (*callObj)[funcSectionNumber + funcDecl->SectionInfo2].classDECL;
        secN = ((CLavaProgram*)ckd.document)->GetVStatCallSecN(ckd, callObj, callCtx, vBaseType,
                                                            vBaseSectionNumber, vBaseIsOuter);
        if (ckd.exceptionThrown)
          return false;
        callObj = callObj - (*callObj)->sectionOffset;
        callObj = callObj + (*callObj)[secN].sectionOffset;
        secN = ckd.document->GetSectionNumber(ckd, (*callObj)->classDECL, funcSectClass);
        if (ckd.exceptionThrown)
          return false;
       funcSect = &((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[secN];
      }
      else {
        callObj = callObj - (*callObj)->sectionOffset + (*callObj)[funcSectionNumber].sectionOffset;
        funcSect = &((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[funcDecl->SectionInfo2];
      }
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
      frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      __asm {
        sub esp, frameSize
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
      newStackFrame[0] = callObj + ((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[fDesc->delta].sectionOffset;
    }
    else {
      funcSect = &(*callObj)[funcSectionNumber + funcDecl->SectionInfo2];
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
      frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      __asm {
        sub esp, frameSize
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
      newStackFrame[0] = callObj - (*callObj)->sectionOffset
                       + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
    }
  }
  else {
    if (funcDecl->TypeFlags.Contains(isNative)) 
      frameSize = (funcDecl->nInput + funcDecl->nOutput + 1)*4;
    else {
      staticExec = (SelfVar*)funcDecl->RuntimeDECL->Exec.ptr;
      frameSize = staticExec->stackFrameSize;
    }
#ifdef WIN32
    __asm {
      sub esp, frameSize
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  }

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    nInputs++;
    newStackFrame[pos++] = 0;
  }
  pos = 1;
  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    inActParm = (Expression*)che->data;
    object = inActParm->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      ok = false;
      goto ret;
    }
    if (object) {
      if (!((SynFlags*)(object-(*object)->sectionOffset+1))->Contains(finished)
      && !inActParm->flags.Contains(unfinishedAllowed)) {
        // unfinished objects may be passed only to input parms of initializers
        inActParm->SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        newStackFrame[pos++] = object;
        goto ret;
      }
      if (inActParm->formVType->DeclType == VirtualType) {
        if (objRef)
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, newStackFrame[0][0][0].classDECL, inActParm->formVType, inActParm->vSectionNumber, inActParm->isOuter);
        else
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, objTypeDecl, inActParm->formVType, inActParm->vSectionNumber, inActParm->isOuter);
        if (ckd.exceptionThrown) {
          newStackFrame[pos] = 0;
          ok = false;
          goto ret;
        }
      }
      else
        newStackFrame[pos++] = CASTOBJECT(object, inActParm->sectionNumber);
    }
    else
      newStackFrame[pos++] = 0;
  }

  for (; pos < nParams;)
    newStackFrame[pos++] = 0;

  if (objRef)
    if (fDesc->isNative) {
      ckd.stateObj = stateObj;
      TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    }
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)
  else
    if (funcDecl->TypeFlags.Contains(isNative)) {
      ckd.stateObj = stateObj;
      TRY_FCALL(ckd,((TAdapterFunc)funcDecl->RuntimeDECL),newStackFrame,frameSize,ok)
    }
    else
      FCALL(ckd,staticExec->Execute,newStackFrame,frameSize,ok)
  if (!ok) {
    goto ret;
  }

  pos = nInputs+1;
  for (che = (CHE*)outputs.first; che; che = (CHE*)che->successor) {
    outActParm = (Parameter*)che->data;
    object = newStackFrame[pos++];
    if (((SynObject*)outActParm->parameter.ptr)->primaryToken == ObjPH_T) {
      if (object)
        DFC(object);
    }
    else {
      if (outActParm->formVType->DeclType == VirtualType) {
        object = ((CLavaProgram*)ckd.document)->CastVOutObj(ckd, object, callCtx, outActParm->formVType, outActParm->vSectionNumber, outActParm->isOuter);
        if (ckd.exceptionThrown) {
          ok = false;
          goto ret;
        }
      }
      ok = ((ObjReferenceX*)outActParm->parameter.ptr)->assign(0,
             object,
             outActParm->kindOfTarget,
             outActParm->sectionNumber,
             ckd,
             stackFrame);
    }
  }
ret:
  for (pos=1; pos <= nInputs; pos++)
    if (newStackFrame[pos])
      DFC(newStackFrame[pos]);  // release input parameters
  if (callObj)
    DFC(callObj); 
    // release self after input parameters since it might be needed for releasing
    // the inputs in case "set.Remove(#elem)"; see CheDecFunc in BAdapter.cpp
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
	delete [] newStackFrame;
#endif
  RETURN(ok)
}

LavaObjectPtr FuncExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE* che;
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj=0;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=1, frameSize, nInputs=0;
  bool ok;
  Expression *objRef, *actParm;
  SelfVar* staticExec;
  SynObjectBase *mySelfVar=ckd.selfVar;
  LavaDECL *funcSectClass;
  int secN;

  objRef = (ExpressionX*)handle.ptr;
  if (objRef) {
    callObj = objRef->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      return (LavaObjectPtr)-1;
    if (!callObj) {
      objRef->SetRTError(ckd,&ERR_NullCallObject,stackFrame);
      return 0;
    }
    if (flags.Contains(staticCall)) {
      if (vBaseType) {
        funcSectClass = (*callObj)[funcSectionNumber + funcDecl->SectionInfo2].classDECL;
        secN = ((CLavaProgram*)ckd.document)->GetVStatCallSecN(ckd, callObj, callCtx, vBaseType,
                                                            vBaseSectionNumber, vBaseIsOuter);
        if (ckd.exceptionThrown)
          return (LavaObjectPtr)-1;
        callObj = callObj - (*callObj)->sectionOffset;
        callObj = callObj + (*callObj)[secN].sectionOffset;
        secN = ckd.document->GetSectionNumber(ckd, (*callObj)->classDECL, funcSectClass);
        if (ckd.exceptionThrown)
          return (LavaObjectPtr)-1;
        funcSect = &((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[secN];
      }
      else {
        callObj = callObj - (*callObj)->sectionOffset + (*callObj)[funcSectionNumber].sectionOffset;
        funcSect = &((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[funcDecl->SectionInfo2];
      }
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
      frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      __asm {
        sub esp, frameSize
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
      newStackFrame[0] = callObj + ((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[fDesc->delta].sectionOffset;
    }
    else {
      funcSect = &(*callObj)[funcSectionNumber + funcDecl->SectionInfo2];
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
      frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      __asm {
        sub esp, frameSize
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
      newStackFrame[0] = callObj - (*callObj)->sectionOffset
                         + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
    }
  }
  else {
    if (funcDecl->TypeFlags.Contains(isNative)) 
      frameSize = (funcDecl->nInput + funcDecl->nOutput + 1)*4;
    else {
      staticExec = (SelfVar*)funcDecl->RuntimeDECL->Exec.ptr;
      frameSize = staticExec->stackFrameSize;
    }
#ifdef WIN32
    __asm {
      sub esp, frameSize
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  }

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    nInputs++;
    newStackFrame[pos++] = 0;
  }
  pos = 1;
  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    actParm = (Expression*)che->data;
    object = actParm->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown) {
      object = 0;
      goto ret;
    }
    if (object) {
      if (!((SynFlags*)(object-(*object)->sectionOffset+1))->Contains(finished)
      && !actParm->flags.Contains(unfinishedAllowed)) {
        // unfinished objects may be passed only to input parms of initializers
        actParm->SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        newStackFrame[pos++] = object;
        goto ret;
      }
      if (actParm->formVType->DeclType == VirtualType)
        if (objRef)
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, newStackFrame[0][0][0].classDECL, actParm->formVType, actParm->vSectionNumber, actParm->isOuter);
        else
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, objTypeDecl, actParm->formVType, actParm->vSectionNumber, actParm->isOuter);
      else
        newStackFrame[pos++] = CASTOBJECT(object, actParm->sectionNumber);
    }
    else
      newStackFrame[pos++] = 0;
  }
  newStackFrame[pos] = 0;


  if (objRef)
    if (fDesc->isNative) {
      ckd.stateObj = stateObj;
      TRY_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    }
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr)->Execute,newStackFrame,frameSize,ok)
  else
    if (funcDecl->TypeFlags.Contains(isNative)) {
      ckd.stateObj = stateObj;
      TRY_FCALL(ckd,((TAdapterFunc)funcDecl->RuntimeDECL),newStackFrame,frameSize,ok)
    }
    else
      FCALL(ckd,staticExec->Execute,newStackFrame,frameSize,ok)
  if (!ok) {
    object = (LavaObjectPtr)-1;
    goto ret;
  }

  object = newStackFrame[pos];
  if (formVType->DeclType == VirtualType) {
    object = ((CLavaProgram*)ckd.document)->CastVOutObj(ckd, object, callCtx, formVType, vSectionNumber, isOuter);
    if (ckd.exceptionThrown)
      object = 0;
  }
ret:
  for (pos=1; pos <= nInputs; pos++)
    if (newStackFrame[pos])
      DFC(newStackFrame[pos]);  // release input parameters
  if (callObj)
    DFC(callObj);  // release self
    // release self after input parameters since it might be needed for releasing
    // the inputs in case "set.Remove(#elem)"; see CheDecFunc in BAdapter.cpp
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
  delete [] newStackFrame;
#endif
  return object;
}

bool RunX::Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
  CHE *che;
  LavaVariablePtr newStackFrame;
  unsigned pos=1, nInputs=0, frameSize;
  LavaObjectPtr object;
  LavaDECL *myDECL = ckd.myDECL;
  SynObjectBase *mySelfVar=ckd.selfVar;

  if (flags.Contains(ignoreSynObj)) return true;

  if (!execDECL->WorkFlags.Contains(runTimeOK)) {
    ckd.myDECL = execDECL;
    ckd.inINCL = execDECL->inINCL;
    try {
      ((SelfVar*)execDECL->Exec.ptr)->Check(ckd);
      ckd.selfVar = mySelfVar;
    }
    catch(CUserException *) {
      ckd.selfVar = mySelfVar;
      return false;
    }
    ckd.myDECL = myDECL;
    ckd.inINCL = myDECL->inINCL;
    execDECL->WorkFlags.INCL(runTimeOK);
  }
  frameSize = ((SelfVar*)execDECL->Exec.ptr)->stackFrameSize;
#ifdef WIN32
  __asm {
    sub esp, frameSize
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
  newStackFrame[0] = 0;

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    object = ((Expression*)che->data)->Evaluate(ckd,stackFrame);
    if (ckd.exceptionThrown)
      goto ret;
    if (object)
      newStackFrame[pos++] = CASTOBJECT(object, ((Expression*)che->data)->sectionNumber);
    else
      newStackFrame[pos++] = 0;
  }

  ((SelfVar*)execDECL->Exec.ptr)->Execute(ckd,newStackFrame);
  ckd.selfVar = mySelfVar;

ret:
#ifdef WIN32
  __asm {
    add esp, frameSize
  }
#else
  delete [] newStackFrame;
#endif
    SUCCEED  // initiators are autonomous, ==> initiator calls can only succeed
}

bool CVFuncDescX::Execute(SynObjectBase* obj, CheckData& ckd, LavaVariablePtr stackFrame)
{
  RETURN(((SelfVar*)obj)->Execute(ckd, stackFrame))
}


LavaVariablePtr ObjReferenceX::GetMemberVarPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos)
{
  LavaDECL *fieldDECL;
  CHE *cheo, *DODs=(CHE*)refIDs.first, *oldCheo=DODs;
  CVAttrDesc* aDesc;
  CSectionDesc* attrSect;
  LavaVariablePtr objPtr=stackFrame+stackPos, memObjPtr = objPtr, newStackFrame;
  LavaObjectPtr memObj=*objPtr, parent; //casted to real value of virtual type
  SynObjectBase *mySelfVar=ckd.selfVar;
  TDOD *dod;
  int sectIndex;
  unsigned frameSize;
  bool parentUnfinished=false;

  if (!DODs->successor) {
    if (memObj && ((SynFlags*)(memObj-(*memObj)->sectionOffset+1))->Contains(zombified)) {
      SetRTError(ckd,&ERR_ZombieAccess,stackFrame);
      return 0;
    }
    return memObjPtr;
  }

  parent = 0;
  for (cheo = (CHE*)DODs->successor; cheo; cheo = (CHE*)cheo->successor) {
    dod = (TDOD*)cheo->data;
    fieldDECL = dod->fieldDecl;
    if (memObj) {
      if (((SynFlags*)(memObj-(*memObj)->sectionOffset+1))->Contains(zombified)) {
        ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_ZombieAccess,stackFrame);
        return 0;
      }
      if (parentUnfinished) {
        ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        return 0;
      }
      if (parent && !((SynFlags*)(parent+1))->Contains(finished))
        parentUnfinished = true;

      parent = memObj;
      sectIndex = dod->sectionNumber + fieldDECL->SectionInfo2;
      memObjPtr = (LavaVariablePtr)(memObj
                  - (*memObj)->sectionOffset
                  + (*memObj)[sectIndex].sectionOffset
                  + fieldDECL->SectionInfo1 + LSH);
      if (dod->isProperty) {
        attrSect = &(*memObj)[sectIndex];
        aDesc = &((CVAttrDesc*)attrSect->attrDesc)[fieldDECL->SectionInfo1];
        if (aDesc->getExec->Exec.ptr == ckd.selfVar
        || aDesc->setExec->Exec.ptr == ckd.selfVar/*|| !cheo->successor*/) {
          memObj = *memObjPtr;
          if (memObj && dod->vType->DeclType == VirtualType) {
            memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
            if (ckd.exceptionThrown)
              return 0;
          }
        }
        else {
          frameSize = ((SelfVar*)aDesc->getExec->Exec.ptr)->stackFrameSize;
#ifdef WIN32
          __asm {
            sub esp, frameSize
            mov newStackFrame, esp
          }
#else
					newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
          newStackFrame[0] = memObj - (*memObj)->sectionOffset
                         + (*(memObj - (*memObj)->sectionOffset))[aDesc->delta].sectionOffset;
          if (!newStackFrame[0]) {
#ifdef WIN32
            __asm {
              add esp, frameSize
            }
#else
						delete [] newStackFrame;
#endif
            ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
            return 0;
          }
          newStackFrame[1] = 0;
          ((SynObject*)aDesc->getExec->Exec.ptr)->Execute(ckd,newStackFrame);
          ckd.selfVar = mySelfVar;
          memObj = newStackFrame[1];
#ifdef WIN32
          __asm {
            add esp, frameSize
          }
#else
					delete [] newStackFrame;
#endif
          if (dod->vType->DeclType == VirtualType) {
            memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
            if (ckd.exceptionThrown)
              return 0;
          }
        }
      }
      else {
        memObj = *memObjPtr;
        if (memObj && dod->vType->DeclType == VirtualType) {
          memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
          if (ckd.exceptionThrown)
            return 0;
        }
      }
    }
    else {
      ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
      return 0;
    }
    oldCheo=cheo;
  }
  return memObjPtr;
}

LavaObjectPtr ObjReferenceX::GetMemberObjPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos)
{
  LavaVariablePtr var; 
  LavaObjectPtr obj;
  CHE *DODs=(CHE*)refIDs.first, *last;
  TDOD* lastdod;
  
  if (!(var = GetMemberVarPtr(ckd, stackFrame, stackPos)))
    return 0;

  if (!(obj = *var)) {
    return 0;
  }

  if (DODs->successor) {
    last=(CHE*)refIDs.last;
    lastdod = (TDOD*)last->data;
    if (obj && (lastdod->vType->DeclType == VirtualType)) {
      obj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, obj, lastdod->eType);
      if (ckd.exceptionThrown)
        return 0;
    }
  }
  return obj;
}

LavaObjectPtr ObjReferenceX::GetPropertyInfo(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, LavaDECL*& setExec)
{
  LavaVariablePtr objPtr=stackFrame+stackPos, memObjPtr, newStackFrame;
  LavaObjectPtr memObj=*objPtr, parent;
  LavaDECL *fieldDECL, *efType;
  CHE *DODs=(CHE*)refIDs.first, *cheo=(CHE*)DODs->successor, *oldCheo=DODs;
  CVAttrDesc *aDesc;
  CSectionDesc *attrSect;
  int sectIndex;
  unsigned frameSize;
  TDOD* dod=(TDOD*)cheo->data;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool parentUnfinished=false;

  efType = ((TDOD*)DODs->data)->eType;
  parent = 0;
  for (; cheo->successor; cheo = (CHE*)cheo->successor) {
    dod = (TDOD*)cheo->data;
    fieldDECL = dod->fieldDecl;
    if (memObj) {
      if (((SynFlags*)(memObj-(*memObj)->sectionOffset+1))->Contains(zombified)) {
        ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_ZombieAccess,stackFrame);
        return 0;
      }
      if (parentUnfinished) {
        ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_UnfinishedObject,stackFrame);
        return 0;
      }
      if (parent && !((SynFlags*)(parent+1))->Contains(finished))
        parentUnfinished = true;

      parent = memObj;
      sectIndex = dod->sectionNumber + fieldDECL->SectionInfo2;
      memObjPtr = (LavaVariablePtr)(memObj
                  - (*memObj)->sectionOffset
                  + (*memObj)[sectIndex].sectionOffset
                  + fieldDECL->SectionInfo1);
      if (dod->isProperty) {
        attrSect = &(*memObj)[sectIndex];
        aDesc = &((CVAttrDesc*)attrSect->attrDesc)[fieldDECL->SectionInfo1];
        if (aDesc->getExec->Exec.ptr == ckd.selfVar) {
          memObj = *memObjPtr;
          if (memObj && dod->vType->DeclType == VirtualType) {
            memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
            if (ckd.exceptionThrown)
              return 0;
          }
        }
        else {
          frameSize = ((SelfVar*)aDesc->getExec->Exec.ptr)->stackFrameSize;
#ifdef WIN32
          __asm {
            sub esp, frameSize
            mov newStackFrame, esp
          }
#else
					newStackFrame = new LavaObjectPtr[frameSize>>2];
#endif
          newStackFrame[0] = memObj - (*memObj)->sectionOffset
                         + (*(memObj - (*memObj)->sectionOffset))[aDesc->delta].sectionOffset;
          if (!newStackFrame[0]) {
#ifdef WIN32
            __asm {
              add esp, frameSize
            }
#else
						delete [] newStackFrame;
#endif
            ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
            return 0;
          }
          newStackFrame[1] = 0;
          ((SynObject*)aDesc->getExec->Exec.ptr)->Execute(ckd,newStackFrame);
          ckd.selfVar = mySelfVar;
          if (ckd.exceptionThrown) {
#ifdef WIN32
            __asm {
              add esp, frameSize
            }
#else
						delete [] newStackFrame;
#endif
            return 0;
          }
          memObj = newStackFrame[1];
#ifdef WIN32
          __asm {
            add esp, frameSize
          }
#else
					delete [] newStackFrame;
#endif
          if (dod->vType->DeclType == VirtualType) {
            memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
            if (ckd.exceptionThrown)
              return 0;
          }
        }
      }
      else {  
        memObj = *memObjPtr;
        if (memObj && dod->vType->DeclType == VirtualType) {
          memObj = ((CLavaProgram*)ckd.document)->CastVObj(ckd, memObj, dod->eType);
          if (ckd.exceptionThrown)
            return 0;
        }
      }
      efType = dod->eType;
    }
    else {
      ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
      return 0;
    }
  }
  fieldDECL = dod->fieldDecl;
  sectIndex = dod->sectionNumber + fieldDECL->SectionInfo2; 
  attrSect = &(*memObj)[sectIndex];
  aDesc = &((CVAttrDesc*)attrSect->attrDesc)[fieldDECL->SectionInfo1];
  setExec = aDesc->setExec; //return call-object casted to setExec-type
  memObj = memObj - (*memObj)->sectionOffset 
                       + (*(memObj - (*memObj)->sectionOffset))[aDesc->delta].sectionOffset;
  return memObj;
}

