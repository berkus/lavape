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
#include "qbuttongroup.h"
#include "qmessagebox.h"
#include "qtabbar.h"
#include "qsizepolicy.h"
#include "qfontdialog.h"
#include "qcolordialog.h"
#include "qfiledialog.h"
//Added by qt3to4:
#include <QFrame>

#pragma hdrstop


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
  QWidget* qvbox = new QWidget(this);
  layout->addWidget(qvbox);
  QVBoxLayout* qvl = new QVBoxLayout(qvbox);
  QWidget* qhbox = new QWidget(qvbox);
  QHBoxLayout* qhl = new QHBoxLayout(qhbox);
  resetButton = new QPushButton("Reset", qhbox);
  applyButton = new QPushButton("Apply", qhbox);
  helpButton = new QPushButton("Help", qhbox);
  qhl->addWidget(resetButton);
  qhl->addWidget(applyButton);
  qhl->addWidget(helpButton);
  myScrv = new GUIScrollView(this, false);
  qvl->addWidget(qhbox);
  qvl->addWidget(myScrv);
  guibox = myScrv->qvbox;
  connect (applyButton,SIGNAL(clicked()), this, SLOT(Apply()));
  connect (resetButton,SIGNAL(clicked()), this, SLOT(Reset()));
  connect (helpButton,SIGNAL(clicked()), this, SLOT(Help()));
  wizard = 0;
  postedPage = 0;
  delayedID = 0;
  myTree = (CLavaPEView*)((CFormFrame*)GetParentFrame())->viewR;
}

void CWizardView::SetFont(const QFont& font)
{
  CLavaBaseView::setFont(font);
  int pageI = 0;
  if (wizard && !wizard->isActive) {
    int x = myScrv->horizontalScrollBar()->value();
    int y = myScrv->verticalScrollBar()->value();
    wizard->UpdateData();
    LavaDECL** synEl = wizard->synEl;
    LavaDECL* FormDECL = wizard->FormDECL;
    bool mod = wizard->modified;
    wizard->FormDECL = 0;
    pageI = wizard->currentIndex();
    delete wizard;
    wizard = new CLavaPEWizard(synEl, FormDECL, this, guibox);
    wizard->setWindowFlags(wizard->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    wizard->setFont(font);
    wizard->setModified(mod);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
    myScrv->horizontalScrollBar()->setValue(x);
    myScrv->verticalScrollBar()->setValue(y);
    //myScrv->setContentsPos(x,y);
    if (pageI < wizard->count())
      wizard->setCurrentIndex(pageI);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
  }
}


void CWizardView::customEvent(QEvent *ev0)
{
  CustomEvent *ev=(CustomEvent*)ev0;

  if (ev->type() == UEV_LavaPE_CalledView) {
    CLavaPEHint* hint = (CLavaPEHint*)ev->data();
    ((CLavaPEDoc*)hint->fromDoc)->UndoMem.AddToMem(hint);
    ((CLavaPEDoc*)hint->fromDoc)->UpdateDoc(((CLavaPEDoc*)hint->fromDoc)->MainView, false, hint);
    ((CLavaPEDoc*)hint->fromDoc)->SetLastHint();
  }
  if(delayedID) {
    TIDType itype;
    LavaDECL** synEl = (LavaDECL**)((CLavaPEDoc*)GetDocument())->IDTable.GetVar(TID(delayedID,0), itype);
    CTreeItem* item = myTree->BrowseTree(*synEl, (CTreeItem*)myTree->Tree->RootItem);
    if (item)
      myTree->Tree->setCurAndSel(item);
    delayedID = 0;
    if (!wizard) {
      wizard = new CLavaPEWizard(synEl, 0, this, guibox);
      wizard->setWindowFlags(wizard->windowFlags() ^ Qt::WindowContextHelpButtonHint);
      myScrv->MaxBottomRight = wizard->rect();
      Resize();
      myScrv->horizontalScrollBar()->setValue(0);
      myScrv->verticalScrollBar()->setValue(0);
//      myScrv->setContentsPos(0,0);
      if (postedPage < wizard->count())
        wizard->setCurrentIndex(postedPage);
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
        pageI = wizard->currentIndex();
      if (wizard->modified) {
          CLavaPEHint* hint = wizard->ApplyHint();
          postedPage = pageI;
          LavaDECL* decl = (LavaDECL*)((CLavaPEHint*)pHint)->CommandData1;
          delayed = wxDocManager::GetDocumentManager()->GetActiveView() == myTree;
          if (delayed)
            delayedID = decl->OwnID;
          wizard->FormDECL = 0;
          QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_CalledView,(void*)hint));
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
    myScrv->horizontalScrollBar()->setValue(0);
    myScrv->verticalScrollBar()->setValue(0);
//    myScrv->setContentsPos(0,0);
    if (pageI < wizard->count())
      wizard->setCurrentIndex(pageI);

  }
}

void CWizardView::PostApplyHint()
{
  postedPage = wizard->currentIndex();
  CLavaPEHint* hint = wizard->ApplyHint();
  wizard->FormDECL = 0;
  wizard->modified = false;
  QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_CalledView,(void*)hint));
}

void CWizardView::Resize()
{
  QSize size = myScrv->MaxBottomRight.size();
  ((GUIScrollView*)myScrv)->qvbox->resize(size);
//  myScrv->resizeContents(size.width(),size.height());
}


void CWizardView::Apply()
{
  wizard->OnApply();
  Reset();
}


void CWizardView::Help()
{
	QString path(ExeDir+"/assistant");
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
    pageI = wizard->currentIndex();
    delete wizard;
    wizard = new CLavaPEWizard(synEl, 0, this, guibox);
    wizard->setWindowFlags(wizard->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    myScrv->MaxBottomRight = wizard->rect();
    Resize();
    myScrv->horizontalScrollBar()->setValue(0);
    myScrv->verticalScrollBar()->setValue(0);
//    myScrv->setContentsPos(0,0);
    if (pageI < wizard->count())
      wizard->setCurrentIndex(pageI);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CLavaPEWizard


CLavaPEWizard::CLavaPEWizard(LavaDECL ** p_origDECL, LavaDECL* formDECL, CWizardView* view, QWidget* parent)
  :QTabWidget(parent)
{
  setObjectName("LavaPEWizard");
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
  connect (this,SIGNAL(currentChanged(int)), this, SLOT(OnSetActive(int)));
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

  return new CLavaPEHint(CPECommand_Change, myDoc, (const unsigned long)1, FormDECL, name, 0, synEl);
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
    //FormTextPage->setMinimumSize(FormTextPage->size());
    addTab(FormTextPage, "Formtext");
  }
  else {
    if (FormDECL->DeclType == FormDef) {
      if (FormDECL->Supports.first) {
        SupportPage = new CSupportPage(this);
        //SupportPage->setMinimumSize(SupportPage->size());
        addTab(SupportPage, "General");
      }
    }
    else {
      if (FormDECL->DeclType != VirtualType) {
  //      if (FormDECL->DeclDescType == NamedType) {
        GenPage = new CGeneralPage(this);
        //GenPage->setMinimumSize(GenPage->size());
        addTab(GenPage, "General");
      }
    }
    if ((FormDECL->DeclDescType != BasicType)
        || (FormDECL->BType == B_Bool)   || (FormDECL->BType == Char)
        || (FormDECL->BType ==  Integer) || (FormDECL->BType ==  Float)
        || (FormDECL->BType ==  Double)  || (FormDECL->BType ==  VLString)) {
      IOPage = new CIOPage(this, FormDECL, false);
      sz =IOPage->size();
      //IOPage->setMinimumSize(sz);
      addTab(IOPage, "IO+Layout");
      if ((FormDECL->DeclType == Attr) || (FormDECL->DeclType == VirtualType)) {
        LitPage = new CLiteralsPage(this, FormDECL, false);
        sz = LitPage->size();
        //LitPage->setMinimumSize(sz);
        addTab(LitPage, "Content");
      }
      FontColorPage = new CFontColorPage(this, FormDECL);
      sz =FontColorPage->size();
      //FontColorPage->setMinimumSize(sz);
      addTab(FontColorPage, "Font+Color");
      if (FormDECL->DeclDescType == EnumType) {
        MenuPage = new CMenuPage(this);
        //MenuPage->setMinimumSize(MenuPage->size());
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
  //ChainIOPage->setMinimumSize(ChainIOPage->size());
  addTab(ChainIOPage, "Chain/array element - IO*Layout");
  ChainLitPage = new CLiteralsPage(this, chainEl, true);
  //ChainLitPage->setMinimumSize(ChainLitPage->size());
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
  if (GenPage)
    ResetComboItems(GenPage->NamedTypes);
  if (MenuPage)
    ResetComboItems(MenuPage->Menutype);
  if (LitPage) {
    ResetComboItems(LitPage->BoolDefault);
    ResetComboItems(LitPage->EnumDefault);
  }
  if (ChFormPage)
    ResetComboItems(ChFormPage->ChElemFormTypes);
  if (FormDECL)
    delete FormDECL;
}


void CLavaPEWizard::OnSetActive(int i)
{
  if (LitPage == widget(i))
    LitPage->OnSetActive();
  if (MenuPage == widget(i))
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
    m_ChFormElemStr = ChFormElem->text();
    m_InsText = EditInsertButton->text();
    m_DelText = EDITDelButton->text();
    //m_Name = NewName2->text();
    m_Len = LengthSPIN->value();
  }
  else {
    modify = false;
    ChFormElem->setText(m_ChFormElemStr) ;
    EditInsertButton->setText(m_InsText);
    EDITDelButton->setText(m_DelText);
    //NewName2->setText(m_Name);
    LengthSPIN->setValue(m_Len);
    modify = true;
  }
}

CChainFormPage::CChainFormPage(CLavaPEWizard* wizard)
 : Ui_IDD_ChainFormPage()
{
  LavaDECL *formDECL, *classDECL;
  TAnnotation* anno;
  bool noElli, isAr;

  setupUi(this);
  modify = true;
  m_ElemSel = "";
  m_ChFormElemStr = "";
  m_DelText = "";
  m_InsText = "";
  myWizard = wizard;
  m_Len = 1;
  NewName->setText(myWizard->VFormDECL->LocalName.c);
  //m_Name = QString(myWizard->VFormDECL->LocalName.c);
  if (!myWizard->FormDECL->Annotation.ptr)
    myWizard->FormDECL->Annotation.ptr = new TAnnotation;
  anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  isAr = myWizard->VFormDECL->SecondTFlags.Contains(isArray);
  if (isAr) {
		//m_ArrayLen->setEnabled(true);
		LengthSPIN->setEnabled(true);
    LengthSPIN->setMinimum (1);
    LengthSPIN->setMaximum (10000);
    m_Len = myWizard->FormDECL->Annotation.ptr->Length.DecPoint;
  }
  if (anno->IterFlags.Contains(FixedCount))
    anno->IterFlags.INCL(NoEllipsis);
  noElli = anno->IterFlags.Contains(NoEllipsis) || isAr;
  if (!noElli)
    anno->IterFlags.EXCL(FixedCount);
  ConstChain->setChecked(anno->IterFlags.Contains(FixedCount));
  HasButtons->setChecked(!noElli);
  EDITDelButton->setEnabled(!noElli);
  EditInsertButton->setEnabled(!noElli);
  if (anno && !noElli) {
    if (anno->String1.l)
      m_InsText = QString(anno->String1.c);
    if (anno->String2.l)
      m_DelText = QString(anno->String2.c);
  }
  HasButtons->setEnabled(!anno->IterFlags.Contains(FixedCount) && !isAr);
  ConstChain->setEnabled(noElli && !isAr);
  if (myWizard->FormDECL->Annotation.ptr->IterOrig.ptr
    && ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr)
    CHEEl = ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
  else
    return;
  classDECL = myWizard->myDoc->IDTable.GetDECL(((CHETID*)myWizard->FormDECL->Supports.first)->data, myWizard->FormDECL->inINCL);
  QString* err = myWizard->myDoc->CheckFormEl(myWizard->FormDECL, classDECL);
  if (err && (err != &ERR_InvisibleType)) {
    formDECL = classDECL;
    ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.Destroy();
    CHEEl = NewLavaDECL();
    ((TIteration*)myWizard->FormDECL->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr = CHEEl;
    CHEEl->DeclDescType = classDECL->DeclDescType;
    CHEEl->BType = classDECL->BType;
    CHEEl->SecondTFlags = classDECL->SecondTFlags;
    CHEEl->DeclType = PatternDef; //not VirtualType
    myWizard->FormDECL->DeclDescType = classDECL->DeclDescType;
    myWizard->FormDECL->BType = classDECL->BType;
    myWizard->FormDECL->DeclType = classDECL->DeclType;
    CHEEl->RefID = TID(classDECL->RefID.nID, myWizard->myDoc->IDTable.IDTab[classDECL->inINCL]->nINCLTrans[classDECL->RefID.nINCL].nINCL);
    CHEEl->Annotation.ptr = new TAnnotation;
    CHEEl->Annotation.ptr->FA.ptr = new TAnnotation;
    CHEEl->Annotation.ptr->FrameSpacing = 1;
  }
  for (formDECL = CHEEl;
       formDECL && (formDECL->DeclDescType != BasicType)
            && ((formDECL->DeclType == VirtualType) || (formDECL->DeclType == PatternDef));
       formDECL = myWizard->myDoc->IDTable.GetDECL(formDECL->RefID, formDECL->inINCL));
  if (formDECL) {
    if (formDECL->DeclDescType == BasicType) {
      CHEEl->DeclDescType = BasicType;
      CHEEl->BType = formDECL->BType;
      CHEEl->RefID = formDECL->RefID;
      m_ChFormElemStr = QString(myWizard->myDoc->GetTypeLabel(CHEEl, false).c);
    }
    else {
      if (formDECL->DeclType == FormDef)
        m_ChFormElemStr = QString(formDECL->ParentDECL->LocalName.c);
      else
          m_ChFormElemStr = QString(formDECL->LocalName.c);
      CExecLike * execLike = new CExecLike(myWizard->myDoc, ChElemFormTypes, formDECL);
      delete execLike;
    }
  }
  //setMinimumSize(size());
  myWizard->addTab(this, "Chain/array element");

  myWizard->AddChainPages(CHEEl);
  UpdateData(false);

}


void CChainFormPage::on_ChElemFormTypes_activated(int)
{
  UpdateData(true);
  if (SelEndOKToStr(ChElemFormTypes, &m_ChFormElemStr, &CHEEl->RefID) > 0) {
    CHEEl->DeclDescType = NamedType;
    UpdateData(false);
    myWizard->setModified(true);
  }
}

void CChainFormPage::on_ConstChain_clicked()
{
  TAnnotation* anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  if (ConstChain->isChecked()) {
    anno->IterFlags.INCL(FixedCount);
    anno->IterFlags.INCL(NoEllipsis);
  }
  else
    anno->IterFlags.EXCL(FixedCount);
  HasButtons->setChecked(!anno->IterFlags.Contains(NoEllipsis));
  HasButtons->setEnabled(!anno->IterFlags.Contains(FixedCount));
  EDITDelButton->setEnabled(!anno->IterFlags.Contains(NoEllipsis));
  EditInsertButton->setEnabled(!anno->IterFlags.Contains(NoEllipsis));
  myWizard->setModified(true);

}

void CChainFormPage::on_HasButtons_clicked()
{
  TAnnotation* anno =  (TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr;
  bool has = (HasButtons->isChecked());
  EDITDelButton->setEnabled(has);
  EditInsertButton->setEnabled(has);
  if (has) {
    anno->IterFlags.EXCL(NoEllipsis);
    anno->IterFlags.EXCL(FixedCount);
  }
  else
    anno->IterFlags.INCL(NoEllipsis);
  ConstChain->setChecked(anno->IterFlags.Contains(FixedCount));
  ConstChain->setEnabled(anno->IterFlags.Contains(NoEllipsis));
  myWizard->setModified(true);

}


void CChainFormPage::on_EditInsertButton_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CChainFormPage::on_EDITDelButton_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CChainFormPage::on_DefaultLength_valueChanged( int )
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
      anno->String1 = STRING(qPrintable(m_InsText));
    if (dl)
      anno->String2 = STRING(qPrintable(m_DelText));
  }
  if (myWizard->VFormDECL->SecondTFlags.Contains(isArray))
    myWizard->FormDECL->Annotation.ptr->Length.DecPoint = m_Len;
  myWizard->FormDECL->RefID = CHEEl->RefID;
  return true;
}

void CFormTextPage::UpdateData(bool getData)

{
  if (getData) {
    m_LiSpace = HSpaceSPIN->value();
    m_LiTab = HTabSPIN->value();
    m_LiFrmSpace = VSpaceSPIN->value();
    m_lit = EditLiteral->text();
  }
  else {
    modify = false;
    HSpaceSPIN->setValue(m_LiSpace);
    HTabSPIN->setValue(m_LiTab);
    VSpaceSPIN->setValue(m_LiFrmSpace);
    EditLiteral->setText(m_lit);
    modify = true;
  }
}


CFormTextPage::CFormTextPage(LavaDECL * litEl, CLavaPEWizard *wizard)
  : Ui_FormtextPage()
{
  setupUi(this);
  myWizard = wizard;
  m_lit = "";
  myDecl = litEl;
  modify = false;
  HSpaceSPIN->setMinimum(0);
  HSpaceSPIN->setMaximum(100);
  HTabSPIN->setMinimum(0);
  HTabSPIN->setMaximum(100);
  VSpaceSPIN->setMinimum(0);
  VSpaceSPIN->setMaximum(100);
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
    beforeBase->setChecked(false);
    beforeBase->setEnabled(false);
  }
  else {
    beforeBase->setChecked(myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
    beforeBase->setEnabled(true);
  }
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l && lf.fromString(myDecl->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
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


void CFormTextPage::on_defaultFont_clicked()
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

void CFormTextPage::on_fontButton_clicked()
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
    myDecl->Annotation.ptr->String1 = STRING(qPrintable(lf.toString()));
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
    fontName->setText(nn);
    fontName->show();
    myWizard->setModified(modify);
    if (modify)
      myWizard->myView->PostApplyHint();
  }
}


void CFormTextPage::on_defaultForeground_clicked()
{
  if (colorSetting.on_defaultForeground_clicked())
    myWizard->setModified(modify);

}

void CFormTextPage::on_defaultBackground_clicked()
{
  if (colorSetting.on_defaultBackground_clicked())
    myWizard->setModified(modify);
}

void CFormTextPage::on_colorButtonF_clicked()
{
  if (colorSetting.on_colorButtonF_clicked() && modify) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFormTextPage::on_colorButtonB_clicked()
{
  if (colorSetting.on_colorButtonB_clicked() && modify) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}



void CFormTextPage::on_EditLiteral_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CFormTextPage::on_HorizTab_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::on_HorizSpace_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::on_VertSpace_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CFormTextPage::OnApply()
{
  UpdateData(true);
  if (!myDecl->LocalName.l)
    myDecl->LocalName = DString("_Text");
  myDecl->LitStr = STRING(qPrintable(m_lit));
  GetSpace(&myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  if (beforeBase->isChecked())
    myDecl->Annotation.ptr->BasicFlags.INCL(beforeBaseType);
  else
    myDecl->Annotation.ptr->BasicFlags.EXCL(beforeBaseType);
}



void CFormTextBox::UpdateData(bool getData)

{
  if (getData) {
    m_LiSpace = HSpaceSPIN->value();
    m_LiTab = HTabSPIN->value();
    m_LiFrmSpace = VSpaceSPIN->value();
    m_lit = EditLiteral->text();
  }
  else {
    HSpaceSPIN->setValue(m_LiSpace);
    HTabSPIN->setValue(m_LiTab);
    VSpaceSPIN->setValue(m_LiFrmSpace);
    EditLiteral->setText(m_lit);
  }
}


CFormTextBox::CFormTextBox(LavaDECL * litEl, QWidget* pParent, bool asFirst)
  : Ui_IDD_LiteralItem()
{
  setupUi(this);
  m_lit = "";
  myDecl = litEl;
  HSpaceSPIN->setMinimum(0);
  HSpaceSPIN->setMaximum(100);
  HTabSPIN->setMinimum(0);
  HTabSPIN->setMaximum(100);
  VSpaceSPIN->setMinimum(0);
  VSpaceSPIN->setMaximum(100);
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
    beforeBase->setChecked(false);
    beforeBase->setEnabled(false);
  }
  else {
    beforeBase->setChecked(myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
    beforeBase->setEnabled(true);
  }
  beforeBase->setChecked(asFirst && myDecl->Supports.first && myDecl->Annotation.ptr->BasicFlags.Contains(beforeBaseType));
  UpdateData(false);
}


void CFormTextBox::on_ID_OK_clicked()
{
  UpdateData(true);
  if (!myDecl->LocalName.l)
    myDecl->LocalName = DString("_Text");
  myDecl->LitStr = STRING(qPrintable(m_lit));
  GetSpace(&myDecl->Annotation.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  if (beforeBase->isChecked())
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
    //m_NewName = NewName6->text();
    m_NewType = NewTypeType->text();
  }
  else {
    //NewName6->setText(m_NewName);
    NewTypeType->setText(m_NewType);

  }
}

CGeneralPage::CGeneralPage(CLavaPEWizard *wizard)
: Ui_IDD_GeneralPage()
{
  DString dstr;
  LavaDECL *classDECL, *decl=0;

  setupUi(this);
  myWizard = wizard;
  //m_NewName = QString(myWizard->FormDECL->LocalName.c);
  NewName->setText(myWizard->FormDECL->LocalName.c);
  dstr = myWizard->myDoc->GetTypeLabel(myWizard->FormDECL, false);
  m_NewType = QString(dstr.c);
  classDECL = myWizard->myDoc->IDTable.GetDECL(((CHETID*)myWizard->FormDECL->Supports.first)->data, myWizard->FormDECL->inINCL);
  if (classDECL)
    decl = myWizard->myDoc->IDTable.GetDECL(classDECL->RefID, classDECL->inINCL);

  if (decl) {
    for (; decl && (decl->DeclType == VirtualType); decl = myWizard->myDoc->IDTable.GetDECL(decl->RefID, decl->inINCL));
    CExecLike * execLike = new CExecLike(myWizard->myDoc, NamedTypes, decl);
    delete execLike;
  }
  int listItem = NamedTypes->findText(m_NewType);
  if (listItem)
    NamedTypes->setItemText(NamedTypes->currentIndex(),NamedTypes->itemText(listItem));

  UpdateData(false);
}

void CGeneralPage::on_NamedTypes_activated(int)
{
  UpdateData(true);
  if (SelEndOKToStr(NamedTypes, &m_NewType, &myWizard->FormDECL->RefID) > 0) {
    myWizard->FormDECL->DeclDescType = NamedType;
    if (myWizard->myDoc->IDTable.GetDECL(myWizard->FormDECL->RefID)->DeclType == FormDef) {
      ((TAnnotation*)myWizard->FormDECL->Annotation.ptr->FA.ptr)->BasicFlags.INCL(Groupbox);
      if (myWizard->IOPage)
        myWizard->IOPage->groupbox->setChecked(true);
    }
    UpdateData(false);
    myWizard->setModified(true);
  }
}

void ColorSetting::Init(AnnoExType role, LavaDECL* decl, QCheckBox* defaultB,
                               QCheckBox* defaultF,
                               QPushButton* ButtonB,
                               QPushButton* ButtonF,
                               QFrame* Fore,
                               QFrame* Back)
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
    QPalette p = BackColor->palette();
    p.setColor(QPalette::Active, QPalette::Window, BColor);
    p.setColor(QPalette::Inactive, QPalette::Window, BColor);
    BackColor->setPalette(p);
    //BackColor->setPaletteBackgroundColor(BColor);
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
    QPalette p = ForeColor->palette();
    p.setColor(QPalette::Active, QPalette::Window, FColor);
    p.setColor(QPalette::Inactive, QPalette::Window, FColor);
    p.setColor(QPalette::Disabled, QPalette::Window, FColor);
    ForeColor->setPalette(p);
//    ForeColor->setPaletteBackgroundColor(FColor);
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


bool ColorSetting::on_defaultForeground_clicked()
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

bool ColorSetting::on_defaultBackground_clicked()
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

bool ColorSetting::on_colorButtonF_clicked()
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
    QPalette p = ForeColor->palette();
    p.setColor(QPalette::Active, QPalette::Window, BColor);
    p.setColor(QPalette::Inactive, QPalette::Window, BColor);
    ForeColor->setPalette(p);
//    ForeColor->setPaletteBackgroundColor(FColor);
    return true;
  }
  return false;
}

bool ColorSetting::on_colorButtonB_clicked()
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
    QPalette p = BackColor->palette();
    p.setColor(QPalette::Active, QPalette::Window, BColor);
    p.setColor(QPalette::Inactive, QPalette::Window, BColor);
    BackColor->setPalette(p);
//    BackColor->setPaletteBackgroundColor(BColor);
    return true;
  }
  return false;

}


CFontColorPage::CFontColorPage(CLavaPEWizard *wizard, LavaDECL *formDECL)
: Ui_idd_fontcolorpage()
{
  setupUi(this);
  myWizard = wizard;
  FormDECL = formDECL;
  QFont lf;
  if (FormDECL->Annotation.ptr->String1.l && lf.fromString(FormDECL->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
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
    QString nn = lf.family() + QString(komma.c) +	QString::number(lf.pointSizeF());
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
    TextColorSetting.Init(anno_TextColor, FormDECL, defaultTBackground,
      defaultTForeground, TColorButtonB, TColorButtonF, ForeTColor, BackTColor);
    PBColorSetting.Init(anno_PBColor, FormDECL, defaultPBBackground,
      defaultPBForeground, PBColorButtonB, PBColorButtonF, ForePBColor, BackPBColor);
  }
  else {
    groupBoxT->hide();
    groupBoxPB->hide();
  }
  update();
}

CFontColorPage::~CFontColorPage()
{

}

bool CFontColorPage::OnApply()
{
  return true;
}


void CFontColorPage::on_defaultFontL_clicked()
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

void CFontColorPage::on_fontButtonL_clicked()
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
    FormDECL->Annotation.ptr->String1 = STRING(qPrintable(lf.toString()));
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
    fontNameL->setText(nn);
    fontNameL->show();
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::on_defaultFontT_clicked()
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

void CFontColorPage::on_fontButtonT_clicked()
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
    FormDECL->Annotation.ptr->String2 = STRING(qPrintable(lf.toString()));
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
    fontNameT->setText(nn);
    fontNameT->show();
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::on_defaultForeground_clicked()
{
  if (colorSetting.on_defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_defaultBackground_clicked()
{
  if (colorSetting.on_defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_colorButtonF_clicked()
{
  if (colorSetting.on_colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::on_colorButtonB_clicked()
{
  if (colorSetting.on_colorButtonB_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }

}

void CFontColorPage::on_defaultTForeground_clicked()
{
  if (TextColorSetting.on_defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_TColorButtonF_clicked()
{
  if (TextColorSetting.on_colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::on_TColorButtonB_clicked()
{
  if (TextColorSetting.on_colorButtonB_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::on_defaultTBackground_clicked()
{
  if (TextColorSetting.on_defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_defaultPBForeground_clicked()
{
  if (PBColorSetting.on_defaultForeground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_PBColorButtonF_clicked()
{
  if (PBColorSetting.on_colorButtonF_clicked()) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
}

void CFontColorPage::on_defaultPBBackground_clicked()
{
  if (PBColorSetting.on_defaultBackground_clicked())
    myWizard->setModified(true);
}

void CFontColorPage::on_PBColorButtonB_clicked()
{
  if (PBColorSetting.on_colorButtonB_clicked()) {
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
    v_Input = isInput->isChecked();
    v_Output = isOutput->isChecked();
    v_DefaultIO = UseDefault->isChecked();
    v_noEcho = noEcho->isChecked();
    v_NoFIO = NoFIO->isChecked();
    v_MultiLine = MultiLine->isChecked();
    v_isPopupW = Popupw->isChecked();

    m_Space = FHSpaceSPIN->value();
    m_Tab = FHTabSPIN->value();
    m_Frmspace = FVSpaceSPIN->value();
    m_SpaceItem = DHSpaceSPIN->value();
    m_TabItem = DHTabSPIN->value();
    m_FramespaceItem = DVSpaceSPIN->value();
    m_FLength = ColsSPIN->value();
    m_Rows = RowsSPIN->value();

  }
  else {
    modify = false;
    isInput->setChecked(v_Input);
    isOutput->setChecked(v_Output);
    UseDefault->setChecked(v_DefaultIO);
    noEcho->setChecked(v_noEcho);
    NoFIO->setChecked(v_NoFIO);
    MultiLine->setChecked(v_MultiLine);
    Popupw->setChecked(v_isPopupW);
    FHSpaceSPIN->setValue(m_Space);
    FHTabSPIN->setValue(m_Tab);
    FVSpaceSPIN->setValue(m_Frmspace);
    DHSpaceSPIN->setValue(m_SpaceItem);
    DHTabSPIN->setValue(m_TabItem);
    DVSpaceSPIN->setValue(m_FramespaceItem);
    ColsSPIN->setValue(m_FLength);
    RowsSPIN->setValue(m_Rows);
    modify = true;
  }
}

CIOPage::CIOPage(CLavaPEWizard *wizard, LavaDECL *formDECL, bool forChElem)
 : Ui_IDD_IOPage()
{
  setupUi(this);
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
  FHTabSPIN->setMinimum (0);
  FHSpaceSPIN->setMinimum (0);
  FVSpaceSPIN->setMinimum (0);
  DHTabSPIN->setMinimum (0);
  DHSpaceSPIN->setMinimum (0);
  DVSpaceSPIN->setMinimum (0);
  ColsSPIN->setMinimum (0);
  RowsSPIN->setMinimum (0);
  FHTabSPIN->setMaximum (100);
  FHSpaceSPIN->setMaximum (100);
  FVSpaceSPIN->setMaximum (100);
  DHTabSPIN->setMaximum (100);
  DHSpaceSPIN->setMaximum (100);
  DVSpaceSPIN->setMaximum (100);
  ColsSPIN->setMaximum (100);
  RowsSPIN->setMaximum (100);
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
    isInput->setEnabled(false);
    isOutput->setEnabled(false);
    UseDefault->setEnabled(false);
    noEcho->setEnabled(false);
    NoFIO->setEnabled(false);
  }
  else {
    v_noEcho  = (anno && (anno->Emphasis == NoEcho));
    v_Input = anno->IoSigFlags.Contains(Flag_INPUT);
    v_Output = anno->IoSigFlags.Contains(Flag_OUTPUT);
    v_NoFIO =  anno->IoSigFlags.Contains(DONTPUT);
    v_DefaultIO = !(v_Input || v_Output || v_NoFIO);
  }
  v_isPopupW = anno->BasicFlags.Contains(PopUp);
  groupbox->setChecked(anno->BasicFlags.Contains(Groupbox));
  if (FormDECL->DeclType == FormDef) {
    FHSpaceSPIN->setEnabled(false);
    FHTabSPIN->setEnabled(false);
    FVSpaceSPIN->setEnabled(false);
    groupbox->setEnabled(false);
  }
  else
    SetSpace(FormDECL->Annotation.ptr, m_Tab, m_Space, m_Frmspace);
  if (!ForChainElem && (inEl->DeclType == VirtualType))
    etype = NonBasic;
  else
    etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, inEl);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString);
  if (myWizard->IOPage)
    myWizard->IOPage->Popupw->setEnabled(false);
  Popupw->setEnabled((etype == NonBasic)  && (!myWizard->ChainIOPage || (myWizard->ChainIOPage == this)));
  ColsSPIN->setEnabled(atomic);
  if (anno && atomic && (anno->Length.Field > 0))
    m_FLength = anno->Length.Field;
  SetSpace(anno, m_TabItem, m_SpaceItem, m_FramespaceItem);
  MultiLine->setEnabled(etype == VLString);
  v_MultiLine = anno->BasicFlags.Contains(Text);
  if (v_MultiLine)
    m_Rows = anno->Length.DecPoint;
  RowsSPIN->setEnabled(v_MultiLine);
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
  if (groupbox->isChecked())
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


void CIOPage::on_isInput_clicked()
{
  if (isInput->isChecked()) {
    UseDefault->setChecked(0);
    NoFIO->setChecked(0);
    v_DefaultIO = false;
    v_NoFIO = false;
  }
  else
    if (!(NoFIO->isChecked() || isOutput->isChecked())) {
      UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);
}

void CIOPage::on_isOutput_clicked()
{
  if (isOutput->isChecked())  {
    UseDefault->setChecked(0);
    NoFIO->setChecked(0);
    v_DefaultIO = false;
    v_NoFIO = false;
  }
  else
    if (!(isInput->isChecked() || NoFIO->isChecked())) {
      UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);

}

void CIOPage::on_NoFIO_clicked()
{
  if (NoFIO->isChecked()) {
    isInput->setChecked(0);
    isOutput->setChecked(0);
    noEcho->setChecked(0);
    UseDefault->setChecked(0);
    v_Input = false;
    v_Output = false;
    v_noEcho = false;
    v_DefaultIO = false;
    UseDefault->setEnabled(true);
  }
  else
    if (!(isInput->isChecked() || isOutput->isChecked())) {
      UseDefault->setChecked(1);
      v_DefaultIO = true;
    }
  UseDefault->setEnabled(!v_DefaultIO);
  myWizard->setModified(true);
}

void CIOPage::on_UseDefault_clicked()
{
  if (UseDefault->isChecked()) {
    isInput->setChecked(0);
    isOutput->setChecked(0);
    NoFIO->setChecked(0);
    v_Input = false;
    v_Output = false;
    v_NoFIO = false;
  }
  UseDefault->setEnabled(!UseDefault->isChecked());
  myWizard->setModified(true);
}

void CIOPage::on_Popupw_clicked()
{
  TDeclDescType dtype = ((CLavaPEApp*)wxTheApp)->Browser.GetExprType(myWizard->myDoc->mySynDef,  FormDECL);
  ColsSPIN->setEnabled(dtype == BasicType);
  v_isPopupW = !v_isPopupW;
  myWizard->setModified(true);
}

void CIOPage::on_MultiLine_clicked()
{
  v_MultiLine = !v_MultiLine;
  RowsSPIN->setEnabled(v_MultiLine);
  myWizard->setModified(true);
}

void CIOPage::on_noEcho_clicked()
{
  myWizard->setModified(true);
}

void CIOPage::on_FHTabSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_FHSpaceSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_FVSpaceSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_DHTabSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_DHSpaceSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_DVSpaceSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_ColsSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}

void CIOPage::on_RowsSPIN_valueChanged( int )
{
  myWizard->setModified(modify);
}


void CIOPage::on_groupbox_clicked()
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
    v_Default = Default->text();
    if (EnumDefault->currentIndex())
      v_EnumDefault = EnumDefault->currentText();
    else
      v_EnumDefault = QString("");
    if (BoolDefault->currentIndex())
      v_BoolDefault = BoolDefault->currentText();
    else
      v_BoolDefault = QString("");
  }
  else {
    modify = false;
    Default->setText(v_Default);
    if (v_EnumDefault.length())
      EnumDefault->setItemText(EnumDefault->currentIndex(),v_EnumDefault);
    else
      EnumDefault->setCurrentIndex(0);
    if (v_BoolDefault.length())
      BoolDefault->setItemText(BoolDefault->currentIndex(),v_BoolDefault);
    else
      BoolDefault->setCurrentIndex(0);
    modify = true;
  }
}

CLiteralsPage::CLiteralsPage(CLavaPEWizard *wizard, LavaDECL* formDECL, bool forChElem)
   : Ui_LiteralsPage()
{
  setupUi(this);
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
    ADDPre->setEnabled(true);
    DELETEPre->setEnabled(false);
    EDITPre->setEnabled(false);
    ADDSuf->setEnabled(true);
    DELETESuf->setEnabled(false);
    EDITSuf->setEnabled(false);
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
  CListItem *item;
  CComboBoxItem *cbitem;
  TID tid0;

  if ((FormDECL->DeclType == VirtualType) && !ForChainElem)
    etype = NonBasic;
  else
    etype = ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(myWizard->myDoc->mySynDef, FormDECL);
  atomic = (etype == B_Bool) || (etype == Char) || (etype ==  Integer) || (etype ==  Float) || (etype ==  Double) || (etype ==  VLString);
  isEnumera = (etype == Enumeration);
  if (isEnumera) {
    /*
    Default->hide();
    BoolDefault->hide();
    EnumDefault->show();
    */
    Default->setEnabled(false);
    BoolDefault->setEnabled(false);
    EnumDefault->setEnabled(true);
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
      cbitem = new CComboBoxItem(tid0);
      EnumDefault->addItem(QString(enumsel->data.Id.c),QVariant::fromValue(item));
      enumsel = (CHEEnumSelId*)enumsel->successor;
    }
    if (inEl->Annotation.ptr && inEl->Annotation.ptr->FA.ptr)
      str = ((TAnnotation*)inEl->Annotation.ptr->FA.ptr)->StringValue.c; //Defaultwert
    if (!str.isEmpty()) {
      v_EnumDefault = str;
      EnumDefault->setItemText(EnumDefault->currentIndex(),v_EnumDefault);
    }
    else
      EnumDefault->setCurrentIndex(0);
  }
  else {
    EnumDefault->setEnabled(false);
    if (FormDECL->Annotation.ptr && FormDECL->Annotation.ptr->FA.ptr) {
      if (!atomic) {
        Default->setEnabled(false);
        BoolDefault->setEnabled(false);
      }
      else {
        anno = (TAnnotation*)FormDECL->Annotation.ptr->FA.ptr;
        if (inEl->BType == B_Bool) {
          Default->setEnabled(false);
          BoolDefault->setEnabled(true);
          if (anno->IoSigFlags.Contains(trueValue)) {
            v_BoolDefault = anno->StringValue.c;
            BoolDefault->setItemText(BoolDefault->currentIndex(),v_BoolDefault); //Defaultwert
          }
          else
            BoolDefault->setCurrentIndex(0);
        }
        else {
          Default->setEnabled(true);
          BoolDefault->setEnabled(false);
          if (anno->BType == Integer) {
            if (anno->IoSigFlags.Contains(trueValue)) {
              str = QString("%1").arg(anno->I);
              Default->setAlignment(Qt::AlignRight);
            }
          }
          else if (inEl->BType == Char) {
            if (anno->IoSigFlags.Contains(trueValue))
              str = anno->StringValue.c; //Defaultwert
            Default->setMaxLength(1);
            str = anno->StringValue.c;
          }
          else
            str = anno->StringValue.c; //Defaultwert
        }
      }
      Default->setEnabled(atomic && (etype != B_Bool));
      v_Default = str;
    }
  }
  inDefEl = (CHE*)FormDECL->Annotation.ptr->Prefixes.first;
  while (inDefEl /*&& !((LavaDECL*)inDefEl->data)->Annotation.ptr->BasicFlags.Contains(Toggle)*/) {
    item = new CListItem(((LavaDECL*)inDefEl->data)->LitStr, tid0);
    Prefixe->addItem(item);
    inDefEl = (CHE*)inDefEl->successor;
  }
  inDefEl = (CHE*)FormDECL->Annotation.ptr->Suffixes.first;
  while (inDefEl) {
    item = new CListItem(((LavaDECL*)inDefEl->data)->LitStr, tid0);
    Suffixe->addItem(item);
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
      selPos = EnumDefault->currentIndex();
      if ((*p_anno == 0) && selPos && (selPos > 0))
        *p_anno = new TAnnotation;
    }
    else if (inEl->BType == B_Bool) {
      selPos = BoolDefault->currentIndex();
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
          (*p_anno)->StringValue = STRING(qPrintable(v_EnumDefault));
        else
          (*p_anno)->StringValue.Reset(0);
      }
      else if (inEl->BType == B_Bool) {
        (*p_anno)->BType = B_Bool;
        if (selPos && (selPos > 0)) {
          (*p_anno)->IoSigFlags.INCL(trueValue);
          (*p_anno)->StringValue = STRING(qPrintable(v_BoolDefault));
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
        (*p_anno)->StringValue = STRING(qPrintable(v_Default));
      }
      else  if (inEl->BType == Integer) {
        char * endptr;
        (*p_anno)->BType = Integer;
        (*p_anno)->StringValue = STRING(qPrintable(v_Default));
        if ((*p_anno)->StringValue.l) {
          (*p_anno)->IoSigFlags.INCL(trueValue);
          (*p_anno)->I = strtol(v_Default.toAscii(), &endptr, 10);
        }
        else
          (*p_anno)->IoSigFlags.EXCL(trueValue);
        (*p_anno)->Alignment = RightAligned;
      }
      else {
        if (atomic) {
          (*p_anno)->StringValue = STRING(qPrintable(v_Default));
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


bool CLiteralsPage::OnAdd(ChainAny0* chain, QListWidget* list/*, int transpos*/)
{
  int ins = list->currentRow();
  LavaDECL* Decl = NewLavaDECL();
  CHE* cheDecl;

  if (ins < 0)
    ins = 0;
  else
    ins += 1;
  Decl->Annotation.ptr = new TAnnotation;
  Decl->DeclDescType = LiteralString;
  CLiteralItem *lItem = new CLiteralItem(this, true, ins, list, Decl);
  if (lItem->exec() == QDialog::Accepted) {
    QString qs = list->item(ins)->text();
    Decl->LitStr = STRING(qPrintable(qs));
    if (Decl->LitStr.l != 0) {
      cheDecl = NewCHE(Decl);
      chain->AddNth(ins+1/*+transpos*/, cheDecl);
      list->setCurrentRow(ins);
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

void CLiteralsPage::on_ADDPre_clicked()
{
  if (OnAdd(&FormDECL->Annotation.ptr->Prefixes, Prefixe)) {
    DELETEPre->setEnabled(true);
    EDITPre->setEnabled(true);
  }
}

void CLiteralsPage::on_ADDSuf_clicked()
{
  if (OnAdd(&FormDECL->Annotation.ptr->Suffixes, Suffixe/*, sufTranspos*/)) {
    DELETESuf->setEnabled(true);
    EDITSuf->setEnabled(true);
  }
}

void CLiteralsPage::on_DELETEPre_clicked()
{
  int idel = Prefixe->currentRow();
  if (idel >= 0) {
    CHE* cheDecl = (CHE*)FormDECL->Annotation.ptr->Prefixes.UncoupleNth(idel+1);
    if (cheDecl) {
      delete cheDecl;
      delete Prefixe->takeItem(idel);
      DELETEPre->setEnabled((Prefixe->count() > 0));
      EDITPre->setEnabled((Prefixe->count() > 0));
      myWizard->setModified(true);
    }
  }
}

void CLiteralsPage::on_DELETESuf_clicked()
{
  int idel = Suffixe->currentRow();
  if (idel >= 0) {
    CHE * cheDecl = (CHE *)FormDECL->Annotation.ptr->Suffixes.UncoupleNth(idel+1/*+sufTranspos*/);
    if (cheDecl) {
      delete cheDecl;
      delete Suffixe->takeItem(idel);
      DELETESuf->setEnabled((Suffixe->count() > 0));
      EDITSuf->setEnabled((Suffixe->count() > 0));
      myWizard->setModified(true);
    }
  }
}


bool CLiteralsPage::OnEdit(ChainAny0 * chain, QListWidget* list/*, int transpos*/)
{
  int ins = list->currentRow();
  CHE* cheDecl = (CHE*)chain->GetNth(ins+1/*+transpos*/);
  LavaDECL* Decl = (LavaDECL*) cheDecl->data;
  if (ins >= 0) {
    CLiteralItem *lItem = new CLiteralItem(this, false, ins, list, Decl);
    if (lItem->exec() == QDialog::Accepted) {
      Decl->LitStr = STRING(qPrintable(list->item(ins)->text()));
      if (Decl->LitStr.l == 0) {
        if (chain->Uncouple(cheDecl))
          delete cheDecl;
        delete list->takeItem(ins);
        list->setCurrentRow(ins-1);
        delete lItem;
        myWizard->setModified(true);
        myWizard->myView->PostApplyHint();
        return false;
      }
      else {
        list->setCurrentRow(ins);
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

void CLiteralsPage::on_EDITPre_clicked()
{
  if (OnEdit(&FormDECL->Annotation.ptr->Prefixes, Prefixe)) {
    DELETEPre->setEnabled(true);
    EDITPre->setEnabled(true);
  }
  else {
    DELETEPre->setEnabled((Prefixe->count() > 0));
    EDITPre->setEnabled((Prefixe->count() > 0));
  }
}

void CLiteralsPage::on_EDITSuf_clicked()
{
  if (OnEdit(&FormDECL->Annotation.ptr->Suffixes, Suffixe/*, sufTranspos*/)) {
    DELETESuf->setEnabled(true);
    EDITSuf->setEnabled(true);
  }
  else {
    DELETESuf->setEnabled((Suffixe->count() > 0));
    EDITSuf->setEnabled((Suffixe->count() > 0));
  }
}


void CLiteralsPage::on_Prefixe_itemDoubleClicked( QListWidgetItem * )
{
  on_EDITPre_clicked();
}


void CLiteralsPage::on_Suffixe_itemDoubleClicked( QListWidgetItem *)
{
  on_EDITSuf_clicked();
}

void CLiteralsPage::on_Prefixe_currentItemChanged(QListWidgetItem* current, QListWidgetItem * previous)
{
  if (Prefixe->currentItem()) {
    DELETEPre->setEnabled(true);
    EDITPre->setEnabled(true);
  }
  else {
    DELETEPre->setEnabled(false);
    EDITPre->setEnabled(false);
  }
}

void CLiteralsPage::on_Suffixe_currentItemChanged(QListWidgetItem* current, QListWidgetItem * previous)
{
  if (Suffixe->currentItem()) {
    DELETESuf->setEnabled(true);
    EDITSuf->setEnabled(true);
  }
  else {
    DELETESuf->setEnabled(false);
    EDITSuf->setEnabled(false);
  }
}

void CLiteralsPage::on_Default_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CLiteralsPage::on_EnumDefault_activated(int )
{
  myWizard->setModified(true);
}

void CLiteralsPage::on_BoolDefault_activated(int )
{
  myWizard->setModified(true);
}

bool CLiteralsPage::OnSetActive()
{
  enabled = ((FormDECL->DeclType == Attr)
    || (FormDECL->DeclType == PatternDef)
    || (FormDECL->DeclType == VirtualType));
  ADDSuf->setEnabled(enabled);
  ADDPre->setEnabled(enabled);
  EDITSuf->setEnabled(false);
  EDITPre->setEnabled(false);
  DELETESuf->setEnabled(false);
  DELETEPre->setEnabled(false);
  Suffixe->setEnabled(enabled);
  Prefixe->setEnabled(enabled);
  if (enabled) {
    Prefixe->setCurrentRow(-1);
    Suffixe->setCurrentRow(-1);
  }
  return true;
}


/*
void CLiteralsPage::OnLButtonDown(QMouseEvent *)
{
  Prefixe->setCurrentRow(-1);
  Suffixe->setCurrentRow(-1);
  DELETEPre->setEnabled(false);
  EDITPre->setEnabled(false);
  DELETESuf->setEnabled(false);
  EDITSuf->setEnabled(false);
}
*/

bool CLiteralsPage::OnApply()
{
  UpdateData(true);
  GetProps();
  return true;
}



void CLiteralItem::UpdateData(bool getData)
{
  if (getData) {
    m_lit = EditLiteral->text();
    m_LiSpace = HSpaceSPIN->value();
    m_LiTab = HTabSPIN->value();
    m_LiFrmSpace = VSpaceSPIN->value();
  }
  else {
    EditLiteral->setText(m_lit);
    HSpaceSPIN->setValue(m_LiSpace);
    HTabSPIN->setValue(m_LiTab);
    VSpaceSPIN->setValue(m_LiFrmSpace);
  }
}


CLiteralItem::CLiteralItem(CLiteralsPage *page, bool isNew, int ipos, QListWidget *litList, LavaDECL * litEl)
  : Ui_IDD_LiteralItem()
{
  setupUi(this);
  m_lit = "";
  beforeBase->hide();
  LitPage = page;
  litPos = ipos;
  LitList = litList;
  insert = isNew;
  myDecl = litEl;
  HSpaceSPIN->setMinimum (0);
  HTabSPIN->setMinimum (0);
  VSpaceSPIN->setMinimum (0);
  HSpaceSPIN->setMaximum (100);
  HTabSPIN->setMaximum (100);
  VSpaceSPIN->setMaximum (100);
  m_LiFrmSpace = 0;
  m_LiSpace = 0;
  m_LiTab = 0;

  if (!insert) {
    m_lit = LitList->item(litPos)->text();
    SetSpace((TAnnotation*)myDecl->Annotation.ptr->FA.ptr, m_LiTab, m_LiSpace, m_LiFrmSpace);
  }
  QFont lf;
  if (myDecl->Annotation.ptr->String1.l && lf.fromString(myDecl->Annotation.ptr->String1.c)) {
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
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


void CLiteralItem::on_defaultFont_clicked()
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

void CLiteralItem::on_fontButton_clicked()
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
    myDecl->Annotation.ptr->String1 = STRING(qPrintable(lf.toString()));
    QString nn = lf.family() + QString(komma.c) +	QString::number(  lf.pointSizeF() );
    fontName->setText(nn);
    fontName->show();
  }
}


void CLiteralItem::on_defaultForeground_clicked()
{
  colorSetting.on_defaultForeground_clicked();
}

void CLiteralItem::on_defaultBackground_clicked()
{
  colorSetting.on_defaultBackground_clicked();
}

void CLiteralItem::on_colorButtonF_clicked()
{
  colorSetting.on_colorButtonF_clicked();
}

void CLiteralItem::on_colorButtonB_clicked()
{
  colorSetting.on_colorButtonB_clicked();
}


void CLiteralItem::on_ID_OK_clicked()
{
  UpdateData(true);
  TID tid0;
  if (!insert)
    delete LitList->takeItem(litPos);
  CListItem* item = new CListItem(m_lit, tid0);
  LitList->insertItem(litPos,item);//-1);
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
    v_Menutype = Menutype->currentIndex();
    m_ItemNr = Menuitems->currentRow();
    v_LeftLabel = LeftLabel->isChecked();
    v_ToggleLabel = ToggleLabel->text();
  }
  else {
    modify = false;
    Menutype->setCurrentIndex(v_Menutype);
    Menuitems->setCurrentRow(m_ItemNr);
    LeftLabel->setChecked(v_LeftLabel);
    ToggleLabel->setText(v_ToggleLabel);
    modify = true;
  }
}


CMenuPage::CMenuPage(CLavaPEWizard *wizard)
 : Ui_IDD_MenuPage()
{
  setupUi(this);
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
    EditButton->setEnabled(false);
    DeleteButton->setEnabled(false);
    AddButton->setEnabled(v_Menutype != int(isOMenu));
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
  DString label = DString(qPrintable(v_ToggleLabel));
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
  CListItem *item;
  QListWidgetItem *pixItem;

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
    item = new CListItem(*labe, mflags);
    Menuitems->addItem(item);
    item = new CListItem(*labb, mflags);
    LButtonText->addItem(item);
    pixItem = new CListItem(*labpix, mflags);
    Pixmap->addItem(pixItem);
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
      anno->BasicFlags.EXCL(RadioButtons);
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
  maxi = qMax(Menuitems->count(), LButtonText->count());
  while (ipos < maxi) {
    mflags = ((CListItem*)Menuitems->item(ipos))->flags();
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
            qs = LButtonText->item(ipos)->text();
            enumsel->data.SelectionCode = STRING(qPrintable(qs));
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
        qs = LButtonText->item(ipos)->text();
        ((LavaDECL*)ininDefEl->data)->LitStr = STRING(qPrintable(qs));
        if (Pixmap->item(ipos)->text().length()) {
          CHETAnnoEx* ex = ((LavaDECL*)ininDefEl->data)->Annotation.ptr->GetAnnoEx(anno_Pixmap, true);
          ex->data.xpmFile = DString(qPrintable(Pixmap->item(ipos)->text()));
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


void CMenuPage::on_Menuitems_currentItemChanged (QListWidgetItem* , QListWidgetItem*)
{
  int ss = Menuitems->currentRow();
  LButtonText->setCurrentRow(ss);
  Pixmap->setCurrentRow(ss);
  SetButtons(ss);
}

void CMenuPage::on_LButtonText_currentItemChanged(QListWidgetItem* , QListWidgetItem *)
{
  int ss = LButtonText->currentRow();
  Menuitems->setCurrentRow(ss);
  Pixmap->setCurrentRow(ss);
  SetButtons(ss);
}

void CMenuPage::on_Pixmap_currentItemChanged(QListWidgetItem* ,QListWidgetItem*)
{
  int ss = LButtonText->currentRow();
  Menuitems->setCurrentRow(ss);
  LButtonText->setCurrentRow(ss);
  SetButtons(ss);
}


void CMenuPage::SetButtons(int sel)
{
  if (sel < 0) {
    DeleteButton->setEnabled(false);
    EditButton->setEnabled(false);
  }
  else {
    unsigned *mflags = ((CListItem*)Menuitems->item(sel))->flags();
    DeleteButton->setEnabled(mflags[0] == 3);
    EditButton->setEnabled(true);
  }
}


bool CMenuPage::OnSetActive()
{
  LeftLabel->setEnabled(isBool);
  ToggleLabel->setEnabled(isBool);
  LButtonText->setEnabled(isEnumera);
  Menuitems->setEnabled(isEnumera);
  Menutype->setEnabled(isEnumera);
  EditButton->setEnabled(false);
  DeleteButton->setEnabled(false);
  AddButton->setEnabled(isEnumera && (v_Menutype != int(isOMenu)));
  if (isEnumera) {
    LButtonText->setCurrentRow(-1);
    Menuitems->setCurrentRow(-1);
  }
  return true;
}

void CMenuPage::on_AddButton_clicked()
{
  int ss = Menuitems->currentRow();
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
    LButtonText->setCurrentRow(ss);
    Menuitems->setCurrentRow(ss);
    unsigned *mflags1 = ((CListItem*)Menuitems->item(ss))->flags();
    DeleteButton->setEnabled(mflags1[0] == 3);
    EditButton->setEnabled(true);
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
  delete MItem;
  //delete [] mflags;
}


void CMenuPage::on_EditButton_clicked()
{
  int ss = LButtonText->currentRow();

  if (ss < 0)
    return;
  QString iT, bT, pT;
  iT = Menuitems->item(ss)->text();
  bT = LButtonText->item(ss)->text();
  pT = Pixmap->item(ss)->text();
  unsigned  *mflags;
  mflags = ((CListItem*)Menuitems->item(ss))->flags();
  CMenuItem* cm = new CMenuItem(this, false, ss, iT, bT, pT, mflags);
  if (cm->exec() == QDialog::Accepted) {
    myWizard->setModified(true);
    myWizard->myView->PostApplyHint();
  }
  delete cm;
}


void CMenuPage::on_DeleteButton_clicked()
{
  int ss = Menuitems->currentRow();
  if (ss >= 0) {
    unsigned *mflags = ((CListItem*)Menuitems->item(ss))->flags();
    if (mflags[0] == 3) {
      delete Menuitems->takeItem(ss);
      delete LButtonText->takeItem(ss);
      if (ss)
        ss = ss-1;
      Menuitems->setCurrentRow(ss);
      mflags = ((CListItem*)Menuitems->item(ss))->flags();
      DeleteButton->setEnabled(mflags[0] == 3);
      EditButton->setEnabled(true);
      myWizard->setModified(true);
    }
  }
}

/*
void CMenuPage::OnLButtonDown(QMouseEvent *)
{
  LButtonText->setCurrentRow(-1);
  Menuitems->setCurrentRow(-1);
  DeleteButton->setEnabled(false);
  EditButton->setEnabled(false);

}*/

void CMenuPage::on_Menutype_activated(int current)
{
  int oldType = v_Menutype;
  v_Menutype = current;
  SetDefaults((EMenuType)v_Menutype, (EMenuType)oldType);
  AddButton->setEnabled(isEnumera && (v_Menutype != int(isOMenu)));
  myWizard->setModified(true);
}


void CMenuPage::SetDefaults(EMenuType newMenuT, EMenuType oldMenuT)
{
  if ((newMenuT != isNoMenu)  && (oldMenuT == isNoMenu)
       || (newMenuT == isOMenu) && (oldMenuT != isOMenu)) {
    int cc = Menuitems->count();
    int rr = 0;
    CListItem* item;
    TID tid0;
    QString iT, bT, pT;
    if (newMenuT == isOMenu)
      LButtonText->clear();
    for (int ss = cc-1; ss >= 0; ss--) {
      if (oldMenuT == isNoMenu) {
        iT = Menuitems->item(ss)->text();
        rr = LButtonText->item(ss)->text().length();
        if (rr == 0)
          delete LButtonText->takeItem(ss);
        if ((rr == 0) || (rr < 0)) {
          item = new CListItem(iT, tid0);
          LButtonText->insertItem(ss-1,item);
        }
      }
      if ((newMenuT == isOMenu) && (Menuitems->item(ss)->text().length() > 0)) {
        bT = Menuitems->item(ss)->text();
        if ((rr == 0) || (rr < 0)) {
          item = new CListItem(bT, tid0);
          LButtonText->insertItem(ss-1,item);
        }
      }
    }//for
  }
}

void CMenuPage::on_ToggleLabel_textChanged( const QString & )
{
  myWizard->setModified(modify);
}

void CMenuPage::on_LeftLabel_clicked()
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
    m_MenuFrmspace = VSpaceSPIN->value();
    m_Menuspace = HSpaceSPIN->value();
    m_Menutab = HTabSPIN->value();
    if (isButton->isChecked())
      m_flags = 0;
    else if (noButton->isChecked())
      m_flags = 1;
    else if (NoFIO->isChecked())
      m_flags = 2;
    else
      m_flags = 3;
  }
  else {
    VSpaceSPIN->setValue(m_MenuFrmspace);
    HSpaceSPIN->setValue(m_Menuspace);
    HTabSPIN->setValue(m_Menutab);
    switch (m_flags) {
    case 0:
      isButton->setChecked(true);
      break;
    case 1:
      noButton->setChecked(true);
      break;
    case 2:
      NoFIO->setChecked(true);
      break;
    default:
      isMenuText->setChecked(true);
    }
  }
}


CMenuItem::CMenuItem(CMenuPage* mpage, bool isNew, int ipos,
                     QString enumsel, QString btext, QString stext,
                     unsigned *itemData)
  : Ui_IDD_Menuitem()
{
  setupUi(this);
  mflags = itemData;

  m_flags = mflags[0];
  m_Menutab = mflags[1];
  m_Menuspace = mflags[2];
  m_MenuFrmspace = mflags[3];
  Enumsel->setText(enumsel);
  Enumsel->setEnabled(false);
  StaticText->setText(btext);
  Buttontext->setText(btext);
  if (stext.length())
    Pixmap->setText(stext);
  menuPage = mpage;
  insert = isNew;
  iPos = ipos;

  HSpaceSPIN->setMinimum (0);
  HTabSPIN->setMinimum (0);
  VSpaceSPIN->setMinimum  (0);
  HSpaceSPIN->setMaximum (100);
  HTabSPIN->setMaximum (100);
  VSpaceSPIN->setMaximum  (100);
  EnableTextWindows();
  if (insert) {
    isButton->setEnabled(false);
    isMenuText->setEnabled(false);
    noButton->setEnabled(false);
    NoFIO->setEnabled(false);
  }
  else
    isMenuText->setEnabled(false);
  UpdateData(false);
}

void CMenuItem::EnableTextWindows()
{
  Buttontext->setEnabled(m_flags == 0);
  StaticText->setEnabled(m_flags == 3);
  Pixmap->setEnabled(menuPage->v_Menutype == isBMenu);
  button_browse->setEnabled(menuPage->v_Menutype == isBMenu);
  /*
  if (m_flags != 0)
    m_Buttontext = QString("");
  if (m_flags != 3)
    v_MenuText = QString("");
    */
}


void CMenuItem::on_noButton_clicked(bool)
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::on_NoFIO_clicked(bool)
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::on_isButton_clicked(bool)
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::on_isMenuText_clicked(bool)
{
    UpdateData(true);
    EnableTextWindows();
}

void CMenuItem::on_button_browse_clicked()
{
  QDir dir(menuPage->myWizard->myDoc->IDTable.DocDir.c);
  QFileInfo qf(dir, Pixmap->text());
  //QString pix = QFileDialog::getOpenFileName( qf.absFilePath(), "Pixmap (*.xpm)",
  //                  this,"open file dialog", "Choose a pixmap file", 0, false );
  QString pix = L_GetOpenFileName(
                    qf.absoluteFilePath(),
                    this,
                    "Choose a pixmap file",
                    "Pixmap (*.xpm)",
                    "xpm"
                    );
  if (pix.length()) {
    DString rpfn = DString(qPrintable(pix));
    RelPathName(rpfn,menuPage->myWizard->myDoc->IDTable.DocDir);
    Pixmap->setText(rpfn.c);
  }
}

void CMenuItem::on_ID_OK_clicked()
{
  CListItem *item;
  UpdateData(true);
  /*
  if (m_flags) {
    m_Buttontext = QString("");
  }
  */
  if (!insert) {
    delete menuPage->LButtonText->takeItem(iPos);
    delete menuPage->Menuitems->takeItem(iPos);
    delete menuPage->Pixmap->takeItem(iPos);
  }
  if (m_flags == 3) {
    item = new CListItem(StaticText->text(), mflags);
    menuPage->LButtonText->insertItem(iPos,item);
  }
  else {
    item = new CListItem(Buttontext->text(), mflags);
    menuPage->LButtonText->insertItem(iPos,item);
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
  item = new CListItem(Enumsel->text(), mflags);
  menuPage->Menuitems->insertItem(iPos,item);
  item = new CListItem(Pixmap->text(), mflags);
  menuPage->Pixmap->insertItem(iPos,item);
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
  CListItem *item;

  //v_FormClass = "";

  setupUi(this);
  myWizard = wizard;
  Name->setText(myWizard->FormDECL->LocalName.c);
  //v_Name = QString(myWizard->FormDECL->LocalName.c);
  LavaDECL *classDECL, *fclassDECL/*, *baseClassDECL*/, *basefDECL;
  CHETID *cheS;
  TIDTable *IDTab = &myWizard->myDoc->IDTable;
  classDECL = myWizard->myDoc->IDTable.GetDECL(myWizard->FormDECL->RefID);
  if (classDECL) {
    //v_FormClass = QString(classDECL->LocalName.c);
    FormsClass->setText(classDECL->LocalName.c);
    cheS = (CHETID*)myWizard->FormDECL->ParentDECL->Supports.first;
    fclassDECL = IDTab->GetDECL(cheS->data);
    if (fclassDECL) {
      cheS = (CHETID*)fclassDECL->Supports.first;
      while (cheS) {//!!
        basefDECL = IDTab->GetFinalDef(cheS->data, fclassDECL->inINCL);
        if (basefDECL && basefDECL->SecondTFlags.Contains(isGUI) && (basefDECL->fromBType == NonBasic)) {
          if (IDTab->IsAnc(myWizard->FormDECL->RefID, 0, basefDECL->RefID, basefDECL->inINCL)) {
            item = new CListItem(basefDECL->LocalName, TID(basefDECL->OwnID, basefDECL->inINCL));
            Supports->addItem(item);
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
  ListToChain(Supports, &myWizard->FormDECL->Supports);
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
  //SortCombo(List);

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
    if (base->SecondTFlags.Contains(isGUI) && (base->fromBType == NonBasic))
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
  CComboBoxItem *item = new CComboBoxItem(TID((*pelDef)->OwnID, incl));
  addItemAlpha(List, QString((*pelDef)->ParentDECL->FullName.c),QVariant::fromValue(item)); //sort
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
    CComboBoxItem *item = new CComboBoxItem(TID(elDef->OwnID, incl));
    addItemAlpha(List, QString(elDef->FullName.c), QVariant::fromValue(item));
  }
}
