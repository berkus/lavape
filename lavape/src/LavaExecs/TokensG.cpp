//*******************************************************************
//
//   public procedures of structs/classes defined in Tokens.h
//
//*******************************************************************

#include "Tokens.h"


#include "CDPpp.h"


void CDPTToken (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool)

{
  TToken *vp = (TToken*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TToken)cid->FGETunsigned();
} // END OF CDPTToken
