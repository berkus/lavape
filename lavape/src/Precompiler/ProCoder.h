#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class ProcCoderCLASS

 **********************************************************************/

#ifndef __ProcCoder
#define __ProcCoder


#include "IntCode.h"



/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

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
