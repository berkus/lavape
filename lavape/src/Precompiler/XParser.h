#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class XParserCLASS

 **********************************************************************/

#ifndef __XParser
#define __XParser


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

#include "Scanner.h"

#include "Atoms.h"

#include "IntCode.h"

#include "PCerrors.h"
/* 
FROM X409pp IMPORT
(* CONST: *)
HighOID, HighVAL;
*/
#include "DString.h"


/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class XParserCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  void CallMacro ();

  void CheckMacro ();

  void CheckpointMacro ();

  void DropMacro ();

  void GetMacro ();

  void GetPrivMacro ();

  void OrgunitMacro ();

  void ProcedureMacro ();

  void PutMacro ();

  void PutPrivMacro ();

  void ReturnMacro ();

  void ShowMacro ();

  void StartMacro ();

  void StopMacro ();

/**********************************************************************/

private:


  void putMacro ();


  /* ************************************************************************/
  /* auxiliary parsing procedures: */


  void inputOutputObjects ();


  void orgUnit ();


  void receiver ();


  void receivers ();


  void constOrVar ();


  void variableSpec (bool shouldBeFormType);
};

extern XParserCLASS XParser;

#endif
