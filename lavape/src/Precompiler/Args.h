#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class ArgumentsCLASS

 **********************************************************************/

#ifndef __Arguments
#define __Arguments


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"
#include "DString.h"

/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class ArgumentsCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
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
