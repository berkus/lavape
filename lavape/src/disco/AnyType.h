#ifndef __AnyType
#define __AnyType
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "wx_obj.h"
#include "ASN1pp.h"


class DISCO_DLL AnyType : public wxObject {
public: 
  virtual void CopyData (AnyType *);

  virtual AnyType* Clone ();
};


typedef AnyType* (*NestedAllocationProc)();


class DISCO_DLL DObject : public AnyType {
public:
  virtual void CDP (PutGetFlag pgf, ASN1* cid, bool baseCDP)=0;
};



#endif
