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

wxMainFrame::wxMainFrame(QWidget* parent, const char* name)
 : QMainWindow(parent)
{
  setObjectName(name);
  QStatusBar *stb=new QStatusBar(this);
  completelyCreated = false;

  setStatusBar(stb);
  wxTheApp->m_appWindow = this;
  m_childFrameHistory = new wxHistory;
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

QWorkspace* wxMainFrame::CreateWorkspace(QWidget* parent)
{
  m_workspace = new QWorkspace(parent);
  connect(m_workspace , SIGNAL(windowActivated(QWidget*)), SLOT(windowActivated(QWidget*)));
  return m_workspace;
}


void wxMainFrame::windowActivated(QWidget* w)
{
  theActiveFrame = (QMainWindow*)w;
  if (w && w->inherits("wxMDIChildFrame")) {
    ((wxMDIChildFrame*)w)->Activate();
  }
}
/*
bool wxMainFrame::eventFilter(QObject *obj, QEvent *ev) {
  return false;
}
*/
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
  int ii;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();
  QSize sz=m_workspace->size();

  m_workspace->cascade();
  wxTheApp->isChMaximized = false;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized())
      if (window->inherits("wxMDIChildFrame")) {
        ((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
        window->resize(sz.width()*7/10, sz.height()*7/10);
      }
  }
}

void wxMainFrame::TileVertic(QMenuBar *menubar, int& lastTile)
{
  int ii, cc = 0, x = 0, minHeight=0;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();

  wxTheApp->isChMaximized = false;
  lastTile = 1;

  if (!windows.count() )
    return;
  cc = (int)windows.count();
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (((QMainWindow*)window)->isMinimized()) {
      cc--;
      minHeight = menubar->height();
    }
    else
      if (window->inherits("wxMDIChildFrame"))
        ((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
  }
  if (!cc)
    return;
//  if (cc > 3) {
    m_workspace->tile();
    return;
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
  int ii, cc = 0, y = 0, heightForEach, preferredHeight, actHeight, minHeight=0;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();

  lastTile = 2;

  if (!windows.count() )
    return;
  cc = (int)windows.count();
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (((QMainWindow*)window)->isMinimized()) {
      cc--;
      minHeight = menubar->height();
    }
    else
      if (window->inherits("wxMDIChildFrame"))
        ((wxMDIChildFrame*)window)->oldWindowState = Qt::WindowNoState;
  }
  if (!cc)
    return;
  if (cc > 2) {
    m_workspace->tile();
    return;
  }
  heightForEach = (m_workspace->height() - minHeight) / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
      actHeight = qMax(heightForEach, preferredHeight);
      if (window == theActiveFrame)
        window->showNormal();
      window->parentWidget()->setGeometry( 0, y, m_workspace->width(), actHeight );
      y += actHeight;
    }
  }
}

void wxMainFrame::OnCloseWindow()
{
    destroy();
}

wxMDIChildFrame::wxMDIChildFrame(QWidget *parent)
    : QWidget(parent)
{
  resize(500,300);
  ((QWorkspace*)parent)->addWindow(this);
  layout = new QVBoxLayout(this);
  setLayout(layout);
  layout->setMargin(0);
  setAttribute(Qt::WA_DeleteOnClose);

  QSize sz = ((wxMainFrame*)wxTheApp->m_appWindow)->GetClientWindow()->size();
  resize(sz.width()*7/10, sz.height()*7/10);
  m_viewCount = 0;
  deleting = false;
  m_clientWindow = this;
  if (wxTheApp->isChMaximized)
    oldWindowState = Qt::WindowMaximized;
  else
    oldWindowState = Qt::WindowNoState;
  lastActive = 0;
  QApplication::postEvent(wxTheApp->m_appWindow,new CustomEvent(QEvent::User,this));
}

bool wxMDIChildFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  doc->AddChildFrame(this);
  m_document = doc;
  if (oldWindowState == Qt::WindowMaximized)
    showMaximized();
  else
    showNormal();
  return true;
}

void wxMDIChildFrame::AddView(wxView *view)
{
  m_viewList.append(view);
  m_viewCount++;
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

 if (isMinimized() && windowMenuAction)
  if (oldWindowState == Qt::WindowMaximized)
    showMaximized();
  else
    showNormal();
  if (title.length() && title.at(title.length()-1) == '*')
    title = title.left(title.length()-1);
  wxTheApp->m_appWindow->GetWindowHistory()->SetFirstInHistory(title);
  if (activate && m_viewCount)
    if (lastActive)
      wxDocManager::GetDocumentManager()->SetActiveView(lastActive, activate);
    else
      wxDocManager::GetDocumentManager()->SetActiveView(m_viewList.first(),activate);
}

void wxMDIChildFrame::SetTitle(QString &title)
{
  QString oldTitle=parentWidget()->windowTitle(), newTitle=title;

  setWindowTitle(title);
  if (newTitle.at(newTitle.length()-1) == '*')
          newTitle = newTitle.left(newTitle.length()-1);
  if (!oldTitle.isEmpty())
          wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}

bool wxMDIChildFrame::event(QEvent * e )
{
  switch (e->type()) {
  case Qt::WindowNoState:
    oldWindowState = Qt::WindowNoState;
    wxTheApp->isChMaximized = false;
    break;
  case Qt::WindowMaximized:
    oldWindowState = Qt::WindowMaximized;
    wxTheApp->isChMaximized = true;
    break;
  default: ;
  }
  return QWidget::event(e);
}

void wxMDIChildFrame::RemoveView(wxView *view)
{
  if (deleting)
    return;

  m_viewCount--;
  if (!m_viewCount)
    deleteLater();
}

wxMDIChildFrame::~wxMDIChildFrame()
{
  QString title;

  title = windowTitle();
  if (!title.isEmpty() && title.at(title.length()-1) == '*')
    title = title.left(title.length()-1);
  deleting = true;
  if (!m_document->deleting)
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
    case PM_ToolBarIconSize:\
      px = 16; break;\
    default: break;\
  }\
  return px;\
}

#ifdef WIN32
MYSTYLEIMP(WindowsXP)
#endif

MYSTYLEIMP(Windows)
MYSTYLEIMP(Plastique)
MYSTYLEIMP(CDE)
MYSTYLEIMP(Motif)
