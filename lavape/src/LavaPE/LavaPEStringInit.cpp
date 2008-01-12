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


#include "qobject.h"
#include "qstring.h"
#include "LavaPEStringInit.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
//
// String Table

QString IDP_UndefinedTypes;
QString IDP_AlreadyIncluded;
QString IDP_InclInherited;
QString IDP_NoTypeSel;
QString IDP_NoEvTypeSel;
QString IDP_NoElemSel;
QString IDP_IsNoID;
QString IDP_NoTableView;
QString IDP_NoFieldInsertion;
QString IDP_NoFormView;
QString IDP_NoExecView;
QString IDP_NoInsertionPos;
QString IDP_NoDefaultForm;
QString IDP_NoDefFound;
QString IDP_DefaultFormName;
QString IDP_DefaultFormName2;
QString IDP_SelInterface;
QString IDP_NoEnumItem;
QString IDP_SaveFirst;
QString IDP_NoOpenStr;
QString IDP_NoCompoProt;
QString IDP_RefNotFound;


void LavaPEStringInit() {
  IDP_UndefinedTypes = QObject::tr("There are still undefined references in use");
  IDP_AlreadyIncluded = QObject::tr("This file is already included");
  IDP_InclInherited = QObject::tr("You cannot remove this include file because it is inherited from ");
  IDP_NoTypeSel = QObject::tr("You must select a type for the new item");
  IDP_NoEvTypeSel = QObject::tr("You must select an event type for the new handler");
  IDP_NoElemSel = QObject::tr("No selection");
  IDP_IsNoID = QObject::tr("This is not a valid identifier.");
  IDP_NoTableView = QObject::tr("No table view available for the selected item");
  IDP_NoFieldInsertion = QObject::tr("You cannot insert a type or field at the selected position");
  IDP_NoFormView  = QObject::tr("No form view available for the selected item");
  IDP_NoExecView = QObject::tr("No invariant available for the selected item");
  IDP_NoInsertionPos = QObject::tr("No item selected which indicate the insertion postion");
  IDP_NoDefaultForm = QObject::tr("The selected source interface has no valid form to derived from.");
  IDP_NoDefFound = QObject::tr("Missing declaration of referenced type");
  IDP_DefaultFormName = QObject::tr("Would you like to override the existing default form?");
  IDP_DefaultFormName2 = QObject::tr("Cannot insert the default form, because there exists an object with the F__xxx  name.");
  IDP_SelInterface = QObject::tr("Please select the interface(s) to be implemented by this implementation");
  IDP_NoEnumItem = QObject::tr("Please define the items of the enumeration type");
  IDP_SaveFirst = QObject::tr("Please save the current Lava document first");
  IDP_NoOpenStr = QObject::tr("Please enter the open string of the data base");
  IDP_NoCompoProt = QObject::tr("Please select the component protocol");
  IDP_RefNotFound = QObject::tr("Cannot show the listed reference ");

}
