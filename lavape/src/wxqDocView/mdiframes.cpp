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

	 /* This file has been taken over from www.wxwindows.org
   and adapted to the needs of LavaPE */


/////////////////////////////////////////////////////////////////////////////
// Name:        mdiframes.cpp
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUC__
#pragma implementation
#endif


#include "SYSTEM.h"
#include "defs.h"
#include "mdiframes.h"
#include "qstring.h"
#include "qmenubar.h"
#include "qstatusbar.h"
#include "qmessagebox.h"
#include "qvbox.h"
#include "qfile.h"

/*
 * Docview MDI parent frame
 */

wxMainFrame::wxMainFrame(QWidget* parent, const char* name, WFlags fl)
 : QMainWindow(parent,name,WDestructiveClose)
{
	completelyCreated = false;
  m_CentralWidget = new QVBox(this);
  m_CentralWidget->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setCentralWidget(m_CentralWidget);
  ((wxApp*)qApp)->m_appWindow = this;
  qApp->setMainWidget(this);
	m_childFrameHistory = new wxHistory;
}

bool wxMainFrame::OnCreate()
{
  CreateWorkspace(m_CentralWidget);
//  resize(800,600);
  LoadFileHistory();
	completelyCreated = true;
  return true;
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


void wxMainFrame::closeEvent (QCloseEvent*)
{
  fileExit();
}

void wxMainFrame::fileExit()
{
  ((wxApp*)qApp)->apExit = true;
  if ( wxDocManager::GetDocumentManager()->Clear(false))
	  qApp->quit();
	else
		((wxApp*)qApp)->apExit = false;
}

void wxMainFrame::LoadFileHistory() {
  wxDocManager::GetDocumentManager()->FileHistoryLoad(*wxTheApp->settings);
}

void wxMainFrame::OnMRUFile(int histFileIndex)
{
    QString *file=wxDocManager::GetDocumentManager()->GetHistoryFile(histFileIndex);
		
    if (file->isEmpty())
        return;
    QFile f(*file);
    if (!f.open(IO_ReadOnly))
    {
        wxDocManager::GetDocumentManager()->RemoveFileFromHistory(histFileIndex);
        QMessageBox::critical(qApp->mainWidget(),qApp->name(),tr("Sorry, could not open this file."),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
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



void wxMainFrame::Cascade()
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
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
        window->resize(sz.width()*7/10, sz.height()*7/10);
      }
  }
}

void wxMainFrame::TileVertic(QMenuBar *menubar, int& lastTile)
{
  int ii, cc = 0, x = 0, widthForEach, preferredWidth, actWidth, minHeight=0, allHeight;
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
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
  }
  if (!cc)
    return;
  if (cc > 3) {
    m_workspace->tile();
    return;
  }
  allHeight = m_workspace->height() - minHeight;
  widthForEach = m_workspace->width() / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredWidth = window->minimumWidth()+window->parentWidget()->baseSize().width();
      actWidth = QMAX(widthForEach, preferredWidth);
      if (window == theActiveFrame) 
        window->showNormal();
      window->parentWidget()->resize(actWidth, allHeight );
      window->parentWidget()->move( x, 0);
      x += actWidth;
    }
  }
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
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
  }
  if (!cc)
    return;
  if (cc > 3) {
    m_workspace->tile();
    return;
  }
  heightForEach = (m_workspace->height() - minHeight) / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
      actHeight = QMAX(heightForEach, preferredHeight);
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

wxMDIChildFrame::wxMDIChildFrame(QWidget *parent, const char* name)
    : QMainWindow(parent,name,WDestructiveClose)
{
  QSize sz = ((wxMainFrame*)qApp->mainWidget())->GetClientWindow()->size();

  m_viewCount = 0;
  deleting = false;
  m_clientWindow = this;
  resize(sz.width()*7/10, sz.height()*7/10);
  if (wxTheApp->isChMaximized)
	  oldWindowState = QEvent::ShowMaximized;
  else
    oldWindowState = QEvent::ShowNormal;
  lastActive = 0;
  QApplication::postEvent(qApp->mainWidget(),new QCustomEvent(QEvent::User,this));
}

bool wxMDIChildFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  if (!temp->m_viewClassInfo)
    return false;
  wxView *view = (wxView *)temp->m_viewClassInfo(GetClientWindow(),doc);
  setCentralWidget(view);
  view->SetDocument(doc);
  if (oldWindowState == QEvent::ShowMaximized)
    showMaximized();
  else
    showNormal();
  if (view->OnCreate()) {
    wxDocManager::GetDocumentManager()->SetActiveView(view, true);
    return true;
  }
  else {
    delete view;
    return false;
  }
}

void wxMDIChildFrame::AddView(wxView *view)
{
  m_viewList.append(view);
  m_viewCount++;
}

void wxMDIChildFrame::InitialUpdate()
{
  wxView *view = m_viewList.first();
  while (view) {
    view->OnInitialUpdate();
		view->show();
    view = m_viewList.next();
  }
  show();
}

void wxMDIChildFrame::Activate(bool activate, bool windowMenuAction)
{
	QString title=caption();
	if (isMinimized() && windowMenuAction)
    if (oldWindowState == QEvent::ShowMaximized)
      showMaximized();
    else
      showNormal();
	if (title.at(title.length()-1) == '*')
		title = title.left(title.length()-1);
	wxTheApp->m_appWindow->GetWindowHistory()->SetFirstInHistory(title);
  if (activate && m_viewCount)
    if (lastActive) {
      wxDocManager::GetDocumentManager()->SetActiveView(lastActive, activate);
    }
    else
      wxDocManager::GetDocumentManager()->SetActiveView(m_viewList.first(),activate);
}

void wxMDIChildFrame::SetTitle(QString &title)
{
	QString oldTitle=parentWidget()->caption(), newTitle=title;

	setCaption(title);
	if (newTitle.at(newTitle.length()-1) == '*')
		newTitle = newTitle.left(newTitle.length()-1);
	if (!oldTitle.isEmpty())
		wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}

bool wxMDIChildFrame::event(QEvent * e )
{
  switch (e->type()) {
	case QEvent::ShowNormal:
		oldWindowState = QEvent::ShowNormal;
		wxTheApp->isChMaximized = false;
    break;
	case QEvent::ShowMaximized:
		oldWindowState = QEvent::ShowMaximized;
		wxTheApp->isChMaximized = true;
	default: ;
  }
  return QMainWindow::event(e);
}

void wxMDIChildFrame::RemoveView(wxView *view)
{
  if (deleting)
    return;

  m_viewCount--;
  if (!m_viewCount)
    delete this;
}

wxMDIChildFrame::~wxMDIChildFrame()
{
	QString title;

	title = caption();
	if (title.at(title.length()-1) == '*')
		title = title.left(title.length()-1);
  deleting = true;
	wxTheApp->m_appWindow->GetWindowHistory()->RemoveItemFromHistory(title);
}
