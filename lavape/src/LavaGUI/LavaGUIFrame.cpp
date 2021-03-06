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


#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "DumpView.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>

#pragma hdrstop
#include "Lava.xpm"

CLavaGUIFrame::CLavaGUIFrame(QWidget *parent) : wxChildFrame(parent)
{
  myView = 0;
  onClose = false;
}

bool CLavaGUIFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  doc->AddChildFrame(this);
  m_document = doc;
	//setWindowIcon(QIcon(QPixmap((const char**) Lava)));
	//resize(500,300);
  myDoc = (CLavaBaseDoc*)doc;
  m_document = (CLavaBaseDoc*)doc;
  QString str(myDoc->GetFilename());
  SetTitle(str);
  if (LBaseData->inRuntime)
    if (myDoc->isObject) {
      myView = new CLavaGUIView(this, myDoc);
      myDoc->AddView(myView);
      layout->addWidget(myView);
      layout->setMargin(0);
      show();
    }
    else
      hide();
	return true;

}

void CLavaGUIFrame::InitialUpdate()
{
  for (int i=0; i<m_viewList.size(); i++)
    m_viewList.at(i)->OnInitialUpdate();
  Activate(true);
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
  if (!wxTheApp->appExit && !wxTheApp->deletingMainFrame
    && myDoc->DumpFrame) 
    ((LavaDumpFrame*)myDoc->DumpFrame)->returned = true;
  deleting = true;
}


DString CLavaGUIFrame::CalcTitle(LavaDECL* decl, const DString& lavaName)
{
  //DString title = lavaName;
  //title += DString(" - ");
  //DString title += decl->FullName;
  return decl->FullName + ": Form";
}

void CLavaGUIFrame::NewTitle(LavaDECL *decl, const DString& lavaName)
{
  QString oldTitle=windowTitle(), newTitle;

  if (decl) {
    DString title = CalcTitle(decl, lavaName);
    newTitle = QString(title.c);
  }
  else
    newTitle = QString(lavaName.c);
  setWindowTitle(newTitle);
  //if (!oldTitle.isEmpty() && newTitle != oldTitle)
  //  wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}


void CLavaGUIFrame::SetModified(const QString& lavaName, bool changed)
{
  DString title = qPrintable(lavaName);
  if (title.l && (title[title.l-1] == '*')) {
    if (!changed)
      title.Delete(title.l-1, 1);
  }
  else
    if (changed)
      title += "*";
  setWindowTitle(title.c);
}

