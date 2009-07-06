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

#include "SynIDTable.h"
#include "SynIO.h"
#include "Syntax.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"

#include "qobject.h"
#include "qstring.h"
#include "SYSTEM.h"

#pragma hdrstop


TSimpleTable::TSimpleTable(int minimum)
{
  maxID = (minimum/1000 + 1) * 1000;
  SimpleIDTab = new TIDEntry* [maxID];
  for (int ii = 0; ii < maxID; ii++)
    SimpleIDTab[ii] = new TIDEntry;
  nINCLTrans = 0;
  isValid = false;
  SimpleIDTab[0]->idType = noID;
  maxTrans = 0;
  freeID = 1;
}

TSimpleTable::~TSimpleTable()
{
  for (int ii = 0; ii < maxID; ii++)
    delete SimpleIDTab[ii];
  delete [] SimpleIDTab;
  if (nINCLTrans)
    delete [] nINCLTrans;
}

void TSimpleTable::AddID(LavaDECL ** pdecl)
{
  if ((*pdecl)->OwnID >= maxID) {
    int oldMaxID = maxID;
    for (; (*pdecl)->OwnID >= maxID; maxID += 1000);
    TIDEntry **idTab = new  TIDEntry* [maxID];
    int i1;
    for (i1=0; i1<oldMaxID; i1++)
      idTab[i1] = SimpleIDTab[i1];
    for (i1 = oldMaxID; i1< maxID; i1++)
      idTab[i1] = new TIDEntry;
    delete [] SimpleIDTab;
    SimpleIDTab = idTab;
  }
  if ((*pdecl)->OwnID >= 0) {
    SimpleIDTab[(*pdecl)->OwnID]->pDECL = pdecl;
    SimpleIDTab[(*pdecl)->OwnID]->idType = globalID;
    if ((*pdecl)->OwnID >= freeID)
      freeID = (*pdecl)->OwnID+1;
  }

}

void TSimpleTable::AddLocalID(DWORD pvar, int id)
{
  if (id >= maxID) {
    for (; id >= maxID; maxID += 1000);
    TIDEntry **idTab = new  TIDEntry* [maxID];
    int i1;
    for (i1=0; i1<freeID; i1++)
      idTab[i1] = SimpleIDTab[i1];
    for (i1 = freeID; i1< maxID; i1++)
      idTab[i1] = new TIDEntry;
    delete [] SimpleIDTab;
    SimpleIDTab = idTab;
  }
  if (id >= 0) {
    SimpleIDTab[id]->pDECL = (LavaDECL**)pvar;
    SimpleIDTab[id]->idType = localID;
    if (id >= freeID)
      freeID = id+1;
  }
}


void TSimpleTable::NewID(LavaDECL ** pdecl)
{
  (*pdecl)->OwnID = freeID;
  AddID(pdecl);
}

int TSimpleTable::NewLocalID(DWORD pvar)
{
  int id = freeID;
  AddLocalID(pvar, id);
  return id;
}

TIDType TSimpleTable::DeleteID(int delID)
{
  TIDType type = SimpleIDTab[delID]->idType;
  SimpleIDTab[delID]->idType = noID;
  return type;
}

void TSimpleTable::UndoDeleteID(int delID)
{
  SimpleIDTab[delID]->idType = globalID;
}

void TSimpleTable::UndoDeleteLocalID(int delID)
{
  SimpleIDTab[delID]->idType = localID;
}


TIDTable::TIDTable()
{
  freeINCL = 0;
  InCopy = false;
  ClipTree = 0;
  maxINCL = 0;
  IDTab = 0;
  useInINCL = -1;
  inPattern = false;
  DragINCL = 0;
  DropINCL = 0;
  lastImpl = 0;
  catchfuncDecl = 0;
  //implOfExToBase = 0;
  inDragExToBase = false;
  inDropExToBase = false;
  inExToBase = false;
  inUpdateDoc = false;

  for (int ii = 0; ii < 30; ii++)
    BasicTypesID [ii] = -1;

}

TIDTable::~TIDTable()
{
  isAncChain.DestroyKeepElems();
  DestroyTable();
  //if (IDTab)
  //  delete [] IDTab;
}

void TIDTable::DestroyTable()
{
  if (IDTab) {
    for (int i1 = 0; i1 < maxINCL; i1++) {
      for (int i2 = 0; i2 < IDTab[i1]->maxID; i2++) {
        delete IDTab[i1]->SimpleIDTab[i2];
        IDTab[i1]->SimpleIDTab[i2] = 0;
      }
      delete IDTab[i1];
    }
    delete [] IDTab;
    IDTab = 0;
  }
}

void TIDTable::AddID(LavaDECL ** pdecl, int incl)
{
  (*pdecl)->inINCL = incl;
  IDTab[incl]->AddID(pdecl);
  (*pdecl)->VElems.UpdateNo = 0;
  if (!inUpdateDoc && (((*pdecl)->DeclType == Impl) || ((*pdecl)->DeclType == CompObj))) {
    (*pdecl)->RuntimeDECL = lastImpl;
    lastImpl = *pdecl;
  }
  if (((*pdecl)->DeclType == Function)
           && !(*pdecl)->TypeFlags.Contains(isAbstract)
           && !(*pdecl)->TypeFlags.Contains(isNative)
      || ((*pdecl)->DeclType == Attr)
         && (*pdecl)->TypeFlags.Contains(hasSetGet)
         && !(*pdecl)->TypeFlags.Contains(isAbstract)
         && !(*pdecl)->TypeFlags.Contains(isNative)
        /* && !(*pdecl)->ParentDECL->TypeFlags.Contains(isComponent)*/)
    (*pdecl)->ParentDECL->WorkFlags.INCL(implRequired);

  Down(*pdecl, onAddID, incl);
}

void TIDTable::SetPatternStart(int overID, int newID)
{
  CHETID *che = new CHETID;
  che->data = TID(overID, newID); //old ID, new ID
  ChangeTab.Append(che);
}

int TIDTable::GetINCL(const DString& otherSytaxName, const DString& otherDocDir)
{
  CHESimpleSyntax* cheSyn;
  for (cheSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
       cheSyn && !SameFile(otherSytaxName, otherDocDir, cheSyn->data.SyntaxName, DocDir);
       cheSyn = (CHESimpleSyntax*)cheSyn->successor);
  if (cheSyn)
    return cheSyn->data.nINCL;
  else
    return -1;
}

void TIDTable::NewID(LavaDECL ** pdecl)
{
  CHETID *cheID, *che;
  CHESimpleSyntax *clipChe;
  bool copyStart = !inPattern && ((*pdecl)->inINCL != 0)
                    || ((*pdecl)->DeclType == PatternDef)
                    || ((*pdecl)->DeclType == DragDef);
  if (copyStart)
    inPattern = ((*pdecl)->DeclType == PatternDef);
  if (copyStart || InCopy) {  //the DECL is copied from another document or is a collection of pattern definitions
    if (((*pdecl)->DeclType != PatternDef)
        && ((*pdecl)->DeclType != DragDef)) {
      che = new CHETID;
      che->data.nID = (*pdecl)->OwnID; //old ID
      IDTab[0]->NewID(pdecl);
      if ((*pdecl)->DeclType == Impl)
        AddImplID(*pdecl);
      che->data.nINCL = (*pdecl)->OwnID; //new ID
      ChangeTab.Append(che);
      if (inDropExToBase && (*pdecl)->DeclType == Function) {
        cheID = new CHETID;
        cheID->data = TID((*pdecl)->OwnID, 0);
        implOfExToBase.Append(cheID);
      }
      if (inPattern && che->data.nID >= 0) {
        cheID = new CHETID;
        cheID->data = TID(che->data.nID, (*pdecl)->inINCL);
        (*pdecl)->Supports.Prepend(cheID);
        if ((*pdecl)->DeclType != Interface)
          (*pdecl)->SecondTFlags.INCL(overrides);

      }
    }
    else
      if (!ClipTree)
        useInINCL = (*pdecl)->inINCL;
    InCopy = true;
    (*pdecl)->inINCL = 0;
  }
  else
    if (((*pdecl)->DeclType != PatternDef)
        && ((*pdecl)->DeclType != DragDef)) {
      IDTab[0]->NewID(pdecl);
      if ((*pdecl)->DeclType == Impl)
        AddImplID(*pdecl);
    }
  Down(*pdecl, onNewID);

  if (copyStart) {
    if (ClipTree) {
      DString clipDocDir = *ClipDocPathName;
      CalcDirName(clipDocDir);
      clipChe = (CHESimpleSyntax*)ClipTree->first;
      while (clipChe) {
        if (clipChe->data.nINCL == 1)
          clipChe->data.clipIncl = 1;
        else
          clipChe->data.clipIncl = GetINCL(clipChe->data.SyntaxName, clipDocDir);
        /*
        for (cheSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
             cheSyn && !SameFile(clipChe->data.SyntaxName, clipDocDir, cheSyn->data.SyntaxName, DocDir);
             cheSyn = (CHESimpleSyntax*)cheSyn->successor);
        if (cheSyn)
          clipChe->data.clipIncl = cheSyn->data.nINCL;
        else
          if (clipChe->data.nINCL == 1)
            clipChe->data.clipIncl = 1;
          else
            clipChe->data.clipIncl = -1;
            */
        clipChe = (CHESimpleSyntax*)clipChe->successor;
      }
    }
    DragINCL = 0;
    DropINCL = -1;
    CopiedToDoc(pdecl);
    if (ClipTree && !(*pdecl)->WorkFlags.Contains(fromPrivToPub)
                 && !(*pdecl)->WorkFlags.Contains(ImplFromBaseToEx)) {
      delete ClipTree;
      ClipTree = 0;
    }
    else
      if ((*pdecl)->WorkFlags.Contains(ImplFromBaseToEx))
        ClipTree = 0;
    useInINCL = -1;
    inPattern = false;
    InCopy = false;
  }
}

void TIDTable::AddLocalID(DWORD pvar, int id, int incl)
{
  IDTab[incl]->AddLocalID(pvar, id);
}

void TIDTable::NewLocalID(DWORD pvar, int& id, bool execCopy)
{
  int oldID = id;
  id = IDTab[0]->NewLocalID(pvar);
  if (InCopy || execCopy) {
    CHETID *che = new CHETID;
    che->data = TID(oldID, id);
    ChangeTab.Append(che);
  }
}

void TIDTable::ChangeIDFromTab(int& id)
{
  CHETID *che;
  for (che = (CHETID*)ChangeTab.first;
       che && (che->data.nID != id);
       che = (CHETID*)che->successor);
  if (che)
    id = che->data.nINCL;
  else
    id = -1;
}

bool TIDTable::ChangeFromTab(TID& id)
{
  CHESimpleSyntax* clipChe;
  TID id2;
  CHETID *che = (CHETID*)ChangeTab.first;
  if (inExToBase) {
    if (id.nID < -1) {
      id.nID = -id.nID;
      if ((id.nID >= 0) && (id.nINCL == DragINCL)) {
        for ( ;che && (che->data.nID != id.nID); che = (CHETID*)che->successor);
        if (che) {
          id.nID = che->data.nINCL;
          return true;
        }
      }
    }
  }
  else {
    if ((id.nID >= 0) && (id.nINCL == DragINCL)) {
      for ( ;che && (che->data.nID != id.nID); che = (CHETID*)che->successor);
      if (che) {
        id.nID = che->data.nINCL;
        return true;
      }
    }
  }
  //only in the the copied subtree:
  if ((id.nID >= 0) && (id.nINCL == DropINCL)) {
    //special: public function to private:
    //map the id of the old implementation io to the mapped old interface io
    for ( che = (CHETID*)ChangeTab.first;
      che && (che->data.nID != -id.nID);
      che = (CHETID*)che->successor);
    if (che) {
      id = TID(che->data.nINCL, DragINCL);
      return ChangeFromTab(id);
    }
  }
  if (DropINCL < 0) {
    if ((id.nID >= 0) && ClipTree) {
      for (clipChe = (CHESimpleSyntax*)ClipTree->first;
           clipChe && (clipChe->data.nINCL != id.nINCL);
           clipChe = (CHESimpleSyntax*)clipChe->successor);
      if (clipChe && (clipChe->data.clipIncl >= 0))
        id.nINCL = clipChe->data.clipIncl;
      else {
        if (inDropExToBase )
          id.nID = -id.nID;
        else
          id.nID = -1;
      }
    }
    else
      if ((id.nID >= 0) && (useInINCL >= 0))
        id.nINCL = IDTab[useInINCL]->nINCLTrans[id.nINCL].nINCL;
      else
        id.nID = -1;
  }
  return false;
}

void TIDTable::CopiedToDoc(LavaDECL ** pnewDECL)
{
  LavaDECL *decl = *pnewDECL;
  CHETID *che;
  CHETIDs *cheTIDs;
//  if (decl->WorkFlags.Contains(skipOnCopy))
//    return;
//  if (!decl->WorkFlags.Contains(fromPubToPriv))
  ChangeFromTab(decl->RefID);
  if (!decl->WorkFlags.Contains(fromPrivToPub) && !decl->WorkFlags.Contains(SupportsReady)) {
    che = (CHETID*)decl->Supports.first;
    if (inPattern && che)
      che = (CHETID*)che->successor; //skip the first which is the overwritten
    for (; che; che = (CHETID*)che->successor)
      ChangeFromTab(che->data);
  }
  else
    decl->WorkFlags.EXCL(SupportsReady);
  for (che = (CHETID*)decl->Inherits.first; che; che = (CHETID*)che->successor)
    ChangeFromTab(che->data);
  for (cheTIDs = (CHETIDs*)decl->HandlerClients.first; cheTIDs; cheTIDs = (CHETIDs*)cheTIDs->successor) {
    for (che = (CHETID*)cheTIDs->data.first; che; che = (CHETID*)che->successor) 
      ChangeFromTab(che->data);
  }
  if ((decl->DeclType == VirtualType)
    && decl->ParentDECL
    && (decl->ParentDECL->DeclType == FormDef)
    && decl->Annotation.ptr
    && decl->Annotation.ptr->IterOrig.ptr
    && ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
    LavaDECL* CHEEl = ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
    ChangeFromTab(CHEEl->RefID);
  }
  Down(*pnewDECL, onCopy);
}

void TIDTable::StartClipIDs(int dragINCL, int dropINCL, TIDTable* dropTable)
{
  DragINCL = dragINCL;
  DropINCL = dropINCL;
  DropTable = dropTable;
  hasRefToClipID = false;
}

void TIDTable::ChangeRefToClipID(TID& id)
{
  if ((id.nINCL == DragINCL) || (id.nINCL == DropINCL))
    if (DropTable->ChangeFromTab(id)) {
      hasRefToClipID = true;
      id.nINCL = DropINCL;
    }
}

void TIDTable::ChangeRefsToClipIDs(LavaDECL *decl)
{
  CHETID *che;
  CHETIDs *cheTIDs;

  ChangeRefToClipID(decl->RefID);
  if (!decl->WorkFlags.Contains(fromPrivToPub) && !decl->WorkFlags.Contains(SupportsReady))
    for (che = (CHETID*)decl->Supports.first; che; che = (CHETID*)che->successor)
      ChangeRefToClipID(che->data);
  else
    decl->WorkFlags.EXCL(SupportsReady);
  for (che = (CHETID*)decl->Inherits.first; che; che = (CHETID*)che->successor)
    ChangeRefToClipID(che->data);
  for (cheTIDs = (CHETIDs*)decl->HandlerClients.first; cheTIDs; cheTIDs = (CHETIDs*)cheTIDs->successor) {
    for (che = (CHETID*)cheTIDs->data.first; che; che = (CHETID*)che->successor) 
     ChangeRefToClipID(che->data);
  }
  if ((decl->DeclType == VirtualType) && (decl->ParentDECL->DeclType == FormDef)
    && decl->Annotation.ptr
    && decl->Annotation.ptr->IterOrig.ptr
    && ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
    LavaDECL* CHEEl = ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
    ChangeRefToClipID(CHEEl->RefID);//ChangeFromTab(CHEEl->RefID);
  }
  Down(decl, onMove);
}

void TIDTable::ChangeRefsToClipIDsApx()
{
  LavaDECL *func, **pFunc;
  CHETID* cheID;
  TIDType type = globalID;
  int iDropINCL;
  if (inDragExToBase && implOfExToBase.first) {
    cheID = (CHETID*)implOfExToBase.first;
    func = ((CLavaBaseDoc*)DropImplDoc)->IDTable.GetDECL(cheID->data);
    if (func && func->Supports.first && (((CHETID*)func->Supports.first)->data.nINCL == DragINCL)) {
      inExToBase = true;
      DropTable->inExToBase = true;
      for (; cheID; cheID = (CHETID*)cheID->successor) {
        pFunc = (LavaDECL**)((CLavaBaseDoc*)DropImplDoc)->IDTable.GetVar(cheID->data, type);
        if (*pFunc) {
           iDropINCL = DropINCL;
          //if (DropTable == &((CLavaBaseDoc*)DropImplDoc)->IDTable)
          //  DropTable->CopiedToDoc(pFunc);
          //else {
            DropINCL = ((CLavaBaseDoc*)DropImplDoc)->IDTable.GetINCL(((CHESimpleSyntax*)DropTable->mySynDef->SynDefTree.first)->data.SyntaxName, DropTable->DocDir);
            ChangeRefsToClipIDs(*pFunc);
            DropINCL = iDropINCL;
          }
      }
      DropTable->inExToBase = false;
      inDragExToBase = false;
      implOfExToBase.Destroy();
      inExToBase = false;
      DropImplDoc = 0;
    }
  }
}

void TIDTable::DownChange(LavaDECL ** pnewDECL)
{
  if ((*pnewDECL)->OwnID >= 0) {
    IDTab[0]->SimpleIDTab[(*pnewDECL)->OwnID]->pDECL = pnewDECL;
    IDTab[0]->SimpleIDTab[(*pnewDECL)->OwnID]->idType = globalID;
  }
  else {
    IDTab[0]->NewID(pnewDECL);
    if ((*pnewDECL)->DeclType == Impl)
      AddImplID(*pnewDECL);
  }
  Down(*pnewDECL, onChange);
}

void TIDTable::Change(LavaDECL ** pnewDECL)
{
  if ((*pnewDECL)->inINCL)
   NewID(pnewDECL);
  else {
    if ((*pnewDECL)->OwnID >= 0) {
      if ((*pnewDECL)->FullName.l ) {
        IDTab[0]->SimpleIDTab[(*pnewDECL)->OwnID]->pDECL = pnewDECL;
        IDTab[0]->SimpleIDTab[(*pnewDECL)->OwnID]->idType = globalID;
      }
      else { //a named package becomes unnamed
        IDTab[0]->DeleteID((*pnewDECL)->OwnID);
        (*pnewDECL)->OwnID = -1;
        ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr = *pnewDECL;
      }
    }
    else { //a unnamed package gets a name, or automatic insertion of a func-impl, func-IO, form-field
      IDTab[0]->NewID(pnewDECL);
      if ((*pnewDECL)->DeclType == Package)
        ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr = *pnewDECL;
    }
    Down(*pnewDECL, onChange);
  }
}

void TIDTable::UndoChange(LavaDECL ** poldDECL, LavaDECL* newDECL)
{
  if (newDECL)
    if ((newDECL->OwnID >= 0)  && (*poldDECL)->OwnID < 0)
      /* ??
      && ( ((*poldDECL)->OwnID < 0)
         || !(*poldDECL)->LocalName.l
         ||      ((*poldDECL)->FullName != newDECL->FullName)
              && !isPartOf((*poldDECL)->FullName, newDECL->FullName)
              && !isPartOf(newDECL->FullName, (*poldDECL)->FullName) ) )
     */
      DeleteID(newDECL->OwnID);
    else
      Down(newDECL, onDeleteID);
  if ((*poldDECL)->OwnID >= 0) {
    IDTab[0]->SimpleIDTab[(*poldDECL)->OwnID]->pDECL = poldDECL;
    IDTab[0]->SimpleIDTab[(*poldDECL)->OwnID]->idType = globalID;
    IDTab[0]->SimpleIDTab[(*poldDECL)->OwnID]->newID = -1;
  }
  if (((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr == newDECL)
    ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr = *poldDECL;

  Down(*poldDECL, onUndoChange);
}

void TIDTable::ChangeLocalID(DWORD pvar, int id)
{
  if (id > -1) {
    IDTab[0]->SimpleIDTab[id]->pDECL = (LavaDECL**)pvar;
    IDTab[0]->SimpleIDTab[id]->idType = localID;
  }
}

void TIDTable::DeleteID(int delID)
{
  LavaDECL *elDecl;
  if ((delID>=0) && IDTab[0]->DeleteID(delID) == globalID) {
    elDecl = *IDTab[0]->SimpleIDTab[delID]->pDECL;
    if (elDecl && (elDecl->DeclType == Impl))
      RemoveImplID(elDecl);
    Down(elDecl, onDeleteID);
  }
}

void TIDTable::UndoDeleteID(int delID)
{
  if (delID>=0) {
    IDTab[0]->UndoDeleteID(delID);
    Down(*IDTab[0]->SimpleIDTab[delID]->pDECL, onUndoDeleteID);
  }
}

void TIDTable::UndoDeleteLocalID(int delID)
{
  if (delID>=0)
    IDTab[0]->UndoDeleteLocalID(delID);
}


void TIDTable::AddINCL(const DString& name, int incl)
{
  if (incl >= maxINCL) {
    int usedINCL = lmin(maxINCL,freeINCL);
    for (; incl >= maxINCL; maxINCL += 5);
    TSimpleTable **idTab = new  TSimpleTable* [maxINCL];
    int ii;
    for ( ii=0; ii<usedINCL; ii++)
      idTab[ii] = IDTab[ii];
    for (ii = usedINCL; ii < maxINCL; ii++)
      idTab[ii] = new TSimpleTable();
    if (IDTab)
      delete [] IDTab;
    IDTab = idTab;
  }
  IDTab[incl]->FileName = name;
  IDTab[incl]->isValid = true;
  if (incl >= freeINCL)
    freeINCL = incl+1;
  if (IDTab[0]->nINCLTrans) {
    IDTab[0]->maxTrans = lmin(IDTab[0]->maxTrans, incl)+1;
    IDTab[0]->nINCLTrans[incl].nINCL = incl;
    IDTab[0]->nINCLTrans[incl].isValid = true;
    IDTab[0]->nINCLTrans[incl].FileName = name;
  }
}

int TIDTable::NewINCL(const DString& name)
{
  AddINCL(name, freeINCL);
  return freeINCL-1;
}

void TIDTable::DeleteINCL(int nINCL)
{
  IDTab[nINCL]->isValid = false;
}

void TIDTable::UndoDeleteINCL(int nINCL)
{
  IDTab[nINCL]->isValid = true;
}


LavaDECL* TIDTable::GetDECL(const TID& id, int fromIncl)
{
  return GetDECL(id.nINCL, id.nID, fromIncl);
}

LavaDECL* TIDTable::GetDECL(int incl, int id, int fromIncl)
{
  int inc;
  if ((fromIncl >= maxINCL) || (fromIncl < 0) || (incl < 0) || (id < 0))
    return 0;
  if (fromIncl) {
    if (incl >= IDTab[fromIncl]->maxTrans)
      return 0;
    inc = IDTab[fromIncl]->nINCLTrans[incl].nINCL;
  }
  else
    inc = incl;
  if ((inc >= 0) && (inc < freeINCL) && IDTab[inc]->isValid && (id < IDTab[inc]->maxID) && (IDTab[inc]->SimpleIDTab[id]->idType == globalID))
    return *IDTab[inc]->SimpleIDTab[id]->pDECL;
  else
    return 0;
}

DWORD TIDTable::GetVar(const TID& id, TIDType& idtype, int fromIncl)
{
  int incl;

  if ((fromIncl >= maxINCL) || (fromIncl < 0) || (id.nINCL < 0) || (id.nID < 0))
    return 0;
  if (fromIncl) {
    if (id.nINCL >= IDTab[fromIncl]->maxTrans)
      return 0;
    incl = IDTab[fromIncl]->nINCLTrans[id.nINCL].nINCL;
  }
  else
    incl = id.nINCL;
  if ((incl >= 0) && IDTab[incl]->isValid && (id.nID < IDTab[incl]->maxID)) {
    idtype = IDTab[incl]->SimpleIDTab[id.nID]->idType;
    switch (idtype) {
      case noID:
        return 0;
      case globalID:
        return (DWORD)IDTab[incl]->SimpleIDTab[id.nID]->pDECL;
      case localID:
        return (DWORD)IDTab[incl]->SimpleIDTab[id.nID]->pDECL;
    }
    return 0;
  }
  else {
    idtype = noID;
    return 0;
  }
}

void TIDTable::SetVar(const TID& id, DWORD data)
{
  if ((id.nID >= 0)
      && IDTab[id.nINCL]->isValid
      && (id.nID < IDTab[id.nINCL]->freeID ) )
    IDTab[id.nINCL]->SimpleIDTab[id.nID]->pDECL = (LavaDECL**)data;
}


DString TIDTable::GetRelSynFileName(const TID& id)
{
  if ((id.nID >= 0) && IDTab[id.nINCL]->isValid )
    return IDTab[id.nINCL]->FileName;
  else {
    DString str;
    return str;
  }
}

bool TIDTable::EQEQ(const TID& id1, int inINC1, const TID& id2, int inINC2)
{
  return (id1.nID == id2.nID)
         && IDTab[inINC1]->nINCLTrans[id1.nINCL].isValid
         && IDTab[inINC2]->nINCLTrans[id2.nINCL].isValid
         && ( IDTab[inINC1]->nINCLTrans[id1.nINCL].nINCL == IDTab[inINC2]->nINCLTrans[id2.nINCL].nINCL );
}

void TIDTable::SetAsName(int incl, const DString& newAsName, const DString& oldAsName, LavaDECL* topDECL )
{
  int dell;
  LavaDECL* decl;
  for (int id = 0; (id < IDTab[incl]->freeID); id++) {
    if (IDTab[incl]->SimpleIDTab[id]->idType == globalID) {
      decl = *IDTab[incl]->SimpleIDTab[id]->pDECL;
      if (oldAsName.l) {
        dell = oldAsName.l;
        if (!newAsName.l && decl->FullName.l) {
          dell++;
          if (decl->FullName[dell] == ':')
            dell++;
        }
        decl->FullName.Delete(0, dell);

      }
      if (newAsName.l) {
        if (!oldAsName.l && decl->FullName.l)
          if ((decl->ParentDECL == topDECL) && (decl->DeclType == VirtualType))
            decl->FullName.Insert(pkt, 0);
          else
            decl->FullName.Insert(ddppkt, 0);
        decl->FullName.Insert(newAsName, 0);
      }

    }//if
  }//for
}


int TIDTable::AddSimpleSyntax(SynDef *isyntax, const DString& idocDir, bool reload, CHESimpleSyntax* ownSyn)
{
  CHESimpleSyntax *simpleSyntax = (CHESimpleSyntax*)isyntax->SynDefTree.first;
  DString relFn = simpleSyntax->data.SyntaxName;
  int incl, in, is = 1, usedINCL = lmin(maxINCL, freeINCL);

  AbsPathName(relFn, idocDir);
  RelPathName(relFn, DocDir);
  inUpdateDoc = reload;
  for ( incl = 0;
        (incl < usedINCL)
          && (!IDTab[incl]->isValid || !SameFile(IDTab[incl]->FileName, DocDir, relFn, DocDir));
        incl++);
  if (incl < freeINCL) {
    if (!IDTab[incl]->isValid)
      reload = true;
    AddINCL(relFn, incl);
  }
  else
    incl = NewINCL(relFn);
  if (incl == 1)
    FillBasicTypesID(simpleSyntax);
  if (reload) {
    for (int ii = 0; ii < IDTab[incl]->freeID; ii++)
      IDTab[incl]->SimpleIDTab[ii]->idType = noID;
    if (ownSyn) {
      if (ownSyn->data.TopDef.ptr)
        AddID(&ownSyn->data.TopDef.ptr, incl);
    }
    else
      if (simpleSyntax->data.TopDef.ptr)
        AddID(&simpleSyntax->data.TopDef.ptr, incl);
  }
  else
    if (IDTab[incl]->isValid ) {
      if (simpleSyntax->data.TopDef.ptr)
        AddID(&simpleSyntax->data.TopDef.ptr, incl);
    }
  if (incl) {
    for (simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
         simpleSyntax;
         simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor)
      is = lmax(is,simpleSyntax->data.nINCL+1);
  }
  else
    is = 1000;
  if (reload) {
    if (IDTab[incl]->maxTrans < is) {
      delete [] IDTab[incl]->nINCLTrans;
      IDTab[incl]->nINCLTrans = new TINCLTrans [is];
      IDTab[incl]->maxTrans = is;
    }
  }
  else {
    IDTab[incl]->nINCLTrans = new TINCLTrans [is];
    IDTab[incl]->maxTrans = is;
  }
  IDTab[incl]->nINCLTrans[0].isValid = true;
  IDTab[incl]->nINCLTrans[0].nINCL = incl;
  IDTab[incl]->nINCLTrans[0].FileName = relFn;
  simpleSyntax = (CHESimpleSyntax*)isyntax->SynDefTree.first->successor;
  while ( simpleSyntax) {
    IDTab[incl]->nINCLTrans[simpleSyntax->data.nINCL].isValid = true;
    relFn = simpleSyntax->data.SyntaxName;
    AbsPathName(relFn, idocDir);
    RelPathName(relFn, DocDir);
    for ( in = 0;
          (in < freeINCL) && (!IDTab[in]->isValid || !SameFile(IDTab[in]->FileName, DocDir, relFn, DocDir));
          in++);
    if (in < freeINCL)
      IDTab[incl]->nINCLTrans[simpleSyntax->data.nINCL].nINCL = in;
    else {
      in = NewINCL(relFn); //not yet in mySynDef,
      IDTab[incl]->nINCLTrans[simpleSyntax->data.nINCL].nINCL = in;
    }
    IDTab[incl]->nINCLTrans[simpleSyntax->data.nINCL].FileName = relFn;
    simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
  }
  inUpdateDoc = false;
  return incl;
}

void TIDTable::RemoveFromInclTrans(int incl, const DString& fileName)
{
  int ii;
  for (ii = 2; (ii < IDTab[incl]->maxTrans)
               && (!IDTab[incl]->nINCLTrans[ii].isValid
                   || !SameFile(IDTab[incl]->nINCLTrans[ii].FileName, DocDir, fileName, DocDir)); ii++);
  if (ii < IDTab[incl]->maxTrans)
    IDTab[incl]->nINCLTrans[ii].isValid = false;
}

int TIDTable::IsFileInherited(const DString& fileName)
{
  int in, ii;
  for (in = 1; in < freeINCL; in++)
    if (IDTab[in]->isValid) {
      for (ii = 2; (ii < IDTab[in]->maxTrans)
                   && (!IDTab[in]->nINCLTrans[ii].isValid
                      || !SameFile(IDTab[in]->nINCLTrans[ii].FileName, DocDir, fileName, DocDir)); ii++);
      if (ii < IDTab[in]->maxTrans)
        return in;
    }
  return 0;
}

bool TIDTable::IsFileInherited(const DString& relFileName, int incl)
{
  int ii;

  if (IDTab[incl]->isValid) {
    for (ii = 2; (ii < IDTab[incl]->maxTrans)
                 && (!IDTab[incl]->nINCLTrans[ii].isValid
                    || !SameFile(IDTab[incl]->nINCLTrans[ii].FileName, DocDir, relFileName, DocDir)); ii++);
    if (ii < IDTab[incl]->maxTrans)
      return true;
  }
  return false;
}

void TIDTable::SetInclTransValid(int incl, const DString& fileName, int newINCL)
{
  int ii, is;
  TINCLTrans *nTrans;
  if (newINCL >= IDTab[incl]->maxTrans) {
    nTrans = IDTab[incl]->nINCLTrans;
    is = IDTab[incl]->maxTrans;
    IDTab[incl]->maxTrans = newINCL + 5;
    IDTab[incl]->nINCLTrans = new TINCLTrans [IDTab[incl]->maxTrans];
    for (ii = 0;  ii < is; ii++)
      IDTab[incl]->nINCLTrans[ii] = nTrans[ii];
    delete [] nTrans;
  }
  IDTab[incl]->nINCLTrans[newINCL].isValid = true;
  IDTab[incl]->nINCLTrans[newINCL].FileName = fileName;
  for (ii = 0;
      (ii < freeINCL) && (!IDTab[ii]->isValid || !SameFile(IDTab[ii]->FileName, DocDir, fileName, DocDir));
       ii++);
  if (ii < freeINCL)
    IDTab[incl]->nINCLTrans[newINCL].nINCL = ii;
}

void TIDTable::FillBasicTypesID(CHESimpleSyntax* stdSyntax, bool isstd)
{
  int ii = 1;
  CHE *che, *cheChe;
  che = (CHE*)((LavaDECL*)stdSyntax->data.TopDef.ptr)->NestedDecls.first;
  LavaDECL* elDECL, *paramDECL;
  while (che && (ii < (int)Identifier)) {
    elDECL = (LavaDECL*)che->data;
    BasicTypesID[ii] = elDECL->OwnID;
    elDECL->fromBType = (TBasicType)ii;
    if (elDECL->fromBType == B_Exception) {
      DString catchname = DString("catch");
      for (cheChe = (CHE*)elDECL->NestedDecls.first;
           cheChe && (((LavaDECL*)cheChe->data)->LocalName != catchname);
           cheChe = (CHE*)cheChe->successor);
      if (cheChe)
        catchfuncDecl = (LavaDECL*)cheChe->data;
    }
    if (isstd) {
      if (elDECL->fromBType == B_Set) {
        ((LavaDECL*)((CHE*)elDECL->NestedDecls.first)->data)->SecondTFlags.INCL(isSet);
        elDECL->SecondTFlags.INCL(isSet);
      }
      else if (elDECL->fromBType == B_Chain) {
        paramDECL =  (LavaDECL*)((CHE*)elDECL->NestedDecls.first)->data;
        paramDECL->SecondTFlags.INCL(isSet);
        paramDECL->SecondTFlags.INCL(isChain);
        elDECL->SecondTFlags.INCL(isSet);
        elDECL->SecondTFlags.INCL(isChain);
      }
      else if (elDECL->fromBType == B_Array) {
        ((LavaDECL*)((CHE*)elDECL->NestedDecls.first)->data)->SecondTFlags.INCL(isArray);
        elDECL->SecondTFlags.INCL(isArray);
      }
      //else if (elDECL->fromBType == B_GUI) {
      //  ((LavaDECL*)((CHE*)elDECL->NestedDecls.first)->data)->SecondTFlags.INCL(isGUI);
      //  ((LavaDECL*)((CHE*)elDECL->NestedDecls.first)->data)->SecondTFlags.EXCL(oldIsGUI);
      //  elDECL->SecondTFlags.INCL(isGUI);
      //  elDECL->SecondTFlags.EXCL(oldIsGUI);
      //}
      else if ((elDECL->fromBType == B_Exception)
            || (elDECL->fromBType == B_HWException)
            || (elDECL->fromBType == B_RTException)) {
        elDECL->SecondTFlags.INCL(isException);
        cheChe = (CHE*)elDECL->NestedDecls.first; //the enumaration type
        ((LavaDECL*)cheChe->data)->SecondTFlags.INCL(isException);
        for (; cheChe && (((LavaDECL*)cheChe->data)->LocalName != "catch");
               cheChe = (CHE*)cheChe->successor);
        if (cheChe)
          catchfuncDecl = (LavaDECL*)cheChe->data;
      }
    }
    ii++;
    che = (CHE*)che->successor;
  }
}


void TIDTable::MakeTable(SynDef *syn, bool isstd)
{
  int incl = 1, ii;
  mySynDef = syn;
  CHESimpleSyntax *simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
  while ( simpleSyntax) {
    incl = lmax(incl, simpleSyntax->data.nINCL);
    simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
  }
  simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
  freeINCL = mySynDef->FreeINCL;

  maxINCL = lmax(5, freeINCL) ;
  IDTab = new TSimpleTable* [maxINCL];
  IDTab[0] = new TSimpleTable(mySynDef->FreeID);
  if (maxINCL > 1)
    for (ii = 1; ii < maxINCL; ii++)
      IDTab[ii] = new TSimpleTable();
  IDTab[0]->isValid = true;

  IDTab[0]->freeID = mySynDef->FreeID;
  IDTab[0]->nINCLTrans = new TINCLTrans [1000];
  while ( simpleSyntax) {
    incl = simpleSyntax->data.nINCL;
    AddINCL(simpleSyntax->data.SyntaxName, incl);
    IDTab[incl]->isValid = true;
    IDTab[0]->nINCLTrans[incl].isValid = true;
    IDTab[0]->nINCLTrans[incl].nINCL = incl;
    IDTab[0]->nINCLTrans[incl].FileName = simpleSyntax->data.SyntaxName;
    if (simpleSyntax->data.TopDef.ptr)
      AddID(&simpleSyntax->data.TopDef.ptr, incl);
    simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
  }
  if (isstd)
    FillBasicTypesID((CHESimpleSyntax*)mySynDef->SynDefTree.first, isstd);

}//MakeTable

TID TIDTable::FindImpl(TID interID, int funcnID)
{
  CHE *che;
  LavaDECL *decl;
  int incl, id;
  if (GetDECL(interID)->DeclType == Impl)
    if (funcnID)
      return TID(funcnID, interID.nINCL);
    else
      return interID;
  for (incl = 0; incl < freeINCL; incl++)
    if (IDTab[incl]->isValid)
      for (id = 0; id < IDTab[incl]->freeID; id++)
        if (IDTab[incl]->SimpleIDTab[id]->idType == globalID) {
          decl = *IDTab[incl]->SimpleIDTab[id]->pDECL;
          if (((decl->DeclType == Impl) || (decl->DeclType == CompObj))
               && EQEQ(((CHETID*)decl->Supports.first)->data, incl, interID, 0)) {
            if (funcnID) {
              for (che = (CHE*)decl->NestedDecls.first;
                   che && (!((LavaDECL*)che->data)->Supports.first
                          || (funcnID != ((CHETID*)((LavaDECL*)che->data)->Supports.first)->data.nID));
                   che = (CHE*)che->successor);
              if (che)
                return TID(((LavaDECL*)che->data)->OwnID, incl);
            }
            else
              return TID(decl->OwnID, incl);
          }
        }
  return TID(-1,-1);
}


QString* TIDTable::SetImplDECLs(LavaDECL*& errDECL) //returns error code
{
  LavaDECL *classDECL, *nextImpl = lastImpl;
  while (nextImpl) {
    classDECL = GetDECL(((CHETID*)nextImpl->Supports.first)->data, nextImpl->inINCL);
    if (classDECL && !classDECL->RuntimeDECL)
      classDECL->RuntimeDECL = nextImpl;
    else {
      errDECL = nextImpl;
      if (classDECL)
        return &ERR_SecondImpl;
      else
        return &ERR_Broken_ref;
    }
    nextImpl = nextImpl->RuntimeDECL;
    classDECL->RuntimeDECL->RuntimeDECL = 0;
  }
  return 0;
}


void TIDTable::SetImplIDs(bool withTest)
{
  LavaDECL* classDECL, *nextImpl = lastImpl;
  CHETID* che;
  while (nextImpl) {
    classDECL = GetDECL(((CHETID*)nextImpl->Supports.first)->data, nextImpl->inINCL);
    if (classDECL) {
      if (withTest) {
        che = (CHETID*)classDECL->ImplIDs.first;
        while (che && (che->data != TID(nextImpl->OwnID, nextImpl->inINCL)))
          che = (CHETID*)che->successor;
      }
      else
        che = 0;
      if (!che) {
        che = new CHETID;
        che->data = TID(nextImpl->OwnID, nextImpl->inINCL);
        classDECL->ImplIDs.Append(che);
      }
    }
    lastImpl = nextImpl->RuntimeDECL;
    nextImpl->RuntimeDECL = 0;
    nextImpl = lastImpl;
  }
  lastImpl = 0;
}


void TIDTable::AddImplID(LavaDECL* impl)
{
  CHETID *che;
  LavaDECL *classDECL = GetDECL(((CHETID*)impl->Supports.first)->data, impl->inINCL);
  if (classDECL) {
    che = new CHETID;
    che->data = TID(impl->OwnID, impl->inINCL);
    classDECL->ImplIDs.Append(che);
  }
//  impl->RuntimeDECL = lastImpl;
//  lastImpl = impl;
}

void TIDTable::RemoveImplID(LavaDECL* impl)
{
  CHETID *che;
  //LavaDECL* imp;
  LavaDECL *classDECL = GetDECL(((CHETID*)impl->Supports.first)->data, impl->inINCL);
  if (classDECL) {
    che = (CHETID*)classDECL->ImplIDs.first;
    while (che && (che->data != TID(impl->OwnID, impl->inINCL)))
      che = (CHETID*)che->successor;
    if (che)
      classDECL->ImplIDs.Delete(che);
  }
  /*
  if (impl == lastImpl)
    lastImpl = impl->RuntimeDECL;
  else {
    imp = lastImpl;
    while (imp) {
      if (imp->RuntimeDECL == impl) {
        imp->RuntimeDECL = impl->RuntimeDECL;
        imp = 0;
      }
      imp = imp->RuntimeDECL;
    }
  }*/
}


void TIDTable::Down(LavaDECL *elDef, TTableUpdate onWhat, int nINCL)
{
  CHE *inCheEl;
  LavaDECL *inEl;
  CSearchData sData;
  TTableUpdate execOnWhat;

  if ((elDef->DeclDescType != ExecDesc)
      && (elDef->DeclType != PatternDef)
      && (elDef->DeclType != DragDef) ) {
    if (elDef->ParentDECL && elDef->ParentDECL->FullName.l) {
      elDef->FullName = elDef->ParentDECL->FullName;
      elDef->CompleteFullName();
    }
    else
      elDef->FullName = elDef->LocalName;
  }
  else
    if ((elDef->ParentDECL) && elDef->ParentDECL->FullName.l)
      elDef->FullName = elDef->ParentDECL->FullName;
  if ((elDef->DeclDescType == StructDesc) || (elDef->DeclDescType == EnumType)) {
    inCheEl = (CHE*)elDef->NestedDecls.first;
    while (inCheEl) {
      inEl = (LavaDECL*)inCheEl->data;
      if ((inEl->DeclType == Interface) || (inEl->DeclType == Package))
        inEl->WorkFlags.INCL(recalcVT);
      if (inEl->DeclDescType != ExecDesc) {
        switch (onWhat) {
        case onUndoDeleteID:
          UndoDeleteID(inEl->OwnID);
          break;
        case onDeleteID:
          DeleteID(inEl->OwnID);
          break;
        case onAddID:
          inEl->FullName = elDef->FullName;
          inEl->ParentDECL = elDef;
          if (inEl->TypeFlags.Contains(defaultInitializer))
            elDef->WorkFlags.INCL(hasDefaultIni);
          if (inEl->DeclType == VirtualType) {
            elDef->WorkFlags.INCL(isPattern);
            inEl->WorkFlags.INCL(isPartOfPattern);
          }
          //if ((mySynDef->Release < 5) && inEl->TypeFlags.Contains(oldIsGUI)) {
          //  inEl->SecondTFlags.INCL(isGUI);
          //  //inEl->TypeFlags.EXCL(oldIsGUI);
          //}
          if ((inEl->DeclType == Interface)
            && (elDef->WorkFlags.Contains(isPartOfPattern) || elDef->WorkFlags.Contains(isPattern)))
            inEl->WorkFlags.INCL(isPartOfPattern);
          AddID((LavaDECL**)&inCheEl->data, nINCL);
          break;
        case onNewID:
          inEl->FullName = elDef->FullName;
          if ((elDef->DeclType == PatternDef) || (elDef->DeclType == DragDef))
            inEl->ParentDECL = elDef->ParentDECL;
          else
            inEl->ParentDECL = elDef;
          if (inEl->TypeFlags.Contains(defaultInitializer))
            elDef->WorkFlags.INCL(hasDefaultIni);
          NewID((LavaDECL**)&inCheEl->data);
          break;
        case onChange:
          inEl->FullName = elDef->FullName;
          inEl->ParentDECL = elDef;
          DownChange((LavaDECL**)&inCheEl->data);
          break;
        case onUndoChange:
          UndoChange((LavaDECL**)&inCheEl->data);
          break;
        case onCopy:
          CopiedToDoc((LavaDECL**)&inCheEl->data);
          break;
        case onMove:
          inEl->ParentDECL = elDef;
          ChangeRefsToClipIDs((LavaDECL*)inCheEl->data);
          break;
        }
      }
      else { //exec
        if ( (onWhat == onAddID) || (onWhat == onNewID) || (onWhat == onChange)) {
          inEl->FullName = elDef->FullName;
          inEl->ParentDECL = elDef;
        }
        inEl->inINCL = nINCL;
        if (!nINCL || LBaseData->inRuntime || (onWhat == onMove)) {
          if (( (onWhat != onNewID) && (onWhat != onCopy)
                || !elDef->WorkFlags.Contains(skipOnCopy))
            && ((onWhat != onDeleteID) || !elDef->WorkFlags.Contains(skipOnDeleteID))) {
            if ((onWhat == onChange) && elDef->WorkFlags.Contains(skipOnCopy))
              execOnWhat = onMove;
            else
              execOnWhat = onWhat;
            ((SynObjectBase*)inEl->Exec.ptr)->MakeTable((address)this, nINCL, (SynObjectBase*)inEl, execOnWhat, 0, 0, (address)&sData);
          }
          if (onWhat == onCopy)
            elDef->WorkFlags.EXCL(skipOnCopy);
        }
      }
      inCheEl = (CHE*)inCheEl->successor;
    }
    elDef->WorkFlags.EXCL(skipOnDeleteID);
  }
}// Down



bool TIDTable::GetParamRefID(LavaDECL* decl, TID& ElID, SecondTFlag flag)
{
  //get the set element type
  if (!decl)
    return true;
  CHE* che;
  for (che = (CHE*)decl->NestedDecls.first;
       che && (((LavaDECL*)che->data)->DeclType == VirtualType);
       che = (CHE*)che->successor) {
    if (((LavaDECL*)che->data)->SecondTFlags.Contains(flag)) {
      ElID = ((LavaDECL*)che->data)->RefID;
      ElID.nINCL = IDTab[decl->inINCL]->nINCLTrans[ElID.nINCL].nINCL;
      return true;
    }
  }
  CHETID* cheID;
  LavaDECL* patternDECL;
  for (cheID = (CHETID*)decl->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    patternDECL = GetDECL(cheID->data, decl->inINCL);
    if (patternDECL->SecondTFlags.Contains(flag) )
      return GetParamRefID(patternDECL, ElID, flag);
  }
  return false;
}

bool TIDTable::GetParamID(LavaDECL* decl, TID& ElID, SecondTFlag flag)
{
  //get the setelem virtual type
  CHE *che;
  CHETID *cheID;
  LavaDECL* patternDECL;
  if (!decl)
    return false;
  if (decl->DeclType == VirtualType)
    decl = GetDECL(decl->RefID, decl->inINCL);
  if (!decl)
    return false;
  for (che = (CHE*)decl->NestedDecls.first;
       che && (((LavaDECL*)che->data)->DeclType == VirtualType);
       che = (CHE*)che->successor ) {
    if (((LavaDECL*)che->data)->SecondTFlags.Contains(flag)) {
      ElID = TID(((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL);
      return true;
    }
  }
  for (cheID = (CHETID*)decl->Supports.first; cheID; cheID = (CHETID*)cheID->successor) {
    patternDECL = GetDECL(cheID->data, decl->inINCL);
    if (patternDECL && patternDECL->SecondTFlags.Contains(flag) )
      return GetParamID(patternDECL, ElID, flag);
  }
  return false;
}

void TIDTable::GetPattern(LavaDECL* decl, CContext& context, bool start)
{
  LavaDECL* pdecl;
  context.iContext = 0;
  context.oContext = 0;
  bool jump = true;
  if (!decl)
    return;
  CContext con;
//  if (decl->WorkFlags.Contains(isPattern))
  if (decl && decl->NestedDecls.first
    && (((LavaDECL*)((CHE*)decl->NestedDecls.first)->data)->DeclType == VirtualType))
    context.oContext = decl;
  else {
    if (decl->DeclType == Impl)
      decl = GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
    if (decl && ((decl->DeclType == Interface) || (decl->DeclType == Package))) {
      jump = false;
      CHETID* cheID = (CHETID*)decl->Supports.first;//!
      while (cheID) {
        pdecl = GetFinalDef(cheID->data, decl->inINCL);
//        if (pdecl && pdecl->WorkFlags.Contains(isPattern)) {
        if (pdecl && pdecl->NestedDecls.first
          && (((LavaDECL*)((CHE*)pdecl->NestedDecls.first)->data)->DeclType == VirtualType)) {
          context.oContext = decl;
          cheID = 0;
        }
        else {
          GetPattern(pdecl, con, false);
          if (con.oContext) {
            context.oContext = decl;
            cheID = 0;
          }
          else
            cheID = (CHETID*)cheID->successor;
        }
      }
    }
  }
  if (!start || !decl)
    return;
  pdecl = decl->ParentDECL;
  while (pdecl) {
    if (jump && (pdecl->DeclType == Impl)) {
      pdecl = GetDECL(((CHETID*)pdecl->Supports.first)->data, decl->inINCL);//!
      GetPattern(pdecl, con, true);
    }
    else
      GetPattern(pdecl, con, false);
    jump = jump && (decl->DeclType != Interface) && (decl->DeclType != Package);
    if (con.oContext) {
      if (context.oContext)
        context.iContext = context.oContext;
      context.oContext = con.oContext;
      if (context.iContext)
        return;
      else
        if (con.iContext) {
          context.iContext = con.iContext;
          return;
        }
    }
    if (pdecl)
      pdecl = pdecl->ParentDECL;
  }
}


void TIDTable::GetContextDECLs(LavaDECL* decl, CContext& context, bool start)
{
  LavaDECL* pdecl;
  context.iContext = 0;
  context.oContext = 0;
  bool jump = true;
  if (!decl)
    return;
  CContext con;
//  if (decl->WorkFlags.Contains(isPattern))
  if (decl && decl->NestedDecls.first
    && (((LavaDECL*)((CHE*)decl->NestedDecls.first)->data)->DeclType == VirtualType))
    context.oContext = decl;
  else {
    if (decl->DeclType == Impl)
      decl = GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
    if (decl && (decl->DeclType == Interface) || (decl->DeclType == Package)) {
      jump = false;
      CHETID* cheID = (CHETID*)decl->Supports.first;//!
      while (cheID) {
        pdecl = GetFinalDef(cheID->data, decl->inINCL);
//        if (pdecl && pdecl->WorkFlags.Contains(isPattern)) {
        if (pdecl && pdecl->NestedDecls.first
          && (((LavaDECL*)((CHE*)pdecl->NestedDecls.first)->data)->DeclType == VirtualType)) {
          context.oContext = pdecl;
          cheID = 0;
        }
        else {
          GetContextDECLs(pdecl, con, false);
          if (con.oContext) {
            context.oContext = con.oContext;
            cheID = 0;
          }
          else
            cheID = (CHETID*)cheID->successor;
        }
      }
    }
  }
  if (!start || !decl)
    return;
  pdecl = decl->ParentDECL;
  while (pdecl) {
    if (jump && (pdecl->DeclType == Impl)) {
      pdecl = GetDECL(((CHETID*)pdecl->Supports.first)->data, decl->inINCL);//!
      GetContextDECLs(pdecl, con, true);
    }
    else
      GetContextDECLs(pdecl, con, false);
    jump = jump && (decl->DeclType != Interface) && (decl->DeclType != Package);
    if (con.oContext) {
      if (context.oContext)
        context.iContext = context.oContext;
      context.oContext = con.oContext;
      if (context.iContext)
        return;
      else
        if (con.iContext) {
          context.iContext = con.iContext;
          return;
        }
    }
    if (pdecl)
      pdecl = pdecl->ParentDECL;
  }
}

bool TIDTable::Overrides(const TID& upId, int upinINCL, const TID& id, int inINCL, LavaDECL* conDECL)
{
  //only usable for functions at the same position in the function table
  LavaDECL* funcDECL1 = GetDECL(upId, upinINCL);
  LavaDECL* funcDECL2 = GetDECL(id, inINCL);
  if (!funcDECL1 || !funcDECL2)
    return false;
  if (funcDECL1 == funcDECL2)
    return true;
  return IsAnc(funcDECL1->ParentDECL, TID(funcDECL2->ParentDECL->OwnID, funcDECL2->inINCL), 0, conDECL, true);

}

bool TIDTable::IsAn(LavaDECL *decl, const TID& id, int inINCL )
{
  if (decl == GetDECL(id, inINCL))
    return true;
  else
    return IsAnc(decl, id, inINCL);
}

bool TIDTable::IsAn(const TID& upId, int upinINCL, const TID& id, int inINCL )
{
  if (EQEQ(upId, upinINCL, id, inINCL))
    return true;
  else
    return IsAnc(GetDECL(upId, upinINCL), id, inINCL);
}

bool TIDTable::IsAnc(const TID& upId, int upinINCL, const TID& id, int inINCL, LavaDECL* conDECL, bool isI )
{
  return IsAnc(GetDECL(upId, upinINCL), id, inINCL, conDECL, isI );
}

bool TIDTable::IsAnc(LavaDECL *decl, const TID& id, int inINCL, LavaDECL* conDECL, bool isI, bool cheStart)
{
  CHETID* cheID;
  LavaDECL *baseDecl, *baseDecl0;
  CHE *che;

  if (!decl)
    return false;
  if (cheStart) {
    isAncChain.DestroyKeepElems();
    cheStart = false;
  }
  else {
    for (che = (CHE*)isAncChain.first; che && (che->data != decl); che = (CHE*)che->successor);
    if (che)
      return false;
  }
  che = NewCHE(decl);
  isAncChain.Append(che);
  if (!conDECL) {
    isI = decl->DeclType == Interface;
    if (isI) {
      conDECL = decl;
      baseDecl0 = GetFinalBasicType(id, inINCL, conDECL);
    }
    else
      baseDecl0 = GetDECL(id, inINCL);
  }
  else
    baseDecl0 = GetDECL(id, inINCL);
  if (decl == baseDecl0)
    return true;
  if (!baseDecl0)
    return false;

  for (cheID = (CHETID*)decl->Supports.first;
       cheID && (isI && (GetFinalBasicType(cheID->data, decl->inINCL, conDECL) != baseDecl0)
                || !isI && (GetDECL(cheID->data, decl->inINCL) != baseDecl0));
       cheID = (CHETID*)cheID->successor);
  if (cheID)
    return true;
  for (cheID = (CHETID*)decl->Supports.first;
       cheID ;
       cheID = (CHETID*)cheID->successor) {
    if (isI) {
      baseDecl = GetFinalBasicType(cheID->data, decl->inINCL, conDECL);
      for (che = (CHE*)isAncChain.first; che && (che->data != baseDecl); che = (CHE*)che->successor);
      if (che) {
        //cheID->data.nID = -cheID->data.nID;
        //return false;
        return true;
      }
      if (baseDecl && IsAnc(baseDecl, TID(baseDecl0->OwnID, baseDecl0->inINCL), 0, conDECL, isI, cheStart))
        return true;
    }
    else {
      baseDecl = GetDECL(cheID->data, decl->inINCL);
      for (che = (CHE*)isAncChain.first; che && (che->data != baseDecl); che = (CHE*)che->successor);
      if (che) {
        //cheID->data.nID = -cheID->data.nID;
        //return false;
        return true;
      }
      if (IsAnc(baseDecl, id, inINCL, conDECL, isI, cheStart))
        return true;
    }
  }
  return false;
}


bool TIDTable::HasVBase(LavaDECL *decl, const TID& vid, int inINCL)
{ //virtual vid in decl as virtual basic type
  if (!decl)
    return false;

  CHETID* cheID;
  LavaDECL *baseDecl;

  for (cheID = (CHETID*)decl->Supports.first;
       cheID;
       cheID = (CHETID*)cheID->successor) {
    baseDecl = GetDECL(cheID->data, decl->inINCL);
    if (baseDecl && (baseDecl->DeclType == VirtualType)
         && (IsAn(baseDecl, vid, inINCL) || IsAn(vid, inINCL, cheID->data, decl->inINCL)))
      return true;
    if (HasVBase(baseDecl, vid, inINCL))
      return true;
  }
  return false;
}


LavaDECL* TIDTable::GetFinalDef(const TID& id, int inINCL)
{
  //returns the final type if virtual (not mapped)
  LavaDECL *startDecl = GetDECL(id, inINCL);
  LavaDECL *decl = startDecl;
  while (decl   &&  (decl->DeclType == VirtualType)) {
    decl = GetDECL(decl->RefID.nINCL, decl->RefID.nID, decl->inINCL);
    if (decl == startDecl)
      return 0;
  }
  return decl;
}


LavaDECL* TIDTable::GetFinalBasicType(const TID& id, int inINCL, LavaDECL* conDECL)
{
  LavaDECL *decl = GetDECL(id, inINCL);
  CHE *che;
  if (!decl || (decl->DeclType != VirtualType))
    return decl;
  CContext con;
  GetContextDECLs(conDECL, con);
  if (con.iContext) {
    for (che = (CHE*)con.iContext->NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType == VirtualType)
             && !IsAn(id, inINCL, TID(((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL), 0);
         che = (CHE*)che->successor);
    if (che && (((LavaDECL*)che->data)->DeclType == VirtualType))
      return GetDECL(((LavaDECL*)che->data)->RefID, ((LavaDECL*)che->data)->inINCL);
  }
  if (con.oContext) {
    for (che = (CHE*)con.oContext->NestedDecls.first;
         che && (((LavaDECL*)che->data)->DeclType == VirtualType)
             && !IsAn((LavaDECL*)che->data, id, inINCL);
         che = (CHE*)che->successor);
    if (che && (((LavaDECL*)che->data)->DeclType == VirtualType))
      return GetDECL(((LavaDECL*)che->data)->RefID, ((LavaDECL*)che->data)->inINCL);
  }
  return 0;
}

bool TIDTable::Overrides(const TID& upId, int upinINCL, const TID& id, int inINCL)
{
  // overrides upId (id of a function or member or virtual type) the element with TID id
  // in the virtual table?
  LavaDECL* decl;
  for (decl = GetDECL(id, inINCL);
       decl && decl->Supports.first;
       decl = GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL));
  if (decl && IsAnc(upId, upinINCL, TID(decl->OwnID, decl->inINCL), 0))
    return true;
  return false;
}


bool TIDTable::Overrides(LavaDECL* decl1, LavaDECL* decl2)
{
  // overrides decl1 (function or member or virtual type) the element decl2
  // in the virtual table?
  LavaDECL* decl;
  if (!decl1)
    return false;
  for (decl = decl2;
       decl && decl->Supports.first;
       decl = GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL));
  if (decl && IsAnc(decl1, TID(decl->OwnID, decl->inINCL), 0))
    return true;
  return false;
}

CHETVElem* TIDTable::FindElemInVT(LavaDECL* myClassDecl, LavaDECL* yourClassDecl, LavaDECL* yourEl, int yourINCL)
{ //elDecl ist aus dem durch yourINCL definierten Dokument
  CHETVElem *el1, *el2;
  for (el1 = (CHETVElem*)yourClassDecl->VElems.VElems.first;
       el1 && (el1->data.VTEl.nID != yourEl->OwnID) || (el1->data.VTEl.nINCL != yourEl->inINCL);
       el1 = (CHETVElem*)el1->successor);
  for (el2 = (CHETVElem*)myClassDecl->VElems.VElems.first;
       el2 && !EQEQ(el2->data.VTBaseEl, 0, el1->data.VTBaseEl, yourINCL);
       el2 = (CHETVElem*)el2->successor);
  return el2;
}

CHETVElem* TIDTable::FindSamePosInVT(CHETVElem *ElFind, CHETVElem *ElTable, CHETVElem *ElLast)
{
  TID id, idEl;
  CHETID *cheID, *cheIDEl;
  CHETVElem *El = 0;
  if (!ElFind || !ElTable)
    return 0;
  LavaDECL *declEl, *decl = GetDECL(ElFind->data.VTEl, 0);
  if (decl) {
    cheID = (CHETID*)decl->Supports.first;
    while (decl && cheID) {
      decl = GetDECL(cheID->data, decl->inINCL);
      if (decl)
        cheID = (CHETID*)decl->Supports.first;
    }
    if (decl)
      id = TID(decl->OwnID, decl->inINCL);
    El = ElTable;
    declEl = GetDECL(El->data.VTEl, 0);
    if (declEl)
      idEl = TID(declEl->OwnID, declEl->inINCL);
    while (El && (id != idEl)) {
      declEl = GetDECL(El->data.VTEl, 0);
      if (declEl)
        cheIDEl = (CHETID*)declEl->Supports.first;
      while (declEl && cheIDEl) {
        declEl = GetDECL(cheIDEl->data, declEl->inINCL);
        if (declEl)
          cheIDEl = (CHETID*)declEl->Supports.first;
      }
      if (declEl)
        idEl = TID(declEl->OwnID, declEl->inINCL);
      if ((id != idEl)) {
        if (El == ElLast)
          El = 0;
        else
          El = (CHETVElem*)El->successor;
      }
    }
  }
  return El;
}

bool TIDTable::okPatternLevel(LavaDECL* paramDECL)
{
  CContext con;
  GetPattern(paramDECL, con);
  if (con.iContext)
    GetPattern(con.oContext, con);
  return con.iContext == 0;
}


bool TIDTable::lowerOContext(LavaDECL* highDECL, LavaDECL *lowDECL, bool& sameContext)
{  //check this: if a base type bt of an interface intf is polymorph,
   //            then intf must be polymorph and the pattern of bt is a base pattern
   //                   of the pattern of intf
   //            is lowDECL in a base pattern (or the same) of the pattern in which highDECL lies?
  LavaDECL *lowpat, *highpat;
  CContext lCon, hCon;
  if (!highDECL || !lowDECL)
    return true;
  sameContext = false;
  if (lowDECL->DeclType == Impl)
    lowpat = GetDECL(((CHETID*)lowDECL->Supports.first)->data, lowDECL->inINCL);//!
  else
    lowpat = lowDECL;
  GetPattern(lowpat, lCon);
  if (!lCon.oContext)
    return true;
  if (highDECL->DeclType == Impl)
    highpat = GetDECL(((CHETID*)highDECL->Supports.first)->data, highDECL->inINCL);//!
  else
    highpat = highDECL;
  GetPattern(highpat, hCon);
  if (!hCon.oContext)
    return false;
  if (hCon.oContext == lCon.oContext) {
    sameContext = true;
    return true;
  }
  else
    return IsAn(hCon.oContext,TID(lCon.oContext->OwnID, lCon.oContext->inINCL),0);
}

QString* TIDTable::CheckValOfVirtual(LavaDECL* VTDecl, bool cor)
{
  LavaDECL *decl, *valDECL;
  bool ok = true;
  CHETID *che, *cheOver;

  if (!VTDecl)
    return 0;
  valDECL = GetDECL(VTDecl->RefID, VTDecl->inINCL);
  if (!valDECL)
    return &ERR_InvalidValOfVT;
  if (valDECL->DeclType == VirtualType)
    return 0;
  for (che = (CHETID*)VTDecl->Supports.first;
       che;
       che = (CHETID*)che->successor) {
    decl = GetDECL(che->data, VTDecl->inINCL);
    if (decl && decl->TypeFlags.Contains(isFinalVT)) 
      return &ERR_FinalVTisOverridden;
  }
  decl = valDECL;
  for (; decl && (decl != VTDecl->ParentDECL);
         decl = decl->ParentDECL);
  if (!decl)
    return 0;
  for (che = (CHETID*)VTDecl->Supports.first;
       che && ok;
       che = (CHETID*)che->successor) {
    decl = GetDECL(che->data, VTDecl->inINCL);
    if (decl) {
      if (decl->TypeFlags.Contains(isAbstract))
        ok = true;
      else {
        ok = IsAn(VTDecl->RefID, VTDecl->inINCL, decl->RefID, decl->inINCL);
        if (!ok && cor) {
          cheOver = new CHETID;
          cheOver->data = decl->RefID; //they have the same inINCL
          valDECL->Supports.Append(cheOver);
          ok = true;
        }
      }
    }
    else
      ok = false;
  }
  if (ok)
    return 0;
  else
    return &ERR_InvalidValOfVT;
}

bool TIDTable::otherOContext(LavaDECL* decl1, LavaDECL *decl2)
{
  CContext con1, con2;
  if (!decl1 || !decl2)
    return true;
  GetPattern(decl1, con1);
  GetPattern(decl2, con2);
  if (!con1.oContext || !con2.oContext)
    return true;
  if (con1.iContext && !con2.iContext) {
    if (IsAn(con1.iContext,TID(con2.oContext->OwnID, con2.oContext->inINCL),0))
      return false;
  }
  else
    if (!con1.iContext && con2.iContext)
      if (IsAn(con1.oContext,TID(con2.iContext->OwnID, con2.iContext->inINCL),0))
        return false;

  return !IsAn(con1.oContext,TID(con2.oContext->OwnID, con2.oContext->inINCL),0)
         && !IsAn(con2.oContext,TID(con1.oContext->OwnID, con1.oContext->inINCL),0);
}

bool TIDTable::virtualSelf(LavaDECL *decl, LavaDECL*& paramDECL)
{
  LavaDECL *pat;
  paramDECL = 0;
  if (!decl)
    return false;
  if (decl->DeclType == Impl)
    decl = GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);//!
  if (decl && (decl->DeclType == Interface)) {
    pat = decl;
    while (pat) {
      if (pat->WorkFlags.Contains(isPattern)) {
        if (FindParamOfVal(decl, pat, paramDECL))
          return true;
      }
      if (pat->WorkFlags.Contains(isPartOfPattern))
        pat = pat->ParentDECL;
      else
        pat = 0;
    }
  }
  paramDECL = decl;
  return false;
}

bool TIDTable::FindParamOfVal(LavaDECL *decl, LavaDECL *pat, LavaDECL*& paramDECL)
{
  CHE *che;
  LavaDECL *valDECL, *pDECL = paramDECL;
  if (!pat)
    return false;
  if (!pDECL) {
    che = (CHE*)pat->NestedDecls.first;
    while (che) {
      paramDECL = (LavaDECL*) che->data;
      if (paramDECL->DeclType == VirtualType) {
        valDECL = GetDECL(paramDECL->RefID,paramDECL->inINCL);
        if (valDECL
            && (valDECL->DeclType == Interface)
            && valDECL->isInSubTree(pat)
            && EQEQ(TID(decl->OwnID,0), decl->inINCL, paramDECL->RefID,paramDECL->inINCL))
          return true;
        che = (CHE*)che->successor;
      }
      else
        che = 0;
    }
  }
  che = (CHE*)pat->NestedDecls.first;
  while (che) {
    paramDECL = (LavaDECL*) che->data;
    if (paramDECL->DeclType == VirtualType) {
      valDECL = GetDECL(paramDECL->RefID,paramDECL->inINCL);
      if ((pDECL != paramDECL)
          && valDECL
          && (valDECL->DeclType == Interface)
          && valDECL->isInSubTree(pat)
          && IsAn(decl,paramDECL->RefID,paramDECL->inINCL))
        return true;
      che = (CHE*)che->successor;
    }
    else
      che = 0;
  }
  paramDECL = 0;
  return false;
}

QString* TIDTable::CleanSupports(LavaDECL *decl, LavaDECL* contDECL)
{
  QString *ret1 = 0;
  int ret2;
  CHAINANY chain;
  CHETID *che;
  LavaDECL* basedecl;
  if (!decl)
    return 0;
  chain.first = decl->Supports.first;
  chain.last = decl->Supports.last;
  decl->Supports.first = 0;
  decl->Supports.last = 0;
  for (che = (CHETID*)chain.first; che; che = (CHETID*)che->successor) {
    basedecl = GetDECL(che->data);
    if (basedecl) {
      ret2 = InsertBaseClass(decl, basedecl, contDECL, true);
      if (!ret1 && (!ret2 || (ret2 == 2)))
        ret1 = &ERR_CleanSupports;
    }
    else
      ret1 = &ERR_NoBaseIF;
  }
  return ret1;
}

int TIDTable::InsertBaseClass(LavaDECL *decl, LavaDECL* newbasedecl, LavaDECL* contDECL, bool putBase)
{
  //returns 0: type already contained return -1: second container class, 1 : type realy new, 2 : overrides an already contained type
  LavaDECL* findecl, *finalnewBasedecl, *bdecl;
  CHETID *che;
  bool multiContainer;

  if (!decl || !newbasedecl || !contDECL)
    return 0;
  if ((contDECL->OwnID == decl->OwnID)
      && ((contDECL == newbasedecl)
        || IsAnc(newbasedecl, TID(contDECL->OwnID, contDECL->inINCL),0,contDECL,true)))
    return 0;
  TID newbaseID = TID(newbasedecl->OwnID, newbasedecl->inINCL);
  if (newbasedecl->DeclType == VirtualType)
    finalnewBasedecl = GetFinalBasicType(newbaseID, decl->inINCL, contDECL);
  else
    finalnewBasedecl = newbasedecl;
  if (!finalnewBasedecl)
    return 0;
  multiContainer = decl->Supports.first && (decl->SecondTFlags.Contains(isSet) || decl->SecondTFlags.Contains(isChain) || decl->SecondTFlags.Contains(isArray))
        && (finalnewBasedecl->SecondTFlags.Contains(isSet) || finalnewBasedecl->SecondTFlags.Contains(isChain) || finalnewBasedecl->SecondTFlags.Contains(isArray));
  TID finalBaseID = TID(finalnewBasedecl->OwnID, finalnewBasedecl->inINCL);
  che = (CHETID*)decl->Supports.first;
  while (che) {
    bdecl = GetDECL(che->data, decl->inINCL);
    if (bdecl) {
      if (bdecl == newbasedecl)
        return 0;
      if (bdecl->DeclType == VirtualType) {
        findecl = GetFinalBasicType(che->data, decl->inINCL, contDECL);
        if (finalnewBasedecl == findecl)
          return 0;
        if (newbasedecl->DeclType != VirtualType)
          if (IsAnc(findecl, finalBaseID, decl->inINCL, contDECL, true))
            return 0;
          else
            if (HasVBase(finalnewBasedecl, che->data, 0)) {
              if (putBase)
                che->data = newbaseID;
              return 2;
            }
      }
      else {
        if (newbasedecl->DeclType == VirtualType) {
          //hat bdecl eine virtuelle basis die im context von contDECL == newbasedecl wird?
          if (HasVBase(bdecl, newbaseID, 0))
            return 0;
        }
        else {
          if (IsAnc(bdecl, finalBaseID, 0, contDECL, true))
            return 0;
        }
        if ((finalnewBasedecl == bdecl)
            || IsAnc(finalnewBasedecl, che->data, decl->inINCL, contDECL, true)) {
          if (putBase) {
            che->data = newbaseID;
            multiContainer = false;
          }
          return 2;
        }
      }
    }
    che = (CHETID*)che->successor;
  }
  if (multiContainer)
    return -1;
  if (putBase) {
    che = new CHETID;
    che->data = newbaseID;
    decl->Supports.Append(che);
  }
  return 1;
}


bool TIDTable::isValOfVirtual(LavaDECL *decl, LavaDECL* baseDECL)
//decl is value of a virtual type in the outer or own context of decl
//and if indicated, baseDECL is value of a base of this virtual type
{
  LavaDECL *param = 0, *bparam = 0;
  CContext con, bcon;
  if (!decl || (decl->DeclType != Interface))
    return false;
  GetPattern(decl, con);
  if (con.iContext) {
    if (!FindParamOfVal(decl, con.iContext, param))
      if (!FindParamOfVal(decl, con.oContext, param))
        return false;
  }
  else
    if (con.oContext)
      if (!FindParamOfVal(decl, con.oContext, param))
        return false;
  if (baseDECL) {
    GetPattern(baseDECL, bcon);
    if (bcon.iContext) {
      if (!FindParamOfVal(baseDECL, bcon.iContext, bparam))
        if (!FindParamOfVal(baseDECL, bcon.oContext, bparam))
          return false;
    }
    else
      if (bcon.oContext)
        if (!FindParamOfVal(baseDECL, bcon.oContext, bparam))
          return false;
    if (IsAn(param,TID(bparam->OwnID, bparam->inINCL),0))
      return true;
    else
      return false;
  }
  else
    return param != 0;
}

bool TIDTable::isValOfOtherVirtual(LavaDECL *decl, LavaDECL* vdecl)
//decl is value of a virtual type in the outer or own context of decl
{
  LavaDECL *param = vdecl;
  CContext con, bcon;
  if (!decl || (decl->DeclType != Interface))
    return false;
  GetPattern(decl, con);
  if (con.iContext) {
    if (FindParamOfVal(decl, con.iContext, param))
      return true;
    else {
      param = vdecl;
      if (FindParamOfVal(decl, con.oContext, param))
        return true;
    }
  }
  else
    if (con.oContext)
      if (FindParamOfVal(decl, con.oContext, param))
        return true;

  return false;
}

DString dach;
DString pkt;
DString kla;
DString klz;
DString komma;
DString ddppkt;
DString lthen;
DString grthen;
DString pktbsl;
DString pktpktbsl;
DString pktsl;
DString pktpktsl;
QString StdLava;
QString StdLavaLog;
QString ComponentLinkDir;
QString ExeDir;

void InitGlobalStrings()
{
  ComponentLinkDir = QString("/Components/");
  dach = DString("^");
  pkt =  DString(".");
  kla =  DString("(");
  klz =  DString(")");
  komma = DString(",");
  ddppkt = DString("::");
  lthen = DString("<");
  grthen = DString(">");
  pktbsl = DString(".\\");
  pktsl = DString("./");
  pktpktbsl = DString("..\\");
  pktpktsl = DString("../");

}
