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


#include "GUIProg.h"
#include "TButton.h"
#include "GUIProgBase.h"
#include "FormWid.h"
#include "CmdExec.h"
#include "qstyle.h"
//Added by qt3to4:
#include <QFrame>
#include <QFocusEvent>
#include "MACROS.h"
#include <QStyleOptionButton>

#pragma hdrstop


CToggleButton::CToggleButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, 
              const char* lpszWindowName , const QString& label, bool leftText)
:QCheckBox(label, parent)
{
  setObjectName("ToggleButton");
  DISCOButtonType = isToggle;
  myFormNode = data;
  hasMenu = false;
  hasFuncMenu = false;
  GUIProg = guiPr;
  QStyleOptionButton qsob=QStyleOptionButton();

  myFormNode->data.ownLFont = GUIProg->SetLFont(this, myFormNode);
//  if (GUIProg->Font)
//    setFont(*GUIProg->Font);
  QSize size = GUIProg->CalcStringRect(label, font());
  int bord = GUIProg->GetLineWidth(parent);
  size = qApp->style()->sizeFromContents(QStyle::CT_CheckBox, &qsob, size, this);
  //size.setWidth(size.width()+5);
  //setGeometry(bord,bord, size.width(), size.height());
  //setWFlags(GUIProg->checkBoxStyle);
  setChecked(myFormNode->data.B);
  setAutoFillBackground(true);
  if (GUIProg->FrozenObject || myFormNode->data.IoSigFlags.Contains(DONTPUT)
  || !myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  myFormNode->data.IoSigFlags.INCL(trueValue);
  GUIProg->SetColor(this, myFormNode, QPalette::Window, QPalette::WindowText);
  if (!myFormNode->data.ColorFValid) {
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::WindowText, parentWidget()->palette().color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Inactive, QPalette::WindowText, parentWidget()->palette().color(QPalette::Active, QPalette::WindowText));
    setPalette(p);
  }
    //setPaletteForegroundColor(parent->colorGroup().foreground());
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (myFormNode->data.allowOwnHandler && isEnabled()) {
    if (!LBaseData->inRuntime) {
      hasFuncMenu = true;
      myFormNode->data.myHandlerNode = myFormNode;
    }
  }
  if (!myFormNode->data.handlerSearched && isEnabled())
    GUIProg->setHandler(myFormNode);
  show();
  connect(this,SIGNAL(clicked()),SLOT(OnClicked()));
}


void CToggleButton::focusInEvent(QFocusEvent *ev) 
{
  //bool ch = isChecked();
  GUIProg->editNode = 0;
  GUIProg->butNode = 0;
  GUIProg->setFocNode(myFormNode);
  GUIProg->ActNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  GUIProg->ScrollIntoFrame(this);
  //setChecked(ch);
  //if (GUIProg->isView)
  //  ((wxView*)GUIProg->ViewWin)->focusInEvent(ev);
  QCheckBox::focusInEvent(ev);
  GUIProg->SyncTree(myFormNode);
}

void CToggleButton::focusOutEvent(QFocusEvent *ev) 
{
  //GUIProg->setFocNode(0);
  QCheckBox::focusOutEvent(ev);
}

void CToggleButton::OnClicked() 
{
  GUIProg->NoteLastModified();
  GUIProg->editNode = 0;
  GUIProg->butNode = 0;
  GUIProg->ActNode = myFormNode;
  GUIProg->setFocNode(myFormNode);
  GUIProg->CurPTR = myFormNode;
  GUIProg->SyncTree(myFormNode);

  if (LBaseData->inRuntime) {
    if (myFormNode->data.myHandler.first 
      && ((CGUIProg*)GUIProg)->CmdExec.ButtonHandlerCall(myFormNode, -1)) 
      return;
    else {
      myFormNode->data.B = isChecked();
      *(bool*)(*(myFormNode->data.ResultVarPtr)+LSH) = myFormNode->data.B;
      ((CGUIProg*)GUIProg)->OnModified();
    }
  }
  else
    myFormNode->data.B = isChecked();
  myFormNode->data.IoSigFlags.INCL(trueValue);
}

void CToggleButton::contextMenuEvent(QContextMenuEvent * e) 
{
  QMenu *myMenu = 0;
  QAction* action;
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  GUIProg->ActNode = myFormNode;
  if (hasMenu) {
    myMenu = new QMenu(menuLabel, this);
    myMenu->addAction(GUIProg->insActionPtr);
    myMenu->addAction(GUIProg->delActionPtr);
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
  if (myMenu) {
    action = myMenu->exec(e->globalPos());
    delete myMenu;
    if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
      ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
    else if (action == GUIProg->newHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnNewHandler();
    else if (action == GUIProg->attachHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnAttachHandler();
  }
}

CPushButton::CPushButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, 
          const char* lpszWindowName , const QString& label, QWidget* radioBox, unsigned num)
:QPushButton(label, parent)
{
  setObjectName(lpszWindowName);
  QStyleOptionButton qsob=QStyleOptionButton();

  myFormNode = data;
  GUIProg = guiPr;
  DISCOButtonType = isPush;
  IterNode = 0;
  hasMenu = false;
  hasFuncMenu = false;
  Radio = radioBox;
  EnumNode = (CHEFormNode*)((CFormWid*)Radio)->myFormNode->data.SubTree.first;
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  GUIProg->SetColor(this, myFormNode, QPalette::Button, QPalette::ButtonText);
  QSize size = GUIProg->CalcStringRect(label, font());
  int bord = GUIProg->GetLineWidth(parent);
  int iw1 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_ButtonMargin, &qsob,  this);
  int iw2 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &qsob, this)*2;
  setGeometry(bord,bord, size.width() + iw1 + iw2, size.height() + iw1 + iw2);
  //size = qApp->style().sizeFromContents(QStyle::CT_PushButton, this, size);
  //setGeometry(bord,bord, size.width(), size.height());
  if (GUIProg->FrozenObject || ((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(DONTPUT)
  || !((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (myFormNode->data.FIP.up->data.IterFlags.Contains(Optional)
    && myFormNode->data.allowOwnHandler  && isEnabled())
    myFormNode->data.myHandlerNode = myFormNode;
  if (!myFormNode->data.handlerSearched && isEnabled())
    GUIProg->setHandler(myFormNode);
  show();
  connect(this,SIGNAL(clicked()),SLOT(OnClicked()));
}


CPushButton::CPushButton(bool withPix, CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, 
          const char* lpszWindowName , const QString& label, QWidget* radioBox, unsigned num)
:QPushButton(*data->data.Pixmap, label, parent)
{
  setObjectName(lpszWindowName);
  QStyleOptionButton qsob=QStyleOptionButton();

  myFormNode = data;
  GUIProg = guiPr;
  DISCOButtonType = isPush;
  IterNode = 0;
  Radio = radioBox;
  hasMenu = false;
  hasFuncMenu = false;
  EnumNode = (CHEFormNode*)((CFormWid*)Radio)->myFormNode->data.SubTree.first;
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  GUIProg->SetColor(this, myFormNode, QPalette::Button, QPalette::ButtonText);
  QSize size = GUIProg->CalcStringRect(label, font());
  size.setHeight(lmax(size.height(), myFormNode->data.Pixmap->height()));
  int bord = GUIProg->GetLineWidth(parent);
  int iw1 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_ButtonMargin, &qsob, this);
  int iw2 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &qsob, this)*2;
  setGeometry(bord,bord, size.width() + iw1 + iw2 + myFormNode->data.Pixmap->width(), size.height() + iw1 + iw2);
  if (GUIProg->FrozenObject || ((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(DONTPUT)
  || !((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (myFormNode->data.FIP.up->data.IterFlags.Contains(Optional)
    && myFormNode->data.allowOwnHandler && isEnabled())
    myFormNode->data.myHandlerNode = myFormNode;
  if (!myFormNode->data.handlerSearched && isEnabled())
    GUIProg->setHandler(myFormNode);
  show();
  connect(this,SIGNAL(clicked()),SLOT(OnClicked()));
}


CPushButton::CPushButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, 
       const char* lpszWindowName, const QString& label, unsigned cmnd) 
:QPushButton(label, parent)
{
  setObjectName(lpszWindowName);
  QStyleOptionButton qsob=QStyleOptionButton();

  Cmnd = cmnd;
  Radio = 0;
  IterNode = 0;
  EnumNode = 0;
  hasMenu = false;
  hasFuncMenu = false;
  myFormNode = data;
  GUIProg = guiPr;
  if ((Cmnd == INSERT) || (Cmnd == DELETEWin)) {
    DISCOButtonType = isElli;
    if (Cmnd == DELETEWin)
      IterNode = ((CGUIProg*)GUIProg)->CmdExec.GetIterNode(myFormNode);
    else
      if (myFormNode->data.FIP.up->data.IterFlags.Contains(Optional))
        IterNode = ((CGUIProg*)GUIProg)->CmdExec.GetOptNode(myFormNode);
      else
        IterNode = ((CGUIProg*)GUIProg)->CmdExec.GetIterNode(myFormNode);
  }
  else if (Cmnd == WINPOPUP)
    DISCOButtonType = isPop;
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  GUIProg->SetColor(this, myFormNode, QPalette::Button, QPalette::ButtonText);
  int bord = GUIProg->GetLineWidth(parent);
  QSize size = GUIProg->CalcStringRect(label, font());
  int iw1 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_ButtonMargin, &qsob, this);
  int iw2 = ((wxApp*)wxTheApp)->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &qsob, this)*2;
  setGeometry(bord,bord, size.width() + iw1 + iw2, size.height() + iw1 + iw2);
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT)
      || !myFormNode->data.IoSigFlags.Contains(UnprotectedUser)
          && !myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (isElli && myFormNode->data.allowOwnHandler && isEnabled()) {
    myFormNode->data.myHandlerNode = myFormNode;
    if (!LBaseData->inRuntime)
      hasFuncMenu = true;
  }
  if (!myFormNode->data.handlerSearched && isEnabled())
    GUIProg->setHandler(myFormNode);
  connect(this,SIGNAL(clicked()),SLOT(OnClicked()));
  show();
}


void CPushButton::focusInEvent(QFocusEvent *ev) 
{
  //((CGUIProg*)GUIProg)->OnSetFocususerData;
  GUIProg->editNode = 0;
  GUIProg->ActNode = myFormNode;
  if (DISCOButtonType == isPush) {
    GUIProg->setFocNode(myFormNode);
    GUIProg->butNode = myFormNode;
  }
  else {
    GUIProg->butNode = 0;
    GUIProg->setFocNode(myFormNode);
  }
  GUIProg->ActNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  GUIProg->ScrollIntoFrame(this);
  QPushButton::focusInEvent(ev);
  GUIProg->SyncTree(myFormNode);
  //if (GUIProg->isView && (wxDocManager::GetDocumentManager()->GetActiveView() != GUIProg->ViewWin))
  //  ((wxView*)GUIProg->ViewWin)->focusInEvent(ev);
}

void CPushButton::focusOutEvent(QFocusEvent *ev) 
{
  //GUIProg->setFocNode(0);
  QPushButton::focusOutEvent(ev);
}

void CPushButton::OnClicked() 
{
//  CHEFormNode *iter;
  GUIProg->NoteLastModified();
  GUIProg->ActNode = myFormNode;
  GUIProg->editNode = 0;
  GUIProg->setFocNode(myFormNode);
  GUIProg->ActNode = myFormNode;
  if (DISCOButtonType == isPush) 
    GUIProg->butNode = myFormNode;
  else 
    GUIProg->butNode = 0;
  GUIProg->CurPTR = myFormNode;
  GUIProg->SyncTree(myFormNode);
  switch (DISCOButtonType) {
    case isPush:
      if (LBaseData->inRuntime && myFormNode->data.myHandler.first 
        && ((CGUIProg*)GUIProg)->CmdExec.ButtonHandlerCall(myFormNode, -1))
          return;
      else {
        ((CGUIProg*)GUIProg)->CmdExec.HandleButtonClick(myFormNode, EnumNode);//(CHEFormNode*)((CFormWid*)Radio)->myFormNode->data.SubTree.first);
        EnumNode->data.IoSigFlags.INCL(trueValue);
        //GUIProg->SyncTree(myFormNode);
      }
      break;

    case isElli:
      if (Cmnd == DELETEWin)
        ((CGUIProg*)GUIProg)->CmdExec.DeleteIterItem(myFormNode);
      else
        if (myFormNode->data.IterFlags.Contains(Optional))
          ((CGUIProg*)GUIProg)->CmdExec.InsertOptionalItem(myFormNode);
        else
          ((CGUIProg*)GUIProg)->CmdExec.InsertIterItem(myFormNode);
      break;

    case isPop:
      ((CGUIProg*)GUIProg)->CmdExec.HandlePopup(myFormNode);
      break;
    default: ;
  }
}

void CPushButton::contextMenuEvent(QContextMenuEvent * e) 
{
  QMenu *myMenu = 0;
  QAction* action;
  GUIProg->ActNode = myFormNode;
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  if (hasMenu) {
    myMenu = new QMenu(menuLabel, this);
    myMenu->addAction(GUIProg->insActionPtr);
    myMenu->addAction(GUIProg->delActionPtr);
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
  if (myMenu) {
    action = myMenu->exec(e->globalPos());
    delete myMenu;
    if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
      ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
    else if (action == GUIProg->newHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnNewHandler();
    else if (action == GUIProg->attachHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnAttachHandler();
  }
}

CRadioButton::CRadioButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName,
             const QString& label, QWidget* radioBox, unsigned num)
:QRadioButton(label, parent)
{
  setObjectName("RadioButton");
  QStyleOptionButton qsob=QStyleOptionButton();

  DISCOButtonType = isRadio;
  myFormNode = data;
  GUIProg = guiPr;
  Radio = radioBox;
  hasMenu = false;
  hasFuncMenu = false;
  setFocusPolicy(Qt::StrongFocus);
  setChecked(false);
  EnumNode = (CHEFormNode*)((CFormWid*)Radio)->myFormNode->data.SubTree.first;
  myFormNode->data.ownLFont = GUIProg->SetLFont(this, myFormNode);
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  QSize size = GUIProg->CalcStringRect(label, font());
  int bord = GUIProg->GetLineWidth(parent);
  size = qApp->style()->sizeFromContents(QStyle::CT_RadioButton, &qsob, size, this);
  //size.setWidth(size.width()+5);
  //setGeometry(bord,bord, size.width(), size.height());
  if (!myFormNode->data.IoSigFlags.Contains(DONTPUT)) 
    setChecked(num == EnumNode->data.D+1);
  if (GUIProg->FrozenObject || ((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(DONTPUT)
  || !((CFormWid*)Radio)->myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
//  else
//    myFormNode->data.IoSigFlags.INCL(UnprotectedUser);
  GUIProg->SetColor(this, myFormNode, QPalette::Window, QPalette::WindowText);
  setAutoFillBackground(true);
  if (!myFormNode->data.ColorFValid) {
    QPalette p = palette();
        p.setColor(QPalette::Active, QPalette::WindowText, parentWidget()->palette().color(QPalette::Active, QPalette::WindowText));
        p.setColor(QPalette::Inactive, QPalette::WindowText, parentWidget()->palette().color(QPalette::Active, QPalette::WindowText));
    setPalette(p);
  }
    //setPaletteForegroundColor(parent->colorGroup().foreground());
  QWidget* par = parent;
  while (par && !par->inherits("CFormWid"))
    par = par->parentWidget();
  if (par) {
    hasMenu = ((CFormWid*)par)->hasMenu;
    hasFuncMenu = ((CFormWid*)par)->hasFuncMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (!myFormNode->data.handlerSearched && isEnabled())
    GUIProg->setHandler(myFormNode);
  show();
  connect(this,SIGNAL(clicked()),SLOT(OnClicked()));
}


void CRadioButton::OnClicked() 
{
  GUIProg->editNode = 0;
  GUIProg->setFocNode(myFormNode);
  GUIProg->butNode = myFormNode;
  GUIProg->ActNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  int ii;
  for (ii = 0; ii < ((CFormWid*)Radio)->nRadio; ii++) {
    if (((CFormWid*)Radio)->Radio[ii] == (QPushButton*)this) {
      if (myFormNode->data.myHandler.first 
        && ((CGUIProg*)GUIProg)->CmdExec.ButtonHandlerCall(myFormNode, ii))
          return;
      else {
        setChecked(true);
        EnumNode->data.D = ii;
        EnumNode->data.StringValue = myFormNode->data.StringValue;
        if (LBaseData->inRuntime) {
          ((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(EnumNode);//(CHEFormNode*)((CFormWid*)Radio)->myFormNode->data.SubTree.first);
          EnumNode->data.IoSigFlags.INCL(trueValue);
        }
      }
    }
    else
      ((CRadioButton*)((CFormWid*)Radio)->Radio[ii])->setChecked(false);
  }
  GUIProg->SyncTree(myFormNode);
}


void CRadioButton::contextMenuEvent(QContextMenuEvent * e)
{
  QMenu *myMenu = 0;
  QAction* action;
  QString menuLabel;

  if (LBaseData->inRuntime)
    menuLabel = "Lava object";
  else
    menuLabel = "Lava";
  GUIProg->ActNode = myFormNode;
  if (hasMenu) {
    myMenu = new QMenu(menuLabel, this);
    myMenu->addAction(GUIProg->insActionPtr);
    myMenu->addAction(GUIProg->delActionPtr);
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
  if (myMenu) {
    action = myMenu->exec(e->globalPos());
    delete myMenu;
    if ((action == GUIProg->insActionPtr) || (action == GUIProg->delActionPtr))
      ((CGUIProg*)GUIProg)->ExecuteChainAction(action);
    else if (action == GUIProg->newHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnNewHandler();
    else if (action == GUIProg->attachHandlerActionPtr)
      ((CGUIProg*)GUIProg)->OnAttachHandler();
    GUIProg->setHandler(myFormNode);
  }
}

void CRadioButton::focusInEvent(QFocusEvent *ev) 
{
  //if (GUIProg->isView)
  //  ((wxView*)GUIProg->ViewWin)->focusInEvent(ev);
  if (ev->reason() != Qt::MouseFocusReason) {
    GUIProg->ActNode = myFormNode;
    GUIProg->editNode = 0;
    GUIProg->setFocNode(myFormNode);
    GUIProg->butNode = myFormNode;
    GUIProg->CurPTR = myFormNode;
    GUIProg->ScrollIntoFrame(this);
    QRadioButton::focusInEvent(ev);
    GUIProg->SyncTree(myFormNode);
  }
  else
    QRadioButton::focusInEvent(ev);
}

void CRadioButton::focusOutEvent(QFocusEvent *ev) 
{
  //GUIProg->setFocNode(0);
  QRadioButton::focusOutEvent(ev);
}
