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
#include "ExecView.h"
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
  firstDebug = true;
  stopDoc = 0;
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
  if ((tab == tabDebug) && firstDebug) {
    QSize sz = ((CLavaMainFrame*)wxTheApp->m_appWindow)->size();
    int h = sz.height()/3;
    resize(sz.width(), h);
    firstDebug = false;
  }
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

void COutputBar::setDebugData(DbgMessages* dbgReceived, CLavaBaseDoc* doc)
{
  CLavaBaseView *view=0;

  if (dbgReceived && dbgReceived->newReceived) {
    switch (dbgReceived->newReceived->Command) {
    case Dbg_MemberData:
      if (VarView->itemToOpen) {
        VarView->itemToOpen->makeChildren(((DDItemData*)dbgReceived->newReceived->ObjData.ptr)->Children);
        VarView->itemToOpen->setOpen(true);
        VarView->setSelected(VarView->itemToOpen, true);
        VarView->itemToOpen = 0;
      }
      break;
    case Dbg_StopData: 
      StackView->makeItems((DbgStopData*)dbgReceived->newReceived->DbgData.ptr, doc);
      if (dbgReceived->lastReceived)
        removeExecStackPos((DbgStopData*)dbgReceived->lastReceived->DbgData.ptr, doc);
    case Dbg_Stack: 
      showExecStackPos((DbgStopData*)dbgReceived->newReceived->DbgData.ptr, doc);
      VarView->makeItems((DbgStopData*)dbgReceived->newReceived->DbgData.ptr);
      break;
    default:;
    }
    SetTab(tabDebug);
  }
  else { //reset
    if (stopDoc) {
      POSITION pos = stopDoc->GetFirstViewPos();
      while (pos) {
        view = (CLavaBaseView*)stopDoc->GetNextView(pos);
        if (view->inherits("CExecView")/* && (((CExecView*)view)->myDECL == stopExecDECL)*/) {
          ((CExecView*)view)->sv->debugStopToken = 0;
          ((CExecView*)view)->sv->viewport()->repaint();
          pos = 0;
        }
      }
    }
    while (VarView->firstChild())
      delete VarView->firstChild();
    while (StackView->firstChild())
      delete StackView->firstChild();
    ((CMainFrame*)wxTheApp->m_appWindow)->DbgStepNextAct->setEnabled(false);
    ((CMainFrame*)wxTheApp->m_appWindow)->DbgStepNextFunctionAct->setEnabled(false);
    ((CMainFrame*)wxTheApp->m_appWindow)->DbgStepintoAct->setEnabled(false);
    ((CMainFrame*)wxTheApp->m_appWindow)->DbgStepoutAct->setEnabled(false);
    ((CMainFrame*)wxTheApp->m_appWindow)->DbgRunToSelAct->setEnabled(false);
  }

}

void COutputBar::removeExecStackPos(DbgStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData *cheData;
  LavaDECL *funcDecl;
  TID tid;
  POSITION pos;
  wxView *view;
  QString qfn;
  DString fn;

  if (!data)
    return;
  cheData = (CHEStackData*)data->StackChain.first;
  while (cheData)  {
    funcDecl = doc->IDTable.GetDECL(cheData->data.FuncID);
    if (funcDecl && funcDecl->inINCL) {
      fn = ((CLavaPEDoc*)doc)->IDTable.GetRelSynFileName(cheData->data.FuncID);
      AbsPathName(fn, ((CLavaPEDoc*)doc)->IDTable.DocDir);
      qfn = QString(fn.c);
      stopDoc = 0;
      pos = wxTheApp->m_docManager->GetFirstDocPos();
      while (pos) {
        stopDoc = (CLavaPEDoc*)wxTheApp->m_docManager->GetNextDoc(pos);
        if (stopDoc && (qfn == stopDoc->GetFilename()))
          pos = 0;
        else
          stopDoc = 0;
      }
      if (stopDoc)
        funcDecl = stopDoc->IDTable.GetDECL(0, cheData->data.FuncID.nID);
    }
    else
      stopDoc = doc;
    if (stopDoc) {
      stopExecDECL = stopDoc->GetExecDECL(funcDecl, cheData->data.ExecType, false,false);
      if (stopExecDECL) {
        pos = stopDoc->GetFirstViewPos();
        view = 0;
        while (pos) {
          view = (CLavaBaseView*)stopDoc->GetNextView(pos);
          if (view->inherits("CExecView") && (((CExecView*)view)->myDECL == stopExecDECL))
            pos =0;
          else
            view = 0;
        }
        if (view) {
          ((CExecView*)view)->sv->debugStopToken = 0;
          ((CExecView*)view)->sv->viewport()->repaint();
        }
      }
    }
    cheData = (CHEStackData*)cheData->successor;
  }//while
}

void COutputBar::showExecStackPos(DbgStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData *cheData, *cheDataAct = (CHEStackData*)data->StackChain.GetNth(data->ActStackLevel+1);
  LavaDECL *funcDecl;
  CSearchData sData;
  TID tid;
  POSITION pos;
  wxView *view;
  QString qfn;
  DString fn;

  cheData = (CHEStackData*)data->StackChain.first;
  while (cheData)  {
    if (cheData != cheDataAct) {
      funcDecl = doc->IDTable.GetDECL(cheData->data.FuncID);
      //((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, funcDecl, tid, true, 0, true, false);
      if (funcDecl && funcDecl->inINCL) {
        fn = ((CLavaPEDoc*)doc)->IDTable.GetRelSynFileName(cheData->data.FuncID);
        AbsPathName(fn, ((CLavaPEDoc*)doc)->IDTable.DocDir);
        qfn = QString(fn.c);
        if (cheDataAct) {
          stopDoc = 0;
          pos = wxTheApp->m_docManager->GetFirstDocPos();
          while (pos) {
            stopDoc = (CLavaPEDoc*)wxTheApp->m_docManager->GetNextDoc(pos);
            if (stopDoc && (qfn == stopDoc->GetFilename()))
              pos = 0;
            else
              stopDoc = 0;
          }
        }
        else
          stopDoc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(qfn);//and open it
        if (stopDoc)
          funcDecl = stopDoc->IDTable.GetDECL(0, cheData->data.FuncID.nID);
      }
      else
        stopDoc = doc;
      if (stopDoc) {
        stopExecDECL = stopDoc->GetExecDECL(funcDecl, cheData->data.ExecType, false,false);
        if (stopExecDECL) {
          sData.synObjectID = cheData->data.SynObjID;
          if (cheDataAct) { //= not the actual stack position
            sData.execView = 0;
            pos = stopDoc->GetFirstViewPos();
            while (pos) {
              view = (CLavaBaseView*)stopDoc->GetNextView(pos);
              if (view->inherits("CExecView") && (((CExecView*)view)->myDECL == stopExecDECL)) {
                sData.execView = (CExecView*)view;
                break;
              }
            }
          }
          else {
            ((CLavaPEDoc*)stopDoc)->OpenExecView(stopExecDECL);
            sData.execView = wxDocManager::GetDocumentManager()->GetActiveView();
          }
          if (sData.execView) {
            sData.debugStop = true;
            sData.stopReason = data->stopReason;
            if (cheData == data->StackChain.first)
              sData.innermostStop = true;
            else
              sData.innermostStop = false;
            ((SynObjectBase*)stopExecDECL->Exec.ptr)->MakeTable((address)&stopDoc->IDTable, 0, (SynObjectBase*)stopExecDECL, onSelect, 0,0, (address)&sData);
      
          }
        }
      }
    }
    if (cheDataAct) {
        cheData = (CHEStackData*)cheData->successor;
      if (!cheData) {
        cheData = cheDataAct;
        cheDataAct = 0;
      }
    }
    else
      cheData = 0;
  }//while
}


VarItem::VarItem(VarItem* parent, VarItem* afterItem, DDItemData* data, VarListView* view)
  :QListViewItem(parent, afterItem) 
{
  myView = view;
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarItem* parent, DDItemData* data, VarListView* view)
  :QListViewItem(parent) 
{
  myView = view;
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, VarItem* afterItem, DDItemData* data)
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
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, DDItemData* data)
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
  //setHeight(16);
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
      item = new VarItem(this, item, (DDItemData*)chData->data, myView);
    else
      item = new VarItem(this, (DDItemData*)chData->data, myView);
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
  DbgMessage* mess;
  ChObjRq *rqs;

  if (O && hasChildren) {
    if (!childrenDrawn) {
      rqs = new ChObjRq;
      makeNrs(rqs);
      myView->itemToOpen = this;
      mess = new DbgMessage(Dbg_MemberDataRq,rqs);
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

void VarListView::makeItems(DbgStopData* data)
{
  CHE* chData;
  VarItem* item=0;
  int cc=0;
  while (firstChild())
    delete firstChild();
  for (chData = (CHE*)data->ObjectChain.first; chData; chData = (CHE*)chData->successor) {
    if (item) 
      item = new VarItem(this, item, (DDItemData*)chData->data);
    else
      item = new VarItem(this, (DDItemData*)chData->data);
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
    DbgMessage* mess = new DbgMessage(Dbg_StackRq,0);
    mess->CallStackLevel = lastSelected->itemCount;
    QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
  }
}

void StackListView::makeItems(DbgStopData* data, CLavaBaseDoc* doc)
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
    if (funcDecl) {
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


  }
  lastSelected->setSelected(true);
  allDrawn = true;
}
