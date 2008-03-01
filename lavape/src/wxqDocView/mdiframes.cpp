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
//  resize(800,600);
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

QTabWidget* wxMainFrame::CreateWorkspace(QWidget* parent)
{
  m_workspace = new QTabWidget(parent);
  m_workspace->setElideMode(Qt::ElideLeft);
  m_workspace->setUsesScrollButtons(true);
  connect(m_workspace ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
  return m_workspace;
}


void wxMainFrame::windowActivated(int index)
{
  theActiveFrame = m_workspace->widget(index);
  if (theActiveFrame && theActiveFrame->inherits("wxMDIChildFrame"))
    ((wxMDIChildFrame*)theActiveFrame)->Activate();
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
  //    if (window->inherits("wxMDIChildFrame")) {
  //      //((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
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
  //  //  if (window->inherits("wxMDIChildFrame"))
  //  //    ((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
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
  //  //  if (window->inherits("wxMDIChildFrame"))
  //  //    ((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
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

wxMDIChildFrame::wxMDIChildFrame(QWidget *parent)
    : QWidget(parent)
{
  m_clientWindow = this;
  m_tabWidget= (QTabWidget*)parent;
  lastActive = 0;
  m_tabWidget->addTab(this,QString());
  layout = new QVBoxLayout(this);
  setLayout(layout);
  layout->setMargin(0);
  setAttribute(Qt::WA_DeleteOnClose);

  QSize sz = ((wxMainFrame*)wxTheApp->m_appWindow)->GetClientWindow()->size();
  resize(sz.width()*7/10, sz.height()*7/10);
  deleting = false;
  QApplication::postEvent(wxTheApp->m_appWindow,new CustomEvent(QEvent::User,this));
}

void wxMDIChildFrame::resizeEvent(QResizeEvent *ev) {
}

bool wxMDIChildFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  doc->AddChildFrame(this);
  m_document = doc;
  m_tabWidget->setCurrentWidget(this);
  return true;
}

void wxMDIChildFrame::AddView(wxView *view)
{
  m_viewList.append(view);
}

void wxMDIChildFrame::InitialUpdate()
{
  for (int i=0; i<m_viewList.size(); i++) {
    m_viewList.at(i)->OnInitialUpdate();
    m_viewList.at(i)->show();
  }
  show();
}

void wxMDIChildFrame::Activate(bool activate, bool windowMenuAction)
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

void wxMDIChildFrame::SetTitle(QString &title)
{
  QString oldTitle=parentWidget()->windowTitle(), newTitle=title, tooltip;
  QTabWidget *tw=(QTabWidget*)parentWidget()->parentWidget();

  if (title.contains(": ")) {
    tw->setTabText(tw->indexOf(this),title);
    tooltip = m_document->GetFilename() + ", " + title;
    tw->setTabToolTip(tw->indexOf(this),tooltip);
  }
  else {
    QFileInfo fi(title);
    tw->setTabText(tw->indexOf(this),fi.fileName());
    tw->setTabToolTip(tw->indexOf(this),m_document->GetFilename());
  }
  if (newTitle.at(newTitle.length()-1) == '*')
    newTitle = newTitle.left(newTitle.length()-1);
  if (!oldTitle.isEmpty())
    wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}

bool wxMDIChildFrame::event(QEvent *ev)
{
  if (ev->type() == QEvent::WindowStateChange)
    wxTheApp->isChMaximized = parentWidget()->isMaximized();
  //else if (ev->type() == QEvent::Close)
  //  wxTheApp->m_appWindow->m_workspace->setUpdatesEnabled(false);
  return QWidget::event(ev);
}

void wxMDIChildFrame::RemoveView(wxView *view)
// Called from ~wxView
{
  if (view == lastActive)
    lastActive = 0;
  int sz = m_viewList.size();
  int ind = m_viewList.indexOf(view);
  if (ind >= 0 && ind < sz)
    m_viewList.removeAt(ind);
}

wxMDIChildFrame::~wxMDIChildFrame()
{
  QString title;

  //if (isVisible())
  //  setFocus();  // to prevent a mysterious crash on file-exit/~CExecFrame
  //clearFocus();

  title = windowTitle();
  if (!title.isEmpty() && title.at(title.length()-1) == '*')
    title = title.left(title.length()-1);
  deleting = true;
  m_document->RemoveChildFrame(this);
  if (!wxTheApp->deletingMainFrame)
    wxTheApp->m_appWindow->GetWindowHistory()->RemoveItemFromHistory(title);
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
