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


#include "docview.h"
#include "mdiframes.h"
#include "qapplication.h"
#include "qframe.h"
#include "qtoolbar.h"
#include "qcombobox.h"
#include <QMenu>
#include <QTreeWidget>
#include "qsplitter.h"
#include "qevent.h"
//#include "q3header.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QHeaderView>
#include "TreeView.h"
#include "Resource.h"
#include "qpixmap.h"
#include "qpixmapcache.h"
#include "LavaPE.h"

#pragma hdrstop


CTreeItem::CTreeItem (QString label, int ipix, CTreeItem* parent, CTreeItem* afterItem)
  :QTreeWidgetItem(parent, afterItem)
{
  pixmapIndex = ipix;
  nPix = ((CLavaPEApp*)wxTheApp)->LavaIcons[ipix];
  normalPix = nPix;
  delPix = false;
  data = 0;
  setText(0,label);
  setIcon(0, *nPix);
//  setHeight(16);
//  setVisible(true);
  inRename = false;
}

CTreeItem::CTreeItem(QString label, QTreeWidget* parent, CTreeItem* afterItem)
  :QTreeWidgetItem(parent, afterItem)
{
  delPix = false;
  nPix = 0;
  normalPix = 0;
  data = 0;
  //setHeight(16);
  setText(0,label);
//  setVisible(true);
  inRename = false;
}

CTreeItem::CTreeItem(QString label, QTreeWidget* parent)
  :QTreeWidgetItem(parent)
{
  delPix = false;
  nPix = 0;
  normalPix = 0;
  data = 0;
  //setHeight(16);
  setText(0,label);
//  setVisible(true);
  inRename = false;
}

CTreeItem::CTreeItem (QString label, int ipix, MyListView* parent)
  :QTreeWidgetItem(parent)
{
  pixmapIndex = ipix;
  nPix = ((CLavaPEApp*)wxTheApp)->LavaIcons[ipix];
  normalPix = nPix;
  delPix = false;
  data = 0;
  setText(0,label);
  setIcon(0, *nPix);
//  setHeight(16);
//  setVisible(true);
  inRename = false;
  parent->RootItem = this;
}

CTreeItem::~CTreeItem()
{
  if (delPix) {
    delete nPix;
  }
}


void CTreeItem::startRename(QLineEdit* editor, int col)
{
  if (!inRename) {
    inRename = true;
    ((MyListView*)treeWidget())->lavaView->RenameStart(editor, this);
  }
}

void CTreeItem::okRename(QLineEdit* editor, int col)
{
  if (inRename)
    ((MyListView*)treeWidget())->lavaView->RenameOk(editor, this);
  inRename = false;
}

void CTreeItem::cancelRename(int col)
{
  if (inRename)
    ((MyListView*)treeWidget())->lavaView->RenameCancel(this);
  inRename = false;
}


void CTreeItem::setPixmapIndex(int ipix)
{
  pixmapIndex = ipix;
  SetIcon(((CLavaPEApp*)wxTheApp)->LavaIcons[ipix]);
}

void CTreeItem::SetIcon(QIcon* icon)
{
  if (delPix) {
    delete nPix;
    delPix = false;
  }
  nPix = icon;
  normalPix = icon;
  setIcon(0, *nPix);
}


void CTreeItem::SetItemMask(bool hasErr, bool hasCom)
{
  if (hasErr && hasCom)
    SetItemMask(xerrcomBM);//QPixmapCache::find("l_xerrcommask"));
  else if (hasErr)
    SetItemMask(xerrBM);//QPixmapCache::find("l_xerrmask"));
  else if (hasCom)
    SetItemMask(xcomBM);//QPixmapCache::find("l_xcommask"));
  else
    SetIcon(normalPix);
}

void CTreeItem::SetItemMask(int maskIndex) {
  //pixMask wird oben und rechts bndig in nPix kopiert
  QPixmap mPix = QPixmap(((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex]->width(), ((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex]->height());
  mPix.fill();
  QPainter painter(&mPix);
  painter.drawPixmap(0,0,*((CLavaPEApp*)wxTheApp)->LavaPixmaps[pixmapIndex]);
//bitBlt (mPix, 0, 0, ((CLavaPEApp*)wxTheApp)->LavaPixmaps[pixmapIndex],0,0,16,((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex]->height());
  painter.drawPixmap(16,0,*((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex],16,0,5,((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex]->height());
  //bitBlt (mPix, 16, 0, ((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex],QRect(16,0,5,((CLavaPEApp*)wxTheApp)->LavaPixmaps[maskIndex]->height()));
  if (delPix)
    delete nPix;
  nPix = new QIcon(mPix);
  delPix = true;
  setIcon(0, *nPix);
//  delete mPix;
}

CTreeItem* CTreeItem::nextSibling()
{
  if (parent()) {
    int ind = parent()->indexOfChild(this);
    return (CTreeItem*)parent()->child(ind+1);
  }
  else
    return 0;
}

void CTreeItem::setDropEnabled(bool enable)
{
  if (enable)
    setFlags(flags() | Qt::ItemIsDropEnabled);
  else
    setFlags(flags() & ~Qt::ItemIsDropEnabled);
}


void CTreeItem::setRenameEnabled(int col, bool enable)
{
  if (enable)
    setFlags(flags() | Qt::ItemIsEditable);
  else
    setFlags(flags() & ~Qt::ItemIsEditable);
}

bool CTreeItem::renameEnabled(int col)
{
  return flags() & Qt::ItemIsEditable;
}

//**********************************************************************************


MyListView::MyListView(CTreeView* view)
:QTreeWidget(view)
{
  RootItem = 0;
  withShift = false;
  withControl = false;
  lavaView = view;
  setFocusPolicy(Qt::StrongFocus);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  viewport()->setAcceptDrops(true);
  setAutoScroll(true);
  setEditTriggers(QAbstractItemView::SelectedClicked);
}

void MyListView::setCurAndSel(QTreeWidgetItem* item, bool singleSel)
{
  QTreeWidgetItem *oldi = currentItem();
  if (oldi != item) {
    if (oldi && singleSel && oldi->isSelected()) //isItemSelected(oldi))//oldi->isSelected())
      oldi->setSelected(false);
    setCurrentItem(item);
  }
  if (!item->isSelected()) //isItemSelected(item)) //->isSelected())
    item->setSelected(true);
  QSize s1 = size();
  QSize s2 = viewport()->size();
  QSize s3 = lavaView->size();

}

void MyListView::ResetSelections()
{
  QList<QTreeWidgetItem *> list = selectedItems ();
  for (int ii = 0; ii < list.size(); ii++)
     list.at(ii)->setSelected(false);
}

bool MyListView::event(QEvent *ev) {
  QHelpEvent *hevWT;
  WhatNextEvent *wnev;
  QString href;

  if (ev->type() == UEV_WhatNext) {
    wnev = (WhatNextEvent*)ev;
    hevWT = new QHelpEvent(QEvent::WhatsThis,
      wnev->pos+QPoint(100,18),
      wnev->globalPos+QPoint(100,18));
    return QWidget::event(hevWT);
  }
  else
    return QTreeWidget::event(ev);
}

void MyListView::keyPressEvent(QKeyEvent *ev)
{
  if ((ev->key() == Qt::Key_Delete // 0x2e DEL key
  || ev->key() == Qt::Key_Backspace)) // 0x08 BACKSPACE key
    lavaView->OnDelete();
  else if ((ev->key() == Qt::Key_Return) && !((CTreeItem*)currentItem())->inRename)
    lavaView->OnVkreturn();
  else {
    withShift = ev->modifiers() & Qt::ShiftModifier;
    withControl = ev->modifiers() & Qt::ControlModifier;
    QTreeWidget::keyPressEvent(ev);
    withShift = false;
    withControl = false;
  }
}

void MyListView::mouseMoveEvent(QMouseEvent *ev)
{
  QTreeWidgetItem* item2, *item = currentItem();
  if (!lavaView->multiSelectCanceled && item && (item->flags() & Qt::ItemIsDragEnabled))
    QTreeWidget::mouseMoveEvent(ev);
  else {
    QPoint p = /*contentsToViewport(*/ev->pos();//);
    item2 = itemAt(p);
    if (!lavaView->multiSelectCanceled && (ev->modifiers() == Qt::LeftButton)) {
      if (item2 != item) {
        withControl = true;
        setCurrentItem(item2);
        item2->setSelected(true);
        withControl = false;
      }
    }
    else
      if (lavaView->multiSelectCanceled) {
        ResetSelections();
        setCurAndSel(item2);
      }
  }
}

void MyListView::mouseReleaseEvent(QMouseEvent *ev)
{
  lavaView->multiSelectCanceled = false;
  QTreeWidget::mouseReleaseEvent(ev);
}


void MyListView::mousePressEvent(QMouseEvent *ev)
{
  withShift = ev->modifiers() & Qt::ShiftModifier;
  withControl = ev->modifiers() & Qt::ControlModifier;
  QPoint p = /*contentsToViewport(*/ev->pos();//);
  if(itemAt(p))
    QTreeWidget::mousePressEvent(ev);
  withShift = false;
  withControl = false;
  if(!itemAt(p) && currentItem()) {
    currentItem()->setSelected(true);
//    ensureItemVisible(currentItem());
//    currentItem()->repaint();
  }
}

void MyListView::startDrag(Qt::DropActions supportedActions)
{
  lavaView->OnDragBegin();
}

void MyListView::dragMoveEvent(QDragMoveEvent *ev)
{
  if (Qt::ShiftModifier == ev->keyboardModifiers())
    ev->setDropAction(Qt::MoveAction);
  else
    if (Qt::ControlModifier == ev->keyboardModifiers())
      ev->setDropAction(Qt::CopyAction);
    else
      ev->setDropAction(Qt::MoveAction);
  QAbstractItemView::dragMoveEvent(ev);
  lavaView->OnDragOver(ev);
}

void MyListView::dragEnterEvent(QDragEnterEvent *ev)
{
  if (Qt::ShiftModifier == ev->keyboardModifiers())
    ev->setDropAction(Qt::MoveAction);
  else
    if (Qt::ControlModifier == ev->keyboardModifiers())
      ev->setDropAction(Qt::CopyAction);
    else
      ev->setDropAction(Qt::MoveAction);
  lavaView->OnDragEnter(ev);
}

void MyListView::dragLeaveEvent(QDragLeaveEvent *ev)
{
  lavaView->OnDragLeave(ev);
  QAbstractItemView::dragLeaveEvent(ev);
}

void MyListView::dropEvent(QDropEvent* ev)
{
  if (Qt::ShiftModifier == ev->keyboardModifiers())
    ev->setDropAction(Qt::MoveAction);
  else
    if (Qt::ControlModifier == ev->keyboardModifiers())
      ev->setDropAction(Qt::CopyAction);
    else
      ev->setDropAction(Qt::MoveAction);
  lavaView->OnDrop(ev);
  stopAutoScroll();
  setState(NoState);
}

/*
void MyListView::focusInEvent ( QFocusEvent * e )
{
  lavaView->focusInEvent (e);

}*/

CTreeItem* MyListView::itemAtIndex(const QModelIndex& index)
{
 return (CTreeItem*)itemFromIndex(index);
}

CTreeItem* MyListView::itemAbove(CTreeItem* item)
{
  QModelIndex ind = indexAbove(indexFromItem(item));
  if (ind.isValid())
    return (CTreeItem*)itemFromIndex(ind);
  else
    return 0;
}

void MyListView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
  if (committed)
    ((CTreeItem*)currentItem())->okRename((QLineEdit*)editor, 0);
  else
    ((CTreeItem*)currentItem())->cancelRename(0);
  committed = false;
  QAbstractItemView::closeEditor(editor, hint);
}

void MyListView::commitData(QWidget* editor)
{
  committed = true;
}


CTreeItemDelg::CTreeItemDelg(MyListView* tree)
{
  Tree = tree;
  Tree->committed = false;
}

void CTreeItemDelg::setEditorData(QWidget * editor, const QModelIndex & index ) const
{
  ((CTreeItem*)Tree->currentItem())->startRename((QLineEdit*)editor, 0);
}

QWidget* CTreeItemDelg::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  QWidget* ed = QItemDelegate::createEditor(parent, option, index);
  ((QLineEdit*)ed)->setFrame(true);
  return ed;
}

//**********************************************************************************

CTreeView::CTreeView(QWidget *parent,wxDocument *doc, const char* name)
   : CLavaBaseView(parent,doc,name)
{
  Tree = new MyListView(this);
  layout->addWidget(Tree);
  //  setFocusProxy(Tree);
//  Tree->setSorting(-1);
  Tree->setColumnCount(1);
  Tree->setRootIsDecorated(true);
  Tree->header()->hide();
  Tree->setSelectionMode(QAbstractItemView::ExtendedSelection);//Single);
  CollectDECL = 0;
  multiSelectCanceled = false;
}


CTreeItem* CTreeView::GetPrevSiblingItem(CTreeItem* item)
{
  CTreeItem* ib = (CTreeItem*)Tree->itemAbove(item);
  CTreeItem* par = (CTreeItem*)item->parent();
  if (ib == par)
    return 0;
  else {
    for ( ; ib && (par != ib->parent()); ib = (CTreeItem*)Tree->itemAbove(ib));
    return ib;
  }
}

/*
CTreeItem* CTreeView::HitTest(QPoint qp)
{
  return (CTreeItem*)Tree->itemAt(qp);
}
*/

CTreeItem* CTreeView::InsertItem(QString label, int nPix, CTreeItem* parent, CTreeItem* afterItem)
{
  if (parent == MY_TVI_ROOT)
    return new CTreeItem(label, nPix, Tree);
  else {
    if (afterItem == MY_TVI_FIRST)
      return new CTreeItem(label, nPix, parent, 0);
    else if (afterItem == MY_TVI_LAST) {
      afterItem = (CTreeItem*)parent->child(parent->childCount()-1);
      /*
      for (afterItem = (CTreeItem*)parent->child(0); afterItem && afterItem->nextSibling(); afterItem = (CTreeItem*)afterItem->nextSibling());
      */
      return new CTreeItem(label, nPix, parent, afterItem);
    }
    else
      return new CTreeItem(label, nPix, parent, afterItem);
  }
}

