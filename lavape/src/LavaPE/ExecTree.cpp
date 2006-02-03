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


#include "LavaPE.h"
#include "ExecTree.h"
#include "VTView.h"
#include "LavaAppBase.h"
//#include "Constructs.h"
#include "LavaBaseStringInit.h"
#include "Resource.h"
#include "LavaPEView.h"

#include "Convert.h"

#include "SylTraversal.h"
#include "qpixmapcache.h"
//Added by qt3to4:
#include <QPixmap>



CExecTree::CExecTree(CLavaPEView *tree, bool autoBox, bool finalUpdate, int check)
{
  viewTree = tree;
  if (check < CHLV_noCheck)
    checkLevel = CHLV_noCheck;
  else
    checkLevel = check;
  Doc = ((CLavaPEDoc*)viewTree->GetDocument());
  Travers = new CSylTraversal(this, Doc->mySynDef);
  ActItem = 0;
  ParItem = 0;
  ActLevel = 0;
  skipLevel = 10000;
  downAwaited = false;
  FinalUpdate = finalUpdate;
  useAutoBox = autoBox;
  Doc->hasErrorsInTree = false;
}

bool CExecTree::CalcPos(int level)
{
  if (skipLevel <= level) {
    isOnSkip = true;
    return false; //auslassen
  }
  skipLevel = 100000;
  isOnSkip = false;
  if (!level && !ParItem)  //the top decl
    return true;
  //if (downAwaited && (level<= ActLevel) && allNodes) 
  //  CTreeItem* item = getSectionNode(ParItem, ExecDef);
  CMainItemData *data;
  if (downAwaited && (level<= ActLevel))  {
    if (!ParItem->firstChild()) {
      data = (CMainItemData*)ParItem->getItemData();
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt))
        CTreeItem* item = getSectionNode(ParItem, ExecDef);
    }
  }
  downAwaited = false;
  if (ActLevel > level) {
    while (ActLevel > level) {
      ActItem = (CTreeItem*)ActItem->parent();//GetParentItem(ActItem);
      data = (CMainItemData*)ActItem->getItemData();
      if ( (data->type != TIType_EnumItems)
         && (data->type != TIType_VTypes)
         && (data->type != TIType_Input)
         && (data->type != TIType_Output)
         && (data->type != TIType_Defs)
         && (data->type != TIType_Features)
         && (data->type != TIType_Require)
         && (data->type != TIType_Ensure)
         && (data->type != TIType_Exec)
         ) 
        ActLevel -= 1;
    }
    ParItem = (CTreeItem*)ActItem->parent();
    data = (CMainItemData*)ParItem->getItemData();
      if ( (data->type != TIType_EnumItems)
         && (data->type != TIType_VTypes)
         || (data->type == TIType_Input)
         || (data->type == TIType_Output)
         || (data->type == TIType_Defs)
         || (data->type == TIType_Features)
         || (data->type == TIType_Require)
         || (data->type == TIType_Ensure)
         || (data->type == TIType_Exec)
         )
      ParItem = (CTreeItem*)ParItem->parent();
  }
  else
    if (ActLevel < level) {
      ParItem = ActItem;
      ActItem = 0;
    }
  ActLevel = level;
  return true;
}


void CExecTree::ExecDefs(LavaDECL ** pelDef, int level)
{
  if (!CalcPos(level))
    return;
  LavaDECL *decl, *decl2, *elDef = *pelDef;
  QString* errCode;
  QPixmap *bm=0;
  DString lName, lab1, lab = elDef->LocalName;
  QString cstr;
  bool catErr;
  TID IOid;
  SynFlags typeFlags;
  TreeFlag secNode = DefsExpanded;
  CHETID *cheImpl, *nextImpl;

  if (FinalUpdate) {
    elDef->DECLError1.Destroy();
    elDef->DECLError1 = elDef->DECLError2;
    elDef->DECLError2.Destroy();
  }
  if (elDef->DeclType != CompObjSpec) {//save the component protocol
    elDef->nInput = 0;
    elDef->nOutput = 0;
  }
  switch (elDef->DeclType) {

  case VirtualType:
    if (elDef->ParentDECL->DeclType != FormDef) {
      elDef->WorkFlags.INCL(isPartOfPattern);
      errCode = Doc->CheckScope(elDef);
      if (errCode)
        new CLavaError(&elDef->DECLError1, errCode);
    }
    typeFlags = elDef->TypeFlags;
    Doc->GetCategoryFlags(elDef, catErr);
    if (catErr)
      new CLavaError(&elDef->DECLError1, &ERR_IncompatibleCategory);
    if ((typeFlags != elDef->TypeFlags) && (checkLevel != CHLV_fit)) {
      elDef->TypeFlags = typeFlags;
      new CLavaError(&elDef->DECLError1, &ERR_OverriddenDiffs);
    }
    if (elDef->TypeFlags.Contains(isAbstract)) {
      if (!elDef->ParentDECL->TypeFlags.Contains(isAbstract)
          && (elDef->ParentDECL->DeclType != Package))
        new CLavaError(&elDef->DECLError1, &ERR_NoAbstract);
      if (elDef->TypeFlags.Contains(stateObject))
        lab += DString("~");
      else if (elDef->TypeFlags.Contains(isAnyCategory))
        lab += DString("*");
    }
    else {
      lab += DString(" = ");
      if (elDef->TypeFlags.Contains(substitutable))
        lab += DString("{");
      if (elDef->TypeFlags.Contains(stateObject))
        lab += DString("~");
      else if (elDef->TypeFlags.Contains(isAnyCategory))
        lab += DString("*");
      if (elDef->DeclDescType == NamedType) {
        decl2 = Doc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
        if (decl2) {
          lab += Doc->GetTypeLabel(elDef, true);
          if (errCode = Doc->TestValOfVirtual(elDef, decl2))
            new CLavaError(&elDef->DECLError1, errCode);
        }
        else {
          lab += DString("??");
          new CLavaError(&elDef->DECLError1, &ERR_NoRefType);
        }
      }
      else 
        lab += Doc->GetTypeLabel(elDef, true);
      if (elDef->TypeFlags.Contains(substitutable))
        lab += DString("}");
    }
    if (errCode = Doc->CommonContext(elDef))
      new CLavaError(&elDef->DECLError1, errCode);
    secNode = ParaExpanded;
    break;
  case Function:
    if (checkLevel != CHLV_noCheck) {
      errCode = Doc->CheckException(elDef, 0);
      if (errCode)
        new CLavaError(&elDef->DECLError1, errCode);
    }
    decl = elDef;
    if (decl->Supports.first && (decl->DeclType == Function))
      if (elDef->ParentDECL->DeclType == Impl)
        decl = Doc->IDTable.GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
      else
        while (decl && decl->Supports.first && (decl->DeclType == Function)) 
          decl = Doc->IDTable.GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
    if (decl) {
      if (decl != elDef) {
        TID id1 = TID(elDef->ParentDECL->OwnID, 0);
        TID id2 = TID(decl->ParentDECL->OwnID, decl->inINCL);
        if (Doc->IDTable.IsAn(id1,0,id2,0)) {
          if (elDef->TypeFlags.Contains(isPropGet) || elDef->TypeFlags.Contains(isPropSet)) {
            if (elDef->TypeFlags.Contains(isPropGet))
              elDef->LocalName = DString("Get_") + decl->LocalName;
            else
              elDef->LocalName = DString("Set_") + decl->LocalName;
            if (!decl->TypeFlags.Contains(hasSetGet))
              new CLavaError(&elDef->DECLError1, &ERR_NoSetGetMember);

            IOid = TID(((LavaDECL*)((CHE*)elDef->NestedDecls.first)->data)->OwnID, 0);
            ((CLavaPEApp*)wxTheApp)->ExecUpdate.MakeSetGet(Doc, Doc->GetExecDECL(*pelDef,ExecDef,true,false),
                                           ((CHETID*)elDef->Supports.first)->data, IOid);
          }
          else {
            if (!elDef->SecondTFlags.Contains(enableName))
              elDef->LocalName = decl->LocalName;
            elDef->op = decl->op;
          }
          if (lab != elDef->LocalName) {
            lab = elDef->LocalName;
            Doc->changeInUpdate = true;
          }
          elDef->FullName = elDef->ParentDECL->FullName + ddppkt + elDef->LocalName;
        }
        else
          if (elDef->SecondTFlags.Contains(overrides))
            new CLavaError(&elDef->DECLError1, &ERR_NoOverridden);
          else
            new CLavaError(&elDef->DECLError1, &ERR_MissingFuncDecl);

      }
    }
    if (elDef->SecondTFlags.Contains(funcImpl)) {
      if (elDef->TypeFlags.Contains(isProtected))
        lab1 += DString("protected ");
      if (elDef->TypeFlags.Contains(isStatic))
        lab1 += DString("static ");
      else {
        if (elDef->TypeFlags.Contains(isConst))
          lab1 += DString("read-only ");
        if (elDef->TypeFlags.Contains(defaultInitializer))
          lab1 += DString("default ");
        if (elDef->TypeFlags.Contains(isInitializer))
          lab1 += DString("initializer ");
      }
      if (elDef->TypeFlags.Contains(isPropGet))
        lab1 += DString("attribute get function");
      else
        if (elDef->TypeFlags.Contains(isPropSet))
          lab1 += DString("attribute set function");
        else
          if (elDef->op == OP_noOp)
            lab1 += DString("function");
      decl = Doc->IDTable.GetDECL(((CHETID*)elDef->Supports.first)->data, elDef->inINCL);
      if (decl && decl->TypeFlags.Contains(isAbstract))
        new CLavaError(&elDef->DECLError1, &ERR_ImplOfAbstract, 0, useAutoBox);
      if (decl && decl->TypeFlags.Contains(isNative))
        new CLavaError(&elDef->DECLError1, &ERR_NoImplForAbstract, 0, useAutoBox);
    }
    else {
      if (checkLevel != CHLV_noCheck) {
        bool changed = Doc->CheckOverInOut(elDef, checkLevel);
        Doc->changeInUpdate = Doc->changeInUpdate || changed;
      }
      if (elDef->SecondTFlags.Contains(isLavaSignal))
        lab1 += DString("signal");
      else {
        if (elDef->TypeFlags.Contains(isNative))
          lab1 += DString("native ");
        if (elDef->TypeFlags.Contains(isAbstract)) {
          lab1 += DString("abstract ");
          if (!elDef->ParentDECL->TypeFlags.Contains(isAbstract))
            new CLavaError(&elDef->DECLError1, &ERR_NoAbstract, 0, useAutoBox);
        }
        if (elDef->TypeFlags.Contains(isProtected))
          lab1 += DString("protected ");
        if (elDef->ParentDECL->DeclType == Impl) 
          lab1 += DString("private ");
        if (elDef->TypeFlags.Contains(isStatic))
          lab1 += DString("static ");
        else
          if (elDef->TypeFlags.Contains(isConst))
            lab1 += DString("read-only ");
        if (elDef->TypeFlags.Contains(defaultInitializer)) {
          if ((elDef->ParentDECL->DeclType == Interface)
              && elDef->ParentDECL->WorkFlags.Contains(hasDefaultIni))
            new CLavaError(&elDef->DECLError1, &ERR_SecondDefaultIni);
          elDef->ParentDECL->WorkFlags.INCL(hasDefaultIni);
          lab1 += DString("default ");
        }
        if (elDef->TypeFlags.Contains(isInitializer))
          lab1 += DString("initializer ");
        if (elDef->op == OP_noOp)
          lab1 += DString("function");
      }
    }
    if (lab1.l) {
      if (elDef->op == OP_noOp)
        lab += DString(" := ");
      else
        lab += DString(" : ");
      lab1[0] -= 'a'-'A';
      lab = lab + lab1;
    }
    secNode = MemsExpanded;

    break;
  case Impl:
    elDef->WorkFlags.EXCL(hasDefaultIni);
    errCode = Doc->CheckScope(elDef);
    if (errCode)
      new CLavaError(&elDef->DECLError1, errCode);
    if (checkLevel != CHLV_noCheck)
      Doc->CheckImpl(elDef, checkLevel);
    if (elDef->TypeFlags.Contains(isGUI))
      lab = DString("GUI service implementation of ");
    else
      lab = DString("Implementation of ");
    decl = Doc->IDTable.GetDECL(((CHETID*)elDef->Supports.first)->data, elDef->inINCL);
    if (decl) {
      lab += decl->FullName;
      cheImpl = (CHETID*)decl->ImplIDs.first;
      while (cheImpl) {
        decl2 = Doc->IDTable.GetDECL(cheImpl->data);//, decl->inINCL);
        nextImpl = (CHETID*)cheImpl->successor;
        if (!decl2) 
          decl->ImplIDs.Delete(cheImpl);
        cheImpl = nextImpl;
      }
      if (decl->ImplIDs.first != decl->ImplIDs.last)
        new CLavaError(&elDef->DECLError1, &ERR_SecondImpl);
    }
    else {
      lab += "??";
      new CLavaError(&elDef->DECLError1, &ERR_NoImplClass);
    }
    break;
  case Interface:
    elDef->WorkFlags.EXCL(hasDefaultIni);
    if (elDef->NestedDecls.first
      && (((LavaDECL*)((CHE*)elDef->NestedDecls.first)->data)->DeclType == VirtualType))
      elDef->WorkFlags.INCL(isPattern);
    else 
      elDef->WorkFlags.EXCL(isPattern);
    if (elDef->ParentDECL->WorkFlags.Contains(isPartOfPattern) || elDef->ParentDECL->WorkFlags.Contains(isPattern))
      elDef->WorkFlags.INCL(isPartOfPattern);
    else
      elDef->WorkFlags.EXCL(isPartOfPattern);
    errCode = Doc->CheckScope(elDef);
    if (errCode)
      new CLavaError(&elDef->DECLError1, errCode);
    if (elDef->WorkFlags.Contains(recalcVT)
         || (checkLevel == CHLV_fit)) {
      if (Doc->MakeVElems(elDef) )
        elDef->WorkFlags.EXCL(recalcVT);
      else
        elDef->WorkFlags.INCL(recalcVT);
    }
    if (!elDef->WorkFlags.Contains(isPattern) && !Doc->okPattern(elDef))
      new CLavaError(&elDef->DECLError1, &ERR_CommonContext);
    lab += DString(" := ");
    if (elDef->TypeFlags.Contains(isNative))
      lab1 = DString("native ");
    if (elDef->TypeFlags.Contains(thisComponent))
      lab1 += DString("component assembly interface");
    else
      if (elDef->TypeFlags.Contains(isComponent))
        lab1 += DString("component object interface");
      else
        if (elDef->TypeFlags.Contains(isGUI)) {
          if (!Doc->IDTable.GetDECL(elDef->RefID, elDef->inINCL))
            new CLavaError(&elDef->DECLError1, &ERR_NoIFforForm);
          lab1 += DString("GUI service");
        }
        else
          lab1 += DString("interface");
    if (lab1[0] >= 'a') 
      lab1[0] -= 'a'-'A';
    lab = lab + lab1;
    cheImpl = (CHETID*)elDef->ImplIDs.first;
    while (cheImpl) {
      decl = Doc->IDTable.GetDECL(cheImpl->data);
      nextImpl = (CHETID*)cheImpl->successor;
      if (!decl) 
        elDef->ImplIDs.Delete(cheImpl);
      cheImpl = nextImpl;
    }
    if (elDef->ImplIDs.first != elDef->ImplIDs.last)
      new CLavaError(&elDef->DECLError1, &ERR_SecondImpl);
    break;
  case CompObjSpec:
    errCode = Doc->CheckScope(elDef);
    if (errCode)
      new CLavaError(&elDef->DECLError1, errCode);
    lab += DString(" := Component obj. spec., supports ");
    break;
  case CompObj:
    if (checkLevel != CHLV_noCheck) {
      if (!Doc->CheckCompObj(elDef))
        new CLavaError(&elDef->DECLError1, &ERR_noCompoInterf);
    }
    lab += DString(" := Component obj. impl.");
    break;
  case Initiator:
    if (elDef->TypeFlags.Contains(isConst))
      lab += DString(" := Read-only main program");
    else
      lab += DString(" := Main program");
    break;
  case Package:
    errCode = Doc->CheckScope(elDef);
    if (errCode)
      new CLavaError(&elDef->DECLError1, errCode);
    Doc->MakeVElems(elDef);
    if (elDef->NestedDecls.first
      && (((LavaDECL*)((CHE*)elDef->NestedDecls.first)->data)->DeclType == VirtualType))
      elDef->WorkFlags.INCL(isPattern);
    else {
      elDef->WorkFlags.EXCL(isPattern);
      if (!Doc->okPattern(elDef))
        new CLavaError(&elDef->DECLError1, &ERR_CommonContext);
    }
    if (elDef->TypeFlags.Contains(isProtected))
      lab1 += DString("opaque ");
    lab1 += DString("package");
    lab1[0] -= 'a'-'A';
    lab = lab + DString(" := ") +lab1;
    break;
  case Component:
    lab += DString(" := Component");
    break;
  default: ;
  }
  CHETID *cheS;
  AddExtends(elDef, &lab);
  cheS = (CHETID*)elDef->Inherits.first;
  if (cheS) {
    if (elDef->DeclType == Function)
      lab += DString(", throws ");
    else
      if ((elDef->DeclType == Interface) || (elDef->DeclType == CompObjSpec))
        lab += DString(", signals ");
    decl2 = Doc->IDTable.GetDECL(cheS->data, elDef->inINCL);
    if ( decl2)
      lab += decl2->FullName; 
    else {
      lab += DString("??");
      new CLavaError(&elDef->DECLError1, &ERR_NoFiredIF);
    }
    cheS = (CHETID*)cheS->successor;
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl2 = Doc->IDTable.GetDECL(cheS->data, elDef->inINCL);
        if ( decl2)
          lab = lab + DString(", ") + decl2->FullName; //LocalName; //cheS->data.ifaceName;
        else {
          lab += DString(", ??");
          new CLavaError(&elDef->DECLError1, &ERR_NoFiredIF);
        }
      }
      else
        if (elDef->DeclType == Function)
          lab = lab + DString(", ") + DString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[cheS->data.nINCL]); 
      cheS = (CHETID*)cheS->successor;
    }
  }
  CTreeItem* parent = 0;
  if (ParItem)
    if (elDef->DeclType == VirtualType)
      parent = getSectionNode(ParItem, VirtualType);
    else
      if (elDef->DeclType == Function)
        parent = getSectionNode(ParItem, Attr);
      else
        parent = getSectionNode(ParItem, Interface);
  if (!bm) {
    bm = GetPixmap(false, false, elDef->DeclType, elDef->TypeFlags);
    if (elDef->DECLError1.first || elDef->DECLError2.first) 
      Doc->hasErrorsInTree = true;
  }
  MakeItem(lab, bm, parent, pelDef);
}


void CExecTree::AddExtends(LavaDECL* elDef, DString* lab)
{
  LavaDECL *pp;
  CHETID* cheS = (CHETID*)elDef->Supports.first;
  bool withName = true;
  SynFlags typeFlags;
  DString lName = elDef->LocalName;
//  CContext con;
  QString *errID, *ids=0;
  if (cheS) {
    if ((cheS->data.nINCL != 1)  && !Doc->isStd
      ||  (cheS->data.nID != Doc->IDTable.BasicTypesID[B_Object])) {
      if ((elDef->DeclType == Interface) || (elDef->DeclType == Package)) {
        *lab += DString(", extends ");
        ids = &ERR_NoBaseIF;
      }
      else if (elDef->DeclType == CompObjSpec)
        ids = &ERR_CompObjIntfNotFound;
      else if (elDef->SecondTFlags.Contains(overrides)) { //function declaration or virtual type
        *lab += DString(", overrides ");
        if ((elDef->DeclType == VirtualType) 
            || (elDef->DeclType == Attr) 
            || (elDef->DeclType == IAttr) 
            || (elDef->DeclType == OAttr)) {
          if (!Doc->OverriddenMatch(elDef, true))
            if (elDef->DeclType == VirtualType) 
              new CLavaError(&elDef->DECLError1, &ERR_ParIncompatible);
            else
              if (elDef->DeclType == Attr) 
                new CLavaError(&elDef->DECLError1, &ERR_MemIncompatible);
              else
                new CLavaError(&elDef->DECLError1, &ERR_OverriddenIOType);
          else
            if (lName != elDef->LocalName) {
              lab->Delete(0, lName.l);
              lab->Insert(elDef->LocalName, 0);
            }
        }
        ids = &ERR_NoOverridden;
      }
      else if ((elDef->DeclType == Function) 
              || (elDef->DeclType == Impl) 
              || (elDef->DeclType == CompObj)) {
        withName = false;
        ids = &ERR_MissingItfFuncDecl; //&ERR_NoRefType;
      }
      else 
        if ((elDef->DeclType == VirtualType) && (elDef->ParentDECL->DeclType == FormDef))
          return;
      pp = Doc->IDTable.GetDECL(cheS->data, elDef->inINCL);
      if (pp) {
        elDef->WorkFlags.EXCL(allowDEL);
        if ((elDef->DeclType == VirtualType)
             && (elDef->SecondTFlags.Contains(isSet) || elDef->SecondTFlags.Contains(isArray))
             && !pp->TypeFlags.Contains(isAbstract)) 
          if (pp->TypeFlags.Contains(constituent)) {
            elDef->TypeFlags.INCL(constituent);
            elDef->TypeFlags.EXCL(acquaintance);
          }
          else {
            elDef->TypeFlags.EXCL(constituent);
            if (pp->TypeFlags.Contains(acquaintance))
              elDef->TypeFlags.INCL(acquaintance);
            else
              elDef->TypeFlags.EXCL(acquaintance);
          }
        if (withName) 
          if (pp->DeclType == VirtualType) {
            *lab += lthen;
            *lab += pp->FullName;
            *lab += grthen;
          }
          else
            *lab += pp->FullName;
        if ((elDef->DeclType == Interface)
            && elDef->TypeFlags.Contains(isGUI)
            && pp->TypeFlags.Contains(isGUI)) 
          if (!Doc->IDTable.GetDECL(pp->RefID, pp->inINCL))
            new CLavaError(&elDef->DECLError1, &ERR_NoBaseFormIF);
        if ((elDef->DeclType == Interface) && (errID = Doc->ExtensionAllowed(elDef, pp, 0)))
          new CLavaError(&elDef->DECLError1, errID);
        if (pp->TypeFlags.Contains(isStatic) && !elDef->SecondTFlags.Contains(funcImpl) )
          new CLavaError(&elDef->DECLError1, &ERR_OverriddenStatic, 0, useAutoBox);
      }
      else {
        *lab += DString("??");
        if (elDef->DeclType != Impl)
          new CLavaError(&elDef->DECLError1, ids, 0, useAutoBox);
      }
      cheS = (CHETID*)cheS->successor;
      while (cheS) {
        pp = Doc->IDTable.GetDECL(cheS->data, elDef->inINCL);
        if (pp) {
          if ((elDef->DeclType == Interface) && (errID = Doc->ExtensionAllowed(elDef, pp, 0)))
            new CLavaError(&elDef->DECLError1, errID);
          if ((elDef->DeclType == Interface)
              && elDef->TypeFlags.Contains(isGUI)
              && pp->TypeFlags.Contains(isGUI)) 
            if (!Doc->IDTable.GetDECL(pp->RefID, elDef->inINCL))
              new CLavaError(&elDef->DECLError1, &ERR_NoBaseFormIF);
          *lab = *lab + DString(", ");
          if (pp->DeclType == VirtualType) {
            *lab += lthen;
            *lab += pp->FullName;
            *lab += grthen;
          }
          else
            *lab += pp->FullName;
        }
        else {
          *lab += DString(", ??");
          new CLavaError(&elDef->DECLError1, ids, 0, useAutoBox);
        }
        cheS = (CHETID*)cheS->successor;
      }
    }
  }
  else
    elDef->WorkFlags.EXCL(allowDEL);
}

void CExecTree::MakeItem(DString& label, QPixmap* bm, CTreeItem* parent, LavaDECL** pelDef)
{
  CMainItemData *itd, *oldItd;
  LavaDECL *elDef = *pelDef;
  CTreeItem* item;
  if (viewTree->drawTree || elDef->WorkFlags.Contains(newTreeNode)) {
    if (ActItem) 
      ActItem = viewTree->InsertItem(label.c, bm, parent, ActItem);
    else
      if (parent)
        ActItem = viewTree->InsertItem(label.c, bm, parent, TVI_FIRST);
      else
        ActItem = viewTree->InsertItem(label.c, bm, TVI_ROOT, 0);
  }
  else {
    if (ActItem)
      ActItem = (CTreeItem*)ActItem->nextSibling();
    if (!ActItem)
      if (parent)
        ActItem = (CTreeItem*)parent->firstChild();
      else
        ActItem = (CTreeItem*)viewTree->GetListView()->firstChild();
    ActItem->setText(0, label.c);
    ActItem->setPix(bm);
    //ActItem->setPixmap(0, *bm);
    oldItd = (CMainItemData*)ActItem->getItemData();
    delete oldItd;
    for (item = (CTreeItem*)ActItem->firstChild(); item; item = (CTreeItem*)item->nextSibling()) {
      itd = (CMainItemData*)item->getItemData();
      if ((itd->type != TIType_Require) 
        && (itd->type != TIType_Ensure)
        && (itd->type != TIType_Exec)
        && (itd->type != TIType_EnumItems))
        itd->synEl = (DWORD)pelDef;
    }
  }
  elDef->WorkFlags.EXCL(newTreeNode);
  itd = new CMainItemData(TIType_DECL,  (unsigned long) pelDef, elDef->TreeFlags.Contains(isExpanded));
  ActItem->setItemData(itd);
  bool labelEditEnable = !Doc->changeNothing
                         && (elDef->DeclType != Impl)
                         && !elDef->SecondTFlags.Contains(funcImpl)
                         && !elDef->SecondTFlags.Contains(overrides)
                         && (elDef->op == OP_noOp)
                         && (viewTree->myInclView || (elDef->DeclDescType == LiteralString));
  ActItem->setRenameEnabled(0, labelEditEnable);
  bool enableDrag = !elDef->TypeFlags.Contains(thisComponent)
                    && !elDef->TypeFlags.Contains(thisCompoForm);
  if (viewTree->drawTree && FinalUpdate)
    ActItem->setDragEnabled(enableDrag);
  //else 
  //  ActItem->setDragEnabled(false);
  if (elDef->WorkFlags.Contains(selAfter)) {
    viewTree->SelItem = ActItem;
    elDef->WorkFlags.EXCL(selAfter);
    if (elDef->WorkFlags.Contains(selIn)) {
      viewTree->SelType = IAttr;
      elDef->WorkFlags.EXCL(selIn);
    }
    else if (elDef->WorkFlags.Contains(selOut)) {
      viewTree->SelType = OAttr;
      elDef->WorkFlags.EXCL(selOut);
    }
    else if (elDef->WorkFlags.Contains(selPara)) {
      viewTree->SelType = VirtualType;
      elDef->WorkFlags.EXCL(selPara);
    }
    else if (elDef->WorkFlags.Contains(selMems)) {
      viewTree->SelType = Attr;
      elDef->WorkFlags.EXCL(selMems);
    }
    else if (elDef->WorkFlags.Contains(selDefs)) {
      viewTree->SelType = Interface;
      elDef->WorkFlags.EXCL(selDefs);
    }
    else
      viewTree->SelType = NoDef;
  }
}

void CExecTree::ExecMember(LavaDECL ** pelDef, int level)
{
  DString lab, lab0;
  LavaDECL *decl, *elDef = *pelDef, *typeDECL;
  bool catErr;
  TDeclDescType exprType = LiteralString;
  TDeclType defType = NoDef;
  SynFlags typeFlags, flags;
  QString* errCode;
  QPixmap* bm;
  TID refID;
  CTreeItem* parent;

  if (FinalUpdate) {
    elDef->DECLError1.Destroy();
    elDef->DECLError1 = elDef->DECLError2;
    elDef->DECLError2.Destroy();
  }
  if ((elDef->DeclDescType == UnknownDDT)
      || (elDef->DeclDescType == Undefined)
         && (elDef->LocalName.l == 0))
//         && (!viewCase || viewTree->myInclView))
    return;
  if (!CalcPos(level))
    return;
  errCode = Doc->CheckScope(elDef);
  if (errCode)
    new CLavaError(&elDef->DECLError1, errCode);
  if (!elDef->ParentDECL->TypeFlags.Contains(isAbstract)) 
    elDef->TypeFlags.EXCL(isAbstract);
  parent = getSectionNode(ParItem, elDef->DeclType);
  if ((elDef->DeclDescType == Undefined)  && (elDef->LocalName.l == 0)) {
    bm = GetPixmap(false, true, BasicDef);
    lab = DString("Literal");
  }
  else {
    if (elDef->ParentDECL->DeclType != FormDef) {
      typeFlags = elDef->TypeFlags;
      Doc->GetCategoryFlags(elDef, catErr);
      if (catErr)
        new CLavaError(&elDef->DECLError1, &ERR_IncompatibleCategory);
      if ((typeFlags != elDef->TypeFlags) && (checkLevel != CHLV_fit)) {
        elDef->TypeFlags = typeFlags;
        new CLavaError(&elDef->DECLError1, &ERR_OverriddenDiffs);
      }
    }
    lab = elDef->LocalName;
    lab += DString(" : ");
    if (elDef->TypeFlags.Contains(substitutable))
      lab += DString("{");
    if (elDef->TypeFlags.Contains(stateObject))
      lab += DString("~");
    else if (elDef->TypeFlags.Contains(isAnyCategory))
      lab += DString("*");
    lab += Doc->GetTypeLabel(elDef, true);
    if (elDef->TypeFlags.Contains(substitutable))
      lab += DString("}");
    if (elDef->TypeFlags.Contains(sameAsSelf))
      lab += DString(", same category as \"self\"");
    if ( ( elDef->DeclType == Attr) 
         && elDef->Supports.first 
         && elDef->SecondTFlags.Contains(overrides)) {
      decl = Doc->IDTable.GetDECL(((CHETID*)elDef->Supports.first)->data, elDef->inINCL);
      for ( ;decl && decl->Supports.first && (decl->DeclType == Attr);
             decl = Doc->IDTable.GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL));
      if (decl) {
        TID id1 = TID(elDef->ParentDECL->OwnID, elDef->inINCL);
        TID id2 = TID(decl->ParentDECL->OwnID, decl->inINCL);
        if (!Doc->IDTable.IsAn(id1,0,id2,0)) 
          new CLavaError(&elDef->DECLError1, &ERR_NoOverridden);
      }
      else
        new CLavaError(&elDef->DECLError1, &ERR_NoOverridden);
      if (checkLevel != CHLV_noCheck) {
        bool changed = Doc->CheckOverInOut(elDef, checkLevel);
        Doc->changeInUpdate = Doc->changeInUpdate || changed;
      }
      AddExtends(elDef, &lab);
    }
    else {
      if (elDef->ParentDECL->Supports.first && elDef->SecondTFlags.Contains(overrides))
        AddExtends(elDef, &lab0);
      else {
        if (!elDef->ParentDECL->Supports.first
          || !elDef->ParentDECL->SecondTFlags.Contains(funcImpl)) {
          elDef->SecondTFlags.EXCL(overrides);
          elDef->SecondTFlags.EXCL(funcImpl);
          elDef->TypeFlags.EXCL(isProtected);
          elDef->TypeFlags.EXCL(isPropGet);
          elDef->TypeFlags.EXCL(isPropSet);
          elDef->TypeFlags.EXCL(isInitializer);
          elDef->TypeFlags.EXCL(defaultInitializer);
          if (elDef->ParentDECL->DeclType != FormDef)
            elDef->Supports.Destroy();
        }
      }
    }
    if (elDef->TypeFlags.Contains(isProtected))
      lab += DString(", protected ");
    if (elDef->TypeFlags.Contains(isPlaceholder))
      lab += DString(", placeholder ");
    else {
      if (elDef->TypeFlags.Contains(isOptional))
        lab += DString(", optional");
      if ( elDef->DeclType == Attr) {
        if (elDef->TypeFlags.Contains(isConst))
          lab += DString(", read-only ");
        if (elDef->TypeFlags.Contains(acquaintance)) 
          lab += DString(", acquaintance ");
        else
          if (!elDef->TypeFlags.Contains(constituent))
            lab += DString(", reverse ref ");
      }
    }   
    if (elDef->TypeFlags.Contains(hasSetGet))
      lab += DString(", access via set/get");
    defType = ((CLavaPEApp*)wxTheApp)->Browser.GetCategory(Doc->mySynDef, elDef, flags);
    refID = elDef->RefID;
    typeDECL = Doc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
    if (typeDECL) {
      /*
      if (typeDECL->TypeFlags.Contains(isAbstract))
        if (elDef->DeclType == Attr) {
          if (!elDef->ParentDECL->TypeFlags.Contains(isAbstract)) 
            new CLavaError(&elDef->DECLError1, &ERR_AbstrMemType);
        }
        else
          if (!elDef->ParentDECL->ParentDECL->TypeFlags.Contains(isAbstract)) 
            new CLavaError(&elDef->DECLError1, &ERR_AbstrMemType);
      */

      if (typeDECL->DeclType == Interface) {
        if (errCode = Doc->TypeForMem(elDef, typeDECL, 0))
          new CLavaError(&elDef->DECLError1, errCode);
      }
      else
        if (defType == VirtualType) {
          if (checkLevel != CHLV_noCheck)
            if (!Doc->CheckGetFinalMType(elDef))
              new CLavaError(&elDef->DECLError1, &ERR_VTOutOfScope);
            else 
              Doc->changeInUpdate = Doc->changeInUpdate || (refID != elDef->RefID);
        }
    }
    else
      new CLavaError(&elDef->DECLError1, &ERR_NoRefType);
    bm = GetPixmap(false, true, defType, flags);
    if ((elDef->DECLError1.first || elDef->DECLError2.first)) 
      Doc->hasErrorsInTree = true;
  }
  MakeItem(lab, bm, parent, pelDef);
}

void CExecTree::ExecFText(LavaDECL ** pelDef, int level)
{
  QPixmap* bm;
  DString lab;

  (*pelDef)->DECLError1.Destroy();
  if ((*pelDef)->LocalName.l == 0)
    (*pelDef)->LocalName = DString("_Text");
  if (!CalcPos(level))
    return;
  TDeclDescType exprType = LiteralString;
  TDeclType defType = NoDef;
  lab = (*pelDef)->LocalName;
  //SynFlags flags;
  bm = GetPixmap(false, true, FormText/*, flags*/);
  CTreeItem* parent = getSectionNode(ParItem, FormText);
  MakeItem(lab, bm, parent, pelDef);
}

void CExecTree::ExecIn(LavaDECL ** pelDef, int level)
{
  (*pelDef)->nInput = (*pelDef)->ParentDECL->nInput;
  (*pelDef)->ParentDECL->nInput++;
  ExecMember(pelDef, level);
}

void CExecTree::ExecOut(LavaDECL ** pelDef, int level)
{
  (*pelDef)->nInput = (*pelDef)->ParentDECL->nInput + (*pelDef)->ParentDECL->nOutput;
  (*pelDef)->ParentDECL->nOutput++;
  ExecMember(pelDef, level);
}

void CExecTree::ExecFormDef(LavaDECL ** pelDef, int level)
{
  LavaDECL *pp, *elDef = *pelDef;
  QString cstr;
  DString lab;
  QPixmap* bm;
  CMainItemData* data;
  CTreeItem *item, *parent;
  bool changed = false;

  if (FinalUpdate) 
    elDef->DECLError1.Destroy();
  lab = elDef->ParentDECL->LocalName + DString (" := ");
  if (viewTree->myInclView) {
    if (checkLevel != CHLV_noCheck) {
      changed = Doc->CheckForm(elDef, checkLevel);
      Doc->changeInUpdate = Doc->changeInUpdate || changed;
      if (elDef->DECLError1.first) {
        new CLavaError(&elDef->ParentDECL->DECLError1, &ERR_ErrInForm);
        Doc->hasErrorsInTree = true;
      }
    }
    if (downAwaited && !ActItem->firstChild()) {
      data = (CMainItemData*)ActItem->getItemData();
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt))
        item = getSectionNode(ActItem, ExecDef);
    }
    downAwaited = false;
    skipLevel = level+1;
    return;
  }
  else {
    if (RemoveErrCode(&elDef->ParentDECL->DECLError1, &ERR_ErrInForm)
      && !elDef->ParentDECL->DECLError1.first /* && Doc->nTreeErrors*/) {
      //Doc->nTreeErrors--;
      ((CLavaPEDoc*)Doc)->SetTreeItemImage(elDef->ParentDECL, false);

    }
    if (checkLevel != CHLV_noCheck)
      changed = Doc->CheckForm(elDef, checkLevel);
    if (!CalcPos(level))
      return;
    pp = Doc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
    if (pp) 
      lab += Doc->GetTypeLabel(elDef, true);
    else {
      lab += DString("??");
      new CLavaError(&elDef->DECLError1, &ERR_NoRefType);
    }
    bm = GetPixmap(false, false, FormDef);
    parent = getSectionNode(ParItem, FormDef);
    MakeItem(lab, bm, parent, pelDef);
    if (elDef->DECLError1.first || elDef->DECLError2.first) {
      Doc->hasErrorsInTree = true;
      new CLavaError(&elDef->ParentDECL->DECLError1, &ERR_ErrInForm);
    }
    if (elDef->DECLError2.first) {
      if (elDef->DECLError1.last)
        elDef->DECLError1.last->successor = elDef->DECLError2.first;
      else
        elDef->DECLError1.first = elDef->DECLError2.first;
      elDef->DECLError2.first->predecessor = elDef->DECLError1.last;
      elDef->DECLError1.last = elDef->DECLError2.last;
      elDef->DECLError2.first = 0;
      elDef->DECLError2.last = 0;
    }
    if (elDef->ParentDECL->DECLError1.first || elDef->ParentDECL->DECLError2.first)
      ((CLavaPEDoc*)Doc)->SetTreeItemImage(elDef->ParentDECL, true);
    else
      ((CLavaPEDoc*)Doc)->SetTreeItemImage(elDef->ParentDECL, false);

  }
}

void CExecTree::ExecExec(LavaDECL ** pelDef, int level)
{
  QPixmap *sm = 0, *bm;
  LavaDECL *elDef = *pelDef;
  SynFlags flag; 
  if (elDef->Exec.ptr) {
    if (!CalcPos(level))
      return;
    ActItem = getSectionNode(ParItem, elDef->DeclType);
    if (!viewTree->drawTree) {
      CMainItemData* data = (CMainItemData*)ActItem->getItemData();
      data->synEl = ((CMainItemData*)ParItem->getItemData())->synEl;
    }
    if ((elDef->ParentDECL->DeclType == Interface) || (elDef->ParentDECL->DeclType == Impl))
      flag.INCL(invariantPM);
    if (((SelfVar*)elDef->Exec.ptr)->IsEmptyExec()) {
      flag.INCL(emptyPM);
      bm = GetPixmap(true, false, elDef->DeclType, flag);
      if ( (elDef->ParentDECL->DeclType == Function) && (elDef->DeclType == ExecDef)
        || (elDef->ParentDECL->DeclType == Initiator)) {
        elDef->DECLError2.Destroy();
        new CLavaError(&elDef->DECLError2, &ERR_NoExecBody);
      }
    }
    else
      bm = GetPixmap(true, false, elDef->DeclType, flag);
    ActItem->setPix(bm); //viewTree->SetItemImage(ActItem, bm, bm);
    ActItem->SetItemMask(elDef->DECLError1.first || elDef->DECLError2.first, 
        elDef->DECLComment.ptr && elDef->DECLComment.ptr->Comment.l);
  }
}



void CExecTree::ExecStruct(LavaDECL** pinEl, DString fieldID)
{
  if (!isOnSkip) {
    downAwaited = true;
    if (!(*pinEl)->NestedDecls.first
        || ( ((LavaDECL*)((CHE*)(*pinEl)->NestedDecls.first)->data)->DeclDescType == ExecDesc)) {
      CMainItemData * data = (CMainItemData*)ActItem->getItemData();
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt))
        CTreeItem* item = getSectionNode(ActItem, Interface);
    }
  }
}

void CExecTree::ExecEnum(LavaDECL** pinEl, DString fieldID)
{
  CTreeItem* item;

  if (skipLevel <= ActLevel+2) {
    isOnSkip = true;
    return;
  }
  downAwaited = true;
  QPixmap* bm = ((CLavaPEApp*)wxTheApp)->LavaPixmaps[enumBM];//QPixmapCache::find("l_enum"); 
  if ((*pinEl)->Items.first)
    ParItem = ActItem;
  if (!viewTree->drawTree && ActItem->firstChild()) {
    item = (CTreeItem*)ActItem->firstChild();
    viewTree->DeleteItemData(item);
    delete item;
  }
  item = viewTree->InsertItem("Enumeration", bm, ActItem, TVI_FIRST);
  CMainItemData * data = (CMainItemData*)ActItem->getItemData();
  CMainItemData * dd = new CMainItemData(TIType_EnumItems, data->synEl, (*(LavaDECL**)data->synEl)->TreeFlags.Contains(ItemsExpanded));
  item->setItemData( dd);
  if (!CalcPos(ActLevel))
    return;
}

void CExecTree::ExecEnumItem(CHEEnumSelId * enumsel, int level)
{
  if (!CalcPos(level))
    return;
  QPixmap* bm = ((CLavaPEApp*)wxTheApp)->LavaPixmaps[enumselBM];//QPixmapCache::find("l_enumsel");
  CTreeItem* section = (CTreeItem*)ParItem->firstChild();
  ActItem = viewTree->InsertItem(enumsel->data.Id.c, bm, section, ActItem);
//  if (enumsel->data.DECLComment.ptr && enumsel->data.DECLComment.ptr->Comment.l)
//    viewTree->SetItemState( ActItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK );
  ActItem->SetItemMask(false,  enumsel->data.DECLComment.ptr && enumsel->data.DECLComment.ptr->Comment.l);

  CMainItemData *itd = new CMainItemData(TIType_CHEEnumSel, (unsigned long) enumsel);
  ActItem->setRenameEnabled(0, true);
  ActItem->setItemData( itd);
  ActItem->setDragEnabled(true);
  if (viewTree->drawTree && FinalUpdate) {
    if (enumsel->data.selItem) {
      viewTree->GetListView()->setCurAndSel(ActItem);
      viewTree->SelItem = ActItem; 
      enumsel->data.selItem = false;
    }
  }
}


CTreeItem* CExecTree::getSectionNode(CTreeItem* parent, TDeclType ncase)
{
  return viewTree->getSectionNode(parent, ncase);
} 


QPixmap* CExecTree::GetPixmap(bool isParent, bool isAttr, TDeclType ptype, const SynFlags flag) 
{  
  return ((CLavaPEView*)Doc->MainView)->GetPixmap(isParent, isAttr, ptype, flag);
}


CExecImpls::CExecImpls(SynDef *lavaCode, const TID& ifaceID, const TID& funcID, CLavaPEHint* hint)
{
  IFaceID = ifaceID; //of the interface
  Hint = hint;
  Travers = new CSylTraversal(this, lavaCode);
  Travers->AllDefs(false);

}


void CExecImpls::ExecDefs(LavaDECL ** pelDef, int level)
{
  TID iID;
  CLavaPEHint *newHint = 0;
  DString *pStr;
  SynFlags firstlast;
  DWORD d7=0;

  if (Hint->FirstLast.Contains(multiDocHint))
    firstlast.INCL(multiDocHint);
  if ( ((*pelDef)->DeclType == Impl) || ((*pelDef)->DeclType == CompObj)) { 
    if (((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(*pelDef, IFaceID, 0 )) {
      LavaDECL* newDECL = NewLavaDECL();
      *newDECL = *(*pelDef);
      if ( ((*pelDef)->DeclType == Impl) 
        && ((CLavaPEDoc*)Hint->fromDoc)->CheckImpl(newDECL, CHLV_inUpdateHigh /*CHLV_fit*/)) {
        pStr = new DString((*pelDef)->FullName);
        if (newDECL->WorkFlags.Contains(newTreeNode)) {
          newDECL->WorkFlags.EXCL(newTreeNode);
          d7 = 1;
        }
        newHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)newDECL, (DWORD)pStr, 0, (DWORD)pelDef,0,0,d7);
        ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(newHint);
        ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, newHint);
      }
      else
        delete newDECL;
    }
  }
}


CExecOverrides::CExecOverrides(SynDef *lavaCode, const TID& ifaceID, const TID& funcID, CLavaPEHint* hint)
{
  FuncID = funcID;   //the function
  IFaceID = ifaceID; //the interface
  Hint = hint;
  NewHint = 0;
  HintDECL = (LavaDECL*)Hint->CommandData1;
  TabTravers = new CTabTraversal(this, lavaCode);
  TabTravers->Run(false, true);
}


void CExecOverrides::ExecDefs(LavaDECL ** pelDef, int )
{
  DString *pStr;
  TID id2, id= TID((*pelDef)->OwnID, (*pelDef)->inINCL);
  SynFlags typeFlags, firstlast;
  LavaDECL *newDECL;
  bool catErr;

  NewHint = 0;
  if (Hint->FirstLast.Contains(multiDocHint))
    firstlast.INCL(multiDocHint);
  if ( (HintDECL->DeclType == Function) || (HintDECL->DeclType == Attr))
    if ((*pelDef)->DeclType == Interface) { 
      id.nID = (*pelDef)->OwnID;
      id2.nID = HintDECL->ParentDECL->OwnID;
      if (((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(id, 0, id2, HintDECL->inINCL)
         || ((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc( id2, HintDECL->inINCL, id, 0)) 
         (*pelDef)->WorkFlags.INCL(recalcVT);
      return;
    }
    else {
      if ((*pelDef)->DeclType == HintDECL->DeclType) {

      }
    }
  if ( ( (*pelDef)->DeclType == Function) && (*pelDef)->SecondTFlags.Contains(overrides)) {
    id2 = TID((*pelDef)->ParentDECL->OwnID,  (*pelDef)->inINCL);
    if ( ((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(id, 0, FuncID, HintDECL->inINCL)
        || (HintDECL->DeclType == Interface) 
           && (Hint->com != CPECommand_Insert) 
           && ((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(id2, 0, IFaceID, HintDECL->inINCL) ) {                                                
      newDECL = NewLavaDECL();
      *newDECL = *(*pelDef);
      if (((CLavaPEDoc*)Hint->fromDoc)->CheckOverInOut(newDECL, CHLV_inUpdateHigh/*CHLV_fit*/)) {
        pStr = new DString((*pelDef)->FullName);
        NewHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)newDECL, (DWORD)pStr, 0, (DWORD)pelDef);
        ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(NewHint);
        ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, NewHint);
        ((CLavaPEDoc*)Hint->fromDoc)->ConcernImpls(NewHint, *pelDef);
      }
      else
        delete newDECL;
    }
    return;
  }
  if ( ( (*pelDef)->DeclType == VirtualType)
       && ( (*pelDef)->SecondTFlags.Contains(overrides)
             && ((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(id, 0, FuncID, HintDECL->inINCL) 
           || (HintDECL->DeclType == VirtualType) 
               && ((CLavaPEDoc*)Hint->fromDoc)->IDTable.EQEQ(FuncID, HintDECL->inINCL,
                                                  (*pelDef)->RefID, (*pelDef)->inINCL))) {
    typeFlags = (*pelDef)->TypeFlags;
    ((CLavaPEDoc*)Hint->fromDoc)->GetCategoryFlags(*pelDef, catErr);
    if (typeFlags != (*pelDef)->TypeFlags) {
      newDECL = NewLavaDECL();
      *newDECL = *(*pelDef);
      (*pelDef)->TypeFlags = typeFlags;
      pStr = new DString((*pelDef)->FullName);
      NewHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)newDECL, (DWORD)pStr, 0, (DWORD)pelDef);
      ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(NewHint);
      ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, NewHint);
      ((CLavaPEDoc*)Hint->fromDoc)->ConcernImpls(NewHint, *pelDef);
    }
  }
}

void CExecOverrides::ExecIn(LavaDECL ** pelDef, int )
{
  ExecMember(pelDef, 0);
}

void CExecOverrides::ExecOut(LavaDECL ** pelDef, int )
{
  ExecMember(pelDef, 0);
}

void CExecOverrides::ExecMember(LavaDECL ** pelDef, int )
{
  DString *pStr;
  TID id,idB; 
  SynFlags typeFlags, firstlast;
  LavaDECL *newDECL, *newBDECL;
  bool catErr;

  NewHint = 0;
  if (Hint->FirstLast.Contains(multiDocHint))
    firstlast.INCL(multiDocHint);
  id = TID((*pelDef)->OwnID, (*pelDef)->inINCL);
  if ( (*pelDef)->DeclType == Attr)  
    idB = FuncID;
  else 
    idB = TID(HintDECL->OwnID,0);
  newBDECL = ((CLavaPEDoc*)Hint->fromDoc)->IDTable.GetDECL(idB, HintDECL->inINCL);
  if (!newBDECL)
    return;
  if (( (*pelDef)->DeclType == Attr) 
       && (*pelDef)->SecondTFlags.Contains(overrides)
       && (Hint->com == CPECommand_Change) 
       && ((CLavaPEDoc*)Hint->fromDoc)->IDTable.IsAnc(id, 0, idB, HintDECL->inINCL)) {
    typeFlags = (*pelDef)->TypeFlags;
    ((CLavaPEDoc*)Hint->fromDoc)->GetCategoryFlags(*pelDef, catErr);
    if (typeFlags != (*pelDef)->TypeFlags) {
      newDECL = NewLavaDECL();
      *newDECL = *(*pelDef);
      (*pelDef)->TypeFlags = typeFlags;
      if ((*pelDef)->DeclType == Attr) {
        /*
        if (newBDECL->TypeFlags.Contains(hasSetGet) && (*pelDef)->TypeFlags.Contains(inheritsBody))
          newDECL->TypeFlags.INCL(inheritsBody);
        */
        if (newBDECL->TypeFlags.Contains(isAbstract) != (*pelDef)->TypeFlags.Contains(isAbstract))
          newDECL->TypeFlags.EXCL(isAbstract);
      }
      pStr = new DString((*pelDef)->FullName);
      NewHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)newDECL, (DWORD)pStr, 0, (DWORD)pelDef);
      ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(NewHint);
      ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, NewHint);
      ((CLavaPEDoc*)Hint->fromDoc)->ConcernImpls(NewHint, *pelDef);
    }
    return;
  }
  if ((HintDECL->DeclType == VirtualType) 
      && ((CLavaPEDoc*)Hint->fromDoc)->IDTable.EQEQ(FuncID, HintDECL->inINCL,
                                             (*pelDef)->RefID, (*pelDef)->inINCL)) {
    typeFlags = (*pelDef)->TypeFlags;
    ((CLavaPEDoc*)Hint->fromDoc)->GetCategoryFlags(*pelDef, catErr);
    if (typeFlags != (*pelDef)->TypeFlags) {
      newDECL = NewLavaDECL();
      *newDECL = *(*pelDef);
      (*pelDef)->TypeFlags = typeFlags;
      pStr = new DString((*pelDef)->FullName);
      NewHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)newDECL, (DWORD)pStr, 0, (DWORD)pelDef);
      ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(NewHint);
      ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, NewHint);
      ((CLavaPEDoc*)Hint->fromDoc)->ConcernImpls(NewHint, *pelDef);
    }
    return;
  }
}

CExecForms::CExecForms(SynDef *lavaCode, CLavaPEHint *hint)
{
  Hint = hint; 
  Travers = new CSylTraversal(this, lavaCode);
  Travers->AllDefs(false);
}


void CExecForms::ExecFormDef(LavaDECL ** pelDef, int )
{
  LavaDECL *structDecl, *inDecl, *decl, *hintDECL, *partDECL;
  TID structID;
  CHE *elChe;
  CHETID* cheTID;
  CLavaPEHint *newHint=0;
  int pos=0;
  bool found;
  DString *pStr=0;
  SynFlags flags, firstlast;
  TDeclType defType;

  structDecl = ((TIDTable*)Travers->mySynDef->IDTable)->GetDECL((*pelDef)->RefID, (*pelDef)->inINCL);
  if (!structDecl)
    return;
  if (Hint->com == CPECommand_Change)
    hintDECL = *(LavaDECL**)Hint->CommandData4;
  else
    hintDECL = (LavaDECL*)Hint->CommandData1;
  for (partDECL = hintDECL;
       partDECL && (partDECL != structDecl);
       partDECL = partDECL->ParentDECL);
  bool hintIsPart = partDECL != 0;
  for (partDECL = structDecl;
       partDECL && (partDECL != hintDECL);
       partDECL = partDECL->ParentDECL);
  bool structIsPart = partDECL != 0;
  if (hintIsPart && !structIsPart) 
    //member of parentpattern?
    hintIsPart = ( ( ( hintDECL->DeclType == Attr)
         || ( hintDECL->DeclType == VirtualType)
             && (hintDECL->SecondTFlags.Contains(isSet) || hintDECL->SecondTFlags.Contains(isArray)))
         && (( (*pelDef)->DeclDescType == StructDesc) || ((*pelDef)->DeclDescType == EnumType)));
  if (!hintIsPart && !structIsPart)
    return;

  if (Hint->FirstLast.Contains(multiDocHint))
    firstlast.INCL(multiDocHint);
  if (hintIsPart && structIsPart) {
    switch (Hint->com) {
      case CPECommand_Delete:
        //the structure is deleted, the error message is in CExecTree
        
        /*
        if ((*pelDef)->ParentDECL && (*pelDef)->ParentDECL->ParentDECL && (*pelDef)->ParentDECL->ParentDECL->FullName.l)
          pStr = new DString((*pelDef)->ParentDECL->ParentDECL->FullName);
        pos = GetPos(*pelDef);
        if (pos <0)
          return;
        newHint = new CLavaPEHint(CPECommand_Delete, Hint->fromDoc, false, (DWORD)*pelDef, (DWORD)pStr, pos, (DWORD)Travers->parent_p_DECL);
        Travers->elRemoved = true;
        */
        break;

      case CPECommand_Change:
        decl = NewLavaDECL();
        *decl = *(*pelDef);
        if (((CLavaPEDoc*)Hint->fromDoc)->CheckForm(decl, CHLV_inUpdateHigh/*CHLV_fit*/)) {
          pStr = new DString((*pelDef)->FullName);
          newHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)decl, (DWORD)pStr, 0, (DWORD)pelDef);
        }
        else
          delete decl;  
        break;

        default:
        return;
    }
  }
  else {
    defType = NoDef;
    if (hintIsPart ) {
      //a member has been changed, deleted or inserted
      switch (Hint->com) {
      case CPECommand_Delete:
        elChe = (CHE*) (*pelDef)->NestedDecls.first;
        structID = TID(hintDECL->OwnID,hintDECL->inINCL);
        pos = 1;
        found = false;
        while (elChe && !found) {
          decl = (LavaDECL*)elChe->data;
          if (decl->DeclType == hintDECL->DeclType)
            if (!decl->Supports.first
              || (((CHETID*)decl->Supports.first)->data != structID)) {
              pos++;
              elChe = (CHE*) elChe->successor;
            }
            else
              found = true;
          else
            elChe = (CHE*) elChe->successor;
        }
        if (elChe && (((LavaDECL*)elChe->data)->DeclDescType != ExecDesc)) 
          decl = (LavaDECL*)elChe->data;
        else
          return;
//        pos = GetPosinForm(hintDECL);
        pStr = new DString((*pelDef)->FullName);
        newHint = new CLavaPEHint(CPECommand_Delete, Hint->fromDoc, firstlast, (DWORD)decl, (DWORD)pStr, pos, (DWORD)pelDef, Hint->CommandData5);
        break;

      case CPECommand_Insert:
        if (!hintDECL->TypeFlags.Contains(constituent) )
          break;
        decl = NewLavaDECL();
        *decl = *hintDECL;
        decl->WorkFlags.EXCL(selAfter);
        decl->ParentDECL = (*pelDef);
        decl->FullName.Delete(0, ((DString*)Hint->CommandData2)->l);
        decl->FullName.Insert((*pelDef)->FullName, 0);
        if (!decl->Annotation.ptr) {
          decl->Annotation.ptr = new TAnnotation;
          decl->Annotation.ptr->FA.ptr = new TAnnotation;
          inDecl = NewLavaDECL();
          inDecl->Annotation.ptr = new TAnnotation;
          inDecl->DeclDescType = LiteralString;
          inDecl->LitStr = decl->LocalName;
          CHE* litEl = NewCHE(inDecl);
          decl->Annotation.ptr->Prefixes.Append(litEl);
          decl->Annotation.ptr->FrameSpacing = 1;
        }
        if (decl->DeclDescType == NamedType) {
          if (!((CPEBaseDoc*)Hint->fromDoc)->IDTable.GetDECL(decl->RefID, decl->inINCL))
            decl->RefID = (*(LavaDECL**)Hint->CommandData4)->RefID;
          defType  = LBaseData->Browser->GetCategory(Travers->mySynDef, decl, flags);
        }
        decl->Supports.Destroy();
        cheTID = new CHETID;
        cheTID->data = TID(decl->OwnID, decl->inINCL);
        decl->Supports.Append(cheTID);
        decl->OwnID = -1;
        if ((decl->DeclDescType == BasicType) || (defType == BasicDef) || (defType == VirtualType))
          ((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm(decl, *(LavaDECL**)Hint->CommandData4, Travers->mySynDef );
        pos = GetPosinForm(hintDECL);
        pStr = new DString((*pelDef)->FullName);
        newHint = new CLavaPEHint(CPECommand_Insert, Hint->fromDoc, firstlast, (DWORD)decl, (DWORD)pStr, pos, (DWORD)pelDef, Hint->CommandData5);
        break;

      case CPECommand_Change:
        structID = TID(hintDECL->OwnID,hintDECL->inINCL);
        for (elChe = (CHE*) (*pelDef)->NestedDecls.first;
             elChe && (((LavaDECL*)elChe->data)->DeclDescType != ExecDesc)
               && ((((LavaDECL*)elChe->data)->DeclDescType == LiteralString)
                 || (((CHETID*)((LavaDECL*)elChe->data)->Supports.first)->data != structID));
             elChe = (CHE*) elChe->successor);
        if (!elChe || (((LavaDECL*)elChe->data)->DeclDescType == ExecDesc)
           || (((LavaDECL*)elChe->data)->DeclDescType == LiteralString)) 
          return;
        if (hintDECL->TypeFlags.Contains(constituent)) {
          decl = NewLavaDECL(); 
  //        *decl = *(LavaDECL*)elChe->data;
          if (((LavaDECL*)elChe->data)->DeclDescType != hintDECL->DeclDescType) {
            *decl = *hintDECL;
            decl->WorkFlags.EXCL(selAfter);
            decl->FullName = ((LavaDECL*)elChe->data)->FullName;
            decl->LocalName = ((LavaDECL*)elChe->data)->LocalName;
            decl->ParentDECL = ((LavaDECL*)elChe->data)->ParentDECL;
            decl->OwnID = ((LavaDECL*)elChe->data)->OwnID;
            decl->inINCL = ((LavaDECL*)elChe->data)->inINCL;
            if (decl->DeclDescType == NamedType) {
              if (!((CPEBaseDoc*)Hint->fromDoc)->IDTable.GetDECL(decl->RefID, decl->inINCL))
                decl->RefID = (*(LavaDECL**)Hint->CommandData4)->RefID;
              defType  = LBaseData->Browser->GetCategory(Travers->mySynDef, decl, flags);
            }
            decl->Supports.Destroy();
            cheTID = new CHETID;
            cheTID->data = TID(hintDECL->OwnID, hintDECL->inINCL);
            decl->Supports.Append(cheTID);
            if ((decl->DeclDescType == BasicType) || (defType == BasicDef) || (defType == VirtualType))
              ((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm(decl, *(LavaDECL**)Hint->CommandData4, Travers->mySynDef );
          }
          else
            *decl = *(LavaDECL*)elChe->data;
          if ((decl->BType != hintDECL->BType)
            && decl->Annotation.ptr && decl->Annotation.ptr->FA.ptr) {
            ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->StringValue.Reset(0);
            ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->IoSigFlags.EXCL(trueValue);
          }
          decl->BType = hintDECL->BType;
          if ((decl->DeclDescType == NamedType) || (decl->DeclDescType == BasicType)) {
            if ( (hintDECL->RefID != ((LavaDECL*)Hint->CommandData1)->RefID)
               || !((CPEBaseDoc*)Hint->fromDoc)->IDTable.GetDECL(decl->RefID, decl->inINCL))
              decl->RefID = hintDECL->RefID;
            defType  = LBaseData->Browser->GetCategory(Travers->mySynDef, decl, flags);
            if ((defType == VirtualType) && decl->Annotation.ptr && decl->Annotation.ptr->IterOrig.ptr)
              ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.Destroy();
          }
          pStr = new DString(decl->FullName); //the old name
          decl->FullName.Delete(((LavaDECL*)elChe->data)->FullName.l - ((LavaDECL*)elChe->data)->LocalName.l, 
                               ((LavaDECL*)elChe->data)->LocalName.l);  //remove the old ID
          decl->FullName += hintDECL->LocalName;
          decl->LocalName =  hintDECL->LocalName;
          if ((decl->DeclDescType == BasicType) || (defType == BasicDef) || (defType == VirtualType) || (decl->DeclDescType == EnumType))
            ((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm((LavaDECL*)decl, hintDECL, Travers->mySynDef );
          newHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, firstlast, (DWORD)decl, (DWORD)pStr, 0, (DWORD)&elChe->data);
        }
        else {
          pos = GetPosinForm((LavaDECL*)elChe->data);
          pStr = new DString((*pelDef)->FullName);
          newHint = new CLavaPEHint(CPECommand_Delete, Hint->fromDoc, firstlast, (DWORD)elChe->data, (DWORD)pStr, pos, (DWORD)pelDef, Hint->CommandData5);
        }
        break;

      default:
        return;
      }
    }
    /*
    else { //parentIsPart
      DString relName;
      switch (Hint->com) {
      case CPECommand_Delete:
        //the structure is deleted, the error message is in CExecTree
        //Delete the form
        if ((*pelDef)->ParentDECL && (*pelDef)->ParentDECL->ParentDECL  && (*pelDef)->ParentDECL->ParentDECL->FullName.l)
          pStr = new DString((*pelDef)->ParentDECL->ParentDECL->FullName);
        pos = GetPos(*pelDef);
        if (pos <0)
          return;
        newHint = new CLavaPEHint(CPECommand_Delete, Hint->fromDoc, false, (DWORD)*pelDef, (DWORD)pStr, pos, (DWORD)Travers->parent_p_DECL, Hint->CommandData5);
        Travers->elRemoved = true;
       break;

      case CPECommand_Change:
        //an upper decl of the class pattern has been changed
        if (((TIDTable*)Travers->mySynDef->IDTable)->CheckID((*pelDef)->RefID, ppdecl, (*pelDef)->inINCL)) {
          if ((*pelDef)->ParentDECL && (*pelDef)->ParentDECL->ParentDECL  && (*pelDef)->ParentDECL->ParentDECL->FullName.l)
            pStr = new DString((*pelDef)->ParentDECL->ParentDECL->FullName);
          pos = GetPos(*pelDef);
          if (pos <0)
            return;
          newHint = new CLavaPEHint(CPECommand_Delete, Hint->fromDoc, false, (DWORD)*pelDef,
                                    (DWORD)pStr, pos, (DWORD)Travers->parent_p_DECL);
          Travers->elRemoved = true;
        }
        else {
          if (((LavaDECL*)Hint->CommandData1)->FullName != *(DString*)Hint->CommandData2) {
            decl = NewLavaDECL();
            *decl = **pelDef;
            pStr = new DString((*pelDef)->FullName);
            newHint = new CLavaPEHint(CPECommand_Change, Hint->fromDoc, false, (DWORD)decl,
                                     (DWORD)pStr, pos, (DWORD)pelDef);
          }
          else
            return;
        }
        break;

      default:
        return;
      }
    }//structIsPart
    */
  }

  if (newHint) {
    ((CPEBaseDoc*)Hint->fromDoc)->UndoMem.AddToMem(newHint);
    ((CPEBaseDoc*)Hint->fromDoc)->UpdateDoc(NULL, false, newHint);
  }
}

int CExecForms::GetPosinForm(LavaDECL* decl)
{
  LavaDECL* pDECL = decl->ParentDECL;
  CHE* cheEl;
  int pos=1;

  if (pDECL && (pDECL->DeclDescType != StructDesc) && (pDECL->DeclDescType != EnumType))
    pDECL = pDECL->ParentDECL;
  if (pDECL && (pDECL->DeclDescType == StructDesc) || (pDECL->DeclDescType == EnumType)) {
    for (cheEl = (CHE*)pDECL->NestedDecls.first;
         cheEl && ( (LavaDECL*)cheEl->data != decl);
         cheEl = (CHE*)cheEl->successor)
      if ((decl->DeclType == ((LavaDECL*)cheEl->data)->DeclType)
          && ((decl->DeclType == Attr) 
              || ((LavaDECL*)cheEl->data)->SecondTFlags.Contains(isSet)
              || ((LavaDECL*)cheEl->data)->SecondTFlags.Contains(isArray)))
        pos++;
    if (cheEl) 
      return pos;
    else
      return -1;
  }
  return -1;
}



CExecChecks::CExecChecks(CLavaPEDoc* doc, bool inopen)
{
  Doc = doc;
  Doc->nPlaceholders = 0;
  Doc->nErrors = 0;
  inOpen = inopen;
  Travers = new CSylTraversal(this, doc->mySynDef);
  Travers->AllDefs(false);
}


void CExecChecks::ExecExec(LavaDECL ** pelDef, int level)
{
  CSearchData sData;
  sData.doc = Doc;
  if ((*pelDef)->Exec.ptr) {
    CheckData ckd;
    ckd.document = Doc;
    ckd.myDECL = *pelDef; 
    ((SynObject*)(*pelDef)->Exec.ptr)->Check(ckd);
    sData.doc = Doc;
    sData.execDECL = *pelDef;
    ((SynObject*)(*pelDef)->Exec.ptr)->MakeTable((address)&Doc->IDTable, 0, (SynObjectBase*)ckd.myDECL, onSetSynOID, 0,0, (address)&sData);
    Doc->nErrors += ckd.nErrors;
    Doc->nPlaceholders += ckd.nPlaceholders;
  }
}


void CExecChecks::ExecFormDef(LavaDECL ** pelDef, int level)
{
  LavaDECL *pp;
  CHETID *cheS;

  (*pelDef)->DECLError2.Destroy();
  if (RemoveErrCode(&(*pelDef)->ParentDECL->DECLError1, &ERR_ErrInForm)
    && !(*pelDef)->ParentDECL->DECLError1.first /*&& Doc->nTreeErrors*/) {
    Doc->nErrors--; //Doc->nTreeErrors--;
    ((CLavaPEDoc*)Doc)->SetTreeItemImage((*pelDef)->ParentDECL, false);
  }
  cheS = (CHETID*)(*pelDef)->Supports.first;
  if (cheS) {
    pp = Doc->IDTable.GetDECL(cheS->data, (*pelDef)->inINCL);
    if (!pp) 
      new CLavaError(&(*pelDef)->DECLError2, &ERR_NoExtForm);
    for (cheS = (CHETID*)cheS->successor; cheS; cheS = (CHETID*)cheS->successor) {
      pp = Doc->IDTable.GetDECL(cheS->data, (*pelDef)->inINCL);
      if (!pp) 
        new CLavaError(&(*pelDef)->DECLError2, &ERR_NoExtForm);
    }
  }
  ((CLavaPEDoc*)Doc)->CheckForm(*pelDef, CHLV_showError);
  pp = Doc->IDTable.GetDECL((*pelDef)->RefID, (*pelDef)->inINCL);
  if ((*pelDef)->DECLError1.first || (*pelDef)->DECLError2.first) {
    new CLavaError(&(*pelDef)->ParentDECL->DECLError2, &ERR_ErrInForm);
    Doc->nErrors++;
  }
  if ((*pelDef)->ParentDECL->DECLError2.first || (*pelDef)->ParentDECL->DECLError1.first) {
    ((CLavaPEDoc*)Doc)->SetTreeItemImage((*pelDef)->ParentDECL, true);
    //Doc->nErrors++;
  }
  else
    ((CLavaPEDoc*)Doc)->SetTreeItemImage((*pelDef)->ParentDECL, false);
}

void CExecChecks::ExecDefs(LavaDECL ** pelDef, int level)
{
  if ((*pelDef)->DeclType == Interface) {
    (*pelDef)->DECLError2.Destroy();
    bool hasErr = ((*pelDef)->DECLError1.first != 0);

    if (!((CLavaPEDoc*)Doc)->MakeVElems(*pelDef)) {
      ((CLavaPEDoc*)Doc)->SetTreeItemImage(*pelDef, true);
      if (!hasErr) {
        (*pelDef)->DECLError2.first = (*pelDef)->DECLError1.first;
        (*pelDef)->DECLError2.last = (*pelDef)->DECLError1.last;
        (*pelDef)->DECLError1.first = 0;
        (*pelDef)->DECLError1.last = 0;
      }
    }
  }
  if ((*pelDef)->DECLError1.first || (*pelDef)->DECLError2.first)
    Doc->nErrors++;
}


void CExecChecks::ExecMember(LavaDECL ** pelDef, int level)
{
  SynFlags flags;
  CHAINX* errChain, *errChainP;
  TDeclType defType;

  if ((*pelDef)->ParentDECL->DeclType != FormDef) {
    if ((*pelDef)->DECLError1.first || (*pelDef)->DECLError2.first)
      Doc->nErrors++;
    return;
  }
  (*pelDef)->DECLError2.Destroy();
  if (((*pelDef)->DeclDescType == UnknownDDT)
      || ((*pelDef)->DeclDescType == Undefined)
         && ((*pelDef)->LocalName.l == 0))
    return;
  errChain = &(*pelDef)->DECLError2;
  errChainP = &(*pelDef)->ParentDECL->ParentDECL->DECLError2;
  defType = ((CLavaPEApp*)wxTheApp)->Browser.GetCategory(((CLavaPEDoc*)Doc)->mySynDef, *pelDef, flags);
  if (defType == UnDef) 
    new CLavaError(errChain, &ERR_NoRefType);
  else
    if ((defType == VirtualType) && !Doc->CheckGetFinalMType(*pelDef))
      new CLavaError(errChain, &ERR_VTOutOfScope);
  if (errChain->first && !errChainP->first) {
    new CLavaError(errChainP, &ERR_ErrInForm);
    ((CLavaPEDoc*)Doc)->SetTreeItemImage((*pelDef)->ParentDECL->ParentDECL, true);
    Doc->nErrors++;
  }
}

void CExecChecks::ExecIn(LavaDECL ** pelDef, int )
{
  if ((*pelDef)->DECLError1.first || (*pelDef)->DECLError2.first)
    Doc->nErrors++;
}

void CExecChecks::ExecOut(LavaDECL ** pelDef, int)
{
  if ((*pelDef)->DECLError1.first || (*pelDef)->DECLError2.first)
    Doc->nErrors++;

}
