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
#include "SETpp.h"
#include <stdarg.h>
//#include "disco.h"


SETpp::SETpp (int arg1 ...)
{
  va_list ap;
  int member;
  
  bits = 0;
  if (arg1 < 0) return;
  else INCL(arg1);
  
  va_start(ap,arg1);
  for (;;) {
    member = va_arg(ap,int);
    if (member < 0) return;
    INCL(member);
  }
  va_end(ap);
}  


void SETpp::CDP (PutGetFlag pgf, ASN1 *cid)
{
  if (pgf == PUT) cid->PUTunsigned(bits);
  else cid->GETunsigned(bits);
}

