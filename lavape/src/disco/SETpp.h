#ifndef __SETpp
#define __SETpp
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

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
