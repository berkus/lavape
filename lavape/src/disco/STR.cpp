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

#include "disco_all.h"
/*
#include "STR.h"
#include "ASN1.h"
*/

void STRING::CDP (PutGetFlag pgf, ASN1 *cid)
{
  if (pgf == PUT) cid->PUTstring(*this);
  else cid->GETstring(*this);
}


void ARBITRARY::CDP (PutGetFlag pgf, ASN1 *cid)
{
  bool eoc;
  
  if (pgf == PUT)
    if (l > 0) cid->PutBytes(*this);
    else cid->PutNULL();
  else {
    l = 0;
    cid->GetElement(*this,eoc);
    if ((l == 2) && (c[0] == '\005') && (c[1] == '\0'))
      l = 0; // NULL-element, = empty ARBITRARY
  }
}


void CDPARBCERTLIST (PutGetFlag pgf, ASN1 *cid, address varAddr)
{
  bool eoc;
  ARBITRARY **arbPPtr=(ARBITRARY**)varAddr;
  
  if (pgf == PUT) {
    if ((*arbPPtr)->l > 0) cid->PutBytes(**arbPPtr);
    else cid->PutNULL();
  }
  else {
    *arbPPtr = new ARBITRARY(1000);
    cid->GetElement(**arbPPtr,eoc);
  }
}


void BYTES::CDP (PutGetFlag pgf, ASN1 *cid)
{
  if (pgf == PUT) cid->PUTbytes(*this);
  else cid->GETbytes(*this);
}


void BITSTRING::CDP (PutGetFlag pgf, ASN1 *cid)
{
  if (pgf == PUT) cid->PUTbits(*this);
  else cid->GETbits(*this);
}


/*
ChainAnyElem* NewCHESTRING0 ()
{ return (ChainAnyElem*)(new CHESTRING0); }
*/

ChainAnyElem* NewCHESTRING ()
{ return (ChainAnyElem*)(new CHESTRING); }


ChainAnyElem* NewCHEBYTES ()
{ return (ChainAnyElem*)(new CHEBYTES); }


ChainAnyElem* NewCHEBITSTRING ()
{ return (ChainAnyElem*)(new CHEBITSTRING); }


ChainAnyElem* NewCHEARBITRARY ()
{ return (ChainAnyElem*)(new CHEARBITRARY); }

