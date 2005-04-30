#ifndef __Halt
#define __Halt
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "PMDump.h"

typedef struct epiProcDesc *epiProc;

struct DISCO_DLL epiProcDesc {
  void (*epiProcedure)();
  epiProc nextEpiProc;
};



class DISCO_DLL HaltCLASS {

public:

  void Halt ();
  /* Terminates execution after having called all epilogue procedures which
     have been declared previously (by calling "DeclareEpilogue"). */
  
  void HaltPMD ();
  /* Terminates execution after having called all epilogue procedures which
     have been declared previously (by calling "DeclareEpilogue"). Finally
     PMDump.PostMortemDump is called. */

  void DeclareEpilogue (DPROC epilogueProc);
  /* an arbitrary number of parameterless "termination procedures"
     can be declared by calling "DeclareEpilogue"; they are invoked
     in LIFO order if "Halt" is called. */

  void Epilogue ();
  /* calls the epilogue procedures which have been declared previously */

};

extern DISCO_DLL HaltCLASS Halt;

#endif
