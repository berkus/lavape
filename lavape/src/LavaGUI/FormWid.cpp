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


#include "FormWid.h"
#include "GUIProgBase.h"
#include "GUIProg.h"
#include "LavaGUIView.h"
#include "qtooltip.h"
//#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////////
// CFormWid



CFormWid::~CFormWid()
{
//  if (origMenu)
//    delete myMenu;
}

CFormWid::CFormWid(CGUIProgBase *guiPr, CHEFormNode* data,
                QWidget* parent, int border, SynFlags iterFlags, const char* lpszWindowName)
                :QFrame(parent, "FormWid")
{
  nRadio = 0;
  myMenu = 0;
  iterData = 0;
  BGroup = 0;
  GUIProg = guiPr;
  origMenu = false;
  usedInFormNode = false;
  myFormNode = (CHEFormNode*)data;
  if (border) { //the FIP.widget 
    myFormNode->data.ownLFont = GUIProg->SetLFont(0, myFormNode);
    myFormNode->data.ownTFont = GUIProg->SetTFont(0, myFormNode);
    GUIProg->SetColor(this, myFormNode);
    if (parentWidget()->parentWidget()->inherits("GUIVBox")) 
      parentWidget()->parentWidget()->setPaletteBackgroundColor(colorGroup().background());
  }
  show();
  int bord = ((QFrame*)parent)->lineWidth();
  QRect rect(0,0,GUIProg->globalIndent,GUIProg->globalIndent);
  setGeometry(rect);
  if (iterFlags.Contains(IteratedItem))
    IterFlags.INCL(IteratedItem);
  if (iterFlags.Contains(FixedCount))
    IterFlags.INCL(FixedCount);
  if (iterFlags.Contains(Optional))
    IterFlags.INCL(Optional);
  if (border > 0) {
    setFrameShape(QFrame::Box );
    setFrameShadow(QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(1);
  }  
  if (!LBaseData->inRuntime && border
    && myFormNode->data.FormSyntax->ParentDECL
    &&  (myFormNode->data.FormSyntax->ParentDECL->ParentDECL == GUIProg->myDECL))
      {
    setLineWidth(1);
    setMidLineWidth(1);
    //setPaletteBackgroundColor(QColor("#FF0000"));
  }
  else {
    setLineWidth(0);
    setMidLineWidth(0);
    //setPaletteBackgroundColor(QColor("#00FF00"));
  }
  if (IterFlags.BITS()
      && !IterFlags.Contains(FixedCount)
      && !myFormNode->data.IoSigFlags.Contains(DONTPUT)
      && myFormNode->data.IoSigFlags.Contains(Flag_INPUT)) {
    iterData = myFormNode;

    myMenu = new QPopupMenu(this);

    origMenu = true;
    if (IterFlags.Contains(IteratedItem)) {
      myMenu->insertItem("Delete chain element", this, SLOT(DelActivated()),0,IDM_ITER_DEL);
      myMenu->insertItem("Insert chain element", this, SLOT(InsActivated()),0, IDM_ITER_INSERT);
    }
    else 
      myMenu->insertItem("Delete optional", this, SLOT(DelActivated()),0, IDM_ITER_DEL);
//      myMenu->insertItem("Delete optional", this, "DelActivated()",0, IDM_ITER_DEL);
  }
  else
    if (parent->inherits("CFormWid")) {
      myMenu = ((CFormWid*)parent)->myMenu;
      iterData = ((CFormWid*)parent)->iterData;
    }

  ForegroundColor = colorGroup().foreground();
}
 

void CFormWid::AddRadio(QButton* radio)
{
  Radio[nRadio] = radio;
  nRadio++;
}


void CFormWid::mousePressEvent(QMouseEvent* ev) 
{
  if (myMenu) 
    myMenu->popup(ev->globalPos());//QPoint(ev->x(), ev->y()));
  else
    if (wxDocManager::GetDocumentManager()->GetActiveView() != GUIProg->ViewWin)
      wxDocManager::GetDocumentManager()->SetActiveView((wxView*)GUIProg->ViewWin);
}

void CFormWid::InsActivated()
{
  QApplication::postEvent(this, new QCustomEvent(IDU_LavaGUIInsDel,(void*)IDM_ITER_INSERT));
}

void CFormWid::DelActivated()
{
  QApplication::postEvent(this, new QCustomEvent(IDU_LavaGUIInsDel,(void*)IDM_ITER_DEL));
}

bool CFormWid::event(QEvent* ev)
{
  if (ev->type() == IDU_LavaGUIInsDel) {
    int id = (int)((QCustomEvent*)ev)->data();
    if (id == IDM_ITER_DEL) 
      if (iterData->data.IterFlags.Contains(Optional))
        ((CGUIProg*)GUIProg)->CmdExec.DeleteOptionalItem(iterData);
      else 
        ((CGUIProg*)GUIProg)->CmdExec.DeleteIterItem(iterData);
    else if (id == IDM_ITER_INSERT) 
      ((CGUIProg*)GUIProg)->CmdExec.InsertIterItem(iterData);
    return true;
  }
  else {
    if (ev->type() == QEvent::Enter) 
      ((CLavaGUIView*)GUIProg->ViewWin)->MessToStatusbar();
    return QFrame::event(ev);
  }
}

