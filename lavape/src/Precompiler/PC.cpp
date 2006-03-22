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



#include "SYSTEM.h"
#include "Args.h"

#include "Parser.h"
#include "Coder.h"
#include "PCerrors.h"

#pragma hdrstop



int main (unsigned argc, char **argv)
{
  Arguments.ArgCount = argc;
  Arguments.ArgTable = argv;

#ifndef WIN32
  if (access("pause.file",4) == 0)
    pause();
#endif
  Parser.INIT();
  Coder.INIT();
  PCerrors.INIT();

  Parser.Parser();
  Coder.Coder();
  if (PCerrors.ErrorCount > 0) exit(1);
  exit(0);
  return 0;
}
