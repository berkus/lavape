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


#include "LavaPEStringInit.h"
#include "LavaPE.h"
#include "TreeView.h"
#include "Bars.h"
//#include "ExecView.h"
#include "LavaPEDoc.h"
#include "LavaPEFrames.h"
#include "Boxes.h"
#include "qtabbar.h"
#include "qstatusbar.h"
#include "qheader.h"
#include "qmessagebox.h"

/////////////////////////////////////////////////////////////////////////////
// CSizeDlgBar
// The base class for resizable dialog bars. In the dialog template choose the 
// resize border style. Use the WM_SIZE message to arrange the control items on the
// dialog bar after the size of the bar has changed (first call CSizeBar::OnSize).
//


COutputBar::COutputBar(QWidget *parent)
: QTabWidget(parent, "OutputBar")
{
	QString emptyString;
	
  FindPage = new QListView(this);
  FindPage->setSorting(-1);
  FindPage->addColumn(emptyString);
  FindPage->setRootIsDecorated(false);
  FindPage->header()->hide();
  FindPage->setSelectionMode(QListView::Single);//Extended); 
  CommentPage = new QTextEdit(this);
  CommentPage->setReadOnly(true);
//  QTextEdit *CommentPage2 = new QTextEdit(this);
//  CommentPage2->setReadOnly(true);
  ErrorPage = new QTextEdit(this);
  ErrorPage->setReadOnly(true);
  DebugPage = new QSplitter(this);
  DebugPage->setOrientation(Qt::Horizontal);
  VarView = new VarListView(DebugPage, this);
  StackView = new StackListView(DebugPage, this);
  QValueList<int> list = DebugPage->sizes();
  QValueList<int>::Iterator it = list.begin();
  int totalW = *it;
  ++it;
  totalW += *it;
  it = list.begin();
  *it = totalW/4 * 3;
  ++it;
  *it = totalW/4;
  DebugPage->setSizes(list);
  QIconSet icoFind = QIconSet(QPixmap((const char**)PX_findtab));
  QIconSet icoErr = QIconSet(QPixmap((const char**)PX_errtab));
  QIconSet icoCom = QIconSet(QPixmap((const char**)PX_commentt));
  QIconSet icoDebug = QIconSet(QPixmap((const char**)PX_debugTab));
  setTabPosition(Bottom);
//  addTab (CommentPage, icoCom, "Comment" ); 
  addTab (CommentPage, icoCom, "Comment" ); 
  addTab (ErrorPage, icoErr, "Error" ); 
  addTab (FindPage, icoFind, "Find" ); 
  addTab (DebugPage, icoDebug, "Debug" ); 
  QSize sz = tabBar()->size();
  tabBar()->resize(sz.width(), 16);
  ActTab = tabComment;
  setCurrentPage((int)ActTab);
  connect(this,SIGNAL(currentChanged(QWidget*)), SLOT(OnTabChange(QWidget*)));
  connect(FindPage,SIGNAL(doubleClicked(QListViewItem*)), SLOT(OnDblclk(QListViewItem*)));
  ErrorEmpty = true;
  CommentEmpty = true;
}

COutputBar::~COutputBar()
{
}


void COutputBar::ResetError()
{
  ErrorPage->clear();
  ErrorEmpty = true;
}

void COutputBar::SetErrorOnBar(LavaDECL* decl)
{
  QString cstrA;
  setError(decl->DECLError1, &cstrA);
  setError(decl->DECLError2, &cstrA);
  ErrorPage->setText(cstrA);
  ErrorEmpty = (cstrA == QString::null) || !cstrA.length();
}

void COutputBar::SetErrorOnBar(const CHAINX& ErrChain)
{
  QString cstrA;
  setError(ErrChain, &cstrA);
  ErrorPage->setText(cstrA);
  ErrorEmpty = (cstrA == QString::null) || !cstrA.length();
}

void COutputBar::setError(const CHAINX& ErrChain, QString* cstrA)
{
  QString cstr;
  CHE* che = (CHE*)ErrChain.first;
  while (che) {
    cstr = *((CLavaError*)che->data)->IDS;
    if (((CLavaError*)che->data)->textParam.l)
      cstr = QString(((CLavaError*)che->data)->textParam.c) + cstr;
    if (cstrA->length())
      *cstrA += "\r\n";
    else
      wxTheApp->m_appWindow->statusBar()->message(cstr);
    *cstrA += cstr;
    che = (CHE*)che->successor;
  }
}

void COutputBar::SetComment(const DString& text, bool toStatebar)
{
  if (text.l) {
    CommentPage->setText(text.c);
    if (toStatebar)
      wxTheApp->m_appWindow->statusBar()->message(text.c);
  }
  else {
    CommentPage->clear();
    if (toStatebar)
      wxTheApp->m_appWindow->statusBar()->message(IdlMsg);
  }
  CommentEmpty = !text.l;
}

void COutputBar::SetFindText(const DString& text, CFindData* data)
{
  CTreeItem *item = new CTreeItem(text.c, FindPage, (CTreeItem*)FindPage->currentItem());
  FindPage->setCurrentItem(item);
  item->setItemData((TItemData*)data);
}

void COutputBar::OnDblclk(QListViewItem* item)
{
  CFindData *data;
  TID tid;
  CLavaPEDoc *doc;
  CSearchData sData;
  LavaDECL *decl, *execDecl;
  TDeclType type;

  data = (CFindData*)((CTreeItem*)item)->getItemData();
  doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(data->fname.c);
  if (doc) {
    decl = doc->IDTable.GetDECL(0, data->nID);
    if (decl) {
      if (!data->index) {
        if (doc->TrueReference(decl, data->refCase, data->refTid))
          ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, decl, tid, true, &data->enumID);
        else
          QMessageBox::critical(this, qApp->name(), IDP_RefNotFound,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      }
      else if (data->index == 2)
        ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, decl, tid, true, &data->enumID);
      else {
        if (data->index < 100)
          type = (TDeclType)data->index;
        else
          type = (TDeclType)(data->index - 100);
        execDecl = doc->GetExecDECL(decl, type, false,false);
        if (execDecl) {
          sData.synObjectID = data->refCase;
          doc->OpenExecView(execDecl);
          sData.execView = wxDocManager::GetDocumentManager()->GetActiveView();
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&doc->IDTable, 0, (SynObjectBase*)execDecl, onSelect, 0,0, (address)&sData);
        }
      }
    }
  }
}


void COutputBar::DeleteAllFindItems()
{
  CTreeItem* item;
  CFindData * itd;
  for (item = (CTreeItem*)FindPage->firstChild();
       item;
       item = (CTreeItem*)item->nextSibling()) {
    itd = (CFindData*)item->getItemData();
    if (itd)
      delete itd;
  }
  FindPage->clear();
}

void COutputBar::SetTab(BarTabs tab)
{
  setCurrentPage((int)tab);
  if (((CLavaMainFrame*)wxTheApp->m_appWindow)->OutputBarHidden) {
    show();
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastBarState = -1;
  }
  else
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastBarState = (int)ActTab;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->OutputBarHidden = false;
}


void COutputBar::OnTabChange(QWidget* curPage)
{
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastBarState = (int)ActTab;
  ActTab = (BarTabs)currentPageIndex();
  if (ActTab == tabFind) {
    FindPage->show();
    setUpdatesEnabled(true);
  }
}

void COutputBar::setDebugData(DebugMessage* message, CLavaBaseDoc* doc)
{
  switch (message->Command) {
  case Dbg_MemberData:
    if (VarView->itemToOpen) {
      VarView->itemToOpen->makeChildren(((ObjItemData*)message->ObjData.ptr)->Children);
      VarView->itemToOpen->setOpen(true);
      VarView->setSelected(VarView->itemToOpen, true);
      VarView->itemToOpen = 0;
    }
    break;
  case Dbg_StopData: 
    StackView->makeItems((DebugStopData*)message->DbgData.ptr, doc);
  case Dbg_Stack: 
    showExecStackPos((DebugStopData*)message->DbgData.ptr, doc);
    VarView->makeItems((DebugStopData*)message->DbgData.ptr);
    break;
  default:;
  }
  SetTab(tabDebug);

}


void COutputBar::showExecStackPos(DebugStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData* chData = (CHEStackData*)data->StackChain.GetNth(data->ActStackLevel+1);
  LavaDECL *funcDecl, *execDecl;
  CLavaBaseDoc* actDoc;
  CSearchData sData;
  TID tid;

  funcDecl = doc->IDTable.GetDECL(chData->data.FuncID);
  ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, funcDecl, tid/*chData->data.FuncID*/, true, 0, false);
  if (funcDecl->inINCL) {
    DString fn = ((CLavaPEDoc*)doc)->IDTable.GetRelSynFileName(chData->data.FuncID);
    AbsPathName(fn, ((CLavaPEDoc*)doc)->IDTable.DocDir);
    actDoc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(fn.c);
      funcDecl = actDoc->IDTable.GetDECL(0, chData->data.FuncID.nID);
  }
  else
    actDoc = doc;
  execDecl = actDoc->GetExecDECL(funcDecl, chData->data.ExecType, false,false);
  if (execDecl) {
    sData.synObjectID = chData->data.SynObjID;
    ((CLavaPEDoc*)actDoc)->OpenExecView(execDecl);
    sData.execView = wxDocManager::GetDocumentManager()->GetActiveView();
    ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&actDoc->IDTable, 0, (SynObjectBase*)execDecl, onSelect, 0,0, (address)&sData);
  }
}


VarItem::VarItem(VarItem* parent, VarItem* afterItem, ObjItemData* data)
  :QListViewItem(parent, afterItem) 
{
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarItem* parent, ObjItemData* data)
  :QListViewItem(parent) 
{
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, VarItem* afterItem, ObjItemData* data)
  :QListViewItem(parent, afterItem) 
{
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  myView = parent;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, ObjItemData* data)
  :QListViewItem(parent) 
{
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  myView = parent;
  isPriv = data->isPrivate;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}


void VarItem::makeChildren(const CHAINX& data)
{
  CHE* chData;
  VarItem* item=0;
  int cc = 0;
  for (chData = (CHE*)data.first; chData; chData = (CHE*)chData->successor) {
    if (item)
      item = new VarItem(this, item, (ObjItemData*)chData->data);
    else
      item = new VarItem(this, (ObjItemData*)chData->data);
    item->itemCount = cc;
    cc++;
  }
  childrenDrawn = true;
}

void VarItem::paintCell( QPainter * p, const QColorGroup & cg,
			       int column, int width, int align )
{
  if (!column && isPriv) {
    QColorGroup cgN ( cg);
    cgN.setColor(QColorGroup::Text,red);
    QListViewItem::paintCell( p, cgN, column, width, align );
  }
  else
    QListViewItem::paintCell( p, cg, column, width, align );
}


void VarItem::setOpen(bool O)
{
  DebugMessage* mess;
  ChObjRq *rqs;

  if (O && hasChildren) {
    if (!childrenDrawn) {
      rqs = new ChObjRq;
      makeNrs(rqs);
      myView->itemToOpen = this;
      mess = new DebugMessage(Dbg_MemberDataRq,rqs);
      QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
      return;
    }
  }
  QListViewItem::setOpen(O);
}

void VarItem::makeNrs(ChObjRq *rqs)
{
  CHEint* che = new CHEint;
  che->data = itemCount;
  rqs->Prepend(che);
  if (parent())
    ((VarItem*)parent())->makeNrs(rqs);
}

VarListView::VarListView(QWidget *parent, COutputBar* bar)
:QListView(parent, "VarListView")
{
  QString label;

  myBar = bar;
  itemToOpen = 0;
  setRootIsDecorated(true);
  setFocusPolicy(QWidget::StrongFocus);
  setSorting(-1);
  addColumn("varName (static type [/ runtime type])");
  addColumn("Address");
  addColumn("Value");
  //setRootIsDecorated(true);
  header()->show();
  setSelectionMode(QListView::Single);
}

void VarListView::makeItems(DebugStopData* data)
{
  CHE* chData;
  VarItem* item=0;
  int cc=0;
  while (firstChild())
    delete firstChild();
  for (chData = (CHE*)data->ObjectChain.first; chData; chData = (CHE*)chData->successor) {
    if (item)
      item = new VarItem(this, item, (ObjItemData*)chData->data);
    else
      item = new VarItem(this, (ObjItemData*)chData->data);
    item->itemCount = cc;
    cc++;
  }
  myBar->SetTab(tabDebug);
}


StackListView::StackListView(QWidget *parent, COutputBar* bar)
:QListView(parent, "StackListView")
{
  QString label, emptyString;

  myBar = bar;
  lastSelected = 0;
  addColumn(emptyString);
  setRootIsDecorated(false);
  header()->hide();
  setFocusPolicy(QWidget::StrongFocus);
  setSorting(-1);
  setSelectionMode(QListView::Single);
  allDrawn = false;
  connect(this,SIGNAL(selectionChanged()), SLOT(selChanged()));
}

void StackListView::selChanged()
{
  lastSelected = (CTreeItem*)currentItem();
  if (allDrawn) {
    DebugMessage* mess = new DebugMessage(Dbg_StackRq,0);
    mess->CallStackLevel = lastSelected->itemCount;
    QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
  }
}

void StackListView::makeItems(DebugStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData* chData;
  CTreeItem *item=0, *lastItem=0;
  QString label;
  LavaDECL *funcDecl;
  int ii = 0;

  allDrawn = false;
  while (firstChild())
    delete firstChild();
  for (chData = (CHEStackData*)data->StackChain.first; 
       chData; chData = (CHEStackData*)chData->successor) {
    funcDecl = doc->IDTable.GetDECL(chData->data.FuncID);
    label = QString(funcDecl->FullName.c) + " (";
    switch (funcDecl->DeclType) {
    case Function:
      switch (chData->data.ExecType) {
      case ExecDef:
        label = label + "function)";
        break;
      case Require:
        label = label + "require)";
        break;
      case Ensure:
        label = label + "ensure)";
        break;
      default:;
      }
      break;
    case Interface:
    case Impl:
      label = label + "invariant)";
      break;
    case Initiator:
      label = label + "initiator)";
      break;
    default:;
    }
    item = new CTreeItem(label, this, item);
    item->itemCount = ii;
    if (ii == data->ActStackLevel) 
      lastSelected = item;
    ii++;


  }
  lastSelected->setSelected(true);
  allDrawn = true;
}
