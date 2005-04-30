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


#include "docview.h"
#include "qstring.h"
#include "LavaAppBase.h"
//#include "Resource.h"
#include "ComboBar.h"
#include "ExecView.h"
#include "PEBaseDoc.h"
#include "qpopupmenu.h"
#include "qlayout.h"


int SelEndOKToStr(int pos, QComboBox* cbox, QString* editStr, TID* exID)
{
  if (pos > 0) {
    QListBoxItem *comboItem = cbox->listBox()->item(pos);
    if (comboItem) {
      *editStr = ((CListItem*)comboItem)->text();
      if (exID)
        *exID = ((CListItem*)comboItem)->itemData();
    }
    else
      pos = -1;
  }
  return pos;
}


void ResetComboItems(QComboBox* box)
{
  int pos, c = box->count();
  for (pos = 1; pos < c; pos++)
    box->removeItem(1);
}

/////////////////////////////////////////////////////////////////////////////
// CComboBar dialog


CComboBar::CComboBar()
{
}

/////////////////////////////////////////////////////////////////////////////

#undef QMainWindow

CComboBar::CComboBar(LavaDECL* execDecl, CPEBaseDoc *doc, QMainWindow* parent)
:QDockWindow(parent, "ComboBar")//:QToolBar( parent ) 
{
  ExecDECL = execDecl;
  myDECL = execDecl->ParentDECL;
  if (myDECL->DeclType == Function)
    SelfTypeDECL = myDECL->ParentDECL;
  else
    SelfTypeDECL = myDECL;
  myDoc = doc;
  if (SelfTypeDECL->DeclType == Impl)
    IntfDECL = myDoc->IDTable.GetDECL(((CHETID*)SelfTypeDECL->Supports.first)->data, SelfTypeDECL->inINCL);
  else
    IntfDECL = SelfTypeDECL;
  FuncParentDecl = 0;
	NewFuncShow = false;
  m_ComboBarDlg = new ComboBarDlg(this);
  m_ComboBarDlg->adjustSize();
  adjustSize();
  SetCombos(true, false);
  myDoc->MakeBasicBox(m_BasicTypesCtrl, NoDef, true);
  m_BasicTypesCtrl->setCurrentItem(0);
  m_ObjectsCtrl->show();

  m_TypesCtrl->listBox()->setVariableWidth(true);        
  m_SetTypesCtrl->listBox()->setVariableWidth(true);  
  //m_SignalsCtrl->listBox()->setVariableWidth(true);   
  m_BasicTypesCtrl->listBox()->setVariableWidth(true);  
  m_EnumsCtrl->listBox()->setVariableWidth(true);
  m_NewCtrl->listBox()->setVariableWidth(true); 
  m_SNewCtrl->listBox()->setVariableWidth(true); 
  m_ObjectsCtrl->listBox()->setVariableWidth(true);
  m_SetObjectsCtrl->listBox()->setVariableWidth(true);
  m_AttachCtrl->listBox()->setVariableWidth(true);
  m_CallIntCtrl->listBox()->setVariableWidth(true);
  m_CompoObjIntCtrl->listBox()->setVariableWidth(true);
  m_ClassFuncsCtrl->listBox()->setVariableWidth(true);
  m_VFuncsCtrl->listBox()->setVariableWidth(true);
  m_SubObjectsCtrl->listBox()->setVariableWidth(true);
  m_CompaObjectsCtrl->listBox()->setVariableWidth(true);
  m_BaseInisCtrl->listBox()->setVariableWidth(true);
  m_StaticFuncsCtrl->listBox()->setVariableWidth(true);
  m_CompaTypesCtrl->listBox()->setVariableWidth(true);
  m_CompaBTypesCtrl->listBox()->setVariableWidth(true);


  m_EnumsCtrl->show();
  EnumsEnable = false;
  EnumsShow = true;
  NewFuncEnable = false;
  NewPFuncEnable = false;

  CContext context;
  myDoc->NextContext(myDECL, context);
  if (myDECL->DeclType == Function) {
    context.ContextFlags.INCL(selfiContext);
    context.ContextFlags.INCL(selfoContext);
    if (myDECL->TypeFlags.Contains(forceOverride))
      context.ContextFlags.INCL(staticContext);
    else
      context.ContextFlags.EXCL(staticContext);
  }
  else {
    context.ContextFlags.INCL(staticContext);
    context.ContextFlags.EXCL(selfiContext);
    context.ContextFlags.EXCL(selfoContext);
  }
  for (int ii = 0; ii < hcombosEnum; ii++)
    VisibleList[ii] = false;
  ((SelfVar*)ExecDECL->Exec.ptr)->selfCtx = context;
  OnUpdate(ExecDECL,true);   //calculates all other boxes
  connect (&EnumMenu, SIGNAL(activated(int)), this, SLOT(OnEnumMenu(int )));
  connect (m_ComboBarDlg->IDC_ComboObjects, SIGNAL(activated(int)), this, SLOT(OnSelendokComboObjects(int)));
  connect (m_ComboBarDlg->IDC_ComboSetObjects, SIGNAL(activated(int)), this, SLOT(OnSelendokComboSetObjects(int)));
  connect (m_ComboBarDlg->IDC_ComboCompObjects, SIGNAL(activated(int)), this, SLOT(OnSelendokComboCompObjects(int)));
  connect (m_ComboBarDlg->IDC_ComboSubObjects, SIGNAL(activated(int)), this, SLOT(OnSelendokComboSubObjects(int)));
  connect (m_ComboBarDlg->IDC_ComboTypes, SIGNAL(activated(int)), this, SLOT(OnSelendokComboTypes(int)));
  connect (m_ComboBarDlg->IDC_CompaTypes, SIGNAL(activated(int)), this, SLOT(OnSelendokCompaTypes(int)));
  connect (m_ComboBarDlg->IDC_ComboFuncs, SIGNAL(activated(int)), this, SLOT(OnSelendokComboFuncs(int)));
  connect (m_ComboBarDlg->IDC_ComboBaseInis, SIGNAL(activated(int)), this, SLOT(OnSelendokComboBaseInis(int)));
  connect (m_ComboBarDlg->IDC_BasicTypes, SIGNAL(activated(int)), this, SLOT(OnSelendokBasicTypes(int)));
  connect (m_ComboBarDlg->IDC_CompaBTypes, SIGNAL(activated(int)), this, SLOT(OnSelendokCompaBTypes(int)));
  connect (m_ComboBarDlg->IDC_ComboClassFuncs, SIGNAL(activated(int)), this, SLOT(OnSelendokComboClassFuncs(int)));
  connect (m_ComboBarDlg->IDC_ComboNew, SIGNAL(activated(int)), this, SLOT(OnSelendokComboNew(int)));
  connect (m_ComboBarDlg->IDC_ComboSNew, SIGNAL(activated(int)), this, SLOT(OnSelendokComboSNew(int)));
  connect (m_ComboBarDlg->IDC_ComboAttach, SIGNAL(activated(int)), this, SLOT(OnSelendokComboAttach(int)));
  connect (m_ComboBarDlg->IDC_ComboEnums, SIGNAL(activated(int)), this, SLOT(OnSelendokComboEnums(int)));
  connect (m_ComboBarDlg->IDC_ComboSetTypes, SIGNAL(activated(int)), this, SLOT(OnSelendokComboSetTypes(int)));
  connect (m_ComboBarDlg->IDC_CompoInterf, SIGNAL(activated(int)), this, SLOT(OnSelendokCompoInterf(int)));
  connect (m_ComboBarDlg->IDC_COMBOCall, SIGNAL(activated(int)), this, SLOT(OnSelendokCOMBOCall(int)));
  //connect (m_ComboBarDlg->IDC_COMBOSignals, SIGNAL(activated(int)), this, SLOT(OnSelendokCOMBOSignals(int)));
  connect (m_ComboBarDlg->IDC_StaticFuncs, SIGNAL(activated(int)), this, SLOT(OnSelendokStaticFuncs(int)));
  connect (m_ComboBarDlg->IDC_ButtonEnum, SIGNAL(clicked()), this, SLOT(OnButtonEnum()));
  connect (m_ComboBarDlg->IDC_NewFunc, SIGNAL(clicked()), this, SLOT(OnNewFunc()));
  connect (m_ComboBarDlg->IDC_NewPFunc, SIGNAL(clicked()), this, SLOT(OnNewPFunc()));
 
}

void CComboBar::OnUpdate(LavaDECL *execDecl, bool externalHint)
{
  SetCombos(true, false);
  DeleteObjData(false); //delete the object combos
  if (externalHint) {
    ExecDECL = execDecl;
    myDECL = execDecl->ParentDECL;
    LeftCombo = 0;
    RightCombo = 0;
    ThirdCombo = 0;
    if (myDECL->DeclType == Function)
      SelfTypeDECL = myDECL->ParentDECL;
    else
      SelfTypeDECL = myDECL;
    if (SelfTypeDECL->DeclType == Impl)
      IntfDECL = myDoc->IDTable.GetDECL(((CHETID*)SelfTypeDECL->Supports.first)->data, SelfTypeDECL->inINCL);
    else
      IntfDECL = SelfTypeDECL;
    ResetComboItems(m_TypesCtrl);
    ResetComboItems(m_ObjectsCtrl);
    //ResetComboItems(m_SignalsCtrl);
    ResetComboItems(m_SetTypesCtrl);
    ResetComboItems(m_NewCtrl);
    ResetComboItems(m_SNewCtrl);
    ResetComboItems(m_AttachCtrl);
    ResetComboItems(m_CallIntCtrl);
    ResetComboItems(m_EnumsCtrl);
    ResetComboItems(m_CompoObjIntCtrl);

    CExecTypes * execFuncType = new CExecTypes(this);
    delete execFuncType;
    CExecFields* execFields = new CExecFields(this, myDECL, ((SelfVar*)ExecDECL->Exec.ptr)->selfCtx);
    delete execFields;
    m_TypesCtrl->setCurrentItem(0);
    m_SetTypesCtrl->setCurrentItem(0);
    //m_SignalsCtrl->setCurrentItem(0);
    m_ObjectsCtrl->setCurrentItem(0);
    m_EnumsCtrl->setCurrentItem(0);
  //  m_StatFuncsCtrl->setCurrentItem(0);
    m_NewCtrl->setCurrentItem(0);
    m_SNewCtrl->setCurrentItem(0);
    m_AttachCtrl->setCurrentItem(0);
    m_CallIntCtrl->setCurrentItem(0);
    m_CompoObjIntCtrl->setCurrentItem(0);
  }
  lastCombo = invalidateLast;
  lastOK = false;
}


void CComboBar::DeleteObjData(bool setCombo)
{
  //int pos;
  //CFieldsItemData * data;

  if (setCombo) {
    m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
    m_SubObjectsCtrl = m_ComboBarDlg->IDC_ComboSubObjects;
    m_SetObjectsCtrl = m_ComboBarDlg->IDC_ComboSetObjects;
    m_CompaObjectsCtrl = m_ComboBarDlg->IDC_ComboCompObjects;
  }

  ResetComboItems(m_SubObjectsCtrl);
  ResetComboItems(m_SetObjectsCtrl);
  ResetComboItems(m_CompaObjectsCtrl);

}


void CComboBar::OnSelendokComboObjects(int pos) 
{
  m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
  //m_ObjectsCtrl->ShowDropDown(false);
  //int pos = m_ObjectsCtrl->currentItem();
  lastOK = pos > 0; //&& (pos > 0);
  if (lastOK) {
    //m_ObjectsCtrl->GetLBText(pos, lastSelStr);
    CFieldsItem *data = (CFieldsItem*)m_ObjectsCtrl->listBox()->item(pos);
    lastSelStr = data->text();
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,false);
  }
  else
    m_ObjectsCtrl->setCurrentItem(0);
}

void CComboBar::OnSelendokComboCompObjects(int pos) 
{
  m_CompaObjectsCtrl = m_ComboBarDlg->IDC_ComboCompObjects;
  //m_CompaObjectsCtrl->ShowDropDown(false);
  //int pos = m_CompaObjectsCtrl->currentItem();
  lastOK = pos > 0;
  if (lastOK) {
    CFieldsItem *data = (CFieldsItem*)m_CompaObjectsCtrl->listBox()->item(pos);
    lastSelStr = data->text();
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,false);
  }
  else 
    m_CompaObjectsCtrl->setCurrentItem(0);

  /*
  lastOK = pos && (pos > 0);
  if (lastOK) {
    m_CompaObjectsCtrl->GetLBText(pos, lastSelStr);
    CFieldsItemData *data = (CFieldsItemData*)m_CompaObjectsCtrl->GetItemData(pos);
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,false);
  }
  else 
    m_CompaObjectsCtrl->setCurrentItem(0);
    */
}

void CComboBar::OnSelendokComboSetObjects(int pos) 
{
  m_SetObjectsCtrl = m_ComboBarDlg->IDC_ComboSetObjects;
  //m_SetObjectsCtrl->ShowDropDown(false);
  //int pos = m_SetObjectsCtrl->currentItem();
  lastOK = pos > 0;
  if (lastOK) {
    CFieldsItem *data = (CFieldsItem*)m_SetObjectsCtrl->listBox()->item(pos);
    lastSelStr = data->text();
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,false);
  }
  else 
    m_SetObjectsCtrl->setCurrentItem(0);
  /*
  lastOK = pos && (pos > 0);
  if (lastOK) {
    m_SetObjectsCtrl->GetLBText(pos, lastSelStr);
    CFieldsItemData *data = (CFieldsItemData*)m_SetObjectsCtrl->GetItemData(pos);
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,false);
  }
  else 
    m_SetObjectsCtrl->setCurrentItem(0);
    */
}


void CComboBar::OnSelendokComboEnums(int pos) 
{
  CHEEnumSelId* enumsel;
  LavaDECL* decl;

  m_EnumsCtrl = m_ComboBarDlg->IDC_ComboEnums;
  //m_EnumsCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_EnumsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK) {
    if (lastCombo == objSetEnumCombo)
      ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
    else {
      EnumMenu.clear();//DestroyMenu( );
      //EnumMenu.CreatePopupMenu( );
      m_ButtonEnum = m_ComboBarDlg->IDC_ButtonEnum;
      decl = myDoc->IDTable.GetDECL(lastSelTID);
      if (decl && (decl->DeclDescType == EnumType)) {
        pos = 0;
        for (enumsel = (CHEEnumSelId*)((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Items.first;
             enumsel;
             enumsel = (CHEEnumSelId*)enumsel->successor) {   
          //EnumMenu.AppendMenu(MF_STRING, pos, enumsel->data.Id.c);
          EnumMenu.insertItem(enumsel->data.Id.c, pos);
          pos++;
        }
        EnumsEnable = true;
      }
    }
  }
  else {
    EnumsEnable = false;
    m_EnumsCtrl->setCurrentItem(0);
  }
}

void CComboBar::TrackEnum()
{
  QPushButton* button = m_ComboBarDlg->IDC_ButtonEnum;
  button->setFocus();
  EnumMenu.popup (button->mapToGlobal(QPoint(0, 0)) );
}

void CComboBar::OnButtonEnum() 
{
  QPushButton* button = m_ComboBarDlg->IDC_ButtonEnum;
  EnumMenu.popup (button->mapToGlobal(QPoint(0, 0)) );
}

void CComboBar::OnNewPFunc()
{
  TID id;
  LavaDECL *funcDecl;

  CExecView *view = (CExecView*)wxDocManager::GetDocumentManager()->GetActiveView();
  id.nID = myDoc->MakeFunc(SelfTypeDECL, false, view);
  id.nINCL = 0;
  view->GetParentFrame()->Activate(true);
  if (id.nID > 0) {
    funcDecl = myDoc->IDTable.GetDECL(id);
    QString refName = QString(funcDecl->LocalName.c);
    view->OnInsertRef(refName, id, false, 0, true);
  }
}

void CComboBar::OnNewFunc()
{
  if (FuncParentDecl->DeclType == Impl)
    FuncParentDecl = myDoc->IDTable.GetDECL(((CHETID*)FuncParentDecl->Supports.first)->data, FuncParentDecl->inINCL);
  TID otherID, id = TID(FuncParentDecl->OwnID, FuncParentDecl->inINCL);
  DString fn;
  CPEBaseDoc* doc;
  LavaDECL* destDECL = 0, *funcDecl;

  CExecView *view = (CExecView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (id.nINCL > 0) {
    fn = myDoc->IDTable.GetRelSynFileName(id);
    AbsPathName(fn, myDoc->IDTable.DocDir);
    doc = (CPEBaseDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(fn.c);
    if (doc) 
      destDECL = doc->IDTable.GetDECL(0, id.nID);
  }
  else {
    doc = myDoc;
    destDECL = FuncParentDecl;
  }
  if (destDECL) 
    id.nID = doc->MakeFunc(destDECL, doc != myDoc, view);
  view->GetParentFrame()->Activate(true);
  if (id.nID > 0) {
    otherID = TID(id.nID, 0);
    funcDecl = doc->IDTable.GetDECL(otherID);
    QString refName = QString(funcDecl->LocalName.c);
    view->OnInsertRef(refName, id, false, 0, doc == myDoc);
  }
}


void CComboBar::OnEnumMenu(int id) 
{
  lastSelStr = EnumMenu.text(id);
  m_ComboBarDlg->IDC_ComboEnums->setFocus();
  ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertEnum(lastSelStr, lastSelTID, id);
}


void CComboBar::OnSelendokComboSubObjects(int pos) 
{
  m_SubObjectsCtrl = m_ComboBarDlg->IDC_ComboSubObjects;
  //m_SubObjectsCtrl->ShowDropDown(false);
  //int pos = m_SubObjectsCtrl->currentItem();
  lastOK = pos > 0;
  if (lastOK) {
    CFieldsItem *data = (CFieldsItem*)m_SubObjectsCtrl->listBox()->item(pos);
    lastSelStr = data->text();
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs,true);
  }
  else 
    m_SubObjectsCtrl->setCurrentItem(0);
  /*
  lastOK = pos && (pos > 0);
  if (lastOK) {
    m_SubObjectsCtrl->GetLBText(pos, lastSelStr);
    CFieldsItemData *data = (CFieldsItemData*)m_SubObjectsCtrl->GetItemData(pos);
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertObjRef(lastSelStr, data->IDs, true);
  }
  else 
    m_SubObjectsCtrl->setCurrentItem(0);
  */
}

void CComboBar::OnSelendokBasicTypes(int pos) 
{
  m_BasicTypesCtrl = m_ComboBarDlg->IDC_BasicTypes;
  //m_BasicTypesCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_BasicTypesCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK) 
    if (onShowStatic) {
      showIFFuncs();
    }
    else 
      ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
    else {
    m_BasicTypesCtrl->setCurrentItem(0);
  }
}

void CComboBar::OnSelendokCompaBTypes(int pos) 
{
  m_CompaBTypesCtrl = m_ComboBarDlg->IDC_CompaBTypes;
  //m_CompaBTypesCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_CompaBTypesCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK) 
    if (onShowStatic) {
      showIFFuncs();
    }
    else 
      ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
    else {
    m_CompaBTypesCtrl->setCurrentItem(0);
  }
}

/*
void CComboBar::OnSelendokComboBasicUpcasts() 
{
  m_BasicUpcastsCtrl = m_ComboBarDlg->IDC_ComboBasicUpcasts;
  int pos = SelEndOKToStr(m_BasicUpcastsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK) 
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else
    m_BasicUpcastsCtrl->setCurrentItem(0);
}
*/

void CComboBar::OnSelendokComboTypes(int pos) 
{
  m_TypesCtrl = m_ComboBarDlg->IDC_ComboTypes;
  //m_TypesCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_TypesCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    if (onShowStatic) 
      showIFFuncs();
    else
      ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_TypesCtrl->setCurrentItem(0);
}

void CComboBar::OnSelendokCompaTypes(int pos) 
{
  m_CompaTypesCtrl = m_ComboBarDlg->IDC_CompaTypes;
  //m_CompaTypesCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_CompaTypesCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    if (onShowStatic) 
      showIFFuncs();
    else
      ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_CompaTypesCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokComboSetTypes(int pos) 
{
  m_SetTypesCtrl = m_ComboBarDlg->IDC_ComboSetTypes;
  //m_SetTypesCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_SetTypesCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_SetTypesCtrl->setCurrentItem(0); 
}

/*
void CComboBar::OnSelendokCOMBOSignals(int pos) 
{
  m_SignalsCtrl = m_ComboBarDlg->IDC_COMBOSignals;
  //m_SignalsCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_SignalsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_SignalsCtrl->setCurrentItem(0);
}
*/

/*
void CComboBar::OnSelendokComboUpcasts() 
{
  m_UpcastsCtrl = m_ComboBarDlg->IDC_ComboUpcasts;
  int pos = SelEndOKToStr(m_UpcastsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_UpcastsCtrl->setCurrentItem(0);
}
*/

void CComboBar::OnSelendokComboFuncs(int pos) 
{ //Virtual functions
  m_VFuncsCtrl = m_ComboBarDlg->IDC_ComboFuncs;
  //m_VFuncsCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_VFuncsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_VFuncsCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokComboClassFuncs(int pos) 
{
  m_ClassFuncsCtrl = m_ComboBarDlg->IDC_ComboClassFuncs;
  CStatFuncItem *idata;
  //int pos = m_ClassFuncsCtrl->currentItem();
  lastOK = pos > 0;
  if (lastOK) {
    idata = (CStatFuncItem*)m_ClassFuncsCtrl->listBox()->item(pos);
    lastSelStr = idata->text();
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, idata->funcID, true, &idata->vtypeID);
  }
  else 
    m_ClassFuncsCtrl->setCurrentItem(0);

  /*
  //m_ClassFuncsCtrl->ShowDropDown(false);
  //int pos = SelEndOKToStr(m_ClassFuncsCtrl, &lastSelStr, &lastSelTID);
  if (pos > 0) {
    m_ClassFuncsCtrl->GetLBText(pos, lastSelStr);
    idata = (CStatFuncItem*)m_ClassFuncsCtrl->GetItemData(pos);
  }
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, idata->funcID, true, &idata->vtypeID);
  else 
    m_ClassFuncsCtrl->setCurrentItem(0);
  */
}


void CComboBar::OnSelendokComboBaseInis(int pos) 
{
  m_BaseInisCtrl = m_ComboBarDlg->IDC_ComboBaseInis;
  //m_BaseInisCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_BaseInisCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID, true);
  else 
    m_BaseInisCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokComboNew(int pos) 
{
  m_NewCtrl = m_ComboBarDlg->IDC_ComboNew;
  //m_NewCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_NewCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_NewCtrl->setCurrentItem(0);
}

void CComboBar::OnSelendokComboSNew(int pos) 
{
  m_SNewCtrl = m_ComboBarDlg->IDC_ComboSNew;
  //m_SNewCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_SNewCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_SNewCtrl->setCurrentItem(0);
}

void CComboBar::OnSelendokComboAttach(int pos) 
{
  m_AttachCtrl = m_ComboBarDlg->IDC_ComboAttach;
  //m_AttachCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_AttachCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_AttachCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokCOMBOCall(int pos) 
{
  m_CallIntCtrl = m_ComboBarDlg->IDC_COMBOCall;
  //m_CallIntCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_CallIntCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_CallIntCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokCompoInterf(int pos) 
{
  m_CompoObjIntCtrl = m_ComboBarDlg->IDC_CompoInterf;
  //m_CompoObjIntCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_CompoObjIntCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_CompoObjIntCtrl->setCurrentItem(0);
}


void CComboBar::OnSelendokStaticFuncs(int pos) 
{
  m_StaticFuncsCtrl = m_ComboBarDlg->IDC_StaticFuncs;
  //m_StaticFuncsCtrl->ShowDropDown(false);
  pos = SelEndOKToStr(pos, m_StaticFuncsCtrl, &lastSelStr, &lastSelTID);
  lastOK = pos && (pos > 0);
  if (lastOK)
    ((CExecView*)wxDocManager::GetDocumentManager()->GetActiveView())->OnInsertRef(lastSelStr, lastSelTID);
  else 
    m_StaticFuncsCtrl->setCurrentItem(0);  
}


void CComboBar::RemoveLocals()
{
  CHE *firstID;
  TIDType idtype;
  m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
  m_SetObjectsCtrl = m_ComboBarDlg->IDC_ComboSetObjects;
  int pos = 1;
  int cnt = m_ObjectsCtrl->count();
  while ( pos < cnt) {
    firstID = (CHE*)((CFieldsItem*)m_ObjectsCtrl->listBox()->item(pos))->IDs.first;
    myDoc->IDTable.GetVar(((TDOD*)firstID->data)->ID, idtype); 
    if ((idtype == localID) || (idtype == noID)) {
      m_ObjectsCtrl->removeItem(pos);
      /*
      data = (CFieldsItemData*)m_ObjectsCtrl->GetItemData(pos);
      delete data;
      m_ObjectsCtrl->DeleteString(pos);
      */
      cnt--;
    }
    else
      pos++;
  }
  pos = 1;
  cnt = m_SetObjectsCtrl->count();
  while ( pos < cnt) {
    firstID = (CHE*)((CFieldsItem*)m_SetObjectsCtrl->listBox()->item(pos))->IDs.first;
    if (firstID) {
      if ((idtype == localID) || (idtype == noID)) { 
        m_SetObjectsCtrl->removeItem(pos);
        /*
        data = (CFieldsItemData*)m_SetObjectsCtrl->GetItemData(pos);
       myDoc->IDTable.GetVar(((TDOD*)firstID->data)->ID, idtype); 
       delete data;
        m_SetObjectsCtrl->DeleteString(pos);
        */
        cnt--;
      }
      else
        pos++;
    }
  }
}

void CComboBar::AddLocal(const TID& id, const DString& name, const TID& typeID, bool subst)
{
  LavaDECL* typeDECL;
  CFieldsItem * data = new CFieldsItem(name);
  TDODC accuIDs;
  TDOD *tdod = new TDODV(true);
  CHE *che;
  DString accuName;

  m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
  m_SetObjectsCtrl = m_ComboBarDlg->IDC_ComboSetObjects;
  tdod->ID = id;
  tdod->name = name;
  tdod->context = ((SelfVar*)ExecDECL->Exec.ptr)->selfCtx;
  che = NewCHE(tdod);
  accuIDs.Append(che);
  data->IDs = accuIDs;
  m_ObjectsCtrl->listBox()->insertItem(data);//sort#
  SortCombo(m_ObjectsCtrl);
  CContext con = tdod->context;
  if (typeID.nID >= 0) {
    accuName = name;
    typeDECL = myDoc->GetFinalMTypeAndContext(typeID, 0, con, 0);
    if (typeDECL) {
      if (subst) 
        con.ContextFlags = SET(multiContext,-1);
      if (typeDECL->SecondTFlags.Contains(isSet)) {
        data = new CFieldsItem(name);
        data->QName = name;
        data->IDs = accuIDs;
        m_SetObjectsCtrl->listBox()->insertItem(data);//sort#
        SortCombo(m_SetObjectsCtrl);
        m_SetObjectsCtrl->setCurrentItem(0);

      }
      CExecFields *execF = new CExecFields(this, typeDECL, con, false, &accuName, &accuIDs);
      delete execF;
    }
  }
}

bool CComboBar::UsedName(const DString& name)
{
  m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
  CFieldsItem* fdata = (CFieldsItem*)m_ObjectsCtrl->listBox()->findItem(QString(name.c), Qt::ExactMatch | Qt::CaseSensitive);
  return fdata != 0;
//  return FindStringCaseExact(m_ObjectsCtrl, name, 1) >= 0;
}

void CComboBar::ShowCombos(TShowCombo what, TID* pID)
{
  if (lastCombo != what) {
    lastOK = false;
    lastCombo = what;
    SetCombos(true, false);
    if (what == disableCombo) {
      m_TypesCtrl->setEnabled(false);
      m_SetTypesCtrl->setEnabled(false);
      //m_SignalsCtrl->setEnabled(false);
      m_BasicTypesCtrl->setEnabled(false);
      m_EnumsCtrl->setEnabled(false);
      m_ButtonEnum->setEnabled(false);
      EnumsEnable = false;
      m_SNewCtrl->setEnabled(false);
      m_NewCtrl->setEnabled(false);
      m_ObjectsCtrl->setEnabled(false);
      m_SetObjectsCtrl->setEnabled(false);
      m_AttachCtrl->setEnabled(false);
      m_CallIntCtrl->setEnabled(false);
      m_CompoObjIntCtrl->setEnabled(false);

      m_BaseInisCtrl->setEnabled(false);
      m_StaticFuncsCtrl->setEnabled(false);
      m_ClassFuncsCtrl->setEnabled(false);
      m_VFuncsCtrl->setEnabled(false);
      NewFuncEnable = false;
      NewPFuncEnable = false;
      m_NewFunc->setEnabled(false);
      m_NewPFunc->setEnabled(false);
      m_SubObjectsCtrl->setEnabled(false);
      m_CompaObjectsCtrl->setEnabled(false);
      m_CompaTypesCtrl->setEnabled(false);
      m_CompaBTypesCtrl->setEnabled(false);
    }
    else {
      SetCombos(false, true);
      switch (what) {
      case objCombo:
      case objEnumCombo:
        m_ObjectsCtrl->show();
        if (!VisibleList[v_Objects])
          m_ObjectsCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_ObjectsCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboObjects;
        if (what == objEnumCombo) {
          m_EnumsCtrl->show();
          if (!VisibleList[v_Enums])
            m_EnumsCtrl->setCurrentItem(0);
          if (!LBaseData->debugOn)
            m_EnumsCtrl->setEnabled(true);
          RightCombo = m_ComboBarDlg->IDC_ComboEnums;
          m_ButtonEnum->show();
          EnumsShow = true;
          if (pID) {
            LavaDECL* decl = myDoc->IDTable.GetDECL(*pID);
            if (decl) {
              QListBoxItem* item = m_EnumsCtrl->listBox()->findItem(decl->LocalName.c, Qt::ExactMatch | Qt::CaseSensitive);
              if (item)
                m_EnumsCtrl->setCurrentText(item->text());
            }
            //m_EnumsCtrl->SelectString(0, decl->LocalName.c);
          }
        }
        break;
      case objSetCombo:
        m_SetObjectsCtrl->show();
        if (!VisibleList[v_SetObjects])
          m_SetObjectsCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_SetObjectsCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboSetObjects;
        m_EnumsCtrl->hide();
        m_EnumsCtrl->setCurrentItem(0);
        m_EnumsCtrl->setEnabled(false);
        RightCombo = m_ComboBarDlg->IDC_ComboEnums;
        m_ButtonEnum->hide();
        EnumsShow = false;
        break;
      case objSetEnumCombo:
        m_SetObjectsCtrl->show();
        if (!VisibleList[v_SetObjects])
          m_SetObjectsCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_SetObjectsCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboSetObjects;
        m_EnumsCtrl->show();
        if (!VisibleList[v_Enums])
          m_EnumsCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_EnumsCtrl->setEnabled(true);
        RightCombo = m_ComboBarDlg->IDC_ComboEnums;
        m_ButtonEnum->hide();
        EnumsShow = false;
        break;
      case typeCombo:
        m_TypesCtrl->show();
        if (!VisibleList[v_Types])
          m_TypesCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_TypesCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboTypes;
        m_BasicTypesCtrl->show();
        if (!VisibleList[v_BasicTypes])
          m_BasicTypesCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_BasicTypesCtrl->setEnabled(true);
        RightCombo = m_ComboBarDlg->IDC_BasicTypes;
        break;
      case setTypeCombo:
        m_SetTypesCtrl->show();
        if (!VisibleList[v_SetTypes])
          m_SetTypesCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_SetTypesCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboSetTypes;
        break;
        /*
      case signalsCombo:
        m_SignalsCtrl->show();
        if (!VisibleList[v_Signals])
          m_SignalsCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_SignalsCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_COMBOSignals;
        break;
        */
      case newCombo:
        m_NewCtrl->show();
        if (!VisibleList[v_New])
          m_NewCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_NewCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboNew;
        break;
      case newAndCObjCombo:
        m_NewCtrl->show();
        if (!VisibleList[v_New])
          m_NewCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_NewCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboNew;
        m_SNewCtrl->show();
        if (!VisibleList[v_SNew])
          m_SNewCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_SNewCtrl->setEnabled(true);
        RightCombo = m_ComboBarDlg->IDC_ComboSNew;
        break;
      case attachCombo:
        m_AttachCtrl->show();
        if (!VisibleList[v_Attach])
          m_AttachCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_AttachCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_ComboAttach;
        break;
      case coiCombo:
        m_CompoObjIntCtrl->show();
        if (VisibleList[v_CompoObjInt])
          m_CompoObjIntCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_CompoObjIntCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_CompoInterf;
        break;
      case callCombo:
        m_CallIntCtrl->show();
        if (!VisibleList[v_CallInt])
          m_CallIntCtrl->setCurrentItem(0);
        if (!LBaseData->debugOn)
          m_CallIntCtrl->setEnabled(true);
        LeftCombo = m_ComboBarDlg->IDC_COMBOCall;
        break;
      default: ;
      }
    }
  }
}

void CComboBar::SetCombos(bool setVar, bool hideCombo)
{
  if (setVar) {
    m_TypesCtrl = m_ComboBarDlg->IDC_ComboTypes;
    m_CompaTypesCtrl = m_ComboBarDlg->IDC_CompaTypes;
    m_SetTypesCtrl = m_ComboBarDlg->IDC_ComboSetTypes;
    //m_SignalsCtrl = m_ComboBarDlg->IDC_COMBOSignals;
    m_BasicTypesCtrl = m_ComboBarDlg->IDC_BasicTypes;
    m_CompaBTypesCtrl = m_ComboBarDlg->IDC_CompaBTypes;
    m_EnumsCtrl = m_ComboBarDlg->IDC_ComboEnums;
    m_ButtonEnum = m_ComboBarDlg->IDC_ButtonEnum;
    m_NewCtrl = m_ComboBarDlg->IDC_ComboNew;
    m_SNewCtrl = m_ComboBarDlg->IDC_ComboSNew;
    m_ObjectsCtrl = m_ComboBarDlg->IDC_ComboObjects;
    m_SetObjectsCtrl = m_ComboBarDlg->IDC_ComboSetObjects;
    m_AttachCtrl = m_ComboBarDlg->IDC_ComboAttach;
    m_CallIntCtrl = m_ComboBarDlg->IDC_COMBOCall;
    m_CompoObjIntCtrl = m_ComboBarDlg->IDC_CompoInterf;
    m_VFuncsCtrl = m_ComboBarDlg->IDC_ComboFuncs;
    m_NewFunc = m_ComboBarDlg->IDC_NewFunc;
    m_NewPFunc = m_ComboBarDlg->IDC_NewPFunc;
    m_ClassFuncsCtrl = m_ComboBarDlg->IDC_ComboClassFuncs;
    m_SubObjectsCtrl = m_ComboBarDlg->IDC_ComboSubObjects;
    m_CompaObjectsCtrl = m_ComboBarDlg->IDC_ComboCompObjects;
    m_CompaTypesCtrl = m_ComboBarDlg->IDC_CompaTypes;
    m_CompaBTypesCtrl = m_ComboBarDlg->IDC_CompaBTypes;
    m_BaseInisCtrl = m_ComboBarDlg->IDC_ComboBaseInis;
    m_StaticFuncsCtrl = m_ComboBarDlg->IDC_StaticFuncs;
  }
  if (hideCombo) {
    LeftCombo = 0;
    RightCombo = 0;
    ThirdCombo = 0;
    VisibleList[v_VFuncs] = m_VFuncsCtrl->isVisible();
    VisibleList[v_ClassFuncs] = m_ClassFuncsCtrl->isVisible();
    VisibleList[v_Types] = m_TypesCtrl->isVisible();
    VisibleList[v_CompaTypes] = m_CompaTypesCtrl->isVisible();
    VisibleList[v_SetTypes] = m_SetTypesCtrl->isVisible();
    //VisibleList[v_Signals] = m_SignalsCtrl->isVisible();
    VisibleList[v_BasicTypes] = m_BasicTypesCtrl->isVisible();
    VisibleList[v_CompaBTypes] = m_CompaBTypesCtrl->isVisible();
    VisibleList[v_Objects] = m_ObjectsCtrl->isVisible();
    VisibleList[v_Enums] = m_EnumsCtrl->isVisible();
    VisibleList[v_SubObjects] = m_SubObjectsCtrl->isVisible();
    VisibleList[v_SNew] = m_SNewCtrl->isVisible();
    VisibleList[v_Attach] = m_AttachCtrl->isVisible();
    VisibleList[v_New] = m_NewCtrl->isVisible();
    VisibleList[v_SetObjects] = m_SetObjectsCtrl->isVisible();
    VisibleList[v_CallInt] = m_CallIntCtrl->isVisible();
    VisibleList[v_CompoObjInt] = m_CompoObjIntCtrl->isVisible();
    VisibleList[v_CompaObjects] = m_CompaObjectsCtrl->isVisible();
    VisibleList[v_BaseInis] = m_BaseInisCtrl->isVisible();
    VisibleList[v_StaticFuncs] = m_StaticFuncsCtrl->isVisible();

    m_VFuncsCtrl->hide();
    m_NewFunc->hide();
    m_NewPFunc->hide();
    NewFuncShow = false;
    m_ClassFuncsCtrl->hide();
    m_TypesCtrl->hide();
    m_CompaTypesCtrl->hide();
    m_SetTypesCtrl->hide();
    //m_SignalsCtrl->hide();
    m_BasicTypesCtrl->hide();
    m_CompaBTypesCtrl->hide();
    m_ObjectsCtrl->hide();
    m_EnumsCtrl->hide();
    m_ButtonEnum->hide();
    EnumsShow = false;
    m_SubObjectsCtrl->hide();
    m_SNewCtrl->hide();
    m_AttachCtrl->hide();
    m_NewCtrl->hide();
    m_SetObjectsCtrl->hide();
    m_CallIntCtrl->hide();
    m_CompoObjIntCtrl->hide();
    m_CompaObjectsCtrl->hide();
    m_BaseInisCtrl->hide();
    m_StaticFuncsCtrl->hide();
  }
  onShowStatic = false;
}

/* not used
void CComboBar::ShowUpcasts(const TID& id, bool theTop)     
//id is interface or implementation
{
  LavaDECL* decl = myDoc->IDTable.GetDECL(id);
  if (decl) {
    if (theTop)
      m_UpcastsCtrl = m_ComboBarDlg->IDC_ComboUpcasts;
    else {
      int pos = m_UpcastsCtrl->AddString(decl->LocalName.c);
      m_UpcastsCtrl->SetItemData(pos, MAKELONG(id.nID, id.nINCL));
    }
    cheID = (CHETID*)decl->Supports.first;
    while (cheID) {     
      ShowUpcasts(cheID->data, false);
      cheID = (CHETID*)cheID->successor;
    }
  }
}
*/

void CComboBar::ShowCompObjects(CheckData &ckd, LavaDECL* decl, const CContext &context, Category category, bool forInput, bool forCopy)
 //if decl != 0: decl is final virtual type
 //if decl = 0 local variables only for handle operator (#)
{
  Category cat = unknownCategory, catComp = unknownCategory;
  CContext compContext = context, bContext;
  LavaDECL *fmvType=0, *memDECL, *enumDecl;
  SynFlags ctxFlags;
  DString name;
  int cnt, pos; //, maxW = 0, npos, epos;
  CFieldsItem *data, *ndata;
  QListBoxItem* item;
  DWORD var;
  CHEEnumSelId* enumsel;
  TDOD *lastDOD;
  bool showObjs = false, showEnums = false, showEnumCombo=false, forHandle;
  TIDType idtype;
  QSize sz;

  m_CompaObjectsCtrl = m_ComboBarDlg->IDC_ComboCompObjects;
  m_EnumsCtrl = m_ComboBarDlg->IDC_ComboEnums;
  ResetComboItems(m_CompaObjectsCtrl);
  /*Q
  pos = m_CompaObjectsCtrl->GetCount();
  while (pos > 1) {
    ndata = (CFieldsItemData*)m_CompaObjectsCtrl->GetItemData(1);
    delete ndata;
    pos = m_CompaObjectsCtrl->DeleteString(1);
  }
  */

  EnumsShow = false;

  cnt = m_ObjectsCtrl->count();
  for (pos = 1; pos < cnt; pos++) {
    forHandle = false;
    data = (CFieldsItem*)m_ObjectsCtrl->listBox()->item(pos);
    if (data && data->IDs.last) {
      lastDOD = (TDOD*)((CHE*)data->IDs.last)->data;
      var = myDoc->IDTable.GetVar(lastDOD->ID, idtype); 
      catComp = unknownCategory;
      if (var) {
        if (idtype == globalID) {
          memDECL = *(LavaDECL**)var;
          bContext = lastDOD->context;
          if (memDECL->TypeFlags.Contains(trueObjCat)) 
            if (memDECL->TypeFlags.Contains(stateObject))
              catComp = stateObj;
            else 
              if (memDECL->TypeFlags.Contains(sameAsSelf))
                catComp = sameAsSelfObj;
              else 
                catComp = valueObj;
          myDoc->MemberTypeContext(memDECL, bContext,0);
          fmvType = myDoc->GetFinalMVType(memDECL->RefID, memDECL->inINCL, bContext, cat, 0);
          if (memDECL->TypeFlags.Contains(substitutable))
            bContext.ContextFlags = SET(multiContext,-1);
        }
        else {
          if (decl) {
            ((VarName*)var)->ExprGetFVType(ckd, fmvType, cat,ctxFlags);
            bContext = ckd.tempCtx;
            if (ctxFlags.bits)
              bContext.ContextFlags = ctxFlags;
          }
          else
            forHandle = true;
        }
        if (catComp == unknownCategory)
          catComp = cat;
        if ((forHandle || fmvType) && (forCopy || catComp == category || catComp == unknownCategory || category == unknownCategory)) {
          if (forHandle || !forHandle && 
              (   (( forInput || forCopy)
                              && compatibleTypes(ckd, fmvType, bContext, decl, compContext))
               || ((!forInput || forCopy)
                              && compatibleTypes(ckd, decl, compContext, fmvType, bContext)) )) {
            ndata = new CFieldsItem(data);
            m_CompaObjectsCtrl->listBox()->insertItem(ndata);//sort#
            showObjs = true;
          }
        }
      }
    }
  }
  SortCombo(m_CompaObjectsCtrl);
  item = 0;
  if (forInput && decl) {
    if (decl->DeclType == VirtualType)
      enumDecl = ckd.document->IDTable.GetDECL(decl->RefID, decl->inINCL);
    else
      enumDecl = decl;
    if (enumDecl && (enumDecl->DeclDescType == EnumType)) {
      item = m_EnumsCtrl->listBox()->findItem(enumDecl->FullName.c, Qt::ExactMatch | Qt::CaseSensitive);
      if (item) {
        m_EnumsCtrl->setCurrentText(item->text());
        lastSelStr = enumDecl->FullName.c;
      }
      else {
        item = m_EnumsCtrl->listBox()->findItem(enumDecl->LocalName.c, Qt::ExactMatch | Qt::CaseSensitive);
        if (item) {
          m_EnumsCtrl->setCurrentText(item->text());
          lastSelStr = enumDecl->LocalName.c;
        }
      }
      if (item) { //epos && (epos != LB_ERR)) {
        lastSelTID = TID(enumDecl->OwnID, enumDecl->inINCL);
        EnumMenu.clear();//DestroyMenu( );
        m_ButtonEnum = m_ComboBarDlg->IDC_ButtonEnum;
        pos = 0;
        for (enumsel = (CHEEnumSelId*)((TEnumDescription*)enumDecl->EnumDesc.ptr)->EnumField.Items.first;
             enumsel;
             enumsel = (CHEEnumSelId*)enumsel->successor) {   
          EnumMenu.insertItem(enumsel->data.Id.c, pos);
          pos++;
        }
        showEnums = true;
      }
    }
	  else if (enumDecl->fromBType == Enumeration) //SecondTFlags.Contains(isEnum)))
		  showEnumCombo = true;
  }
  if (showObjs || showEnums || showEnumCombo) {
    SetCombos(true, true);
    m_CompaObjectsCtrl->show();
    m_CompaObjectsCtrl->setCurrentItem(0);
    if (showObjs) {
//      m_CompaObjectsCtrl->SetDroppedWidth(maxW+6);
      if (!LBaseData->debugOn)
        m_CompaObjectsCtrl->setEnabled(true);
      LeftCombo = m_ComboBarDlg->IDC_ComboCompObjects;
    }
    else 
      m_CompaObjectsCtrl->setEnabled(false);
    if (showEnums) {
      EnumsShow = true;
      EnumsEnable = true;
      m_EnumsCtrl->show();
      m_EnumsCtrl->setEnabled(false);
      m_ButtonEnum->show();
      if (!LBaseData->debugOn)
        m_ButtonEnum->setEnabled(true);
    }
		else if (showEnumCombo) {
			EnumsShow = true;
			m_EnumsCtrl->show();
			if (!VisibleList[v_Enums])
				m_EnumsCtrl->setCurrentItem(0);
      if (!LBaseData->debugOn)
			  m_EnumsCtrl->setEnabled(true);
			RightCombo = m_ComboBarDlg->IDC_ComboEnums;
			m_ButtonEnum->show();
			if (m_EnumsCtrl->currentItem()) {
        if (!LBaseData->debugOn)
				  m_ButtonEnum->setEnabled(true);
				EnumsEnable = true;
			}
		}
  }
  else
    ShowCombos(disableCombo);
  lastCombo = invalidateLast;
}

void CComboBar::ShowCompaTypes(CheckData &ckd, LavaDECL *decl, const CContext &context)
{
  int pos, cnt; //, ipos, maxW=0, maxBW=0;
  //DWORD wID;
  //TID typeID;
  CContext compContext = ckd.lpc;
  LavaDECL *typeDecl;
  //QSize sz;
  QString label;
  CListItem *item, *itemComp;
  bool withBTypes=false, withTypes=false;

  m_TypesCtrl = m_ComboBarDlg->IDC_ComboTypes;
  m_BasicTypesCtrl = m_ComboBarDlg->IDC_BasicTypes;
  m_CompaTypesCtrl = m_ComboBarDlg->IDC_CompaTypes;
  m_CompaBTypesCtrl = m_ComboBarDlg->IDC_CompaBTypes;

  ResetComboItems(m_CompaTypesCtrl);
  ResetComboItems(m_CompaBTypesCtrl);
  /*Q
  pos = m_CompaTypesCtrl->GetCount();
  while (pos > 1) 
    pos = m_CompaTypesCtrl->DeleteString(1);
  pos = m_CompaBTypesCtrl->GetCount();
  while (pos > 1) 
    pos = m_CompaBTypesCtrl->DeleteString(1);
  */
  if (!decl)
    return;
 
  cnt = m_TypesCtrl->count();
  for (pos = 1; pos < cnt; pos++) {
    /*
    wID = m_TypesCtrl->GetItemData(pos);
    typeID.nID = LOWORD(wID);
    typeID.nINCL = HIWORD(wID);
    */
    item = (CListItem*)m_TypesCtrl->listBox()->item(pos);
    typeDecl = ckd.document->IDTable.GetDECL(item->itemData());
    if (compatibleTypes(ckd, typeDecl, compContext, decl, context)) {
      itemComp = new CListItem(item->text(), item->itemData());
      m_CompaTypesCtrl->listBox()->insertItem(itemComp);//sort#
      withTypes = true;
    }
  }
  SortCombo(m_CompaTypesCtrl);
  cnt = m_BasicTypesCtrl->count();
  for (pos = 1; pos < cnt; pos++) {
    item = (CListItem*)m_BasicTypesCtrl->listBox()->item(pos);
    typeDecl = ckd.document->IDTable.GetDECL(item->itemData());
    if (compatibleTypes(ckd, typeDecl, compContext, decl, context)) {
      itemComp = new CListItem(item->text(), item->itemData());
      m_CompaBTypesCtrl->listBox()->insertItem(itemComp);//sort#
      withBTypes = true;
    }
  }
  SortCombo(m_CompaBTypesCtrl);
  SetCombos(true, true);
  if (withTypes) {
    m_CompaTypesCtrl->show();
    m_CompaTypesCtrl->setCurrentItem(0);
    //m_CompaTypesCtrl->SetDroppedWidth(maxW+6);
    if (!LBaseData->debugOn)
      m_CompaTypesCtrl->setEnabled(true);
    LeftCombo = m_ComboBarDlg->IDC_CompaTypes;
  }
  else 
    m_CompaTypesCtrl->setEnabled(false);
  if (withBTypes) {
    m_CompaBTypesCtrl->show();
    m_CompaBTypesCtrl->setCurrentItem(0);
    //m_CompaBTypesCtrl->SetDroppedWidth(maxBW+6);
    if (!LBaseData->debugOn)
      m_CompaBTypesCtrl->setEnabled(true);
    RightCombo = m_ComboBarDlg->IDC_CompaBTypes;
  }
  else 
    m_CompaBTypesCtrl->setEnabled(false);
}

void CComboBar::ShowSubObjects(LavaDECL* decl, const CContext &context)
{
  //Annahme: die id ist id des "Roh-Typs" oder eines Feldes oder Input/Output,
  //deshalb sind die Aufrufe von GetFinalTypeAndContext erforderlich
  LavaDECL* mDECL;
  CContext con = context;
  TDODC accuIDs;
  DString accuName = pkt;
//  int pos;

  m_SubObjectsCtrl = m_ComboBarDlg->IDC_ComboSubObjects;
  ResetComboItems(m_SubObjectsCtrl);
  /*Q
  pos = m_SubObjectsCtrl->GetCount();
  while (pos > 1) {
    data = (CFieldsItemData*)m_SubObjectsCtrl->GetItemData(1);
    delete data;
    pos = m_SubObjectsCtrl->DeleteString(1);
  }
  */
  if (!decl)
    return;
  mDECL = decl;
  if ((mDECL->DeclType == Attr) || (mDECL->DeclType == IAttr) || (mDECL->DeclType == OAttr)) {
    myDoc->MemberTypeContext(mDECL, con,0);
    decl = myDoc->GetFinalMTypeAndContext(mDECL->RefID, mDECL->inINCL, con, 0);
    if (!decl)
      return;
  }
  else
    decl = myDoc->GetTypeAndContext(mDECL, con);
  if (mDECL->TypeFlags.Contains(substitutable)) 
    con.ContextFlags = SET(multiContext,-1);
  if (decl) {
    CExecFields *execF = new CExecFields(this, decl, con, true, &accuName, &accuIDs);
    delete execF;
  }  
  lastCombo = invalidateLast;
  if (m_SubObjectsCtrl->count() == 1) 
    ShowCombos(disableCombo);
  else {
    SetCombos(true, true);
    m_SubObjectsCtrl->show();
    m_SubObjectsCtrl->setCurrentItem(0);
    if (!LBaseData->debugOn)
      m_SubObjectsCtrl->setEnabled(true);
    LeftCombo = m_ComboBarDlg->IDC_ComboSubObjects;
  }
}

void CComboBar::ShowBaseInis(const TID& id) //id is interface, service interface or implementation
{
  SetCombos(true, true);
  LavaDECL *decl, *elDECL;
  DString vStr;
  CHE* che;
  CListItem* item;
  ResetComboItems(m_BaseInisCtrl);
  /*Q
  int pos = m_BaseInisCtrl->GetCount();
  while (pos > 1) 
    pos = m_BaseInisCtrl->DeleteString(1);
  */
  decl = myDoc->IDTable.GetDECL(id);
  if (!decl) return;
  if (decl->DeclType == VirtualType)
    return;
  for (che = (CHE*)decl->NestedDecls.first; che; che = (CHE*)che->successor) {
    elDECL = (LavaDECL*)che->data;
    if ((elDECL->DeclType == Function) 
        && !elDECL->SecondTFlags.Contains(funcImpl)
        && elDECL->TypeFlags.Contains(isInitializer)) {
      item = new CListItem(elDECL->FullName, TID(elDECL->OwnID, elDECL->inINCL));
      m_BaseInisCtrl->listBox()->insertItem(item);//sort#
    }
  }
  SortCombo(m_BaseInisCtrl);
  m_BaseInisCtrl->show();
  m_BaseInisCtrl->setCurrentItem(0);
  if (!LBaseData->debugOn)
    m_BaseInisCtrl->setEnabled(true);
  LeftCombo = m_ComboBarDlg->IDC_ComboBaseInis;
  lastCombo = invalidateLast;
}


void CComboBar::ShowClassFuncs(CheckData &ckd, LavaDECL* decl, LavaDECL* signalDecl, const CContext &callCtx, bool withStatic, bool showSignals) 
//decl is the final static real type (interface or implementation)
{
  CHETVElem* El;
  LavaDECL *elDECL, *conDECL, *oC=0/*, *implItfDecl*/;
  CHE* che;
  CStatFuncItem *idata;
  CListItem *item;
  //int pos;
  bool sameName=false, allowProtected=true;

  if (!decl)
    return;
  FuncParentDecl = decl;
//  allowProtected = myDoc->IDTable.IsAn(SelfTypeDECL, TID(decl->OwnID,decl->inINCL), 0);
/*  if (ckd.selfTypeDECL->DeclType == Initiator)
    allowProtected = false;
  else {
    implItfDecl = ckd.document->IDTable.GetDECL(((CHETID*)ckd.selfTypeDECL->Supports.first)->data,ckd.inINCL);
    if (!ckd.document->IDTable.IsAn(OWNID(decl),0,OWNID(implItfDecl),0))
      allowProtected = false;
    else
      allowProtected = true;
  }*/
  CContext signalCtx = ckd.tempCtx;

  SetCombos(true, true);
  ResetComboItems(m_ClassFuncsCtrl);
  ResetComboItems(m_VFuncsCtrl);
  m_VFuncsCtrl->removeItem(0);
  if (showSignals)
    m_VFuncsCtrl->insertItem( tr( " (Signals) " ) );
  else if (signalDecl)
    m_VFuncsCtrl->insertItem( tr( " (Slots) " ) );
  else
    m_VFuncsCtrl->insertItem( tr( " (Virtual calls) " ) );

  if (SelfTypeDECL->Supports.first && (((CHETID*)SelfTypeDECL->Supports.first)->data == TID(decl->OwnID,decl->inINCL))) {
    che = (CHE*)SelfTypeDECL->NestedDecls.first; //!!
    while (che) {
      elDECL = (LavaDECL*)che->data;
      if (elDECL->DeclType == Function) {
        if (!elDECL->SecondTFlags.Contains(funcImpl)
            && !elDECL->TypeFlags.Contains(isStatic)
            && !elDECL->TypeFlags.Contains(isInitializer)
            && (elDECL->op == OP_noOp)
            && (showSignals == elDECL->SecondTFlags.Contains(isLavaSignal))
            && (!signalDecl || slotFunction(ckd, elDECL, callCtx, signalDecl,signalCtx))
            && !IsInBox(m_VFuncsCtrl,elDECL->LocalName, elDECL->OwnID, elDECL->inINCL, sameName)) {
          if (sameName) {
            item = new CListItem(elDECL->FullName, TID(elDECL->OwnID, elDECL->inINCL));
            m_VFuncsCtrl->listBox()->insertItem(item);//sort#
          }
          else {
            item = new CListItem(elDECL->LocalName, TID(elDECL->OwnID, elDECL->inINCL));
            m_VFuncsCtrl->listBox()->insertItem(item);//sort#
          }
        }
      }
      che = (CHE*)che->successor;
    }
    SortCombo(m_VFuncsCtrl);
  }
  if (withStatic) {
    for (che = (CHE*)decl->NestedDecls.first; che; che = (CHE*)che->successor) {
      elDECL = (LavaDECL*)che->data;
      if (elDECL->DeclType == Function) {
        if (!elDECL->SecondTFlags.Contains(funcImpl)
            && !elDECL->TypeFlags.Contains(isStatic)
            && !elDECL->TypeFlags.Contains(isInitializer)
            && (elDECL->op == OP_noOp)
            && !signalDecl
            && (!elDECL->TypeFlags.Contains(isProtected) || allowProtected)) {
          //pos = m_ClassFuncsCtrl->AddString(elDECL->FullName.c);
          idata = new CStatFuncItem(elDECL->FullName, elDECL->OwnID, elDECL->inINCL);
          m_ClassFuncsCtrl->listBox()->insertItem(idata); //sort#
        }
      }
    }
    SortCombo(m_ClassFuncsCtrl);
  }
  if (decl->DeclType == Impl)
    conDECL = myDoc->IDTable.GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
  else
    conDECL = decl;
  FuncsInSupports(ckd, decl, conDECL, 0, signalDecl, signalCtx, callCtx, withStatic,showSignals);
  if (decl->DeclType == Interface) {
    myDoc->MakeVElems(decl, 0);
    for (El = (CHETVElem*)decl->VElems.VElems.first; El; El = (CHETVElem*)El->successor) {
      elDECL = myDoc->IDTable.GetDECL(El->data.VTEl);
      if (elDECL && (elDECL->DeclType == Function)
          && !elDECL->TypeFlags.Contains(isInitializer)
          && (elDECL->op == OP_noOp)
          && (!elDECL->TypeFlags.Contains(isProtected) || allowProtected)
          && (showSignals == elDECL->SecondTFlags.Contains(isLavaSignal))
          && (!signalDecl || slotFunction(ckd, elDECL, callCtx, signalDecl,signalCtx))
          && !IsInBox(m_VFuncsCtrl, elDECL->LocalName, elDECL->OwnID, elDECL->inINCL, sameName)) {
        if (sameName) {
          item = new CListItem(elDECL->FullName, TID(elDECL->OwnID, elDECL->inINCL));
          m_VFuncsCtrl->listBox()->insertItem(item);//sort#
        }
        else {
          item = new CListItem(elDECL->LocalName, TID(elDECL->OwnID, elDECL->inINCL));
          m_VFuncsCtrl->listBox()->insertItem(item);//sort#
        }
      }
    }
  }
  SortCombo(m_VFuncsCtrl);
  m_VFuncsCtrl->show();
  m_VFuncsCtrl->setCurrentItem(0);
  if (!LBaseData->debugOn)
    m_VFuncsCtrl->setEnabled(true);
  m_NewFunc = m_ComboBarDlg->IDC_NewFunc;
  m_NewPFunc = m_ComboBarDlg->IDC_NewPFunc;
  m_NewFunc->show();
  //m_NewFunc->setEnabled(true);
  m_NewPFunc->show();
  //m_NewPFunc->setEnabled(true);
  NewFuncShow = true;
  NewFuncEnable = true;
  if (FuncParentDecl->DeclType == Impl)
    NewPFuncEnable = true;
  else
    if (SelfTypeDECL->Supports.first && (((CHETID*)SelfTypeDECL->Supports.first)->data == TID(FuncParentDecl->OwnID,FuncParentDecl->inINCL))) 
      NewPFuncEnable = true;
    else
      NewPFuncEnable = false;
  LeftCombo = m_ComboBarDlg->IDC_ComboFuncs;
  m_ClassFuncsCtrl->show();
  m_ClassFuncsCtrl->setCurrentItem(0);
  if (signalDecl || !withStatic)
    m_ClassFuncsCtrl->setEnabled(false);
  else {
    if (!LBaseData->debugOn)
      m_ClassFuncsCtrl->setEnabled(true);
    RightCombo = m_ComboBarDlg->IDC_ComboClassFuncs;
  }
  lastCombo = invalidateLast;
}

void CComboBar::ShowStaticFuncs(CheckData &ckd)
{
  SetCombos(true, true);
  onShowStatic = true;
  m_TypesCtrl->show();
//  if (!VisibleList[v_Types])
  m_TypesCtrl->setCurrentItem(0);
  if (!LBaseData->debugOn)
    m_TypesCtrl->setEnabled(true);
  LeftCombo = m_ComboBarDlg->IDC_ComboTypes;
  m_BasicTypesCtrl->show();
//  if (!VisibleList[v_Types])
  m_BasicTypesCtrl->setCurrentItem(0);
  if (!LBaseData->debugOn)
    m_BasicTypesCtrl->setEnabled(true);
  RightCombo = m_ComboBarDlg->IDC_BasicTypes;
  m_StaticFuncsCtrl->show();
//  if (!VisibleList[v_StaticFuncs])
    m_StaticFuncsCtrl->setCurrentItem(0);
  m_StaticFuncsCtrl->setEnabled(false);
  lastCombo = invalidateLast;
}

void CComboBar::showIFFuncs()
{
  LavaDECL *decl, *elDECL;
  CHE* che;
//  int pos;
  CListItem *item;

  SetCombos(true, true);
  onShowStatic = true;
  ResetComboItems(m_StaticFuncsCtrl);
  /*Q
  pos = m_StaticFuncsCtrl->GetCount();
  while (pos > 1) 
    pos = m_StaticFuncsCtrl->DeleteString(1);
  */
  decl = myDoc->IDTable.GetDECL(lastSelTID);
  while (decl) {
    for (che = (CHE*)decl->NestedDecls.first; che; che = (CHE*)che->successor) {
      elDECL = (LavaDECL*)che->data;
      if ((elDECL->DeclType == Function) 
        && !elDECL->SecondTFlags.Contains(funcImpl)
        && elDECL->TypeFlags.Contains(isStatic)) {
          item = new CListItem(elDECL->LocalName, TID(elDECL->OwnID, elDECL->inINCL));
          m_StaticFuncsCtrl->listBox()->insertItem(item);//sort#
      }
    }
    if ((decl == IntfDECL) && (IntfDECL != SelfTypeDECL))
      decl = SelfTypeDECL;
    else
      decl = 0;
  }
  SortCombo(m_StaticFuncsCtrl);
  m_StaticFuncsCtrl->show();
  m_TypesCtrl->show();
  m_BasicTypesCtrl->show();
  m_StaticFuncsCtrl->setCurrentItem(0);
  if (!LBaseData->debugOn)
    m_StaticFuncsCtrl->setEnabled(true);
  RightCombo = m_ComboBarDlg->IDC_BasicTypes;
  ThirdCombo = m_ComboBarDlg->IDC_StaticFuncs;
  lastCombo = invalidateLast;
}


bool CComboBar::IsInBox(QComboBox* combo, const DString& name, int id, int incl, bool& sameName)
{
  bool inBox = false;
  int cnt, pos; //, posi;
  //DWORD wID;
  LavaDECL* idecl;
  DString istr;
  CListItem *item, *nitem;
  sameName = false;
  cnt = combo->count();
  for (pos = 1; (pos < cnt) && !inBox; pos++) {
    item = (CListItem*)combo->listBox()->item(pos);
    if ((item->itemData().nID == id) && (item->itemData().nINCL == incl)) 
      inBox = true;
    else {
      //istr.Reset(combo->GetLBTextLen(pos));
      //istr.l = combo->GetLBText(pos, istr.c);
      if (QString(name.c) == item->text()) {
        idecl = myDoc->IDTable.GetDECL(item->itemData(), 0);
        if (idecl && (name == idecl->LocalName)) {
          nitem = new CListItem(idecl->FullName, item->itemData());
          combo->listBox()->changeItem(nitem, pos);
          //combo->DeleteString(pos);
          //posi = combo->AddString(idecl->FullName.c);
          sameName = true;
          //if (pos < posi)
          //  pos--;
        }
      }
    }
  }
  return inBox;
}

void CComboBar::FuncsInSupports(CheckData &ckd, LavaDECL *decl, LavaDECL* conDECL, LavaDECL* vbaseDECL, LavaDECL* signalDecl, const CContext &signalCtx, const CContext &callCtx, bool withStatic, bool showSignals)
{
  CHE* che;
  CHETID *cheS;
  LavaDECL *inDECL, *cheDECL, *vDECL = vbaseDECL, *ElDECL/*, *implItfDecl*/;
  CHETVElem* El;
//  int pos;
  CContext context;
  DString vStr;
  CStatFuncItem *idata;
  CListItem *item;
  bool sameName, allowProtected=true;

//  allowProtected = myDoc->IDTable.IsAn(SelfTypeDECL, TID(decl->OwnID,decl->inINCL), 0);
/*  if (ckd.selfTypeDECL->DeclType == Initiator)
    allowProtected = false;
  else {
    implItfDecl = ckd.document->IDTable.GetDECL(((CHETID*)ckd.selfTypeDECL->Supports.first)->data,ckd.inINCL);
    if (!ckd.document->IDTable.IsAn(OWNID(decl),0,OWNID(implItfDecl),0))
      allowProtected = false;
    else
      allowProtected = true;
  }*/
  cheS = (CHETID*)decl->Supports.first;
  while (cheS ) {
    if (SelfTypeDECL->Supports.first && (((CHETID*)SelfTypeDECL->Supports.first)->data == cheS->data)) {
      che = (CHE*)SelfTypeDECL->NestedDecls.first;
      while (che) { 
        cheDECL = (LavaDECL*)che->data;
        if ( (cheDECL->DeclType == Function)
            && !cheDECL->TypeFlags.Contains(isInitializer)
            && !cheDECL->TypeFlags.Contains(isStatic)
            && (cheDECL->op == OP_noOp)
            && !cheDECL->SecondTFlags.Contains(funcImpl)
            && (showSignals == cheDECL->SecondTFlags.Contains(isLavaSignal))
            && (!signalDecl || slotFunction(ckd, cheDECL, callCtx, signalDecl,signalCtx))
            && !IsInBox(m_VFuncsCtrl,cheDECL->LocalName, cheDECL->OwnID, cheDECL->inINCL, sameName)) {
          if (sameName) {
            item = new CListItem(cheDECL->FullName, TID(cheDECL->OwnID, cheDECL->inINCL));
            m_VFuncsCtrl->listBox()->insertItem(item);//sort#
          }
          else {
            item = new CListItem(cheDECL->LocalName, TID(cheDECL->OwnID, cheDECL->inINCL));
            m_VFuncsCtrl->listBox()->insertItem(item);//sort#
          }
        }//function
        che = (CHE*)che->successor;
      }//while che
    }
    inDECL = myDoc->IDTable.GetDECL(cheS->data, decl->inINCL);
    if (inDECL && (inDECL->DeclType == VirtualType)) {
      vDECL = inDECL;
      inDECL = myDoc->IDTable.GetFinalBasicType(cheS->data, decl->inINCL, conDECL);
    }
    if (inDECL)  {
      che = (CHE*)inDECL->NestedDecls.first;
      while (che) {
        cheDECL = (LavaDECL*)che->data;
        if ( (cheDECL->DeclType == Function) 
             && !cheDECL->TypeFlags.Contains(isInitializer)
             && !cheDECL->TypeFlags.Contains(isStatic)
             && (cheDECL->op == OP_noOp)) {
          if (!cheDECL->TypeFlags.Contains(isAbstract)  && !cheDECL->SecondTFlags.Contains(funcImpl)) {
            if (withStatic && !signalDecl
                && !cheDECL->TypeFlags.Contains(forceOverride)
                && !IsInBox(m_ClassFuncsCtrl,cheDECL->FullName, cheDECL->OwnID, cheDECL->inINCL, sameName)) {
              idata = new CStatFuncItem(cheDECL->FullName, cheDECL->OwnID, cheDECL->inINCL);
              m_ClassFuncsCtrl->listBox()->insertItem(idata);//sort#
              if (vDECL) {
                vStr = lthen + vDECL->FullName + grthen + ddppkt + cheDECL->LocalName;
                idata = new CStatFuncItem(vStr, cheDECL->OwnID, cheDECL->inINCL, vDECL->OwnID, vDECL->inINCL);
                m_ClassFuncsCtrl->listBox()->insertItem(idata);//sort#
              }
            }
            if ((decl->DeclType == Impl)
              && (showSignals == cheDECL->SecondTFlags.Contains(isLavaSignal))
              && (!signalDecl || slotFunction(ckd, cheDECL, callCtx, signalDecl,signalCtx))
              && (!cheDECL->TypeFlags.Contains(isProtected) || allowProtected)
              && !IsInBox(m_VFuncsCtrl,cheDECL->LocalName/*FullName*/, cheDECL->OwnID, cheDECL->inINCL, sameName)) {
              if (sameName) {
                item = new CListItem(cheDECL->FullName, TID(cheDECL->OwnID, cheDECL->inINCL));
                m_VFuncsCtrl->listBox()->insertItem(item);//sort#
              }
              else {
                item = new CListItem(cheDECL->LocalName, TID(cheDECL->OwnID, cheDECL->inINCL));
                m_VFuncsCtrl->listBox()->insertItem(item);//sort#
              }
            }
          }
          /*
          if (!((LavaDECL*)che->data)->TypeFlags.Contains(Overrides)) {
            //allready in box?
            if (!IsInBox(m_VFuncsCtrl, ((LavaDECL*)che->data)->LocalName, ((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL)) {
              pos = m_VFuncsCtrl->AddString(((LavaDECL*)che->data)->LocalName.c);
              m_VFuncsCtrl->SetItemData(pos, MAKELONG(((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL));
            }
          }
          */
        }//function
        che = (CHE*)che->successor;
      }//while che
      if (decl->DeclType == Impl) {
        if (!inDECL->VElems.VElems.first)
          myDoc->MakeVElems(inDECL, 0);
        El = (CHETVElem*)inDECL->VElems.VElems.first;
        while (El) {
          ElDECL = myDoc->IDTable.GetDECL(El->data.VTEl);
          if (ElDECL && (ElDECL->DeclType == Function)
              && (ElDECL->op == OP_noOp)
              && (showSignals == ElDECL->SecondTFlags.Contains(isLavaSignal))
              && (!signalDecl || slotFunction(ckd, ElDECL, callCtx, signalDecl,signalCtx))
              && (!ElDECL->TypeFlags.Contains(isProtected) || allowProtected)
              && !ElDECL->TypeFlags.Contains(isInitializer) 
              && !IsInBox(m_VFuncsCtrl, ElDECL->LocalName, ElDECL->OwnID, ElDECL->inINCL, sameName)) {
            if (sameName) {
              item = new CListItem(ElDECL->FullName, TID(ElDECL->OwnID, ElDECL->inINCL));
              m_VFuncsCtrl->listBox()->insertItem(item);//sort#
            }
            else {
              item = new CListItem(ElDECL->LocalName, TID(ElDECL->OwnID, ElDECL->inINCL));
              m_VFuncsCtrl->listBox()->insertItem(item);//sort#
            }
            /*
            if (sameName)
              pos = m_VFuncsCtrl->AddString(ElDECL->FullName.c);
            else
              pos = m_VFuncsCtrl->AddString(ElDECL->LocalName.c);
            m_VFuncsCtrl->SetItemData(pos, MAKELONG(ElDECL->OwnID, ElDECL->inINCL));
            */
          }
          El = (CHETVElem*)El->successor;
        }
      }
      FuncsInSupports(ckd, inDECL, conDECL, vDECL, signalDecl, signalCtx, callCtx, withStatic);
    }//inDECL
    cheS = (CHETID*)cheS->successor;
  }//while cheS
}


CExecTypes::CExecTypes(CComboBar* bar)
{
  Bar = bar;
  TabTravers = new CTabTraversal(this, Bar->myDoc->mySynDef);
  TabTravers->Run(true, true);
  SortCombo(Bar->m_NewCtrl);
  SortCombo(Bar->m_SNewCtrl);
  SortCombo(Bar->m_CallIntCtrl);
  SortCombo(Bar->m_TypesCtrl);
  SortCombo(Bar->m_SetTypesCtrl);
  //SortCombo(Bar->m_SignalsCtrl);
  SortCombo(Bar->m_EnumsCtrl);
  SortCombo(Bar->m_CompoObjIntCtrl);
  SortCombo(Bar->m_AttachCtrl);
}

void CExecTypes::ExecDefs (LavaDECL ** pelDef, int incl)
{
//  int pos, pos2, pos3, *pMax, *pMax2, *pMax3=0;
  TID refID;
  TDeclType elType;
  LavaDECL *oc=0, *elDef = *pelDef, *refEl = *pelDef;
  QComboBox *combo = 0, *combo2 = 0, *combo3 = 0;
  unsigned loc;
  DString Label;
  bool setClassName;
  CListItem *item;

  if (elDef->fromBType != NonBasic)
    return;
  elType = elDef->DeclType;
  CContext context = ((SelfVar*)Bar->ExecDECL->Exec.ptr)->selfCtx;
  while (elType == VirtualType){
    refID = TID(refEl->OwnID, refEl->inINCL);
    if (context.oContext && (refID == Bar->myDoc->GetMappedVType(refID, context, 0)))  {
      refEl = Bar->myDoc->IDTable.GetDECL(refEl->RefID, refEl->inINCL);
      if (refEl)
        elType = refEl->DeclType;
      else
        elType = UnDef;
    }
    else
      elType = UnDef;
  }
  setClassName = false;
  switch (elType) {
  case Function:
    if (elDef->TypeFlags.Contains(isInitializer)) 
      if ( (elDef->ParentDECL->DeclType == Interface)
          && !elDef->ParentDECL->TypeFlags.Contains(isAbstract)) {
        combo = Bar->m_NewCtrl;
        //pMax = &maxIW;
        setClassName = true;
        //combo2 = Bar->m_CallIntCtrl;
        //pMax2 = &maxCallW;
      }
    break;
  case Interface: 
    if (!Bar->myDoc->TypeForMem(Bar->SelfTypeDECL, elDef, 0)) {
      combo = Bar->m_TypesCtrl;
      //pMax = &maxTW;
      if (/*elDef*/refEl->SecondTFlags.Contains(isSet)) {
        combo2 = Bar->m_SetTypesCtrl;
        //pMax2 = &maxSTW;
      }
      //if (/*elDef*/refEl->SecondTFlags.Contains(isCallbackServer)) {
      //  combo2 = Bar->m_SignalsCtrl;
      //}
      if (/*elDef*/refEl->DeclDescType == EnumType) {
        combo2 = Bar->m_EnumsCtrl;
        //pMax2 = &maxEnuW;
      }
      if (/*elDef*/refEl->TypeFlags.Contains(isComponent)) {
        combo2 = Bar->m_CompoObjIntCtrl;
        //pMax2 = &maxCOIW;
      }
    }
    break;
  case CompObjSpec:
    if (elDef->usableIn(Bar->SelfTypeDECL)) {
      combo = Bar->m_SNewCtrl;
      //pMax = &maxSIW;
      if (elDef->TypeFlags.Contains(isPersistent)) {
        combo2 = Bar->m_AttachCtrl;
        //pMax2 = &maxAtW;
      }
    }
    break;

  case Initiator:
    if (elDef->usableIn(Bar->SelfTypeDECL)) {
      combo = Bar->m_CallIntCtrl;
      //pMax = &maxCallW;
    }
    break;

  default: ;
  }
  if (combo) {
    if (Bar->myDECL && Bar->myDECL->FullName.l) {
      Label = LBaseData->calcRelName (elDef->FullName, Bar->myDECL->FullName);
      if (setClassName && !Label.Contains(ddppkt,1,loc)) {
        Label.Insert( ddppkt, 0); 
        Label.Insert( elDef->ParentDECL->LocalName, 0);
      }
    }
    else 
      Label = elDef->FullName;
    if (elDef->DeclType == VirtualType) {
      Label.Insert(lthen, 0);
      Label += grthen;
    }
    item = new CListItem(Label, TID(elDef->OwnID, incl));
    combo->listBox()->insertItem(item);//sort#
    if (combo2) {
      item = new CListItem(item->text(), item->itemData());
      combo2->listBox()->insertItem(item);//sort#
    }
    if (combo3) {
      item = new CListItem(item->text(), item->itemData());
      combo3->listBox()->insertItem(item);//sort#
    }
  }
}

CExecFields::CExecFields(CComboBar* bar, LavaDECL* startDECL, const CContext &context,
                         bool subO, DString* startName, TDODC* startIDs)

{
  DString accuName;
  TDODC accuIDs;
  TIDs typeIDs;

  if ((startDECL->DeclType != Function) && (startDECL->DeclType != Initiator)
      && (!startName || !startName->l))
    return;
  Bar = bar;
  CContext con = context;
  Bar->myDoc->NextContext(startDECL, con);
  if (subO) 
    FieldList = Bar->m_SubObjectsCtrl;
  else 
    FieldList = Bar->m_ObjectsCtrl;
  maxW = 0;
  maxSetW = 0;
  if (startName)
    accuName = *startName;
  else
    accuName.Reset(0);
  if (startIDs)
    accuIDs = *startIDs;
  OnType(startDECL, accuName, accuIDs, typeIDs, con);
  SetWidth(FieldList, maxW);
  SortCombo(Bar->m_SetObjectsCtrl);
  Bar->m_SetObjectsCtrl->setCurrentItem(0);
  if (subO) {
    SortCombo(Bar->m_SubObjectsCtrl);
    Bar->m_SubObjectsCtrl->setCurrentItem(0);
  }
  else {
    SortCombo(Bar->m_ObjectsCtrl);
    Bar->m_ObjectsCtrl->setCurrentItem(0);
  }
}


void CExecFields::SetWidth(QComboBox* list, int wmax)
{
  CFieldsItem *data, *ndata;
  int pos, cnt = list->count();

  for (pos = 1; pos < cnt; pos++) {
    data = (CFieldsItem*)list->listBox()->item(pos);
    if (data->withClass) {
      ndata = new CFieldsItem(data->QName);
      ndata->withClass = true;
      ndata->IDs = data->IDs;
      ndata->QName = data->QName;
      list->listBox()->changeItem(ndata, pos);
    }
  }
}

bool CExecFields::SameField(TDODC& IDs1, TDODC& IDs2, bool& replace)
{
  CHE *che1, *che2;
  che1 = (CHE*)IDs1.first;
  che2 = (CHE*)IDs2.first;
  replace = false;
  while (che1 && che2) {
    if ( ( ((TDOD*)che1->data)->ID.nID != ((TDOD*)che2->data)->ID.nID)
         || ( ((TDOD*)che1->data)->ID.nINCL != ((TDOD*)che2->data)->ID.nINCL)) 
      if ( Bar->myDoc->IDTable.IsAnc(((TDOD*)che1->data)->ID, 0, ((TDOD*)che2->data)->ID, 0)) {
        replace = true;
        return false;
      }
      else
        if (!Bar->myDoc->IDTable.IsAnc(((TDOD*)che2->data)->ID, 0, ((TDOD*)che1->data)->ID, 0))
          return false;
    che1 = (CHE*)che1->successor;
    che2 = (CHE*)che2->successor;
  }
  return true;

}

void CExecFields::AddToBox(LavaDECL** pdecl, DString& name, QComboBox* fieldList)
{
  int pos;
  DString fName;
  CFieldsItem *fdata, *data, *nfdata, *ndata;
  bool found = false, replace;

  data = new CFieldsItem(name);
  data->IDs = *pAccuIDs;
  if (pdecl)
    data->QName = (*pdecl)->FullName;
  
  fdata = (CFieldsItem*)fieldList->listBox()->findItem(QString(name.c), Qt::ExactMatch | Qt::CaseSensitive);
  if (fdata) {
    pos = fieldList->listBox()->index(fdata);
    found = SameField(fdata->IDs, data->IDs, replace);
    if (!found) {
      if (replace) {
        fieldList->removeItem(pos);
      }
      else {
        if (!fdata->withClass) {
          name.Delete(name.l - (*pdecl)->LocalName.l, (*pdecl)->LocalName.l);
          fName = name + fdata->QName;
          nfdata = new CFieldsItem(fName);
          nfdata->IDs = fdata->IDs;
          nfdata->QName = fdata->QName;
          nfdata->withClass = true;
          fieldList->listBox()->changeItem(nfdata, pos);
          name += (*pdecl)->FullName;
          ndata = new CFieldsItem(name);
          ndata->IDs = data->IDs;
          ndata->QName = data->QName;
          ndata->withClass = true;
          delete data;
          data = ndata;
        }
      }
    }
  }
  if (found) 
    delete data;
  else 
    fieldList->listBox()->insertItem(data);//sort#
}

void CExecFields::OnField(LavaDECL **pdecl, DString accuName, TDODC accuIDs, 
                          TIDs typeIDs, const CContext &context)
{
  DString nAccuName;
  LavaDECL *nnEl;
  CHE *cheo;
  TDOD *tdod = new TDODV(true);

  if (accuName.l)  {
    nAccuName = accuName;
    if (accuName != pkt)
      nAccuName += pkt;
    nAccuName += (*pdecl)->LocalName;
    tdod->name = (*pdecl)->LocalName;
  }
  else {
    nAccuName = (*pdecl)->LocalName;
    tdod->name = (*pdecl)->LocalName;
    accuIDs.Destroy();
  }
  tdod->context = context;
  tdod->ID.nID = (*pdecl)->OwnID;
  tdod->ID.nINCL = (*pdecl)->inINCL;
  cheo = NewCHE(tdod);
  accuIDs.Append(cheo);
  pAccuIDs = &accuIDs;
  CContext con = context;
  Bar->myDoc->MemberTypeContext(*pdecl, con,0);
  nnEl = Bar->myDoc->GetFinalMTypeAndContext((*pdecl)->RefID, (*pdecl)->inINCL, con, 0);
  if ((*pdecl)->TypeFlags.Contains(substitutable)) 
    con.ContextFlags = SET(multiContext,-1);
  if (nAccuName.l && (nAccuName != pkt))
    AddToBox(pdecl, nAccuName, FieldList);
  if (Bar->m_SetObjectsCtrl && nnEl && nnEl->SecondTFlags.Contains(isSet))
    AddToBox(pdecl, nAccuName, Bar->m_SetObjectsCtrl);
  if (nnEl) 
    OnType(nnEl, nAccuName, accuIDs, typeIDs, con);
}

void CExecFields::OnType(LavaDECL *decl, DString accuName, TDODC accuIDs, 
                         TIDs typeIDs, const CContext &context)
{
  CHE *inCheEl;
  CHETID *che;
//  LavaDECL *implItfDecl;
  bool inOwn=true;

  if (!decl)
    return;
  pAccuIDs = &accuIDs;
  for (che = (CHETID*)typeIDs.first;
       che && ( (che->data.nID != decl->OwnID) || (che->data.nINCL != decl->inINCL));
       che = (CHETID*)che->successor);

  if (!che) {
    che = new CHETID;
    che->data.nID = decl->OwnID;
    che->data.nINCL = decl->inINCL;
    typeIDs.Append(che);
    OnSupports(decl, accuName, accuIDs, typeIDs, context);

    if ((Bar->SelfTypeDECL->DeclType != Interface)
        && Bar->SelfTypeDECL->Supports.first 
        && (((CHETID*)Bar->SelfTypeDECL->Supports.first)->data == TID(decl->OwnID, decl->inINCL))) {
      for (inCheEl = (CHE*)Bar->SelfTypeDECL->NestedDecls.first; inCheEl;
           inCheEl = (CHE*)inCheEl->successor) 
        if ( (( ((LavaDECL*)inCheEl->data)->DeclType == Attr)  || 
              ( ((LavaDECL*)inCheEl->data)->DeclType == IAttr)  || 
              ( ((LavaDECL*)inCheEl->data)->DeclType == OAttr)  ))            
          OnField((LavaDECL**)&inCheEl->data, accuName, accuIDs, typeIDs, context);
    }
//    bool inOwn = Bar->myDoc->IDTable.IsAn(Bar->SelfTypeDECL, TID(decl->OwnID,decl->inINCL), 0);
/*    if (Bar->SelfTypeDECL->DeclType == Initiator)
      inOwn = false;
    else {
      implItfDecl = Bar->myDoc->IDTable.GetDECL(((CHETID*)Bar->SelfTypeDECL->Supports.first)->data,ckd.inINCL);
      if (!Bar->myDoc->IDTable.IsAn(OWNID(decl),0,OWNID(implItfDecl),0))
        inOwn = false;
      else
        inOwn = true;
    }*/
    for (inCheEl = (CHE*)decl->NestedDecls.first; inCheEl;
         inCheEl = (CHE*)inCheEl->successor ) {
      if ( (( ((LavaDECL*)inCheEl->data)->DeclType == Attr)  || 
            ( ((LavaDECL*)inCheEl->data)->DeclType == IAttr)  || 
            ( ((LavaDECL*)inCheEl->data)->DeclType == OAttr)  )
            ) {
        if (inOwn || !((LavaDECL*)inCheEl->data)->TypeFlags.Contains(isProtected))
          OnField((LavaDECL**)&inCheEl->data, accuName, accuIDs, typeIDs, context);
      }
    }
  }
}


void CExecFields::OnSupports(LavaDECL *decl, DString accuName, TDODC accuIDs, TIDs typeIDs,  
                             const CContext &context)
{
  if (decl->DeclType == Function)// && !decl->SecondTFlags.Contains(funcImpl))
    return;
  CHETID *cheSup;
  LavaDECL* inDECL, *returnDECL = 0;
  CContext con = context; //!!!
  for (cheSup = (CHETID*)decl->Supports.first; cheSup; cheSup = (CHETID*)cheSup->successor) {
    inDECL = Bar->myDoc->GetFinalMTypeAndContext(cheSup->data, decl->inINCL, con, 0);
    if (inDECL)  
      OnType(inDECL, accuName, accuIDs, typeIDs, con);
  }
}

