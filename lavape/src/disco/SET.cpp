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

#include "SYSTEM.h"
#include "stdio.h"
#include "stdarg.h"
//#include "disco.h"


SET::SET (int arg1 ...)
{
  va_list ap;
  int member;
  
  bits = 0;
  if (arg1 < 0) return;
  else INCL(arg1);
  
  va_start(ap,arg1);
  for (;;) {
    member = va_arg(ap,int);
    if (member < 0) {
      va_end(ap);
      return;
    }
    INCL(member);
  }
}  


void SET::print ()
{
  unsigned long shiftedBits, nBits;
  bool first=true;
  
  shiftedBits = bits;
  printf("SET(");
  nBits = sizeof(long)*8;
  for (unsigned i=0; i<nBits; i++) {
    if (shiftedBits & 1) {
      if (first)
        printf("%d",i);
      else
        printf(",%d",i);
      first=false;
    }
    shiftedBits = shiftedBits>>1;
  }
  if (first)
    printf(")\n");
  else
    printf(",-1)\n");
}

