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


#include "LavaBase_all.h"
#include "BASEMACROS.h"

#include "LavaBaseDoc.h"
#include "BAdapter.h"
#include "docview.h"
#include "LavaBaseStringInit.h"

#include "qobject.h"
#include "qstring.h"
#include "qlibrary.h"

#include <QtDebug>

#pragma hdrstop

#ifdef WIN32
#include <windows.h>
#else
#include <signal.h>
#include <string.h>
#include <unistd.h>
#endif


bool INC_FWD_CNT(CheckData &ckd, LavaObjectPtr object) {
  register unsigned short fwdCnt;

  object = object - (*object)->sectionOffset;
  fwdCnt = *(((unsigned short *)object)-1);
  if (!fwdCnt || ((SynFlags*)(object+1))->Contains(zombified))
    return false; // better: fail exception
  *(((unsigned short *)object)-1) = ++fwdCnt;
  return true;
}

bool INC_REV_CNT(CheckData &ckd, LavaObjectPtr object) {
  register unsigned short fwdCnt, revCnt;

  object = object - (*object)->sectionOffset;
  fwdCnt = *(((unsigned short *)object)-1);
  revCnt = *(((unsigned short *)object)-2);
  if (!fwdCnt || ((SynFlags*)(object+1))->Contains(zombified))
    return false; // better: fail exception
  *(((unsigned short *)object)-2) = ++revCnt;
  return true;
}


bool DEC_FWD_CNT (CheckData &ckd, LavaObjectPtr object) {
  register unsigned short fwdCnt, revCnt;
  LavaObjectPtr callPtr; /*sectionPtr, newStackFrame[SFH+1]*/;
  CVFuncDesc *fDesc;
  LavaVariablePtr newStackFrame;
  int frameSizeBytes, frameSize=SFH+1;
  bool ok;

  object = object - (*object)->sectionOffset;
  fwdCnt = *(((unsigned short *)object)-1);
  if (!fwdCnt)
    return true; // recursive invocation, stop recursion

  if (fwdCnt > 1) {
    *(((unsigned short *)object)-1) = --fwdCnt;
    return true;
  }

////////////////////////////////////////////////////////////////
// finalize:


  fDesc = &(*(object + object[0][object[0][0].nSections-1].sectionOffset))->funcDesc[1]; 
	  // call the object's finalize method
  callPtr = object + (*object)[fDesc->delta].sectionOffset;
#ifdef __GNUC__
      newStackFrame = new LavaObjectPtr[SFH+1];
#else
      frameSizeBytes = frameSize<<2;
      __asm {
        sub esp, frameSizeBytes
        mov newStackFrame, esp
      }
#endif
  newStackFrame[0] = 0;
  newStackFrame[1] = 0;
  newStackFrame[2] = 0;
  newStackFrame[SFH] = callPtr;
  if (fDesc->isNative) 
    ok = (*fDesc->funcPtr)(ckd, newStackFrame);
  else 
    ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ckd, newStackFrame);

#ifdef __GNUC__
	delete [] newStackFrame;
#else
  __asm {
     add esp, frameSizeBytes
  }

#endif       
  if (!ok) {
    ckd.document->LavaError(ckd, true, object[0]->classDECL, &ERR_RunTimeException,0);
    ((SynFlags*)(object + 1))->INCL(zombified);
    return false;
  }
////////////////////////////////////////////////////////////////
// delete object:

  fwdCnt = *(((unsigned short *)object)-1);
  *(((unsigned short *)object)-1) = --fwdCnt;
  revCnt = *(((unsigned short *)object)-2);

  if (!fwdCnt && !revCnt) { // both counts are 0
    ((CLavaBaseDoc*)ckd.document)->numAllocObjects--;
#ifdef ALLOCOBJLIST
    ((CLavaBaseDoc*)ckd.document)->allocatedObjects.removeAt(((CLavaBaseDoc*)ckd.document)->allocatedObjects.indexOf(object));
#endif
    delete [] (object-LOH);
    return true;
  }

  ((SynFlags*)(object+1))->INCL(releaseFinished);
  // indicator for "release-recursion finished"; see DEC_REV_CNT

  return true;
}


bool DEC_REV_CNT (CheckData &ckd, LavaObjectPtr object) {
  register unsigned short fwdCnt;
  register unsigned short revCnt;

  object = object - (*object)->sectionOffset;
  fwdCnt = *(((unsigned short *)object)-1);
  revCnt = *(((unsigned short *)object)-2);
  if (!revCnt)
    return false; // better: fail exc.

  *(((unsigned short *)object)-2) = --revCnt;
  if (!fwdCnt && !revCnt && ((SynFlags*)(object+1))->Contains(releaseFinished)) {
    ((CLavaBaseDoc*)ckd.document)->numAllocObjects--;
#ifdef ALLOCOBJLIST
    ((CLavaBaseDoc*)ckd.document)->allocatedObjects.removeAt(((CLavaBaseDoc*)ckd.document)->allocatedObjects.indexOf(object));
#endif
    delete [] (object-LOH);
  }
  return true;
}


//void forceDRC (CheckData &ckd, LavaObjectPtr object) {
//  register unsigned short fwdCnt;
//  register unsigned short revCnt;
//
//  object = object - (*object)->sectionOffset;
//  revCnt = *(((unsigned short *)object)-2);
//  if (!revCnt)
//    return;
//
//  *(((unsigned short *)object)-2) = --revCnt;
//}


int GetObjectLength(LavaDECL* typeDECL)
{
  int lObject = 0;
  int nsect = ((CSectionDesc*)typeDECL->SectionTabPtr)[0].nSections;
  for (int ii = 0; ii < nsect; ii++)
    if (((CSectionDesc*)typeDECL->SectionTabPtr)[ii].SectionFlags.Contains(SectPrimary))
      lObject = lObject + ((CSectionDesc*)typeDECL->SectionTabPtr)[ii].classDECL->SectionInfo2 + LSH;
  return lObject;
}


bool forceZombify (CheckData &ckd, LavaObjectPtr object, bool aquaintancesToo) {
  CSectionDesc* secTab;
  LavaObjectPtr sectionPtr, callPtr, newStackFrame[SFH+1], member;
  LavaDECL *classDECL, *secClassDECL, *attrDECL;
  TAdapterFunc *funcAdapter;
  long int ii, lmem, llast;
  CVFuncDesc *fDesc;

  object = object - (*object)->sectionOffset;

  if (!((SynFlags*)(object + 1))->Contains(zombified)) {
    ((SynFlags*)(object + 1))->INCL(zombified);

	// call the object's finalize method before its member objects are decremented
    fDesc = &(*(object + object[0][object[0][0].nSections-1].sectionOffset))->funcDesc[1]; 
    callPtr = object + (*object)[fDesc->delta].sectionOffset;
    newStackFrame[0] = 0;
    newStackFrame[1] = 0;
    newStackFrame[2] = 0;
    newStackFrame[SFH] = callPtr;
    if (fDesc->isNative) {
      if (!(*fDesc->funcPtr)(ckd, newStackFrame) ) {
        ckd.document->LavaError(ckd, true, object[0]->classDECL, &ERR_RunTimeException,0);
        ((SynFlags*)(object + 1))->INCL(zombified);
        return false;
      }
    }
    else 
      if (!fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ckd, newStackFrame)) {
        ckd.document->LavaError(ckd, true, object[0]->classDECL, &ERR_RunTimeException,0);
        ((SynFlags*)(object + 1))->INCL(zombified);
        return false;
      }
    
    //zombify and finalize members
    classDECL = (*object)->classDECL;
 
    secTab = (CSectionDesc*)classDECL->SectionTabPtr;
    for (ii = 0; ii < secTab->nSections; ii++) {
      if (secTab[ii].SectionFlags.Contains(SectPrimary)) {
        secClassDECL = secTab[ii].classDECL;
        sectionPtr = object + secTab[ii].sectionOffset;
        lmem = LSH;
        if (secClassDECL->TypeFlags.Contains(isNative)) { //native base class
          funcAdapter = GetAdapterTable(ckd, secClassDECL, classDECL);
          if (!*(((unsigned short *)object)-1)
          && !*(((unsigned short *)object)-2)
          && funcAdapter
          && funcAdapter[5]) {
          // section/base class has release function; call it
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = sectionPtr;
            funcAdapter[5](ckd, newStackFrame);
          }
          if (funcAdapter)
            lmem = (long int)funcAdapter[0] + LSH;
        }
        llast = LSH + secClassDECL->SectionInfo2;
        for (; lmem < llast; lmem++) {
          attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
          member = (LavaObjectPtr)sectionPtr[lmem];
          if (member) {
            if (attrDECL->TypeFlags.Contains(constituent)) {
              forceZombify(ckd,member,aquaintancesToo);
              DFC(member);
            }
            else if (attrDECL->TypeFlags.Contains(acquaintance)) {
              if (aquaintancesToo) {
                forceZombify(ckd,member,aquaintancesToo);
                DFC(member);
              }
            }
            else // reverse link
              DRC(member);
          }
        }
      }
    }
  }

  return true;
}



typedef TAdapterFunc* (*TS) ();


TAdapterFunc* GetAdapterTable(CheckData &ckd, LavaDECL* classDECL, LavaDECL* specDECL)
{
  if (classDECL->fromBType != NonBasic) 
    return StdAdapterTab[classDECL->fromBType];
  else if (((CSectionDesc*)classDECL->SectionTabPtr)->adapterTab)
    return ((CSectionDesc*)classDECL->SectionTabPtr)->adapterTab;
  else if (specDECL && (specDECL->DeclType == CompObjSpec) 
  &&  classDECL->TypeFlags.Contains(isComponent)) {
#ifdef WIN32
    QString lib = QString (((CHEEnumSelId*)specDECL->Items.first)->data.Id.c);
#else
    QString lib = QString("lib")+QString (((CHEEnumSelId*)specDECL->Items.first)->data.Id.c);
#endif
    lib = ExeDir + "/Components/" + lib;
    TS adapt = (TS)QLibrary::resolve(lib, classDECL->LitStr.c);
//    qDebug() << "lib=" << lib << "LitStr=" << classDECL->LitStr.c << "adapt=" << adapt;
    ((CSectionDesc*)classDECL->SectionTabPtr)->adapterTab =  adapt();
    return ((CSectionDesc*)classDECL->SectionTabPtr)->adapterTab;
  }
  return 0;
}

LavaObjectPtr AllocateObject(CheckData &ckd, LavaDECL* classDECL, bool stateObjectCat, LavaObjectPtr urlObj)
{
  int ii;
  LavaObjectPtr object, sectionPtr, newStackFrame[SFH+1];
  TAdapterFunc *funcAdapter;
  SynObjectBase *mySelfVar=ckd.selfVar;
  
  if (!classDECL->WorkFlags.Contains(runTimeOK)) {
    ckd.document->CheckImpl(ckd, classDECL, classDECL);
    if (ckd.exceptionThrown) {
      ckd.selfVar = mySelfVar;
      return 0;
    }
  }
  int lObject = LOH + GetObjectLength(classDECL);
  object = new CSectionDesc* [lObject];
  if (!object) {
    if (!ckd.exceptionThrown)
      ckd.document->LavaError(ckd, true, classDECL, &ERR_AllocObjectFailed, 0);
    ckd.selfVar = mySelfVar;
    return 0;
  }

  for (ii = 0; ii < lObject; ii++)
    *(object + ii) = 0;
  object = object + LOH;
#ifdef ALLOCOBJLIST
  ckd.document->allocatedObjects.append(object);
#endif

  if (urlObj) {
    if (!*(object-LOH)) *(LavaVariablePtr)(object-LOH) = (LavaObjectPtr)new RunTimeData;
    ((RunTimeData*)*(object-LOH))->urlObj = urlObj;
  }
  object[0] = &((CSectionDesc*)classDECL->SectionTabPtr)[0];
  int nsect = ((CSectionDesc*)classDECL->SectionTabPtr)[0].nSections;
  for (ii = nsect-1; ii >= 0; ii--) {
    if (((CSectionDesc*)classDECL->SectionTabPtr)[ii].SectionFlags.Contains(SectPrimary)) {
      sectionPtr = object - (object[0])[0].sectionOffset + (object[0])[ii].sectionOffset;
      if  (!((SynFlags*)(sectionPtr+1))->Contains(sectEstablished)) {
        sectionPtr[0] = &((CSectionDesc*)classDECL->SectionTabPtr)[ii];
        if (sectionPtr[0]->classDECL->TypeFlags.Contains(isNative)) { //native base class
          funcAdapter = GetAdapterTable(ckd, sectionPtr[0]->classDECL, classDECL);
          if (funcAdapter && funcAdapter[4]) {  //section has new function
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = sectionPtr;
            funcAdapter[4](ckd, newStackFrame);
          }
        }
        if (stateObjectCat)
          ((SynFlags*)(sectionPtr+1))->INCL(stateObjFlag);
        else
          ((SynFlags*)(sectionPtr+1))->EXCL(stateObjFlag);
        ((SynFlags*)(sectionPtr+1))->INCL(sectEstablished);
      }
    }
  }
  (*(((unsigned short *)(object - (*object)->sectionOffset))-1))++;
  ((CLavaBaseDoc*)ckd.document)->numAllocObjects++;
  ckd.selfVar = mySelfVar;
  return object;
}


bool CallDefaultInit(CheckData &ckd, LavaObjectPtr object)
{
  CVFuncDesc *fDesc;
  LavaVariablePtr newStackFrame;
  bool ok=true;
  int fsize, fsizeBytes;
  unsigned myNativeStackDepth = currentStackDepth;
  DebugStep oldDebugStep=nextDebugStep;

  if (!object)
    return true;

  fDesc = &object[0]->funcDesc[0];
  fsize = fDesc->stackFrameSize;
  if (fDesc && fsize) {
#ifndef __GNUC__
    fsizeBytes = fsize<<2;
    __asm {
      sub esp, fsizeBytes
      mov newStackFrame, esp
    }
#else
		newStackFrame = new LavaObjectPtr[fsize];
#endif
    newStackFrame[0] = ckd.stackFrame[0];
    newStackFrame[1] = ckd.stackFrame[1];
    newStackFrame[2] = (LavaObjectPtr)((long unsigned)ckd.stackFrame[2] & ~2);
    newStackFrame[SFH] = object;

    if (nextDebugStep == nextStm || nextDebugStep == nextFunc)
      nextDebugStep = noStep;
    if (fDesc->isNative)
      ok = (*fDesc->funcPtr)(ckd, newStackFrame);
    else
      ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ckd, newStackFrame);
    
    if (!((long unsigned)newStackFrame[2] & 2))
      nextDebugStep = oldDebugStep;
#ifndef __GNUC__
    __asm {
      add esp, fsizeBytes
    }
#else
	  delete [] newStackFrame;
#endif
    currentStackDepth = myNativeStackDepth;
    if (!ok)
      ckd.document->LavaError(ckd, true, object[0]->classDECL, &ERR_RunTimeException,0);
  }
  return ok;
}

void ToggleObjectCat(LavaObjectPtr object)
{
  LavaObjectPtr sectionPtr;
  bool stateObjectCat = !((SynFlags*)(object+1))->Contains(stateObjFlag);
  for (int ii = 0; ii < object[0][0].nSections; ii++) {
    sectionPtr = object - (object[0])[0].sectionOffset + (object[0])[ii].sectionOffset;
    if (stateObjectCat)
      ((SynFlags*)(sectionPtr+1))->INCL(stateObjFlag);
    else
      ((SynFlags*)(sectionPtr+1))->EXCL(stateObjFlag);
  }
}

LavaObjectPtr CreateObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObjectCat)
{
  int secn;
  LavaObjectPtr object;

  switch ((TCompoProt)specDECL->nOutput) {
  case PROT_LAVA:
    ckd.document->CheckCompObj(specDECL->RuntimeDECL);
    object = AllocateObject(ckd, specDECL->RuntimeDECL->RelatedDECL, stateObjectCat, urlObj);
    if (!object)
      return 0;
    if (object)
      CallDefaultInit(ckd, object);
    if (ckd.exceptionThrown)
      return 0;

    //((SynFlags*)(object+1))->INCL(finished);

    secn = ckd.document->GetSectionNumber(ckd, specDECL->RuntimeDECL->RelatedDECL, classDECL);
    return (LavaObjectPtr)(object - (*object)[0].sectionOffset + (*object)[secn].sectionOffset);

  case PROT_NATIVE: ; 
    object = AllocateObject(ckd, specDECL, stateObjectCat, urlObj);
    if (!object)
      return 0;
    if (ckd.exceptionThrown)
      return 0;

    //((SynFlags*)(object+1))->INCL(finished);

    secn = ckd.document->GetSectionNumber(ckd, specDECL, classDECL);
    object = (LavaObjectPtr)(object - (*object)[0].sectionOffset + (*object)[secn].sectionOffset);
    ((SynFlags*)(object+1))->INCL(compoPrim);
    return object;

  default:
    ckd.document->LavaError(ckd, true, specDECL, &ERR_NotYetImplemented, 0);
    return 0;
  }
}


LavaObjectPtr AttachLavaObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObjectCat)
{
  int secn;
  LavaObjectPtr object;
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();
  COpenObjectParms oop(&ckd,urlObj,currentThread);

  switch ((TCompoProt)specDECL->nOutput) {
  case PROT_LAVA:
//    object = ckd.document->OpenObject(ckd, urlObj);
		QApplication::postEvent(ckd.document,new CustomEvent(UEV_OpenObject,&oop));
    currentThread->suspend();
		object = oop.obj;
		
    if (!object)
      return 0;
    if (!ckd.document->CheckCompObj(specDECL->RuntimeDECL)) {
      ckd.document->LavaError(ckd, true, specDECL, &ERR_CompoNotFound,0);
      return 0;
    }
    if ((*object)->classDECL != specDECL->RuntimeDECL->RelatedDECL) {
      ckd.document->LavaError(ckd, true, specDECL, &ERR_ldocIncompatible,0);
      return 0;
    }
    if (stateObjectCat != ((SynFlags*)(object+1))->Contains(stateObjFlag)) {
      ckd.document->LavaError(ckd, true, specDECL, &ERR_IncompatibleCategory, 0);
      return 0;
    }
    //((SynFlags*)(object+1))->INCL(finished);
    secn = ckd.document->GetSectionNumber(ckd, specDECL->RuntimeDECL->RelatedDECL, classDECL);
    return (LavaObjectPtr)(object - (*object)[0].sectionOffset + (*object)[secn].sectionOffset);

  case PROT_NATIVE:
    return CreateObject(ckd, urlObj, specDECL, classDECL, stateObjectCat);

  default:
    ckd.document->LavaError(ckd, true, specDECL, &ERR_NotYetImplemented, 0);
  }
  return 0;
}



CCopied::CCopied(LavaObjectPtr sc, LavaObjectPtr rs)
{
  sourceObjPtr = sc - sc[0]->sectionOffset;
  resultObjPtr = rs - rs[0]->sectionOffset;
  next = 0;
}

void CCopied::Add(LavaObjectPtr sc, LavaObjectPtr rs)
{
  CCopied* cc = new CCopied(sc, rs);
  last->next = cc;
  last = cc;
}

LavaObjectPtr CCopied::Find(LavaObjectPtr sc) 
{
  CCopied* cc;
  LavaObjectPtr scc = sc - sc[0]->sectionOffset;
  for (cc = this; cc && (cc->sourceObjPtr != scc); cc = cc->next);
  if (cc)
    return cc->resultObjPtr + sc[0]->sectionOffset;
  else
    return 0;
}

void CCopied::Destroy() 
{
  CCopied *cn, *cc = this;
  while (cc) {
    cn = cc;
    cc = cc->next;
    delete cn;
  }
}


CRuntimeException* CopyObject(CheckData &ckd, LavaVariablePtr sourceVarPtr, LavaVariablePtr resultVarPtr, bool stateObjectCat, LavaDECL* resultClassDECL, CCopied* copied)
{
  LavaObjectPtr sourceSectionPtr, resultSectionPtr, sourceObjPtr, resultObjPtr, cr;
  LavaDECL *classDECL, *secClassDECL, *attrDECL, *sourceClassDECL, *resultMemberClass;
  LavaVariablePtr resultAttr, newStackFrame;
  CSectionDesc* sourceSecTab;
  TAdapterFunc *funcAdapter;
  //CVFuncDesc *fDesc;
  CRuntimeException *ex;
  CContext context;
  long int ii, sii, ll, lmem, llast;
  //unsigned fsizeBytes;
  bool constit, secCopy=false, fullCopy, stateO, copyStart = false, isNew = false;

  sourceObjPtr = *sourceVarPtr;
  resultObjPtr = *resultVarPtr;
  if (resultObjPtr)
    resultClassDECL = resultObjPtr[0][0].classDECL;
  if (!sourceObjPtr)
    if (!resultObjPtr)
      return 0;
    else
      return new CRuntimeException(RunTimeException_ex, &ERR_CopyNullToNonNull);
  sourceObjPtr = sourceObjPtr - (sourceObjPtr[0])[0].sectionOffset;
  sourceClassDECL = sourceObjPtr[0][0].classDECL;
  if (resultClassDECL) {
    classDECL = resultClassDECL;
    fullCopy = (resultClassDECL == sourceClassDECL);
    if (!fullCopy) 
      sourceSecTab = sourceObjPtr[0];
  }
  else {
    fullCopy = true;
    classDECL = sourceObjPtr[0]->classDECL;
  }
  if (!resultObjPtr) {
    resultObjPtr = AllocateObject(ckd, classDECL, stateObjectCat);
    isNew = true;
  }
  if (!resultObjPtr)
    return new CRuntimeException(memory_ex ,&ERR_AllocObjectFailed);
  context = ckd.tempCtx;
  ckd.document->NextContext(sourceClassDECL, ckd.tempCtx);
  for (ii = 0; ii < resultObjPtr[0]->nSections; ii++) {
    if (((CSectionDesc*)classDECL->SectionTabPtr)[ii].SectionFlags.Contains(SectPrimary)) {
      secClassDECL = ((CSectionDesc*)classDECL->SectionTabPtr)[ii].classDECL;
      if (fullCopy) 
        sii = ii;
      else {
        for (sii = 0; (sii < sourceSecTab->nSections) && (sourceSecTab[sii].classDECL != secClassDECL); sii++);
        secCopy = sii <  sourceSecTab->nSections;
      }
      if (isNew && !secCopy && (ii == 0)) {
        CallDefaultInit(ckd, resultObjPtr);
        if (ckd.exceptionThrown)
          return 0;
      }
      if (fullCopy || secCopy) {
        sourceSectionPtr = sourceObjPtr - (sourceObjPtr[0])[0].sectionOffset + (sourceObjPtr[0])[sii].sectionOffset;
        resultSectionPtr = resultObjPtr - (resultObjPtr[0])[0].sectionOffset + (resultObjPtr[0])[ii].sectionOffset;
        if (!copied) {
          copyStart = true;
          copied = new CCopied(sourceSectionPtr, resultSectionPtr);
          copied->last = copied;
        }
        lmem = LSH;
        if (/*(*resultSectionPtr)->classDECL*/secClassDECL->TypeFlags.Contains(isNative)) { //native base class
          funcAdapter = GetAdapterTable(ckd, secClassDECL,0);
          if (funcAdapter[1]) { //section has copy function
#ifndef __GNUC__
            __asm {
              sub esp, 20
              mov newStackFrame, esp
            }
#else
						newStackFrame = new LavaObjectPtr[SFH+2];
#endif
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = sourceSectionPtr;
            newStackFrame[SFH+1] = resultSectionPtr;
            funcAdapter[1](ckd, newStackFrame);
#ifndef __GNUC__
            __asm {
              add esp, 20
              mov newStackFrame, esp
            }
#else
						delete [] newStackFrame;
#endif
          }
          else {        
            llast = (long int)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
            for (ll = LSH; ll < llast; ll++)
              *(resultSectionPtr + ll) = *(sourceSectionPtr + ll);
          }
          if (funcAdapter)
            lmem = (long int)funcAdapter[0] + LSH;
        }
        //else { //(##########)
          llast = secClassDECL->SectionInfo2 + LSH;
          for (/*ll = LSH*/; lmem < llast; lmem++) {
            attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
            constit = attrDECL->TypeFlags.Contains(constituent);
            stateO = attrDECL->TypeFlags.Contains(isStateObjectY);
            resultAttr = (LavaVariablePtr)(resultSectionPtr + lmem);
            if (*(sourceSectionPtr + lmem) && constit) {
              cr = copied->Find(*((LavaVariablePtr)(sourceSectionPtr + lmem)));
              if (cr) {
                if (*resultAttr)
                  DFC( *resultAttr);
                *resultAttr = cr;
                IFC( cr);
              }
              else {
                if (*resultAttr) {
                  DFC( *resultAttr);
                  *resultAttr = 0;
                }
                if (resultClassDECL) {
                  ckd.document->MemberTypeContext(attrDECL, ckd.tempCtx,0);
                  resultMemberClass = ckd.document->GetFinalMTypeAndContext(attrDECL->RefID, attrDECL->inINCL, ckd.tempCtx, 0);
                  ex = CopyObject(ckd, (LavaVariablePtr)(sourceSectionPtr + lmem), resultAttr, stateO, resultMemberClass, copied);
                  ckd.tempCtx = context;                  
                  if (ex)
                    return ex;
                  else
                    if (ckd.exceptionThrown)
                      return 0;
                }
                else {
                  ex = CopyObject(ckd, (LavaVariablePtr)(sourceSectionPtr + lmem), resultAttr, stateO, 0, copied);
                  if (ex)
                    return ex;
                  else
                    if (ckd.exceptionThrown)
                      return 0;
                }
                copied->Add(*((LavaVariablePtr)(sourceSectionPtr + lmem)), *resultAttr);
              }
            }
            else
              if (attrDECL->TypeFlags.Contains(acquaintance)) {
                if (*resultAttr) 
                  DFC( *resultAttr);
                *resultAttr = *(LavaVariablePtr)(sourceSectionPtr + lmem);
                if (*resultAttr) 
                  IFC( *resultAttr);
              }
              else {
                if (*resultAttr) 
                  DRC( *resultAttr);
                if (*(sourceSectionPtr + lmem)) {
                  cr = copied->Find(*((LavaVariablePtr)(sourceSectionPtr + lmem)));
                  if (cr) 
                    *resultAttr = cr;
                  else
                    *resultAttr = *(LavaVariablePtr)(sourceSectionPtr + lmem);
                  if (*resultAttr) 
                    IRC(*resultAttr);
                }
                else
                  *resultAttr = 0;
              }
          }
        //}
      }
//      else
//        if (!ii && !(*resultVarPtr)) {
//          //call default initializer if the result object has been allocated here
//          fDesc = &(*resultObjPtr)[0].funcDesc[0];
//          if (fDesc && fDesc->stackFrameSize) {
//            if (fDesc->isNative) {
//              fsize = fDesc->stackFrameSize;
//#ifndef __GNUC__
//              fsizeBytes = fsize<<2;
//              __asm {
//                sub esp, fsizeBytes
//                mov newStackFrame, esp
//              }
//#else
//							newStackFrame = new LavaObjectPtr[fsize];
//#endif
//              newStackFrame[0] = 0;
//              newStackFrame[1] = 0;
//              newStackFrame[2] = 0;
//              newStackFrame[SFH] = resultObjPtr;
//              //TRY_FUNCCALL(ckd, (*fDesc->funcPtr), newStackFrame, (fsize), ok)
//              ok = (*fDesc->funcPtr)(ckd, newStackFrame);
//#ifndef __GNUC__
//              __asm {
//                add esp, fsizeBytes
//                mov newStackFrame, esp
//              }
//#else
//							delete [] newStackFrame;
//#endif
//            }
//            else 
//              ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ckd, &resultObjPtr);
//            if (!ok && !ckd.exceptionThrown) {
//              ckd.document->LavaError(ckd, true, secClassDECL, &ERR_RunTimeException,0);
//              return 0;
//            }
//          }
//        }
    }
  }
  if (copyStart)
    copied->Destroy();
  if (!*resultVarPtr)
    if (resultClassDECL)
      *resultVarPtr = resultObjPtr;
    else
      *resultVarPtr = resultObjPtr + ((*sourceVarPtr)[0])[0].sectionOffset;
  return 0;
}


bool EqualObjects(CheckData &ckd, LavaObjectPtr leftPtr, LavaObjectPtr rightPtr, int specialEQ)
// specialEQ: 0: normal ( operator "="), 1: same as (compare like state object), 2:equals (compare like value object)
{
  LavaObjectPtr leftObjPtr, leftSectionPtr, rightObjPtr, rightSectionPtr;
  LavaVariablePtr newStackFrame;
  LavaDECL *classDECL, *secClassDECL, *attrDECL;
  TAdapterFunc *funcAdapter;
  long int ii, ll, lmem, llast;
  bool equ;
  CSectionDesc* secTab;

  leftObjPtr = leftPtr;
  rightObjPtr = rightPtr;
  if (leftObjPtr == rightObjPtr)
    return true;
  if (!leftObjPtr || !rightObjPtr)
    return false;
  leftObjPtr = leftObjPtr - (*leftObjPtr)->sectionOffset;
  rightObjPtr = rightObjPtr - (*rightObjPtr)->sectionOffset;
  if ((specialEQ != 2) && (((SynFlags*)(leftObjPtr+1))->Contains(stateObjFlag)
                           || ((SynFlags*)(rightObjPtr+1))->Contains(stateObjFlag))
      || (specialEQ == 1))
    return (rightObjPtr == leftObjPtr);
  classDECL = (*leftObjPtr)->classDECL;
  if (classDECL != (*rightObjPtr)->classDECL)
    return false;
 
  secTab = (CSectionDesc*)classDECL->SectionTabPtr;
  for (ii = 0; ii < secTab->nSections; ii++) {
    if (secTab[ii].SectionFlags.Contains(SectPrimary)) {
      secClassDECL = secTab[ii].classDECL;
      leftSectionPtr = leftObjPtr + secTab[ii].sectionOffset;
      rightSectionPtr = rightObjPtr + secTab[ii].sectionOffset;
      lmem = LSH;
      if (/*(*rightSectionPtr)->classDECL*/secClassDECL->TypeFlags.Contains(isNative)) { //native base class
        funcAdapter = GetAdapterTable(ckd, secClassDECL, classDECL);
        if (funcAdapter) {
          if (funcAdapter[2]) { //section has compare function
#ifndef __GNUC__
            __asm {
              sub esp, 20
              mov newStackFrame, esp
            }
  #else
					  newStackFrame = new LavaObjectPtr[SFH+2];
  #endif
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = leftSectionPtr;
            newStackFrame[SFH+1] = rightSectionPtr;
            equ = funcAdapter[2](ckd, newStackFrame);
#ifndef __GNUC__
            __asm {
              add esp, 20
              mov newStackFrame, esp
            }
  #else
					  delete [] newStackFrame;
  #endif
            if (!equ)
              return false;
          }
          else {        
            llast = (long int)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
            for (ll = LSH; ll < llast; ll++)
              if (rightSectionPtr[ll] != leftSectionPtr[ll])
                return false;
          }
          lmem = LSH + (long int)funcAdapter[0];
        }//has funcadapter
      }
     // else { //(##########)
        llast = secClassDECL->SectionInfo2 + LSH;
        for (/*ll = LSH*/; lmem < llast; lmem++) {
          if (*(rightSectionPtr + lmem) != *(leftSectionPtr + lmem)) {
            attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
            if (attrDECL->TypeFlags.Contains(constituent)) {
              if (!EqualObjects(ckd, *(LavaVariablePtr)(leftSectionPtr + lmem), *(LavaVariablePtr)(rightSectionPtr + lmem), specialEQ))
                return false;
            }
            else
              if (*(rightSectionPtr + lmem) != *(leftSectionPtr + lmem))
                return false;
          }
        }
      //}
    }
  }
  return true;
}

bool OneLevelCopy(CheckData &ckd, LavaObjectPtr& object)
{
  int castVal = object[0][0].sectionOffset;

  LavaObjectPtr sourceSectionPtr, resultSectionPtr, sourceObjPtr, resultObjPtr;
  LavaDECL *secClassDECL, *attrDECL, *classDECL;
  long int ii, ll, lmem, llast;
  TAdapterFunc *funcAdapter;

  if (!object)
    return true;
  sourceObjPtr = object - castVal;
  classDECL = sourceObjPtr[0][0].classDECL;
  resultObjPtr = AllocateObject(ckd, classDECL, ((SynFlags*)(sourceObjPtr+1))->Contains(stateObjFlag));
  if (!resultObjPtr)
    return false;
  for (ii = 0; ii < resultObjPtr[0]->nSections; ii++) {
    if (((CSectionDesc*)classDECL->SectionTabPtr)[ii].SectionFlags.Contains(SectPrimary)) {
      sourceSectionPtr = sourceObjPtr - (sourceObjPtr[0])[0].sectionOffset + (sourceObjPtr[0])[ii].sectionOffset;
      resultSectionPtr = resultObjPtr - (resultObjPtr[0])[0].sectionOffset + (resultObjPtr[0])[ii].sectionOffset;
      secClassDECL = sourceSectionPtr[0][0].classDECL;
      lmem = LSH;
      if (secClassDECL->TypeFlags.Contains(isNative)) { //native base class
        funcAdapter = GetAdapterTable(ckd, secClassDECL,0);
        if (secClassDECL->fromBType == B_Set) 
          HSetOneLevelCopy(ckd, sourceSectionPtr, resultSectionPtr); //new chain with same element objects
        else if (secClassDECL->fromBType == B_Array) 
          HArrayOneLevelCopy(ckd, sourceSectionPtr, resultSectionPtr); //new array with same element objects
        else {
          if (funcAdapter) {
            llast = (long int)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
            for (ll = LSH; ll < llast; ll++)
              *(resultSectionPtr + ll) = *(sourceSectionPtr + ll);
          }
        }
        if (funcAdapter)
          lmem = (long int)funcAdapter[0] + LSH;
      }
      //else { //(##########)
        llast = secClassDECL->SectionInfo2 + LSH;
        for (/*ll = LSH*/; lmem < llast; lmem++) {
          attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
          *(LavaVariablePtr)(resultSectionPtr + lmem) = *(LavaVariablePtr)(sourceSectionPtr + lmem);
          if (*(LavaVariablePtr)(resultSectionPtr + lmem))
            if (attrDECL->TypeFlags.Contains(acquaintance) || attrDECL->TypeFlags.Contains(constituent))
              INC_FWD_CNT(ckd, *(LavaVariablePtr)(resultSectionPtr + lmem));
            else
              INC_REV_CNT(ckd, *(LavaVariablePtr)(resultSectionPtr + lmem));
        }
      //}
    }
  }
//  DEC_FWD_CNT(ckd, object);
  object = resultObjPtr + castVal;
  return true;
}

bool UpdateObject(CheckData &ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& objModf)
//
// Function returns true if a value object has updates
// 
{
  LavaObjectPtr origSectionPtr, updateObj, updateSectionPtr, newObject;
  LavaVariablePtr newStackFrame;
  LavaDECL *classDECL, *secClassDECL, *attrDECL;
  TAdapterFunc *funcAdapter;
  long int ii, ll, lmem, llast;
  CSectionDesc* secTab;
  bool isNew = false, equ;

  updateObj = *updatePtr;
  if (origObj == updateObj)
    return false;
  if (!origObj && updateObj) {
    origObj = *updatePtr;
    *updatePtr = 0;
    return true;
  }
  if (origObj && !updateObj) {
    DEC_FWD_CNT(ckd, origObj);
    origObj = 0;
    return true;
  }
  classDECL = (*origObj)->classDECL;
  secTab = (CSectionDesc*)classDECL->SectionTabPtr;
  for (ii = 0; ii < secTab->nSections; ii++) {
    if (secTab[ii].SectionFlags.Contains(SectPrimary)) {
      secClassDECL = secTab[ii].classDECL;
      origSectionPtr = origObj + secTab[ii].sectionOffset;
      updateSectionPtr = updateObj + secTab[ii].sectionOffset;
      lmem = LSH;
      if (secClassDECL->TypeFlags.Contains(isNative)) { //native base class
        funcAdapter = GetAdapterTable(ckd, secClassDECL, classDECL);
        if (secClassDECL->fromBType == B_Set) {
          HSetUpdate(ckd, origSectionPtr, &updateSectionPtr, isNew, objModf);
          origObj = origSectionPtr - secTab[ii].sectionOffset; //may be new object
        }
        else if (secClassDECL->fromBType == B_Array) {
          HArrayUpdate(ckd, origSectionPtr, &updateSectionPtr, isNew, objModf);
          origObj = origSectionPtr - secTab[ii].sectionOffset; //may be new object
        }
        else {
          if (funcAdapter && funcAdapter[2]) { //section has compare function
#ifndef __GNUC__
            __asm {
              sub esp, 20
              mov newStackFrame, esp
            }
#else
						newStackFrame = new LavaObjectPtr[SFH+2];
#endif
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = origSectionPtr;
            newStackFrame[SFH+1] = updateSectionPtr;
            equ = funcAdapter[2](ckd, newStackFrame);
#ifndef __GNUC__
            __asm {
              add esp, 20
              mov newStackFrame, esp
            }
#else
						delete [] newStackFrame;
#endif
          }
          else {
            equ = true;
            llast = (long int)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
            for (ll = LSH; equ && (ll < llast); ll++)
              equ = updateSectionPtr[ll] == origSectionPtr[ll];
          }
          if (!equ) {
            objModf = true;
            if (isNew) { // || ((SynFlags*)(origObj+1))->Contains(stateObjFlag)) {
              if (secClassDECL->fromBType == VLString)
                HStringCopy(origSectionPtr, updateSectionPtr);
              else {
                llast = (long int)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
                for (ll = LSH; ll < llast; ll++) 
                  *(origSectionPtr + ll) = *(updateSectionPtr + ll);
              }
            }
            else {
              origObj = *updatePtr;
              *updatePtr = 0;
              return true;
            }
          }
        }//not set and not array
        if (funcAdapter)
          lmem = (long int)funcAdapter[0] + LSH;
      }//native
      //else { //(##########)
        llast = secClassDECL->SectionInfo2 + LSH;
        for (/*ll = LSH*/; lmem < llast; lmem++) {
          attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
          if (attrDECL->TypeFlags.Contains(constituent)) {
            newObject = *(LavaVariablePtr)(origSectionPtr + lmem);
            if (UpdateObject(ckd, newObject, (LavaVariablePtr)(updateSectionPtr + lmem), objModf)) {
              //if (!isNew && !((SynFlags*)(origObj+1))->Contains(stateObjFlag))  {
              //  isNew = true;
              //  if (!OneLevelCopy(ckd, origObj))
              //    return false;
              //}
              if (*(LavaVariablePtr)(origObj + secTab[ii].sectionOffset + lmem))
                DEC_FWD_CNT(ckd, *(LavaVariablePtr)(origObj + secTab[ii].sectionOffset + lmem));
              *(LavaVariablePtr)(origObj + secTab[ii].sectionOffset + lmem) = newObject;
            }
            if (ckd.exceptionThrown)
              return false;
          }
        }//all members in section
      //}//not native
    }//primary section
  }//all sections in object
  return isNew;
}

LavaObjectPtr CastTo(LavaDECL* classDECL, LavaObjectPtr obj)
{
  int ii;
  LavaObjectPtr objC;
  objC = obj - obj[0]->sectionOffset;
  for (ii = 0; (ii < objC[0][0].nSections) && (objC[0][ii].classDECL != classDECL); ii++);
  if (ii < objC[0][0].nSections) 
    return objC + objC[0][ii].sectionOffset;
  else
    return 0;
}

LavaObjectPtr CastEnumType(CheckData& ckd, LavaObjectPtr eTypeObjPtr)
{
  int ii;
  LavaObjectPtr obj = eTypeObjPtr - eTypeObjPtr[0]->sectionOffset;
  for (ii = 0; (ii < obj[0][0].nSections) && (obj[0][ii].classDECL != ckd.document->DECLTab[Enumeration]); ii++);
  if (ii < obj[0][0].nSections)
    return obj + obj[0][ii].sectionOffset;
  else
    ckd.document->LavaError(ckd, true, obj[0][0].classDECL, &ERR_NotBaseIF, ckd.document->DECLTab[Enumeration]);
  return 0;
}

LavaObjectPtr CastChainType(CheckData& ckd, LavaObjectPtr chainTypeObjPtr/*, LavaDECL* chainTypeDECL*/)
{
  int ii;
  LavaObjectPtr obj = chainTypeObjPtr - chainTypeObjPtr[0]->sectionOffset;
  for (ii = 0; (ii < obj[0][0].nSections) && (obj[0][ii].classDECL != ckd.document->DECLTab[B_Chain]); ii++);
  if (ii < obj[0][0].nSections)
    return obj + obj[0][ii].sectionOffset;
  else
    ckd.document->LavaError(ckd, true, obj[0][0].classDECL, &ERR_NotBaseIF, ckd.document->DECLTab[B_Chain]);
  return 0;
}

LavaObjectPtr CastSetType(CheckData& ckd, LavaObjectPtr setTypeObjPtr)
{
  int ii;
  LavaObjectPtr obj = setTypeObjPtr - setTypeObjPtr[0]->sectionOffset;
  for (ii = 0; (ii < obj[0][0].nSections) && (obj[0][ii].classDECL != ckd.document->DECLTab[B_Set]); ii++);
  if (ii < obj[0]->nSections)
    return obj + obj[0][ii].sectionOffset;
  else
    ckd.document->LavaError(ckd, true, obj[0][0].classDECL, &ERR_NotBaseIF, ckd.document->DECLTab[B_Set]);
  return 0;
}

LavaObjectPtr CastArrayType(CheckData& ckd, LavaObjectPtr arrayTypeObjPtr)
{
  int ii;
  LavaObjectPtr obj = arrayTypeObjPtr - arrayTypeObjPtr[0]->sectionOffset;
  for (ii = 0; (ii < obj[0][0].nSections) && (obj[0][ii].classDECL != ckd.document->DECLTab[B_Array]); ii++);
  if (ii < obj[0]->nSections)
    return obj + obj[0][ii].sectionOffset;
  else
    ckd.document->LavaError(ckd, true, obj[0][0].classDECL, &ERR_NotBaseIF, ckd.document->DECLTab[B_Array]);
  return 0;
}

#ifdef WIN32
CHWException::CHWException(unsigned n)
{
  codeHW = n;
  switch (codeHW) {
  case STATUS_ACCESS_VIOLATION:
    message = QString("Hardware exception: Reading from or writing to an inaccessible memory location");
    lavaCode = seg_violation_ex;
    break;    
  case STATUS_FLOAT_DIVIDE_BY_ZERO:
    message = QString("Hardware exception: Floating-point division by 0.0");
    lavaCode = float_div_by_zero_ex;
    break; 
  case STATUS_FLOAT_OVERFLOW:
    message = QString("Hardware exception: Exceeding maximum positive exponent of floating-point type");
    lavaCode = float_overflow_ex;
    break;
  case STATUS_FLOAT_UNDERFLOW:
    message = QString("Hardware exception: Exceeding magnitude of lowest negative exponent of floating-point type");
    lavaCode = float_underflow_ex;
    break;
  case STATUS_FLOAT_INEXACT_RESULT:
    message = QString("Hardware exception: Floating-point operation yields inexact result");
    lavaCode = float_inexact_result_ex;
    break;
  case STATUS_INTEGER_DIVIDE_BY_ZERO:
    message = QString("Hardware exception: Integer division by 0");
    lavaCode = integer_div_by_zero_ex;
    break;
  case STATUS_INTEGER_OVERFLOW:
    message = QString("Hardware exception: Integer overflow");
    lavaCode = integer_overflow_ex;
    break;/*
  case STATUS_BREAKPOINT:
    message = QString("Hardware exception: Encountering a hardware-defined breakpoint; used only by debuggers");
    lavaCode = debug_ex;
    break;*/
  case STATUS_ILLEGAL_INSTRUCTION:
    message = QString("Hardware exception: Attempting to execute an instruction code not defined by the processor");
    lavaCode = other_hardware_ex;
    break; 
/*  case STATUS_PRIVILEGED_INSTRUCTION:
    message = QString("Hardware exception: Executing an instruction not allowed in current machine mode");
    lavaCode = other_hardware_ex;
    break;*/
  case STATUS_DATATYPE_MISALIGNMENT:
    message = QString("Hardware exception: Reading or writing to data at an address that is not properly aligned; for example, 16-bit entities must be aligned on 2-byte boundaries.");
    lavaCode = other_hardware_ex;
    break;
/*  case STATUS_SINGLE_STEP:
    message = QString("Hardware exception: Executing one instruction in single-step mode; used only by debuggers");
    lavaCode = other_hardware_ex;
    break;*/
  default: ;
    lavaCode = other_hardware_ex;
    message = QString("Unknown hardware exception; infinite recursion??");
  }
}

#else
#ifndef __SunOS
#define STRSIGNAL (const char *)sys_siglist[codeHW]
#else
#define STRSIGNAL strsignal(codeHW)
#endif

CHWException::CHWException(int sig_num, siginfo_t *info)
{
  codeHW = sig_num;
  switch (sig_num) {
  case SIGSEGV:
    lavaCode = seg_violation_ex;
    message = QString("Hardware exception: Segmentation violation");
    break;
  case SIGFPE:
    switch (info->si_code) {
    case FPE_FLTDIV:
      message = QString("Hardware exception: Floating-point division by 0.0");
      lavaCode = float_div_by_zero_ex;
      break; 
    case FPE_FLTOVF:
      message = QString("Hardware exception: Exceeding maximum positive exponent of floating-point type");
      lavaCode = float_overflow_ex;
      break;
    case FPE_FLTUND:
      message = QString("Hardware exception: Exceeding magnitude of lowest negative exponent of floating-point type");
      lavaCode = float_underflow_ex;
      break;
/*    case FPE_FLTRES:
      message = QString("Hardware exception: Inexact result");
      lavaCode = float_inexact_result_ex;
      break;*/
    case FPE_FLTINV:
      message = QString("Hardware exception: Invalid operation");
      lavaCode = float_invalid_op_ex;
      break;
//#ifndef __Darwin
    case FPE_FLTSUB:
      message = QString("Hardware exception: Subscript out of range");
      lavaCode = float_subscript_out_of_range_ex;
      break;
    case FPE_INTDIV:
      message = QString("Hardware exception: Integer division by 0");
      lavaCode = integer_div_by_zero_ex;
      break;
    case FPE_INTOVF:
      message = QString("Hardware exception: Integer overflow");
      lavaCode = integer_overflow_ex;
      break;
//#endif
    default:
      lavaCode = other_hardware_ex;
      message = QString(STRSIGNAL);
    }
    break;
  default: ;
    lavaCode = other_hardware_ex;
    message = QString(STRSIGNAL);
  }
}
#endif

bool CHWException::SetLavaException(CheckData& ckd)
{
  LavaObjectPtr strObj, codeObj;
  int ii;

  ckd.lastException = AllocateObject(ckd, ckd.document->DECLTab[B_HWException], false);
  if (!ckd.lastException)
    return false;
  //((SynFlags*)(ckd.lastException+1))->INCL(finished);
  for (ii = 0; (ii < ckd.lastException[0][0].nSections) && (ckd.lastException[0][ii].classDECL != ckd.document->DECLTab[B_Exception]); ii++);
  ckd.lastException = ckd.lastException + ckd.lastException[0][ii].sectionOffset;
  codeObj = AllocateObject(ckd, (LavaDECL*)((CHE*)ckd.document->DECLTab[B_HWException]->NestedDecls.first->successor)->data, false);
  strObj = AllocateObject(ckd, ckd.document->DECLTab[VLString], false);
  if (!ckd.lastException || !strObj || !codeObj) {
    ckd.exceptionThrown = true;
    return false;
  }
  *(LavaVariablePtr)(ckd.lastException + LSH) = codeObj;
  *(LavaVariablePtr)(ckd.lastException + LSH + 1) = strObj;
  NewQString((QString*)strObj+LSH, message.toAscii());
  ckd.exceptionThrown = true;
  if (HEnumSetVal(ckd, codeObj, lavaCode))
    return true;
  else
    return false;
}

CFPException::CFPException(bool isQuietNaN) {
  if (isQuietNaN) {
    message = QString("Hardware exception: \"Not-a-number(quietNaN)\" in floating-point operation");
    lavaCode = float_quietNAN_ex;
  }
  else {
    message = QString("Hardware exception: Exceeding maximum positive exponent of floating-point type;\nmaybe division by zero");
    lavaCode = float_overflow_ex;
  }
}


CRuntimeException::CRuntimeException(int co, QString *err_code)
{
  code = co;
  message = *err_code;
}


bool CRuntimeException::SetLavaException(CheckData& ckd)
{
  LavaObjectPtr strObj, codeObj;
  int ii;

  ckd.lastException = AllocateObject(ckd, ckd.document->DECLTab[B_RTException], false);
  if (!ckd.lastException)
    return false;
  //((SynFlags*)(ckd.lastException+1))->INCL(finished);
  for (ii = 0; (ii < ckd.lastException[0][0].nSections) && (ckd.lastException[0][ii].classDECL != ckd.document->DECLTab[B_Exception]); ii++);
  ckd.lastException = ckd.lastException + ckd.lastException[0][ii].sectionOffset;
  strObj = AllocateObject(ckd, ckd.document->DECLTab[VLString], false);
  codeObj = AllocateObject(ckd, (LavaDECL*)((CHE*)ckd.document->DECLTab[B_RTException]->NestedDecls.first->successor)->data, false);
  if (!ckd.lastException || !strObj || !codeObj) {
    ckd.exceptionThrown = true;
    return false;
  }
  *(LavaVariablePtr)(ckd.lastException + LSH + 1) = strObj;
  *(LavaVariablePtr)(ckd.lastException + LSH) = codeObj;
  NewQString((QString*)strObj+LSH, message.toAscii());
  ckd.exceptionThrown = true;
  if (HEnumSetVal(ckd, codeObj, code))
    return true;
  else
    return false;
}

bool SetLavaException(CheckData& ckd, int code, const QString& mess)
{
  LavaObjectPtr ex, strObj, codeObj;

  ex = AllocateObject(ckd, ckd.document->DECLTab[B_RTException], false);
  strObj = AllocateObject(ckd, ckd.document->DECLTab[VLString], false);
  codeObj = AllocateObject(ckd, (LavaDECL*)((CHE*)ckd.document->DECLTab[B_RTException]->NestedDecls.first->successor)->data, false);
  if (!ex || !strObj || !codeObj) {
    ckd.exceptionMsg = ERR_SetLavaExceptionFailed;
    ckd.exceptionThrown = true;
    return false;
  }
  //((SynFlags*)(ex+1))->INCL(finished);
  int ii;
  for (ii = 0; (ii < ex[0][0].nSections) && (ex[0][ii].classDECL != ckd.document->DECLTab[B_Exception]); ii++);
  ex = ex + ex[0][ii].sectionOffset;
  ckd.lastException = ex;
  *(LavaVariablePtr)(ckd.lastException + LSH) = codeObj;
  *(LavaVariablePtr)(ckd.lastException + LSH + 1) = strObj;
  NewQString((QString*)strObj+LSH, mess.toAscii());
  if (HEnumSetVal(ckd, codeObj, code)) {
    ckd.exceptionMsg = mess;
    ckd.exceptionThrown = true;
    return true;
  }
  else {
    ckd.exceptionMsg = ERR_SetLavaExceptionFailed;
    ckd.exceptionThrown = true;
    return false;
  }
}
