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


#include "PEBaseDoc.h"
#include "ExecFrame.h"
#include "cmainframe.h"
#include "ComboBar.h"
#include "LavaAppBase.h"
#include "qlayout.h"
//Added by qt3to4:
#include <QPixmap>

#pragma hdrstop
#include "execframe.xpm"

/////////////////////////////////////////////////////////////////////////////
// CExecFrame construction/destruction

CExecFrame::CExecFrame(QWidget *parent) : wxMDIChildFrame(parent)
{
  // TODO: add member initialization code here
//  m_ComboBar = 0;
  myDoc = LBaseData->actHint->fromDoc;
  LavaDECL *decl = (LavaDECL*)LBaseData->actHint->CommandData1;
  myDECL = decl->ParentDECL;
}

CExecFrame::~CExecFrame() {
}

void CExecFrame::closeEvent(QCloseEvent *e)
{
  //deleteLater();
}

void CExecFrame::InitialUpdate()
{
  wxMDIChildFrame::InitialUpdate();
  lastActive = m_viewList.first();
  wxDocManager::GetDocumentManager()->SetActiveView(lastActive, true);
}

bool CExecFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  LavaDECL *decl = (LavaDECL*)LBaseData->actHint->CommandData1;

  if (!temp->m_viewClassInfo)
    return false;
  m_ComboBar = new CComboBar(decl, (CPEBaseDoc*)myDoc, GetClientWindow());
  wxView *view = (wxView *)temp->m_viewClassInfo(GetClientWindow(),doc);
  if (view->OnCreate())
    wxDocManager::GetDocumentManager()->SetActiveView(view, true);
  else {
    delete view;
    return false;
  }
  view->SetDocument(doc);
  layout->setSpacing(1);
  layout->addWidget(m_ComboBar);
  layout->addWidget(view);
//  m_ComboBar->show();
  setWindowIcon(QIcon(QPixmap((const char**) execframe)));
//  NewTitle(decl);
  return wxMDIChildFrame::OnCreate(temp,doc);
}

void CExecFrame::OnDestroy()
{
 // if (m_ComboBar)
 //   m_ComboBar->DeleteObjData();
 // delete m_ComboBar;
}

/////////////////////////////////////////////////////////////////////////////
// CExecFrame message handlers

DString CExecFrame::CalcTitle(LavaDECL *decl)
{
  DString title = ((CPEBaseDoc*)myDoc)->IDTable.DocName; //((CPEBaseDoc*)myDoc)->GetSyntaxName();
//  if (title != decl->FullName) {
    title += DString(" - ");
    title += decl->FullName;
//  }
  LavaDECL* parDECL = decl->ParentDECL;
  switch (decl->DeclType) {
  case ExecDef:
    if (parDECL->DeclType == Interface
      || parDECL->DeclType == Impl) {
      if (parDECL->DeclType == Impl)
        title += "::impl";
      title += ": Invariant";
    }
    else
      title += ": Exec";
    break;
  case Require:
    if (parDECL->ParentDECL->DeclType == Impl)
      title += "::impl";
    title += ": Require";
    break;
  case Ensure:
    if (parDECL->ParentDECL->DeclType == Impl)
      title += "::impl";
    title += ": Ensure";
    break;
  }
  return title;
}

void CExecFrame::NewTitle(LavaDECL *decl)
{
  QString title = CalcTitle(decl).c;
  SetTitle(title);
}

