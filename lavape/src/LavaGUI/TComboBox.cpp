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
#include "GUIProg.h"
#include "TComboBox.h"
#include "GUIProgBase.h"
#include "FormWid.h"
#include "MACROS.h"
#include "qstyle.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QFrame>
#include <QFocusEvent>
#include <QEvent>
#include <QMenu>
#include <QListView>

#pragma hdrstop


CTComboBox::CTComboBox(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* pParentWnd, char* WindowName, DString& defaultSel, unsigned width)
:QComboBox(pParentWnd)

{
  setObjectName("TComboBox");
  CHEEnumSelId *enumSel;
  myFormNode = data;
  GUIProg = guiPr;
  myMenu = 0;
  QStyleOptionComboBox qsob=QStyleOptionComboBox();


  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  setAutoFillBackground(true);
  GUIProg->SetColor(this, myFormNode, QPalette::Base, QPalette::Text);
  QSize size = GUIProg->CalcTextRect(width, 1, font());
  size = qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &qsob, size, this);
  size.setWidth(lmax(size.width(), GUIProg->MinSizeComboBox(font()).width() + GUIProg->ArrowSize(font()).width()));
  int bord = GUIProg->GetLineWidth(pParentWnd);
  setGeometry(bord,bord, size.width(), size.height());
  DefaultSel = defaultSel;
  if (myFormNode->data.EnumField) {
    for (enumSel = (CHEEnumSelId*)myFormNode->data.EnumField->Items.first;
         enumSel;
         enumSel = (CHEEnumSelId*)enumSel->successor) 
      AddItem(enumSel->data.Id.c, myFormNode);        
  }
  else
    setCurrentIndex(0);
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT)
      || !myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  ((QListView*)view())->setResizeMode(QListView::Adjust);
  connect(this,SIGNAL(activated(int)), this, SLOT(OnSelendok(int)));
  if (myFormNode->data.IterFlags.Contains(Optional)) {
    myMenu = new QMenu(this);
    myMenu->addAction(LBaseData->delActionPtr);//DelAction);
    myMenu->addAction(LBaseData->insActionPtr);//InsAction);
    LBaseData->insActionPtr->setEnabled(false);
  }
  QWidget* par = pParentWnd;
  while (par && (!par->inherits("CFormWid") 
    || (((CFormWid*)par)->myFormNode->data.FormSyntax->OwnID < 0)))
    par = par->parentWidget();
  if (par) {
    myMenu = ((CFormWid*)par)->myMenu;
    myFormNode->data.myHandlerNode = ((CFormWid*)par)->myFormNode->data.myHandlerNode;
  }
  if (myFormNode->data.allowOwnHandler) {
    if (!LBaseData->inRuntime) {
      if (!myMenu)
        myMenu = new QMenu(this);
      myMenu->addAction(LBaseData->newFuncActionPtr);
    }
    myFormNode->data.myHandlerNode = myFormNode;
  }   
  if (par && !myFormNode->data.handlerSearched)
    GUIProg->setHandler(((CFormWid*)par)->myFormNode);
  show();
}

void CTComboBox::AddItem(const DString& label, CHEFormNode* data)
{
/*  CComboItem* item = new CComboItem(label, data);
  listBox()->insertItem(item);*/
  addItem(QString(label.c),QVariant::fromValue(data));
  if (DefaultSel.l && (label == DefaultSel))
    setCurrentIndex(count()-1);

}

CTComboBox::~CTComboBox()
{
}

void CTComboBox::focusOutEvent(QFocusEvent *ev) 
{
  //GUIProg->setFocNode(0);
  QComboBox::focusOutEvent(ev);
}

void CTComboBox::mousePressEvent(QMouseEvent* ev)
{
  GUIProg->ActNode = myFormNode;
  if ((ev->button() == Qt::RightButton) && myMenu) {
    ((CGUIProg*)GUIProg)->OnUpdateInsertopt(LBaseData->insActionPtr);
    ((CGUIProg*)GUIProg)->OnUpdateDeleteopt(LBaseData->delActionPtr);
    ((CGUIProg*)GUIProg)->OnUpdateNewFunc(LBaseData->newFuncActionPtr);
    myMenu->popup(ev->globalPos());//QPoint(ev->x(), ev->y()));
  }
  else {
    QComboBox::mousePressEvent(ev);
    GUIProg->editNode = 0;
    GUIProg->butNode = 0;
    GUIProg->setFocNode(myFormNode);
    GUIProg->SyncTree(myFormNode);
  }
}

void CTComboBox::OnSelendok(int sel) 
{
  GUIProg->editNode = 0;
  GUIProg->butNode = 0;
  GUIProg->setFocNode(myFormNode);

  if (!GUIProg->FrozenObject && sel >= 0) {
    myFormNode->data.StringValue = STRING(qPrintable(currentText()));
    myFormNode->data.D = sel;
    if (LBaseData->inRuntime) {
      ((CGUIProg*)GUIProg)->CmdExec.ConvertAndStore(myFormNode);
      myFormNode->data.IoSigFlags.INCL(trueValue);
    }
  }
}

void CTComboBox::focusInEvent(QFocusEvent *ev)
{
  //((CGUIProg*)GUIProg)->OnSetFocususerData;
  GUIProg->editNode = 0;
  GUIProg->butNode = 0;
  GUIProg->setFocNode(myFormNode);
  GUIProg->ActNode = myFormNode;
  GUIProg->ScrollIntoFrame(this);
  QComboBox::focusInEvent(ev);
  GUIProg->SyncTree(myFormNode);
}

/*
bool CTComboBox::event(QEvent* ev)
{
  if (ev->type() == UEV_LavaGUIInsDel) {
    if (myFormNode->data.IterFlags.Contains(Optional))
      ((CGUIProg*)GUIProg)->CmdExec.DeleteOptionalItem(myFormNode);
    return true;
  }
  else 
    return QComboBox::event(ev);
}*/

/*
void CTComboBox::DelActivated()
{
  QApplication::postEvent(this, new CustomEvent(UEV_LavaGUIInsDel,(void*)IDM_ITER_DEL));
}*/

