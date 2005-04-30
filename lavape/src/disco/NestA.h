#ifndef __NestedAnyCLASS
#define __NestedAnyCLASS
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "AnyType.h"


class DISCO_DLL NestedAny0 {
protected:

  void init (const NestedAny0&);

  void copy (const NestedAny0&);

public:
  
  AnyType *ptr;
  
  NestedAny0 () { ptr = 0; }

  NestedAny0 (const NestedAny0& c) { init(c); }
  
  void Destroy()
  { delete ptr; ptr = 0; }

  NestedAny0& operator= (const NestedAny0& c)
  { copy(c); return *this; }
};  


struct DISCO_DLL NestedAny : NestedAny0 {

  NestedAny () { ptr = 0; }

  NestedAny (const NestedAny0& c) { init(c); }

  NestedAny (const NestedAny& c) { init(c); }
  // only this is recognized as a copy constructor

  virtual ~NestedAny()
  { Destroy(); }


  NestedAny& operator= (const NestedAny0& c)
  { copy(c); return *this; }

  NestedAny& operator= (const NestedAny& c)
  { copy(c); return *this; }
};  

#endif
