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

#include "Syntax.h"
#include "NESTANY.h"
#include "AnyType.h"
//#include "disco.h"


void NESTEDANY0::CDP (PutGetFlag pgf, ASN1* cid)//,
                      //ConversionProc cdp,
                      //NestedAllocationProc allocNewElem)
{ 
  DObject *oPtr;
  wxClassInfo *cli;
  DString className;
  bool eoc;

  if (cid->Skip()) return;
  
  if (pgf == PUT)
    if (ptr == 0) cid->PutOptionalAbsent();
    else {
      cid->PUTstring(ptr->GetWxClassInfo()->className);
      oPtr = (DObject*)ptr;
      oPtr->CDP(PUT,cid,false);
    }
  else
    if (cid->GotOptional()) {
      cid->GETstring(className);
      if (className == DString("TBNFexpression"))
        className = "LavaDECL";
      else if (className == DString("MakeCreate"))
        className = "NewExpression";
      else if (className == DString("MakeClone"))
        className = "CloneExpression";
      else if (className == DString("SuccStatement"))
        className = "SucceedStatement";
      if (className == DString("EnumConst")
      && cid->Release < 6) {
        className = "SynObject";
//        cli = wxClassInfo::FindClass(className.c);
        ptr = (DObject*)wxCreateDynamicObject(className.c);//(DObject*)cli->CreateObject();
        ((SynObjectBase*)ptr)->MakeExpression();
        cid->SkipElement(eoc);
      }
      else if (className == DString("FailStatement")
      && cid->Release < 7) {
//        cli = wxClassInfo::FindClass(className.c);
        ptr = (DObject*)wxCreateDynamicObject(className.c);//(DObject*)cli->CreateObject();
        cid->SkipElement(eoc);
      }
      else {
        cli = wxClassInfo::FindClass(className.c);
        if (ptr) {
          if (ptr->GetWxClassInfo() != cli) {
            delete ptr;
            ptr = (AnyType*)cli->CreateObject();
          }
        }
        else
          ptr = (AnyType*)cli->CreateObject();
        oPtr = (DObject*)ptr;
        oPtr->CDP(GET,cid,false);
      }
    }
    else {
      delete ptr;
      ptr = 0;
    }
}

/*
void NESTEDANY0::FIO (PutGetFlag pgf, ASN1* cid,
                      ConversionProc fio,
                      NestedAllocationProc allocNewElem)
{ 
  if (cid->Skip()) return;
  
  if (pgf == PUT)
    if (ptr == 0) FIOpp.EOANULL(pgf,cid);
    else {
      FIOpp.DOWN(1);
      fio(pgf,cid,(address)ptr+sizeof(AnyType),false);
      FIOpp.UP(1);
    }
  else {
    FIOpp.flushAddressQueue(GET,cid);
    if (FIOpp.isEOA(cid)) {
      cid->WrongElemStop(false);
      cid->GetNULL();
      cid->WrongElemStop(true);
      if (!cid->WrongElem()) {
        FIOpp.GetEOA(cid);
        delete ptr;
        ptr = 0;
        return;
      }
    }
  
    FIOpp.DOWN(1);
    if (ptr == 0) ptr = (AnyType*)allocNewElem();
    fio(pgf,cid,(address)ptr+sizeof(AnyType),false);
    FIOpp.UP(1);
  }
}


void NESTEDANY0::SIG (ASN1* cid,
                      SignatureProc sig)
{ 
  if (cid->Skip()) return;
  
  if (ptr == 0)
    return;
  else
    sig(cid,(address)ptr+sizeof(AnyType));
}
*/
