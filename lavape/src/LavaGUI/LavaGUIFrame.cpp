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


#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "DumpView.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "Lava.xpm"


CLavaGUIFrame::CLavaGUIFrame(QWidget *parent) : wxMDIChildFrame(parent, "LavaGUIFrame")
{
  myView = 0;
  onClose = false;

}

bool CLavaGUIFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  if (wxMDIChildFrame::OnCreate(temp, doc)) {
		setIcon(QPixmap((const char**) Lava));
		resize(500,300);
    if (LBaseData->inRuntime)
      hide();
    myDoc = (CLavaBaseDoc*)doc;
		return true;
	}
	else
		return false;
}

void CLavaGUIFrame::InitialUpdate()
{
  wxView *view = m_viewList.first();
  while (view) {
    view->OnInitialUpdate();
    view = m_viewList.next();
  }
}

void CLavaGUIFrame::closeEvent(QCloseEvent *e)
{
  if (myView) {
    onClose = true;
    ((CLavaGUIView*)myView)->NoteLastModified();
    if (LBaseData->inRuntime) {
      if (myDoc->DumpFrame) {
        ((LavaDumpFrame*)myDoc->DumpFrame)->returned = true;
        delete myDoc->DumpFrame;
      }
      if (myDoc->isObject) {
	      if (myDoc->Close())
		      QWidget::closeEvent(e);
        else
          onClose = false;
      }
      else {
        if (((CLavaGUIView*)myView)->OnKill())
		      QWidget::closeEvent(e);
        else
          onClose = false;
      }
    }
    else
		  QWidget::closeEvent(e);
  }
}

CLavaGUIFrame::~CLavaGUIFrame()
{
  if (!wxTheApp->apExit && !wxTheApp->deletingMainFrame
    && myDoc->DumpFrame) 
    ((LavaDumpFrame*)myDoc->DumpFrame)->returned = true;
  deleting = true;
}


DString CLavaGUIFrame::CalcTitle(LavaDECL* decl, const DString& lavaName)
{
  DString title = lavaName;
  title += DString(" - ");
  title += decl->FullName;
  return title;
}

void CLavaGUIFrame::NewTitle(LavaDECL *decl, const DString& lavaName)
{
  QString oldTitle=caption(), newTitle;

  if (decl) {
    DString title = CalcTitle(decl, lavaName);
    newTitle = QString(title.c);
  }
  else
    newTitle = QString(lavaName.c);
  setCaption(newTitle);
  if (!oldTitle.isEmpty() && newTitle != oldTitle)
    wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
//  QApplication::postEvent(qApp->mainWidget(),new QCustomEvent(QEvent::User,this));
}


void CLavaGUIFrame::SetModified(const QString& lavaName, bool changed)
{
  DString title = DString(lavaName);
  if (title.l && (title[title.l-1] == '*')) {
    if (!changed)
      title.Delete(title.l-1, 1);
  }
  else
    if (changed)
      title += "*";
  setCaption(title.c);
}


/*
BEGIN_MESSAGE_MAP(CLavaGUIFrame, CMDIChildWnd)
  //{{AFX_MSG_MAP(CLavaGUIFrame)
  ON_WM_SYSCOMMAND( )
  ON_WM_CLOSE()
  ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLavaGUIFrame message handlers

bool CLavaGUIFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.lpszClass = LPCTSTR(LBaseData->WndPopup);
  return CMDIChildWnd::PreCreateWindow(cs);
}

bool CLavaGUIFrame::Create(LPCTSTR lpszClassName, LPCTSTR , DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
//  dwStyle = dwStyle & (~FWS_ADDTOTITLE);
  DString title;
  title.Reset(1);
  return CMDIChildWnd::Create(lpszClassName, title.c, dwStyle, rect, pParentWnd, pContext);

}


int CLavaGUIFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc);
{
  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
//  CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)GetParentFrame();
  if (!LBaseData->inRuntime) {
    EnableDocking(CBRS_ALIGN_ANY);
    m_GUIBar = new CToolBar;
    m_GUIBar->Create(this);
    m_GUIBar->LoadToolBar(IDR_GUIBAR);
    m_GUIBar->ModifyStyle(0,TBSTYLE_FLAT);
    m_GUIBar->EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(m_GUIBar,AFX_IDW_DOCKBAR_TOP);
    m_GUIBar->SetBarStyle(m_GUIBar->GetBarStyle() |
                      CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
    ShowControlBar(m_GUIBar,true,false);
  }
  else
    m_GUIBar = 0;

//  ShowWindow(SW_SHOWMAXIMIZED);
  RecalcLayout(); 
  return 0;
}


void CLavaGUIFrame::OnSysCommand( UINT nID, LPARAM lParam )
{
  if (nID == SC_MAXIMIZE)
    wxTheApp->isChMaximized = TRUE;
  else
    if (nID == SC_RESTORE)
      wxTheApp->isChMaximized = FALSE;
  CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CLavaGUIFrame::OnClose() 
{
  if (myView) {
    ((CLavaGUIView*)myView)->NoteLastModified();
    if (LBaseData->inRuntime && !myDoc->isObject) 
      ((CLavaGUIView*)myView)->OnKill();
    else
      CMDIChildWnd::OnClose();
  }
  else
    CMDIChildWnd::OnClose();
}
*/
