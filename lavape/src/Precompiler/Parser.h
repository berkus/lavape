#ifndef __Parser
#define __Parser


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
