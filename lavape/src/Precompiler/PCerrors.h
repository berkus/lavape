#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class PCerrorsCLASS

 **********************************************************************/

#ifndef __PCerrors
#define __PCerrors


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

#include "Atoms.h"

enum LexErrType {
  EmptyString,
  IllegalChar,
  MissingExponent,
  OpenComment,
  StringExceedsLine,
  WrongDecimal,
  WrongHexadecimal,
  WrongOctal,
  WrongExponent};

enum InternalErrType {
  InputBufferOfl,
  StackOverflow,
  StringBufferOverflow,
  StackUnderflow,
  SynIOError,
  IOppError};

enum ContextErrType {
  DataLengthError,
  FieldLength0,
  DataLength0,
  DuplTypeDef,
  DuplTypeDefSpec,
  DuplTypeSpec,
  ForbiddenQualRef,
  InFormsOnly,
  NoFormType,
  BaseTypesOnly,
  NotAllowed,
  NotAllowedInForm,
  NotSupported,
  OIDTooLong,
  OnTopLevelOnly,
  NotOnTopLevel,
  CLincompatTF,
  SingleCharExpected,
  TFNotItem,
  UndefinedType,
  NoLocalType,
  IncompBaseType,
  MissingCont,
  VALTooLong};

/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/


#include "UNIX.h"

#include "PMDump.h"

#include "DIO.h"
/* 
FROM X409pp IMPORT
(* CONST: *)
HighOID, HighVAL;
*/
#include "CmdParms.h"

#include "IntCode.h"

static const unsigned maxErrors = 50;

/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class PCerrorsCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  unsigned ErrorCount;

  bool Recovery;
  /* is set to "true" in PCerrors.mod;

     "true" causes "NextAtom" in "Scanner" to stay at the last error
     position and to return "noAtom" */

  bool AfterContextError;
  /* is set to "true" in PCerrors.mod */


  void LexicalError (LexErrType lexErr);

  void Expected (const char * const s);
  void Expected (const DString& s)
  { Expected(s.c); }

  void NotYetImplemented ();

  void InternalError (InternalErrType intErr);

  void ContextError (ContextErrType codeErr,
                     unsigned line,
                     unsigned col);

  void Missing (TStringBufferPos objId);

/**********************************************************************/

private:

  unsigned lastErrLine, lastErrCol;


  void setError (bool isContextError);
};

extern PCerrorsCLASS PCerrors;

#endif
