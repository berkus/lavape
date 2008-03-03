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

         /* This file has been taken over from www.wxwidgets.org
   and adapted to the needs of LavaPE */


/////////////////////////////////////////////////////////////////////////////
// Name:        mdiframes.cpp
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#include "SYSTEM.h"
#include "defs.h"

#include "mdiframes.h"
#include "qstring.h"
#include "qmenubar.h"
#include "qstatusbar.h"
#include "qmessagebox.h"
#include "qfile.h"
//Added by qt3to4:
#include <QCloseEvent>
#include <QEvent>

#pragma hdrstop

/*
 * Docview MDI parent frame
 */

wxMainFrame::wxMainFrame() : QMainWindow()
{
  wxDocManager *docMan=wxDocManager::GetDocumentManager();

  m_workspace = 0;
  QStatusBar *stb=new QStatusBar(this);
  completelyCreated = false;

  setStatusBar(stb);
  wxTheApp->m_appWindow = this;

  docMan->m_fileHistory = new wxHistory(this);
  connect(docMan->m_fileHistory->m_signalMapper,SIGNAL(mapped(int)),wxTheApp->m_appWindow,SLOT(histFile(int)));
  m_childFrameHistory = new wxHistory(this);
  connect(m_childFrameHistory->m_signalMapper,SIGNAL(mapped(int)),wxTheApp->m_appWindow,SLOT(histWindow(int)));
}

bool wxMainFrame::OnCreate()
{
  m_CentralWidget = CreateWorkspace(this);
  setCentralWidget(m_CentralWidget);
  LoadFileHistory();
  completelyCreated = true;
  return true;
}

wxMainFrame::~wxMainFrame()
{
  wxTheApp->deletingMainFrame = true;
  delete m_childFrameHistory;
}

QSplitter* wxMainFrame::CreateWorkspace(QWidget* parent)
{
  QSplitter *split=new QSplitter(parent);
  m_workspace = new MyTabWidget(split);
  m_workspace->setUsesScrollButtons(true);
  QPushButton *close=new QPushButton(QPixmap(QString::fromUtf8(":/LavaPE/res/TOOLBUTTONS/LavaPE/res/TOOLBUTTONS/close.xpm")),QString());
  close->setToolTip("Close current page");
  connect(close,SIGNAL(clicked(bool)),m_workspace,SLOT(closePage(bool)));
  m_workspace->setCornerWidget(close);
  connect(m_workspace ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
  return split;
}


void wxMainFrame::windowActivated(int index)
{
  theActiveFrame = m_workspace->widget(index);
  if (theActiveFrame && theActiveFrame->inherits("wxChildFrame"))
    ((wxChildFrame*)theActiveFrame)->Activate();
}

void wxMainFrame::closeEvent (QCloseEvent*)
{
  OnFileExit();
}

void wxMainFrame::OnFileExit()
{
  ((wxApp*)qApp)->appExit = true;
  if ( wxDocManager::GetDocumentManager()->Clear(false)) {
    qApp->quit();
  }
  else
    ((wxApp*)qApp)->appExit = false;
}

void wxMainFrame::LoadFileHistory() {
  QSettings settings(QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

  wxDocManager::GetDocumentManager()->FileHistoryLoad(settings);
}

void wxMainFrame::OnMRUFile(int histFileIndex)
{
    QString *file=wxDocManager::GetDocumentManager()->GetHistoryFile(histFileIndex);

    if (file->isEmpty())
        return;
    QFile f(*file);
    if (!f.open(QIODevice::ReadOnly))
    {
        wxDocManager::GetDocumentManager()->RemoveFileFromHistory(histFileIndex);
        QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("Sorry, couldn't open this file."),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
                                delete file;
        return;
    }
    wxTheApp->SetLastFileOpen(*file);
    if (!wxDocManager::GetDocumentManager()->CreateDocument(*file, wxDOC_SILENT))
        wxDocManager::GetDocumentManager()->RemoveFileFromHistory(histFileIndex);
        delete file;
}

void wxMainFrame::OnMRUWindow(int histWindowIndex)
{
  HistWindow *hw = (HistWindow*)m_childFrameHistory->GetHistoryItem(histWindowIndex);
  hw->window->Activate(true,true);
}



void wxMainFrame::on_cascadeAction_triggered()
{
  //int ii;
  //QWidget *window;
  //QWidgetList windows = m_workspace->windowList();
  //QSize sz=m_workspace->size();

  //m_workspace->cascade();
  //wxTheApp->isChMaximized = false;
  //for (ii = 0; ii < int(windows.count()); ++ii ) {
  //  window = windows.at(ii);
  //  if (!((QMainWindow*)window)->isMinimized())
  //    if (window->inherits("wxChildFrame")) {
  //      //((wxChildFrame*)window)->oldWindowState = Qt::WindowNoState;
  //      window->resize(sz.width()*7/10, sz.height()*7/10);
  //    }
  //}
}

void wxMainFrame::TileVertic(QMenuBar *menubar, int& lastTile)
{
  //int ii, cc = 0, x = 0, minHeight=0;
  //QWidget *window;
  //QWidgetList windows = m_workspace->windowList();

  //wxTheApp->isChMaximized = false;
  //lastTile = 1;

  //if (!windows.count() )
  //  return;
  //cc = (int)windows.count();
  //for (ii = 0; ii < int(windows.count()); ++ii ) {
  //  window = windows.at(ii);
  //  if (((QMainWindow*)window)->isMinimized()) {
  //    cc--;
  //    minHeight = menubar->height();
  //  }
  //  //else
  //  //  if (window->inherits("wxChildFrame"))
  //  //    ((wxChildFrame*)window)->oldWindowState = Qt::WindowNoState;
  //}
//  if (!cc)
//    return;
////  if (cc > 3) {
//    m_workspace->tile();
//    return;
/*  }
  allHeight = m_workspace->height() - minHeight;
  widthForEach = m_workspace->width() / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredWidth = window->minimumWidth()+window->parentWidget()->baseSize().width();
      actWidth = qMax(widthForEach, preferredWidth);
      if (window == theActiveFrame)
        window->showNormal();
      window->parentWidget()->resize(actWidth, allHeight );
      window->parentWidget()->move( x, 0);
      x += actWidth;
    }
  }*/
}

void wxMainFrame::TileHoriz(QMenuBar *menubar, int& lastTile)
{
  //int ii, cc = 0, y = 0, heightForEach, preferredHeight, actHeight, minHeight=0;
  //QWidget *window;
  //QWidgetList windows = m_workspace->windowList();

  //lastTile = 2;
  //wxTheApp->isChMaximized = false;

  //if (!windows.count() )
  //  return;
  //cc = (int)windows.count();
  //for (ii = 0; ii < int(windows.count()); ++ii ) {
  //  window = windows.at(ii);
  //  if (((QMainWindow*)window)->isMinimized()) {
  //    cc--;
  //    minHeight = menubar->height();
  //  }
  //  //else
  //  //  if (window->inherits("wxChildFrame"))
  //  //    ((wxChildFrame*)window)->oldWindowState = Qt::WindowNoState;
  //}
  //if (!cc)
  //  return;
  //if (cc > 2) {
  //  m_workspace->tile();
  //  return;
  //}
  //heightForEach = (m_workspace->height() - minHeight) / cc;
  //for (ii = 0; ii < int(windows.count()); ++ii ) {
  //  window = windows.at(ii);
  //  if (!((QMainWindow*)window)->isMinimized()) {
  //    preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
  //    actHeight = qMax(heightForEach, preferredHeight);
  //    if (window == theActiveFrame)
  //      window->showNormal();
  //    window->parentWidget()->setGeometry( 0, y, m_workspace->width(), actHeight );
  //    y += actHeight;
  //  }
  //}
}

void wxMainFrame::MoveToNewTabbedWindow(MyTabWidget *tw,int index){
  MyTabWidget *newTW=new MyTabWidget(0);
  newTW->setUsesScrollButtons(true);
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  QWidget *page=tw->widget(index);

  newTW->addTab(page,QString());
  newTW->setTabText(0,tt);
  newTW->setTabToolTip(0,ttt);
  QPushButton *close=new QPushButton(QPixmap(QString::fromUtf8(":/LavaPE/res/TOOLBUTTONS/LavaPE/res/TOOLBUTTONS/close.xpm")),QString());
  close->setToolTip("Close current page");
  connect(close,SIGNAL(clicked(bool)),newTW,SLOT(closePage(bool)));
  newTW->setCornerWidget(close);
  QSplitter *splitter = (QSplitter*)tw->parentWidget();
  int splitterIndex = splitter->indexOf(tw);

  splitter->insertWidget(splitterIndex+1,newTW);
  connect(newTW ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
}

void wxMainFrame::MoveToNextTabbedWindow(MyTabWidget *tw,int index){
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  QWidget *page=tw->widget(index);
  QSplitter *splitter = (QSplitter*)tw->parentWidget();
  int splitterIndex = splitter->indexOf(tw);
  MyTabWidget *nextTW=(MyTabWidget*)splitter->widget(splitterIndex+1);

  nextTW->insertTab(0,page,QString(tt));
  nextTW->setTabToolTip(0,ttt);
  nextTW->setCurrentIndex(0);
  if (tw->count() == 0 && splitter->count() > 1)
    tw->deleteLater();
  wxTheApp->updateButtonsMenus();
}

void wxMainFrame::MoveToPrecedingTabbedWindow(MyTabWidget *tw,int index){
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  QWidget *page=tw->widget(index);
  QSplitter *splitter = (QSplitter*)tw->parentWidget();
  int splitterIndex = splitter->indexOf(tw);
  MyTabWidget *precTW=(MyTabWidget*)splitter->widget(splitterIndex-1);

  precTW->insertTab(0,page,QString(tt));
  precTW->setTabToolTip(0,ttt);
  precTW->setCurrentIndex(0);
  if (tw->count() == 0 && splitter->count() > 1)
    tw->deleteLater();
  wxTheApp->updateButtonsMenus();
}

wxChildFrame::wxChildFrame(QWidget *parent)
    : QWidget(parent)
{
  m_clientWindow = this;
  m_tabWidget = (MyTabWidget*)parent;
  lastActive = 0;
  m_tabWidget->addTab(this,QString());
  layout = new QVBoxLayout(this);
  setLayout(layout);
  layout->setMargin(0);
  setAttribute(Qt::WA_DeleteOnClose);

  deleting = false;
  QApplication::postEvent(wxTheApp->m_appWindow,new CustomEvent(QEvent::User,this));
}

void wxChildFrame::resizeEvent(QResizeEvent *ev) {
}

bool wxChildFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  doc->AddChildFrame(this);
  m_document = doc;
  m_tabWidget->setCurrentWidget(this);
  return true;
}

void wxChildFrame::AddView(wxView *view)
{
  m_viewList.append(view);
}

void wxChildFrame::InitialUpdate()
{
  for (int i=0; i<m_viewList.size(); i++) {
    m_viewList.at(i)->OnInitialUpdate();
    m_viewList.at(i)->show();
  }
  show();
}

void wxChildFrame::Activate(bool activate, bool windowMenuAction)
{
 QString title=windowTitle();

 if (activate)
  m_tabWidget->setCurrentWidget(this);

 if (title.length() && title.at(title.length()-1) == '*')
   title = title.left(title.length()-1);
 wxTheApp->m_appWindow->GetWindowHistory()->SetFirstInHistory(title);
 if (activate)
   if (lastActive)
     wxDocManager::GetDocumentManager()->SetActiveView(lastActive, true);
   else if (!m_viewList.empty())
     wxDocManager::GetDocumentManager()->SetActiveView(m_viewList.first(),true);
}

void wxChildFrame::SetTitle(QString &title)
{
  QString oldTitle=parentWidget()->windowTitle(), newTitle=title, tooltip;
  QTabWidget *tw=(QTabWidget*)parentWidget()->parentWidget();

  if (title.contains(": ")) {
    tw->setTabText(tw->indexOf(this),title);
    tooltip = m_document->GetFilename() + ", " + title + "\nNote the context menu on every tab!";
    tw->setTabToolTip(tw->indexOf(this),tooltip);
  }
  else {
    QFileInfo fi(title);
    tw->setTabText(tw->indexOf(this),fi.fileName());
    tw->setTabToolTip(tw->indexOf(this),m_document->GetFilename() + "\nNote the context menu on every tab!");
  }
  if (newTitle.at(newTitle.length()-1) == '*')
    newTitle = newTitle.left(newTitle.length()-1);
  if (!oldTitle.isEmpty())
    wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}

bool wxChildFrame::event(QEvent *ev)
{
  if (ev->type() == QEvent::WindowStateChange)
    wxTheApp->isChMaximized = parentWidget()->isMaximized();
  return QWidget::event(ev);
}

void wxChildFrame::RemoveView(wxView *view)
// Called from ~wxView
{
  if (view == lastActive)
    lastActive = 0;
  int sz = m_viewList.size();
  int ind = m_viewList.indexOf(view);
  if (ind >= 0 && ind < sz)
    m_viewList.removeAt(ind);
}

wxChildFrame::~wxChildFrame()
{
  QString title;

  title = windowTitle();
  if (!title.isEmpty() && title.at(title.length()-1) == '*')
    title = title.left(title.length()-1);
  deleting = true;
  int count = m_document->RemoveChildFrame(this);
  if (!wxTheApp->deletingMainFrame)
    wxTheApp->m_appWindow->GetWindowHistory()->RemoveItemFromHistory(title);
  if (!count && !m_document->deleting)
    delete m_document;
}


void MyTabWidget::mousePressEvent ( QMouseEvent *evt ) {
  QTabBar *tb=tabBar();
  QPoint pt=evt->pos();
  Qt::MouseButton mb=evt->button();
  QSplitter *splitter=(QSplitter*)parentWidget();
  int index=tb->tabAt(pt), splitterIndex=splitter->indexOf(this);
  wxChildFrame *page=(wxChildFrame*)widget(index);

  if (mb != Qt::RightButton || index == -1)
    return;

  QMenu tabMenu;
  QAction *triggeredAction;

  QAction *closePageAction = tabMenu.addAction("Close this page");
  QAction *closeFileAction = tabMenu.addAction("Close this file");
  QAction *newTabWidAction = tabMenu.addAction("Move to new tabbed window");
  QAction *movePageRightAction = tabMenu.addAction("Move to next tabbed window");
  QAction *movePageLeftAction = tabMenu.addAction("Move to preceding tabbed window");

  if (count() == 1)
    newTabWidAction->setEnabled(false);
  if (splitterIndex == splitter->count()-1)
    movePageRightAction->setEnabled(false);
  if (splitterIndex == 0)
    movePageLeftAction->setEnabled(false);

  triggeredAction = tabMenu.exec(QCursor::pos());

  if (triggeredAction == closePageAction) {
    if (page->inherits("CTreeFrame")
    || (page->inherits("CLavaGUIFrame") && wxTheApp->inherits("CLavaApp"))) {
      page->Activate(true);
      wxDocManager::GetDocumentManager()->OnFileClose();
    }
    else {
      removeTab(index);
      delete page;
    }
    if (count() == 0 && splitter->count() > 1)
      deleteLater();
    wxTheApp->updateButtonsMenus();
  }
  else if (triggeredAction == closeFileAction) {
    page->Activate(true);
    wxDocManager::GetDocumentManager()->OnFileClose();
    if (count() == 0 && splitter->count() > 1)
      deleteLater();
    wxTheApp->updateButtonsMenus();
  }
  else if (triggeredAction == newTabWidAction) {
    page->Activate(true);
    wxTheApp->m_appWindow->MoveToNewTabbedWindow(this,index);
    wxTheApp->updateButtonsMenus();
  }
  else if (triggeredAction == movePageRightAction) {
    page->Activate(true);
    wxTheApp->m_appWindow->MoveToNextTabbedWindow(this,index);
    wxTheApp->updateButtonsMenus();
  }
  else if (triggeredAction == movePageLeftAction) {
    page->Activate(true);
    wxTheApp->m_appWindow->MoveToPrecedingTabbedWindow(this,index);
    wxTheApp->updateButtonsMenus();
  }
}

void MyTabWidget::closePage(bool) {
  wxChildFrame *page=(wxChildFrame*)currentWidget();
  int index=currentIndex();
  QSplitter *splitter=(QSplitter*)parentWidget();

  if (page->inherits("CTreeFrame")
  || (page->inherits("CLavaGUIFrame") && wxTheApp->inherits("CLavaApp"))) {
    page->Activate(true);
    wxDocManager::GetDocumentManager()->OnFileClose();
  }
  else {
    removeTab(index);
    delete page;
  }
  if (count() == 0 && splitter->count() > 1)
    deleteLater();
  wxTheApp->updateButtonsMenus();
}


#define MYSTYLEIMP(sty)\
  int My##sty##Style::pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const\
{\
  int px = Q##sty##Style::pixelMetric( pm, option, widget);\
  switch( pm )\
  {\
    case PM_ToolBarItemMargin:\
    case PM_ToolBarItemSpacing:\
      px = 0; break;\
    default: break;\
  }\
  return px;\
}
    //case PM_ToolBarFrameWidth:\
    //case PM_ToolBarIconSize:\
    //  px = 16; break;

#ifdef WIN32
MYSTYLEIMP(WindowsXP)
MYSTYLEIMP(WindowsVista)
#endif
#ifdef Darwin
MYSTYLEIMP(Mac)
#endif
MYSTYLEIMP(Windows)
MYSTYLEIMP(Plastique)
MYSTYLEIMP(CDE)
MYSTYLEIMP(Motif)
MYSTYLEIMP(Cleanlooks)
