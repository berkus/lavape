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
#include "cmainframe.h"
#include "Check.h"
#include "DbgThread.h"
#include "BAdapter.h"
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "LavaThread.h"
#include "LavaBaseStringInit.h"
#include "LavaProgram.h"
#include "SafeInt.h"
#include "qmap.h"
#include "qstring.h"
#include <float.h>
#include <stdlib.h>
#include <errno.h>
#ifndef WIN32
#include <setjmp.h>
#endif
 
#define IsPH(PTR) ((SynObject*)PTR)->IsPlaceHolder()

#define SUCCEED {if (ckd.exceptionThrown || ckd.immediateReturn) return false; else return true;}

#define RETURN(OK) {if (!(OK) || ckd.exceptionThrown) return false; else return true;}

#ifdef WIN32
#define FCALL(CKD, CALLEDSELF, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; \
  unsigned newOldExprLevel=FRAMESIZE-1, fSizeBytes = FRAMESIZE<<2;\
  DebugStep oldDebugStep=nextDebugStep; \
  if (LBaseData->debugOn && (nextDebugStep == nextStm || nextDebugStep == nextFunc)) \
    nextDebugStep = noStep; \
  if (primaryToken == ObjRef_T) \
    NEWSTACK[0] = (LavaObjectPtr)parentObject; \
  else if (IsFuncInvocation()) \
    NEWSTACK[0] = (LavaObjectPtr)((FuncExpression*)this)->function.ptr; \
  else \
    NEWSTACK[0] = (LavaObjectPtr)this; \
  NEWSTACK[1] = (LavaObjectPtr)stackFrame;\
  NEWSTACK[2] = (LavaObjectPtr)((unsigned)stackFrame[2] & ~2); \
  try { RESULT = CALLEDSELF->Execute(CKD, NEWSTACK, newOldExprLevel); \
    ckd.selfVar = mySelfVar;}\
  catch (CHWException ex) {\
    ckd.selfVar = mySelfVar;\
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (CRuntimeException ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (caught) {\
  __asm {\
    sub esp, fSizeBytes}\
  }\
  if (LBaseData->debugOn && !caught) {\
    if (nextDebugStep == noStep) {\
      stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | ((unsigned)NEWSTACK[2] & 2)); \
      if (!((unsigned)NEWSTACK[2] & 2) && (oldDebugStep == nextStm || oldDebugStep == nextFunc)) \
        nextDebugStep = oldDebugStep; }\
    else if (nextDebugStep == stepOut) {\
      if (currentStackDepth < stepOutStackDepth) { \
        stepOutStackDepth = 0; \
        if (IsFuncInvocation()) \
          CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
        else \
          CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
    else { \
      if (IsFuncInvocation()) \
        CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
      else \
        CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
}
#else
#define FCALL(CKD, CALLEDSELF, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; \
  unsigned newOldExprLevel=FRAMESIZE-1;\
  DebugStep oldDebugStep=nextDebugStep; \
  if (LBaseData->debugOn && (nextDebugStep == nextStm || nextDebugStep == nextFunc)) \
    nextDebugStep = noStep; \
  if (primaryToken == ObjRef_T) \
    NEWSTACK[0] = (LavaObjectPtr)parentObject; \
  else if (IsFuncInvocation()) \
    NEWSTACK[0] = (LavaObjectPtr)((FuncExpression*)this)->function.ptr; \
  else \
    NEWSTACK[0] = (LavaObjectPtr)this; \
  NEWSTACK[1] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[2] = (LavaObjectPtr)((unsigned)stackFrame[2] & ~2); \
  try {\
    if (!setjmp(contOnHWexception)) {\
      RESULT = CALLEDSELF->Execute(CKD, NEWSTACK, newOldExprLevel); ckd.selfVar = mySelfVar;}\
    else {\
      throw hwException;}\
  }\
  catch (CHWException ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (CRuntimeException ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (LBaseData->debugOn && !caught) {\
    if (nextDebugStep == noStep) {\
      stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | ((unsigned)NEWSTACK[2] & 2)); \
      if (!((unsigned)NEWSTACK[2] & 2) && (oldDebugStep == nextStm || oldDebugStep == nextFunc)) \
        nextDebugStep = oldDebugStep; }\
    else if (nextDebugStep == stepOut) {\
      if (currentStackDepth < stepOutStackDepth) { \
        stepOutStackDepth = 0; \
        if (IsFuncInvocation()) \
          CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
        else \
          CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
    else { \
      if (IsFuncInvocation()) \
        CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
      else \
        CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
}
#endif


#ifdef WIN32
#define NATIVE_FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; \
  QString myExMessage; \
  unsigned fSizeBytes = FRAMESIZE<<2;\
  DebugStep oldDebugStep=nextDebugStep; \
  if (LBaseData->debugOn && (nextDebugStep == nextStm || nextDebugStep == nextFunc)) \
    nextDebugStep = noStep; \
  if (primaryToken == ObjRef_T) \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)parentObject; \
  else if (IsFuncInvocation()) \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)((FuncExpression*)this)->function.ptr; \
  else \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)this; \
  NEWSTACK[1] = CKD.stackFrame[1] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[2] = CKD.stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] & ~2); \
  try { RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
  catch (CHWException ex) {\
    ckd.selfVar = mySelfVar;\
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw; \
  }\
  catch (SafeIntException ex) {\
    CHWException exHW(ex.m_code == ERROR_ARITHMETIC_OVERFLOW?STATUS_INTEGER_OVERFLOW:STATUS_INTEGER_DIVIDE_BY_ZERO); \
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,exHW.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,exHW.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (exHW.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (CRuntimeException ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (caught) {\
  __asm {\
    sub esp, fSizeBytes}\
  }\
  if (LBaseData->debugOn && !caught) {\
    if (nextDebugStep == noStep) {\
      stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | ((unsigned)NEWSTACK[2] & 2)); \
      if (!((unsigned)NEWSTACK[2] & 2) && (oldDebugStep == nextStm || oldDebugStep == nextFunc)) \
        nextDebugStep = oldDebugStep; }\
    else if (nextDebugStep == stepOut) {\
      if (currentStackDepth < stepOutStackDepth) { \
        stepOutStackDepth = 0; \
        if (IsFuncInvocation()) \
          CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
        else \
          CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
    else { \
      if (IsFuncInvocation()) \
        ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
      else \
        DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
}
#else
#define NATIVE_FCALL(CKD, FUNC, NEWSTACK, FRAMESIZE, RESULT)  {\
  bool caught=false; \
  DebugStep oldDebugStep=nextDebugStep; \
  if (LBaseData->debugOn && (nextDebugStep == nextStm || nextDebugStep == nextFunc)) \
    nextDebugStep = noStep; \
  if (primaryToken == ObjRef_T) \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)parentObject; \
  else if (IsFuncInvocation()) \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)((FuncExpression*)this)->function.ptr; \
  else \
    NEWSTACK[0] = CKD.stackFrame[0] = (LavaObjectPtr)this; \
  NEWSTACK[1] = CKD.stackFrame[1] = (LavaObjectPtr)stackFrame; \
  NEWSTACK[2] = CKD.stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] & ~2); \
  try { \
    if (!setjmp(contOnHWexception)) {\
      RESULT = FUNC(CKD, NEWSTACK); ckd.selfVar = mySelfVar;}\
    else \
      throw hwException;\
  }\
  catch (CHWException ex) {\
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (SafeIntException ex) {\
    siginfo_t si;\
    si.si_code = ex.m_code == ERROR_ARITHMETIC_OVERFLOW?FPE_INTOVF:FPE_INTDIV;\
    CHWException exHW(SIGFPE,&si); \
    ckd.selfVar = mySelfVar; \
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,exHW.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,exHW.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (exHW.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  catch (CRuntimeException ex) {\
    ckd.selfVar = mySelfVar;\
    RESULT = false;\
    if (IsFuncInvocation()) \
      CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else \
      CKD.callStack = DebugStop(CKD,stackFrame,ex.message,Stop_Exception,newStackFrame,((Operation*)this)->funcDecl); \
    if (ex.SetLavaException(CKD)) caught = true;\
    else throw;\
  }\
  if (LBaseData->debugOn && !caught) { \
    if (nextDebugStep == noStep) {\
      stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | ((unsigned)NEWSTACK[2] & 2)); \
      if (!((unsigned)NEWSTACK[2] & 2) && (oldDebugStep == nextStm || oldDebugStep == nextFunc)) \
        nextDebugStep = oldDebugStep; }\
    else if (nextDebugStep == stepOut) {\
      if (currentStackDepth < stepOutStackDepth) { \
        stepOutStackDepth = 0; \
        if (IsFuncInvocation()) \
          CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
        else \
          CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
    else { \
      if (IsFuncInvocation()) \
        CKD.callStack = ((Reference*)((FuncStatement*)this)->function.ptr)->DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((FuncStatement*)this)->funcDecl); \
      else \
        CKD.callStack = DebugStop(CKD,stackFrame,QString::null,Stop_StepOut,newStackFrame,((Operation*)this)->funcDecl); }} \
}
#endif

#define STOP_AT_STM(CKD, STACK, RC) { \
  if (flags.Contains(ignoreSynObj)) return RC; \
  if (LBaseData->debugOn) \
    if (type == Stm_T) \
      if ((nextDebugStep == nextStm || nextDebugStep == stepInto) \
      && (primaryToken != assignFS_T \
          || parentObject->primaryToken != new_T \
          || !((FuncStatement*)this)->funcDecl->TypeFlags.Contains(defaultInitializer))) \
        DebugStop(CKD,STACK,QString::null,Stop_NextStm); \
      else if (workFlags.Contains(isBrkPnt) || workFlags.Contains(isTempPoint)) { \
        DebugStop(CKD,STACK,QString::null,Stop_BreakPoint); }}

#define STOP_AT_BPT(CKD, STACK) { \
  if (LBaseData->debugOn) \
    if (workFlags.Contains(isBrkPnt) || workFlags.Contains(isTempPoint)) \
      DebugStop(CKD,STACK,QString::null,Stop_BreakPoint); } \

#define STOP_AT_OP(CKD, STACK) { \
  if (LBaseData->debugOn) \
    if (nextDebugStep == nextFunc) \
      DebugStop(CKD,STACK,QString::null,Stop_NextOp,newStackFrame,((Operation*)this)->funcDecl); \
    else if (workFlags.Contains(isBrkPnt) || workFlags.Contains(isTempPoint)) \
      DebugStop(CKD,STACK,QString::null,Stop_NextOp,newStackFrame,((Operation*)this)->funcDecl);}

#define STOP_AT_FUNC(CKD, STACK,SYNOBJ) { \
  if (LBaseData->debugOn) \
    if ((nextDebugStep == nextFunc) \
      && (primaryToken != assignFS_T \
          || parentObject->primaryToken != new_T \
          || !((FuncExpression*)this)->funcDecl->TypeFlags.Contains(defaultInitializer))) \
      SYNOBJ->DebugStop(CKD,STACK,QString::null,Stop_NextFunc,newStackFrame,((FuncStatement*)this)->funcDecl); \
    else if (SYNOBJ->workFlags.Contains(isBrkPnt) || SYNOBJ->workFlags.Contains(isTempPoint)) \
      SYNOBJ->DebugStop(CKD,STACK,QString::null,Stop_NextFunc,newStackFrame,((FuncStatement*)this)->funcDecl); }


QString SynObject::DebugStop(CheckData &ckd,LavaVariablePtr stopStack,QString excMsg=QString::null,StopReason stopReason=Stop_Exception,LavaVariablePtr calleeStack=0,LavaDECL *calleeFunc=0) {
  SynObject *synObj=this, *oldSynObj;
  CHE *chp;
  LavaVariablePtr stack=stopStack;
  QString cFileName, cSynObjName, msg, path, pmMsg;
  unsigned inINCL;
  bool debug=true, isEx = excMsg.length() != 0;
  CHEStackData* stData;
  int rc;
  DebugStep ds=nextDebugStep;

  debug = ckd.document->debugOn || LBaseData->m_pmDumps && !((CLavaDebugThread*)LBaseData->debugThread)->running();
  if (debug && !ckd.document->debugOn) 
    ((CLavaDebugThread*)LBaseData->debugThread)->initData(ckd.document);
  if (debug) {
    stopStack[2] = (LavaObjectPtr)((unsigned)stopStack[2] | 2);
      // set flag: breakpoint encountered
    if (primaryToken == assignFS_T
    && parentObject->primaryToken == new_T
    && ((FuncStatement*)this)->funcDecl->TypeFlags.Contains(defaultInitializer))
      synObj = parentObject;
    else if (primaryToken == FuncRef_T
    && parentObject->parentObject->primaryToken == new_T
    && ((FuncStatement*)parentObject)->funcDecl->TypeFlags.Contains(defaultInitializer))
      synObj = parentObject->parentObject;
    else if (primaryToken == ObjRef_T
    && parentObject->primaryToken == assign_T)
      synObj = parentObject;
    else if (synObj->primaryToken == parameter_T)
      synObj = (SynObject*)((Parameter*)this)->parameter.ptr;

    if (LBaseData->tempBrkPoint) {
      ((SynObject*)LBaseData->tempBrkPoint)->workFlags.EXCL(isTempPoint);
      LBaseData->tempBrkPoint = 0;
    }

    stData = new CHEStackData;
    stData->data.SynObjID = synObj->synObjectID;
    stData->data.Stack = (CSecTabBase***)stopStack;
    stData->data.SynObj = synObj;
    ((CLavaDebugThread*)LBaseData->debugThread)->dbgStopData->stopReason = stopReason;
    ((CLavaDebugThread*)LBaseData->debugThread)->dbgStopData->CalleeStack = (CSecTabBase***)calleeStack;
    ((CLavaDebugThread*)LBaseData->debugThread)->dbgStopData->CalleeFunc = calleeFunc;
    ((CLavaDebugThread*)LBaseData->debugThread)->dbgStopData->StackChain.Append(stData);
  }

  if (isEx)
    msg = QString("Call stack:\n");

  do {
    if (synObj->primaryToken == VarName_T)
      cSynObjName = QString(((VarName*)synObj)->varName.c);
    else if (synObj->primaryToken == TDOD_T) {
      if (isEx)
        cSynObjName = QString(((TDOD*)synObj)->name.c);
      chp = (CHE*)((CHE*)synObj->whereInParent)->predecessor;
      while (chp) {
        synObj = (SynObject*)chp->data;
        if (isEx)
          cSynObjName = QString(((TDOD*)synObj)->name.c) + "." + cSynObjName;
        chp = (CHE*)chp->predecessor;
      }
      synObj = synObj->parentObject;
    }
    else if (synObj->IsFuncInvocation()) {
      if (isEx)
        path = "| " + QString(((Reference*)((FuncExpression*)synObj)->function.ptr)->refName.c)
             + "() " + path;
      synObj = synObj->parentObject;
      continue;
    }
    else if (synObj->type == implementation_T) { // exec SelfVar
      oldSynObj = synObj;
      inINCL = ((SelfVar*)synObj)->inINCL;
      if (isEx)
        cFileName = QString(ckd.document->IDTable.IDTab[inINCL]->FileName.c);

      synObj = (SynObject*)(stack[0]);
      if (synObj) {
        if (synObj->primaryToken == assignFS_T
        && synObj->parentObject->primaryToken == new_T
        && ((FuncStatement*)synObj)->funcDecl->TypeFlags.Contains(defaultInitializer))
          synObj = parentObject;
        else if (synObj->primaryToken == FuncRef_T
        && synObj->parentObject->parentObject->primaryToken == new_T
        && ((FuncStatement*)synObj->parentObject)->funcDecl->TypeFlags.Contains(defaultInitializer))
          synObj = synObj->parentObject->parentObject;
      }

      if (debug) {
        stData->data.FuncID = TID(((SelfVar*)oldSynObj)->execDECL->ParentDECL->OwnID,
                                  ((SelfVar*)oldSynObj)->execDECL->ParentDECL->inINCL);
        stData->data.ExecType = ((SelfVar*)oldSynObj)->execDECL->DeclType;
        if (synObj) {
          stData = new CHEStackData;
          stData->data.SynObjID = synObj->synObjectID;
          stData->data.Stack = (CSecTabBase ***)stack[1];
          stData->data.SynObj = synObj;
          ((CLavaDebugThread*)LBaseData->debugThread)->dbgStopData->StackChain.Append(stData);
        }
      }
      stack = (LavaVariablePtr)stack[1];
      if (isEx) {
        switch (oldSynObj->primaryToken) {
        case invariant_T:
          path = QString("\n| invariant of ")
                 + ((SelfVar*)oldSynObj)->execDECL->FullName.c
                 + ", file " + cFileName + "\n" + path;
          break;
        case function_T:
        case initializer_T:
        case dftInitializer_T:
          path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                 + "(), file " + cFileName + "\n" + path;
          break;
        case require_T:
          if (((SelfVar*)oldSynObj)->execDECL->ParentDECL->ParentDECL->DeclType == Impl)
            path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                   + "()::impl_require, file " + cFileName + "\n" + path;
          else
            path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                   + "()::require, file " + cFileName + "\n" + path;
          break;
        case ensure_T:
          if (((SelfVar*)oldSynObj)->execDECL->ParentDECL->ParentDECL->DeclType == Impl)
            path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                   + "()::impl_ensure, file " + cFileName + "\n" + path;
          else
            path = "\n| " + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                   + "()::ensure, file " + cFileName + "\n" + path;
          break;
        default: // initiator
          path = "\ninitiator "
                 + QString(((SelfVar*)oldSynObj)->execDECL->FullName.c)
                 + ", file "
                 + cFileName + "\n" + path;
        }
      }
      if (stack) {
        synObj = synObj->parentObject;
        continue;
      }
      else
        break;
    }
    else {
      if (isEx) {
        if (synObj->primaryToken == ObjRef_T)
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
        else if (synObj->primaryToken == arrayAtIndex_T)
          cSynObjName = QString(((ObjReference*)((ArrayAtIndex*)synObj)->arrayObj.ptr)->refName.c)
                        + "[]";
        else
          cSynObjName = QString(TOKENSTR[synObj->primaryToken]);
      }
    }

    if (stack) {
      if (isEx) {
        if (synObj->parentObject->primaryToken == parameter_T
        || synObj->parentObject->IsUnaryOp()
        || ((synObj->parentObject->IsMultOp()
            || synObj->parentObject->IsBinaryOp())
           && synObj->parentObject->type != Stm_T))
          cSynObjName = QString("(") + cSynObjName + ")";
        path = "| " + cSynObjName + " " + path;
      }
      if (synObj->parentObject->primaryToken == parameter_T)
        synObj = synObj->parentObject;
      synObj = synObj->parentObject;
    }
    else
      if (isEx)
        path = cSynObjName + path;
  } while (stack);
  if (isEx)
    msg = msg + path;
  if (debug) {
    if (isEx) 
      if (debug && !ckd.document->debugOn) {
        pmMsg = excMsg + "\n\n" + msg + "\n\nDebug this exception?\n\nClick \"No to all\" to disable debugging perpetually";
        rc = information(qApp->mainWidget(),qApp->name(),QApplication::tr(pmMsg),QMessageBox::Yes|QMessageBox::Default,QMessageBox::No,QMessageBox::NoAll);
    
      }
      else {
        rc = QMessageBox::Yes;
        pmMsg = excMsg + "\n\n" + msg;
        information(qApp->mainWidget(),qApp->name(),QApplication::tr(pmMsg),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton,QMessageBox::NoButton);
      }
    else
      rc = QMessageBox::Yes;
    if (rc == QMessageBox::Yes) {
      if (debug && !ckd.document->debugOn) {
        ((CLavaDebugThread*)LBaseData->debugThread)->wait();
        if (((CLavaDebugThread*)LBaseData->debugThread)->pContExecEvent->available())
          (*((CLavaDebugThread*)LBaseData->debugThread)->pContExecEvent)++;
        ((CLavaDebugThread*)LBaseData->debugThread)->start();
      }
      else
        (*((CLavaDebugThread*)LBaseData->debugThread)->pContDebugEvent)--; //debug thread continue
      (*((CLavaDebugThread*)LBaseData->debugThread)->pContExecEvent)++;   //execution thread wait
    }
    else
      if (rc == QMessageBox::NoAll) {
        LBaseData->m_pmDumps = false;
        LBaseData->m_strPmDumps = "false";
        ((CMainFrame*)qApp->mainWidget())->pmDumpAction->setOn(false);
      }
  }
  return msg;
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
  else if(errorCode == &ERR_InvariantViolation)
    code = InvariantViolation_ex;
  else if(errorCode == &ERR_PreconditionViolation)
    code = PreconditionViolation_ex;
  else if(errorCode == &ERR_PreconditionConsistency)
    code = PreconditionConsistency_ex;
  else if(errorCode == &ERR_PostconditionViolation)
    code = PostconditionViolation_ex;
  else if(errorCode == &ERR_PostconditionConsistency)
    code = PostconditionConsistency_ex;
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

  if (textParam)
    msgText = QString(textParam) + ": " + msgText;
  else if (errorCode == &ERR_AssertionViolation
  && comment.ptr)
    msgText = msgText + ": " + comment.ptr->str.c;

  ckd.callStack = DebugStop(ckd,stackFrame,msgText);

  if (!SetLavaException(ckd, code, msgText)) {
    CRuntimeException* ex = new CRuntimeException(code, &msgText);
    throw ex;
  }

  ckd.exceptionThrown = true;
}


bool AssertionData::EvalOldExpressions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, unsigned &oldExprLevel) {
  OldExpression *oep;
  AssertionData *adp;
  LavaObjectPtr obj=0, oldSelfObject;
  LavaDECL *objClassDECL;
  int secn;
  CRuntimeException *ex;
  unsigned iOldExpr=0;
  bool ok;

  oldSelfObject = stackFrame[SFH];
  for (oep = oldExpressions.first();
       oep;
       oep = oldExpressions.next()) {
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != funcDECL->ParentDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, funcDECL->ParentDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    obj = ((SynObject*)oep->paramExpr.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
    stackFrame[oldExprLevel] = 0;
    if (obj) {
      ex = CopyObject(ckd,&obj,&stackFrame[oldExprLevel],((SynFlags*)(obj+1))->Contains(stateObjFlag));
      ok = !ex && !ckd.exceptionThrown;
      if (ex) {
        ex->message = ex->message + ((SelfVar*)ckd.selfVar)->DebugStop(ckd,stackFrame,ex->message);
        if (ex->SetLavaException(ckd)) 
          delete ex;
      }
      DFC(obj);
      if (!ok)
        return false;
      ((SynFlags*)(stackFrame[oldExprLevel]+1))->INCL(finished);
    }
    oldExprLevel--;
    oep->iOldExpr = iOldExpr++;
  }

  for (adp=overridden.first();
       adp;
       adp=overridden.next()) {
    if (rtadDict.find(adp))
      continue;
    ok = adp->EvalOldExpressions(ckd,rtadDict,stackFrame,oldExprLevel);
    if (!ok)
      return false;
  }

  return true;
}


bool AssertionData::EvalPreConditions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, AssertionData *parent, bool parentOK) {
  AssertionData *adp;
  RTAssertionData *rtad;
  LavaObjectPtr oldSelfObject=stackFrame[SFH], oldSF2=stackFrame[2];
  LavaDECL *objClassDECL;
  int secn;
  bool ok=true, thisOk;
  unsigned oldExprLevel=0;

  stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | 1);
  oldSelfObject ;
  if (requireDECL) {
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != funcDECL->ParentDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, funcDECL->ParentDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    ok = ((SynObject*)requireDECL->Exec.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
  }
  if (!parent && !ok) {
    ((SynObject*)requireDECL->Exec.ptr)->SetRTError(ckd,&ERR_PreconditionViolation,stackFrame);
    return false;
  }
  if (ok && parent && !parentOK) { // ok means: precondition true OR ABSENT
    ((SynObject*)parent->requireDECL->Exec.ptr)->SetRTError(ckd,&ERR_PreconditionConsistency,stackFrame);
    return false;
  }

  thisOk = ok;

  if (!parent && requireDECLimpl) {
    oldSelfObject = stackFrame[SFH];
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != funcDECL->ParentDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, funcDECL->ParentDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    ok = ((SynObject*)requireDECLimpl->Exec.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
    if (!ok) {
      ((SynObject*)requireDECLimpl->Exec.ptr)->SetRTError(ckd,&ERR_PreconditionViolation,stackFrame);
      return false;
    }
  }

  if (funcDECL->TypeFlags.Contains(isInitializer))
    return ok;

  rtad = rtadDict[this];
  rtad->requireChecked = true;
  rtad->requireOK = thisOk;

  for (adp=overridden.first();
       adp;
       adp=overridden.next()) {
    rtad = rtadDict[adp];
    if (rtad) {
      if (rtad->requireChecked) {
        if (rtad->requireOK && !thisOk) { // !thisOk implies requireDECL != 0
           ((SynObject*)requireDECL->Exec.ptr)->SetRTError(ckd,&ERR_PreconditionConsistency,stackFrame);
          return false;
        }
        else
          continue;
      }
    }
    else 
      rtadDict.insert(adp,new RTAssertionData(oldExprLevel));
    ok = adp->EvalPreConditions(ckd,rtadDict,stackFrame,this,thisOk);
    if (ckd.exceptionThrown)
      return false;
  }
  
  return true;
}

bool AssertionData::EvalPostConditions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, AssertionData *parent) {
  AssertionData *adp;
  RTAssertionData *rtad;
  LavaObjectPtr oldSelfObject=stackFrame[SFH], oldSF2=stackFrame[2];
  LavaDECL *objClassDECL;
  int secn;
  bool ok=true;
  unsigned oldExprLevel=0;

  oldExprLevel = rtadDict[this]->oldExprLevel;
  if (ensureDECL) {
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != funcDECL->ParentDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, funcDECL->ParentDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | 1);
    ok = ((SynObject*)ensureDECL->Exec.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
  }
  if (!parent && !ok) { // !ok implies ensureDECL != 0
    ((SynObject*)ensureDECL->Exec.ptr)->SetRTError(ckd,&ERR_PostconditionViolation,stackFrame);
    return false;
  }

  if (!parent && ensureDECLimpl) {
    oldSelfObject = stackFrame[SFH];
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != funcDECL->ParentDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, funcDECL->ParentDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | 1);
    ok = ((SynObject*)ensureDECLimpl->Exec.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
    if (!ok) {
      ((SynObject*)ensureDECLimpl->Exec.ptr)->SetRTError(ckd,&ERR_PostconditionViolation,stackFrame);
      return false;
    }
  }

  if (funcDECL->TypeFlags.Contains(isInitializer))
    return ok;

  if (!ok && parent) { // !ok implies ensureDECL != 0
    rtad = rtadDict.find(this);
    if (rtad->requireOK) {
      ((SynObject*)ensureDECL->Exec.ptr)->SetRTError(ckd,&ERR_PostconditionConsistency,stackFrame);
      return false;
    }
  }

  if (!ok)
    return true;
    // we may stop the recursion here since a lower level postcondition
    // inconsistency could be encountered in this case only if also a
    // previous precondition inconsistency had occurred (which would already
    // have triggered a corresponding exception, however!)

  for (adp=overridden.first();
       adp;
       adp=overridden.next()) {
    rtad = rtadDict.find(adp);
    if (rtad->ensureChecked)
      if (!rtad->ensureOK && rtad->requireOK) {
        rtad = rtadDict.find(adp);
        if (rtad->requireOK) {
          ((SynObject*)adp->ensureDECL->Exec.ptr)->SetRTError(ckd,&ERR_PostconditionConsistency,stackFrame);
          return false;
        }
      }
      else
        continue;
    ok = adp->EvalPostConditions(ckd,rtadDict,stackFrame,this);
    if (ckd.exceptionThrown)
      return false;
    rtadDict.find(this)->ensureChecked = true;
  }
  
  return true;
}

bool InvarData::EvalInvariants (CheckData &ckd, RTInvDataDict &rtidDict, LavaVariablePtr stackFrame) {
  InvarData *idp;
  LavaObjectPtr oldSelfObject=stackFrame[SFH], oldSF2=stackFrame[2];
  int secn;
  LavaDECL *rtid, *objClassDECL;
  bool ok=true, thisOk;

  if (invariantDECL) {
    oldSelfObject = stackFrame[SFH];
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != itfDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, itfDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | 1);
    ok = ((SynObject*)invariantDECL->Exec.ptr)->Execute(ckd,stackFrame,0);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
    if (!ok) {
      ((SynObject*)invariantDECL->Exec.ptr)->SetRTError(ckd,&ERR_InvariantViolation,stackFrame);
      return false;
    }
  }

  if (invariantDECLimpl) {
    oldSelfObject = stackFrame[SFH];
    objClassDECL = oldSelfObject[0][0].classDECL;
    if (objClassDECL != itfDECL) {
      secn = ckd.document->GetSectionNumber(ckd, objClassDECL, itfDECL);
      stackFrame[SFH] = (LavaObjectPtr)(oldSelfObject + (*oldSelfObject)[secn].sectionOffset);
    }
    stackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] | 1);
    ok = ((SynObject*)invariantDECLimpl->Exec.ptr)->Execute(ckd,stackFrame,0);
    stackFrame[2] = oldSF2;
    stackFrame[SFH] = oldSelfObject;
    if (ckd.exceptionThrown)
      return false;
    if (!ok) {
      ((SynObject*)invariantDECLimpl->Exec.ptr)->SetRTError(ckd,&ERR_InvariantViolation,stackFrame);
      return false;
    }
  }

  thisOk = ok;
  for (idp=overridden.first();
       idp;
       idp=overridden.next()) {
    rtid = rtidDict.find(idp);
    if (rtid)
      continue;
    ok = idp->EvalInvariants(ckd,rtidDict,stackFrame);
    rtidDict.insert(this,itfDECL);
    if (!ok)
      return false;
  }
  
  return true;
}

bool SelfVarX::ExecBaseInits (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  int secN;
  bool ok;
  LavaObjectPtr callObj;
  LavaVariablePtr newStackFrame;
  CHE *baseInit;
  BaseInit* baseInitDat;
  CVFuncDesc *fDesc;
  unsigned frameSize, frameSizeBytes;
  SynObjectBase *mySelfVar=this;

  for (baseInit = (CHE*)baseInitCalls.first;
       baseInit;
       baseInit = (CHE*)baseInit->successor) {
    baseInitDat = (BaseInit*)baseInit->data;
    if (baseInitDat->initializerCall.ptr) {
      ok = ((SynObject*)baseInitDat->initializerCall.ptr)->Execute(ckd,stackFrame,oldExprLevel);
      if (ckd.exceptionThrown)
        return false;
    }
    else { // virtual type case: call dft initializer
      secN = ((CLavaProgram*)ckd.document)->GetVStatCallSecN(ckd, stackFrame[SFH], selfCtx,
                                               ((Reference*)baseInitDat->baseItf.ptr)->refDecl,
                                               baseInitDat->vSectionNumber,
                                               baseInitDat->isOuter);
      if (ckd.exceptionThrown)
        return false; // goto ret;
      callObj = stackFrame[SFH] - (*stackFrame[SFH])[0].sectionOffset;
      fDesc = &(*callObj)[secN].funcDesc[0];
      frameSize = fDesc->stackFrameSize;
      if (frameSize) {
#ifdef WIN32
        frameSizeBytes = frameSize<<2;
        __asm {
          sub esp, frameSizeBytes
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize];
#endif
        newStackFrame[SFH] = callObj + (*callObj)[secN].sectionOffset;

        if (fDesc->isNative) {
          NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
        }
        else
          FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)
#ifdef WIN32
        __asm {
          add esp, frameSizeBytes
        }
#else
        delete [] newStackFrame;
#endif
        if (ckd.exceptionThrown)
          return false; // goto ret
      }
    }
  }
  return true;
}

bool SelfVarX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  unsigned oldStackLevel=ckd.currentStackLevel, newOldExprLevel=stackFrameSize-1,
	         oeLevel, i;
  bool ok;
  RTAssDataDict rtadDict;
  RTInvDataDict rtidDict;
  AssertionData *adp;
  InvarData *idp;

//  if (isFuncBody)
    currentStackDepth++;
  ckd.currentStackLevel = nParams + SFH;
  ckd.selfVar = this;
  if (funcDECL)
    adp = (AssertionData*)funcDECL->Exec.ptr;
  if (itfDECL)
    idp = (InvarData*)itfDECL->Exec.ptr;

  ok = ExecBaseInits(ckd,stackFrame,oldExprLevel);
  if (!ok || ckd.exceptionThrown)
    goto ret;

  if (isFuncBody && adp) {
    rtadDict.setAutoDelete(true);
    rtadDict.insert(adp,new RTAssertionData(oldExprLevel));
  }

  if (LBaseData->m_checkPreconditions
  && isFuncBody
  && !((unsigned)stackFrame[2] & 1)
  && adp
  && adp->hasOrInheritsPreconditions) {
    ok = adp->EvalPreConditions(ckd,rtadDict,stackFrame,0,true);
    if (!ok || ckd.exceptionThrown)
      goto ret2;
  }

  if (isFuncBody && adp) {
	  oeLevel = stackFrameSize;
		i = adp->nOwnOldExpr+adp->nOvrOldExpr;
		while (i--)
			stackFrame[--oeLevel] = 0;

    ok = adp->EvalOldExpressions(ckd,rtadDict,stackFrame,newOldExprLevel);
    // also fills rtadDict which is needed below
    if (!ok || ckd.exceptionThrown)
      goto ret2;
  }

  ok = ((SynObject*)body.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  if (!ok && execDECL->DeclType == ExecDef && execDECL->ParentDECL->DeclType == Function && nOutputs)
    SetRTError(ckd,&ERR_OutFunctionFailed,stackFrame);
  if (!ok || ckd.exceptionThrown)
    goto ret;

  if (LBaseData->m_checkPostconditions
  && isFuncBody
  && !((unsigned)stackFrame[2] & 1)
  && adp
  && adp->hasOrInheritsPostconditions) {
    ok = adp->EvalPostConditions(ckd,rtadDict,stackFrame,0);
    if (!ok || ckd.exceptionThrown)
      goto ret;
  }

  if (LBaseData->m_checkInvariants
  && isFuncBody
  && !funcDECL->TypeFlags.Contains(isInitializer)
  && !funcDECL->TypeFlags.Contains(isConst)
  && !((unsigned)stackFrame[2] & 1)
  && idp
  && idp->hasOrInheritsInvariants)
    ok = idp->EvalInvariants(ckd,rtidDict,stackFrame);

ret:
  if (isFuncBody && adp) { // release old-expression-objects
	  oeLevel = stackFrameSize;
		i = adp->nOwnOldExpr+adp->nOvrOldExpr;
		while (i--)
			if (stackFrame[--oeLevel])
				DFC(stackFrame[oeLevel]);
	}
ret2:
//  if (isFuncBody)
    currentStackDepth--;
  ckd.currentStackLevel = oldStackLevel;
  ckd.immediateReturn = false;
  RETURN(ok)
}

bool SemicolonOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  bool ok;

//  STOP_AT_STM(ckd, stackFrame, true)

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame,oldExprLevel);
    if (!ok || ckd.exceptionThrown)
      return false;
    else if (ckd.immediateReturn)
      return true;
  }
  SUCCEED
}

bool OrOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, true)

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown || ckd.immediateReturn)
      return ok; // ok is true in succeed case!
    else if (ok)
      return true;
  }

  return false;
}
/*
bool OrExOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  bool ok, firstTime=true;

  if (flags.Contains(ignoreSynObj)) return true;

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame,oldExprLevel);
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
bool XorOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  unsigned nTrue=0;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, true)

  for ( opd=(CHE*)operands.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)opd->data)->Execute(ckd,stackFrame,oldExprLevel);
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


bool SucceedStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  STOP_AT_STM(ckd, stackFrame, 0)
  ckd.immediateReturn = true;
  return true;
}


bool FailStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr object=0, lastEx=ckd.lastException;
  int ii;

  STOP_AT_STM(ckd, stackFrame, 0)
  if (lastEx)
    DFC(lastEx);
  if (exception.ptr) {
    object = ((Expression*)exception.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown)
      return false;
    if (!object) {
      ((SynObject*)exception.ptr)->SetRTError(ckd,&ERR_NullException,stackFrame,"fail statement: exception object =0");
      return false;
    }
    ((SynFlags*)(object+1))->INCL(finished);
    for (ii = 0; (ii < object[0][0].nSections) && (object[0][ii].classDECL != ckd.document->DECLTab[B_Exception]); ii++);
    ckd.callStack = DebugStop(ckd,stackFrame,"Exception thrown by \"fail\" statement");
    ckd.lastException = object + object[0][ii].sectionOffset;
    ckd.exceptionThrown = true;
  }
  else
    ckd.immediateReturn = true;
  return false;
}

LavaObjectPtr OldExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {

  STOP_AT_BPT(ckd,stackFrame)

  if (!INC_FWD_CNT(ckd,stackFrame[oldExprLevel-iOldExpr])) return (LavaObjectPtr)(-1);
  return stackFrame[oldExprLevel-iOldExpr];
}

LavaObjectPtr UnaryOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  Expression *opd;
  LavaObjectPtr object;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize, frameSizeBytes;

  if (flags.Contains(ignoreSynObj)) return 0;

  opd = (Expression*)operand.ptr;
  object = opd->Evaluate(ckd,stackFrame,oldExprLevel);
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
  frameSizeBytes = frameSize<<2;
  __asm {
    sub esp, frameSizeBytes
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize];
#endif
  newStackFrame[SFH] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
//                            + funcSect[fDesc->delta].sectionOffset;
  newStackFrame[SFH+1] = 0;

  STOP_AT_OP(ckd,stackFrame)
  
  if (fDesc->isNative) 
    NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
  else 
    FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)

  DFC(object);
  object = newStackFrame[SFH+1];
#ifdef WIN32
  __asm {
    add esp, frameSizeBytes
  }
#else
	delete [] newStackFrame;
#endif
  if (!ok)
    return (LavaObjectPtr)-1;;
  return object;
}

bool BinaryOpX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  Expression *opd, *opd2;
  LavaObjectPtr object, object2;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize, frameSizeBytes;

  STOP_AT_STM(ckd, stackFrame, true)

  opd = (Expression*)operand1.ptr;
  object = opd->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown) {
    if (object)
      DFC(object); 
    return false;
  }
  opd2 = (Expression*)operand2.ptr;
  object2 = opd2->Evaluate(ckd,stackFrame,oldExprLevel);
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
  frameSizeBytes = frameSize<<2;
  __asm {
    sub esp, frameSizeBytes
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize];
#endif
  newStackFrame[SFH] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
  if (object2) {
    if (opd2->formVType->DeclType == VirtualType) {
      newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object2, callCtx, newStackFrame[SFH][0][0].classDECL, opd2->formVType, opd2->vSectionNumber, opd2->isOuter);
      if (ckd.exceptionThrown)
        goto ret;
    }
    else
      newStackFrame[SFH+1] = CASTOBJECT(object2, opd2->sectionNumber);
  }
  else
    newStackFrame[SFH+1] = 0;

  STOP_AT_OP(ckd,stackFrame)

  if (fDesc->isNative) 
    NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
  else 
    FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)

  if (newStackFrame[SFH+1])
    DFC(newStackFrame[SFH+1]);  // release input parameter
ret:
  DFC(newStackFrame[SFH]);  // release self
#ifdef WIN32
  __asm {
    add esp, frameSizeBytes
  }
#else
  delete [] newStackFrame;
#endif
  RETURN(ok)
}

LavaObjectPtr MultipleOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  LavaObjectPtr object, object2;
  LavaVariablePtr newStackFrame;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;
  unsigned frameSize, frameSizeBytes;

  opd = (CHE*)operands.first;
  object = ((Expression*)opd->data)->Evaluate(ckd,stackFrame,oldExprLevel);
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
  frameSizeBytes = frameSize<<2;
  __asm {
    sub esp, frameSizeBytes
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize];
#endif
  newStackFrame[SFH] = object - (*object)->sectionOffset
                       + (*(object - (*object)->sectionOffset))[fDesc->delta].sectionOffset;
  opd = (CHE*)opd->successor;
  for (; opd; opd = (CHE*)opd->successor) {
    object2 = ((Expression*)opd->data)->Evaluate(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown) {
      object = (LavaObjectPtr)-1;
      goto ret;
    }
    if (object2) {
      if (formVType->DeclType == VirtualType) {
        newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object2, callCtx, newStackFrame[SFH][0][0].classDECL, formVType, vSectionNumber, isOuter);
        if (ckd.exceptionThrown) {
          ok = false;
          object = (LavaObjectPtr)-1;
          goto ret0;
        }
      }
      else
        newStackFrame[SFH+1] = CASTOBJECT(object2,((Expression*)opd->data)->sectionNumber);
    }
    else
      newStackFrame[SFH+1] = 0;
    newStackFrame[SFH+2] = 0;

    STOP_AT_OP(ckd,stackFrame)

    if (fDesc->isNative) 
      NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)
    
    if (newStackFrame[SFH+1])
      DFC(newStackFrame[SFH+1]);  // release input parameter
ret0:
    DFC(newStackFrame[SFH]);  // release self
    if (!ok)
      goto ret;
    object = newStackFrame[SFH+2];
    if (!object) {
      SetRTError(ckd,&ERR_NullCallObject,stackFrame,"MultipleOpX::Evaluate3");
      goto ret;
    }
    newStackFrame[SFH] = object; // no cast???
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
    add esp, frameSizeBytes
  }
#else
  delete [] newStackFrame;
#endif
  return object;
}

LavaObjectPtr EvalExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr trueObj;
  
  trueObj = AllocateObject(ckd,ckd.document->DECLTab[B_Bool],flags.Contains(isVariable));
  if (!trueObj) {
    if (!ckd.exceptionThrown)
      SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
    return (LavaObjectPtr)-1;
  }
  ((SynFlags*)(trueObj+1))->INCL(finished);
  *(bool*)(trueObj+LSH) = ((SynObject*)operand.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  return trueObj;
}

bool EvalStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr trueObj, object;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, 0)

  trueObj = AllocateObject(ckd,ckd.document->DECLTab[B_Bool],flags.Contains(isVariable));
  if (!trueObj) {
    if (!ckd.exceptionThrown)
      SetRTError(ckd,&ERR_AllocObjectFailed,stackFrame);
    return false;
  }
  ((SynFlags*)(trueObj+1))->INCL(finished);
  *(bool*)(trueObj+LSH) = true;
  object = ((SynObject*)operand.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown) {
    DFC(trueObj);
    return false;
  }
  ok = EqualObjects(ckd,object,trueObj,0);
  DFC(trueObj);
  DFC(object);
  RETURN(ok);
}

bool InSetStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr setObj, setElem, fromObj, toObj;
  IntegerInterval *intv;
  int fromInt, toInt, secN, ii;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, 0)

  setElem = ((SynObject*)operand1.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown)
    return false;
  if (!setElem) {
    ((SynObject*)operand1.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
    return false;
  }
  if (((SynObject*)operand2.ptr)->IsIntIntv()) {
    intv = (IntegerInterval*)operand2.ptr;
    fromObj = ((SynObject*)intv->from.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown) {
      DFC(setElem);
      return false;
    }
    if (!fromObj) {
      ((SynObject*)intv->from.ptr)->SetRTError(ckd,&ERR_NullMandatory,stackFrame);
      DFC(setElem);
      return false;
    }
    toObj = ((SynObject*)intv->to.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
    setObj = ((SynObject*)operand2.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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


LavaObjectPtr CallbackX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  FuncStatement *funcStm=(FuncStatement*)callback.ptr;
  Expression *onEventExpr=(Expression*)onEvent.ptr;
  Expression *objRef;
  LavaObjectPtr callObj, object, result;
  CallbackObject cbObj;

  objRef = (ExpressionX*)funcStm->handle.ptr;
  cbObj.callObj = objRef->Evaluate(ckd,stackFrame,oldExprLevel);

  object = onEventExpr->Evaluate(ckd,stackFrame,oldExprLevel);
  result = AllocateObject(ckd,ckd.document->DECLTab[B_Callback],flags.Contains(isVariable));
  return result;
}


bool AssertStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {

  STOP_AT_STM(ckd, stackFrame, true)

  if (!((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel)) {
    SetRTError(ckd,&ERR_AssertionViolation,stackFrame);
    return false;
  }
  else
    SUCCEED
}

bool IfStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, true)

  for ( opd=(CHE*)ifThens.first;
        opd;
        opd = (CHE*)opd->successor) {
    ok = ((SynObject*)((IfThenX*)opd->data)->ifCondition.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown || ckd.immediateReturn)
      return false;
    else if (ok)
      RETURN(((SynObject*)((IfThenX*)opd->data)->thenPart.ptr)->Execute(ckd,stackFrame,oldExprLevel))
  }

  if (elsePart.ptr)
    RETURN(((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame,oldExprLevel))
  else
    return true;
}

bool TryStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, true)

  ok = ((SynObject*)tryStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  if (!ckd.exceptionThrown)
    return ok;

  for ( opd=(CHE*)catchClauses.first;
        opd;
        opd = (CHE*)opd->successor) {
    ckd.exceptionThrown = false;
    ok = ((SynObject*)((CatchClauseX*)opd->data)->catchClause.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    if (!ckd.exceptionThrown) {
      DropException(ckd,0);
			return ok;
    }
  }
	return false;
}
 
LavaObjectPtr IfExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *opd;
  LavaObjectPtr result=(LavaObjectPtr)-1;

  STOP_AT_BPT(ckd,stackFrame)

  for ( opd=(CHE*)ifThens.first;
        opd;
        opd = (CHE*)opd->successor)
    if (((Expression*)((IfxThenX*)opd->data)->ifCondition.ptr)->Execute(ckd,stackFrame,oldExprLevel)) {
      if (ckd.exceptionThrown || ckd.immediateReturn)
        return (LavaObjectPtr)-1;
      result = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->Evaluate (ckd,stackFrame,oldExprLevel);
      if (ckd.exceptionThrown || ckd.immediateReturn)
        return (LavaObjectPtr)-1;
      if (((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->IsIfStmExpr())
        sectionNumber = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->sectionNumber;
      else if (((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->primaryToken != nil_T)
        sectionNumber = ckd.document->GetSectionNumber(ckd, ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->finalType,targetDecl);
      return result;
    }
  result = ((Expression*)elsePart.ptr)->Evaluate (ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown || ckd.immediateReturn)
    return (LavaObjectPtr)-1;
  if (((Expression*)elsePart.ptr)->IsIfStmExpr())
    sectionNumber = ((Expression*)((IfxThenX*)opd->data)->thenPart.ptr)->sectionNumber;
  else if (((Expression*)elsePart.ptr)->primaryToken != nil_T)
    sectionNumber = ckd.document->GetSectionNumber(ckd, ((Expression*)elsePart.ptr)->finalType,targetDecl);
  return result;
}

bool SwitchStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *branchp, *caseLabelp;
  BranchX *branch;
  SynObject *caseLabel;
  LavaObjectPtr caseExpr, caseLab;
  bool ok;

  STOP_AT_STM(ckd, stackFrame, true)

  caseExpr = ((Expression*)caseExpression.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
      caseLab = caseLabel->Evaluate(ckd,stackFrame,oldExprLevel);
      if (ckd.exceptionThrown) {
        DFC(caseExpr);
        return false;
      }
      ok = EqualObjects(ckd,caseLab,caseExpr,0);
      if (ok) {
        DFC(caseExpr);
        ok = ((SynObject*)branch->thenPart.ptr)->Execute(ckd,stackFrame,oldExprLevel);
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
    RETURN(((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame,oldExprLevel))
   else {
    SetRTError(ckd,&ERR_SwitchMissingElse,stackFrame);
    return false;
  }
}

bool TypeSwitchStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *branchp;
  TypeBranch *branch;
  LavaObjectPtr caseExpr, caseExprCasted;
  bool ok=true;
  int secN;

  STOP_AT_STM(ckd, stackFrame, true)

  ckd.currentStackLevel++;
  caseExpr =((Expression*)caseExpression.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
      ok = ((SynObject*)branch->thenPart.ptr)->Execute(ckd,stackFrame,oldExprLevel);
      DFC(caseExpr);
      ckd.currentStackLevel--;
      RETURN(ok)
    }
  }

  DFC(caseExpr);
  if (elsePart.ptr)
    ok = ((SynObject*)elsePart.ptr)->Execute(ckd,stackFrame,oldExprLevel);

  ckd.currentStackLevel--;
  RETURN(ok)
}

bool DeclareX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  unsigned i;
  bool result;
  unsigned stackLevel = ckd.currentStackLevel;
  
//  STOP_AT_STM(ckd, stackFrame, true)

  for ( i = stackLevel;
        i < stackLevel+nQuantVars;
        i++)
    stackFrame[i] = 0;
  ckd.currentStackLevel += nQuantVars;
  result = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  for ( i = stackLevel;
        i < stackLevel+nQuantVars;
        i++)
    if (stackFrame[i])
      DFC(stackFrame[i]);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(result);
}

static bool Enumerate (SynObject *callObj, CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel,
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
    fromObj = ((SynObject*)intv->from.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
    toObj = ((SynObject*)intv->to.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
        ok = callObj->Recursion(ckd,stackFrame,oldExprLevel,cheQuant,cheVar,rSet);
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
        ok = callObj->Recursion(ckd,stackFrame,oldExprLevel,cheQuant,cheVar,rSet);
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
      ok = callObj->Recursion(ckd,stackFrame,oldExprLevel,cheQuant,cheVar,rSet);
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

      ok = callObj->Recursion(ckd,stackFrame,oldExprLevel,cheQuant,cheVar,rSet);
      if (!ok) break;
    }
  }

  return ok;
}

bool ExistsX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel,
                         CHE *cheQuant, CHE *cheVar, LavaObjectPtr) {
  Quantifier *quant;
//  LavaObjectPtr setObj;
//  CHAINX *setChain;
  CHE /**cheElem,*/ *cheVar2;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,(CHE*)cheVar->successor);
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
      ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar2);
    }
    else
      if (updateStatement.ptr) {
        if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel))
          ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
      }
      else
        ok = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  }
  return ok;
}

bool ExistsX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  unsigned stackLevel = ckd.currentStackLevel;
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  bool ok;
  
  STOP_AT_STM(ckd, stackFrame, true)

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(ok)
}

bool ForeachX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel,
                          CHE *cheQuant, CHE *cheVar, LavaObjectPtr) {
  Quantifier *quant;
  CHE *cheVar2;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,(CHE*)cheVar->successor);
  else {
    if (cheQuant->successor) {
      cheQuant = (CHE*)cheQuant->successor;
      quant = (Quantifier*)cheQuant->data;
      cheVar2 = (CHE*)quant->quantVars.first;
      ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar2);
    }
    else
      if (statement.ptr)
        if (updateStatement.ptr) {
          if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel))
            ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
        }
        else
          ok = ((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
      else
        ok = ((SynObject*)updateStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
  }
  return ok;
}

bool ForeachX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  bool ok;
  
  STOP_AT_STM(ckd, stackFrame, true)

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar);
  ckd.currentStackLevel -= nQuantVars;
  RETURN(ok)
}

bool SelectExpressionX::Recursion (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel,
                                   CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet) {
  Quantifier *quant;
  CHE *cheVar2;
  LavaObjectPtr resultObj;
  LavaVariablePtr newStackFrame;
  unsigned frameSize, frameSizeBytes;
  bool ok=true;

  if (cheVar->successor)
    ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,(CHE*)cheVar->successor,rSet);
  else {
    if (cheQuant->successor) {
      cheQuant = (CHE*)cheQuant->successor;
      quant = (Quantifier*)cheQuant->data;
      cheVar2 = (CHE*)quant->quantVars.first;
      ok = Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar2,rSet);
    }
    else
      if (((SynObject*)statement.ptr)->Execute(ckd,stackFrame,oldExprLevel)) { // add resultObj to rSet
        resultObj = ((Expression*)addObject.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
        frameSize = 8;
#ifdef WIN32
        frameSizeBytes = frameSize<<2;
        __asm {
          sub esp, frameSizeBytes
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize];
#endif
        newStackFrame[SFH] = CastSetType(ckd,rSet);
        newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVObj(ckd, resultObj,0);
        SetAdd(ckd, newStackFrame);
        DFC(resultObj); // release new object
        // release stackFrame frame
#ifdef WIN32
        __asm {
          add esp, frameSizeBytes
        }
#else
				delete [] newStackFrame;
#endif
      }
  }
  return ok;
}

LavaObjectPtr SelectExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  //unsigned i;
  unsigned stackLevel = ckd.currentStackLevel;
  CHE *cheQuant, *cheVar;
  Quantifier *quant;
  LavaObjectPtr rSet=0;
  
  STOP_AT_BPT(ckd,stackFrame)

  ckd.currentStackLevel += nQuantVars;

  cheQuant = (CHE*)quantifiers.first;
  quant = (Quantifier*)cheQuant->data;
  cheVar = (CHE*)quant->quantVars.first;

  rSet =((Expression*)resultSet.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);

  Enumerate(this,ckd,stackFrame,oldExprLevel,cheQuant,cheVar,rSet);
  ckd.currentStackLevel -= nQuantVars;
  return rSet;
}

bool ObjReferenceX::assign (SynObject *source,
                    LavaObjectPtr object,
                    TargetType targetType,
                    unsigned sectionNumber,
                    CheckData &ckd,
                    LavaVariablePtr stackFrame,
                    unsigned oldExprLevel) {

  LavaVariablePtr var, newStackFrame;
  LavaObjectPtr arrayObj, indexObj, propertyInfo;
  SynObject *arrayRef;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=1, frameSize, frameSizeBytes;
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
    if (!(var = GetMemberVarPtr(ckd,stackFrame,stackPos,oldExprLevel))) {
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
    propertyInfo = GetPropertyInfo(ckd,stackFrame,stackPos,setExec,oldExprLevel);
    if (setExec && setExec->Exec.ptr == ckd.selfVar) goto fieldCase;
    if (!propertyInfo) {
      SetRTError(ckd,&ERR_NullCallObject,stackFrame,"assign/set function");
      return false;
    }
    frameSize = ((SelfVar*)setExec->Exec.ptr)->stackFrameSize;
#ifdef WIN32
    frameSizeBytes = frameSize<<2;
    __asm {
      sub esp, frameSizeBytes
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize];
#endif
    newStackFrame[SFH] = propertyInfo; // set self
    IFC(newStackFrame[SFH]); 
    if (object) {
      if (refIDs.first != refIDs.last) {
        lastDOD = (TDOD*)((CHE*)refIDs.last)->data;
        if (lastDOD->vType->DeclType == VirtualType) {
          ckd.tempCtx = lastDOD->context;
          ckd.document->GetTypeAndContext(lastDOD->vType, ckd.tempCtx);
          newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, ckd.tempCtx, newStackFrame[SFH][0][0].classDECL, lastDOD->vType, lastDOD->vSectionNumber, lastDOD->isOuter);
        }
        else
          newStackFrame[SFH+1] = CASTOBJECT(object, sectionNumber);
      }
      else
        newStackFrame[SFH+1] = CASTOBJECT(object, sectionNumber);
      IFC(newStackFrame[SFH+1]);  // set input parameter
    }
    else
      newStackFrame[SFH+1] = 0;

    FCALL(ckd,((SynObject*)setExec->Exec.ptr),newStackFrame,frameSize,ok);

    DFC(newStackFrame[SFH]);  // release self
    if (newStackFrame[SFH+1])
      DFC(newStackFrame[SFH+1]);  // release input parameter
#ifdef WIN32
    __asm {
      add esp, frameSizeBytes
    }
#else
    delete [] newStackFrame;
#endif
    break;

  case arrayElem: // call operator[]<- function
    aai = (ArrayAtIndex*)this;
    arrayRef = (SynObject*)aai->arrayObj.ptr;
    arrayObj = arrayRef->Evaluate(ckd,stackFrame,oldExprLevel);
    if (!arrayObj) {
      arrayRef->SetRTError(ckd,&ERR_NullCallObject,stackFrame,"assign array element, null-array");
      return false;
    }
    funcSect = &(*arrayObj)[aai->funcSectionNumber + aai->funcDecl->SectionInfo2];
    fDesc = &funcSect->funcDesc[aai->funcDecl->SectionInfo1];
#ifdef WIN32
    __asm {
      sub esp, 24
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[SFH+3];
#endif
    newStackFrame[SFH] = arrayObj - (*arrayObj)->sectionOffset
                       + (*(arrayObj - (*arrayObj)->sectionOffset))[fDesc->delta].sectionOffset;
    //IFC(newStackFrame[SFH]);  // set self
    indexExpr = (Expression*)aai->arrayIndex.ptr;
    indexObj = indexExpr->Evaluate(ckd,stackFrame,oldExprLevel);
    if (indexObj) {
      if (indexExpr->formVType->DeclType == VirtualType)
        newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, indexObj, aai->callCtx, newStackFrame[SFH][0][0].classDECL, indexExpr->formVType, indexExpr->vSectionNumber, indexExpr->isOuter);
      else
        newStackFrame[SFH+1] = CASTOBJECT(indexObj, indexExpr->sectionNumber);
    }
    else {
      source->SetRTError(ckd,&ERR_NullMandatory,stackFrame,"assign array element, null-index");
      goto ret0;
    }
    if (object)
      newStackFrame[SFH+2] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, aai->callCtx, newStackFrame[SFH][0][0].classDECL, aai->formVType, aai->vSectionNumber, aai->isOuter);
    else
      newStackFrame[SFH+2] = 0;

    if (fDesc->isNative) 
      NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,6,ok)
    else 
      FCALL(ckd,((SynObject*)fDesc->funcExec->Exec.ptr),newStackFrame,6,ok)

    if (newStackFrame[SFH+2])
      DFC(newStackFrame[SFH+2]);  // release input parameter 2
    DFC(newStackFrame[SFH+1]);  // release input parameter 1
ret0: DFC(newStackFrame[SFH]);  // release self

#ifdef WIN32
    __asm {
      add esp, 24
    }
#else
    delete [] newStackFrame;
#endif
    break;
  }

  return ok;
}

bool AssignmentX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr object;

  STOP_AT_STM(ckd, stackFrame, true)

  object = ((Expression*)exprValue.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown)
    return false;

  RETURN(((ObjReferenceX*)targetObj.ptr)->assign((Expression*)exprValue.ptr,
         object,
         kindOfTarget,
         ((Expression*)exprValue.ptr)->sectionNumber,
         ckd,
         stackFrame,
         oldExprLevel))
}

LavaObjectPtr ArrayAtIndexX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  Expression *array, *expr;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;

  array = (Expression*)arrayObj.ptr;
  callObj = array->Evaluate(ckd,stackFrame,oldExprLevel);
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
  newStackFrame = new LavaObjectPtr[SFH+3];
#endif
  newStackFrame[SFH] = callObj - (*callObj)->sectionOffset
                       + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
//                            + funcSect[fDesc->delta].sectionOffset;
  expr = (Expression*)arrayIndex.ptr;
  object = expr->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown)
    goto ret;
  if (object) {
    if (expr->formVType->DeclType == VirtualType)
      newStackFrame[SFH+1] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx/*callObj[0][0].classDECL*/, newStackFrame[SFH][0][0].classDECL, expr->formVType, expr->vSectionNumber, expr->isOuter);
    else
      newStackFrame[SFH+1] = CASTOBJECT(object, expr->sectionNumber);
  }
  else
    newStackFrame[SFH+1] = 0;
  newStackFrame[SFH+2] = 0;
  if (fDesc->isNative) 
    NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,20,ok)
  else 
    FCALL(ckd,((SynObject*)fDesc->funcExec->Exec.ptr),newStackFrame,5,ok)
  if (ok) {
    object = newStackFrame[SFH+2];
    if (object)
      object = ((CLavaProgram*)ckd.document)->CastVOutObj(ckd, object, callCtx, formVType, vSectionNumber, isOuter);
  }
ret:
  DFC(callObj); // release call object
  DFC(newStackFrame[SFH+1]); // release input parameter (= array index)
#ifdef WIN32
  __asm { // not earlier, since newStackFrame[SFH+1] is still used in the preceding statement
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

LavaObjectPtr ConstantX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel)
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
#ifdef WIN32
//      _clearfp();
#endif
      *(float*)(value+LSH) = (float)strtod(str.c,0); //(float)atof(str.c);
      break;
    case Double:
#ifdef WIN32
//      _clearfp();
#endif
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
      sub esp, 12
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[SFH+1];
#endif
    newStackFrame[SFH] = value;
    StringDecFunc(ckd, newStackFrame);
#ifdef WIN32
    __asm {
      add esp, 12
    }
#else
		delete [] newStackFrame;
#endif
    break;
  }
  delete [](value-LOH);
}

LavaObjectPtr BoolConstX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel)
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

LavaObjectPtr EnumConstX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel)
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
  LavaObjectPtr newStackFrame[SFH+1];

  if (value) {
    newStackFrame[SFH] = enumBaseObj+1; // +1 since the QString value follows after the int value
    StringDecFunc(ckd,newStackFrame);
    delete [] (value-LOH);
  }
}

LavaObjectPtr ObjReferenceX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr object=GetMemberObjPtr(ckd,stackFrame,stackPos,oldExprLevel);
  if (object)
    IFC(object);  // set "output parameter"
  return object;
}

LavaObjectPtr CloneExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr obj, copyOfObj=0;
  CRuntimeException *ex;
  SynObjectBase *mySelfVar=ckd.selfVar;
  bool ok;

  STOP_AT_BPT(ckd,stackFrame)

  obj = ((ObjReferenceX*)fromObj.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);  
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  ex = CopyObject(ckd,&obj,&copyOfObj,obj?((SynFlags*)(obj+1))->Contains(stateObjFlag):true/*ckd.stateObj*/);
  ckd.selfVar = mySelfVar;
  if (ex) {
    ex->message = ex->message + DebugStop(ckd,stackFrame,ex->message);
    if (ex->SetLavaException(ckd)) 
      delete ex;
  }
  if (obj)
    DFC(obj);
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  
  stackFrame[((VarName*)varName.ptr)->stackPos] = copyOfObj;

  if (butStatement.ptr) {
    ((SynObject*)butStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown)
      return (LavaObjectPtr)-1;
  }

  if (execDECL) {
    ok = ((SelfVar*)execDECL->Exec.ptr)->Execute(ckd,stackFrame,oldExprLevel);
    ckd.selfVar = mySelfVar;
    if (!ok) {
      SetRTError(ckd,&ERR_AssertionViolation,stackFrame);
      return (LavaObjectPtr)-1;
    }
  }
  ((SynFlags*)(copyOfObj+1))->INCL(finished);
  return copyOfObj;
}

bool CopyStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr obj, copyOfObj;
  bool ok = true, nullTarget;
  CRuntimeException *ex;
  SynObjectBase *mySelfVar=ckd.selfVar;

  STOP_AT_STM(ckd, stackFrame, true)
 
  obj = ((ExpressionX*)fromObj.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);  
  if (ckd.exceptionThrown)
    return false;
  copyOfObj = ((ObjReferenceX*)ontoObj.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);  
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
    ex->message = ex->message + DebugStop(ckd,stackFrame,ex->message);
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
           stackFrame,
           oldExprLevel);
  }
  else if (copyOfObj)
    DFC(copyOfObj);
  RETURN(ok)
}

LavaObjectPtr EnumItemX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr value, index, enumBaseObj;
  int indexInt, secN, ii;
  LavaDECL *enumDecl;
  TEnumDescription *enumDesc;
  CHEEnumSelId *cheItem;

  STOP_AT_STM(ckd, stackFrame, 0)
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
  index = ((Expression*)itemNo.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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

LavaObjectPtr ExtendExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr obj, copyOfObj=0;
  SynObjectBase *mySelfVar=ckd.selfVar;
  CRuntimeException *ex;

  obj = ((ObjReferenceX*)extendObj.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);  
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  ex = CopyObject(ckd,&obj,&copyOfObj,false,extendTypeDecl);
  ckd.selfVar = mySelfVar;
  if (ckd.exceptionThrown)
    return (LavaObjectPtr)-1;
  if (ex) {
    ex->message = ex->message + DebugStop(ckd,stackFrame,ex->message);
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

LavaObjectPtr AttachObjectX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaObjectPtr urlObj, rtObj, urlStr;
  LavaDECL *cosDecl;
  int secn;

  STOP_AT_BPT(ckd,stackFrame)

  cosDecl = ((Reference*)objType.ptr)->refDecl;
  urlObj = ((Expression*)url.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
  if (ckd.exceptionThrown)
    return false;
  if ((cosDecl->nOutput == PROT_LAVA) || (cosDecl->nOutput == PROT_NATIVE)) {
    rtObj = AttachLavaObject(ckd,urlObj,cosDecl,typeDECL,attachCat == stateObj);
    if (!rtObj && !ckd.exceptionThrown) {
      secn = ckd.document->GetSectionNumber(ckd, (*urlObj)->classDECL, ckd.document->DECLTab[VLString]);
      urlStr = (LavaObjectPtr)(urlObj - (*urlObj)->sectionOffset + (*urlObj)[secn].sectionOffset);
      SetRTError(ckd,&ERR_ldocNotOpened,stackFrame, *(QString*)(urlStr + LSH));
    }
    return rtObj;
  }
  else if (cosDecl->nOutput == PROT_NATIVE) {
  }
  return 0;
}


LavaObjectPtr NewExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  LavaVariablePtr newStackFrame;
  unsigned frameSize, frameSizeBytes, oldStackLevel = ckd.currentStackLevel;
  LavaObjectPtr object, urlObj;
  CVFuncDesc *fDesc;
  bool ok=true;
  LavaDECL *cosDecl;
  SynObjectBase *mySelfVar=ckd.selfVar;
  InvarData *idp;
  RTInvDataDict rtidDict;

  STOP_AT_BPT(ckd,stackFrame)

  if (itf.ptr) {
    cosDecl = ((Reference*)objType.ptr)->refDecl;
    urlObj = ((Expression*)url.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
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
      if (!((SynObject*)initializerCall.ptr)->Execute(ckd,stackFrame,oldExprLevel)) {
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
        frameSizeBytes = frameSize<<2;
        __asm {
          sub esp, frameSizeBytes
          mov newStackFrame, esp
        }
#else
        newStackFrame = new LavaObjectPtr[frameSize];
#endif
        newStackFrame[SFH] = object;
        if (fDesc->isNative) {
          NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
          if (ckd.exceptionThrown) {
            ckd.currentStackLevel = oldStackLevel;
            DFC(object);
            return (LavaObjectPtr)-1;
          }
        }
        else {
          FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)
          if (ckd.exceptionThrown) {
            ckd.currentStackLevel = oldStackLevel;
            DFC(object);
            return (LavaObjectPtr)-1;
          }
        }

#ifdef WIN32
        __asm {
          add esp, frameSizeBytes
        }
#else
        delete [] newStackFrame;
#endif
      }
    }

    if (butStatement.ptr) {
      ok = ((SynObject*)butStatement.ptr)->Execute(ckd,stackFrame,oldExprLevel);
      if (!ok) {
        ckd.currentStackLevel = oldStackLevel;
        DFC(object);
        return (LavaObjectPtr)-1;
      }
    }

    idp = (InvarData*)typeDECL->Exec.ptr;
    if (LBaseData->m_checkInvariants
    && !((unsigned)stackFrame[2] & 1)
    && idp
    && idp->hasOrInheritsInvariants) { // invoke invariant if it exists
      frameSize=idp->stackFrameSize;
#ifdef WIN32
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize];
#endif
      newStackFrame[0] = (LavaObjectPtr)this;
      newStackFrame[1] = (LavaObjectPtr)stackFrame;
      newStackFrame[2] = (LavaObjectPtr)((unsigned)stackFrame[2] & ~2);
      newStackFrame[SFH] = object;
      ckd.selfVar = mySelfVar;
      DebugStep oldDebugStep=nextDebugStep;
      nextDebugStep = noStep;

      ok = idp->EvalInvariants(ckd,rtidDict,newStackFrame);

      nextDebugStep = oldDebugStep;
#ifdef WIN32
      __asm {
        add esp, frameSizeBytes
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

LavaObjectPtr ParameterX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  return ((Expression*)parameter.ptr)->Evaluate(ckd,stackFrame,oldExprLevel);
}

LavaObjectPtr HandleOpX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  ObjReference *opd=(ObjReference*)operand.ptr;
  LavaObjectPtr handle=stackFrame[opd->stackPos + 1];

  IFC(handle);
  return handle;
}

bool FuncStatementX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE* che;
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj=0;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=SFH+1, nInputs=0, frameSize, frameSizeBytes;
  bool ok, dftInitializer=false;
  Expression *objRef, *inActParm;
  Parameter *outActParm;
  SelfVar *staticExec;
  SynObjectBase *mySelfVar=ckd.selfVar;
  LavaDECL *funcSectClass;
  int secN;

  if (parentObject->primaryToken == new_T && funcDecl->TypeFlags.Contains(defaultInitializer))
    dftInitializer = true;

  STOP_AT_STM(ckd, stackFrame, true)

  objRef = (ExpressionX*)handle.ptr;
  if (objRef) {
    callObj = objRef->Evaluate(ckd,stackFrame,oldExprLevel);
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
/*      if (funcDecl->TypeFlags.Contains(isNative)) 
        frameSize = funcDecl->nInput + funcDecl->nOutput + 1 + SFH;
      else*/
        frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize];
#endif
      newStackFrame[SFH] = callObj + ((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[fDesc->delta].sectionOffset;
    }
    else {
      funcSect = &(*callObj)[funcSectionNumber + funcDecl->SectionInfo2];
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
/*      if (funcDecl->TypeFlags.Contains(isNative)) 
        frameSize = funcDecl->nInput + funcDecl->nOutput + 1 + SFH;
      else*/
        frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize];
#endif
      newStackFrame[SFH] = callObj - (*callObj)->sectionOffset
                       + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
    }
  }
  else {
    if (funcDecl->TypeFlags.Contains(isNative)) 
      frameSize = funcDecl->nInput + funcDecl->nOutput + 1 + SFH;
    else {
      staticExec = (SelfVar*)funcDecl->RuntimeDECL->Exec.ptr;
      frameSize = staticExec->stackFrameSize;
    }
#ifdef WIN32
    frameSizeBytes = frameSize<<2;
    __asm {
      sub esp, frameSizeBytes
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize];
#endif
    newStackFrame[SFH] = 0; // self=0 for static functions!
  }

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    nInputs++;
    newStackFrame[pos++] = 0;
  }
  pos = SFH+1;
  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    inActParm = (Expression*)che->data;
    object = inActParm->Evaluate(ckd,stackFrame,oldExprLevel);
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
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, newStackFrame[SFH][0][0].classDECL, inActParm->formVType, inActParm->vSectionNumber, inActParm->isOuter);
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

  for (; pos < nParams+SFH;)
    newStackFrame[pos++] = 0;

  if (!dftInitializer)
    STOP_AT_FUNC(ckd,stackFrame,((Reference*)function.ptr))

  if (objRef)
    if (fDesc->isNative) {
      ckd.stateObj = stateObj;
      NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    }
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)
  else
    if (funcDecl->TypeFlags.Contains(isNative)) {
      ckd.stateObj = stateObj;
      NATIVE_FCALL(ckd,((TAdapterFunc)funcDecl->RuntimeDECL),newStackFrame,frameSize,ok)
    }
    else
      FCALL(ckd,staticExec,newStackFrame,frameSize,ok)
  if (!ok) {
    goto ret;
  }

  pos = nInputs+SFH+1;
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
             stackFrame,
             oldExprLevel);
    }
  }
ret:
  for (pos=SFH+1; pos <= nInputs+SFH; pos++)
    if (newStackFrame[pos])
      DFC(newStackFrame[pos]);  // release input parameters
  if (callObj)
    DFC(callObj); 
    // release self after input parameters since it might be needed for releasing
    // the inputs in case "set.Remove(#elem)"; see CheDecFunc in BAdapter.cpp
#ifdef WIN32
  __asm {
    add esp, frameSizeBytes
  }
#else
	delete [] newStackFrame;
#endif
  RETURN(ok)
}

LavaObjectPtr FuncExpressionX::Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE* che;
  LavaVariablePtr newStackFrame;
  LavaObjectPtr object, callObj=0;
  CSectionDesc *funcSect;
  CVFuncDesc *fDesc;
  unsigned pos=SFH+1, frameSize, frameSizeBytes, nInputs=0, newOldExprLevel;
  bool ok;
  Expression *objRef, *actParm;
  SelfVar* staticExec;
  SynObjectBase *mySelfVar=ckd.selfVar;
  LavaDECL *funcSectClass;
  int secN;

  objRef = (ExpressionX*)handle.ptr;
  if (objRef) {
    callObj = objRef->Evaluate(ckd,stackFrame,oldExprLevel);
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
      newOldExprLevel = frameSize - 3;
#ifdef WIN32
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize];
#endif
      newStackFrame[SFH] = callObj + ((CSectionDesc*)(*callObj)->classDECL->SectionTabPtr)[fDesc->delta].sectionOffset;
    }
    else {
      funcSect = &(*callObj)[funcSectionNumber + funcDecl->SectionInfo2];
      fDesc = &funcSect->funcDesc[funcDecl->SectionInfo1];
/*      if (funcDecl->TypeFlags.Contains(isNative)) 
        frameSize = funcDecl->nInput + funcDecl->nOutput + 1 + SFH;
      else*/
        frameSize = fDesc->stackFrameSize;
#ifdef WIN32
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#else
      newStackFrame = new LavaObjectPtr[frameSize];
#endif
      newStackFrame[SFH] = callObj - (*callObj)->sectionOffset
                         + (*(callObj - (*callObj)->sectionOffset))[fDesc->delta].sectionOffset;
    }
  }
  else {
    if (funcDecl->TypeFlags.Contains(isNative)) 
      frameSize = funcDecl->nInput + funcDecl->nOutput + 1 + SFH;
    else {
      staticExec = (SelfVar*)funcDecl->RuntimeDECL->Exec.ptr;
      frameSize = staticExec->stackFrameSize;
    }
#ifdef WIN32
    frameSizeBytes = frameSize<<2;
    __asm {
      sub esp, frameSizeBytes
      mov newStackFrame, esp
    }
#else
    newStackFrame = new LavaObjectPtr[frameSize];
#endif
    newStackFrame[SFH] = 0; // self=0 for static functions!
  }

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    nInputs++;
    newStackFrame[pos++] = 0;
  }
  pos = SFH + 1;
  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    actParm = (Expression*)che->data;
    object = actParm->Evaluate(ckd,stackFrame,oldExprLevel);
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
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, newStackFrame[SFH][0][0].classDECL, actParm->formVType, actParm->vSectionNumber, actParm->isOuter);
        else
          newStackFrame[pos++] = ((CLavaProgram*)ckd.document)->CastVInObj(ckd, object, callCtx, objTypeDecl, actParm->formVType, actParm->vSectionNumber, actParm->isOuter);
      else
        newStackFrame[pos++] = CASTOBJECT(object, actParm->sectionNumber);
    }
    else
      newStackFrame[pos++] = 0;
  }
  newStackFrame[pos] = 0;

  STOP_AT_FUNC(ckd,stackFrame,((Reference*)function.ptr))

  if (objRef)
    if (fDesc->isNative) {
      ckd.stateObj = stateObj;
      NATIVE_FCALL(ckd,(*fDesc->funcPtr),newStackFrame,frameSize,ok)
    }
    else 
      FCALL(ckd,((SelfVar*)fDesc->funcExec->Exec.ptr),newStackFrame,frameSize,ok)
  else
    if (funcDecl->TypeFlags.Contains(isNative)) {
      ckd.stateObj = stateObj;
      NATIVE_FCALL(ckd,((TAdapterFunc)funcDecl->RuntimeDECL),newStackFrame,frameSize,ok)
    }
    else
      FCALL(ckd,staticExec,newStackFrame,frameSize,ok)
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
  for (pos=SFH+1; pos <= nInputs+SFH; pos++)
    if (newStackFrame[pos])
      DFC(newStackFrame[pos]);  // release input parameters
  if (callObj)
    DFC(callObj);  // release self
    // release self after input parameters since it might be needed for releasing
    // the inputs in case "set.Remove(#elem)"; see CheDecFunc in BAdapter.cpp
#ifdef WIN32
  __asm {
    add esp, frameSizeBytes
  }
#else
  delete [] newStackFrame;
#endif
  return object;
}

bool RunX::Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
  CHE *che;
  LavaVariablePtr newStackFrame;
  unsigned pos=1, nInputs=0, frameSize, frameSizeBytes;
  LavaObjectPtr object;
  LavaDECL *myDECL = ckd.myDECL;
  SynObjectBase *mySelfVar=ckd.selfVar;

  STOP_AT_STM(ckd, stackFrame, true)

  if (!execDECL->WorkFlags.Contains(runTimeOK)) {
    ckd.myDECL = execDECL;
    ckd.inINCL = execDECL->inINCL;
    try {
      ((SelfVar*)execDECL->Exec.ptr)->Check(ckd);
      ckd.selfVar = mySelfVar;
    }
    catch(CUserException) {
      ckd.selfVar = mySelfVar;
      return false;
    }
    ckd.myDECL = myDECL;
    ckd.inINCL = myDECL->inINCL;
    execDECL->WorkFlags.INCL(runTimeOK);
  }
  frameSize = ((SelfVar*)execDECL->Exec.ptr)->stackFrameSize;
#ifdef WIN32
    frameSizeBytes = frameSize<<2;
  __asm {
    sub esp, frameSizeBytes
    mov newStackFrame, esp
  }
#else
  newStackFrame = new LavaObjectPtr[frameSize];
#endif
  newStackFrame[SFH] = 0;

  for (che = (CHE*)inputs.first; che; che = (CHE*)che->successor) {
    object = ((Expression*)che->data)->Evaluate(ckd,stackFrame,oldExprLevel);
    if (ckd.exceptionThrown)
      goto ret;
    if (object)
      newStackFrame[pos++] = CASTOBJECT(object, ((Expression*)che->data)->sectionNumber);
    else
      newStackFrame[pos++] = 0;
  }

  ((SelfVar*)execDECL->Exec.ptr)->Execute(ckd,newStackFrame,oldExprLevel);
  ckd.selfVar = mySelfVar;

ret:
#ifdef WIN32
  __asm {
    add esp, frameSizeBytes
  }
#else
  delete [] newStackFrame;
#endif
    SUCCEED  // initiators are autonomous, ==> initiator calls can only succeed
}

bool CVFuncDescX::Execute(SynObjectBase* obj, CheckData& ckd, LavaVariablePtr stackFrame)
{
  unsigned oldExprLevel=((SelfVar*)obj)->stackFrameSize-1;

  RETURN(((SelfVar*)obj)->Execute(ckd, stackFrame,oldExprLevel))
}


LavaVariablePtr ObjReferenceX::GetMemberVarPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, unsigned oldExprLevel)
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
  unsigned frameSize, frameSizeBytes;
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
          frameSizeBytes = frameSize<<2;
          __asm {
            sub esp, frameSizeBytes
            mov newStackFrame, esp
          }
#else
					newStackFrame = new LavaObjectPtr[frameSize];
#endif
          newStackFrame[SFH] = memObj - (*memObj)->sectionOffset
                         + (*(memObj - (*memObj)->sectionOffset))[aDesc->delta].sectionOffset;
          if (!newStackFrame[SFH]) {
#ifdef WIN32
            __asm {
              add esp, frameSizeBytes
            }
#else
						delete [] newStackFrame;
#endif
            ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
            return 0;
          }
          newStackFrame[SFH+1] = 0;
          ((SynObject*)aDesc->getExec->Exec.ptr)->Execute(ckd,newStackFrame,oldExprLevel);
          ckd.selfVar = mySelfVar;
          memObj = newStackFrame[SFH+1];
#ifdef WIN32
          __asm {
            add esp, frameSizeBytes
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

LavaObjectPtr ObjReferenceX::GetMemberObjPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, unsigned oldExprLevel)
{
  LavaVariablePtr var; 
  LavaObjectPtr obj;
  CHE *DODs=(CHE*)refIDs.first, *last;
  TDOD* lastdod;
  
  if (!(var = GetMemberVarPtr(ckd, stackFrame, stackPos,oldExprLevel)))
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

LavaObjectPtr ObjReferenceX::GetPropertyInfo(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, LavaDECL*& setExec, unsigned oldExprLevel)
{
  LavaVariablePtr objPtr=stackFrame+stackPos, memObjPtr, newStackFrame;
  LavaObjectPtr memObj=*objPtr, parent;
  LavaDECL *fieldDECL, *efType;
  CHE *DODs=(CHE*)refIDs.first, *cheo=(CHE*)DODs->successor, *oldCheo=DODs;
  CVAttrDesc *aDesc;
  CSectionDesc *attrSect;
  int sectIndex;
  unsigned frameSize, frameSizeBytes;
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
          frameSizeBytes = frameSize<<2;
          __asm {
            sub esp, frameSizeBytes
            mov newStackFrame, esp
          }
#else
					newStackFrame = new LavaObjectPtr[frameSize];
#endif
          newStackFrame[SFH] = memObj - (*memObj)->sectionOffset
                         + (*(memObj - (*memObj)->sectionOffset))[aDesc->delta].sectionOffset;
          if (!newStackFrame[SFH]) {
#ifdef WIN32
            __asm {
              add esp, frameSizeBytes
            }
#else
						delete [] newStackFrame;
#endif
            ((TDOD*)oldCheo->data)->SetRTError(ckd,&ERR_NullParent,stackFrame);
            return 0;
          }
          newStackFrame[SFH+1] = 0;
          ((SynObject*)aDesc->getExec->Exec.ptr)->Execute(ckd,newStackFrame,oldExprLevel);
          ckd.selfVar = mySelfVar;
          if (ckd.exceptionThrown) {
#ifdef WIN32
            __asm {
              add esp, frameSizeBytes
            }
#else
						delete [] newStackFrame;
#endif
            return 0;
          }
          memObj = newStackFrame[SFH+1];
#ifdef WIN32
          __asm {
            add esp, frameSizeBytes
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
