#ifndef __ProcCoder
#define __ProcCoder


#include "IntCode.h"


class ProcCoderCLASS {

public:

  void CodeProcs (TreeNodePtr node);
  
  void DeclSpecifiers (TreeNodePtr node);

  void CodeDeferredProcs ();


private:
  
  void declaratorList (TreeNodePtr node);

};

extern ProcCoderCLASS ProcCoder;

#endif
