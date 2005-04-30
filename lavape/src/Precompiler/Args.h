#ifndef __Arguments
#define __Arguments


#include "SYSTEM.h"
#include "DString.h"


class ArgumentsCLASS {

public:

  void INIT ();

  unsigned ArgCount;
  char **ArgTable;


  void GetArg (unsigned argnum,
         DString& str);
  /* Returns the 'argnum'th argument
     * on the command line. The command name is argument number zero.
     * If str.Length() is zero, the argument was not found.
     * Note: 'ArgCount' = n  ==>  arguments 0..n-1 exist
  */

};

extern ArgumentsCLASS Arguments;

extern "C" unsigned Arguments_argc;
extern "C" char **Arguments_argv;

#endif
