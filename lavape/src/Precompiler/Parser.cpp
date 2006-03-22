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


#include "Parser.h"

#pragma hdrstop


ParserCLASS Parser;
static bool __INITstarted=false;


/* **********************************************************************/

void ParserCLASS::Parser ()

{
  DParser.ImportStatements();

  DParser.TypeAndFormStatements();

  if (IntCode.IncludeLevel) return;
  
  while (Atom != Eof) {
    switch (Atom) {

    case Call:
    case Callsuspend:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.CallMacro();
      break;

    case CheckA:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.CheckMacro();
      break;

    case Checkpoint:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.CheckpointMacro();
      break;

    case Drop:
      XParser.DropMacro();
      break;

    case Get:
    case Getsuspend:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.GetMacro();
      break;

    case Getprivate:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.GetPrivMacro();
      break;

    case Orgunit:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.OrgunitMacro();
      break;

    case Procedure:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.ProcedureMacro();
      break;

    case Put:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.PutMacro();
      break;

    case Putprivate:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.PutPrivMacro();
      break;

    case Return:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.ReturnMacro();
      break;

    case Show:
    case ShowEnc:
      IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
      XParser.ShowMacro();
      break;

    case Start:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.StartMacro();
      break;

    case StopA:
      IntCode.GlobalProperties.Flags.INCL(OrcServUsed);
      XParser.StopMacro();
      break;

    default:
      if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
        PCerrors.Expected("TYPE | FORM");
      else
        PCerrors.Expected("executable preprocessor macro");
    }
    DParser.NonPPstatements();
  }
  IntCode.CheckAllTypesDefined();
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void ParserCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  ContextCK.INIT();
  DParser.INIT();
  XParser.INIT();
  Scanner_INIT();
  Atoms_INIT();
  IntCode.INIT();
  PCerrors.INIT();
}
