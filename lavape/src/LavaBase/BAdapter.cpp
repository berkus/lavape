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

#include "BAdapter.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "DumpView.h"

#include "SafeInt.h"
#include "qobject.h"
#include "qstring.h"
#include "qmessagebox.h"
//Added by qt3to4:
#include <float.h>
#include <stdio.h>
#include <limits>
#include <math.h>

#pragma hdrstop

using namespace std;

#undef new
#define new1(ADDR) new(ADDR)


#define OBJALLOC(RESULT, CKD, DECL) {\
  RESULT = AllocateObject(CKD, DECL);\
  if (!RESULT)\
    if (!CKD.exceptionThrown)\
      throw CRuntimeException(memory_ex ,&ERR_AllocObjectFailed);\
    else\
      return false;\
}

#define ONELEVELCOPY(CKD, OBJ) {\
  if (!OneLevelCopy(CKD, OBJ)) \
    if (!CKD.exceptionThrown)\
      throw CRuntimeException(memory_ex ,&ERR_AllocObjectFailed);\
    else\
      return false;\
}
	//((SynFlags*)(RESULT+1))->INCL(finished); \

#define TEST_AND_THROW \
  if (!qIsFinite(result)) throw CFPException(false); \
  else if (qIsNaN(result)) throw CFPException(true);


void NewQString(QString* pstr, const char* str)
{
  new1(pstr) QString(str);
}

TAdapterFunc* StdAdapterTab [Identifier];

//LavaDECL* DECLTab [Identifier];

static TAdapterFunc ObjectAdapter[LAH + 7];
static TAdapterFunc BitsetAdapter[LAH + 7];
static TAdapterFunc BoolAdapter[LAH + 5];
static TAdapterFunc CharAdapter[LAH + 1];
static TAdapterFunc IntAdapter[LAH + 10];
static TAdapterFunc FloatAdapter[LAH + 9];
static TAdapterFunc DoubleAdapter[LAH + 9];
static TAdapterFunc StringAdapter[LAH + 4];
static TAdapterFunc EnumAdapter[LAH +4];
static TAdapterFunc SetAdapter[LAH + 6];
static TAdapterFunc ChainAdapter[LAH + 7];
static TAdapterFunc CheAdapter[LAH ];
static TAdapterFunc ArrayAdapter[LAH + 4];
static TAdapterFunc ExceptionAdapter[LAH + 3];
static TAdapterFunc HW_L_ExceptionAdapter[LAH ];
static TAdapterFunc LinkAdapter[LAH ];
/*static*/ TAdapterFunc GUIAdapter[LAH + 3];

//static TAdapterFunc ServiceAdapter[LAH ];

bool DefaultEq(CheckData& /*ckd*/, LavaVariablePtr /*stack*/)
{
  return true;
}


bool ObjectFinalize(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object, sectionPtr, newStackFrame[SFH+1];
  CSectionDesc* secTab;
  LavaDECL *classDECL, *secClassDECL, *attrDECL;
  TAdapterFunc *funcAdapter;
  int ii, lmem, llast;

  object = stack[SFH] - stack[SFH][0][0].sectionOffset;
  if (*(object - LOH) && ((RunTimeData*)*(object-LOH))->urlObj && ((SynFlags*)(object+1))->Contains(objectModified))  //url object?
    if (object[0][0].implDECL != object[0][0].classDECL) { //not native i.e. Lava component
      if ( !((SynFlags*)(object + 1))->Contains(dontSave))
        ckd.document->SaveObject(ckd, stack[SFH]);
    }
  if (!((SynFlags*)(object + 1))->Contains(zombified)) {// DEC members:
    ((SynFlags*)(object + 1))->INCL(zombified);

    classDECL = (*object)->classDECL;
    secTab = (CSectionDesc*)classDECL->SectionTabPtr;
    for (ii = 0; ii < secTab->nSections; ii++) {
      if (secTab[ii].SectionFlags.Contains(SectPrimary)) {
        secClassDECL = secTab[ii].classDECL;
        sectionPtr = object + secTab[ii].sectionOffset;
        lmem = LSH;
        if (secClassDECL->TypeFlags.Contains(isNative)) { //native base class
          funcAdapter = GetAdapterTable(ckd, secClassDECL, classDECL);
          if (funcAdapter && funcAdapter[5]) { // section has release function
            newStackFrame[2] = 0;
            newStackFrame[SFH] = sectionPtr;
            funcAdapter[5](ckd, newStackFrame);
          }
          if (funcAdapter)
            lmem = (int)(unsigned)funcAdapter[0] + LSH;
        }
        llast = LSH + secClassDECL->SectionInfo2;
        for (/*ll = LSH*/; lmem < llast; lmem++) {
          attrDECL = ((CSectionDesc*)secClassDECL->SectionTabPtr)[0].attrDesc[lmem-LSH].attrDECL;
          if (sectionPtr[lmem])
            if (attrDECL->TypeFlags.Contains(constituent)
            || attrDECL->TypeFlags.Contains(acquaintance)) {
              DFC((LavaObjectPtr)sectionPtr[lmem]);
            }
            else
              DRC((LavaObjectPtr)sectionPtr[lmem]);
        }
      }
    }
  }
  RunTimeData* runTimeData = (RunTimeData*)*(object-LOH);
  if (runTimeData)
    delete runTimeData;
  *(object-LOH) = 0;
  return true;
}


bool ObjectZombifyRec(CheckData& ckd, LavaVariablePtr stack)
{
  //(*(((unsigned short *)stack[SFH])-1))++;
  forceZombify(ckd,stack[SFH],*(bool*)(stack[SFH+1]+LSH));
  return true;
}

//general operator ==
bool ObjectOpEqual(CheckData& ckd, LavaVariablePtr stack)
{
  return EqualObjects(ckd, stack[SFH], stack[SFH+1], 0);
}

//general operator !=
bool ObjectOpNotEqual(CheckData& ckd, LavaVariablePtr stack)
{
  return !EqualObjects(ckd, stack[SFH], stack[SFH+1], 0);
}

//bool ObjectSameAs(CheckData& ckd, LavaVariablePtr stack)
//{
//  return EqualObjects(ckd, stack[SFH], stack[SFH+1], 1);
//}

bool ObjectEquals(CheckData& ckd, LavaVariablePtr stack)
{
  return EqualObjects(ckd, stack[SFH], stack[SFH+1], 2);
}

//bool ObjectSnapshot(CheckData& ckd, LavaVariablePtr stack)
//{
//  CRuntimeException* ex = CopyObject(ckd, &stack[SFH], &stack[SFH+1]);
//  if (ex)
//    throw *ex;
//  if (ckd.exceptionThrown)
//    return false;
//  return true;
//}

bool ObjectDontSave(CheckData& ckd, LavaVariablePtr stack)
{
  if (*(bool*)(stack[SFH+1]+LSH))
    ((SynFlags*)(stack[SFH] - stack[SFH][0][0].sectionOffset + 1))->INCL(dontSave);
  else
    ((SynFlags*)(stack[SFH] - stack[SFH][0][0].sectionOffset + 1))->EXCL(dontSave);
  return true;
}

bool ObjectDump(CheckData& ckd, LavaVariablePtr stack)
{
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();
  DumpEventData* data = new DumpEventData(ckd.document, stack, currentThread);
  currentThread->waitingForUI = true;
  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaDump,(void*)data));
  currentThread->suspend();
  ckd.document->DumpFrame = 0;
  return true;
}

//Bitset adapter functions

bool BsetEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(unsigned*)(stack[SFH]+LSH) == *(unsigned*)(stack[SFH+1]+LSH);
}


bool BsetLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTunsigned(*(unsigned*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETunsigned(*(unsigned*)(stack[SFH]+LSH));
  return true;
}

bool BsetBwAnd(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = *(unsigned*)(stack[SFH]+LSH) & *(unsigned*)(stack[SFH+1]+LSH);
  return true;
}

bool BsetBwInclOr(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = *(unsigned*)(stack[SFH]+LSH) | *(unsigned*)(stack[SFH+1]+LSH);
  return true;
}

bool BsetBwExclOr(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = *(unsigned*)(stack[SFH]+LSH) ^ *(unsigned*)(stack[SFH+1]+LSH);
  return true;
}

bool BsetOnesComplement(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = ~*(unsigned*)(stack[SFH]+LSH);
  return true;
}

bool BsetLShift(CheckData& ckd, LavaVariablePtr stack)
{
  register unsigned ui = *(unsigned*)(stack[SFH]+LSH) << *(unsigned*)(stack[SFH+1]+LSH);
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = ui;
  return true;
}

bool BsetRShift(CheckData& ckd, LavaVariablePtr stack)
{
  register unsigned ui = *(unsigned*)(stack[SFH]+LSH) >> *(unsigned*)(stack[SFH+1]+LSH);
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Bitset])
  *(unsigned*)(stack[SFH+2]+LSH) = ui;
  return true;
}


bool BsetString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  QString str = QString("%1").arg(*(ulong*)(stack[SFH]+LSH), sizeof(unsigned) * 8,2);
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}

bool BoolEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(bool*)(stack[SFH]+LSH) == *(bool*)(stack[SFH+1]+LSH);
}


bool BoolLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTboolean(*(bool*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETboolean(*(bool*)(stack[SFH]+LSH));
  return true;
}

bool BoolAnd(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[B_Bool])
  *(bool*)(stack[SFH+2]+LSH) = *(bool*)(stack[SFH]+LSH) && *(bool*)(stack[SFH+1]+LSH);
  return true;
}

bool BoolInclOr(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[B_Bool])
  *(bool*)(stack[SFH+2]+LSH) = *(bool*)(stack[SFH]+LSH) || *(bool*)(stack[SFH+1]+LSH);
  return true;
}

bool BoolExclOr(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[B_Bool])
  *(bool*)(stack[SFH+2]+LSH) = *(bool*)(stack[SFH]+LSH) && !*(bool*)(stack[SFH+1]+LSH)
                             || !*(bool*)(stack[SFH]+LSH) && *(bool*)(stack[SFH+1]+LSH);
  return true;
}

bool BoolNot(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[B_Bool])
  *(bool*)(stack[SFH+1]+LSH) = !*(bool*)(stack[SFH]+LSH);
  return true;
}

bool BoolString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  QString str;
  if (*(bool*)(stack[SFH]+LSH))
    str = QString("true");
  else
    str = QString("false");
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}


bool CharEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(char*)(stack[SFH]+LSH) == *(char*)(stack[SFH+1]+LSH);
}


bool CharLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTchar(*(char*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETchar(*(char*)(stack[SFH]+LSH));
  return true;
}

bool CharString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  QString str;
  str = *(char*)(stack[SFH]+LSH);
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}

//Integer adapter functions

bool IntEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(int*)(stack[SFH]+LSH) == *(int*)(stack[SFH+1]+LSH);
}


bool IntLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTint(*(int*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETint(*(int*)(stack[SFH]+LSH));
  return true;
}

bool IntPercent(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+2]+LSH) = (SafeInt<int>(*(int*)(stack[SFH]+LSH)) % SafeInt<int>(*(int*)(stack[SFH+1]+LSH))).Value();
  return true;
}

bool IntLT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return SafeInt<int>(*(int*)(stack[SFH]+LSH)) < SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
}

bool IntGT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return SafeInt<int>(*(int*)(stack[SFH]+LSH)) > SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
}

bool IntLET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return SafeInt<int>(*(int*)(stack[SFH]+LSH)) <= SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
}

bool IntGET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(int*)(stack[SFH]+LSH) >= *(int*)(stack[SFH+1]+LSH);
}

bool IntPlus(CheckData& ckd, LavaVariablePtr stack)
{
  SafeInt<int> ii =  SafeInt<int>(*(int*)(stack[SFH]+LSH)) + SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+2]+LSH) =  ii.Value();
  return true;
}

//bool IntIncBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  SafeInt<int> ii =  SafeInt<int>(*(int*)(stack[SFH]+LSH)) + SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
////  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer], false)
//  *(int*)(stack[SFH]+LSH) =  ii.Value();
//  return true;
//}

bool IntMinus(CheckData& ckd, LavaVariablePtr stack)
{
  SafeInt<int> ii = - SafeInt<int>(*(int*)(stack[SFH]+LSH));
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+1]+LSH) =  ii.Value();
  return true;
}

bool IntMulti(CheckData& ckd, LavaVariablePtr stack)
{
  SafeInt<int> ii = SafeInt<int>(*(int*)(stack[SFH]+LSH)) * SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+2]+LSH) =  ii.Value();
  return true;
}

//bool IntMultBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  SafeInt<int> ii = SafeInt<int>(*(int*)(stack[SFH]+LSH)) * SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
////  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer], false)
//  *(int*)(stack[SFH]+LSH) =  ii.Value();
//  return true;
//}

bool IntDiv(CheckData& ckd, LavaVariablePtr stack)
{
  SafeInt<int> ii = SafeInt<int>(*(int*)(stack[SFH]+LSH)) / SafeInt<int>(*(int*)(stack[SFH+1]+LSH));
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+2]+LSH) =  ii.Value();
  return true;
}


bool IntString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
//  register char* str = new char [33];
  QString str = QString("%1").arg(*(int*)(stack[SFH]+LSH));
//  _itoa(*(int*)(stack[SFH]+LSH), str, 10) ;
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}



//Float adapter functions

bool FloatEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(float*)(stack[SFH]+LSH) == *(float*)(stack[SFH+1]+LSH);
}

bool FloatLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTfloat(*(float*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETfloat(*(float*)(stack[SFH]+LSH));
  return true;
}

bool FloatLT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(float*)(stack[SFH]+LSH) < *(float*)(stack[SFH+1]+LSH);
}

bool FloatGT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(float*)(stack[SFH]+LSH) > *(float*)(stack[SFH+1]+LSH);
}

bool FloatLET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(float*)(stack[SFH]+LSH) <= *(float*)(stack[SFH+1]+LSH);
}

bool FloatGET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(float*)(stack[SFH]+LSH) >= *(float*)(stack[SFH+1]+LSH);
}

bool FloatPlus(CheckData& ckd, LavaVariablePtr stack)
{
  register float result = *(float*)(stack[SFH]+LSH) + *(float*)(stack[SFH+1]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Float])
  *(float*)(stack[SFH+2]+LSH) = result;
  return true;
}

//bool FloatIncBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  register float result = *(float*)(stack[SFH]+LSH) + *(float*)(stack[SFH+1]+LSH);
//  TEST_AND_THROW
//  *(float*)(stack[SFH]+LSH) = result;
//  return true;
//}

bool FloatMinus(CheckData& ckd, LavaVariablePtr stack)
{
  register float result = - *(float*)(stack[SFH]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Float])
  *(float*)(stack[SFH+1]+LSH) = result;
  return true;
}

bool FloatMulti(CheckData& ckd, LavaVariablePtr stack)
{
  register float result = *(float*)(stack[SFH]+LSH) * (*(float*)(stack[SFH+1]+LSH));
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Float])
  *(float*)(stack[SFH+2]+LSH) = result;
  return true;
}

//bool FloatMultBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  register float result = *(float*)(stack[SFH]+LSH) * (*(float*)(stack[SFH+1]+LSH));
//  TEST_AND_THROW
//  *(float*)(stack[SFH]+LSH) = result;
//  return true;
//}

bool FloatDiv(CheckData& ckd, LavaVariablePtr stack)
{
  register float result = *(float*)(stack[SFH]+LSH) / *(float*)(stack[SFH+1]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Float])
  *(float*)(stack[SFH+2]+LSH) = result;
  return true;
}

bool FloatString(CheckData& ckd, LavaVariablePtr stack)
{
//  char buf[200];

  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  QString str = QString::number(*(float*)(stack[SFH]+LSH));
//  sprintf(buf,"%g",*(float*)(stack[SFH]+LSH));//  QString str = QString(buf);
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}

//Double adapter functions

bool DoubleEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(double*)(stack[SFH]+LSH) == *(double*)(stack[SFH+1]+LSH);
}


bool DoubleLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTdouble(*(double*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETdouble(*(double*)(stack[SFH]+LSH));
  return true;
}

bool DoubleLT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(double*)(stack[SFH]+LSH) < *(double*)(stack[SFH+1]+LSH);
}

bool DoubleGT(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(double*)(stack[SFH]+LSH) > *(double*)(stack[SFH+1]+LSH);
}

bool DoubleLET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(double*)(stack[SFH]+LSH) <= *(double*)(stack[SFH+1]+LSH);
}

bool DoubleGET(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(double*)(stack[SFH]+LSH) >= *(double*)(stack[SFH+1]+LSH);
}

bool DoublePlus(CheckData& ckd, LavaVariablePtr stack)
{
  register double result =  *(double*)(stack[SFH]+LSH) + *(double*)(stack[SFH+1]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Double])
  *(double*)(stack[SFH+2]+LSH) = result;
  return true;
}

//bool DoubleIncBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  register double result =  *(double*)(stack[SFH]+LSH) + *(double*)(stack[SFH+1]+LSH);
//  TEST_AND_THROW
//  *(double*)(stack[SFH]+LSH) = result;
//  return true;
//}

bool DoubleMinus(CheckData& ckd, LavaVariablePtr stack)
{
  register double result = - *(double*)(stack[SFH]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Double])
  *(double*)(stack[SFH+1]+LSH) = result;
  return true;
}

bool DoubleMulti(CheckData& ckd, LavaVariablePtr stack)
{
  double result = *(double*)(stack[SFH]+LSH) * (*(double*)(stack[SFH+1]+LSH));
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Double])
  *(double*)(stack[SFH+2]+LSH) = result;
  return true;
}

//bool DoubleMultBy(CheckData& ckd, LavaVariablePtr stack)
//{
//  register double result = *(double*)(stack[SFH]+LSH) * (*(double*)(stack[SFH+1]+LSH));
//  TEST_AND_THROW
//  *(double*)(stack[SFH]+LSH) = result;
//  return true;
//}

bool DoubleDiv(CheckData& ckd, LavaVariablePtr stack)
{
  register double result = *(double*)(stack[SFH]+LSH) / *(double*)(stack[SFH+1]+LSH);
  TEST_AND_THROW
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Double])
  *(double*)(stack[SFH+2]+LSH) = result;
  return true;
}

bool DoubleString(CheckData& ckd, LavaVariablePtr stack)
{
//  char buf[200];

  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  QString str = QString::number(*(double*)(stack[SFH]+LSH));
//  sprintf(buf,"%g",*(double*)(stack[SFH]+LSH));
//  QString str = QString(buf);
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}

// String adapter functions
bool StringNewFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  new1((stack[SFH])+LSH) QString();
  return true;
}

bool StringCopy(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  if ((QString*)(stack[SFH]+LSH))
    new1(stack[SFH+1]+LSH) QString(*(QString*)(stack[SFH]+LSH));
  return true;
}

bool HStringCopy(LavaObjectPtr s0, LavaObjectPtr s1)
{
  if ((QString*)(s1+LSH)) {
    new1(s0+LSH) QString(*(QString*)(s1+LSH));
    return true;
  }
  return false;
}

bool StringEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(QString*)(stack[SFH]+LSH) == *(QString*)(stack[SFH+1]+LSH);
}


bool StringLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTCString((QString*)(stack[SFH]+LSH));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETCString((QString*)(stack[SFH]+LSH));
  return true;
}


bool StringDecFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  ((QString*)((stack[SFH])+LSH))->QString::~QString();
  // destructor of embedded/explicitly placed QString must be called explicitly
  return true;
}

bool StringPlus(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[VLString])//, ((SynFlags*)(stack[SFH]+1))->Contains(stateObjFlag))
  if ((QString*)(stack[SFH]+LSH) && (QString*)(stack[SFH+1]+LSH))
    NewQString((QString*)stack[SFH+2]+LSH, (*(QString*)(stack[SFH]+LSH) + *(QString*)(stack[SFH+1]+LSH)).toAscii());
  else if ((QString*)(stack[SFH+1]+LSH))
    NewQString((QString*)stack[SFH+2]+LSH, ((QString*)(stack[SFH+1]+LSH))->toAscii());
  else if ((QString*)(stack[SFH]+LSH))
    NewQString((QString*)stack[SFH+2]+LSH, ((QString*)(stack[SFH]+LSH))->toAscii());
  return true;
}

bool StringBox(CheckData& ckd, LavaVariablePtr stack)
{
  QWidget *parent=wxTheApp->m_appWindow;

//  wxTheApp->m_appWindow->activateWindow();
  int rc=Qt::NoButton;
  if ((QString*)(stack[SFH]+LSH))
    rc = information(
      parent,qApp->applicationName(),*(QString*)(stack[SFH]+LSH),
      QMessageBox::Ok, QMessageBox::Cancel,
      0);

  if (rc == QMessageBox::Cancel)
		if (question(
			parent,qApp->applicationName(),ERR_AbortQuestion,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No,
      0) == QMessageBox::Yes) {
        CRuntimeException *ex;
        ex = new CRuntimeException(RunTimeException_ex, &ERR_ExecutionAborted);
        throw *ex;
    }
  return true;
}

bool StringQuestionBox(CheckData& ckd, LavaVariablePtr stack)
{
  QWidget *parent=wxTheApp->m_appWindow;

//  wxTheApp->m_appWindow->activateWindow();
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[B_Bool])
  int rc=Qt::NoButton;
  if ((QString*)(stack[SFH]+LSH)) {
		*(bool*)(stack[SFH+1]+LSH) = question(
			parent,qApp->applicationName(),*(QString*)(stack[SFH]+LSH),
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No,
			0) == QMessageBox::Yes;
      return true;
  }
  return false;
}



//Enumeration

bool HEnumSetVal(CheckData& ckd, LavaObjectPtr obj, unsigned num)
{
  TEnumDescription *enumDesc;
  CHEEnumSelId *enumSel;
  int ii, ee;
  unsigned nn;
  LavaObjectPtr totalObj = obj - obj[0][0].sectionOffset;
  for (ii = 0; (ii < totalObj[0][0].nSections) && (totalObj[0][ii].classDECL->DeclDescType != EnumType); ii++);
  for (ee = 0; (ee < totalObj[0][0].nSections) && (totalObj[0][ee].classDECL != ckd.document->DECLTab[Enumeration]); ee++);
  if (ii < totalObj[0][0].nSections) {
    enumDesc = (TEnumDescription*)totalObj[0][ii].classDECL->EnumDesc.ptr;
    enumSel = (CHEEnumSelId*)enumDesc->EnumField.Items.first;
    for (nn = 0; (nn < num) && enumSel; nn++)
      enumSel = (CHEEnumSelId*)enumSel->successor;
    if (enumSel) {
      *(unsigned*)(totalObj + totalObj[0][ee].sectionOffset + LSH) = num;
      new1(totalObj + totalObj[0][ee].sectionOffset + LSH + 1) QString(enumSel->data.Id.c);
      return true;
    }
  }
  return false;
}

bool EnumNewFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  TEnumDescription *enumDesc;
  int ii;
  LavaObjectPtr totalObj = stack[SFH] - stack[SFH][0][0].sectionOffset;
  for (ii = 0; (ii < totalObj[0][0].nSections) && (totalObj[0][ii].classDECL->DeclDescType != EnumType); ii++);
  if (ii < totalObj[0][0].nSections) {
    enumDesc = (TEnumDescription*)totalObj[0][ii].classDECL->EnumDesc.ptr;
    new1(stack[SFH]+LSH+1) QString(((CHEEnumSelId*)enumDesc->EnumField.Items.first)->data.Id.c);
  }
  else
    new1(stack[SFH]+LSH+1) QString("");
  return true;
}

bool EnumCopy(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  *(int*)(stack[SFH+1]+LSH) = *(int*)(stack[SFH]+LSH);
  if ((QString*)(stack[SFH]+LSH+1))
    new1(stack[SFH+1]+LSH+1) QString(*(QString*)(stack[SFH]+LSH+1));
  return true;
}


bool EnumEq(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return *(int*)(stack[SFH+1]+LSH) == *(int*)(stack[SFH]+LSH);
}


bool EnumLavaIO(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  //stack{0] is the object
  //(ASN1ToFromAr*)stack[SFH+1] the cid
  //remember: only the name string of the enumeration value will be stored,
  //          the integer value will be recalculated after loading
  if (((ASN1tofromAr*)stack[SFH+1])->Ar->device()->isWritable())
    ((ASN1tofromAr*)stack[SFH+1])->PUTCString((QString*)(stack[SFH]+LSH+1));
  else
    ((ASN1tofromAr*)stack[SFH+1])->GETCString((QString*)(stack[SFH]+LSH+1));
  return true;
}


bool EnumDecFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  ((QString*)(stack[SFH]+LSH+1))->QString::~QString();
  // destructor of embedded/explicitly placed QString must be called explicitly
  return true;
}

bool EnumOrdFunc(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+1]+LSH) = *(int*)(stack[SFH]+LSH);

  return true;
}

bool EnumStringFunc(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
  *(QString*)(stack[SFH+1]+LSH) = *(QString*)(stack[SFH]+LSH+1);
  return true;
}

bool EnumCommentFunc(CheckData& ckd, LavaVariablePtr stack)
{
  CHEEnumSelId *che;
  TEnumDescription* enumDesc;
  int ii, num = *(int*)(stack[SFH]+LSH);
  LavaObjectPtr totalObj = stack[SFH] - stack[SFH][0][0].sectionOffset;
  for (ii = 0; (ii < totalObj[0][0].nSections) && (totalObj[0][ii].classDECL->DeclDescType != EnumType); ii++);
  if (ii < totalObj[0][0].nSections) {
    ii = 0;
    enumDesc = (TEnumDescription*)totalObj[0][ii].classDECL->EnumDesc.ptr;
    for (che = (CHEEnumSelId*)enumDesc->EnumField.Items.first;
         che && (ii < num); che = (CHEEnumSelId*)che->successor)
      ii++;
    if (ii == num) {
      OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString])
      *(QString*)(stack[SFH+1]+LSH) = QString(che->data.DECLComment.ptr->Comment.c);
      return true;
    }
  }
  CRuntimeException* ex;
  if (num < 0)
    ex = new CRuntimeException(EnumOrdHigh_ex, &ERR_EnumOrdHigh);
    //SetLavaException(ckd, EnumOrdHigh_ex, &&ERR_EnumOrdHigh);
  else
    ex = new CRuntimeException(EnumOrdLow_ex, &ERR_EnumOrdLow);
    //SetLavaException(ckd, EnumOrdLow_ex, &&ERR_EnumOrdLow);
  throw *ex;
}

//Set
bool SetNewFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  new1((CHAINX*)(stack[SFH]+LSH)) CHAINX();
  return true;
}

bool SetCopy(CheckData& ckd, LavaVariablePtr stack)
{
  CHE* che;
  LavaObjectPtr handle;
  CHAINX *chainL, *chainR;
  CRuntimeException *ex;

  chainL = (CHAINX*)(stack[SFH]+LSH);
  if (chainL) {
    new1(stack[SFH+1]+LSH) CHAINX();
    chainR = (CHAINX*)(stack[SFH+1]+LSH);
    //if (((SynFlags*)stack[SFH]+1)->Contains(stateObjFlag))
    //  ((SynFlags*)stack[SFH+1]+1)->INCL(stateObjFlag);
    for (che = (CHE*)chainL->first; che; che = (CHE*)che->successor) {
      OBJALLOC(handle, ckd, ckd.document->DECLTab[B_Che])
      *(LavaVariablePtr)(handle+LSH) = stack[SFH+1]; //note set object in handle
      if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)) {
        if (((CHE*)(handle+LSH+1))->data)
          DFC((LavaObjectPtr)((CHE*)(handle+LSH+1))->data);
        ((CHE*)(handle+LSH+1))->data = 0;
        ex = CopyObject(ckd, (LavaVariablePtr)&che->data, (LavaVariablePtr)&((CHE*)(handle+LSH+1))->data);//, ((SynFlags*)(((LavaObjectPtr)che->data)+1))->Contains(stateObjFlag));
        if (ex)
          throw *ex;
        if (ckd.exceptionThrown)
          return false;
      }
      else {
        if ((*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance)) {
          if (((CHE*)(handle+LSH+1))->data) {
            DFC( (LavaObjectPtr)((CHE*)(handle+LSH+1))->data);
            IFC((LavaObjectPtr)che->data); // inc data
          }
        }
        else {
          if (((CHE*)(handle+LSH+1))->data) {
            DRC((LavaObjectPtr)((CHE*)(handle+LSH+1))->data);
            IRC((LavaObjectPtr)che->data); // inc data
          }
        }
        ((CHE*)(handle+LSH+1))->data = che->data;
      }
      chainR->Append((CHE*)(handle+LSH+1));
    }
  }
  return true;
}


bool SetEq(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *cheR, *cheL;
  CHAINX *chainL, *chainR;
  if (stack[SFH] == stack[SFH+1])
    return true;
  chainL = (CHAINX*)(stack[SFH]+LSH);
  chainR = (CHAINX*)(stack[SFH+1]+LSH);
  if (!chainL || !chainR)
    return false;
  if ((*stack[SFH])->SectionFlags.Contains(SectChainType)) {
    cheL = (CHE*)chainL->first;
    cheR = (CHE*)chainR->first;
    while (cheL && cheR) {
      if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)) {
        if (!EqualObjects(ckd, (LavaObjectPtr)cheL->data, (LavaObjectPtr)cheR->data, 0))
          return false;
      }
      else
        if (cheL->data != cheR->data)
          return false;
      cheL = (CHE*)cheL->successor;
      cheR = (CHE*)cheR->successor;
    }
    return !cheL && !cheR;
  }
  else {
    for (cheL = (CHE*)chainL->first; cheL; cheL = (CHE*)cheL->successor) {
      for (cheR = (CHE*)chainR->first;
           cheR
           && ((cheL->data != cheR->data) || ((SynFlags*)(cheR-LSH))->Contains(localCheckmark));
           cheR = (CHE*)cheR->successor);
      if (cheR)
        ((SynFlags*)(cheR-LSH))->INCL(localCheckmark);
      else
        return false;
    }
    for (cheR = (CHE*)chainR->first; cheR; cheR = (CHE*)cheR->successor)
      if (((SynFlags*)(cheR-LSH))->Contains(localCheckmark))
        ((SynFlags*)(cheR-LSH))->EXCL(localCheckmark);
      else
        return false;
    return true;
  }
}

bool SetDecFunc(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *che, *succ;
  bool forward = (*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
                 || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance);
  CHAINX *chain = (CHAINX*)(stack[SFH]+LSH);

  for (che = (CHE*)chain->first; che; che = succ) {
    succ = (CHE*)che->successor; // save successor since che may be deleted
    if (forward)
      if (che->data)
        DFC((LavaObjectPtr)che->data) // dec data
      else
        DRC((LavaObjectPtr)che->data) // dec data
    DFC(((LavaObjectPtr)che)-LSH-1);
  }
  return true;
}

bool SetDD(CheckData& ckd, LavaVariablePtr stack)
{
  DDMakeClass* dd = new DDSetClass;
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer])
  *(stack[SFH+1]+LSH) = (CSectionDesc*)dd;
  return true;
}


bool SetAdd(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr che;
  OBJALLOC(che, ckd, ckd.document->DECLTab[B_Che])
  *(LavaVariablePtr)(che+LSH) = stack[SFH];
  ((CHE*)(che+LSH+1))->data = (DObject*)stack[SFH+1];
  if (stack[SFH+1])
    if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
    || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+1]) // inc data
    else
      IRC(stack[SFH+1]) // inc data
  ((CHAINX*)(stack[SFH]+LSH))->Append((CHE*)(che+LSH+1));
  return true;
}

bool SetAddGetHandle(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[B_Che])
  IFC(stack[SFH+2]);
  *(LavaVariablePtr)(stack[SFH+2]+LSH) = stack[SFH];
  ((CHE*)(stack[SFH+2]+LSH+1))->data = (DObject*)stack[SFH+1];
  if (stack[SFH+1])
    if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
    || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+1]) // inc data
    else
      IRC(stack[SFH+1]) // inc data
  ((CHAINX*)(stack[SFH]+LSH))->Append((CHE*)(stack[SFH+2]+LSH+1));
  return true;
}


bool SetGet(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *che=(CHE*)(stack[SFH+1]+LSH+1);

  if (stack[SFH+1]) {
    if ((*(LavaVariablePtr)(stack[SFH+1]+LSH) == stack[SFH]) && che) {
      stack[SFH+2] = (LavaObjectPtr)((CHE*)(stack[SFH+1]+LSH+1))->data;
      if (stack[SFH+2])
        IFC(stack[SFH+2]);
    }
    else
      throw CRuntimeException(ElemNotInSet_ex,&ERR_ElemNotInSet);
  }
  else
    stack[SFH+2] = 0;
  return true;
}

bool SetRemove(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *che=(CHE*)(stack[SFH+1]+LSH+1);

  if (stack[SFH+1]) {
    if (*(LavaVariablePtr)(stack[SFH+1]+LSH) == stack[SFH] && che) {
      ((CHAINX*)(stack[SFH]+LSH))->Uncouple(che);
      *(LavaVariablePtr)(stack[SFH+1]+LSH) = 0;
      if (((CHE*)(stack[SFH+1]+LSH+1))->data)
        if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
        || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance))
          DFC((LavaObjectPtr)((CHE*)(stack[SFH+1]+LSH+1))->data) // dec data
        else
          DRC((LavaObjectPtr)((CHE*)(stack[SFH+1]+LSH+1))->data) // dec data
      DFC(stack[SFH+1]); // dec element handle
    }
    else
      throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
  }
  return true;
}


bool SetRemoveAndGet(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *che=(CHE*)(stack[SFH+1]+LSH+1);

  if (stack[SFH+1]) {
    if ((*(LavaVariablePtr)(stack[SFH+1]+LSH) == stack[SFH]) && che) {
      stack[SFH+2] = (LavaObjectPtr)che->data;
      ((CHAINX*)(stack[SFH]+LSH))->Uncouple(che);
      *(LavaVariablePtr)(stack[SFH+1]+LSH) = 0;
      if (stack[SFH+2] && !(*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
                   && !(*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance)) {
        IFC(stack[SFH+2]); // inc data
        DRC(stack[SFH+2]); // dec data
      }
      DFC(stack[SFH+1]); // dec element handle
    }
    else
      throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
  }
  else
    stack[SFH+2] = 0;
  return true;
}

bool SetCount(CheckData& ckd, LavaVariablePtr stack)
{
  CHE *che;
  int num=0;
  LavaObjectPtr objPtr = stack[SFH+1];

  if (stack[SFH] && (stack[SFH]+LSH)) {
    if (objPtr) {
      objPtr = objPtr - (*objPtr)->sectionOffset;
      for (che = (CHE*)((CHAINX*)(stack[SFH]+LSH))->first;
           che;
           che = (CHE*)che->successor)
        if (!EqualObjects(ckd,objPtr,(LavaObjectPtr)che->data,0))
          num++;
    }
    else
      for (che = (CHE*)((CHAINX*)(stack[SFH]+LSH))->first;
           che;
           che = (CHE*)che->successor)
        num++;
  }
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+2]+LSH) = num;
  return true;
}

//helper functions, no adapterfunc

bool HSetContains(LavaObjectPtr setObj, LavaObjectPtr elemObj)
{
  CHE *che;
  if (setObj+LSH) {
    for (che = (CHE*)((CHAINX*)(setObj+LSH))->first;
         che && ((LavaObjectPtr)che->data != elemObj);
         che = (CHE*)che->successor);
    return che != 0;
  }
  else
    return false;
}

CRuntimeException* HSetInsertBefore(CheckData& ckd, LavaVariablePtr stack)
{
  stack[SFH+3]  = AllocateObject(ckd, ckd.document->DECLTab[B_Che]);
  if (!stack[SFH+3])
    return new CRuntimeException(memory_ex, &ERR_AllocObjectFailed);
  CHAINX* chain = (CHAINX*)(stack[SFH]+LSH);
  *(LavaVariablePtr)(stack[SFH+3]+LSH) = stack[SFH];
  ((CHE*)(stack[SFH+3]+LSH+1))->data = (DObject*)stack[SFH+2];
  if (stack[SFH+1])
    if ((*(LavaVariablePtr)(stack[SFH+1]+LSH) == stack[SFH]) && (CHE*)(stack[SFH+1]+LSH+1))
      chain->Insert(((CHE*)(stack[SFH+1]+LSH+1))->predecessor, (CHE*)(stack[SFH+3]+LSH+1));
    else
      return new CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
  else
    chain->Append((CHE*)(stack[SFH+3]+LSH+1));
  if (stack[SFH+2])
    if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
    || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+2]) // inc data
    else
      IRC(stack[SFH+2]) // inc data
  return 0;
}


bool HSetFirst(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  CHAINX* chain = (CHAINX*)(stack[SFH] +LSH);
  if (chain->first)
    stack[SFH+1] = (LavaObjectPtr)(chain->first)-LSH-1;
  else
    stack[SFH+1] = 0;
  return true;
}


CRuntimeException* HSetSucc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  if (*(LavaVariablePtr)(stack[SFH+1]+LSH) == stack[SFH]) {
    if (stack[SFH+1] && (stack[SFH+1]+LSH+1) && ((CHE*)(stack[SFH+1]+LSH+1))->successor)
      stack[SFH+2] = (LavaObjectPtr)(((CHE*)(stack[SFH+1]+LSH+1))->successor)-LSH-1;
    else
      stack[SFH+2] = 0;
    return 0;
  }
  else
    return new CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
}

bool HSetOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr)
{
  LavaObjectPtr handle;
  CHAINX *resultChain, *sourceChain;
  CHE *sourceChe;

  if (sourceSectionPtr[0]->SectionFlags.Contains(ElemsConstituent)) {
    resultChain = (CHAINX*)(resultSectionPtr+LSH);
    sourceChain = (CHAINX*)(sourceSectionPtr+LSH);
    sourceChe = (CHE*)sourceChain->first;
    for (sourceChe = (CHE*)sourceChain->first; sourceChe; sourceChe = (CHE*)sourceChe->successor) {
      OBJALLOC(handle, ckd, ckd.document->DECLTab[B_Che])
      *(LavaVariablePtr)(handle+LSH) = resultSectionPtr; //note set object in handle
      ((CHE*)(handle+LSH+1))->data = sourceChe->data;
      if (sourceChe->data)
        INC_FWD_CNT(ckd, (LavaObjectPtr)sourceChe->data);
      resultChain->Append((CHE*)(handle+LSH+1));
    }
  }
  return true;
}

bool HSetUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew, bool& objModf)
{
  CHE *cheUpdate, *cheOrig, *el;
  LavaObjectPtr elOrig, elUpdate, handle;
  CHAINX *chainUpdate, *chainOrig;
  int iEl = 0, secOff;

  chainUpdate = (CHAINX*)((*updatePtr)+LSH);
  chainOrig = (CHAINX*)(origObj+LSH);
  cheOrig = (CHE*)chainOrig->first;
  cheUpdate = (CHE*)chainUpdate->first;
  while (cheUpdate) {
    if (((SynFlags*)(((LavaObjectPtr)cheUpdate)-LSH))->Contains(deletedItem)) {
      if (!((SynFlags*)(((LavaObjectPtr)cheUpdate->data)+1))->Contains(insertedItem)) {
        //if (!isNew && !((SynFlags*)(origObj+1))->Contains(stateObjFlag)) {
        //  ONELEVELCOPY(ckd, origObj)
        //  isNew = true;
        //  chainOrig = (CHAINX*)(origObj+LSH);
        //  cheOrig = (CHE*)chainOrig->first;
        //  for (ii = 0; ii<iEl; ii++)
        //    cheOrig = (CHE*)cheOrig->successor;
        //}
        el = cheOrig;
        cheOrig = (CHE*)cheOrig->successor;
        chainOrig->Uncouple(el);
        objModf = true;
        if (el->data)
          DFC((LavaObjectPtr)(el->data));
        DFC((LavaObjectPtr)el-LSH-1);
      }
      cheUpdate = (CHE*)cheUpdate->successor;
    }
    else {
      if (((SynFlags*)(((LavaObjectPtr)cheUpdate->data)+1))->Contains(insertedItem)) {
        ((SynFlags*)(((LavaObjectPtr)cheUpdate->data)+1))->EXCL(insertedItem);
        //if (!isNew && !((SynFlags*)(origObj+1))->Contains(stateObjFlag)) {
        //  ONELEVELCOPY(ckd, origObj);
        //  isNew = true;
        //  chainOrig = (CHAINX*)(origObj+LSH);
        //  if (!iEl)
        //    cheOrig = 0;
        //  else {
        //    cheOrig = (CHE*)chainOrig->first;
        //    for (ii = 0; ii<iEl-1; ii++)
        //      cheOrig = (CHE*)cheOrig->successor;
        //  }
        //}
        //else
        if (cheOrig)
          cheOrig = (CHE*)cheOrig->predecessor;
        else
          cheOrig = (CHE*)chainOrig->last;
        el = cheUpdate;
        cheUpdate = (CHE*)cheUpdate->successor;
        chainUpdate->Uncouple(el);
        handle = (LavaObjectPtr)el-LSH-1;
        *(LavaVariablePtr)(handle + LSH) = origObj; //now chain element of origObj
        chainOrig->Insert(cheOrig, el);
        objModf = true;
        cheOrig = (CHE*)el->successor;
      }
      else {
        elOrig = (LavaObjectPtr)cheOrig->data;
        elUpdate = (LavaObjectPtr)cheUpdate->data;
        secOff = (elUpdate[0])[0].sectionOffset;
        elUpdate = elUpdate - secOff;
        elOrig = elOrig - secOff;
        if (UpdateObject(ckd, elOrig, &elUpdate, objModf)) {
          //if (!isNew && !((SynFlags*)(origObj+1))->Contains(stateObjFlag)) {
          //  ONELEVELCOPY(ckd, origObj);
          //  isNew = true;
          //  chainOrig = (CHAINX*)(origObj+LSH);
          //  cheOrig = (CHE*)chainOrig->first;
          //  for (ii = 0; ii<iEl; ii++)
          //    cheOrig = (CHE*)cheOrig->successor;
          //}
          if (cheOrig->data)
            DFC((LavaObjectPtr)cheOrig->data);
          cheOrig->data = (DObject*)(elOrig + secOff);
        }
        if (ckd.exceptionThrown)
          return false;
        cheUpdate = (CHE*)cheUpdate->successor;
        cheOrig = (CHE*)cheOrig->successor;
      }
      iEl++;
    }
  }
  return isNew;
}

//Chain adapter functions


bool ChainAppend(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+2], ckd, ckd.document->DECLTab[B_Che])
  IFC(stack[SFH+2]);
  LavaObjectPtr setObjPtr = stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset;
  CHAINX* chain = (CHAINX*)(setObjPtr+LSH);
  *(LavaVariablePtr)(stack[SFH+2]+LSH) = setObjPtr;
  ((CHE*)(stack[SFH+2]+LSH+1))->data = (DObject*)stack[SFH+1];
  if (stack[SFH+1])
    if ((*setObjPtr)->SectionFlags.Contains(ElemsConstituent)
    || (*setObjPtr)->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+1]) // inc data
    else
      IRC(stack[SFH+1]) // inc data
  chain->Append((CHE*)(stack[SFH+2]+LSH+1));
  return true;
}

bool ChainInsertBefore(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+3], ckd, ckd.document->DECLTab[B_Che])
  IFC(stack[SFH+3]);
  LavaObjectPtr setObjPtr = stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset;
  CHAINX* chain = (CHAINX*)(setObjPtr+LSH);
  *(LavaVariablePtr)(stack[SFH+3]+LSH) = setObjPtr;
  ((CHE*)(stack[SFH+3]+LSH+1))->data = (DObject*)stack[SFH+2];
  if (stack[SFH+1])
    if ((*(LavaVariablePtr)(stack[SFH+1]+LSH) == setObjPtr) && (CHE*)(stack[SFH+1]+LSH+1))
      chain->Insert(((CHE*)(stack[SFH+1]+LSH+1))->predecessor, (CHE*)(stack[SFH+3]+LSH+1));
    else
      throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
  else
    chain->Append((CHE*)(stack[SFH+3]+LSH+1));
  if (stack[SFH+2])
    if ((*setObjPtr)->SectionFlags.Contains(ElemsConstituent)
    || (*setObjPtr)->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+2]) // inc data
    else
      IRC(stack[SFH+2]) // inc data
  return true;
}

bool ChainInsertAfter(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+3], ckd, ckd.document->DECLTab[B_Che])
  IFC(stack[SFH+3]);
  LavaObjectPtr setObjPtr = stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset;
  CHAINX* chain = (CHAINX*)(setObjPtr+LSH);
  *(LavaVariablePtr)(stack[SFH+3]+LSH) = setObjPtr;
  ((CHE*)(stack[SFH+3]+LSH+1))->data = (DObject*)stack[SFH+2];
  if (stack[SFH+1])
    if ((*(LavaVariablePtr)(stack[SFH+1]+LSH) == setObjPtr) && (CHE*)(stack[SFH+1]+LSH+1))
      chain->Insert((CHE*)(stack[SFH+1]+LSH+1), (CHE*)(stack[SFH+3]+LSH+1));
    else
      throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
  else
    chain->Insert(0, (CHE*)(stack[SFH+3]+LSH+1));
  if (stack[SFH+2])
    if ((*setObjPtr)->SectionFlags.Contains(ElemsConstituent)
    || (*setObjPtr)->SectionFlags.Contains(ElemsAcquaintance))
      IFC(stack[SFH+2]) // inc data
    else
      IRC(stack[SFH+2]) // inc data
  return true;
}


bool ChainFirst(CheckData& ckd, LavaVariablePtr stack)
{
  CHAINX* chain = (CHAINX*)(stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset+LSH);
  if (chain->first) {
    stack[SFH+1] = (LavaObjectPtr)(chain->first)-LSH-1;
    IFC(stack[SFH+1]); // inc handle
  }
  else
    stack[SFH+1] = 0;
  return true;
}

bool ChainLast(CheckData& ckd, LavaVariablePtr stack)
{
  CHAINX* chain = (CHAINX*)(stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset+LSH);
  if (chain->last) {
    stack[SFH+1] = (LavaObjectPtr)(chain->last)-LSH-1;
    IFC(stack[SFH+1]); // inc handle
  }
  else
    stack[SFH+1] = 0;
  return true;
}

bool ChainSucc(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr setObjPtr = stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset;
  if (*(LavaVariablePtr)(stack[SFH+1]+LSH) == setObjPtr) {
    if (stack[SFH+1] && (stack[SFH+1]+LSH+1) && ((CHE*)(stack[SFH+1]+LSH+1))->successor) {
      stack[SFH+2] = (LavaObjectPtr)(((CHE*)(stack[SFH+1]+LSH+1))->successor)-LSH-1;
      if (stack[SFH+2])
        IFC(stack[SFH+2]); // inc handle
    }
    else
      stack[SFH+2] = 0;
    return true;
  }
  else
    throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
}

bool ChainPrev(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr setObjPtr = stack[SFH] - stack[SFH][0][0].sectionOffset + stack[SFH][0][1].sectionOffset;
  if (*(LavaVariablePtr)(stack[SFH+1]+LSH) == setObjPtr) {
    if (stack[SFH+1] && (stack[SFH+1]+LSH+1) && ((CHE*)(stack[SFH+1]+LSH+1))->predecessor) {
      stack[SFH+2] = (LavaObjectPtr)(((CHE*)(stack[SFH+1]+LSH+1))->predecessor)-LSH-1;
      if (stack[SFH+2])
        IFC(stack[SFH+2]); // inc handle
    }
    else
      stack[SFH+2] = 0;
    return true;
  }
  else
    throw CRuntimeException(ElemNotInSet_ex, &ERR_ElemNotInSet);
}


//Che adapter functions
bool CheNewFunc(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  new1((CHE*)(stack[SFH]+LSH)) CHE();
  return true;
}

//Array adapter functions

bool ArrayCopy(CheckData& ckd, LavaVariablePtr stack)
{
  int i0, i1, ii;
  LavaVariablePtr resultElPtr, sourceElPtr;
  CRuntimeException *ex;

  i0 = *(int*)(stack[SFH]+LSH);
  i1 = *(int*)(stack[SFH+1]+LSH);
  if (!i0)
    return true;
  if (!i1) {
    i1 = i0;
    HArrayMakeLen(stack[SFH+1], i1);
  }
  //if (((SynFlags*)stack[SFH]+1)->Contains(stateObjFlag))
  //  ((SynFlags*)stack[SFH+1]+1)->INCL(stateObjFlag);
  for (ii = 0; (ii < i0) && (ii < i1); ii++) {
    resultElPtr = (*(LavaVariablePtr*)(stack[SFH+1]+LSH+1))+ii;
    sourceElPtr = (*(LavaVariablePtr*)(stack[SFH]+LSH+1))+ii;
    if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)) {
      if ( *resultElPtr) {
        DFC( *resultElPtr);
        *resultElPtr = 0;
      }
      if (*sourceElPtr) {
        ex = CopyObject(ckd, sourceElPtr, resultElPtr);//,  ((SynFlags*)((*sourceElPtr)+1))->Contains(stateObjFlag));
        if (ex)
          throw *ex;
        if (ckd.exceptionThrown)
          return false;
      }
    }
    else {
      if ((*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance)) {
        if ( *resultElPtr)
          DFC(*resultElPtr);
        *resultElPtr = (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[ii];
        if ( *resultElPtr)
          IFC( *resultElPtr); // inc data
      }
      else {
        if ( *resultElPtr)
          DRC(  *resultElPtr);
        *resultElPtr = (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[ii];
        if ( *resultElPtr)
          IRC( *resultElPtr); // inc data
      }
    }
  }
  return true;
}


bool ArrayEq(CheckData& ckd, LavaVariablePtr stack)
{
  int i0, ii;

  i0 = *(int*)(stack[SFH]+LSH);
  if (i0 != *(int*)(stack[SFH+1]+LSH))
    return false;
  for (ii = 0; (ii < i0); ii++) {
    if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)) {
      if (!EqualObjects(ckd,(*(LavaVariablePtr*)(stack[SFH]+LSH+1))[ii], (*(LavaVariablePtr*)(stack[SFH+1]+LSH+1))[ii], 0))
        return false;
    }
    else
      if ((*(LavaVariablePtr*)(stack[SFH+1]+LSH+1))[ii] != (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[ii])
        return false;
  }
  return true;
}

bool ArrayDecFunc(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr elObj;
  int ii, len = *(int*)(stack[SFH]+LSH);
  bool forward = stack[SFH][0][0].SectionFlags.Contains(ElemsConstituent)
                 || stack[SFH][0][0].SectionFlags.Contains(ElemsAcquaintance);
  for (ii = 0; ii < len; ii++) {
    elObj = (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[ii];
    if (elObj)
      if (forward)
        DFC(elObj) // dec data
      else
        DRC(elObj) // dec data
  }
  delete [] (*(LavaVariablePtr*)(stack[SFH]+LSH+1));
  return true;
}

bool ArrayDD(CheckData& ckd, LavaVariablePtr stack)
{
  DDMakeClass* dd = new DDArrayClass;
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer])
  *(stack[SFH+1]+LSH) = (CSectionDesc*)dd;
  return true;
}

bool ArraySetLength(CheckData& /*ckd*/, LavaVariablePtr stack)
{
  return HArrayMakeLen(stack[SFH], *(int*)(stack[SFH+1]+LSH));
}

bool ArrayGetLength(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer])
  *(int*)(stack[SFH+1]+LSH) = *(int*)(stack[SFH]+LSH);
  return true;
}

bool ArraySetEl(CheckData& ckd, LavaVariablePtr stack)
{
  int iSet = *(int*)(stack[SFH+1]+LSH);
  if (iSet >= 0 && iSet < *(int*)(stack[SFH]+LSH)) {
    (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[iSet] = stack[SFH+2];
    if (stack[SFH+2])
      if ((*stack[SFH])->SectionFlags.Contains(ElemsConstituent)
         || (*stack[SFH])->SectionFlags.Contains(ElemsAcquaintance))
        IFC(stack[SFH+2]) // inc data
      else
        IRC(stack[SFH+2]) // inc data
  }
  else
    throw CRuntimeException(ArrayXOutOfRange_ex, &ERR_ArrayXOutOfRange);
  return true;
}

bool ArrayGetEl(CheckData& ckd, LavaVariablePtr stack)
{
  int iSet = *(int*)(stack[SFH+1]+LSH);
  if (iSet >= 0 && iSet < *(int*)(stack[SFH]+LSH)) {
    stack[SFH+2] = (*(LavaVariablePtr*)(stack[SFH]+LSH+1))[iSet];
    if (stack[SFH+2])
      IFC(stack[SFH+2]); // inc data
    return true;
  }
  else
    throw CRuntimeException(ArrayXOutOfRange_ex, &ERR_ArrayXOutOfRange);
}

//helper functions

int HArrayGetLen(LavaObjectPtr array)
{
  if (array)
    return *(int*)(array+LSH);
  return 0;
}

bool HArrayMakeLen(LavaObjectPtr array, int len)
{
  LavaVariablePtr newAr, oldAr;
  int iold, ii;
  iold = *(int*)(array+LSH);
  if (iold < len) {
    oldAr = *(LavaVariablePtr*)(array+LSH+1);
    newAr = new LavaObjectPtr [len];
    for (ii=0; ii<len; ii++)
      newAr[ii] = 0;
    if (oldAr) {
      for (ii=0; ii<iold; ii++)
        newAr[ii] = oldAr[ii];
      delete [] oldAr;
    }
    *(int*)(array+LSH) = len;
    *(LavaVariablePtr*)(array+LSH+1) = newAr;
  }
  else
    if (len < iold)
      return false;
  return true;
}

bool HArraySetEl(CheckData& ckd, LavaObjectPtr array, LavaObjectPtr elem, int pos)
{
  if (pos < *(int*)(array+LSH)) {
    (*(LavaVariablePtr*)(array+LSH+1))[pos] = elem;
    if (elem)
      if ((*array)->SectionFlags.Contains(ElemsConstituent)
         || (*array)->SectionFlags.Contains(ElemsAcquaintance))
        IFC(elem) // inc data
      else
        IRC(elem) // inc data
  }
  else
    return false;
  return true;
}

LavaObjectPtr HArrayGetEl(LavaObjectPtr array, int pos)
{
  if (pos < *(int*)(array+LSH)) {
    return (*(LavaVariablePtr*)(array+LSH+1))[pos];
  }
  else
    return 0;
}

void HArrayOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr)
{
  int iLen, ii;
  LavaVariablePtr sourceElPtr, resultElPtr;

  if (sourceSectionPtr[0]->SectionFlags.Contains(ElemsConstituent)) {
    iLen = *(int*)(sourceSectionPtr+LSH);
    HArrayMakeLen(resultSectionPtr, iLen);
    for (ii = 0; ii < iLen; ii++) {
      sourceElPtr = (*(LavaVariablePtr*)(sourceSectionPtr+LSH+1))+ii;
      resultElPtr = (*(LavaVariablePtr*)(resultSectionPtr+LSH+1))+ii;
      if (*sourceElPtr)
        INC_FWD_CNT(ckd, *sourceElPtr);
      *resultElPtr = *sourceElPtr;
    }
  }
}

bool HArrayUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew, bool& objModf)
{
  int iLen, ii/*, secOff*/;
  LavaVariablePtr origElPtr, updateElPtr;
  LavaObjectPtr origElN;

  if (origObj[0]->SectionFlags.Contains(ElemsConstituent)) {
    iLen = *(int*)(origObj+LSH);
    for (ii = 0; ii < iLen; ii++) {
      origElPtr = (*(LavaVariablePtr*)(origObj+LSH+1))+ii;
      origElN = *origElPtr;
      updateElPtr = (*(LavaVariablePtr*)((*updatePtr)+LSH+1))+ii;
      //secOff = ((updateElPtr[0])[0])[0].sectionOffset;
      //*updateElPtr = updateElPtr[0] - secOff;
      //origElN = origElN - secOff;
      if (UpdateObject(ckd, origElN, updateElPtr, objModf)) {
        //if (!isNew && !((SynFlags*)(origObj+1))->Contains(stateObjFlag))  {
        //  isNew = true;
        //  ONELEVELCOPY(ckd, origObj);
        //  origElPtr = (*(LavaVariablePtr*)(origObj+LSH+1))+ii;
        //}
        if (*origElPtr)
          DEC_FWD_CNT(ckd, *origElPtr);
        *origElPtr = origElN /*+ secOff*/;
      }
      if (ckd.exceptionThrown)
        return false;
    }
    return isNew;
  }
  else
    return false;
}

//Exception

bool LastException(CheckData& ckd, LavaVariablePtr stack)
{
  IFC(ckd.lastException);
  stack[SFH+1] = ckd.lastException;
  //ckd.exceptionThrown = false;
  return true;
}

bool DropException(CheckData& ckd, LavaVariablePtr stack)
{
  if (ckd.lastException) {
    DFC(ckd.lastException);
    ckd.lastException = 0;
    ckd.callStack.resize(0);
    ckd.exceptionThrown = false;
  }
  return true;
}

bool ExceptionCallStack(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr strObj;

  if (ckd.lastException) {
    OBJALLOC(strObj, ckd, ckd.document->DECLTab[VLString])
    NewQString((QString*)strObj+LSH, ckd.callStack.toAscii());
    stack[SFH+1] = strObj;
  }
  return true;
}

void HGUISetData(CheckData& ckd, LavaObjectPtr guiService, LavaVariablePtr dataPtr, QWidget* dialog)
{
  if (!guiService)
    return;
  int ii;
  LavaObjectPtr guiObject = guiService - guiService[0][0].sectionOffset;
  for (ii = 0; (ii < guiObject[0][0].nSections) && (guiObject[0][ii].classDECL != ckd.document->DECLTab[B_GUI]); ii++);
  guiObject = guiObject + guiObject[0][ii].sectionOffset;
  *(LavaVariablePtr*)(guiObject+LSH) = dataPtr;
  *(QWidget**)(guiObject+LSH+1) = dialog;
}

void HGUISetUnused(CheckData& ckd, LavaObjectPtr guiService)
{
  if (!guiService)
    return;
  int ii;
  LavaObjectPtr guiObject = guiService - guiService[0][0].sectionOffset;
  for (ii = 0; (ii < guiObject[0][0].nSections) && (guiObject[0][ii].classDECL != ckd.document->DECLTab[B_GUI]); ii++);
  guiObject = guiObject + guiObject[0][ii].sectionOffset;
  *(QWidget**)(guiObject+LSH+1) = 0;
  *(LavaVariablePtr*)(guiObject+LSH) = 0;
}

QWidget* HGUIGetDialog(CheckData& ckd, LavaObjectPtr guiService)
{
  if (!guiService)
    return 0;
  int ii;
  LavaObjectPtr guiObject = guiService - guiService[0][0].sectionOffset;
  for (ii = 0; (ii < guiObject[0][0].nSections) && (guiObject[0][ii].classDECL != ckd.document->DECLTab[B_GUI]); ii++);
  guiObject = guiObject + guiObject[0][ii].sectionOffset;
  return *(QWidget**)(guiObject+LSH+1);
}

bool GUIData(CheckData& ckd, LavaVariablePtr stack)
{
  int ii;
  LavaObjectPtr dataObj = *(LavaVariablePtr)(stack[SFH] + LSH);
  if (!dataObj)
    return false; 
  LavaDECL *dataType = stack[SFH][0][0].implDECL->RelatedDECL;
  dataObj = dataObj - dataObj[0][0].sectionOffset;
  for (ii = 0; (ii < dataObj[0][0].nSections) && (dataObj[0][ii].classDECL != dataType); ii++);
  if (ii < dataObj[0][0].nSections) {
    stack[SFH+1] = dataObj + dataObj[0][ii].sectionOffset;
    return true;
  }
  else
    return false;
}


/*
bool ExceptionDecFunc(CheckData& ckd, LavaVariablePtr stack)
{
  if (*(stack[SFH]+LSH))
    DFC(*(LavaVariablePtr)(stack[SFH]+LSH));
  if (*(stack[SFH]+LSH+1))
    DFC(*(LavaVariablePtr)(stack[SFH]+LSH+1));
  return true;
}

bool ExceptionEq(CheckData& ckd, LavaVariablePtr stack)
{
  int ll, llast;
  llast = LSH+2;
  for (ll = LSH; ll < llast; ll++) {
    if (*(stack[SFH] + ll) != *(stack[SFH+1] + ll))
      if (!EqualObjects(ckd, *(LavaVariablePtr)(stack[SFH+1] + ll), *(LavaVariablePtr)(stack[SFH] + ll), 0))
        return false;
  }
  return true;
}
*/


void MakeStdAdapter()
{
  ObjectAdapter[0] = 0;
  ObjectAdapter[1] = 0;
  ObjectAdapter[2] = DefaultEq;
  ObjectAdapter[3] = 0;
  ObjectAdapter[4] = 0;
  ObjectAdapter[5] = 0;
  ObjectAdapter[6] = 0;
  ObjectAdapter[LAH] =   ObjectFinalize; //no change of position in virtual function table
  ObjectAdapter[LAH+1] = ObjectZombifyRec;
  ObjectAdapter[LAH+2] = ObjectOpEqual;
  ObjectAdapter[LAH+3] = ObjectOpNotEqual;
  ObjectAdapter[LAH+4] = ObjectEquals;
  ObjectAdapter[LAH+5] = ObjectDontSave;
  ObjectAdapter[LAH+6] = ObjectDump;

  BitsetAdapter[0] = (TAdapterFunc)1;
  BitsetAdapter[1] = 0;
  BitsetAdapter[2] = BsetEq;
  BitsetAdapter[3] = BsetLavaIO;
  BitsetAdapter[4] = 0;
  BitsetAdapter[5] = 0;
  BitsetAdapter[6] = 0;
  BitsetAdapter[LAH]   = BsetBwAnd;
  BitsetAdapter[LAH+1] = BsetBwInclOr;
  BitsetAdapter[LAH+2] = BsetBwExclOr;
  BitsetAdapter[LAH+3] = BsetOnesComplement;
  BitsetAdapter[LAH+4] = BsetLShift;
  BitsetAdapter[LAH+5] = BsetRShift;
  BitsetAdapter[LAH+6] = BsetString;

  BoolAdapter[0] = (TAdapterFunc)1;
  BoolAdapter[1] = 0;
  BoolAdapter[2] = BoolEq;
  BoolAdapter[3] = BoolLavaIO;
  BoolAdapter[4] = 0;
  BoolAdapter[5] = 0;
  BoolAdapter[6] = 0;
  BoolAdapter[LAH]   = BoolAnd;
  BoolAdapter[LAH+1] = BoolInclOr;
  BoolAdapter[LAH+2] = BoolExclOr;
  BoolAdapter[LAH+3] = BoolNot;
  BoolAdapter[LAH+4] = BoolString;

  CharAdapter[0] = (TAdapterFunc)1;
  CharAdapter[1] = 0;
  CharAdapter[2] = CharEq;
  CharAdapter[3] = CharLavaIO;
  CharAdapter[4] = 0;
  CharAdapter[5] = 0;
  CharAdapter[6] = 0;
  CharAdapter[LAH] = CharString;

  IntAdapter[0] = (TAdapterFunc)1;
  IntAdapter[1] = 0;
  IntAdapter[2] = IntEq;
  IntAdapter[3] = IntLavaIO;
  IntAdapter[4] = 0;
  IntAdapter[5] = 0;
  IntAdapter[6] = 0;
  IntAdapter[LAH]   = IntMinus;
  IntAdapter[LAH+1] = IntLT;
  IntAdapter[LAH+2] = IntGT;
  IntAdapter[LAH+3] = IntLET;
  IntAdapter[LAH+4] = IntGET;
  IntAdapter[LAH+5] = IntPlus;
  IntAdapter[LAH+6] = IntMulti;
  IntAdapter[LAH+7] = IntDiv;
  IntAdapter[LAH+8] = IntPercent;
  IntAdapter[LAH+9] = IntString;

  FloatAdapter[0] = (TAdapterFunc)1;
  FloatAdapter[1] = 0;
  FloatAdapter[2] = FloatEq;
  FloatAdapter[3] = FloatLavaIO;
  FloatAdapter[4] = 0;
  FloatAdapter[5] = 0;
  FloatAdapter[6] = 0;
  FloatAdapter[LAH]   = FloatMinus;
  FloatAdapter[LAH+1] = FloatLT;
  FloatAdapter[LAH+2] = FloatGT;
  FloatAdapter[LAH+3] = FloatLET;
  FloatAdapter[LAH+4] = FloatGET;
  FloatAdapter[LAH+5] = FloatPlus;
  FloatAdapter[LAH+6] = FloatMulti;
  FloatAdapter[LAH+7] = FloatDiv;
  FloatAdapter[LAH+8] = FloatString;

  DoubleAdapter[0] = (TAdapterFunc)2;
  DoubleAdapter[1] = 0;
  DoubleAdapter[2] = DoubleEq;
  DoubleAdapter[3] = DoubleLavaIO;
  DoubleAdapter[4] = 0;
  DoubleAdapter[5] = 0;
  DoubleAdapter[6] = 0;
  DoubleAdapter[LAH]   = DoubleMinus;
  DoubleAdapter[LAH+1] = DoubleLT;
  DoubleAdapter[LAH+2] = DoubleGT;
  DoubleAdapter[LAH+3] = DoubleLET;
  DoubleAdapter[LAH+4] = DoubleGET;
  DoubleAdapter[LAH+5] = DoublePlus;
  DoubleAdapter[LAH+6] = DoubleMulti;
  DoubleAdapter[LAH+7] = DoubleDiv;
  DoubleAdapter[LAH+8] = DoubleString;

  StringAdapter[0] = (TAdapterFunc)((sizeof(QString)+3)/4);
  StringAdapter[1] = StringCopy;
  StringAdapter[2] = StringEq;
  StringAdapter[3] = StringLavaIO;
  StringAdapter[4] = StringNewFunc;
  StringAdapter[5] = StringDecFunc;
  StringAdapter[6] = 0;
  StringAdapter[LAH]   = StringPlus;
  StringAdapter[LAH+1] = StringBox;
  StringAdapter[LAH+2] = StringQuestionBox;

//remember: enumeration section contains after the number of item the string of the item name
  EnumAdapter[0] = (TAdapterFunc)(1 + (sizeof(QString)+3)/4);
  EnumAdapter[1] = EnumCopy;
  EnumAdapter[2] = EnumEq;
  EnumAdapter[3] = EnumLavaIO;
  EnumAdapter[4] = EnumNewFunc;
  EnumAdapter[5] = EnumDecFunc;
  EnumAdapter[6] = 0;
  EnumAdapter[LAH] = EnumOrdFunc;
  EnumAdapter[LAH+1] = EnumOrdFunc;
  EnumAdapter[LAH+2] = EnumStringFunc;
  EnumAdapter[LAH+3] = EnumCommentFunc;

  SetAdapter[0] = (TAdapterFunc)((sizeof(CHAINX)+3)/4);
  SetAdapter[1] = SetCopy;
  SetAdapter[2] = SetEq;
  SetAdapter[3] = 0;
  SetAdapter[4] = SetNewFunc;
  SetAdapter[5] = SetDecFunc;
  SetAdapter[6] = SetDD;
  SetAdapter[LAH]   = SetAdd;
  SetAdapter[LAH+1] = SetAddGetHandle;
  SetAdapter[LAH+2] = SetGet;
  SetAdapter[LAH+3] = SetRemove;
  SetAdapter[LAH+4] = SetRemoveAndGet;
  SetAdapter[LAH+5] = SetCount;


  ChainAdapter[0] = 0;
  ChainAdapter[1] = 0;
  ChainAdapter[2] = DefaultEq;
  ChainAdapter[3] = 0;
  ChainAdapter[4] = 0;
  ChainAdapter[5] = 0;
  ChainAdapter[6] = 0;
  ChainAdapter[LAH]   = ChainAppend;
  ChainAdapter[LAH+1] = ChainInsertBefore;
  ChainAdapter[LAH+2] = ChainInsertAfter;
  ChainAdapter[LAH+3] = ChainFirst;
  ChainAdapter[LAH+4] = ChainLast;
  ChainAdapter[LAH+5] = ChainSucc;
  ChainAdapter[LAH+6] = ChainPrev;

  //Che+LSH:LavaObjectPtr to chain object
  //(CHE*)(Che+LSH+1): the disco-CHE
  CheAdapter[0] = (TAdapterFunc)((sizeof(CHE)+3)/4 + (sizeof(LavaObjectPtr)+3)/4);
  CheAdapter[1] = 0;
  CheAdapter[2] = 0,//DefaultEq;
  CheAdapter[3] = 0;
  CheAdapter[4] = CheNewFunc;
  CheAdapter[5] = 0;
  CheAdapter[6] = 0;

  ArrayAdapter[0] = (TAdapterFunc)2; //length and LavaVariablePtr
  ArrayAdapter[1] = ArrayCopy;
  ArrayAdapter[2] = ArrayEq;
  ArrayAdapter[3] = 0;
  ArrayAdapter[4] = 0;
  ArrayAdapter[5] = ArrayDecFunc;
  ArrayAdapter[6] = ArrayDD;
  ArrayAdapter[LAH]   = ArrayGetLength;
  ArrayAdapter[LAH+1] = ArraySetLength;
  ArrayAdapter[LAH+2] = ArrayGetEl;
  ArrayAdapter[LAH+3] = ArraySetEl;

  ExceptionAdapter[0] = 0;
  ExceptionAdapter[1] = 0;
  ExceptionAdapter[2] = DefaultEq; //ExceptionEq;
  ExceptionAdapter[3] = 0;
  ExceptionAdapter[4] = 0;
  ExceptionAdapter[5] = 0; //ExceptionDecFunc;
  ExceptionAdapter[6] = 0;
//  ExceptionAdapter[LAH] = LastException;
//  ExceptionAdapter[LAH] = DropException;
  ExceptionAdapter[LAH] = ExceptionCallStack;

  HW_L_ExceptionAdapter[0] = (TAdapterFunc)2; //code and message
  HW_L_ExceptionAdapter[1] = 0;
  HW_L_ExceptionAdapter[2] = DefaultEq;
  HW_L_ExceptionAdapter[3] = 0;
  HW_L_ExceptionAdapter[4] = 0;  
  HW_L_ExceptionAdapter[5] = 0;  
  HW_L_ExceptionAdapter[6] = 0; 

  //GUI+LSH: LavavariablePtr to data object GUI+LSH+1:  LavaGUIDialog*
  GUIAdapter[0] = (TAdapterFunc)((sizeof(LavaObjectPtr)+3)/4 + 1); //LavavariablePtr to data object and LavaGUIDialog*
  GUIAdapter[1] = 0;
  GUIAdapter[2] = 0; 
  GUIAdapter[3] = 0;
  GUIAdapter[4] = 0;
  GUIAdapter[5] = 0; 
  GUIAdapter[6] = 0;
  GUIAdapter[LAH] = &GUIData; 
  //GUIAdapter[LAH+1] = &GUIEdit; implemented in LavaProgram
  //GUIAdapter[LAH+2] = &GUIFillOut; implemented in LavaProgram

  StdAdapterTab[B_Object]     = &ObjectAdapter[0];
  StdAdapterTab[Bitset]       = &BitsetAdapter[0];
  StdAdapterTab[B_Bool]       = &BoolAdapter[0];
  StdAdapterTab[Char]         = &CharAdapter[0];
  StdAdapterTab[Integer]      = &IntAdapter[0];
  StdAdapterTab[Float]        = &FloatAdapter[0];
  StdAdapterTab[Double]       = &DoubleAdapter[0];
  StdAdapterTab[VLString]     = &StringAdapter[0];
  StdAdapterTab[Enumeration]  = &EnumAdapter[0];
  StdAdapterTab[B_Set]        = &SetAdapter[0];
  StdAdapterTab[B_Chain]      = &ChainAdapter[0];
  StdAdapterTab[B_Che]        = &CheAdapter[0];
  StdAdapterTab[B_Array]      = &ArrayAdapter[0];
  StdAdapterTab[ComponentObj] = &LinkAdapter[0];
  StdAdapterTab[B_Exception]  = &ExceptionAdapter[0];
  StdAdapterTab[B_HWException]= &HW_L_ExceptionAdapter[0];
  StdAdapterTab[B_RTException] = &HW_L_ExceptionAdapter[0];
  StdAdapterTab[B_GUI]        = &GUIAdapter[0];

}
