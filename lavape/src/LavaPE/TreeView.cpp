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
#include "q3frame.h"
#include "qtoolbar.h"
#include "q3combobox.h"
#include "q3popupmenu.h"
#include "q3listview.h"
#include "qsplitter.h"
#include "q3vbox.h"
#include "qevent.h"
#include "q3header.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include "TreeView.h"
#include "Resource.h"
#include "qpixmap.h"
#include "qpixmapcache.h"
#include "LavaPE.h"


CTreeItem::CTreeItem (QString label, QPixmap* pix, CTreeItem* parent, CTreeItem* afterItem)
  :Q3ListViewItem(parent, afterItem) 
{ 
  nPix = pix; 
  normalPix = pix;
  delPix = false;
  data = 0; 
  setHeight(16);
  setText(0,label);
  setVisible(true);
  inRename = false;
}

CTreeItem::CTreeItem(QString label, Q3ListView* parent, CTreeItem* afterItem)
  :Q3ListViewItem(parent, afterItem) 
{ 
  delPix = false;
  nPix = 0;
  normalPix = 0;
  data = 0; 
  //setHeight(16);
  setText(0,label);
  setVisible(true);
  inRename = false;
}

CTreeItem::CTreeItem (QString label, QPixmap* pix, Q3ListView* parent)
  :Q3ListViewItem(parent) 
{ 
  nPix = pix;
  normalPix = pix;
  delPix = false;
  data = 0; 
  setHeight(16);
  setText(0,label);
  setVisible(true);
  inRename = false;
}

CTreeItem::~CTreeItem()
{
  if (delPix) {
    delete nPix;
  } 
}

void CTreeItem::startRename(int col)
{
  if (!inRename) {
    inRename = true;
    ((MyListView*)listView())->lavaView->RenameStart(this);
    Q3ListViewItem::startRename(col);
  }
}

void CTreeItem::okRename(int col)
{
  Q3ListViewItem::okRename(col);
  ((MyListView*)listView())->lavaView->RenameOk(this);
  inRename = false;
}

void CTreeItem::cancelRename(int col)
{
  ((MyListView*)listView())->lavaView->RenameCancel(this);
  Q3ListViewItem::cancelRename(col);
  inRename = false;
}

void CTreeItem::setPix(QPixmap* pix)
{
  if (delPix) {
    delete nPix;
    delPix = false;
  } 
  nPix = pix;
  normalPix = pix;
}

void CTreeItem::SetItemMask(bool hasErr, bool hasCom)
{
  if (hasErr && hasCom)
    SetItemMask(((CLavaPEApp*)wxTheApp)->LavaPixmaps[xerrcomBM]);//QPixmapCache::find("l_xerrcommask"));
  else if (hasErr)
    SetItemMask(((CLavaPEApp*)wxTheApp)->LavaPixmaps[xerrBM]);//QPixmapCache::find("l_xerrmask"));
  else if (hasCom)
    SetItemMask(((CLavaPEApp*)wxTheApp)->LavaPixmaps[xcomBM]);//QPixmapCache::find("l_xcommask"));
  else 
    setPix(normalPix);
}

void CTreeItem::SetItemMask(QPixmap* pixMask) {
  //pixMask wird oben und rechts bündig in nPix kopiert
  QPixmap* mPix = new QPixmap(nPix->width(), nPix->height());
  mPix->fill();
  bitBlt (mPix, 0, 0, nPix, 0, 0, 16, pixMask->height(), Qt::CopyROP, true);
  bitBlt (mPix, 16, 0, pixMask, 16, 0, 5, pixMask->height(), Qt::CopyROP, true);
  if (delPix) 
    delete nPix;
  nPix = mPix;
  delPix = true;
}

//**********************************************************************************


MyListView::MyListView(CTreeView* view)
:Q3ListView(view, "MyListView")
{
  withShift = false;
  withControl = false;
  lavaView = view;
  setFocusPolicy(Qt::StrongFocus);
  viewport()->setAcceptDrops(true);
  setDragAutoScroll(true);
}

void MyListView::setCurAndSel(Q3ListViewItem* item, bool singleSel)
{
  Q3ListViewItem *oldi = currentItem();
  if (oldi != item) {
    if (oldi && singleSel && oldi->isSelected())
      setSelected(oldi, false);
    setCurrentItem(item);
  }
  if (!item->isSelected())
    setSelected(item, true);
}

void MyListView::keyPressEvent(QKeyEvent *ev)
{
  if ((ev->key() == Qt::Key_Delete // 0x2e DEL key
   || ev->key() == Qt::Key_Backspace)) // 0x08 BACKSPACE key

    lavaView->OnDelete();
  else if ((ev->key() == Qt::Key_Return) && !((CTreeItem*)currentItem())->inRename)
    lavaView->OnVkreturn();
  else {
    withShift = ev->state() & Qt::ShiftModifier;
    withControl = ev->state() & Qt::ControlModifier;
    Q3ListView::keyPressEvent(ev);
    withShift = false;
    withControl = false;
  }
}

void MyListView::contentsMouseMoveEvent(QMouseEvent *ev)
{
  Q3ListViewItem* item2, *item = currentItem();
  if (!lavaView->multiSelectCanceled && item && item->dragEnabled())
    Q3ListView::contentsMouseMoveEvent(ev);
  else {
    QPoint p = contentsToViewport(ev->pos());
    item2 = itemAt(p);
    if (!lavaView->multiSelectCanceled && (ev->state() == Qt::LeftButton)) {
      if (item2 != item) {
        withControl = true;
        setCurrentItem(item2);
        setSelected(item2, true);
        withControl = false;
      }
    }
    else 
      if (lavaView->multiSelectCanceled) {
        selectAll(false);
        setCurAndSel(item2);
      } 
  }
}

void MyListView::contentsMouseReleaseEvent(QMouseEvent *ev)
{
  lavaView->multiSelectCanceled = false;
  Q3ListView::contentsMouseReleaseEvent(ev);
}


void MyListView::contentsMousePressEvent(QMouseEvent *ev)
{
  withShift = ev->state() & Qt::ShiftModifier;
  withControl = ev->state() & Qt::ControlModifier;
  QPoint p = contentsToViewport(ev->pos());
  if(itemAt(p)) 
    Q3ListView::contentsMousePressEvent(ev);
  withShift = false;
  withControl = false;
  if(!itemAt(p)) {
    setSelected(currentItem(), true);
    ensureItemVisible(currentItem());
    currentItem()->repaint();
  }
}


Q3DragObject* MyListView::dragObject()
{
  return lavaView->OnDragBegin();
}

void MyListView::contentsDragMoveEvent(QDragMoveEvent *ev)
{
  lavaView->OnDragOver(ev);
}

void MyListView::contentsDragEnterEvent(QDragEnterEvent *ev)
{
  lavaView->OnDragEnter(ev);
}

void MyListView::contentsDragLeaveEvent(QDragLeaveEvent *ev)
{
  lavaView->OnDragLeave(ev);
}

void MyListView::contentsDropEvent(QDropEvent* ev)
{
  lavaView->OnDrop(ev);
}


void MyListView::focusInEvent ( QFocusEvent * e )
{
  lavaView->focusInEvent (e);

}

//**********************************************************************************

CTreeView::CTreeView(QWidget *parent,wxDocument *doc, const char* name): CLavaBaseView(parent,doc,name)
{ 
  m_tree = new MyListView(this);
	//setFocusProxy(m_tree);
  m_tree->setSorting(-1);
  m_tree->addColumn("");
  m_tree->setRootIsDecorated(true);
  m_tree->header()->hide();
  m_tree->setSelectionMode(Q3ListView::Extended);//Single);
  CollectDECL = 0;
  multiSelectCanceled = false;
}



CTreeItem* CTreeView::GetPrevSiblingItem(CTreeItem* item)
{
  CTreeItem* ib = (CTreeItem*)item->itemAbove();
  CTreeItem* par = (CTreeItem*)item->parent();
  if (ib == par)
    return 0;
  else {
    for ( ; ib && (par != ib->parent()); ib = (CTreeItem*)ib->itemAbove());
    return ib;
  }
}

/*
CTreeItem* CTreeView::HitTest(QPoint qp)
{
  return (CTreeItem*)m_tree->itemAt(qp);
}
*/

CTreeItem* CTreeView::InsertItem(QString label, QPixmap* nPix, CTreeItem* parent, CTreeItem* afterItem)
{
  if (parent == TVI_ROOT)
    return new CTreeItem(label, nPix, m_tree);
  else {
    if (afterItem == TVI_FIRST)
      return new CTreeItem(label, nPix, parent, 0);
    else if (afterItem == TVI_LAST) {
      for (afterItem = (CTreeItem*)parent->firstChild(); afterItem && afterItem->nextSibling(); afterItem = (CTreeItem*)afterItem->nextSibling());
      return new CTreeItem(label, nPix, parent, afterItem);
    }
    else
      return new CTreeItem(label, nPix, parent, afterItem);
  }
}

