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


#include "SYSTEM.h"
#include "GUIres.h"
#include "TEdit.h"
#include "GUIProgBase.h"
#include "GUIProg.h"
#include "qvalidator.h"
//Added by qt3to4:
#include <QFrame>
#include <QFocusEvent>
#include <QEvent>
#include <QByteArray>
#include <QMenu>
#include "FormWid.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CTEdit


CTEdit::~CTEdit()
{
}


CTEdit::CTEdit(CGUIProgBase *guiPr, CHEFormNode* data,
               QWidget* parent, const char* name, QSize size,  bool withEcho)
:QLineEdit(parent)
{
  QWidget* par;

  myFormNode = data;
  GUIProg = guiPr;
  CaretP.setX(0);
  CaretP.setY(0);
  inError = false;
  hasMenu = false;
  hasFuncMenu = false;
  enableInsert =false;
  enableDelete =false;
  enableFunc = false;
  GUIProg->SetColor(this, myFormNode, QPalette::Base, QPalette::Text);
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT)) {
      setReadOnly(true);
      if (!myFormNode->data.ColorBValid) {
        QPalette p = palette();
        p.setColor(QPalette::Active, QPalette::Base, parent->palette().color(QPalette::Active, QPalette::Window));
        p.setColor(QPalette::Inactive, QPalette::Base, parent->palette().color(QPalette::Active, QPalette::Window));
        //setPaletteBackgroundColor(parentWidget->paletteBackgroundColor());
        setPalette(p);
      }
      setFocusPolicy(Qt::NoFocus);
  }
  else {
    if (!myFormNode->data.IoSigFlags.Contains(Flag_INPUT) || GUIProg->FrozenObject)
      setReadOnly(true);
    else
      setEnabled(true);
    if (!withEcho)
      setEchoMode(QLineEdit::Password);
  }
  if (myFormNode->data.BType == Integer)
    setValidator(new QIntValidator(INT_MIN, INT_MAX, this)); //style = style | ES_NUMBER;
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  setContentsMargins(0,0,0,0);
  par = parentWidget();
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (!LBaseData->inRuntime) {
    if (myFormNode->data.allowOwnHandler && !isReadOnly() && isEnabled()) 
      myFormNode->data.myHandlerNode = myFormNode;
    else {
      if (par)
        myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
    }
  }
  GUIProg->ActNode = myFormNode;
  if (myFormNode->data.IterFlags.Contains(Optional)
      || ((CFormWid*)par)->iterData && ((CFormWid*)par)->hasMenu) {
    hasMenu = true;
    enableInsert = ((CGUIProg*)GUIProg)->OnUpdateInsertopt(LBaseData->insActionPtr);
    enableDelete = ((CGUIProg*)GUIProg)->OnUpdateDeleteopt(LBaseData->delActionPtr);
  }
  if (!LBaseData->inRuntime) {
    hasFuncMenu = myFormNode->data.allowOwnHandler && !isReadOnly() && isEnabled()
              || ((CFormWid*)par)->hasFuncMenu;
    enableFunc = GUIProg->ActNode && GUIProg->ActNode->data.myHandlerNode;
  }
  int bord = GUIProg->GetLineWidth(parent);
  size = GUIProg->CalcTextRect(size.width(), 1, font());

  setGeometry(bord,bord, size.width(), minimumSizeHint().height());
  setModified(false);
  if (!myFormNode->data.handlerSearched && !isReadOnly() && isEnabled()) {
    GUIProg->setHandler(myFormNode);
  }
  show();
}

void CTEdit::focusInEvent(QFocusEvent *ev)
{
  GUIProg->setFocNode(myFormNode);
  GUIProg->ActNode = myFormNode;
  GUIProg->editNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  GUIProg->butNode = 0;
  GUIProg->ScrollIntoFrame(this);
  if (inError)
    setCursorPosition(int(GUIProg->ErrPos));
  QLineEdit::focusInEvent(ev);
  GUIProg->SyncTree(myFormNode);
}

void CTEdit::focusOutEvent(QFocusEvent *ev)
{
  if (isModified()) {
    if (LBaseData->inRuntime) {
      if (myFormNode->data.myHandler.first 
        && ((CGUIProg*)GUIProg)->CmdExec.EditHandlerCall(myFormNode, STRING(qPrintable(text())))) {
        myFormNode->data.IoSigFlags.INCL(trueValue);
        setText(myFormNode->data.StringValue.c);
      }
      else {
        myFormNode->data.StringValue = STRING(qPrintable(text()));
        inError = !((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(myFormNode);
        if (inError)
          setCursorPosition(int(GUIProg->ErrPos));
        else {
          myFormNode->data.IoSigFlags.INCL(trueValue);
        }
      }
      setModified(false);
    }
  }
  //GUIProg->setFocNode(0);
  QLineEdit::focusOutEvent(ev);
}

void CTEdit::contextMenuEvent(QContextMenuEvent * e) 
{
  QMenu* StdMenu = createStandardContextMenu();
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
    }
    if (!LBaseData->inRuntime && hasFuncMenu) {
      if (!myMenu)
        myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->newHandlerActionPtr);
      myMenu->addAction(GUIProg->attachHandlerActionPtr);
    }
    StdMenu->addSeparator();
    StdMenu->addMenu(myMenu);
    GUIProg->insActionPtr->setEnabled(enableInsert);
    GUIProg->delActionPtr->setEnabled(enableDelete);
    if (!LBaseData->inRuntime) {
      GUIProg->newHandlerActionPtr->setEnabled(enableFunc);
      ((CGUIProg*)GUIProg)->OnUpdateAttachHandler(GUIProg->attachHandlerActionPtr);
    }
  }
  QAction* action = StdMenu->exec(e->globalPos());
  delete StdMenu;
  if (myMenu)
    delete myMenu;
  if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
    ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
  else if (action == GUIProg->newHandlerActionPtr)
    ((CGUIProg*)GUIProg)->OnNewHandler();
  else if (action == GUIProg->attachHandlerActionPtr)
    ((CGUIProg*)GUIProg)->OnAttachHandler();
}


CMultiLineEdit::CMultiLineEdit(CGUIProgBase *guiPr, CHEFormNode* data,
          QWidget* parentWidget, const char* name, QSize size, bool withEcho)
:QTextEdit(parentWidget)
{
  QWidget* par;
  myFormNode = data;
  GUIProg = guiPr;
  hasMenu = false;
  hasFuncMenu = false;
  enableInsert =false;
  enableDelete =false;
  enableFunc = false;
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT))
    setDisabled(true);
  else  if (!myFormNode->data.IoSigFlags.Contains(Flag_INPUT) || GUIProg->FrozenObject)
    setReadOnly(true);
  else
    setEnabled(true);
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  GUIProg->SetColor(this, myFormNode, QPalette::Base, QPalette::Text);

  par = parentWidget;
  if (myFormNode->data.allowOwnHandler && !isReadOnly() && isEnabled()) 
    myFormNode->data.myHandlerNode = myFormNode;
  else {
    while (par && !par->inherits("CFormWid"))
      par = par->parentWidget();
    if (par) 
      myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
    else
      par = parentWidget;
  }
  GUIProg->ActNode = myFormNode;
  if (((CFormWid*)par)->iterData) {
    enableInsert = ((CGUIProg*)GUIProg)->OnUpdateInsertopt(LBaseData->insActionPtr);
    enableDelete = ((CGUIProg*)GUIProg)->OnUpdateDeleteopt(LBaseData->delActionPtr);
  }
  if (!LBaseData->inRuntime) 
    enableFunc = GUIProg->ActNode && GUIProg->ActNode->data.myHandlerNode;

  setContentsMargins(0, 0, 0, 0);
  int bord = GUIProg->GetLineWidth(parentWidget);
  size = GUIProg->CalcTextRect(size.width(), size.height(), font());
  size.setWidth(size.width()+ 2*frameWidth());
  size.setHeight(size.height()+ 2*frameWidth());
  setGeometry(bord,bord, size.width(), size.height());
  show();
  setTabChangesFocus(true);
  document()->setModified(false);
  if (!myFormNode->data.handlerSearched && !isReadOnly() && isEnabled()) {
    GUIProg->setHandler(myFormNode);
  }  //connect(this,SIGNAL(textChanged()), this, SLOT(OnChange()));
}

void CMultiLineEdit::focusInEvent(QFocusEvent *ev)
{
  GUIProg->setFocNode(myFormNode);
  GUIProg->ActNode = myFormNode;
  GUIProg->editNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  GUIProg->butNode = 0;
  GUIProg->ScrollIntoFrame(this);
  //if (inError)
  //  setCursorPosition(int(GUIProg->ErrPos), int(GUIProg->ErrPos));
  QTextEdit::focusInEvent(ev);
  GUIProg->SyncTree(myFormNode);
}

void CMultiLineEdit::focusOutEvent(QFocusEvent *ev)
{
  if (document()->isModified()) {
    if (LBaseData->inRuntime) {
      if (myFormNode->data.myHandler.first) {
        ((CGUIProg*)GUIProg)->CmdExec.EditHandlerCall(myFormNode, STRING(qPrintable(document()->toPlainText())));
        myFormNode->data.IoSigFlags.INCL(trueValue);
        setPlainText(myFormNode->data.StringValue.c);
      }
      else {
        myFormNode->data.StringValue = STRING(qPrintable(document()->toPlainText()));
        inError = !((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(myFormNode);
        myFormNode->data.IoSigFlags.INCL(trueValue);
        document()->setModified(false);
      }

    }
  }
  //GUIProg->setFocNode(0);
  QTextEdit::focusOutEvent(ev);
}


void CMultiLineEdit::contextMenuEvent(QContextMenuEvent * e) 
{
  QMenu* StdMenu = createStandardContextMenu();
  QMenu* myMenu = 0;
  QWidget *par= parentWidget();
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  GUIProg->ActNode = myFormNode;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (!LBaseData->inRuntime) 
    if (myFormNode->data.allowOwnHandler && !isReadOnly() && isEnabled()) {
      if (!myMenu)
        myMenu = new QMenu(menuLabel, this);
      myMenu->addAction(GUIProg->newHandlerActionPtr);
      myMenu->addAction(GUIProg->attachHandlerActionPtr);
    } 
  if (myFormNode->data.IterFlags.Contains(Optional)
      || ((CFormWid*)par)->iterData && ((CFormWid*)par)->hasMenu) {
    if (!myMenu)
      myMenu = new QMenu(menuLabel, this);
    myMenu->addAction(GUIProg->delActionPtr);//DelAction);
    myMenu->addAction(GUIProg->insActionPtr);//InsAction);
    StdMenu->addSeparator();
    StdMenu->addMenu(myMenu);
  }
  GUIProg->insActionPtr->setEnabled(enableInsert);
  GUIProg->delActionPtr->setEnabled(enableDelete);
  if (!LBaseData->inRuntime) {
    GUIProg->newHandlerActionPtr->setEnabled(enableFunc);
    ((CGUIProg*)GUIProg)->OnUpdateAttachHandler(GUIProg->attachHandlerActionPtr);
  }
  if (myMenu) {
    StdMenu->addSeparator();
    StdMenu->addMenu(myMenu);
  }
  QAction* action = StdMenu->exec(e->globalPos());
  delete StdMenu;
  if (myMenu)
    delete myMenu;
  if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
    ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
  else if (action == GUIProg->newHandlerActionPtr)
    ((CGUIProg*)GUIProg)->OnNewHandler();
  else if (action == GUIProg->attachHandlerActionPtr)
    ((CGUIProg*)GUIProg)->OnAttachHandler();
}


