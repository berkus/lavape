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

#include "BAdapter.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "qobject.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "LavaAppBase.h"
#include "LavaStream.h"
#ifndef WIN32
//#include <fenv.h>
#endif


static QString inStrT("TextIStream");
static QString outStrT("TextOStream");
static QString inOutStrT("TextStream");
static QString inStrD("DataIStream");
static QString outStrD("DataOStream");
static QString inOutStrD("DataStream");
static int szQFile = (sizeof(QFile)+3)/4;

QString ERR_OpenInFailed(QObject::tr("Open for input failed for file "));
QString ERR_OpenOutFailed(QObject::tr("Open for output failed for file "));
QString ERR_OpenInOutFailed(QObject::tr("Open for inout failed for file "));

#define OBJALLOC(RESULT, CKD, DECL, ST) {\
  RESULT = AllocateObject(CKD, DECL, ST);\
	((SynFlags*)(RESULT+1))->INCL(finished); \
  if (!RESULT && !CKD.exceptionThrown) {\
    CRuntimeException* ex = new CRuntimeException(memory_ex ,&ERR_AllocObjectFailed);\
    throw ex;\
  }\
}


#define OPENIN(CKD, OBJ) {\
  QFile* qf = (QFile*)(OBJ+LSH);\
  if (!(qf->mode() & (IO_ReadOnly | IO_ReadWrite))) \
    if (((SynFlags*)(OBJ+1))->Contains(compoPrim)) {\
      if (!qf->open(IO_ReadWrite) && !CKD.exceptionThrown) {\
        QFileInfo qfi(*qf);\
        QString str = ERR_OpenInOutFailed + qfi.absFilePath();\
        CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);\
        throw ex;\
      }\
    }\
    else \
      if (!qf->open(IO_ReadOnly) && !CKD.exceptionThrown) {\
        QFileInfo qfi(*qf);\
        QString str = ERR_OpenInFailed + qfi.absFilePath();\
        CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);\
        throw ex;\
      }\
}

#define OPENOUT(CKD, OBJ) {\
  QFile* qf = (QFile*)(OBJ+LSH);\
  if (!(qf->mode() & (IO_WriteOnly | IO_ReadWrite))) \
    if (((SynFlags*)(OBJ+1))->Contains(compoPrim)){\
      if (!qf->open(IO_ReadWrite) && !CKD.exceptionThrown) {\
        QFileInfo qfi(*qf);\
        QString str = ERR_OpenInOutFailed + qfi.absFilePath();\
        CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);\
        throw ex;\
      }\
    }\
    else \
      if (!qf->open(IO_WriteOnly) && !CKD.exceptionThrown) {\
          QFileInfo qfi(*qf);\
          QString str = ERR_OpenOutFailed + qfi.absFilePath();\
          CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);\
        throw ex;\
      }\
}
 

bool TGetChar(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Char], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) >> *(char*)(stack[SFH+1]+LSH);
  return true;
}
bool TGetInt(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) >> *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool TGetFloat(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Float], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) >> *(float*)(stack[SFH+1]+LSH);
  return true;
}
bool TGetDouble(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Double], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) >> *(double*)(stack[SFH+1]+LSH);
  return true;
}

bool TGetString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  QString str;
  (*(QTextStream*)(object+LSH+szQFile)) >> str;
  new(stack[SFH+1]+LSH) QString(str);
  return true;
}


bool TPutChar(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) << *(char*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutInt(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) << *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool TPutFloat(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) << *(float*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutDouble(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) << *(double*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutString(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+szQFile)) << *(QString*)(stack[SFH+1]+LSH);
  return true;
}


bool TNewFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr obj = stack[SFH]-stack[SFH][0]->sectionOffset;
  LavaObjectPtr urlObj = *(LavaVariablePtr)(obj-2);
  QString fileName =  *(QString*)(urlObj+LSH);
  new(stack[SFH]+LSH) QFile(fileName);
  new(stack[SFH]+LSH+szQFile) QTextStream((QFile*)(stack[SFH]+LSH));
  for (int secn = 0; (secn < obj[0]->nSections); secn++) {
    if (!inStrT.compare(obj[0][secn].classDECL->LitStr.c ))
      *(int*)(obj + obj[0][secn].sectionOffset+LSH) = obj[0][secn].sectionOffset-stack[SFH][0]->sectionOffset;
    else if (!outStrT.compare(obj[0][secn].classDECL->LitStr.c ))
      *(int*)(obj + obj[0][secn].sectionOffset+LSH) = obj[0][secn].sectionOffset-stack[SFH][0]->sectionOffset;
  }
  return true;
}

bool TDecFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  ((QTextStream*)(stack[SFH]+LSH+szQFile))->QTextStream::~QTextStream();
  ((QFile*)(stack[SFH]+LSH))->QFile::~QFile();
  return true;
}

TAdapterFunc TOutAdapter[LAH + 5] =
{    (TAdapterFunc)1,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     TPutChar,
     TPutInt,
     TPutFloat,
     TPutDouble,
     TPutString
};

TAdapterFunc TInAdapter[LAH + 5] =
{    (TAdapterFunc)1,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     TGetChar,
     TGetInt,
     TGetFloat,
     TGetDouble,
     TGetString
};

TAdapterFunc TInOutAdapter[LAH] =
{    (TAdapterFunc)(szQFile + sizeof(QTextStream)+3/4),
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     TNewFuncInOut,
     TDecFuncInOut
};


TAdapterFunc* TextIStream()
{
  return TInAdapter;
}

TAdapterFunc* TextOStream()
{
  return TOutAdapter;
}

TAdapterFunc* TextStream()
{
  return TInOutAdapter;
}

//DataStream-------------------------------------------------------------------------------- 

bool DGetInt(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  QFile* qf = (QFile*)(object+LSH);
  if (!(qf->mode() & (IO_ReadOnly | IO_ReadWrite))) 
    if (((SynFlags*)(object+1))->Contains(compoPrim)) {
      if (!qf->open(IO_ReadWrite) && !ckd.exceptionThrown) {
        QFileInfo qfi(*qf);
        QString str = ERR_OpenInOutFailed + qfi.absFilePath();
        CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);
        throw ex;
      }
    }
    else 
      if (!qf->open(IO_ReadOnly) && !ckd.exceptionThrown) {
        QFileInfo qfi(*qf);
        QString str = ERR_OpenInFailed + qfi.absFilePath();
        CRuntimeException* ex = new CRuntimeException(memory_ex ,&str);
        throw ex;
      }
//  OPENIN(ckd,object);
  *(int*)(stack[SFH+1]+LSH) = 0;
  (*(QDataStream*)(object+LSH+szQFile)) >> *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool DGetFloat(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Float], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  *(float*)(stack[SFH+1]+LSH) = 0;
  (*(QDataStream*)(object+LSH+szQFile)) >> *(float*)(stack[SFH+1]+LSH);
  return true;
}
bool DGetDouble(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Double], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  (*(QDataStream*)(object+LSH+szQFile)) >> *(double*)(stack[SFH+1]+LSH);
  return true;
}

bool DGetString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString], false)
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  QString str;
  (*(QDataStream*)(object+LSH+szQFile)) >> str;
  new(stack[SFH+1]+LSH) QString(str);
  return true;
}


bool DPutInt(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+szQFile)) << *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool DPutFloat(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+szQFile)) << *(float*)(stack[SFH+1]+LSH);
  return true;
}


bool DPutDouble(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+szQFile)) << *(double*)(stack[SFH+1]+LSH);
  return true;
}


bool DPutString(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+szQFile)) << (*(QString*)(stack[SFH+1]+LSH));
  return true;
}


bool DNewFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr obj = stack[SFH]-stack[SFH][0]->sectionOffset;
  LavaObjectPtr urlObj = *(LavaVariablePtr)(obj-2);
  QString fileName =  *(QString*)(urlObj+LSH);
  new(stack[SFH]+LSH) QFile(fileName);
  new(stack[SFH]+LSH+szQFile) QDataStream((QFile*)(stack[SFH]+LSH));
  for (int secn = 0; (secn < obj[0]->nSections); secn++) {
    if (!inStrD.compare(obj[0][secn].classDECL->LitStr.c ))
      *(int*)(obj + obj[0][secn].sectionOffset+LSH) = obj[0][secn].sectionOffset-stack[SFH][0]->sectionOffset;
    else if (!outStrD.compare(obj[0][secn].classDECL->LitStr.c ))
      *(int*)(obj + obj[0][secn].sectionOffset+LSH) = obj[0][secn].sectionOffset-stack[SFH][0]->sectionOffset;
  }
  return true;
}

bool DDecFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  ((QFile*)(stack[SFH]+LSH))->QFile::~QFile();
  ((QDataStream*)(stack[SFH]+LSH+szQFile))->QDataStream::~QDataStream();
  return true;
}

TAdapterFunc DOutAdapter[LAH + 5] =
{    (TAdapterFunc)1,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     DPutInt,
     DPutFloat,
     DPutDouble,
     DPutString
};

TAdapterFunc DInAdapter[LAH + 5] =
{    (TAdapterFunc)1,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     DGetInt,
     DGetFloat,
     DGetDouble,
     DGetString
};

TAdapterFunc DInOutAdapter[LAH] =
{    (TAdapterFunc)(szQFile + sizeof(QDataStream)+3/4),
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     DNewFuncInOut,
     DDecFuncInOut
};


TAdapterFunc* DataIStream()
{
  return DInAdapter;
}

TAdapterFunc* DataOStream()
{
  return DOutAdapter;
}

TAdapterFunc* DataStream()
{
  return DInOutAdapter;
}
