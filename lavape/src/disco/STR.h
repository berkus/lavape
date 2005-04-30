#ifndef __STRING0
#define __STRING0
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"

#include "DString.h"
#include "CDPpp.h"
#include "ASN1.h"
#include "CHAINANY.h"
#include "NESTANY.h"


struct DISCO_DLL STRING : public DString {
  
  STRING () : DString() {}
  
  STRING (const DString& str) { init(str); }
  
  STRING (const STRING& str) { init(str); }
  // only this is recognized as a copy constructor
  
  STRING (const unsigned maxLen) : DString(maxLen) {}
  
  STRING (const char * const s) : DString(s) {}
  // '\0' terminates s
  
  STRING (const char * const s, const unsigned length)
    : DString(s,length) {}
  // explicit length specification for s

  STRING (const char ch, const unsigned count)
    : DString(ch,count) {}
  /* Afterwards *this consists of count characters = ch */

  virtual ~STRING () { Destroy(); }


  STRING& operator= (const DString& str)
  { copy(str); return *this; }

  STRING& operator= (const STRING& str)
  { copy(str); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid);

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAstring(pgf,cid,*this); }
  
};


struct DISCO_DLL ARBITRARY : public STRING {
  
  ARBITRARY () : STRING() {}
  
  ARBITRARY (const ARBITRARY& str) { init(str); }
  
  ARBITRARY (const unsigned maxLen) : STRING(maxLen) {}

  virtual ~ARBITRARY () {}



  ARBITRARY& operator= (const DString& str)
  { copy(str); return *this; }

  ARBITRARY& operator= (const ARBITRARY& str)
  { copy(str); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid);

  friend void CDPARBCERTLIST (PutGetFlag pgf, ASN1 *cid, address);

};


struct DISCO_DLL BYTES : public STRING {
  
  BYTES () : STRING() {}
  
  BYTES (const BYTES& str) { init(str); }
  
  BYTES (const unsigned maxLen) : STRING(maxLen) {}

  virtual ~BYTES () {}



  BYTES& operator= (const DString& str)
  { copy(str); return *this; }

  BYTES& operator= (const BYTES& str)
  { copy(str); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid);

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAbytes(pgf,cid,*this); }

};


struct DISCO_DLL BITSTRING : public Bitstring {
  
  BITSTRING () : Bitstring() {}
  
  BITSTRING (const BITSTRING& str) { init(str); }
  
  BITSTRING (const unsigned maxLen) : Bitstring(maxLen) {}
  
  virtual ~BITSTRING() {}



  BITSTRING& operator= (const Bitstring& str)
  { copy(str); return *this; }

  BITSTRING& operator= (const BITSTRING& str)
  { copy(str); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid);

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAbitstring(pgf,cid,*this); }

};


/*
struct CHESTRING0 : ChainAnyElem {
  STRING0 data;

  ChainAnyElem* Clone ()
  { return new CHESTRING0(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHESTRING0*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPSTRING(pgf,cid,(address)&data); }
};

struct NST0STRING0 {
  struct STRING0 *ptr;

  NST0STRING0 () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { ptr->CDP(pgf,cid); }

  void FIO (PutGetFlag pgf, ASN1 *cid)
  { ptr->FIO(pgf,cid); }
};

struct NSTSTRING0 : NST0STRING0 {
  NSTSTRING0 () : NST0STRING0() {}

  virtual ~NSTSTRING0 () { Destroy(); }
};

extern ChainAnyElem* NewCHESTRING0 ();
*/


struct DISCO_DLL CHESTRING : ChainAnyElem {
  STRING data;

  ChainAnyElem* Clone ()
  { return new CHESTRING(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHESTRING*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPSTRING(pgf,cid,(address)&data); }
};

struct DISCO_DLL NST0STRING {
  struct STRING *ptr;

  NST0STRING () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { ptr->CDP(pgf,cid); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ ptr->FIO(pgf,cid); }
};

struct DISCO_DLL NSTSTRING : NST0STRING {
  NSTSTRING () : NST0STRING() {}

  virtual ~NSTSTRING () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHESTRING ();



struct DISCO_DLL CHEBYTES : ChainAnyElem {
  BYTES data;

  ChainAnyElem* Clone ()
  { return new CHEBYTES(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEBYTES*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPBYTES(pgf,cid,(address)&data); }
};

struct DISCO_DLL NST0BYTES {
  struct BYTES *ptr;

  NST0BYTES () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { ptr->CDP(pgf,cid); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ ptr->FIO(pgf,cid); }
};

struct DISCO_DLL NSTBYTES : NST0BYTES {
  NSTBYTES () : NST0BYTES() {}

  virtual ~NSTBYTES () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEBYTES ();



struct DISCO_DLL CHEBITSTRING : ChainAnyElem {
  BITSTRING data;

  ChainAnyElem* Clone ()
  { return new CHEBITSTRING(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEBITSTRING*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPBITSTRING(pgf,cid,(address)&data); }
};

struct DISCO_DLL NST0BITSTRING {
  struct BITSTRING *ptr;

  NST0BITSTRING () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { ptr->CDP(pgf,cid); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ ptr->FIO(pgf,cid); }
};

struct DISCO_DLL NSTBITSTRING : NST0BITSTRING {
  NSTBITSTRING () : NST0BITSTRING() {}

  virtual ~NSTBITSTRING () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEBITSTRING ();



struct DISCO_DLL CHEARBITRARY : ChainAnyElem {
  ARBITRARY data;

  ChainAnyElem* Clone ()
  { return new CHEARBITRARY(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEARBITRARY*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPARBITRARY(pgf,cid,(address)&data); }
};

struct DISCO_DLL NST0ARBITRARY {
  struct ARBITRARY *ptr;

  NST0ARBITRARY () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
    { ptr->CDP(pgf,cid); }
};

struct DISCO_DLL NSTARBITRARY : NST0ARBITRARY {
  NSTARBITRARY () : NST0ARBITRARY() {}

  virtual ~NSTARBITRARY () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEARBITRARY ();

#endif
