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
#include "LavaPEFrames.h"
#include "LavaPEWizard.h"
#include "Boxes.h"
#include "LavaBaseStringInit.h"
#include "qcheckbox.h"
#include "qlineedit.h"
#include "qlabel.h"
#include "qlayout.h"
#include "qpushbutton.h"
#include "qradiobutton.h"
#include "qspinbox.h"
#include "q3buttongroup.h"
#include "qmessagebox.h"
#include "qtabbar.h"
#include "qsizepolicy.h"
#include "qfontdialog.h"
#include "qcolordialog.h"
#include "q3filedialog.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QCustomEvent>

static void SetSpace(TAnnotation* anno, unsigned& m_tab, unsigned& m_space, unsigned& m_frmSpace)
{
  if (anno) {
    if (anno->TabPosition > 0)
      m_tab = anno->TabPosition;
    else
      m_tab = 0;
    if (anno->FrameSpacing > 0)
      m_frmSpace = anno->FrameSpacing;
    else
      if (anno->FrameSpacing == -1)
        m_frmSpace = 0;
    m_space = anno->Space;
  }

}

static void GetSpace(TAnnotation** p_anno, unsigned m_tab, unsigned m_space, unsigned m_frmSpace)
{
  if (!(*p_anno))
    *p_anno = new TAnnotation;
  if (m_tab > 0)
    (*p_anno)->TabPosition = m_tab;
  else
    (*p_anno)->TabPosition = -1;
  if (m_frmSpace)
    (*p_anno)->FrameSpacing = m_frmSpace;
  else
    (*p_anno)->FrameSpacing = -1;
  (*p_anno)->Space = m_space;

}

CWizardView::CWizardView(QWidget* parent, wxDocument* doc) 
  : CLavaBaseView(parent,doc, "WizardView")
{ 
  //setFont(LBaseData->m_TreeFont);
  Q3VBox* qvbox = new Q3VBox(this);
  Q3HBox* qhbox = new Q3HBox(qvbox);
  resetButton = new QPushButton("Reset", qhbox);
  applyButton = new QPushButton("Apply", qhbox);
  helpButton = new QPushButton("Help", qhbox);
  myScrv = new GUIScrollView(qvbox, false);
  guibox = myScrv->qvbox; 
  connect (applyButton, SIGNAL(clicked()), this, SLOT(Apply()));
  connect (resetButton, SIGNAL(clicked()), this, SLOT(Reset()));
  connect (helpButton, SIGNAL(clicked()), this, SLOT(Help()));
  wizard = 0;
  postedPage = 0;
  delayedID = 0;
  myTree = (CLavaPEView*)((CFormFrame*)GetParentFrame())->viewR;
}

void CWizardView::setFont(const QFont& font)
{
  CLavaBaseView::setFont(font);
  int pageI = 0;
  if (wizard && !wizard->isActive) {
    int x = myScrv->contentsX();
    int y = myScrv->contentsY();
    wizard->UpdateData();
    LavaDECL** synEl = wizard->synEl;
    LavaDECL* FormDECL = wizard->FormDECL;
    bool mod = wizard->modified;
    wizard->FormDECL = 0;
    pageI = wizard->currentPageIndex();
    delete wizard;
    wizard = new CLavaPEWizard(synEl, FormDECL, this, guibox);
    wizard->setModified(mod);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
    myScrv->setContentsPos(x,y);
    if (pageI < wizard->count())
      wizard->setCurrentPage(pageI);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
  }
}


void CWizardView::customEvent(QCustomEvent *ev)
{
  if (ev->type() == IDU_LavaPE_CalledView) {
    CLavaPEHint* hint = (CLavaPEHint*)ev->data();
    ((CLavaPEDoc*)hint->fromDoc)->UndoMem.AddToMem(hint);
    ((CLavaPEDoc*)hint->fromDoc)->UpdateDoc(((CLavaPEDoc*)hint->fromDoc)->MainView, false, hint);
    ((CLavaPEDoc*)hint->fromDoc)->SetLastHint();
  }
  if(delayedID) {
    TIDType itype;
    LavaDECL** synEl = (LavaDECL**)((CLavaPEDoc*)GetDocument())->IDTable.GetVar(TID(delayedID,0), itype);
    CTreeItem* item = myTree->BrowseTree(*synEl, (CTreeItem*)myTree->GetListView()->firstChild());
    if (item)
      myTree->GetListView()->setCurAndSel(item);
    delayedID = 0;
    if (!wizard) {
      wizard = new CLavaPEWizard(synEl, 0, this, guibox);
      myScrv->MaxBottomRight = wizard->rect();
      Resize();
      myScrv->setContentsPos(0,0);
      if (postedPage < wizard->count())
        wizard->setCurrentPage(postedPage);
      postedPage = 0;
    }
  }
}

void CWizardView::OnUpdate(wxView* , unsigned undoRedo, QObject* pHint)
{
  if (!pHint && (undoRedo != 3) && (undoRedo < CHLV_noCheck) || wizard && wizard->isActive)
    return;
  int pageI = 0;
  bool delayed = false;
  if (pHint && ((CLavaPEHint*)pHint)->com == CPECommand_OpenWizardView) {
    if (wizard) {
      if (postedPage) {
        pageI = postedPage;
        postedPage = 0;
      }
      else
        pageI = wizard->currentPageIndex();
      if (wizard->modified) {
          CLavaPEHint* hint = wizard->ApplyHint();
          postedPage = pageI;
          LavaDECL* decl = (LavaDECL*)((CLavaPEHint*)pHint)->CommandData1;
          delayed = wxDocManager::GetDocumentManager()->GetActiveView() == myTree;
          if (delayed)
            delayedID = decl->OwnID;
          wizard->FormDECL = 0;
          QApplication::postEvent(this, new QCustomEvent(IDU_LavaPE_CalledView,(void*)hint));
        }
      //}
      delete wizard;
      wizard = 0;
      if (delayed)
        return;
    }
    wizard = new CLavaPEWizard( (LavaDECL**)((CLavaPEHint*)pHint)->CommandData4, 0,
                               this, guibox);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
    myScrv->setContentsPos(0,0);
    if (pageI < wizard->count())
      wizard->setCurrentPage(pageI);

  }
}

void CWizardView::PostApplyHint()
{
  postedPage = wizard->currentPageIndex();
  CLavaPEHint* hint = wizard->ApplyHint();
  wizard->FormDECL = 0;
  wizard->modified = false;
  QApplication::postEvent(this, new QCustomEvent(IDU_LavaPE_CalledView,(void*)hint));
}

void CWizardView::Resize()
{
  QSize size = myScrv->MaxBottomRight.size();
  ((GUIScrollView*)myScrv)->qvbox->resize(size);
  myScrv->resizeContents(size.width(),size.height());
}


void CWizardView::Apply()
{
  wizard->OnApply();
  Reset();
}


void CWizardView::Help()
{
	QString path(ExeDir);
	QStringList args;
	args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	
	if (!qacl) {
		qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		qacl->setArguments(args);
	}

	qacl->showPage(ExeDir + "/../doc/html/EditForm.htm");
}

void CWizardView::Reset()
{
  int pageI = 0;
  if (wizard && !wizard->isActive) {
    LavaDECL** synEl = wizard->synEl;
    pageI = wizard->currentPageIndex();
    delete wizard;
    wizard = new CLavaPEWizard(synEl, 0, this, guibox);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
    myScrv->setContentsPos(0,0);
    if (pageI < wizard->count())
      wizard->setCurrentPage(pageI);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CLavaPEWizard


CLavaPEWizard::CLavaPEWizard(LavaDECL ** p_origDECL, LavaDECL* formDECL, CWizardView* view, QWidget* parent)
  :QTabWidget(parent, "LavaPEWizard") 
{
  isActive = false;
  OrigDECL = *p_origDECL;
  synEl = p_origDECL;
  myView = view;
  myDoc = (CLavaPEDoc*)myView->GetDocument();
  if (!formDECL) {
    FormDECL = NewLavaDECL();
    *FormDECL = *OrigDECL;
  }
  else
    FormDECL = formDECL;
  VFormDECL = FormDECL;
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFont(LBaseData->m_TreeFont);

  modified = false;
  GenPage = 0;
  MenuPage = 0;
  IOPage = 0;
  LitPage = 0;
  ChFormPage = 0;
  SupportPage = 0;
  ChainIOPage = 0;
  ChainLitPage = 0;
  FormTextPage = 0;
  FontColorPage = 0;

  AddPages();
  resize(minimumSizeHint());
  show();
  connect (this, SIGNAL(currentChanged(QWidget*)), this, SLOT(OnSetActive(QWidget*)));
  modified = false;
  myView->applyButton->setEnabled(false);
  myView->resetButton->setEnabled(false);
}


CLavaPEHint* CLavaPEWizard::ApplyHint()
{
  UpdateData();
  if (FormDECL->Annotation.ptr && FormDECL->Annotation.ptr->FA.ptr)
      ((TAnnotation*)FormDECL->Annotation.ptr->FA.ptr)->BType =
                              ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myDoc->mySynDef, FormDECL);
  DString *name = new DString(OrigDECL->FullName);  //bisheriger Name

  return new CLavaPEHint(CPECommand_Change, myDoc, (const unsigned long)1, (DWORD)FormDECL, (DWORD)name, 0, (unsigned long)synEl);
}


void CLavaPEWizard::OnApply()
{
  if (modified) {
    isActive = true;
    if (!((CHESimpleSyntax*)myDoc->mySynDef->SynDefTree.first)->data.SyntaxName.l)
      DString str = "WWWW";
    CLavaPEHint *hint = ApplyHint();
    myDoc->UndoMem.AddToMem(hint);
    FormDECL = 0;
    modified = false;
    myDoc->UpdateDoc(myDoc->MainView, false, hint);
    myDoc->SetLastHint();
    isActive = false;
  }
}

void CLavaPEWizard::UpdateData()
{
  if (modified) {
    if (MenuPage)
      MenuPage->OnApply();
    if (FontColorPage)
      FontColorPage->OnApply();
    if (IOPage)
      IOPage->OnApply();
    if (LitPage)
      LitPage->OnApply();
    if (ChainIOPage)
      ChainIOPage->OnApply();
    if (ChainLitPage)
      ChainLitPage->OnApply();
    if (ChFormPage)
      ChFormPage->OnApply();
    if (FormTextPage)
      FormTextPage->OnApply();
  }
}

void CLavaPEWizard::AddPages()
{
  QSize sz;
  if (FormDECL->DeclType == FormText) {
    FormTextPage = new CFormTextPage(FormDECL, this);
    FormTextPage->setMinimumSize(FormTextPage->size());
    addTab(FormTextPage, "Formtext");
  }
  else {
    if (FormDECL->DeclType == FormDef) {
      if (FormDECL->Supports.first) {
        SupportPage = new CSupportPage(this);
        SupportPage->setMinimumSize(SupportPage->size());
        addTab(SupportPage, "General");
      }
    }
    else {
      if (FormDECL->DeclType != VirtualType) {
  //      if (FormDECL->DeclDescType == NamedType) {
        GenPage = new CGeneralPage(this);
        GenPage->setMinimumSize(GenPage->size());
        addTab(GenPage, "General");
      }
    }
    if ((FormDECL->DeclDescType != BasicType)
        || (FormDECL->BType == B_Bool)   || (FormDECL->BType == Char)
        || (FormDECL->BType ==  Integer) || (FormDECL->BType ==  Float)
        || (FormDECL->BType ==  Double)  || (FormDECL->BType ==  VLString)) {
      IOPage = new CIOPage(this, FormDECL, false);
      sz =IOPage->size(); 
      IOPage->setMinimumSize(sz);
      addTab(IOPage, "IO+Layout");
      if ((FormDECL->DeclType == Attr) || (FormDECL->DeclType == VirtualType)) {
        LitPage = new CLiteralsPage(this, FormDECL, false);
        sz = LitPage->size(); 
        LitPage->setMinimumSize(sz);
        addTab(LitPage, "Content");
      }
      FontColorPage = new CFontColorPage(this, FormDECL);
      sz =FontColorPage->size(); 
      FontColorPage->setMinimumSize(sz);
      addTab(FontColorPage, "Font+Color");
      if (FormDECL->DeclDescType == EnumType) {
        MenuPage = new CMenuPage(this);
        MenuPage->setMinimumSize(MenuPage->size());
        addTab(MenuPage, "Menu or button");
      }
      if (FormDECL->DeclType == VirtualType) {
        if ((FormDECL->SecondTFlags.Contains(isArray) || FormDECL->SecondTFlags.Contains(isSet)) 
              && FormDECL->Annotation.ptr->IterOrig.ptr
              && ((TIteration*)FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
          ChFormPage = new CChainFormPage(this);
  //        ChFormPage->setMinimumSize(ChFormPage->size());
  //        addTab(ChFormPage, "Chain/array element");
        }
      }
    }
  }
}

void CLavaPEWizard::AddChainPages(LavaDECL *chainEl)
{
  ChainIOPage = new CIOPage(this, chainEl, true);
  ChainIOPage->setMinimumSize(ChainIOPage->size());
  addTab(ChainIOPage, "Chain/array element - IO*Layout");
  ChainLitPage = new CLiteralsPage(this, chainEl, true);
  ChainLitPage->setMinimumSize(ChainLitPage->size());
  addTab(ChainLitPage, "Chain/array element - Content");
}

void CLavaPEWizard::setModified(bool mod)
{
  if(mod && !modified) {
    myView->applyButton->setEnabled(true);
    myView->resetButton->setEnabled(true);
    modified = true;
  }
}

CLavaPEWizard::~CLavaPEWizard()
{
  /*
  if (GenPage)
    delete GenPage;
  if (MenuPage)
    delete MenuPage;
  if (FontColorPage)
    delete FontColorPage;
  if (IOPage)
    delete IOPage;
  if (LitPage)
    delete LitPage;
  if (ChainIOPage)
    delete ChainIOPage;
  if (ChainLitPage)
    delete ChainLitPage;
  if (ChFormPage)
    delete ChFormPage;
    */
  if (FormDECL)
    delete FormDECL;
}


void CLavaPEWizard::OnSetActive(QWidget *page)
{
  if (LitPage == page)
    LitPage->OnSetActive();
  if (MenuPage == page)
    MenuPage->OnSetActive();

}


/////////////////////////////////////////////////////////////////////////////
// CChainFormPage property page

CChainFormPage::~CChainFormPage()
{
}


void CChainFormPage::UpdateData(bool getData)
{
  if (getData) {
    m_ChFormElemStr = m_ChFormElem->text();
    m_InsText = m_EditInsertButton->text();
    m_DelText = m_EDITDelButton->text();
    //m_Name = m_NewName2->text();
    m_Len = SpinBox_IDC_SPINAR->value();
  }
  else {
    modify = false;
    m_ChFormElem->setText(m_ChFormElemStr) ;
    m_EditInsertButton->setText(m_InsText);
    m_EDITDelButton->setText(m_DelText);
    //m_NewName2->setText(m_Name);
    SpinBox_IDC_SPINAR->setValue(m_Len);
    modify = true;
  }
}

CChainFormPage::CChainFormPage(CLavaPEWizard* wizard)
 : Ui_IDD_ChainFormPage()
{
  LavaDECL *formDecl, *classDecl;
  TAnnotation* anno;
  bool noElli, isAr;
  
  modify = true;
  m_ElemSel = "";
  m_ChFormElemStr = "";
  m_DelText = "";
  m_InsText = "";
  myWizard = wizard;
  m_Len = 1;
  m_NewName2->setText(myWizard->VFormDECL->LocalName.c);
  //m_Name = QString(myWizard->VFormDECL->LocalName.c);
  if (!myWizard->FormDECL->Annotation.ptr)
    myWizard->FormDECL->Annotation.ptr = new TAnnotation;
  anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  isAr = myWizard->VFormDECL->SecondTFlags.Contains(isArray);
  if (isAr) {
		//m_ArrayLen->setEnabled(true);
		SpinBox_IDC_SPINAR->setEnabled(true);
    SpinBox_IDC_SPINAR->setMinValue (1);
    SpinBox_IDC_SPINAR->setMaxValue (10000);
    m_Len = myWizard->FormDECL->Annotation.ptr->Length.DecPoint;
  }
  if (anno->IterFlags.Contains(FixedCount))
    anno->IterFlags.INCL(NoEllipsis);
  noElli = anno->IterFlags.Contains(NoEllipsis) || isAr;
  if (!noElli)
    anno->IterFlags.EXCL(FixedCount);
  m_ConstChain->setChecked(anno->IterFlags.Contains(FixedCount));
  m_HasButtons->setChecked(!noElli);
  m_EDITDelButton->setEnabled(!noElli);
  m_EditInsertButton->setEnabled(!noElli);
  if (anno && !noElli) {
    if (anno->String1.l)
      m_InsText = QString(anno->String1.c);
    if (anno->String2.l)
      m_DelText = QString(anno->String2.c);
  }
  m_HasButtons->setEnabled(!anno->IterFlags.Contains(FixedCount) && !isAr);
  m_ConstChain->setEnabled(noElli && !isAr);
  if (myWizard->FormDECL->Annotation.ptr->IterOrig.ptr
    && ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr)
    CHEEl = ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
  else
    return;
  classDecl = myWizard->myDoc->IDTable.GetDECL(((CHETID*)myWizard->FormDECL->Supports.first)->data, myWizard->FormDECL->inINCL);
  QString* err = myWizard->myDoc->CheckFormEl(myWizard->FormDECL, classDecl);
  if (err && (err != &ERR_InvisibleType)) {
    formDecl = classDecl;
    ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.Destroy();
    CHEEl = NewLavaDECL();
    ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr = CHEEl;
    CHEEl->DeclDescType = classDecl->DeclDescType;
    CHEEl->BType = classDecl->BType;
    CHEEl->SecondTFlags = classDecl->SecondTFlags;
    CHEEl->DeclType = PatternDef; //not VirtualType
    myWizard->FormDECL->DeclDescType = classDecl->DeclDescType;
    myWizard->FormDECL->BType = classDecl->BType;
    myWizard->FormDECL->DeclType = classDecl->DeclType; 
    CHEEl->RefID = TID(classDecl->RefID.nID, myWizard->myDoc->IDTable.IDTab[classDecl->inINCL]->nINCLTrans[classDecl->RefID.nINCL].nINCL);
    CHEEl->Annotation.ptr = new TAnnotation;
    CHEEl->Annotation.ptr->FA.ptr = new TAnnotation;
    CHEEl->Annotation.ptr->FrameSpacing = 1;
  }
  for (formDecl = CHEEl;
       formDecl && (formDecl->DeclDescType != BasicType)
            && ((formDecl->DeclType == VirtualType) || (formDecl->DeclType == PatternDef)); 
       formDecl = myWizard->myDoc->IDTable.GetDECL(formDecl->RefID, formDecl->inINCL));
  if (formDecl) {
    if (formDecl->DeclDescType == BasicType) {
      CHEEl->DeclDescType = BasicType;
      CHEEl->BType = formDecl->BType;
      CHEEl->RefID = formDecl->RefID;
      m_ChFormElemStr = QString(myWizard->myDoc->GetTypeLabel(CHEEl, false).c);
    }
    else {
      if (formDecl->DeclType == FormDef)
        m_ChFormElemStr = QString(formDecl->ParentDECL->LocalName.c);
      else 
          m_ChFormElemStr = QString(formDecl->LocalName.c); 
      CExecLike * execLike = new CExecLike(myWizard->myDoc, m_ChElemFormTypes, formDecl);
      delete execLike;
    }
  }
  setMinimumSize(size());
  myWizard->addTab(this, "Chain/array element");

  myWizard->AddChainPages(CHEEl);
  UpdateData(false);

}


void CChainFormPage::OnSelendokChElemFormTypes(int) 
{
  UpdateData(true);
  if (SelEndOKToStr(m_ChElemFormTypes, &m_ChFormElemStr, &CHEEl->RefID) > 0) {
    CHEEl->DeclDescType = NamedType;
    UpdateData(false);
    myWizard->setModified(true);
  }
}


void CChainFormPage::OnConstChain() 
{
  TAnnotation* anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  if (m_ConstChain->isOn()) {
    anno->IterFlags.INCL(FixedCount);
    anno->IterFlags.INCL(NoEllipsis);
  }
  else
    anno->IterFlags.EXCL(FixedCount);
  m_HasButtons->setChecked(!anno->IterFlags.Contains(NoEllipsis));
  m_HasButtons->setEnabled(!anno->IterFlags.Contains(FixedCount));
  m_EDITDelButton->setEnabled(!anno->IterFlags.Contains(NoEllipsis));
  m_EditInsertButton->setEnabled(!anno->IterFlags.Contains(NoEllipsis));
  myWizard->setModified(true);
  
}

void CChainFormPage::OnHasButtons() 
{
  TAnnotation* anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  bool has = (m_HasButtons->isOn());
  m_EDITDelButton->setEnabled(has);
  m_EditInsertButton->setEnabled(has);
  if (has) {
    anno->IterFlags.EXCL(NoEllipsis);
    anno->IterFlags.EXCL(FixedCount);
  }
  else
    anno->IterFlags.INCL(NoEllipsis);
  m_ConstChain->setChecked(anno->IterFlags.Contains(FixedCount));
  m_ConstChain->setEnabled(anno->IterFlags.Contains(NoEllipsis));
  myWizard->setModified(true);

}


void CChainFormPage::m_EditInsertButton_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CChainFormPage::m_EDITDelButton_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CChainFormPage::m_DefaultLength_valueChanged( int )
{
  myWizard->setModified(modify);
}

bool CChainFormPage::OnApply() 
{
  int il, dl;
  UpdateData(true);
  TAnnotation* anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  if (anno && anno->IterFlags.Contains(NoEllipsis) ) {
    anno->String1.Reset(0);
    anno->String2.Reset(0);
  }
  else {
    il = m_InsText.length();
    dl = m_DelText.length();
    if ((il || dl) && !anno)
      myWizard->FormDECL->Annotation.ptr->FA.ptr = new TAnnotation;
    anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
    if (il)
      anno->String1 = DString(m_InsText);
    if (dl)
      anno->String2 = DString(m_DelText);
  }
  if (myWizard->VFormDECL->SecondTFlags.Contains(isArray))
    myWizard->FormDECL->Annotation.ptr->Length.DecPoint = m_Len;
  myWizard->FormDECL->RefID = CHEEl->RefID;
  return true;  
}

void CFormTextPage::UpdateData(bool getData)

{
  if (getData) {
    m_LiSpace = m_LiSPIN1->value();
    m_LiTab = m_LiSPIN2->value();
    m_LiFrmSpace = m_LiSPIN3->value();
    m_lit = m_EditLiteral->text();
  }
  else {
    modify = false;
    m_LiSPIN1->setValue(m_LiSpace);
    m_LiSPIN2->setValue(m_LiTab);
    m_LiSPIN3->setValue(m_LiFrmSpace);
    m_EditLiteral->setText(m_lit);
    modify = true;
  }
}


CFormTextPage::CFormTextPage(LavaDECL * litEl, CLavaPEWizard *wizard)  
  : Ui_FormtextPage()
{
  myWizard = wizard;
  m_lit = "";
  myDecl = litEl;
  modify = false;
  m_LiSPIN1->setMinValue(0);
  m_LiSPIN1->setMaxValue(100);
  m_LiSPIN2->setMinValue(0);
  m_LiSPIN2->setMaxValue(100);
  m_LiSPIN3->setMinValue(0);
  m_LiSPIN3->setMaxValue(100);
  m_LiFrmSpace = 0;
  m_LiSpace = 0;
  m_LiTab = 0;
  modify = true;
  myDecl->DeclDescType = LiteralString;
  if (myDecl->LitStr.l)
    m_lit = QString(myDecl->LitStr.c);
  if (myDecl->Annotation.ptr)
    SetSpace((TAnnotation*)myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  else {
    if (!myDecl->Annotation.ptr)
      myDecl->Annotation.ptr = new TAnnotation;
    myDecl->Annotation.ptr->FA.ptr = new TAnnotation;
  }
  bool asFirst = myDecl->OwnID == ((LavaDECL*)((CHE*)myDecl->ParentDECL->NestedDecls.first)->data)->OwnID;
  if (!asFirst || !myDecl->ParentDECL->Supports.first) {
    m_beforeBase->setChecked(false);
    m_beforeBase->setEnabled(false);
  }
  else {
    m_beforeBase->setChecked(myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
    m_beforeBase->setEnabled(true);
  }
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l && lf.fromString(myDecl->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontName->setText(nn);
    fontName->show();
    fontButton->show();
    defaultFont->setChecked(false);
  }
  else {
    fontName->hide();
    fontName->setText(QString::null);
    fontButton->hide();
    defaultFont->setChecked(true);
  }
  colorSetting.Init(anno_Color, myDecl, defaultBackground, defaultForeground,
                    colorButtonB, colorButtonF, ForeColor, BackColor);
  UpdateData(false);
}


void CFormTextPage::defaultFont_clicked()
{
  if (defaultFont->isChecked()) {
    myDecl->Annotation.ptr->String1.Reset(0);
    fontName->hide();
    fontName->setText(QString::null);
    fontButton->hide();
  }  
  else {
    fontButton->show();
  }
  myWizard->setModified(modify);
}

void CFormTextPage::fontButton_clicked()
{
  bool ok;
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l) {
    lf.fromString(myDecl->Annotation.ptr->String1.c);
    lf = QFontDialog::getFont(&ok,lf,this);
  }
  else
    lf = QFontDialog::getFont(&ok,LBaseData->m_FormLabelFont,this);
  if (ok) {
    myDecl->Annotation.ptr->String1 = DString(lf.toString());
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontName->setText(nn);
    fontName->show();
    myWizard->setModified(modify);
    if (modify)
      myWizard->myView->PostApplyHint();
  }
}


void CFormTextPage::defaultForeground_clicked()
{
  if (colorSetting.defaultForeground_clicked())
    myWizard->setModified(modify);

}

void CFormTextPage::defaultBackground_clicked()
{
  if (colorSetting.defaultBackground_clicked())
    myWizard->setModified(modify);
}

void CFormTextPage::colorButtonF_clicked()
{
  if (colorSetting.colorButtonF_clicked() && modify) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFormTextPage::colorButtonB_clicked()
{
  if (colorSetting.colorButtonB_clicked() && modify) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}



void CFormTextPage::m_EditLiteral_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CFormTextPage::m_HorizTab_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::m_HorizSpace_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::m_VertSpace_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::OnApply() 
{
  UpdateData(true);
  if (!myDecl->LocalName.l)
    myDecl->LocalName = DString("_Text");
  myDecl->LitStr = DString(m_lit);
  GetSpace(&myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  if (m_beforeBase->isChecked())
    myDecl->Annotation.ptr->BasicFlags.INCL(beforeBaseType);
  else
    myDecl->Annotation.ptr->BasicFlags.EXCL(beforeBaseType);
}



void CFormTextBox::UpdateData(bool getData)

{
  if (getData) {
    m_LiSpace = m_LiSPIN1->value();
    m_LiTab = m_LiSPIN2->value();
    m_LiFrmSpace = m_LiSPIN3->value();
    m_lit = m_EditLiteral->text();
  }
  else {
    m_LiSPIN1->setValue(m_LiSpace);
    m_LiSPIN2->setValue(m_LiTab);
    m_LiSPIN3->setValue(m_LiFrmSpace);
    m_EditLiteral->setText(m_lit);
  }
}


CFormTextBox::CFormTextBox(LavaDECL * litEl, QWidget* pParent, bool asFirst)  
  : Ui_IDD_LiteralItem()
{
  m_lit = "";
  myDecl = litEl;
  m_LiSPIN1->setMinValue(0);
  m_LiSPIN1->setMaxValue(100);
  m_LiSPIN2->setMinValue(0);
  m_LiSPIN2->setMaxValue(100);
  m_LiSPIN3->setMinValue(0);
  m_LiSPIN3->setMaxValue(100);
  m_LiFrmSpace = 0;
  m_LiSpace = 0;
  m_LiTab = 0;
  myDecl->DeclDescType = LiteralString;
  if (myDecl->LitStr.l)
    m_lit = QString(myDecl->LitStr.c);
  if (myDecl->Annotation.ptr)
    SetSpace((TAnnotation*)myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  else {
    if (!myDecl->Annotation.ptr)
      myDecl->Annotation.ptr = new TAnnotation;
    myDecl->Annotation.ptr->FA.ptr = new TAnnotation;
  }
  if (!asFirst || !myDecl->ParentDECL->Supports.first) {
    m_beforeBase->setChecked(false);
    m_beforeBase->setEnabled(false);
  }
  else {
    m_beforeBase->setChecked(myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
    m_beforeBase->setEnabled(true);
  }
  m_beforeBase->setChecked(asFirst && myDecl->Supports.first && myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
  UpdateData(false);
}


void CFormTextBox::OnOK() 
{
  UpdateData(true);
  if (!myDecl->LocalName.l)
    myDecl->LocalName = DString("_Text");
  myDecl->LitStr = DString(m_lit);
  GetSpace(&myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  if (m_beforeBase->isChecked())
    myDecl->Annotation.ptr->BasicFlags.INCL(beforeBaseType);
  else
    myDecl->Annotation.ptr->BasicFlags.EXCL(beforeBaseType);
  QDialog::accept();
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::UpdateData(bool getData)
{
  if (getData) {
    //m_NewName = m_NewName6->text();
    m_NewType = m_NewTypeType->text();
  }
  else {
    //m_NewName6->setText(m_NewName);
    m_NewTypeType->setText(m_NewType);

  }
}

CGeneralPage::CGeneralPage(CLavaPEWizard *wizard) 
: Ui_IDD_GeneralPage()
{
  DString dstr;
  LavaDECL *classDecl, *decl=0;
  
  myWizard = wizard;
  //m_NewName = QString(myWizard->FormDECL->LocalName.c);
  m_NewName6->setText(myWizard->FormDECL->LocalName.c);
  dstr = myWizard->myDoc->GetTypeLabel(myWizard->FormDECL, false);
  m_NewType = QString(dstr.c);
  classDecl = myWizard->myDoc->IDTable.GetDECL(((CHETID*)myWizard->FormDECL->Supports.first)->data, myWizard->FormDECL->inINCL);
  if (classDecl)
    decl = myWizard->myDoc->IDTable.GetDECL(classDecl->RefID, classDecl->inINCL);

  if (decl) {
    for (; decl && (decl->DeclType == VirtualType); decl = myWizard->myDoc->IDTable.GetDECL(decl->RefID, decl->inINCL));
    CExecLike * execLike = new CExecLike(myWizard->myDoc, m_NamedTypes, decl);
    delete execLike;
  }
  CComboBoxItem *listItem;
  listItem = (CComboBoxItem*)m_NamedTypes->listBox()->findItem(m_NewType, Qt::ExactMatch | Qt::CaseSensitive);
  if (listItem) 
    m_NamedTypes->setCurrentText(listItem->text());
  
  UpdateData(false);
}

void CGeneralPage::OnSelendokNamedTypes(int) 
{
  UpdateData(true);
  if (SelEndOKToStr(m_NamedTypes, &m_NewType, &myWizard->FormDECL->RefID) > 0) {
    myWizard->FormDECL->DeclDescType = NamedType;
    if (myWizard->myDoc->IDTable.GetDECL(myWizard->FormDECL->RefID)->DeclType == FormDef) {
      ((TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr)->BasicFlags.INCL(Groupbox);
      if (myWizard->IOPage)
        myWizard->IOPage->m_groupbox->setChecked(true);
    }
    UpdateData(false);
    myWizard->setModified(true);
  }
}

void ColorSetting::Init(AnnoExType role, LavaDECL* decl, QCheckBox* defaultB,
                               QCheckBox* defaultF,
                               QPushButton* ButtonB,
                               QPushButton* ButtonF,
                               Q3Frame* Fore,
                               Q3Frame* Back)
{
  Role = role;
  FormDECL = decl;
  defaultBackground = defaultB;
  defaultForeground = defaultF;
  colorButtonB = ButtonB;
  colorButtonF = ButtonF;
  ForeColor = Fore;
  BackColor = Back;
  CHETAnnoEx* ex = FormDECL->Annotation.ptr->GetAnnoEx(Role);
  if (ex && ex->data.RgbBackValid) {
    BColor = QColor(ex->data.RgbBackColor);
    BackColor->setPaletteBackgroundColor(BColor);
    BackColor->show();
    colorButtonB->show();
    defaultBackground->setChecked(false);
  }
  else {
    BackColor->hide();
    colorButtonB->hide();
    defaultBackground->setChecked(true);
  }
  if (ex && ex->data.RgbForeValid) {
    FColor = QColor(ex->data.RgbForeColor);
    ForeColor->setPaletteBackgroundColor(FColor);
    ForeColor->show();
    colorButtonF->show();
    defaultForeground->setChecked(false);
  }
  else {
    ForeColor->hide();
    colorButtonF->hide();
    defaultForeground->setChecked(true);
  }
}


bool ColorSetting::defaultForeground_clicked()
{
  CHETAnnoEx* ex = FormDECL->Annotation.ptr->GetAnnoEx(Role);
  if (defaultForeground->isChecked()) {
    ForeColor->hide();
    colorButtonF->hide();
    if (ex) {
      ex->data.RgbForeValid = false;
      if (!ex->data.RgbBackValid)
        FormDECL->Annotation.ptr->DelAnnoEx(Role);
    }
    return true;
  }
  else 
    colorButtonF->show();
  return false;
}

bool ColorSetting::defaultBackground_clicked()
{
  CHETAnnoEx* ex = FormDECL->Annotation.ptr->GetAnnoEx(Role);
  if (defaultBackground->isChecked()) {
    BackColor->hide();
    colorButtonB->hide();
    if (ex) {
      ex->data.RgbBackValid = false;
      if (!ex->data.RgbForeValid)
        FormDECL->Annotation.ptr->DelAnnoEx(Role);
    }
    return true;
  }
  else 
    colorButtonB->show();
  return false;
}

bool ColorSetting::colorButtonF_clicked()
{
  CHETAnnoEx* ex = FormDECL->Annotation.ptr->GetAnnoEx(Role);
  QColor cl;
  if (ex && ex->data.RgbForeValid)
    cl = QColorDialog::getColor(FColor);
  else
    cl = QColorDialog::getColor();
  if (cl.isValid()) {
    ex = FormDECL->Annotation.ptr->GetAnnoEx(Role, true);
    FColor = cl;
    ex->data.RgbForeValid = true;
    ex->data.RgbForeColor = FColor.rgb();
    ForeColor->show();
    ForeColor->setPaletteBackgroundColor(FColor);
    return true;
  }
  return false;
}

bool ColorSetting::colorButtonB_clicked()
{
  CHETAnnoEx* ex = FormDECL->Annotation.ptr->GetAnnoEx(Role);
  QColor cl;
  if (ex && ex->data.RgbBackValid)
    cl = QColorDialog::getColor(BColor);
  else
    cl = QColorDialog::getColor();
  if (cl.isValid()) {
    ex = FormDECL->Annotation.ptr->GetAnnoEx(Role, true);
    BColor = cl;
    ex->data.RgbBackValid = true;
    ex->data.RgbBackColor = BColor.rgb();
    BackColor->show();
    BackColor->setPaletteBackgroundColor(BColor);
    return true;
  }
  return false;

}


CFontColorPage::CFontColorPage(CLavaPEWizard *wizard, LavaDECL *formDECL)
: Ui_idd_fontcolorpage()
{
  myWizard = wizard;
  FormDECL = formDECL;
  QFont lf;
  if (FormDECL->Annotation.ptr->String1.l && lf.fromString(FormDECL->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontNameL->setText(nn);
    fontNameL->show();
    fontButtonL->show();
    defaultFontL->setChecked(false);
  }
  else {
    fontNameL->hide();
    fontNameL->setText(QString::null);
    fontButtonL->hide();
    defaultFontL->setChecked(true);
  }
  if (FormDECL->Annotation.ptr->String2.l && lf.fromString(FormDECL->Annotation.ptr->String2.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontNameT->setText(nn);
    fontNameT->show();
    fontButtonT->show();
    defaultFontT->setChecked(false);
  }
  else {
    fontNameT->hide();
    fontNameT->setText(QString::null);
    fontButtonT->hide();
    defaultFontT->setChecked(true);
  }
  
  colorSetting.Init(anno_Color, FormDECL, defaultBackground, defaultForeground,
                    colorButtonB, colorButtonF, ForeColor, BackColor);
  if (FormDECL->DeclType == FormDef) {
    TextColorSetting.Init(anno_TextColor, FormDECL, defaultTBackground, defaultTForeground,
                      TColorButtonB, TColorButtonF, ForeTColor, BackTColor);
    PBColorSetting.Init(anno_PBColor, FormDECL, defaultPBBackground, defaultPBForeground,
                      PBColorButtonB, PBColorButtonF, ForePBColor, BackPBColor);
  }
  else {
    groupBoxT->hide();
    groupBoxPB->hide();
  }

}

CFontColorPage::~CFontColorPage()
{

}

bool CFontColorPage::OnApply()
{
  return true;
}


void CFontColorPage::defaultFontL_clicked()
{
  if (defaultFontL->isChecked()) {
    FormDECL->Annotation.ptr->String1.Reset(0);
    fontNameL->hide();
    fontNameL->setText(QString::null);
    fontButtonL->hide();
    myWizard->setModified(true);
  }  
  else {
    fontButtonL->show();
  }
}

void CFontColorPage::fontButtonL_clicked()
{
  bool ok;
  QFont lf;
  if (FormDECL->Annotation.ptr->String1.l) {
    lf.fromString(FormDECL->Annotation.ptr->String1.c);
    lf = QFontDialog::getFont(&ok,lf,this);
  }
  else
    lf = QFontDialog::getFont(&ok,LBaseData->m_FormLabelFont,this);
  if (ok) {
    FormDECL->Annotation.ptr->String1 = DString(lf.toString());
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontNameL->setText(nn);
    fontNameL->show();
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::defaultFontT_clicked()
{
  if (defaultFontT->isChecked()) {
    FormDECL->Annotation.ptr->String2.Reset(0);
    fontNameT->hide();
    fontNameT->setText(QString::null);
    fontButtonT->hide();
    myWizard->setModified(true);
  }  
  else {
    fontButtonT->show();
  }
}

void CFontColorPage::fontButtonT_clicked()
{
  bool ok;
  QFont lf;
  if (FormDECL->Annotation.ptr->String2.l) {
    lf.fromString(FormDECL->Annotation.ptr->String2.c);
    lf = QFontDialog::getFont(&ok,lf,this);
  }
  else
    lf = QFontDialog::getFont(&ok,LBaseData->m_FormFont,this);
  if (ok) {
    FormDECL->Annotation.ptr->String2 = DString(lf.toString());
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontNameT->setText(nn);
    fontNameT->show();
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::defaultForeground_clicked()
{
  if (colorSetting.defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::defaultBackground_clicked()
{
  if (colorSetting.defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::colorButtonF_clicked()
{
  if (colorSetting.colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::colorButtonB_clicked()
{
  if (colorSetting.colorButtonB_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::defaultTForeground_clicked()
{
  if (TextColorSetting.defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::TColorButtonF_clicked()
{
  if (TextColorSetting.colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::TColorButtonB_clicked()
{
  if (TextColorSetting.colorButtonB_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::defaultTBackground_clicked()
{
  if (TextColorSetting.defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::defaultPBForeground_clicked()
{
  if (PBColorSetting.defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::PBColorButtonF_clicked()
{
  if (PBColorSetting.colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::defaultPBBackground_clicked()
{
  if (PBColorSetting.defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::PBColorButtonB_clicked()
{
  if (PBColorSetting.colorButtonB_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

CIOPage::~CIOPage()
{
}

void CIOPage::UpdateData(bool getData)
{
  if (getData) {
    v_Input = m_isInput->isOn();
    v_Output = m_isOutput->isOn();
    v_DefaultIO = m_UseDefault->isOn();
    v_noEcho = m_Echo->isOn();
    v_NoFIO = m_NoFIO->isOn();
    v_MultiLine = m_MultiLine->isOn();
    v_isPopupW = m_Popupw->isOn();

    m_Space = m_SPIN1->value();
    m_Tab = m_SPIN2->value();
    m_Frmspace = m_SPIN3->value();
    m_SpaceItem = m_SPIN4->value();
    m_TabItem = m_SPIN5->value();
    m_FramespaceItem = m_SPIN6->value();
    m_FLength = m_SPIN7->value();
    m_Rows = m_SPIN8->value();
  
  }
  else {
    modify = false;
    m_isInput->setChecked(v_Input);
    m_isOutput->setChecked(v_Output);
    m_UseDefault->setChecked(v_DefaultIO);
    m_Echo->setChecked(v_noEcho);
    m_NoFIO->setChecked(v_NoFIO);
    m_MultiLine->setChecked(v_MultiLine);
    m_Popupw->setChecked(v_isPopupW);
    m_SPIN1->setValue(m_Space);
    m_SPIN2->setValue(m_Tab);
    m_SPIN3->setValue(m_Frmspace);
    m_SPIN4->setValue(m_SpaceItem);
    m_SPIN5->setValue(m_TabItem);
    m_SPIN6->setValue(m_FramespaceItem);
    m_SPIN7->setValue(m_FLength);
    m_SPIN8->setValue(m_Rows);
    modify = true;
  }
}

CIOPage::CIOPage(CLavaPEWizard *wizard, LavaDECL *formDECL, bool forChElem)
 : Ui_IDD_IOPage()
{
  v_Input = false;
  v_Output = false;
  v_NoFIO = false;
  v_DefaultIO = false;
  v_noEcho = false;
  v_isPopupW = false;
  m_Frmspace = 0;
  m_Space = 0;
  m_Tab = 0;
  m_SpaceItem = 0;
  m_TabItem = 0;
  m_FramespaceItem = 0;
  m_FLength = 0;
  m_Rows = 0;
  v_MultiLine = false;
  modify = false;
  myWizard = wizard;
  FormDECL = formDECL;
  ForChainElem = forChElem;
  if (!FormDECL->Annotation.ptr)
    FormDECL->Annotation.ptr = new TAnnotation;
  if (!FormDECL->Annotation.ptr->FA.ptr)
    FormDECL->Annotation.ptr->FA.ptr = new TAnnotation;
  SetProps();


  UpdateData(false);
  m_SPIN1->setMinValue (0);
  m_SPIN2->setMinValue (0);
  m_SPIN3->setMinValue (0);
  m_SPIN4->setMinValue (0);
  m_SPIN5->setMinValue (0);
  m_SPIN6->setMinValue (0);
  m_SPIN7->setMinValue (0);
  m_SPIN8->setMinValue (0);
  m_SPIN1->setMaxValue (100);
  m_SPIN2->setMaxValue (100);
  m_SPIN3->setMaxValue (100);
  m_SPIN4->setMaxValue (100);
  m_SPIN5->setMaxValue (100);
  m_SPIN6->setMaxValue (100);
  m_SPIN7->setMaxValue (100);
  m_SPIN8->setMaxValue (100);
  modify = true;
}


void CIOPage::SetProps()
{
  LavaDECL *inEl;
  TAnnotation *anno;
  TBasicType etype;
  bool atomic;

  if (myWizard->FormDECL->DeclDescType == EnumType) 
    inEl = &((TEnumDescription*)FormDECL->EnumDesc.ptr)->EnumField;
  else 
    inEl = FormDECL;
  anno = (TAnnotation*)inEl->Annotation.ptr->FA.ptr;
  if (myWizard->FormDECL->TypeFlags.Contains(isPlaceholder)) {
    v_NoFIO = true;
    v_noEcho = false;
    v_Input = false;
    v_Output = false;
    v_DefaultIO = false;
    m_isInput->setEnabled(false);
    m_isOutput->setEnabled(false);
    m_UseDefault->setEnabled(false);
    m_Echo->setEnabled(false);
    m_NoFIO->setEnabled(false);
  }
  else {
    v_noEcho  = (anno && (anno->Emphasis == NoEcho));
    v_Input = anno->IoSigFlags.Contains(Flag_INPUT);
    v_Output = anno->IoSigFlags.Contains(Flag_OUTPUT);
    v_NoFIO =  anno->IoSigFlags.Contains(DONTPUT);
    v_DefaultIO = !(v_Input || v_Output || v_NoFIO);
  }
  v_isPopupW = anno->BasicFlags.Contains(PopUp);
  m_groupbox->setChecked(anno->BasicFlags.Contains(Groupbox));
  if (FormDECL->DeclType == FormDef) {
    m_SPIN2->setEnabled(false);
    m_SPIN1->setEnabled(false);
    m_SPIN3->setEnabled(false);
    m_groupbox->setEnabled(false);
  }
  else
    SetSpace(FormDECL->Annotation.ptr, m_Tab, m_Space, m_Frmspace);
  if (!ForChainElem && (inEl->DeclType == VirtualType))
    etype = NonBasic;
  else
    etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, inEl);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString);
  if (myWizard->IOPage)
    myWizard->IOPage->m_Popupw->setEnabled(false);
  m_Popupw->setEnabled((etype == NonBasic)  && (!myWizard->ChainIOPage || (myWizard->ChainIOPage == this)));
  m_SPIN7->setEnabled(atomic);
  if (anno && atomic && (anno->Length.Field > 0))
    m_FLength = anno->Length.Field;
  SetSpace(anno, m_TabItem, m_SpaceItem, m_FramespaceItem);
  m_MultiLine->setEnabled(etype == VLString);
  v_MultiLine = anno->BasicFlags.Contains(Text);
  if (v_MultiLine)
    m_Rows = anno->Length.DecPoint;
  m_SPIN8->setEnabled(v_MultiLine);
}

void CIOPage::GetProps()
{
  LavaDECL *inEl; 
  TAnnotation * anno, **p_anno;
  TBasicType etype;
  bool atomic;

  if (!FormDECL->Annotation.ptr)
    FormDECL->Annotation.ptr = new TAnnotation;
  if (FormDECL->DeclDescType == EnumType) 
    inEl = &((TEnumDescription*)FormDECL->EnumDesc.ptr)->EnumField;
  else 
    inEl = FormDECL;
  p_anno = (TAnnotation**)&inEl->Annotation.ptr->FA.ptr;
  if (!(*p_anno)) 
    *p_anno = new TAnnotation;
  anno = *p_anno;
  if (v_isPopupW)
    anno->BasicFlags.INCL(PopUp);
  else 
    anno->BasicFlags.EXCL(PopUp);
  if (m_groupbox->isOn())
    anno->BasicFlags.INCL(Groupbox);
  else 
    anno->BasicFlags.EXCL(Groupbox);
  GetSpace(&FormDECL->Annotation.ptr, m_Tab, m_Space, m_Frmspace); 
  if (v_noEcho) 
    anno->Emphasis = NoEcho;
  else
    anno->Emphasis = Low;
  anno->IoSigFlags.EXCL(Flag_INPUT);
  anno->IoSigFlags.EXCL(Flag_OUTPUT);
  anno->IoSigFlags.EXCL(DONTPUT);
  if (inEl == FormDECL)
    inEl->Annotation.ptr->IoSigFlags.EXCL(DONTPUT);
  if (!v_DefaultIO) {
    if (v_Input)
      anno->IoSigFlags.INCL(Flag_INPUT);
    if (v_Output)
      anno->IoSigFlags.INCL(Flag_OUTPUT);
    if (v_NoFIO) {
      anno->IoSigFlags.INCL(DONTPUT);
      if (inEl == FormDECL)
        inEl->Annotation.ptr->IoSigFlags.INCL(DONTPUT);
    }
  }
  GetSpace(p_anno, m_TabItem, m_SpaceItem, m_FramespaceItem); 
  etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, inEl);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString);
  if (atomic) {
    anno->Length.Field = m_FLength;
    anno->BType = VLString;
  }
  if (v_MultiLine) {
    anno->BasicFlags.INCL(Text);
    anno->Length.DecPoint = m_Rows;
  }
  else
    anno->BasicFlags.EXCL(Text);
}


void CIOPage::OnisInput() 
{
  if (m_isInput->isOn()) {
    m_UseDefault->setChecked(0);
    m_NoFIO->setChecked(0);
    v_DefaultIO = false;
    v_NoFIO = false;
  }
  else
    if (!(m_NoFIO->isOn() || m_isOutput->isOn())) {
      m_UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  m_UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);
}

void CIOPage::OnisOutput() 
{
  if (m_isOutput->isOn())  {
    m_UseDefault->setChecked(0);
    m_NoFIO->setChecked(0);
    v_DefaultIO = false;
    v_NoFIO = false;
  }
  else
    if (!(m_isInput->isOn() || m_NoFIO->isOn())) {
      m_UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  m_UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);

}

void CIOPage::OnNoFIO() 
{
  if (m_NoFIO->isOn()) {
    m_isInput->setChecked(0);
    m_isOutput->setChecked(0);
    m_Echo->setChecked(0);
    m_UseDefault->setChecked(0);
    v_Input = false;
    v_Output = false;
    v_noEcho = false;
    v_DefaultIO = false;
    m_UseDefault->setEnabled(true);
  }
  else
    if (!(m_isInput->isOn() || m_isOutput->isOn())) {
      m_UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  m_UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);
}

void CIOPage::OnUseDefault() 
{
  if (m_UseDefault->isOn()) {
    m_isInput->setChecked(0);
    m_isOutput->setChecked(0);
    m_NoFIO->setChecked(0);
    v_Input = false;
    v_Output = false;
    v_NoFIO = false;
  }
  m_UseDefault->setEnabled(!m_UseDefault->isOn());
  myWizard->setModified(true);
}

void CIOPage::OnPopupw() 
{
  TDeclDescType dtype = ((CLavaPEApp*)wxTheApp)->Browser.GetExprType(myWizard->myDoc->mySynDef,  FormDECL);
  m_SPIN7->setEnabled(dtype == BasicType);
  v_isPopupW = !v_isPopupW;
  myWizard->setModified(true);
}

void CIOPage::OnMultiLine() 
{
  v_MultiLine = !v_MultiLine;
  m_SPIN8->setEnabled(v_MultiLine);
  myWizard->setModified(true);
}

void CIOPage::m_Echo_clicked()
{
  myWizard->setModified(true);
}

void CIOPage::horizTabChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::horizSpaceChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::vertSpaceChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::colsChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::rowsChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::m_groupbox_clicked()
{
  myWizard->setModified(modify);
}

bool CIOPage::OnApply() 
{
  UpdateData(true);
  GetProps();
  return true;
}


CLiteralsPage::~CLiteralsPage()
{
}


void CLiteralsPage::UpdateData(bool getData)
{
  if (getData) {
    v_Default = m_Default->text();
    if (m_EnumDefault->currentItem())
      v_EnumDefault = m_EnumDefault->currentText();
    else
      v_EnumDefault = QString("");
    if (m_BoolDefault->currentItem())
      v_BoolDefault = m_BoolDefault->currentText();
    else
      v_BoolDefault = QString("");
  }
  else {
    modify = false;
    m_Default->setText(v_Default);
    if (v_EnumDefault.length())
      m_EnumDefault->setCurrentText(v_EnumDefault);
    else
      m_EnumDefault->setCurrentItem(0);
    if (v_BoolDefault.length())
      m_BoolDefault->setCurrentText(v_BoolDefault);
    else
      m_BoolDefault->setCurrentItem(0);
    modify = true;
  }
}

CLiteralsPage::CLiteralsPage(CLavaPEWizard *wizard, LavaDECL* formDECL, bool forChElem)
   : Ui_LiteralsPage()
{
  v_Default = "";
  v_EnumDefault = "";
  v_BoolDefault = "";
  myWizard = wizard;
  FormDECL = formDECL;
  ForChainElem = forChElem;
  if (!FormDECL->Annotation.ptr)
    FormDECL->Annotation.ptr = new TAnnotation;
  SetProps();
  if ((FormDECL->DeclType == Attr)
    || (FormDECL->DeclType == VirtualType)
    || (FormDECL->DeclType == PatternDef)) {
    m_ADDPre->setEnabled(true);
    m_DELETEPre->setEnabled(false);
    m_EDITPre->setEnabled(false);
    m_ADDSuf->setEnabled(true);
    m_DELETESuf->setEnabled(false);
    m_EDITSuf->setEnabled(false);
    UpdateData(false);
  }
}


void CLiteralsPage::SetProps() //Element vom Typ FieldDesc
{
  QString str;
  TAnnotation* anno;
  LavaDECL *inEl = FormDECL, *decl, *enumEl=0;
  TBasicType etype;
  CHEEnumSelId* enumsel;
  CHE *inDefEl;
  bool atomic;
  CComboBoxItem *item;
  TID tid0;

  if ((FormDECL->DeclType == VirtualType) && !ForChainElem)
    etype = NonBasic;
  else
    etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, FormDECL);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString);
  isEnumera = (etype == Enumeration);
  if (isEnumera) {
    /*
    m_Default->hide();
    m_BoolDefault->hide();
    m_EnumDefault->show();
    */
    m_Default->setEnabled(false);
    m_BoolDefault->setEnabled(false);
    m_EnumDefault->setEnabled(true);
    if (FormDECL->DeclDescType == EnumType) {
      inEl = &((TEnumDescription*)inEl->EnumDesc.ptr)->EnumField;
      enumEl = inEl;
    }
    if (inEl->DeclDescType == NamedType) {
      decl = myWizard->myDoc->IDTable.GetFinalDef(inEl->RefID);
      if (decl) {
        enumEl = (LavaDECL*)decl;
        enumEl = &((TEnumDescription*)enumEl->EnumDesc.ptr)->EnumField;
      }
    }
    enumsel = (CHEEnumSelId*)enumEl->Items.first;
    while (enumsel) {   
      item = new CComboBoxItem(enumsel->data.Id, tid0);
      m_EnumDefault->listBox()->insertItem(item);
      enumsel = (CHEEnumSelId*)enumsel->successor;
    }
    if (inEl->Annotation.ptr && inEl->Annotation.ptr->FA.ptr)
      str = ((TAnnotation*)inEl->Annotation.ptr->FA.ptr)->StringValue.c; //Defaultwert
    if (!str.isEmpty()) {
      v_EnumDefault = str;
      m_EnumDefault->setCurrentText(v_EnumDefault);
    }
    else
      m_EnumDefault->setCurrentItem(0);
  }
  else {
    m_EnumDefault->setEnabled(false);
    if (FormDECL->Annotation.ptr && FormDECL->Annotation.ptr->FA.ptr) {
      if (!atomic) {
        m_Default->setEnabled(false);
        m_BoolDefault->setEnabled(false);
      }
      else {
        anno = (TAnnotation*)FormDECL->Annotation.ptr->FA.ptr;
        if (inEl->BType == B_Bool) {
          m_Default->setEnabled(false);
          m_BoolDefault->setEnabled(true);
          if (anno->IoSigFlags.Contains(trueValue)) {
            v_BoolDefault = anno->StringValue.c;
            m_BoolDefault->setCurrentText(v_BoolDefault); //Defaultwert
          }
          else
            m_BoolDefault->setCurrentItem(0);
        }
        else {
          m_Default->setEnabled(true);
          m_BoolDefault->setEnabled(false);
          if (anno->BType == Integer) {
            if (anno->IoSigFlags.Contains(trueValue)) {
              str = QString("%1").arg(anno->I);
              m_Default->setAlignment(Qt::AlignRight);
            }
          }
          else if (inEl->BType == Char) {
            if (anno->IoSigFlags.Contains(trueValue)) 
              str = anno->StringValue.c; //Defaultwert
            m_Default->setMaxLength(1);
            str = anno->StringValue.c;
          }
          else
            str = anno->StringValue.c; //Defaultwert
        }
      }
      m_Default->setEnabled(atomic && (etype != B_Bool));
      v_Default = str;
    }
  }
  inDefEl = (CHE*)FormDECL->Annotation.ptr->Prefixes.first;
  while (inDefEl /*&& !((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.Contains(Toggle)*/) {
    item = new CComboBoxItem(((LavaDECL*)inDefEl->data)->LitStr, tid0);
    m_Prefixe->insertItem(item);
    inDefEl = (CHE*)inDefEl->successor;
  }
  inDefEl = (CHE*)FormDECL->Annotation.ptr->Suffixes.first;
  while (inDefEl) {
    item = new CComboBoxItem(((LavaDECL*)inDefEl->data)->LitStr, tid0);
    m_Suffixe->insertItem(item);
    inDefEl = (CHE*)inDefEl->successor;
  }
}

void CLiteralsPage::GetProps() 
{
  LavaDECL *inEl, *litEl, *litEl1;
  TAnnotation** p_anno;
  int selPos;
  TBasicType etype;
  bool atomic;

  if (FormDECL->DeclDescType == EnumType) 
    inEl = &((TEnumDescription*)FormDECL->EnumDesc.ptr)->EnumField;
  else 
    inEl = FormDECL;
  p_anno = (TAnnotation**)&inEl->Annotation.ptr->FA.ptr;
  etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, inEl);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString) || isEnumera;
  if ( atomic) {
    if (isEnumera) {
      selPos = m_EnumDefault->currentItem();
      if ((*p_anno == 0) && selPos && (selPos > 0))
        *p_anno = new TAnnotation;
    }
    else if (inEl->BType == B_Bool) {
      selPos = m_BoolDefault->currentItem();
      if ((*p_anno == 0) && selPos && (selPos > 0))
        *p_anno = new TAnnotation;
    }
    else
      if ((*p_anno == 0) && v_Default.length())
        *p_anno = new TAnnotation;
    if (*p_anno) {
      if (isEnumera) {
        (*p_anno)->BType = Identifier;
        if (selPos && (selPos > 0))
          (*p_anno)->StringValue = DString(v_EnumDefault);
        else
          (*p_anno)->StringValue.Reset(0);
      }
      else if (inEl->BType == B_Bool) {
        (*p_anno)->BType = B_Bool;
        if (selPos && (selPos > 0)) {
          (*p_anno)->IoSigFlags.INCL(trueValue);
          (*p_anno)->StringValue = DString(v_BoolDefault);
          if (selPos == 1)
            (*p_anno)->B = false;
          else
            (*p_anno)->B = true;
        }
        else {
          (*p_anno)->StringValue.Reset(0);
          (*p_anno)->IoSigFlags.EXCL(trueValue);
        }

        litEl = 0;
        if (inEl->Annotation.ptr->Suffixes.first)
          litEl = (LavaDECL*)((CHE*)inEl->Annotation.ptr->Suffixes.first)->data;
        if (inEl->Annotation.ptr->Prefixes.first) {
          if (litEl) {
            litEl1 = (LavaDECL*)((CHE*)inEl->Annotation.ptr->Prefixes.first)->data;
            litEl1->Annotation.ptr->BasicFlags.EXCL(Toggle);
          }
          else
            litEl = (LavaDECL*)((CHE*)inEl->Annotation.ptr->Prefixes.first)->data;
        }
        if  (litEl)
          litEl->Annotation.ptr->BasicFlags.INCL(Toggle);
      } 
      else  if ((inEl->BType == VLString) || (inEl->BType == Char)) {
        (*p_anno)->BType = VLString;
        (*p_anno)->StringValue = DString(v_Default);
      }
      else  if (inEl->BType == Integer) {
        char * endptr;
        (*p_anno)->BType = Integer;
        (*p_anno)->StringValue = DString(v_Default);
        if ((*p_anno)->StringValue.l) {
          (*p_anno)->IoSigFlags.INCL(trueValue);
          (*p_anno)->I = strtol( v_Default, &endptr, 10);
        }
        else
          (*p_anno)->IoSigFlags.EXCL(trueValue);
        (*p_anno)->Alignment = RightAligned;
      }
      else {
        if (atomic) {
          (*p_anno)->StringValue = DString(v_Default);
          (*p_anno)->BType = VLString;
          if ((*p_anno)->StringValue.l)
            (*p_anno)->IoSigFlags.INCL(trueValue);
          else
            (*p_anno)->IoSigFlags.EXCL(trueValue);
        }
        else
          (*p_anno)->IoSigFlags.EXCL(trueValue);
      }
    }
  }

}


bool CLiteralsPage::OnAdd(ChainAny0* chain, Q3ListBox* m_list/*, int transpos*/) 
{
  int ins = m_list->currentItem();
  LavaDECL* Decl = NewLavaDECL();
  CHE* cheDecl;

  if (ins < 0)
    ins = 0;
  else
    ins += 1;
  Decl->Annotation.ptr = new TAnnotation;
  Decl->DeclDescType = LiteralString;
  CLiteralItem *lItem = new CLiteralItem(this, true, ins, m_list, Decl);
  if (lItem->exec() == QDialog::Accepted) {
    QString qs = m_list->item(ins)->text();
    Decl->LitStr = DString(qs);
    if (Decl->LitStr.l != 0) {
      cheDecl = NewCHE(Decl);
      chain->AddNth(ins+1/*+transpos*/, cheDecl);
      m_list->setCurrentItem(ins);
      delete lItem;
      myWizard->setModified(true);
      myWizard->myView->PostApplyHint();
      return true;
    }
    else
      delete Decl;
  }
  delete lItem;
  return false;
}

void CLiteralsPage::OnADDPre() 
{
  if (OnAdd(&FormDECL->Annotation.ptr->Prefixes, m_Prefixe)) {
    m_DELETEPre->setEnabled(true);
    m_EDITPre->setEnabled(true);
  }
}

void CLiteralsPage::OnADDSuf() 
{
  if (OnAdd(&FormDECL->Annotation.ptr->Suffixes, m_Suffixe/*, sufTranspos*/)) {
    m_DELETESuf->setEnabled(true);
    m_EDITSuf->setEnabled(true);
  }
}

void CLiteralsPage::OnDELETEPre() 
{
  int idel = m_Prefixe->currentItem();
  if (idel >= 0) {
    CHE* cheDecl = (CHE*)FormDECL->Annotation.ptr->Prefixes.UncoupleNth(idel+1);
    if (cheDecl) {
      delete cheDecl;
      m_Prefixe->removeItem(idel);
      m_DELETEPre->setEnabled((m_Prefixe->count() > 0));
      m_EDITPre->setEnabled((m_Prefixe->count() > 0));
      myWizard->setModified(true);
    }
  }
}

void CLiteralsPage::OnDELETESuf() 
{
  int idel = m_Suffixe->currentItem();
  if (idel >= 0) {
    CHE * cheDecl = (CHE *)FormDECL->Annotation.ptr->Suffixes.UncoupleNth(idel+1/*+sufTranspos*/);
    if (cheDecl) {
      delete cheDecl;
      m_Suffixe->removeItem(idel);
      m_DELETESuf->setEnabled((m_Suffixe->count() > 0));
      m_EDITSuf->setEnabled((m_Suffixe->count() > 0));
      myWizard->setModified(true);
    }
  }
}


bool CLiteralsPage::OnEdit(ChainAny0 * chain, Q3ListBox* m_list/*, int transpos*/)
{
  int ins = m_list->currentItem();
  CHE* cheDecl = (CHE*)chain->GetNth(ins+1/*+transpos*/);
  LavaDECL* Decl = (LavaDECL*) cheDecl->data;
  if (ins >= 0) {
    CLiteralItem *lItem = new CLiteralItem(this, false, ins, m_list, Decl);
    if (lItem->exec() == QDialog::Accepted) {
      Decl->LitStr = DString(m_list->item(ins)->text());
      if (Decl->LitStr.l == 0) {
        if (chain->Uncouple(cheDecl))
          delete cheDecl;
        m_list->removeItem(ins);
        m_list->setCurrentItem(ins-1);
        delete lItem;
        myWizard->setModified(true);
        myWizard->myView->PostApplyHint();
        return false;
      }
      else {
        m_list->setCurrentItem(ins);
        delete lItem;
        myWizard->setModified(true);
        myWizard->myView->PostApplyHint();
        return true;
      }
    }
    delete lItem;
  }
  return false;
}

void CLiteralsPage::OnEDITPre() 
{
  if (OnEdit(&FormDECL->Annotation.ptr->Prefixes, m_Prefixe)) {
    m_DELETEPre->setEnabled(true);
    m_EDITPre->setEnabled(true);
  }
  else {
    m_DELETEPre->setEnabled((m_Prefixe->count() > 0));
    m_EDITPre->setEnabled((m_Prefixe->count() > 0));
  }
}

void CLiteralsPage::OnEditSuf() 
{
  if (OnEdit(&FormDECL->Annotation.ptr->Suffixes, m_Suffixe/*, sufTranspos*/)) {
    m_DELETESuf->setEnabled(true);
    m_EDITSuf->setEnabled(true);
  }
  else {
    m_DELETESuf->setEnabled((m_Suffixe->count() > 0));
    m_EDITSuf->setEnabled((m_Suffixe->count() > 0));
  }
}


void CLiteralsPage::m_Prefixe_doubleClicked( Q3ListBoxItem * )
{
  OnEDITPre(); 
}


void CLiteralsPage::m_Suffixe_doubleClicked( Q3ListBoxItem * )
{
  OnEditSuf();
}

void CLiteralsPage::OnSelchangePrefixe() 
{
  if (m_Prefixe->currentItem() < 0) {
    m_DELETEPre->setEnabled(false);
    m_EDITPre->setEnabled(false);
  }
  else {
    m_DELETEPre->setEnabled(true);
    m_EDITPre->setEnabled(true);
  }
}

void CLiteralsPage::OnSelchangeSuffixe() 
{
  if (m_Suffixe->currentItem() < 0) {
    m_DELETESuf->setEnabled(false);
    m_EDITSuf->setEnabled(false);
  }
  else {
    m_DELETESuf->setEnabled(true);
    m_EDITSuf->setEnabled(true);
  }
}

void CLiteralsPage::m_Default_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CLiteralsPage::m_EnumDefault_triggered(int )
{
  myWizard->setModified(true);
}

void CLiteralsPage::m_BoolDefault_triggered(int )
{
  myWizard->setModified(true);
}

bool CLiteralsPage::OnSetActive() 
{
  enabled = ((FormDECL->DeclType == Attr) 
    || (FormDECL->DeclType == PatternDef)
    || (FormDECL->DeclType == VirtualType));
  m_ADDSuf->setEnabled(enabled);
  m_ADDPre->setEnabled(enabled);
  m_EDITSuf->setEnabled(false);
  m_EDITPre->setEnabled(false);
  m_DELETESuf->setEnabled(false);
  m_DELETEPre->setEnabled(false);
  m_Suffixe->setEnabled(enabled);
  m_Prefixe->setEnabled(enabled);
  if (enabled) {
    m_Prefixe->setCurrentItem(-1);
    m_Suffixe->setCurrentItem(-1);
  }
  return true;
}


void CLiteralsPage::OnLButtonDown(unsigned nFlags, QPoint point) 
{
  m_Prefixe->setCurrentItem(-1);
  m_Suffixe->setCurrentItem(-1);
  m_DELETEPre->setEnabled(false);
  m_EDITPre->setEnabled(false);
  m_DELETESuf->setEnabled(false);
  m_EDITSuf->setEnabled(false);
}


bool CLiteralsPage::OnApply() 
{
  UpdateData(true);
  GetProps();
  return true;
}



void CLiteralItem::UpdateData(bool getData)
{
  if (getData) {
    m_lit = m_EditLiteral->text();
    m_LiSpace = m_LiSPIN1->value();
    m_LiTab = m_LiSPIN2->value();
    m_LiFrmSpace = m_LiSPIN3->value();
  }
  else {
    m_EditLiteral->setText(m_lit);
    m_LiSPIN1->setValue(m_LiSpace);
    m_LiSPIN2->setValue(m_LiTab);
    m_LiSPIN3->setValue(m_LiFrmSpace);
  }
}


CLiteralItem::CLiteralItem(CLiteralsPage *page, bool isNew, int ipos, Q3ListBox *litList, LavaDECL * litEl)  
  : Ui_IDD_LiteralItem()
{
  m_lit = "";
  m_beforeBase->hide();
  LitPage = page;
  litPos = ipos;
  LitList = litList;
  insert = isNew;
  myDecl = litEl;
  m_LiSPIN1->setMinValue (0);
  m_LiSPIN2->setMinValue (0);
  m_LiSPIN3->setMinValue (0);
  m_LiSPIN1->setMaxValue (100);
  m_LiSPIN2->setMaxValue (100);
  m_LiSPIN3->setMaxValue (100);
  m_LiFrmSpace = 0;
  m_LiSpace = 0;
  m_LiTab = 0;

  if (!insert) {
    m_lit = LitList->item(litPos)->text();
    SetSpace((TAnnotation*)myDecl->Annotation.ptr->FA.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  }
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l && lf.fromString(myDecl->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontName->setText(nn);
    fontName->show();
    fontButton->show();
    defaultFont->setChecked(false);
  }
  else {
    fontName->hide();
    fontName->setText(QString::null);
    fontButton->hide();
    defaultFont->setChecked(true);
  }
  colorSetting.Init(anno_Color, myDecl, defaultBackground, defaultForeground,
                    colorButtonB, colorButtonF, ForeColor, BackColor);
  UpdateData(false);
}


void CLiteralItem::defaultFont_clicked()
{
  if (defaultFont->isChecked()) {
    myDecl->Annotation.ptr->String1.Reset(0);
    fontName->hide();
    fontName->setText(QString::null);
    fontButton->hide();
  }  
  else {
    fontButton->show();
  }
}

void CLiteralItem::fontButton_clicked()
{
  bool ok;
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l) {
    lf.fromString(myDecl->Annotation.ptr->String1.c);
    lf = QFontDialog::getFont(&ok,lf,this);
  }
  else
    lf = QFontDialog::getFont(&ok,LBaseData->m_FormLabelFont,this);
  if (ok) {
    myDecl->Annotation.ptr->String1 = DString(lf.toString());
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeFloat() );
    fontName->setText(nn);
    fontName->show();
  }
}


void CLiteralItem::defaultForeground_clicked()
{
  colorSetting.defaultForeground_clicked();
}

void CLiteralItem::defaultBackground_clicked()
{
  colorSetting.defaultBackground_clicked();
}

void CLiteralItem::colorButtonF_clicked()
{
  colorSetting.colorButtonF_clicked();
}

void CLiteralItem::colorButtonB_clicked()
{
  colorSetting.colorButtonB_clicked();
}


void CLiteralItem::OnOK() 
{
  UpdateData(true);
  TID tid0;
  if (!insert)
    LitList->removeItem(litPos);
  CComboBoxItem* item = new CComboBoxItem(m_lit, tid0);
  LitList->insertItem(item, litPos);//-1);
  //LitList->InsertString(litPos, m_lit);
  GetSpace((TAnnotation**)&myDecl->Annotation.ptr->FA.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  LitPage->UpdateData(false);
  QDialog::accept();
}


CMenuPage::~CMenuPage()
{
}

void CMenuPage::UpdateData(bool getData)
{
  if (getData) {
    v_Menutype = m_Menutype->currentItem();
    m_ItemNr = m_Menuitems->currentItem();
    v_LeftLabel = m_LeftLabel->isOn();
    v_ToggleLabel = m_ToggleLabel->text();
  }
  else {
    modify = false;
    m_Menutype->setCurrentItem(v_Menutype);
    m_Menuitems->setCurrentItem(m_ItemNr);
    m_LeftLabel->setChecked(v_LeftLabel);
    m_ToggleLabel->setText(v_ToggleLabel);
    modify = true;
  }
}


CMenuPage::CMenuPage(CLavaPEWizard *wizard)
 : Ui_IDD_MenuPage()
{
  v_Menutype = -1;
  m_ItemNr = -1;
  v_LeftLabel = false;
  v_ToggleLabel = "";
  myWizard = wizard;
  if (!myWizard->FormDECL->Annotation.ptr)
    myWizard->FormDECL->Annotation.ptr = new TAnnotation;
  isEnumera = (myWizard->FormDECL->DeclDescType == EnumType);
  isBool = ((myWizard->FormDECL->DeclDescType == BasicType) && (myWizard->FormDECL->BType == B_Bool));
  if (isEnumera) {
    SetEProps();
    m_EditButton->setEnabled(false);
    m_DeleteButton->setEnabled(false);
    m_AddButton->setEnabled(v_Menutype != int(isOMenu));
    UpdateData(false);
  }
  else {
    if (isBool) {
      SetBProps();
      UpdateData(false);
    }
  }
}


void CMenuPage::SetBProps() 
{
  CHE *litEl = (CHE*)myWizard->FormDECL->Annotation.ptr->Prefixes.last;
  v_LeftLabel = (litEl && ((LavaDECL*)litEl->data)->Annotation.ptr->BasicFlags.Contains(Toggle));
  if (!v_LeftLabel)
    litEl = (CHE*)myWizard->FormDECL->Annotation.ptr->Suffixes.first;
  if  (litEl && ((LavaDECL*)litEl->data)->Annotation.ptr->BasicFlags.Contains(Toggle))
    v_ToggleLabel = QString(((LavaDECL*)litEl->data)->LitStr.c);
}

void CMenuPage::GetBProps() 
{
  DString label = DString(v_ToggleLabel);
  CHE *litElL = (CHE *)myWizard->FormDECL->Annotation.ptr->Prefixes.last;
  CHE *litElR = (CHE *)myWizard->FormDECL->Annotation.ptr->Suffixes.first;
  LavaDECL* Decl;
  if (v_LeftLabel) {
    if (!litElL || !((TAnnotation*)((LavaDECL*)litElL->data)->Annotation.ptr->FA.ptr)->BasicFlags.Contains(Toggle)) {
      if (litElR && ((TAnnotation*)((LavaDECL*)litElR->data)->Annotation.ptr->FA.ptr)->BasicFlags.Contains(Toggle))
        litElL = (CHE*) myWizard->FormDECL->Annotation.ptr->Suffixes.Uncouple(litElR);
      else {
        Decl = NewLavaDECL();
        Decl->DeclDescType = LiteralString;
        Decl->Annotation.ptr = new TAnnotation;
        Decl->Annotation.ptr->FA.ptr = new TAnnotation;
        ((TAnnotation*)Decl->Annotation.ptr->FA.ptr)->BasicFlags.INCL(Toggle);
        GetSpace((TAnnotation**)&Decl->Annotation.ptr->FA.ptr, 0, 0, 0);
        litElL = NewCHE(Decl);
      }
      myWizard->FormDECL->Annotation.ptr->Prefixes.Append(litElL);
    }
    ((LavaDECL*)litElL->data)->LitStr = label;
  }
  else {
    if (!litElR || !((TAnnotation*)((LavaDECL*)litElR->data)->Annotation.ptr->FA.ptr)->BasicFlags.Contains(Toggle)) {
      if (litElL && ((TAnnotation*)((LavaDECL*)litElL->data)->Annotation.ptr->FA.ptr)->BasicFlags.Contains(Toggle))
        litElR = (CHE*)myWizard->FormDECL->Annotation.ptr->Prefixes.Uncouple(litElL);
      else {
        Decl = NewLavaDECL();
        Decl->DeclDescType = LiteralString;
        Decl->Annotation.ptr = new TAnnotation;
        Decl->Annotation.ptr->FA.ptr = new TAnnotation;
        ((TAnnotation*)Decl->Annotation.ptr->FA.ptr)->BasicFlags.INCL(Toggle);
        GetSpace((TAnnotation**)&Decl->Annotation.ptr->FA.ptr, 0, 0, 0);
        litElL = NewCHE(Decl);
      }
      myWizard->FormDECL->Annotation.ptr->Suffixes.Prepend(litElR);
    }
    ((LavaDECL*)litElR->data)->LitStr = label;
  }
}


void CMenuPage::SetEProps() 
{
  DString *labe, *labpix, *labb;
  CHE *inDefEl;
  CHE*ininDefEl;
  LavaDECL *inEl;
  CHEEnumSelId * enumsel;
  TAnnotation* anno;
  CComboBoxItem *item;
  Q3ListBoxItem *pixItem;

  inEl = (LavaDECL*)myWizard->FormDECL->Annotation.ptr->MenuDECL.ptr;
  if (inEl) {
    anno = (TAnnotation*)inEl->Annotation.ptr->FA.ptr;
    myWizard->FormDECL->Annotation.ptr->FA.ptr = anno;
    inEl->Annotation.ptr->FA.ptr = 0;
    myWizard->FormDECL->Annotation.ptr->MenuDECL.Destroy();
  }
  else
    anno = (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  TAnnotation* enumAnno = ((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->EnumField.Annotation.ptr;
  if (!enumAnno) {
    enumAnno = new TAnnotation;
    ((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->EnumField.Annotation.ptr = enumAnno;
  }
  if (!(TAnnotation*)enumAnno->FA.ptr) 
    enumAnno->FA.ptr = new TAnnotation;
  enumAnno = (TAnnotation*)enumAnno->FA.ptr;
  if (anno->BasicFlags.Contains(ButtonMenu)) {
    if (enumAnno->IoSigFlags.Contains(Enter))
      v_Menutype = int(isBMenu);
    else
      v_Menutype = int(isRMenu);
  }
  else
    if (anno->BasicFlags.Contains(OptionMenu))
      v_Menutype = int(isOMenu);
    else
      if (anno->BasicFlags.Contains(PopUpMenu))
        v_Menutype = int(isPMenu);
      else
      v_Menutype = int(isNoMenu);
  inEl = &((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->EnumField;
  //the menu labels:
  inDefEl = (CHE*)((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->MenuTree.NestedDecls.first;
  enumsel = (CHEEnumSelId*)inEl->Items.first;
  unsigned *mflags;
  while (enumsel || inDefEl) {   //Menuprops zu actElem fuellen
    mflags = new unsigned[4];
    mflags[0] = 0;
    if (enumsel && (enumsel->data.SelectionCode.l == 0) 
      && !((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.Contains(hasPixmap))
      mflags[0] = 2;
    else {
      Q_ASSERT (inDefEl);
      if (((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.Contains(BlankSelCode))
        mflags[0] = 1;
      if (((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.Contains(MenuText))
        mflags[0] = 3;
    }
    mflags[1] = 0;
    mflags[2] = 0;
    mflags[3] = 0;
    if ((mflags[0] != 1) && (mflags[0] != 2))
      SetSpace(((LavaDECL*)inDefEl->data)->Annotation.ptr, mflags[1], mflags[2], mflags[3] );
    labe = new DString(50);
    labb = new DString(50);
    labpix = new DString(250);
    labe->Reset(0);
    labb->Reset(0);
    labpix->Reset(0);
    if (mflags[0] != 3) {
      Q_ASSERT (enumsel);
      *labe = enumsel->data.Id;
//      *labs = enumsel->data.SelectionCode;
      enumsel = (CHEEnumSelId*)enumsel->successor;
    }
    if (mflags[0] != 2) {  //not -FIO, 0,1,3 
      Q_ASSERT(inDefEl);
      ininDefEl = (CHE*)((LavaDECL*)inDefEl->data)->Annotation.ptr->Prefixes.first;
      if (mflags[0] == 0) {
        Q_ASSERT(ininDefEl->successor);  //0
        ininDefEl = (CHE*)ininDefEl->successor;
      }
      *labb = ((LavaDECL*)ininDefEl->data)->LitStr; //0,3
      CHETAnnoEx* ex = ((LavaDECL*)ininDefEl->data)->Annotation.ptr->GetAnnoEx(anno_Pixmap);
      if (ex && ex->data.xpmFile.l)
        *labpix = ex->data.xpmFile;
      inDefEl = (CHE*)inDefEl->successor;
    }
    item = new CComboBoxItem(*labe, mflags);
    m_Menuitems->insertItem(item);
    item = new CComboBoxItem(*labb, mflags);
    m_LButtonText->insertItem(item);
    pixItem = new CComboBoxItem(*labpix, mflags);
    m_Pixmap->insertItem(pixItem);
    delete labe;
    delete labb;
    delete labpix;
  //  delete [] mflags;
  }
}//SetProps

void CMenuPage::GetEProps()
{
  CHE *inDefEl;
  CHE *ininDefEl;
  LavaDECL *inEl;
  CHEEnumSelId * enumsel=0;
  unsigned *mflags; 
  int ipos = 0, maxi;
  QString qs;

  TAnnotation* anno = (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  anno->BasicFlags.EXCL(ButtonMenu);
  anno->BasicFlags.EXCL(OptionMenu);
  anno->BasicFlags.EXCL(PopUpMenu);
  TAnnotation* enumAnno = (TAnnotation*)((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->EnumField.Annotation.ptr->FA.ptr;
  enumAnno->IoSigFlags.INCL(Enter);
  switch ((EMenuType)v_Menutype) { 
    case isBMenu:
      anno->BasicFlags.INCL(ButtonMenu);
      break;
    case isRMenu:
      anno->BasicFlags.INCL(ButtonMenu);
      anno->BasicFlags.INCL(RadioButtons);
      //anno->BasicFlags.INCL(Groupbox);
      enumAnno->IoSigFlags.EXCL(Enter);
      break;
    case isOMenu:
      anno->BasicFlags.INCL(OptionMenu);
      break;
    case isPMenu:
      anno->BasicFlags.INCL(PopUpMenu);
      break;
    default: ;
  }
  inEl = &((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->EnumField;
//  inEl->Items.Destroy();
  ((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->MenuTree.NestedDecls.Destroy();
  maxi = QMAX(m_Menuitems->count(), m_LButtonText->count());
  while (ipos < maxi) {
   mflags = ((CComboBoxItem*)m_Menuitems->item(ipos))->flags();
    if (mflags[0] != 3) { //not static MenuText
      if (!enumsel)
        enumsel = (CHEEnumSelId*)inEl->Items.first;
      else
        enumsel = (CHEEnumSelId*)enumsel->successor;
    } 
    if (mflags[0] != 2) {  //not -FIO
      LavaDECL* Decl = NewLavaDECL();
      Decl->Annotation.ptr = new TAnnotation;
      Decl->Annotation.ptr->FA.ptr = new TAnnotation;
      if (mflags[0] != 3)  //not static MenuText
        Decl->LocalName = enumsel->data.Id;
      inDefEl = NewCHE(Decl);
      ((LavaDECL*)inDefEl->data)->DeclType = Attr;
      ((LavaDECL*)inDefEl->data)->DeclDescType = Undefined;
      ((TEnumDescription*)myWizard->FormDECL->EnumDesc.ptr)->MenuTree.NestedDecls.Append(inDefEl);
      if (mflags[0] == 1)
        ((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.INCL(BlankSelCode);
      else {
        ((TAnnotation*)((LavaDECL*)inDefEl->data)->Annotation.ptr->FA.ptr)->BasicFlags.EXCL(BlankSelCode);
        LavaDECL* inDecl = NewLavaDECL();
        inDecl->Annotation.ptr = new TAnnotation;
        inDecl->Annotation.ptr->FA.ptr = new TAnnotation;
        ininDefEl = NewCHE(inDecl);
        ((LavaDECL*)ininDefEl->data)->DeclDescType = LiteralString;
        GetSpace(&((LavaDECL*)inDefEl->data)->Annotation.ptr, mflags[1], mflags[2], mflags[3] );
        if (mflags[0] == 0) {
          if ((EMenuType)v_Menutype != isOMenu) {
            qs = m_LButtonText->item(ipos)->text();
            enumsel->data.SelectionCode = DString(qs);
          }
          else {
            if (!enumsel->data.SelectionCode.l)
              enumsel->data.SelectionCode = enumsel->data.Id;
          }
          ((LavaDECL*)ininDefEl->data)->LitStr = enumsel->data.SelectionCode;
          ((LavaDECL*)inDefEl->data)->Annotation.ptr->Prefixes.Append(ininDefEl);
          LavaDECL* inDecl = NewLavaDECL();
          inDecl->Annotation.ptr = new TAnnotation;
          inDecl->Annotation.ptr->FA.ptr = new TAnnotation;
          inDecl->DeclDescType = LiteralString;
          ininDefEl = NewCHE(inDecl);
        }
        qs = m_LButtonText->item(ipos)->text();
        ((LavaDECL*)ininDefEl->data)->LitStr = DString(qs);
        if (m_Pixmap->item(ipos)->text().length()) {
          CHETAnnoEx* ex = ((LavaDECL*)ininDefEl->data)->Annotation.ptr->GetAnnoEx(anno_Pixmap, true);
          ex->data.xpmFile = DString(m_Pixmap->item(ipos)->text());
          ((LavaDECL*)ininDefEl->data)->Annotation.ptr->BasicFlags.INCL(hasPixmap);
          ((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.INCL(hasPixmap);
        }
        else {
          ((LavaDECL*)ininDefEl->data)->Annotation.ptr->DelAnnoEx(anno_Pixmap);
          ((LavaDECL*)ininDefEl->data)->Annotation.ptr->BasicFlags.EXCL(hasPixmap);
          ((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.EXCL(hasPixmap);
        }
        ((LavaDECL*)inDefEl->data)->Annotation.ptr->Prefixes.Append(ininDefEl);
        if (mflags[0] == 3)
          ((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.INCL(MenuText);
        else
          ((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.EXCL(MenuText);
      }
    }
    ipos = ipos+1;
    //delete [] mflags;
  }//while
}//GetProps


void CMenuPage::OnSelchangeMenuitems() 
{
  int ss = m_Menuitems->currentItem();
  m_LButtonText->setCurrentItem(ss);
  m_Pixmap->setCurrentItem(ss);
  SetButtons(ss);
}

void CMenuPage::OnSelchangeLButtonText() 
{
  int ss = m_LButtonText->currentItem();
  m_Menuitems->setCurrentItem(ss);
  m_Pixmap->setCurrentItem(ss);
  SetButtons(ss);
}

void CMenuPage::m_Pixmap_selectionChanged()
{
  int ss = m_LButtonText->currentItem();
  m_Menuitems->setCurrentItem(ss);
  m_LButtonText->setCurrentItem(ss);
  SetButtons(ss);
}


void CMenuPage::SetButtons(int sel)
{
  if (sel < 0) {
    m_DeleteButton->setEnabled(false);
    m_EditButton->setEnabled(false);
  }
  else {
    unsigned *mflags = ((CComboBoxItem*)m_Menuitems->item(sel))->flags();
    m_DeleteButton->setEnabled(mflags[0] == 3);
    m_EditButton->setEnabled(true);
  }
}


bool CMenuPage::OnSetActive() 
{
  m_LeftLabel->setEnabled(isBool);
  m_ToggleLabel->setEnabled(isBool);
  m_LButtonText->setEnabled(isEnumera);
  m_Menuitems->setEnabled(isEnumera);
  m_Menutype->setEnabled(isEnumera);
  m_EditButton->setEnabled(false);
  m_DeleteButton->setEnabled(false);
  m_AddButton->setEnabled(isEnumera && (v_Menutype != int(isOMenu)));
  if (isEnumera) {
    m_LButtonText->setCurrentItem(-1);
    m_Menuitems->setCurrentItem(-1);
  }
  return true;
}

void CMenuPage::OnAddButton() 
{
  int ss = m_Menuitems->currentItem();
  if (ss < 0)
    ss = 0;
  else
    ss += 1;
  unsigned *mflags = new unsigned[4];
  mflags[0] = 3;
  mflags[1] = 0;
  mflags[2] = 0;
  mflags[3] = 0;
  CMenuItem *MItem = new CMenuItem(this, true, ss, "", "", "", mflags);
  if (MItem->exec() == QDialog::Accepted) {
    m_LButtonText->setCurrentItem(ss);
    m_Menuitems->setCurrentItem(ss);
    unsigned *mflags1 = ((CComboBoxItem*)m_Menuitems->item(ss))->flags();
    m_DeleteButton->setEnabled(mflags1[0] == 3);
    m_EditButton->setEnabled(true);
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  } 
  delete MItem;
  //delete [] mflags;
}


void CMenuPage::OnEditButton()
{
  int ss = m_LButtonText->currentItem();

  if (ss < 0)
    return;
  QString iT, bT, pT;
  iT = m_Menuitems->text(ss);
  bT = m_LButtonText->text(ss);
  pT = m_Pixmap->text(ss);
  unsigned  *mflags;
  mflags = ((CComboBoxItem*)m_Menuitems->item(ss))->flags();
  CMenuItem* cm = new CMenuItem(this, false, ss, iT, bT, pT, mflags);   
  if (cm->exec() == QDialog::Accepted) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
  delete cm;
}


void CMenuPage::OnDeleteButton() 
{
  int ss = m_Menuitems->currentItem();
  if (ss >= 0) {
    unsigned *mflags = ((CComboBoxItem*)m_Menuitems->item(ss))->flags();
    if (mflags[0] == 3) {
      m_Menuitems->removeItem(ss);
      m_LButtonText->removeItem(ss);
      if (ss) 
        ss = ss-1;
      m_Menuitems->setCurrentItem(ss);
      mflags = ((CComboBoxItem*)m_Menuitems->item(ss))->flags();
      m_DeleteButton->setEnabled(mflags[0] == 3);
      m_EditButton->setEnabled(true);
      myWizard->setModified(true);
    }
  } 
}


void CMenuPage::OnLButtonDown(unsigned nFlags, QPoint point) 
{
  m_LButtonText->setCurrentItem(-1);
  m_Menuitems->setCurrentItem(-1);
  m_DeleteButton->setEnabled(false);
  m_EditButton->setEnabled(false);

}

void CMenuPage::OnSelendokMenutype(int current) 
{
  int oldType = v_Menutype; 
  v_Menutype = current;
  SetDefaults((EMenuType)v_Menutype, (EMenuType)oldType);
  m_AddButton->setEnabled(isEnumera && (v_Menutype != int(isOMenu)));
  myWizard->setModified(true);
}


void CMenuPage::SetDefaults(EMenuType newMenuT, EMenuType oldMenuT)
{
  if ((newMenuT != isNoMenu)  && (oldMenuT == isNoMenu)
       || (newMenuT == isOMenu) && (oldMenuT != isOMenu)) {
    int cc = m_Menuitems->count();
    int rr;
    CComboBoxItem* item;
    TID tid0;
    QString iT, bT, pT;
    for (int ss = cc-1; ss >= 0; ss--) {
      if (oldMenuT == isNoMenu) {
        iT = m_Menuitems->text(ss);
        rr = m_LButtonText->text(ss).length();
        if (rr == 0) 
          m_LButtonText->removeItem(ss);
        if ((rr == 0) || (rr < 0)) {
          item = new CComboBoxItem(iT, tid0);
          m_LButtonText->insertItem(item, ss-1);
        }
      }
      if ((newMenuT == isOMenu) && (m_Menuitems->text(ss).length() > 0)) {
        bT = m_LButtonText->text(ss);
        if ((rr == 0) || (rr < 0)) {
          item = new CComboBoxItem(bT, tid0);
          m_LButtonText->insertItem(item, ss-1);
        }
      }
    }//for
  }
}

void CMenuPage::m_ToggleLabel_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CMenuPage::m_LeftLabel_clicked()
{
  myWizard->setModified(true);
}


bool CMenuPage::OnApply() 
{
  UpdateData(true);
  if (isEnumera)
    GetEProps();
  else
    if (isBool)
      GetBProps();
  return true;  
}


void CMenuItem::UpdateData(bool getData)
{
  if (getData) {
    m_MenuFrmspace = m_MenuSPIN3->value();
    m_Menuspace = m_MenuSPIN1->value();
    m_Menutab = m_MenuSPIN2->value();
    if (BGroup_Style->find(0)->isOn())
      m_flags = 0;
    else if (BGroup_Style->find(1)->isOn())
      m_flags = 1;
    else if (BGroup_Style->find(2)->isOn())
      m_flags = 2;
    else
      m_flags = 3;
  }
  else {
    m_MenuSPIN3->setValue(m_MenuFrmspace);
    m_MenuSPIN1->setValue(m_Menuspace);
    m_MenuSPIN2->setValue(m_Menutab);
    BGroup_Style->setButton(m_flags);
  }
}

 
CMenuItem::CMenuItem(CMenuPage* mpage, bool isNew, int ipos,
                     QString enumsel, QString btext, QString stext,
                     unsigned *itemData)
  : Ui_IDD_Menuitem()
{
  mflags = itemData;

  m_flags = mflags[0];
  m_Menutab = mflags[1];
  m_Menuspace = mflags[2];
  m_MenuFrmspace = mflags[3];
  m_Enumsel->setText(enumsel);
  m_Enumsel->setEnabled(false);
  m_MenuText->setText(btext);
  m_Buttontext->setText(btext);
  if (stext.length())
    m_Pixmap->setText(stext);
  menuPage = mpage;
  insert = isNew;
  iPos = ipos;

  m_MenuSPIN1->setMinValue (0);
  m_MenuSPIN2->setMinValue (0);
  m_MenuSPIN3->setMinValue  (0);
  m_MenuSPIN1->setMaxValue (100);
  m_MenuSPIN2->setMaxValue (100);
  m_MenuSPIN3->setMaxValue  (100);
  EnableTextWindows();
  if (insert) {
    m_isButton->setEnabled(false);
    m_isMenuText->setEnabled(false);
    m_noButton->setEnabled(false);
    m_NoFIO->setEnabled(false);
  }
  else
    m_isMenuText->setEnabled(false);
  UpdateData(false);
}

void CMenuItem::EnableTextWindows()
{
  m_Buttontext->setEnabled(m_flags == 0);
  m_MenuText->setEnabled(m_flags == 3);
  m_Pixmap->setEnabled(menuPage->v_Menutype == isBMenu);
  /*
  if (m_flags != 0)
    m_Buttontext = QString("");
  if (m_flags != 3)
    v_MenuText = QString("");
    */
}


void CMenuItem::OnnoButton(bool) 
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::OnNoFIO(bool) 
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::OnisButton(bool) 
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::OnisMenuText(bool) 
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::button_browse_clicked()
{
  QDir dir(menuPage->myWizard->myDoc->IDTable.DocDir.c);
  QFileInfo qf(dir, m_Pixmap->text());
  //QString pix = QFileDialog::getOpenFileName( qf.absFilePath(), "Pixmap (*.xpm)",
  //                  this,"open file dialog", "Choose a pixmap file", 0, false );
  QString pix = L_GetOpenFileName(
                    qf.absFilePath(),
                    this,
                    "Choose a pixmap file",
                    "Pixmap (*.xpm)",
                    "xpm"
                    );
  if (pix.length()) {
    DString rpfn = DString(pix);
    RelPathName(rpfn,menuPage->myWizard->myDoc->IDTable.DocDir);
    m_Pixmap->setText(rpfn.c);
  }
}

void CMenuItem::OnOK() 
{
  CComboBoxItem *item;
  UpdateData(true);
  /*
  if (m_flags) {
    m_Buttontext = QString("");
  }
  */
  if (!insert) {
    menuPage->m_LButtonText->removeItem(iPos);
    menuPage->m_Menuitems->removeItem(iPos);
    menuPage->m_Pixmap->removeItem(iPos);
  }
  if (m_flags == 3) {
    item = new CComboBoxItem(m_MenuText->text(), mflags);
    menuPage->m_LButtonText->insertItem(item, iPos);
  }
  else {
    item = new CComboBoxItem(m_Buttontext->text(), mflags);
    menuPage->m_LButtonText->insertItem(item, iPos);
  }
  /*
  if (menuPage->v_Menutype == isBMenu) {
    item = new CComboBoxItem(m_Pixmap->text(), mflags);
    menuPage->m_Pixmap->insertItem(item, iPos);
  }
  */
  mflags[0] = m_flags;
  mflags[1] = m_Menutab ;
  mflags[2] = m_Menuspace ;
  mflags[3] = m_MenuFrmspace ;
  item = new CComboBoxItem(m_Enumsel->text(), mflags);
  menuPage->m_Menuitems->insertItem(item, iPos);
  item = new CComboBoxItem(m_Pixmap->text(), mflags);
  menuPage->m_Pixmap->insertItem(item, iPos);
  menuPage->UpdateData(false);
  QDialog::accept();
}


CSupportPage::~CSupportPage()
{
}
/*
void CSupportPage::UpdateData(bool getData)
{
  if (getData) {
    //v_Name = m_Name->text();
    v_FormClass = m_FormsClass->text();
  }
  else {
    //m_Name->setText(v_Name);
    m_FormsClass->setText(v_FormClass);

  }
}
*/

CSupportPage::CSupportPage(CLavaPEWizard* wizard)
 : Ui_IDD_SupportPage()
{
  int count = 0;
  CComboBoxItem *item;

  //v_FormClass = "";

  myWizard = wizard;
  m_Name->setText(myWizard->FormDECL->LocalName.c);
  //v_Name = QString(myWizard->FormDECL->LocalName.c);
  LavaDECL *classDecl, *fclassDECL/*, *baseClassDECL*/, *basefDECL;
  CHETID *cheS;
  TIDTable *IDTab = &myWizard->myDoc->IDTable;
  classDecl = myWizard->myDoc->IDTable.GetDECL(myWizard->FormDECL->RefID);
  if (classDecl) {
    //v_FormClass = QString(classDecl->LocalName.c);
    m_FormsClass->setText(classDecl->LocalName.c);
    cheS = (CHETID*)myWizard->FormDECL->ParentDECL->Supports.first;
    fclassDECL = IDTab->GetDECL(cheS->data);
    if (fclassDECL) {
      cheS = (CHETID*)fclassDECL->Supports.first;
      while (cheS) {//!!
        basefDECL = IDTab->GetFinalDef(cheS->data, fclassDECL->inINCL);
        if (basefDECL && basefDECL->TypeFlags.Contains(isGUI)) {
          if (IDTab->IsAnc(myWizard->FormDECL->RefID, 0, basefDECL->RefID, basefDECL->inINCL)) {
            item = new CComboBoxItem(basefDECL->LocalName, TID(basefDECL->OwnID, basefDECL->inINCL));
            m_Supports->insertItem(item);
          }
        }
        cheS = (CHETID*)cheS->successor;
      }
    }
  }
  //UpdateData(false);
}


/*
bool CSupportPage::OnApply() 
{
  UpdateData(true);
  ListToChain(m_Supports, &myWizard->FormDECL->Supports);
  return true;
}
*/

  
CExecLike::CExecLike(CLavaPEDoc* doc, QComboBox* list, LavaDECL * Ldecl, LavaDECL* form)
{
  myDoc = doc;
  myDECL = Ldecl; //class or form
  if (myDECL->DeclType == FormDef) 
    ContextDECL = myDoc->IDTable.GetDECL(myDECL->RefID, myDECL->inINCL);
  else
    ContextDECL = myDECL;
  List = list;
  FormDECL = form;
  TabTravers = new CTabTraversal(this, myDoc->mySynDef);
  TabTravers->Run(true, true);
  SortCombo(List);

}

bool CExecLike::CheckSupports(LavaDECL *basedecl, LavaDECL *FormDECL)
{
  CHETID* cheID = (CHETID*)FormDECL->ParentDECL->Supports.first;
  LavaDECL* base = myDoc->IDTable.GetDECL(cheID->data, FormDECL->inINCL); //Interface
  if (!base)
    return false;
  cheID = (CHETID*)base->Supports.first; //!
  while (cheID) {
    base = myDoc->IDTable.GetFinalDef(cheID->data, FormDECL->inINCL);
    if (base->TypeFlags.Contains(isGUI)) 
      return  myDoc->IDTable.EQEQ(basedecl->RefID, basedecl->inINCL, base->RefID, base->inINCL);
    cheID = (CHETID*)cheID->successor;
  }
  return false;
}


void CExecLike::ExecFormDef(LavaDECL ** pelDef, int incl)
{
  if (myDoc->IDTable.EQEQ((*pelDef)->RefID, (*pelDef)->inINCL, myDECL->RefID, myDECL->inINCL)) {
    if (FormDECL && !CheckSupports(*pelDef, FormDECL)) 
      return;
  }
  else
    if (!myDoc->IDTable.EQEQ((*pelDef)->RefID, (*pelDef)->inINCL, TID(myDECL->OwnID,myDECL->inINCL), 0))
      return;
  CComboBoxItem *item = new CComboBoxItem((*pelDef)->ParentDECL->FullName, TID((*pelDef)->OwnID, incl));
  List->listBox()->insertItem(item); //sort
}

void CExecLike::ExecDefs (LavaDECL ** pelDef, int incl)
{
  LavaDECL *elDef = *pelDef;

  if (!elDef->usableIn(myDECL->ParentDECL))
      return;
  if ((elDef->DeclType == Interface) && myDoc->TypeForMem(myDECL, elDef, 0))
    return;
  TID id = TID(elDef->OwnID, 0);
  if ( (elDef == myDECL)
       || (myDECL->DeclType == FormDef)
          &&  myDoc->IDTable.EQEQ(id, elDef->inINCL, myDECL->RefID, myDECL->inINCL)) {
    CComboBoxItem *item = new CComboBoxItem(elDef->FullName, TID(elDef->OwnID, incl));
    List->listBox()->insertItem(item);
  }
}
