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
#include <QCustomEvent>
#include <QFrame>
#include <QFocusEvent>
#include <QEvent>
#include <QMenu>
#include "FormWid.h"


/////////////////////////////////////////////////////////////////////////////
// CTEdit


CTEdit::~CTEdit()
{
}


CTEdit::CTEdit(CGUIProgBase *guiPr, CHEFormNode* data,
               QWidget* parentWidget, const char* name, QSize size,  bool withEcho)
:QLineEdit(parentWidget, "TEdit")
{
  myFormNode = data;
  GUIProg = guiPr;
  CaretP.setX(0);
  CaretP.setY(0);
  inError = false;
  GUIProg->SetColor(this, myFormNode);
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT)) {
      setReadOnly(true);
      if (!myFormNode->data.ColorBValid)
        setPaletteBackgroundColor(parentWidget->paletteBackgroundColor());
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
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  setContentsMargins(0,0,0,0);
  if (myFormNode->data.IterFlags.Contains(Optional)) {
    myMenu = new QMenu("Lava object", this);
    myMenu->insertItem("Delete optional", this, SLOT(DelActivated()),0, IDM_ITER_DEL);
    myMenu->insertItem("Insert optional", this, SLOT(InsActivated()),0, IDM_ITER_INSERT);
    myMenu->setItemEnabled(IDM_ITER_INSERT, false);
  }
  int bord = GUIProg->GetLineWidth(parentWidget);
  size = GUIProg->CalcTextRect(size.width(), 1, font());
//???  size.setFixedWidth(size.width());
//???  size.setFixedHeight(size.height());
  int h = minimumSizeHint().height();
  setGeometry(bord,bord, size.width(), h);//size.height());
  clearModified();
  show();
}

void CTEdit::focusInEvent(QFocusEvent *ev)
{
  GUIProg->focNode = myFormNode;
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
    myFormNode->data.StringValue = STRING(qPrintable(text()));
    if (LBaseData->inRuntime) {
      inError = !((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(myFormNode);
      if (inError)
        setCursorPosition(int(GUIProg->ErrPos));
      else
        myFormNode->data.IoSigFlags.INCL(trueValue);
      clearModified();
    }
  }
  QLineEdit::focusOutEvent(ev);
}

bool CTEdit::event(QEvent* ev)
{
  if (ev->type() == IDU_LavaGUIInsDel) {
    if (myFormNode->data.IterFlags.Contains(Optional))
      ((CGUIProg*)GUIProg)->CmdExec.DeleteOptionalItem(myFormNode);
    return true;
  }
  else 
    return QLineEdit::event(ev);
}

void CTEdit::contextMenuEvent(QContextMenuEvent * e) {
  QMenu *pm = createStandardContextMenu();
  QWidget* par = parentWidget();
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (((CFormWid*)par)->iterData && ((CFormWid*)par)->myMenu) {
    pm->insertSeparator();
    pm->addMenu(((CFormWid*)par)->myMenu);
  }
  else {
    if (myFormNode->data.IterFlags.Contains(Optional)) {
      pm->insertSeparator();
      pm->addMenu(myMenu);
    }
  }
  pm->exec(e->globalPos());
  //delete pm;
}

void CTEdit::DelActivated()
{
  QApplication::postEvent(this, new QCustomEvent(IDU_LavaGUIInsDel,(void*)IDM_ITER_DEL));
}


CMultiLineEdit::CMultiLineEdit(CGUIProgBase *guiPr, CHEFormNode* data,
          QWidget* parentWidget, const char* name, QSize size, bool withEcho)
:QTextEdit(parentWidget, "MultiLineEdit")
{
  myFormNode = data;
  GUIProg = guiPr;
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT))
    setDisabled(true);
  else  if (!myFormNode->data.IoSigFlags.Contains(Flag_INPUT) || GUIProg->FrozenObject)
    setReadOnly(true);
  else
    setEnabled(true);
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  GUIProg->SetColor(this, myFormNode);
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  if (myFormNode->data.IterFlags.Contains(Optional)) {
    myMenu = new QMenu(this);
    myMenu->insertItem("Delete optional", this, SLOT(DelActivated()),0, IDM_ITER_DEL);
    myMenu->insertItem("Insert optional", this, SLOT(InsActivated()),0, IDM_ITER_INSERT);
    myMenu->setItemEnabled(IDM_ITER_INSERT, false);
  }
  setContentsMargins(0, 0, 0, 0);
  int bord = GUIProg->GetLineWidth(parentWidget);
  size = GUIProg->CalcTextRect(size.width(), size.height(), font());
  size.setWidth(size.width()+ 2*frameWidth());
  size.setHeight(size.height()+ 2*frameWidth());
  setGeometry(bord,bord, size.width(), size.height());
  show();
  setTabChangesFocus(true);
  setModified(false);
  //connect(this, SIGNAL(textChanged()), this, SLOT(OnChange()));
}

void CMultiLineEdit::focusInEvent(QFocusEvent *ev)
{
  GUIProg->focNode = myFormNode;
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
  if (isModified()) {
    myFormNode->data.StringValue = STRING(qPrintable(text()));
    if (LBaseData->inRuntime) {
      inError = !((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(myFormNode);
      //if (inError)
      //  SetSel(int(GUIProg->ErrPos), int(GUIProg->ErrPos));
      //else
        myFormNode->data.IoSigFlags.INCL(trueValue);
//      changed = false;
        setModified(false);

    }
  }
  QTextEdit::focusOutEvent(ev);
}


bool CMultiLineEdit::event(QEvent* ev)
{
  if (ev->type() == IDU_LavaGUIInsDel) {
    if (myFormNode->data.IterFlags.Contains(Optional))
      ((CGUIProg*)GUIProg)->CmdExec.DeleteOptionalItem(myFormNode);
    return true;
  }
  else 
    return QTextEdit::event(ev);
}

QMenu* CMultiLineEdit::createStandardContextMenu()
{
  QMenu* pm = QTextEdit::createStandardContextMenu();
  QWidget* par = parentWidget();
  while (par && !par->inherits("CFormWid"))
    par = parentWidget();
  if (((CFormWid*)par)->iterData && ((CFormWid*)par)->myMenu) {
    pm->insertSeparator();
    pm->insertItem("Iteration", ((CFormWid*)par)->myMenu);
  }
  else {
    if (myFormNode->data.IterFlags.Contains(Optional)) {
      pm->insertSeparator();
      pm->insertItem("Lava object", myMenu);
    }
  }
  return pm;
}


void CMultiLineEdit::DelActivated()
{
  QApplication::postEvent(this, new QCustomEvent(IDU_LavaGUIInsDel,(void*)IDM_ITER_DEL));
}
