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


#include "UNIX.h"
#include "SYSTEM.h"
#include <stdlib.h>
#include "Halt.h"
#pragma hdrstop


HaltCLASS Halt;

static epiProc epiProcs=0;


/************************************************************************/

void HaltCLASS::DeclareEpilogue (DPROC epilogueProc)

{
  epiProc newEpiProc;

  newEpiProc = new epiProcDesc;
  newEpiProc->epiProcedure = epilogueProc;
  newEpiProc->nextEpiProc = epiProcs;
  epiProcs = newEpiProc;
}


void HaltCLASS::Epilogue ()
/* calls the epilogue procedures which have been declared previously */

{
  while (epiProcs != 0) {
    epiProcs->epiProcedure();
    epiProcs = epiProcs->nextEpiProc;
  }
}



void HaltCLASS::Halt ()
/* terminates execution after having called the epilogue procedures which
   have been declared previously */

{
  Epilogue();
  exit(0);
}


void HaltCLASS::HaltPMD ()
/* terminates execution after having called the epilogue procedures which
   have been declared previously. Finally PMDump.PostMortemDump is called. */

{
  Epilogue();
  PMDump.PostMortemDump();
}
