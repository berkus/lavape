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
  m_ComboBar = new CComboBar(decl, (CPEBaseDoc*)myDoc, this);
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

