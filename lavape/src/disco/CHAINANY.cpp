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

#include "CHAINANY.h"
#include "wx_obj.h"



CHE *NewCHE() {return new CHE;}

CHE *NewCHE(AnyType *p) {return new CHE(p);}

void CHAINANY0::CDP (PutGetFlag pgf, ASN1* cid,
                     ConversionProc cdp,
                     ChainAllocationProc allocNewElem)

{
  ChainAnyElem *elem, *predecessor;
  
  if (cid->Skip()) return;
  
  if (pgf == PUT) {
    cid->PutSEQUENCE();
    for (elem = first; elem; elem = elem->successor)
      cdp(pgf,cid,(address)elem+sizeof(ChainAnyElem),false);
    cid->PutEOC();
  }
  else {
    predecessor = 0;
    cid->WrongElemStop(false);
    cid->GetSEQUENCE();
    cid->WrongElemStop(true);
    if (cid->Skip()) return;
    
    if (!cid->WrongElem())
      for (elem = first; !cid->Skip(); elem = elem->successor) {
        cid->WrongElemStop(false);
        cid->GetEOC();
        cid->WrongElemStop(true);
        if (!cid->WrongElem()) break;
        if (!elem) {
          elem = (ChainAnyElem*)allocNewElem();
          Append(elem);
        }
        cdp(pgf,cid,(address)elem+sizeof(ChainAnyElem),false);
        predecessor = elem;
      }
    Cut(predecessor);
  }
}


void CHE::CopyData (ChainAnyElem *c)

{
  if (!((CHE*)c)->data) {
    delete data;
    data = 0;
  }
  else {
    if (data)
      delete data;
    data = (DObject*)((CHE*)c)->data->Clone();
  }
}

void CHAINX::DestroyKeepElems()

{
  CHE *elem, *next;
  
  for (elem = (CHE*)first; elem; elem = (CHE*)next) {
    next = (CHE*)elem->successor;
    elem->data = 0;
    delete elem;
  }
  first = 0;
  last = 0;
}

void CHAINX::CDP (PutGetFlag pgf, ASN1* cid, bool)

{
  
  CHE *elem, *predecessor;
  DString className;
  wxClassInfo *cli;
  
  if (cid->Skip()) return;
  
  if (pgf == PUT) {
    cid->PutSEQUENCE();
    for (elem = (CHE*)first; elem; elem = (CHE*)elem->successor) {
      if (elem->data)
        cid->PUTstring(elem->data->GetWxClassInfo()->className);
      else
        cid->PutNULL();
      elem->data->CDP(pgf,cid,false);
    }
    cid->PutEOC();
  }
  else {
    predecessor = 0;
    cid->GetSEQUENCE();
    
    for (elem = (CHE*)first; !cid->Skip(); elem = (CHE*)elem->successor) {
      cid->WrongElemStop(false);
      cid->GetEOC();
      cid->WrongElemStop(true);
      if (cid->Skip()) return;
      if (!cid->WrongElem()) break;
      if (!cid->GotOptional())
        if (elem) {
          if (elem->data) {
            delete elem->data;
            elem->data = 0;
          }
        }
        else {
          elem = (CHE*) new CHE;
          Append(elem);
        }
      else {
        if (!elem) {
          elem = (CHE*) new CHE;
          Append(elem);
        }
        cid->GETstring(className);
        if (cid->Skip()) return;
        cli = wxClassInfo::FindClass(className.c);
        if (elem->data) {
          if (elem->data->GetWxClassInfo() != cli) {
            delete elem->data;
            elem->data = (DObject*)cli->CreateObject();
          }
        }
        else
          elem->data = (DObject*)cli->CreateObject();
        elem->data->CDP(pgf,cid,false);
      }
      predecessor = elem;
    }
    Cut(predecessor);
  }
}
