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
#include <QList>
#include <QEvent>

#pragma hdrstop

/*
 * Docview MDI parent frame
 */

wxMainFrame::wxMainFrame() : QMainWindow()
{
  wxDocManager *docMan=wxDocManager::GetDocumentManager();

  m_currentTabWidget = 0;
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

bool wxMainFrame::eventFilter(QObject *o, QEvent *e) {
  return false;
}

QSplitter* wxMainFrame::CreateWorkspace(QWidget* parent)
{
  m_ClientArea=new QSplitter(parent);
  m_currentTabWidget = new wxTabWidget(m_ClientArea);
  m_currentTabWidget->setTabShape(QTabWidget::Triangular);
  m_currentTabWidget->setUsesScrollButtons(true);
  QToolButton *close = new QToolButton();
  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
  pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
  pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));
  close->setPalette(pal);
  close->setCursor(Qt::ArrowCursor);
  close->setAutoRaise(true);
  close->setIcon(QPixmap(QString::fromUtf8(":/LavaPE/res/TOOLBUTTONS/LavaPE/res/TOOLBUTTONS/close.xpm")));
  m_currentTabWidget->setCornerWidget(close);
  close->setToolTip("Close current page");
  connect(close,SIGNAL(clicked()),m_currentTabWidget,SLOT(closePage()));
  connect(m_currentTabWidget ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
  return m_ClientArea;
}


void wxMainFrame::windowActivated(int index)
{
  theActiveFrame = m_currentTabWidget->widget(index);
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

void wxMainFrame::MoveToNewTabbedWindow(wxTabWidget *tw,int index){
  //QSplitter *split=(QSplitter*)tw->parentWidget()->parentWidget();
  wxTabWidget *newTW=new wxTabWidget(0);
  newTW->setTabShape(QTabWidget::Triangular);
  newTW->setUsesScrollButtons(true);
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  wxChildFrame *page=(wxChildFrame*)tw->widget(index);

  newTW->addTab(page,QString());
  page->correctMyTabWidget(newTW);
  newTW->setTabText(0,tt);
  newTW->setTabToolTip(0,ttt);
  SetCurrentTabWindow(newTW);
  page->Activate(true);
  QToolButton *close = new QToolButton();
  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
  pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
  pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));
  close->setPalette(pal);
  close->setIcon(QPixmap(QString::fromUtf8(":/LavaPE/res/TOOLBUTTONS/LavaPE/res/TOOLBUTTONS/close.xpm")));
  close->setCursor(Qt::ArrowCursor);
  close->setAutoRaise(true);
  newTW->setCornerWidget(close);
  connect(close,SIGNAL(clicked()),m_currentTabWidget,SLOT(closePage()));

  int splitterIndex = m_ClientArea->indexOf(tw);
  m_ClientArea->insertWidget(splitterIndex+1,newTW);
  connect(newTW ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
  equalize();
}

void wxMainFrame::MoveToNextTabbedWindow(wxTabWidget *tw,int index){
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  wxChildFrame *page=(wxChildFrame*)tw->widget(index);
  int splitterIndex = m_ClientArea->indexOf(tw);
  wxTabWidget *nextTW=(wxTabWidget*)m_ClientArea->widget(splitterIndex+1);

  nextTW->insertTab(0,page,QString(tt));
  page->correctMyTabWidget(nextTW);
  nextTW->setTabToolTip(0,ttt);
  nextTW->setCurrentIndex(0);
  SetCurrentTabWindow(nextTW);
  page->Activate(true);
  if (tw->count() == 0 && m_ClientArea->count() > 1) {
    delete tw;
    equalize();
  }
  wxTheApp->updateButtonsMenus();
}

void wxMainFrame::MoveToPrecedingTabbedWindow(wxTabWidget *tw,int index){
  QString tt=tw->tabText(index), ttt=tw->tabToolTip(index);
  wxChildFrame *page=(wxChildFrame*)tw->widget(index);
  int splitterIndex = m_ClientArea->indexOf(tw);
  wxTabWidget *precTW=(wxTabWidget*)m_ClientArea->widget(splitterIndex-1);

  precTW->insertTab(0,page,QString(tt));
  page->correctMyTabWidget(precTW);
  precTW->setTabToolTip(0,ttt);
  precTW->setCurrentIndex(0);
  SetCurrentTabWindow(precTW);
  page->Activate(true);
  if (tw->count() == 0 && m_ClientArea->count() > 1) {
    delete tw;
    equalize();
  }
  wxTheApp->updateButtonsMenus();
}

void wxMainFrame::equalize() {
  if (m_ClientArea->count()>1) {
    QList<int> sz;
    for (int i=0; i<m_ClientArea->count(); i++)
      sz.append(3000);
    m_ClientArea->setSizes(sz);
  }
}

wxChildFrame::wxChildFrame(QWidget *parent)
    : QFrame(parent)
{
  m_clientWindow = this;
  
  setLineWidth(2);
  setFrameShape(QFrame::Panel);
  setFrameShadow(QFrame::Sunken);
  m_tabWidget = (wxTabWidget*)parent;
  lastActive = 0;
  m_tabWidget->addTab(this,QString());
  layout = new QVBoxLayout(this);
  setLayout(layout);
  layout->setMargin(0);
  //setAttribute(Qt::WA_DeleteOnClose);

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
 //wxTheApp->m_appWindow->GetWindowHistory()->SetFirstInHistory(title);
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

void wxChildFrame::correctMyTabWidget(wxTabWidget *tw) {
  for (int i=0; i<m_viewList.size(); i++)
    m_viewList.at(i)->myTabWidget = tw;
  m_tabWidget = tw;
}

wxChildFrame::~wxChildFrame()
{
  QString title;

  title = windowTitle();
  if (!title.isEmpty() && title.at(title.length()-1) == '*')
    title = title.left(title.length()-1);
  deleting = true;
  while (m_viewList.size()) {
    m_document->RemoveView(m_viewList.at(0));
    RemoveView(m_viewList.at(0));
  }
  int count = m_document->RemoveChildFrame(this);
  if (!wxTheApp->deletingMainFrame)
    wxTheApp->m_appWindow->GetWindowHistory()->RemoveItemFromHistory(title);
  if (!count && !m_document->deleting)
    delete m_document;
}


void wxTabWidget::mousePressEvent ( QMouseEvent *evt ) {
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

void wxTabWidget::closePage() {
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
