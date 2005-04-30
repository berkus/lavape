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


#include "CmdParms.h"

#include "CmdError.h"
#include "IntCode.h"
#include "Args.h"
#include "STR.h"

#include "DIO.h"


CmdParmsCLASS CmdParms;
static bool __INITstarted=false;

/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void CmdParmsCLASS::INIT ()
{
  bool genSpecified=false;
  DString arg;

  if (__INITstarted) return;
  __INITstarted = true;

  CmdError.INIT();
  IntCode.INIT();
  Arguments.INIT();

  IntCode.GlobalProperties.Flags = SET();

  ErrorFile = "PP.errors";

  i = 1;
  if (Arguments.ArgCount == 1)
    CmdError.CommandError(IncompleteFileid);
  Arguments.GetArg(i,arg);
  
  IncludeDirectories.AppendIfMissing(".");
  
  while (arg[0] == '-') {
    switch (arg[1]) {
    case 'd':
      genSpecified = true;
      IntCode.GlobalProperties.Flags.INCL(DEPgen);
      break;
    case 'm':
      genSpecified = true;
      IntCode.GlobalProperties.Flags.INCL(MODgen);
      break;
    case 's':
      genSpecified = true;
      IntCode.GlobalProperties.Flags.INCL(SYNgen);
      break;
    case 'I':
      if (arg.l == 2) {
        if (Arguments.ArgCount <= ++i)
          CmdError.CommandError(MissingParms);
        Arguments.GetArg(i,SourceFile);
        if (arg[0] == '-')
          CmdError.CommandError(MissingParms);
      }
      else
        arg = arg.Substr(2,0);
      IncludeDirectories.AppendIfMissing(arg);
      break;
    case 'X':
      if (arg.l == 2) {
        if (Arguments.ArgCount <= ++i)
          CmdError.CommandError(MissingParms);
        Arguments.GetArg(i,DLLexport);
        if (DLLexport[0] == '-')
          CmdError.CommandError(MissingParms);
      }
      else
        DLLexport = arg.Substr(2,0);      
      break;
    default:
      CmdError.CommandError(UnknownOption);
    }
    if (Arguments.ArgCount <= ++i)
      CmdError.CommandError(MissingParms);
    Arguments.GetArg(i,arg);
  }

  if (!genSpecified)
    IntCode.GlobalProperties.Flags += SET(SYNgen,MODgen,-1);

  SourceFile = arg;

  if (SourceFile.Contains(".pC",0,loc)) {
    OutputFile = SourceFile.Substr(0,loc);
    ModuleName = OutputFile;
    SynDefFile = OutputFile;
    OutputFile += ".cpp";
  }
  else
    if (SourceFile.Contains(".ph",0,loc)) {
      IntCode.GlobalProperties.Flags.INCL(IncludeFile);
      OutputFile = SourceFile.Substr(0,loc);
      ModuleName = OutputFile;
      OutputFileImplMod = OutputFile + "G.cpp";
      SynDefFile = OutputFile;
      OutputFile += ".h";

      HasImplMod = false;
      pCfile = SynDefFile + ".cpp";
      if (access(pCfile.c,R_OK) == 0)
        HasImplMod = true;
      else {
  pCfile = SynDefFile + ".pC";
  if (access(pCfile.c,R_OK) == 0) HasImplMod = true;
      }
    }
    else
      CmdError.CommandError(IllegalFileType);
    
  SynDefFile += ".syn";
  while (ModuleName.Contains(slash,0,loc))
    ModuleName = ModuleName.Substr(loc+1,0);

  if (Arguments.ArgCount > i+1)
    CmdError.CommandError(TooManyParms);

}
