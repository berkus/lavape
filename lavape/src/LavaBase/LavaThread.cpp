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


#include "LavaBase_all.h"
#include "BASEMACROS.h"

#include "LavaThread.h"
#include "LavaBaseDoc.h"

#pragma hdrstop


CLavaThread::CLavaThread(CLavaBaseDoc* docu)
{
	myDoc = docu;
  mySemaphore.acquire();
  if (!myDoc->ThreadList)
		myDoc->ThreadList = new CThreadList;
}

CLavaDbgBase::CLavaDbgBase(CLavaBaseDoc* docu)
{
	myDoc = docu;
  isRunning = false;
  if (!myDoc->ThreadList)
		myDoc->ThreadList = new CThreadList;
}
