#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class ParserCLASS

 **********************************************************************/

#ifndef __Parser
#define __Parser


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

#include "ContxtCK.h"

#include "DParser.h"

#include "XParser.h"

#include "Scanner.h"

#include "Atoms.h"

#include "IntCode.h"

#include "PCerrors.h"


/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class ParserCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  void Parser ();

/**********************************************************************/

private:
};

extern ParserCLASS Parser;

#endif
