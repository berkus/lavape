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


#include "LavaPEStringInit.h"
#include "LavaPE.h"
#include "TreeView.h"
#include "ExecView.h"
#include "LavaPE_all.h"
#include "Bars.h"
//#include "ExecView.h"
#include "LavaPEDoc.h"
#include "LavaPEFrames.h"
#include "Boxes.h"
#include "qtabbar.h"
#include "qstatusbar.h"
//#include "q3header.h"
#include "qmessagebox.h"
//Added by qt3to4:
#include <QPixmap>
//#include <Q3ValueList>
#include <QHeaderView>

#pragma hdrstop


CUtilityView::CUtilityView(QWidget *parent)
: QTabWidget(parent)
{
  setObjectName("UtilityView");
	QString emptyString;
  IdlMsg = "Ready";
	
  FindPage = new QTreeWidget(0);
  FindPage->setColumnCount(1);
  FindPage->header()->hide();
  FindPage->setRootIsDecorated(false);
  FindPage->setSelectionMode(QAbstractItemView::SingleSelection);//Extended); 
  HandlerPage = new QTreeWidget(0);
  HandlerPage->setColumnCount(3);
  HandlerPage->setRootIsDecorated(false);
  HandlerPage->setSelectionMode(QAbstractItemView::SingleSelection);
  CTreeItem* item = new CTreeItem(QString("Handler"), 0);
  item->setText(1, "Event type");
  item->setText(2, "Client");
  HandlerPage->setHeaderItem(item);
  CommentPage = new QTextEdit(0);
  CommentPage->setReadOnly(true);
  ErrorPage = new QTreeWidget(0);
  HandlerPage->setColumnCount(1);
  ErrorPage->setRootIsDecorated(false);
  ErrorPage->setSelectionMode(QAbstractItemView::SingleSelection);
  ErrorPage->header()->hide();
  singleError = true;
  //ErrorPage = new QTextEdit(0);
  //ErrorPage->setReadOnly(true);
  DebugPage = new QFrame(0);
  splitter = new QSplitter(DebugPage);
  splitter->setOrientation(Qt::Horizontal);
  splitter->setHandleWidth(3);
  QVBoxLayout* layout = new QVBoxLayout(DebugPage);
  DebugPage->setLayout(layout);
  DebugPage->installEventFilter(this);
  firstTime = true;
  layout->addWidget(splitter);
  layout->setMargin(0);
  VarView = new VarListView(0, this, false);
  ParamView = new VarListView(0, this, true);
  StackView = new StackListView(0, this);
  splitter->addWidget(VarView);
  splitter->addWidget(ParamView);
  splitter->addWidget(StackView);

  QIcon icoFind = QIcon(QPixmap((const char**)PX_findtab));
  QIcon icoErr = QIcon(QPixmap((const char**)PX_errtab));
  QIcon icoCom = QIcon(QPixmap((const char**)PX_commentt));
  QIcon icoDebug = QIcon(QPixmap((const char**)PX_debugTab));
  QIcon icoHandler = QIcon(QPixmap((const char**)PX_functab));
  setTabPosition(QTabWidget::South);
  addTab (CommentPage, icoCom, "Comment" ); 
  addTab (ErrorPage, icoErr, "Error" ); 
  addTab (FindPage, icoFind, "Find" ); 
  addTab (HandlerPage, icoHandler, "Handler" ); 
  addTab (DebugPage, icoDebug, "Debug" ); 
  QSize sz = tabBar()->size();
  tabBar()->resize(sz.width(), 16);
  ActTab = tabComment;
  setCurrentIndex((int)ActTab);
  connect(this,SIGNAL(currentChanged(int)), SLOT(OnTabChange(int)));
  connect(FindPage,SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int  )), SLOT(OnDblclk(QTreeWidgetItem*, int)));
  connect(HandlerPage,SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int  )), SLOT(OnDblclk(QTreeWidgetItem*, int)));
  connect(ErrorPage,SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int  )), SLOT(OnDblclk(QTreeWidgetItem*, int)));
  ErrorEmpty = true;
  CommentEmpty = true;
  firstDebug = true;
  stopDoc = 0;
  itemToOpen = 0;
}

CUtilityView::~CUtilityView()
{
  DeleteAllPageItems(tabFind);
  DeleteAllPageItems(tabHandler);
}


void CUtilityView::ResetError()
{
  ErrorPage->clear();
  ErrorEmpty = true;
}

void CUtilityView::SetErrorOnUtil(LavaDECL* decl)
{
  QString cstrA;
  ErrorPage->clear();
  singleError = true;
  setError(decl->DECLError1, &cstrA);
  setError(decl->DECLError2, &cstrA);
  ErrorEmpty = (cstrA == QString::null) || !cstrA.length();
  if (!ErrorEmpty)
    CTreeItem *item = new CTreeItem(cstrA, ErrorPage);
  //ErrorPage->setPlainText(cstrA);
}

void CUtilityView::SetErrorOnUtil(const CHAINX& ErrChain)
{
  QString cstrA;
  ErrorPage->clear();
  singleError = true;
  setError(ErrChain, &cstrA);
  //ErrorPage->setPlainText(cstrA);
  ErrorEmpty = (cstrA == QString::null) || !cstrA.length();
  //if (ErrorEmpty)
    ;//((CLavaMainFrame*)wxTheApp->m_appWindow)->on_showUtilWindowAction_triggered();
  //else
    CTreeItem *item = new CTreeItem(cstrA, ErrorPage);
}

void CUtilityView::setError(const CHAINX& ErrChain, QString* cstrA)
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
      wxTheApp->m_appWindow->statusBar()->showMessage(cstr);
    *cstrA += cstr;
    che = (CHE*)che->successor;
  }
}

void CUtilityView::setErrorFile(QString& file)
{
  if (singleError) {
    singleError = false;
    ErrorPage->clear();
    SetTab (tabError);
  }
  CTreeItem *item = new CTreeItem(file, ErrorPage);
}


bool CUtilityView::eventFilter(QObject *watched,QEvent *ev) {
  if (watched == DebugPage && ev->type() == QEvent::Paint && firstTime) {
    QList<int> list=splitter->sizes();
    int totalW = 0;
    for (int i=0; i<3; i++)
      totalW += list.at(i);
    list.replace(0,totalW/5 * 2);
    list.replace(1,totalW/5 * 2);
    list.replace(2,totalW/5);
    splitter->setSizes(list);
    firstTime = false;
  }
  return false;
}

void CUtilityView::SetComment(const DString& text, bool toStatebar)
{
  if (text.l) {
    CommentPage->setPlainText(text.c);
    if (toStatebar)
      wxTheApp->m_appWindow->statusBar()->showMessage(text.c);
  }
  else {
    CommentPage->clear();
    if (toStatebar)
      wxTheApp->m_appWindow->statusBar()->showMessage(IdlMsg);
  }
  CommentEmpty = !text.l;
}

void CUtilityView::SetFindText(const DString& text, CFindData* data)
{
  CTreeItem *item = new CTreeItem(text.c, FindPage);
  item->setItemData((TItemData*)data);
}

void CUtilityView::AddHandler(LavaDECL* func, CLavaBaseDoc* doc)
{
  CTreeItem *item;
  LavaDECL *decl;
  CHETIDs *cheTIDs;
  QString label1, label2;
  CHETID *cheS;
  CFindData *data;//, *dataCont;
  item = new CTreeItem(func->FullName.c, HandlerPage);
  data = new CFindData();
  data->refCase = 4;
  data->nID = func->OwnID;
  data->refTid.nID = data->nID;
  data->refTid.nINCL = 0;
  data->fname = doc->IDTable.IDTab[func->inINCL]->FileName;
  AbsPathName(data->fname, doc->IDTable.DocDir);
  item->setItemData((TItemData*)data);
  if (func->GUISignaltype == Ev_ValueChanged)
    label1 = QString("New Value");
  else if (func->GUISignaltype == Ev_ChainInsert)
    label1 = QString("Insert chain element");
  else if (func->GUISignaltype == Ev_ChainDelete)
    label1 = QString("Delete chain element");
  else if (func->GUISignaltype == Ev_OptInsert)
    label1 = QString("Insert optional element");
  else if (func->GUISignaltype == Ev_OptDelete)
    label1 = QString("Delete optional element");
  item->setText(1, label1);

  cheTIDs = (CHETIDs*)func->HandlerClients.first;
  if (cheTIDs) {
    label2.clear(); 
    while (cheTIDs) {
      cheS = (CHETID*)cheTIDs->data.first;
      label2 += QString(func->ParentDECL->FullName.c);
      if (cheS) {
        label2 += "::";
        while (cheS) {
          decl = doc->IDTable.GetDECL(cheS->data);
          if (decl)
            label2 += QString(decl->LocalName.c);
          else { /*error, messagebox */}
          cheS = (CHETID*)cheS->successor;
          if (cheS)
            label2 += QString(".");
        }
      }
      else
        label2 += QString(" (Attached to the form) ");
      cheTIDs = (CHETIDs*)cheTIDs->successor;
      if (cheTIDs) {
        //item = new CTreeItem(func->FullName.c, HandlerPage);
        //item->setText(1, label1);
        //dataCont = new CFindData();
        //dataCont = data;
        //item->setItemData((TItemData*)dataCont);
        label2 += QString(", ");
      }
    }//while cheTIDs
  }//if cheTIDs
  else
    label2 = "no client attached";
  item->setText(2, label2);
  HandlerPage->resizeColumnToContents(0);
  HandlerPage->resizeColumnToContents(1);
  HandlerPage->resizeColumnToContents(2);
  if (HandlerPage->columnWidth(0) < 30)
    HandlerPage->setColumnWidth(0, 30);
  if (HandlerPage->columnWidth(1) < 30)
    HandlerPage->setColumnWidth(1, 30);
  if (HandlerPage->columnWidth(2) < 30)
    HandlerPage->setColumnWidth(2, 30);
  SetTab(tabHandler);
}


void CUtilityView::SetHandler(HandlerInfos* handlerInfos, CLavaBaseDoc* doc)
{
  CTreeItem *item;
  LavaDECL *func, *decl;
  CHETIDs *cheTIDs;
  CHETID *cheS;
  QString label1, label2;
  CHEHandlerInfo *cheHand = (CHEHandlerInfo*)handlerInfos->first;
  CFindData *data;//, *dataCont;

  DeleteAllPageItems(tabHandler);
  while (cheHand) {
    func = doc->IDTable.GetDECL(cheHand->data.HandlerID);
    item = new CTreeItem(func->FullName.c, HandlerPage);
    data = new CFindData();
    data->refCase = 4;
    data->nID = func->OwnID;
    data->refTid.nID = data->nID;
    data->refTid.nINCL = 0;
    data->fname = doc->IDTable.IDTab[func->inINCL]->FileName;
    AbsPathName(data->fname, doc->IDTable.DocDir);
    item->setItemData((TItemData*)data);
    if (func->GUISignaltype == Ev_ValueChanged)
      label1 = QString("New Value");
    else if (func->GUISignaltype == Ev_ChainInsert)
      label1 = QString("Insert chain element");
    else if (func->GUISignaltype == Ev_ChainDelete)
      label1 = QString("Delete chain element");
    else if (func->GUISignaltype == Ev_OptInsert)
      label1 = QString("Insert optional element");
    else if (func->GUISignaltype == Ev_OptDelete)
      label1 = QString("Delete optional element");
    item->setText(1, label1);

    cheTIDs = (CHETIDs*)func->HandlerClients.first;
    label2.clear(); 
    if (cheTIDs) {
      while (cheTIDs) {
        cheS = (CHETID*)cheTIDs->data.first;
        label2 += QString(func->ParentDECL->FullName.c);
        if (cheS) {
          label2 += "::";
          while (cheS) {
            decl = doc->IDTable.GetDECL(cheS->data);
            if (decl)
              label2 += QString(decl->LocalName.c);
            else { /*error, messagebox */}
            cheS = (CHETID*)cheS->successor;
            if (cheS)
              label2 += QString(".");
          }//while cheS
        }//if cheS
        else
          label2 += QString(" (Attached to the form)");
        cheTIDs = (CHETIDs*)cheTIDs->successor;
        if (cheTIDs) {
          //item = new CTreeItem(func->FullName.c, HandlerPage);
          //item->setText(1, label1);
          //dataCont = new CFindData();
          //dataCont = data;
          //item->setItemData((TItemData*)dataCont);
          label2 += QString(", ");
        }
      }//while cheTIDs
    }//if cheTIDs
    else
      label2 = "no client attached";
    item->setText(2, label2);
    cheHand = (CHEHandlerInfo*)cheHand->successor;
  }

  if (handlerInfos->first) {
    HandlerPage->resizeColumnToContents(0);
    HandlerPage->resizeColumnToContents(1);
    HandlerPage->resizeColumnToContents(2);
    if (HandlerPage->columnWidth(0) < 30)
      HandlerPage->setColumnWidth(0, 30);
    if (HandlerPage->columnWidth(1) < 30)
      HandlerPage->setColumnWidth(1, 30);
    if (HandlerPage->columnWidth(2) < 30)
      HandlerPage->setColumnWidth(2, 30);
    SetTab(tabHandler);
  }
  delete handlerInfos;
}

void CUtilityView::OnDblclk(QTreeWidgetItem* item, int col)
{
  CFindData *data;
  TID tid;
  CLavaPEDoc *doc;
  CSearchData sData;
  LavaDECL *decl, *execDecl;
  TDeclType type;

  if (currentWidget() == ErrorPage) {
    if (singleError)
      return;
    else {
      QString file = item->text(0);
      wxDocManager::GetDocumentManager()->FindOpenDocument(file);
      return;
    }
  }
  data = (CFindData*)((CTreeItem*)item)->getItemData();
  doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(data->fname.c);
  if (doc && data) {
    decl = doc->IDTable.GetDECL(0, data->nID);
    if (decl) {
      if (!data->index) {
        if (doc->TrueReference(decl, data->refCase, data->refTid))
          ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, decl, tid, true, &data->enumID);
        else
          QMessageBox::critical(this, qApp->applicationName(), IDP_RefNotFound);
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
          // sData.finished = false;
          ((SynObjectBase*)execDecl->Exec.ptr)->MakeTable((address)&doc->IDTable, 0, (SynObjectBase*)execDecl, onSelect, 0,0, (address)&sData);
          ((CExecView*)sData.execView)->RedrawExec();
        }
      }
    }
  }
}


void CUtilityView::DeleteAllPageItems(UtilityTabs tab)
{
  CTreeItem* item;
  CFindData * itd;
  QTreeWidget* page = 0;
  int ii=0;
  if (tab == tabFind)
    page = FindPage;
  else if (tab == tabHandler)
    page = HandlerPage;
  if (page) {
    for (item = (CTreeItem*)page->topLevelItem(ii);
         item;
         item = (CTreeItem*)item->nextSibling()) {
      ii++;
      itd = (CFindData*)item->getItemData();
      if (itd)
        delete itd;
    }
    page->clear();
  }
}

void CUtilityView::SetTab(UtilityTabs tab)
{

  setCurrentIndex((int)tab);
  if ((tab == tabDebug) && firstDebug) {
    QSize sz = ((CLavaMainFrame*)wxTheApp->m_appWindow)->size();
    int h = sz.height()/3;
    resize(sz.width(), h);
    firstDebug = false;
  }
  if (((CLavaMainFrame*)wxTheApp->m_appWindow)->UtilitiesHidden) {
    show();
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastUtilitiesState = -1;
  }
  else
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastUtilitiesState = (int)ActTab;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->UtilitiesHidden = false;
}


void CUtilityView::OnTabChange(int)
{
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->LastUtilitiesState = (int)ActTab;
  ActTab = (UtilityTabs)currentIndex();
  if (ActTab == tabFind) {
    FindPage->show();
    setUpdatesEnabled(true);
  }  
  if (ActTab == tabHandler) {
    HandlerPage->show();
    setUpdatesEnabled(true);
  }
}

void CUtilityView::setDebugData(DbgMessages* dbgReceived, CLavaBaseDoc* doc)
{
  CLavaBaseView *view=0;
  LavaDECL* decl;
  TID id;
  DString *s0=0;
  DbgStopData* dbgStData;
  bool ok;

  if (dbgReceived && dbgReceived->newReceived) {
    switch (dbgReceived->newReceived->Command) {
    case Dbg_MemberData:
      if (itemToOpen) {
        itemToOpen->makeChildren(((DDItemData*)dbgReceived->newReceived->ObjData.ptr)->Children);
        itemToOpen->setOpen(true);
        itemToOpen->setSelected(true);
        itemToOpen->myView->setCurrentItem(itemToOpen);
        itemToOpen = 0;
      }
      break;
    case Dbg_StopData:
      dbgStData = (DbgStopData*)dbgReceived->newReceived->DbgData.ptr;
      if (dbgStData->StackChain.first) {
        StackView->makeItems(dbgStData, doc);
        if (dbgReceived->lastReceived)
          removeExecStackPos((DbgStopData*)dbgReceived->lastReceived->DbgData.ptr, doc);
        showExecStackPos((DbgStopData*)dbgReceived->newReceived->DbgData.ptr, doc);
        VarView->makeItems(((DbgStopData*)dbgReceived->newReceived->DbgData.ptr)->ObjectChain);
        ParamView->makeItems(((DbgStopData*)dbgReceived->newReceived->DbgData.ptr)->ParamChain);
      }
      if (dbgStData->SynErrData.ptr) {
        id = dbgStData->SynErrData.ptr->ErrID;
        decl = ((TIDTable*)doc->mySynDef->IDTable)->GetDECL(id);
        if (dbgStData->SynErrData.ptr->SynObjID > -1)
          ok = (decl!=0) && ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, decl, id, false, s0, true, dbgStData->SynErrData.ptr->ExecType);
        else
          ok = (decl!=0) && ((CLavaPEApp*)wxTheApp)->Browser.GotoDECL(doc, decl, id, false, s0);
        if (!ok) {/*??*/}
      }
      if (!dbgStData->StackChain.first)
        return;
      break;
    case Dbg_Stack: 
      showExecStackPos((DbgStopData*)dbgReceived->newReceived->DbgData.ptr, doc);
      VarView->makeItems(((DbgStopData*)dbgReceived->newReceived->DbgData.ptr)->ObjectChain);
      ParamView->makeItems(((DbgStopData*)dbgReceived->newReceived->DbgData.ptr)->ParamChain);
      //VarView->update();
      //ParamView->update();
      break;
    default:;
    }
    VarView->resizeColumnToContents(0);
    VarView->resizeColumnToContents(1);
    ParamView->resizeColumnToContents(0);
    ParamView->resizeColumnToContents(1);
    SetTab(tabDebug);
  }
  else { //reset
    if (doc && dbgReceived && dbgReceived->lastReceived) {
      removeExecStackPos((DbgStopData*)dbgReceived->lastReceived->DbgData.ptr, doc);
      delete dbgReceived->lastReceived;
      dbgReceived->lastReceived = 0;
    }
    StackView->allDrawn = false;
    while (VarView->topLevelItem(0)) //firstChild())
      delete VarView->takeTopLevelItem(0);
    while (ParamView->topLevelItem(0))
      delete ParamView->takeTopLevelItem(0);
    while (StackView->topLevelItem(0))
      delete StackView->takeTopLevelItem(0);
  }

}

void CUtilityView::removeExecStackPos(DbgStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData *cheData;
  LavaDECL *funcDecl;
  TID tid;
  int pos;
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
      //pos = wxTheApp->m_docManager->GetFirstDocPos();
      //while (pos) {
      for (pos = 0; pos < wxTheApp->m_docManager->m_docs.size(); pos++) {
        stopDoc = (CLavaPEDoc*)wxTheApp->m_docManager->m_docs[pos];
        if (stopDoc && (qfn == stopDoc->GetFilename()))
          pos = wxTheApp->m_docManager->m_docs.size();
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
        view = 0;
        for (pos = 0; pos < stopDoc->m_documentViews.size(); pos++) {
          view = (CLavaBaseView*)stopDoc->m_documentViews[pos];
          if (view->inherits("CExecView") && (((CExecView*)view)->myDECL == stopExecDECL))
            pos = stopDoc->m_documentViews.size();
          else
            view = 0;
        }
        if (view && (((CExecView*)view)->sv->execCont->debugStopToken || ((CExecView*)view)->sv->execCont->callerStopToken) ) {
          ((CExecView*)view)->sv->execCont->debugStopToken = 0;
          ((CExecView*)view)->sv->execCont->callerStopToken = 0;
          ((CExecView*)view)->redCtl->update();
        }
      }
    }
    cheData = (CHEStackData*)cheData->successor;
  }//while
}

void CUtilityView::showExecStackPos(DbgStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData *cheData, *cheDataAct = (CHEStackData*)data->StackChain.GetNth(data->ActStackLevel+1);
  LavaDECL *funcDecl;
  CSearchData sData;
  TID tid;
  int pos;
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
          //pos = wxTheApp->m_docManager->GetFirstDocPos();
          //while (pos) {
          for (pos = 0; pos < wxTheApp->m_docManager->m_docs.size(); pos++) {
            stopDoc = (CLavaPEDoc*)wxTheApp->m_docManager->m_docs[pos];
            if (stopDoc && (qfn == stopDoc->GetFilename()))
              pos = wxTheApp->m_docManager->m_docs.size();
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
            for (pos = 0; pos < stopDoc->m_documentViews.size(); pos++) {
              view = (CLavaBaseView*)stopDoc->m_documentViews[pos];
              if (view->inherits("CExecView") && (((CExecView*)view)->myDECL == stopExecDECL)) {
                sData.execView = view;
                pos = stopDoc->m_documentViews.size();
              }
            }
          }
          else {
            ((CLavaPEDoc*)stopDoc)->OpenExecView(stopExecDECL);
            sData.execView = wxDocManager::GetDocumentManager()->GetActiveView();
          }
          if (sData.execView) {
            if (!cheDataAct || (cheData == data->StackChain.first)) {
              sData.debugStop = true;
              sData.stopReason = data->stopReason;
              if (cheData == data->StackChain.first)
                sData.innermostStop = true;
              else
                sData.innermostStop = false;
              // sData.finished = false;
              ((SynObjectBase*)stopExecDECL->Exec.ptr)->MakeTable((address)&stopDoc->IDTable, 0, (SynObjectBase*)stopExecDECL, onSelect, 0,0, (address)&sData);
              ((CExecView*)sData.execView)->redCtl->update();
              ((CExecView*)sData.execView)->redCtl->innermostStop = sData.innermostStop;
            }
            else 
              if (((CExecView*)sData.execView)->redCtl->callerStopToken) {
                ((CExecView*)sData.execView)->redCtl->callerStopToken = 0;
                ((CExecView*)sData.execView)->redCtl->update();
              }
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
  :QTreeWidgetItem(parent, afterItem) 
{
  myView = view;
//  setMultiLinesEnabled(true);
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  if (isPriv)
    setTextColor(0, QColor(Qt::red));
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarItem* parent, DDItemData* data, VarListView* view)
  :QTreeWidgetItem(parent) 
{
  myView = view;
//  setMultiLinesEnabled(true);
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  if (isPriv)
    setTextColor(0, QColor(Qt::red));
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, VarItem* afterItem, DDItemData* data)
  :QTreeWidgetItem(parent, afterItem) 
{
//  setMultiLinesEnabled(true);
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  isPriv = data->isPrivate;
  if (isPriv)
    setTextColor(0, QColor(Qt::red));
  myView = parent;
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}

VarItem::VarItem(VarListView* parent, DDItemData* data)
  :QTreeWidgetItem(parent) 
{
//  setMultiLinesEnabled(true);
  setText(0, data->Column0.c);
  setText(1, data->Column1.c);
  setText(2, data->Column2.c);
  myView = parent;
  isPriv = data->isPrivate;
  if (isPriv)
    setTextColor(0, QColor(Qt::red));
  childrenDrawn = false;
  hasChildren = data->HasChildren;
  setExpandable(hasChildren);
  //setHeight(16);
  if (data->Children.first)
    makeChildren(data->Children);
}


void VarItem::setExpandable(bool withChildren)
{
  if (withChildren && !childrenDrawn) {
    QTreeWidgetItem* it = new QTreeWidgetItem(this);
  }
}

void VarItem::makeChildren(const CHAINX& data)
{
  CHE* chData;
  VarItem* item=0;
  int cc = 0;
  if (!childrenDrawn) {
    QTreeWidgetItem* it = takeChild(0);
    if (it) 
      delete it;
  }
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

/*
void VarItem::paintCell( QPainter * p, const QColorGroup & cg,
			       int column, int width, int align )
{
  if (!column && isPriv) {
    QColorGroup cgN ( cg);
    cgN.setColor(QColorGroup::Text,Qt::red);
    QTreeWidgetItem::paintCell( p, cgN, column, width, align );
  }
  else
    QTreeWidgetItem::paintCell( p, cg, column, width, align );
    
}*/


void VarItem::setOpen(bool O)
{
  DbgMessage* mess;
  ChObjRq *rqs;

  if (O && hasChildren) {
    if (!childrenDrawn) {
      rqs = new ChObjRq;
      makeNrs(rqs);
      myView->myUtilityView->itemToOpen = this;
      mess = new DbgMessage(Dbg_MemberDataRq);
      mess->ObjNr.ptr = rqs;
      mess->fromParams = myView->myUtilityView->ParamView == myView;
      QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
      return;
    }
  }
  if (O)
    myView->expandItem(this);
  else
    myView->collapseItem(this);
    //QTreeWidgetItem::setOpen(O);
}

void VarItem::makeNrs(ChObjRq *rqs)
{
  CHEint* che = new CHEint;
  che->data = itemCount;
  rqs->Prepend(che);
  if (parent())
    ((VarItem*)parent())->makeNrs(rqs);
}

VarListView::VarListView(QWidget *parent, CUtilityView* bar, bool forParams)
:QTreeWidget(parent)
{
  QString label;

  myUtilityView = bar;
//  itemToOpen = 0;
  setRootIsDecorated(true);
  setFocusPolicy(Qt::StrongFocus);
//  setSorting(-1);
  setColumnCount(3);
  connect(this,SIGNAL(itemExpanded (QTreeWidgetItem*)), this, SLOT(expandedItem(QTreeWidgetItem*)));
 if (forParams)
    setHeaderLabels(QStringList() << "vfunc.params. (type[/RT type])" << "address" << "value");
 else
    setHeaderLabels(QStringList() << "variables (type[/RT type])" << "address" << "value");
 resizeColumnToContents(0);
 resizeColumnToContents(1);
  header()->show();
//  setShowToolTips(true);
  setSelectionMode(QAbstractItemView::SingleSelection);
}

void VarListView::makeItems(const CHAINX& objChain) //DbgStopData* data)
{
  CHE* chData;
  VarItem* item=0;
  int cc=0;
  while (topLevelItem(0))
    delete takeTopLevelItem(0);
  /*
  setColumnWidth(0, width0);
  setColumnWidth(1, width1);
  setColumnWidth(2, width2);
  */
  for (chData = (CHE*)objChain.first; chData; chData = (CHE*)chData->successor) {
    if (item) 
      item = new VarItem(this, item, (DDItemData*)chData->data);
    else
      item = new VarItem(this, (DDItemData*)chData->data);
    item->itemCount = cc;
    cc++;
  }
  myUtilityView->SetTab(tabDebug);
}

void VarListView::expandedItem(QTreeWidgetItem* item)
{
  ((VarItem*)item)->setOpen(true);
}


StackListView::StackListView(QWidget *parent, CUtilityView* bar)
:QTreeWidget(parent)
{
  QString label, emptyString;

  myUtilityView = bar;
  lastSelected = 0;
//  addColumn(emptyString);
  setColumnCount(1);
  setRootIsDecorated(false);
  header()->hide();
  setFocusPolicy(Qt::StrongFocus);
//  setSorting(-1);
//  setShowToolTips(true);
  setSelectionMode(QAbstractItemView::SingleSelection);
  allDrawn = false;
  connect(this,SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(itemClicked(QTreeWidgetItem*, int)));
  connect(this,SIGNAL(itemSelectionChanged()), SLOT(selChanged()));
}

void StackListView::itemClicked(QTreeWidgetItem *item, int)
{
  selChanged();
}

void StackListView::selChanged()
{
  lastSelected = (CTreeItem*)currentItem();
  if (!lastSelected)// || lastSelected->isDisabled())
    return;
  if (allDrawn) {
    DbgMessage* mess = new DbgMessage(Dbg_StackRq);
    mess->CallStackLevel = lastSelected->itemCount;
    QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
  }
}

void StackListView::makeItems(DbgStopData* data, CLavaBaseDoc* doc)
{
  CHEStackData* chData;
  CTreeItem *item=0, *lastItem=0;
  QString label;
  LavaDECL *funcDecl;
  int ii = 0;
  bool disableIt;

  allDrawn = false;
  while (topLevelItem(0))
    delete takeTopLevelItem(0);
  for (chData = (CHEStackData*)data->StackChain.first; 
       chData; chData = (CHEStackData*)chData->successor) {
    disableIt = false;
    funcDecl = doc->IDTable.GetDECL(chData->data.FuncID);
    if (funcDecl) {
      label = QString(funcDecl->FullName.c) + " (";
      switch (funcDecl->DeclType) {
      case Function:
        if (funcDecl->TypeFlags.Contains(isNative)) {
          label = label + "native function)";
          disableIt = true;
        }
        else
          switch (chData->data.ExecType) {
          case ExecDef:
            label = label + "function)";
            break;
          case Require:
            if (funcDecl->ParentDECL->DeclType == Interface)
              label = label + "require)";
            else
              label = label + "impl. require)";
            break;
          case Ensure:
            if (funcDecl->ParentDECL->DeclType == Interface)
              label = label + "ensure)";
            else
              label = label + "impl. ensure)";
            break;
          default:;
          }
        break;
      case Interface:
        label = label + "invariant)";
        break;
      case Impl:
        label = label + "impl. invariant)";
        break;
      case Initiator:
        label = label + "main program)";
        break;
      default:;
      }
      item = new CTreeItem(label, this, item);
      item->itemCount = ii;
      if (disableIt)
        item->setFlags(0);
      if (ii == data->ActStackLevel) 
        lastSelected = item;
      ii++;
    }


  }
  lastSelected->setSelected(true);
  setCurrentItem(lastSelected);
  allDrawn = true;
}

