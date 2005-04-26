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

#include "LavaBase_all.h"
#include "MACROS.h"
/*
#include "LavaAppBase.h"
#include "SylTraversal.h"
#include "PEBaseDoc.h"
*/
#include "qobject.h"
#include "qstring.h"



CSylTraversal::CSylTraversal(CExec *execSyl, SynDef *syn)
{
  ExecSyl = execSyl;
  mySynDef = syn;
  Done = FALSE;
  FillOut = FALSE;
  elRemoved = FALSE;
}


int CSylTraversal::AllDefs(bool withImported, bool fillOut)
{  
  int it=0; //, max;
  FillOut = FillOut || fillOut;
  if (mySynDef) {
    DString str;
    nINCL = 0;
    CHESimpleSyntax *simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
    while (simpleSyntax && !Done ) {
      if (simpleSyntax->data.TopDef.ptr) {
        if (simpleSyntax->data.LocalTopName.l) 
          ((LavaDECL*)simpleSyntax->data.TopDef.ptr)->LocalName = simpleSyntax->data.LocalTopName;
        DownTree(&simpleSyntax->data.TopDef.ptr, 0, str);
      }
      it++;
      nINCL++;
      Done = !withImported;
      simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
    }
  }
  else
    it = 1;
  nINCL = -1;
  return it;
}

void CSylTraversal::DownTree(LavaDECL **pelDef, int level, DString& name)
{
  CHE *inCheEl, *NinCheEl;
  LavaDECL * elDef, *inEl;
  LavaDECL **localparent_p_DECL;
  CHEEnumSelId * enumsel;
  DString noName;
  if (Done)
    return;
  if (!pelDef) {
    AllDefs(FALSE);
    return;
  }
  localparent_p_DECL = parent_p_DECL;

  if ((*pelDef)->ParentDECL && (*pelDef)->ParentDECL->FullName.l) {
    (*pelDef)->FullName = (*pelDef)->ParentDECL->FullName;
    (*pelDef)->CompleteFullName();
  }
  else
    (*pelDef)->FullName = (*pelDef)->LocalName;
  elDef = *pelDef;
  switch ((*pelDef)->DeclType) {
    case VirtualType:
      ExecSyl->ExecDefs(pelDef, level);
      break;
    case Attr:
      ExecSyl->ExecMember(pelDef, level);
      break;
    case FormText:
      ExecSyl->ExecFText(pelDef, level);
      break;
    case IAttr:
      ExecSyl->ExecIn(pelDef, level);
      break;
    case OAttr:
      ExecSyl->ExecOut(pelDef, level);
      break;
    case FormDef:
      ExecSyl->ExecFormDef(pelDef, level);
      break;
    case NoDef:
      break;  //the ...->Iter.ptr)->IteratedExpr.ptr
    case Require:
    case Ensure:
    case ExecDef:
      break;
    default:
      ExecSyl->ExecDefs(pelDef, level);
  }
  if (!elRemoved) {
    parent_p_DECL = pelDef;
    localparent_p_DECL = pelDef;
    parent_p_DECL = localparent_p_DECL;
  }

   switch ((*pelDef)->DeclDescType) {
    case LiteralString:
      ExecSyl->ExecLit(pelDef);
      break;
    case ExecDesc:
      if ((*pelDef)->ParentDECL && (*pelDef)->ParentDECL->FullName.l)
        (*pelDef)->FullName = (*pelDef)->ParentDECL->FullName;
      ExecSyl->ExecExec(pelDef, level);
      break;
    case Undefined:         //das sind die Literale
      ExecSyl->ExecUndef(pelDef,name);
      break;
    case NamedType:
      ExecSyl->ExecNamed(pelDef,name);
      break;
    case BasicType:
      ExecSyl->ExecBasic(pelDef,name);
      break;

    case EnumType:
      ExecSyl->ExecEnum(pelDef, name);
      ExecSyl->ExecStruct(pelDef, name);
      if (!elRemoved) {
        inEl = &((TEnumDescription*)(*pelDef)->EnumDesc.ptr)->EnumField;
        inEl->ParentDECL = *pelDef;
        for (enumsel = (CHEEnumSelId*)inEl->Items.first; enumsel; enumsel = (CHEEnumSelId*)enumsel->successor)
          ExecSyl->ExecEnumItem(enumsel, level +1);
        elRemoved = FALSE;
      }
//      break;
    case StructDesc:
      ExecSyl->ExecStruct(pelDef, name);
      if (!elRemoved) {
        inCheEl = (CHE*)(*pelDef)->NestedDecls.first;
        while (inCheEl) {
          ((LavaDECL*)inCheEl->data)->FullName = (*pelDef)->FullName;
          ((LavaDECL*)inCheEl->data)->ParentDECL = *pelDef;
          NinCheEl = (CHE*)inCheEl->successor;
          DownTree((LavaDECL**)&inCheEl->data, level+1, noName);
          parent_p_DECL = localparent_p_DECL;
          inCheEl = NinCheEl;
        }
        elRemoved = FALSE;
      }
      break;
    default: ;
  }
}// DownTree

void CTabTraversal::Run(bool withINCL, bool withStd)
{
  TIDTable *IDTable = (TIDTable*)mySynDef->IDTable;
  int incl, id;
  for (incl = 0; !Done && (incl < IDTable->freeINCL); incl++) {
    if ((withStd || (incl != 1)) && (IDTable->IDTab[incl]->isValid)) {
      for (id = 0; !Done && (id < IDTable->IDTab[incl]->freeID); id++) {
        if (IDTable->IDTab[incl]->SimpleIDTab[id]->idType == globalID) {
          switch ((*IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL)->DeclType) {
          case VirtualType:
            ExecTab->ExecDefs(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          case FormDef:
            ExecTab->ExecFormDef(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          case Attr:
            ExecTab->ExecMember(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          case FormText:
            ExecTab->ExecFText(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          case IAttr:
            ExecTab->ExecIn(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          case OAttr:
            ExecTab->ExecOut(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
            break;
          default: 
            ExecTab->ExecDefs(IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL, incl);
          }//switch
        }//if
      }//for
    }//if
    if (!withINCL)
      return;
  }//for
}


CExec::~CExec()
{ 
  if (Travers)
    delete Travers;
  if (TabTravers)
    delete TabTravers;
}


CFindLikeForm::CFindLikeForm(SynDef *mySynDef, const TID& refID, int inINC, LavaDECL *visibleIn, LavaDECL* guiDECL)
{
  RefID = refID; //of the basic type
  inINCL = inINC;
  GUIDECL = guiDECL;
  visibleDECL = visibleIn;
  if (GUIDECL)
    GUIID = TID(GUIDECL->OwnID, GUIDECL->inINCL);
  pdecl = 0;
  IDTab = (TIDTable*)mySynDef->IDTable;
  TabTravers = new CTabTraversal(this, mySynDef);
  TabTravers->Run();
}

void CFindLikeForm::ExecFormDef(LavaDECL ** pelDef, int )
{
  if ((*pelDef)->usableIn(visibleDECL))
    if (IDTab->EQEQ((*pelDef)->RefID, (*pelDef)->inINCL, RefID, inINCL)) {
      if (!GUIDECL
        || IDTab->EQEQ(((CHETID*)(*pelDef)->ParentDECL->Supports.first)->data, (*pelDef)->inINCL, GUIID, 0)) {
        pdecl = pelDef;
        TabTravers->Done = TRUE;
      }
    }
}


CExecSetImpls::CExecSetImpls(SynDef *syn)
{
  IDTab = (TIDTable*)syn->IDTable;
  TabTravers = new CTabTraversal(this, syn);
  rs = true;
  TabTravers->Run(true, true);
  TabTravers->Done = false;
  rs = false;
  TabTravers->Run(true, true);
}


void CExecSetImpls::ExecDefs (LavaDECL ** pelDef, int )
{
  LavaDECL *intf;
  CHETID* che;
  if (rs) {
    if ((*pelDef)->DeclType == Interface)
      (*pelDef)->ImplIDs.Destroy();
  }
  else
    if ((*pelDef)->DeclType == Impl) {
      intf = IDTab->GetDECL(((CHETID*)(*pelDef)->Supports.first)->data, (*pelDef)->inINCL);
      if (intf) {
        che = new CHETID;
        che->data = TID((*pelDef)->OwnID, (*pelDef)->inINCL);
        intf->ImplIDs.Append(che);
      }
    }
}
