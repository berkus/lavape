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


#include "LavaPE.h"
#include "Boxes.h"
#include "Convert.h"
#include "ExecTree.h"
#include "LavaBaseStringInit.h"
#include "LavaPEStringInit.h"
#include "ComboBar.h"
#include "LavaPEWizard.h"
#include "mdiframes.h"

#include "qlineedit.h"
#include "q3button.h"
#include "qradiobutton.h"
#include "qpushbutton.h"
#include "q3buttongroup.h"
#include "qcheckbox.h"
#include "qstring.h"
#include "qlabel.h"
#include "qmessagebox.h"
#include "q3filedialog.h"
#include "QtAssistant/qassistantclient.h"



/////////////////////////////////////////////////////////////////////////////
// CAttrBox dialog


CAttrBox::CAttrBox(QWidget*  parent)
  : QDialog(parent)
{
}

CAttrBox::CAttrBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  second = false;
  //OnInitDialog();
}

void CAttrBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    valNewTypeType = m_NewTypeType->text();
    if (BGroup_KindOfRef->find(0)->isOn())
      valkindOfRef = 0;
    else if (BGroup_KindOfRef->find(1)->isOn())
      valkindOfRef = 1;
    else
      valkindOfRef = 2;
    if (BGroup_Mode->find(0)->isOn())
      valkindOfField = 0;
    else if (BGroup_Mode->find(1)->isOn())
      valkindOfField = 1;
    else
      valkindOfField = 2;
  }
  else {
    m_NewName->setText(valNewName);
    m_NewTypeType->setText(valNewTypeType);
    BGroup_KindOfRef->setButton(valkindOfRef);
    BGroup_Mode->setButton(valkindOfField);
  }
}

ValOnInit CAttrBox::OnInitDialog() 
{
  CHETID *cheS;
  LavaDECL *baseDECL;
  CExecAllDefs * execAllPatt=0;
  TypeFlags = myDECL->ParentDECL->TypeFlags;
  bool catErr;

  if (myDoc->changeNothing) {
    IDOK18->setEnabled(false);
    IDOK18->setDefault( false );
    IDCANCEL18->setDefault( true );
  }
  if (onNew) {
    TypeFlags.INCL(constituent);
    myDECL->TypeFlags.INCL(constituent);
    myDECL->TypeFlags.INCL(trueObjCat);
    m_ValueObject->setChecked(true);
    myDoc->MakeBasicBox(m_BasicTypes, NoDef, true, TypeFlags.Contains(constituent));
    execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    //m_BasicTypes->SelectString(-1, ((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[(int)(VLString)]);
    m_BasicTypes->setCurrentText(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[(int)(VLString)]);
    if (SelEndOKToStr(m_BasicTypes, &valNewTypeType, &myDECL->RefID) > 0) {
      myDECL->DeclDescType = BasicType;
      myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
      valkindOfField = 0;
      valkindOfRef = 0;
    }
  }
  else {
    valNewName = QString(myDECL->LocalName.c);
    DString dstr = myDoc->GetTypeLabel(myDECL, false);
    valNewTypeType = QString(dstr.c);
    if (myDoc->GetCategoryFlags(myDECL, catErr).Contains(definesObjCat)) {
      m_StateObject->setEnabled(false);
      m_ValueObject->setEnabled(false);
      m_AnyCategory->setEnabled(false);
    }
    if (myDECL->TypeFlags.Contains(isOptional))
      valkindOfField = 1;
    else
      if (myDECL->TypeFlags.Contains(isPlaceholder))
        valkindOfField = 2;
      else
        valkindOfField = 0;
    if (myDECL->TypeFlags.Contains(constituent)) {
      TypeFlags.INCL(constituent);
      valkindOfRef = 0;
    }
    else {
      if (myDECL->TypeFlags.Contains(acquaintance))
        valkindOfRef = 1;
      else
        valkindOfRef = 2;
    }
    if (myDECL->TypeFlags.Contains(substitutable)) {
      m_Substitutable2->setChecked(true);
      TypeFlags.INCL(substitutable);
    }
    else
      m_Substitutable2->setChecked(false); 
    if (myDECL->SecondTFlags.Contains(overrides)) {
      cheS = (CHETID*)myDECL->Supports.first;
      while (cheS) {
        baseDECL = myDoc->IDTable.GetDECL(cheS->data);
        if (!baseDECL) {
          CCorrOverBox * box = new CCorrOverBox(myDECL, myDoc, parentWidget());
          if (box->OnInitDialog() == BoxContinue) {
            if (box->exec() != QDialog::Accepted) 
              return BoxCancel;  //QDialog::reject();
            return BoxOK; //QDialog::accept();
          }
          else
            return BoxCancel;
        }
        else
          if (baseDECL->TypeFlags.Contains(isStatic)) {
            if (myDoc->AutoCorrBox(&ERR_OverriddenStatic) != QDialog::Accepted) {
              return BoxCancel; //QDialog::reject();
            }
            myDECL->Supports.Destroy();
            cheS = 0;
          }
          else
            cheS = (CHETID*)cheS->successor;
      }
      if (!myDECL->Supports.first) 
        myDECL->SecondTFlags.EXCL(overrides);
    }
    if (myDECL->SecondTFlags.Contains(overrides)) {
      //CExecExtensions *exec = new CExecExtensions(myDoc, &valNamedTypes, m_BasicTypes, myDECL);
      //delete exec;
      m_RMOverrides2->setEnabled(true);
      m_NamedTypes->setEnabled(false);
      m_BasicTypes->setEnabled(false);
    }
    else {
      myDoc->MakeBasicBox(m_BasicTypes, NoDef, true, TypeFlags.Contains(constituent));
      execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    }
    /*
    if (myDoc->GetCategoryFlags(myDECL).Contains(definesObjCat)) {
      m_StateObject->setEnabled(false);
      m_ValueObject->setEnabled(false);
    }
    */
    if (myDECL->TypeFlags.Contains(trueObjCat))
      if (myDECL->TypeFlags.Contains(stateObject)) {
        m_StateObject->setChecked(true);
        m_ValueObject->setChecked(false);
        m_AnyCategory->setChecked(false);
      }
      else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
        m_StateObject->setChecked(false);
        m_ValueObject->setChecked(false);
        m_AnyCategory->setChecked(true);
      }
      else {
        m_StateObject->setChecked(false);
        m_ValueObject->setChecked(true);
        m_AnyCategory->setChecked(true);
      }
    else {
      m_StateObject->setChecked(false);
      m_ValueObject->setChecked(false);
      m_AnyCategory->setChecked(false);
    }

  }
  if (TypeFlags.Contains(isComponent)) {
    myDECL->TypeFlags.INCL(hasSetGet);
    m_SetGet->setChecked(1); 
    m_SetGet->setEnabled(false);
  }
  if (myDECL->ParentDECL->DeclType == Interface) {
    m_ConstProp->setEnabled(true);
    if (myDECL->TypeFlags.Contains(isProtected))
      m_Protected->setChecked(1);
    else
      m_Protected->setChecked(0);
    if (myDECL->TypeFlags.Contains(isConst))
      m_ConstProp->setChecked(1);
    else
      m_ConstProp->setChecked(0);
    if (myDECL->TypeFlags.Contains(copyOnAccess))
      m_PassByValue->setChecked(1);
    else
      m_PassByValue->setChecked(0);
    m_PassByValue->setEnabled(true);
    if (myDECL->TypeFlags.Contains(consumable))
      m_Consumable->setChecked(true);
    else
      m_Consumable->setChecked(false);
    if (myDECL->TypeFlags.Contains(hasSetGet)) 
      m_SetGet->setChecked(true);
    else
      m_SetGet->setChecked(false);
    m_Consumable->setEnabled(true);
    if (!myDECL->ParentDECL->TypeFlags.Contains(isComponent)
        && myDECL->TypeFlags.Contains(hasSetGet)
        && myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      if (myDECL->TypeFlags.Contains(isAbstract)) 
        m_Abstract->setChecked(true);
      else
        m_Abstract->setChecked(false);
      m_Abstract->setEnabled(true);
    }
    else {
      m_Abstract->setChecked(false);
      m_Abstract->setEnabled(false);
    }
  }
  else {
    m_ConstProp->setEnabled(false);
    m_PassByValue->setEnabled(false);
    m_Consumable->setEnabled(false);
    m_Abstract->setEnabled(false);
    m_Protected->setEnabled(false);
  }
  if (myDECL->SecondTFlags.Contains(overrides)) {
    m_Protected->setEnabled(baseDECL->TypeFlags.Contains(isProtected));
    m_StateObject->setEnabled(false);
    m_SetGet->setEnabled(false);
    m_PassByValue->setEnabled(false);
    m_Consumable->setEnabled(false);
    m_ConstProp->setEnabled(baseDECL->TypeFlags.Contains(isConst));
    m_Substitutable2->setEnabled(false);
    m_EnableName2->setEnabled(true);
    if (myDECL->SecondTFlags.Contains(enableName)) {
      m_EnableName2->setChecked(true);
      m_NewName->setEnabled(true);
    }
    else {
      m_EnableName2->setChecked(false);
      m_NewName->setEnabled(false);
    }
    m_DownC->setEnabled(false);
    m_DownInd->setEnabled(false);
    m_Back->setEnabled(false);
  }
  if (execAllPatt)
    delete execAllPatt;
  //SetSelections();  
  SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
  UpdateData(false);
  m_NewName->setFocus();
  return BoxContinue;
}



void CAttrBox::on_m_SetGet_clicked() 
{
  UpdateData(true);
  if (m_SetGet->isOn())
    m_Abstract->setEnabled(true);
  else {
    m_Abstract->setChecked(false);
    m_Abstract->setEnabled(false);
  }
  UpdateData(false);
}

void CAttrBox::on_m_RMOverrides2_clicked() 
{
  UpdateData(true);
  m_RMOverrides2->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);
  second = true;
  m_NamedTypes->setEnabled(true);
  m_BasicTypes->setEnabled(true);
  m_StateObject->setEnabled(true);
  m_SetGet->setEnabled(true);
  m_PassByValue->setEnabled(true);
  m_Consumable->setEnabled(true);
  m_ConstProp->setEnabled(true);
  m_Protected->setEnabled(true);
  m_DownC->setEnabled(true);
  m_DownInd->setEnabled(true);
  m_Back->setEnabled(true);
  OnInitDialog();
}


void CAttrBox::on_m_DownInd1_clicked() 
{
  UpdateData(true);
  if (valkindOfRef && TypeFlags.Contains(constituent)) {
    TypeFlags.EXCL(constituent);  
    ResetComboItems(m_BasicTypes);
    myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
    ResetComboItems(m_NamedTypes);
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    delete execAllPatt;
  if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
//    if (!SetSelections())
      valNewTypeType = QString("");
  }
  UpdateData(false);
}

void CAttrBox::on_m_DownC1_clicked() 
{
  UpdateData(true);
  if (!valkindOfRef) {
    TypeFlags.INCL(constituent);  
    ResetComboItems(m_BasicTypes);
    myDoc->MakeBasicBox(m_BasicTypes, NoDef, true, true);
    ResetComboItems(m_NamedTypes);
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    delete execAllPatt;
  if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
//    if (!SetSelections())
      valNewTypeType = QString("");
  }
  else 
    if (TypeFlags.Contains(constituent)) {
      TypeFlags.EXCL(constituent);  
      ResetComboItems(m_BasicTypes);
      myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
      ResetComboItems(m_NamedTypes);
      CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
      delete execAllPatt;
      if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
      //if (!SetSelections())
        valNewTypeType = QString("");
    }
  UpdateData(false);
}

void CAttrBox::on_m_NamedTypes5_triggered(int pos) 
{
  
  if (!pos) return;
  UpdateData(true);
  m_BasicTypes->setCurrentItem(0);
  bool catErr;
  if (SelEndOKToStr(m_NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    myDECL->DeclDescType = NamedType;
    SynFlags inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr); 
    m_StateObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    m_ValueObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    m_AnyCategory->setEnabled(!inheritedFlag.Contains(definesObjCat));
    if (inheritedFlag.Contains(definesObjCat))
      if (myDECL->TypeFlags.Contains(trueObjCat)) {
        if (myDECL->TypeFlags.Contains(stateObject)) {
          m_StateObject->setChecked(true);
          m_ValueObject->setChecked(false);
          m_AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
          m_StateObject->setChecked(false);
          m_ValueObject->setChecked(false);
          m_AnyCategory->setChecked(true);
        }
        else {
          m_StateObject->setChecked(false);
          m_ValueObject->setChecked(true);
          m_AnyCategory->setChecked(false);
        }
      }
      else {
        m_StateObject->setChecked(false);
        m_ValueObject->setChecked(false);
        m_AnyCategory->setChecked(false);
      }
  }
  else 
   SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
   //SetSelections();
  UpdateData(false);
}

void CAttrBox::on_m_BasicTypes5_triggered(int pos) 
{
  if (!pos) return;
  LavaDECL* decl;
  UpdateData(true);
  m_NamedTypes->setCurrentItem(0);
  if (SelEndOKToStr(m_BasicTypes, &valNewTypeType, &myDECL->RefID) > 0 ) {
    decl = myDoc->IDTable.GetDECL(myDECL->RefID);
    myDECL->DeclDescType = BasicType;
    myDECL->BType = decl->fromBType;
  }
  else 
    SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
  UpdateData(false);
}

/*
bool CAttrBox::SetSelections()
{
  m_BasicTypes->setCurrentItem(0);
  m_NamedTypes->setCurrentItem(0);
  //int cb1= CB_ERR, cb2= CB_ERR;
  if (myDECL->DeclDescType == BasicType) 
    cb1 = m_BasicTypes->setCurrentText(valNewTypeType);   
  else
    cb2 = m_NamedTypes->setCurrentText(valNewTypeType);
  return true;//(cb1 != CB_ERR) || (cb2 != CB_ERR);
}*/

void CAttrBox::on_m_Substitutable2_clicked() 
{
  UpdateData(true);
  if (m_Substitutable2->isOn())
    TypeFlags.INCL(substitutable);
  else
    TypeFlags.EXCL(substitutable);
  ResetComboItems(m_BasicTypes);
  myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
  ResetComboItems(m_NamedTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
  delete execAllPatt;
  if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
//  if (!SetSelections())
    valNewTypeType = QString("");
  UpdateData(false);
}


void CAttrBox::on_m_EnableName2_clicked() 
{
  UpdateData(true);
  if (m_EnableName2->isOn()) 
    m_NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    m_NewName->setEnabled(false);
  }
  UpdateData(false);  
}

void CAttrBox::OnOK() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc/*, OrigDECL*/);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    //m_NewName->SetCurSel(0, -1);
    m_NewName->setFocus();
    return;
  }
  if (m_Substitutable2->isOn()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (m_EnableName2->isOn()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (!valNewTypeType.length()) {
    QMessageBox::critical(this,qApp->name(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    m_BasicTypes->setFocus();
    return;
  }
  if (m_ConstProp->isOn())
    myDECL->TypeFlags.INCL(isConst);
  else
    myDECL->TypeFlags.EXCL(isConst);
  if (m_PassByValue->isOn())
    myDECL->TypeFlags.INCL(copyOnAccess);
  else
    myDECL->TypeFlags.EXCL(copyOnAccess);
  if (m_Protected->isOn())
    myDECL->TypeFlags.INCL(isProtected);
  else
    myDECL->TypeFlags.EXCL(isProtected);
  if (m_Consumable->isOn())
    myDECL->TypeFlags.INCL(consumable);
  else
    myDECL->TypeFlags.EXCL(consumable);
  if (m_StateObject->isOn())
    myDECL->TypeFlags.INCL(stateObject);
  else
    myDECL->TypeFlags.EXCL(stateObject);
  if (m_SetGet->isOn()) {
    myDECL->TypeFlags.INCL(hasSetGet);
    if (myDECL->ParentDECL && myDECL->ParentDECL->TypeFlags.Contains(isNative)
      || OrigDECL && OrigDECL->ParentDECL && OrigDECL->ParentDECL->TypeFlags.Contains(isNative))
      myDECL->TypeFlags.INCL(isNative);
  }
  else
    myDECL->TypeFlags.EXCL(hasSetGet);
  
  if (!myDECL->ParentDECL->TypeFlags.Contains(isComponent)) {
    if (m_Abstract->isOn())
      myDECL->TypeFlags.INCL(isAbstract);
    else
      myDECL->TypeFlags.EXCL(isAbstract);
    /*
    if (m_InheritsBody->isOn())
      myDECL->TypeFlags.INCL(inheritsBody);
    else
      myDECL->TypeFlags.EXCL(inheritsBody);
    */
  }
  myDECL->TypeFlags.EXCL(isOptional);
  myDECL->TypeFlags.EXCL(isPlaceholder);
  if (valkindOfField == 1) 
    myDECL->TypeFlags.INCL(isOptional);
  else 
    if (valkindOfField == 2) 
      myDECL->TypeFlags.INCL(isPlaceholder);
  myDECL->TypeFlags.EXCL(constituent);
  myDECL->TypeFlags.EXCL(acquaintance);
  if (valkindOfRef == 0) 
    myDECL->TypeFlags.INCL(constituent);
  else 
    if (valkindOfRef == 1) 
      myDECL->TypeFlags.INCL(acquaintance);
  QDialog::accept();
}



/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCompSpecBox 


CCompSpecBox::CCompSpecBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CCompSpecBox::CCompSpecBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
}

void CCompSpecBox::UpdateData(bool getData)
{
 if (getData) 
    valNewName = m_NewName->text();
  else 
    m_NewName->setText(valNewName);
}

ValOnInit CCompSpecBox::OnInitDialog() 
{
  CHEEnumSelId* enumsel;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    PushButton_IDOK13->setEnabled(false);
    PushButton_IDOK13->setDefault( false );
    m_CANCEL13->setDefault( true );
  }
  m_ExtTypes->setCurrentItem(0);
  if (!onNew) {
    valNewName = QString(myDECL->LocalName.c);
    CHETID *cheS = (CHETID*)myDECL->Supports.first;
    CHETID *ncheS;
    LavaDECL* decl;
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) {
          item = new CListBoxItem(decl->FullName, cheS->data);
          m_Extends->insertItem(item);
          cheS = (CHETID*)cheS->successor;
        }
        else {
          ncheS = (CHETID*)cheS->successor;
          myDECL->Supports.Uncouple(cheS);
          delete cheS;
          cheS = ncheS;
          if (myDoc->AutoCorrBox(&ERR_NoBaseIF) != QDialog::Accepted)  {
            QDialog::reject();
            return BoxCancel;
          }
        }
      }
      else
        cheS = (CHETID*)cheS->successor;
    }
    if (myDECL->nOutput == -1) {
      m_EnumAdd1->setEnabled(false);
      m_EnumDel1->setEnabled(false);
      m_EnumEdit1->setEnabled(false);
      m_CompoProt->setCurrentItem(0);
      //m_CompoProt.SetCurSel(-1);
    }
    else {
      m_CompoProt->setCurrentItem(myDECL->nOutput+1);
      m_EnumAdd1->setEnabled(true);
      m_EnumDel1->setEnabled(true);
      m_EnumEdit1->setEnabled(true);
      for (enumsel = (CHEEnumSelId*)myDECL->Items.first; enumsel;
      enumsel = (CHEEnumSelId*)enumsel->successor) {
        item = new CListBoxItem(enumsel->data.Id, TID(-1,-1));
        m_EnumItems1->insertItem(item);
      }
    }
    m_Persistent->setChecked(myDECL->TypeFlags.Contains(isPersistent));
  }
  m_EnumEdit1->setEnabled(false);
  m_EnumDel1->setEnabled(false);
//  m_EnumAdd->setEnabled(enabled);
  //m_EnumItems1->SetCurSel(-1);
  m_EnumItems1->setCurrentItem(0);
  SynFlags typeflag;
  CExecAllDefs * execAllPatt = new CExecAllDefs(myDoc, m_ExtTypes, 0, myDECL->ParentDECL,
                                                OrigDECL, CompObjSpec, typeflag);
  m_NewName->setFocus();
  delete execAllPatt;
  UpdateData(false);
  m_NewName->setFocus();
  
  return BoxContinue;  // return true unless you set the focus to a control
                // EXCEPTION: OCX-Eigenschaftenseiten sollten false zurückgeben
}

void CCompSpecBox::on_m_DelSupport2_clicked() 
{
  int pos = m_Extends->currentItem();
  if (pos >= 0) 
    m_Extends->removeItem(pos);
}

void CCompSpecBox::on_m_ExtTypes2_triggered(int pos) 
{
  if (!pos) return;
  SelEndOKToList(m_ExtTypes, m_Extends);
}

void CCompSpecBox::on_m_CompoProt_triggered(int pos) 
{
  if (!pos) return;
  myDECL->nOutput = m_CompoProt->currentItem() - 1;
  if (myDECL->nOutput >= 0) {
    m_EnumAdd1->setEnabled(true);
    m_EnumDel1->setEnabled(true);
    m_EnumEdit1->setEnabled(true);
    if ((myDECL->nOutput != PROT_LAVA) && (myDECL->nOutput != PROT_NATIVE))
      QMessageBox::critical(this,qApp->name(),ERR_NotYetImplemented,QMessageBox::Ok,0,0);
  }
}

/*
void CCompSpecBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //m_EnumItems1->SetCurSel(-1);
  m_EnumDel1->setEnabled(false);
  m_EnumEdit1->setEnabled(false);
  CDialog::OnLButtonDown(nFlags, point);
}*/


void CCompSpecBox::on_m_EnumAdd1_clicked() 
{
  QString iT, fileName, dir;
  DString linkName, dstrDir;
  int dd=0, ss;
  CListBoxItem *item;

  UpdateData(true);
  ss = m_EnumItems1->currentItem();
  if (ss >= 0)
    ss = 0;
  else
    ss += 1;
  if (myDECL->nOutput == PROT_LAVA) {
    if (ss > 1)
      QMessageBox::critical(this,qApp->name(),ERR_ExactlyOneLcom,QMessageBox::Ok,0,0);
    else {
      dir = ExeDir + ComponentLinkDir;
      fileName = L_GetOpenFileName(
                    dir,
                    this,
                    IDS_LAVACOM_FILE,
                    "Lava.Component (*.lcom)",
                    "lcom"
                    ); 
      fileName.remove(0,dir.length());
#ifdef WIN32
      fileName.truncate(fileName.length()-4);
#endif
      m_EnumItems1->insertItem(fileName, 0);
      m_EnumItems1->setSelected(0, true);
      m_EnumDel1->setEnabled(true);
      m_EnumEdit1->setEnabled(true);
    }
  }
  else if (myDECL->nOutput == PROT_NATIVE) {
    if (ss > 1)
      QMessageBox::critical(this,qApp->name(),ERR_OneLibName,QMessageBox::Ok,0,0);
    else {
      CEnumItem *cm = new CEnumItem(&iT, 0, 0, false, this);
      if (cm->exec() == QDialog::Accepted) {
        item = new CListBoxItem(iT, TID(-1,-1));
        m_EnumItems1->insertItem(item, 0);
        m_EnumItems1->setSelected(0, true);
        m_EnumDel1->setEnabled(true);
        m_EnumEdit1->setEnabled(true);
      } 
      delete cm;
   }
  }
  else {
    CEnumItem *cm = new CEnumItem(&iT, 0, 0, false, this);
    if (cm->exec() == QDialog::Accepted) {
      item = new CListBoxItem(iT, TID(-1,-1));
      m_EnumItems1->insertItem(item, ss);
      m_EnumItems1->setSelected(ss, true);
      m_EnumDel1->setEnabled(true);
      m_EnumEdit1->setEnabled(true);
    } 
    delete cm;
  }
  UpdateData(false);
}

void CCompSpecBox::on_m_EnumDel1_clicked() 
{
  int ss = m_EnumItems1->currentItem();
  if (ss >= 0) {
    m_EnumItems1->removeItem(ss);
    m_EnumDel1->setEnabled(false);
    m_EnumEdit1->setEnabled(false);
  } 
}

void CCompSpecBox::on_m_EnumEdit1_clicked() 
{
  QString iT, dir, fileName;
  DString linkName, dstrDir;
  int dd=0, ss;
  CListBoxItem *item;

  ss = m_EnumItems1->currentItem();
  if (ss >= 0) {
    item = (CListBoxItem*)m_EnumItems1->item(ss);
    if (myDECL->nOutput == PROT_LAVA) {
      dir = ExeDir + ComponentLinkDir;
      fileName = dir + item->text();
#ifdef WIN32
      fileName += ".lnk";
#endif
      fileName = L_GetOpenFileName(
                    fileName,
                    this,
                    IDS_LAVACOM_FILE,
                    "Lava.Component (*.lcom)",
                    "lcom"
                    ); 
      fileName.remove(0,dir.length());
#ifdef WIN32
      fileName.truncate(fileName.length()-4);
#endif
      item->setText(fileName);
      m_EnumItems1->setSelected(0, true);
    }
    else if (myDECL->nOutput == PROT_NATIVE) {
      iT = item->text();
      CEnumItem *cm = new CEnumItem(&iT, m_EnumItems1, 0, false, this);
      if (cm->exec() == QDialog::Accepted) {
        item->setText(iT);
        m_EnumItems1->setSelected(0, true);
      } 
      delete cm;
    }
    else {
      iT = item->text();
      CEnumItem* cm = new CEnumItem(&iT, m_EnumItems1, 0, false, this);//parentWidget());   
      if (cm->exec() == QDialog::Accepted) {
        m_EnumItems1->removeItem(ss);
        item = new CListBoxItem(iT, TID(-1,-1));
        m_EnumItems1->insertItem(item, ss);
      }
      delete cm;
    }
  }
}


void CCompSpecBox::m_EnumItems1_selectionChanged(Q3ListBoxItem* selItem) 
{
  int ss = m_EnumItems1->currentItem();
  SetButtons(ss);
}

void CCompSpecBox::SetButtons(int sel)
{
  if (sel < 0) {
    m_EnumDel1->setEnabled(false);
    m_EnumEdit1->setEnabled(false);
  }
  else {
    m_EnumDel1->setEnabled(true);
    m_EnumEdit1->setEnabled(true);
  }
}


void CCompSpecBox::OnOK() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    //m_NewName->SetCurSel(0, -1);
    m_NewName->setFocus();
    return;
  }
  ListToChain(m_Extends, &myDECL->Supports);
  if (!myDECL->Supports.first) {
    QMessageBox::critical(this,qApp->name(),IDP_SelInterface,QMessageBox::Ok,0,0);
    m_ExtTypes->setFocus();
    return;
  }
  myDECL->Items.Destroy();
  int ipos = 0;
  int maxi = m_EnumItems1->count();
  if (m_Persistent->isOn())
    myDECL->TypeFlags.INCL(isPersistent);
  else
    myDECL->TypeFlags.EXCL(isPersistent);
  if (myDECL->nOutput < 0) {
    QMessageBox::critical(this,qApp->name(),IDP_NoCompoProt,QMessageBox::Ok,0,0);
    m_CompoProt->setFocus();
    return;
  }

  if (myDECL->nOutput == PROT_LAVA) {
    if (m_EnumItems1->count() > 1) {
      QMessageBox::critical(this,qApp->name(),ERR_ExactlyOneLcom,QMessageBox::Ok,0,0);
      m_EnumItems1->setFocus();
      return;
    }
  }
  else if (myDECL->nOutput == PROT_NATIVE) {
    if (m_EnumItems1->count() > 1) {
      QMessageBox::critical(this,qApp->name(),ERR_OneLibName,QMessageBox::Ok,0,0);
      m_EnumItems1->setFocus();
      return;
    }
    myDECL->TypeFlags.INCL(isNative);
  }
  CHEEnumSelId * enumsel;
  while (ipos < maxi) {
    enumsel =  new CHEEnumSelId;
    CListBoxItem* it = (CListBoxItem*)m_EnumItems1->item(ipos);
    enumsel->data.Id = STRING(qPrintable(it->text()));
    myDECL->Items.Append(enumsel);
    ipos++;
  }//while  
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CCorrOverBox dialog


CCorrOverBox::CCorrOverBox(QWidget* parent )
 : QDialog(parent)
{
}

CCorrOverBox::CCorrOverBox(LavaDECL* decl, CLavaPEDoc* doc, QWidget* parent)
 : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
}

ValOnInit CCorrOverBox::OnInitDialog() 
{
  CHETVElem* che;
  LavaDECL *vtDecl;
  CComboBoxItem* item;

  myDoc->MakeVElems(myDECL->ParentDECL);
  //the reference that must be corrected
  for (CheTID = (CHETID*)myDECL->Supports.first;
      CheTID && myDoc->IDTable.GetDECL(CheTID->data, myDECL->inINCL);
      CheTID = (CHETID*)CheTID->successor);
  if (!CheTID)
    return BoxCancel; //QDialog::reject();
  for (che = (CHETVElem*)myDECL->ParentDECL->VElems.VElems.first;
       che;
       che = (CHETVElem*)che->successor) {
    vtDecl = myDoc->IDTable.GetDECL(che->data.VTEl);
    if (vtDecl->ParentDECL != myDECL->ParentDECL)
      if (PossibleOver(vtDecl)) {
        item = new CComboBoxItem(TID(vtDecl->OwnID, vtDecl->inINCL));
        m_PossibleOvers->addItem(QString(vtDecl->FullName.c),QVariant::fromValue(item));
      }
  }
  if (m_PossibleOvers->count() > 1) {
    m_NewOver->setEnabled(true);
  }
  else {
    m_PossibleOvers->setEnabled(false);
    m_NewOver->setEnabled(false);
  }
  SortCombo(m_PossibleOvers);
  m_PossibleOvers->setCurrentItem(0);
  return BoxContinue;
}

bool CCorrOverBox::PossibleOver(LavaDECL* vtDecl)
{
  CHE* myChe, *vtChe;
  CHETID *ioCheTID;
  TID oldID, oldIDIO;
  bool match;
  SynFlags typeFlags;
  bool catErr;
  
  if (vtDecl->DeclType != myDECL->DeclType)
    return false;
  oldID = CheTID->data;
  CheTID->data = TID(vtDecl->OwnID, vtDecl->inINCL);
  if ((myDECL->DeclType == VirtualType)
    || (myDECL->DeclType == Attr))  {
    match = myDoc->OverriddenMatch(myDECL);
    if (match) {
      typeFlags = myDECL->TypeFlags;
      myDoc->GetCategoryFlags(myDECL, catErr);
      CheTID->data = oldID;
      if (!catErr && (typeFlags == myDECL->TypeFlags)) 
        return true;
      else {
        myDECL->TypeFlags = typeFlags;
        return false;
      }
    }
    return false;
  }
  else { //function
    if ((vtDecl->TypeFlags.Contains(isConst) != myDECL->TypeFlags.Contains(isConst)) 
        || !vtDecl->TypeFlags.Contains(isAbstract) && myDECL->TypeFlags.Contains(isAbstract)
        || !vtDecl->TypeFlags.Contains(isProtected) && myDECL->TypeFlags.Contains(isProtected)) {
      CheTID->data = oldID;
      return false;
    }
    myChe = (CHE*)myDECL->NestedDecls.first;
    vtChe = (CHE*)vtDecl->NestedDecls.first;
    while (myChe && vtChe) {
      for (ioCheTID = (CHETID*)((LavaDECL*)myChe->data)->Supports.first;
           ioCheTID && myDoc->IDTable.GetDECL(ioCheTID->data, myDECL->inINCL);
           ioCheTID = (CHETID*)ioCheTID->successor);
      if (!ioCheTID)
        return false;
      else {
        oldIDIO = ioCheTID->data;
        ioCheTID->data = TID(((LavaDECL*)vtChe->data)->OwnID, ((LavaDECL*)vtChe->data)->inINCL);
        match = myDoc->OverriddenMatch((LavaDECL*)myChe->data);
        ioCheTID->data = oldIDIO;
        if (!match) {
          CheTID->data = oldID;
          return false;
        }
        else
          ioCheTID->data = oldIDIO;
      }
      myChe = (CHE*)myChe->successor;
      vtChe = (CHE*)vtChe->successor;
    }
  }
  CheTID->data = oldID;
  return !myChe && !vtChe;
}

void CCorrOverBox::on_m_NewOver_clicked() 
{
  m_PossibleOvers->setEnabled(true);
}

void CCorrOverBox::on_m_NoOver_clicked() 
{
  m_PossibleOvers->setEnabled(false);
}

void CCorrOverBox::OnOK() 
{
  int pos;
  QVariant var;
  //TID newID;
  //CListBoxItem *item;
  
  if (m_NewOver->isOn()) {
    pos = m_PossibleOvers->currentItem();
    //item = (CComboBoxItem*)m_PossibleOvers->listBox()->item(pos);
    var = m_PossibleOvers->itemData(pos);
    CheTID->data = var.value<CComboBoxItem*>()->itemData();
  }
  else {
    myDECL->Supports.Delete(CheTID);
    if (!myDECL->Supports.first)
      myDECL->SecondTFlags.EXCL(overrides);
  }
	QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CEnumBox

CEnumBox::CEnumBox(QWidget* parent)
  : QDialog( parent)
{
}

CEnumBox::CEnumBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog( parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  valBuildSet = false;
}

CEnumBox::~CEnumBox()
{
}

void CEnumBox::UpdateData(bool getData)
{
  if (getData) {
    valItemNr = m_EnumItems->currentItem();
    valNewName = m_NewName->text();
    valBuildSet = m_BuildSet->isOn();
  }
  else {
    m_EnumItems->setSelected(valItemNr, true);
    m_NewName->setText(valNewName);
    m_BuildSet->setChecked(valBuildSet);
  }
}

ValOnInit CEnumBox::OnInitDialog() 
{
  CHEEnumSelId* enumsel;
  CHETID* cheID;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    PushButton_IDOK5->setEnabled(false);
    PushButton_IDOK5->setDefault( false );
    m_CANCEL5->setDefault( true );
  }
  if (onNew) {
    myDECL->DeclDescType = EnumType;
    cheID = new CHETID;
    cheID->data.nID = myDoc->IDTable.BasicTypesID[Enumeration];
    if (myDoc->isStd)
      cheID->data.nINCL = 0;
    else
      cheID->data.nINCL = 1;
    myDECL->Supports.Append(cheID);
    myDECL->SecondTFlags.INCL(isEnum);
    if (!myDECL->EnumDesc.ptr)
      myDECL->EnumDesc.ptr = new TEnumDescription;
    ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.DeclDescType = BasicType;
    ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.BType = Enumeration;
    ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.RefID = cheID->data;
  }
  else {
    valNewName = QString(myDECL->LocalName.c);
    for (enumsel = (CHEEnumSelId*)((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.Items.first;
    enumsel; enumsel = (CHEEnumSelId*)enumsel->successor) {
      item = new CListBoxItem(enumsel->data.Id, TID(-1,-1));
      m_EnumItems->insertItem(item);
    }
    m_BuildSet->setEnabled(false);
  }
  m_EnumEdit->setEnabled(false);
  m_EnumDel->setEnabled(false);
//  m_EnumAdd->setEnabled(enabled);
  //m_EnumItems->setCurrentItem(0);
  UpdateData(false);
  m_NewName->setFocus();
  return BoxContinue;
}

/////////////////////////////////////////////////////////////////////////////
// CEnumBox message handlers

/*
void CEnumBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
  m_Enum->setCurrentItem(0);
  m_EnumDel->setEnabled(false);
  m_EnumEdit->setEnabled(false);
  CDialog::OnLButtonDown(nFlags, point);
}
*/

void CEnumBox::on_m_EnumAdd_clicked() 
{
  CListBoxItem *item;
  int ss = m_EnumItems->currentItem();
  if (ss < 0)
    ss = 0;
  else
    ss += 1;
  QString iT("");
  CEnumItem *cm = new CEnumItem(&iT, m_EnumItems, 0, true, this);//parentWidget());
  if (cm->exec() == QDialog::Accepted) {
    item = new CListBoxItem(iT, TID(-1,-1));
    m_EnumItems->insertItem(item, ss);
    m_EnumItems->setSelected(ss, true);
    m_EnumDel->setEnabled(true);
    m_EnumEdit->setEnabled(true);
  } 
  delete cm;
}

void CEnumBox::on_m_EnumDel_clicked() 
{
  int ss = m_EnumItems->currentItem();
  if (ss >= 0) {
    m_EnumItems->removeItem(ss);
    m_EnumDel->setEnabled(false);
    m_EnumEdit->setEnabled(false);
  } 
}

void CEnumBox::on_m_EnumEdit_clicked() 
{
  CListBoxItem *item;
  QString txt;
  int ss = m_EnumItems->currentItem();
  if (ss >= 0) {
    item = (CListBoxItem*)m_EnumItems->item(ss);
    txt = item->text();
    CEnumItem* cm = new CEnumItem(&txt, m_EnumItems, 0, true, this );//parentWidget());   
    if (cm->exec() == QDialog::Accepted) {
      m_EnumItems->removeItem(ss);
      item = new CListBoxItem(txt, TID(-1,-1));
      m_EnumItems->insertItem(item, ss);
    }
    delete cm;
  }
}


void CEnumBox::m_EnumItems_selectionChanged(Q3ListBoxItem *selItem) 
{
  int ss = m_EnumItems->currentItem();
  SetButtons(ss);
}

void CEnumBox::SetButtons(int sel)
{
  if (sel < 0) {
    m_EnumDel->setEnabled(false);
    m_EnumEdit->setEnabled(false);
  }
  else {
    m_EnumDel->setEnabled(true);
    m_EnumEdit->setEnabled(true);
  }
}


void CEnumBox::OnOK() 
{
  CListBoxItem *item;
  QString str;

  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    m_NewName->setCursorPosition(0);
    m_NewName->setFocus();
    return;
  }
  LavaDECL *inEl = &((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField;
  inEl->Items.Destroy();
  int ipos = 0;
  int maxi = m_EnumItems->count();
  if (!maxi) {
    QMessageBox::critical(this,qApp->name(),IDP_NoEnumItem,QMessageBox::Ok,0,0);
    m_EnumAdd->setFocus();
    return;
  }
  CHEEnumSelId * enumsel;
  while (ipos < maxi) {
    enumsel =  new CHEEnumSelId;
    item = (CListBoxItem*)m_EnumItems->item(ipos);
    str = item->text();
    enumsel->data.Id = STRING(qPrintable(str));
    inEl->Items.Append(enumsel);
    ipos = ipos+1;
  }//while
  myDoc->MakeIniFunc(myDECL);

  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CEnumItem dialog

// This box is also used for component IDs
CEnumItem::CEnumItem(QWidget* parent)
  : QDialog(parent)
{
}

CEnumItem::CEnumItem(QString *enumItem, Q3ListBox* itemsBox, ChainAny0* items, bool isId, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  m_ItemAdr = enumItem;
  StartText = *enumItem;
  m_EnumItem->setText(StartText);
  ItemsBox = itemsBox; //ItemsBox and Items != 0: Enum item, else Component object ID
  Items = items;
  isID = isId;
  if (!ItemsBox && !Items)
    setCaption("Component object ID");
}

void CEnumItem::OnOK() 
{
  int ipos, maxi;
  CHEEnumSelId *che;
  DString str;
  QString txt, newText;
  CListBoxItem *item;

  if ((Items || ItemsBox) && isID && !((CLavaPEApp*)wxTheApp)->LBaseData.isIdentifier(m_EnumItem->text()))
    QMessageBox::critical(this,qApp->name(),IDP_IsNoID,QMessageBox::Ok,0,0);
  else {
    if (ItemsBox) {
      maxi = ItemsBox->count();
      newText = m_EnumItem->text();
      if (newText != StartText) {
        for (ipos = 0; (ipos < maxi) && (newText != txt); ipos++) {
          item = (CListBoxItem*)ItemsBox->item(ipos);
          txt = item->text();
        }
        if (newText == txt) 
          QMessageBox::critical(this,qApp->name(),ERR_NameInUse,QMessageBox::Ok,0,0);
        else {
          *m_ItemAdr = m_EnumItem->text();
          QDialog::accept();
        }
      }
      else {
        *m_ItemAdr = m_EnumItem->text();
        QDialog::accept();
      }
    }
    else {
      if (Items) {
        str = qPrintable(m_EnumItem->text());
        for (che = (CHEEnumSelId*)Items->first; che && (che->data.Id != str); che = (CHEEnumSelId*)che->successor);
        if (che)
          QMessageBox::critical(this,qApp->name(),ERR_NameInUse,QMessageBox::Ok,0,0);
        else {
          *m_ItemAdr = m_EnumItem->text();
          QDialog::accept();
        }
      }
      else {
        *m_ItemAdr = m_EnumItem->text();
        QDialog::accept();
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFuncBox 


CFuncBox::CFuncBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CFuncBox::CFuncBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  second = false;
  setFont(LBaseData->m_TreeFont);
}

void CFuncBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    if (BGroup_ExecMode->find(0)->isOn())
      valSynch = 0;
    else if (BGroup_ExecMode->find(1)->isOn())
      valSynch = 1;
    else
      valSynch = 2;
  }
  else {
    m_NewName->setText(valNewName);
    BGroup_ExecMode->setButton(valSynch);
  }
}

ValOnInit CFuncBox::OnInitDialog()
{
  CHE* cheIO;
  SynFlags typeflag;
  LavaDECL *decl,  *baseDECL;
  CHETID *ncheS, *cheS;
  CListBoxItem *listItem;

  if (myDoc->changeNothing) {
    PushButton_IDOK11->setEnabled(false);
    PushButton_IDOK11->setDefault( false );
    m_CANCEL11->setDefault( true );
  }

  if (!second) {
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes1, 0, myDECL->ParentDECL, OrigDECL, Function, typeflag);
    delete execAllPatt;
  }
  if ((myDECL->ParentDECL->DeclType == Impl)
    || myDECL->TypeFlags.Contains(isGUI) ) {
    //|| myDECL->ParentDECL->TypeFlags.Contains(isComponent) ) {
    m_Native1->setEnabled(false);
    m_Abstract->setEnabled(false);
    m_Initializer->setEnabled(false);
    if (myDECL->ParentDECL->DeclType == Impl)
      m_Protected->setEnabled(false);
  }
  else {
    if (!myDECL->ParentDECL->TypeFlags.Contains(isAbstract) )
      m_Abstract->setEnabled(false);
    if (!myDECL->ParentDECL->TypeFlags.Contains(isNative))
      m_Native1->setEnabled(false);
  }
  if (onNew) {
    hasParams = 0;
    valSynch = 0;
    if (myDECL->ParentDECL->TypeFlags.Contains(isNative)) {
      myDECL->TypeFlags.INCL(isNative);
      m_Native1->setChecked(true);
    }
    myDECL->TypeFlags.INCL(isConst);
    m_ConstFunc->setChecked(true);
    /*
    else
      if (myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
        myDECL->TypeFlags.INCL(isAbstract);
        m_Abstract->setChecked(true);
      }
    */
    if ((myDECL->ParentDECL->DeclType == Interface)
         || (myDECL->ParentDECL->DeclType == Impl) ) {
      m_CHECKOp->setEnabled(true);
      CalcOpBox();
    }
//    m_InheritsBody->setEnabled(false);
  }  
  else {
    valNewName = QString(myDECL->LocalName.c);
    cheIO = (CHE*)myDECL->NestedDecls.first;
    hasParams = cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc);
    hasOutput = false;
    while (cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
      hasOutput = hasOutput || (((LavaDECL*)cheIO->data)->DeclType == OAttr);
      if (((LavaDECL*)cheIO->data)->TypeFlags.Contains(sameAsSelf)) {
        m_StaticFunc->setEnabled(false);
        cheIO = 0;
      }
      else
        cheIO = (CHE*)cheIO->successor;
    }
    hasOutput = hasOutput || myDECL->Inherits.first;

    if (myDECL->SecondTFlags.Contains(isLavaSignal)) {
      m_Signal->setChecked(true);
      m_StaticFunc->setEnabled(false);
      m_Protected->setEnabled(false);
      m_Initializer->setEnabled(false);
      m_Abstract->setEnabled(false);
      m_EnforceOver->setEnabled(false);
      m_ConstFunc->setEnabled(false);
    }
    else
      m_Signal->setChecked(false);
    if (!myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) 
      myDECL->TypeFlags.EXCL(isAbstract);
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      m_Abstract->setChecked(true);
      m_StaticFunc->setEnabled(false);
      m_StaticFunc->setChecked(false);
    }
    else
      m_Abstract->setChecked(false);
    if (myDECL->TypeFlags.Contains(isStatic)) {
      m_StaticFunc->setChecked(true);
      m_Protected->setEnabled(false);
      m_Initializer->setEnabled(false);
      m_Abstract->setEnabled(false);
      m_EnforceOver->setEnabled(false);
      m_ConstFunc->setEnabled(false);
    }
    else
      m_StaticFunc->setChecked(false);
    if (myDECL->TypeFlags.Contains(isProtected)) {
      m_Protected->setChecked(true);
      m_StaticFunc->setEnabled(false);
      m_StaticFunc->setChecked(false);
    }
    else
      m_Protected->setChecked(false);
    if (myDECL->TypeFlags.Contains(forceOverride)) {
      m_EnforceOver->setChecked(true);
      m_StaticFunc->setEnabled(false);
      m_StaticFunc->setChecked(false);
    }
    else
      m_EnforceOver->setChecked(false);
    if (myDECL->TypeFlags.Contains(isConst)) 
      m_ConstFunc->setChecked(true);
    else
      m_ConstFunc->setChecked(false);
    if (myDECL->TypeFlags.Contains(isInitializer)) {
      m_ConstFunc->setEnabled(true); 
      m_Protected->setEnabled(false);
      m_Initializer->setChecked(true);
      m_DefaultIni->setEnabled(!hasParams
                                && (!myDECL->ParentDECL->WorkFlags.Contains(hasDefaultIni)
                                    || myDECL->TypeFlags.Contains(defaultInitializer)));
      if (myDECL->TypeFlags.Contains(defaultInitializer)) 
        m_DefaultIni->setChecked(true);
      if (myDECL->TypeFlags.Contains(isGUI)) 
        m_Initializer->setEnabled(false);
      myDECL->TypeFlags.EXCL(isAbstract);
      m_Abstract->setChecked(false);
      m_Abstract->setEnabled(false);
      m_StaticFunc->setEnabled(false);
      m_StaticFunc->setChecked(false);
    }
    else
      m_Initializer->setChecked(false);
    if (myDECL->TypeFlags.Contains(isNative)) 
      m_Native1->setChecked(true);
    else
      m_Native1->setChecked(false);
    /*
    if (myDECL->TypeFlags.Contains(isTransaction)) 
      m_Transaction->setChecked(true);
    else
      m_Transaction->setChecked(false);
    */
    if (myDECL->TypeFlags.Contains(execIndependent)) 
      valSynch = 2;
    else
      if (myDECL->TypeFlags.Contains(execConcurrent)) 
        valSynch = 1;
      else
        valSynch = 0;

    if (hasOutput) {
      m_Signal->setEnabled(false);
      m_Independent->setEnabled(false);
    }
    CalcOpBox();
    if (myDECL->op != OP_noOp) {
      m_CHECKOp->setChecked(true);
      m_NewName->setEnabled(false);
    }
    cheS = (CHETID*)myDECL->Inherits.first;
    while (cheS) {
      if (myDoc->AllowThrowType(myDECL, cheS->data, 0)) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl && decl->SecondTFlags.Contains(isException)) {
          listItem = new CListBoxItem(decl->LocalName, cheS->data);
          m_Inherits1->insertItem(listItem);
          cheS = (CHETID*)cheS->successor;
        }
        else {
          ncheS = (CHETID*)cheS->successor;
          myDECL->Inherits.Uncouple(cheS);
          delete cheS;
          cheS = ncheS;
          if (myDoc->AutoCorrBox(&ERR_NoFiredIF) != QDialog::Accepted)  {
            return BoxCancel; //QDialog::reject();
          }
        }
      }
      else {
        ncheS = (CHETID*)cheS->successor;
        myDECL->Inherits.Uncouple(cheS);
        delete cheS;
        cheS = ncheS;
        if (myDoc->AutoCorrBox(&ERR_OverThrow)  != QDialog::Accepted) {
          return BoxCancel; //QDialog::reject();
        }
      }
    }
    if (myDECL->SecondTFlags.Contains(overrides)) {
      cheS = (CHETID*)myDECL->Supports.first;
      while (cheS) {
        baseDECL = myDoc->IDTable.GetDECL(cheS->data);
        if (!baseDECL) {
          CCorrOverBox * box = new CCorrOverBox(myDECL, myDoc, parentWidget());
          if (box->OnInitDialog() == BoxContinue) {
            if (box->exec() != QDialog::Accepted) 
              return BoxCancel;  
            return BoxOK; 
          }
          else
            return BoxCancel;
        }
        else
          if (baseDECL->TypeFlags.Contains(isStatic)) {
            if (myDoc->AutoCorrBox(&ERR_OverriddenStatic) != QDialog::Accepted) {
              return BoxCancel; //QDialog::reject();
            }
            myDECL->Supports.Destroy();
            cheS = 0;
          }
          else
            cheS = (CHETID*)cheS->successor;
      }
      if (!myDECL->Supports.first) 
        myDECL->SecondTFlags.EXCL(overrides);
    }
    if (myDECL->SecondTFlags.Contains(overrides)) {
      baseDECL = myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data);
      m_ConstFunc->setEnabled(!baseDECL->TypeFlags.Contains(isConst)); 
      m_Protected->setEnabled(baseDECL->TypeFlags.Contains(isProtected));
      m_StaticFunc->setEnabled(false);
      m_RMOverrides->setEnabled(true);
      m_EnableName->setEnabled(true);
      if (myDECL->SecondTFlags.Contains(enableName)) {
        m_EnableName->setChecked(true);
        m_NewName->setEnabled(true);
      }
      else {
        m_EnableName->setChecked(false);
        m_NewName->setEnabled(false);
      }
    }
    if (myDECL->SecondTFlags.Contains(funcImpl)) {
      decl = myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data);
      if (!decl ) {
        myDECL->SecondTFlags.EXCL(funcImpl);
        myDECL->TypeFlags.EXCL(isProtected);
        myDECL->TypeFlags.EXCL(isPropGet);
        myDECL->TypeFlags.EXCL(isPropSet);
        myDECL->Supports.Destroy();
        if (myDoc->AutoCorrBox(&ERR_MissingItfFuncDecl) != QDialog::Accepted) {
          OrigDECL->WorkFlags.INCL(allowDEL);
          return BoxCancel; //QDialog::reject();
        }
      }
      else 
        if ((decl->DeclType == Attr) && !decl->TypeFlags.Contains(hasSetGet)) {
          myDECL->SecondTFlags.EXCL(funcImpl);
          myDECL->TypeFlags.EXCL(isPropGet);
          myDECL->TypeFlags.EXCL(isPropSet);
          myDECL->Supports.Destroy();
          if (myDoc->AutoCorrBox(&ERR_NoSetGetMember)  != QDialog::Accepted) {
            OrigDECL->WorkFlags.INCL(allowDEL);
            return BoxCancel; //QDialog::reject();
          }
        }
    }
    if (myDECL->SecondTFlags.Contains(funcImpl)
          || myDECL->TypeFlags.Contains(isGUI)) {
      m_ConstFunc->setEnabled(false);
      m_NewName->setEnabled(false);
      m_Signal->setEnabled(false);
      //m_Transaction->setEnabled(false);
      m_Protected->setEnabled(false);
      m_EnforceOver->setEnabled(false);
      m_DefaultIni->setEnabled(false);
      m_DelInherits1->setEnabled(false);
      m_NamedTypes1->setEnabled(false);
      m_Inherits1->setEnabled(false);
      m_Synch->setEnabled(false);
      m_Concurrent->setEnabled(false);
      m_Independent->setEnabled(false);
    }
    if (myDECL->SecondTFlags.Contains(funcImpl)
        || myDECL->TypeFlags.Contains(isGUI)
        || myDECL->SecondTFlags.Contains(overrides)) {
      m_Initializer->setEnabled(false);
      m_CHECKOp->setEnabled(false);
      m_CMBOperator->setEnabled(false);
//      m_BasicTypes->setEnabled(false);
    }
  }
  UpdateData(false); 
  m_NewName->setFocus();
  return BoxContinue;
}

void CFuncBox::CalcOpBox()
{
  TID id;
  int high, iop;
  CComboBoxItem *listItem;
  high = m_CMBOperator->count();
  if (high)
    for (iop = 1; iop < high; iop++)
      m_CMBOperator->removeItem(1);
  high = (int)OP_high;
  if (!myDECL->ParentDECL->SecondTFlags.Contains(isArray))
    high = high-2;
  myDoc->MakeVElems(myDECL->ParentDECL);

  for (iop = ((int)OP_noOp)+1; iop < (int)OP_high; iop++) {
    id.nID = -1;
    if (!myDoc->getOperatorID(myDECL->ParentDECL, (TOperator) iop, id)
        || (id.nID == myDECL->OwnID) && (id.nINCL == 0)) {
      listItem = new CComboBoxItem(TID(iop, 0));
      m_CMBOperator->addItem(QString(LBaseData->OperatorNames [iop].c),QVariant::fromValue(listItem));
//      m_CMBOperator->listBox()->insertItem(listItem);//sort
      if ((id.nID == myDECL->OwnID) && (id.nINCL == 0))
        m_CMBOperator->setCurrentText(QString(LBaseData->OperatorNames [myDECL->op ].c));
    }
  }
  SortCombo(m_CMBOperator);
}


void CFuncBox::on_m_RMOverrides_clicked() 
{
  UpdateData(true);
  m_RMOverrides->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);  
  second = true;
  m_Initializer->setEnabled(true);
  m_ConstFunc->setEnabled(true);
  m_Protected->setEnabled(true);
  m_CHECKOp->setEnabled(true);
  m_CMBOperator->setEnabled(true);
  m_DelInherits1->setEnabled(true);
  m_NamedTypes1->setEnabled(true);
  m_Inherits1->setEnabled(true);
//  m_BasicTypes->setEnabled(true);
  OnInitDialog();
}

/*
void CFuncBox::OnSelendokBasicTypes() 
{
  SelEndOKToList(m_BasicTypes, &m_Inherits1);
}
*/

void CFuncBox::on_m_DelInherits1_clicked() 
{
  int pos = m_Inherits1->currentItem();
  if (pos >= 0)
    m_Inherits1->removeItem(pos); 
}

  
void CFuncBox::on_m_NamedTypes1_triggered(int pos) 
{
  if (!pos) return;
  SelEndOKToList(m_NamedTypes1, m_Inherits1);
  
}


void CFuncBox::on_m_CHECKOp_clicked() 
{
  UpdateData(true);
  if (m_CHECKOp->isOn()) {
    m_CMBOperator->setEnabled(true);
    m_NewName->setEnabled(false);
  }
  else {
    m_CMBOperator->setEnabled(false);
    m_NewName->setEnabled(true);
    if (myDECL->op != OP_noOp) {
      valNewName = QString("");
      myDECL->op = OP_noOp;
    }
  }
  UpdateData(false);

}


void CFuncBox::on_m_StaticFunc_clicked() 
{
  UpdateData(true);
  if (m_StaticFunc->isOn()) {
    m_Protected->setChecked(false);
    m_Initializer->setChecked(false);
    m_Abstract->setChecked(false);
    m_EnforceOver->setChecked(false);
    m_ConstFunc->setChecked(false);
    m_Protected->setEnabled(false);
    m_Initializer->setEnabled(false);
    m_Abstract->setEnabled(false);
    m_EnforceOver->setEnabled(false);
    myDECL->TypeFlags.INCL(isConst); 
    m_ConstFunc->setChecked(true);
    m_ConstFunc->setEnabled(false);
  }
  else {
    m_Protected->setEnabled(true);
    m_Initializer->setEnabled(true);
    m_Abstract->setEnabled(true);
    m_EnforceOver->setEnabled(true);
    m_ConstFunc->setEnabled(true);
  }
  UpdateData(false);
}

void CFuncBox::on_m_Initializer_clicked() 
{
  UpdateData(true);
  bool ini = m_Initializer->isOn();
  m_DefaultIni->setEnabled(!hasParams
                            && ini
                            && (!myDECL->ParentDECL->WorkFlags.Contains(hasDefaultIni)
                                || myDECL->TypeFlags.Contains(defaultInitializer)));
  if (ini) {
    m_ConstFunc->setEnabled(false); 
    m_Protected->setEnabled(false);
    m_StaticFunc->setEnabled(false);
    m_StaticFunc->setChecked(false);
    myDECL->TypeFlags.EXCL(isAbstract);
    m_Abstract->setChecked(false);
    m_Abstract->setEnabled(false);
  }
  else {
    m_DefaultIni->setChecked(false);
    m_Abstract->setEnabled(true);
    m_ConstFunc->setEnabled(true); 
    m_Protected->setEnabled(true);
  }
  myDECL->TypeFlags.EXCL(isConst);
  UpdateData(false);  
}

void CFuncBox::on_m_CMBOperator_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  TID id;
  SelEndOKToStr(m_CMBOperator, &valNewName, &id);
  myDECL->op = (TOperator)id.nID;
  UpdateData(false);
}

/*
void CFuncBox::OnInheritsBody() 
{
  UpdateData(true);
  if (m_InheritsBody->isOn()) {
    m_Abstract->setChecked(false);
    m_Abstract->setEnabled(false); 
  }
  else
    m_Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));  
  UpdateData(false);
}
*/

void CFuncBox::on_m_Abstract_clicked() 
{
  UpdateData(true);
  if (m_Abstract->isOn()) {
    m_Native1->setChecked(false);
    m_Native1->setEnabled(false);
    m_StaticFunc->setEnabled(false);
    m_StaticFunc->setChecked(false);
  }
  else {
    m_Native1->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isNative));
//    m_InheritsBody->setEnabled(myDECL->SecondTFlags.Contains(overrides));
  }
  UpdateData(false);
}

void CFuncBox::on_m_Native1_clicked() 
{
  UpdateData(true);
  if (m_Native1->isOn()) {
    m_Abstract->setChecked(false);
    m_Abstract->setEnabled(false); 
  }
  else
    m_Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));  
  UpdateData(false);
}

void CFuncBox::on_m_Signal_clicked()
{
  if (m_Signal->isOn()) {
    m_ConstFunc->setChecked(true); 
    m_ConstFunc->setEnabled(false);
    myDECL->TypeFlags.INCL(isConst); 
    m_Protected->setChecked(true);
    m_Protected->setEnabled(false);
    m_Abstract->setChecked(false);
    m_Abstract->setEnabled(false);
    m_StaticFunc->setChecked(false);
    m_StaticFunc->setEnabled(false); 
    m_Initializer->setChecked(false);
    m_Initializer->setEnabled(false);
    m_Native1->setChecked(true);
    m_Native1->setEnabled(false);
    m_EnforceOver->setChecked(false);
    m_EnforceOver->setEnabled(false);
  }
  else {
    m_ConstFunc->setEnabled(true);
    m_Protected->setEnabled(true);
    m_Abstract->setEnabled(true);
    m_StaticFunc->setEnabled(true); 
    m_Initializer->setEnabled(true);
    m_Native1->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isNative));
    m_Native1->setChecked(myDECL->ParentDECL->TypeFlags.Contains(isNative));
    m_EnforceOver->setEnabled(true);
  }
}

void CFuncBox::on_m_EnableName_clicked() 
{
  UpdateData(true);
  if (m_EnableName->isOn()) 
    m_NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    m_NewName->setEnabled(false);
  }
  UpdateData(false);
}


void CFuncBox::OnOK() 
{
  if (myDECL->SecondTFlags.Contains(funcImpl))
    if ( myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)) {
      QDialog::reject();
      return;
    }
    else {
      myDECL->SecondTFlags.EXCL(funcImpl);
      myDECL->TypeFlags.EXCL(isProtected);
      myDECL->Supports.Destroy();
    }
  UpdateData(true);
  if (m_EnableName->isOn()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (m_Initializer->isOn())
    myDECL->TypeFlags.INCL(isInitializer); 
  else
    myDECL->TypeFlags.EXCL(isInitializer); 
  if (!myDECL->SecondTFlags.Contains(overrides) || myDECL->SecondTFlags.Contains(enableName)) {
    if (myDECL->op == OP_noOp) {
      QString* ids = CheckNewName(valNewName, myDECL, myDoc);
      if (ids) {
        QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
        m_NewName->setFocus();
        //m_NewName->SetSel(0, -1);
        return;
      }
    }
    if (myDECL->op != OP_noOp)
      myDoc->MakeOperator(myDECL);
  }
  ListToChain(m_Inherits1, &myDECL->Inherits);  //fires
  if (m_Native1->isOn())
    myDECL->TypeFlags.INCL(isNative); 
  else
    myDECL->TypeFlags.EXCL(isNative); 
  if (!myDECL->TypeFlags.Contains(isNative) && (myDECL->ParentDECL->DeclType == Impl) )
    myDoc->GetExecDECL(myDECL, ExecDef);
  if (!myDECL->Supports.first) 
    myDECL->SecondTFlags.EXCL(overrides);
  if (m_Abstract->isOn())
    myDECL->TypeFlags.INCL(isAbstract); 
  else
    myDECL->TypeFlags.EXCL(isAbstract);
  if (m_StaticFunc->isOn())
    myDECL->TypeFlags.INCL(isStatic);
  else
    myDECL->TypeFlags.EXCL(isStatic);
  if (m_Protected->isOn())
    myDECL->TypeFlags.INCL(isProtected);
  else
    myDECL->TypeFlags.EXCL(isProtected);
  if (m_EnforceOver->isOn())
    myDECL->TypeFlags.INCL(forceOverride);
  else
    myDECL->TypeFlags.EXCL(forceOverride);
  if (m_ConstFunc->isOn())
    myDECL->TypeFlags.INCL(isConst); 
  else
    myDECL->TypeFlags.EXCL(isConst); 
  if (m_Signal->isOn())
    myDECL->SecondTFlags.INCL(isLavaSignal); 
  else
    myDECL->SecondTFlags.EXCL(isLavaSignal); 
  if (m_DefaultIni->isOn())
    myDECL->TypeFlags.INCL(defaultInitializer); 
  else
    myDECL->TypeFlags.EXCL(defaultInitializer); 
  myDECL->TypeFlags.EXCL(execIndependent);
  myDECL->TypeFlags.EXCL(execConcurrent);
  if (valSynch >= 1)
    myDECL->TypeFlags.INCL(execConcurrent);
  else
    if (valSynch == 2)
      myDECL->TypeFlags.INCL(execIndependent);

  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CImplBox dialog


CImplBox::CImplBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{

}

CImplBox::CImplBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
}

void CImplBox::UpdateData(bool getData)
{
  if (getData) 
    valImplSel = m_ImplSel->text();
  else 
    m_ImplSel->setText(valImplSel);
}

ValOnInit CImplBox::OnInitDialog() 
{
  DString strINCL, strID;
  CExecAllDefs * execAllPatt;
  int pos, icount;
  TID id;
  bool ex=false;
  QVariant var;

  if (myDoc->changeNothing) {
    PushButton_IDOK7->setEnabled(false);
    PushButton_IDOK7->setDefault( false );
    PushButton_IDCANCEL7->setDefault( true );
  }

  if (!onNew) {
    LavaDECL* decl;
    CHETID *cheS = (CHETID*)myDECL->Supports.first; //implements
    if (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) 
          valImplSel = QString(decl->LocalName.c); 
        else {
          QMessageBox::critical(this, qApp->name(), ERR_NoImplIF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          valImplSel = QString("??: ");
          Convert.IntToString(cheS->data.nINCL, strINCL);
          Convert.IntToString(cheS->data.nID, strID);
          valImplSel = valImplSel + QString("(") 
            + QString(strINCL.c) + QString(",") + QString(strID.c) + QString(")");
        }
      }
    }
  }
  execAllPatt = new CExecAllDefs(myDoc, m_ImplTypes, 0, myDECL->ParentDECL, OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  if (myDECL->DeclType == CompObj) {
    setCaption("Component object implementation");
    Label_IDC_ComboEx->setText( "Component object to be implemented");
  }
  if (!onNew) {
    icount = m_ImplTypes->count();
    for (pos=1; (pos < icount) && !ex; pos++) {
      var = m_ImplTypes->itemData(pos);
      id = var.value<CComboBoxItem*>()->itemData();
//      id = ((CComboBoxItem*)m_ImplTypes->listBox()->item(pos))->itemData();
      if (id == ((CHETID*)myDECL->Supports.first)->data)
        ex = true;
    }
    if (pos < icount)
      m_ImplTypes->setCurrentItem(pos-1);
    m_ImplTypes->setEnabled(false);
  }
  delete execAllPatt;
  UpdateData(false);
  return BoxContinue;
}


void CImplBox::on_m_ImplTypes_triggered(int pos) 
{
  if (!pos) return;
  CHETID* che;
  if (!myDECL->Supports.first) {
    che = new CHETID;
    myDECL->Supports.Append(che);
  }
  else
    che = (CHETID*)myDECL->Supports.first;
  int onSel = SelEndOKToStr(m_ImplTypes, &valImplSel, &che->data);
  UpdateData(false);

}

void CImplBox::OnOK() 
{
  LavaDECL *interfDECL, *inDECL;
  CHE *Che;
  CHETID* cheID;
  
  UpdateData(true);
  if (!myDECL->Supports.first) {
    QMessageBox::critical(this,qApp->name(),IDP_SelInterface,QMessageBox::Ok,0,0);
    m_ImplTypes->setFocus();
    return;
  }
  interfDECL = myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data);
  if (interfDECL) {
    CheckNewName(valImplSel, myDECL, myDoc);
    if (interfDECL->TypeFlags.Contains(isAbstract))
      myDECL->TypeFlags.INCL(isAbstract);
    else
      myDECL->TypeFlags.EXCL(isAbstract);
    if (interfDECL->TypeFlags.Contains(isGUI)) {
      myDECL->TypeFlags.INCL(isGUI);
      myDECL->RefID.nID = interfDECL->RefID.nID;
      myDECL->RefID.nINCL = myDoc->IDTable.IDTab[interfDECL->inINCL]->nINCLTrans[interfDECL->RefID.nINCL].nINCL;
    }
    else {
      myDECL->TypeFlags.EXCL(isGUI);
      myDECL->RefID.nID = -1;
    }
    if (myDECL->DeclType == Impl)  
      myDoc->CheckImpl(myDECL, CHLV_inUpdateLow);
    else { //Component
      if (onNew) {
        inDECL = NewLavaDECL();
        inDECL->DeclType = Interface;
        inDECL->DeclDescType = StructDesc;
        inDECL->ParentDECL = myDECL;
        inDECL->LocalName = myDECL->LocalName;;
        inDECL->FullName = myDECL->FullName + inDECL->LocalName;
        inDECL->TypeFlags += SET(isComponent,-1);
        inDECL->Supports = interfDECL->Supports;
        for (cheID = (CHETID*)inDECL->Supports.first; cheID; cheID = (CHETID*)cheID->successor) 
          cheID->data.nINCL = myDoc->IDTable.IDTab[interfDECL->inINCL]->nINCLTrans[cheID->data.nINCL].nINCL;
        Che = NewCHE(inDECL);
        myDECL->NestedDecls.Append(Che);
        inDECL->TypeFlags.INCL(thisComponent);
      }
    }
    QDialog::accept();
  }
  else
    QDialog::reject();
}

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIncludeBox 


CIncludeBox::CIncludeBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CIncludeBox::CIncludeBox(CLavaPEDoc* myDoc, CHESimpleSyntax* newChe, CHESimpleSyntax* oldChe, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  NewChe = newChe;
  OldChe = oldChe;
  MyDoc = myDoc;
  PushButton_IDOK15->setEnabled(!myDoc->changeNothing);
  if (NewChe->data.LocalTopName.l)
    valUseAs = QString(NewChe->data.LocalTopName.c);
  DString fn = NewChe->data.UsersName; 
  AbsPathName(fn, MyDoc->IDTable.DocDir);
  valFullPathName = QString(fn.c);
  UpdateData(false);
  //return BoxContinue;
}

void CIncludeBox::UpdateData(bool getData)
{
  if (getData) {
    valUseAs = m_UseAs->text();
    valFullPathName = m_FullPathName->text();
  }
  else {
    m_UseAs->setText(valUseAs);
    m_FullPathName->setText(valFullPathName);
  }
}


void CIncludeBox::OnOK() 
{
  UpdateData(true);
  NewChe->data.UsersName = STRING(qPrintable(valFullPathName));
  QFileInfo fi(valFullPathName);
  QString qfn = ResolveLinks(fi);
  NewChe->data.SyntaxName = STRING(qPrintable(qfn));
  RelPathName(NewChe->data.SyntaxName, MyDoc->IDTable.DocDir); 
  RelPathName(NewChe->data.UsersName, MyDoc->IDTable.DocDir);   
  NewChe->data.LocalTopName = STRING(qPrintable(valUseAs)); 
  QDialog::accept();
}

void CIncludeBox::on_m_OtherPath_clicked() 
{
  UpdateData(true);
  valFullPathName = L_GetOpenFileName(
                    valFullPathName,
                    this,
                    "Choose a file to include",
                    "Lava files (*.lava)",
                    "lava"
                    );
  UpdateData(false);
}


/////////////////////////////////////////////////////////////////////////////
// CInitBox dialog


CInitBox::CInitBox(QWidget* parent)
  : QDialog(parent)
{
}

CInitBox::CInitBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
 : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  isTop = (OrigDECL == ((CHESimpleSyntax*)myDoc->mySynDef->SynDefTree.first)->data.TopDef.ptr);
}

void CInitBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    if (BGroup_ExecMode->find(0)->isOn())
      valSynch = 0;
    else
      valSynch = 1;
  }
  else {
    m_NewName->setText(valNewName);
    BGroup_ExecMode->setButton(valSynch);
  }
}

ValOnInit CInitBox::OnInitDialog() 
{

  if (myDoc->changeNothing) {
    PushButton_IDOK14->setEnabled(false);
    PushButton_IDOK14->setDefault( false );
    PushButton_IDCANCEL14->setDefault( true );
  }
  if (!onNew) 
    valNewName = QString(myDECL->LocalName.c);

  if (onNew) {
    m_Transaction1->setChecked(false);
    m_const->setChecked(false);
    valSynch = 0;
  }
  else {
    if (myDECL->TypeFlags.Contains(isTransaction)) 
      m_Transaction1->setChecked(true);
    else
      m_Transaction1->setChecked(false);
    if (myDECL->TypeFlags.Contains(isConst)) 
      m_const->setChecked(true);
    else
      m_const->setChecked(false);
    if (myDECL->TypeFlags.Contains(execConcurrent)) 
      valSynch = 1;
    /*
    else
      if (myDECL->TypeFlags.Contains(execIndependent)) 
        valSynch = 2;
    */
    else
      valSynch = 0;
  }
  m_NewName->setFocus();

  UpdateData(false);
  return BoxContinue;
}

void CInitBox::OnOK() 
{
  UpdateData(true);
  if (isTop && !valNewName.length()) {
    myDECL->FullName.Destroy();
    myDECL->LocalName.Destroy();
  }
  else  {
    QString* ids = CheckNewName(valNewName, myDECL, myDoc);
    if (ids) {
      QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
      m_NewName->setFocus();
      return;
    }
  }
  myDoc->GetExecDECL(myDECL, ExecDef);
  if (m_Transaction1->isOn())
    myDECL->TypeFlags.INCL(isTransaction); 
  else
    myDECL->TypeFlags.EXCL(isTransaction); 
  if (m_const->isOn())
    myDECL->TypeFlags.INCL(isConst); 
  else
    myDECL->TypeFlags.EXCL(isConst); 
//  myDECL->TypeFlags.EXCL(execIndependent);
  myDECL->TypeFlags.INCL(execIndependent);
  myDECL->TypeFlags.EXCL(execConcurrent);
  if (valSynch == 1)
    myDECL->TypeFlags.INCL(execConcurrent);
  /*
  else
    if (valSynch == 2)
      myDECL->TypeFlags.INCL(execIndependent);
  */
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CInterfaceBox dialog


CInterfaceBox::CInterfaceBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CInterfaceBox::CInterfaceBox(LavaDECL* mydecl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  valNewName = QString("");
  valIfaceID = QString("");
  valBuildSet = false;
  valIsGUI = false;
  valKindOfInterface =0;
  myDECL = mydecl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  if (OrigDECL)
    ContextDECL = OrigDECL;
  else
    ContextDECL = myDECL->ParentDECL;
}

void CInterfaceBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    valIfaceID = m_InterfaceID->text();
    valBuildSet = m_BuildSet1->isOn();
    valIsGUI = m_IsGUI->isOn();
    if (BGroup_KindOfInterface->find(0)->isOn())
      valKindOfInterface = 0;
    else if (BGroup_KindOfInterface->find(1)->isOn())
      valKindOfInterface = 1;
    else
      valKindOfInterface = 2;
  }
  else {
    m_NewName->setText(valNewName);
    m_InterfaceID->setText(valIfaceID);
    m_BuildSet1->setChecked(valBuildSet);
    m_IsGUI->setChecked(valIsGUI);
    BGroup_KindOfInterface->setButton(valKindOfInterface);
  }
}

ValOnInit CInterfaceBox::OnInitDialog() 
{
  DString str;
  CContext context;
  int pos, icount;
  QString * err;
  CExecAllDefs * execAllPatt;
  QVariant var;
//  CHETID *ncheS, *cheS;
//  LavaDECL* decl;
  bool found=false;
//  CComboBoxItem *listItem;


//  GetDlgItem(IDC_StatIDLabel)->SetWindowText("CLSID of implementation DLL");

  if (myDoc->changeNothing) {
    PushButton_IDOK9->setEnabled(false);
    PushButton_IDOK9->setDefault( false );
    m_CANCEL9->setDefault( true );
  }
  if (onNew) {
    myDoc->IDTable.GetPattern(myDECL, context);
    m_IsGUI->setEnabled(!context.oContext);
    m_Native->setChecked(false);
  }
  else {
    m_IsGUI->setEnabled(false);
    valNewName = QString(myDECL->LocalName.c);
    err = myDoc->IDTable.CleanSupports(myDECL, ContextDECL);
    if (err == &ERR_NoBaseIF) {
      if (myDoc->AutoCorrBox(&ERR_NoBaseIF) == QMessageBox::Cancel) {
        return BoxCancel; //QDialog::reject();
      }
    }
    else if (err == &ERR_CleanSupports) {
      if (myDoc->AutoCorrBox(&ERR_CleanSupports) == QMessageBox::Cancel) {
        myDECL->Supports.Destroy();
        myDECL->Supports = OrigDECL->Supports;
      }
    }
    SupportsToList();
    /*
    cheS = (CHETID*)myDECL->Inherits.first;
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) {
          listItem = new CComboBoxItem(decl->LocalName, cheS->data);
          m_Inherits->insertItem(listItem);
          cheS = (CHETID*)cheS->successor;
        }
        else {
          ncheS = (CHETID*)cheS->successor;
          myDECL->Inherits.Uncouple(cheS);
          delete cheS;
          cheS = ncheS;
          if (myDoc->AutoCorrBox(&ERR_NoFiredIF) == QMessageBox::Cancel) {
            return BoxCancel; //QDialog::reject();
          }
        }
      }
      else
        cheS = (CHETID*)cheS->successor;
    } */
    m_BuildSet1->setEnabled(false);
    m_Native->setChecked(myDECL->TypeFlags.Contains(isNative));
    if (myDECL->TypeFlags.Contains(isComponent)) {
      valKindOfInterface = 2;
      m_InterfaceID->setEnabled(true);
      //m_Native->setEnabled(false);
      if (myDECL->LitStr.l) {
        valIfaceID = QString(myDECL->LitStr.c);
      }
    }
    else
      if (myDECL->TypeFlags.Contains(isAbstract))
        valKindOfInterface = 1;
  }
  myDoc->MakeBasicBox(m_BasicTypes, myDECL->DeclType, false);
  execAllPatt = new CExecAllDefs(myDoc, m_ExtTypes, m_GUIStructs, 
                        myDECL->ParentDECL, OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  delete execAllPatt;
  m_BasicTypes->setCurrentItem(0);
  m_ExtTypes->setCurrentItem(0);
  TID id;
  if (!onNew && myDECL->TypeFlags.Contains(isGUI)) {
    valIsGUI = true; 
    icount = m_GUIStructs->count();
    bool ex = false;
    for (pos=1;
         (pos < icount)
           && !ex;
        // && (myDECL->RefID != ((CComboBoxItem*)m_GUIStructs->listBox()->item(pos))->itemData());
         pos++) {
           var = m_GUIStructs->itemData(pos);
           id = var.value<CComboBoxItem*>()->itemData();
           if (id == myDECL->RefID)
             ex = true;}
    if (pos <= icount)
      m_GUIStructs->setCurrentItem(pos-1);
  }
  m_NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}

void CInterfaceBox::SupportsToList()
{
  LavaDECL* decl;
  CListBoxItem* listItem;
  CHETID *ncheS, *cheS = (CHETID*)myDECL->Supports.first; //extends
  DString str;
  m_Extends->clear();
  if (cheS && ((cheS->data.nINCL == 1) || myDoc->isStd) //!!
      && (cheS->data.nID == myDoc->IDTable.BasicTypesID[B_Object]))
    myDECL->Supports.Destroy();
  else {
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) {
          if (decl->DeclType == VirtualType) {
            str = lthen + decl->FullName + grthen;
            listItem = new CListBoxItem(str, cheS->data);
          }
          else 
            listItem = new CListBoxItem(decl->FullName, cheS->data);
          m_Extends->insertItem(listItem);
          cheS = (CHETID*)cheS->successor;
        }
        else {
          ncheS = (CHETID*)cheS->successor;
          myDECL->Supports.Uncouple(cheS);
          delete cheS;
          cheS = ncheS;
          if (myDoc->AutoCorrBox(&ERR_NoBaseIF) == QMessageBox::Cancel) {
            QDialog::reject();
            return;
          }
        }
      }
      else
        cheS = (CHETID*)cheS->successor;
    }
  }
}

void CInterfaceBox::on_m_DelSupport_clicked() 
{
  int pos;

  UpdateData(true);
  CListBoxItem *listItem = (CListBoxItem*)m_Extends->selectedItem();
  if (listItem) {
    pos = m_Extends->index(listItem);
    m_Extends->removeItem(pos);
    ListToChain(m_Extends, &myDECL->Supports);
    ResetComboItems(m_ExtTypes);
    CExecBase *execBase = new CExecBase(this);
    delete execBase;
    UpdateData(false);
  }
}

void CInterfaceBox::on_m_ExtTypes_triggered(int pos) 
{
  QVariant var;

  if (!pos) return;
  UpdateData(true);
  var = m_ExtTypes->itemData(pos);
  CComboBoxItem *listItem = var.value<CComboBoxItem*>();
  if (listItem) {
    if (myDoc->IDTable.InsertBase(myDECL, myDoc->IDTable.GetDECL(listItem->itemData(), 0), ContextDECL, true)) {
      SupportsToList();
      ResetComboItems(m_ExtTypes);
      CExecBase *execBase = new CExecBase(this);
      delete execBase;
      m_BasicTypes->setCurrentItem(0);
      UpdateData(false);
    }
  }
}


void CInterfaceBox::on_m_BasicTypes1_triggered(int pos) 
{
  QVariant var;

  if (!pos) return;
  UpdateData(true);
  var = m_BasicTypes->itemData(pos);
  CComboBoxItem *listItem = var.value<CComboBoxItem*>();
  if (listItem) {
    if (myDoc->IDTable.InsertBase(myDECL, myDoc->IDTable.GetDECL(listItem->itemData(), 0), ContextDECL, true)) {
      SupportsToList();
      m_ExtTypes->setCurrentItem(0);
      UpdateData(false);
    }
  }
  m_BasicTypes->setCurrentItem(0);
}

/*
void CInterfaceBox::on_m_DelInherits_clicked() 
{
  int pos;
  CComboBoxItem *listItem = (CComboBoxItem*)m_Inherits->selectedItem();
  if (listItem) {
    pos = m_Inherits->index(listItem);
    m_Inherits->removeItem(pos);
  }  
}
void CInterfaceBox::on_m_InheritTypes_triggered(int pos) 
{
  if (!pos) return;
  SelEndOKToList(m_InheritTypes, m_Inherits);
}

*/

void CInterfaceBox::on_m_IsComponent_clicked() 
{
  UpdateData(true);
  if  (valKindOfInterface == 2) 
    myDECL->TypeFlags.INCL(isComponent);
  else 
    myDECL->TypeFlags.EXCL(isComponent);
  ResetComboItems(m_ExtTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_ExtTypes,
                 m_GUIStructs, myDECL->ParentDECL,
                 OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  m_ExtTypes->setCurrentItem(0);
  m_InterfaceID->setEnabled(true);
  //m_Native->setChecked(false);
  //m_Native->setEnabled(false);
  UpdateData(false);
}

void CInterfaceBox::on_m_NonCreatable_clicked() 
{
  m_InterfaceID->setEnabled(false);
  m_Native->setEnabled(true);
}

void CInterfaceBox::on_m_Creatable_clicked() 
{
  m_InterfaceID->setEnabled(false);
  m_Native->setEnabled(true);
}

void CInterfaceBox::on_m_IsGUI_clicked() 
{
  UpdateData(true);
  m_GUIStructs->setEnabled(valIsGUI && onNew);
  
}

void CInterfaceBox::on_m_GUIStructs_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  if (SelEndOKToStr(m_GUIStructs, &valGUIStruct, &exID) <= 0)
    m_GUIStructs->setCurrentText(valGUIStruct);
}

void CInterfaceBox::OnOK() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc/*, OrigDECL*/);
  LavaDECL *suDECL;
  CHETID* cheS;
  bool extendsSet = false, extendsArray = false, extendsChain = false,
    extendsException = false, extendsEnum = false;

  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    m_NewName->setCursorPosition(0);
    m_NewName->setFocus();
    return;
  }
  myDECL->LitStr.Destroy();
  if (m_Native->isOn())
    myDECL->TypeFlags.INCL(isNative);
  else
    myDECL->TypeFlags.EXCL(isNative);
  if  (valKindOfInterface == 1) {
    myDECL->TypeFlags.INCL(isAbstract);
    myDECL->TypeFlags.EXCL(isComponent);
  }
  else {
    if  (valKindOfInterface == 2) {
      myDECL->TypeFlags.EXCL(isAbstract);
      if (valIfaceID.length() > 0)
        myDECL->LitStr = STRING(qPrintable(valIfaceID));
      myDECL->TypeFlags.INCL(isComponent);
      if (!onNew) {
        CHE* che = (CHE*)myDECL->NestedDecls.first;
        while (che) {
          if (((LavaDECL*)che->data)->DeclType == Attr)
            ((LavaDECL*)che->data)->TypeFlags.INCL(hasSetGet);
          che = (CHE*)che->successor;
        }
      }
    }
    else {
      myDECL->TypeFlags.EXCL(isComponent);
      myDECL->TypeFlags.EXCL(isAbstract);
    }
  }
  if (onNew) {
    if (valIsGUI) {
      if (exID.nID >= 0) {
        myDECL->RefID = exID;
        myDoc->MakeGUIFuncs(myDECL);
      }
      else {
        QMessageBox::critical(this, qApp->name(), IDP_NoTypeSel, QMessageBox::Ok,0,0);
        m_GUIStructs->setFocus();
        return;
      }
    }
    if (!myDECL->TypeFlags.Contains(isComponent))
      myDoc->MakeIniFunc(myDECL);
  }
  ListToChain(m_Extends, &myDECL->Supports);
  cheS = (CHETID*)myDECL->Supports.first;
  if (cheS) {
    while (cheS) {
      suDECL = myDoc->IDTable.GetDECL(cheS->data);//!!
      if (suDECL) {
        extendsSet = extendsSet || suDECL->SecondTFlags.Contains(isSet);
        extendsArray = extendsArray || suDECL->SecondTFlags.Contains(isArray);
        extendsChain = extendsChain || suDECL->SecondTFlags.Contains(isChain);
        extendsException = extendsException || suDECL->SecondTFlags.Contains(isException);
        extendsEnum = extendsEnum || suDECL->SecondTFlags.Contains(isEnum);
        if ((myDoc->isStd || (cheS->data.nINCL == 1)) && (suDECL->fromBType == Enumeration)) {
          myDECL->DeclDescType = EnumType;
          if (!myDECL->EnumDesc.ptr)
            myDECL->EnumDesc.ptr = new TEnumDescription;
          ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.DeclDescType = BasicType;
          ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.BType = Enumeration;
          ((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField.RefID = cheS->data;
        }
      }
      cheS = (CHETID*)cheS->successor;
    }
    if (myDECL->DeclDescType != EnumType)
      myDECL->EnumDesc.Destroy();
    if (extendsSet)
      myDECL->SecondTFlags.INCL(isSet);
    else
      myDECL->SecondTFlags.EXCL(isSet);
    if (extendsArray)
      myDECL->SecondTFlags.INCL(isArray);
    else
      myDECL->SecondTFlags.EXCL(isArray);
    if (extendsException)
      myDECL->SecondTFlags.INCL(isException);
    else
      myDECL->SecondTFlags.EXCL(isException);
    if (extendsChain)
      myDECL->SecondTFlags.INCL(isChain);
    else
      myDECL->SecondTFlags.EXCL(isChain);
    if (extendsEnum)
      myDECL->SecondTFlags.INCL(isEnum);
    else
      myDECL->SecondTFlags.EXCL(isEnum);
  }
  else 
    if (/*!myDECL->TypeFlags.Contains(isComponent)
        && */(!myDoc->isStd || (myDECL->fromBType != B_Object)
                          && (myDECL->fromBType != NonBasic))) {
      cheS = new CHETID;
        cheS->data.nID = myDoc->IDTable.BasicTypesID[B_Object];
      if (myDoc->isStd)
        cheS->data.nINCL = 0;
      else
        cheS->data.nINCL = 1;
      myDECL->Supports.Append(cheS);
    }
//  ListToChain(m_Inherits, &myDECL->Inherits);  //signals
  myDECL->WorkFlags.INCL(recalcVT);
  QDialog::accept();
}

/////////////////////////////////////////////////////////////////////////////
// CIOBox dialog


CIOBox::CIOBox(QWidget*  /*=NULL*/)
{
}

CIOBox::CIOBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
}

void CIOBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    valNewTypeType = m_NewTypeType->text();
    if (BGroup_Mode->find(0)->isOn())
      valkindOfField = 0;
    else 
      valkindOfField = 1;
  }
  else {
    m_NewName->setText(valNewName);
    m_NewTypeType->setText(valNewTypeType);
    BGroup_Mode->setButton(valkindOfField);
  }
}

void CIOBox::BaseClassesToCombo(LavaDECL *decl)
{
  LavaDECL *baseDECL;
  CHETVElem*  El;
  TID id1 = TID(-1, 0);
  CComboBoxItem *item;

  myDoc->MakeVElems(decl);
  El = (CHETVElem*)decl->VElems.VElems.first;
  while (El) {
    if (id1 != El->data.VTClss) {
      baseDECL = myDoc->IDTable.GetDECL(El->data.VTClss);
      if (baseDECL) {
        if (baseDECL->fromBType == NonBasic) {
          item = new CComboBoxItem(TID(baseDECL->OwnID, baseDECL->inINCL));
      m_NamedTypes->addItem(QString(baseDECL->FullName.c),QVariant::fromValue(item));
//          m_NamedTypes->listBox()->insertItem(item);//sort
        }     
        else 
          item = new CComboBoxItem(/*((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[baseDECL->fromBType],*/ TID(baseDECL->OwnID, 1));
          m_BasicTypes->addItem(QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[baseDECL->fromBType]), QVariant::fromValue(item));//sort
      }
      id1 = El->data.VTClss;
    }
    El = (CHETVElem*)El->successor;
  }
  SortCombo(m_BasicTypes);
  SortCombo(m_NamedTypes);
  //m_NamedTypes->SetDroppedWidth(maxWidth+6);
}

ValOnInit CIOBox::OnInitDialog()
{ 
  CExecAllDefs * execAllPatt=0;
  LavaDECL* typeDECL, *baseDECL;
  CHETID* che;
  bool catErr, callBox;
  DString dstr;

  if (myDoc->changeNothing) {
    PushButton_IDOK18->setEnabled(false);
    PushButton_IDOK18->setDefault( false );
    PushButton_IDCANCEL18->setDefault( true );
  }

  TypeFlags = myDECL->ParentDECL->ParentDECL->TypeFlags;
  if (onNew) {
    m_ValueObject1->setChecked(true);
    myDECL->TypeFlags.INCL(trueObjCat);
    myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
    execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    m_BasicTypes->setCurrentText(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[(int)(VLString)]);
    int num = SelEndOKToStr(m_BasicTypes, &valNewTypeType, &myDECL->RefID);
    if (num > 0) {
      myDECL->DeclDescType = BasicType;
      myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
    }
    valkindOfField = 0;
  }
  else {
    valNewName = QString(myDECL->LocalName.c);
    if (myDECL->TypeFlags.Contains(isOptional))
       valkindOfField = 1;
    else
       valkindOfField = 0;
    if (myDECL->ParentDECL->TypeFlags.Contains(isStatic)) {
      m_SameAsSelf->setChecked(false);
      myDECL->TypeFlags.EXCL(sameAsSelf);
      m_SameAsSelf->setEnabled(false);
    }
    if (myDECL->TypeFlags.Contains(substitutable)) {
      m_Substitutable1->setChecked(true);
      TypeFlags.INCL(substitutable);
    }
    else
      m_Substitutable1->setChecked(false); 
   if (myDoc->GetCategoryFlags(myDECL, catErr).Contains(definesObjCat)) {
      m_StateObject1->setChecked(false);
      m_ValueObject1->setChecked(false);
      m_AnyCategory->setChecked(false);
      m_SameAsSelf->setChecked(false);
      m_StateObject1->setEnabled(false);
      m_ValueObject1->setEnabled(false);
      m_SameAsSelf->setEnabled(false);
      m_AnyCategory->setEnabled(false);
    }
    else 
      if (myDECL->SecondTFlags.Contains(overrides)) {
        m_StateObject1->setEnabled(false);
        m_ValueObject1->setEnabled(false);
        m_SameAsSelf->setEnabled(false);
        m_AnyCategory->setEnabled(false);
      }
    if (myDECL->TypeFlags.Contains(trueObjCat))
      if (myDECL->TypeFlags.Contains(stateObject)) {
        m_StateObject1->setChecked(true);
        m_ValueObject1->setChecked(false);
        m_SameAsSelf->setChecked(false);
        m_AnyCategory->setChecked(false);
      }
      else {
        m_StateObject1->setChecked(false);
        if (myDECL->TypeFlags.Contains(sameAsSelf)) {
          m_ValueObject1->setChecked(false);
          m_SameAsSelf->setChecked(true);
          m_AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
          m_ValueObject1->setChecked(false);
          m_SameAsSelf->setChecked(false);
          m_AnyCategory->setChecked(true);
        }
        else {
          m_ValueObject1->setChecked(true);
          m_SameAsSelf->setChecked(false);
          m_AnyCategory->setChecked(false);
        }
      }
    if (myDECL->TypeFlags.Contains(isGUI)) {
      if (myDECL->DeclType == IAttr) {
        typeDECL = myDoc->IDTable.GetDECL(myDECL->ParentDECL->ParentDECL->RefID, myDECL->inINCL);
        if (typeDECL)
          BaseClassesToCombo(typeDECL);
      }
      else {
        m_NamedTypes->setEnabled(false);
        m_BasicTypes->setEnabled(false);
      }
    }
    else {
      che = (CHETID*)myDECL->Supports.first;
      if (myDECL->SecondTFlags.Contains(overrides)) {
        if (OrigDECL->DECLError1.first) {
          baseDECL = myDoc->IDTable.GetDECL(che->data, myDECL->inINCL);
          if (((CLavaError*)((CHE*)OrigDECL->DECLError1.first)->data)->IDS == &ERR_OverriddenIOType)
            callBox = true;
          else 
            callBox = baseDECL && !baseDECL->DECLError1.first;
        }
        else
          callBox = false;

        if (!callBox) {
          m_NamedTypes->setEnabled(false);
          m_BasicTypes->setEnabled(false);
        }
        else {
          if (myDoc->AutoCorrBox(&ERR_OverriddenIOType) == QDialog::Accepted) {
            myDECL->RefID.nID = baseDECL->RefID.nID;
            if (baseDECL->RefID.nID >= 0)
              myDECL->RefID.nINCL = myDoc->IDTable.IDTab[baseDECL->inINCL]->nINCLTrans[baseDECL->RefID.nINCL].nINCL;
            m_NamedTypes->setEnabled(false);
            m_BasicTypes->setEnabled(false);
          }
          else {
            myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
            execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL,
                                         Attr, TypeFlags);
          }
        }
      }
      else {
        myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
        execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL,
                                     Attr, TypeFlags);
      }
    }
    dstr = myDoc->GetTypeLabel(myDECL, false);
    valNewTypeType = QString(dstr.c);
  }
  if (myDECL->ParentDECL->TypeFlags.Contains(isStatic)) 
    m_SameAsSelf->setEnabled(false);
  if (execAllPatt)
    delete execAllPatt;
  SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
  //SetSelections();  
  if (myDECL->SecondTFlags.Contains(funcImpl) || myDECL->TypeFlags.Contains(isGUI)) {
    m_NewTypeType->setEnabled(false);
    m_NewName->setEnabled(false);
    m_NamedTypes->setEnabled(false);
    m_BasicTypes->setEnabled(false);
    m_Mandatory->setEnabled(false);
    m_Optional->setEnabled(false);
    m_StateObject1->setEnabled(false);
    m_ValueObject1->setEnabled(false);
    m_AnyCategory->setEnabled(false);
    m_Substitutable1->setEnabled(false);
  }
  if (myDECL->SecondTFlags.Contains(funcImpl)
      || (myDECL->ParentDECL->DeclType != Function)
      || myDECL->TypeFlags.Contains(isGUI)
      || myDECL->ParentDECL->TypeFlags.Contains(isInitializer))
    m_SameAsSelf->setEnabled(false);
  if (myDECL->SecondTFlags.Contains(overrides) || myDECL->TypeFlags.Contains(isGUI)) 
    m_Substitutable1->setEnabled(false);

  UpdateData(false);
  m_NewName->setFocus();
  return BoxContinue;
}

void CIOBox::on_m_NamedTypes4_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  bool catErr;

  m_BasicTypes->setCurrentItem(0);
  if (SelEndOKToStr(m_NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    myDECL->DeclDescType = NamedType;
    SynFlags inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr); 
    m_StateObject1->setEnabled(!inheritedFlag.Contains(definesObjCat));
    m_ValueObject1->setEnabled(!inheritedFlag.Contains(definesObjCat));
    m_SameAsSelf->setEnabled(!inheritedFlag.Contains(definesObjCat));
    m_AnyCategory->setEnabled(!inheritedFlag.Contains(definesObjCat));
    
    if (inheritedFlag.Contains(definesObjCat)) {
      m_SameAsSelf->setChecked(false);
      if (myDECL->TypeFlags.Contains(trueObjCat)) {
        if (myDECL->TypeFlags.Contains(stateObject)) {
          m_StateObject1->setChecked(true);
          m_ValueObject1->setChecked(false);
          m_AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
          m_StateObject1->setChecked(false);
          m_ValueObject1->setChecked(false);
          m_AnyCategory->setChecked(true);
        }
        else {
          m_StateObject1->setChecked(false);
          m_ValueObject1->setChecked(true);
          m_AnyCategory->setChecked(false);
        }
      }
      else {
        m_StateObject1->setChecked(false);
        m_ValueObject1->setChecked(false);
        m_AnyCategory->setChecked(false);
      }
    }
  }
  else 
    SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}

void CIOBox::on_m_BasicTypes4_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  m_NamedTypes->setCurrentItem(0);
  int num = SelEndOKToStr(m_BasicTypes, &valNewTypeType, &myDECL->RefID);
  if (num > 0) {
    myDECL->DeclDescType = BasicType;
    myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
  }
  else 
    SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}

/*
bool CIOBox::SetSelections()
{
  m_BasicTypes->setCurrentItem(0);
  m_NamedTypes->setCurrentItem(0);
  int cb1= CB_ERR, cb2= CB_ERR;
  if (myDECL->DeclDescType == BasicType) 
    cb1 = m_BasicTypes->setCurrentText(valNewTypeType);   
  else
    cb2 = m_NamedTypes->setCurrentText(valNewTypeType);
  return (cb1 != CB_ERR) || (cb2 != CB_ERR);
}*/


void CIOBox::on_m_Substitutable1_clicked() 
{
  UpdateData(true);
  if (m_Substitutable1->isOn())
    TypeFlags.INCL(substitutable);
  else
    TypeFlags.EXCL(substitutable);
  ResetComboItems(m_BasicTypes);
  myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
  ResetComboItems(m_NamedTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
  delete execAllPatt;
  if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
//  if (!SetSelections())
    valNewTypeType = QString("");
  UpdateData(false);
}


void CIOBox::OnOK() 
{
  UpdateData(true);
  if (myDECL->SecondTFlags.Contains(funcImpl)) {
    QDialog::reject();
    return;
  }
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
//    m_NewName->SetSel(0, -1);
    m_NewName->setFocus();
    return;
  }
  if (!valNewTypeType.length()) {
    QMessageBox::critical(this,qApp->name(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    m_BasicTypes->setFocus();
    return;
  }
  myDECL->TypeFlags.EXCL(isOptional);
  if (m_Substitutable1->isOn()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (valkindOfField == 1) 
    myDECL->TypeFlags.INCL(isOptional);
  if (m_StateObject1->isOn())
    myDECL->TypeFlags.INCL(stateObject);
  else
    myDECL->TypeFlags.EXCL(stateObject);
  if (m_SameAsSelf->isOn())
    myDECL->TypeFlags.INCL(sameAsSelf);
  else
    myDECL->TypeFlags.EXCL(sameAsSelf);
  if (m_AnyCategory->isOn())
    myDECL->TypeFlags.INCL(isAnyCategory);
  else
    myDECL->TypeFlags.EXCL(isAnyCategory);

  QDialog::accept();
}



/////////////////////////////////////////////////////////////////////////////
// CPackageBox dialog


CPackageBox::CPackageBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CPackageBox::CPackageBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
   setupUi(this);
 myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  isTop = (OrigDECL == ((CHESimpleSyntax*)myDoc->mySynDef->SynDefTree.first)->data.TopDef.ptr);
  if (!onNew) 
    valNewName = QString(myDECL->LocalName.c);
}

void CPackageBox::UpdateData(bool getData)
{
  if (getData) 
    valNewName = m_NewName->text();
  else 
    m_NewName->setText(valNewName);
}

ValOnInit CPackageBox::OnInitDialog() 
{
  CHETID *ncheS, *cheS;
  LavaDECL* decl;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    PushButton_IDOK10->setEnabled(false);
    PushButton_IDOK10->setDefault( false );
    mANCEL10->setDefault( true );
  }

  if (!onNew && myDECL->LocalName.l) {
    valNewName = QString(myDECL->LocalName.c);
    if (myDECL->TypeFlags.Contains(isProtected))
      m_Opaque->setChecked(true);
    else
      m_Opaque->setChecked(false);
    cheS = (CHETID*)myDECL->Supports.first; //extends
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) {
          item = new CListBoxItem(decl->FullName, cheS->data);
          m_Extends->insertItem(item);
          cheS = (CHETID*)cheS->successor;
        }
        else {
          ncheS = (CHETID*)cheS->successor;
          myDECL->Supports.Uncouple(cheS);
          delete cheS;
          cheS = ncheS;
          if (myDoc->AutoCorrBox(&ERR_NoBaseIF) != QDialog::Accepted) {
            //QDialog::reject();
            return BoxCancel; ;
          }
        }
      }
      else
        cheS = (CHETID*)cheS->successor;
    }
  }
  CExecDefs * execDefs = new CExecDefs(myDoc, m_ExtTypes, OrigDECL);
  delete execDefs;
  m_NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}


void CPackageBox::on_m_DelSupport1_clicked() 
{
  int pos = m_Extends->currentItem();
  if (pos >= 0) 
    m_Extends->removeItem(pos);
}

void CPackageBox::on_m_ExtTypes1_triggered(int pos) 
{
  if (!pos) return;
  SelEndOKToList(m_ExtTypes, m_Extends);
}

void CPackageBox::OnOK() 
{
  UpdateData(true);
  ListToChain(m_Extends, &myDECL->Supports);
  if (m_Opaque->isOn())
    myDECL->TypeFlags.INCL(isProtected);
  else
    myDECL->TypeFlags.EXCL(isProtected);
  if (isTop && !valNewName.length() && !myDECL->Supports.first) {
    myDECL->FullName.Destroy();
    myDECL->LocalName.Destroy();
  }
  else  {
    QString* ids = CheckNewName(valNewName, myDECL, myDoc);
    if (ids) {
      QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
      //m_NewName->SetSel(0, -1);
      m_NewName->setFocus();
      return;
    }
  }
  QDialog::accept();
}

/////////////////////////////////////////////////////////////////////////////
// CSetBox dialog


CSetBox::CSetBox(QWidget* parent )
  : QDialog(parent)
{
}

CSetBox::CSetBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
}

void CSetBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    valExtend = m_Extend->text();
  }
  else {
    m_NewName->setText(valNewName);
    m_Extend->setText(valExtend);
  }
}

ValOnInit CSetBox::OnInitDialog() 
{
  SynFlags typeflag;

  if (myDoc->changeNothing) {
    PushButton_IDOK3->setEnabled(false);
    PushButton_IDOK3->setDefault( false );
    PushButton_IDCANCEL3->setDefault( true );
  }
  CExecAllDefs * execAllPatt = new CExecAllDefs(myDoc, m_ExTypes, 0, myDECL->ParentDECL, OrigDECL,  Attr, typeflag);
  myDoc->MakeBasicBox(m_BasicTypes, NoDef, false);
  delete execAllPatt;
  /* 
    ---->remember: this box is used only in case of a new set
  */
  m_BasicTypes->setCurrentItem(0);
  m_ExTypes->setCurrentItem(0);
  m_NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}


void CSetBox::on_m_ExTypes_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  m_BasicTypes->setCurrentItem(0);
  if (SelEndOKToStr(m_ExTypes, &valExtend, &exID) > 0)
    UpdateData(false);
  else
    if (exID.nINCL == 1)
      m_BasicTypes->setCurrentText(valExtend);
    else
      m_ExTypes->setCurrentText(valExtend);
}

void CSetBox::on_m_BasicTypes_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  m_ExTypes->setCurrentItem(0);
  if (SelEndOKToStr(m_BasicTypes, &valExtend, &exID) > 0)
    UpdateData(false);
  else
    if ((exID.nINCL == 1) || myDoc->isStd)
      m_BasicTypes->setCurrentText(valExtend);
    else
      m_ExTypes->setCurrentText(valExtend);
}


void CSetBox::OnOK() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    m_NewName->setFocus();
    //m_NewName->SetSel(0, -1);
    return;
  }
  if (valExtend.length()) {
    myDoc->MakeSet(exID, myDECL);
    myDECL->TreeFlags.INCL(isExpanded);
    myDECL->TreeFlags.INCL(ParaExpanded);
  }
  else {
    QMessageBox::critical(this,qApp->name(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    m_BasicTypes->setFocus();
    m_ExTypes->setCurrentItem(0);
    m_BasicTypes->setCurrentItem(0);
    return;
  }
  myDECL->WorkFlags.INCL(recalcVT);
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CVTypeBox dialog


CVTypeBox::CVTypeBox(QWidget* parent)
  : QDialog(parent)
{
}

CVTypeBox::CVTypeBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent, Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  second = false;
}

void CVTypeBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = m_NewName->text();
    valNewTypeType = m_NewTypeType1->text();
    if (BGroup_KindOfRef->find(0)->isOn())
      valkindOfLink = 0;
    else if (BGroup_KindOfRef->find(1)->isOn())
      valkindOfLink = 1;
    else
      valkindOfLink = 2;
 }
  else {
    m_NewName->setText(valNewName);
    m_NewTypeType1->setText(valNewTypeType);
    BGroup_KindOfRef->setButton(valkindOfLink);
  }
}

ValOnInit CVTypeBox::OnInitDialog() 
{
  bool baseAbstract = true;
  CExecAllDefs * execAllPatt=0;
  SynFlags typeflag;
  LavaDECL* decl = 0;
  DString dstr;
  CHETID* che;

  if (myDoc->changeNothing) {
    PushButton_IDOK16->setEnabled(false);
    PushButton_IDOK16->setDefault( false );
    PushButton_IDCANCEL16->setDefault( true );
  }
  if (second) {
    ResetComboItems(m_NamedTypes);
    ResetComboItems(m_BasicTypes);
  }
  
  if (onNew) {
    myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
    execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, typeflag);
    if ((myDECL->ParentDECL->DeclType == Interface)
    && !myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      m_isAbstract->setEnabled(false);
      valkindOfLink = 0;
    }
  }
  else {
    valNewName = myDECL->LocalName.c;
    dstr = myDoc->GetTypeLabel(myDECL, false);
    valNewTypeType = QString(dstr.c);
    che = (CHETID*)myDECL->Supports.first;
    while (che) {
      decl = myDoc->IDTable.GetDECL(che->data);
      if (!decl) {
        CCorrOverBox * box = new CCorrOverBox(myDECL, myDoc, parentWidget());
        if (box->OnInitDialog() == BoxContinue) {
          if (box->exec() != QDialog::Accepted) 
            return BoxCancel;  //QDialog::reject();
          return BoxOK; //QDialog::accept();
        }
        else
          return BoxCancel;
        return BoxOK;
      }
      else {
        baseAbstract = baseAbstract && decl->TypeFlags.Contains(isAbstract);
        che = (CHETID*)che->successor;
      }
    } 
    decl = 0;
    che = (CHETID*)myDECL->Supports.first;
    if (myDECL->TypeFlags.Contains(substitutable)) {
      m_Substitutable->setChecked(true);
      if (myDECL->SecondTFlags.Contains(overrides) && !baseAbstract) {
        m_NamedTypes->setEnabled(false);
        m_BasicTypes->setEnabled(false);
      }
    }
    else
      m_Substitutable->setChecked(false); 
    if (che && (che == (CHETID*)myDECL->Supports.last)) 
      decl = myDoc->IDTable.GetDECL(che->data);
    if ((myDECL->SecondTFlags.Contains(isSet) || myDECL->SecondTFlags.Contains(isArray))
      && !myDECL->TypeFlags.Contains(isAbstract)) {
      if (decl->TypeFlags.Contains(isAbstract)) {
        m_DownC->setEnabled(true);
        m_DownInd->setEnabled(true);
        m_Back->setEnabled(true);
      }
      if (myDECL->TypeFlags.Contains(constituent)) 
        valkindOfLink = 0;
      else 
        if (myDECL->TypeFlags.Contains(acquaintance))
          valkindOfLink = 1;
        else
          valkindOfLink = 2;
    }
    if (myDECL->SecondTFlags.Contains(overrides)) {
      m_RMOverrides1->setEnabled(true);
      m_EnableName1->setEnabled(true);
      if (myDECL->SecondTFlags.Contains(enableName)) {
        m_EnableName1->setChecked(true);
        m_NewName->setEnabled(true);
      }
      else {
        m_EnableName1->setChecked(false);
        m_NewName->setEnabled(false);
      }
    }
    if ((myDECL->SecondTFlags.Contains(overrides))  && !myDECL->TypeFlags.Contains(isAbstract)
      && (!decl || !decl->TypeFlags.Contains(isAbstract))) { //and not only overriding an abstract virtual type
      CExecExtensions *exec = new CExecExtensions(myDoc, m_NamedTypes, m_BasicTypes, myDECL, OrigDECL);
      delete exec;
      m_isAbstract->setEnabled(false);
    }
    else {
      myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
      execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, myDECL->TypeFlags);
    }
    if ((myDECL->ParentDECL->DeclType == Package) 
        || myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      m_isAbstract->setEnabled(true);
      if (myDECL->TypeFlags.Contains(isAbstract)) {
        m_isAbstract->setChecked(true);
        m_NamedTypes->setEnabled(false);
        m_BasicTypes->setEnabled(false);
        m_Substitutable->setEnabled(false);
      }
    }
    else {
      m_Substitutable->setEnabled(!myDECL->SecondTFlags.Contains(overrides) || baseAbstract);
      m_isAbstract->setEnabled(false);
      m_isAbstract->setChecked(false);
    }
    SetCategoryChecks();
  }
  if (execAllPatt)
    delete execAllPatt;
  SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
  //SetSelections();  
  UpdateData(false);
  m_NewName->setFocus();
  return BoxContinue;
}

void CVTypeBox::SetCategoryChecks()
{
  bool catErr;
  if (myDECL->TypeFlags.Contains(definesObjCat)) 
    m_DefCat->setChecked(true);
  else 
    m_DefCat->setChecked(false);
  inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr);;
  if (inheritedFlag.Contains(definesObjCat))
    m_DefCat->setEnabled(false);
  else
    m_DefCat->setEnabled(true);
  if (inheritedFlag.Contains(trueObjCat)) {
    if (myDECL->TypeFlags.Contains(stateObject)) {
      m_StateObject->setChecked(true);
      m_ValueObject->setChecked(false);
      m_AnyCategory->setChecked(false);
    }
    else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
      m_StateObject->setChecked(false);
      m_ValueObject->setChecked(false);
      m_AnyCategory->setChecked(true);
    }
    else {
      m_StateObject->setChecked(false);
      m_ValueObject->setChecked(true);
      m_AnyCategory->setChecked(false);
    }
    m_StateObject->setEnabled(false);
    m_ValueObject->setEnabled(false);
    m_AnyCategory->setEnabled(false);
  }
  else {
    if (myDECL->TypeFlags.Contains(isAbstract)) 
      if (myDECL->Supports.first && !inheritedFlag.Contains(isAbstract))
        myDECL->TypeFlags.EXCL(isAbstract);
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      m_StateObject->setChecked(false);
      m_ValueObject->setChecked(false);
      m_AnyCategory->setChecked(false);
      m_StateObject->setEnabled(false);
      m_ValueObject->setEnabled(false);
      m_AnyCategory->setEnabled(false);
   }
    else {
      if (myDECL->TypeFlags.Contains(definesObjCat)) {
        if (myDECL->TypeFlags.Contains(stateObject)) {
          m_StateObject->setChecked(true);
          m_ValueObject->setChecked(false);
          m_AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCategory)) {
          m_StateObject->setChecked(false);
          m_ValueObject->setChecked(false);
          m_AnyCategory->setChecked(true);
        }
        else {
          m_StateObject->setChecked(false);
          m_ValueObject->setChecked(true);
          m_AnyCategory->setChecked(false);
        }
        m_StateObject->setEnabled(true);
        m_ValueObject->setEnabled(true);
        m_AnyCategory->setEnabled(true);
      }
      else {
        m_StateObject->setChecked(false);
        m_ValueObject->setChecked(false);
        m_AnyCategory->setChecked(false);
      }
    }
  }
}


void CVTypeBox::on_m_RMOverrides1_clicked() 
{
  UpdateData(true);
  m_RMOverrides1->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);
  second = true;
  OnInitDialog();
}

void CVTypeBox::on_m_NamedTypes2_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  m_BasicTypes->setCurrentItem(0);
  if (SelEndOKToStr(m_NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      myDECL->TypeFlags.EXCL(isAbstract);
      valkindOfLink = 0;
      m_DownC->setEnabled(true);
      m_DownInd->setEnabled(true);
      m_Back->setEnabled(true);
      SetCategoryChecks();
    }
    myDECL->DeclDescType = NamedType;
    SetCategoryChecks();
  }
  else 
   SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
  // SetSelections();
  UpdateData(false);
}

void CVTypeBox::on_m_BasicTypes2_triggered(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  m_NamedTypes->setCurrentItem(0);
  int num = SelEndOKToStr(m_BasicTypes, &valNewTypeType, &myDECL->RefID);
  if (num > 0) {
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      myDECL->TypeFlags.EXCL(isAbstract);
      m_DownC->setEnabled(true);
      m_DownInd->setEnabled(true);
      m_Back->setEnabled(true);
      SetCategoryChecks();
      valkindOfLink = 0;
    }
    myDECL->DeclDescType = BasicType;
    myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
    SetCategoryChecks();
  }
  else 
    SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}


void CVTypeBox::on_m_isAbstract_clicked() 
{
  UpdateData(true);
  bool abs = m_isAbstract->isOn();
  m_BasicTypes->setCurrentItem(0);
  m_NamedTypes->setCurrentItem(0);
  if (!abs) {
    myDECL->TypeFlags.EXCL(isAbstract);
    m_Substitutable->setEnabled(!myDECL->SecondTFlags.Contains(overrides) || baseAbstract);
    valkindOfLink = 0;
  }
  else {
    myDECL->TypeFlags.INCL(isAbstract);
    myDECL->TypeFlags.EXCL(substitutable);
    m_Substitutable->setChecked(false);
    m_Substitutable->setEnabled(false);
  }
  m_NamedTypes->setEnabled(!abs);
  m_BasicTypes->setEnabled(!abs);
  SetCategoryChecks();

  valNewTypeType = QString("");
  UpdateData(false);
}


void CVTypeBox::on_m_DefCat_clicked() 
{
  UpdateData(true);
  bool defCat = m_DefCat->isOn()
                && !inheritedFlag.Contains(trueObjCat)
                && !myDECL->TypeFlags.Contains(isAbstract);
  m_StateObject->setEnabled(defCat);
  m_ValueObject->setEnabled(defCat);
  m_AnyCategory->setEnabled(defCat);
  if (defCat) {
    m_ValueObject->setChecked(true);
    myDECL->TypeFlags.INCL(definesObjCat);
  }
  else {
    m_ValueObject->setChecked(false);
    m_StateObject->setChecked(false);
    myDECL->TypeFlags.EXCL(definesObjCat);
  }
  UpdateData(false);  
}


void CVTypeBox::on_m_StateObject_clicked() 
{
  UpdateData(true);
  if (m_StateObject->isOn())
    myDECL->TypeFlags.INCL(stateObject);
  else
    myDECL->TypeFlags.EXCL(stateObject);
}


void CVTypeBox::on_m_Substitutable_clicked() 
{
  UpdateData(true);
  bool sbst = m_Substitutable->isOn();
  if (sbst)
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  ResetComboItems(m_BasicTypes);
  myDoc->MakeBasicBox(m_BasicTypes, NoDef, true);
  ResetComboItems(m_NamedTypes);
  CExecAllDefs* execAllPatt = new CExecAllDefs(myDoc, m_NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, myDECL->TypeFlags);
  delete execAllPatt;
  if (!SetSelections(m_BasicTypes, m_NamedTypes, valNewTypeType))
    valNewTypeType = QString("");
  UpdateData(false);
}

void CVTypeBox::on_m_ValueObject_clicked() 
{
  UpdateData(true);
  if (m_ValueObject->isOn())
    myDECL->TypeFlags.EXCL(stateObject);
}

void CVTypeBox::on_m_EnableName1_clicked() 
{
  UpdateData(true);
  bool enabl = m_EnableName1->isOn();
  if (enabl) 
    m_NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    m_NewName->setEnabled(false);
  }
  UpdateData(false);  
}

void CVTypeBox::OnOK() 
{
  UpdateData(true);
  bool extendsSet = false, extendsArray = false, extendsException = false, extendsChain = false,
     extendsEnum = false;
  LavaDECL* suDECL;
  CHETID* cheS;

  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->name(),*ids,QMessageBox::Ok,0,0);
    //m_NewName->SetSel(0, -1);
    m_NewName->setFocus();
    return;
  }
  if (m_EnableName1->isOn()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (m_DefCat->isOn()) {
    myDECL->TypeFlags.INCL(definesObjCat);
    myDECL->TypeFlags.INCL(trueObjCat);
    if (m_StateObject->isOn()) 
      myDECL->TypeFlags.INCL(stateObject);
    else 
      myDECL->TypeFlags.EXCL(stateObject);
  }
  else {
    myDECL->TypeFlags.EXCL(stateObject);
    myDECL->TypeFlags.EXCL(definesObjCat);
    myDECL->TypeFlags.EXCL(trueObjCat);
  }
  if (m_Substitutable->isOn()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (m_isAbstract->isOn()) {
    myDECL->TypeFlags.INCL(isAbstract);
    myDECL->TypeFlags.EXCL(trueObjCat);
    myDECL->TypeFlags.EXCL(stateObject);
    myDECL->RefID  = TID(-1,0);
    myDECL->DeclDescType = NamedType;
  }
  else 
    myDECL->TypeFlags.EXCL(isAbstract);
  if (!valNewTypeType.length() && !myDECL->TypeFlags.Contains(isAbstract)) {
    QMessageBox::critical(this,qApp->name(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    m_BasicTypes->setFocus();
    return;
  }
  cheS = (CHETID*)myDECL->Supports.first;
  if (cheS) {
    while (cheS) {
      suDECL = myDoc->IDTable.GetDECL(cheS->data);//!!
      if (suDECL) {
        extendsSet = extendsSet || suDECL->SecondTFlags.Contains(isSet);
        extendsArray = extendsArray || suDECL->SecondTFlags.Contains(isArray);
        extendsException = extendsException || suDECL->SecondTFlags.Contains(isException);
        extendsChain = extendsChain || suDECL->SecondTFlags.Contains(isChain);
      }
      cheS = (CHETID*)cheS->successor;
    }
    if (extendsSet)
      myDECL->SecondTFlags.INCL(isSet);
    else
      myDECL->SecondTFlags.EXCL(isSet);
    if (extendsArray)
      myDECL->SecondTFlags.INCL(isArray);
    else
      myDECL->SecondTFlags.EXCL(isArray);
    if (extendsException)
      myDECL->SecondTFlags.INCL(isException);
    else
      myDECL->SecondTFlags.EXCL(isException);
    if (extendsChain)
      myDECL->SecondTFlags.INCL(isChain);
    else
      myDECL->SecondTFlags.EXCL(isChain);
  }
  if ((myDECL->SecondTFlags.Contains(isSet) || myDECL->SecondTFlags.Contains(isArray))
    && !myDECL->TypeFlags.Contains(isAbstract)) {
    myDECL->TypeFlags.EXCL(constituent);
    myDECL->TypeFlags.EXCL(acquaintance);
    if (valkindOfLink == 0) 
      myDECL->TypeFlags.INCL(constituent);
    else 
      if (valkindOfLink == 1) 
        myDECL->TypeFlags.INCL(acquaintance);
  }
  QDialog::accept();
}



void CAttrBox::on_ID_HELP15_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/MemVarBox.htm");
}

void CCompSpecBox::on_ID_HELP10_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/COSpecBox.htm");
}

void CEnumBox::on_ID_HELP3_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/EnumBox.htm");
}

void CEnumItem::on_ID_HELP4_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/EnumItemBox.htm");
}

void CFuncBox::on_ID_HELP9_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/FunctionBox.htm");
}

void CImplBox::on_ID_HELP5_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/ImplementationBox.htm");
}

void CIncludeBox::on_ID_HELP12_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/IncludeBox.htm");
}

void CInitBox::on_ID_HELP11_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/InitiatorBox.htm");
}

void CInterfaceBox::on_ID_HELP7_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/InterfaceBox.htm");
}

void CIOBox::on_ID_HELP14_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/FuncParmBox.htm");
}

void CPackageBox::on_ID_HELP8_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/PackageBox.htm");
}

void CSetBox::on_ID_HELP1_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/SetBox.htm");
}

void CVTypeBox::on_ID_HELP13_clicked()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/VirtualTypeBox.htm");
}


//////////////////////////////////////////////////////////////////////////
/*
void ResetComboItems(QComboBox* box)
{
  int pos, c = box->count();
  for (pos = 1; pos < c; pos++)
    box->removeItem(1);
}
*/

bool SetSelections(QComboBox* basics, QComboBox* types, const QString& name)
{
  int pos;
  /*CComboBoxItem *listItem;
  listItem = (CComboBoxItem*)basics->listBox()->findItem(name, Qt::ExactMatch | Qt::CaseSensitive);
  if (listItem) {
    basics->setCurrentText(listItem->text());*/
  pos = basics->findText(name, Qt::MatchExactly | Qt::MatchCaseSensitive);
  if (pos > 0) {
    basics->setCurrentIndex(pos);
    types->setCurrentIndex(0);
    return true;
  }
  pos = types->findText(name, Qt::MatchExactly | Qt::MatchCaseSensitive);
  if (pos > 0) {
    types->setCurrentIndex(pos);
    basics->setCurrentIndex(0);
    return true;
  }
  types->setCurrentIndex(0);
  basics->setCurrentIndex(0);
  return false;
}

QString*  CheckNewName(const QString& valNewName, LavaDECL *myDECL, CLavaPEDoc* myDoc/*, LavaDECL *OrigDECL*/)
{
  if (!((CLavaPEApp*)wxTheApp)->LBaseData.isIdentifier(valNewName)) 
    return &IDP_IsNoID;
  DString fullName;
  LavaDECL *parDECL;
  if (myDECL->ParentDECL && myDECL->ParentDECL->FullName.l)
    fullName = myDECL->ParentDECL->FullName;
  parDECL = myDECL->ParentDECL;
  myDECL->FullName = fullName;
  myDECL->LocalName = STRING(qPrintable(valNewName));
  myDECL->CompleteFullName();
  if (myDoc->FindInSupports(qPrintable(valNewName), myDECL, parDECL, (myDECL->DeclType == Attr) || (myDECL->DeclType == Function)))
    return &ERR_NameInUse;
  else
    return 0;
}


int SelEndOKToList(QComboBox* cbox, Q3ListBox* list, int chpos)
{
  CListBoxItem *listItem;
  QVariant var;
  //CComboBoxItem *comboItem = cbox->listBox()->selectedItem();
  int pos = cbox->currentIndex();
  if (pos > 0) {
    listItem = (CListBoxItem*)list->findItem(cbox->currentText());
    if (listItem) {
      list->setSelected(listItem, true);
      return -1;
    }
    if (chpos >= 0) 
      list->removeItem(chpos);
    var = cbox->itemData(pos);
    listItem = new CListBoxItem(cbox->currentText(), var.value<CComboBoxItem*>()->itemData());
    list->insertItem(listItem);
    return 1;
  }
  return -1;
}

int SelEndOKToStr(QComboBox* cbox, QString* editStr, TID* exID)
{
  QVariant var;
  //CComboBoxItem *comboItem = (CComboBoxItem*)cbox->listBox()->selectedItem();
  //if (cbox->listBox()->index(comboItem) > 0)
  int pos = cbox->currentIndex();
  if (pos > 0) {
    *editStr = cbox->currentText();
    if (exID) {
      var = cbox->itemData(pos);
      *exID = var.value<CComboBoxItem*>()->itemData();
    }
  }
  return pos;
}


void ListToChain(Q3ListBox* list, TSupports* supports)
{
  int pos, icount;
  CHETID *cheS;
  CListBoxItem *listItem;

  supports->Destroy();
  icount = list->count();
  for (pos = 0; pos < icount; pos++) {
    listItem = (CListBoxItem*)list->item(pos);
    cheS = new CHETID;
    supports->Append(cheS);
    cheS->data = listItem->itemData();
  }
}


int CallBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew,
            bool& buildSet, QWidget* parent, bool asFirst) 
{
  int okBox = QDialog::Rejected;
  QDialog * box = 0;
  ValOnInit valIni;
  buildSet = false;
  switch (decl->DeclType) {
  case Attr:
    box = new CAttrBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CAttrBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case CompObjSpec:
    box = new CCompSpecBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CCompSpecBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case EnumDef:
    decl->DeclType = Interface;
    box = new CEnumBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CEnumBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    buildSet = ((CEnumBox*)box)->valBuildSet; 
    break;

  case Function:
    box = new CFuncBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CFuncBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case IAttr:
  case OAttr:
    box = new CIOBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CIOBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case Impl:
  case CompObj:
    box = new CImplBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CImplBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case Initiator:
    box = new CInitBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CInitBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;
  case Interface:
    if (decl->SecondTFlags.Contains(isSet) && isNew) {
      box = new CSetBox(decl, origDECL, doc, isNew, parent);
      valIni = ((CSetBox*)box)->OnInitDialog();
      if (valIni == BoxContinue)
        okBox = box->exec();
      else if (valIni == BoxOK)
        okBox = QDialog::Accepted;
      else
        okBox = QDialog::Rejected;
    }
    else {
      box = new CInterfaceBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CInterfaceBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
      buildSet = ((CInterfaceBox*)box)->valBuildSet;
    }
    break;

  case Package:
    box = new CPackageBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CPackageBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;

  case VirtualType:
    box = new CVTypeBox(decl, origDECL, doc, isNew, parent);
    valIni = ((CVTypeBox*)box)->OnInitDialog();
    if (valIni == BoxContinue)
      okBox = box->exec();
    else if (valIni == BoxOK)
      okBox = QDialog::Accepted;
    else
      okBox = QDialog::Rejected;
    break;
  case FormText:
    box = new CFormTextBox(decl, parent, asFirst);
    okBox = box->exec();
    break;
  default: ;
  }
  if (box)
    delete box;
  return okBox;
}

CExecExtensions::CExecExtensions(CLavaPEDoc* doc, QComboBox* combo, QComboBox* basicCombo, LavaDECL* startDECL, LavaDECL* origDECL)
{
  Combo = combo;
  BasicCombo = basicCombo;
  myDECL = startDECL;
  OrigDECL = origDECL;
  myDoc = doc;
//  sz.cx = 0;
//  maxWidth = 0;
  CHETID* cheBasic;
  CHETID* che = (CHETID*)myDECL->Supports.first;
  LavaDECL* decl;
  while (che) {
    decl = myDoc->IDTable.GetDECL(che->data);
    if (decl && !decl->TypeFlags.Contains(isAbstract)) {
      cheBasic = new CHETID;
      cheBasic->data.nID = decl->RefID.nID;
      cheBasic->data.nINCL = myDoc->IDTable.IDTab[decl->inINCL]->nINCLTrans[decl->RefID.nINCL].nINCL;
//      myDoc->ReduceType(cheBasic->data, decl->inINCL, Refs);
      if (cheBasic->data.nID >= 0)
        BasicIDs.Append(cheBasic);
    }
    che = (CHETID*)che->successor;
  }
  //CFont* font = Combo->GetFont();
  //dc = new CClientDC(Combo->GetParent());
  //dc->SelectObject (font);
  TabTravers = new CTabTraversal(this, myDoc->mySynDef);
  TabTravers->Run(true, true);
  SortCombo(Combo);
  //Combo->SetDroppedWidth(maxWidth+6);
}


void CExecExtensions::ExecDefs (LavaDECL ** pelDef, int )
{
  int pos=0, it, incl;
  LavaDECL *elDef = *pelDef;
  TID id = TID(elDef->OwnID, elDef->inINCL);
  TDeclType elType = elDef->DeclType;
  bool sameContext = false;
  CContext context, con;
  CComboBoxItem *item;

  myDoc->IDTable.GetPattern(elDef, con);
  if (elDef->usableIn(myDECL)
     && ((elDef->DeclType != Interface)
        || !con.oContext
        || myDoc->IDTable.lowerOContext(myDECL, elDef, sameContext) && sameContext
        || myDoc->IDTable.otherOContext(myDECL, elDef)))
    if (BasicIDs.first && myDoc->IsSpecialOf(elDef, BasicIDs, !sameContext) ) {
      if ((elDef->inINCL != 1) && !myDoc->isStd
          || (elDef->fromBType == NonBasic)) {
        myDoc->NextContext(myDECL, context);
        if ((elDef != OrigDECL)
            && (  (elDef->DeclType != VirtualType)
               || (id == myDoc->GetMappedVType(id, context,0))
                  && (myDECL->TypeFlags.Contains(stateObject) == elDef->TypeFlags.Contains(stateObject))
               )) {
          //item = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, elDef->inINCL));
          //Combo->listBox()->insertItem(item);//sort
          item = new CComboBoxItem(TID(elDef->OwnID, elDef->inINCL));
          Combo->addItem(QString(elDef->FullName.c), QVariant::fromValue(item));//sort
        }
      }
      else {
        it = elDef->fromBType;
        if ((it != B_Object) || !BasicIDs.first ) {
          if (myDoc->isStd)
            incl = 0;
          else
            incl = 1;
//          item = new CComboBoxItem(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it], TID(elDef->OwnID, incl));
//          Combo->listBox()->insertItem(item);//sort
          item = new CComboBoxItem(TID(elDef->OwnID, incl));
          Combo->addItem(QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it]), QVariant::fromValue(item));//sort
        }
      }
    }
    else
      if (!BasicIDs.first
          && ( (elType == Interface)
             || (elType == CompObjSpec)
             || (elType == VirtualType) && myDECL->isInSubTree(elDef->ParentDECL) ) ) {
        if ((elDef->inINCL != 1) && !myDoc->isStd || (elDef->fromBType == NonBasic)) {
          if ((elDef != OrigDECL)
            //  && ((myDECL->DeclType != VirtualType) || (elDef->DeclType != VirtualType))
        ) {
          //item = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, elDef->inINCL));
          //Combo->listBox()->insertItem(item);//sort
            item = new CComboBoxItem(TID(elDef->OwnID, elDef->inINCL));
            Combo->addItem(QString(elDef->FullName.c), QVariant::fromValue(item));//sort
          }
        }
        else {
          it = elDef->fromBType;
          if (myDoc->isStd)
            incl = 0;
          else
            incl = 1;
//          item = new CComboBoxItem(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it], TID(elDef->OwnID, incl));
//          Combo->listBox()->insertItem(item);//sort
          item = new CComboBoxItem(TID(elDef->OwnID, incl));
          Combo->addItem(QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it]), QVariant::fromValue(item));//sort
        }
      }
}

 
CExecBase::CExecBase(CInterfaceBox* box)
{
  IBox = box;
  TabTravers = new CTabTraversal(this, IBox->myDoc->mySynDef);
  TabTravers->Run(true, true);
  SortCombo(IBox->m_ExtTypes);
}

void CExecBase::ExecDefs(LavaDECL ** pelDef, int level)
{
  if ((*pelDef)->fromBType != NonBasic)
    return;
  LavaDECL *elDef = *pelDef;
  DString lab;
  bool sameContext;
  CContext con;
  CComboBoxItem *listItem;

  if ((elDef->DeclType != Interface) && (elDef->DeclType != VirtualType))
    return;
  if (!elDef->usableIn(IBox->myDECL->ParentDECL))
    return;
  if (elDef == IBox->OrigDECL)
    return;
  IBox->myDoc->IDTable.GetPattern(elDef, con);
  if ( IBox->myDoc->IDTable.InsertBase(IBox->myDECL, elDef, IBox->ContextDECL, false)
        && (IBox->myDECL->TypeFlags.Contains(isComponent) == elDef->TypeFlags.Contains(isComponent))
        && (!con.oContext
            || (con.oContext == elDef)
            || (con.oContext == IBox->OrigDECL)
            || IBox->myDoc->IDTable.lowerOContext(IBox->myDECL->ParentDECL, elDef, sameContext) && sameContext)) {
    lab = elDef->FullName;
    if (elDef->DeclType == VirtualType) {
      lab.Insert(grthen, lab.l);
      lab.Insert(lthen, 0);
    }
    //listItem = new CComboBoxItem(lab, TID(elDef->OwnID, elDef->inINCL));
    //IBox->m_ExtTypes->listBox()->insertItem(listItem);//sort
    listItem = new CComboBoxItem(TID(elDef->OwnID, elDef->inINCL));
    IBox->m_ExtTypes->addItem(QString(lab.c), QVariant::fromValue(listItem));//sort
    //sz = dc->GetTextExtent(lab.c, lab.l);
    //maxWidth = lmax(maxWidth, sz.cx);
  }
}
  
CExecDefs::CExecDefs(CLavaPEDoc* doc, QComboBox* combo, LavaDECL* decl)
{
  Combo = combo;
  myDECL = decl;
  myDoc = doc;
  TabTravers = new CTabTraversal(this, myDoc->mySynDef);
  TabTravers->Run(true, true);
  SortCombo(Combo);
}


void CExecDefs::ExecDefs (LavaDECL ** pelDef, int )
{
  CComboBoxItem *listItem;

  if ( ((*pelDef)->DeclType == Package)
      && (*pelDef)->LocalName.l
      && !myDECL->isInSubTree(*pelDef)
      && !(*pelDef)->isInSubTree(myDECL)) {
    CHE* che = (CHE*)(*pelDef)->NestedDecls.first;
    if (che && (((LavaDECL*)che->data)->DeclType == VirtualType)) { 
//      listItem = new CComboBoxItem((*pelDef)->FullName, TID((*pelDef)->OwnID, (*pelDef)->inINCL));
//      Combo->listBox()->insertItem(listItem);//sort
      listItem = new CComboBoxItem(TID((*pelDef)->OwnID, (*pelDef)->inINCL));
      Combo->addItem(QString((*pelDef)->FullName.c), QVariant::fromValue(listItem));//sort
    }
  }
} 


CExecAllDefs::CExecAllDefs(CLavaPEDoc* doc, 
                           QComboBox* combo, QComboBox* combo2, //QComboBox* list3,
                           LavaDECL *parentDECL, LavaDECL *callingDECL,
                           TDeclType deftype, SynFlags typeFlag)
{
//  CFont* font;

  Combo = combo;
  Combo2 = combo2;
  //List3 = list3;
  CallingDECL = callingDECL;
  ParentDECL = parentDECL;
  IntfDECL = ParentDECL;
  TypeFlags = typeFlag;
  DeclType = deftype; 
  myDoc = doc;
  if (ParentDECL->DeclType == Function) { //IO
    IntfDECL = ParentDECL->ParentDECL;
    ClassID = TID(IntfDECL->OwnID, ParentDECL->inINCL);
  }
  else 
    ClassID = TID(ParentDECL->OwnID, ParentDECL->inINCL);
  if (IntfDECL && (IntfDECL->DeclType == Impl) && ((DeclType == Attr) || (DeclType == VirtualType)))
    IntfDECL = myDoc->IDTable.GetDECL(((CHETID*)IntfDECL->Supports.first)->data, IntfDECL->inINCL);
//  Flag = flag;
  TabTravers = new CTabTraversal(this, myDoc->mySynDef);
  TabTravers->Run(true, true);
  if (Combo) 
    FitBox(Combo, 0); 
  if (Combo2)
    FitBox(Combo2, 0); //GUI-Box only
  /*
  if (List3)
    FitBox(List3, 0);
    */
  //delete dc;
}


void CExecAllDefs::FitBox(QComboBox* combo, int maxWidth)
{
  QString strA, strB;
  bool nowB = false;
  CComboBoxItem *listItemA, *listItemB;
  LavaDECL* decl;
  int pos, posA, posB, icount = combo->count();
  QVariant var;

  for (pos = 2; pos < icount; pos++) {
    if (nowB) {
      var = combo->itemData(pos);
      listItemB = var.value<CComboBoxItem*>();
      strB = combo->itemText(pos);
      posB = pos;
    }
    else {
      var = combo->itemData(pos);
      listItemA = var.value<CComboBoxItem*>();
      strA = combo->itemText(pos);
      posA = pos;
    }
    if (strB == strA) {
      decl = myDoc->IDTable.GetDECL(listItemA->itemData());
      listItemA = new CComboBoxItem(listItemA->itemData());
      combo->setItemText(posA, QString(decl->FullName.c));
      combo->setItemData(posA, QVariant::fromValue(listItemA));
      //combo->listBox()->changeItem(listItemA, posA);
      //sz = dc->GetTextExtent(decl->FullName.c, decl->FullName.l);
      //maxWidth = lmax(maxWidth, sz.cx);
      decl = myDoc->IDTable.GetDECL(listItemB->itemData());
      //listItemB = new CComboBoxItem(decl->FullName, listItemB->itemData());
      //combo->listBox()->changeItem(listItemB, posB);
      listItemB = new CComboBoxItem(listItemB->itemData());
      combo->setItemText(posB, QString(decl->FullName.c));
      combo->setItemData(posB, QVariant::fromValue(listItemB));
      //sz = dc->GetTextExtent(decl->FullName.c, decl->FullName.l);
      //maxWidth = lmax(maxWidth, sz.cx);
    }
    nowB = !nowB;
  }
  SortCombo(combo);
  combo->setCurrentItem(0);
  //if (maxWidth)
  //  list->SetDroppedWidth(maxWidth+6);
  
}


void CExecAllDefs::ExecDefs (LavaDECL ** pelDef, int incl)
{
  LavaDECL *elDef = *pelDef, *decl = elDef;
  if ((elDef->fromBType != NonBasic)
    && ((DeclType != Function)
        || (elDef->DeclType != Interface)
        || !elDef->SecondTFlags.Contains(isException) ))
    return;
  TID pID;
  bool putIt = false, sameContext;
  CContext con;
  CComboBoxItem *listItem;
  TDeclType elType = elDef->DeclType;
  DString lab, LocalName;

  //if (TypeFlags.Contains(substitutable)
  //    && !myDoc->IDTable.otherOContext(elDef, ParentDECL))
  //  return;

  if (CallingDECL && (CallingDECL->DeclType == VirtualType) && (elType == VirtualType)) 
    for  ( ;decl && (decl->DeclType == VirtualType) && (decl != CallingDECL); 
            decl = myDoc->IDTable.GetDECL(decl->RefID, decl->inINCL));
  if (!elDef->usableIn(ParentDECL))
      return;
  if ((elDef == CallingDECL) || (decl == CallingDECL) || (elType == UnDef))
    return;
  myDoc->IDTable.GetPattern(elDef, con);
  if (DeclType == Attr) {
    if (elType == Interface)  //type for attr, iattr, oattr
      putIt = !myDoc->TypeForMem(ParentDECL, elDef, 0);
    else
      if ((elType == VirtualType) 
        && myDoc->IDTable.lowerOContext(ParentDECL, elDef, sameContext))
        putIt = (myDoc->CheckGetFinalMType(ParentDECL, elDef) == elDef);
  }
  else if (DeclType == Function) {
    if ((elType == Interface) && elDef->SecondTFlags.Contains(isException))
      putIt = !myDoc->TypeForMem(ParentDECL, elDef, 0);
    else {
      if ((elType == VirtualType) && myDoc->IDTable.lowerOContext(ParentDECL, elDef, sameContext)) {
        putIt = (myDoc->CheckGetFinalMType(ParentDECL, elDef) == elDef);
        if (putIt) {
          decl = myDoc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
          putIt = decl && decl->SecondTFlags.Contains(isException);
        }
      }
    }
    if (putIt && CallingDECL) 
      putIt = myDoc->AllowThrowType(CallingDECL, TID(elDef->OwnID, elDef->inINCL), 0);
  }
  else if ((DeclType == VirtualType) && ((elType == Interface) || (elType == VirtualType)))
    putIt = !myDoc->TestValOfVirtual(ParentDECL, elDef);
  else if ((DeclType == Interface) && (elType == Interface)) { //extension of
    putIt = (TypeFlags.Contains(isComponent) == elDef->TypeFlags.Contains(isComponent))
            && (!con.oContext
               || (con.oContext == elDef)
               || myDoc->IDTable.lowerOContext(ParentDECL, elDef, sameContext) && sameContext);
//             || (DeclType == CompObjSpec) && (elType == Interface) && elDef->TypeFlags.Contains(isComponent)
//             || (DeclType == Package) && (elType == Package);
    putIt = putIt && (!CallingDECL || !myDoc->IDTable.IsAn(elDef, TID(CallingDECL->OwnID, CallingDECL->inINCL), 0)
                  && !myDoc->IDTable.IsAn(CallingDECL, TID(elDef->OwnID, elDef->inINCL), 0));
  }
  else if ((DeclType == Interface) && (elType == VirtualType)) 
    if (CallingDECL) {
      putIt = myDoc->IDTable.lowerOContext(CallingDECL, elDef, sameContext)
              && sameContext && (myDoc->CheckGetFinalMType(CallingDECL, elDef) == elDef);
      putIt = putIt && !myDoc->IDTable.IsAn(CallingDECL, TID(elDef->OwnID, elDef->inINCL), 0);
    }
    else
      putIt = myDoc->IDTable.lowerOContext(ParentDECL, elDef, sameContext)
              && sameContext && (myDoc->CheckGetFinalMType(ParentDECL, elDef) == elDef);
  else if ((DeclType == Impl) && (elType == Interface)) 
    putIt = (*pelDef)->inINCL != 1;
                 //true; //!elDef->TypeFlags.Contains(isComponent) //impl of
                 //|| (DeclType == CompObj) && (elType == CompObjSpec);
  else if ((DeclType == CompObjSpec) && (elType == Interface)) 
    putIt = elDef->TypeFlags.Contains(isComponent);
  else
    putIt = (DeclType == CompObj) && (elType == CompObjSpec);
  if (putIt && Combo) {
    if (ParentDECL && ParentDECL->FullName.l) 
      lab = ((CLavaPEApp*)wxTheApp)->LBaseData.calcRelName (elDef->FullName, ParentDECL->FullName);
    else 
      lab = elDef->FullName;
    if (elDef->DeclType == VirtualType) {
      lab.Insert(grthen, lab.l);
      lab.Insert(lthen, 0);
    }
    //listItem = new CComboBoxItem(lab, TID(elDef->OwnID, incl));
    //Combo->listBox()->insertItem(listItem);//sort
    listItem = new CComboBoxItem(TID(elDef->OwnID, incl));
    Combo->addItem(QString(lab.c), QVariant::fromValue(listItem));//sort
  }
  /*
  if (((DeclType == Interface) || (DeclType == CompObjSpec))
    && Combo2  && elDef->SecondTFlags.Contains(isEnum)) {
    listItem = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, incl));
    Combo2->listBox()->insertItem(listItem);//sort
  }
  */
  if (elType == VirtualType) 
    decl = myDoc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
  if (Combo2 && //List3 && 
       (elType == Interface) && !elDef->TypeFlags.Contains(isGUI)
     ) {
    if (ParentDECL && ParentDECL->FullName.l) {
      LocalName = ((CLavaPEApp*)wxTheApp)->LBaseData.calcRelName (elDef->FullName, ParentDECL->FullName);
//      listItem = new CComboBoxItem(LocalName, TID(elDef->OwnID, incl));
//      Combo2->listBox()->insertItem(listItem);//sort
      listItem = new CComboBoxItem(TID(elDef->OwnID, incl));
      Combo2->addItem(QString(LocalName.c), QVariant::fromValue(listItem));//sort
    }
    else {
      listItem = new CComboBoxItem(TID(elDef->OwnID, incl));
      Combo2->addItem(QString(elDef->FullName.c), QVariant::fromValue(listItem));//sort
    }
  }
}


/*
void CExecAllDefs::ExecFormDef (LavaDECL ** pelDef, int incl)
{
  int pos;
  if (*pelDef != CallingDECL) {
    if ((DeclType == Alias) || (DeclType == Attr)) {
      if (ParentDECL && ParentDECL->FullName.l) {
        DString LocalName = ((CLavaPEApp*)wxTheApp)->LBaseData.calcRelName ((*pelDef)->FullName, ParentDECL->FullName);
        pos = Combo->AddString(LocalName.c);
        sz = dc->GetTextExtent(LocalName.c, LocalName.l);
        maxWidth = lmax(maxWidth, sz.cx);
      }
      else {
        pos = Combo->AddString((*pelDef)->FullName.c);
        sz = dc->GetTextExtent((*pelDef)->FullName.c, (*pelDef)->FullName.l);
        maxWidth = lmax(maxWidth, sz.cx);
      }
      Combo->SetItemData(pos, MAKELONG((*pelDef)->OwnID, incl));
    }
  }
}
*/

