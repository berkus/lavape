#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class CmdParmsCLASS

 **********************************************************************/

#ifndef __CmdParms
#define __CmdParms


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"
#include "ChString.h"

/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class CmdParmsCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  DString SourceFile, ErrorFile, OutputFile, OutputFileImplMod,
     SynDefFile, ModuleName, CurrentIncludeFile, DLLexport;
  
  bool HasImplMod;

  ChainOfString IncludeDirectories;

/**********************************************************************/

private:

  unsigned loc, i;
  
  DString pCfile;

};

extern CmdParmsCLASS CmdParms;

#endif
