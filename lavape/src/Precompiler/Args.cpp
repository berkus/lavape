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


/**********************************************************************

               implementation of class ArgumentsCLASS

 **********************************************************************/

#include "Args.h"
//#include "disco.h"


ArgumentsCLASS Arguments;
static bool __INITstarted=false;

extern "C" {
  unsigned Arguments_argc;
  char **Arguments_argv;
}


/************************************************************************/

void ArgumentsCLASS::GetArg (unsigned argnum,
           DString& str)
/* Returns the value and length of the 'argnum'th argument
   * on the command line. The command name is argument number zero.
   * If 'length' is equal to the size of 'arg' then 'arg' wasn't
   * long enough.
   * If 'length' is zero, the argument was not found.
   * Note: 'ArgCount' = n  ==>  arguments 0..n-1 exist
*/

{
  if (argnum >= ArgCount) {
    str.Reset(0);
    return;
  }
  str = DString(ArgTable[argnum]);
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void ArgumentsCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Arguments_argc = ArgCount;
  Arguments_argv = ArgTable;
}
