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

/* This file has been taken over from www.wxwindows.org
   and adapted to the needs of LavaPE */

/*
#ifdef __GNUC__
#pragma implementation "wx_obj.h"
#endif
*/

/*
 * File:  wb_obj.cc
 * Purpose: wxObject base class implementation
 * Author:  Julian Smart
 * Created: 1993
 * Updated: August 1994
 * RCS_ID:      $Id$
 * Copyright: (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_obj.cc  1.2 5/9/94"; */


// For compilers that support precompilation, includes "wx.h".
//#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx_obj.h"
#include "string.h"
#include "qapplication.h"


// Hand-coded IMPLEMENT... macro for wxObject (define static data)
wxClassInfo wxObject::classwxObject("wxObject", NULL, NULL, sizeof(wxObject), NULL);
wxClassInfo *wxClassInfo::first = NULL;

// Useful buffer, initialized in wxCommonInit
char *wxBuffer = NULL;

const char *wxFatalErrorStr = "wxWindows Fatal Error";


/*
 * wxWindows root object.
 */

wxObject::wxObject(void)
{
//  WXSET_TYPE(wxObject, wxTYPE_ANY)

  refCount = 0;
}

wxObject::~wxObject(void)
{
}


/*
 * Class info: provides run-time class type information.
 */
 
wxClassInfo::wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn constr)
{
  className = cName;
  baseClassName1 = baseName1;
  baseClassName2 = baseName2;

  objectSize = sz;
  objectConstructor = constr;
  
  next = first;
  first = this;

  baseInfo1 = NULL;
  baseInfo2 = NULL;
}

wxObject *wxClassInfo::CreateObject(void)
{
  if (objectConstructor)
    return (wxObject *)(*objectConstructor)();
  else
    return NULL;
}

wxClassInfo *wxClassInfo::FindClass(char *c)
{
  wxClassInfo *p = first;
  while (p)
  {
    if (p && p->GetClassName() && strcmp(p->GetClassName(), c) == 0)
      return p;
    p = p->next;
  }
  return NULL;
}

wxObject *wxCreateDynamicObject(char *name)
{
  wxClassInfo *info = wxClassInfo::first;
  while (info)
  {
    if (info->className && strcmp(info->className, name) == 0)
      return info->CreateObject();
    info = info->next;
  }
  return NULL;
}
