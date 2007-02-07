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
#include "VTView.h"
#include "ExecTree.h"
#include "LavaPEFrames.h"
#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "Boxes.h"
#include "qpixmapcache.h"
//Added by qt3to4:
#include <QPixmap>

#pragma hdrstop


CVTItemData::CVTItemData(TNodeType t, CHETVElem* V, int A)
{
  type = t;
  VTEl_Tree = V->data;
  AmbgNo = A;
}

CVTView::CVTView(QWidget* parent, wxDocument *doc)
: CTreeView(parent, doc, "VTView")
{
  myMainView = 0;
  myDECL = 0;
  CollectDECL = 0;
  collectParent = 0;
  lastCurrent = 0;
  activeInt = false;
  currentBrType = findTID;
  setFont(LBaseData->m_TreeFont);
  Tree->viewport()->installEventFilter(this);
}

CVTView::~CVTView()
{
  if (!wxTheApp->deletingMainFrame)
    DisableActions();
}


CLavaPEDoc* CVTView::GetDocument() // non-debug version is inline
{
  return (CLavaPEDoc*)m_viewDocument;
}

bool CVTView::OnCreate()
{
//  connect(Tree,SIGNAL(itemSelectionChanged()), SLOT(OnSelchanged()));
  connect(Tree,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT    (OnSelchanged(QTreeWidgetItem*, QTreeWidgetItem*)));
  //connect(Tree,SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)), SLOT(OnDblclk(QListViewItem*, const QPoint&, int)));
  return true;
}

void CVTView::UpdateUI()
{
  if (!myDECL)
    return;
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  OnUpdateGotodef(frame->gotoDeclAction);
  OnUpdateOverride(frame->overrideAction);
  OnUpdateGotoImpl(frame->gotoImplAction);
}

bool CVTView::event(QEvent* ev)
{
  QWhatsThisClickedEvent *wtcEv;
  QString href;

  if (ev->type() == UEV_LavaPE_SyncTree) {
    CLavaPEHint* hint = (CLavaPEHint*)((CustomEvent*)ev)->data();
    OnUpdate(myMainView, 0, hint);
    delete hint;
    return true;
  }
  else if (ev->type() == UEV_LavaPE_setSel) {
    setSelPost((QTreeWidgetItem*)((CustomEvent*)ev)->data());
    return true;
  }
  else if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    href = wtcEv->href();
    ShowPage(QString("whatsThis/")+href);
    return true;
  }
  else
    return QWidget::event(ev);
}


void CVTView::OnInitialUpdate()
{
  OnUpdate(this, 0, 0);
  if (Tree->RootItem)
    Tree->setCurAndSel(Tree->RootItem);
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
    bb = GetDocument()->IDTable.GetDECL(myID) != 0;
  }
  if (!myDECL)
    return;
  if (!bb) {
    myDECL = GetDocument()->IDTable.GetDECL(myID);
    if (myDECL)
      bb = true;
    else {
      DeleteItemData(0);
      delete Tree->RootItem;
      Tree->RootItem = 0;
      return;
    }
  }
  if ((undoRedo == 1) && pHint && ((CLavaPEHint*)pHint)->FirstLast.Contains(firstHint)
      || (undoRedo != 1) && pHint && ((CLavaPEHint*)pHint)->FirstLast.Contains(lastHint)
      || (undoRedo == 3) || (undoRedo >= CHLV_showError)
      || (((CLavaPEHint*)pHint)->com == CPECommand_FromOtherDoc)) {
    if (bb) {
      setUpdatesEnabled(false);
			if (GetDocument()->MakeVElems(myDECL) )
				myDECL->WorkFlags.EXCL(recalcVT);
        DeleteItemData(0);
        delete Tree->RootItem;
        Tree->RootItem = 0;
      if (DrawTreeAgain()) {
        DeleteItemData(0);
        delete Tree->RootItem;
        Tree->RootItem = 0;
        DrawTreeAgain();
      }
      ExpandItem((CTreeItem*)Tree->RootItem, 5);
      setUpdatesEnabled(true);
      Tree->update();
      if (currentBaseID.nID != -1)
        lastCurrent = BrowseTree(currentBaseID,0,(int)currentBrType);
      if (lastCurrent)
        Tree->setCurAndSel(lastCurrent);
      else
        Tree->setCurAndSel(Tree->RootItem);
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
  int bm, bmCl, bmPP, bmP, bmF, bmA, bmAA;
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
  topNode = InsertItem(lab.c, overRidesBM,/*QPixmapCache::find("l_overwrit"),*/ TVI_ROOT,TVI_FIRST);
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
               && !ElDECL->SecondTFlags.Contains(isGUI)
               && !ElDECL->SecondTFlags.Contains(isLavaSignal)) {
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
        type = TNodeType_Feature;
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
    parent = (CTreeItem*)Tree->RootItem;
  if (parent) {
    if (parent != Tree->RootItem) {
      CVTItemData * itd = (CVTItemData*)parent->getItemData();
      parent->setItemData(0);
      if (itd)
        delete itd;
    }
    CTreeItem* item = (CTreeItem*)parent->child(0);
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
  Tree->expandItem(item);//->setOpen(true);
  htr = (CTreeItem*)item->child(0);
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
  //CTreeView::destroy();
}

void CVTView::OnGotoImpl()
{
  gotoDef((CTreeItem*)Tree->currentItem(), true);

}


void CVTView::OnGotoDecl()
{
  gotoDef((CTreeItem*)Tree->currentItem(), false);
}

void CVTView::gotoDef(CTreeItem* item, bool toImpl)
{
  TID id;
  CHETID* cheID;
  int ii;
  LavaDECL *decl;
  TBrowseCase bCase = findTID;
  CTreeItem *child;

  if (item) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    if (itd->type == TNodeType_Class) {
      child = (CTreeItem*)item->child(0);
      if (((CVTItemData*)child->getItemData())->type == TNodeType_VT)
        bCase = findInterfaceVTs;
      else
        bCase = findInterfaceFeatures;
      ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, GetDocument()->IDTable.GetDECL(itd->VTEl_Tree.VTClss), (int)bCase);//, (CTreeItem*)item->parent());//GetParentItem(item));
      if (toImpl) {
        if (!LBaseData->Browser->GotoImpl(GetDocument(), itd->VTEl_Tree.VTClss))
          QMessageBox::critical(this, qApp->applicationName(),ERR_NoClassImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      }
      else
        LBaseData->Browser->BrowseDECL(GetDocument(), itd->VTEl_Tree.VTClss);
    }
    else {
      id.nID = -1;
      if (!itd->AmbgNo)
        id = itd->VTEl_Tree.VTEl;
      else {
        cheID = (CHETID*)itd->VTEl_Tree.Ambgs.first;
        for (ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++)
          cheID = (CHETID*)cheID->successor;
        if (cheID)
          id = cheID->data;
      }
      decl = GetDocument()->IDTable.GetDECL(id);
      ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, decl);//, (CTreeItem*)item->parent());//GetParentItem(item));
      if (toImpl) {
        if (!LBaseData->Browser->GotoImpl(GetDocument(), decl))
          if (decl->TypeFlags.Contains(isAbstract) || decl->TypeFlags.Contains(isNative))
            QMessageBox::information(this, qApp->applicationName(),ERR_NoImplForAbstract,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          else
            QMessageBox::critical(this, qApp->applicationName(),ERR_NoFuncImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      }
      else
        ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(GetDocument(), id);
    }
  }
}

CTreeItem* CVTView::BrowseTree(TID id, CTreeItem* start, int browseCase)
{
  TID elId;
  TBrowseCase bCase = (TBrowseCase)browseCase;
  CTreeItem *topItem, *item=0;
  if (start)
    topItem = start;
  else
    topItem = (CTreeItem*)Tree->RootItem;
  if (!topItem)
    return 0;
  CVTItemData * itd = (CVTItemData*)topItem->getItemData();
  if (itd)
    if (itd->type != TNodeType_Class) {
      if ((itd->type == TNodeType_Feature ) && (bCase == findBaseTID))
        elId = itd->VTEl_Tree.VTBaseEl;
      else {
        if (!itd->AmbgNo)
          elId = itd->VTEl_Tree.VTEl;
        else {
          CHETID* cheID = (CHETID*)itd->VTEl_Tree.Ambgs.first;
          for (int ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++)
            cheID = (CHETID*)cheID->successor;
          if (cheID)
            elId = cheID->data;
        }
      }
      if (id == elId)
        return topItem;
    }
    else {
      elId = itd->VTEl_Tree.VTClss;
      if ((bCase == findInterfaceVTs) && (id == elId))
        return topItem;
      else
        if (bCase == findInterfaceFeatures)
          bCase = findInterfaceVTs;
    }
  CTreeItem* nitem;
  bool isValid = false;
  while (topItem && !item) {
    nitem = (CTreeItem*)topItem->child(0);
    if (nitem)
      item = BrowseTree(id, nitem, (int)bCase);
    if (!item) {
      topItem = (CTreeItem*)topItem->nextSibling();
      if (topItem) {
        itd = (CVTItemData*)topItem->getItemData();
        if (itd && (itd->type != TNodeType_Class)) {
          if (!itd->AmbgNo)
            elId = itd->VTEl_Tree.VTEl;
          else {
            CHETID* cheID = (CHETID*)itd->VTEl_Tree.Ambgs.first;
            for (int ii =1; (cheID != 0) && (ii<itd->AmbgNo); ii++)
              cheID = (CHETID*)cheID->successor;
            if (cheID)
              elId = cheID->data;
          }
          if (id == elId)
            return topItem;
        }
        else {
          if ((bCase == findInterfaceVTs) && (id == elId))
            return topItem;
          else
            if (bCase == findInterfaceFeatures)
              bCase = findInterfaceVTs;
        }
      }
    }
  }
  return item;

}

void CVTView::OnUpdateGotoImpl(QAction* action)
{
  CVTItemData * itd;
  LavaDECL* decl;
  bool enable;

  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  if (item) {
    itd = (CVTItemData*)item->getItemData();
    enable = itd && (itd->type != TNodeType_VT);
    if (enable && (itd->type == TNodeType_Feature)) {
      decl = GetDocument()->IDTable.GetDECL(itd->VTEl_Tree.VTEl);
      enable = (decl->DeclType != Attr) || decl->TypeFlags.Contains(hasSetGet);
    }
    action->setEnabled(enable);
  }
  else
    action->setEnabled(false);
}

void CVTView::OnUpdateGotodef(QAction* action)
{
  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  if (item) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    action->setEnabled(itd );
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
  if (!multiSelectCanceled && activeInt && item) {
    itd = (CVTItemData*)item->getItemData();
    if (itd && ((itd->type == TNodeType_VT)
               || (itd->type == TNodeType_Feature))) {
      ElDECL = GetDocument()->IDTable.GetDECL(itd->VTEl_Tree.VTEl);
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

void CVTView::OnUpdateOverride(QAction* action)
{
  action->setEnabled(!multiSelectCanceled && (!GetDocument()->changeNothing) &&
        EnableOverride((CTreeItem*)Tree->currentItem()));
}

void CVTView::OnOverride()
{
  OnOverrideI((CTreeItem*)(CTreeItem*)Tree->currentItem());
}

void CVTView::PrepareDECL(LavaDECL* decl, CVTItemData* itd)
{
  int fromINCL = decl->inINCL;
  decl->inINCL = 0;
  decl->DECLComment.Destroy();
  decl->SecondTFlags.INCL(overrides);
  decl->Supports = itd->VTEl_Tree.Ambgs;
  CHETID* cheTID = new CHETID;
  cheTID->data = itd->VTEl_Tree.VTEl;
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
  CVTItemData *itd;
  LavaDECL *OverDECL, *elDECL, *parentDECL, *decl;
  TID elID;
  CHETVElem *El;
  TIDType type;
  CHE* che;
  DString *str2;
  void *d4;
  int pos;
  CLavaPEHint *hintConcern, *hint;

  if (item)
    itd = (CVTItemData*)item->getItemData();
  else
    return;
  if (CollectDECL) {
    OverDECL = CollectDECL;
    parentDECL = ((LavaDECL*)((CHE*)CollectDECL->NestedDecls.first)->data)->ParentDECL;
  }
  else {
    OverDECL = GetDocument()->IDTable.GetDECL(itd->VTEl_Tree.VTEl);
    parentDECL = OverDECL->ParentDECL;
  }
  if (OverDECL) {
    bool isN = myDECL->TypeFlags.Contains(isNative);
    if ((OverDECL->DeclType == VirtualType) || (OverDECL->DeclType == PatternDef)) {
      decl = NewLavaDECL();
      decl->ParentDECL = myDECL;
      GetDocument()->CollectPattern(OverDECL, decl);
      decl->FullName = myDECL->FullName;
      che = (CHE*)parentDECL->NestedDecls.first;
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
        else {
          if (!isN)
            elDECL->TypeFlags.EXCL(isNative);
          elDECL->TypeFlags.EXCL(isAbstract);
          if (elDECL->DeclType == Interface)
            elDECL->fromBType = NonBasic;
        }

        che = (CHE*)che->successor;
      }
      currentBaseID = itd->VTEl_Tree.VTBaseEl;
      currentBrType = findBaseTID;
      myDECL->ResetCheckmarks();
      OverDECL->ResetCheckmarks();
      myDECL->TreeFlags.INCL(isExpanded);
      str2 = new DString(myDECL->FullName);
      d4 = (void*)GetDocument()->IDTable.GetVar(myID, type);
      pos = 10000;
      hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1,  decl, str2, (void*)pos, d4);
      if (myDECL->DeclType != Package)
        GetDocument()->IDTable.SetPatternStart(parentDECL->OwnID, myDECL->OwnID);
      GetDocument()->UpdateDoc(this, false, hint);
      hintConcern = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)0,  myDECL, 0, 0, d4);
      GetDocument()->ConcernForms(hintConcern);
      GetDocument()->ConcernImpls(hintConcern, *(LavaDECL**)d4);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
      lastCurrent = BrowseTree(currentBaseID,0,(int)currentBrType);
      if (lastCurrent)
        Tree->setCurAndSel(lastCurrent);
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
      d4 = (void*)GetDocument()->IDTable.GetVar(myID, type);
      str2 = new DString(myDECL->FullName);
      pos = myDECL->GetAppendPos(decl->DeclType);
      currentBaseID = itd->VTEl_Tree.VTBaseEl;
      currentBrType = findBaseTID;
      hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1,  decl, str2, (void*)pos, d4);
      if (!CollectDECL)
        GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
      lastCurrent = BrowseTree(currentBaseID,0,(int)currentBrType);
      if (lastCurrent)
        Tree->setCurAndSel(lastCurrent);
    }
  }
  if (CollectDECL)
    DeleteExChain(true);
}

bool CVTView::VerifyItem(CTreeItem* item, CTreeItem* topItem)
{
  if (!topItem)
    topItem = (CTreeItem*)Tree->RootItem;
  CTreeItem* nitem;
  bool isValid = false;
  while (topItem && (topItem != item) && !isValid) {
    nitem = (CTreeItem*)topItem->child(0);
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
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetComment(str0, true);
  if (item  && (item != Tree->RootItem)) {
    CVTItemData * itd = (CVTItemData*)item->getItemData();
    if (itd && ((itd->type == TNodeType_VT)
               || (itd->type == TNodeType_Feature))
         && itd->VTEl_Tree.Ambgs.first) {
      CHAINX ErrChain;
      new CLavaError(&ErrChain, &ERR_AmbgInVT);
      ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetErrorOnUtil(ErrChain);
    }
  }
  else
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->ResetError();
}

bool CVTView::AddToExChain(CTreeItem* itemOver)
{
  LavaDECL *decl, *de;
  if ((collectParent == (CTreeItem*)itemOver->parent())
      && EnableOverride(itemOver)) {
    CVTItemData * itd = (CVTItemData*)itemOver->getItemData();
    if (!itd->VTEl_Tree.ElDECL) {
      decl = NewLavaDECL();
      de = GetDocument()->IDTable.GetDECL(itd->VTEl_Tree.VTEl);
      if (!de)
        return false;
      *decl = *de;
      itd->VTEl_Tree.ElDECL = decl;
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

void CVTView::DeleteExChain(bool inSel)
{
  CHETVElem* El = (CHETVElem*)myDECL->VElems.VElems.first;
  while (El) {
    El->data.ElDECL = 0;
    El = (CHETVElem*)El->successor;
  }
  CTreeItem* item = (CTreeItem*)Tree->RootItem;
  if (CollectDECL) {
    delete CollectDECL;
    CollectDECL = 0;
  }
}

void CVTView::setSelPost(QTreeWidgetItem* selItem)
{
  Tree->ResetSelections();
  Tree->setCurAndSel(selItem);
}


void CVTView::OnSelchanged(QTreeWidgetItem* selItem, QTreeWidgetItem*)
{
  bool ok = true;
  CTreeItem *itemOver = lastCurrent;// *selItem = (CTreeItem*)Tree->currentItem();
  if (!selItem)
    return;
  if (this == (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView())
    SetVTError((CTreeItem*)selItem);
  if (itemOver && (Tree->withControl && !Tree->withShift || Tree->withShift && !Tree->withControl)) {
    if (!CollectDECL) {
      CollectDECL = NewLavaDECL();
      collectParent = (CTreeItem*)itemOver->parent();
    }
    if (itemOver && AddToExChain(itemOver)) {
      if (Tree->withShift) {
        while (itemOver && (itemOver != selItem)) {
          itemOver = (CTreeItem*)itemOver->nextSibling();//Tree->GetNextSiblingItem(itemOver);
          if (itemOver && !AddToExChain(itemOver))
            itemOver = 0;

        }
        if (!itemOver || !CollectDECL->NestedDecls.first || (itemOver != selItem)) {
          multiSelectCanceled = true;
          DeleteExChain(false);
          Tree->withShift = false;
          QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_setSel, (void*)selItem));
          /*Tree->ResetSelections();
          Tree->setCurAndSel(selItem);*/
        }
      }
      else { //Control
        itemOver = (CTreeItem*)selItem;
        if (itemOver && !AddToExChain(itemOver)) {
          multiSelectCanceled = true;
          DeleteExChain(false);
          Tree->withControl = false;
          QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_setSel, (void*)selItem));
          /*Tree->ResetSelections();
          Tree->setCurAndSel(selItem);*/
        }
      }
    }
    else {
      multiSelectCanceled = true;
      DeleteExChain(false);
      Tree->withShift = false;
      Tree->withControl = false;
      QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_setSel, (void*)selItem));
      /*Tree->ResetSelections();
      Tree->setCurAndSel(selItem);*/
    }
  }
  else {
    if (!itemOver)
      DeleteExChain(false);
  }
  lastCurrent = (CTreeItem*)selItem;
  CVTItemData * itd = (CVTItemData*)lastCurrent->getItemData();
  if (itd) {
    if (itd->type == TNodeType_Class) {
      CTreeItem* child = (CTreeItem*)lastCurrent->child(0);
      if (((CVTItemData*)child->getItemData())->type == TNodeType_VT)
        currentBrType = findInterfaceVTs;
      else
        currentBrType = findInterfaceFeatures;
      currentBaseID = itd->VTEl_Tree.VTClss;
    }
    else {
      currentBaseID = itd->VTEl_Tree.VTBaseEl;
      currentBrType = findBaseTID;
    }
  }
  if (active)
    wxTheApp->updateButtonsMenus();
}

void CVTView::OnActivateView(bool bActivate, wxView *deactiveView)
{
  CTreeItem* sel;
  if (GetDocument()->mySynDef)
    if (bActivate) {
      active = true;
      sel = (CTreeItem*)Tree->currentItem();
      SetVTError(sel);
      if (!Tree->hasFocus())
        Tree->setFocus();
      wxTheApp->updateButtonsMenus();
    }
    else {
      active = false;
      DisableActions();
    }
}

void CVTView::DisableActions()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->gotoDeclAction->setEnabled(false);
  frame->overrideAction->setEnabled(false);
  frame->gotoImplAction->setEnabled(false);
}


QString CVTView::text(const QPoint &point) {
  CTreeItem *item=(CTreeItem*)Tree->itemAt(point);
  CVTItemData *itd=(CVTItemData*)item->getItemData();
  LavaDECL *decl;

  Tree->setCurAndSel(item);

  if (itd)
    switch (itd->type) {
    case TNodeType_Class:
      decl = ((CLavaBaseDoc*)Tree->lavaView->GetDocument())->IDTable.GetDECL(itd->VTEl_Tree.VTClss);
      if (decl->DeclType == Package)
        return QString(QObject::tr("<p>This is a <a href=\"../Packages.htm#packages\">package</a></p>"));
      else
        return QString(QObject::tr("<p>This is the <a href=\"../SepItfImpl.htm\">interface</a> of a <b><i><font color=red>Lava</font></i></b> class</p>"));
    case TNodeType_VT:
      return QString(QObject::tr("<p>This is a <a href=\"../PatternsFrameworks.htm#VT\">virtual type</a></p>"));
    case TNodeType_Feature:
      decl = ((CLavaBaseDoc*)Tree->lavaView->GetDocument())->IDTable.GetDECL(itd->VTEl_Tree.VTEl);
      if (decl->DeclType == Function)
        return QString(QObject::tr("<p>This is a member function of a <b><i><font color=red>Lava</font></i></b> <a href=\"../SepItfImpl.htm\">class</a></p>"));
      else if (decl->DeclType == Attr)
        return QString(QObject::tr("<p>This is a member <a href=\"../ObjectLifeCycle.htm\">variable</a> of a <b><i><font color=red>Lava</font></i></b> <a href=\"../SepItfImpl.htm\">class</a></p>"));
      break;
    }
  return QString(QObject::tr("<p>No specific help available for this tree node</p>"));
}

bool CVTView::eventFilter(QObject *, QEvent *ev) {
  QHelpEvent *hev;
  QWhatsThisClickedEvent *wtcEv;
  QString href;

  if (ev->type() == QEvent::WhatsThis) {
    hev = (QHelpEvent*)ev;
    setWhatsThis(text(hev->pos()));
    return QWidget::event(ev);
  }
  else if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    href = wtcEv->href();
    ShowPage(QString("whatsThis/")+href);
    return true;
  }
  else
    return false;
}
