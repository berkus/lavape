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
: QTabWidget(parent, "BuilBar")
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
  QIconSet icoFind = QIconSet(QPixmap((const char**)PX_findtab));
  QIconSet icoErr = QIconSet(QPixmap((const char**)PX_errtab));
  QIconSet icoCom = QIconSet(QPixmap((const char**)PX_commentt));
  setTabPosition(Bottom);
//  addTab (CommentPage, icoCom, "Comment" ); 
  addTab (CommentPage, icoCom, "Comment" ); 
  addTab (ErrorPage, icoErr, "Error" ); 
  addTab (FindPage, icoFind, "Find" ); 
  QSize sz = tabBar()->size();
  tabBar()->resize(sz.width(), 16);
  ActTab = tabComment;
  setCurrentPage((int)ActTab);
  connect(this,SIGNAL(currentChanged(QWidget*)), SLOT(OnTabChange(QWidget*)));
  connect(FindPage,SIGNAL(doubleClicked(QListViewItem*)), SLOT(OnDblclk(QListViewItem*)));
}

COutputBar::~COutputBar()
{
}


void COutputBar::ResetError()
{
  ErrorPage->clear();
}

void COutputBar::SetErrorOnBar(LavaDECL* decl)
{
  QString cstrA;
  setError(decl->DECLError1, &cstrA);
  setError(decl->DECLError2, &cstrA);
  ErrorPage->setText(cstrA);
}

void COutputBar::SetErrorOnBar(const CHAINX& ErrChain)
{
  QString cstrA;
  setError(ErrChain, &cstrA);
  ErrorPage->setText(cstrA);
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
        execDecl = doc->GetConstrDECL(decl, type, false,false);
        if (execDecl) {
          sData.synObjectID = data->refCase;
          doc->OpenCView(execDecl);
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
  /*
  pTab->SetCurSel((int) tab);
  ChangeTab(tab);
  */
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


