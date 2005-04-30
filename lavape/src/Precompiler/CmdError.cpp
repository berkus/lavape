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


#include "CmdError.h"


CmdErrorCLASS CmdError;
static bool __INITstarted=false;


void CmdErrorCLASS::CommandError (CmdErrType cmdErr)

{
  IO.CloseOutput();
  IO.WriteLn();
  IO.WriteLn();
  switch (cmdErr) {

  case IncompleteFileid:
    IO.WriteString("+++++ pp: name of source file is missing or incomplete");
    break;

  case TooManyParms:
    IO.WriteString("+++++ pp: too many command parameters");
    break;

  case MissingParms:
    IO.WriteString("+++++ pp: command parameters missing");
    break;

  case IllegalFileType:
    IO.WriteString("+++++ pp: source file has illegal filetype/extension");
    break;

  case UnknownOption:
    IO.WriteString("+++++ pp: unknown option in command parameters");
    break;

  case OpenErrorOutput:
    IO.WriteString("+++++ pp: output file couldn't be opened");
    break;

  case OpenErrorSource:
    IO.WriteString("+++++ pp: source file couldn't be opened");
    break;

  case SourceEmpty:
    IO.WriteString("+++++ pp: source file is empty");
    break;
  }
  IO.WriteLn();
  exit(1);
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void CmdErrorCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  IO.INIT();
}
