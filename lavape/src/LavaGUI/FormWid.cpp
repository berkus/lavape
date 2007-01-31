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


#include "GUIres.h"
#include "FormWid.h"
#include "GUIProgBase.h"
#include "GUIProg.h"
#include "LavaGUIView.h"
#include "qtooltip.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QMenu>

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CFormWid



CFormWid::~CFormWid()
{
}

CFormWid::CFormWid(CGUIProgBase *guiPr, CHEFormNode* data,
                QWidget* parent, int border, const char* lpszWindowName)
                :QFrame(parent)
{
  nRadio = 0;
  hasMenu = false;
  hasFuncMenu = false;
  iterData = 0;
  BGroup = 0;
  GUIProg = guiPr;
  usedInFormNode = false;
  myFormNode = (CHEFormNode*)data;
  setEnabled(true);
  setAutoFillBackground(true);
  GUIProg->SetColor(this, myFormNode, QPalette::Window, QPalette::WindowText);
  if (border) { //the FIP.widget 
    myFormNode->data.ownLFont = GUIProg->SetLFont(0, myFormNode);
    myFormNode->data.ownTFont = GUIProg->SetTFont(0, myFormNode);
    if (parent->parentWidget()->inherits("GUIVBox")) {
      QPalette pp = parent->parentWidget()->palette();
      pp.setColor(QPalette::Active, QPalette::Window, palette().color(QPalette::Active, QPalette::Window));
      pp.setColor(QPalette::Inactive, QPalette::Window, palette().color(QPalette::Active, QPalette::Window));
      parent->parentWidget()->setPalette(pp);
      //parentWidget()->parentWidget()->setPaletteBackgroundColor(colorGroup().background());
    }
  }
  else
    if (parent->inherits("GUIVBox")) {
      QPalette p = parent->palette();
      p.setColor(QPalette::Active, QPalette::Window, palette().color(QPalette::Active, QPalette::Window));
      p.setColor(QPalette::Inactive, QPalette::Window, palette().color(QPalette::Active, QPalette::Window));
      parent->setPalette(p);
      //parentWidget()->parentWidget()->setPaletteBackgroundColor(colorGroup().background());
    }
  show();
  int bord = GUIProg->GetLineWidth(parent);//((QFrame*)parent)->lineWidth();
  QRect rect(0,0,GUIProg->globalIndent,GUIProg->globalIndent);
  setGeometry(rect);
  if (myFormNode->data.IterFlags.Contains(IteratedItem))
    IterFlags.INCL(IteratedItem);
  if (myFormNode->data.IterFlags.Contains(FixedCount))
    IterFlags.INCL(FixedCount);
  if (myFormNode->data.IterFlags.Contains(Optional))
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
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (IterFlags.BITS()
      && !IterFlags.Contains(FixedCount)
      && !myFormNode->data.IoSigFlags.Contains(DONTPUT)
      && myFormNode->data.IoSigFlags.Contains(Flag_INPUT)) {
    iterData = myFormNode;
    hasMenu = true;
  }
  else {
    if (par) 
      iterData = ((CFormWid*)par)->iterData;
  }
  if (myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
    && (myFormNode->data.FormSyntax->DeclType == PatternDef) ) {
    myFormNode->data.myHandlerNode = myFormNode->data.FIP.up->data.myHandlerNode;
    myFormNode->data.allowOwnHandler = myFormNode->data.FIP.up->data.allowOwnHandler;
  }
  if ( myFormNode->data.allowOwnHandler
    && (myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
      || myFormNode->data.BasicFlags.Contains(ButtonMenu))) {
    if (!LBaseData->inRuntime) {
      hasFuncMenu = true;
      if (myFormNode->data.FormSyntax->DeclType != PatternDef)
        myFormNode->data.myHandlerNode = myFormNode;
    }
  }
  else
    if (!myFormNode->data.allowOwnHandler
       && myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet))
      myFormNode->data.myHandlerNode = 0;

  if ( myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
       && !myFormNode->data.handlerSearched)
    GUIProg->setHandler(myFormNode);

  ForegroundColor = palette().windowText().color();
}
 

void CFormWid::AddRadio(QPushButton* radio)
{
  Radio[nRadio] = radio;
  nRadio++;
}


void CFormWid::mousePressEvent(QMouseEvent* ev) 
{
  QAction* action;
  QMenu* myMenu = 0;
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  GUIProg->ActNode = myFormNode;
  if (hasMenu || hasFuncMenu) {
    if (hasMenu) { 
      myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->delActionPtr);//DelAction);
      myMenu->addAction(GUIProg->insActionPtr);//InsAction);
      ((CGUIProg*)GUIProg)->OnUpdateInsertopt(GUIProg->insActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateDeleteopt(GUIProg->delActionPtr);
    }
    if (!LBaseData->inRuntime && hasFuncMenu) {
      if (!myMenu)
        myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->newHandlerActionPtr);
      myMenu->addAction(GUIProg->attachHandlerActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateNewHandler(GUIProg->newHandlerActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateAttachHandler(GUIProg->attachHandlerActionPtr);
    }
    action = myMenu->exec(ev->globalPos());
    delete myMenu;
    if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
      ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
    else if (action == GUIProg->newHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnNewHandler();
    else if (action == GUIProg->attachHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnAttachHandler();

  }
  else
    if (GUIProg->isView && 
      (wxDocManager::GetDocumentManager()->GetActiveView() != GUIProg->ViewWin))
      wxDocManager::GetDocumentManager()->SetActiveView((wxView*)GUIProg->ViewWin);
}


bool CFormWid::event(QEvent* ev)
{
  if ((ev->type() == QEvent::Enter) && GUIProg->isView)
    ((CLavaGUIView*)GUIProg->ViewWin)->MessToStatusbar();
  return QFrame::event(ev);
}


CLavaGroupBox::CLavaGroupBox(CGUIProgBase *guiPr, CHEFormNode* data, QString label, QWidget* pParentWnd)
: QGroupBox(label, pParentWnd)
{
  InitGroupBox(guiPr, data);
}

CLavaGroupBox::CLavaGroupBox(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* pParentWnd)
: QGroupBox(pParentWnd)
{
  InitGroupBox(guiPr, data);
}

void CLavaGroupBox::InitGroupBox(CGUIProgBase *guiPr, CHEFormNode* data)
{
  QWidget* par;

  hasMenu = false;
  hasFuncMenu = false;
  GUIProg = guiPr;
  myFormNode = (CHEFormNode*)data;
  show();
  par = parentWidget();
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  hasMenu = myFormNode->data.IterFlags.BITS()
      && !myFormNode->data.IterFlags.Contains(FixedCount)
      && !myFormNode->data.IoSigFlags.Contains(DONTPUT)
      && myFormNode->data.IoSigFlags.Contains(Flag_INPUT);
  if (myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
    && (myFormNode->data.FormSyntax->DeclType == PatternDef) ) {
    myFormNode->data.myHandlerNode = myFormNode->data.FIP.up->data.myHandlerNode;
    myFormNode->data.allowOwnHandler = myFormNode->data.FIP.up->data.allowOwnHandler;
  }
  if ( myFormNode->data.allowOwnHandler
    && (myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
      || myFormNode->data.BasicFlags.Contains(ButtonMenu))) {
    if (!LBaseData->inRuntime) {
      hasFuncMenu = true;
      if (myFormNode->data.FormSyntax->DeclType != PatternDef)
        myFormNode->data.myHandlerNode = myFormNode;
    }
  }
  else
    if (!myFormNode->data.allowOwnHandler
       && myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet))
      myFormNode->data.myHandlerNode = 0;

  if ( myFormNode->data.FormSyntax->SecondTFlags.Contains(isSet)
       && !myFormNode->data.handlerSearched)
    GUIProg->setHandler(myFormNode);
}

void CLavaGroupBox::mousePressEvent(QMouseEvent* ev)
{
  QAction* action;
  QMenu* myMenu = 0;
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  GUIProg->ActNode = myFormNode;
  if (hasMenu || hasFuncMenu) {
    if (hasMenu) { 
      myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->delActionPtr);//DelAction);
      myMenu->addAction(GUIProg->insActionPtr);//InsAction);
      ((CGUIProg*)GUIProg)->OnUpdateInsertopt(GUIProg->insActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateDeleteopt(GUIProg->delActionPtr);
    }
    if (!LBaseData->inRuntime && hasFuncMenu) {
      if (!myMenu)
        myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->newHandlerActionPtr);
      myMenu->addAction(GUIProg->attachHandlerActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateNewHandler(GUIProg->newHandlerActionPtr);
      ((CGUIProg*)GUIProg)->OnUpdateAttachHandler(GUIProg->attachHandlerActionPtr);
    }
    action = myMenu->exec(ev->globalPos());
    delete myMenu;
    if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
      ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
    else if (action == GUIProg->newHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnNewHandler();
    else if (action == GUIProg->attachHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnAttachHandler();
  }
  else
    if (GUIProg->isView && 
      (wxDocManager::GetDocumentManager()->GetActiveView() != GUIProg->ViewWin))
      wxDocManager::GetDocumentManager()->SetActiveView((wxView*)GUIProg->ViewWin);
}

bool CLavaGroupBox::event(QEvent* ev)
{
  if ((ev->type() == QEvent::Enter) && GUIProg->isView)
    ((CLavaGUIView*)GUIProg->ViewWin)->MessToStatusbar();
  return QGroupBox::event(ev);
}

