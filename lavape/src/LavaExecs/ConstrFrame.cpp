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

#include "PEBaseDoc.h"
#include "ConstrFrame.h"
#include "cmainframe.h"
#include "ComboBar.h"
#include "LavaAppBase.h"
#include "qlayout.h"
#include "execframe.xpm"
//#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////////
// CExecFrame used for FormView and ConstrView

//IMPLEMENT_DYNCREATE(CExecFrame, CMDIChildWnd)
/*
BEGIN_MESSAGE_MAP(CExecFrame, CMDIChildWnd)
  //{{AFX_MSG_MAP(CExecFrame)
  ON_WM_DESTROY()
  ON_WM_CLOSE()
  ON_WM_SYSCOMMAND( )
  ON_WM_CREATE()
  ON_COMMAND(ID_Execbar4, OnExecbar4)
  ON_UPDATE_COMMAND_UI(ID_Execbar4, OnUpdateExecbar4)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/
/////////////////////////////////////////////////////////////////////////////
// CExecFrame construction/destruction

CExecFrame::CExecFrame(QWidget *parent) : wxMDIChildFrame(parent, "ExecFrame")
{
  // TODO: add member initialization code here
//  m_ComboBar = 0;
  myDoc = LBaseData->actHint->fromDoc;
  LavaDECL *decl = (LavaDECL*)LBaseData->actHint->CommandData1;
  myDECL = decl->ParentDECL;
}

void CExecFrame::InitialUpdate()
{
  wxMDIChildFrame::InitialUpdate();
  lastActive = m_viewList.first();
  wxDocManager::GetDocumentManager()->SetActiveView(lastActive, true);
}

bool CExecFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  setIcon(QPixmap((const char**) execframe));
  //resize(500,300);

  LavaDECL *decl = (LavaDECL*)LBaseData->actHint->CommandData1;
  NewTitle(decl);
  m_ComboBar = new CComboBar(myDECL, (CPEBaseDoc*)myDoc, this);
  return wxMDIChildFrame::OnCreate(temp,doc);
}
/*
int CExecFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  EnableDocking(CBRS_ALIGN_ANY);

  if (!((CComboBar*)m_ComboBar)->Create(0, this,
       IDD_ComboBar, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY
       | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI | CBRS_GRIPPER, IDD_ComboBar))
  m_ComboBar->ModifyStyle(0,TBSTYLE_FLAT|CCS_ADJUSTABLE);
  m_ComboBar->EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(m_ComboBar,AFX_IDW_DOCKBAR_TOP);
  m_ComboBar->SetBarStyle(m_ComboBar->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
  RecalcLayout();
  return 0;
}
*/

void CExecFrame::OnDestroy()
{
 // if (m_ComboBar)
 //   m_ComboBar->DeleteObjData();
 // delete m_ComboBar;
}

/////////////////////////////////////////////////////////////////////////////
// CExecFrame diagnostics


/////////////////////////////////////////////////////////////////////////////
// CExecFrame message handlers
/*
BOOL CExecFrame::Create(char* lpszClassName, char*, DWORD dwStyle, const RECT& rect, wxMainFrame* pParentWnd, CCreateContext* pContext) 
{
  // TODO: Add your specialized code here and/or call the base class
  dwStyle = dwStyle & (~FWS_ADDTOTITLE);
  myDoc = LBaseData->actHint->fromDoc;
  LavaDECL * decl = (LavaDECL*)LBaseData->actHint->CommandData1;
  myDECL = decl->ParentDECL;
  DString title = CalcTitle(decl);
  return CMDIChildWnd::Create(lpszClassName, title.c, dwStyle, rect, pParentWnd, pContext);
  
}
*/
DString CExecFrame::CalcTitle(LavaDECL *decl)
{
  DString title = ((CPEBaseDoc*)myDoc)->IDTable.DocName; //((CPEBaseDoc*)myDoc)->GetSyntaxName();
  if (title != decl->FullName) {
    title += DString(" - ");
    title += decl->FullName;
  }
  LavaDECL* parDECL = decl->ParentDECL;
  if (parDECL && (parDECL->DeclType == Function) || (parDECL->DeclType == Initiator)
      || parDECL->DeclType == Impl)
    title += ": Exec";
  else
    title += ": Constraint";
  return title;
}

void CExecFrame::NewTitle(LavaDECL *decl)
{
  QString title = CalcTitle(decl).c;
  SetTitle(title);
}

