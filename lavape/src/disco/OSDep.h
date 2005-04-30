#ifndef __OSDep
#define __OSDep

/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "UNIX.h"
#include "DString.h"


class DISCO_DLL OSDepCLASS {

public:

  void INIT ();

#ifndef WIN32
  sigset_t AllSignals;
#endif
  
  
  DString OS;
  /* name of the local operating system */

  DString VertikalDirectory;

  DString SuccessorExec, AddrFile, AMSynFile, MakeApplDescSynFile, TIPSynFile;
  /* these files contain the syntax definitions of the DialogAM forms,
     "MakeApplDesc" forms, and of the TIP forms */

  DString Syn, Syl, SynSuffix, SylSuffix;



  void SynFileName (DString moduleName,
                    DString& synFileName);
  /* returns the name of the "syntax file" 
     for the specified module "moduleName" */

  void SylFileName (DString moduleName,
                    DString& synFileName);
  /* returns the name of the "linked syntax file" 
     for the specified program module "moduleName" */

  void StdSylFileName (DString moduleName,
                       DString& sylFileName);
  /* returns the name of the standard VERTIKAL "syntax file" 
     for the specified module "moduleName" */

  void SynSyl (DString synFileName,
               DString& sylFileName);
  /* replaces the syn suffix with a syl suffix  */

  void ProgSyl (DString& progFile,
                DString& sylFile);
  /* computes the final names of the syntax and program files
     of an application */

};

extern DISCO_DLL OSDepCLASS OSDep;

#endif
