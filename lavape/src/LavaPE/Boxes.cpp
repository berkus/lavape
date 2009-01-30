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
#include "qpushbutton.h"
#include "qradiobutton.h"
#include "qpushbutton.h"
#include "qbuttongroup.h"
#include "qcheckbox.h"
#include "qstring.h"
#include "qlabel.h"
#include "qmessagebox.h"
#include "qfiledialog.h"
#include "QtAssistant/qassistantclient.h"

#pragma hdrstop


CAttachHandler::CAttachHandler(QWidget* pParent) 
 : QDialog(pParent)
{

}

CAttachHandler::CAttachHandler(LavaDECL* guiService, TIDs client, LavaDECL* attachedFunc, CLavaPEDoc* doc, QWidget* pParent) 
 : QDialog(pParent)
{
  setupUi(this);
  GUIService = guiService;
  Client = client;
  AttachedFunc = attachedFunc;
  myDoc = doc;
  SelectedFunc = 0;
}

ValOnInit CAttachHandler::OnInitDialog()
{
  CHE* che;
  CComboBoxItem *comboItem;
  CHETIDs *cheTIDs;
  CHETID *cheTID, *cheTIDCl;
  bool putIt = true;

  if (Client.last) {
    ClientMem = myDoc->IDTable.GetDECL(((CHETID*)Client.last)->data);
    for (che = (CHE*)GUIService->NestedDecls.first; che; che = (CHE*)che->successor) {
      if ((((LavaDECL*)che->data)->DeclType == Function)
        && ((LavaDECL*)che->data)->SecondTFlags.Contains(isHandler)
        && (myDoc->CheckHandlerIO((LavaDECL*)che->data, ClientMem) >= 0)) {
        cheTIDs = (CHETIDs*)((LavaDECL*)che->data)->HandlerClients.first;
        while (cheTIDs && putIt) {
          cheTIDCl = (CHETID*)Client.first;
          putIt = true;
          for (cheTID = (CHETID*)cheTIDs->data.first; cheTID && cheTIDCl && (cheTID->data == cheTIDCl->data); cheTID = (CHETID*)cheTID->successor)
            cheTIDCl = (CHETID*)cheTIDCl->successor;
          if (!cheTIDCl && !cheTID) {
            putIt = false;
          }
          else
            cheTIDs = (CHETIDs*)cheTIDs->successor;
        }
        if (putIt) {
          comboItem = new CComboBoxItem(TID(((LavaDECL*)che->data)->OwnID, 0));
          addItemAlpha(HandlerList, QString(((LavaDECL*)che->data)->LocalName.c), QVariant::fromValue(comboItem));//sort
        }
      }
    }
  }
  if (!HandlerList->count()) {
    //messagebox machen
    return BoxCancel;
  }
  ID_OK->setEnabled(false);
  return BoxContinue;
;
}

void CAttachHandler::on_HandlerList_activated(int)
{
  QVariant var;
  TID funcID;

  int pos = HandlerList->currentIndex();

  if (pos > 0) {
    var = HandlerList->itemData(pos);
    funcID = var.value<CComboBoxItem*>()->itemData();
    SelectedFunc = myDoc->IDTable.GetDECL(funcID);
    if (SelectedFunc->GUISignaltype == Ev_ValueChanged)
      EventType->setText("New value");
    else if( SelectedFunc->GUISignaltype == Ev_ChainInsert)
      EventType->setText("Insert chain element");
    else if( SelectedFunc->GUISignaltype == Ev_ChainDelete)
      EventType->setText("Delete chain element");
    else if( SelectedFunc->GUISignaltype == Ev_OptInsert)
      EventType->setText("Insert optional");
    else if( SelectedFunc->GUISignaltype == Ev_OptDelete)
      EventType->setText("Delete optional");
    ID_OK->setEnabled(true);
  }
  else {
    EventType->setText("");
    SelectedFunc = 0;
    ID_OK->setEnabled(false);
  }
}

void CAttachHandler::on_ID_OK_clicked()
{
  CHETIDs *che;
  if (SelectedFunc) {
    *AttachedFunc = *SelectedFunc;
    che = new CHETIDs;
    che->data = Client;
    AttachedFunc->HandlerClients.Append(che);
  }
  QDialog::accept();

}

void CAttachHandler::on_ID_HELP_clicked() {
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/EditForm.htm");
}



/////////////////////////////////////////////////////////////////////////////
// CAttrBox dialog


CAttrBox::CAttrBox(QWidget*  parent)
  : QDialog(parent)
{
}

CAttrBox::CAttrBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent)
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
    valNewName = NewName->text();
    valNewTypeType = NewTypeType->text();
    if (DownC->isChecked())
      valkindOfRef = 0;
    else if (DownInd->isChecked())
      valkindOfRef = 1;
    else
      valkindOfRef = 2;
    if (Mandatory->isChecked())
      valkindOfField = 0;
    else if (Optional->isChecked())
      valkindOfField = 1;
    else
      valkindOfField = 2;
  }
  else {
    NewName->setText(valNewName);
    NewTypeType->setText(valNewTypeType);
    switch (valkindOfRef) {
    case 0:
      DownC->setChecked(true);
      break;
    case 1:
      DownInd->setChecked(true);
      break;
    default:
      Back->setChecked(true);
    }
    switch (valkindOfField) {
    case 0:
      Mandatory->setChecked(true);
      break;
    case 1:
      Optional->setChecked(true);
      break;
    default:
      Placeholder->setChecked(true);
    }
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
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  if (onNew) {
    TypeFlags.INCL(constituent);
    myDECL->TypeFlags.INCL(constituent);
    myDECL->TypeFlags.INCL(definiteCat);
    ValueObject->setChecked(true);
    myDoc->MakeBasicBox(BasicTypes, NoDef, true, TypeFlags.Contains(constituent));
    execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    //BasicTypes->setCurrentIndex((int)(VLString));
    BasicTypes->setCurrentIndex(BasicTypes->findText("String"));
    //BasicTypes->setItemText(BasicTypes->currentIndex(),((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[(int)(VLString)]);
    if (SelEndOKToStr(BasicTypes, &valNewTypeType, &myDECL->RefID) > 0) {
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
      StateObject->setEnabled(false);
      ValueObject->setEnabled(false);
      AnyCategory->setEnabled(false);
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
      Substitutable->setChecked(true);
      TypeFlags.INCL(substitutable);
    }
    else
      Substitutable->setChecked(false); 
    if (myDECL->SecondTFlags.Contains(overrides)) {
      cheS = (CHETID*)myDECL->Supports.first;
      while (cheS) {
        baseDECL = myDoc->IDTable.GetDECL(cheS->data);
        if (!baseDECL) {
          CCorrOverBox * box = new CCorrOverBox(myDECL, myDoc, parentWidget());
          box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
      //CExecExtensions *exec = new CExecExtensions(myDoc, &valNamedTypes, BasicTypes, myDECL);
      //delete exec;
      RMOverrides->setEnabled(true);
      NamedTypes->setEnabled(false);
      BasicTypes->setEnabled(false);
    }
    else {
      myDoc->MakeBasicBox(BasicTypes, NoDef, true, TypeFlags.Contains(constituent));
      execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    }
    /*
    if (myDoc->GetCategoryFlags(myDECL).Contains(definesObjCat)) {
      StateObject->setEnabled(false);
      ValueObject->setEnabled(false);
    }
    */
    if (myDECL->TypeFlags.Contains(definiteCat))
      if (myDECL->TypeFlags.Contains(isStateObjectY)) {
        StateObject->setChecked(true);
        ValueObject->setChecked(false);
        AnyCategory->setChecked(false);
      }
      else if (myDECL->TypeFlags.Contains(isAnyCatY)) {
        StateObject->setChecked(false);
        ValueObject->setChecked(false);
        AnyCategory->setChecked(true);
      }
      else {
        StateObject->setChecked(false);
        ValueObject->setChecked(true);
        AnyCategory->setChecked(false);
      }
    else {
      StateObject->setChecked(false);
      ValueObject->setChecked(false);
      AnyCategory->setChecked(false);
    }

  }
  if (TypeFlags.Contains(isComponent)) {
    myDECL->TypeFlags.INCL(hasSetGet);
    SetGet->setChecked(true); 
    SetGet->setEnabled(false);
  }
  if (myDECL->ParentDECL->DeclType == Interface) {
    ConstProp->setEnabled(true);
    Protected->setChecked(myDECL->TypeFlags.Contains(isProtected));
    ConstProp->setChecked(myDECL->TypeFlags.Contains(isConst));
    PassByValue->setChecked(myDECL->TypeFlags.Contains(copyOnAccess));
    PassByValue->setEnabled(true);
    Consumable->setChecked(myDECL->TypeFlags.Contains(consumable));
    SetGet->setChecked(myDECL->TypeFlags.Contains(hasSetGet));
    Consumable->setEnabled(true);
    if (!myDECL->ParentDECL->TypeFlags.Contains(isComponent)
        && myDECL->TypeFlags.Contains(hasSetGet)
        && myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      Abstract->setChecked(myDECL->TypeFlags.Contains(isAbstract));
      Abstract->setEnabled(true);
    }
    else {
      Abstract->setChecked(false);
      Abstract->setEnabled(false);
    }
  }
  else {
    ConstProp->setEnabled(false);
    PassByValue->setEnabled(false);
    Consumable->setEnabled(false);
    Abstract->setEnabled(false);
    Protected->setEnabled(false);
  }
  if (myDECL->SecondTFlags.Contains(overrides)) {
    Protected->setEnabled(baseDECL->TypeFlags.Contains(isProtected));
    StateObject->setEnabled(false);
    SetGet->setEnabled(false);
    PassByValue->setEnabled(false);
    Consumable->setEnabled(false);
    ConstProp->setEnabled(baseDECL->TypeFlags.Contains(isConst));
    Substitutable->setEnabled(false);
    EnableName->setEnabled(true);
    if (myDECL->SecondTFlags.Contains(enableName)) {
      EnableName->setChecked(true);
      NewName->setEnabled(true);
    }
    else {
      EnableName->setChecked(false);
      NewName->setEnabled(false);
    }
    DownC->setEnabled(false);
    DownInd->setEnabled(false);
    Back->setEnabled(false);
  }
  if (execAllPatt)
    delete execAllPatt;
  //SetSelections();  
  SetSelections(BasicTypes, NamedTypes, valNewTypeType);
  UpdateData(false);
  NewName->setFocus();
  return BoxContinue;
}



void CAttrBox::on_SetGet_clicked() 
{
  UpdateData(true);
  if (SetGet->isChecked())
    Abstract->setEnabled(true);
  else {
    Abstract->setChecked(false);
    Abstract->setEnabled(false);
  }
  UpdateData(false);
}

void CAttrBox::on_RMOverrides_clicked() 
{
  UpdateData(true);
  RMOverrides->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);
  second = true;
  NamedTypes->setEnabled(true);
  BasicTypes->setEnabled(true);
  StateObject->setEnabled(true);
  SetGet->setEnabled(true);
  PassByValue->setEnabled(true);
  Consumable->setEnabled(true);
  ConstProp->setEnabled(true);
  Protected->setEnabled(true);
  DownC->setEnabled(true);
  DownInd->setEnabled(true);
  Back->setEnabled(true);
  OnInitDialog();
}


void CAttrBox::on_DownInd_clicked() 
{
  UpdateData(true);
  if (valkindOfRef && TypeFlags.Contains(constituent)) {
    TypeFlags.EXCL(constituent);  
    ResetComboItems(BasicTypes);
    myDoc->MakeBasicBox(BasicTypes, NoDef, true);
    ResetComboItems(NamedTypes);
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    delete execAllPatt;
  if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
//    if (!SetSelections())
      valNewTypeType = QString("");
  }
  UpdateData(false);
}

void CAttrBox::on_DownC_clicked() 
{
  UpdateData(true);
  if (!valkindOfRef) {
    TypeFlags.INCL(constituent);  
    ResetComboItems(BasicTypes);
    myDoc->MakeBasicBox(BasicTypes, NoDef, true, true);
    ResetComboItems(NamedTypes);
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    delete execAllPatt;
  if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
//    if (!SetSelections())
      valNewTypeType = QString("");
  }
  else 
    if (TypeFlags.Contains(constituent)) {
      TypeFlags.EXCL(constituent);  
      ResetComboItems(BasicTypes);
      myDoc->MakeBasicBox(BasicTypes, NoDef, true);
      ResetComboItems(NamedTypes);
      CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
      delete execAllPatt;
      if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
      //if (!SetSelections())
        valNewTypeType = QString("");
    }
  UpdateData(false);
}

void CAttrBox::on_NamedTypes_activated(int pos) 
{
  
  if (!pos) return;
  UpdateData(true);
  BasicTypes->setCurrentIndex(0);
  bool catErr;
  if (SelEndOKToStr(NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    myDECL->DeclDescType = NamedType;
    SynFlags inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr); 
    StateObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    ValueObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    AnyCategory->setEnabled(!inheritedFlag.Contains(definesObjCat));
    if (inheritedFlag.Contains(definesObjCat))
      if (myDECL->TypeFlags.Contains(definiteCat)) {
        if (myDECL->TypeFlags.Contains(isStateObjectY)) {
          StateObject->setChecked(true);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCatY)) {
          StateObject->setChecked(false);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(true);
        }
        else {
          StateObject->setChecked(false);
          ValueObject->setChecked(true);
          AnyCategory->setChecked(false);
        }
      }
      else {
        StateObject->setChecked(false);
        ValueObject->setChecked(false);
        AnyCategory->setChecked(false);
      }
  }
  else 
   SetSelections(BasicTypes, NamedTypes, valNewTypeType);
   //SetSelections();
  UpdateData(false);
}

void CAttrBox::on_BasicTypes_activated(int pos) 
{
  if (!pos) return;
  LavaDECL* decl;
  UpdateData(true);
  NamedTypes->setCurrentIndex(0);
  if (SelEndOKToStr(BasicTypes, &valNewTypeType, &myDECL->RefID) > 0 ) {
    decl = myDoc->IDTable.GetDECL(myDECL->RefID);
    myDECL->DeclDescType = BasicType;
    myDECL->BType = decl->fromBType;
  }
  else 
    SetSelections(BasicTypes, NamedTypes, valNewTypeType);
  UpdateData(false);
}

/*
bool CAttrBox::SetSelections()
{
  BasicTypes->setCurrentIndex(0);
  NamedTypes->setCurrentIndex(0);
  //int cb1= CB_ERR, cb2= CB_ERR;
  if (myDECL->DeclDescType == BasicType) 
    cb1 = BasicTypes->setCurrentText(valNewTypeType);   
  else
    cb2 = NamedTypes->setCurrentText(valNewTypeType);
  return true;//(cb1 != CB_ERR) || (cb2 != CB_ERR);
}*/

void CAttrBox::on_Substitutable_clicked() 
{
  UpdateData(true);
  if (Substitutable->isChecked())
    TypeFlags.INCL(substitutable);
  else
    TypeFlags.EXCL(substitutable);
  ResetComboItems(BasicTypes);
  myDoc->MakeBasicBox(BasicTypes, NoDef, true);
  ResetComboItems(NamedTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
  delete execAllPatt;
  if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
//  if (!SetSelections())
    valNewTypeType = QString("");
  UpdateData(false);
}


void CAttrBox::on_EnableName_clicked() 
{
  UpdateData(true);
  if (EnableName->isChecked()) 
    NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    NewName->setEnabled(false);
  }
  UpdateData(false);  
}

void CAttrBox::reject()
{
  ResetComboItems(BasicTypes);
  ResetComboItems(NamedTypes);
  QDialog::reject();
}

void CAttrBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc/*, OrigDECL*/);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    //NewName->SetCurSel(0, -1);
    NewName->setFocus();
    return;
  }
  if (Substitutable->isChecked()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (EnableName->isChecked()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (!valNewTypeType.length()) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    BasicTypes->setFocus();
    return;
  }
  if (ConstProp->isChecked())
    myDECL->TypeFlags.INCL(isConst);
  else
    myDECL->TypeFlags.EXCL(isConst);
  if (PassByValue->isChecked())
    myDECL->TypeFlags.INCL(copyOnAccess);
  else
    myDECL->TypeFlags.EXCL(copyOnAccess);
  if (Protected->isChecked())
    myDECL->TypeFlags.INCL(isProtected);
  else
    myDECL->TypeFlags.EXCL(isProtected);
  if (Consumable->isChecked())
    myDECL->TypeFlags.INCL(consumable);
  else
    myDECL->TypeFlags.EXCL(consumable);

  if (StateObject->isChecked()) {
    myDECL->TypeFlags.INCL(isStateObjectY);
    myDECL->TypeFlags.EXCL(isAnyCatY);
  }
  else if (AnyCategory->isChecked()) {
    myDECL->TypeFlags.INCL(isAnyCatY);
    myDECL->TypeFlags.EXCL(isStateObjectY);
  }
  else {
    myDECL->TypeFlags.EXCL(isStateObjectY);
    myDECL->TypeFlags.EXCL(isAnyCatY);
  }

  if (SetGet->isChecked()) {
    myDECL->TypeFlags.INCL(hasSetGet);
    if (myDECL->ParentDECL && myDECL->ParentDECL->TypeFlags.Contains(isNative)
      || OrigDECL && OrigDECL->ParentDECL && OrigDECL->ParentDECL->TypeFlags.Contains(isNative))
      myDECL->TypeFlags.INCL(isNative);
  }
  else
    myDECL->TypeFlags.EXCL(hasSetGet);
  
  if (!myDECL->ParentDECL->TypeFlags.Contains(isComponent)) {
    if (Abstract->isChecked())
      myDECL->TypeFlags.INCL(isAbstract);
    else
      myDECL->TypeFlags.EXCL(isAbstract);
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
  ResetComboItems(BasicTypes);
  ResetComboItems(NamedTypes);
  QDialog::accept();
}



/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCompSpecBox 


CCompSpecBox::CCompSpecBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CCompSpecBox::CCompSpecBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent)
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
    valNewName = NewName->text();
  else 
    NewName->setText(valNewName);
}

ValOnInit CCompSpecBox::OnInitDialog() 
{
  CHEEnumSelId* enumsel;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  ExtTypes->setCurrentIndex(0);
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
          Extends->addItem(item);
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
      EnumAdd->setEnabled(false);
      EnumDel->setEnabled(false);
      EnumEdit->setEnabled(false);
      CompoProt->setCurrentIndex(0);
      //CompoProt.SetCurSel(-1);
    }
    else {
      CompoProt->setCurrentIndex(myDECL->nOutput+1);
      EnumAdd->setEnabled(true);
      EnumDel->setEnabled(true);
      EnumEdit->setEnabled(true);
      for (enumsel = (CHEEnumSelId*)myDECL->Items.first; enumsel;
      enumsel = (CHEEnumSelId*)enumsel->successor) {
        item = new CListBoxItem(enumsel->data.Id, TID(-1,-1));
        EnumItems->addItem(item);
      }
    }
    Persistent->setChecked(myDECL->TypeFlags.Contains(isPersistent));
  }
  EnumEdit->setEnabled(false);
  EnumDel->setEnabled(false);
//  EnumAdd->setEnabled(enabled);
  //EnumItems1->SetCurSel(-1);
  //EnumItems->setCurrentItem(0);
  SynFlags typeflag;
  CExecAllDefs * execAllPatt = new CExecAllDefs(myDoc, ExtTypes, 0, myDECL->ParentDECL,
                                                OrigDECL, CompObjSpec, typeflag);
  NewName->setFocus();
  delete execAllPatt;
  UpdateData(false);
  NewName->setFocus();
  
  return BoxContinue;  // return true unless you set the focus to a control
                // EXCEPTION: OCX-Eigenschaftenseiten sollten false zur�ckgeben
}

void CCompSpecBox::on_DelSupport_clicked() 
{
  int pos = Extends->currentRow();
  if (pos >= 0) 
    delete Extends->takeItem(pos);
}

void CCompSpecBox::on_ExtTypes_activated(int pos) 
{
  if (!pos) return;
  SelEndOKToList(ExtTypes, Extends);
}

void CCompSpecBox::on_CompoProt_activated(int pos) 
{
  if (!pos) return;
  myDECL->nOutput = CompoProt->currentIndex() - 1;
  if (myDECL->nOutput >= 0) {
    EnumAdd->setEnabled(true);
    EnumDel->setEnabled(true);
    EnumEdit->setEnabled(true);
    if ((myDECL->nOutput != PROT_LAVA) && (myDECL->nOutput != PROT_NATIVE))
      QMessageBox::critical(this,qApp->applicationName(),ERR_NotYetImplemented,QMessageBox::Ok,0,0);
  }
}

/*
void CCompSpecBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //EnumItems1->SetCurSel(-1);
  EnumDel1->setEnabled(false);
  EnumEdit1->setEnabled(false);
  CDialog::OnLButtonDown(nFlags, point);
}*/


void CCompSpecBox::on_EnumAdd_clicked() 
{
  QString iT, fileName, dir, filter;
  DString linkName, dstrDir;
  int dd=0, ss;
  CListBoxItem *item;

  UpdateData(true);
  ss = EnumItems->currentRow();
  if (ss >= 0)
    ss = 0;
  else
    ss += 1;
  if (myDECL->nOutput == PROT_LAVA) {
    if (ss > 1)
      QMessageBox::critical(this,qApp->applicationName(),ERR_ExactlyOneLcom,QMessageBox::Ok,0,0);
    else {
      dir = ExeDir + ComponentLinkDir;

#ifdef WIN32
      filter = ("LavaCom file (*.lcom *.lcom.lnk)");
#else
      filter = ("LavaCom file (*.lcom)");
#endif
      fileName = L_GetOpenFileName(
                    dir,
                    this,
                    IDS_LAVACOM_FILE,
                    filter
                    ); 
      fileName.remove(0,dir.length());
#ifdef WIN32
      fileName.truncate(fileName.length()-4);
#endif
      EnumItems->insertItem(0,fileName);
      EnumItems->item(0)->setSelected(true);
      EnumDel->setEnabled(true);
      EnumEdit->setEnabled(true);
    }
  }
  else if (myDECL->nOutput == PROT_NATIVE) {
    if (ss > 1)
      QMessageBox::critical(this,qApp->applicationName(),ERR_OneLibName,QMessageBox::Ok,0,0);
    else {
      CEnumItem *cm = new CEnumItem(&iT, 0, 0, false, this);
      if (cm->exec() == QDialog::Accepted) {
        item = new CListBoxItem(iT, TID(-1,-1));
        EnumItems->insertItem(0,item);
        EnumItems->item(0)->setSelected(true);
        EnumDel->setEnabled(true);
        EnumEdit->setEnabled(true);
      } 
      delete cm;
   }
  }
  else {
    CEnumItem *cm = new CEnumItem(&iT, 0, 0, false, this);
    if (cm->exec() == QDialog::Accepted) {
      item = new CListBoxItem(iT, TID(-1,-1));
      EnumItems->insertItem(ss,item);
      EnumItems->item(ss)->setSelected(true);
      EnumDel->setEnabled(true);
      EnumEdit->setEnabled(true);
    } 
    delete cm;
  }
  UpdateData(false);
}

void CCompSpecBox::on_EnumDel_clicked() 
{
  int ss = EnumItems->currentRow();
  if (ss >= 0) {
    delete EnumItems->takeItem(ss);
    EnumDel->setEnabled(false);
    EnumEdit->setEnabled(false);
  } 
}

void CCompSpecBox::on_EnumEdit_clicked() 
{
  QString iT, dir, fileName, filter;
  DString linkName, dstrDir;
  int dd=0, ss;
  CListBoxItem *item;

  ss = EnumItems->currentRow();
  if (ss >= 0) {
    item = (CListBoxItem*)EnumItems->item(ss);
    if (myDECL->nOutput == PROT_LAVA) {
      dir = ExeDir + ComponentLinkDir;
      fileName = dir + item->text();
#ifdef WIN32
      fileName += ".lnk";
      filter = ("LavaCom file (*.lcom *.lcom.lnk)");
#else
      filter = ("LavaCom file (*.lcom)");
#endif
      fileName = L_GetOpenFileName(
                    fileName,
                    this,
                    IDS_LAVACOM_FILE,
                    filter
                    ); 
      fileName.remove(0,dir.length());
#ifdef WIN32
      fileName.truncate(fileName.length()-4);
#endif
      item->setText(fileName);
      EnumItems->item(0)->setSelected(true);
    }
    else if (myDECL->nOutput == PROT_NATIVE) {
      iT = item->text();
      CEnumItem *cm = new CEnumItem(&iT, EnumItems, 0, false, this);
      if (cm->exec() == QDialog::Accepted) {
        item->setText(iT);
        EnumItems->item(0)->setSelected(true);
      } 
      delete cm;
    }
    else {
      iT = item->text();
      CEnumItem* cm = new CEnumItem(&iT, EnumItems, 0, false, this);//parentWidget());   
      if (cm->exec() == QDialog::Accepted) {
        delete EnumItems->takeItem(ss);
        item = new CListBoxItem(iT, TID(-1,-1));
        EnumItems->insertItem(ss,item);
      }
      delete cm;
    }
  }
}


void CCompSpecBox::on_EnumItems_itemSelectionChanged() 
{
  int ss = EnumItems->currentRow();
  SetButtons(ss);
}

void CCompSpecBox::SetButtons(int sel)
{
  if (sel < 0) {
    EnumDel->setEnabled(false);
    EnumEdit->setEnabled(false);
  }
  else {
    EnumDel->setEnabled(true);
    EnumEdit->setEnabled(true);
  }
}


void CCompSpecBox::reject()
{
  ResetComboItems(CompoProt);
  ResetComboItems(ExtTypes); 
  QDialog::reject();
}


void CCompSpecBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    //NewName->SetCurSel(0, -1);
    NewName->setFocus();
    return;
  }
  ListToChain(Extends, &myDECL->Supports);
  if (!myDECL->Supports.first) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_SelInterface,QMessageBox::Ok,0,0);
    ExtTypes->setFocus();
    return;
  }
  myDECL->Items.Destroy();
  int ipos = 0;
  int maxi = EnumItems->count();
  if (Persistent->isChecked())
    myDECL->TypeFlags.INCL(isPersistent);
  else
    myDECL->TypeFlags.EXCL(isPersistent);
  if (myDECL->nOutput < 0) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoCompoProt,QMessageBox::Ok,0,0);
    CompoProt->setFocus();
    return;
  }

  if (myDECL->nOutput == PROT_LAVA) {
    if (EnumItems->count() > 1) {
      QMessageBox::critical(this,qApp->applicationName(),ERR_ExactlyOneLcom,QMessageBox::Ok,0,0);
      EnumItems->setFocus();
      return;
    }
  }
  else if (myDECL->nOutput == PROT_NATIVE) {
    if (EnumItems->count() > 1) {
      QMessageBox::critical(this,qApp->applicationName(),ERR_OneLibName,QMessageBox::Ok,0,0);
      EnumItems->setFocus();
      return;
    }
    myDECL->TypeFlags.INCL(isNative);
  }
  CHEEnumSelId * enumsel;
  while (ipos < maxi) {
    enumsel =  new CHEEnumSelId;
    CListBoxItem* it = (CListBoxItem*)EnumItems->item(ipos);
    enumsel->data.Id = STRING(qPrintable(it->text()));
    myDECL->Items.Append(enumsel);
    ipos++;
  }//while  
  ResetComboItems(CompoProt);
  ResetComboItems(ExtTypes); 
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CCorrOverBox dialog


CCorrOverBox::CCorrOverBox(QWidget* parent )
 : QDialog(parent)
{
}

CCorrOverBox::CCorrOverBox(LavaDECL* decl, CLavaPEDoc* doc, QWidget* parent)
 : QDialog(parent)
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
        addItemAlpha(PossibleOvers, QString(vtDecl->FullName.c),QVariant::fromValue(item));
      }
  }
  if (PossibleOvers->count() > 1) {
    NewOver->setEnabled(true);
  }
  else {
    PossibleOvers->setEnabled(false);
    NewOver->setEnabled(false);
  }
  //SortCombo(PossibleOvers);
  PossibleOvers->setCurrentIndex(0);
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

void CCorrOverBox::on_NewOver_clicked() 
{
  PossibleOvers->setEnabled(true);
}

void CCorrOverBox::on_NoOver_clicked() 
{
  PossibleOvers->setEnabled(false);
}

void CCorrOverBox::reject()
{
  ResetComboItems(PossibleOvers); 
  QDialog::reject();
}

void CCorrOverBox::on_ID_OK_clicked() 
{
  int pos;
  QVariant var;
  //TID newID;
  //CListBoxItem *item;
  
  if (NewOver->isChecked()) {
    pos = PossibleOvers->currentIndex();
    //item = (CComboBoxItem*)PossibleOvers->listBox()->item(pos);
    var = PossibleOvers->itemData(pos);
    CheTID->data = var.value<CComboBoxItem*>()->itemData();
  }
  else {
    myDECL->Supports.Delete(CheTID);
    if (!myDECL->Supports.first)
      myDECL->SecondTFlags.EXCL(overrides);
  }
  ResetComboItems(PossibleOvers); 
	QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CEnumBox

CEnumBox::CEnumBox(QWidget* parent)
  : QDialog( parent)
{
}

CEnumBox::CEnumBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog( parent)
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
    valItemNr = EnumItems->currentRow();
    valNewName = NewName->text();
    valBuildSet = BuildSet->isChecked();
  }
  else {
    EnumItems->item(valItemNr)->setSelected(true);
    NewName->setText(valNewName);
    BuildSet->setChecked(valBuildSet);
  }
}

ValOnInit CEnumBox::OnInitDialog() 
{
  CHEEnumSelId* enumsel;
  CHETID* cheID;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
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
      EnumItems->addItem(item);
    }
    BuildSet->setEnabled(false);
  }
  EnumEdit->setEnabled(false);
  EnumDel->setEnabled(false);
//  EnumAdd->setEnabled(enabled);
  //EnumItems->setCurrentIndex(0);
  UpdateData(false);
  NewName->setFocus();
  return BoxContinue;
}

/////////////////////////////////////////////////////////////////////////////
// CEnumBox message handlers

/*
void CEnumBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
  Enum->setCurrentIndex(0);
  EnumDel->setEnabled(false);
  EnumEdit->setEnabled(false);
  CDialog::OnLButtonDown(nFlags, point);
}
*/

void CEnumBox::on_EnumAdd_clicked() 
{
  CListBoxItem *item;
  int ss = EnumItems->currentRow();
  if (ss < 0)
    ss = 0;
  else
    ss += 1;
  QString iT("");
  CEnumItem *cm = new CEnumItem(&iT, EnumItems, 0, true, this);//parentWidget());
  if (cm->exec() == QDialog::Accepted) {
    item = new CListBoxItem(iT, TID(-1,-1));
    EnumItems->insertItem(ss,item);
    EnumItems->item(ss)->setSelected(true);
    EnumDel->setEnabled(true);
    EnumEdit->setEnabled(true);
  } 
  delete cm;
}

void CEnumBox::on_EnumDel_clicked() 
{
  int ss = EnumItems->currentRow();
  if (ss >= 0) {
    delete EnumItems->takeItem(ss);
    EnumDel->setEnabled(false);
    EnumEdit->setEnabled(false);
  } 
}

void CEnumBox::on_EnumEdit_clicked() 
{
  CListBoxItem *item;
  QString txt;
  int ss = EnumItems->currentRow();
  if (ss >= 0) {
    item = (CListBoxItem*)EnumItems->item(ss);
    txt = item->text();
    CEnumItem* cm = new CEnumItem(&txt, EnumItems, 0, true, this );//parentWidget());   
    if (cm->exec() == QDialog::Accepted) {
      delete EnumItems->takeItem(ss);
      item = new CListBoxItem(txt, TID(-1,-1));
      EnumItems->insertItem(ss,item);
    }
    delete cm;
  }
}


void CEnumBox::on_EnumItems_itemSelectionChanged() 
{
  int ss = EnumItems->currentRow();
  SetButtons(ss);
}

void CEnumBox::SetButtons(int sel)
{
  if (sel < 0) {
    EnumDel->setEnabled(false);
    EnumEdit->setEnabled(false);
  }
  else {
    EnumDel->setEnabled(true);
    EnumEdit->setEnabled(true);
  }
}


void CEnumBox::on_ID_OK_clicked() 
{
  CListBoxItem *item;
  QString str;

  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    NewName->setCursorPosition(0);
    NewName->setFocus();
    return;
  }
  LavaDECL *inEl = &((TEnumDescription*)myDECL->EnumDesc.ptr)->EnumField;
  inEl->Items.Destroy();
  int ipos = 0;
  int maxi = EnumItems->count();
  if (!maxi) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoEnumItem,QMessageBox::Ok,0,0);
    EnumAdd->setFocus();
    return;
  }
  CHEEnumSelId * enumsel;
  while (ipos < maxi) {
    enumsel =  new CHEEnumSelId;
    item = (CListBoxItem*)EnumItems->item(ipos);
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

CEnumItem::CEnumItem(QString *enumItem, QListWidget* itemsBox, ChainAny0* items, bool isId, QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);
  m_ItemAdr = enumItem;
  StartText = *enumItem;
  EnumItem->setText(StartText);
  ItemsBox = itemsBox; //ItemsBox and Items != 0: Enum item, else Component object ID
  Items = items;
  isID = isId;
  if (!ItemsBox && !Items)
    setWindowTitle("Component object ID");
}

void CEnumItem::on_ID_OK_clicked() 
{
  int ipos, maxi;
  CHEEnumSelId *che;
  DString str;
  QString txt, newText;
  CListBoxItem *item;

  if ((Items || ItemsBox) && isID && !((CLavaPEApp*)wxTheApp)->LBaseData.isIdentifier(EnumItem->text()))
    QMessageBox::critical(this,qApp->applicationName(),IDP_IsNoID,QMessageBox::Ok,0,0);
  else {
    if (ItemsBox) {
      maxi = ItemsBox->count();
      newText = EnumItem->text();
      if (newText != StartText) {
        for (ipos = 0; (ipos < maxi) && (newText != txt); ipos++) {
          item = (CListBoxItem*)ItemsBox->item(ipos);
          txt = item->text();
        }
        if (newText == txt) 
          QMessageBox::critical(this,qApp->applicationName(),ERR_NameInUse,QMessageBox::Ok,0,0);
        else {
          *m_ItemAdr = EnumItem->text();
          QDialog::accept();
        }
      }
      else {
        *m_ItemAdr = EnumItem->text();
        QDialog::accept();
      }
    }
    else {
      if (Items) {
        str = qPrintable(EnumItem->text());
        for (che = (CHEEnumSelId*)Items->first; che && (che->data.Id != str); che = (CHEEnumSelId*)che->successor);
        if (che)
          QMessageBox::critical(this,qApp->applicationName(),ERR_NameInUse,QMessageBox::Ok,0,0);
        else {
          *m_ItemAdr = EnumItem->text();
          QDialog::accept();
        }
      }
      else {
        *m_ItemAdr = EnumItem->text();
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
  : QDialog(parent)
{
  setupUi(this);
  myDECL = decl;
  myDoc = doc;
  onNew = isNew;
  OrigDECL = origDECL;
  second = false;
//  setFont(LBaseData->m_TreeFont);
}

void CFuncBox::UpdateData(bool getData)
{
  if (getData) {
    valNewName = NewName->text();
    if (Synch->isChecked())
      valSynch = 0;
    else if (Concurrent->isChecked())
      valSynch = 1;
    else
      valSynch = 2;
  }
  else {
    NewName->setText(valNewName);
    switch (valSynch) {
    case 0:
      Synch->setChecked(true);
      break;
    case 1:
      Concurrent->setChecked(true);
      break;
    default:
      Independent->setChecked(true);
    }
  }
}

ValOnInit CFuncBox::OnInitDialog()
{ 
  CHE* cheIO;
  SynFlags typeflag;
  LavaDECL *decl,  *baseDECL;
  CHETID *ncheS, *cheS;
  CListBoxItem *listItem;
  CHETIDs *cheTIDs, *ncheTIDs = 0;
  QString nameText, cstr;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }

  if (!second) {
    CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Function, typeflag);
    delete execAllPatt;
  }
  if (myDECL->ParentDECL->DeclType == Impl) {
   // || myDECL->TypeFlags.Contains(isGUI) ) {
    //|| myDECL->ParentDECL->TypeFlags.Contains(isComponent) ) {
    Native->setEnabled(false);
    Abstract->setEnabled(false);
    Initializer->setEnabled(false);
    if (myDECL->ParentDECL->DeclType == Impl) {
      Protected->setEnabled(false);
      if (myDECL->SecondTFlags.Contains(funcImpl))
        Closed->setEnabled(false);
    }
  }
  else {
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));
    Native->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isNative));
  }
  if (myDECL->SecondTFlags.Contains(isHandler)) {
    CHECKOp->setChecked(false);
    CHECKOp->setEnabled(false);
    Initializer->setChecked(false);
    Initializer->setEnabled(false);
    Synch->setChecked(true);
    Concurrent->setChecked(false);
    Independent->setChecked(false);
    Synch->setEnabled(false);
    Concurrent->setEnabled(false);
    Independent->setEnabled(false);
    CHECKHandler->setChecked(true);
    cheTIDs = (CHETIDs*)myDECL->HandlerClients.first;
    while (cheTIDs) {
      cheS = (CHETID*)cheTIDs->data.first;
      if (!cheS) {
        nameText = " (Attached to the form) ";
        nameText += QString(myDECL->ParentDECL->FullName.c);
      }
      else
        nameText.clear();
        while (cheS) {
          decl = myDoc->IDTable.GetDECL(cheS->data);
          if (decl)
            nameText += QString(decl->LocalName.c);
          else { 
            nameText += "???";
            cstr = "Broken reference in handler client.\n"
             "Click \"yes\" to remove this client.";
            if (QMessageBox::question(0,qApp->applicationName(),cstr,QMessageBox::Yes,QMessageBox::Cancel,0) == QMessageBox::Cancel)
              nameText += "???";
            else {
              ncheTIDs = (CHETIDs*)cheTIDs->successor;
              myDECL->HandlerClients.Delete(cheTIDs);
            }
          }
          if (ncheTIDs)
            cheS = 0;
          else
            cheS = (CHETID*)cheS->successor;
          if (cheS)
            nameText += QString(".");
        }
      listItem = new CListBoxItem(nameText, cheTIDs->data);
      FieldList->addItem(listItem);
      if (ncheTIDs) {
        cheTIDs = ncheTIDs;
        ncheTIDs = 0;
      }
      else
        cheTIDs = (CHETIDs*)cheTIDs->successor;
    }
    if (((CHETIDs*)myDECL->HandlerClients.first)->data.last) 
      decl = myDoc->IDTable.GetDECL(((CHETID*)((CHETIDs*)myDECL->HandlerClients.first)->data.last)->data);
    else 
      decl = myDECL->ParentDECL;
    FieldTypeDECL = myDoc->IDTable.GetDECL(decl->RefID, decl->inINCL);
    if (FieldTypeDECL->SecondTFlags.Contains(isGUI) || FieldTypeDECL->DeclType == FormDef)
      FieldTypeDECL = myDoc->IDTable.GetDECL(FieldTypeDECL->RefID, FieldTypeDECL->inINCL);
		if (myDECL->GUISignaltype >= Ev_OptInsert) {
			EventType->addItem(QString("Insert optional"));
			EventType->addItem(QString("Delete optional"));
		}
		else if (FieldTypeDECL->SecondTFlags.Contains(isSet)) {
			EventType->addItem(QString("Insert chain element"));
			EventType->addItem(QString("Delete chain element"));
			if (myDECL->GUISignaltype == 0)
				myDECL->GUISignaltype = Ev_ChainInsert;
		}
    else {
      EventType->addItem(QString("New value"));
    }
  }
  if (onNew) {
    hasParams = 0;
    valSynch = 0;
    if (myDECL->ParentDECL->TypeFlags.Contains(isNative)) {
      myDECL->TypeFlags.INCL(isNative);
      Native->setChecked(true);
    }
    EventType->setCurrentIndex(0);

    EnforceOver->setEnabled(myDECL->ParentDECL->DeclType == Interface);
    EnforceOver->setChecked(myDECL->ParentDECL->DeclType == Impl);

    if ((myDECL->ParentDECL->DeclType == Interface)
    || (myDECL->ParentDECL->DeclType == Impl) ) {
      CHECKOp->setEnabled(true);
      CalcOpBox();
    }
    CHECKHandler->setEnabled(false);
  }  
  else {
    if (myDECL->TypeFlags.Contains(isPropGet)) {
      SelfCategory->setEnabled(false);
      SelfCategory->setCurrentIndex(2);
    }
    else if (myDECL->TypeFlags.Contains(isPropSet)) {
      SelfCategory->setEnabled(false);
      SelfCategory->setCurrentIndex(1);
    }

    valNewName = QString(myDECL->LocalName.c);
    cheIO = (CHE*)myDECL->NestedDecls.first;
    hasParams = cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc);
    hasOutput = false;
    EnforceOver->setEnabled(myDECL->ParentDECL->DeclType == Interface);
    while (cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
      hasOutput = hasOutput || (((LavaDECL*)cheIO->data)->DeclType == OAttr);
      //if (((LavaDECL*)cheIO->data)->TypeFlags.Contains(sameAsSelf)) {
      //  StaticFunc->setEnabled(false);
      //  cheIO = 0;
      //}
      //else
        cheIO = (CHE*)cheIO->successor;
    }
    hasOutput = hasOutput || myDECL->Inherits.first;

    if (myDECL->SecondTFlags.Contains(isLavaSignal)) {
      Signal->setChecked(true);
      StaticFunc->setEnabled(false);
      Protected->setEnabled(false);
      Initializer->setEnabled(false);
      Abstract->setEnabled(false);
      EnforceOver->setEnabled(false);
      SelfCategory->setEnabled(false);
    }
    else
      Signal->setChecked(false);
    if (!myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) 
      myDECL->TypeFlags.EXCL(isAbstract);
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      Abstract->setChecked(true);
      StaticFunc->setEnabled(false);
      StaticFunc->setChecked(false);
    }
    else
      Abstract->setChecked(false);
    if (myDECL->TypeFlags.Contains(isStatic)) {
      StaticFunc->setChecked(true);
      Protected->setEnabled(false);
      Initializer->setEnabled(false);
      Abstract->setEnabled(false);
      EnforceOver->setEnabled(false);
      SelfCategory->setEnabled(false);
      Closed->setEnabled(false);
      Closed->setChecked(false);
    }
    else
      StaticFunc->setChecked(false);
    if (myDECL->TypeFlags.Contains(isProtected)) {
      Protected->setChecked(true);
      StaticFunc->setEnabled(false);
      StaticFunc->setChecked(false);
    }
    else
      Protected->setChecked(false);
    if (myDECL->TypeFlags.Contains(forceOverride)) {
      EnforceOver->setChecked(true);
      StaticFunc->setEnabled(false);
      StaticFunc->setChecked(false);
    }
    else
      EnforceOver->setChecked(false);

    if (myDECL->TypeFlags.Contains(isStateObjectY))
      SelfCategory->setCurrentIndex(1);
    else if (myDECL->TypeFlags.Contains(isAnyCatY))
      SelfCategory->setCurrentIndex(2);
    else
      SelfCategory->setCurrentIndex(0);

    if (myDECL->TypeFlags.Contains(isInitializer)) {
      SelfCategory->setEnabled(false); 
      SelfCategory->setCurrentIndex(1);
      Protected->setEnabled(false);
      Initializer->setChecked(true);
      DefaultIni->setEnabled(!hasParams
                                && (!myDECL->ParentDECL->WorkFlags.Contains(hasDefaultIni)
                                    || myDECL->TypeFlags.Contains(defaultInitializer)));
      if (myDECL->TypeFlags.Contains(defaultInitializer)) 
        DefaultIni->setChecked(true);
      myDECL->TypeFlags.EXCL(isAbstract);
      Abstract->setChecked(false);
      Abstract->setEnabled(false);
      StaticFunc->setEnabled(false);
      StaticFunc->setChecked(false);
      Signal->setEnabled(false);
      Signal->setChecked(false);
      //Closed->setEnabled(false);
      //Closed->setChecked(false);
    }
    else
      Initializer->setChecked(false);
    Native->setChecked(myDECL->TypeFlags.Contains(isNative));
    //Transaction->setChecked(myDECL->TypeFlags.Contains(isTransaction));
    if (myDECL->TypeFlags.Contains(execIndependent)) 
      valSynch = 2;
    else
      if (myDECL->TypeFlags.Contains(execConcurrent)) 
        valSynch = 1;
      else
        valSynch = 0;

    if (hasOutput) {
      Signal->setEnabled(false);
      Independent->setEnabled(false);
    }
    CalcOpBox();
    if (myDECL->op != OP_noOp) {
      CHECKOp->setChecked(true);
      NewName->setEnabled(false);
    }
    cheS = (CHETID*)myDECL->Inherits.first;
    while (cheS) {
      if (myDoc->AllowThrowType(myDECL, cheS->data, 0)) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl && decl->SecondTFlags.Contains(isException)) {
          listItem = new CListBoxItem(decl->LocalName, cheS->data);
          Inherits->addItem(listItem);
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
          box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
      SelfCategory->setEnabled(baseDECL->TypeFlags.Contains(isStateObjectY)); 
      Protected->setEnabled(baseDECL->TypeFlags.Contains(isProtected));
      StaticFunc->setEnabled(false);
      RMOverrides->setEnabled(true);
      EnableName->setEnabled(true);
      Closed->setEnabled(false);
      Signal->setEnabled(false);
      if (myDECL->SecondTFlags.Contains(enableName)) {
        EnableName->setChecked(true);
        NewName->setEnabled(true);
      }
      else {
        EnableName->setChecked(false);
        NewName->setEnabled(false);
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
        if (myDoc->AutoCorrBox(&ERR_MissingItfFuncDecl) != QMessageBox::Ok) {
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
          if (myDoc->AutoCorrBox(&ERR_NoSetGetMember)  != QMessageBox::Ok) {
            OrigDECL->WorkFlags.INCL(allowDEL);
            return BoxCancel; //QDialog::reject();
          }
        }
    }
    if (myDECL->SecondTFlags.Contains(funcImpl)) {
      SelfCategory->setEnabled(false);
      NewName->setEnabled(false);
      Signal->setEnabled(false);
      //Transaction->setEnabled(false);
      Protected->setEnabled(false);
      EnforceOver->setEnabled(false);
      DefaultIni->setEnabled(false);
      DelInherits->setEnabled(false);
      NamedTypes->setEnabled(false);
      Inherits->setEnabled(false);
      Synch->setEnabled(false);
      Concurrent->setEnabled(false);
      Independent->setEnabled(false);
    }
    if (myDECL->SecondTFlags.Contains(funcImpl)
        || myDECL->SecondTFlags.Contains(overrides)) {
      Initializer->setEnabled(false);
      CHECKOp->setEnabled(false);
      CMBOperator->setEnabled(false);
    }
    if (myDECL->SecondTFlags.Contains(closed)) {
      //Initializer->setEnabled(false);
      StaticFunc->setEnabled(false);
      Closed->setChecked(true);
    }
    else
      Closed->setChecked(false);
    if (myDECL->SecondTFlags.Contains(isHandler)) {
      CHECKHandler->setEnabled(true);
      if ((myDECL->GUISignaltype == Ev_ChainDelete) || (myDECL->GUISignaltype == Ev_ChainInsert))
        EventType->setCurrentIndex(myDECL->GUISignaltype-1);
      else if ((myDECL->GUISignaltype == Ev_OptDelete) || (myDECL->GUISignaltype == Ev_OptInsert))
        EventType->setCurrentIndex(myDECL->GUISignaltype-3);
      else
        EventType->setCurrentIndex(myDECL->GUISignaltype);
      EventType->setEnabled(false);
    }
    else {
      if (!myDECL->SecondTFlags.Contains(funcImpl)
           && myDECL->ParentDECL->SecondTFlags.Contains(isGUI)) {
        int type = myDoc->CheckHandlerIO(myDECL, 0);
        if (type >= 0) {
          if (type == 2)
            EventType->addItem("Insert");
          else if (type == 3)
            EventType->addItem("Delete");
          else
            EventType->addItem("New value");
          EventType->setEnabled(true);
          CHECKHandler->setEnabled(true);
        }
        else
          CHECKHandler->setEnabled(false);
      }
      else
        CHECKHandler->setEnabled(false);
    }
  }
  CHECKHandler->setChecked(myDECL->SecondTFlags.Contains(isHandler));
  EventType->setEnabled(myDECL->SecondTFlags.Contains(isHandler) && !myDECL->SecondTFlags.Contains(funcImpl));
  FieldList->setEnabled(myDECL->SecondTFlags.Contains(isHandler) && !myDECL->SecondTFlags.Contains(funcImpl));
  FieldRemove->setEnabled(myDECL->SecondTFlags.Contains(isHandler));

  UpdateData(false); 
  NewName->setFocus();
  return BoxContinue;
}

void CFuncBox::CalcOpBox()
{
  TID id;
  int high, iop;
  CComboBoxItem *comboItem;
  high = CMBOperator->count();
  if (high)
    for (iop = 1; iop < high; iop++)
      CMBOperator->removeItem(1);
  high = (int)OP_high;
  if (!myDECL->ParentDECL->SecondTFlags.Contains(isArray))
    high = high-2;
  myDoc->MakeVElems(myDECL->ParentDECL);

  for (iop = ((int)OP_noOp)+1; iop < (int)OP_high; iop++) {
    id.nID = -1;
    if (!myDoc->getOperatorID(myDECL->ParentDECL, (TOperator) iop, id)
        || (id.nID == myDECL->OwnID) && (id.nINCL == 0)) {
      comboItem = new CComboBoxItem(TID(iop, 0));
      addItemAlpha(CMBOperator, QString(LBaseData->OperatorNames [iop].c),QVariant::fromValue(comboItem));
//      CMBOperator->listBox()->insertItem(comboItem);//sort
      if ((id.nID == myDECL->OwnID) && (id.nINCL == 0))
        CMBOperator->setCurrentIndex((int)myDECL->op);
    }
  }
  //SortCombo(CMBOperator);
}

void CFuncBox::on_CHECKHandler_clicked()
{
  if (CHECKHandler->isChecked()) {
    CHECKOp->setChecked(false);
    CHECKOp->setEnabled(false);
    Initializer->setChecked(false);
    Initializer->setEnabled(false);
    Synch->setChecked(true);
    Concurrent->setChecked(false);
    Independent->setChecked(false);
    Synch->setEnabled(false);
    Concurrent->setEnabled(false);
    Independent->setEnabled(false);
    EventType->setEnabled(true);
    FieldList->setEnabled(true);
    FieldRemove->setEnabled(true);
  }
  else {
    CHECKOp->setEnabled(true);
    Initializer->setEnabled(true);
    Synch->setEnabled(true);
    Concurrent->setEnabled(true);
    Independent->setEnabled(true);
    EventType->setEnabled(false);
    FieldList->setEnabled(false);
    FieldRemove->setEnabled(false);
  }
}

void CFuncBox::on_EventType_activated( int )
{

}

void CFuncBox::on_FieldRemove_clicked()
{
  int pos = FieldList->currentRow();
  if (pos >= 0)
    delete FieldList->takeItem(pos);
}

void CFuncBox::on_Closed_clicked()
{
  if (Closed->isChecked()) {
    StaticFunc->setEnabled(false);
    //Initializer->setEnabled(false);
  }
  else {
    StaticFunc->setEnabled(true);
    Initializer->setEnabled(myDECL->ParentDECL->DeclType == Interface);
  }
}


void CFuncBox::on_RMOverrides_clicked() 
{
  UpdateData(true);
  RMOverrides->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);  
  second = true;
  Initializer->setEnabled(true);
  //ConstFunc->setEnabled(true);
  Protected->setEnabled(true);
  CHECKOp->setEnabled(true);
  CMBOperator->setEnabled(true);
  DelInherits->setEnabled(true);
  NamedTypes->setEnabled(true);
  Inherits->setEnabled(true);
  Closed->setEnabled(true);
  Signal->setEnabled(true);
//  BasicTypes->setEnabled(true);
  OnInitDialog();
}

/*
void CFuncBox::OnSelendokBasicTypes() 
{
  SelEndOKToList(BasicTypes, &Inherits1);
}
*/

void CFuncBox::on_DelInherits_clicked() 
{
  int pos = Inherits->currentRow();
  if (pos >= 0)
    delete Inherits->takeItem(pos); 
}

  
void CFuncBox::on_NamedTypes_activated(int pos) 
{
  if (!pos) return;
  SelEndOKToList(NamedTypes, Inherits);
  
}


void CFuncBox::on_CHECKOp_clicked() 
{
  UpdateData(true);
  if (CHECKOp->isChecked()) {
    CMBOperator->setEnabled(true);
    NewName->setEnabled(false);
  }
  else {
    CMBOperator->setEnabled(false);
    NewName->setEnabled(true);
    if (myDECL->op != OP_noOp) {
      valNewName = QString("");
      myDECL->op = OP_noOp;
    }
  }
  UpdateData(false);

}


void CFuncBox::on_StaticFunc_clicked() 
{
  UpdateData(true);
  if (StaticFunc->isChecked()) {
    Protected->setChecked(false);
    Initializer->setChecked(false);
    Abstract->setChecked(false);
    EnforceOver->setChecked(false);
    Protected->setEnabled(false);
    Initializer->setEnabled(false);
    Abstract->setEnabled(false);
    EnforceOver->setEnabled(false);
    myDECL->TypeFlags.INCL(isConst); 
    //ConstFunc->setChecked(true);
    //ConstFunc->setEnabled(false);
    Closed->setEnabled(false);
  }
  else {
    Protected->setEnabled(true);
    Initializer->setEnabled(myDECL->ParentDECL->DeclType == Interface);
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));
    EnforceOver->setEnabled(myDECL->ParentDECL->DeclType == Interface);
    //ConstFunc->setEnabled(true);
    Closed->setEnabled(true);
  }
  UpdateData(false);
}

void CFuncBox::on_Initializer_clicked() 
{
  UpdateData(true);
  bool ini = Initializer->isChecked();
  DefaultIni->setEnabled(!hasParams
                            && ini
                            && (!myDECL->ParentDECL->WorkFlags.Contains(hasDefaultIni)
                                || myDECL->TypeFlags.Contains(defaultInitializer)));
  if (ini) {
    //ConstFunc->setEnabled(false); 
    //ConstFunc->setChecked(false); 
    //Protected->setEnabled(false);
    StaticFunc->setEnabled(false);
    StaticFunc->setChecked(false);
    myDECL->TypeFlags.EXCL(isAbstract);
    Abstract->setChecked(false);
    Abstract->setEnabled(false);
    //Closed->setEnabled(false);
    Signal->setEnabled(false);
    Signal->setChecked(false);
  }
  else {
    DefaultIni->setChecked(false);
    StaticFunc->setEnabled(true);
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));
    //ConstFunc->setEnabled(true); 
    Protected->setEnabled(true);
    //Closed->setEnabled(true);
    Signal->setEnabled(true);
  }
  myDECL->TypeFlags.EXCL(isConst);
  UpdateData(false);  
}

void CFuncBox::on_CMBOperator_activated(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  TID id;
  SelEndOKToStr(CMBOperator, &valNewName, &id);
  myDECL->op = (TOperator)id.nID;
  UpdateData(false);
}

/*
void CFuncBox::OnInheritsBody() 
{
  UpdateData(true);
  if (InheritsBody->isChecked()) {
    Abstract->setChecked(false);
    Abstract->setEnabled(false); 
  }
  else
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));  
  UpdateData(false);
}
*/

void CFuncBox::on_Abstract_clicked() 
{
  UpdateData(true);
  if (Abstract->isChecked()) {
    Native->setChecked(false);
    Native->setEnabled(false);
    StaticFunc->setEnabled(false);
    StaticFunc->setChecked(false);
  }
  else {
    Native->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isNative));
//    InheritsBody->setEnabled(myDECL->SecondTFlags.Contains(overrides));
  }
  UpdateData(false);
}

void CFuncBox::on_Native_clicked() 
{
  UpdateData(true);
  if (Native->isChecked()) {
    Abstract->setChecked(false);
    Abstract->setEnabled(false); 
  }
  else
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));  
  UpdateData(false);
}

void CFuncBox::on_Signal_clicked()
{
  if (Signal->isChecked()) {
    //ConstFunc->setChecked(true); 
    //ConstFunc->setEnabled(false);
    myDECL->TypeFlags.INCL(isConst); 
    Protected->setChecked(true);
    Protected->setEnabled(false);
    Abstract->setChecked(false);
    Abstract->setEnabled(false);
    StaticFunc->setChecked(false);
    StaticFunc->setEnabled(false); 
    Initializer->setChecked(false);
    Initializer->setEnabled(false);
    Native->setChecked(true);
    Native->setEnabled(false);
    EnforceOver->setChecked(false);
    EnforceOver->setEnabled(false);
  }
  else {
    //ConstFunc->setEnabled(true);
    Protected->setEnabled(true);
    Abstract->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isAbstract));
    StaticFunc->setEnabled(true); 
    Initializer->setEnabled(myDECL->ParentDECL->DeclType == Interface);
    Native->setEnabled(myDECL->ParentDECL->TypeFlags.Contains(isNative));
    Native->setChecked(myDECL->ParentDECL->TypeFlags.Contains(isNative));
    EnforceOver->setEnabled(myDECL->ParentDECL->DeclType == Interface);
  }
}

void CFuncBox::on_EnableName_clicked() 
{
  UpdateData(true);
  if (EnableName->isChecked()) 
    NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    NewName->setEnabled(false);
  }
  UpdateData(false);
}


void CFuncBox::reject()
{
  ResetComboItems(CMBOperator);
  ResetComboItems(NamedTypes); 
  QDialog::reject();
}

void CFuncBox::makeHandler()
{
  LavaDECL *IOEl, *IOEl2; 
  if (myDECL->GUISignaltype == Ev_ChainDelete) {
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->LocalName = STRING("chain");
    IOEl->DeclType = IAttr;
    IOEl->DeclDescType = NamedType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));

    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("delHandle");
    IOEl->DeclDescType = BasicType;
    IOEl->BType = B_Che;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(myDoc->IDTable.BasicTypesID[B_Che], 1);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = OAttr;
    IOEl->LocalName = STRING("delete");
    IOEl->DeclDescType = BasicType;
    IOEl->BType = B_Bool;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(myDoc->IDTable.BasicTypesID[B_Bool], 1);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
  }
  else if (myDECL->GUISignaltype == Ev_ChainInsert) {
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->LocalName = STRING("chain");
    IOEl->DeclType = IAttr;
    IOEl->DeclDescType = NamedType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));

    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->LocalName = STRING("afterHandle");
    IOEl->DeclType = IAttr;
    IOEl->DeclDescType = BasicType;
    IOEl->BType = B_Che;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(myDoc->IDTable.BasicTypesID[B_Che], 1);
    IOEl->TypeFlags.INCL(isOptional);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("defaultElem");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    myDoc->IDTable.GetParamRefID(FieldTypeDECL, IOEl->RefID, isSet);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl2 = NewLavaDECL();
    IOEl2->TypeFlags.INCL(definiteCat);
    IOEl2->LocalName = STRING("insert");
    IOEl2->DeclType = OAttr;
    IOEl2->DeclDescType = BasicType;
    IOEl2->BType = B_Bool;
    IOEl2->inINCL = 0;
    IOEl2->RefID = TID(myDoc->IDTable.BasicTypesID[B_Bool], 1);
    myDECL->NestedDecls.Append(NewCHE(IOEl2));
    IOEl2 = NewLavaDECL();
    IOEl2->TypeFlags.INCL(definiteCat);
    IOEl2->TypeFlags.INCL(isOptional);
    IOEl2->DeclType = OAttr;
    IOEl2->LocalName = STRING("insertElem");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl2->DeclDescType = NamedType;
    else
      IOEl2->DeclDescType = BasicType;
    IOEl2->BType = FieldTypeDECL->fromBType;
    IOEl2->inINCL = 0;
    IOEl2->RefID = IOEl->RefID;
    myDECL->NestedDecls.Append(NewCHE(IOEl2));
  }
	else if (myDECL->GUISignaltype == Ev_OptInsert) {
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("defaultOpt");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->LocalName = STRING("insert");
    IOEl->DeclType = OAttr;
    IOEl->DeclDescType = BasicType;
    IOEl->BType = B_Bool;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(myDoc->IDTable.BasicTypesID[B_Bool], 1);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->TypeFlags.INCL(isOptional);
    IOEl->DeclType = OAttr;
    IOEl->LocalName = STRING("insertOpt");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
  }
	else if (myDECL->GUISignaltype == Ev_OptDelete) {
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("delOpt");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->LocalName = STRING("delete");
    IOEl->DeclType = OAttr;
    IOEl->DeclDescType = BasicType;
    IOEl->BType = B_Bool;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(myDoc->IDTable.BasicTypesID[B_Bool], 1);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
  }
  else if (myDECL->GUISignaltype == Ev_ValueChanged) {
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("oldValue");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->DeclType = IAttr;
    IOEl->LocalName = STRING("enteredValue");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
    IOEl = NewLavaDECL();
    IOEl->TypeFlags.INCL(definiteCat);
    IOEl->TypeFlags.INCL(isOptional);
    IOEl->DeclType = OAttr;
    IOEl->LocalName = STRING("newValue");
    if (FieldTypeDECL->fromBType == NonBasic)
      IOEl->DeclDescType = NamedType;
    else
      IOEl->DeclDescType = BasicType;
    IOEl->BType = FieldTypeDECL->fromBType;
    IOEl->inINCL = 0;
    IOEl->RefID = TID(FieldTypeDECL->OwnID, FieldTypeDECL->inINCL);
    myDECL->NestedDecls.Append(NewCHE(IOEl));
  }
}

void CFuncBox::on_ID_OK_clicked() 
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
  if (EnableName->isChecked()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (Initializer->isChecked())
    myDECL->TypeFlags.INCL(isInitializer); 
  else
    myDECL->TypeFlags.EXCL(isInitializer); 
  if (!myDECL->SecondTFlags.Contains(overrides) || myDECL->SecondTFlags.Contains(enableName)) {
    if (myDECL->op == OP_noOp) {
      QString* ids = CheckNewName(valNewName, myDECL, myDoc);
      if (ids) {
        QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
        NewName->setFocus();
        //NewName->SetSel(0, -1);
        return;
      }
    }
    if (myDECL->op != OP_noOp)
      myDoc->MakeOperator(myDECL);
  }
  if (myDECL->SecondTFlags.Contains(isHandler)) {
	  if (myDECL->GUISignaltype >= Ev_OptInsert)
      myDECL->GUISignaltype = EventType->currentIndex()+3;
    else if (myDECL->GUISignaltype >= Ev_ChainInsert) 
      myDECL->GUISignaltype = EventType->currentIndex()+1;
    else
      myDECL->GUISignaltype = EventType->currentIndex();
    if (myDECL->GUISignaltype < 1) {
      QMessageBox::critical(this, qApp->applicationName(), IDP_NoEvTypeSel, QMessageBox::Ok,0,0);
      EventType->setFocus();
      return;
    }
    if (onNew)
      makeHandler();
  }

  ListToChain(Inherits, &myDECL->Inherits);  //fires
  if (Native->isChecked())
    myDECL->TypeFlags.INCL(isNative); 
  else
    myDECL->TypeFlags.EXCL(isNative); 
  if (!myDECL->TypeFlags.Contains(isNative) && (myDECL->ParentDECL->DeclType == Impl) )
    myDoc->GetExecDECL(myDECL, ExecDef);
  if (!myDECL->Supports.first) 
    myDECL->SecondTFlags.EXCL(overrides);
  if (Abstract->isChecked())
    myDECL->TypeFlags.INCL(isAbstract); 
  else
    myDECL->TypeFlags.EXCL(isAbstract);
  if (StaticFunc->isChecked())
    myDECL->TypeFlags.INCL(isStatic);
  else
    myDECL->TypeFlags.EXCL(isStatic);
  if (Protected->isChecked())
    myDECL->TypeFlags.INCL(isProtected);
  else
    myDECL->TypeFlags.EXCL(isProtected);
  if (EnforceOver->isChecked())
    myDECL->TypeFlags.INCL(forceOverride);
  else
    myDECL->TypeFlags.EXCL(forceOverride);

  switch (SelfCategory->currentIndex()) {
  case 1:
    myDECL->TypeFlags.INCL(isStateObjectY);
    myDECL->TypeFlags.EXCL(isAnyCatY);
    break;
  case 2:
    myDECL->TypeFlags.INCL(isAnyCatY);
    myDECL->TypeFlags.EXCL(isStateObjectY);
    break;
  default:
    myDECL->TypeFlags.EXCL(isStateObjectY);
    myDECL->TypeFlags.EXCL(isAnyCatY);
  }

  if (Signal->isChecked())
    myDECL->SecondTFlags.INCL(isLavaSignal); 
  else
    myDECL->SecondTFlags.EXCL(isLavaSignal); 
  if (DefaultIni->isChecked())
    myDECL->TypeFlags.INCL(defaultInitializer); 
  else
    myDECL->TypeFlags.EXCL(defaultInitializer); 
  if (Closed->isChecked())
    myDECL->SecondTFlags.INCL(closed); 
  else
    myDECL->SecondTFlags.EXCL(closed); 
  myDECL->TypeFlags.EXCL(execIndependent);
  myDECL->TypeFlags.EXCL(execConcurrent);
  if (valSynch >= 1)
    myDECL->TypeFlags.INCL(execConcurrent);
  else
    if (valSynch == 2)
      myDECL->TypeFlags.INCL(execIndependent);

  ResetComboItems(CMBOperator);
  ResetComboItems(NamedTypes); 
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CImplBox dialog


CImplBox::CImplBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{

}

CImplBox::CImplBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent)
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
    valImplSel = ImplSel->text();
  else 
    ImplSel->setText(valImplSel);
}

ValOnInit CImplBox::OnInitDialog() 
{
  DString strINCL, strID;
  CExecAllDefs * execAllPatt;
  LavaDECL *classDECL;
  QString cstr;
  CHE *che;
  int pos, icount;
  TID id, dataID;
  bool ex=false;
  QVariant var;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }

  if (!onNew) {
    ID_OK->setEnabled(false);
    CHETID *cheS = (CHETID*)myDECL->Supports.first; //implements
    if (cheS) {
      if (cheS->data.nID >= 0) {
        classDECL = myDoc->IDTable.GetDECL(cheS->data);
        if (classDECL) 
          valImplSel = QString(classDECL->LocalName.c); 
        else {
          QMessageBox::critical(this, qApp->applicationName(), ERR_NoImplIF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          valImplSel = QString("??: ");
          Convert.IntToString(cheS->data.nINCL, strINCL);
          Convert.IntToString(cheS->data.nID, strID);
          valImplSel = valImplSel + QString("(") 
            + QString(strINCL.c) + QString(",") + QString(strID.c) + QString(")");
        }
      }
    }
  }
  execAllPatt = new CExecAllDefs(myDoc, ImplTypes, 0, myDECL->ParentDECL, OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  if (myDECL->DeclType == CompObj) {
    setWindowTitle("Component object implementation");
    Label_IDC_ComboEx->setText( "Component object to be implemented");
  }
  if (!onNew) {
    icount = ImplTypes->count();
    for (pos=1; (pos < icount) && !ex; pos++) {
      var = ImplTypes->itemData(pos);
      id = var.value<CComboBoxItem*>()->itemData();
      if (id == ((CHETID*)myDECL->Supports.first)->data)
        ex = true;
    }
    if (pos < icount)
      ImplTypes->setCurrentIndex(pos-1);
    ImplTypes->setEnabled(false);
    if (myDECL->SecondTFlags.Contains(isGUI)) {
      dataID = myDoc->GetGUIDataTypeID(classDECL);      
      if (dataID != myDECL->RefID) {  
        cstr = "DATATYPE of GUI-interface differs from DATATYPE in form declaration of GUI-implementation.\n"
               "Click \"yes\" to replace the form declaration.";
        if (QMessageBox::question(0,qApp->applicationName(),cstr,QMessageBox::Yes,QMessageBox::Cancel,0) == QMessageBox::Yes) {
          che = (CHE*)myDECL->NestedDecls.first;
          while (che && ((LavaDECL*)che->data)->DeclType != FormDef)
            che = (CHE*)che->successor;
          if (che) 
            myDECL->NestedDecls.Delete(che); //the new form will be constructed in CheckImpl
        }
      }
    }
  }
  delete execAllPatt;
  UpdateData(false);
  return BoxContinue;
}


void CImplBox::on_ImplTypes_activated(int pos) 
{
  if (!pos) return;
  CHETID* che;
  if (!myDECL->Supports.first) {
    che = new CHETID;
    myDECL->Supports.Append(che);
  }
  else
    che = (CHETID*)myDECL->Supports.first;
  int onSel = SelEndOKToStr(ImplTypes, &valImplSel, &che->data);
  UpdateData(false);

}


void CImplBox::reject()
{
  ResetComboItems(ImplTypes); 
  QDialog::reject();
}

void CImplBox::on_ID_OK_clicked() 
{
  LavaDECL *interfDECL, *inDECL;
  CHE *Che;
  CHETID* cheID;
  TID paramID;
  
  UpdateData(true);
  if (!myDECL->Supports.first) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_SelInterface,QMessageBox::Ok,0,0);
    ImplTypes->setFocus();
    return;
  }
  interfDECL = myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data);
  if (interfDECL) {
    CheckNewName(valImplSel, myDECL, myDoc);
    if (interfDECL->TypeFlags.Contains(isAbstract))
      myDECL->TypeFlags.INCL(isAbstract);
    else
      myDECL->TypeFlags.EXCL(isAbstract);
    if (interfDECL->SecondTFlags.Contains(isGUI)) {
      myDECL->SecondTFlags.INCL(isGUI);
      myDECL->RefID = myDoc->GetGUIDataTypeID(interfDECL); //IDTable.GetParamID(interfDECL, paramID, isGUI);
      //if (paramID.nID >= 0) 
      //  myDECL->RefID = paramID;
    }
    else {
      myDECL->SecondTFlags.EXCL(isGUI);
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
    ResetComboItems(ImplTypes); 
    QDialog::accept();
  }
  else {
    ResetComboItems(ImplTypes); 
    QDialog::reject();
  }
}

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIncludeBox 


CIncludeBox::CIncludeBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CIncludeBox::CIncludeBox(CLavaPEDoc* myDoc, CHESimpleSyntax* newChe, CHESimpleSyntax* oldChe, QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);
  NewChe = newChe;
  OldChe = oldChe;
  MyDoc = myDoc;
  ID_OK->setEnabled(!myDoc->changeNothing);
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
    valUseAs = UseAs->text();
    valFullPathName = FullPathName->text();
  }
  else {
    UseAs->setText(valUseAs);
    FullPathName->setText(valFullPathName);
  }
}


void CIncludeBox::on_ID_OK_clicked() 
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

void CIncludeBox::on_OtherPath_clicked() 
{
  UpdateData(true);
  valFullPathName = L_GetOpenFileName(
                    valFullPathName,
                    this,
                    "Choose a file to include",
                    "Lava files (*.lava)"
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
 : QDialog(parent)
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
    valNewName = NewName->text();
    if (Synch->isChecked())
      valSynch = 0;
    else
      valSynch = 1;
  }
  else {
    NewName->setText(valNewName);
    switch (valSynch) {
    case 0:
      Synch->setChecked(true);
      break;
    default:
      Concurrent->setChecked(true);
    }
  }
}

ValOnInit CInitBox::OnInitDialog() 
{

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  if (!onNew) 
    valNewName = QString(myDECL->LocalName.c);

  if (onNew) {
    Transaction->setChecked(false);
    ReadOnly->setChecked(false);
    valSynch = 0;
  }
  else {
    Transaction->setChecked(myDECL->TypeFlags.Contains(isTransaction));
    ReadOnly->setChecked(myDECL->TypeFlags.Contains(isConst));
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
  NewName->setFocus();

  UpdateData(false);
  return BoxContinue;
}

void CInitBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  if (isTop && !valNewName.length()) {
    myDECL->FullName.Destroy();
    myDECL->LocalName.Destroy();
  }
  else  {
    QString* ids = CheckNewName(valNewName, myDECL, myDoc);
    if (ids) {
      QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
      NewName->setFocus();
      return;
    }
  }
  myDoc->GetExecDECL(myDECL, ExecDef);
  if (Transaction->isChecked())
    myDECL->TypeFlags.INCL(isTransaction); 
  else
    myDECL->TypeFlags.EXCL(isTransaction); 
  if (ReadOnly->isChecked())
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
  : QDialog(parent)
{
  setupUi(this);
  valNewName = QString("");
  valIfaceID = QString("");
  valBuildSet = false;
  //valIsGUI = false;
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
    valNewName = NewName->text();
    valIfaceID = InterfaceID->text();
    valBuildSet = BuildSet->isChecked();
    //valIsGUI = IsGUI->isChecked();
    if (Creatable->isChecked())
      valKindOfInterface = 0;
    else if (NonCreatable->isChecked())
      valKindOfInterface = 1;
    else
      valKindOfInterface = 2;
  }
  else {
    NewName->setText(valNewName);
    InterfaceID->setText(valIfaceID);
    BuildSet->setChecked(valBuildSet);
    //IsGUI->setChecked(valIsGUI);
    switch (valKindOfInterface) {
    case 0:
      Creatable->setChecked(true);
      break;
    case 1:
      NonCreatable->setChecked(true);
      break;
    default:
      IsComponent->setChecked(true);
    }
  }
}

ValOnInit CInterfaceBox::OnInitDialog() 
{
  DString str;
  CContext context;
//  int pos, icount;
  QString * err;
  CExecAllDefs * execAllPatt;
  QVariant var;
//  CHETID *ncheS, *cheS;
//  LavaDECL* decl;
  bool found=false;
//  CComboBoxItem *comboItem;


//  GetDlgItem(IDC_StatIDLabel)->SetWindowText("CLSID of implementation DLL");

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  if (onNew) {
    myDoc->IDTable.GetPattern(myDECL, context);
    //IsGUI->setEnabled(!context.oContext);
    Native->setChecked(false);
  }
  else {
    //IsGUI->setEnabled(false);
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
    BuildSet->setEnabled(false);
    Native->setChecked(myDECL->TypeFlags.Contains(isNative));
    if (myDECL->TypeFlags.Contains(isComponent)) {
      valKindOfInterface = 2;
      InterfaceID->setEnabled(true);
      //Native->setEnabled(false);
      if (myDECL->LitStr.l) {
        valIfaceID = QString(myDECL->LitStr.c);
      }
    }
    else
      if (myDECL->TypeFlags.Contains(isAbstract))
        valKindOfInterface = 1;
  }
  myDoc->MakeBasicBox(BasicTypes, myDECL->DeclType, false);
  execAllPatt = new CExecAllDefs(myDoc, ExtTypes, 0, //GUIStructs, 
                        myDECL->ParentDECL, OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  delete execAllPatt;
  BasicTypes->setCurrentIndex(0);
  ExtTypes->setCurrentIndex(0);
  /*
  TID id;
  if (!onNew && myDECL->TypeFlags.Contains(isGUI)) {
    valIsGUI = true; 
    icount = GUIStructs->count();
    bool ex = false;
    for (pos=1;
         (pos < icount)
           && !ex;
        // && (myDECL->RefID != ((CComboBoxItem*)GUIStructs->listBox()->item(pos))->itemData());
         pos++) {
           var = GUIStructs->itemData(pos);
           id = var.value<CComboBoxItem*>()->itemData();
           if (id == myDECL->RefID)
             ex = true;}
    if (pos <= icount)
      GUIStructs->setCurrentIndex(pos-1);
  }
  */
  NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}

void CInterfaceBox::SupportsToList()
{
  LavaDECL* decl;
  CListBoxItem* listItem;
  CHETID *ncheS, *cheS = (CHETID*)myDECL->Supports.first; //extends
  DString str;
  Extends->clear();
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
          Extends->addItem(listItem);
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

void CInterfaceBox::on_DelSupport_clicked() 
{
  int pos;

  UpdateData(true);
  CListBoxItem *listItem = (CListBoxItem*)Extends->currentItem();
  if (listItem) {
    pos = Extends->row(listItem);
    delete Extends->takeItem(pos);
    ListToChain(Extends, &myDECL->Supports);
    ResetComboItems(ExtTypes);
    CExecBase *execBase = new CExecBase(this);
    delete execBase;
    UpdateData(false);
  }
}

void CInterfaceBox::on_ExtTypes_activated(int pos) 
{
  QVariant var;
  LavaDECL *baseDECL;

  if (!pos) return;
  UpdateData(true);
  var = ExtTypes->itemData(pos);
  CComboBoxItem *comboItem = var.value<CComboBoxItem*>();
  if (comboItem) {
    baseDECL = myDoc->IDTable.GetDECL(comboItem->itemData());
    if (myDoc->IDTable.InsertBaseClass(myDECL, baseDECL, ContextDECL, true)) {
      SupportsToList();
      ResetComboItems(ExtTypes);
      CExecBase *execBase = new CExecBase(this);
      delete execBase;
      BasicTypes->setCurrentIndex(0);
      if (baseDECL->SecondTFlags.Contains(isSet)) {
        BuildSet->setChecked(false);
        BuildSet->setEnabled(false);
      }
      UpdateData(false);
    }
  }
  ExtTypes->setCurrentIndex(0);
}


void CInterfaceBox::on_BasicTypes_activated(int pos) 
{
  QVariant var;

  if (!pos) return;
  UpdateData(true);
  var = BasicTypes->itemData(pos);
  CComboBoxItem *comboItem = var.value<CComboBoxItem*>();
  if (comboItem) {
    if (myDoc->IDTable.InsertBaseClass(myDECL, myDoc->IDTable.GetDECL(comboItem->itemData(), 0), ContextDECL, true)) {
      SupportsToList();
      ExtTypes->setCurrentIndex(0);
      UpdateData(false);
    }
  }
  BasicTypes->setCurrentIndex(0);
}

/*
void CInterfaceBox::on_DelInherits_clicked() 
{
  int pos;
  CComboBoxItem *comboItem = (CComboBoxItem*)Inherits->selectedItem();
  if (comboItem) {
    pos = Inherits->index(comboItem);
    Inherits->removeItem(pos);
  }  
}
void CInterfaceBox::on_InheritTypes_activated(int pos) 
{
  if (!pos) return;
  SelEndOKToList(InheritTypes, Inherits);
}

*/

void CInterfaceBox::on_IsComponent_clicked() 
{
  UpdateData(true);
  if  (valKindOfInterface == 2) 
    myDECL->TypeFlags.INCL(isComponent);
  else 
    myDECL->TypeFlags.EXCL(isComponent);
  ResetComboItems(ExtTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, ExtTypes, 0,
                myDECL->ParentDECL, OrigDECL, myDECL->DeclType, myDECL->TypeFlags);
  ExtTypes->setCurrentIndex(0);
  InterfaceID->setEnabled(true);
  //Native->setChecked(false);
  //Native->setEnabled(false);
  UpdateData(false);
}

void CInterfaceBox::on_NonCreatable_clicked() 
{
  InterfaceID->setEnabled(false);
  Native->setEnabled(true);
}

void CInterfaceBox::on_Creatable_clicked() 
{
  InterfaceID->setEnabled(false);
  Native->setEnabled(true);
}

/*
void CInterfaceBox::on_IsGUI_clicked() 
{
  UpdateData(true);
  GUIStructs->setEnabled(valIsGUI && onNew);
  
}

void CInterfaceBox::on_GUIStructs_activated(int pos) 
{
  if (!pos) return;
  int oldPos;
  UpdateData(true);
  if (SelEndOKToStr(GUIStructs, &valGUIStruct, &exID) <= 0) {
  //GUIStructs->setItemText(GUIStructs->currentIndex(),valGUIStruct);
    oldPos = GUIStructs->findText(valGUIStruct, Qt::MatchExactly | Qt::MatchCaseSensitive );
    GUIStructs->setCurrentIndex(oldPos);
  }
}*/

void CInterfaceBox::reject()
{
  ResetComboItems(ExtTypes);
//  ResetComboItems(GUIStructs);
  ResetComboItems(BasicTypes); 
  QDialog::reject();
}


void CInterfaceBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc/*, OrigDECL*/);
  LavaDECL *suDECL;
  CHETID* cheS;
  bool extendsSet = false, extendsArray = false, extendsChain = false,
    extendsException = false, extendsEnum = false, extendsGUI = false;

  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    NewName->setCursorPosition(0);
    NewName->setFocus();
    return;
  }
  myDECL->LitStr.Destroy();
  if (Native->isChecked())
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
    /*
    if (valIsGUI) {
      if (exID.nID >= 0) {
        myDECL->RefID = exID;
        myDoc->MakeGUIFuncs(myDECL);
      }
      else {
        QMessageBox::critical(this, qApp->applicationName(), IDP_NoTypeSel, QMessageBox::Ok,0,0);
        GUIStructs->setFocus();
        return;
      }
    }
    */
    if (!myDECL->TypeFlags.Contains(isComponent))
      myDoc->MakeIniFunc(myDECL);
  }
  ListToChain(Extends, &myDECL->Supports);
  cheS = (CHETID*)myDECL->Supports.first;
  if (cheS) {
    while (cheS) {
      suDECL = myDoc->IDTable.GetDECL(cheS->data);//!!
      if (suDECL) {
        extendsSet = extendsSet || suDECL->SecondTFlags.Contains(isSet);
        extendsArray = extendsArray || suDECL->SecondTFlags.Contains(isArray);
        extendsGUI = extendsGUI || suDECL->SecondTFlags.Contains(isGUI);
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
    if (extendsGUI)
      myDECL->SecondTFlags.INCL(isGUI);
    else
      myDECL->SecondTFlags.EXCL(isGUI);
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
    if ((!myDoc->isStd 
      || (myDECL->fromBType != B_Object) && (myDECL->fromBType != NonBasic))) {
      cheS = new CHETID;
        cheS->data.nID = myDoc->IDTable.BasicTypesID[B_Object];
      if (myDoc->isStd)
        cheS->data.nINCL = 0;
      else
        cheS->data.nINCL = 1;
      myDECL->Supports.Append(cheS);
    }
//  ListToChain(Inherits, &myDECL->Inherits);  //signals
  myDECL->WorkFlags.INCL(recalcVT);
  ResetComboItems(ExtTypes);
//  ResetComboItems(GUIStructs);
  ResetComboItems(BasicTypes); 
  QDialog::accept();
}

/////////////////////////////////////////////////////////////////////////////
// CIOBox dialog


CIOBox::CIOBox(QWidget*  /*=NULL*/)
{
}

CIOBox::CIOBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent)
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
    valNewName = NewName->text();
    valNewTypeType = NewTypeType->text();
    if (Mandatory->isChecked())
      valkindOfField = 0;
    else 
      valkindOfField = 1;
  }
  else {
    NewName->setText(valNewName);
    NewTypeType->setText(valNewTypeType);
    switch (valkindOfField) {
    case 0:
      Mandatory->setChecked(true);
      break;
    default:
      Optional->setChecked(true);
    }
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
          addItemAlpha(NamedTypes, QString(baseDECL->FullName.c),QVariant::fromValue(item));
//          NamedTypes->listBox()->insertItem(item);//sort
        }     
        else 
          item = new CComboBoxItem(/*((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[baseDECL->fromBType],*/ TID(baseDECL->OwnID, 1));
          addItemAlpha(BasicTypes, QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[baseDECL->fromBType]), QVariant::fromValue(item));//sort
      }
      id1 = El->data.VTClss;
    }
    El = (CHETVElem*)El->successor;
  }
  //SortCombo(BasicTypes);
  //SortCombo(NamedTypes);
  //NamedTypes->SetDroppedWidth(maxWidth+6);
}

ValOnInit CIOBox::OnInitDialog()
{ 
  CExecAllDefs * execAllPatt=0;
  LavaDECL *baseDECL;
  CHETID* che;
  bool catErr, callBox;
  DString dstr;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }

  TypeFlags = myDECL->ParentDECL->ParentDECL->TypeFlags;
  if (onNew) {
    ValueObject->setChecked(true);
    myDECL->TypeFlags.INCL(definiteCat);
    myDoc->MakeBasicBox(BasicTypes, NoDef, true);
    execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
    //BasicTypes->setCurrentIndex((int)(VLString));
    BasicTypes->setCurrentIndex(BasicTypes->findText("String"));
    int num = SelEndOKToStr(BasicTypes, &valNewTypeType, &myDECL->RefID);
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
    //if (myDECL->ParentDECL->TypeFlags.Contains(isStatic)) {
    //  SameAsSelf->setChecked(false);
    //  //myDECL->TypeFlags.EXCL(sameAsSelf);
    //  SameAsSelf->setEnabled(false);
    //}
    if (myDECL->TypeFlags.Contains(substitutable)) {
      Substitutable->setChecked(true);
      TypeFlags.INCL(substitutable);
    }
    else
      Substitutable->setChecked(false); 
   if (myDoc->GetCategoryFlags(myDECL, catErr).Contains(definesObjCat)) {
      StateObject->setChecked(false);
      ValueObject->setChecked(false);
      AnyCategory->setChecked(false);
      //SameAsSelf->setChecked(false);
      StateObject->setEnabled(false);
      ValueObject->setEnabled(false);
      //SameAsSelf->setEnabled(false);
      AnyCategory->setEnabled(false);
    }
    else 
      if (myDECL->SecondTFlags.Contains(overrides)) {
        StateObject->setEnabled(false);
        ValueObject->setEnabled(false);
        //SameAsSelf->setEnabled(false);
        AnyCategory->setEnabled(false);
        Closed->setEnabled(false);
      }
    if (myDECL->TypeFlags.Contains(definiteCat))
      if (myDECL->TypeFlags.Contains(isStateObjectY)) {
        StateObject->setChecked(true);
        ValueObject->setChecked(false);
        //SameAsSelf->setChecked(false);
        AnyCategory->setChecked(false);
      }
      else {
        StateObject->setChecked(false);
/*        if (myDECL->TypeFlags.Contains(sameAsSelf)) {
          ValueObject->setChecked(false);
          SameAsSelf->setChecked(true);
          AnyCategory->setChecked(false);
        }
        else*/ 
        if (myDECL->TypeFlags.Contains(isAnyCatY)) {
          ValueObject->setChecked(false);
          //SameAsSelf->setChecked(false);
          AnyCategory->setChecked(true);
        }
        else {
          ValueObject->setChecked(true);
          //SameAsSelf->setChecked(false);
          AnyCategory->setChecked(false);
        }
      }
      /*
    if (myDECL->TypeFlags.Contains(isGUI)) {
      if (myDECL->DeclType == IAttr) {
        typeDECL = myDoc->IDTable.GetDECL(myDECL->ParentDECL->ParentDECL->RefID, myDECL->inINCL);
        if (typeDECL)
          BaseClassesToCombo(typeDECL);
      }
      else {
        NamedTypes->setEnabled(false);
        BasicTypes->setEnabled(false);
      }
    }
    else {*/
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
          NamedTypes->setEnabled(false);
          BasicTypes->setEnabled(false);
        }
        else {
          if (myDoc->AutoCorrBox(&ERR_OverriddenIOType) == QDialog::Accepted) {
            myDECL->RefID.nID = baseDECL->RefID.nID;
            if (baseDECL->RefID.nID >= 0)
              myDECL->RefID.nINCL = myDoc->IDTable.IDTab[baseDECL->inINCL]->nINCLTrans[baseDECL->RefID.nINCL].nINCL;
            NamedTypes->setEnabled(false);
            BasicTypes->setEnabled(false);
          }
          else {
            myDoc->MakeBasicBox(BasicTypes, NoDef, true);
            execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL,
                                         Attr, TypeFlags);
          }
        }
      }
      else {
        myDoc->MakeBasicBox(BasicTypes, NoDef, true);
        execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL,
                                     Attr, TypeFlags);
      }
    //}
    dstr = myDoc->GetTypeLabel(myDECL, false);
    valNewTypeType = QString(dstr.c);
  }
  //if (myDECL->ParentDECL->TypeFlags.Contains(isStatic)) 
  //  SameAsSelf->setEnabled(false);
  if (execAllPatt)
    delete execAllPatt;
  SetSelections(BasicTypes, NamedTypes, valNewTypeType);
  //SetSelections();  
  if (myDECL->SecondTFlags.Contains(funcImpl)) { // || myDECL->TypeFlags.Contains(isGUI)) {
    NewTypeType->setEnabled(false);
    NewName->setEnabled(false);
    NamedTypes->setEnabled(false);
    BasicTypes->setEnabled(false);
    Mandatory->setEnabled(false);
    Optional->setEnabled(false);
    StateObject->setEnabled(false);
    ValueObject->setEnabled(false);
    AnyCategory->setEnabled(false);
    Substitutable->setEnabled(false);
    Closed->setEnabled(false);
  }
  Closed->setChecked(myDECL->SecondTFlags.Contains(closed));
  //if (myDECL->SecondTFlags.Contains(funcImpl)
      //|| (myDECL->ParentDECL->DeclType != Function)
      //|| myDECL->TypeFlags.Contains(isGUI)
      //|| myDECL->ParentDECL->TypeFlags.Contains(isInitializer))
    //SameAsSelf->setEnabled(false);
  if (myDECL->SecondTFlags.Contains(overrides))// || myDECL->TypeFlags.Contains(isGUI)) 
    Substitutable->setEnabled(false);

  UpdateData(false);
  NewName->setFocus();
  return BoxContinue;
}

void CIOBox::on_NamedTypes_activated(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  bool catErr;

  BasicTypes->setCurrentIndex(0);
  if (SelEndOKToStr(NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    myDECL->DeclDescType = NamedType;
    SynFlags inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr); 
    StateObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    ValueObject->setEnabled(!inheritedFlag.Contains(definesObjCat));
    //SameAsSelf->setEnabled(!inheritedFlag.Contains(definesObjCat));
    AnyCategory->setEnabled(!inheritedFlag.Contains(definesObjCat));
    
    if (inheritedFlag.Contains(definesObjCat)) {
      //SameAsSelf->setChecked(false);
      if (myDECL->TypeFlags.Contains(definiteCat)) {
        if (myDECL->TypeFlags.Contains(isStateObjectY)) {
          StateObject->setChecked(true);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCatY)) {
          StateObject->setChecked(false);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(true);
        }
        else {
          StateObject->setChecked(false);
          ValueObject->setChecked(true);
          AnyCategory->setChecked(false);
        }
      }
      else {
        StateObject->setChecked(false);
        ValueObject->setChecked(false);
        AnyCategory->setChecked(false);
      }
    }
  }
  else 
    SetSelections(BasicTypes, NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}

void CIOBox::on_BasicTypes_activated(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  NamedTypes->setCurrentIndex(0);
  int num = SelEndOKToStr(BasicTypes, &valNewTypeType, &myDECL->RefID);
  if (num > 0) {
    myDECL->DeclDescType = BasicType;
    myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
  }
  else 
    SetSelections(BasicTypes, NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}

/*
bool CIOBox::SetSelections()
{
  BasicTypes->setCurrentIndex(0);
  NamedTypes->setCurrentIndex(0);
  int cb1= CB_ERR, cb2= CB_ERR;
  if (myDECL->DeclDescType == BasicType) 
    cb1 = BasicTypes->setCurrentText(valNewTypeType);   
  else
    cb2 = NamedTypes->setCurrentText(valNewTypeType);
  return (cb1 != CB_ERR) || (cb2 != CB_ERR);
}*/


void CIOBox::on_Substitutable_clicked() 
{
  UpdateData(true);
  if (Substitutable->isChecked())
    TypeFlags.INCL(substitutable);
  else
    TypeFlags.EXCL(substitutable);
  ResetComboItems(BasicTypes);
  myDoc->MakeBasicBox(BasicTypes, NoDef, true);
  ResetComboItems(NamedTypes);
  CExecAllDefs *execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, Attr, TypeFlags);
  delete execAllPatt;
  if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
//  if (!SetSelections())
    valNewTypeType = QString("");
  UpdateData(false);
}


void CIOBox::reject()
{
  ResetComboItems(BasicTypes);
  ResetComboItems(NamedTypes); 
  QDialog::reject();
}

void CIOBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  if (myDECL->SecondTFlags.Contains(funcImpl)) {
    QDialog::reject();
    return;
  }
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
//    NewName->SetSel(0, -1);
    NewName->setFocus();
    return;
  }
  if (!valNewTypeType.length()) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    BasicTypes->setFocus();
    return;
  }
  myDECL->TypeFlags.EXCL(isOptional);
  if (Substitutable->isChecked()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (valkindOfField == 1) 
    myDECL->TypeFlags.INCL(isOptional);
  if (StateObject->isChecked())
    myDECL->TypeFlags.INCL(isStateObjectY);
  else
    myDECL->TypeFlags.EXCL(isStateObjectY);
  //if (SameAsSelf->isChecked())
  //  myDECL->TypeFlags.INCL(sameAsSelf);
  //else
  //  myDECL->TypeFlags.EXCL(sameAsSelf);
  if (AnyCategory->isChecked())
    myDECL->TypeFlags.INCL(isAnyCatY);
  else
    myDECL->TypeFlags.EXCL(isAnyCatY);
  if (Closed->isChecked())
    myDECL->SecondTFlags.INCL(closed); 
  else
    myDECL->SecondTFlags.EXCL(closed); 

  ResetComboItems(BasicTypes);
  ResetComboItems(NamedTypes); 
  QDialog::accept();
}



/////////////////////////////////////////////////////////////////////////////
// CPackageBox dialog


CPackageBox::CPackageBox(QWidget* parent /*=NULL*/)
  : QDialog(parent)
{
}

CPackageBox::CPackageBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent)
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
    valNewName = NewName->text();
  else 
    NewName->setText(valNewName);
}

ValOnInit CPackageBox::OnInitDialog() 
{
  CHETID *ncheS, *cheS;
  LavaDECL* decl;
  CListBoxItem *item;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }

  if (!onNew && myDECL->LocalName.l) {
    valNewName = QString(myDECL->LocalName.c);
    Opaque->setChecked(myDECL->TypeFlags.Contains(isProtected));
    cheS = (CHETID*)myDECL->Supports.first; //extends
    while (cheS) {
      if (cheS->data.nID >= 0) {
        decl = myDoc->IDTable.GetDECL(cheS->data);
        if (decl) {
          item = new CListBoxItem(decl->FullName, cheS->data);
          Extends->addItem(item);
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
  CExecDefs * execDefs = new CExecDefs(myDoc, ExtTypes, OrigDECL);
  delete execDefs;
  NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}


void CPackageBox::on_DelSupport_clicked() 
{
  int pos = Extends->currentRow();
  if (pos >= 0) 
    delete Extends->takeItem(pos);
}

void CPackageBox::on_ExtTypes_activated(int pos) 
{
  if (!pos) return;
  SelEndOKToList(ExtTypes, Extends);
}

void CPackageBox::reject()
{
  ResetComboItems(ExtTypes); 
  QDialog::reject();
}

void CPackageBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  ListToChain(Extends, &myDECL->Supports);
  if (Opaque->isChecked())
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
      QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
      //NewName->SetSel(0, -1);
      NewName->setFocus();
      return;
    }
  }
  ResetComboItems(ExtTypes); 
  QDialog::accept();
}

/////////////////////////////////////////////////////////////////////////////
// CSetBox dialog


CSetBox::CSetBox(QWidget* parent )
  : QDialog(parent)
{
}

CSetBox::CSetBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent)
  : QDialog(parent)
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
    valNewName = NewName->text();
    valExtend = Extend->text();
  }
  else {
    NewName->setText(valNewName);
    Extend->setText(valExtend);
  }
}

ValOnInit CSetBox::OnInitDialog() 
{
  SynFlags typeflag;

  if (myDoc->changeNothing) {
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  CExecAllDefs * execAllPatt = new CExecAllDefs(myDoc, ExTypes, 0, myDECL->ParentDECL, OrigDECL,  Attr, typeflag);
  myDoc->MakeBasicBox(BasicTypes, NoDef, false);
  delete execAllPatt;
  /* 
    ---->remember: this box is used only in case of a new set
  */
  BasicTypes->setCurrentIndex(0);
  ExTypes->setCurrentIndex(0);
  NewName->setFocus();
  UpdateData(false);
  return BoxContinue;
}


void CSetBox::on_ExTypes_activated(int pos) 
{
  if (!pos) return;
  int oldPos;
  UpdateData(true);
  BasicTypes->setCurrentIndex(0);
  if (SelEndOKToStr(ExTypes, &valExtend, &exID) > 0)
    UpdateData(false);
  else
    if (exID.nINCL == 1) {
      oldPos = BasicTypes->findText(valExtend, Qt::MatchExactly | Qt::MatchCaseSensitive ); 
      BasicTypes->setCurrentIndex(oldPos);
    }
    else {
      oldPos = ExTypes->findText(valExtend, Qt::MatchExactly | Qt::MatchCaseSensitive ); 
      ExTypes->setCurrentIndex(oldPos);
    }
}

void CSetBox::on_BasicTypes_activated(int pos) 
{
  if (!pos) return;
  int oldPos;
  UpdateData(true);
  ExTypes->setCurrentIndex(0);
  if (SelEndOKToStr(BasicTypes, &valExtend, &exID) > 0)
    UpdateData(false);
  else
    if ((exID.nINCL == 1) || myDoc->isStd) {
      oldPos = BasicTypes->findText(valExtend, Qt::MatchExactly | Qt::MatchCaseSensitive );
      BasicTypes->setCurrentIndex(oldPos);
    }
    else {
      oldPos = ExTypes->findText(valExtend, Qt::MatchExactly | Qt::MatchCaseSensitive );
      ExTypes->setCurrentIndex(oldPos);
    }
}


void CSetBox::reject()
{
  ResetComboItems(ExTypes);
  ResetComboItems(BasicTypes); 
  QDialog::reject();
}

void CSetBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    NewName->setFocus();
    //NewName->SetSel(0, -1);
    return;
  }
  if (valExtend.length()) {
    myDoc->MakeSet(exID, myDECL);
    myDECL->TreeFlags.INCL(isExpanded);
    myDECL->TreeFlags.INCL(ParaExpanded);
  }
  else {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    BasicTypes->setFocus();
    ExTypes->setCurrentIndex(0);
    BasicTypes->setCurrentIndex(0);
    return;
  }
  myDECL->WorkFlags.INCL(recalcVT);
  ResetComboItems(ExTypes);
  ResetComboItems(BasicTypes); 
  QDialog::accept();
}


/////////////////////////////////////////////////////////////////////////////
// CVTypeBox dialog


CVTypeBox::CVTypeBox(QWidget* parent)
  : QDialog(parent)
{
}

CVTypeBox::CVTypeBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew, QWidget* parent) 
  : QDialog(parent)
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
    valNewName = NewName->text();
    valNewTypeType = NewTypeType->text();
    if (DownC->isChecked())
      valkindOfLink = 0;
    else if (DownInd->isChecked())
      valkindOfLink = 1;
    else
      valkindOfLink = 2;
 }
  else {
    NewName->setText(valNewName);
    NewTypeType->setText(valNewTypeType);
    switch (valkindOfLink) {
    case 0:
      DownC->setChecked(true);
      break;
    case 1:
      DownInd->setChecked(true);
      break;
    default:
      Back->setChecked(true);
    }
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
    ID_OK->setEnabled(false);
    ID_OK->setDefault( false );
    ID_CANCEL->setDefault( true );
  }
  if (second) {
    ResetComboItems(NamedTypes);
    ResetComboItems(BasicTypes);
  }
  
  if (onNew) {
    myDoc->MakeBasicBox(BasicTypes, NoDef, true);
    execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, typeflag);
    if ((myDECL->ParentDECL->DeclType == Interface)
    && !myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      VTAbstract->setEnabled(false);
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
        box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
      Substitutable->setChecked(true);
      if (myDECL->SecondTFlags.Contains(overrides) && !baseAbstract) {
        NamedTypes->setEnabled(false);
        BasicTypes->setEnabled(false);
      }
    }
    else
      Substitutable->setChecked(false); 
    if (che && (che == (CHETID*)myDECL->Supports.last)) 
      decl = myDoc->IDTable.GetDECL(che->data);
    if ((myDECL->SecondTFlags.Contains(isSet) || myDECL->SecondTFlags.Contains(isArray))
      && !myDECL->TypeFlags.Contains(isAbstract)) {
      if (decl->TypeFlags.Contains(isAbstract)) {
        DownC->setEnabled(true);
        DownInd->setEnabled(true);
        Back->setEnabled(true);
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
      RMOverrides->setEnabled(true);
      EnableName->setEnabled(true);
      if (myDECL->SecondTFlags.Contains(enableName)) {
        EnableName->setChecked(true);
        NewName->setEnabled(true);
      }
      else {
        EnableName->setChecked(false);
        NewName->setEnabled(false);
      }
    }
    if ((myDECL->SecondTFlags.Contains(overrides))  && !myDECL->TypeFlags.Contains(isAbstract)
      && (!decl || !decl->TypeFlags.Contains(isAbstract))) { //and not only overriding an abstract virtual type
      CExecExtensions *exec = new CExecExtensions(myDoc, NamedTypes, BasicTypes, myDECL, OrigDECL);
      delete exec;
      VTAbstract->setEnabled(false);
    }
    else {
      myDoc->MakeBasicBox(BasicTypes, NoDef, true);
      execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, myDECL->TypeFlags);
    }
    if ((myDECL->ParentDECL->DeclType == Package) 
        || myDECL->ParentDECL->TypeFlags.Contains(isAbstract)) {
      VTAbstract->setEnabled(true);
      if (myDECL->TypeFlags.Contains(isAbstract)) {
        VTAbstract->setChecked(true);
        NamedTypes->setEnabled(false);
        BasicTypes->setEnabled(false);
        Substitutable->setEnabled(false);
      }
    }
    else {
      Substitutable->setEnabled(!myDECL->SecondTFlags.Contains(overrides) || baseAbstract);
      VTAbstract->setEnabled(false);
      VTAbstract->setChecked(false);
    }
    SetCategoryChecks();
  }
  if (execAllPatt)
    delete execAllPatt;
  SetSelections(BasicTypes, NamedTypes, valNewTypeType);
  //SetSelections();  
  UpdateData(false);
  NewName->setFocus();
  return BoxContinue;
}

void CVTypeBox::SetCategoryChecks()
{
  bool catErr;
  if (myDECL->TypeFlags.Contains(definesObjCat)) 
    DefCat->setChecked(true);
  else 
    DefCat->setChecked(false);
  inheritedFlag = myDoc->GetCategoryFlags(myDECL, catErr);;
  DefCat->setEnabled(!inheritedFlag.Contains(definesObjCat));
  if (inheritedFlag.Contains(definiteCat)) {
    if (myDECL->TypeFlags.Contains(isStateObjectY)) {
      StateObject->setChecked(true);
      ValueObject->setChecked(false);
      AnyCategory->setChecked(false);
    }
    else if (myDECL->TypeFlags.Contains(isAnyCatY)) {
      StateObject->setChecked(false);
      ValueObject->setChecked(false);
      AnyCategory->setChecked(true);
    }
    else {
      StateObject->setChecked(false);
      ValueObject->setChecked(true);
      AnyCategory->setChecked(false);
    }
    StateObject->setEnabled(false);
    ValueObject->setEnabled(false);
    AnyCategory->setEnabled(false);
  }
  else {
    if (myDECL->TypeFlags.Contains(isAbstract)) 
      if (myDECL->Supports.first && !inheritedFlag.Contains(isAbstract))
        myDECL->TypeFlags.EXCL(isAbstract);
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      StateObject->setChecked(false);
      ValueObject->setChecked(false);
      AnyCategory->setChecked(false);
      StateObject->setEnabled(false);
      ValueObject->setEnabled(false);
      AnyCategory->setEnabled(false);
   }
    else {
      if (myDECL->TypeFlags.Contains(definesObjCat)) {
        if (myDECL->TypeFlags.Contains(isStateObjectY)) {
          StateObject->setChecked(true);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(false);
        }
        else if (myDECL->TypeFlags.Contains(isAnyCatY)) {
          StateObject->setChecked(false);
          ValueObject->setChecked(false);
          AnyCategory->setChecked(true);
        }
        else {
          StateObject->setChecked(false);
          ValueObject->setChecked(true);
          AnyCategory->setChecked(false);
        }
        StateObject->setEnabled(true);
        ValueObject->setEnabled(true);
        AnyCategory->setEnabled(true);
      }
      else {
        StateObject->setChecked(false);
        ValueObject->setChecked(false);
        AnyCategory->setChecked(false);
      }
    }
  }
}


void CVTypeBox::on_RMOverrides_clicked() 
{
  UpdateData(true);
  RMOverrides->setEnabled(false);
  myDECL->Supports.Destroy();
  myDECL->SecondTFlags.EXCL(overrides);
  second = true;
  OnInitDialog();
}

void CVTypeBox::on_NamedTypes_activated(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  BasicTypes->setCurrentIndex(0);
  if (SelEndOKToStr(NamedTypes, &valNewTypeType, &myDECL->RefID) > 0) {
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      myDECL->TypeFlags.EXCL(isAbstract);
      valkindOfLink = 0;
      DownC->setEnabled(true);
      DownInd->setEnabled(true);
      Back->setEnabled(true);
      SetCategoryChecks();
    }
    myDECL->DeclDescType = NamedType;
    SetCategoryChecks();
  }
  else 
   SetSelections(BasicTypes, NamedTypes, valNewTypeType);
  // SetSelections();
  UpdateData(false);
}

void CVTypeBox::on_BasicTypes_activated(int pos) 
{
  if (!pos) return;
  UpdateData(true);
  NamedTypes->setCurrentIndex(0);
  int num = SelEndOKToStr(BasicTypes, &valNewTypeType, &myDECL->RefID);
  if (num > 0) {
    if (myDECL->TypeFlags.Contains(isAbstract)) {
      myDECL->TypeFlags.EXCL(isAbstract);
      DownC->setEnabled(true);
      DownInd->setEnabled(true);
      Back->setEnabled(true);
      SetCategoryChecks();
      valkindOfLink = 0;
    }
    myDECL->DeclDescType = BasicType;
    myDECL->BType = myDoc->IDTable.GetDECL(myDECL->RefID)->fromBType;
    SetCategoryChecks();
  }
  else 
    SetSelections(BasicTypes, NamedTypes, valNewTypeType);
    //SetSelections();
  UpdateData(false);
}


void CVTypeBox::on_VTAbstract_clicked() 
{
  UpdateData(true);
  bool abs = VTAbstract->isChecked();
  BasicTypes->setCurrentIndex(0);
  NamedTypes->setCurrentIndex(0);
  if (!abs) {
    myDECL->TypeFlags.EXCL(isAbstract);
    Substitutable->setEnabled(!myDECL->SecondTFlags.Contains(overrides) || baseAbstract);
    valkindOfLink = 0;
  }
  else {
    myDECL->TypeFlags.INCL(isAbstract);
    myDECL->TypeFlags.EXCL(substitutable);
    Substitutable->setChecked(false);
    Substitutable->setEnabled(false);
  }
  NamedTypes->setEnabled(!abs);
  BasicTypes->setEnabled(!abs);
  SetCategoryChecks();

  valNewTypeType = QString("");
  UpdateData(false);
}


void CVTypeBox::on_DefCat_clicked() 
{
  UpdateData(true);
  bool defCat = DefCat->isChecked()
                && !inheritedFlag.Contains(definiteCat)
                && !myDECL->TypeFlags.Contains(isAbstract);
  StateObject->setEnabled(defCat);
  ValueObject->setEnabled(defCat);
  AnyCategory->setEnabled(defCat);
  if (defCat) {
    ValueObject->setChecked(true);
    myDECL->TypeFlags.INCL(definesObjCat);
  }
  else {
    ValueObject->setChecked(false);
    StateObject->setChecked(false);
    AnyCategory->setChecked(false);
    myDECL->TypeFlags.EXCL(definesObjCat);
  }
  UpdateData(false);  
}

//void CVTypeBox::on_ValueObject_clicked() 
//{
//  UpdateData(true);
//  if (ValueObject->isChecked())
//    myDECL->TypeFlags.EXCL(isStateObjectY);
//}
//
//void CVTypeBox::on_StateObject_clicked() 
//{
//  UpdateData(true);
//  if (StateObject->isChecked())
//    myDECL->TypeFlags.INCL(isStateObjectY);
//  else
//    myDECL->TypeFlags.EXCL(isStateObjectY);
//}


void CVTypeBox::on_Substitutable_clicked() 
{
  UpdateData(true);
  bool sbst = Substitutable->isChecked();
  if (sbst)
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  ResetComboItems(BasicTypes);
  myDoc->MakeBasicBox(BasicTypes, NoDef, true);
  ResetComboItems(NamedTypes);
  CExecAllDefs* execAllPatt = new CExecAllDefs(myDoc, NamedTypes, 0, myDECL->ParentDECL, OrigDECL, VirtualType, myDECL->TypeFlags);
  delete execAllPatt;
  if (!SetSelections(BasicTypes, NamedTypes, valNewTypeType))
    valNewTypeType = QString("");
  UpdateData(false);
}

void CVTypeBox::on_EnableName_clicked() 
{
  UpdateData(true);
  if (EnableName->isChecked()) 
    NewName->setEnabled(true);
  else {
    valNewName = QString(myDoc->IDTable.GetDECL(((CHETID*)myDECL->Supports.first)->data)->LocalName.c);
    NewName->setEnabled(false);
  }
  UpdateData(false);  
}

void CVTypeBox::reject()
{
  ResetComboItems(NamedTypes);
  ResetComboItems(BasicTypes);
  QDialog::reject();
}


void CVTypeBox::on_ID_OK_clicked() 
{
  UpdateData(true);
  bool extendsSet = false, extendsArray = false, extendsException = false, extendsChain = false,
     extendsEnum = false, extendsGUI=false;
  LavaDECL* suDECL;
  CHETID* cheS;

  QString* ids = CheckNewName(valNewName, myDECL, myDoc);
  if (ids) {
    QMessageBox::critical(this,qApp->applicationName(),*ids,QMessageBox::Ok,0,0);
    //NewName->SetSel(0, -1);
    NewName->setFocus();
    return;
  }
  if (EnableName->isChecked()) 
    myDECL->SecondTFlags.INCL(enableName);
  else
    myDECL->SecondTFlags.EXCL(enableName);
  if (DefCat->isChecked()) {
    myDECL->TypeFlags.INCL(definesObjCat);
    myDECL->TypeFlags.INCL(definiteCat);
    if (StateObject->isChecked()) {
      myDECL->TypeFlags.INCL(isStateObjectY);
      myDECL->TypeFlags.EXCL(isAnyCatY);
    }
    else if (AnyCategory->isChecked()) {
      myDECL->TypeFlags.INCL(isAnyCatY);
      myDECL->TypeFlags.EXCL(isStateObjectY);
    }
    else {
      myDECL->TypeFlags.EXCL(isStateObjectY);
      myDECL->TypeFlags.EXCL(isAnyCatY);
    }
  }
  else {
    myDECL->TypeFlags.EXCL(isStateObjectY);
    myDECL->TypeFlags.EXCL(isAnyCatY);
    myDECL->TypeFlags.EXCL(definesObjCat);
    myDECL->TypeFlags.EXCL(definiteCat);
  }
  if (Substitutable->isChecked()) 
    myDECL->TypeFlags.INCL(substitutable);
  else
    myDECL->TypeFlags.EXCL(substitutable);
  if (VTAbstract->isChecked()) {
    myDECL->TypeFlags.INCL(isAbstract);
    myDECL->TypeFlags.EXCL(definiteCat);
    myDECL->TypeFlags.EXCL(isStateObjectY);
    myDECL->RefID  = TID(-1,0);
    myDECL->DeclDescType = NamedType;
  }
  else 
    myDECL->TypeFlags.EXCL(isAbstract);
  if (!valNewTypeType.length() && !myDECL->TypeFlags.Contains(isAbstract)) {
    QMessageBox::critical(this,qApp->applicationName(),IDP_NoTypeSel,QMessageBox::Ok,0,0);
    BasicTypes->setFocus();
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
        extendsGUI = extendsGUI || suDECL->SecondTFlags.Contains(isGUI);
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
    if (extendsGUI) {
      myDECL->SecondTFlags.INCL(isGUI);
      myDECL->ParentDECL->RefID = myDECL->RefID;
    }
    else
      myDECL->SecondTFlags.EXCL(isGUI);
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
  ResetComboItems(NamedTypes);
  ResetComboItems(BasicTypes);
  QDialog::accept();
}



void CAttrBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/MemVarBox.htm");
}

void CCompSpecBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/COSpecBox.htm");
}

void CEnumBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/EnumBox.htm");
}

void CEnumItem::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/EnumItemBox.htm");
}

void CFuncBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/FunctionBox.htm");
}

void CImplBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/ImplementationBox.htm");
}

void CIncludeBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/IncludeBox.htm");
}

void CInitBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/InitiatorBox.htm");
}

void CInterfaceBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/InterfaceBox.htm");
}

void CIOBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/FuncParmBox.htm");
}

void CPackageBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/PackageBox.htm");
}

void CSetBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/dialogs/SetBox.htm");
}

void CVTypeBox::on_ID_HELP_clicked()
{
	QString path(QASSISTANT);
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
  /*CComboBoxItem *comboItem;
  comboItem = (CComboBoxItem*)basics->listBox()->findItem(name, Qt::ExactMatch | Qt::CaseSensitive);
  if (comboItem) {
    basics->setCurrentText(comboItem->text());*/
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


int SelEndOKToList(QComboBox* cbox, QListWidget* list, int chpos)
{
  QList<QListWidgetItem*> found;
  CListBoxItem *listItem;
  QVariant var;
  int pos = cbox->currentIndex();
  if (pos > 0) {
    found = list->findItems(cbox->currentText(),Qt::MatchExactly);
    if (!found.isEmpty()) {
      found.first()->setSelected(true);
      return -1;
    }
    if (chpos >= 0) 
      delete list->takeItem(chpos);
    var = cbox->itemData(pos);
    listItem = new CListBoxItem(cbox->currentText(), var.value<CComboBoxItem*>()->itemData());
    list->addItem(listItem);
    return 1;
  }
  return -1;
}

int SelEndOKToStr(QComboBox* cbox, QString* editStr, TID* exID)
{
  QVariant var;
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


void ListToChain(QListWidget* list, TSupports* supports)
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
      box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
      box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
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
  //SortCombo(Combo);
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
                  && (myDECL->TypeFlags.Contains(isStateObjectY) == elDef->TypeFlags.Contains(isStateObjectY))
               )) {
          //item = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, elDef->inINCL));
          //Combo->listBox()->insertItem(item);//sort
          item = new CComboBoxItem(TID(elDef->OwnID, elDef->inINCL));
          addItemAlpha(Combo, QString(elDef->FullName.c), QVariant::fromValue(item));//sort
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
          addItemAlpha(Combo, QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it]), QVariant::fromValue(item));//sort
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
            addItemAlpha(Combo, QString(elDef->FullName.c), QVariant::fromValue(item));//sort
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
          addItemAlpha(Combo, QString(((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[it]), QVariant::fromValue(item));//sort
        }
      }
}

 
CExecBase::CExecBase(CInterfaceBox* box)
{
  IBox = box;
  TabTravers = new CTabTraversal(this, IBox->myDoc->mySynDef);
  TabTravers->Run(true, true);
  //SortCombo(IBox->ExtTypes);
}

void CExecBase::ExecDefs(LavaDECL ** pelDef, int level)
{
  if ((*pelDef)->fromBType != NonBasic)
    return;
  LavaDECL *elDef = *pelDef;
  DString lab;
  bool sameContext;
  CContext con;
  CComboBoxItem *comboItem;

  if ((elDef->DeclType != Interface) && (elDef->DeclType != VirtualType))
    return;
  if (!elDef->usableIn(IBox->myDECL->ParentDECL))
    return;
  if (elDef == IBox->OrigDECL)
    return;
  IBox->myDoc->IDTable.GetPattern(elDef, con);
  if ( IBox->myDoc->IDTable.InsertBaseClass(IBox->myDECL, elDef, IBox->ContextDECL, false)
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
    //comboItem = new CComboBoxItem(lab, TID(elDef->OwnID, elDef->inINCL));
    //IBox->ExtTypes->listBox()->insertItem(comboItem);//sort
    comboItem = new CComboBoxItem(TID(elDef->OwnID, elDef->inINCL));
    addItemAlpha(IBox->ExtTypes, QString(lab.c), QVariant::fromValue(comboItem));//sort
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
  //SortCombo(Combo);
}


void CExecDefs::ExecDefs (LavaDECL ** pelDef, int )
{
  CComboBoxItem *comboItem;

  if ( ((*pelDef)->DeclType == Package)
      && (*pelDef)->LocalName.l
      && !myDECL->isInSubTree(*pelDef)
      && !(*pelDef)->isInSubTree(myDECL)) {
    CHE* che = (CHE*)(*pelDef)->NestedDecls.first;
    if (che && (((LavaDECL*)che->data)->DeclType == VirtualType)) { 
//      comboItem = new CComboBoxItem((*pelDef)->FullName, TID((*pelDef)->OwnID, (*pelDef)->inINCL));
//      Combo->listBox()->insertItem(comboItem);//sort
      comboItem = new CComboBoxItem(TID((*pelDef)->OwnID, (*pelDef)->inINCL));
      addItemAlpha(Combo, QString((*pelDef)->FullName.c), QVariant::fromValue(comboItem));//sort
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
  CComboBoxItem *comboItemA, *comboItemB;
  LavaDECL* decl;
  int pos, posA, posB, icount = combo->count();
  QVariant var;

  for (pos = 2; pos < icount; pos++) {
    if (nowB) {
      var = combo->itemData(pos);
      comboItemB = var.value<CComboBoxItem*>();
      strB = combo->itemText(pos);
      posB = pos;
    }
    else {
      var = combo->itemData(pos);
      comboItemA = var.value<CComboBoxItem*>();
      strA = combo->itemText(pos);
      posA = pos;
    }
    if (strB == strA) {
      decl = myDoc->IDTable.GetDECL(comboItemA->itemData());
      comboItemA = new CComboBoxItem(comboItemA->itemData());
      combo->setItemText(posA, QString(decl->FullName.c));
      combo->setItemData(posA, QVariant::fromValue(comboItemA));
      //combo->listBox()->changeItem(comboItemA, posA);
      //sz = dc->GetTextExtent(decl->FullName.c, decl->FullName.l);
      //maxWidth = lmax(maxWidth, sz.cx);
      decl = myDoc->IDTable.GetDECL(comboItemB->itemData());
      //comboItemB = new CComboBoxItem(decl->FullName, comboItemB->itemData());
      //combo->listBox()->changeItem(comboItemB, posB);
      comboItemB = new CComboBoxItem(comboItemB->itemData());
      combo->setItemText(posB, QString(decl->FullName.c));
      combo->setItemData(posB, QVariant::fromValue(comboItemB));
      //sz = dc->GetTextExtent(decl->FullName.c, decl->FullName.l);
      //maxWidth = lmax(maxWidth, sz.cx);
    }
    nowB = !nowB;
  }
  //SortCombo(combo);
  combo->setCurrentIndex(0);
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
  CComboBoxItem *comboItem;
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
    //comboItem = new CComboBoxItem(lab, TID(elDef->OwnID, incl));
    //Combo->listBox()->insertItem(comboItem);//sort
    comboItem = new CComboBoxItem(TID(elDef->OwnID, incl));
    addItemAlpha(Combo, QString(lab.c), QVariant::fromValue(comboItem));//sort
  }
  /*
  if (((DeclType == Interface) || (DeclType == CompObjSpec))
    && Combo2  && elDef->SecondTFlags.Contains(isEnum)) {
    comboItem = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, incl));
    Combo2->listBox()->insertItem(comboItem);//sort
  }
  */
  if (elType == VirtualType) 
    decl = myDoc->IDTable.GetDECL(elDef->RefID, elDef->inINCL);
  if (Combo2 && //List3 && 
       (elType == Interface) && !elDef->SecondTFlags.Contains(isGUI)
     ) {
    if (ParentDECL && ParentDECL->FullName.l) {
      LocalName = ((CLavaPEApp*)wxTheApp)->LBaseData.calcRelName (elDef->FullName, ParentDECL->FullName);
//      comboItem = new CComboBoxItem(LocalName, TID(elDef->OwnID, incl));
//      Combo2->listBox()->insertItem(comboItem);//sort
      comboItem = new CComboBoxItem(TID(elDef->OwnID, incl));
      addItemAlpha(Combo2, QString(LocalName.c), QVariant::fromValue(comboItem));//sort
    }
    else {
      comboItem = new CComboBoxItem(TID(elDef->OwnID, incl));
      addItemAlpha(Combo2, QString(elDef->FullName.c), QVariant::fromValue(comboItem));//sort
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

