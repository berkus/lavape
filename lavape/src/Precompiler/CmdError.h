#ifndef __CmdError
#define __CmdError


#include "SYSTEM.h"

#include "Atoms.h"

enum CmdErrType {
  TooManyParms,
  MissingParms,
  IncompleteFileid,
  IllegalFileType,
  UnknownOption,
  OpenErrorSource,
  OpenErrorOutput,
  SourceEmpty};

/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/

#include "UNIX.h"

#include "DIO.h"


/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class CmdErrorCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  void CommandError (CmdErrType cmdErr);

/**********************************************************************/

private:
};

extern CmdErrorCLASS CmdError;

#endif
