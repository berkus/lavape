#ifdef __GNUC__
#pragma interface
#endif

#ifndef __SETpp
#define __SETpp

#include "SYSTEM.h"
#include "ASN1.h"


class DISCO_DLL SETpp : public SET {

public:

  SETpp () {};

  SETpp (int arg1/* = -1*/, ...);

  SETpp (const unsigned long b) { bits = b; }
  
  SETpp (const SET& set) { bits = set.bits; }
  
  void CDP (PutGetFlag pgf, ASN1 *cid);
};

#endif
