#ifndef __ASN1pp
#define __ASN1pp
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "ASN1.h"
#include "DString.h"
//#include "ChainA.h"



/* This module is an extension of the basic ASN1 module and is primarily
   used by the DISCO application programs. Calls to the procedures of
   this module are generated by the DISCO pre-processor (--> ASN1"pp") */


const int HighOID = 79;
const int HighVAL = 255;
const int BufferLength = 1000;


typedef void (*ConversionProc)(PutGetFlag,ASN1*,address,bool);

typedef void (*SignatureProc)(ASN1*,address);

/* type of the conversion procedures that are generated by the
   pre-processor for converting complex data objects from/to
   X.409 format; ADDRESS identifies the program variable which
   contains or is to receive the object */

class ChainAnyElem;
typedef ChainAnyElem* (*ChainAllocationProc)();

typedef DString TOIDstring;

typedef DString TVALstring;


struct TIPCMem {
  unsigned OIDcard, VALcard;
  int OIDint, VALint;
  TOIDstring OIDstring;
  TVALstring VALstring;
  address RCVchain;
  DString RCVorgUnit;
  address TempCertList;
  bool Return; /* skip signature CvtPROCS after sig encountered */
  bool HasCertList; /* signature list follows after data object */
  unsigned PUTencrypted, GOTencrypted;
};

typedef TIPCMem *TPIPCMem;



/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class DISCO_DLL ASN1ppCLASS {

public:

/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

  void INIT ();


  TPIPCMem IPCMem;
  
  void (*CDPCERTLIST)(PutGetFlag,ASN1*,address);
};

extern DISCO_DLL ASN1ppCLASS ASN1pp;




/**********************************************************************/
/*                     macros for NESTED-CDP/FIO:                     */
/**********************************************************************/


#if (__GNUC__ || SOLARIS2 || WIN32)

#define NESTEDINITCOPY(PARMTYPE) \
void NST0##PARMTYPE::init (const NST0##PARMTYPE &n) \
{ \
  if (n.ptr) { \
    ptr = new PARMTYPE; \
    *ptr = *n.ptr; \
  } \
  else \
    ptr = 0; \
} \
void NST0##PARMTYPE::copy (const NST0##PARMTYPE &n) \
{ \
  if (n.ptr) { \
    if (!ptr) \
      ptr = new PARMTYPE; \
    *ptr = *n.ptr; \
  } \
  else { \
    delete ptr; \
    ptr = 0; \
  } \
}


#define NESTEDCDP(PARMTYPE) \
void NST0##PARMTYPE::CDP (PutGetFlag pgf, ASN1* cid, ConversionProc cdp) \
{ \
  if (cid->Skip()) return; \
\
  if (pgf == PUT) \
    if (ptr == 0) cid->PutOptionalAbsent(); \
    else cdp(PUT,cid,(address)ptr,false); \
  else \
    if (cid->GotOptional()) { \
      if (ptr == 0) ptr = new PARMTYPE; \
      cdp(GET,cid,(address)ptr,false); \
    } \
    else { \
      delete ptr; \
      ptr = 0; \
    } \
}


#define NESTEDFIO(PARMTYPE) \
void NST0##PARMTYPE::FIO (PutGetFlag pgf, ASN1* cid, ConversionProc fio) \
{ \
  if (cid->Skip()) return; \
\
  if (pgf == PUT) \
    if (ptr == 0) FIOpp.EOANULL(pgf,cid); \
    else { \
      FIOpp.DOWN(1); \
      fio(pgf,cid,(address)ptr,false); \
      FIOpp.UP(1); \
    } \
  else { \
    FIOpp.flushAddressQueue(GET,cid); \
    if (FIOpp.isEOA(cid)) { \
      cid->WrongElemStop(false); \
      cid->GetNULL(); \
      cid->WrongElemStop(true); \
      if (!cid->WrongElem()) { \
  FIOpp.GetEOA(cid); \
  delete ptr; \
  ptr = 0; \
  return; \
      } \
    } \
\
    FIOpp.DOWN(1); \
    if (ptr == 0) ptr = new PARMTYPE; \
    fio(pgf,cid,(address)ptr,false); \
    FIOpp.UP(1); \
  } \
}

#else


#define NESTEDINITCOPY(PARMTYPE) \
void NST0/**/PARMTYPE::init (const NST0/**/PARMTYPE &n) \
{ \
  if (n.ptr) { \
    ptr = new PARMTYPE; \
    *ptr = *n.ptr; \
  } \
  else \
    ptr = 0; \
} \
void NST0/**/PARMTYPE::copy (const NST0/**/PARMTYPE &n) \
{ \
  if (n.ptr) { \
    if (!ptr) \
      ptr = new PARMTYPE; \
    *ptr = *n.ptr; \
  } \
  else { \
    delete ptr; \
    ptr = 0; \
  } \
}


#define NESTEDCDP(PARMTYPE) \
void NST0/**/PARMTYPE::CDP (PutGetFlag pgf, ASN1* cid, ConversionProc cdp) \
{ \
  if (cid->Skip()) return; \
\
  if (pgf == PUT) \
    if (ptr == 0) cid->PutOptionalAbsent(); \
    else cdp(PUT,cid,(address)ptr,false); \
  else \
    if (cid->GotOptional()) { \
      if (ptr == 0) ptr = new PARMTYPE; \
      cdp(GET,cid,(address)ptr,false); \
    } \
    else { \
      delete ptr; \
      ptr = 0; \
    } \
}


#define NESTEDFIO(PARMTYPE) \
void NST0/**/PARMTYPE::FIO (PutGetFlag pgf, ASN1* cid, ConversionProc fio) \
{ \
  if (cid->Skip()) return; \
\
  if (pgf == PUT) \
    if (ptr == 0) FIOpp.EOANULL(pgf,cid); \
    else { \
      FIOpp.DOWN(1); \
      fio(pgf,cid,(address)ptr,false); \
      FIOpp.UP(1); \
    } \
  else { \
    FIOpp.flushAddressQueue(GET,cid); \
    if (FIOpp.isEOA(cid)) { \
      cid->WrongElemStop(false); \
      cid->GetNULL(); \
      cid->WrongElemStop(true); \
      if (!cid->WrongElem()) { \
  FIOpp.GetEOA(cid); \
  delete ptr; \
  ptr = 0; \
  return; \
      } \
    } \
\
    FIOpp.DOWN(1); \
    if (ptr == 0) ptr = new PARMTYPE; \
    fio(pgf,cid,(address)ptr,false); \
    FIOpp.UP(1); \
  } \
}


#endif

#endif
