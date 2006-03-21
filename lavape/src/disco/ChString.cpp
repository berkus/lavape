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


#include "ChString.h"
#pragma hdrstop


bool ChainOfString::Contains (const DString& elem)

{
  CHEString *next;
  
  for (next = (CHEString*)first;
       next && (next->data != elem);
       next = (CHEString*)next->successor);
  if (next) return true;
  else return false;
}


bool ChainOfString::AppendIfMissing (const DString& elem)

{
  CHEString *newElem;
  
  if (Contains(elem)) return false;
  else {
    newElem = new CHEString;
    newElem->data = elem;
    Append(newElem);
    return true;
  }
}

