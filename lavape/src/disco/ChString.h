#ifdef __GNUC__
#pragma interface
#endif

#ifndef __ChainOfStringCLASS
#define __ChainOfStringCLASS

#include "ChainA.h"
#include "DString.h"


struct DISCO_DLL CHEString : ChainAnyElem {
  DString data;

  ChainAnyElem* Clone ()
  { return new CHEString(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEString*)from)->data; }
  
};


class DISCO_DLL ChainOfString : public ChainAny {
public:

  ChainOfString () : ChainAny() {}

  ChainOfString (const ChainAny0& c) { init(c); }

  ChainOfString (const ChainOfString& c) { init(c); }
  // only this is recognized as a copy constructor

  virtual ~ChainOfString() { Destroy(); }

  ChainOfString& operator= (const ChainAny0& c)
  { copy(c); return *this; }

  ChainOfString& operator= (const ChainOfString& c)
  { copy(c); return *this; }


  bool Contains (const DString& elem);
  
  bool AppendIfMissing (const DString& elem);
  // true if missing and appended  
};

#endif
