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


#include "FindRefsBox.h"
#include "LavaAppBase.h"
#include "LavaPE.h"
#include "Boxes.h"
#include "LavaPEDoc.h"
#include "ChString.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "q3buttongroup.h"
#include "qcheckbox.h"
#include "qradiobutton.h"


/////////////////////////////////////////////////////////////////////////////
// CGotoBox dialog


CGotoBox::CGotoBox(QWidget* pParent)
  : QDialog(pParent)
{
}

CGotoBox::CGotoBox(LavaDECL * fromDECL, CLavaPEDoc* doc, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{

  SelID.nID = -1;
  myDECL = fromDECL;
  myDoc = doc;
  bool notAll = false;
  CComboBoxItem *item;

  CHETID *cheS = (CHETID*)myDECL->Supports.first;
  LavaDECL *decl;
  TID rID = myDECL->RefID;
  if (rID.nID >= 0) {
    decl = myDoc->IDTable.GetDECL(rID);
    if (decl) {
      if (decl->DeclType == FormDef)
        decl = decl->ParentDECL;
      item = new CComboBoxItem(rID);
      m_GotoCombo->addItem(QString(decl->LocalName.c), QVariant::fromValue(item));
    }
  }
  while (cheS) {
    if (cheS->data.nID >= 0) {
      decl = myDoc->IDTable.GetDECL(cheS->data);
      if (decl) {
        item = new CComboBoxItem(cheS->data);
        if (decl->LocalName == myDECL->LocalName)
          m_GotoCombo->addItem(QString(decl->FullName.c), QVariant::fromValue(item));
        else
          m_GotoCombo->addItem(QString(decl->LocalName.c), QVariant::fromValue(item)); 
      }
      else {
        notAll = true;
      }
    }
    cheS = (CHETID*)cheS->successor;
  }
  cheS = (CHETID*)myDECL->Inherits.first;
  while (cheS) {
    if (cheS->data.nID >= 0) {
      decl = myDoc->IDTable.GetDECL(cheS->data);
      if (decl) {
        item = new CComboBoxItem(cheS->data);
        m_GotoCombo->addItem(QString(decl->LocalName.c), QVariant::fromValue(item));
      }
      else 
        notAll = true;
    }
    cheS = (CHETID*)cheS->successor;
  }

  m_GotoCombo->setCurrentItem(1);

  QString cname;
  if (SelEndOKToStr(m_GotoCombo, &cname, &SelID) > 0) 
    PushButton_IDOK12->setEnabled(true);
  if (notAll)
    IDC_STATIC_NotAllRefs->show();
  else
    IDC_STATIC_NotAllRefs->hide();
}


void CGotoBox::on_m_GotoCombo_triggered(int pos) 
{
  if (pos > 0) {
    QString cname;
    if (SelEndOKToStr(m_GotoCombo, &cname, &SelID) <= 0) {
      SelID.nID = -1;
      PushButton_IDOK12->setEnabled(false);
    }
    else
      PushButton_IDOK12->setEnabled(true);
  }
  else
    PushButton_IDOK12->setEnabled(false);
}




/////////////////////////////////////////////////////////////////////////////
// CFindRefsBox dialog


void CFindRefsBox::UpdateData(bool getData)
{
  if (getData) {
    valRefName = m_Reference->text();
    if (BGroup_Where->find(0)->isOn())
      valFindWhere = 0;
    else if (BGroup_Where->find(1)->isOn())
      valFindWhere = 1;
    else if (BGroup_Where->find(2)->isOn())
      valFindWhere = 2;
    else 
      valFindWhere = 3;
    if (BGroup_What->find(0)->isOn())
      valFindWhat = 0;
    else if (BGroup_What->find(1)->isOn())
      valFindWhat = 1;
    else if (BGroup_What->find(2)->isOn())
      valFindWhat = 2;
    else 
      valFindWhat = 3;
  }
  else {
    m_Reference->setText(valRefName);
    BGroup_Where->setButton(valFindWhere);
    BGroup_What->setButton(valFindWhat);
  }
}

CFindRefsBox::CFindRefsBox(QWidget* pParent /*=NULL*/)
	: QDialog( pParent)
{

}

CFindRefsBox::CFindRefsBox(LavaDECL* decl, bool inExec, CFindData* fw, QWidget* pParent)
	: QDialog(pParent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  myDECL = decl;
  setExec = inExec;
  findWhat = fw;

  if (myDECL) {
    if (findWhat->enumID.l)
      valRefName = QString(myDECL->FullName.c)
        + QString(" / enumeration item ") + QString(findWhat->enumID.c);
    else
      valRefName = QString(myDECL->FullName.c);
  }
  else {
    if (findWhat->enumID.l)
      valRefName = QString(findWhat->enumID.c);
    else
      valRefName = QString(fw->searchName.c);
    m_InCurrentDoc->setEnabled(false);
    m_InDocs->setEnabled(false);
    m_InIncls->setEnabled(false);
  }
  if (setExec)
    valFindWhere = 0;
  else {
    valFindWhere = 1;
    m_FindInView->setEnabled(false);
  }
  valFindWhat = 0;
  if (myDECL && (myDECL->DeclType != Attr)
      && (myDECL->DeclType != IAttr)
      && (myDECL->DeclType != OAttr)) {
    m_FindWrite->setEnabled(false);
    m_FindRead->setEnabled(false);
  }
  if (!myDECL || findWhat->enumID.l || (myDECL->DeclType != Interface) && (myDECL->DeclType != Function))
    m_FindDerivs->setEnabled(false);
  UpdateData(false);
}

void CFindRefsBox::m_FindDerivs_toggled(bool on) 
{
  if (on) {
    UpdateData(true);
    if (valFindWhere == 0)
      valFindWhere = 1;
    UpdateData(false);
  }
}

void CFindRefsBox::OnOK() 
{
  UpdateData(true);
  if (valFindWhat == 3) {
    findWhat->FindRefFlags.EXCL(readRefs);
    findWhat->FindRefFlags.EXCL(writeRefs);
    findWhat->FindRefFlags.INCL(derivRefs);
  }
  else if (valFindWhat == 2) {
    findWhat->FindRefFlags.INCL(writeRefs);
    findWhat->FindRefFlags.EXCL(readRefs);
    findWhat->FindRefFlags.EXCL(derivRefs);
  }
  else
    if (valFindWhat == 1) {
      findWhat->FindRefFlags.INCL(readRefs);
      findWhat->FindRefFlags.EXCL(writeRefs);
      findWhat->FindRefFlags.EXCL(derivRefs);
    }
    else {
      findWhat->FindRefFlags.INCL(readRefs);
      findWhat->FindRefFlags.INCL(derivRefs);
      /*if (!myDECL || (myDECL->DeclType == Attr)
          || (myDECL->DeclType == IAttr)
          || (myDECL->DeclType == OAttr))*/
        findWhat->FindRefFlags.INCL(writeRefs);
    }
  findWhat->FWhere = (TFindWhere)valFindWhere;
	QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CFindByNameBox dialog


CFindByNameBox::CFindByNameBox(QWidget* pParent)
	: QDialog(pParent)
{
}

CFindByNameBox::CFindByNameBox(CFindData* fw, QWidget* pParent)
	: QDialog(pParent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  findWhat = fw;

  CHEString* next;
  int pos = 0;
  for (next = (CHEString*)((CLavaPEApp*)wxTheApp)->FindList.first;
     next;
     next = (CHEString*)next->successor) {
    pos++;
	  m_SearchName->insertItem(QString(next->data.c), pos);
  }
  if (pos)
    m_SearchName->setCurrentItem(1);
}

void CFindByNameBox::OnOK() 
{
	if (m_SearchCase->isOn())
    findWhat->FindRefFlags.INCL(matchCase);
  else
    findWhat->FindRefFlags.EXCL(matchCase);
	if (m_SearchWholeName->isOn())
    findWhat->FindRefFlags.INCL(wholeWord);
  else
    findWhat->FindRefFlags.EXCL(wholeWord);
  findWhat->searchName = DString(m_SearchName->currentText());
  CHEString* next;
  if (findWhat->searchName.l) {
    for (next = (CHEString*)((CLavaPEApp*)wxTheApp)->FindList.first;
       next && (next->data != findWhat->searchName);
       next = (CHEString*)next->successor);
    if (next) 
      ((CLavaPEApp*)wxTheApp)->FindList.Delete(next);
    next = new CHEString;
    next->data = findWhat->searchName;
    ((CLavaPEApp*)wxTheApp)->FindList.Prepend(next);
  }
  QDialog::accept();
}
