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


#include "LavaPE.h"
#include "VTView.h"
#include "ExecTree.h"
#include "LavaPEFrames.h"
#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "Boxes.h"
#include "qpixmapcache.h"


CVTView::CVTView(QWidget* parent, wxDocument *doc)
: CTreeView(parent, doc, "VTView")
{  
  myMainView = 0;
  myDECL = 0;
  CollectDECL = 0;
  collectParent = 0;
  lastCurrent = 0;
  activeInt = false;
  setFont(LBaseData->m_TreeFont);
}

CVTView::~CVTView()
{
  DisableActions();
}


CLavaPEDoc* CVTView::GetDocument() // non-debug version is inline
{
  return (CLavaPEDoc*)m_viewDocument;
}

bool CVTView::OnCreate() 
{
  connect(m_tree,SIGNAL(selectionChanged()), SLOT(OnSelchanged()));
  //connect(m_tree,SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)), SLOT(OnDblclk(QListViewItem*, const QPoint&, int)));
  return true;
}

void CVTView::UpdateUI()
{
  if (!myDECL)
    return;
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  OnUpdateGotodef(frame->gotoDeclAction);
  OnUpdateOverride(frame->overrideAction);
}

void CVTView::OnInitialUpdate()
{
  OnUpdate(this, 0, 0);
  GetListView()->setCurrentItem(GetListView()->firstChild());
}


void CVTView::OnUpdate(wxView* , unsigned undoRedo, QObject* pHint) 
{
  if (!pHint && (undoRedo != 3) && (undoRedo < CHLV_noCheck))
    return;
  bool bb = false;
  if (pHint && ((CLavaPEHint*)pHint)->com == CPECommand_OpenSelView) {
    bb = true;
    myDECL = (LavaDECL*)((CLavaPEHint*)pHint)->CommandData1;
    myMainView = (CLavaPEView*)((CLavaPEHint*)pHint)->CommandData2;
    myID = TID(myDECL->OwnID, 0);
  }
  if (!myDECL)
    return;
  if (!bb) {
    myDECL = GetDocument()->IDTable.GetDECL(myID);
    if (myDECL) 
      bb = true;
    else {
      DeleteItemData(0);
      delete GetListView()->firstChild();
      return;
    }
  }
  if ((undoRedo == 1) && pHint && ((CLavaPEHint*)pHint)->FirstLast.Contains(firstHint)
      || (undoRedo != 1) && pHint && ((CLavaPEHint*)pHint)->FirstLast.Contains(lastHint)
      || (undoRedo == 3) || (undoRedo >= CHLV_showError)
      || (((CLavaPEHint*)pHint)->com == CPECommand_FromOtherDoc)) {
    if (bb) {
      setUpdatesEnabled(false);
      //if (myDECL->WorkFlags.Contains(recalcVT)) {19.08.2002, nach einfügen von feature in Basisklasse wurde VElems nicht neu brechnet
        //myDECL->VElems.UpdateNo = GetDocument()->UpdateNo;
				if (GetDocument()->MakeVElems(myDECL) )
					myDECL->WorkFlags.EXCL(recalcVT);
      //}
      DeleteItemData(0);
      delete GetListView()->firstChild();
      if (DrawTreeAgain()) {
        DeleteItemData(0);
        delete GetListView()->firstChild();
        DrawTreeAgain();
      }
      ExpandItem((CTreeItem*)GetListView()->firstChild(), 5);
      setUpdatesEnabled(true);
      GetListView()->update();
    }
    else {
      myDECL = 0;
    }
    if (pHint && (((CLavaPEHint*)pHint)->com == CPECommand_OpenSelView) 
              && !((CLavaPEHint*)pHint)->CommandData3)
      ((CTreeFrame*)GetParentFrame())->CalcSplitters(true);
  }
}

bool CVTView::DrawTreeAgain()
{
  DString lab, labCl;
  CTreeItem *itemP=0, *itemA=0, *topNode, *itemPCl=0, *itemACl=0, *item, *itemCl;
  QPixmap *bm, *bmCl, *bmPP, *bmP, *bmF, *bmA, *bmAA;
  int no = 0;
  TNodeType type;
  CHETID* cheID;
  TID TIDCl, TIDDef;
  CVTItemData * data;
  LavaDECL *ClDECL=0, *ElDECL;
  CHETVElem *El;

  bmCl = myMainView->GetPixmap(false, false, myDECL->DeclType);
  bmPP = myMainView->GetPixmap(true, true, VirtualType);
  bmP = myMainView->GetPixmap(false, false, VirtualType);
  bmF = myMainView->GetPixmap(false, false, Function);
  bmA = myMainView->GetPixmap(false, false, Attr);
  bmAA = myMainView->GetPixmap(true, true, Attr);
  lastCurrent = 0;
  if (myDECL->DeclType == Package)
    lab = DString("Virtual types of ") + myDECL->FullName;
  else  
    lab = DString("VTs and features of ") + myDECL->FullName;
  topNode = InsertItem(lab.c, ((CLavaPEApp*)wxTheApp)->LavaPixmaps[overRidesBM],/*QPixmapCache::find("l_overwrit"),*/ TVI_ROOT,TVI_FIRST);
  if (myDECL->DeclType == Package) 
    itemP = topNode;
  El = (CHETVElem*)myDECL->VElems.VElems.first;
  while (El) {
    if (TIDCl != El->data.VTClss) {
      ClDECL = GetDocument()->IDTable.GetDECL(El->data.VTClss);
      TIDCl = El->data.VTClss;
      itemPCl = 0;
      itemACl = 0;
    }
    ElDECL = GetDocument()->IDTable.GetDECL(El->data.VTEl);
    if (ElDECL && !ElDECL->TypeFlags.Contains(isInitializer)
               && !ElDECL->TypeFlags.Contains(isGUI)) {
      TIDDef = TID(ElDECL->ParentDECL->OwnID, ElDECL->inINCL);
      lab = ElDECL->LocalName;
      if (TIDDef != TIDCl) {
        lab += DString(" (in ");
        lab += ElDECL->ParentDECL->FullName;
        lab += klz;
      }
      if (ElDECL->DeclType == VirtualType) {
        bm = bmP;
        type = TNodeType_VT;
        if (!itemPCl) {
          if (!itemP) {
            labCl = DString("Virtual types");
            itemP = InsertItem(labCl.c, bmPP,topNode, TVI_FIRST);
          }
          data = new CVTItemData(TNodeType_Class, El, 0);
          if (ClDECL)
            labCl = ClDECL->FullName;
          else 
            if (El->data.VTClss.nINCL)
              return true;
            else
              labCl = "??";
          itemPCl = InsertItem(labCl.c, bmCl, itemP);
          itemPCl->setItemData( data); 
        }
        itemCl = itemPCl;
      }
      else {
        if (ElDECL->DeclType == Function)
          bm = bmF;
        else
          bm = bmA;
        type = TNodeType_Mem;
        if (!itemACl) {
          if (!itemA) {
            labCl = DString("Features");
            if (itemP)
              itemA = InsertItem(labCl.c, bmAA, topNode, itemP);
            else
              itemA = InsertItem(labCl.c, bmAA, topNode);
          }
          data = new CVTItemData(TNodeType_Class, El, 0);
          if (ClDECL)
            labCl = ClDECL->FullName;
          else 
            if (El->data.VTClss.nINCL)
              return true;
            else
              labCl = "??";
          itemACl = InsertItem(labCl.c, bmCl, itemA);
          itemACl->setItemData( data);
        }
        itemCl = itemACl;
      }
      data = new CVTItemData(type, El, 0);
      item = InsertItem(lab.c, bm, itemCl);
      if (El->data.Ambgs.first || !El->data.ok) 
        item->SetItemMask(true, false);
      item->setItemData(  data); 
      cheID = (CHETID*)El->data.Ambgs.first;
      while (cheID) {
        ElDECL = GetDocument()->IDTable.GetDECL(cheID->data);
        if (ElDECL) {
          lab = ElDECL->LocalName;
          lab += DString(" (in ");
          lab += ElDECL->ParentDECL->LocalName;
          lab += klz;
          data = new CVTItemData(type, El, no);
          item = InsertItem(lab.c, bm, itemCl);
          item->SetItemMask(true, false);
          item->setItemData(  data);
          no++;
        }
        else
          if (cheID->data.nINCL)
            return true;
        cheID = (CHETID*)cheID->successor;
      }
    }
    else
      if (!ElDECL && El->data.VTEl.nINCL)
        return true;
    El = (CHETVElem*)El->successor;
  }
  return false;
}


void CVTView::DeleteItemData(CTreeItem* parent)
{
  if (!parent)
    parent = (CTreeItem*)GetListView()->firstChild();
  if (parent) {
    CVTItemData * itd = (CVTItemData*)parent->getItemData();
    if (itd)
      delete itd;
    CTreeItem* item = (CTreeItem*)parent->firstChild();
    while (item) {
      DeleteItemData(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}


void CVTView::ExpandItem(CTreeItem* item, int level)
{
  if (level == 0)
    return;
  int count = level-1;
  CTreeItem* htr = item;
  item->setOpen(true);
  htr = (CTreeItem*)item->firstChild();
  while (htr) {
    ExpandItem(htr, count);
    htr = (CTreeItem*)htr->nextSibling();
  }

}

void CVTView::destroy() 
{
  if (((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext)
    ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext->RemoveView(this);
  DeleteItemData(0);
  CTreeView::destroy(); 
}

void CVTView::OnGotodef() 
{
  gotoDef((CTreeItem*)GetListView()->currentItem());
}

void CVTView::gotoDef(CTreeItem* item)
{
  if (item) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    if (itd->type != TNodeType_Class) {
      TID id;
      id.nID = -1;
      if (!itd->AmbgNo)
        id = itd->VTEl->data.VTEl;
      else {
        CHETID* cheID = (CHETID*)itd->VTEl->data.Ambgs.first;
        for (int ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++) 
          cheID = (CHETID*)cheID->successor;
        if (cheID)
          id = cheID->data;
      }
      ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, GetDocument()->IDTable.GetDECL(id));//, (CTreeItem*)item->parent());//GetParentItem(item));
      ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(GetDocument(), id);
    }
  }
}

CTreeItem* CVTView::BrowseTree(TID id, CTreeItem* start)
{
  TID elId;
  CTreeItem *topItem, *item=0;
  if (start)
    topItem = start;
  else
    topItem = (CTreeItem*)GetListView()->firstChild();
  CVTItemData * itd = (CVTItemData*)topItem->getItemData();
  if (itd && (itd->type != TNodeType_Class)) {
    if (!itd->AmbgNo)
      elId = itd->VTEl->data.VTEl;
    else {
      CHETID* cheID = (CHETID*)itd->VTEl->data.Ambgs.first;
      for (int ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++) 
        cheID = (CHETID*)cheID->successor;
      if (cheID)
        elId = cheID->data;
    }
  }
  if (id == elId)
    return topItem;
  CTreeItem* nitem;
  bool isValid = false;
  while (topItem && !item) {
    nitem = (CTreeItem*)topItem->firstChild();
    if (nitem)
      item = BrowseTree(id, nitem);
    if (!item) {
      topItem = (CTreeItem*)topItem->nextSibling();
      if (topItem) {
        itd = (CVTItemData*)topItem->getItemData();
        if (itd && (itd->type != TNodeType_Class)) {
          if (!itd->AmbgNo)
            elId = itd->VTEl->data.VTEl;
          else {
            CHETID* cheID = (CHETID*)itd->VTEl->data.Ambgs.first;
            for (int ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++) 
              cheID = (CHETID*)cheID->successor;
            if (cheID)
              elId = cheID->data;
          }
        }
        if (id == elId)
          return topItem;
      }
    }
  }
  return item;

}


void CVTView::OnUpdateGotodef(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    action->setEnabled(itd && (itd->type != TNodeType_Class));
  }
  else
    action->setEnabled(false);
}

/*
void CVTView::OnDblclk(QListViewItem* item, const QPoint&, int) 
{
  gotoDef((CTreeItem*)item);
}
*/

bool CVTView::EnableOverride(CTreeItem* item)
{
  LavaDECL *ElDECL, *baseDECL;
  CVTItemData * itd;
  CHETID* cheID;
  CContext con;
  bool sameContext;
  if (activeInt && item) {
    itd = (CVTItemData*)item->getItemData();
    if (itd && ((itd->type == TNodeType_VT)
               || (itd->type == TNodeType_Func)
               || (itd->type == TNodeType_Mem))) {
      ElDECL = GetDocument()->IDTable.GetDECL(itd->VTEl->data.VTEl);
      if (ElDECL) {
        TID TIDDef(ElDECL->ParentDECL->OwnID, ElDECL->inINCL);
        if (myID != TIDDef) {
          if (ElDECL->DeclType != VirtualType)
            return true;
          GetDocument()->IDTable.GetPattern(ElDECL, con);
          if (con.oContext && (ElDECL->ParentDECL != con.oContext))
             return GetDocument()->IDTable.lowerOContext(myDECL, ElDECL, sameContext) && !sameContext;
          else {
            cheID = (CHETID*)myDECL->Supports.first;
            while (cheID) { //!
              if (GetDocument()->IDTable.IsAn(cheID->data, myDECL->inINCL, TIDDef, 0)) {
                baseDECL = GetDocument()->IDTable.GetFinalDef(cheID->data, myDECL->inINCL);
                GetDocument()->IDTable.GetPattern(baseDECL, con);
                if (con.oContext && (baseDECL != con.oContext)
                  && (!GetDocument()->IDTable.lowerOContext(myDECL, ElDECL, sameContext) || sameContext))
                  return false;
              }
              cheID = (CHETID*)cheID->successor;
            }
            return true;
          }
        }
      }
    }
  }
  return false;
}

void CVTView::OnUpdateOverride(wxAction* action) 
{  
  action->setEnabled((!GetDocument()->changeNothing) &&
        EnableOverride((CTreeItem*)GetListView()->currentItem()));
}

void CVTView::OnOverride() 
{
  OnOverrideI((CTreeItem*)(CTreeItem*)GetListView()->currentItem());
}

void CVTView::PrepareDECL(LavaDECL* decl, CVTItemData* itd)
{
  int fromINCL = decl->inINCL;
  decl->inINCL = 0;
  decl->DECLComment.Destroy();
  decl->SecondTFlags.INCL(overrides);
  decl->Supports = itd->VTEl->data.Ambgs;
  CHETID* cheTID = new CHETID;
  cheTID->data = itd->VTEl->data.VTEl;
  decl->Supports.Prepend(cheTID);
  decl->OwnID = -1;
  decl->FullName = myDECL->FullName + ddppkt + decl->LocalName;
  decl->ParentDECL = myDECL;
  if (decl->DeclType == Function) {
    if (!myDECL->TypeFlags.Contains(isNative))
      decl->TypeFlags.EXCL(isNative);
    decl->NestedDecls.Destroy();
    GetDocument()->CheckOverInOut(decl, CHLV_inUpdateLow);
    decl->Inherits.Destroy();
  }
  else
    if ((decl->RefID.nID >= 0) && fromINCL)
      decl->RefID.nINCL = GetDocument()->IDTable.IDTab[fromINCL]->nINCLTrans[decl->RefID.nINCL].nINCL;
}


void CVTView::OnOverrideI(CTreeItem* item)
{
  CVTItemData * itd;
  LavaDECL *OverDECL, *elDECL, *parentDECL;
  LavaDECL* decl;
  if (CollectDECL) {
    OverDECL = CollectDECL;
    parentDECL = ((LavaDECL*)((CHE*)CollectDECL->NestedDecls.first)->data)->ParentDECL;
  }
  else
    if (item) {
      itd = (CVTItemData*)item->getItemData();
      OverDECL = GetDocument()->IDTable.GetDECL(itd->VTEl->data.VTEl);
      parentDECL = OverDECL->ParentDECL;
    }
  if (OverDECL) {
    bool isN = myDECL->TypeFlags.Contains(isNative);
    if ((OverDECL->DeclType == VirtualType) || (OverDECL->DeclType == PatternDef)) {
      decl = NewLavaDECL();
      decl->ParentDECL = myDECL;
      GetDocument()->CollectPattern(OverDECL, decl);
      decl->FullName = myDECL->FullName;
      CHE* che = (CHE*)parentDECL->NestedDecls.first;
      while (che) {
        elDECL = (LavaDECL*)che->data;
        if (elDECL->WorkFlags.Contains(checkmark)) {
          if ((elDECL->DeclType == Attr) || (elDECL->DeclType == Function)) 
            myDECL->TreeFlags.INCL(MemsExpanded);
          else
            if (elDECL->DeclType == VirtualType)
              myDECL->TreeFlags.INCL(ParaExpanded);
            else
              myDECL->TreeFlags.INCL(DefsExpanded);
        }
        che = (CHE*)che->successor;
      }
      parentDECL->ResetCheckmarks();
      che = (CHE*)decl->NestedDecls.first;
      TID elID;
      CHETVElem *El;
      while (che) {
        elDECL = (LavaDECL*)che->data;
        if ((elDECL->DeclType == VirtualType)
            || (elDECL->DeclType == Function) && !elDECL->TypeFlags.Contains(isStatic)
            || (elDECL->DeclType == Attr)) {
          elID = TID(elDECL->OwnID, elDECL->inINCL);
          El = (CHETVElem*)myDECL->VElems.VElems.first;
          while (El && (El->data.VTEl != elID))
            El = (CHETVElem*)El->successor;
          if (El)
            elDECL->Supports = El->data.Ambgs;
          if (!isN)
            elDECL->TypeFlags.EXCL(isNative);
        }
        che = (CHE*)che->successor;
      }
      myDECL->ResetCheckmarks();
      OverDECL->ResetCheckmarks();
      myDECL->TreeFlags.INCL(isExpanded);
      DString *str2 = new DString(myDECL->FullName);
      TIDType type;
      DWORD d4 = GetDocument()->IDTable.GetVar(myID, type);
      int pos = 10000;
      CLavaPEHint* hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1, (DWORD) decl, (DWORD)str2, (DWORD)pos, d4);
      if (myDECL->DeclType != Package)
        GetDocument()->IDTable.SetPatternStart(parentDECL->OwnID, myDECL->OwnID);
      GetDocument()->UpdateDoc(this, false, hint);
      CLavaPEHint* hintConcern = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)0, (DWORD) myDECL, 0, 0, d4);
      GetDocument()->ConcernForms(hintConcern);
      GetDocument()->ConcernImpls(hintConcern, *(LavaDECL**)d4);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
      myVT.Destroy();
      delete hint;
      delete hintConcern;
    }
    else {
      if (CollectDECL) {
        decl = CollectDECL;
        CollectDECL->DeclType = PatternDef;//DragDef;
      }
      else {
        decl = NewLavaDECL();
        *decl = *OverDECL;
        PrepareDECL(decl, itd);
      }
      TIDType type;
      DWORD d4 = GetDocument()->IDTable.GetVar(myID, type);
      DString *str2 = new DString(myDECL->FullName);
      int pos = myDECL->GetAppendPos(decl->DeclType);
      CLavaPEHint* hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1, (DWORD) decl, (DWORD)str2, (DWORD)pos, d4);
      if (!CollectDECL)
        GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
    }
  }
  if (CollectDECL)
    DeleteExChain();
}

bool CVTView::VerifyItem(CTreeItem* item, CTreeItem* topItem)
{
  if (!topItem)
    topItem = (CTreeItem*)GetListView()->firstChild();
  CTreeItem* nitem;
  bool isValid = false;
  while (topItem && (topItem != item) && !isValid) {
    nitem = (CTreeItem*)topItem->firstChild();
    if (nitem)
      isValid = VerifyItem(item, nitem);
    if (!isValid)
      topItem = (CTreeItem*)topItem->nextSibling();
  }
  return isValid || (topItem == item);
}

void CVTView::SetVTError(CTreeItem* item)
{
  DString str0;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(str0, true);
  if (item  && (item != GetListView()->firstChild())) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    if (itd && ((itd->type == TNodeType_VT)
               || (itd->type == TNodeType_Func)
               || (itd->type == TNodeType_Mem))
         && itd->VTEl->data.Ambgs.first) {
      CHAINX ErrChain;
      new CLavaError(&ErrChain, &ERR_AmbgInVT);
      ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetErrorOnBar(ErrChain);
    }
  }
  else 
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->ResetError();
}

bool CVTView::AddToExChain(CTreeItem* itemOver)
{
  LavaDECL *decl, *de;
  if ((collectParent == (CTreeItem*)itemOver->parent())
      && EnableOverride(itemOver)) {
    CVTItemData * itd = (CVTItemData*)itemOver->getItemData();
    if (!itd->VTEl->data.ElDECL) {
      decl = NewLavaDECL();
      de = GetDocument()->IDTable.GetDECL(itd->VTEl->data.VTEl);
      if (!de)
        return false;
      *decl = *de;
      itd->VTEl->data.ElDECL = decl;
      decl->DECLComment.Destroy();
      if (decl->DeclType == VirtualType)
        CollectDECL->DeclType = PatternDef;
      if ((decl->DeclType == Function) || (decl->DeclType == Attr))
        PrepareDECL(decl, itd);
      CHE* che = NewCHE(decl);
      CollectDECL->NestedDecls.Append(che);
    }
    return true;
  }
  return false;
}

void CVTView::DeleteExChain()
{
  CHETVElem* El = (CHETVElem*)myDECL->VElems.VElems.first;
  while (El) {
    El->data.ElDECL = 0;
    El = (CHETVElem*)El->successor;
  }
  CTreeItem* item = (CTreeItem*)GetListView()->firstChild();
//  unsigned nStateMask=0;// = TVIS_DROPHILITED; 
//  unsigned nState = 0;
//  SetAllStates(item, nState, nStateMask);
  if (CollectDECL) {
    delete CollectDECL;
    CollectDECL = 0;
  }
}

/*
void CVTView::SetAllStates(CTreeItem* item1, unsigned nState, unsigned nStateMask)
{
  //GetListView()->SetItemState(item1, nState, nStateMask);
  CTreeItem* item = GetNextItem(item1, true);
  while(item) {
    SetAllStates(item, nState, nStateMask);
    item = (CTreeItem*)item->nextSibling();
  }
}
*/

void CVTView::OnSelchanged() 
{
  bool ok = true;
  CTreeItem *itemOver = lastCurrent, *selItem = (CTreeItem*)GetListView()->currentItem();
  if (!selItem)
    return;
  SetVTError(selItem);
  
  if (itemOver && (GetListView()->withControl && !GetListView()->withShift || GetListView()->withShift && !GetListView()->withControl)) {
    if (!CollectDECL) {
      CollectDECL = NewLavaDECL();
      collectParent = (CTreeItem*)itemOver->parent();
      if (AddToExChain(itemOver)) {
        if (GetListView()->withShift) {
          while (itemOver && (itemOver != selItem)) {
            itemOver = (CTreeItem*)itemOver->nextSibling();//GetListView()->GetNextSiblingItem(itemOver);
            if (!AddToExChain(itemOver))
              itemOver = 0;
          }
          if (!itemOver || !CollectDECL->NestedDecls.first)
            DeleteExChain();
        }
        else { 
          itemOver = selItem;
          if (itemOver && !AddToExChain(itemOver))
            DeleteExChain();
        }
      }
      else
        DeleteExChain();
    }
  }
  else
    if (!itemOver)
      DeleteExChain();
  lastCurrent = selItem;
}

void CVTView::OnActivateView(bool bActivate, wxView *deactiveView) 
{
  CTreeItem* sel;
  if (GetDocument()->mySynDef)
    if (bActivate) {
      sel = (CTreeItem*)GetListView()->currentItem();
      SetVTError(sel);
      if (!GetListView()->hasFocus())
        GetListView()->setFocus();
      sel->repaint();
    }
    else 
      DisableActions();
}

void CVTView::DisableActions()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->gotoDeclAction->setEnabled(false);
  frame->overrideAction->setEnabled(false);
}
