#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class PMDumpCLASS

 **********************************************************************/

#ifndef __PMDump
#define __PMDump


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"


/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/


//#include "UNIX.h"


/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class DISCO_DLL PMDumpCLASS {

public:

  void PostMortemDump ();
  /* produces a post mortem dump and HALTs */

};

extern DISCO_DLL PMDumpCLASS PMDump;

#endif
