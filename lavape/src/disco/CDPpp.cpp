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


#include "CDPpp.h"
#include "MachDep.h"
#include "STR.h"
#include "DIO.h"
#pragma hdrstop


CDPppCLASS CDPpp;
static bool __INITstarted=false;

/************************************************************************/

void CDPppCLASS::CVTSEQUENCE (PutGetFlag pgf,
                              ASN1 *cid)
{
  if (pgf == PUT)
    cid->PutSEQUENCE();
  else
    cid->GetSEQUENCE();
}


void CDPppCLASS::CVTEOC (PutGetFlag pgf,
                         ASN1 *cid)
{
  if (pgf == PUT)
    cid->PutEOC();
  else
    cid->GetEOC();
}


void CDPppCLASS::CVTNULL (PutGetFlag pgf,
                          ASN1 *cid)
{
  if (pgf == PUT)
    cid->PutNULL();
  else
    cid->GetNULL();
}


void CDPppCLASS::CVTunsigned (PutGetFlag pgf,
                              ASN1 *cid,
                              unsigned& x)
{
  if (pgf == PUT)
    cid->PUTint(x);
  else
    cid->GETunsigned(x);
}


void CDPppCLASS::CVTint (PutGetFlag pgf,
       ASN1 *cid,
       int& x)
{
  if (pgf == PUT)
    cid->PUTint(x);
  else
    cid->GETint(x);
}


void CDPppCLASS::CVTfloat (PutGetFlag pgf,
       ASN1 *cid,
       float& x)
{
  if (pgf == PUT)
    cid->PUTfloat(x);
  else
    cid->GETfloat(x);
}


void CDPppCLASS::CVTdouble (PutGetFlag pgf,
       ASN1 *cid,
       double& x)
{
  if (pgf == PUT)
    cid->PUTdouble(x);
  else
    cid->GETdouble(x);
}


void CDPppCLASS::CVTbool (PutGetFlag pgf,
                          ASN1 *cid,
                          bool& x)
{
  if (pgf == PUT)
    cid->PUTboolean(x);
  else
    cid->GETboolean(x);
}


void CDPppCLASS::CVTbyte (PutGetFlag pgf,
                          ASN1 *cid,
                          byte& x)
{
  if (pgf == PUT)
    cid->PUTbyte(x);
  else
    cid->GETbyte(x);
}


void CDPppCLASS::CVTchar (PutGetFlag pgf,
                          ASN1 *cid,
                          char& x)
{
  if (pgf == PUT)
    cid->PUTchar(x);
  else
    cid->GETchar(x);
}


/* **********************************************************************/


void CDPppCLASS::TagFieldError (ASN1 *cid)

{
  IO.WriteLn();
  IO.WriteLn();
  IO.WriteString("+++++ erroneous tag field value");
  IO.WriteLn();
  IO.WriteLn();
  cid->CallErrorExit();
} // END OF CDPerror


void CDPppCLASS::SKIP (PutGetFlag pgf,
                       ASN1 *cid)
{
  bool eoc;

  if (pgf == GET)
    cid->SkipElement(eoc);
}


void CDPbool (PutGetFlag pgf,
              ASN1 *cid,
              address a,
              bool)
{
  bool *vp;

  vp = (bool *)a;
  if (pgf == PUT)
    cid->PUTboolean(*vp);
  else
    cid->GETboolean(*vp);
}


void CDPchar (PutGetFlag pgf,
              ASN1 *cid,
              address a,
        bool)
{
  char *vp;

  vp = (char *)a;
  if (pgf == PUT)
    cid->PUTchar(*vp);
  else
    cid->GETchar(*vp);
}


void CDPbyte (PutGetFlag pgf,
              ASN1 *cid,
              address a,
              bool)
{
  if (pgf == PUT)
    cid->PUTbyte(*(unsigned char*)a);
  else
    cid->GETbyte(*(unsigned char*)a);
}


void CDPint (PutGetFlag pgf,
             ASN1 *cid,
             address a,
             bool)
{
  int *vp;

  vp = (int *)a;
  if (pgf == PUT)
    cid->PUTint(*vp);
  else
    cid->GETint(*vp);
}


void CDPunsigned (PutGetFlag pgf,
                  ASN1 *cid,
                  address a,
                  bool)
{
  unsigned *vp;

  vp = (unsigned *)a;
  if (pgf == PUT)
    cid->PUTint(*vp);
  else
    cid->GETunsigned(*vp);
}


void CDPfloat (PutGetFlag pgf,
               ASN1 *cid,
               address a,
               bool)
{
  float *vp;

  vp = (float *)a;
  if (pgf == PUT)
    cid->PUTfloat(*vp);
  else
    cid->GETfloat(*vp);
}


void CDPdouble (PutGetFlag pgf,
                ASN1 *cid,
                address a,
                bool)
{
  double *vp;

  vp = (double *)a;
  if (pgf == PUT)
    cid->PUTdouble(*vp);
  else
    cid->GETdouble(*vp);
}


/***********************************************************/


void CDPARBITRARY (PutGetFlag pgf,
                   ASN1 *cid,
                   address a,
       bool)
{
  ARBITRARY *vp=(ARBITRARY *)a;

  vp->CDP(pgf,cid);
}


void CDPBYTES (PutGetFlag pgf,
               ASN1 *cid,
               address a,
         bool)
{
  BYTES *vp=(BYTES *)a;

  vp->CDP(pgf,cid);
}


void CDPSTRING (PutGetFlag pgf,
                ASN1 *cid,
                address a,
          bool)
{
  STRING *vp=(STRING*)a;

  vp->CDP(pgf,cid);
}


void CDPSET (PutGetFlag pgf,
       ASN1 *cid,
       address a,
             bool)
{
  SETpp *vp=(SETpp *)a;

  vp->CDP(pgf,cid);
}


void CDPBITSTRING (PutGetFlag pgf,
                   ASN1 *cid,
                   address a,
       bool)
{
  BITSTRING *vp=(BITSTRING *)a;

  vp->CDP(pgf,cid);
}


/**********************************************************************/


ChainAnyElem* NewCHEunsigned ()
{ return (ChainAnyElem*)(new CHEunsigned); }


ChainAnyElem* NewCHEint ()
{ return (ChainAnyElem*)(new CHEint); }

ChainAnyElem* NewCHEfloat ()
{ return (ChainAnyElem*)(new CHEfloat); }

ChainAnyElem* NewCHEdouble ()
{ return (ChainAnyElem*)(new CHEdouble); }


ChainAnyElem* NewCHEboolean ()
{ return (ChainAnyElem*)(new CHEboolean); }


ChainAnyElem* NewCHEbyte ()
{ return (ChainAnyElem*)(new CHEbyte); }


ChainAnyElem* NewCHEchar ()
{ return (ChainAnyElem*)(new CHEchar); }



/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void CDPppCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  MachDep.INIT();
}
