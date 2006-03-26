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

#include "LavaBaseStringInit.h"
#include "LavaPEStringInit.h"
#include "LavaPEView.h"
#include "VTView.h"
#include "LavaPEFrames.h"
#include "ExecView.h"
#include "ExecTree.h"
#include "DString.h"
#include "Comment.h"
#include "SynIO.h"
#include "LavaPEWizard.h"
#include "Boxes.h"
#include "FindRefsBox.h"
#include "docview.h"
#include "qlineedit.h"
#include "qobject.h"
#include "qstring.h"
#include "qstatusbar.h"
#include "qcheckbox.h"
//Added by qt3to4:
#include <QPixmap>
#include <QDragLeaveEvent>
#include <QEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDrag>
#include <QHeaderView>
#include "cmainframe.h"
#include "qpixmapcache.h"
#include "qclipboard.h"
#include "q3dragobject.h"
#include "qdatastream.h"
#include "qmessagebox.h"
#include "QtAssistant/qassistantclient.h"
#include <QTreeWidget>
#include "q3header.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CLavaPEView


static int ContainTab [FormDef+1] [DragIO+1] = {
  //ContainTab[type1, type2] = 0 type1 contains no type2,
  //                         = 1 type1 may contain some type2

               // has
               //types,Int,Im,Ini, Fu,Pack,Comp,CSpc,CpOb,For,Ali, VT,Bas,Att,IAt,OAt,Cns,FTx, PD, Req,Ens,DP, DD, DF,DFF,DIO
  /*NoDef*/      {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 }, //NoDef
  /*Interface*/  {  1,  1,  0,  0,  1,  0,  0,   0,   0,   1,  1,  1,  0,  1,  0,  0,  1,  0,  1,  0,  0,  1,  1,  1,  1, 1 }, //Interface
  /*Impl*/       {  1,  1,  1,  0,  1,  0,  0,   0,   0,   1,  1,  0,  0,  1,  0,  0,  1,  0,  0,  0,  0,  1,  1,  1,  1, 1 }, //Impl
  /*Initiator*/  {  1,  1,  1,  1,  0,  0,  0,   1,   0,   1,  1,  1,  0,  0,  1,  0,  1,  0,  1,  0,  0,  1,  1,  1,  1, 1 }, //Initiator
  /*Function*/   {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  1,  1,  0,  0,  1,  0, 1 }, //Function
  /*Package*/    {  1,  1,  1,  1,  0,  1,  0,   1,   0,   1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  1,  0,  0, 0 }, //Package
  /*Component*/  {  1,  1,  1,  1,  0,  1,  0,   1,   1,   1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  1,  0,  0, 0 }, //Component
  /*CompObjSpec*/{  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 }, //CompObjSpec
  /*CompObj*/    {  1,  1,  1,  1,  0,  1,  0,   1,   0,   1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  1,  0,  0, 0 }, //CompObj
  /*FormDef*/    {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  1,  0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0, 0 }, //FormDef
};
/*
   Note: Interfaces with Component flag and/or notification flag have no input, output and invariant.
         Functions have no fields (DeclType = Attr) and no invariant.
*/


void CMainItemData::Serialize(QDataStream& ar)
{
  if (ar.device()->isWritable()) {
    ar << (quint64)type;
    ar << (quint64)((LavaDECL*)synEl)->DeclType;
    ar << (quint64)((LavaDECL*)synEl)->TreeFlags.bits;
    ar << (quint64)((LavaDECL*)synEl)->SecondTFlags.bits;
    ar << docPathName->c;
    ASN1tofromAr* cid = new ASN1tofromAr(&ar);
    cid->Release = RELEASE;
    CDPLavaDECL(PUT, cid, (address)synEl, false);
    CDPClipSyntaxDefinition(PUT, cid, (address)ClipTree, false);
    delete cid;
  }
  else {
    quint64 wt;
    char* str;
    ar >> wt;
    type = (TIType)wt;
    ar >> wt;
    ar >> wt;
    ar >> wt;
    ar >> str;
    docPathName = new DString(str);
    synEl = (unsigned long) NewLavaDECL();
    ASN1tofromAr* cid = new ASN1tofromAr(&ar);
    cid->Release = RELEASE;
    CDPLavaDECL(GET, cid, (address)synEl, false);
    ClipTree = new SyntaxDefinition;
    CDPClipSyntaxDefinition(GET, cid, (address)ClipTree, false);
    delete cid;
  }
}

TDeclType CMainItemData::Spick( QByteArray& ar, SynFlags& treeflags, SynFlags& secondtflags)
{
  QDataStream stream(ar/*, QIODevice::ReadOnly*/);
  quint64 wt;
  char* str;
  TDeclType defType;
  stream >> wt;
  type = (TIType)wt;
  stream >> wt;
  defType = (TDeclType)wt;
  stream >> wt;
  treeflags.bits = (unsigned long int)wt;
  stream >> wt;
  secondtflags.bits = (unsigned long int)wt;
  stream >> str;
  docPathName = new DString(str);
  return (TDeclType)defType;
}

void CMainItemData::Destroy()
{
  if (ClipTree)
    delete ClipTree;
  if (synEl)
    delete (LavaDECL*)synEl;
}

CMainItemData::~CMainItemData()
{
  if (docPathName)
    delete docPathName;
}




const char * LavaSource::format (int i) const
{
  if (i==0)
    return "QLavaTreeView";
  else
    return 0;
}


CLavaPEView::CLavaPEView(QWidget* parent, wxDocument *doc)
: CTreeView(parent, doc, "LavaPEView")
{
  myDECL = 0;
  myFormView = 0;
  myVTView = 0;
  myInclView = 0;
  InitFinished = false;
  ItemSel = 0;
  DataSel = 0;
  QSize sz = size();
//  ParItemSel = 0;
//  ParDataSel = 0;
  lastCurrent = 0;
  m_nIDClipFormat = "QLavaTreeView";
  CollectDECL = 0;
  m_hitemDrag = 0;
  m_hitemDrop = 0;
  updateTree = false;
  drawTree = false;
  inSync = false;
  Clipdata = 0;
  clipboard_lava_notEmpty = false;
  DropPosted = false;

  if (!GetDocument()->MainView)
    GetDocument()->MainView = this;
  connect(m_tree,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(OnSelchanged(QTreeWidgetItem*, QTreeWidgetItem*)));
//  connect(m_tree,SIGNAL(itemSelectionChanged()), SLOT(OnSelchanged()));
  connect(m_tree,SIGNAL(doubleClicked( const QModelIndex &)), SLOT(OnDblclk( const QModelIndex &)));
  //connect(m_tree,SIGNAL(rightButtonClicked(QListViewItem*)), SLOT(OnRclick(QListViewItem*)));
  connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem*)), SLOT(OnItemexpanded(QTreeWidgetItem*)));
  connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem*)), SLOT(OnItemcollapsed(QTreeWidgetItem*)));

  if (!GetDocument()->MainView)
    GetDocument()->MainView = this;
//  if (LBaseData->m_lfDefTreeFont.lfHeight != 0)
  setFont(LBaseData->m_TreeFont);
  m_tree->setAcceptDrops(true);
  sz = size();
}


void CLavaPEView::CleanListView()
{
  delete m_tree;
  m_tree = new MyListView(this);
  layout->addWidget(m_tree);
  new TreeWhatsThis(m_tree);
	//setFocusProxy(m_tree);
//  m_tree->setSorting(-1);
  m_tree->setColumnCount(1);
  m_tree->setRootIsDecorated(true);
  m_tree->header()->hide();
  m_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);//Single);
  connect(m_tree,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT    (OnSelchanged(QTreeWidgetItem*, QTreeWidgetItem*)));
//  connect(m_tree,SIGNAL(itemSelectionChanged()), SLOT(OnSelchanged()));
  connect(m_tree,SIGNAL(doubleClicked(const QModelIndex&)), SLOT(OnDblclk(const QModelIndex&)));
  //connect(m_tree,SIGNAL(rightButtonClicked(QListViewItem*)), SLOT(OnRclick(QListViewItem*)));
  connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem*)), SLOT(OnItemexpanded(QTreeWidgetItem*)));
  connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem*)), SLOT(OnItemcollapsed(QTreeWidgetItem*)));

  m_tree->setAcceptDrops(true);
  m_tree->show();
  if (wxDocManager::GetDocumentManager()->GetActiveView() == this)
    m_tree->setFocus();
}

CLavaPEView::~CLavaPEView()
{
	setFocusProxy(0);
  destroy();
}


bool CLavaPEView::AddToDragChain(CTreeItem* itemDrag, bool vkControl, bool sameCat)
{
  CTreeItem *itemP, *itemPP = 0;
  CMainItemData *itdP = 0, *itd, *itdPP = 0;
  LavaDECL *itemDECL, *refDECL, *El;
  CHE *che, *cheIO, *cheTree, *cheCol;
  bool canComponent;
  CHEEnumSelId* cheId;

  if (itemDrag) {
    itd = (CMainItemData*)itemDrag->getItemData();
    itemP = (CTreeItem*)itemDrag->parent();
    if (itemP) {
      itdP = (CMainItemData*)itemP->getItemData();
      itemPP = (CTreeItem*)itemP->parent();
    }
    if (itemPP)
      itdPP = (CMainItemData*)itemPP->getItemData();
    if (itd && (itd->type == TIType_DECL)) {
      itemDECL = *(LavaDECL**)itd->synEl;
      if (CollectDECL->DeclType == NoDef) {
        CollectDECL->DeclDescType = StructDesc;
        if (itemP)
          itemPP = (CTreeItem*)itemP->parent();
        if (itemPP)
          itdPP = (CMainItemData*)itemPP->getItemData();
        if (itemDrag == m_tree->RootItem)
          CollectDECL->DeclType = DragDef;
        else {
          if (itemDECL->DeclType == FormText)
            CollectDECL->DeclType = DragFText;
          else
            switch (itdP->type) {
            case TIType_VTypes:
              CollectDECL->DeclType = DragParam;
              break;
            case TIType_Defs:
              CollectDECL->DeclType = DragDef;
              CollectDECL->TreeFlags.INCL(mixedDrag);
              break;
            case TIType_Features:
              if (!CollectDECL->TreeFlags.Contains(mixedDrag))
                CollectDECL->DeclType = DragFeature;
              break;
            case TIType_Output:
            case TIType_Input:
              CollectDECL->DeclType = DragIO;
              break;
            }
        }
      }
      else
        if ( CollectDECL->NestedDecls.first
            && (itemDECL->ParentDECL
              != ((LavaDECL*)((CHE*)CollectDECL->NestedDecls.first)->data)->ParentDECL))
          return false;
      if (CollectDECL->DeclType == NoDef)
        return false;
      if (vkControl && !sameCat)
        CollectDECL->TreeFlags.INCL(mixedDrag);
      El = NewLavaDECL();
      che = NewCHE(El);
      *El = *itemDECL;
      if (El->DeclType == VirtualType)
        CollectDECL->TreeFlags.INCL(dragVT);
      else if (El->DeclType == Interface)
        CollectDECL->TreeFlags.INCL(dragInt);
      else if (El->DeclType == Impl)
        CollectDECL->TreeFlags.INCL(dragImpl);
      else if (El->DeclType == Initiator)
        CollectDECL->TreeFlags.INCL(dragIni);
      else if ((El->DeclType == Function) || (El->DeclType == Attr)) {
        if (El->NestedDecls.last && (((LavaDECL*)((CHE*)El->NestedDecls.last)->data)->DeclType == ExecDef))
          CollectDECL->TreeFlags.INCL(dragFuncImpl);
        else
          CollectDECL->TreeFlags.INCL(dragFuncDef);
        if (El->SecondTFlags.Contains(overrides))
          CollectDECL->TreeFlags.INCL(dragOverrides);
      }
      else if (El->DeclType == Package)
        CollectDECL->TreeFlags.INCL(dragPack);
      else if (El->DeclType == Component)
        CollectDECL->TreeFlags.INCL(dragCompo);
      else if (El->DeclType == CompObjSpec)
        CollectDECL->TreeFlags.INCL(dragCOS);
      else if (El->DeclType == CompObj)
        CollectDECL->TreeFlags.INCL(dragCO);
      else if (El->DeclType == IAttr)
        CollectDECL->TreeFlags.INCL(dragInput);
      if (itemDECL->ParentDECL) {
        cheTree = (CHE*)itemDECL->ParentDECL->NestedDecls.first;
        cheCol = (CHE*)CollectDECL->NestedDecls.first;
        while (cheCol) {
          while (cheTree && ((LavaDECL*)cheCol->data)->OwnID != ((LavaDECL*)cheTree->data)->OwnID)
            if (((LavaDECL*)cheTree->data)->OwnID == ((LavaDECL*)che->data)->OwnID) {
              if (!cheCol->predecessor) {
                CollectPos = GetPos(itemDrag, 0);
                if ((CollectDECL->DeclType != PatternDef) && (itemDECL->DeclType == VirtualType))
                  CollectDECL->DeclType = DragParam;
              }
              CollectDECL->NestedDecls.Insert(cheCol->predecessor, che);
              cheTree = 0;
            }
            else
              cheTree = (CHE*)cheTree->successor;
          if (cheTree)
            cheCol = (CHE*)cheCol->successor;
          else
            cheCol = 0;
        }
        if (cheTree)
          CollectDECL->NestedDecls.Append(che);
      }
      else
        CollectDECL->NestedDecls.Append(che);
      if ((CollectDECL->DeclType != PatternDef)
        && ((itemDECL->DeclType == Function) || (itemDECL->DeclType == Attr))) {
        if (!CollectDECL->TreeFlags.Contains(mixedDrag))
          CollectDECL->DeclType = DragFeature;
      }
      if ((CollectDECL->DeclType == DragFeature) && (itemDECL->DeclType == Function)) {
        CollectDECL->DeclType = DragFeatureF;
        if (itemDECL->ParentDECL->DeclType == Impl)
          CollectDECL->SecondTFlags.INCL(funcImpl);
      }
      canComponent = !CollectDECL->SecondTFlags.Contains(funcImpl);
      if (canComponent &&
          ((CollectDECL->DeclType == DragFeature) || (CollectDECL->DeclType == DragFeatureF))) {
        canComponent = !itemDECL->ParentDECL || !itemDECL->ParentDECL->NestedDecls.first || CollectDECL->SecondTFlags.Contains(inComponent);
        if (canComponent) {
          if (itemDECL->DeclType == Function) {
            cheIO = (CHE*)itemDECL->NestedDecls.first;
            while (cheIO && canComponent) {
              refDECL = GetDocument()->IDTable.GetDECL(((LavaDECL*)((CHE*)cheIO)->data)->RefID);
              canComponent = refDECL && ((refDECL->DeclType != Interface) || refDECL->TypeFlags.Contains(isComponent));
              cheIO = (CHE*)cheIO->successor;
            }
          }
          else {
            refDECL = GetDocument()->IDTable.GetDECL(itemDECL->RefID);
            canComponent = refDECL && ((refDECL->DeclType != Interface) || refDECL->TypeFlags.Contains(isComponent));
          }
        }
        if (canComponent)
          CollectDECL->SecondTFlags.INCL(inComponent);
        else
          CollectDECL->SecondTFlags.EXCL(inComponent);
      }
      CanDelete = CanDelete && EnableDelete(itemDECL);
//      SetAllStates(itemDrag, nState, nStateMask, false);
      return true;
    }//TITypeDECL
    else {
      if (itd && (itd->type == TIType_CHEEnumSel)) {
        cheId = (CHEEnumSelId*)itd->synEl;
        if (CollectDECL->DeclType == NoDef) {
          CollectDECL->DeclType = DragEnum;
          itemP = (CTreeItem*)itemP->parent();
          itdP = (CMainItemData*)itemP->getItemData();
          CollectDECL->ParentDECL = *(LavaDECL**)itdP->synEl;
          CollectDECL->DeclDescType = EnumType;
          CollectDECL->EnumDesc.ptr = new TEnumDescription;
          ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.DeclDescType = BasicType;
          ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.BType = Enumeration;
        }
        else
          if ((CollectDECL->DeclType != DragEnum)
              || (CollectDECL->ParentDECL != *(LavaDECL**)itdP->synEl))
            return false;
        CHEEnumSelId* newId = new CHEEnumSelId;
        newId->data = cheId->data;
        ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.Items.Append(newId);
//        SetAllStates(itemDrag, nState, nStateMask, false);
        return true;
      }
    }
  }
  return false;
}

CTreeItem* CLavaPEView::BrowseTree(LavaDECL* decl, CTreeItem* startItem, DString* enumID)
{
  CTreeItem* item;
  if (!startItem)
    return NULL;
  CMainItemData *itd = (CMainItemData*)startItem->getItemData();
  if (itd && (itd->type == TIType_DECL) && (decl == *(LavaDECL**)itd->synEl))
    return startItem;
  CTreeItem* topItem = (CTreeItem*)startItem->child(0);
  while (topItem) {
    item = (CTreeItem*)topItem->child(0);
    while (item) {
      itd = (CMainItemData*)item->getItemData();
      if (decl == *(LavaDECL**)itd->synEl)
        if (enumID && enumID->l && decl->DeclDescType == EnumType) {
          item = (CTreeItem*)item->child(0);
          item = (CTreeItem*)item->child(0);
          while (item) {
            itd = (CMainItemData*)item->getItemData();
            if (((CHEEnumSelId*)itd->synEl)->data.Id == *enumID)
              return item;
            item = (CTreeItem*)item->nextSibling();
          }
        }
        else
          return item;
      else
        if (decl->isInSubTree(*(LavaDECL**)itd->synEl))
          return BrowseTree(decl, item, enumID);
      item = (CTreeItem*)item->nextSibling();
    }
    topItem = (CTreeItem*)topItem->nextSibling();
  }
  return NULL;
}


TIType CLavaPEView::CanPaste (TDeclType defType, SynFlags treeFlags, SynFlags secondtflags, CTreeItem* toItem)
{
  if (toItem == m_tree->RootItem)
    return TIType_NoType;
  CMainItemData *pardata, *toData;
  CTreeItem* parItem=0;
  LavaDECL *ppdecl;

  toData = (CMainItemData*)toItem->getItemData();
  if (defType == DragFText) {
    if (!myInclView && (toItem != m_tree->RootItem))
      return TIType_Features;
    else
      return TIType_NoType;
  }
  else
    if (!myInclView)
      return TIType_NoType;
  if (defType == DragEnum) {
    if ((toData->type == TIType_EnumItems)
        || (toData->type == TIType_CHEEnumSel))
      return TIType_CHEEnumSel;
    else
      return TIType_NoType;
  }
  else {
    if (toData->type == TIType_DECL) {
      parItem = (CTreeItem*)toItem->parent();
      toData = (CMainItemData*)parItem->getItemData();
      parItem = (CTreeItem*)parItem->parent();
    }
    else
      if ((toData->type == TIType_EnumItems)
          || (toData->type == TIType_CHEEnumSel))
        return TIType_NoType;
      else
        parItem = (CTreeItem*)toItem->parent();
    if (!parItem)
      return TIType_NoType;
    pardata = (CMainItemData*)parItem->getItemData();
    ppdecl = *(LavaDECL**)pardata->synEl;
    if (!ContainTab[ppdecl->DeclType] [defType])
      return TIType_NoType;
    if ((ppdecl->DeclType == Function)
      && ( ppdecl->SecondTFlags.Contains(overrides) || ppdecl->SecondTFlags.Contains(funcImpl)
         || ppdecl->TypeFlags.Contains(isPropGet) || ppdecl->TypeFlags.Contains(isPropSet)
         || (ppdecl->op != OP_noOp)))
    return TIType_Refac; //TIType_NoType;
    switch (defType) {
      case DragEnum:
        break;
      case DragDef:
      case DragParam:
        if ((defType == DragDef) && (toData->type != TIType_Defs))
          return TIType_NoType;
        else
          if ((defType == DragParam) && (toData->type != TIType_VTypes))
            return TIType_NoType;
        if (treeFlags.Contains(dragVT) && !ContainTab[ppdecl->DeclType] [VirtualType])
          return TIType_NoType;
        if (treeFlags.Contains(dragInt) && !ContainTab[ppdecl->DeclType] [Interface])
          return TIType_NoType;
        if (treeFlags.Contains(dragImpl) && !ContainTab[ppdecl->DeclType] [Impl])
          return TIType_NoType;
        if (treeFlags.Contains(dragIni) && !ContainTab[ppdecl->DeclType] [Initiator])
          return TIType_NoType;
        if (treeFlags.Contains(dragFuncDef) && !ContainTab[ppdecl->DeclType] [Function])
          return TIType_NoType;
        if (treeFlags.Contains(dragFuncImpl)
            && (!ContainTab[ppdecl->DeclType] [Function]
           || (ppdecl->DeclType == Interface) || (ppdecl->DeclType == Package)))
          return TIType_NoType;
        if (treeFlags.Contains(dragPack) && !ContainTab[ppdecl->DeclType] [Package])
          return TIType_NoType;
        if (treeFlags.Contains(dragCompo) && !ContainTab[ppdecl->DeclType] [Component])
          return TIType_NoType;
        if (treeFlags.Contains(dragCOS) && !ContainTab[ppdecl->DeclType] [CompObjSpec])
          return TIType_NoType;
        if (treeFlags.Contains(dragCO) && !ContainTab[ppdecl->DeclType] [CompObj])
          return TIType_NoType;
        if (treeFlags.Contains(dragInput) && !ContainTab[ppdecl->DeclType] [IAttr])
          return TIType_NoType;
        break;
      case DragFeatureF:
        if (toData->type != TIType_Features)
          return TIType_NoType;
        if (secondtflags.Contains(funcImpl))
          if (ppdecl->DeclType != Impl)
            return TIType_Refac; //TIType_NoType;
          /*
          if ((ppdecl->DeclType == Interface)
              && ppdecl->TypeFlags.Contains(isComponent)
              && !secondtflags.Contains(inComponent))
            return TIType_NoType;
              */
        break;
      case DragFeature:
      case DragIO:
        if( (toData->type != TIType_Features)
              && (toData->type != TIType_Input)
              && (toData->type != TIType_Output))
          return TIType_NoType;
        /*
        if ((ppdecl->DeclType == Interface)
            && ppdecl->TypeFlags.Contains(isComponent)
            && !secondtflags.Contains(inComponent))
          return TIType_NoType;
          ??wozu war das da??*/

        break;
  //    case ExecDef:
  //      if (toData->type != TIType_Exec)
  //        return TIType_NoType;
  //      break;
    }
    return toData->type;
  }

}


CLavaPEHint* CLavaPEView::ChangeEnum(LavaDECL* clipDECL, CTreeItem* item, bool cut, bool paste)
{
  //makes the CPECommand_Change-hint for a cut, paste and drag and drop operation with enum-items
  CMainItemData* itemData = (CMainItemData*)item->getItemData();
  CTreeItem* itemP = (CTreeItem*)item->parent();
  LavaDECL** poldDECL;
  CHEEnumSelId* relEl, *afterElem;
  if (itemData->type == TIType_EnumItems) {
    poldDECL = (LavaDECL**)itemData->synEl;
    afterElem = 0;
  }
  else {
    CMainItemData* itemPData = (CMainItemData*)itemP->getItemData();
    poldDECL = (LavaDECL**)itemPData->synEl;
    afterElem = (CHEEnumSelId*)itemData->synEl;
  }
  if ((*poldDECL)->DeclDescType != EnumType)
    return 0;
  LavaDECL* newDECL = NewLavaDECL();
  *newDECL = **poldDECL;

  CHAINANY* newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
  relEl = (CHEEnumSelId*)newItems->first;
  if (afterElem) {
    while (relEl && (relEl->data.Id != afterElem->data.Id))
      relEl = (CHEEnumSelId*)relEl->successor;
    afterElem = relEl;
  }
  CHAINANY* clipItems = &((TEnumDescription*)clipDECL->EnumDesc.ptr)->EnumField.Items;
  CHEEnumSelId* clipEl = (CHEEnumSelId*)clipItems->first;
  while (clipEl) {
    clipEl = (CHEEnumSelId*)clipItems->Uncouple(clipEl);
    if (cut) {
      relEl = (CHEEnumSelId*)newItems->first;
      while (relEl && (relEl->data.Id != clipEl->data.Id))
        relEl = (CHEEnumSelId*)relEl->successor;
      if (relEl) {
        relEl = (CHEEnumSelId*)newItems->Uncouple(relEl);
        if (relEl) {
          if (relEl == afterElem)
            afterElem = (CHEEnumSelId*)afterElem->predecessor;
          delete relEl;
        }
      }
    }
    if (paste) {
      relEl = (CHEEnumSelId*)newItems->first;
      while (relEl) {
        while (relEl && (relEl->data.Id != clipEl->data.Id))
          relEl = (CHEEnumSelId*)relEl->successor;
        if (relEl) {
          clipEl->data.Id.Insert(DString("CopyOf_"), 0);
          relEl = (CHEEnumSelId*)newItems->first;
        }
      }
      newItems->Insert(afterElem, clipEl);
      afterElem = clipEl;
    }
    clipEl = (CHEEnumSelId*)clipItems->first;
  }
  if (afterElem)
    afterElem->data.selItem = true;
  DString* str2 = new DString((*poldDECL)->FullName);
  newDECL->WorkFlags.INCL(selEnum);
  newDECL->TreeFlags.INCL(ItemsExpanded);
  return new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)3, (DWORD)newDECL, (DWORD)str2, 0, (DWORD)poldDECL);

}

void CLavaPEView::CheckAutoCorr(LavaDECL* decl)
{
  CHE *che = (CHE*)decl->DECLError1.first;
  while (che) {
    if (((CLavaError*)che->data)->showAutoCorrBox)  {
      //PostMessage(MESS_AUTOCORR, decl->OwnID);
      GetDocument()->AutoCorr(decl);
      return;
    }
    if (che == decl->DECLError1.last)
      che = (CHE*)decl->DECLError2.first;
    else
      che = (CHE*)che->successor;
  }
}

void CLavaPEView::CorrVT_BaseToEx(LavaDECL *dropParent, LavaDECL *dragParent, LavaDECL *clipDecl, bool& mdh)
{
  CLavaPEDoc *mDoc;
  TID mId;
  int mINCL = 0;
  TIDType type;
  LavaDECL *newmDecl, **p_mDecl, *mDecl;
  CHETVElem* cheElVT;
  DString *str2, absName;
  SynFlags firstlast;
  CHETID *cheID;
  CLavaPEHint *hint;

  cheElVT = GetDocument()->IDTable.FindElemInVT(dropParent, dragParent, clipDecl, DragINCL);
  if (TID(clipDecl->OwnID, DragINCL) == cheElVT->data.VTEl)
    return;
  mId = cheElVT->data.VTEl;
  mDecl = GetDocument()->IDTable.GetDECL(mId, mINCL);
  while (!GetDocument()->IDTable.EQEQ(mId, mINCL, TID(clipDecl->OwnID, DragINCL),0)) {
    mDecl = GetDocument()->IDTable.GetDECL(mId, mINCL);
    mId = ((CHETID*)mDecl->Supports.first)->data;
    mINCL = mDecl->inINCL;
  }
  if (mDecl->inINCL) {
    absName = GetDocument()->IDTable.IDTab[mDecl->inINCL]->FileName;
    AbsPathName(absName, GetDocument()->IDTable.DocDir);
    mDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(absName.c);
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      mdh = true;
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
    }
  }
  else
    mDoc = GetDocument();
  p_mDecl = (LavaDECL**)mDoc->IDTable.GetVar(TID(mDecl->OwnID, 0),type, 0);
  newmDecl = NewLavaDECL();
  *newmDecl = *(*p_mDecl);
  if (clipDecl->Supports.first) {
    ((CHETID*)newmDecl->Supports.first)->data.nID = ((CHETID*)clipDecl->Supports.first)->data.nID;
    for (mINCL = 0;
         mINCL < GetDocument()->IDTable.IDTab[mDecl->inINCL]->maxTrans;
         mINCL++)
      if (GetDocument()->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].isValid
         && (GetDocument()->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].nINCL == DragINCL)) {
        ((CHETID*)newmDecl->Supports.first)->data.nINCL = mDoc->IDTable.IDTab[mINCL]->nINCLTrans[((CHETID*)clipDecl->Supports.first)->data.nINCL].nINCL;
        mINCL = GetDocument()->IDTable.IDTab[mDecl->inINCL]->maxTrans;
      }
  }
  else {
    cheID = new CHETID;
    cheID->data = cheElVT->data.VTEl;
    clipDecl->Supports.Append(cheID);
    clipDecl->WorkFlags.INCL(SupportsReady);
    clipDecl->SecondTFlags.INCL(overrides);
    newmDecl->Supports.Destroy();
  }
  str2 = new DString(newmDecl->FullName);  //bisheriger Name
  FIRSTLAST(mDoc, firstlast);
  hint = new CLavaPEHint(CPECommand_Change, mDoc, firstlast, (DWORD)newmDecl, (DWORD)str2, 0, (DWORD)p_mDecl);
  mDoc->UndoMem.AddToMem(hint);
  mDoc->UpdateDoc(this, false, hint);
}

void CLavaPEView::CorrVT_ExToBase(LavaDECL *dragParent, LavaDECL *dropParent, LavaDECL *clipDecl, bool& mdh, CHE*& vtHints)
{
  CHETVElem* cheElVT;
  DString *str2, absName;
  TIDType type;
  LavaDECL *newmDecl, **p_mDecl, *mDecl, *dropParentInDragDoc;
  CLavaPEDoc *mDoc;
  TID mId;
  int mINCL;
  SynFlags firstlast;
  CHETID *cheID;
  CHE* cheHint;
  CLavaPEHint *mHint;

  dropParentInDragDoc = DragDoc->IDTable.GetDECL(0, dropParent->OwnID, DropINCL);
  cheElVT = DragDoc->IDTable.FindElemInVT(dropParentInDragDoc, dragParent, clipDecl, 0);
  if (cheElVT && ((CHETID*)clipDecl->Supports.first)->data == cheElVT->data.VTEl)
    return;

  if (cheElVT) {
    mId = ((CHETID*)clipDecl->Supports.first)->data;
    mINCL = 0;
    while (!DragDoc->IDTable.EQEQ(mId, mINCL, cheElVT->data.VTEl,0)) {
      mDecl = DragDoc->IDTable.GetDECL(mId, mINCL);
      mId = ((CHETID*)mDecl->Supports.first)->data;
      mINCL = mDecl->inINCL;
    }
  }
  else {
    mDecl = clipDecl;
    while (mDecl->Supports.first)
      mDecl = DragDoc->IDTable.GetDECL(((CHETID*)mDecl->Supports.first)->data, mDecl->inINCL);
  }
  if (mDecl->inINCL) {
    absName = DragDoc->IDTable.IDTab[mDecl->inINCL]->FileName;
    AbsPathName(absName, DragDoc->IDTable.DocDir);
    mDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(absName.c);
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      mdh = true;
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
    }
  }
  else
    mDoc = DragDoc;
  p_mDecl = (LavaDECL**)mDoc->IDTable.GetVar(TID(mDecl->OwnID, 0),type, 0);
  newmDecl = NewLavaDECL();
  *newmDecl = *(*p_mDecl);
  if (!newmDecl->Supports.first) {
    cheID = new CHETID;
    newmDecl->Supports.Append(cheID);
    newmDecl->SecondTFlags.INCL(overrides);
  }
  ((CHETID*)newmDecl->Supports.first)->data.nID = clipDecl->OwnID; //the new OwnID is not yet available
  for (mINCL = 0;
       mINCL < DragDoc->IDTable.IDTab[mDecl->inINCL]->maxTrans;
       mINCL++)
    //DropDoc in mDoc
    if (DragDoc->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].isValid
       && (DragDoc->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].nINCL == DropINCL)) {
      ((CHETID*)newmDecl->Supports.first)->data.nINCL = mINCL;
      mINCL = DragDoc->IDTable.IDTab[mDecl->inINCL]->maxTrans;
    }

  str2 = new DString(newmDecl->FullName);
  mHint = new CLavaPEHint(CPECommand_Change, mDoc, firstlast, (DWORD)newmDecl, (DWORD)str2, 0, (DWORD)p_mDecl);
  cheHint = new CHE;
  cheHint->data = (DObject*)mHint;
  cheHint->successor = vtHints;
  vtHints = cheHint;
  if (cheElVT)
    ((CHETID*)clipDecl->Supports.first)->data = cheElVT->data.VTEl;
  else {
    clipDecl->Supports.Destroy();
    clipDecl->SecondTFlags.EXCL(overrides);
  }
}

void CLavaPEView::DeleteDragChain()
{
//  CTreeItem* item = (CTreeItem*)m_tree->child(0);
//  unsigned nStateMask=0;// = TVIS_DROPHILITED; 
//  unsigned nState = 0;
//  SetAllStates(item, nState, nStateMask, false);
  if (CollectDECL) {
    delete CollectDECL;
    CollectDECL = 0;
  }
}

void CLavaPEView::DeleteItemData(CTreeItem* parent)
{
  if (!parent)
    parent = (CTreeItem*)m_tree->RootItem;
  if (parent) {
    CMainItemData * itd = (CMainItemData*)parent->getItemData();
    parent->setItemData(0);
    if (itd)
      delete itd;
    CTreeItem* item = (CTreeItem*)parent->child(0);
    while (item) {
      DeleteItemData(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}


void CLavaPEView::destroy()
{
  if (!wxTheApp->deletingMainFrame)
    DisableActions();
  if (((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext)
    ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext->RemoveView(this);
  DeleteItemData();

  if (myInclView && GetDocument()->MainView) {
    GetDocument()->OnDestroyMainView(this);
    GetDocument()->MainView = 0;
  }
  CTreeView::destroy();
}


void CLavaPEView::DisableActions()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->newEnumAction->setEnabled(false);
  frame->newFuncAction->setEnabled(false);
  frame->newInitiatorAction->setEnabled(false);
  frame->newMemVarAction->setEnabled(false);
  frame->newPackageAction->setEnabled(false);
  frame->newImplAction->setEnabled(false);
  frame->newCOspecAction->setEnabled(false);
  frame->newCOimplAction->setEnabled(false);
  frame->newVTAction->setEnabled(false);
  frame->newInterfaceAction->setEnabled(false);
  frame->newSetAction->setEnabled(false);
  frame->editCutAction->setEnabled(false);
  frame->editCommentAction->setEnabled(false);
  frame->editSelItemAction->setEnabled(false);
  frame->editPasteAction->setEnabled(false);
  frame->openFormViewAction->setEnabled(false);
  frame->makeGUIAction->setEnabled(false);
  frame->gotoDeclAction->setEnabled(false);
  frame->editCopyAction->setEnabled(false);
  frame->showOverridablesAction->setEnabled(false);
  frame->insertTextAction->setEnabled(false);
	frame->newEnumItemAction->setEnabled(false);
	frame->gotoImplAction->setEnabled(false);
	frame->findRefsAction->setEnabled(false);
  frame->expandAction->setEnabled(false);
  frame->collapseAction->setEnabled(false);
  frame->showOptsAction->setEnabled(false);
  frame->showAllOptsAction->setEnabled(false);
  frame->nextCommentAction->setEnabled(false);
  frame->hideEmptyOptsAction->setEnabled(false);
  frame->prevCommentAction->setEnabled(false);
  frame->nextErrorAction->setEnabled(false);
  frame->prevErrorAction->setEnabled(false);
}


bool CLavaPEView::DrawEmptyOpt(CTreeItem* parent, bool down)
{
  CTreeItem *pitem, *collItem, *declItem, *gp, *item = TVI_FIRST;
  CMainItemData *itd, *dataD;
  TIType  ptype = TIType_NoType;
  TDeclType gpt = Impl;
  DString Fields, ExecLabel;
  int bm;
  SynFlags flags;

  flags.INCL(emptyPM);
  itd = (CMainItemData*)parent->getItemData();
  DWORD synEl = itd->synEl;
  TDeclType parentType = (*(LavaDECL**)itd->synEl)->DeclType;
  if ((parentType > FormDef) || (parentType == FormDef) && myInclView)
    return false;
  pitem = (CTreeItem*)parent->child(0);
  if (down) {
    collItem = pitem;
    while (collItem) {
      declItem = (CTreeItem*)collItem->child(0);
      while (declItem) {
        dataD = (CMainItemData*)declItem->getItemData();
        if (dataD->type == TIType_DECL) {
          DrawEmptyOpt(declItem, true);
          (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
          declItem = (CTreeItem*)declItem->nextSibling();
        }
        else
          declItem = 0;
      }
      collItem = (CTreeItem*)collItem->nextSibling();
    }
  }
  if (pitem)
    ptype = ((CMainItemData*)pitem->getItemData())->type;
  if ((ptype != TIType_EnumItems) && ((*(LavaDECL**)synEl)->DeclDescType == EnumType) ) {
    itd = new CMainItemData(TIType_EnumItems, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ItemsExpanded));
    bm = enumBM;//((CLavaPEApp*)wxTheApp)->LavaIcons[enumBM];
    item = InsertItem("Enumeration",  bm, parent, item);
    item->setItemData(itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_EnumItems) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_VTypes) && ContainTab[parentType] [ VirtualType]
      && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent)
      && GetDocument()->IDTable.okPatternLevel(*(LavaDECL**)synEl)) {
    itd = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
    bm = GetPixmap(true, true, VirtualType);
    item = InsertItem("Virtual types",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_VTypes) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Input) && ContainTab[parentType] [ IAttr]
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)
        /*&& ((parentType != Function) || !(*(LavaDECL**)synEl)->SecondTFlags.Contains(funcImpl))*/ ) {
    itd = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
    bm = GetPixmap(true, true, IAttr);
    item = InsertItem("Inputs",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Input) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Output) && ContainTab[parentType] [ OAttr]
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(execIndependent)
        && !(*(LavaDECL**)synEl)->SecondTFlags.Contains(isLavaSignal)
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)
        /*&& ((parentType != Function) || !(*(LavaDECL**)synEl)->SecondTFlags.Contains(funcImpl)) */
        && ((parentType != Interface)
              || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isAbstract)
                && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) ) ) {
    itd = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
    bm = GetPixmap(true, true, OAttr);
    item = InsertItem("Outputs",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Output) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Defs) && ContainTab[parentType] [ 0]) {
    itd = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded) );
    bm = GetPixmap(true, true, Interface);
    item = InsertItem("Declarations",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Defs) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if (parentType == Function)
    Fields = DString("Fields");
  else
    Fields = DString("Features");
  if ((parentType == Initiator) || (parentType == Function))
     /* || (parentType == Impl))*/
    ExecLabel = DString("Exec");
  else
    ExecLabel = DString("Invariant");
  if ((parentType == Function) && (parent != m_tree->RootItem)) {
    gp = (CTreeItem*)parent->parent();
    gp = (CTreeItem*)gp->parent();
    CMainItemData *gpdata = (CMainItemData*)gp->getItemData();
    gpt = (*(LavaDECL**)gpdata->synEl)->DeclType;
  }
  if ((ptype != TIType_Features) && (ContainTab[parentType] [ Attr] || ContainTab[parentType] [ Function] )) {
    if ((parentType != Function) || (gpt != Interface) ) {
      itd = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
      bm = GetPixmap(true, true, Attr);
      item = InsertItem(Fields.c,  bm, parent, item);
      item->setItemData( itd);
      if (!down)
        m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
        //m_tree->ensureItemVisible(item);
    }
  }
  else {
    if (ptype == TIType_Features) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Require) && ContainTab[parentType] [ Require]) {
    itd = new CMainItemData(TIType_Require, synEl);
    bm = GetPixmap(true,true,Require,flags);
    item = InsertItem("Require" ,bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
      //m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Require) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Ensure) && ContainTab[parentType] [ Ensure]) {
    itd = new CMainItemData(TIType_Ensure, synEl);
    bm = GetPixmap(true,true,Ensure,flags);
    item = InsertItem("Ensure" ,bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
//      m_tree->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Ensure) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Exec) && ContainTab[parentType] [ ExecDef]
      && (gpt != Interface)
      && ((parentType != Interface) || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) )) {
    itd = new CMainItemData(TIType_Exec, synEl);
    if ((parentType == Interface) || (parentType == Impl))
      flags.INCL(invariantPM);
    bm = GetPixmap(true,true,ExecDef,flags);
    item = InsertItem(ExecLabel.c ,bm, parent, item);
    item->setItemData( itd);
    if (!down)
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
//      m_tree->ensureItemVisible(item);
  }
  return true;
}

void CLavaPEView::DrawTree(LavaDECL ** pDECL, bool inUndoRedo, bool finalUpdate, int checkLevel)
{
  CTreeItem *selItem, *firstVisible = 0;
  DString str;
  CMainItemData *data;
  bool drawn = drawTree;

  setUpdatesEnabled(false);
  if (drawTree) {
    DeleteItemData();
    CTreeItem* item = (CTreeItem*)m_tree->takeTopLevelItem (0);
    if (m_tree->RootItem)
      delete m_tree->RootItem;
    //CleanListView();
    ItemSel = 0;
    SelItem = 0;
    SelType = NoDef;
  }
  else
    SelItem = (CTreeItem*)m_tree->currentItem();
  CExecTree* execTree = new CExecTree(this, !inUndoRedo, finalUpdate, checkLevel);
  execTree->Travers->FillOut = (pDECL == 0);
  execTree->Travers->DownTree(pDECL,0,str);
  //if (drawTree)
  //  ((CLavaPEApp*)wxTheApp)->debugThread.checkAndSetBrkPnts(GetDocument());
  drawTree = false;
  GetDocument()->UpdateNo++;
  delete execTree;
  if (finalUpdate) {
    lastCurrent = 0;
    ExpandTree();
    if (SelItem) {
      if (SelType != NoDef)
        SelItem = getSectionNode(SelItem, SelType);
    }
    else {
      SelItem = (CTreeItem*)m_tree->RootItem;
      firstVisible = SelItem;
      data = (CMainItemData*)SelItem->getItemData();
      if ( (*((LavaDECL**)data->synEl))->NestedDecls.first) {
        selItem = (CTreeItem*)SelItem->child(0);
        if (selItem) {
          data = (CMainItemData*)selItem->getItemData();
          if (data->type != TIType_Defs)
            selItem = (CTreeItem*)selItem->nextSibling();
          if (selItem)
            SelItem = selItem;
        }
      }
    }

    if (drawn) {
      m_tree->setCurAndSel(SelItem);
      if (VisibleDECL)
        firstVisible = BrowseTree(VisibleDECL, (CTreeItem*)m_tree->RootItem);
      if (firstVisible)
        m_tree->scrollToItem(firstVisible, QAbstractItemView::EnsureVisible );
//        m_tree->ensureItemVisible(firstVisible);
      else
        m_tree->scrollToItem(SelItem, QAbstractItemView::EnsureVisible );
//        m_tree->ensureItemVisible(SelItem);
    }
    setUpdatesEnabled(true);
    m_tree->update();
  }
}//DrawTree


bool CLavaPEView::EnableDelete(LavaDECL* decl)
{
  return (!decl->ParentDECL || ( decl->ParentDECL->op == OP_noOp) )
         && (!decl->SecondTFlags.Contains(funcImpl) || decl->WorkFlags.Contains(allowDEL))
         && (!decl->SecondTFlags.Contains(overrides)
            || (decl->DeclType != IAttr) && (decl->DeclType != OAttr) )
         && (!decl->TypeFlags.Contains(thisCompoForm)
            || (decl->DeclType != Function) && (decl->ParentDECL->DeclType != Function) )
         && !decl->TypeFlags.Contains(thisComponent);
}

bool CLavaPEView::event(QEvent *ev)
{
  if (ev->type() == IDU_LavaPE_CalledView) {
    wxDocManager::GetDocumentManager()->SetActiveView(this);
    return true;
  }
  else if (ev->type() == IDU_LavaPE_SyncTree) {
    CTreeItem* item = BrowseTree((LavaDECL*)((CustomEvent*)ev)->data(), (CTreeItem*)m_tree->RootItem);
    if (item && (item != (CTreeItem*)m_tree->currentItem())) {
      inSync = true;
      m_tree->setCurAndSel(item);
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
//      m_tree->ensureItemVisible(item);
      QApplication::postEvent(myFormView, new CustomEvent(IDU_LavaPE_CalledView));
      return true;
    }
    else
      return false;
  }
  else if (ev->type() == IDU_LavaPE_SetLastHint) {
    GetDocument()->SetLastHint();
    return true;
  }
  else if (ev->type() == IDU_LavaPE_OnDrop) {
    OnDropPost(((CustomEvent*)ev)->data());
    return true;
  }
  else if (ev->type() == IDU_LavaPE_setSel) {
    setSelPost((QTreeWidgetItem*)((CustomEvent*)ev)->data());
    return true;
  }
  else
		return wxView::event(ev);
}

bool CLavaPEView::ExpandItem(CTreeItem* item, int level/*=-1*/)
{
  if (level == 0)
    return false;
  int count = level-1;
  CTreeItem* htr = item;
  htr = (CTreeItem*)item->child(0);
  if (htr) {
    m_tree->expandItem(item); //->setOpen(true);
    SetTreeFlags(item, true);
  }
  else
    return false;
  while (htr) {
    ExpandItem(htr, count);
    htr = (CTreeItem*)htr->nextSibling();
  }
  return true;
}

void CLavaPEView::ExpandTree(CTreeItem* top)
{
  CTreeItem* item;
  LavaDECL *itemDECL = 0;
  CMainItemData* itd;
  if (top) {
    item = (CTreeItem*)top->child(0);
    while (item) {
      itd = (CMainItemData*)item->getItemData();
      if ((itd->type == TIType_Exec)
          || (itd->type == TIType_Ensure)
          || (itd->type == TIType_Require) ) {
        itemDECL = GetExecDECL(item);
        item->SetItemMask(itemDECL && (itemDECL->DECLError1.first || itemDECL->DECLError2.first),
                          itemDECL && itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
      }
      else if (itd->type == TIType_DECL) {
        itemDECL = *((LavaDECL**)itd->synEl);
        item->SetItemMask(itemDECL && (itemDECL->DECLError1.first || itemDECL->DECLError2.first),
                          itemDECL && itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
      }
      else
        if (itd->type == TIType_CHEEnumSel)
          item->SetItemMask(false, ((CHEEnumSelId*)itd->synEl)->data.DECLComment.ptr!=0);
      if (((CMainItemData*)item->getItemData())->toExpand)
        m_tree->expandItem(item);
      else
        m_tree->collapseItem(item);
      //item->setOpen(((CMainItemData*)item->getItemData())->toExpand);
      ExpandTree(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
  else {
    item = (CTreeItem*)m_tree->RootItem;
    itd = (CMainItemData*)item->getItemData();
    itemDECL = *((LavaDECL**)itd->synEl);
    item->SetItemMask(itemDECL->DECLError1.first || itemDECL->DECLError2.first,
              itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
    m_tree->expandItem(item); //->setOpen(true);
    item = (CTreeItem*)item->child(0);
    while (item) {
      m_tree->expandItem(item); //->setOpen(true);
      SetTreeFlags(item, true);
      ExpandTree(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

CLavaPEDoc* CLavaPEView::GetDocument() // non-debug version is inline
{
  return (CLavaPEDoc*)m_viewDocument;
}


LavaDECL* CLavaPEView::GetExecDECL(CTreeItem* item)
{
  TDeclType declType;
  LavaDECL *itemDECL=0;
  CMainItemData* itd = (CMainItemData*)item->getItemData();
  if ((itd->type == TIType_Exec)
    || (itd->type == TIType_Ensure)
    ||(itd->type == TIType_Require) ) {
    if (itd->type == TIType_Exec)
      declType = ExecDef;
    else if (itd->type == TIType_Ensure)
      declType = Ensure;
    else
      declType = Require;
    itemDECL = GetDocument()->GetExecDECL(*((LavaDECL**)itd->synEl), declType, false,false);

  }
  return itemDECL;
}

int CLavaPEView::GetPixmap(bool isParent, bool isAttr, TDeclType deftype, const SynFlags flag)
{
  int pm;
  if (isParent) {
    switch (deftype) {
    case VirtualType:
      return 27;//((CLavaPEApp*)wxTheApp)->LavaIcons[27];
    case IAttr:
      return 17;//((CLavaPEApp*)wxTheApp)->LavaIcons[17];
    case OAttr:
      return 20;//((CLavaPEApp*)wxTheApp)->LavaIcons[20];
    case Attr:
    case FormText:
    case Function:
      return 19;//((CLavaPEApp*)wxTheApp)->LavaIcons[19];
    case ExecDef:
      if (flag.Contains(invariantPM))
        if (flag.Contains(emptyPM))
          return 37;//((CLavaPEApp*)wxTheApp)->LavaIcons[37];
        else
          return 36;//((CLavaPEApp*)wxTheApp)->LavaIcons[36];
      else
        if (flag.Contains(emptyPM))
          return 7;//((CLavaPEApp*)wxTheApp)->LavaIcons[7];
        else
          return 8;//((CLavaPEApp*)wxTheApp)->LavaIcons[8];
    case Ensure:
      if (flag.Contains(emptyPM))
        return 35;//((CLavaPEApp*)wxTheApp)->LavaIcons[35];
      else
        return 34;//((CLavaPEApp*)wxTheApp)->LavaIcons[34];
    case Require:
      if (flag.Contains(emptyPM))
        return 39;//((CLavaPEApp*)wxTheApp)->LavaIcons[39];
      else
        return 38;//((CLavaPEApp*)wxTheApp)->LavaIcons[38];
    default:
      return 9;//((CLavaPEApp*)wxTheApp)->LavaIcons[9];//all types
    }
  }
  switch (deftype) {
  case NoDef:
  case UnDef:
    return 26;//((CLavaPEApp*)wxTheApp)->LavaIcons[26];
  case VirtualType:
    if (isAttr)
      return 2;//((CLavaPEApp*)wxTheApp)->LavaIcons[2];
    else
      return 3;//((CLavaPEApp*)wxTheApp)->LavaIcons[3];
  case Impl:
    if (flag.Contains(isGUI))
      return 30;//((CLavaPEApp*)wxTheApp)->LavaIcons[30];
    else
      return 4;//((CLavaPEApp*)wxTheApp)->LavaIcons[4];
  case Interface:
    if (flag.Contains(isSet))
      if (isAttr)
        return 29;//((CLavaPEApp*)wxTheApp)->LavaIcons[29];
      else
        return 28;//((CLavaPEApp*)wxTheApp)->LavaIcons[28];
    else
      if (isAttr)
        return 5;//((CLavaPEApp*)wxTheApp)->LavaIcons[5];
      else
        if (flag.Contains(isGUI))
          return 13;//((CLavaPEApp*)wxTheApp)->LavaIcons[13];
        else
          return 6;//((CLavaPEApp*)wxTheApp)->LavaIcons[6];
  case CompObjSpec:
    return 23;//((CLavaPEApp*)wxTheApp)->LavaIcons[23];
  case CompObj:
    return 24;//((CLavaPEApp*)wxTheApp)->LavaIcons[24];

  case Initiator:
    return 16;//((CLavaPEApp*)wxTheApp)->LavaIcons[16];
  case Function:
    return 15;//((CLavaPEApp*)wxTheApp)->LavaIcons[15];
  case Package:
  case Component:
    pm = 22;//((CLavaPEApp*)wxTheApp)->LavaIcons[22];
    return pm;
  case Attr:
    return 0;//((CLavaPEApp*)wxTheApp)->LavaIcons[0];
  case FormText:
    return 18;//((CLavaPEApp*)wxTheApp)->LavaIcons[18];
  case IAttr:
    return 0;//((CLavaPEApp*)wxTheApp)->LavaIcons[0];
  case OAttr:
    return 0;//((CLavaPEApp*)wxTheApp)->LavaIcons[0];
  case FormDef:
    if (isAttr)
      return 14;//((CLavaPEApp*)wxTheApp)->LavaIcons[14];
    else
      return 30;//((CLavaPEApp*)wxTheApp)->LavaIcons[30];
  case ExecDef:
    if (flag.Contains(invariantPM))
      if (flag.Contains(emptyPM))
        return 37;//((CLavaPEApp*)wxTheApp)->LavaIcons[37];
      else
        return 36;//((CLavaPEApp*)wxTheApp)->LavaIcons[36];
    else
      if (flag.Contains(emptyPM))
        return 8;//((CLavaPEApp*)wxTheApp)->LavaIcons[8];
      else
        return 7;//((CLavaPEApp*)wxTheApp)->LavaIcons[7];
  case Ensure:
    if (flag.Contains(emptyPM))
      return 35;//((CLavaPEApp*)wxTheApp)->LavaIcons[35];
    else
      return 34;//((CLavaPEApp*)wxTheApp)->LavaIcons[34];
  case Require:
    if (flag.Contains(emptyPM))
      return 39;//((CLavaPEApp*)wxTheApp)->LavaIcons[39];
    else
      return 38;//((CLavaPEApp*)wxTheApp)->LavaIcons[38];
  case BasicDef:
    if (isAttr)
      return 0;//((CLavaPEApp*)wxTheApp)->LavaIcons[0];
    else
      return 1;//((CLavaPEApp*)wxTheApp)->LavaIcons[1];
  default:
    return 26;//((CLavaPEApp*)wxTheApp)->LavaIcons[26];
  }
  return 26;//((CLavaPEApp*)wxTheApp)->LavaIcons[26];
}

int CLavaPEView::GetPos(CTreeItem* item, CTreeItem* prev)
{
  /*
    Calculate positioning .
    If item != 0, calculate pos from item.
    If item = 0, calculate pos where a new item will be inserted:
      if prev != 0 : the new item will be inserted as sibling after the prev item,
      otherwise result = 1, i.e. the new item will be inserted as first child of the parent item.
  */
  CTreeItem* htree;
  int ii;
  if (item)
    htree = item;
  else
    if (prev)
      htree = prev;
    else
      return 1;
  CTreeItem* parhtree = (CTreeItem*)htree->parent();
  if (prev)
    ii = 2;
  else
    ii = 1;
  CTreeItem* elh = (CTreeItem*)parhtree->child(0);
  while (elh && (elh != htree)) {
    ii +=1;
    elh = (CTreeItem*)elh->nextSibling();
  }
  return ii;

}//GetPos


CTreeItem* CLavaPEView::getSectionNode(CTreeItem* parent, TDeclType ncase)
{
  if (!parent)
    return NULL;
  CMainItemData *data = (CMainItemData*)parent->getItemData();
  TDeclType parentType;
//  if ((parent != m_tree->child(0)) || !myInclView) {
    parentType = (*(LavaDECL**)data->synEl)->DeclType;
    if ((data->type == TIType_DECL)
        //&& (  (*(LavaDECL**)data->synEl)->DeclDescType == DefDesc)
        && (parentType != IAttr) && (parentType != OAttr) && (parentType != FormDef) ) {
      if (( (*(LavaDECL**)data->synEl)->DeclDescType != StructDesc)
          && ( (*(LavaDECL**)data->synEl)->DeclDescType != EnumType))
        return parent;
    }
    else
      if ((parentType != FormDef) || myInclView)
        return parent;
//  }
//  else
//    parentType = Package;
  DWORD synEl = data->synEl;
  CTreeItem* node = (CTreeItem*)parent->child(0);
  if (node)
    if (!ContainTab[parentType] [ncase])
      return node;
    while (node) {
      data = (CMainItemData*)node->getItemData();
      if ( (ncase == VirtualType) && (data->type == TIType_VTypes)
         || (ncase == IAttr) && (data->type == TIType_Input)
         || (ncase == OAttr) && (data->type == TIType_Output)
         || ((ncase == Attr) || (ncase == Function) || (ncase == FormText)) && (data->type == TIType_Features)
         || (ncase == ExecDef) && (data->type == TIType_Exec)
         || (ncase == Ensure) && (data->type == TIType_Ensure)
         || (ncase == Require) && (data->type == TIType_Require)
         || (ncase != NoDef) && (ncase != VirtualType) && (ncase != IAttr)
             && (ncase != OAttr) && (ncase != Attr) && (ncase != Function) && (ncase != FormText)
             && (ncase != ExecDef) && (ncase != Ensure) && (ncase != Require)
             &&  (data->type == TIType_Defs))
        return node;
      else
        node = (CTreeItem*)node->nextSibling();
    }
  int bm;
  SynFlags flags;
  flags.INCL(emptyPM);
  DString Fields, ExecLabel;
  if (parentType == Function)
    Fields = DString("Fields");
  else
    Fields = DString("Features");
  if ((parentType == Initiator) || (parentType == Function)
      /*|| (parentType == Impl)*/ )
    ExecLabel = DString("Exec");
  else
    ExecLabel = DString("Invariant");
  if (!node) {
    if ((*(LavaDECL**)synEl)->TreeFlags.Contains(hasEmptyOpt)) {
      TDeclType gpt = Impl;
      if ((parentType == Function) && (parent != m_tree->RootItem) ) {
        CTreeItem* gp = (CTreeItem*)parent->parent();
        gp = (CTreeItem*)gp->parent();
        CMainItemData *gpdata = (CMainItemData*)gp->getItemData();
        gpt = (*(LavaDECL**)gpdata->synEl)->DeclType;
      }
      if ((ncase == VirtualType)
        || ContainTab[parentType] [VirtualType]
           && GetDocument()->IDTable.okPatternLevel(*(LavaDECL**)synEl)
           && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) ) {

        data = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
        bm = GetPixmap(true, true, VirtualType);
        node = InsertItem("Virtual types", bm, parent);
        node->setItemData( data);
      }
      if ((ncase == IAttr)
        || ContainTab[parentType] [ IAttr]
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)) {

        data = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
        bm = GetPixmap(true, true, IAttr);
        node = InsertItem("Inputs", bm, parent);
        node->setItemData( data);
      }
      if ((ncase == OAttr)
        || ContainTab[parentType] [OAttr]
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(execIndependent)
          && !(*(LavaDECL**)synEl)->SecondTFlags.Contains(isLavaSignal)
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)) {
        data = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
        bm = GetPixmap(true, true, OAttr);
        node = InsertItem("Outputs", bm, parent);
        node->setItemData( data);
      }
      if (ContainTab[parentType] [ 0]) {
        data = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded));
        bm = GetPixmap(true, true, Interface);
        node = InsertItem("Declarations", bm, parent);
        node->setItemData( data );
      }
      if ((ncase == Attr)
        || ContainTab[parentType] [ Attr] || ContainTab[parentType] [ Function] ) {
        if ((parentType != Function)  || (gpt != Interface) ) {
          data = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
          bm = GetPixmap(true, true, Attr);
          node = InsertItem(Fields.c, bm, parent);
          node->setItemData( data );
        }
      }
      if (ContainTab[parentType] [ Require]) {
         data = new CMainItemData(TIType_Require, synEl);
         bm = GetPixmap(true, true, Require, flags);
         node = InsertItem("Require" ,bm, parent);
         node->setItemData(  data);
      }
      if (ContainTab[parentType] [ Ensure]) {
         data = new CMainItemData(TIType_Ensure, synEl);
         bm = GetPixmap(true, true, Ensure, flags);
         node = InsertItem("Ensure" ,bm, parent);
         node->setItemData(  data);
      }
      if (ContainTab[parentType] [ ExecDef]
          && (gpt != Interface)
          && ((parentType != Interface) || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) )) {
         data = new CMainItemData(TIType_Exec, synEl);
         if ((parentType == Interface) || (parentType == Impl))
           flags.INCL(invariantPM);
         bm = GetPixmap(true, true, ExecDef,flags);
         node = InsertItem(ExecLabel.c ,bm, parent);
         node->setItemData(  data);
      }
    }
    else {
      CTreeItem *afterItem, *naItem;
      CMainItemData * afterData;
      switch (ncase) {
        case VirtualType:
          afterItem = (CTreeItem*)parent->child(0);
          if (afterItem) {
            afterData = (CMainItemData*)afterItem->getItemData();
            if (afterData->type != TIType_EnumItems)
              afterItem = TVI_FIRST;
          }
          else
            afterItem = TVI_FIRST;
          data = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Virtual types", bm, parent, afterItem);
          node->setItemData( data);
          break;
        case IAttr:
          naItem = (CTreeItem*)parent->child(0);
          afterItem = 0;
          if (naItem)
            afterData = (CMainItemData*)naItem->getItemData();
          while (naItem && (afterData->type != TIType_VTypes)
                        && (afterData->type == TIType_EnumItems)) {
            afterItem = naItem;
            naItem = (CTreeItem*)afterItem->nextSibling();
            if (naItem)
              afterData = (CMainItemData*)naItem->getItemData();
          }
          if (naItem && (afterData->type == TIType_VTypes))
            afterItem = naItem;
          else
            if (!afterItem)
              afterItem = TVI_FIRST;
          data = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Inputs", bm, parent, afterItem);
          node->setItemData( data );
          break;
        case OAttr:
          naItem = (CTreeItem*)parent->child(0);
          afterItem = 0;
          if (naItem)
            afterData = (CMainItemData*)naItem->getItemData();
          while (naItem && (afterData->type != TIType_Input)
                        && ((afterData->type == TIType_EnumItems)
                            || (afterData->type == TIType_VTypes))) {
            afterItem = naItem;
            naItem = (CTreeItem*)afterItem->nextSibling();
            if (naItem)
              afterData = (CMainItemData*)naItem->getItemData();
          }
          if (naItem && (afterData->type == TIType_Input))
            afterItem = naItem;
          else
            if (!afterItem)
              afterItem = TVI_FIRST;
          data = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Outputs", bm, parent, afterItem);
          node->setItemData( data );
          break;
        case Attr:
        case Function:
        case FormText:
          afterItem = TVI_FIRST;
          naItem = (CTreeItem*)parent->child(0);
          while (naItem) {
            afterData = (CMainItemData*)naItem->getItemData();
            if (afterData->type != TIType_Exec) {
              afterItem = naItem;
              naItem = (CTreeItem*)afterItem->nextSibling();
            }
            else
              naItem = 0;
          }
          data = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem(Fields.c, bm, parent, afterItem);
          node->setItemData( data );
          break;
        case Require:
          data = new CMainItemData(TIType_Require, synEl);
          bm = GetPixmap(true,true,Require,flags);
          node = InsertItem("Require" ,bm, parent, TVI_LAST);
          node->setItemData(  data);
          break;

        case Ensure:
          data = new CMainItemData(TIType_Ensure, synEl);
          bm = GetPixmap(true,true,Ensure,flags);
          node = InsertItem("Ensure" ,bm, parent, TVI_LAST);
          node->setItemData(  data);
          break;

        case ExecDef:
          data = new CMainItemData(TIType_Exec, synEl);
          if ((parentType == Interface) || (parentType == Impl))
            flags.INCL(invariantPM);
          bm = GetPixmap(true,true,ExecDef,flags);
          node = InsertItem(ExecLabel.c ,bm, parent, TVI_LAST);
          node->setItemData(  data);
          break;

        default:
          naItem = (CTreeItem*)parent->child(0);
          afterItem = TVI_FIRST;
          naItem = (CTreeItem*)parent->child(0);
          while (naItem) {
            afterData = (CMainItemData*)naItem->getItemData();
            if ((afterData->type != TIType_Features)
              && (afterData->type != TIType_Require)
              && (afterData->type != TIType_Ensure)
              && (afterData->type != TIType_Exec)
              ) {
              afterItem = naItem;
              naItem = (CTreeItem*)afterItem->nextSibling();
            }
            else
              naItem = 0;
          }
          data = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Declarations", bm, parent, afterItem);
          node->setItemData( data);
      }
    }
    node = (CTreeItem*)parent->child(0);
  }
  if (node)
    while (node) {
      data = (CMainItemData*)node->getItemData();
      if ( (ncase == VirtualType) && (data->type == TIType_VTypes)
         || (ncase == IAttr) && (data->type == TIType_Input)
         || (ncase == OAttr) && (data->type == TIType_Output)
         || (ncase != NoDef) && (ncase != Attr) && (ncase != Function)
            && (ncase != FormText) && (ncase != IAttr) && (ncase != OAttr)
            && (ncase != ExecDef) && (ncase != Ensure) && (ncase != Require)
            && (data->type == TIType_Defs)
         || ((ncase == Attr) || (ncase == Function) || (ncase == FormText)) && (data->type == TIType_Features)
         || (ncase == Require) && (data->type == TIType_Require)
         || (ncase == Ensure) && (data->type == TIType_Ensure)
         || (ncase == ExecDef) && (data->type == TIType_Exec)
         )
        return node;
      else
        node = (CTreeItem*)node->nextSibling();
  }
  return NULL;
}

void CLavaPEView::Gotodef(CTreeItem* item)
{
  LavaDECL *DECL, *decl;
  TID id;
  CGotoBox *box;
  CMainItemData * itd;

  if (item) {
    itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_Exec)
      OnShowExec(ExecDef);
    else if (itd->type == TIType_Ensure)
      OnShowExec(Ensure);
    else if (itd->type == TIType_Require)
      OnShowExec(Require);
    else {
      if (itd->type == TIType_DECL) {
        DECL = *(LavaDECL**)itd->synEl;
        id.nID = -1;
        ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
        id = DECL->RefID;
        if ((id.nID >= 0) && (DECL->Supports.first || DECL->Inherits.first)
            || (DECL->Supports.first != DECL->Supports.last)
            || DECL->Supports.first && DECL->Inherits.first
            || (DECL->Inherits.first != DECL->Inherits.last)) {
         //select ID
          box = new CGotoBox(DECL, GetDocument(), this);
          if (box->exec() == QDialog::Accepted)
            id = box->SelID;
          else
            id.nID = -1;
          delete box;
          if (id.nID < 0)
            return;
        }
        else
          if (id.nID < 0) {
            if (DECL->Supports.first)
              id = ((CHETID*)DECL->Supports.first)->data;
            else
              if (DECL->Inherits.first)
                id = ((CHETID*)DECL->Inherits.first)->data;
          }
        decl = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(GetDocument(), id);
      }
    }
  }
}


void CLavaPEView::IOnEditCopy()
{
  CMainItemData *data, *newClipdata = 0;
  LavaDECL *decl = 0;
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (item) {
    data = (CMainItemData*)item->getItemData();
    if (data && (data->type == TIType_DECL))
      decl = *(LavaDECL**)data->synEl;
    if (data && (decl && !decl->TypeFlags.Contains(thisComponent) && !decl->TypeFlags.Contains(thisCompoForm)
                 || (data->type == TIType_CHEEnumSel))) {
      QByteArray ba;
			QDataStream ar(&ba,QIODevice::WriteOnly);
      CMainItemData clipData(data->type, 0);
      clipData.synEl = (unsigned long) NewLavaDECL();  //the Clipdata synEl is LavaDECL*
      if (!CollectDECL) {
        CollectDECL = NewLavaDECL();
        CollectDECL->DeclType = NoDef;
        AddToDragChain(item);
      }
      *(LavaDECL*)clipData.synEl = *CollectDECL;
      clipData.ClipTree = &GetDocument()->mySynDef->SynDefTree;
      clipData.docPathName = new DString(GetDocument()->GetAbsSynFileName());
      clipData.Serialize(ar);
      defTypeSpicked = ((LavaDECL*)clipData.synEl)->DeclType;
      treeflagsSpicked = ((LavaDECL*)clipData.synEl)->TreeFlags;
      secondtflagsSpicked = ((LavaDECL*)clipData.synEl)->SecondTFlags;

      LavaSource *ls = new LavaSource();
//      ls->setEncodedData(ba);
      wxTheApp->clipboard()->setMimeData(ls, QClipboard::Clipboard);
      clipboard_lava_notEmpty = wxTheApp->clipboard()->data(QClipboard::Clipboard)->provides(m_nIDClipFormat);
    }
  }

}

bool CLavaPEView::IsChildNodeOf(CTreeItem* hitemChild, CTreeItem* hitemSuspectedParent)
{
  do {
    if (hitemChild == hitemSuspectedParent)
      break;
  } while (hitemChild = (CTreeItem*)hitemChild->parent());
  return (hitemChild != NULL);
}

CMainItemData* CLavaPEView::Navigate(bool all, CTreeItem*& item, int& level)
{
  CTreeItem *nextItem=0, *parItem;

  if (item) {
    if (all || m_tree->isItemExpanded(item)) //->isOpen())
      nextItem = (CTreeItem*)item->child(0);
    if (nextItem)
      level = level+1;
    else
      if ((level > 0) || !all)
        nextItem = (CTreeItem*)item->nextSibling();
    while (!nextItem && (level > 0)) {
      parItem = (CTreeItem*)item->parent();
      if (parItem) {
        level = level-1;
        if (level) {
          nextItem = (CTreeItem*)parItem->nextSibling();
          if (!nextItem)
            item = parItem;
        }
      }
      else
        level = 0;
    }
  }
  else {
    level = 0;
    nextItem = (CTreeItem*)m_tree->RootItem;
  }
  if (nextItem) {
    item = nextItem;
    return (CMainItemData*)item->getItemData();
  }
  else {
    item = 0;
    return 0;
  }
}//Navigate


void CLavaPEView::OnActivateView(bool bActivate, wxView *deactiveView)
{
  CTreeItem* sel;
  if (GetDocument()->mySynDef && !((CLavaPEApp*)wxTheApp)->inTotalCheck) {
    if (bActivate) {
      sel = (CTreeItem*)m_tree->currentItem();
      SetErrAndCom(sel);
      if (!m_tree->hasFocus())
        m_tree->setFocus();
//??      sel->repaint();
      clipboard_lava_notEmpty = wxTheApp->clipboard()->data(QClipboard::Clipboard)->provides(m_nIDClipFormat);
      if (clipboard_lava_notEmpty) {
        QByteArray ar = ((LavaSource*)wxTheApp->clipboard()->data(QClipboard::Clipboard))->data/*encodedData*/(m_nIDClipFormat);
        SynFlags treeflags, secondtflags;
        CMainItemData clipData;
        defTypeSpicked = clipData.Spick(ar, treeflagsSpicked, secondtflagsSpicked);
      }
    }
    else
      DisableActions();
  }
}

void CLavaPEView::OnDblclk( const QModelIndex & index)
{
  CTreeItem* itemHit = (CTreeItem*)m_tree->itemAtIndex(index);
  if (itemHit) {
    CMainItemData* data = (CMainItemData*)((CTreeItem*)itemHit)->getItemData();
    if (data->type == TIType_Exec)
      OnShowExec(ExecDef);
    else if (data->type == TIType_Ensure)
      OnShowExec(Ensure);
    if (data->type == TIType_Require)
      OnShowExec(Require);
  }

    //OnEditSelItem((CTreeItem*)itemHit, false);
}

void CLavaPEView::OnDelete()
{
  CTreeItem *item, *par;
  CMainItemData *data, *parData;
  DWORD d4;
  DString *str2;
  CLavaPEHint *hint;
  LavaDECL *cutDECL, *newDECL;
  int pos;
  CHEEnumSelId *delEl, *newEl;

  if (GetDocument()->changeNothing)
    return;
  item = (CTreeItem*)m_tree->currentItem();
  if (!item || (item == m_tree->RootItem))
    return;
  data = (CMainItemData*)item->getItemData();
  if (data && (data->type != TIType_DECL)
           && ((data->type != TIType_CHEEnumSel) || !myInclView))
    return;
  par = (CTreeItem*)item->parent();

  if (CollectDECL) {
    if (CollectDECL->DeclType == DragEnum) {
      hint = ChangeEnum(CollectDECL, item, true, false);
      if (hint) {
        hint->FirstLast.EXCL(lastHint);
        GetDocument()->UndoMem.AddToMem(hint);
        GetDocument()->UpdateDoc(this, false, hint);
        GetDocument()->ConcernForms(hint);
        GetDocument()->ConcernExecs(hint);
        GetDocument()->SetLastHint();
      }
    }
    else {
      if (CanDelete && par) {
        d4 = ((CMainItemData*)par->getItemData())->synEl;
        str2 = 0;
        if (CollectDECL->FullName.l)
          str2 = new DString(CollectDECL->FullName);
        hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)CollectDECL, (DWORD)str2, (DWORD)CollectPos, d4, 0);
        GetDocument()->UpdateDoc(this, false, hint);
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4)
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
    }
    DeleteDragChain();
  }
  else {
    par = (CTreeItem*)par->parent();
    parData = (CMainItemData*)par->getItemData();
    if (data  && (data->type == TIType_DECL)) {
      cutDECL = *(LavaDECL**)data->synEl;
      if (EnableDelete(cutDECL)) {
        d4 = parData->synEl;
        if (par != m_tree->RootItem) {
          str2 = new DString(cutDECL->FullName);
          pos = GetPos(item, 0);
        }
        else {
          str2 = 0;
          if (!myInclView) {
            if (cutDECL->DeclType == FormText)
              pos = GetPos(item);
            else {
              item = myMainView->BrowseTree(cutDECL, (CTreeItem*)myMainView->m_tree->RootItem);
              pos = myMainView->GetPos(item);
            }
          }
          else
            pos = GetPos(item, 0);
        }
        hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)cutDECL, (DWORD)str2, (DWORD)pos, d4/*, (DWORD)dropParent*/);
        GetDocument()->UndoMem.AddToMem(hint);
        GetDocument()->UpdateDoc(this, false, hint);
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4)
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
    }
    else {
      if (data && (data->type == TIType_CHEEnumSel)) {
        delEl = (CHEEnumSelId*)data->synEl;

        par = (CTreeItem*)item->parent();
        par = (CTreeItem*)par->parent();
        parData = (CMainItemData*)par->getItemData();
        cutDECL = *(LavaDECL**)parData->synEl;
        newDECL = NewLavaDECL();
        *newDECL = *cutDECL;
        CHAINANY* newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
        newEl = (CHEEnumSelId*)newItems->first;
        while (newEl && (newEl->data.Id != delEl->data.Id))
          newEl = (CHEEnumSelId*)newEl->successor;
        if (newEl) {
          newEl = (CHEEnumSelId*)newItems->Uncouple(newEl);
          if (newEl->successor)
            ((CHEEnumSelId*)newEl->successor)->data.selItem = true;
          delEl->data.selItem = true;
          delete newEl;
          str2 = new DString((*(LavaDECL**)parData->synEl)->FullName);
          newDECL->WorkFlags.INCL(selEnum);
          newDECL->TreeFlags.INCL(ItemsExpanded);
          hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)newDECL, (DWORD)str2, 0, parData->synEl);
          GetDocument()->UndoMem.AddToMem(hint);
          GetDocument()->UpdateDoc(this, false, hint);
          GetDocument()->ConcernForms(hint);
          GetDocument()->ConcernExecs(hint);
          GetDocument()->SetLastHint();
        }
      }
    }
  }
}

QDrag*  CLavaPEView::OnDragBegin()
{

  /*
  CMainItemData *ddrag;
  CMainItemData clipdata;

  if (GetDocument()->changeNothing)
    return 0;
  DropPosted = 0;
  m_hitemDrop = 0;
  m_hitemDrag = (CTreeItem*)m_tree->currentItem();
  ddrag = (CMainItemData*)m_hitemDrag->getItemData();
  if ((ddrag->type != TIType_DECL) && (ddrag->type != TIType_CHEEnumSel)
      || (m_hitemDrag == (CTreeItem*)m_tree->RootItem)
      || (ddrag->type == TIType_DECL)
          && ((*(LavaDECL**)ddrag->synEl)->TypeFlags.Contains(thisComponent)
              || (*(LavaDECL**)ddrag->synEl)->TypeFlags.Contains(thisCompoForm) ) ) {
    m_hitemDrag = 0;
    return 0;
  }
  if (!CollectDECL) {
    CollectDECL = NewLavaDECL();
    CollectDECL->DeclType = NoDef;
    CanDelete = true;
    AddToDragChain(m_hitemDrag);
    CollectPos = GetPos(m_hitemDrag, 0);
  }
  else
    if (CollectDECL->TreeFlags.Contains(mixedDrag)
        && (CollectDECL->TreeFlags.Contains(dragFuncDef) || CollectDECL->TreeFlags.Contains(dragFuncImpl))) {
      m_hitemDrag = 0;
      if (CollectDECL)
        DeleteDragChain();
      return 0;
    }
  declDrag = CollectDECL;
  lastPressed = m_hitemDrag;
  m_hitemDragP = (CTreeItem*)m_hitemDrag->parent();
  m_hitemDragPP = 0;
  if (m_hitemDragP)
    m_hitemDragPP = (CTreeItem*)m_hitemDragP->parent();
  if (m_hitemDragPP)
    pDeclDragP = (LavaDECL**)((CMainItemData*)m_hitemDragPP->getItemData())->synEl;

	QByteArray ba;
  QDataStream ar(&ba,QIODevice::WriteOnly);
  //Clipdata  = new CMainItemData(ddrag->type, 0);
  clipdata.type = ddrag->type;
  clipdata.synEl = (unsigned long) NewLavaDECL();  //the Clipdata synEl is LavaDECL*
  clipdata.ClipTree = &GetDocument()->mySynDef->SynDefTree;
  *(LavaDECL*)clipdata.synEl = *declDrag;
  clipdata.docPathName = new DString(GetDocument()->GetAbsSynFileName());
  clipdata.Serialize(ar);
  dragSource = new LavaSource(m_tree);
  dragSource->setEncodedData(ba);
  GetDocument()->DragView = this;
  DragDoc = GetDocument();
  dragSource->drag();
  if (clipdata.synEl)
    delete (LavaDECL*)clipdata.synEl;
  //if (m_hitemDrop)
  //  m_tree->setItemSelected(m_hitemDrop, false);
  if (!DropPosted) {
    m_hitemDrag = 0;
    m_hitemDrop = 0;
    GetDocument()->DragView = 0;
    if (CollectDECL)
      DeleteDragChain();
  }
  */
  return 0;
}


void CLavaPEView::OnDragEnter(QDragEnterEvent* ev)
{
  /*
  CLavaPEDoc *dropDoc = GetDocument();
  CHESimpleSyntax *cheSyn;
  SynFlags treeflags, secondtflags;
  DString  dropAbsName;

  if (ev->provides(m_nIDClipFormat)) {
    QByteArray ba = ev->encodedData(m_nIDClipFormat);
		QDataStream ar(ba);
    if (Clipdata) {
      Clipdata->Destroy();
      delete Clipdata;
    }
    Clipdata = new CMainItemData;
    Clipdata->Serialize(ar);
    if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
        || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF)) {
      dropAbsName = dropDoc->GetAbsSynFileName();
      DragINCL = 0;
      DropINCL = 0;
      if (SameFile(dropAbsName, *Clipdata->docPathName) )
        DragDoc = dropDoc;
      else {
        DragDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(Clipdata->docPathName->c);
        if (DragDoc) {
          cheSyn = (CHESimpleSyntax*)dropDoc->mySynDef->SynDefTree.first;
          if (cheSyn) {
            cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is DragDoc included?
            for ( ;cheSyn
                && !SameFile(cheSyn->data.SyntaxName,  dropDoc->IDTable.DocDir, *Clipdata->docPathName);//lowRelName, dropDoc->IDTable.DocDir);
                   cheSyn = (CHESimpleSyntax*)cheSyn->successor);
            if (cheSyn)
              DragINCL = cheSyn->data.nINCL;
          }
          cheSyn = (CHESimpleSyntax*)DragDoc->mySynDef->SynDefTree.first;
          if (cheSyn) {
            cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is DropDoc included?
            for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, DragDoc->IDTable.DocDir, dropAbsName);//lowRelName, DragDoc->IDTable.DocDir);
                   cheSyn = (CHESimpleSyntax*)cheSyn->successor);
            if (cheSyn)
              DropINCL = cheSyn->data.nINCL;
          }
          if (!DragINCL && !DropINCL)
            DragDoc = 0;
        }//DragDoc
        else
          DragDoc = 0;
      }//DragDoc != dropDoc
    }//Features
  }
  else
    ev->ignore();
  */
}

void CLavaPEView::OnDragLeave(QDragLeaveEvent* ev)
{
  /*
<<<<<<< LavaPEView.cpp
  if (m_hitemDrop) 
    m_tree->setItemSelected(m_hitemDrop, false);
=======
  if (m_hitemDrop)
    GetListView()->setItemSelected(m_hitemDrop, false);
>>>>>>> 1.31
  m_hitemDrop = 0;
  if (Clipdata) {
    Clipdata->Destroy();
    delete Clipdata;
    Clipdata = 0;
    DragDoc = 0;
  }
  */
}

void CLavaPEView::OnDragOver(QDragMoveEvent* ev)
{
  /*
  CLavaPEView* dragView;
  TIType tiType;
  CTreeItem* item;

  if (ev->provides(m_nIDClipFormat) && (!GetDocument()->changeNothing)) {
    item = (CTreeItem*)m_tree->itemAt(m_tree->contentsToViewport(ev->pos()));
    if (m_hitemDrop && (item != m_hitemDrop))
      m_tree->setItemSelected(m_hitemDrop, false);
    m_hitemDrop = item;
    if (m_hitemDrop) {
      RefacCase = noRefac;
      tiType = CanPaste(((LavaDECL*)Clipdata->synEl)->DeclType, ((LavaDECL*)Clipdata->synEl)->TreeFlags, ((LavaDECL*)Clipdata->synEl)->SecondTFlags, m_hitemDrop);
      if (tiType != TIType_NoType) {
        DString fn = GetDocument()->GetAbsSynFileName();
        if (fn == *Clipdata->docPathName) {
          dragView = (CLavaPEView*)GetDocument()->DragView;
          if ((dragView == this) &&
              ((m_hitemDrag == m_hitemDrop)
                || (m_hitemDrag == (CTreeItem*)m_tree->RootItem)
                || IsChildNodeOf(m_hitemDrop, m_hitemDrag)
                || (CollectPos == 1) && (m_hitemDrop == m_hitemDrag->parent())
                || (m_hitemDrop == GetPrevSiblingItem(m_hitemDrag) ))) {
            m_hitemDrop->setDropEnabled(false);
            ev->ignore(m_tree->itemRect(m_hitemDrop));
            ev->acceptAction(false);
            m_hitemDrop = 0;
            return;
          }
          else {
            if (ev->action() == QDropEvent::Copy) {
              m_tree->setCurAndSel(m_hitemDrop, false);
              ev->accept(m_tree->itemRect(m_hitemDrop));
              m_hitemDrop->setDropEnabled(true);
              ev->acceptAction(true);
              return;
            }
            if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragIO)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragParam)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragEnum)) {
              if ( (m_hitemDrop == m_hitemDrag->parent())
                 || (m_hitemDrop->parent() == m_hitemDrag->parent())) {
                m_tree->setCurAndSel(m_hitemDrop, false);
                ev->accept(m_tree->itemRect(m_hitemDrop));
                m_hitemDrop->setDropEnabled(true);
                ev->acceptAction(true);
                return;
              }
              else {
                if ( ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
                    || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF))
                    && RefacMove(m_hitemDrop)) {
                  m_tree->setCurAndSel(m_hitemDrop, false);
                  m_hitemDrop->setDropEnabled(true);
                  ev->accept(m_tree->itemRect(m_hitemDrop));
                  ev->acceptAction(true);
                  return;
                }
                else {
                  m_hitemDrop->setDropEnabled(false);
                  ev->ignore(m_tree->itemRect(m_hitemDrop));
                  ev->acceptAction(false);
                  m_hitemDrop = 0;
                  return;
                }
              }
            }
            else { //DragDef
              m_hitemDrop->setDropEnabled(true);
              m_tree->setCurAndSel(m_hitemDrop, false);
              ev->accept(m_tree->itemRect(m_hitemDrop));
              ev->acceptAction(true);
              return;
            }
          }
        }
        else { //in other doc
          if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragDef)
            || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFText)) {
            m_hitemDrop->setDropEnabled(true);
            m_tree->setCurAndSel(m_hitemDrop);
            ev->accept(m_tree->itemRect(m_hitemDrop));
            ev->acceptAction(true);
            return;
          }
          else {
            if ( (((LavaDECL*)Clipdata->synEl)->DeclType != DragFeature)
                && (((LavaDECL*)Clipdata->synEl)->DeclType != DragFeatureF)
                || RefacMove(m_hitemDrop)) {
              m_hitemDrop->setDropEnabled(true);
              m_tree->setCurAndSel(m_hitemDrop);
              ev->accept(m_tree->itemRect(m_hitemDrop));
              ev->acceptAction(true);
              return;
            }
            else {
              m_hitemDrop->setDropEnabled(false);
              ev->ignore(m_tree->itemRect(m_hitemDrop));
              m_hitemDrop = 0;
              ev->acceptAction(false);
              return;
            }
          }
        }
      }
      else {
        m_hitemDrop->setDropEnabled(false);
        ev->ignore(m_tree->itemRect(m_hitemDrop));
        m_hitemDrop = 0;
        ev->acceptAction(false);
        return;
      }
    }//CanPaste
  }//HitTest
  ev->ignore();
  ev->acceptAction(false);
  m_hitemDrop = 0;
  */
}

void CLavaPEView::OnDrop(QDropEvent* ev)
{/*
  if (ev->provides(m_nIDClipFormat)) {
    m_hitemDrop = (CTreeItem*)m_tree->itemAt(m_tree->contentsToViewport(ev->pos()));
    if (m_hitemDrop) {
      lastDropped = m_hitemDrop;
      //m_tree->setItemSelected(m_hitemDrop, false);
      //QDropData *ddata = new QDropData(m_hitemDrop, ev->action());
      DropPosted = true;
      DragDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(Clipdata->docPathName->c);
      if (DragDoc && (DragDoc != GetDocument()))
        ((CLavaPEView*)DragDoc->MainView)->DropPosted = true;
      QApplication::postEvent(this, new CustomEvent(IDU_LavaPE_OnDrop, (void*)ev->action()));
    }
    else {
      if (Clipdata) {
        Clipdata->Destroy();
        delete Clipdata;
      }
    }
    ev->accept(m_hitemDrop != 0);
  }
  else
    ev->ignore();*/
}


void CLavaPEView::OnDropPost(void* act)
{
  CTreeItem *itemDragParent, *itemDropP;
  //CLavaPEView* dragView = 0;
  bool canDrag = false, canDrop, hasDragged = false, mdh = false;
  LavaDECL *declClip, *dragParent = 0, *dropDECL;
  TDeclType clipDefType;
  DString fn, *str2;
  CMainItemData *ddrop;
  TIType dropType, pasteType;
  TIDType idType;
  CHE *che, *vtHints = 0;
  SynFlags firstlast;
  DWORD d4;
  CLavaPEHint* hint=0, *delHint=0;
  CPECommand com;
  int pos;
  CMainItemData* itemDat;
  QDropEvent::Action action = (QDropEvent::Action)(int)act;

      declClip =  (LavaDECL*)Clipdata->synEl;
      clipDefType = declClip->DeclType;
      fn = GetDocument()->GetAbsSynFileName();
      canDrag = (fn == *Clipdata->docPathName);
      if (canDrag) {
        //dragView = (CLavaPEView*)GetDocument()->DragView;
        itemDragParent = (CTreeItem*)/*dragView->*/m_hitemDrag->parent();
        if (//(this == dragView) &&
          ((m_hitemDrop == m_hitemDrag)
          || (CollectPos == 1) && (m_hitemDrop == itemDragParent)
          || (m_hitemDrop == GetPrevSiblingItem(m_hitemDrag) ))) {
          m_hitemDrop = 0;
          m_hitemDrag = 0;
          if (CollectDECL)
            DeleteDragChain();
          return;
        }
        dragParent = /**dragView->*/*pDeclDragP;
      }
      else
        if (!canDrag)
          declClip->inINCL = 10000;
      ddrop = (CMainItemData*)m_hitemDrop->getItemData();
      dropType = ddrop->type;
      canDrop = true;
      dropDECL = *(LavaDECL**)ddrop->synEl;
      if (RefacCase == noRefac)  {
        pasteType = CanPaste(clipDefType, declClip->TreeFlags, declClip->SecondTFlags, m_hitemDrop);
        if (pasteType != TIType_NoType) {
          if ((clipDefType == DragIO) || (clipDefType == DragFeature) ) {
            TDeclType elType;
            switch (pasteType) {
            case TIType_Features:
              elType = Attr;
              break;
            case TIType_Input:
              elType = IAttr;
              break;
            case TIType_Output:
              elType = OAttr;
              break;
            default: ;
            }
            che = (CHE*)declClip->NestedDecls.first;
            while (che) {
              if ((((LavaDECL*)che->data)->DeclType != Attr) && (elType == Attr))
                ((LavaDECL*)che->data)->TypeFlags = SET(acquaintance, -1);
              ((LavaDECL*)che->data)->DeclType = elType;
              che = (CHE*)che->successor;
            }
          }
        }
      }
      else {
        if (action == QDropEvent::Move)
          canDrop = Refac(dropDECL, mdh, vtHints);
      }

      if (canDrag && canDrop) {
        if (CollectDECL) {
          if (CollectDECL->DeclType == DragEnum) {
            hint = ChangeEnum(CollectDECL, m_hitemDrop, action == QDropEvent::Move, true);
            GetDocument()->hasHint = true;
            Clipdata->docPathName = 0;
            m_hitemDrag = 0;
            if (hint) {
              hint->FirstLast.EXCL(lastHint);
              GetDocument()->UndoMem.AddToMem(hint);
              GetDocument()->UpdateDoc(this, false, hint);
              GetDocument()->ConcernForms(hint);
              GetDocument()->ConcernExecs(hint);
              m_hitemDrop = 0;
              GetDocument()->SetLastHint();
            }
            ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
            return;
          }
          else {
            if (action == QDropEvent::Move) {
              d4 = ((CMainItemData*)itemDragParent->getItemData())->synEl;
              if (GetDocument()->hasHint)
                d4 = GetDocument()->IDTable.GetVar(TID((*(LavaDECL**)d4)->OwnID,0),idType);
              if (CollectDECL->FullName.l)
                str2 = new DString(CollectDECL->FullName);
              else
                str2 = 0;
              FIRSTLAST(GetDocument(), firstlast);
              if (mdh)
                firstlast.INCL(multiDocHint);
              hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), firstlast, (DWORD)/*dragView->*/CollectDECL, (DWORD)str2, (DWORD)/*dragView->*/CollectPos, d4, (DWORD)dropDECL->ParentDECL,0,0, (DWORD)RefacCase);
              GetDocument()->UpdateDoc(this, false, hint);
              //drawTree = true;
              //DrawTree(p_myDECL, false, false);
              GetDocument()->UpdateNo++; //the virtual tables are calculated in DrawTree
              hasDragged = true;
              delete hint; //??
            }
          }
        }
      }//canDrag && canDrop
      if (canDrop) {
        if (clipDefType == DragEnum) {
          hint = ChangeEnum(declClip, m_hitemDrop, false, true);
          hint->FirstLast.EXCL(lastHint);
          GetDocument()->UndoMem.AddToMem(hint);
        }
        else {
          if (dropType == TIType_DECL) {
            pos = GetPos(0, m_hitemDrop);
            itemDropP = (CTreeItem*)m_hitemDrop->parent();
            if (hasDragged && (itemDropP == itemDragParent)) {
              if (pos > CollectPos) {
                for (che = (CHE*)declClip->NestedDecls.first; che; che = (CHE*)che->successor)
                  pos--;
              }
            }
            itemDat = (CMainItemData*)itemDropP->getItemData();
            d4 = itemDat->synEl;
          }
          else {
            //as child inserted
            pos = GetPos(0, 0);
            d4 = ((CMainItemData*)m_hitemDrop->getItemData())->synEl;
          }
          if (GetDocument()->hasHint)
            d4 = GetDocument()->IDTable.GetVar(TID((*(LavaDECL**)d4)->OwnID,0),idType);
          if ((*(LavaDECL**)d4)->FullName.l) {
            str2 = new DString( (*(LavaDECL**)d4)->FullName);
            declClip->FullName = *str2;
          }
          else {
            str2 = 0;
            declClip->FullName.Reset(0);
          }
          if (action == QDropEvent::Move)
            com = CPECommand_Move;
          else
            com = CPECommand_Insert;
          DWORD d6 = 0;
          if (!canDrag || mdh)
            d6 = (DWORD)Clipdata->ClipTree;
          if (!canDrag && (action == QDropEvent::Move) || mdh) {
            firstlast.INCL(multiDocHint);
            if (RefacCase == noRefac)
              ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
          }
          FIRSTLAST(GetDocument(), firstlast);
          hint = new CLavaPEHint(com, GetDocument(), firstlast, (DWORD)declClip, (DWORD)str2, (DWORD)pos, d4, (DWORD)dragParent,
                                d6, (DWORD)Clipdata->docPathName, (DWORD)RefacCase);
          delHint = hint;
          Clipdata->ClipTree = 0;
        }
        Clipdata->docPathName = 0;
        if (canDrag)
          m_hitemDrag = 0;
        m_hitemDrop = 0;
        GetDocument()->UpdateDoc(this, false, hint);
        while(vtHints) {
          hint = (CLavaPEHint*)vtHints->data;
          if (DragDoc != GetDocument())
            GetDocument()->IDTable.ChangeIDFromTab(((CHETID*)((LavaDECL*)hint->CommandData1)->Supports.first)->data.nID);
          FIRSTLAST(hint->fromDoc, firstlast);
          hint->FirstLast = firstlast;
          ((CLavaPEDoc*)hint->fromDoc)->UndoMem.AddToMem(hint);
          ((CLavaPEDoc*)hint->fromDoc)->UpdateDoc(this, false, hint);
          che = (CHE*)vtHints->successor;
          vtHints->data = 0;
          delete vtHints;
          vtHints = che;
        }
        if ((clipDefType == DragIO) || (clipDefType == DragEnum) )
          GetDocument()->ConcernForms(hint);
        if (clipDefType == DragIO)
          if (d4)
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
        GetDocument()->ConcernExecs(hint);
        if (RefacCase == publicToPriv)
          GetDocument()->SetCom8();
        if (RefacCase == privToPublic)
          DragDoc->SetCom8();
        GetDocument()->SetLastHints(true, (action == QDropEvent::Move) || !hasDragged
                              || (RefacCase != privToPublic) && (RefacCase != noRefac));
        if (delHint)
          delete delHint;
      }//canDrop
//--    }
//--  }
  ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
  if (canDrag)
    /*dragView->*/m_hitemDrag = 0;
  //if (m_hitemDrop)
  //  m_tree->setItemSelected(m_hitemDrop, false);
  m_hitemDrop = 0;
  if (DragDoc && (DragDoc != GetDocument())) {
    ((CLavaPEView*)DragDoc->MainView)->m_hitemDrag = 0;
    DragDoc->DragView = 0;
    if (((CLavaPEView*)DragDoc->MainView)->CollectDECL)
      ((CLavaPEView*)DragDoc->MainView)->DeleteDragChain();

  }
  if (!canDrop) {
    if (CollectDECL)
      DeleteDragChain();
  }
  if (Clipdata) {
    Clipdata->Destroy();
    delete Clipdata;
  }
  Clipdata = 0;
  DropPosted = false;
  ((CLavaPEView*)DragDoc->MainView)->DropPosted = false;
}

void CLavaPEView::OnEditSelItem(CTreeItem* item, bool clickedOnIcon)
{
  if (!item)
    return;
  CLavaPEHint *hint;
  TisOnWhat isonwhat;
  LavaDECL *itdDECL, *decl, *ppDECL;
  CMainItemData *itd = (CMainItemData*)item->getItemData();
  int okBox;
  DString *name;
  bool onEnumsel = false;
  if (itd->type == TIType_CHEEnumSel) {
    item = (CTreeItem*)item->parent();
    item = (CTreeItem*)item->parent();
    itd = (CMainItemData*)item->getItemData();
    onEnumsel = true;
  }
  else
    if (itd->type == TIType_EnumItems) {
      item = (CTreeItem*)item->parent();
      itd = (CMainItemData*)item->getItemData();
      onEnumsel = true;
    }
  if ((itd->type != TIType_DECL)
  && (itd->type != TIType_Exec)
  && (itd->type != TIType_Ensure)
  && (itd->type != TIType_Require))
    return;
  if (itd->type == TIType_Exec) {
    OnShowExec(ExecDef);
    return;
  }
  else if (itd->type == TIType_Ensure) {
    OnShowExec(Ensure);
    return;
  }
  else if (itd->type == TIType_Require) {
    OnShowExec(Require);
    return;
  }
  if (itd->type == TIType_DECL) {
    itdDECL = *(LavaDECL**)itd->synEl;
    if (clickedOnIcon) {
      Gotodef(item);
      return;
    }
    else
      if (!myInclView)
        isonwhat = isOnFormProps;
      else
        isonwhat = isOnEdit;
    decl = NewLavaDECL();
    *decl = *itdDECL;
    decl->DECLError1.Destroy();
    ppDECL = decl->ParentDECL;
    m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );//ensureItemVisible(item);
    if (isonwhat == isOnFormProps) {
      if (decl->DeclType == FormText) {
        CFormTextBox* fbox = new CFormTextBox(decl, this);
        okBox = fbox->exec();
        delete fbox;
      }
      else {
       // CLavaPEWizard* dtwz = new CLavaPEWizard(GetDocument(), (LavaDECL**)itd->synEl/*itdDECL*/, "LavaPE Wizard", isonwhat, decl, this);
       // okBox = dtwz->exec();
       // delete dtwz;
       // okBox = QDialog::Rejected;
      }
    }
    else {
      bool build=false;
      if (onEnumsel) {
        decl->DeclType = EnumDef; //call the EnumBox!
        decl->TreeFlags.INCL(ItemsExpanded);
      }
      okBox = CallBox(decl, itdDECL, GetDocument(), false, build, this);
    }
    if (okBox == QDialog::Accepted) {
      if (decl->Annotation.ptr && decl->Annotation.ptr->FA.ptr)
          ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->BType =
                                  ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(GetDocument()->mySynDef, decl);
      name = new DString(itdDECL->FullName);  //bisheriger Name

      hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      if (myInclView) {
        GetDocument()->ConcernForms(hint);
        GetDocument()->ConcernImpls(hint, ppDECL);
        GetDocument()->ConcernExecs(hint);
      }
      GetDocument()->SetLastHint();
    }
    else
      delete decl;
  }
}//OnEditSelItem

void CLavaPEView::OnInitialUpdate()
{
  SelItem =0;
  SelType = NoDef;
  VisibleDECL = 0;
  QSize sz = size();
  if (((CLavaPEApp*)wxTheApp)->LBaseData.actHint 
      && (((CLavaPEApp*)wxTheApp)->LBaseData.actHint->com == CPECommand_OpenFormView)
      && !myInclView) {
    CLavaPEHint* pHint = ((CLavaPEApp*)wxTheApp)->LBaseData.actHint;
    myDECL = (LavaDECL*)pHint->CommandData1;
    p_myDECL = (LavaDECL**)pHint->CommandData4;
    myID = TID(myDECL->OwnID, 0);
    myMainView = (CLavaPEView*)pHint->CommandData2;
    InitFinished = true;
    OnUpdate(myMainView, 0, pHint);
  }
  else {
    ((CTreeFrame*)GetParentFrame())->SetModified(false);
    OnUpdate(NULL, CHLV_fit, 0);
    if (GetDocument()->modified) {
      QMessageBox::critical(this, qApp->name(), "This Lava file was modified by automatically generated corrections after opening",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      GetDocument()->Modify(true);
    }
    CExecChecks* ch = new CExecChecks(GetDocument(), true);
    delete ch;
    if (GetDocument()->nErrors || GetDocument()->nPlaceholders)
      GetDocument()->ShowErrorBox(true);
    ((CLavaPEApp*)wxTheApp)->debugThread.restoreBrkPoints(GetDocument());
  }
  sz = size();
}


bool CLavaPEView::OnInsert(TDeclType eType, LavaDECL *iDECL)
{
  CMainItemData *data, *ppdata;
  CTreeItem *par, *ppar, *collNode = NULL, *item;
  SynFlags first = (const unsigned long)1;
  bool asChild, asSibling, callBox = false, isOnNew;
  LavaDECL *decl, *parDECL, *refDECL = 0, *parentT, *setDECL; //*inEl
  CLavaPEHint *hint, *Sethint;
  CHE *cheIOEl, *che;
  DWORD d4 = 0;
  DString *str2 = 0, newName, *setStr2;
  int pos;
  TID setID;

  item = (CTreeItem*)m_tree->currentItem();
  if (!item) {
    QMessageBox::critical(this, qApp->name(),IDP_NoInsertionPos,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  if (myInclView && (item == m_tree->RootItem)) {
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  if (!myInclView && (eType != FormText)) {
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  data = (CMainItemData*)item->getItemData();
  if (data->type == TIType_DECL) {
    switch ((*(LavaDECL**)data->synEl)->DeclType) {
    case FormText:
    case Attr:
    case Function:
      asSibling = (eType == Attr) || (eType == Function) || (eType == FormText);
      break;
    case IAttr:
      asSibling = (eType == Attr);
      if (asSibling)
        eType = IAttr;
      break;
    case OAttr:
      asSibling = (eType == Attr);
      if (asSibling)
        eType = OAttr;
      break;
    default:
      asSibling = (eType != ExecDef) && (eType != Ensure) && (eType != Require) && (eType != Attr) && (eType != FormText);
    }
  }
  else
    asSibling = false;
  if (!asSibling) {
    switch (data->type) {
    case TIType_Defs:
      asChild = (eType != Attr) && (eType != Function) && (eType != ExecDef) && (eType != Ensure) &&(eType != Require);
      break;
    case TIType_Features:
      asChild = (eType == Attr) || (eType == Function) || (eType == FormText);
      break;
    case TIType_VTypes:
      asChild = (eType == VirtualType);
      break;
    case TIType_Input:
      asChild = (eType == Attr);
      if (asChild) {
        eType = IAttr;
      }
      break;
    case TIType_Output:
      asChild = (eType == Attr);
      if (asChild)
        eType = OAttr;
      break;
    case TIType_Require:
      asChild = (eType == Require);
      break;
    case TIType_Ensure:
      asChild = (eType == Ensure);
      break;
    case TIType_Exec:
      asChild = (eType == ExecDef);
      break;
   default:
      asChild = false;
    }
  }
  else
    asChild = false;
  if (asChild)
    par = item;
  else {
    if (asSibling) {
      par= (CTreeItem*)item->parent();
      data = (CMainItemData*)par->getItemData();
    }
    else {
      par = item;
      while ((par != m_tree->RootItem) 
            && !((data->type == TIType_DECL)
            && ( ( (*(LavaDECL**)data->synEl)->DeclDescType == StructDesc)
               || ( (*(LavaDECL**)data->synEl)->DeclDescType == EnumType)
               || (eType == ExecDef) && ( (*(LavaDECL**)data->synEl)->DeclType == FormDef) ))) {
        par= (CTreeItem*)par->parent();
        data = (CMainItemData*)par->getItemData();
      }
      if (myInclView && (eType == ExecDef)
          && ( (*(LavaDECL**)data->synEl)->DeclType == FormDef)) {
        m_tree->setCurAndSel(par);
        OnShowGUIview();
        return true;
      }
      par = getSectionNode(par, eType);
      collNode = par;
      data = (CMainItemData*)par->getItemData();
      if (par) {
        item = par;
        asChild = true;
      }
      else {
        QMessageBox::critical(this, qApp->name(),IDP_NoInsertionPos,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
      }
    }
  }
  if (iDECL)
    decl = iDECL;
  else {
    decl = NewLavaDECL();
    if ((eType <= FormDef) || (eType == SetDef) || (eType == EnumDef))
      decl->TreeFlags.INCL(hasEmptyOpt);
    decl->TreeFlags.INCL(isExpanded);
  }
  if (eType == SetDef) {
    eType = Interface;
    decl->SecondTFlags.INCL(isSet);
  }
  decl->DeclType = eType;

  ppar= (CTreeItem*)par->parent();
  ppdata = (CMainItemData*)ppar->getItemData();

  if (iDECL  && decl->SecondTFlags.Contains(funcImpl) /*&& !dragParent*/) {
    // a function implementation or it's IO-members cannot be inserted in this way
    decl->SecondTFlags.EXCL(funcImpl);
    decl->TypeFlags.EXCL(isPropGet);
    decl->TypeFlags.EXCL(isPropSet);
    decl->Supports.Destroy();
    //decl->RefID.nID = -1;
    if (decl->DeclType == Function)  {
      if (decl->op != OP_noOp) {
        decl->LocalName = DString("copy_of_operator_") + LBaseData->OpFuncNames[decl->op];
        decl->op = OP_noOp;
      }
      cheIOEl = (CHE*)decl->NestedDecls.first;
      while (cheIOEl) {
        if ( ((LavaDECL*)cheIOEl->data)->SecondTFlags.Contains(funcImpl)) {
          ((LavaDECL*)cheIOEl->data)->SecondTFlags.EXCL(funcImpl);
          //((LavaDECL*)cheIOEl->data)->RefID.nID = -1;
          ((LavaDECL*)cheIOEl->data)->Supports.Destroy();
        }
        cheIOEl = (CHE*)cheIOEl->successor;
      }
    }
  }
  if (((decl->DeclType == IAttr) || (decl->DeclType == OAttr))
    && (*(LavaDECL**)ppdata->synEl)->SecondTFlags.Contains(funcImpl)) {
    //no insertion of io into a function implementation
    delete decl;
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }

  if ((data->type == TIType_Exec) || (data->type == TIType_Ensure) || (data->type == TIType_Require)) {
    if (iDECL) {
      hint = new CLavaPEHint(CPECommand_Exec, GetDocument(), first, (DWORD)decl);
      GetDocument()->UndoMem.AddToMem(hint);
    }
    else {
      delete decl;
      m_tree->setCurAndSel(par);
      OnShowExec(eType);
    }
    return true;
  }
  else {
    if ((data->type == TIType_Defs)
        || (data->type == TIType_VTypes)
        || (data->type == TIType_Features)
        || (data->type == TIType_Input)
        || (data->type == TIType_Output)
      ) {
      decl->DeclType = eType;
    }
    else {
      delete decl;
      QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return false;
    }
  }
  if ((ppdata->type != TIType_DECL) && (ppar != m_tree->RootItem)) {
      delete decl;
      QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return false;
  }
  decl->ParentDECL = *(LavaDECL**)ppdata->synEl;
  if (!myInclView || (ppar != m_tree->RootItem)
       && (ppar->parent() != m_tree->RootItem)) {
    decl->FullName = (*(LavaDECL**)ppdata->synEl)->FullName;
    if (iDECL) {
      decl->FullName += ddppkt;
      decl->FullName += iDECL->LocalName;
    }
  }
  else {
    if (iDECL) {
      decl->FullName = decl->LocalName;
    }
  }
  if (iDECL && !decl->SecondTFlags.Contains(funcImpl)) {
    isOnNew = false;
  }
  else {
    isOnNew = true;
    switch (eType) {
    case EnumDef:
      decl->DeclDescType = EnumType;
      decl->SecondTFlags.INCL(isEnum);
      decl->ParentDECL->TreeFlags.INCL(DefsExpanded);
      break;
    case CompObj:
    case FormDef:
    case Impl:
    case Interface:
    case CompObjSpec:
    case Initiator:
    case Package:
      decl->DeclDescType = StructDesc;
      decl->ParentDECL->TreeFlags.INCL(DefsExpanded);
      break;
    case Function:
      decl->DeclDescType = StructDesc;
      decl->ParentDECL->TreeFlags.INCL(MemsExpanded);
      break;
    case Attr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(MemsExpanded);
      break;
    case IAttr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(InExpanded);
      break;
    case OAttr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(OutExpanded);
      break;
     case VirtualType:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(ParaExpanded);
      break;
     case FormText:
      decl->DeclDescType = LiteralString;
        break;
   default:
      decl->DeclDescType = Undefined;
    }
  }
  bool buildSet=false;
  if (!iDECL || callBox) {
    int okBox = CallBox(decl, 0, GetDocument(), isOnNew, buildSet, this, asChild);
    if (okBox == QDialog::Accepted) {
      if (decl->Annotation.ptr && decl->Annotation.ptr->FA.ptr)
         ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->BType =
            ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(GetDocument()->mySynDef, decl);
    }
    else {
      if (collNode)
        if ((CTreeItem*)collNode->child(0) == NULL) {
          DeleteItemData(collNode);
          delete collNode;
        }
      delete decl;
      return false;
    }
  }
  if (asChild)
    pos = GetPos(0, 0);
  else
    pos = GetPos(0, item);
  d4 = ppdata->synEl;
  if ((*(LavaDECL**)ppdata->synEl)->FullName.l)
    str2 = new DString((*(LavaDECL**)ppdata->synEl)->FullName);
  parDECL = *(LavaDECL**)ppdata->synEl;
  decl->ParentDECL = parDECL;
  hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), first, (DWORD) decl, (DWORD)str2, (DWORD)pos, d4/*, (DWORD)dragParent, (DWORD)clipTree, (DWORD)docPathName*/);
  GetDocument()->UndoMem.AddToMem(hint);
  GetDocument()->UpdateDoc(this, false, hint);
  if (buildSet) {
    che = (CHE*)parDECL->NestedDecls.first;
    while (che && ( (LavaDECL*)che->data != decl))
      che = (CHE*)che->successor;
    if (che) {
      if (decl->DeclType == Interface) {
        d4 = (DWORD)&che->data;
        pos = 0;
        parentT = decl;
      }
      else
        parentT = decl->ParentDECL;
      setID = TID(((LavaDECL*)hint->CommandData1)->OwnID, 0);
      setDECL = GetDocument()->MakeSet(setID);
      setDECL->ParentDECL = parentT;
      newName = DString("Set");
        /*;
      while (CheckNewName(newName.c, setDECL, GetDocument(), 0))
        newName += DString("_S");*/
      setDECL->LocalName = newName;
      setDECL->TreeFlags.INCL(isExpanded);
      setDECL->TreeFlags.INCL(ParaExpanded);
      pos++;
      setStr2 = new DString(decl->FullName);
      Sethint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)0, (DWORD) setDECL, (DWORD)setStr2, (DWORD)pos, d4);
      GetDocument()->UndoMem.AddToMem(Sethint);
      GetDocument()->UpdateDoc(this, false, Sethint);
    }
  }
  if (myInclView) {
    GetDocument()->ConcernForms(hint);
    if (d4)
      GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
    GetDocument()->ConcernExecs(hint);
  }
  GetDocument()->SetLastHint();
  return true;
}

void CLavaPEView::OnItemcollapsed(QTreeWidgetItem* item)
{
  SetTreeFlags((CTreeItem*)item, false);
}

void CLavaPEView::OnItemexpanded(QTreeWidgetItem* item)
{
  SetTreeFlags((CTreeItem*)item, true);
}


void CLavaPEView::OnNextEC(CTreeItem* itemStart, bool onErr)
{
  if (!itemStart)
    return;
  CTreeItem* item2 = itemStart;
  if (((CLavaMainFrame*)wxTheApp->m_appWindow)->UtilitiesHidden
      || onErr && (((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->ActTab != tabError)
      || !onErr && (((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->ActTab != tabComment))
    if (PutEC(item2, onErr))
      return;

  CTreeItem* item1 = (CTreeItem*)item2->child(0);
  if (!item1)
    item1 = (CTreeItem*)item2->nextSibling();
  while (!item1 && item2) {
    item2 = (CTreeItem*)item2->parent();
    if (item2)
      item1 = (CTreeItem*)item2->nextSibling();
  }
  if (!item1)
    item1 = (CTreeItem*)m_tree->RootItem;
  while (item1) {
    if (item1 == itemStart) {
      if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return;
    }
    if (PutEC(item1, onErr))
      return;

    item2 = (CTreeItem*)item1->child(0);
    while (item2) {
      if (item2 == itemStart) {
        if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return;
      }
      if (PutEC(item2, onErr))
        return;

      item1 = item2;
      item2 = (CTreeItem*)item2->child(0);
    }
    item2 = item1;
    item1 = (CTreeItem*)item2->nextSibling();
    while (!item1 && item2) {
      item2 = (CTreeItem*)item2->parent();
      if (item2)
        item1 = (CTreeItem*)item2->nextSibling();
    }
    if (!item1) 
      item1 = (CTreeItem*)m_tree->RootItem;
  }

}

void CLavaPEView::OnPrevEC(CTreeItem* itemStart, bool onErr)
{
  CTreeItem *item1 = itemStart, *item2 = 0, *item3;
  while (!item2 && item1) {
    item2 = GetPrevSiblingItem(item1);
    if (!item2) { //if first child then go up to parent
      if (item1 != m_tree->RootItem) {
        item1 = (CTreeItem*)item1->parent();
        if (item1 == itemStart) {
          if (onErr)
            QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          else
            QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          return;
        }
        if (item1) {
          if (PutEC(item1, onErr))
            return;
          item2 = GetPrevSiblingItem(item1);
        }
      }
      else
        item2 = item1;
    }
    item3 = item2;
    while (item3) { //first child ..
      item2 = item3;
      item3 = (CTreeItem*)item2->child(0);
      item1 = item3;
      while (item1) { //then it's last sibling
        item2 = item1;
        item3 = item1;
        item1 = (CTreeItem*)item1->nextSibling();
      }
    }
    if (item2) {
      item1 = item2;
      if (item1 == itemStart) {
        if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return;
      }
      item2 = 0;
      if (PutEC(item1, onErr))
        return;
    }
  }
}

void CLavaPEView::OnRclick(QTreeWidgetItem* itemHit)
{

  if (itemHit) {
    m_tree->setItemSelected(itemHit, true);
    //PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptAction.x, ptAction.y, this);
  }
}

void CLavaPEView::setSelPost(QTreeWidgetItem* selItem)
{
  m_tree->ResetSelections();
  m_tree->setCurAndSel(selItem);
}


void CLavaPEView::OnSelchanged(QTreeWidgetItem* selItem, QTreeWidgetItem* )
{
  bool ok = true, sameCat, collected = false;
  CTreeItem *itemDrag, *item;
  LavaDECL *itemDECL;

  
//  QTreeWidgetItem* selItem = m_tree->currentItem();
  if (!selItem)
    return;
  if (!m_tree->isItemSelected(selItem)) //selItem->isSelected())
    return;
  if (lastCurrent && (m_tree->withControl && !m_tree->withShift || m_tree->withShift && !m_tree->withControl)) {
    if (!CollectDECL) {
      CanDelete = true;
      CollectDECL = NewLavaDECL();
      CollectDECL->DeclType = NoDef;
      multiSelectCanceled = !AddToDragChain(lastCurrent);
      CollectPos = GetPos(lastCurrent, 0);
    }
    if (m_tree->withShift) {
      itemDrag = lastCurrent;
      while (itemDrag && (itemDrag != selItem)) {
        itemDrag = (CTreeItem*)itemDrag->nextSibling();
        if (multiSelectCanceled || !AddToDragChain(itemDrag)) {
          multiSelectCanceled = true;
          itemDrag = 0;
        }
      }
      if (!itemDrag || (itemDrag != selItem)
          || !CollectDECL->NestedDecls.first
          && (!CollectDECL->EnumDesc.ptr || !((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.Items.first)) {
        multiSelectCanceled = true;
        QApplication::postEvent(this, new CustomEvent(IDU_LavaPE_setSel, (void*)selItem));
        DeleteDragChain();
        m_tree->withShift = false;
        QApplication::postEvent(this, new CustomEvent(IDU_LavaPE_setSel, (void*)selItem));
        /*m_tree->ResetSelections();
        m_tree->setCurAndSel(selItem);*/
      }
      else
        collected = true;
    }
    else {//withControl
      sameCat = lastCurrent->parent() == selItem->parent();
      if (multiSelectCanceled || !AddToDragChain((CTreeItem*)selItem, true, sameCat)) {
        multiSelectCanceled = true;
        DeleteDragChain();
        m_tree->withControl = false;
        QApplication::postEvent(this, new CustomEvent(IDU_LavaPE_setSel, (void*)selItem));
        /*m_tree->ResetSelections();
        m_tree->setCurAndSel(selItem);*/
      }
      else
        collected = true;
    }
  }
  else {
    if (!m_hitemDrag)
      DeleteDragChain();
  }
  if (m_hitemDrop)
    return;
//  ParItemSel = 0;
  ItemSel = (CTreeItem*)selItem;
  if (ItemSel) {
    if (myVTView)
      ((CVTView*)myVTView)->activeInt = false;
    DataSel = (CMainItemData*)ItemSel->getItemData();
    if (DataSel) {
      item = ItemSel;
      if (myInclView) {
        if (DataSel->type == TIType_DECL) {
          itemDECL = *(LavaDECL**)DataSel->synEl;
          if ((itemDECL->DeclType == Interface)
              || (itemDECL->DeclType == Package)
              && itemDECL->LocalName.l && itemDECL->Supports.first) {
            ((CVTView*)myVTView)->activeInt = true;
            GetDocument()->OpenVTView((LavaDECL**)DataSel->synEl, (unsigned long)1);
            if (itemDECL->DECLError1.first)
              ItemSel->SetItemMask(itemDECL->DECLError1.first || itemDECL->DECLError2.first,
                itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
          }
          else
            ((CVTView*)myVTView)->activeInt = itemDECL->isInSubTree(((CVTView*)myVTView)->myDECL);
          if (ItemSel != m_tree->RootItem) {
            ItemSel = (CTreeItem*)ItemSel->parent();
            DataSel = (CMainItemData*)ItemSel->getItemData();
          }
        }
        if (ItemSel != m_tree->RootItem) {
          GroupType = DataSel->type;
          ItemSel = (CTreeItem*)ItemSel->parent();
          DataSel = (CMainItemData*)ItemSel->getItemData();
          if (DataSel) {
            if (!((CVTView*)myVTView)->activeInt)
              ((CVTView*)myVTView)->activeInt = (*(LavaDECL**)DataSel->synEl)->isInSubTree(((CVTView*)myVTView)->myDECL);
            DefTypeSel = (*(LavaDECL**)DataSel->synEl)->DeclType;
          }
          else
            ItemSel = 0;
        }
        else
          ItemSel = 0;
      }
      else { //form
        if ((DataSel->type == TIType_DECL))
          GetDocument()->OpenWizardView(this, (LavaDECL**)DataSel->synEl/*, (unsigned long)1*/);
        if (!inSync && myFormView && (DataSel->type == TIType_DECL)) {
          //if (ItemSel != m_tree->child(0)) 
            ((CLavaGUIView*)myFormView)->SyncForm(*(LavaDECL**)DataSel->synEl);
        }
        else
          inSync = false;
        ItemSel = 0;
      }
      if (this == (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView())
        SetErrAndCom(item);
    }
    else
      ItemSel = 0;
  }
  else
    ItemSel = 0;
  lastCurrent = (CTreeItem*)selItem;
}

void CLavaPEView::OnShowSpecialView(TDeclType exprType)
{
  bool hasView = false;
  CMainItemData * itd;
  CTreeItem* root = (CTreeItem*)m_tree->RootItem;
  CTreeItem* item;
  if (!myInclView && (exprType == FormDef)) {
    item = root;
    hasView = true;
  }
  else {
    item = (CTreeItem*)m_tree->currentItem();
    if (!item && !myInclView)
      item = root;
  }
  if (item)
    itd = (CMainItemData*)item->getItemData();
  if ((exprType == ExecDef) && (itd->type == TIType_Exec)
      || (exprType == Ensure) && (itd->type == TIType_Ensure)
      || (exprType == Require) && (itd->type == TIType_Require))
    hasView = GetDocument()->OpenExecView(GetDocument()->GetExecDECL(*(LavaDECL**)itd->synEl,exprType));
  else {
    if ((itd->type == TIType_DECL)
        && (((*(LavaDECL**)itd->synEl)->DeclType == Impl)
       && (*(LavaDECL**)itd->synEl)->TypeFlags.Contains(isGUI)
          || ((*(LavaDECL**)itd->synEl)->DeclType == FormDef)))
        hasView = GetDocument()->OpenGUIView((LavaDECL**)itd->synEl);
  }
  if (!hasView)
    if (exprType == FormDef)
      QMessageBox::critical(this, qApp->name(),IDP_NoFormView,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    else
      QMessageBox::critical(this, qApp->name(),IDP_NoExecView,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
}

void CLavaPEView::OnUpdate(wxView* pSender, unsigned undoRedoCheck, QObject* pHint)
{
  if (m_hitemDrag) //in drag drop no tree drawing
    return;
  CLavaPEHint* hint = (CLavaPEHint*) pHint;
  bool lastUpdate = (undoRedoCheck == 3) ||
                     hint && ((undoRedoCheck == 1) && hint->FirstLast.Contains(firstHint)
                            || (undoRedoCheck != 1) && hint->FirstLast.Contains(lastHint));
  bool foreignHint = hint
                    && (hint->com != CPECommand_OpenFormView)
                    && (hint->com != CPECommand_Insert)
                    && (hint->com != CPECommand_Delete)
                    && (hint->com != CPECommand_Change)
                    && (hint->com != CPECommand_Include)
                    && (hint->com != CPECommand_ChangeInclude)
                    && (hint->com != CPECommand_Exclude)
                    && (hint->com != CPECommand_Comment)
                    && (hint->com != CPECommand_FromOtherDoc);
  bool inUndoRedo = (undoRedoCheck == 1) || (undoRedoCheck == 2);
  updateTree = updateTree || !foreignHint;
  drawTree = drawTree || (undoRedoCheck == CHLV_fit)
                      || hint && (hint->com == CPECommand_FromOtherDoc) && !hint->FirstLast.Contains(noDrawHint)
                        && !((CLavaPEApp*)wxTheApp)->inTotalCheck;
  if (!updateTree)
    return;
  updateTree = !lastUpdate;
  CLavaPEDoc* pDoc = GetDocument();
  if (pHint && hint->FirstLast.Contains(firstHint) && !foreignHint) {
    VisibleDECL = 0;
    if (!pDoc || !pDoc->mySynDef)
      return;
    if (pHint) {
      switch (hint->com) {
        case CPECommand_FromOtherDoc:
          if (VisibleDECL)
            VisibleDECL->WorkFlags.INCL(selAfter);
          break;
        case CPECommand_Comment:
          if (hint->CommandData3) {
            ((CHEEnumSelId*)hint->CommandData1)->data.selItem = true;
          }
          else
            ((LavaDECL*)hint->CommandData1)->WorkFlags.INCL(selAfter);
          break;
        default: ;
      }
    }
  }
  if (!myInclView) {
    if (hint) {
      if (hint->com == CPECommand_OpenFormView) {
        if (myDECL != (LavaDECL*)hint->CommandData1)
          return;
        drawTree = true;
      }
      else {
        if (!foreignHint) {
          DString *pHintName=0;
          if ((hint->com == CPECommand_Delete)
              && myDECL->isInSubTree((LavaDECL*)hint->CommandData1) ) {
            delete GetParentFrame();
            return;
          }
          if (( hint->com == CPECommand_Comment)  && hint->CommandData3)
            pHintName = &((LavaDECL*)hint->CommandData3)->FullName;
          else
            if (hint->CommandData1)
              pHintName = &((LavaDECL*)hint->CommandData1)->FullName;
          if (pHintName && isPartOf(myDECL->FullName, *pHintName)) {
            TIDType tt;
            p_myDECL = (LavaDECL**)GetDocument()->IDTable.GetVar(myID, tt);
            if ((tt != globalID) || !p_myDECL || !*p_myDECL)
              return;
            else {
              myDECL = *p_myDECL;
            }
          }
        }
      }
    }
    CTreeView::OnUpdate(pSender, undoRedoCheck, pHint);
    if ( hint && (hint->com != CPECommand_Comment)
              && ( (undoRedoCheck == 1) && hint->FirstLast.Contains(firstHint)
                 || (undoRedoCheck != 1) && hint->FirstLast.Contains(lastHint))
          || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_noCheck) ) {
      myDECL->DECLError2.Destroy();
      drawTree = drawTree || GetDocument()->UndoMem.DrawTree;
      DrawTree(GetDocument()->GetFormpDECL(myDECL), inUndoRedo);
      if (!drawTree) {
        CTreeItem* item = (CTreeItem*)m_tree->currentItem();
        if (item) {
          QApplication::postEvent(myFormView, new CustomEvent(IDU_LavaPE_SyncForm, (void*)*(LavaDECL**)((CMainItemData*)item->getItemData())->synEl));
          GetDocument()->OpenWizardView(this, (LavaDECL**)((CMainItemData*)item->getItemData())->synEl/*, (unsigned long)1*/);
        }
      }
    }
  } //forms
  else {
    if (hint && ((hint->com == CPECommand_OpenFormView)
         || (hint->com == CPECommand_OpenSelView)))
      return;
    p_myDECL = &((CHESimpleSyntax*)pDoc->mySynDef->SynDefTree.first)->data.TopDef.ptr;
    myDECL = *p_myDECL;
    if ((undoRedoCheck == 1) && hint && hint->FirstLast.Contains(firstHint)
        || (undoRedoCheck != 1) && hint && hint->FirstLast.Contains(lastHint)
        || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_noCheck)) {
      if (undoRedoCheck < CHLV_noCheck)
        undoRedoCheck = CHLV_inUpdateLowFinal;
      CTreeView::OnUpdate(pSender, undoRedoCheck, pHint);
      ((CTreeFrame*)GetParentFrame())->showIt = ((CTreeFrame*)GetParentFrame())->showIt
                                                || !((CLavaPEApp*)wxTheApp)->inTotalCheck
                                                || GetDocument()->hasErrorsInTree
                                                || GetDocument()->nErrors
                                                || GetDocument()->nPlaceholders;
      drawTree = drawTree || GetDocument()->UndoMem.DrawTree;
      DrawTree(0, inUndoRedo, true, undoRedoCheck);
      m_tree->update();
      //if ((undoRedoCheck == CHLV_fit)  //the initial open
      //  && (GetDocument()->nTreeErrors || GetDocument()->nErrors || GetDocument()->nPlaceholders))
      //  GetDocument()->ShowErrorBox(true);
    }
  }
}//OnUpdate


void CLavaPEView::OnVkreturn()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    TDeclType etype;
    switch (data->type) {
    case TIType_Require:
    case TIType_Ensure:
    case TIType_Exec:
      OnEditSelItem(item, false);
      break;
    case TIType_DECL:
      if (myInclView) {
        etype = (*(LavaDECL**)data->synEl)->DeclType;
        if ((etype == IAttr) || (etype == OAttr))
          etype = Attr;
      }
      else
        etype = FormText;
      OnInsert(etype);
      break;
    case TIType_Features:
    case TIType_Input:
    case TIType_Output:
      if (myInclView)
        OnInsert(Attr);
      else
        OnInsert(FormText);
      break;
    case TIType_VTypes:
      if (myInclView)
        OnInsert(VirtualType);
      break;
    case TIType_CHEEnumSel:
    case TIType_EnumItems:
      if (myInclView)
        OnNewEnumItem();
      break;
    default: ;
    }
  }
}


bool CLavaPEView::PutEC(CTreeItem* item, bool onErr)
{
  CMainItemData *dataD = (CMainItemData*)item->getItemData();
  LavaDECL* errDECL;
  if ((dataD->type == TIType_DECL) || (dataD->type == TIType_Exec)
     || (dataD->type == TIType_Ensure) || (dataD->type == TIType_Require)) {
    errDECL = *(LavaDECL**)dataD->synEl;
    if (dataD->type != TIType_DECL)
      errDECL = GetExecDECL(item);
    if (errDECL) {
      if (onErr && (errDECL->DECLError1.first || errDECL->DECLError2.first)) {
        m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );//ensureItemVisible(item);
        m_tree->setCurAndSel(item);
        CheckAutoCorr(errDECL);
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetErrorOnUtil(errDECL);
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetTab(tabError);
        return true;
      }
      else
        if (!onErr && errDECL->DECLComment.ptr) {
          ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetComment(errDECL->DECLComment.ptr->Comment, !errDECL->DECLError1.first && !errDECL->DECLError2.first);
          ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetTab(tabComment);
          m_tree->setCurAndSel(item);
          m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );//ensureItemVisible(item);
          return true;
        }
    }
  }
  return false;
}


bool CLavaPEView::Refac(LavaDECL* dropDECL, bool& mdh, CHE*& vtHints)
{
  LavaDECL *dropParent, **pDragParent, *clipDECL, *clipElDECL, **del_d4, **d4, *execDECL,
    *implDECL, *implElDECL, **pDragImplDecl=0, **pDropImplDecl=0, **pDragP, *collectDECL;
  CHE *clipEl, *dropEl, *ioElImpl, *execEl, *execElIf, *implEl, *nextEl, *clipImplEl, *afterEl;
  TID tid;
  CHETID *che;
  int del_pos, pos, collectPos;
  CLavaPEHint *hint, *hintCol=0;
  DString *str2, *docName;
  CTreeItem *itemDropParent, *itemP;
  CLavaPEView* dragView;
  wxDocument *dragImplDoc=0, *dropImplDoc=0;
  CMainItemData *itemData;
  SynFlags firstlast;
  DWORD d6;
  TIDType idType;
  QString mess;
  bool makeMess  = false;

  if (RefacCase == noRefac)
    return false;
  if ((dropDECL->DeclType == Attr) || (dropDECL->DeclType == Function))
    dropParent = dropDECL->ParentDECL;
  else
    dropParent = dropDECL;
  itemDropParent = (CTreeItem*)m_hitemDrop->parent();
  del_d4 = (LavaDECL**)((CMainItemData*)itemDropParent->getItemData())->synEl;
  clipDECL = (LavaDECL*)Clipdata->synEl;
  dragView = (CLavaPEView*)DragDoc->DragView;
  itemData = (CMainItemData*)dragView->m_hitemDrag->getItemData();
  if (itemData->type == TIType_DECL)
    pos = dragView->GetPos(0, dragView->m_hitemDrag);
  else
    pos = dragView->GetPos(0, 0);
  itemP = (CTreeItem*)dragView->m_hitemDrag->parent();
  pDragParent = (LavaDECL**)((CMainItemData*)itemP->getItemData())->synEl;
  if ((DragDoc != GetDocument()) || mdh) {
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
      mdh = true;
    }
  }
  switch (RefacCase) {
  case publicToPriv:
    for (clipEl = (CHE*)clipDECL->NestedDecls.first;
         clipEl; clipEl = (CHE*)clipEl->successor) {
      clipElDECL = (LavaDECL*)clipEl->data;
      if (!clipElDECL->TypeFlags.Contains(defaultInitializer)) {
        if (clipElDECL->DeclType == Attr)
          clipElDECL->Supports.Destroy();
        else {
          del_pos = 1;
          tid = TID(clipElDECL->OwnID, DragINCL);
          for (dropEl = (CHE*)dropParent->NestedDecls.first;
               dropEl && (!((LavaDECL*)dropEl->data)->Supports.first
                || (((CHETID*)((LavaDECL*)dropEl->data)->Supports.first)->data != tid));
               dropEl = (CHE*)dropEl->successor) {
            if ((((LavaDECL*)dropEl->data)->DeclType == Attr) || (((LavaDECL*)dropEl->data)->DeclType == Function))
              del_pos++;
          }
          if (dropEl) {
            dropDECL = (LavaDECL*)dropEl->data;
            execDECL = NewLavaDECL();
            *execDECL = *(LavaDECL*)((CHE*)dropDECL->NestedDecls.last)->data;
            execEl = NewCHE(execDECL);
            clipElDECL->NestedDecls.Append(execEl);
            afterEl = (CHE*)execEl->predecessor;
            execEl = GetDocument()->GetExecChe(dropDECL,Ensure,false);
            execElIf = GetDocument()->GetExecChe(clipElDECL,Ensure,false);
            if (execElIf) {
              afterEl = (CHE*)execElIf->predecessor;
              clipElDECL->NestedDecls.Delete(execElIf);
              makeMess = true;
              mess = QString("Ensure of interface function declaration ")
                + QString(((LavaDECL*)clipEl->data)->LocalName.c) + QString(" will be lost");
            }
            if (execEl) {
              execDECL = NewLavaDECL();
              *execDECL = *(LavaDECL*)execEl->data;
              execEl = NewCHE(execDECL);
              clipElDECL->NestedDecls.Insert(afterEl, execEl);
              afterEl = (CHE*)execEl->predecessor;
            }
            execEl = GetDocument()->GetExecChe(dropDECL,Require,false);
            execElIf = GetDocument()->GetExecChe(clipElDECL,Require,false);
            if (execElIf) {
              afterEl = (CHE*)execElIf->predecessor;
              clipElDECL->NestedDecls.Delete(execElIf);
              if (makeMess) {
                mess[0] = 'e';
                mess = QString("Require and ") + mess;
              }
              else {
               mess = QString("Require of interface function declaration ") + QString(((LavaDECL*)clipEl->data)->LocalName.c) + QString(" will be lost");
               makeMess = true;
              }
            }
            if (execEl) {
              execDECL = NewLavaDECL();
              *execDECL = *(LavaDECL*)execEl->data;
              execEl = NewCHE(execDECL);
              clipElDECL->NestedDecls.Insert(afterEl, execEl);
            }
            if (makeMess) {
              QMessageBox::information(this, qApp->name(),mess);
              makeMess = false;
            }
            clipElDECL->Supports.Destroy();
            /*
            clipElDECL->SecondTFlags.EXCL(overrides);
            clipElDECL->SecondTFlags.EXCL(funcImpl);
            clipElDECL->TypeFlags.EXCL(isProtected);
            clipElDECL->TypeFlags.EXCL(isAbstract);
            clipElDECL->TypeFlags.EXCL(forceOverride);
            clipElDECL->TypeFlags.EXCL(isNative);
            clipElDECL->TypeFlags.EXCL(defaultInitializer);
            */
            clipElDECL->WorkFlags.INCL(skipOnCopy);
            dropDECL->WorkFlags.INCL(skipOnDeleteID);
            str2 = new DString(dropParent->FullName);
            FIRSTLAST(GetDocument(), firstlast);
            hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), firstlast, (DWORD)dropDECL, (DWORD)str2, (DWORD)del_pos, (DWORD)del_d4, (DWORD)dropParent,0,0);
            GetDocument()->UndoMem.AddToMem(hint);
            GetDocument()->UpdateDoc(this, false, hint);
            for (ioElImpl = (CHE*)dropDECL->NestedDecls.first;
                 ioElImpl && ioElImpl->successor
                   && (((LavaDECL*)ioElImpl->data)->DeclDescType != ExecDesc);
                 ioElImpl = (CHE*)ioElImpl->successor) {
              che = new CHETID;
              che->data = TID(-((LavaDECL*)ioElImpl->data)->OwnID, ((CHETID*)((LavaDECL*)ioElImpl->data)->Supports.first)->data.nID); //old ID, new ID
              GetDocument()->IDTable.ChangeTab.Append(che);
            }
          }
        }
      }
    }
    break;

  case privToPublic:
    implDECL = NewLavaDECL();
    *implDECL = *clipDECL;
    implDECL->WorkFlags.INCL(fromPrivToPub);
    implEl = (CHE*)implDECL->NestedDecls.first;
    for (clipEl = (CHE*)clipDECL->NestedDecls.first; clipEl;
         clipEl = (CHE*)clipEl->successor) {
      implElDECL = (LavaDECL*)implEl->data;
      if (implElDECL->DeclType == Attr) {
        nextEl = (CHE*)implEl->successor;
        implDECL->NestedDecls.Delete(implEl);
        implEl = nextEl;
      }
      else {
        che = new CHETID;
        che->data = TID(implElDECL->OwnID, DragINCL);
        ((LavaDECL*)implEl->data)->Supports.Append(che);
        for (ioElImpl = (CHE*)implElDECL->NestedDecls.first;
             ioElImpl && ioElImpl->successor
               && (((LavaDECL*)ioElImpl->data)->DeclDescType != ExecDesc);
             ioElImpl = (CHE*)ioElImpl->successor) {
          che = new CHETID;
          che->data = TID(((LavaDECL*)ioElImpl->data)->OwnID, DragINCL); //old ID, new ID
          ((LavaDECL*)ioElImpl->data)->Supports.Append(che);
          ((LavaDECL*)ioElImpl->data)->SecondTFlags.INCL(funcImpl);
          ((LavaDECL*)ioElImpl->data)->WorkFlags.INCL(fromPrivToPub);
        }
        implElDECL->SecondTFlags.INCL(funcImpl);
        implElDECL->WorkFlags.INCL(fromPrivToPub);
        ((LavaDECL*)clipEl->data)->NestedDecls.Delete(((LavaDECL*)clipEl->data)->NestedDecls.last);
        execElIf = GetDocument()->GetExecChe((LavaDECL*)clipEl->data,Ensure,false);
        if (execElIf)
          ((LavaDECL*)clipEl->data)->NestedDecls.Delete(execElIf);
        execElIf = GetDocument()->GetExecChe((LavaDECL*)clipEl->data,Require,false);
        if (execElIf)
          ((LavaDECL*)clipEl->data)->NestedDecls.Delete(execElIf);
        implEl = (CHE*)implEl->successor;
      }
    }
    dragView = (CLavaPEView*)DragDoc->DragView;
    itemData = (CMainItemData*)dragView->m_hitemDrag->getItemData();
    if (itemData->type == TIType_DECL)
      pos = dragView->GetPos(0, dragView->m_hitemDrag);
    else
      pos = dragView->GetPos(0, 0);
    itemP = (CTreeItem*)dragView->m_hitemDrag->parent();
    d4 = pDragParent;
    if ((*d4)->FullName.l) {
      str2 = new DString( (*(LavaDECL**)d4)->FullName);
      implDECL->FullName = *str2;
    }
    else {
      str2 = 0;
      implDECL->FullName.Reset(0);
    }
    FIRSTLAST(DragDoc, firstlast);
    hintCol = new CLavaPEHint(CPECommand_Insert, DragDoc, firstlast, (DWORD)implDECL, (DWORD)str2, pos, (DWORD)d4, 0, (DWORD)Clipdata->ClipTree, (DWORD)Clipdata->docPathName, (DWORD)RefacCase);
    DragDoc->UpdateDoc(dragView, false, hintCol);
    break;
  case baseToEx:
  case exToBase:
    for (clipEl = (CHE*)clipDECL->NestedDecls.first;
         clipEl && (
          ( ((LavaDECL*)clipEl->data)->DeclType == Function)
             && (((LavaDECL*)clipEl->data)->TypeFlags.Contains(isAbstract)
                || ((LavaDECL*)clipEl->data)->TypeFlags.Contains(isNative))
             || (((LavaDECL*)clipEl->data)->DeclType == Attr)
                && !((LavaDECL*)clipEl->data)->TypeFlags.Contains(hasSetGet));
         clipEl = (CHE*)clipEl->successor);
    ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
    if (clipEl && ((CLavaPEApp*)wxTheApp)->Browser.FindImpl(DragDoc, *pDragParent, dragImplDoc, pDragImplDecl)
      && ((CLavaPEApp*)wxTheApp)->Browser.FindImpl(GetDocument(), dropParent, dropImplDoc, pDropImplDecl)) {
      ((CLavaPEDoc*)dragImplDoc)->DragView = ((CLavaPEDoc*)dragImplDoc)->MainView;
      pDragP = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP = pDragImplDecl;
      implDECL = NewLavaDECL();
      implDECL->WorkFlags.INCL(ImplFromBaseToEx);
      implDECL->DeclType = clipDECL->DeclType;
      implDECL->FullName = clipDECL->FullName;
      implDECL->ParentDECL = clipDECL->ParentDECL;
      implDECL->TreeFlags = clipDECL->TreeFlags;
      implDECL->DeclDescType = clipDECL->DeclDescType;
      implDECL->SecondTFlags = clipDECL->SecondTFlags;
      pos = 1;
      if ((dropImplDoc != dragImplDoc)
         || (dropImplDoc != DragDoc)
         || (dropImplDoc != GetDocument())
         || (dragImplDoc != DragDoc)
         || (dragImplDoc != GetDocument())
         || mdh) {
         firstlast.INCL(multiDocHint);
         if (!mdh) {
           mdh = true;
           ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
         }
      }
      for (clipEl = (CHE*)clipDECL->NestedDecls.first;
           clipEl;
           clipEl = (CHE*)clipEl->successor) {
        clipElDECL = (LavaDECL*)clipEl->data;
        for (implEl = (CHE*)(*pDragImplDecl)->NestedDecls.first;
             implEl
               && (!((LavaDECL*)implEl->data)->Supports.first
                  || (clipElDECL->OwnID != ((CHETID*)((LavaDECL*)implEl->data)->Supports.first)->data.nID));
             implEl = (CHE*)implEl->successor) {
          if (!implDECL->NestedDecls.first && ((((LavaDECL*)implEl->data)->DeclType == Attr) || (((LavaDECL*)implEl->data)->DeclType == Function)))
            pos++;
        }
        if (implEl) {
          clipImplEl = NewCHE(NewLavaDECL());
          *((LavaDECL*)clipImplEl->data) = *((LavaDECL*)implEl->data);
          if (implDECL->NestedDecls.last && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclDescType == ExecDesc)
            implDECL->NestedDecls.Insert(implDECL->NestedDecls.last->predecessor, clipImplEl);
          else
            implDECL->NestedDecls.Append(clipImplEl);
          if ((clipElDECL->DeclType == Attr) && clipElDECL->TypeFlags.Contains(hasSetGet)) {
            for (implEl = (CHE*)implEl->successor;
                 implEl
                   && (!((LavaDECL*)implEl->data)->Supports.first
                      || (clipElDECL->OwnID != ((CHETID*)((LavaDECL*)implEl->data)->Supports.first)->data.nID));
                 implEl = (CHE*)implEl->successor);
            if (implEl) {
              clipImplEl = NewCHE(NewLavaDECL());
              *((LavaDECL*)clipImplEl->data) = *((LavaDECL*)implEl->data);
              if (implDECL->NestedDecls.last && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclDescType == ExecDesc)
                implDECL->NestedDecls.Insert(implDECL->NestedDecls.last->predecessor, clipImplEl);
              else
                implDECL->NestedDecls.Append(clipImplEl);
            }
          }
        }
        if ((RefacCase == baseToEx)
          && !GetDocument()->IDTable.EQEQ(((CHETID*)dropParent->Supports.first)->data, 0, TID((*pDragParent)->OwnID, DragINCL), 0))
          CorrVT_BaseToEx(dropParent, *pDragParent, (LavaDECL*)clipEl->data, mdh);
        else
          if ((RefacCase == exToBase)
            && !DragDoc->IDTable.EQEQ(((CHETID*)(*pDragParent)->Supports.first)->data, 0, TID(dropParent->OwnID, DropINCL), 0))
            CorrVT_ExToBase(*pDragParent, dropParent, (LavaDECL*)clipEl->data, mdh, vtHints);
      }
      collectDECL = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL;
      collectPos = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = NewLavaDECL();
      *((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = *implDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos = pos;
      if (dragImplDoc == dropImplDoc) {
        d4 = pDragImplDecl;
        str2 = new DString((*pDragImplDecl)->FullName);
        FIRSTLAST(dragImplDoc, firstlast);
        hint = new CLavaPEHint(CPECommand_Delete, dragImplDoc, firstlast, (DWORD)((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL, (DWORD)str2, (DWORD)pos, (DWORD)d4, (DWORD)pDropImplDecl,0,0, (DWORD)RefacCase);
        ((CLavaPEDoc*)dragImplDoc)->UpdateDoc(this, false, hint);
        delete hint;
        d6 = 0;
      }
      else
        d6 = (DWORD)&((CLavaPEDoc*)dragImplDoc)->mySynDef->SynDefTree;

      pos = GetPos(0, 0);
      d4 = pDropImplDecl;
      str2 = new DString((*pDropImplDecl)->FullName);
      docName = new DString(((CLavaPEDoc*)dragImplDoc)->GetAbsSynFileName());
      FIRSTLAST(dropImplDoc, firstlast);
      hint = new CLavaPEHint(CPECommand_Move, dropImplDoc, firstlast, (DWORD)implDECL,
                 (DWORD)str2, (DWORD)pos, (DWORD)d4, (DWORD)*pDragImplDecl, d6, (DWORD)docName, (DWORD)RefacCase);
      ((CLavaPEDoc*)dropImplDoc)->UpdateDoc(this, false, hint);
      delete hint;
      delete implDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP = pDragP;
      delete ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = collectDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos = collectPos;
      ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
    }
    else {
      ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
      if (clipEl) {
        QMessageBox::critical(this, qApp->name(),"Cannot move feature because the implementation\n"
          "of the target interface does not yet exist\n"
          "or is in another Lava file that is not open currently",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
      }
      else
        return true;
    }
    GetParentFrame()->Activate(true);
    break;
  default:
    return false;
  }
  if (hintCol) {
    hintCol->CommandData7 = 0;
    delete hintCol;
  }
  if (DragDoc->hasHint && DragDoc->DragView)
    ((CLavaPEView*)DragDoc->DragView)->pDeclDragP = (LavaDECL**)DragDoc->IDTable.GetVar(TID((*((CLavaPEView*)DragDoc->DragView)->pDeclDragP)->OwnID,0), idType);

  return true;
}

bool CLavaPEView::RefacMove(CTreeItem* dropItem)
{
  //call this function only for dragged functions or member
  if (!DragDoc)
    return false;

  int hDropINCL;
  CHE *clipEl, *dropEl;
  CLavaPEDoc *highDoc=0, *dropDoc = GetDocument();
  CMainItemData *ddrop;
  LavaDECL *dragParent, *dropParent;

  ddrop = (CMainItemData*)dropItem->getItemData();
  dropParent = *(LavaDECL**)ddrop->synEl;
  if (ddrop->type == TIType_DECL)
    dropParent = dropParent->ParentDECL;
  if (DragINCL != 0) {
    highDoc = dropDoc;
    hDropINCL = 0;
  }
  else {
    highDoc = DragDoc;
    hDropINCL = DropINCL;
  }
  dragParent = highDoc->IDTable.GetDECL(0, ((LavaDECL*)((CHE*)((LavaDECL*)Clipdata->synEl)->NestedDecls.first)->data)->OwnID, DragINCL);
  dragParent = dragParent->ParentDECL;
  RefacCase = noRefac;
  if (highDoc->IDTable.IsAn(TID(dropParent->OwnID, hDropINCL), 0,TID(dragParent->OwnID, DragINCL), 0)) {
    //move from base to ex: publicToPriv or baseToEx
    if (dropParent->DeclType == Impl) {
      if (((CHETID*)dropParent->Supports.first)->data == TID(dragParent->OwnID, DragINCL))
        RefacCase = publicToPriv;
    }
    else {
        //no derivate of the moved function in dropParent?
      for (clipEl = (CHE*)((LavaDECL*)Clipdata->synEl)->NestedDecls.first;
           clipEl;
           clipEl = (CHE*)clipEl->successor) {
        for (dropEl = (CHE*)dropParent->NestedDecls.first;
             dropEl
               && (!((LavaDECL*)dropEl->data)->Supports.first
               || !highDoc->IDTable.EQEQ(TID(((LavaDECL*)clipEl->data)->OwnID, DragINCL),0, ((CHETID*)((LavaDECL*)dropEl->data)->Supports.first)->data, hDropINCL));
             dropEl = (CHE*)dropEl->successor);
        if (dropEl)
          return false;
      }
      RefacCase = baseToEx;
    }
  }
  else {
    if (highDoc->IDTable.IsAn(TID(dragParent->OwnID, DragINCL), 0,TID(dropParent->OwnID, hDropINCL), 0))
      //move from ex to base: privToPublic or exToBase
      if (dragParent->DeclType == Impl) {
        if (highDoc->IDTable.EQEQ(((CHETID*)dragParent->Supports.first)->data, DragINCL, TID(dropParent->OwnID, hDropINCL),0))
          RefacCase = privToPublic;
      }
      else
        RefacCase = exToBase;
  }
  return RefacCase != noRefac;
}

void CLavaPEView::RemoveEmptyOpt(CTreeItem* startItem, bool down)
{
  CTreeItem* pitem = (CTreeItem*)startItem->child(0);
  if (!pitem)
    return;
  CTreeItem* item;
  CMainItemData *itd, *dataD;
  LavaDECL *decl;
  bool hasC = false;

  itd = (CMainItemData*)pitem->getItemData();
  if (itd->type == TIType_CHEEnumSel)
    return;

  while (pitem) {
    itd = (CMainItemData*)pitem->getItemData();
    hasC = (itd->type == TIType_Require)
           ||  (itd->type == TIType_Ensure)
           ||  (itd->type == TIType_Exec);
    if (hasC) {
      decl = GetExecDECL(pitem);
      hasC = (decl != 0);
    }
    if (hasC)
      hasC =  decl->Exec.ptr != 0;
    item = (CTreeItem*)pitem->child(0);
    if (item || hasC) {
      if (down) {
        while (item) {
          RemoveEmptyOpt(item, true);
          dataD = (CMainItemData*)item->getItemData();
          if (dataD->type == TIType_DECL)
            (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
          item = (CTreeItem*)item->nextSibling();
        }
      }
      pitem = (CTreeItem*)pitem->nextSibling();
    }
    else {
      delete itd;
      item = (CTreeItem*)pitem->nextSibling();
      delete pitem;
      pitem = item;
    }
  }
}


void CLavaPEView::RenameCancel(CTreeItem *item)
{
  item->setText(0, item->completeText);
}

void CLavaPEView::RenameOk(CTreeItem *item)
{
  CMainItemData *pdata, *itd = (CMainItemData*)item->getItemData();
  LavaDECL *decl, *labelDECL, *ppDECL;
  CLavaPEHint *hint=0;
  CTreeItem *pi;
  CHAINANY* enumItems;
  CHEEnumSelId *relEl;
  QString *err = 0;
  DString *name,  *newLab, *oldLab, lab = DString(qPrintable(item->text(0))) ;

  if (lab.l) {
    if (!((CLavaPEApp*)wxTheApp)->LBaseData.isIdentifier(lab.c)) {
      if ((item != m_tree->RootItem)
          || (*(LavaDECL**)itd->synEl)->Supports.first)
        err = &IDP_IsNoID;
      else {
        item->setText(0, item->completeText);
        return;
      }
    }
    else {
      if (itd->type == TIType_DECL) {
        labelDECL = *(LavaDECL**)itd->synEl;
        if (lab != labelDECL->LocalName) {
          if (GetDocument()->FindInSupports(lab, labelDECL, labelDECL->ParentDECL))
            err = &ERR_NameInUse;
          else {
            decl = NewLavaDECL();
            *decl = *labelDECL;
            decl->LocalName = lab;
            if (decl->ParentDECL)
              decl->FullName = decl->ParentDECL->FullName + decl->LocalName;
            else
              decl->FullName = decl->LocalName;
            name = new DString(labelDECL->FullName);  //bisheriger Name
            ppDECL = decl->ParentDECL;
            hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
          }
        }
        else {
          item->setText(0, item->completeText);
          return;
        }
      }
      else
        if (itd->type == TIType_CHEEnumSel) {
          pi = (CTreeItem*)item->parent();
          pi = (CTreeItem*)pi->parent();
          pdata = (CMainItemData*)pi->getItemData();
          enumItems = &((TEnumDescription*)(*(LavaDECL**)pdata->synEl)->EnumDesc.ptr)->EnumField.Items;
          relEl = (CHEEnumSelId*)enumItems->first;
          if (lab != ((CHEEnumSelId*)itd->synEl)->data.Id) {
            while (relEl && (relEl->data.Id != lab))
              relEl = (CHEEnumSelId*)relEl->successor;
            if (relEl)
              err = &ERR_NameInUse;
            else {
              decl = NewLavaDECL();
              *decl = **(LavaDECL**)pdata->synEl;
              relEl = (CHEEnumSelId*)((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Items.first;
              while (relEl && (relEl->data.Id != ((CHEEnumSelId*)itd->synEl)->data.Id))
                relEl = (CHEEnumSelId*)relEl->successor;
              if (relEl && (relEl->data.Id != lab)) {
                oldLab = new DString(relEl->data.Id);
                newLab = new DString(lab);
                relEl->data.Id = lab;
                ppDECL = decl->ParentDECL;
                name = new DString((*(LavaDECL**)pdata->synEl)->FullName);  //bisheriger Name
                hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, pdata->synEl, (DWORD)newLab, (DWORD)oldLab);
              }
            }
          }//new label
          else { //do nothing
            item->setText(0, item->completeText);
            return;
          }
        }
    }//isIdentifier
  }//len > 0
  else {
    if ((item == m_tree->RootItem) && !(*(LavaDECL**)itd->synEl)->Supports.first) {
      decl = NewLavaDECL();
      *decl = **(LavaDECL**)itd->synEl;
      decl->FullName.Destroy();
      decl->LocalName.Destroy();
      name = new DString((*(LavaDECL**)itd->synEl)->FullName);  //bisheriger Name
      if (name->l) {
        ppDECL = 0;
        hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
      }
    }
    else {
      item->setText(0, item->completeText);
      return;
    }
  }//len == 0
  if (hint) {
    GetDocument()->UndoMem.AddToMem(hint);
    GetDocument()->UpdateDoc(this, false, hint);
    GetDocument()->ConcernForms(hint);
    GetDocument()->ConcernImpls(hint, ppDECL);
    GetDocument()->ConcernExecs(hint);
    if (itd->type == TIType_CHEEnumSel) {
      QApplication::postEvent(this, new CustomEvent(IDU_LavaPE_SetLastHint));
    }
    else
      GetDocument()->SetLastHint();
  }
  else {
    if (err) {
      QMessageBox::critical(this, qApp->name(),*err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      item->actLab = lab;
      item->startRename(0);
    }
    else {
      item->setText(0, item->completeText);
    }
  }
}

void CLavaPEView::RenameStart(CTreeItem *item)
{
  if (!item->renameEnabled(0))
    return;
  CMainItemData* itd = (CMainItemData*)item->getItemData();
  LavaDECL* decl;
  QString str;
  if (item->actLab.l) {
    item->setText(0, item->actLab.c);
    item->actLab.Reset(0);
  }
  else {
    item->completeText = item->text(0);
    if (itd->type == TIType_DECL) {
      decl = *(LavaDECL**)itd->synEl;
      item->setText(0, decl->LocalName.c);
    }
    else if (itd->type == TIType_CHEEnumSel)
      item->setText(0, ((CHEEnumSelId*)itd->synEl)->data.Id.c);
  }
}



void CLavaPEView::SetErrAndCom(CTreeItem* item)
{
  bool hasC;
  LavaDECL *decl = 0;
  DString str0;
  CUtilityView* bar = ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView;

  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_CHEEnumSel) {
      if (((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr)
        bar->SetComment(((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr->Comment, true);
      else {
        DString str0;
        bar->SetComment(str0, true);
      }
      return;
    }
    if (data->type == TIType_DECL)
      decl = *(LavaDECL**)data->synEl;
    else {
      if ((data->type == TIType_Require) || (data->type == TIType_Ensure) ||(data->type == TIType_Exec)) {
        decl = GetExecDECL(item);
        hasC = (decl != 0);
        if (hasC)
          hasC =  decl->Exec.ptr != 0;
      }
      else
        decl = 0; //no invariant node present
    }
    if (decl) {
      if (decl->DECLComment.ptr)
        bar->SetComment(decl->DECLComment.ptr->Comment, !decl->DECLError1.first && !decl->DECLError2.first);
      else
        bar->SetComment(str0, !decl->DECLError1.first && !decl->DECLError2.first);
      CheckAutoCorr(decl);
      bar->SetErrorOnUtil(decl);
      if (!((CLavaMainFrame*)wxTheApp->m_appWindow)->UtilitiesHidden) {
        if ((bar->ActTab == tabError) && decl->DECLComment.ptr && bar->ErrorEmpty )
          bar->SetTab(tabComment);
        else if ((bar->ActTab == tabComment) && !decl->DECLComment.ptr && !bar->ErrorEmpty)
          bar->SetTab(tabError);
      }
    }
    else {
      bar->SetComment(str0, true);
      bar->ResetError();
    }
  }
}


void CLavaPEView::SetTreeFlags(CTreeItem* item, bool exp)
{
  CMainItemData *data = (CMainItemData*)item->getItemData();
  data->toExpand = exp;
  switch (data->type) {
    case TIType_DECL:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(isExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(isExpanded);
      break;
    case TIType_EnumItems:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(ItemsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(ItemsExpanded);
      break;
    case TIType_VTypes:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(ParaExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(ParaExpanded);
      break;
    case TIType_Input:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(InExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(InExpanded);
      break;
    case TIType_Output:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(OutExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(OutExpanded);
      break;
    case TIType_Defs:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(DefsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(DefsExpanded);
      break;
    case TIType_Features:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(MemsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(MemsExpanded);
      break;
    default: ;
  }
}

void CLavaPEView::UpdateUI()
{
  if (!myDECL)
    return;
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->expandAction->setEnabled(true);
  frame->collapseAction->setEnabled(true);
  frame->showOptsAction->setEnabled(true);
  frame->showAllOptsAction->setEnabled(true);
  frame->nextCommentAction->setEnabled(true);
  frame->hideEmptyOptsAction->setEnabled(true);
  frame->prevCommentAction->setEnabled(true);
  frame->nextErrorAction->setEnabled(true);
  frame->prevErrorAction->setEnabled(true);
  ItemSel = (CTreeItem*)m_tree->currentItem();
  if (ItemSel) {
    DataSel = (CMainItemData*)ItemSel->getItemData();
    if (DataSel) {
      if (DataSel->type == TIType_DECL) {
        if (ItemSel != m_tree->RootItem) {
          ItemSel = (CTreeItem*)ItemSel->parent();
          DataSel = (CMainItemData*)ItemSel->getItemData();
        }
      }
      if (ItemSel != m_tree->RootItem) {
        GroupType = DataSel->type;
        ItemSel = (CTreeItem*)ItemSel->parent();
        DataSel = (CMainItemData*)ItemSel->getItemData();
        if (DataSel)
          DefTypeSel = (*(LavaDECL**)DataSel->synEl)->DeclType;
        else
          ItemSel = 0;
      }
      //else
      //  ItemSel = 0;
    }
    else
      ItemSel = 0;
  }
  else
    ItemSel = 0;

  if (ItemSel) {
    ItemSel = (CTreeItem*)m_tree->currentItem();
    DataSel = (CMainItemData*)ItemSel->getItemData();
    OnUpdateEditCopy(frame->editCopyAction);
    OnUpdateEditCut(frame->editCutAction);
    OnUpdateEditCut(frame->deleteAction);
    OnUpdateEditPaste(frame->editPasteAction);
    OnUpdateComment(frame->editCommentAction);
    OnUpdateGotodef(frame->gotoDeclAction);
	  OnUpdateGotoImpl(frame->gotoImplAction);
	  OnUpdateFindreferences(frame->findRefsAction);
	  OnUpdateMakeGUI(frame->makeGUIAction);
    if (myInclView) {
      OnUpdateNewenum(frame->newEnumAction);
      OnUpdateNewfunction(frame->newFuncAction);
      OnUpdateNewinitiator(frame->newInitiatorAction);
      OnUpdateNewmember(frame->newMemVarAction);
      OnUpdateNewPackage(frame->newPackageAction);
      OnUpdateNewImpl(frame->newImplAction);
      OnUpdateNewCSpec(frame->newCOspecAction);
      OnUpdateNewComponent(frame->newCOimplAction);
      OnUpdateNewVirtualType(frame->newVTAction);
      OnUpdateNewInterface(frame->newInterfaceAction);
      OnUpdateNewset(frame->newSetAction);
	    OnUpdateNewEnumItem(frame->newEnumItemAction);

      OnUpdateShowformview(frame->openFormViewAction);
      OnUpdateOverride(frame->showOverridablesAction);
      OnUpdateShowOptionals(frame->showOptsAction);
      OnUpdateEditSel(frame->editSelItemAction);

      frame->insertTextAction->setEnabled(false);
    }
    else {//form
      OnUpdateNewLitStr(frame->insertTextAction);

      frame->newEnumAction->setEnabled(false);
      frame->newFuncAction->setEnabled(false);
      frame->newInitiatorAction->setEnabled(false);
      frame->newMemVarAction->setEnabled(false);
      frame->newPackageAction->setEnabled(false);
      frame->newImplAction->setEnabled(false);
      frame->newCOspecAction->setEnabled(false);
      frame->newCOimplAction->setEnabled(false);
      frame->newVTAction->setEnabled(false);
      frame->newInterfaceAction->setEnabled(false);
      frame->newSetAction->setEnabled(false);
	    frame->newEnumItemAction->setEnabled(false);
      frame->openFormViewAction->setEnabled(false);
      frame->showOverridablesAction->setEnabled(false);
      frame->showOptsAction->setEnabled(false);
      frame->editSelItemAction->setEnabled(false);
    }
  }
  else {
    frame->editCopyAction->setEnabled(false);
    frame->editCutAction->setEnabled(false);
    frame->deleteAction->setEnabled(false);
    frame->editPasteAction->setEnabled(false);

    frame->newEnumAction->setEnabled(false);
    frame->newFuncAction->setEnabled(false);
    frame->newInitiatorAction->setEnabled(false);
    frame->newMemVarAction->setEnabled(false);
    frame->newPackageAction->setEnabled(false);
    frame->newImplAction->setEnabled(false);
    frame->newCOspecAction->setEnabled(false);
    frame->newCOimplAction->setEnabled(false);
    frame->newVTAction->setEnabled(false);
    frame->newInterfaceAction->setEnabled(false);
    frame->newSetAction->setEnabled(false);
	  frame->newEnumItemAction->setEnabled(false);

    frame->insertTextAction->setEnabled(false);
    frame->openFormViewAction->setEnabled(false);
    frame->showOverridablesAction->setEnabled(false);
    frame->showOptsAction->setEnabled(false);
    frame->editCommentAction->setEnabled(false);
    frame->editSelItemAction->setEnabled(false);
    frame->gotoDeclAction->setEnabled(false);
	  frame->gotoImplAction->setEnabled(false);
	  frame->findRefsAction->setEnabled(false);
  }
}


bool CLavaPEView::VerifyItem(CTreeItem* item, CTreeItem* topItem)
{
  if (!topItem)
    topItem = (CTreeItem*)m_tree->RootItem;
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


void CLavaPEView::on_whatNextAction_triggered()
{
/*
  QWhatsThis::showText(m_tree->viewport()->mapToGlobal(m_tree->itemRect(m_tree->currentItem()).topLeft())+QPoint(200,20),
    QString(QObject::tr("<p>You may <b>delete</b> this item"
    " (DEL key or scissors button),<br>or you may <b>insert</b> another item after this one"
    " (click any enabled button on the declaration toolbar),"
    "<br>or you may <b>view/edit the properties</b> of this item (spectacles button),"
    "<br><a href=\"../on_whatNextAction_triggered/DeclViewWhatNext.htm\">or...</a></p>")),
      m_tree);
      */
}

//Toolbutton handler--------------------------------------------------------

void CLavaPEView::OnCollapsAll()
{
  CTreeItem* htr0 = (CTreeItem*)m_tree->currentItem();
  CTreeItem* htr = htr0;
  int level = 0;
  while (htr) {
    m_tree->collapseItem(htr); //->setOpen(false);
    SetTreeFlags(htr, false);
    Navigate(true, htr, level);
  } 
  m_tree->scrollToItem(htr0, QAbstractItemView::EnsureVisible );
  //m_tree->ensureItemVisible(htr0);
}


void CLavaPEView::OnComment()
{
  CTreeItem *pItem, *item = (CTreeItem*)m_tree->currentItem();
  TDECLComment* ptrComment=0, *ptr = 0;
  DWORD d1, d3 = 0;
  CComment *pComment;
  CMainItemData* data;
  bool hasC, hasErr = false;
  CLavaPEHint* hint;
  LavaDECL *decl;

  if (item) {
    data = (CMainItemData*)item->getItemData();
    pComment = new CComment(this);

    if (data->type == TIType_DECL) {
      ptrComment = (*(LavaDECL**)data->synEl)->DECLComment.ptr;
      d1 = (DWORD)*(LavaDECL**)data->synEl;
      hasErr = ((LavaDECL*)d1)->DECLError1.first || ((LavaDECL*)d1)->DECLError2.first;
    }
    else
      if (data->type == TIType_CHEEnumSel) {
        ptrComment = ((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr;
        d1 = data->synEl;
        pItem = (CTreeItem*)item->parent();
        pItem = (CTreeItem*)pItem->parent();
        pItem = (CTreeItem*)pItem->parent();
        d3 = (DWORD)*(LavaDECL**)((CMainItemData*)item->getItemData())->synEl;
      }
      else
        if ((data->type == TIType_Require) || (data->type == TIType_Ensure) || (data->type == TIType_Exec)) {
          decl = GetExecDECL(item);
          hasC = (decl != 0);
          if (hasC)
            hasC = decl->Exec.ptr != 0;
          if (hasC) {
            ptrComment = decl->DECLComment.ptr;
            d1 = (DWORD)decl;
          }
          else
            return;
        }
        else
          return;
    if (ptrComment && ptrComment->Comment.l) {
      //pComment->inline_comment->setChecked(false);
      //pComment->trailing_comment->setChecked(false);
      pComment->trailing_comment->hide();
      pComment->inline_comment->hide();
      //pComment->m_inline = ptrComment->PrintFlags.Contains(inLineComment);
      //pComment->m_trailing = ptrComment->PrintFlags.Contains(trailingComment);
      pComment->text->setText(ptrComment->Comment.c);
    }
    if (pComment->exec() == QDialog::Accepted) {
      if (pComment->text->text().length()) {
        ptr = new TDECLComment;
        //if (pComment->m_inline)
        //  ptr->PrintFlags.INCL(inLineComment);
        //else
          ptr->PrintFlags.EXCL(inLineComment);
        //if (pComment->m_trailing)
        //  ptr->PrintFlags.INCL(trailingComment);
        //else
          ptr->PrintFlags.EXCL(trailingComment);
        ptr->Comment = STRING(qPrintable(pComment->text->text()));
      }
      hint = new CLavaPEHint(CPECommand_Comment, GetDocument(), (const unsigned long)3, d1, (DWORD)ptr, d3);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
     // if (!hasErr)
     //   wxTheApp->m_appWindow->m_wndStatusBar.message(pComment->m_comment);

      delete pComment;
    }
    else
      delete pComment;
  }

}

void CLavaPEView::OnEditCopy()
{
  IOnEditCopy();
  if (CollectDECL) {
    DeleteDragChain();
    m_tree->ResetSelections();
    if (lastCurrent) {
      m_tree->setCurAndSel(lastCurrent);
    }

  }
}

void CLavaPEView::OnEditCut()
{
  if (GetDocument()->changeNothing)
    return;

  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (!item || (item == m_tree->RootItem))
    return;
  int pos;
  if (!CollectDECL)
    pos = GetPos(item, 0);
  else
    pos = CollectPos;
  IOnEditCopy();
  CLavaPEHint* hint;
  if (CollectDECL->DeclType == DragEnum) {
    hint = ChangeEnum(CollectDECL, item, true, false);
    if (hint) {
      hint->FirstLast.EXCL(lastHint);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      GetDocument()->ConcernForms(hint);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
    }
  }
  else {
    CTreeItem* pItem = (CTreeItem*)item->parent();
    if (pItem) {
      DWORD d4 = ((CMainItemData*)pItem->getItemData())->synEl;
      DString* str2 = 0;
      if (CollectDECL->FullName.l)
        str2 = new DString(CollectDECL->FullName);
      hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)CollectDECL, (DWORD)str2, (DWORD)pos, d4, 0);
      GetDocument()->UpdateDoc(this, false, hint);
      if (myInclView) {
        GetDocument()->ConcernForms(hint);
        if (d4)
          GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
        else
          GetDocument()->ConcernImpls(hint, 0);
        GetDocument()->ConcernExecs(hint);
      }
      GetDocument()->SetLastHint();
    }
  }
  if (CollectDECL)
    DeleteDragChain();
}

void CLavaPEView::OnEditPaste()
{
  //remember: the CMainItemData on the Clipboard has LavaDECL* in synEl
  //COleDataObject pDataObject;
  CTreeItem *item, *itemP;
  TIType pasteType;
  LavaDECL *declClip, *decl;
  CMainItemData clipdata, *itemData;
  TDeclType elType;
  CHE* che;
  CLavaPEHint *hint;
  DString *str2;
  DWORD d4;

  if (GetDocument()->changeNothing)
    return;

//  if (wxTheApp->clipboard()->data(QClipboard::Clipboard)->provides(m_nIDClipFormat)) {
  if (clipboard_lava_notEmpty) {
    QByteArray ba = ((LavaSource*)wxTheApp->clipboard()->data(QClipboard::Clipboard))->data/*encodedData*/(m_nIDClipFormat);
		QDataStream ar(ba/*,QIODevice::ReadOnly*/);
    item = (CTreeItem*)m_tree->currentItem();
    clipdata.Serialize(ar);
    declClip = (LavaDECL*)clipdata.synEl;
    pasteType = CanPaste(declClip->DeclType, declClip->TreeFlags, declClip->SecondTFlags, item);
    if (pasteType != TIType_NoType) {
      if ((declClip->DeclType == DragIO) || (declClip->DeclType == DragFeature)) {
        switch (pasteType) {
        case TIType_Features:
          elType = Attr;
          break;
        case TIType_Input:
          elType = IAttr;
          break;
        case TIType_Output:
          elType = OAttr;
          break;
        default: ;
        }
        che = (CHE*)declClip->NestedDecls.first;
        while (che) {
          decl = (LavaDECL*)che->data;
          if ((decl->DeclType == Attr) || (decl->DeclType == IAttr) || (decl->DeclType == OAttr))
            decl->DeclType = elType;
          che = (CHE*)che->successor;
        }
      }
      item = (CTreeItem*)m_tree->currentItem();
      itemData = (CMainItemData*)item->getItemData();
      DString fn = GetDocument()->GetAbsSynFileName();
      if (fn != *clipdata.docPathName)
        declClip->inINCL = 10000;
      itemP = (CTreeItem*)item->parent();
      if (declClip->DeclType == DragEnum) {
        hint = ChangeEnum(declClip, item, false, true);
        if (hint) {
          hint->FirstLast.EXCL(lastHint);
          GetDocument()->UndoMem.AddToMem(hint);
          GetDocument()->UpdateDoc(this, false, hint);
          GetDocument()->ConcernForms(hint);
          GetDocument()->ConcernExecs(hint);
          GetDocument()->SetLastHint();
        }
      }
      else {
        int pos;
        if (itemData->type == TIType_DECL)
          pos = GetPos(0, item);
        else
          pos = GetPos(0, 0);
        d4 = ((CMainItemData*)itemP->getItemData())->synEl;
        if ((*(LavaDECL**)d4)->FullName.l) {
          str2 = new DString( (*(LavaDECL**)d4)->FullName);
          declClip->FullName = *str2;
        }
        else {
          str2 = 0;
          declClip->FullName.Reset(0);
        }
        hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1, (DWORD)declClip, (DWORD)str2, pos, d4, 0, (DWORD)clipdata.ClipTree, (DWORD)clipdata.docPathName);
        GetDocument()->UpdateDoc(this, false, hint);
        clipdata.synEl = 0;
        clipdata.ClipTree = 0;
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4)
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
      clipdata.Destroy();
      clipdata.docPathName = 0;
    }
  }
}

void CLavaPEView::OnEditSel()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (!item ) { //||  (myInclView && (item == m_tree->child(0)))) {
    QMessageBox::critical(this, qApp->name(),IDP_NoElemSel,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
  else
    OnEditSelItem(item, false);
}

void CLavaPEView::OnExpandAll()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (item )
    ExpandItem(item);
  m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
  //m_tree->ensureItemVisible(item);
}


void CLavaPEView::OnFindReferences()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  CFindData fw;
  fw.FWhere = findInThisDoc;
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      LavaDECL* DECL = *(LavaDECL**)itd->synEl;
      ((CLavaPEApp*)wxTheApp)->Browser.OnFindRefs(GetDocument(), DECL, fw);
    }
    else
    if (itd->type == TIType_CHEEnumSel) {
      fw.enumID = ((CHEEnumSelId*)itd->synEl)->data.Id;
      item = (CTreeItem*)item->parent();
      itd = (CMainItemData*)item->getItemData();
      LavaDECL* DECL = *(LavaDECL**)itd->synEl;
      ((CLavaPEApp*)wxTheApp)->Browser.OnFindRefs(GetDocument(), DECL, fw);
    }
  }
}

void CLavaPEView::OnGotoDecl()
{
  Gotodef((CTreeItem*)m_tree->currentItem());
}

void CLavaPEView::OnGotoImpl()
{
  LavaDECL *DECL;
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  TID id;
  TDeclType exprType;
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    DECL = *(LavaDECL**)itd->synEl;
    if ((itd->type == TIType_DECL) &&
      ((DECL->DeclType == Interface)
        || (DECL->DeclType == CompObjSpec)
        || (DECL->ParentDECL->DeclType == Interface)
           && ((DECL->DeclType == Function) || (DECL->DeclType == Attr) && DECL->TypeFlags.Contains(hasSetGet)))) {
      if (((DECL->DeclType == Function) || (DECL->DeclType == Attr))
        && (DECL->TypeFlags.Contains(isAbstract) || DECL->TypeFlags.Contains(isNative)) )
         QMessageBox::information(this, qApp->name(),ERR_NoImplForAbstract,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      else
        //find implemetation id
        ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
        if (!((CLavaPEApp*)wxTheApp)->Browser.GotoImpl(GetDocument(), DECL)) {
          ((CLavaPEApp*)wxTheApp)->Browser.DestroyLastBrsContext();
          if (DECL->DeclType == Interface)
            if (DECL->TypeFlags.Contains(isComponent))
              QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObj,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
            else
              QMessageBox::critical(this, qApp->name(),ERR_NoClassImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          else if (DECL->DeclType == Function)
            if (DECL->ParentDECL->TypeFlags.Contains(isComponent))
              QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObjF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
            else
              QMessageBox::critical(this, qApp->name(),ERR_NoFuncImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          else if (DECL->DeclType == Attr)
            if (DECL->ParentDECL->TypeFlags.Contains(isComponent))
              QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObjF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
            else
              QMessageBox::critical(this, qApp->name(),ERR_SetGetError,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);

          else if (DECL->DeclType == CompObjSpec)
            QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObj,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        }
      }
    else
      if ((itd->type == TIType_Exec)
         || (itd->type == TIType_Ensure)
         || (itd->type == TIType_Require)
         || (itd->type == TIType_DECL)
            && ((DECL->ParentDECL->DeclType == Impl) && (DECL->DeclType == Function)
                || (DECL->DeclType == Initiator))) {
        ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
        if (itd->type == TIType_Ensure)
          exprType = Ensure;
        else if (itd->type == TIType_Require)
          exprType = Require;
        else
          exprType = ExecDef;
        GetDocument()->OpenExecView(GetDocument()->GetExecDECL(DECL,exprType));
      }
  }
}


void CLavaPEView::OnMakeGUI()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  CTreeItem* pitem;
  int pos=0;
  if (item) {
    if (myInclView)
      pitem = (CTreeItem*)item->parent();
    else 
      pitem = myMainView->BrowseTree(myDECL, (CTreeItem*)myMainView->m_tree->child(0));
    GetDocument()->MakeGUI(*(LavaDECL**)((CMainItemData*)item->getItemData())->synEl, (LavaDECL**)((CMainItemData*)pitem->getItemData())->synEl, pos);
  }
}

void CLavaPEView::OnNewComponent()
{
  OnInsert(CompObj);
}

void CLavaPEView::OnNewCSpec()
{
  OnInsert(CompObjSpec);
}

void CLavaPEView::OnNewenum()
{
  OnInsert(EnumDef);
}

void CLavaPEView::OnNewEnumItem()
{
  CTreeItem *item, *par;
  CMainItemData *data, *parData;
  LavaDECL *oldDECL, *newDECL;
  CHEEnumSelId *newEl, *che;
  CHAINANY* newItems;
  DString *str;
  QString iT;
  CLavaPEHint *hint;

  item = (CTreeItem*)m_tree->currentItem();
  if (!item)
    return;
  data = (CMainItemData*)item->getItemData();
  if (!data || (data->type != TIType_CHEEnumSel)
               && (data->type != TIType_EnumItems))
    return;
  par = (CTreeItem*)item->parent();
  if (data->type == TIType_CHEEnumSel)
    par = (CTreeItem*)par->parent();
  parData = (CMainItemData*)par->getItemData();
  oldDECL = *(LavaDECL**)parData->synEl;

  CEnumItem *cm = new CEnumItem(&iT, 0, &((TEnumDescription*)oldDECL->EnumDesc.ptr)->EnumField.Items, true, this);
  if (cm->exec() == QDialog::Accepted) {
    newDECL = NewLavaDECL();
    *newDECL = *oldDECL;
    newEl = new CHEEnumSelId;
    newEl->data.Id = DString(qPrintable(iT));
    newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
    newEl->data.selItem = true;
    if  (data->type == TIType_EnumItems)
      newItems->Prepend(newEl);
    else {
      che = (CHEEnumSelId*)newItems->first;
      while (che && (che->data.Id != ((CHEEnumSelId*)data->synEl)->data.Id))
        che = (CHEEnumSelId*)che->successor;
      if (che)
        newItems->Insert(che, newEl);
      ((CHEEnumSelId*)data->synEl)->data.selItem = true;
    }
    str = new DString((*(LavaDECL**)parData->synEl)->FullName);
    newDECL->WorkFlags.INCL(selEnum);
    newDECL->TreeFlags.INCL(ItemsExpanded);
    hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)newDECL, (DWORD)str, 0, parData->synEl);
    GetDocument()->UndoMem.AddToMem(hint);
    GetDocument()->UpdateDoc(this, false, hint);
    GetDocument()->ConcernForms(hint);
    GetDocument()->ConcernExecs(hint);
    GetDocument()->SetLastHint();
  }
  delete cm;
}

void CLavaPEView::OnNewfunction()
{
  OnInsert(Function);
}

void CLavaPEView::OnNewImpl()
{
  OnInsert(Impl);
}

void CLavaPEView::OnNewinitiator()
{
  OnInsert(Initiator);
}

void CLavaPEView::OnNewInterface()
{
  OnInsert(Interface);
}

void CLavaPEView::OnNewLitStr()
{
  OnInsert(FormText);
}

void CLavaPEView::OnNewmember()
{
  OnInsert(Attr);
}

void CLavaPEView::OnNewPackage()
{
  OnInsert(Package);
}

void CLavaPEView::OnNewset()
{
  OnInsert(SetDef);
}

void CLavaPEView::OnNewVirtualType()
{
  OnInsert(VirtualType);
}

void CLavaPEView::OnNextComment()
{
  CTreeItem* itemStart = (CTreeItem*)m_tree->currentItem();
  OnNextEC(itemStart, false);

}

void CLavaPEView::OnNextError()
{
  CTreeItem* itemStart = (CTreeItem*)m_tree->currentItem();
  OnNextEC(itemStart, true);
}

void CLavaPEView::OnPrevComment()
{
  CTreeItem* itemStart = (CTreeItem*)m_tree->currentItem();
  OnPrevEC(itemStart, false); 
}


void CLavaPEView::OnPrevError()
{
  CTreeItem* itemStart = (CTreeItem*)m_tree->currentItem();
  OnPrevEC(itemStart, true);
}

void CLavaPEView::OnRemoveAllEmptyOpt()
{
  CTreeItem* startItem = (CTreeItem*)m_tree->currentItem();
  if (!startItem)
    return;
  CMainItemData *dataD = (CMainItemData*)startItem->getItemData();
  if (dataD->type == TIType_DECL) {
    RemoveEmptyOpt(startItem, true);
    (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
  }
  else {
    CTreeItem* item = (CTreeItem*)startItem->child(0);
    while (item) {
      dataD = (CMainItemData*)item->getItemData();
      RemoveEmptyOpt(item, true);
      (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

void CLavaPEView::OnShowAllEmptyOpt()
{
  CTreeItem* startItem = (CTreeItem*)m_tree->currentItem();
  if (!startItem)
    return;
  CMainItemData *dataD = (CMainItemData*)startItem->getItemData();
  if (dataD->type == TIType_DECL) {
    DrawEmptyOpt(startItem, true);
    (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
  }
  else {
    CTreeItem* item = (CTreeItem*)startItem->child(0);
    while (item) {
      dataD = (CMainItemData*)item->getItemData();
      DrawEmptyOpt(item, true);
      (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

void CLavaPEView::OnShowExec(TDeclType type)
{
  OnShowSpecialView(type);
}

void CLavaPEView::OnShowGUIview()
{  //show the form view
  OnShowSpecialView(FormDef);
}


void CLavaPEView::OnShowOptionals()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_DECL) {
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt)) {
        RemoveEmptyOpt(item, false);
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(hasEmptyOpt);
      }
      else {
        if (DrawEmptyOpt(item, false)) {
          (*(LavaDECL**)data->synEl)->TreeFlags.INCL(hasEmptyOpt);
          if (!data->toExpand)
            ExpandItem(item, 2);
        }
      }
      m_tree->scrollToItem(item, QAbstractItemView::EnsureVisible );
//      m_tree->ensureItemVisible(item);
    }
  }
}

void CLavaPEView::OnShowOverridables()
{
  CTreeItem* item = (CTreeItem*)m_tree->currentItem();
  if (item != 0) {
    CMainItemData* itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      LavaDECL* decl = *(LavaDECL**)itd->synEl;
      if ((decl->DeclType == Interface)
        || (decl->DeclType == Package) && decl->LocalName.l && decl->Supports.first) {
        GetDocument()->MakeVElems(decl);
        GetDocument()->OpenVTView((LavaDECL**)itd->synEl, (unsigned long)0);
      }
    }
  }
}

//Toolbutton update handler--------------------------------------------------


void CLavaPEView::OnUpdateComment(QAction* action)
{
  LavaDECL* decl;
  bool enable = !GetDocument()->changeNothing;
  if (enable) {
    TDECLComment* ptrComment;
    if (DataSel->type == TIType_DECL) {
      enable = true;
      ptrComment = (*(LavaDECL**)DataSel->synEl)->DECLComment.ptr;
      if (ptrComment)
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->statusBar()->message(ptrComment->Comment.c);
    }
    else {
      if (DataSel->type == TIType_CHEEnumSel)
        enable = true;
      else
        if ((DataSel->type == TIType_Require) || (DataSel->type == TIType_Ensure) || (DataSel->type == TIType_Exec)) {
          decl = GetExecDECL(ItemSel);
          enable = decl && decl->Exec.ptr;
        }
        else
          enable = false;
    }
  }
  action->setEnabled(enable);
}

void CLavaPEView::OnUpdateEditCopy(QAction* action)
{
  LavaDECL *decl = 0;
  if (DataSel && (DataSel->type == TIType_DECL))
    decl = *(LavaDECL**)DataSel->synEl;
  bool enable = (myInclView
       && (decl && !decl->TypeFlags.Contains(thisComponent)
                && !decl->TypeFlags.Contains(thisCompoForm)
           || (DataSel->type == TIType_CHEEnumSel))
           || decl && (decl->DeclType == FormText));
  action->setEnabled(enable);
}

void CLavaPEView::OnUpdateEditCut(QAction* action)
{
  bool enable = !GetDocument()->changeNothing;
  if (enable) {
    if (DataSel  && (DataSel->type == TIType_DECL))
      enable = EnableDelete(*(LavaDECL**)DataSel->synEl)
                && (myInclView
                  || ((*(LavaDECL**)DataSel->synEl)->DeclType == FormText)) ;
    else
      enable = DataSel && (DataSel->type == TIType_CHEEnumSel) && myInclView;
  }
  action->setEnabled(enable);
}

void CLavaPEView::OnUpdateEditPaste(QAction* action)
{
  bool enable = !GetDocument()->changeNothing;
  if (enable) {
    enable = clipboard_lava_notEmpty;
		if (enable)
      enable = (CanPaste(defTypeSpicked, treeflagsSpicked, secondtflagsSpicked, ItemSel) != TIType_NoType);
  }
  action->setEnabled(enable);
}


void CLavaPEView::OnUpdateEditSel(QAction* action)
{
  bool enable = false;
  if (myInclView) {
    enable = (DataSel->type == TIType_DECL)
             || (DataSel->type == TIType_Require)
             || (DataSel->type == TIType_Ensure)
             || (DataSel->type == TIType_Exec);
  }
  action->setEnabled(enable);
}


void CLavaPEView::OnUpdateFindreferences(QAction* action)
{
  action->setEnabled((DataSel->type == TIType_DECL) || (DataSel->type == TIType_CHEEnumSel));
}

void CLavaPEView::OnUpdateGotodef(QAction* action)
{
  action->setEnabled((DataSel->type == TIType_DECL)
               && ((CLavaPEApp*)wxTheApp)->Browser.CanBrowse(*(LavaDECL**)DataSel->synEl));
}

void CLavaPEView::OnUpdateGotoImpl(QAction* action)
{
  bool canBrowse = false;
  LavaDECL* DECL;
  if (DataSel->type == TIType_DECL
  || DataSel->type == TIType_Exec
  || DataSel->type == TIType_Ensure
  || DataSel->type == TIType_Require) {
    DECL = *(LavaDECL**)DataSel->synEl;
    canBrowse = (DECL->DeclType == Interface)
      || (DECL->DeclType == Function)
      || (DECL->DeclType == CompObjSpec)
      || (DECL->DeclType == Initiator)
      || (DECL->DeclType == Impl) && (DataSel->type == TIType_Exec)
      || (DECL->DeclType == Attr) && (DECL->ParentDECL->DeclType == Interface)
           && DECL->TypeFlags.Contains(hasSetGet);
    }
  action->setEnabled(canBrowse);
}

void CLavaPEView::OnUpdateMakeGUI(QAction* action)
{
  LavaDECL* DECL;
  CContext context;
  if (DataSel->type == TIType_DECL) {
    DECL = *(LavaDECL**)DataSel->synEl;
    if (!myInclView) {
      if (((DECL->DeclType == Attr) && (DECL->DeclDescType == NamedType)
         || (DECL->DeclType == VirtualType)
            && (DECL->SecondTFlags.Contains(isSet) || DECL->SecondTFlags.Contains(isArray)))
        )
        DECL = GetDocument()->IDTable.GetDECL(DECL->RefID);
      else
        DECL = 0;
    }
    if (DECL && (DECL->DeclType == Interface)
          && !DECL->TypeFlags.Contains(isAbstract)
          && (!DECL->TypeFlags.Contains(isGUI)
              || DECL->DeclType == FormDef)) {
      GetDocument()->IDTable.GetPattern(DECL, context);
      action->setEnabled(!context.oContext || (context.oContext == DECL));
      return;
    }
  }
  action->setEnabled(false);
}

void CLavaPEView::OnUpdateNewComponent(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
     (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ CompObj] != 0));
}

void CLavaPEView::OnUpdateNewCSpec(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
     (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ CompObjSpec] != 0));
}


void CLavaPEView::OnUpdateNewenum(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

void CLavaPEView::OnUpdateNewEnumItem(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
       ( (GroupType == TIType_EnumItems) || (GroupType == TIType_CHEEnumSel)));
}

void CLavaPEView::OnUpdateNewfunction(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
     (GroupType == TIType_Features) && (ContainTab[DefTypeSel] [ Function] != 0));
}

void CLavaPEView::OnUpdateNewImpl(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Impl] != 0));
}

void CLavaPEView::OnUpdateNewinitiator(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Initiator] != 0));
}

void CLavaPEView::OnUpdateNewInterface(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

void CLavaPEView::OnUpdateNewLitStr(QAction* action)
{
  action->setEnabled(!myInclView && (m_tree->currentItem() != m_tree->RootItem));
}

void CLavaPEView::OnUpdateNewmember(QAction* action)
{
  action->setEnabled( (!GetDocument()->changeNothing)
                 && ( (GroupType == TIType_Features)
                      && ( (ContainTab[DefTypeSel] [ Attr]  != 0)
                         || (*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(funcImpl))
                    || ( (GroupType == TIType_Input) && (ContainTab[DefTypeSel] [ IAttr] != 0)
                    || (GroupType == TIType_Output) && (ContainTab[DefTypeSel] [ OAttr] != 0) )
                      && !(*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(funcImpl)
                      && !(*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(overrides)
                      && ( (*(LavaDECL**)DataSel->synEl)->op == OP_noOp)));

}

void CLavaPEView::OnUpdateNewPackage(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Package] != 0));
}


void CLavaPEView::OnUpdateNewset(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

void CLavaPEView::OnUpdateNewVirtualType(QAction* action)
{
  action->setEnabled((!GetDocument()->changeNothing) &&
    (GroupType == TIType_VTypes) && (ContainTab[DefTypeSel] [ VirtualType] != 0));
}

void CLavaPEView::OnUpdateOverride(QAction* action)
{
  bool enable = !GetDocument()->changeNothing;
  if ( enable) {
    enable = (DataSel->type == TIType_DECL);
    if (enable) {
      LavaDECL* decl = *(LavaDECL**)DataSel->synEl;
      enable = ((decl->DeclType == Interface)
            || (decl->DeclType == Package) && decl->LocalName.l && decl->Supports.first);
    }
  }
  action->setEnabled(enable);
}

void CLavaPEView::OnUpdateShowformview(QAction* action)
{
  bool enable = false;
  if (!myInclView)
    enable = false;
  else
    enable = (DataSel->type == TIType_DECL)
                 && ((*(LavaDECL**)DataSel->synEl)->DeclType == Impl)
                 && (*(LavaDECL**)DataSel->synEl)->TypeFlags.Contains(isGUI);
  action->setEnabled(enable);
}

void CLavaPEView::OnUpdateShowOptionals(QAction* action)
{
  if (DataSel->type == TIType_DECL)
    if ((*(LavaDECL**)DataSel->synEl)->TreeFlags.Contains(hasEmptyOpt)
       && (!myInclView || (*(LavaDECL**)DataSel->synEl)->DeclType != FormDef)) {
      action->setOn(true);
      return;
    }
  action->setOn(false);
}

//--------------------------------------------------------------------------


TreeWhatsThis::TreeWhatsThis(MyListView *lv) : WhatsThis(0,lv) {
  listView = lv;
}


QString TreeWhatsThis::text(const QPoint &point) {
  CTreeItem *item=(CTreeItem*)listView->itemAt(point);
  CMainItemData *itd=(CMainItemData*)item->getItemData();
  LavaDECL *itemDECL;

  listView->setCurAndSel(item);

  switch (itd->type) {
  case TIType_DECL:
    itemDECL = *(LavaDECL**)itd->synEl;
    switch (itemDECL->DeclType) {
    case Package:
      return QString(QObject::tr("<p>This is a <a href=\"../Packages.htm#packages\">package</a>"
        " (= group of declarations/implementations that belong closely together</p>"));
    case Initiator:
      return QString(QObject::tr("<p>This is a <a href=\"../Packages.htm#initiator\">main program</a>"
        " (= <font color=\"red\"><b><i>Lava</i></b></font> main program)</p>"));
    case Interface:
      if (itemDECL->TypeFlags.Contains(isGUI))
        return QString(QObject::tr("<p>This is a <a href=\"../EditForm.htm#GUI\">GUI service</a> interface "
          "generated from a regular <font color=\"red\"><b><i>Lava</i></b></font> interface</p>"));
      else if (itemDECL->SecondTFlags.Contains(isChain)
      || itemDECL->SecondTFlags.Contains(isSet))
        return QString(QObject::tr("<p>This is a <a href=\"SetChain.htm\">Set or Chain</a> type</p>"));
      else if (itemDECL->SecondTFlags.Contains(isEnum))
        return QString(QObject::tr("<p>This is an <a href=\"Enumeration.htm\">enumeration type</a></p>"));
      else
        return QString(QObject::tr("<p>This is an <a href=\"../SepItfImpl.htm\">interface</a>"
        " (= the public part of a <font color=\"red\"><b><i>Lava</i></b></font> class)</p>"));
    case FormDef:
      return QString(QObject::tr("<p>This is a <a href=\"../EditForm.htm#GUI\">GUI service</a> implementation "
        "generated from a regular <font color=\"red\"><b><i>Lava</i></b></font> interface</p>"));
    case Impl:
      if (itemDECL->TypeFlags.Contains(isGUI))
        return QString(QObject::tr("<p>This is a <a href=\"../EditForm.htm#GUI\">GUI service</a> implementation "
          "generated from a regular <font color=\"red\"><b><i>Lava</i></b></font> interface</p>"));
      else
        return QString(QObject::tr("<p>This is an <a href=\"../SepItfImpl.htm\">implementation</a>"
          " (= the private part of a <font color=\"red\"><b><i>Lava</i></b></font> class)</p>"));
    case CompObjSpec:
      return QString(QObject::tr("<p>This is a <a href=\"../Components.htm\">component object</a> specification</p>"));
    case CompObj:
      return QString(QObject::tr("<p>This is a <a href=\"../Components.htm\">component object</a> implementation</p>"));
    case VirtualType:
      return QString(QObject::tr("<p>This is a <a href=\"../PatternsFrameworks.htm#VT\">virtual type</a> declaration</p>"));
    case Function:
      if (itemDECL->TypeFlags.Contains(isInitializer))
        return QString(QObject::tr("<p>This is an <a href=\"../ObjectLifeCycle.htm#initializer\">initializer</a> of the above <font color=\"red\"><b><i>Lava</i></b></font> class</p>"));
      else
        return QString(QObject::tr("<p>This is a <a href=\"MemberFunctions.htm\">member function</a> of the above <font color=\"red\"><b><i>Lava</i></b></font> class</p>"));
    case Attr:
      return QString(QObject::tr("<p>This is a <a href=\"..//dialogs/MemVarBox.htm\">member variable</a> of a <font color=\"red\"><b><i>Lava</i></b></font> class</p>"));
    case IAttr:
      return QString(QObject::tr("<p>This is an <a href=\"..//dialogs/FuncParmBox.htm\">input parameter</a> of the above function</p>"));
    case OAttr:
      return QString(QObject::tr("<p>This is an <a href=\"..//dialogs/FuncParmBox.htm\">output parameter</a> of the above function</p>"));
    }
    break;
  case TIType_CHEEnumSel:
    return QString(QObject::tr("<p>This is an <a href=\"Enumeration.htm\">enumerated item</a></p>"));
  case TIType_EnumItems:
    return QString(QObject::tr("<p>This is the actual <a href=\"Enumeration.htm\">enumeration</a> of an enumeration type</p>"));
  case TIType_Exec:
    itemDECL = *(LavaDECL**)itd->synEl;
    if (itemDECL->DeclType == Interface)
      return QString(QObject::tr("<p>This is the (optional) <a href=\"../DBC.htm\">invariant</a> of the above interface</p>"));
    else if (itemDECL->DeclType == Impl)
      return QString(QObject::tr("<p>This is the (optional) <a href=\"../DBC.htm\">invariant</a> of the above implementation</p>"));
    else
      return QString(QObject::tr("<p>This is the <a href=\"../EditExec.htm#exec\">exec</a> (= executable body) of the above function</p>"));
  case TIType_Require:
    return QString(QObject::tr("<p>This is the (optional) <a href=\"../DBC.htm\">precondition</a> of the above function</p>"));
  case TIType_Ensure:
    return QString(QObject::tr("<p>This is the (optional) <a href=\"../DBC.htm\">postcondition</a> of the above function</p>"));
  case TIType_VTypes:
    return QString(QObject::tr("<p>These are the (optional) <a href=\"../PatternsFrameworks.htm#VT\">virtual types</a> of the above class or package</p>"));
  }
  return QString(QObject::tr("<p>No specific help available for this declaration item</p>"));
}
