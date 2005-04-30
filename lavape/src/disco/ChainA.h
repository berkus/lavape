#ifndef __ChainAnyCLASS
#define __ChainAnyCLASS
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "AnyType.h"


class DISCO_DLL ChainAnyElem {
public:
  ChainAnyElem *successor, *predecessor;
    
  ChainAnyElem() { successor = 0; predecessor = 0; }
  
  virtual ~ChainAnyElem () {}
  
  
  virtual ChainAnyElem* Clone ()=0;
  
  virtual void CopyData (ChainAnyElem *)=0;
};

class DISCO_DLL CHE0 : public ChainAnyElem {
public:
  DObject *data;
  
  CHE0 () {
    data =0;
  }
  
  CHE0 (AnyType *p) {
    data = (DObject*)p;
  }
    
  virtual void CopyData (ChainAnyElem *);

  CHE0 (CHE0 *c) { data = 0; CopyData(c); }
  
  virtual ~CHE0 () {}

  virtual ChainAnyElem* Clone () { return new CHE0(this); }

};


class DISCO_DLL ChainAny0 {
protected:

  void init (const ChainAny0&);

  void copy (const ChainAny0&);
  
public:

  ChainAnyElem *first, *last;

  ChainAny0 () { first = 0; last = 0; }

  ChainAny0 (const ChainAny0& c) { init(c); }

  virtual void Destroy ();
  
  virtual ~ChainAny0(){}

  ChainAny0& operator= (const ChainAny0& c)
  { copy(c); return *this; }
  
    
  void Append (ChainAnyElem * const newElem);

  void Prepend (ChainAnyElem * const newElem);
  
  void Push (ChainAnyElem * const newElem) {
  Prepend(newElem); }
   
  void Insert (ChainAnyElem * const afterElem,
         ChainAnyElem * const newElem);
  
  //remove from chain and delete
  void Remove (ChainAnyElem * const afterElem);

  //remove from chain and return elem to calling program
  ChainAnyElem * Uncouple (ChainAnyElem *elem);

inline void ChainAny0::Delete(ChainAnyElem *elem)
  //remove elem from chain and delete it
  { delete Uncouple(elem); }
  
  ChainAnyElem* Pop (ChainAnyElem **firstElem=0);
  
  void Cut (ChainAnyElem * const fromElem);

  ChainAnyElem* GetNth(int nn);
  bool AddNth(int nn, ChainAnyElem * newElem);
  ChainAnyElem* UncoupleNth(int nn);
};


class DISCO_DLL ChainAny : public ChainAny0 {
public:

  ChainAny () : ChainAny0() {}

  ChainAny (const ChainAny0& c) : ChainAny0() { init(c); }

  ChainAny (const ChainAny& c) { init(c); }
  // only this is recognized as a copy constructor

  virtual ~ChainAny() { Destroy(); }

  ChainAny& operator= (const ChainAny0& c)
  { copy(c); return *this; }
  
  ChainAny& operator= (const ChainAny& c)
  { copy(c); return *this; }
};


#endif
