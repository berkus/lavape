#ifdef __GNUC__
#pragma interface
#endif

#ifndef __CHAINANYCLASS
#define __CHAINANYCLASS

#include "ASN1pp.h"
#include "ChainA.h"
#include "AnyType.h"


typedef ChainAnyElem CHAINANYELEM;


struct DISCO_DLL CHAINANY0 : public ChainAny0 {

  CHAINANY0 () : ChainAny0() {}

  CHAINANY0 (const CHAINANY0& c) { init(c); }

  CHAINANY0& operator= (const CHAINANY0& c)
  { copy(c); return *this; }
  

  void CDP (PutGetFlag pgf, ASN1* cid,
            ConversionProc cdp,
            ChainAllocationProc allocNewElem);
/*
  void FIO (PutGetFlag pgf, ASN1* cid,
            ConversionProc fio,
            ChainAllocationProc allocNewElem);

  void SIG (ASN1* cid,
            SignatureProc sig);*/
};


struct DISCO_DLL CHAINANY : public CHAINANY0 {

  CHAINANY () : CHAINANY0() {}

  CHAINANY (const CHAINANY0& c) { init(c); }

  CHAINANY (const CHAINANY& c) { init(c); }
  // only this is recognized as a copy constructor

  virtual ~CHAINANY()
  { Destroy(); }


  CHAINANY& operator= (const CHAINANY0& c)
  { copy(c); return *this; }

  CHAINANY& operator= (const CHAINANY& c)
  { copy(c); return *this; }

};

struct DISCO_DLL CHAINX : public DObject, ChainAny {

  CHAINX () : ChainAny() {}

  CHAINX (const CHAINX& c) { init(c); }

  CHAINX& operator= (const CHAINX& c)
  { copy(c); return *this; }
  
  virtual ~CHAINX()
  { Destroy(); }

  void DestroyKeepElems();
  void CDP (PutGetFlag pgf, ASN1* cid, bool=false);
  //  the "bool is required to make this the implementation
  //  of the DObject::CDP (pure!)
};


class DISCO_DLL CHE : public ChainAnyElem {
public:
  DObject *data;
  
  CHE () {
    successor = 0;
    predecessor = 0;
    data =0;
  }
  
  CHE (AnyType *p) {
    successor = 0;
    predecessor = 0;
    data = (DObject*)p;
  }
  
  virtual ~CHE () { delete data; data = 0; }
    
  virtual void CopyData (ChainAnyElem *);

  CHE (CHE *c) { data = 0; CopyData(c); }

  virtual ChainAnyElem* Clone () { return new CHE(this); }

};

extern DISCO_DLL CHE *NewCHE();

extern DISCO_DLL CHE *NewCHE(AnyType *p);

#endif
