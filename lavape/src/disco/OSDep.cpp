/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef __GNUC__
#pragma implementation
#endif

#include "disco_all.h"

#include <stdlib.h>
#include <signal.h>
//#include "OSDep.h"

OSDepCLASS OSDep;
static bool __INITstarted=false;



void OSDepCLASS::SynFileName (DString moduleName,
                              DString& synFileName)

{

  synFileName = moduleName + SynSuffix;

} // END OF SynFileName


void OSDepCLASS::SylFileName (DString moduleName,
                              DString& sylFileName)

{

  sylFileName = moduleName + SylSuffix;

} // END OF SylFileName


void OSDepCLASS::StdSylFileName (DString moduleName,
                                 DString& sylFileName)

{

  sylFileName = VertikalDirectory + moduleName;
  sylFileName = sylFileName + SylSuffix;

} // END OF StdSylFileName


void OSDepCLASS::SynSyl (DString synFileName,
                         DString& sylFileName)

{
  unsigned loc;

  if (!synFileName.Contains(Syn,0,loc)
      && !synFileName.Contains(Syl,0,loc)) {
    sylFileName.Destroy() /* obsolete now!! */;
    return;
  }

  sylFileName = synFileName;
  sylFileName.Replace(Syl,loc);
} // END OF SynSyl


void OSDepCLASS::ProgSyl (DString& progFile,
                          DString& sylFile)

{
  unsigned loc;
  
  if (&progFile == &sylFile)
    sylFile += SylSuffix;
  else {
    if (progFile.c && (progFile[0] != '/'))
      progFile = VertikalDirectory + progFile;
    if (sylFile.l == 0)
      sylFile = progFile + SylSuffix;
    else {
      if (sylFile[0] != '/')
        sylFile = VertikalDirectory + sylFile;
      if (!sylFile.Contains(SylSuffix,0,loc))
        sylFile += SylSuffix;
    }
  }
} // END OF ProgSyl

/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

void OSDepCLASS::INIT ()
{
  char *discoPtr;
  
  if (__INITstarted) return;
  __INITstarted = true;

  discoPtr = getenv("DISCO");
  if ((discoPtr == 0) || (discoPtr[0] != '/'))
    discoPtr = "/disco";
  VertikalDirectory = DString(discoPtr) + "/";


  OS = "SunOS";



  AddrFile = VertikalDirectory + "TIP.ADDRFILE";
  AMSynFile = VertikalDirectory + "DialogAM.syl";
  MakeApplDescSynFile = VertikalDirectory + "MTApplAp.syl";
  TIPSynFile = VertikalDirectory + "SForms.syl";

  SuccessorExec = ".applrun";
  Syn = ".syn";
  Syl = ".syl";
  SynSuffix = ".syn";
  SylSuffix = ".syl";
#ifndef WIN32
  sigfillset(&AllSignals);
  sigdelset(&AllSignals,SIGTSTP);
#endif
}
