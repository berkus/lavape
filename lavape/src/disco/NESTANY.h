#ifdef __GNUC__
#pragma interface
#endif

#ifndef __NESTEDANYCLASS
#define __NESTEDANYCLASS

#include "NestA.h"
#include "ASN1pp.h"


typedef AnyType ANYTYPE;


struct DISCO_DLL NESTEDANY0 : NestedAny0 {
  
  NESTEDANY0 () : NestedAny0() {}

  NESTEDANY0 (const NestedAny0& c) { init(c); }

  NESTEDANY0 (const NESTEDANY0& c) { init(c); }
  // only this is recognized as a copy constructor


  NESTEDANY0& operator= (const NESTEDANY0& c)
  { copy(c); return *this; }

  virtual void CDP (PutGetFlag pgf, ASN1* cid); //,
                    //ConversionProc cdp,
                    //NestedAllocationProc allocNewElem);
/*
  virtual void FIO (PutGetFlag pgf, ASN1* cid,
                    ConversionProc fio,
                    NestedAllocationProc allocNewElem);
              
  virtual void SIG (ASN1* cid,
                    SignatureProc sig);*/
};  


struct DISCO_DLL NESTEDANY : public NESTEDANY0 {

  NESTEDANY () : NESTEDANY0() {}

  NESTEDANY (const NESTEDANY0& c) { init(c); }

  NESTEDANY (const NESTEDANY& c) { init(c); }
  // only this is recognized as a copy constructor

  virtual ~NESTEDANY() { Destroy(); }


  NESTEDANY& operator= (const NESTEDANY0& c)
  { copy(c); return *this; }

  NESTEDANY& operator= (const NESTEDANY& c)
  { copy(c); return *this; }
};  

#endif
