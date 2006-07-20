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



#include "BAdapter.h"
#include "DumpView.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "qglobal.h"
#include "qobject.h"
#include "qstring.h"
#include "qmessagebox.h"
//Added by qt3to4:
#include <QTextStream>
#include <QDataStream>
#include <QFocusEvent>
#include "LavaAppBase.h"
#include "LavaStream.h"

#undef new
#define new1(ADDR) new(ADDR)

static QString inStrT("TextIStream");
static QString outStrT("TextOStream");
static QString inOutStrT("TextStream");
static QString inStrD("DataIStream");
static QString outStrD("DataOStream");
static QString inOutStrD("DataStream");
static int szQFile = (sizeof(QFile)+3)/4;

QString ERR_OpenInFailed(QObject::tr("Open for input failed for file "));
QString ERR_OpenOutFailed(QObject::tr("Open for output failed for file "));
//QString ERR_OpenInOutFailed(QObject::tr("Open for inout failed for file "));

#define OBJALLOC(RESULT, CKD, DECL, ST) {\
  RESULT = AllocateObject(CKD, DECL, ST);\
  if (!RESULT && !CKD.exceptionThrown) \
    throw CRuntimeException(memory_ex ,&ERR_AllocObjectFailed);\
}
  //((SynFlags*)(RESULT+1))->INCL(finished); \


#define OPENIN(CKD, OBJ) {\
  QFile* qf = (QFile*)(OBJ+LSH+1);\
  if (!(qf->openMode() & QIODevice::ReadOnly)) \
    if (!qf->open(QIODevice::ReadOnly) && !CKD.exceptionThrown) {\
      QFileInfo qfi(*qf);\
      QString str = ERR_OpenInFailed + qfi.absoluteFilePath();\
      throw CRuntimeException(memory_ex ,&str);\
    }\
}

#define OPENOUT(CKD, OBJ) {\
  QFile* qf = (QFile*)(OBJ+LSH+1);\
  QIODevice::OpenMode mode; \
  LavaObjectPtr appendObj = *(LavaVariablePtr)(stack[SFH]+LSH); \
  if (*(bool*)(appendObj+LSH))   \
    mode = QIODevice::Append | QIODevice::WriteOnly; \
  else \
    mode = QIODevice::WriteOnly;\
  if (!(qf->openMode() & mode)) \
    if (!qf->open(mode) && !CKD.exceptionThrown) {\
      QFileInfo qfi(*qf);\
      QString str = ERR_OpenOutFailed + qfi.absoluteFilePath();\
      throw CRuntimeException(memory_ex ,&str);\
    }\
}

bool TGetChar(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Char], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) >> *(char*)(stack[SFH+1]+LSH);
  return true;
}
bool TGetInt(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) >> *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool TGetFloat(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Float], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) >> *(float*)(stack[SFH+1]+LSH);
  return true;
}

bool TGetDouble(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Double], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) >> *(double*)(stack[SFH+1]+LSH);
  return true;
}

bool TGetString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  QString str;
  (*(QTextStream*)(object+LSH+1+szQFile)) >> str;
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}


bool TAtEnd(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  return (*(QTextStream*)(object+LSH+1+szQFile)).atEnd();
}

bool TReadTotal(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  QString str;
  str = (*(QTextStream*)(object+LSH+1+szQFile)).readAll();
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}


bool TPutChar(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) << *(char*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutInt(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] ;
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) << *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool TPutFloat(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) << *(float*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutDouble(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) << *(double*)(stack[SFH+1]+LSH);
  return true;
}


bool TPutString(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QTextStream*)(object+LSH+1+szQFile)) << *(QString*)(stack[SFH+1]+LSH);
  return true;
}


bool TNewFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr obj = stack[SFH]-stack[SFH][0]->sectionOffset;
  LavaObjectPtr urlObj = ((RunTimeData*)(obj-LOH))->urlObj;
  QString fileName =  *(QString*)(urlObj+LSH);
  LavaObjectPtr modeObj = AllocateObject(ckd, ckd.document->DECLTab[B_Bool], false);
  *(bool*)(modeObj+LSH) = false;
  *(LavaVariablePtr)(stack[SFH]+LSH) = modeObj;
  new1(stack[SFH]+LSH+1) QFile(fileName);
  new1(stack[SFH]+LSH+1+szQFile) QTextStream((QFile*)(stack[SFH]+LSH+1));
  return true;
}

bool TDecFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  ((QTextStream*)(stack[SFH]+LSH+1+szQFile))->QTextStream::~QTextStream();
  ((QFile*)(stack[SFH]+LSH+1))->QFile::~QFile();
  DFC(*(LavaVariablePtr)(stack[SFH]+LSH));
  return true;
}


//DataStream-------------------------------------------------------------------------------- 


bool DGetInt(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  *(int*)(stack[SFH+1]+LSH) = 0;
  (*(QDataStream*)(object+LSH+1+szQFile)) >> *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool DGetFloat(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Float], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  *(float*)(stack[SFH+1]+LSH) = 0;
  (*(QDataStream*)(object+LSH+1+szQFile)) >> *(float*)(stack[SFH+1]+LSH);
  return true;
}
bool DGetDouble(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Double], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  (*(QDataStream*)(object+LSH+1+szQFile)) >> *(double*)(stack[SFH+1]+LSH);
  return true;
}

bool DGetString(CheckData& ckd, LavaVariablePtr stack)
{
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[VLString], false)
  LavaObjectPtr object = stack[SFH];
  OPENIN(ckd,object);
  QString str;
  (*(QDataStream*)(object+LSH+1+szQFile)) >> str;
  new1(stack[SFH+1]+LSH) QString(str);
  return true;
}

bool DAtEnd(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH] - *(int*)(stack[SFH]+LSH);
  OPENIN(ckd,object);
  return (*(QDataStream*)(object+LSH+1+szQFile)).atEnd();
}


bool SetAppendMode(CheckData& ckd, LavaVariablePtr stack)
{
  *(stack[SFH]+LSH) = 0;
  CopyObject(ckd, &stack[SFH+1], (LavaVariablePtr)(stack[SFH]+LSH));
  LavaObjectPtr obj = *(LavaVariablePtr)(stack[SFH]+LSH);
  return true;
}

bool DPutInt(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+1+szQFile)) << *(int*)(stack[SFH+1]+LSH);
  return true;
}

bool DPutFloat(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+1+szQFile)) << *(float*)(stack[SFH+1]+LSH);
  return true;
}


bool DPutDouble(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+1+szQFile)) << *(double*)(stack[SFH+1]+LSH);
  return true;
}


bool DPutString(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr object = stack[SFH];
  OPENOUT(ckd,object);
  (*(QDataStream*)(object+LSH+1+szQFile)) << (*(QString*)(stack[SFH+1]+LSH));
  return true;
}


bool DNewFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  LavaObjectPtr obj = stack[SFH]-stack[SFH][0]->sectionOffset;
  LavaObjectPtr urlObj = ((RunTimeData*)*(obj-LOH))->urlObj;
  QString fileName =  *(QString*)(urlObj+LSH);
  LavaObjectPtr modeObj = AllocateObject(ckd, ckd.document->DECLTab[B_Bool], false);
  //((SynFlags*)(modeObj+1))->INCL(finished);
  *(bool*)(modeObj+LSH) = false;
  *(LavaVariablePtr)(stack[SFH]+LSH) = modeObj;
  new1(stack[SFH]+LSH+1) QFile(fileName);
  new1(stack[SFH]+LSH+1+szQFile) QDataStream((QFile*)(stack[SFH]+LSH+1));
  return true;
}

bool DDecFuncInOut(CheckData& ckd, LavaVariablePtr stack)
{
  ((QFile*)(stack[SFH]+LSH+1))->QFile::~QFile();
  ((QDataStream*)(stack[SFH]+LSH+1+szQFile))->QDataStream::~QDataStream();
  DFC(*(LavaVariablePtr)(stack[SFH]+LSH));
  return true;
}


bool DStreamDDFunc(CheckData& ckd, LavaVariablePtr stack)
{
  DDMakeClass* dd = new DDStreamClass(true);
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  *(stack[SFH+1]+LSH) = (CSectionDesc*)dd;
  return true;
}

bool TStreamDDFunc(CheckData& ckd, LavaVariablePtr stack)
{
  DDMakeClass* dd = new DDStreamClass(false);
  OBJALLOC(stack[SFH+1], ckd, ckd.document->DECLTab[Integer], false)
  *(stack[SFH+1]+LSH) = (CSectionDesc*)dd;
  return true;
}


TAdapterFunc TOutAdapter[LAH + 6] =
{    (TAdapterFunc)(szQFile + sizeof(QTextStream)+3/4+1), //has mode
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     TNewFuncInOut,
     TDecFuncInOut,
     TStreamDDFunc, //DD
     SetAppendMode,
     TPutChar,
     TPutInt,
     TPutFloat,
     TPutDouble,
     TPutString
};

TAdapterFunc TInAdapter[LAH + 7] =
{    (TAdapterFunc)(szQFile + sizeof(QTextStream)+3/4+1), //has no mode
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     TNewFuncInOut,
     TDecFuncInOut,
     TStreamDDFunc, //DD
     TGetChar,
     TGetInt,
     TGetFloat,
     TGetDouble,
     TGetString,
     TAtEnd,
     TReadTotal
};



TAdapterFunc* TextIStream()
{
  return TInAdapter;
}

TAdapterFunc* TextOStream()
{
  return TOutAdapter;
}

TAdapterFunc DOutAdapter[LAH + 6] =
{    (TAdapterFunc)(szQFile + sizeof(QDataStream)+3/4+1),
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     DNewFuncInOut,
     DDecFuncInOut,
     DStreamDDFunc, //DD
     SetAppendMode,
     DPutInt,
     DPutFloat,
     DPutDouble,
     DPutString
};

TAdapterFunc DInAdapter[LAH + 5] =
{    (TAdapterFunc)(szQFile + sizeof(QDataStream)+3/4+1),
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     (TAdapterFunc)0,
     DNewFuncInOut,
     DDecFuncInOut,
     DStreamDDFunc, //DD
     DGetInt,
     DGetFloat,
     DGetDouble,
     DGetString,
     DAtEnd
};


TAdapterFunc* DataIStream()
{
  return DInAdapter;
}

TAdapterFunc* DataOStream()
{
  return DOutAdapter;
}

bool DDStreamClass::hasChildren()
{
  if ((int)myObject <= 3)
    return false;
  else
    return true;
}

void DDStreamClass::makeChildren()
{
  DDItem *childItem = 0;
  QFile* qf = (QFile*)(myObject+LSH+1);
  QString label;
  DDStreamClass *dd;

  dd = new DDStreamClass(isDStream);
  if (qf) {
    if (qf->openMode() & QIODevice::ReadOnly)
      dd->value2 = "read only";
    else if (qf->openMode() & QIODevice::WriteOnly) {
      dd->value2 = dd->value2 + "write only";
      if (qf->openMode() & QIODevice::Append)
        dd->value2 = dd->value2 + " | append mode";
    }
    else if(qf->openMode() & QIODevice::ReadWrite)
       dd->value2 = "read and write";
    else
      dd->value2 = "not opened";
  }
  else
    dd->value2 = "not opened";
  label = "io-mode   ";
  childItem = myItem->createChild(dd, childItem, myDoc, (LavaObjectPtr)1, label, true, true);            

  dd = new DDStreamClass(isDStream);
  if (qf) {
    switch (qf->error()) {
    case QFile::NoError:
      dd->value2 = "ok";
      break;
    case QFile::ReadError:
      dd->value2 = "read error";
      break;
    case QFile::WriteError:
       dd->value2 = "write error";
      break;
    case QFile::OpenError:
      dd->value2 = "open error";
      break;
    case QFile::AbortError:
      dd->value2 = "abort error";
      break;
    case QFile::TimeOutError:
      dd->value2 = "time out error";
      break;
    default:;
      dd->value2 = "unspecified error";
    }
  }

  label = "status   ";
  childItem = myItem->createChild(dd, childItem, myDoc, (LavaObjectPtr)1, label, true, true);            
  if (isDStream) {
    dd = new DDStreamClass(isDStream);
    QDataStream * dstream = (QDataStream*)(myObject+LSH+1+szQFile);
    if (dstream)
      if (dstream->byteOrder() == QDataStream::BigEndian)
        dd->value2 = "BigEndian";
      else
        dd->value2 = "LittleEndian";

    label = "byte order  ";
    childItem = myItem->createChild(dd, childItem, myDoc, (LavaObjectPtr)3, label, true, true);            
  }
}
  
QString DDStreamClass::getValue0(const QString& stdLabel)
{
  return stdLabel;
}

QString DDStreamClass::getValue1()
{
  QString str;
  return str;
}

QString DDStreamClass::getValue2()
{
  return value2;
}

