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

#include "Syntax.h"
//#include "disco.h"


CLavaError::CLavaError(CHAINX* errors, QString* ids, DString *text, bool autoC)
{
  CHE* che = 0;
  if (!text && errors->first) {
    for (che = (CHE*)errors->first; che && (((CLavaError*)che->data)->IDS != ids); che = (CHE*)che->successor);
    if (che)
      errors->Remove(che->predecessor);
  }
  IDS = ids;
  showAutoCorrBox = autoC;
  if (text)
    textParam = *text;
  che = new CHE(this);
  errors->Append(che);
}

bool RemoveErrCode(CHAINX* errors, QString* ids)
{
  CHE *che, *pred=0;
  bool removed = false;
  for (che = (CHE*)errors->first; che; ) {
    if (((CLavaError*)che->data)->IDS == ids) {
      pred = (CHE*)che->predecessor;
      errors->Remove(pred);
      removed = true;
      if (pred)
        che = (CHE*)pred->successor;
      else
        che = 0;
    }
    else
      che = (CHE*)che->successor;
  }
  return removed;
}

LavaDECL *NewLavaDECL()
  { return new LavaDECL; }

TAnnotation *NewTAnnotation()
  { return new TAnnotation; }

LavaDECL::~LavaDECL() {
  if (SectionTabPtr && (DeclType == Interface)) {
		SectionTabPtr->Destroy();
    delete [] SectionTabPtr;
	}
}

LavaDECL::LavaDECL() {
  DeclDescType = StructDesc;
  fromBType = NonBasic;
  BType = NonBasic;
  ParentDECL = 0;
  OwnID = -1; //1000000;
  inINCL = 0;
  DeclType = NoDef;
  RefID.nID = -1;
  nInput = 0;
  nOutput = -1; //LB_ERR;
  op = OP_noOp;
  VElems.UpdateNo = 0;
  RuntimeDECL = 0;  
  RelatedDECL = 0;
  SectionTabPtr = 0;   //used at run time in implementations
  SectionInfo1 = -1;
  SectionInfo2 = -1;
}   

void LavaDECL::ResetCheckmarks()
{
  WorkFlags.EXCL(checkmark);
  CHE* che;
  che = (CHE*)NestedDecls.first;
  while (che) {
    ((LavaDECL*)che->data)->ResetCheckmarks();
    che = (CHE*)che->successor;
  }
}


void LavaDECL::CompleteFullName()
{
  if (FullName.l && LocalName.l)
    FullName = FullName + DString("::") + LocalName;
  else
    FullName = LocalName;
}


bool LavaDECL::isInSubTree(LavaDECL * ancestor)
{
  LavaDECL* upDECL = this; 
  while (upDECL && (upDECL != ancestor))
    upDECL = upDECL->ParentDECL;
  return (upDECL != 0);

}


bool LavaDECL::usableIn(LavaDECL * decl)
{
  LavaDECL* decl1 = this; 
  if (decl1->DeclType == FormDef)
    decl1 = decl1->ParentDECL->ParentDECL;
  while (decl1 && (decl1->DeclType != Impl)
         && ((decl1->DeclType != Package) || !decl1->TypeFlags.Contains(isProtected)))
    decl1 = decl1->ParentDECL;
  if (decl1)
    return decl->isInSubTree(decl1);
  else
    return true;
}

int LavaDECL::GetAppendPos(TDeclType declType)
{
  CHE *che;
  int pos;
  switch (declType) {
  case VirtualType:
    pos = 1; 
    for (che = (CHE*)NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType == VirtualType);
         che = (CHE*)che->successor)
      pos++;
    break;
  case Interface:
  case Impl:
  case Initiator:
  case Package:
  case Component:
  case CompObjSpec:
  case CompObj:
    for (che = (CHE*)NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType != Interface)
             && (((LavaDECL*)che->data)->DeclType != Impl)
             && (((LavaDECL*)che->data)->DeclType != Initiator)
             && (((LavaDECL*)che->data)->DeclType != Package)
             && (((LavaDECL*)che->data)->DeclType != Component)
             && (((LavaDECL*)che->data)->DeclType != CompObjSpec)
             && (((LavaDECL*)che->data)->DeclType != CompObj);
         che = (CHE*)che->successor);
    for (pos = 1;
         che && ( (((LavaDECL*)che->data)->DeclType == Interface)
                 || (((LavaDECL*)che->data)->DeclType == Impl)
                 || (((LavaDECL*)che->data)->DeclType == Initiator)
                 || (((LavaDECL*)che->data)->DeclType == Package)
                 || (((LavaDECL*)che->data)->DeclType == Component)
                 || (((LavaDECL*)che->data)->DeclType == CompObjSpec)
                 || (((LavaDECL*)che->data)->DeclType == CompObj));
         che = (CHE*)che->successor)
      pos++;
    break;
  case Function:
  case Attr:
  case FormText:
    for (che = (CHE*)NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType != Function)
             && (((LavaDECL*)che->data)->DeclType != Attr)
             && (((LavaDECL*)che->data)->DeclType != FormText);
         che = (CHE*)che->successor);
    for (pos = 1;
         che && ( (((LavaDECL*)che->data)->DeclType == Function)
                 || (((LavaDECL*)che->data)->DeclType == Attr)
                 || (((LavaDECL*)che->data)->DeclType == FormText));
         che = (CHE*)che->successor)
      pos++;
    break;
  case IAttr:
    for (che = (CHE*)NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType != IAttr);
         che = (CHE*)che->successor);
    for (pos = 1;
         che && (((LavaDECL*)che->data)->DeclType == IAttr);
         che = (CHE*)che->successor)
      pos++;
    break;
  case OAttr:
    for (che = (CHE*)NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType != IAttr)
             && (((LavaDECL*)che->data)->DeclType != OAttr);
         che = (CHE*)che->successor);
    for (pos = 1;
         che && ( (((LavaDECL*)che->data)->DeclType == IAttr)
                 || (((LavaDECL*)che->data)->DeclType == OAttr));
         che = (CHE*)che->successor)
      pos++;
    break;
  }
  return pos;
}

/*
bool LavaDECL::usableInA(LavaDECL * decl)
{
  LavaDECL* decl1 = this; 
  if (decl1->DeclType == FormDef)
    decl1 = decl1->ParentDECL->ParentDECL;
  if (decl1->DeclType == VirtualType)
    decl1 = decl1->ParentDECL;
  else
    while (decl1 && (decl1->DeclType != Impl)
           && ((decl1->DeclType != Package) || !decl1->TypeFlags.Contains(isProtected)))
      decl1 = decl1->ParentDECL;
  if (decl1)
    return decl->isInSubTree(decl1);
  else
    return true;
}
*/

/*
void LavaDECL::DeleteContents() {
  FullName.Destroy();
  Annotation.Destroy();
  switch (DeclDescType) { 
    case BasicType:
      if (BType == Enumeration)
        Items.Destroy();
      break;

    case StructDesc:
      NestedDecls.Destroy();
      break;
    case EnumType:
      EnumDesc.Destroy();
      break;
    case NamedType:
      //SyntaxName.Destroy();
      //TypeName.Destroy();
      break;
    case LiteralString:
      LitStr.Destroy();
      break;
  }
}
*/
    
TAnnotation::TAnnotation()
{
  myDECL = 0;
  Length.FieldBoundRel = EQ;
  Length.Field = 1;
  Length.DataBoundRel = GE;
  Length.Data = 0;
  Length.DecPoint = 0;
  BType = NonBasic;
  I = 0;
  B = false;
  Space = 0;
  TabPosition = -1;
  FrameSpacing = -1;
  Indentation = 0;
  Emphasis = Low;
  IndType = RelIndent;
  JustType = LeftJustified;
  Alignment = UnAligned;
  Box = NoRules;
}

CHETAnnoEx* TAnnotation::GetAnnoEx(AnnoExType type, bool makeIt)
{
  CHETAnnoEx* ex = (CHETAnnoEx*)AnnoEx.first;
  while (ex && (ex->data.exType != type))
    ex = (CHETAnnoEx*)ex->successor;
  if (!ex && makeIt) {
    ex = new CHETAnnoEx;
    ex->data.exType = type;
    AnnoEx.Append(ex);
  }
  return ex;
}

void TAnnotation::DelAnnoEx(AnnoExType type)
{
  CHETAnnoEx* ex = (CHETAnnoEx*)AnnoEx.first;
  while (ex && (ex->data.exType != type))
    ex = (CHETAnnoEx*)ex->successor;
  if (ex)
    AnnoEx.Remove(ex->predecessor);
}

FormNode::~FormNode()
{
  if (Pixmap)
    delete Pixmap;
}

unsigned FormNode::GetLengthField()
{
  if (Annotation.ptr && Annotation.ptr->myDECL)
    return ((TAnnotation*)Annotation.ptr->myDECL->Annotation.ptr->FA.ptr)->Length.Field;
  else
    return ((TAnnotation*)FormSyntax->Annotation.ptr->FA.ptr)->Length.Field;
}

unsigned FormNode::GetLengthDecPoint()
{
  if (Annotation.ptr && Annotation.ptr->myDECL)
    return ((TAnnotation*)Annotation.ptr->myDECL->Annotation.ptr->FA.ptr)->Length.DecPoint;
  else
    return ((TAnnotation*)FormSyntax->Annotation.ptr->FA.ptr)->Length.DecPoint;
}

bool LavaDECL::HasDefaultInitializer()
{
  CHE *che;

  if (DeclType != Interface)
    return false;
  for (che = (CHE*)NestedDecls.first; che; che = (CHE*)che->successor)
    if (((LavaDECL*)che->data)->TypeFlags.Contains(defaultInitializer))
      return true;
  return false;
}
