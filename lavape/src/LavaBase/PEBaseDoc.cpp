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
#include "MACROS.h"

#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "LavaBaseDoc.h"
#include "SynIO.h"
#include "Convert.h"
#include "SylTraversal.h"
#include "docview.h"
#include "mdiframes.h"

#include "qmessagebox.h"
#include "q3cstring.h"
#include "qobject.h"
#include "qstring.h"
#include "qlineedit.h"
#include "qfileinfo.h"
#include "qdir.h"
#include "UNIX.h"

#include <QComboBox>


//static HRESULT CreateShortCut (LPCSTR pszShortcutFile, LPSTR pszLink);

/////////////////////////////////////////////////////////////////////////////
// CPEBaseDoc
/*
IMPLEMENT_DYNCREATE(CPEBaseDoc, CLavaBaseDoc)

BEGIN_MESSAGE_MAP(CPEBaseDoc, CLavaBaseDoc)
  //{{AFX_MSG_MAP(CPEBaseDoc)
  ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/
/////////////////////////////////////////////////////////////////////////////
// CPEBaseDoc construction/destruction

CPEBaseDoc::CPEBaseDoc()
{
  // TODO: add one-time construction code here
  //itDoc = 0;
  modified=false;
  MainView = 0;
  hasIncludes = false;
  asked = false;
  UpdateNo = 1;
  Redraw = false;
  changeNothing = false;
}

CPEBaseDoc::~CPEBaseDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPEBaseDoc commands




/////////////////////////////////////////////////////////////////////////////
// CPEBaseDoc serialization
void CPEBaseDoc::Serialize(QDataStream& ar)
{
/*
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    //OnOpenDocument(ar.m_strFileName);
  // TODO: add loading code here
  }
*/
}

int CPEBaseDoc::ReadSynDef(const QString& fn, SynDef* &sntx, ASN1*)
{
  //is the sntx open?
  CHESimpleSyntax *cheSyn;
  int fmode = 0;
  QFileInfo fi;
  QString qfn;
  DString docDir = fn.latin1(), stdName;

  CPEBaseDoc* doc = (CPEBaseDoc*)FindOpenDoc(fn);
  if (doc) {
    sntx = new SynDef;
    *sntx = *doc->mySynDef;
    sntx->IDTable = 0;
    for (cheSyn = (CHESimpleSyntax*)sntx->SynDefTree.first->successor; cheSyn; cheSyn = (CHESimpleSyntax*) cheSyn->successor)
      cheSyn->data.TopDef.Destroy();
  }
  else {
    fmode = SynIO.ReadSynDef(fn, sntx);
    if ((fmode < 0) || !sntx)
      return -1;
  }

  CalcDirName(docDir);
  QDir dir(docDir.c);
  cheSyn = (CHESimpleSyntax*)sntx->SynDefTree.first;
  if (!cheSyn->data.UsersName.l)
    cheSyn->data.UsersName = DString(fn.latin1());
  RelPathName(cheSyn->data.UsersName, IDTable.DocDir); //final value
  cheSyn->data.SyntaxName = DString(fn.latin1());
  RelPathName(cheSyn->data.SyntaxName, docDir);
  cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  while (cheSyn) {
    cheSyn->data.SyntaxName = cheSyn->data.UsersName;
    fi.setFile(dir, cheSyn->data.SyntaxName.c);
    qfn = ResolveLinks(fi);
    cheSyn->data.SyntaxName = DString(qPrintable(qfn));
    RelPathName(cheSyn->data.SyntaxName, docDir);
    cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  }
  if (SameFile(fn.latin1(), StdLava.ascii()))
    ((CHESimpleSyntax*)sntx->SynDefTree.first)->data.SyntaxName = DString("std.lava");
  else {
    if (sntx->SynDefTree.first->successor) {
      stdName = DString(qPrintable(StdLavaLog));
      RelPathName(stdName, docDir);
      ((CHESimpleSyntax*)sntx->SynDefTree.first->successor)->data.SyntaxName = stdName;
      ((CHESimpleSyntax*)sntx->SynDefTree.first->successor)->data.UsersName = stdName;
    }
  }
  return fmode;
}


CLavaBaseData* CPEBaseDoc::GetLBaseData()
{
  return LBaseData;
}

void CPEBaseDoc::IncludeHint(const QString& fullfn, CHESimpleSyntax* cheSyn)
{
  QString* pfn = new QString(fullfn);
  CLavaPEHint *newHint = new CLavaPEHint(CPECommand_Include, this, (const unsigned long)0, (DWORD) cheSyn, (DWORD)pfn, (DWORD)cheSyn->predecessor);
  UndoMem.AddToMem(newHint);
}

void CPEBaseDoc::DelSyntax(CHESimpleSyntax *delSyn)
{
  IDTable.DeleteINCL(delSyn->data.nINCL);
  delSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.Uncouple(delSyn);
//    delete cheSyn;
}

void CPEBaseDoc::UndoDelSyntax(CLavaPEHint* hint)
{
  mySynDef->SynDefTree.Insert((ChainAnyElem*)hint->CommandData3, (ChainAnyElem*)hint->CommandData1);
  IDTable.UndoDeleteINCL(((CHESimpleSyntax*)hint->CommandData1)->data.nINCL);
}

void  CPEBaseDoc::MakeBasicBox(QComboBox* cbox, TDeclType defType, bool with, bool skipServices)
{ // defType == NoDef means all basics
  LavaDECL* decl;
  int incl;
  CComboBoxItem *listItem;
  if (isStd)
    incl = 0;
  else
    incl = 1;
  for ( int it = 1; it < LBaseData->maxb; it++) {
    decl = IDTable.GetDECL(incl, IDTable.BasicTypesID[it]);
    //es soll abstürzen, wenn decl = 0
    if (with  || (it != B_Object) 
        && ((defType == NoDef) || (decl->DeclType == defType))
        && (!skipServices || (decl->DeclType != Interface))) {
      listItem = new CComboBoxItem(/*LBaseData->BasicNames[it],*/ TID(IDTable.BasicTypesID[it], incl));
      cbox->addItem(QString(LBaseData->BasicNames[it]),QVariant::fromValue(listItem));
    }
  }
  SortCombo(cbox);
  cbox->setCurrentItem(0);
}


bool CPEBaseDoc::Step(CLavaPEHint* hint, LavaDECL* parDECL, CHE*& relElem)
{
  CHE *cheElLast = 0;
  int ipos;
  
  DString name;
  LavaDECL* hintDECL = (LavaDECL*)hint->CommandData1;
  if ((parDECL->DeclDescType != StructDesc) && (parDECL->DeclDescType != EnumType))
    return false;
  relElem = (CHE*)parDECL->NestedDecls.first;
  
  if (hintDECL->DeclDescType != ExecDesc) {
    for (cheElLast = relElem;
         cheElLast && (((LavaDECL*)cheElLast->data)->DeclType == VirtualType);
         cheElLast = (CHE*)cheElLast->successor);
    if (hintDECL->DeclType != VirtualType) {
      relElem = cheElLast;
      for ( ; cheElLast && (((LavaDECL*)cheElLast->data)->DeclType == IAttr);
              cheElLast = (CHE*)cheElLast->successor);
      if (hintDECL->DeclType != IAttr) {
        relElem = cheElLast;
        for ( ; cheElLast && (((LavaDECL*)cheElLast->data)->DeclType == OAttr);
                cheElLast = (CHE*)cheElLast->successor);
        if (hintDECL->DeclType != OAttr) {
          relElem = cheElLast;
          for ( ; cheElLast && (((LavaDECL*)cheElLast->data)->DeclType != Attr)
                           && (((LavaDECL*)cheElLast->data)->DeclType != Function)
                           && (((LavaDECL*)cheElLast->data)->DeclType != FormText)
                           && (((LavaDECL*)relElem->data)->DeclDescType != ExecDesc);
                  cheElLast = (CHE*)cheElLast->successor);
          if ((hintDECL->DeclType == Attr) || (hintDECL->DeclType == Function) || (hintDECL->DeclType == FormText)) {
            relElem = cheElLast;
            for ( ; cheElLast && (((LavaDECL*)relElem->data)->DeclDescType != ExecDesc);
                    cheElLast = (CHE*)cheElLast->successor);
          }
        }
      }
    }
  }
  else 
    for ( ; relElem && (((LavaDECL*)relElem->data)->DeclDescType != ExecDesc);
            relElem = (CHE*)relElem->successor);
  ipos = (int)hint->CommandData3-1;

  while (relElem && ipos && (relElem != cheElLast)) {
    relElem = (CHE*)relElem->successor;
    ipos--;
  }
  if (!relElem) {
    hint->CommandData3 = (int)hint->CommandData3 - ipos;
    if (cheElLast) 
      relElem = (CHE*)cheElLast->predecessor;
  }
  return true;
}//Step



CLavaPEHint* CPEBaseDoc::InsDelDECL(CLavaPEHint* hint, bool undo, bool redo, bool& localMove)
{
  CLavaPEHint  *newHint, *viewHint = hint;
  CHE *cheElDef, *relElem, *cheHint, *elRemoved, *cheExec, *cheNext;
  DString *str, *str2 = 0, absName;
  LavaDECL *hintDECL, *parEl = 0, **pNewDecl, *elDECL, *colDECL, *oldDECL, *decl4;
  TDeclType lastDefType = NoDef;
  int pos;
  TID id;
  SynFlags firstlast;
  CPECommand com;
  localMove = false;
  int d8=-1;

  if (undo) {
    if (hint->com == CPECommand_Insert)
      com = CPECommand_Delete;
    else
      com = CPECommand_Insert;
    if (hint->CommandData2)
      str2 = new DString(*(DString*)hint->CommandData2);
    viewHint = new CLavaPEHint(com, this, hint->FirstLast, hint->CommandData1, (DWORD)str2, hint->CommandData3,0,0,0,0,hint->CommandData8);
  }

  if (hint->CommandData4) 
    parEl = *(LavaDECL**)hint->CommandData4;
  hintDECL = (LavaDECL*)hint->CommandData1;
  if ((hintDECL->DeclType == DragFeature) 
     || (hintDECL->DeclType == DragFeatureF) 
     || (hintDECL->DeclType == DragParam) 
     || (hintDECL->DeclType == DragFText) 
     || (hintDECL->DeclType == DragIO))
    hintDECL->DeclType = DragDef;
  if ((viewHint->com == CPECommand_Insert) || (viewHint->com == CPECommand_Move)) {
    if ((hintDECL->DeclType == PatternDef) || (hintDECL->DeclType == DragDef))  {
      //this contains a decl collection for consistent drag/drop or copy or override
      pNewDecl = &hintDECL;
      if (viewHint->CommandData7) {
        absName = IDTable.IDTab[0]->FileName;
        AbsPathName(absName, IDTable.DocDir);
        localMove = SameFile(absName, *((DString*)viewHint->CommandData7));
        //localMove = (*((DString*)viewHint->CommandData7) == absName); 
      }
      else
        localMove = false;
      if ((viewHint->com != CPECommand_Move) || viewHint->CommandData6 && !localMove) {
        if (viewHint->CommandData6) 
          IDTable.PrepareClipTree((SyntaxDefinition*) viewHint->CommandData6, (DString*)viewHint->CommandData7);
        IDTable.NewID(pNewDecl);
      }
      cheHint = (CHE*)hintDECL->NestedDecls.first;
      firstlast = hint->FirstLast;
      if (hintDECL->DeclType == PatternDef)
        pos = parEl->GetAppendPos(VirtualType);
      else
        pos = (int)hint->CommandData3;
      decl4 = *(LavaDECL**)hint->CommandData4;
      while (cheHint) {
        elDECL = (LavaDECL*)cheHint->data;
        if (hint->CommandData2)
          str2 = new DString(*(DString*)hint->CommandData2);
        if ((elDECL->DeclType == Attr)
          || (elDECL->DeclType == IAttr)
          || (elDECL->DeclType == OAttr)
          || (elDECL->DeclType == FormText)
          || (elDECL->DeclType == VirtualType))
          str = &pkt;
        else
          str = &ddppkt;
        if ((lastDefType == VirtualType) && (elDECL->DeclType != VirtualType))
          pos = parEl->GetAppendPos(elDECL->DeclType);
        if ((hintDECL->DeclType == PatternDef) && (lastDefType != elDECL->DeclType))
          if (lastDefType == VirtualType)
            pos = parEl->GetAppendPos(elDECL->DeclType);
          else {
            if (((lastDefType == Function) || (lastDefType == Attr))
                && (elDECL->DeclType != Function) && (elDECL->DeclType != Attr))
              pos = parEl->GetAppendPos(elDECL->DeclType);
            else
              if ((lastDefType != Function) && (lastDefType != Attr)
                  && ((elDECL->DeclType == Function) || (elDECL->DeclType == Attr)))
                pos = parEl->GetAppendPos(elDECL->DeclType);
              else
                if ((lastDefType != IAttr) && (lastDefType != OAttr)
                    && ((elDECL->DeclType == IAttr) || (elDECL->DeclType == OAttr)))
                  pos = parEl->GetAppendPos(elDECL->DeclType);
          }
        if (hintDECL->FullName.l)
          elDECL->FullName = hintDECL->FullName + *str + elDECL->LocalName;
        else
          elDECL->FullName = elDECL->LocalName;
        if ((elDECL->op != OP_noOp)
            && getOperatorID(decl4, elDECL->op, id)
            && (!elDECL->Supports.first || (((CHETID*)elDECL->Supports.first)->data != id))) {
          elDECL->LocalName = DString ("copy_of_operator_") + LBaseData->OpFuncNames[elDECL->op];
          elDECL->op = OP_noOp;
        }
        if ((elDECL->DeclType == Function) && (decl4->DeclType == Impl) && !elDECL->Supports.first) {
          elDECL->SecondTFlags.EXCL(overrides);
          elDECL->SecondTFlags.EXCL(funcImpl);
          elDECL->TypeFlags.EXCL(isProtected);
          elDECL->TypeFlags.EXCL(isAbstract);
          elDECL->TypeFlags.EXCL(forceOverride);
          elDECL->TypeFlags.EXCL(isNative);
          elDECL->TypeFlags.EXCL(defaultInitializer);
        }
        if (!hint->CommandData5 && (hintDECL->DeclType == DragDef)
          && !hint->FirstLast.Contains(multiDocHint)
          && !elDECL->WorkFlags.Contains(fromPrivToPub)) { //not drag-drop, no pattern: paste or only drop
          if (elDECL->SecondTFlags.Contains(funcImpl)) {
            elDECL->SecondTFlags.EXCL(funcImpl);
            elDECL->TypeFlags.EXCL(isProtected);
            elDECL->Supports.Destroy();
          }
          if (elDECL->SecondTFlags.Contains(overrides)) {
            elDECL->SecondTFlags.EXCL(overrides);
            elDECL->Supports.Destroy();
          }
          if (elDECL->TypeFlags.Contains(isPropGet)) {
            elDECL->TypeFlags.EXCL(isPropGet);
            elDECL->Supports.Destroy();
          }
          if (elDECL->TypeFlags.Contains(isPropSet)) {
            elDECL->TypeFlags.EXCL(isPropSet);
            elDECL->Supports.Destroy();
          }
          if ((elDECL->DeclType == Attr) && parEl->TypeFlags.Contains(isComponent))
            elDECL->TypeFlags.INCL(hasSetGet);
        }
        if (elDECL->WorkFlags.Contains(fromPrivToPub)) {
          elDECL->WorkFlags.EXCL(fromPrivToPub);
          if (elDECL->DeclType == Function) {
            for (cheElDef = (CHE*)elDECL->NestedDecls.first;
                 cheElDef && cheElDef->successor;
                 cheElDef = (CHE*)cheElDef->successor)
              ((LavaDECL*)cheElDef->data)->WorkFlags.EXCL(fromPrivToPub);
          }
        }
        newHint = new CLavaPEHint(CPECommand_Insert, this, firstlast, (DWORD)elDECL, 
                                  (DWORD)str2, (DWORD)pos, hint->CommandData4);
        newHint->CommandData9 = (DWORD)cheHint;
        lastDefType = elDECL->DeclType;
        cheNext = (CHE*)cheHint->successor;
        cheElDef = (CHE*)hintDECL->NestedDecls.Uncouple(cheHint);
//        if (firstlast.Contains(firstHint))
          ((LavaDECL*)cheElDef->data)->WorkFlags.INCL(selAfter);
//        else
//          ((LavaDECL*)cheElDef->data)->WorkFlags.EXCL(selAfter);
        firstlast.EXCL(firstHint); // = (const unsigned long)0;
        cheHint = cheNext;//(CHE*)cheHint->successor;
        Step(newHint, parEl, relElem);
        pos++;
        UndoMem.AddToMem(newHint);
        if (relElem)
          parEl->NestedDecls.Insert(relElem->predecessor, cheElDef);
        else
          parEl->NestedDecls.Insert(parEl->NestedDecls.last, cheElDef);
        ((LavaDECL*)cheElDef->data)->ParentDECL = parEl;
        if ((viewHint->com == CPECommand_Move) && (!viewHint->CommandData6 || localMove)) {
          //from local drag and drop?
          if (((LavaDECL*)cheElDef->data)->WorkFlags.Contains(skipOnCopy))
            IDTable.StartClipIDs(0, 0, &IDTable);

          IDTable.UndoDeleteID(elDECL->OwnID);
          IDTable.Change((LavaDECL**)&cheElDef->data);
        }
        else 
          if (elDECL->DeclType == Function) {
            cheExec = (CHE*)elDECL->NestedDecls.first;
            while(cheExec) {
              if (((LavaDECL*)cheExec->data)->DeclDescType == ExecDesc)
                ((SynObjectBase*)((LavaDECL*)cheExec->data)->Exec.ptr)->MakeTable((address)&IDTable, 0, (SynObjectBase*)cheExec->data, onTypeID);
              cheExec = (CHE*)cheExec->successor;
            }
          }
        if (!undo && !redo && !elDECL->SecondTFlags.Contains(funcImpl)
            && !elDECL->SecondTFlags.Contains(overrides)
            && (elDECL->DeclType != FormText)) {
          bool cop = false;
          colDECL = FindInSupports(elDECL->LocalName, elDECL, *(LavaDECL**)hint->CommandData4);
          if (colDECL && ((hint->com != CPECommand_Move) || !colDECL->inINCL))
            while (colDECL) {
              if (cop)
                elDECL->LocalName.Insert(DString("_"), 6);
              else {
                cop = true;
                elDECL->LocalName.Insert(DString("CopyOf_"), 0);
              }
              colDECL = FindInSupports(elDECL->LocalName, elDECL, *(LavaDECL**)hint->CommandData4);
            }
        }
        if ((elDECL->DeclType == VirtualType) || (elDECL->DeclType == Function)
           || (elDECL->DeclType == Attr))
          elDECL->ParentDECL->WorkFlags.INCL(recalcVT);
        if ((elDECL->DeclType == Interface) || (elDECL->DeclType == Package))
          elDECL->WorkFlags.INCL(recalcVT);
        if (hint->CommandData5 && (((LavaDECL*)hint->CommandData5)->DeclType == Interface))
          ((LavaDECL*)hint->CommandData5)->WorkFlags.INCL(recalcVT);
        UpdateAllViews(NULL, 0, newHint);
      }//while cheHint

      viewHint = 0;
    }
    else { //no collection
      newHint = hint;
      if (!undo && !redo) {
        cheElDef = NewCHE((LavaDECL*)newHint->CommandData1);
        newHint->CommandData9 = (DWORD)cheElDef;
      }
      else {
        cheElDef = (CHE*)newHint->CommandData9;
        if ((((LavaDECL*)cheElDef->data)->DeclType == Function)
          && ((LavaDECL*)cheElDef->data)->SecondTFlags.Contains(funcImpl)) {
          for (elRemoved = (CHE*)parEl->NestedDecls.first;
               elRemoved 
                 && (!((LavaDECL*)elRemoved->data)->Supports.first
                 || (((CHETID*)((LavaDECL*)elRemoved->data)->Supports.first)->data
                             != ((CHETID*)((LavaDECL*)cheElDef->data)->Supports.first)->data)
                 || (((LavaDECL*)cheElDef->data)->TypeFlags.Contains(isPropGet) 
                    != ((LavaDECL*)elRemoved->data)->TypeFlags.Contains(isPropGet) )
                 || (((LavaDECL*)cheElDef->data)->TypeFlags.Contains(isPropSet) 
                    != ((LavaDECL*)elRemoved->data)->TypeFlags.Contains(isPropSet) ));
               elRemoved = (CHE*)elRemoved->successor);
          if (elRemoved) {
            IDTable.DeleteID(((LavaDECL*)elRemoved->data)->OwnID);
            parEl->NestedDecls.Delete(elRemoved);
          }
        }
      }
      Step(newHint, parEl, relElem);
      if (relElem)
        parEl->NestedDecls.Insert(relElem->predecessor, cheElDef);
      else
        parEl->NestedDecls.Insert(parEl->NestedDecls.last, cheElDef);
      ((LavaDECL*)cheElDef->data)->ParentDECL = parEl;
      if (hint->FirstLast.Contains(firstHint) && !hint->CommandData5)
        ((LavaDECL*)cheElDef->data)->WorkFlags.INCL(selAfter);
      else
        ((LavaDECL*)cheElDef->data)->WorkFlags.EXCL(selAfter);
      pNewDecl = (LavaDECL**)&cheElDef->data;
      if (newHint->CommandData5) //from drag and drop?
        if (undo || redo) {
          oldDECL = IDTable.GetDECL(0, ((LavaDECL*)newHint->CommandData1)->OwnID);
          IDTable.UndoChange(pNewDecl, oldDECL);
        }
        else { //make the ID valid and set the true LavaDECL**
          IDTable.UndoDeleteID(hintDECL->OwnID);
          IDTable.Change(pNewDecl);
          if ((*pNewDecl)->DeclType == VirtualType)
            (*pNewDecl)->ParentDECL->WorkFlags.INCL(recalcVT);
        }
      else {
        if (undo || redo) {//make the ID valid and set the true LavaDECL**
          IDTable.UndoDeleteID(hintDECL->OwnID);
          IDTable.Change(pNewDecl);
          if ((*pNewDecl)->DeclType == VirtualType)
            (*pNewDecl)->ParentDECL->WorkFlags.INCL(recalcVT);
        }
        else {
          if (viewHint->CommandData6) 
            IDTable.PrepareClipTree((SyntaxDefinition*) viewHint->CommandData6, (DString*)viewHint->CommandData7);
          IDTable.NewID(pNewDecl);
          if ((*pNewDecl)->DeclType == VirtualType)
            (*pNewDecl)->ParentDECL->WorkFlags.INCL(recalcVT);
          viewHint->CommandData6 = 0;
          viewHint->CommandData7 = 0;
        }
        if (( (*pNewDecl)->DeclType == VirtualType) || ((*pNewDecl)->DeclType == Function)
            || ((*pNewDecl)->DeclType == Attr))
          (*pNewDecl)->ParentDECL->WorkFlags.INCL(recalcVT);

      }
      bool cop = false;
      if (((*pNewDecl)->SecondTFlags.Contains(funcImpl) || (*pNewDecl)->SecondTFlags.Contains(overrides)) 
          && FindInSupports((*pNewDecl)->LocalName, *pNewDecl, *(LavaDECL**)newHint->CommandData4))
        new CLavaError(&(*pNewDecl)->DECLError2, &ERR_NameCollision);
      else
        if (!undo && !redo && ((*pNewDecl)->DeclType != FormText)) {
          while (FindInSupports((*pNewDecl)->LocalName, *pNewDecl, *(LavaDECL**)newHint->CommandData4)) {
            if (cop)
              (*pNewDecl)->LocalName.Insert(DString("_"), 6);
            else {
              cop = true;
              (*pNewDecl)->LocalName.Insert(DString("CopyOf_"), 0);
            }
          }
      }
      if (newHint->CommandData8)
        ExecViewPrivToPub(*pNewDecl,newHint->CommandData8);
    }
  }//insert
  else {    // CPECommand_Delete:
    if ((hintDECL->DeclType == PatternDef) 
        || (hintDECL->DeclType == DragDef))  { //this contains a decl collection for consistent drag/drop or copy
      cheHint = (CHE*)hintDECL->NestedDecls.first;
      /*
      if (hint->FirstLast.Contains(firstHint))
        firstlast = (const unsigned long)1;
      else
        firstlast = (const unsigned long)0;
      */
      firstlast = hint->FirstLast;
      firstlast.EXCL(lastHint);
      pos = (int)hint->CommandData3;
      lastDefType = ((LavaDECL*)cheHint->data)->DeclType;
      while (cheHint) {
        elDECL = (LavaDECL*)cheHint->data;
        /*
        if (elDECL->WorkFlags.Contains(SupportsReady))
          elDECL->Supports.Destroy();
        */
        if (hint->CommandData2)
          str2 = new DString(*(DString*)hint->CommandData2);
        if (lastDefType == PatternDef)
          if (elDECL->DeclType != PatternDef)
            pos = 1;
        else {
          if (((lastDefType == Function) || (lastDefType == Attr))
                && (elDECL->DeclType != Function) && (elDECL->DeclType != Attr))
              pos = 1;
          else
            if ((lastDefType != Function) && (lastDefType != Attr)
                && ((elDECL->DeclType == Function) || (elDECL->DeclType == Attr)))
              pos = 1;
            else
              if ((lastDefType != IAttr) && (lastDefType != OAttr)
                  && ((elDECL->DeclType == IAttr) || (elDECL->DeclType == OAttr)))
                pos = 1;
          }
        newHint = new CLavaPEHint(CPECommand_Delete, this, firstlast, (DWORD)elDECL, 
                                  (DWORD)str2, (DWORD)pos, hint->CommandData4);
        Step(newHint, parEl, relElem);
        while (relElem //delete von nicht aufeinender folgenden Elementen
          && (((LavaDECL*)relElem->data)->OwnID != elDECL->OwnID)) {
          pos++;
          relElem = (CHE*)relElem->successor;
        }
        newHint->CommandData3 = (DWORD)pos; //korrigiere pos
        UndoMem.AddToMem(newHint);
        elRemoved = (CHE*)parEl->NestedDecls.Uncouple(relElem);
        if (elRemoved) {
          if (firstlast.Contains(firstHint) || ((LavaDECL*)elRemoved->data)->WorkFlags.Contains(selAfter))
            SetSelectDECL(elRemoved);
          ((LavaDECL*)elRemoved->data)->WorkFlags.EXCL(selAfter);
          newHint->CommandData1 = (DWORD)elRemoved->data;
          newHint->CommandData9 = (DWORD)elRemoved;
          IDTable.DeleteID(((LavaDECL*)elRemoved->data)->OwnID);
          if (((LavaDECL*)elRemoved->data)->DeclType == VirtualType)
            ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(recalcVT);
        }
        firstlast.EXCL(firstHint);
        lastDefType = elDECL->DeclType;
        cheHint = (CHE*)cheHint->successor;
        UpdateAllViews(NULL, 0, newHint);
      }
      viewHint = 0;
    }
    else {
      Step(hint, parEl, relElem);
      elRemoved = (CHE*)parEl->NestedDecls.Uncouple(relElem);
      if (elRemoved) {
        if (hint->FirstLast.Contains(firstHint) || ((LavaDECL*)elRemoved->data)->WorkFlags.Contains(selAfter))
          SetSelectDECL(elRemoved);
        ((LavaDECL*)elRemoved->data)->WorkFlags.EXCL(selAfter);
        hint->CommandData1 = (DWORD)elRemoved->data;
        hint->CommandData9 = (DWORD)elRemoved;
        IDTable.DeleteID(hintDECL->OwnID);
        if ((hintDECL->DeclType == VirtualType) || (hintDECL->DeclType == Function) ||  (hintDECL->DeclType == Attr) ) 
          hintDECL->ParentDECL->WorkFlags.INCL(recalcVT);
      }
    }
  }
  return viewHint;
}

void CPEBaseDoc::SetSelectDECL(CHE* elRemoved)
{
  if (elRemoved->successor && InSection(elRemoved))
    ((LavaDECL*)((CHE*)elRemoved->successor)->data)->WorkFlags.INCL(selAfter);
  else
    if (elRemoved->predecessor && InSection((CHE*)elRemoved->predecessor))
      ((LavaDECL*)((CHE*)elRemoved->predecessor)->data)->WorkFlags.INCL(selAfter);
    else {
      ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selAfter);
      switch (((LavaDECL*)elRemoved->data)->DeclType) {
      case IAttr:
        ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selIn);
        break;
      case OAttr:
        ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selOut);
        break;
      case Attr:
      case Function:
      case FormText:
        ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selMems);
        break;
      case VirtualType:
        ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selPara);
        break;
      default: 
        ((LavaDECL*)elRemoved->data)->ParentDECL->WorkFlags.INCL(selDefs);
      }
    }
}

bool CPEBaseDoc::InSection(CHE* el)
{
  if (!el->successor)
    return false;
  switch (((LavaDECL*)el->data)->DeclType) {
    case IAttr:
      return ((LavaDECL*)((CHE*)el->successor)->data)->DeclType == IAttr;
      break;
    case OAttr:
      return ((LavaDECL*)((CHE*)el->successor)->data)->DeclType == OAttr;
      break;
    case Attr:
    case Function:
    case FormDef:
      return (((LavaDECL*)((CHE*)el->successor)->data)->DeclType == Attr)
              || (((LavaDECL*)((CHE*)el->successor)->data)->DeclType == Function)
              || (((LavaDECL*)((CHE*)el->successor)->data)->DeclType == FormText);
      break;
    case VirtualType:
      return ((LavaDECL*)((CHE*)el->successor)->data)->DeclType == VirtualType;
      break;
    default: 
      return (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != IAttr)
              && (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != OAttr)
              && (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != Attr)
              && (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != Function)
              && (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != FormText)
              && (((LavaDECL*)((CHE*)el->successor)->data)->DeclType != VirtualType);
  }
}

void  CPEBaseDoc::ChangeDECL(CLavaPEHint* hint, bool undo)
{
  if (!hint->CommandData1) 
    return;
  LavaDECL *oldDECL = *(LavaDECL**)hint->CommandData4;
  *(LavaDECL**)hint->CommandData4 = (LavaDECL*)hint->CommandData1;
  if (undo)
    IDTable.UndoChange((LavaDECL**)hint->CommandData4, oldDECL);
  else {
    IDTable.Change((LavaDECL**)hint->CommandData4);
  }
  *(DString*)hint->CommandData2 = ((LavaDECL*)hint->CommandData1)->FullName;  //new name
  if (hint->FirstLast.Contains(firstHint))
    ((LavaDECL*)hint->CommandData1)->WorkFlags.INCL(selAfter);
  else
    ((LavaDECL*)hint->CommandData1)->WorkFlags.EXCL(selAfter);
  hint->CommandData1 = (DWORD)oldDECL;
}


bool CPEBaseDoc::UpdateDoc(CLavaBaseView *, bool undo, CLavaPEHint *doHint, bool redo)
{ 
  int newINCL=0, pos = -1, undoRedo = 0;
  LavaDECL *decl;
  CHESimpleSyntax *che1, *che2;
  CLavaPEHint *hint, *viewHint;
  TDECLComment* ptr;
  DString inclFile, clipDoc, oldTopName, oldUsedName;
  SynFlags firstLast;
  bool move, localMove = false, inclDel=false, bb, otherDocs = true;

  if (undo) {
    hint = UndoMem.UndoFromMem(pos);
    undoRedo = 1;
    otherDocs = !hint->FirstLast.Contains(multiDocHint);
  }
  else
    if (redo) {
      hint = UndoMem.RedoFromMem(pos);
      undoRedo = 2;
      otherDocs = !hint->FirstLast.Contains(multiDocHint);
    }
    else
      if (doHint)
        hint = doHint;
      else
        hint = UndoMem.DoFromMem(pos);
  Modify ( UndoMem.DocModified() || modified); 
  UpdateNo++;
  move = (hint->com == CPECommand_Move) && hint->CommandData6;
  if (move)
    clipDoc = *(DString*)hint->CommandData7;
  while (hint) {
    UpdateNo++;
    viewHint = hint;
    switch (hint->com) {
    case CPECommand_Comment: 
      if (hint->CommandData3) {
        ptr = ((CHEEnumSelId*)hint->CommandData1)->data.DECLComment.ptr;
        ((CHEEnumSelId*)hint->CommandData1)->data.DECLComment.ptr = (TDECLComment*)hint->CommandData2;
      }
      else {
        ptr = ((LavaDECL*)hint->CommandData1)->DECLComment.ptr;
        ((LavaDECL*)hint->CommandData1)->DECLComment.ptr = (TDECLComment*)hint->CommandData2;
      }
      hint->CommandData2 = (DWORD)ptr;
      break;
    case CPECommand_Exec:
      if (LBaseData->ExecUpdate) {
        if (undo || redo) {
          decl = ((LavaDECL*)hint->CommandData1)->ParentDECL;
          if (!GetExecDECL(decl,((LavaDECL*)hint->CommandData1)->DeclType, false,false)) 
            SetExecChe(decl,(LavaDECL*)hint->CommandData1);
          OpenExecView((LavaDECL*)hint->CommandData1);
        }
        LBaseData->ExecUpdate->ChangeExec(hint, this, undo || redo);
      }
      break;
    case CPECommand_Change:
      ChangeDECL(hint, undo || redo);
      break;
    case CPECommand_Insert:
    case CPECommand_Move:
    case CPECommand_Delete:
      viewHint = InsDelDECL(hint, undo, redo, localMove);
      break;
    case CPECommand_Exclude:
      newINCL =((CHESimpleSyntax*)hint->CommandData1)->data.nINCL;
      inclFile = *(DString*)hint->CommandData2;
      if (undo) {
        UndoDelSyntax(hint);
        inclDel = false;
      }
      else {
        DelSyntax((CHESimpleSyntax*)hint->CommandData1);
        inclDel = true;
      }
      UpdateAllViews(NULL, 0, 0);
      UpdateOtherDocs(0, inclFile, newINCL, inclDel);
      inclFile.Reset(0);
      otherDocs = false;
      break;
    case CPECommand_Include:
      inclFile = *(DString*)hint->CommandData2;
      if (undo) {
        newINCL =((CHESimpleSyntax*)hint->CommandData1)->data.nINCL;
        DelSyntax((CHESimpleSyntax*)hint->CommandData1);
        inclDel = true;
      }
      else 
        if (redo) {
          UndoDelSyntax(hint);
          newINCL =((CHESimpleSyntax*)hint->CommandData1)->data.nINCL;
          inclDel = false;
        }
        else {
          inclDel = false;
          AbsPathName(inclFile, IDTable.DocDir);
          QString inclFile_q = inclFile.c;
          hint->CommandData1 = (DWORD)IncludeSyntax(inclFile_q, bb, 2);
          if (!hint->CommandData1)
            return false;
          che1 = (CHESimpleSyntax*)hint->CommandData1;
          newINCL = che1->data.nINCL;
          che1->data.UsersName = *(DString*)hint->CommandData4;
          RelPathName(che1->data.UsersName,IDTable.DocDir); 
        }
      UpdateAllViews(NULL, 0, 0);
      UpdateOtherDocs(0, inclFile, newINCL, inclDel);
      inclFile.Reset(0);
      otherDocs = false;
      break;
    case CPECommand_ChangeInclude:
      che1 = (CHESimpleSyntax*)hint->CommandData1;
      che2 = (CHESimpleSyntax*)((CHESimpleSyntax*)hint->CommandData3)->successor;

      if (!SameFile(che1->data.SyntaxName, IDTable.DocDir, che2->data.SyntaxName, IDTable.DocDir)) {
        che2 = (CHESimpleSyntax*)mySynDef->SynDefTree.Uncouple(((CHESimpleSyntax*)hint->CommandData3)->successor);
        hint->CommandData1 = (DWORD)che2;
        mySynDef->SynDefTree.Insert((CHESimpleSyntax*)che2->predecessor, che1);
        inclFile = che1->data.SyntaxName;
        newINCL = che1->data.nINCL;
        AbsPathName(inclFile, IDTable.DocDir);
        che1->data.TopDef.Destroy();
        for (int id = 0; (id < IDTable.IDTab[che1->data.nINCL]->freeID); id++) 
          if (IDTable.IDTab[che1->data.nINCL]->SimpleIDTab[id]->idType == globalID)
            IDTable.IDTab[che1->data.nINCL]->SimpleIDTab[id]->idType = noID;
        IDTable.IDTab[che1->data.nINCL]->FileName = che1->data.SyntaxName;
        QString inclFile_q = inclFile.c;
        che1 = IncludeSyntax(inclFile_q, bb, 3);
        inclDel = false;
      }
      else {
        oldTopName = ((LavaDECL*)che2->data.TopDef.ptr)->LocalName;
        if (che1->data.LocalTopName != oldTopName) {
          IDTable.SetAsName(che2->data.nINCL, che1->data.LocalTopName, oldTopName, che1->data.TopDef.ptr);
          ((LavaDECL*)che2->data.TopDef.ptr)->LocalName = che1->data.LocalTopName;
          oldTopName = che2->data.LocalTopName; //old local name
          che2->data.LocalTopName = che1->data.LocalTopName;
          che1->data.LocalTopName = oldTopName;
        }
        oldUsedName = che2->data.UsersName;
        if (che1->data.UsersName != oldUsedName) {
          che2->data.UsersName = che1->data.UsersName;
          che1->data.UsersName = oldUsedName;
        }
      }
      if (!undo && !redo && !firstLast.Contains(lastHint)) {
        UndoMem.SetLastHint();
        UpdateAllViews(NULL, 3, 0);
        UpdateOtherDocs(0, inclFile, newINCL, inclDel);
      }
      break;
    default: ;
    }//switch Command
    if (viewHint) {
      if (!undoRedo || !viewHint->FirstLast.Contains(multiDocHint))
        UpdateAllViews(NULL, undoRedo, viewHint);
      if (hint != viewHint)
        delete viewHint;
    }
    if (undo) //{
      hint = UndoMem.UndoFromMem(pos);
 //     Modify (UndoMem.DocModified() || modified); 
//    }
    else
      if (redo)
        hint = UndoMem.RedoFromMem(pos);
      else {
        firstLast = hint->FirstLast;
        if (doHint)
          hint = 0;
        else
          hint = UndoMem.DoFromMem(pos);
      }
    Modify (UndoMem.DocModified() || modified); 
  }//while
  if (move && !localMove)
    UpdateMoveInDocs(clipDoc);
  else
    if (otherDocs && (undo || redo || firstLast.Contains(lastHint))) {
      UpdateOtherDocs(0, inclFile, newINCL, inclDel);
      inclFile.Reset(0);
    }
  if (firstLast.Contains(lastHint))
    IDTable.ChangeTab.Destroy();
  IDTable.DragINCL = 0;
  IDTable.DropINCL = 0;
  if (localMove)
    LBaseData->debugThread->checkAndSetBrkPnts(this);
  if (m_savedYet && LBaseData->m_saveEveryChange)
    wxDocManager::GetDocumentManager()->OnFileSave();
  return true;
}

void CPEBaseDoc::SetLastHint(bool otherDocs)
{
  DString str0;
  SynFlags flag;
  if (UndoMem.DrawTree)
    flag.INCL(noDrawHint);
  UndoMem.SetLastHint();
  if (otherDocs) {
    UpdateAllViews(NULL, 3, 0);
    UpdateOtherDocs(0, str0, 0, false, flag);
  }
  IDTable.ChangeTab.Destroy();
}


bool CPEBaseDoc::Undo(bool redo)
{
  return UpdateDoc(NULL, !redo, NULL, redo);
}

/////////////////////////////////////////////////////////////////////////////
// CPEBaseDoc diagnostics


bool CPEBaseDoc::OnSaveDocument(const QString& lpszPathName) 
{ //lpszPathName is the full-path and all links resolved filename 
  DString fn, oldDocDir, oldDocName, oldFileExtension, *pNewName, relFn, str;
  CLavaPEHint *hint;
  bool isnew;
  CHESimpleSyntax *simpleSyntax, *icheSS;
  SynDef *isyntax;

  QFileInfo fi(lpszPathName);
  fn = DString(qPrintable(lpszPathName));
  if (fi.exists() && !fi.isWritable()) {
    str = DString("Lava file '") + fn + DString("' couldn't be opened for writing");
    QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),str.c ,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    return false;
  }
  isReadOnly = false;
  oldDocDir = IDTable.DocDir;
  oldDocName = IDTable.DocName;
  oldFileExtension = IDTable.FileExtension;
  IDTable.DocDir = DString(qPrintable(fi.dirPath())) + "/";
  relFn = fn;
  RelPathName(relFn, IDTable.DocDir);
  simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
  isnew = false;
  if (simpleSyntax) {
    isnew = !SameFile(simpleSyntax->data.SyntaxName, IDTable.DocDir, fn);
    if (isnew ) 
      CalcNames(lpszPathName);
    simpleSyntax->data.UsersName = DString(qPrintable(GetUserFilename()));
    simpleSyntax->data.SyntaxName = relFn; 
    if (!SameDir(IDTable.DocDir, oldDocDir)) {
      simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
      while (simpleSyntax) {
        AbsPathName(simpleSyntax->data.SyntaxName, oldDocDir);
        RelPathName(simpleSyntax->data.SyntaxName, IDTable.DocDir);
        AbsPathName(simpleSyntax->data.UsersName, oldDocDir);
        RelPathName(simpleSyntax->data.UsersName, IDTable.DocDir);
        simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
      }
      for (int incl = 1; incl < IDTable.freeINCL; incl++) 
        if (IDTable.IDTab[incl]->isValid) {
          AbsPathName(IDTable.IDTab[incl]->FileName, oldDocDir);
          RelPathName(IDTable.IDTab[incl]->FileName, IDTable.DocDir);
        }
    }
  }
  if (isnew) {
    pNewName = new DString(IDTable.DocName);
    hint = new CLavaPEHint(CPECommand_Change, this, (const unsigned long)3, 0, (DWORD)pNewName, 0, 0);
    IDTable.IDTab[0]->FileName = relFn;
    UpdateDoc(0, false, hint);
    delete hint;
  }
  if (hasIncludes) {
    simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
    isyntax = new SynDef;
    icheSS = new CHESimpleSyntax;
    isyntax->SynDefTree.Append(icheSS);
    icheSS->data = simpleSyntax->data;
    simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
    while (simpleSyntax) {
      icheSS = new CHESimpleSyntax;
      isyntax->SynDefTree.Append(icheSS);
      icheSS->data.SyntaxName = simpleSyntax->data.SyntaxName;
      icheSS->data.UsersName = simpleSyntax->data.UsersName;
      icheSS->data.LocalTopName = simpleSyntax->data.LocalTopName;
      icheSS->data.nINCL = simpleSyntax->data.nINCL;
      simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
    }
  }
  else
    isyntax = mySynDef;
  isyntax->FreeID = IDTable.IDTab[0]->freeID;
  isyntax->FreeINCL = IDTable.freeINCL;
  if (!SynIO.WriteSynDef(fn.c, isyntax)) {
    str = DString("Error on writing the lava file '") + fn + "'";
    QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),str.c,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    return false;
  }
  if (hasIncludes)
    delete isyntax; 
  if (mySynDef->SynDefTree.first == mySynDef->SynDefTree.last) {
    if (!isStd) {
      str = DString("File '") + fn + "' is not a valid lava file";
      critical(wxTheApp->m_appWindow,qApp->name(),str.c,QMessageBox::Ok,0,0);
    }
    changeNothing = !isStd || !LBaseData->stdUpdate;
  }
	else
		changeNothing = isReadOnly;
  UndoMem.SetSavePos();
  modified = false;
  Modify(false);
  return true; 
}



bool CPEBaseDoc::OnCloseDocument() 
{
  UndoMem.CleanUndoMem();
  MainView = 0;
  return wxDocument::OnCloseDocument();
}



void CPEBaseDoc::OnDestroyMainView(CLavaBaseView* mview)
{
	if (wxTheApp->deletingMainFrame) return;

  POSITION pos = GetFirstViewPos();
  CLavaBaseView* tview;
//  OnSaveModified();
  while (pos ) {
    tview = (CLavaBaseView*)GetNextView(pos);
    if ((tview != mview) && (tview->GetParentFrame() != mview->GetParentFrame())) {
      //SaveModified();
      delete tview->GetParentFrame();
      //tview->GetParentFrame()->DestroyWindow();
      ViewPosRelease(pos);
      pos = GetFirstViewPos();
    }
 }
}


bool CPEBaseDoc::OnSaveModified()
{
  if (!asked) {
    Modify (UndoMem.DocModified() || modified); 
    if (wxDocument::OnSaveModified()) {
      UndoMem.SetSavePos();
      modified = false;
      Modify(false);
      asked = true;
      return true;
    }
    else
      return false;
  }
  return true;
}

void SortCombo(QComboBox* lbox)
{
  QVariant itemData0=lbox->itemData(0);
  QString itemText0=lbox->itemText(0);

  lbox->removeItem(0);// remove headline item
  QAbstractItemModel *mod=lbox->model();
  mod->sort(/*column*/0);
  lbox->insertItem(0,itemText0,itemData0); // re-insert headline item
}

/*
static HRESULT CreateShortCut (LPCSTR pszShortcutFile, LPSTR pszLink)
{
  HRESULT hres;
  IShellLink *psl;

  // Create an IShellLink object and get a pointer to the IShellLink 
  // interface (returned from CoCreateInstance).
  hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
  IID_IShellLink, (void **)&psl);
  if (SUCCEEDED (hres))
  {
    IPersistFile *ppf;

    // Query IShellLink for the IPersistFile interface for 
    // saving the shortcut in persistent storage.
    hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);
    if (SUCCEEDED (hres))
    { 
      WORD wsz [MAX_PATH]; // buffer for Unicode string

      // Set the path to the shortcut target.
      hres = psl->SetPath (pszShortcutFile);

      if (! SUCCEEDED (hres))
        QMessageBox ();//"SetPath failed!");

      // Set the description of the shortcut.
      hres = psl->SetDescription ("");

      if (! SUCCEEDED (hres))
        QMessageBox ();//"SetDescription failed!");

      // Ensure that the string consists of ANSI characters.
      MultiByteToWideChar (CP_ACP, 0, pszLink, -1, wsz, MAX_PATH);

      // Save the shortcut via the IPersistFile::Save member function.
      hres = ppf->Save (wsz, TRUE);

      if (! SUCCEEDED (hres))
        QMessageBox ();//"Save failed!");

      // Release the pointer to IPersistFile.
      ppf->Release ();
    }
    // Release the pointer to IShellLink.
    psl->Release ();
  }
  return hres;
} 
*/

void CLavaBaseView::whatNext()
{
  QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),"\"What next\" help not yet available for this selection",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
}

void CLavaBaseView::howTo()
{
  QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),"\"How to\" help not yet available for this view type",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
}
