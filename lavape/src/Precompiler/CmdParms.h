#ifndef __CmdParms
#define __CmdParms


#include "SYSTEM.h"
#include "ChString.h"


class CmdParmsCLASS {

public:

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
