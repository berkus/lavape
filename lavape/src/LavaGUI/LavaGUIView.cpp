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


#include "MACROS.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "SylTraversal.h"
#include "LavaAppBase.h"
#include "FormWid.h"
#include "TEdit.h"
#include "qpainter.h"
#include "qmessagebox.h"
#include "qclipboard.h"
//Added by qt3to4:
#include <QCloseEvent>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "LavaBaseStringInit.h"
#include "qstatusbar.h"
#include "qpushbutton.h"
//#include "q3vbox.h"
#include "qlayout.h"
#include "Lava.xpm"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIView

GUIScrollView::GUIScrollView(QWidget *parent, bool fromPopup)
    : QScrollArea(parent)
{
  setObjectName("GUIScrollView");
  setFocusPolicy(Qt::StrongFocus);
  qvbox = new GUIVBox(0, fromPopup, this);//myScrv->viewport());
  setWidget(qvbox);
  //setWidgetResizable(true);
  //setResizePolicy(QScrollArea::AutoOne);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  MaxBottomRight = QRect(0,0,0,0);

}


void GUIScrollView::/*viewportR*/resizeEvent(QResizeEvent* ev)
{
  QSize sz = MaxBottomRight.size();
  QSize evsz = ev->size();
  QSize sx = qvbox->size();
  bool ch = false;
  if (sz.width() < ev->size().width() ) {
    sz.setWidth(ev->size().width());
    ch = true;
  }
  if (sz.height() < ev->size().height() ) {
    sz.setHeight(ev->size().height());
    ch = true;
  }
  if (ch)
    qvbox->resize(sz);
  //resizeContents(sz.width(),sz.height());
  //viewport()->update();
  QScrollArea::resizeEvent(ev);
}

LavaGUIDialog::LavaGUIDialog(QWidget *parent,CLavaPEHint *pHint)
: QDialog(parent)
{
  setObjectName("LavaGUIDialog");
  setModal(true);
  returned = false;
  myScrv = new GUIScrollView(this, false);
  QWidget* hb = new QWidget(this); // horiz. box
  QPushButton* okButton = new QPushButton("Ok", hb);
  QPushButton* resetButton = new QPushButton("Reset", hb);
  QPushButton* cancelButton = new QPushButton("Cancel", hb);
  qvbl = new QVBoxLayout(this);
  QHBoxLayout* hbl = new QHBoxLayout(hb);
  qvbl->addWidget(myScrv);
  qvbl->addWidget(hb);
  hbl->addWidget(okButton);
  hbl->addWidget(resetButton);
  hbl->addWidget(cancelButton);
  resize(700,500);
  connect(okButton,SIGNAL(clicked()),SLOT(OnOK()));
  connect(resetButton,SIGNAL(clicked()),SLOT(OnReset()));
  connect(cancelButton,SIGNAL(clicked()),SLOT(OnCancel()));
  qvbox = myScrv->qvbox;
  myDoc = (CLavaBaseDoc*)pHint->fromDoc;
  if (myDoc->mySynDef) {
    myGUIProg = new CGUIProg;
    myGUIProg->Create(myDoc, this);
    myGUIProg->SetFont(&LBaseData->m_FormFont);
    ServicePtr = (LavaVariablePtr)pHint->CommandData1;
    myGUIProg->ServicePtr = ServicePtr;
    IniDataPtr = (LavaVariablePtr)pHint->CommandData2;
    ResultDPtr = (LavaVariablePtr)pHint->CommandData3;
    myDECL = (*ServicePtr)[0]->implDECL;
    NewTitle(myDECL, myDoc->IDTable.DocName);
    myID = TID(myDECL->OwnID, 0);
    myGUIProg->FrozenObject = (int)pHint->CommandData4;
    myThread = (CLavaThread*)pHint->CommandData5;
    myGUIProg->fromFillIn = (int)pHint->CommandData6;
    myGUIProg->myDECL = myDECL;
    myGUIProg->OnUpdate( myDECL, ResultDPtr);
    myGUIProg->MakeGUI.DisplayScreen(false);
  }
  //myScrv->show();
  myScrv->ensureVisible(20, 1200);
}

void LavaGUIDialog::setpropSize(QSize& scrSize)
{
  QSize sizeF = size();
  sizeF.setWidth(lmin(sizeF.width(), scrSize.width() + 40));
  sizeF.setHeight(lmin(sizeF.height(), scrSize.height() + 90));
  resize(sizeF);
}
void LavaGUIDialog::UpdateUI()
{
  OnUpdateInsertopt(LBaseData->insActionPtr);
  OnUpdateDeleteopt(LBaseData->delActionPtr);
}

LavaGUIDialog::~LavaGUIDialog()
{
  if (myGUIProg) {
    if (!wxTheApp->appExit)
      myGUIProg->LavaForm.DeletePopups(myGUIProg->Root);
    delete myGUIProg;
  }
}

void LavaGUIDialog::closeEvent(QCloseEvent *e)
{
  if (myGUIProg && ResultDPtr && *ResultDPtr) {
    if (QMessageBox::question(
          wxTheApp->m_appWindow,qApp->applicationName(),"Do you really want to cancel this Lava dialog?",
          QMessageBox::Yes,
          QMessageBox::No) == QMessageBox::Yes) {
      OnCancel();
    }
    else
      return;
  }
}

void LavaGUIDialog::OnOK()
{
  LavaObjectPtr obj;
  if (myGUIProg) {
    myGUIProg->NoteLastModified();
    while (myGUIProg->allocatedObjects.size()) {
      obj = myGUIProg->allocatedObjects.takeFirst();
      DEC_FWD_CNT(myGUIProg->ckd,obj);
    }
    if (myGUIProg->LavaForm.OnOK( myGUIProg->Root)) {
      if (myThread) {
        myThread->mySemaphore.lastException = myGUIProg->ckd.lastException;
        myThread->mySemaphore.ex = myGUIProg->ex;
        myGUIProg->ckd.lastException = 0;
        myGUIProg->ckd.exceptionThrown = false;
      }
      QDialog::accept();
    }
  }
}

void LavaGUIDialog::OnCancel()
{
  LavaObjectPtr obj;
  if (myGUIProg) {
    myGUIProg->NoteLastModified();
    if (*ResultDPtr) {
      DEC_FWD_CNT(myGUIProg->ckd,*ResultDPtr);
      *ResultDPtr = 0;
      while (myGUIProg->allocatedObjects.count()) {
        obj = myGUIProg->allocatedObjects.takeFirst();
        DEC_FWD_CNT(myGUIProg->ckd,obj);
      }
    }
    if (myThread) {
      myThread->mySemaphore.lastException = myGUIProg->ckd.lastException;
      myThread->mySemaphore.ex = myGUIProg->ex;
      myGUIProg->ckd.lastException = 0;
      myGUIProg->ckd.exceptionThrown = false;
    }
  }
  QDialog::reject();
}

void LavaGUIDialog::OnReset()
{
  bool ok = true;;
  if (myGUIProg) {
    myGUIProg->NoteLastModified();
    if (*ResultDPtr) {
      DEC_FWD_CNT(myGUIProg->ckd,*ResultDPtr);
      *ResultDPtr = 0;
    }
    if (myGUIProg->fromFillIn)
      *ResultDPtr = AllocateObject(myGUIProg->ckd, (*ServicePtr)[0][0].classDECL->RelatedDECL, false);
    if ((*ResultDPtr || !myGUIProg->fromFillIn) && !myGUIProg->ckd.exceptionThrown) {
      if (*IniDataPtr) {
        try {
#ifndef WIN32
          if (setjmp(contOnHWexception)) throw hwException;
#endif
          if (myGUIProg->fromFillIn)
            myGUIProg->ex = CopyObject(myGUIProg->ckd, IniDataPtr, ResultDPtr, ((SynFlags*)((*IniDataPtr)+1))->Contains(stateObjFlag), (*ServicePtr)[0][0].classDECL->RelatedDECL);
          else
            myGUIProg->ex = CopyObject(myGUIProg->ckd, IniDataPtr, ResultDPtr, ((SynFlags*)((*IniDataPtr)+1))->Contains(stateObjFlag));
          if (myGUIProg->ex)
            ok = false;
          if (myGUIProg->ckd.exceptionThrown)
            ok = false;
        }
        catch (CRuntimeException ex) {
          if (!ex.SetLavaException(myGUIProg->ckd))
            //throw;
          ok = false;
        }
        catch (CHWException ex) {
          if (!ex.SetLavaException(myGUIProg->ckd))
            //throw;
          ok = false;
        }
        ok = ok && !myGUIProg->ckd.exceptionThrown && !myGUIProg->ex;
      }
      if (ok && *ResultDPtr) {
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        myGUIProg->MakeGUI.DisplayScreen(false);
        return;
      }
    }
  }
  OnCancel(); //return to calling program if exceptions appear}
}


void LavaGUIDialog::NewTitle(LavaDECL *decl, const DString& lavaName)
{
  QString oldTitle=windowTitle(), newTitle;

  if (decl) {
    DString title = lavaName;
    title += DString(" - ");
    title += decl->FullName;
    newTitle = QString(title.c);
  }
  else
    newTitle = QString(lavaName.c);
  setWindowTitle(newTitle);
  if (!oldTitle.isEmpty() && newTitle != oldTitle)
    wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(oldTitle,newTitle);
}
 

void LavaGUIDialog::OnDeleteOpt()
{
  if (myGUIProg && myGUIProg->DelNode) {
    if (myGUIProg->DelNode->data.IterFlags.Contains(Optional))
      myGUIProg->CmdExec.DeleteOptionalItem(myGUIProg->DelNode);
    else
      myGUIProg->CmdExec.DeleteIterItem(myGUIProg->DelNode);
    myGUIProg->DelNode = 0;
  }
}


void LavaGUIDialog::OnUpdateDeleteopt(QAction* action)
{
  if (myGUIProg)
    myGUIProg->OnUpdateDeleteopt(action);
}

void LavaGUIDialog::OnInsertOpt()
{
  if (myGUIProg && myGUIProg->InsertNode)
    myGUIProg->CmdExec.InsertIterItem(myGUIProg->InsertNode);
}

void LavaGUIDialog::OnUpdateInsertopt(QAction* action)
{
  if (myGUIProg) 
    myGUIProg->OnUpdateInsertopt(action);
}

CLavaGUIView::CLavaGUIView(QWidget *parent,wxDocument *doc)
   : CLavaBaseView(parent,doc,"LavaGUIView")
{
  released = false;
  myScrv = new GUIScrollView(this, false);
  layout->addWidget(myScrv);
  if (LBaseData->inRuntime && !((CLavaBaseDoc*)doc)->isObject) {
    QWidget* hb = new QWidget(this);
    QHBoxLayout* hbl = new QHBoxLayout(hb);
    QPushButton* okButton = new QPushButton("Ok", hb);
    QPushButton* resetButton = new QPushButton("Reset", hb);
    hbl->addWidget(okButton);
    hbl->addWidget(resetButton);
    connect(okButton,SIGNAL(clicked()),SLOT(OnOK()));
    connect(resetButton,SIGNAL(clicked()),SLOT(OnCancel()));
  }
  qvbox = myScrv->qvbox;
  myDECL = 0;
  myGUIProg = 0;
  ServicePtr = 0;
  IniDataPtr = 0;
  ResultDPtr = 0;
  CurrentCategory = false;
  mainTree = 0;
  myTree = 0;
  LastBrowseNode = 0;
  inUpdate = true;
  StatusbarMess = "";
  clipboard_text_notEmpty = false;
}


CLavaGUIView::~CLavaGUIView()
{
  if (!wxTheApp->deletingMainFrame) {
    DisableActions();
    if (LBaseData->Browser->LastBrowseContext)
      LBaseData->Browser->LastBrowseContext->RemoveView(this);
  }
  if (myGUIProg) {
    if (!wxTheApp->appExit)
      myGUIProg->LavaForm.DeletePopups(myGUIProg->Root);
    delete myGUIProg;
  }
  if (!wxTheApp->appExit) {
    QString msg("Document is being closed");
    wxTheApp->m_appWindow->statusBar()->showMessage(msg);
  }
  GetDocument()->RuntimeView = 0;
}


CLavaBaseDoc* CLavaGUIView::GetDocument() // non-debug version is inline
{
  return (CLavaBaseDoc*)m_viewDocument;
}


void CLavaGUIView::UpdateUI()
{
  OnUpdateInsertopt(LBaseData->insActionPtr);
  OnUpdateDeleteopt(LBaseData->delActionPtr);
  OnUpdateEditPaste(LBaseData->editPasteActionPtr);
  OnUpdateEditCopy(LBaseData->editCopyActionPtr);
  OnUpdateEditCut(LBaseData->editCutActionPtr);
  if (LBaseData->updateResetActionPtr) {
    OnUpdateTogglestate(LBaseData->toggleCatActionPtr);
    OnUpdateCancel(LBaseData->updateResetActionPtr);
    //OnUpdateOk(LBaseData->okActionPtr);
  }
  if (!LBaseData->inRuntime) {
    OnUpdateGotodef(LBaseData->gotoDeclActionPtr);
    //OnUpdateNewFunc(LBaseData->newFuncActionPtr);
  }
}

void CLavaGUIView::DisableActions()
{
  LBaseData->insActionPtr->setEnabled(false);
  LBaseData->delActionPtr->setEnabled(false);
  LBaseData->editCutActionPtr->setEnabled(false);
  LBaseData->editCopyActionPtr->setEnabled(false);
  LBaseData->editPasteActionPtr->setEnabled(false);
  if (LBaseData->updateResetActionPtr) {
          LBaseData->toggleCatActionPtr->setEnabled(false);
    LBaseData->updateResetActionPtr->setEnabled(false);
    //LBaseData->okActionPtr->setEnabled(false);
  }
  if (!LBaseData->inRuntime)
    LBaseData->gotoDeclActionPtr->setEnabled(false);
}


void CLavaGUIView::OnInitialUpdate()
{
  CLavaPEHint *pHint = LBaseData->actHint;
  wxMDIChildFrame *pChild = (wxMDIChildFrame*)GetParentFrame();
  if (pChild && pChild->inherits("CLavaGUIFrame"))
    ((CLavaGUIFrame*)pChild)->myView = this;
  GetDocument()->RuntimeView = this;
  if (GetDocument()->mySynDef) {
    myGUIProg = new CGUIProg;
    myGUIProg->Create(GetDocument(), this);
    myGUIProg->SetFont(&LBaseData->m_FormFont);
    if (pHint && LBaseData->actHint->com == CPECommand_OpenFormView) {
      if (!LBaseData->inRuntime) {
        //LavaPE only
        myDECL = (LavaDECL*)pHint->CommandData1;
        for ( ; myDECL && (myDECL->DeclType == VirtualType); myDECL = GetDocument()->IDTable.GetDECL(myDECL->RefID, myDECL->inINCL));
        myID = TID(myDECL->OwnID, myDECL->inINCL);
        mainTree = (CLavaBaseView*)pHint->CommandData2;
        LastBrowseNode = 0;
        myGUIProg->myDECL = myDECL;
        myGUIProg->selDECL = myDECL;
        OnUpdate(this, 0, pHint);
      }
    }
    else { //show Lava object autonom (.ldoc)
      if (LBaseData->inRuntime && GetDocument()->isObject && GetDocument()->DocObjects[2]) {
        GetParentFrame()->showMaximized();
        ServicePtr = &GetDocument()->DocObjects[0];
        myGUIProg->ServicePtr = ServicePtr;
        IniDataPtr = &GetDocument()->DocObjects[1];
        ResultDPtr = &GetDocument()->DocObjects[2];
        if (*ResultDPtr)
          CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
        myGUIProg->FrozenObject = 0;
        myGUIProg->fromFillIn = 1;
        myDECL = (*ServicePtr)[0][0].implDECL;
        ((CLavaGUIFrame*)GetParentFrame())->setWindowTitle(GetDocument()->GetTitle());
        myID = TID(myDECL->OwnID, myDECL->inINCL);
        LastBrowseNode = 0;
        MessToStatusbar();
        myGUIProg->myDECL = myDECL;
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        myGUIProg->MakeGUI.DisplayScreen(false);
      }
    }
  }
  else {
    if (LBaseData->inRuntime && GetDocument()->isObject) {
      GetParentFrame()->showMaximized();
      GetDocument()->SelectLcom(true);
      myGUIProg = new CGUIProg;
      MessToStatusbar();
      myGUIProg->Create(GetDocument(), this);
      ServicePtr = &GetDocument()->DocObjects[0];
      myGUIProg->ServicePtr = ServicePtr;
      IniDataPtr = &GetDocument()->DocObjects[1];
      ResultDPtr = &GetDocument()->DocObjects[2];
      if (*ResultDPtr)
        CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
      myGUIProg->FrozenObject = 0;
      myGUIProg->fromFillIn = 1;
      myDECL = (*ServicePtr)[0][0].implDECL;
      //if (!GetDocument()->IsEmbedded())
        ((CLavaGUIFrame*)GetParentFrame())->setWindowTitle(GetDocument()->GetTitle());
      myID = TID(myDECL->OwnID, myDECL->inINCL);
      LastBrowseNode = 0;
      myGUIProg->myDECL = myDECL;
      myGUIProg->OnUpdate( myDECL, ResultDPtr);
      myGUIProg->MakeGUI.DisplayScreen(false);
    }
  }
}

void CLavaGUIView::OnUpdate(wxView* , unsigned undoRedoCheck, QObject* pHint)
{
  //GetParentFrame()->show();
  //if (myGUIProg->inNewHandler)
  //  return;

  inUpdate = true;
  CLavaPEHint* Hint = (CLavaPEHint*)pHint;
  if (Hint
        && ((Hint->com == CPECommand_Delete)
        ||  (Hint->com == CPECommand_Insert)
        ||  (Hint->com == CPECommand_Change)
        ||  (Hint->com == CPECommand_FromOtherDoc))
        || (undoRedoCheck == 3) || (undoRedoCheck > CHLV_showError)) {
    //LavaPE
    if ((undoRedoCheck == 1) && Hint && Hint->FirstLast.Contains(firstHint)
        || (undoRedoCheck != 1) && Hint && Hint->FirstLast.Contains(lastHint)
        || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_showError)) {
      myGUIProg->newFocus = 0;
      myDECL = GetDocument()->IDTable.GetDECL(myID);
      if (!myDECL)
        return;
      ((CLavaGUIFrame*)GetParentFrame())->NewTitle(myDECL, GetDocument()->IDTable.DocName);
      LastBrowseNode = 0;
      myGUIProg->OnUpdate(myDECL, 0);
      myGUIProg->MakeGUI.DisplayScreen(false);
    }
  }
  else
    if (Hint && (Hint->com == CPECommand_OpenFormView)) {
      if (!LBaseData->inRuntime) {
        //LavaPE
        ServicePtr = 0;
        myDECL = (LavaDECL*)Hint->CommandData1;
      }
      myID = TID(myDECL->OwnID, 0);
      LastBrowseNode = 0;
      myGUIProg->OnUpdate(myDECL, ResultDPtr);
      //if (LBaseData->inRuntime)
      myGUIProg->MakeGUI.DisplayScreen(false);
    }
  inUpdate = false;

}

bool CLavaGUIView::event(QEvent* ev)
{
  if (ev->type() == UEV_LavaPE_CalledView) {
    wxDocManager::GetDocumentManager()->SetActiveView(this);
    if (myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget)
      myGUIProg->focNode->data.FIP.widget->setFocus();
    //myGUIProg->MakeGUI.CursorOnField(myGUIProg->focNode);
    myGUIProg->inSynchTree = false;
    return true;
  }
  else if (ev->type() == UEV_LavaPE_SyncForm) {
    SyncForm((LavaDECL*)((CustomEvent*)ev)->data());
    return true;
  }
  else {
    if (ev->type() == QEvent::Enter)
      MessToStatusbar();
                return wxView::event(ev);
  }
}

void CLavaGUIView::resetLastBrowseNode()
{
  if (LastBrowseNode && LastBrowseNode->data.FIP.widget
    && LastBrowseNode->data.FIP.widget->inherits("QFrame")) {
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShape(QFrame::NoFrame);
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShadow(QFrame::Plain);

    QPalette p = ((QFrame*)LastBrowseNode->data.FIP.widget)->palette();
    p.setColor(QPalette::Active, QPalette::WindowText, ((CFormWid*)LastBrowseNode->data.FIP.widget)->palette().color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Inactive, QPalette::WindowText, ((CFormWid*)LastBrowseNode->data.FIP.widget)->palette().color(QPalette::Active, QPalette::WindowText));
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setPalette(p);
//    ((QFrame*)LastBrowseNode->data.FIP.widget)->setPaletteForegroundColor(((CFormWid*)LastBrowseNode->data.FIP.widget)->ForegroundColor);

    ((QFrame*)LastBrowseNode->data.FIP.widget)->repaint();
    LastBrowseNode = 0;
  }

}

void CLavaGUIView::SyncForm(LavaDECL* selDECL)
{
  if (!inUpdate && !myGUIProg->inSynchTree) {
    resetLastBrowseNode();
    if (scrollView()->MaxBottomRight.bottom()) {
      LastBrowseNode = myGUIProg->LavaForm.BrowseForm(myGUIProg->Root, selDECL);
      if (LastBrowseNode) {
        if (LastBrowseNode->data.FIP.widget && LastBrowseNode->data.FIP.widget->inherits("CFormWid")) {
          //myGUIProg->setFocNode(myGUIProg->TreeSrch.NextUnprotected (LastBrowseNode, LastBrowseNode));
          ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShape(QFrame::Box );
          ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShadow(QFrame::Plain);

          QPalette p = ((QFrame*)LastBrowseNode->data.FIP.widget)->palette();
          p.setColor(QPalette::Active, QPalette::WindowText, QColor("#FF0000"));
          p.setColor(QPalette::Inactive, QPalette::WindowText,QColor("#FF0000"));
          ((QFrame*)LastBrowseNode->data.FIP.widget)->setPalette(p);

//     ((QFrame*)LastBrowseNode->data.FIP.widget)->setPaletteForegroundColor(QColor("#FF0000"));
         myGUIProg->ScrollIntoFrame(LastBrowseNode->data.FIP.widget);
          ((QFrame*)LastBrowseNode->data.FIP.widget)->repaint();
          //myGUIProg->MakeGUI.CursorOnField(myGUIProg->focNode);
        }
      }
      else
        LastBrowseNode = 0;
    }
    else
      myGUIProg->selDECL = selDECL;
  }
  else
    myGUIProg->selDECL = selDECL;
  myGUIProg->inSyncForm = false;
}


void CLavaGUIView::OnUpdateGotodef(QAction* action)
{
  action->setEnabled(myGUIProg && myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget
                    && myGUIProg->focNode->data.FIP.widget->hasFocus());
}

void CLavaGUIView::OnGotoDecl()
{
  TID id;
  LavaDECL *fDecl = myGUIProg->focNode->data.FormSyntax;
  if (fDecl->DeclType == PatternDef)
    fDecl = fDecl->ParentDECL;
  else {
    if ((fDecl->OwnID == -1) && myGUIProg->focNode->data.FIP.widget) {
      CFormWid* radio = 0;
      CHEFormNode *node, *iterNode = 0;
      if (myGUIProg->focNode->data.FIP.widget->inherits("CPushButton")) {
        radio = (CFormWid*)((CPushButton*)myGUIProg->focNode->data.FIP.widget)->Radio;
        iterNode = ((CPushButton*)myGUIProg->focNode->data.FIP.widget)->IterNode;
      }
      else if (myGUIProg->focNode->data.FIP.widget->inherits("CRadioButton"))
        radio = (CFormWid*)((CRadioButton*)myGUIProg->focNode->data.FIP.widget)->Radio;
      if (radio) {
        for (node = radio->myFormNode;
             node && (node->data.FormSyntax->DeclType != Attr)
                  && (node->data.FormSyntax->DeclType != PatternDef)
                  && (node->data.FormSyntax->DeclType != VirtualType);
             node = node->data.FIP.up);
      }
      else
        node = iterNode;
      if (node) {
        fDecl = node->data.FormSyntax;
        if (fDecl->DeclType == PatternDef)
          fDecl = fDecl->ParentDECL;
        else
          fDecl = node->data.FormSyntax;
      }
    }
  }
  id = TID(fDecl->OwnID, fDecl->inINCL);
  LBaseData->Browser->LastBrowseContext = new CBrowseContext(this, myGUIProg->focNode);
  LBaseData->Browser->BrowseDECL(GetDocument(), id);
}

void CLavaGUIView::OnOK()
{
  LavaObjectPtr obj;
  if (released)
    return;
  NoteLastModified();
  while (myGUIProg->allocatedObjects.count()) {
    obj = myGUIProg->allocatedObjects.takeFirst();
    DEC_FWD_CNT(myGUIProg->ckd,obj);
  }
}


void CLavaGUIView::OnReset()
{
  bool ok = true;;
  if (myGUIProg) {
    myGUIProg->NoteLastModified();
    if (*ResultDPtr) {
      DEC_FWD_CNT(myGUIProg->ckd,*ResultDPtr);
      *ResultDPtr = 0;
    }
    if (myGUIProg->fromFillIn)
      *ResultDPtr = AllocateObject(myGUIProg->ckd, (*ServicePtr)[0][0].classDECL->RelatedDECL, false);
    if ((*ResultDPtr || !myGUIProg->fromFillIn) && !myGUIProg->ckd.exceptionThrown) {
      if (*IniDataPtr) {
        try {
#ifndef WIN32
          if (setjmp(contOnHWexception)) throw hwException;
#endif
          if (myGUIProg->fromFillIn)
            myGUIProg->ex = CopyObject(myGUIProg->ckd, IniDataPtr, ResultDPtr, ((SynFlags*)((*IniDataPtr)+1))->Contains(stateObjFlag), (*ServicePtr)[0][0].classDECL->RelatedDECL);
          else
            myGUIProg->ex = CopyObject(myGUIProg->ckd, IniDataPtr, ResultDPtr, ((SynFlags*)((*IniDataPtr)+1))->Contains(stateObjFlag));
          if (myGUIProg->ex)
            ok = false;
          if (myGUIProg->ckd.exceptionThrown)
            ok = false;
        }
        catch (CRuntimeException ex) {
          if (!ex.SetLavaException(myGUIProg->ckd))
            //throw;
          ok = false;
        }
        catch (CHWException ex) {
          if (!ex.SetLavaException(myGUIProg->ckd))
            //throw;
          ok = false;
        }
        ok = ok && !myGUIProg->ckd.exceptionThrown && !myGUIProg->ex;
      }
      if (ok && *ResultDPtr) {
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        myGUIProg->MakeGUI.DisplayScreen(false);
        return;
      }
    }
  }
  OnCancel(); //return to calling program if exceptions appear}
}


void CLavaGUIView::OnCancel()

{
  if (released)
    return;
  bool ok = true;;
  if (myGUIProg && LBaseData->inRuntime) {
    if (GetDocument()->isObject) {
      ((CLavaGUIFrame*)GetParentFrame())->SetModified(GetDocument()->GetTitle(), false);
      GetDocument()->Modify(false);
    }
    NoteLastModified();
    if (*ResultDPtr) {
      DEC_FWD_CNT(myGUIProg->ckd,*ResultDPtr);
      *ResultDPtr = 0;
    }
  }
  OnOK(); //return to calling program if exceptions appear
}


void CLavaGUIView::OnUpdateCancel(QAction* action)
{
  action->setEnabled(LBaseData->inRuntime && GetDocument()->isObject); // && !GetDocument()->IsEmbedded());
}


void CLavaGUIView::NoteLastModified()
{
  if (myGUIProg)
    myGUIProg->NoteLastModified();
}

void CLavaGUIView::NoteNewObj(LavaObjectPtr obj)
{
  *IniDataPtr = obj;
  //OnCancel();
}


void CLavaGUIView::OnDeleteOpt()
{
  if (released)
    return;
  if (myGUIProg && myGUIProg->DelNode) {
    if (myGUIProg->DelNode->data.IterFlags.Contains(Optional))
      myGUIProg->CmdExec.DeleteOptionalItem(myGUIProg->DelNode);
    else
      myGUIProg->CmdExec.DeleteIterItem(myGUIProg->DelNode);
    myGUIProg->DelNode = 0;
  }
}


void CLavaGUIView::OnUpdateDeleteopt(QAction* action)
{
  if (myGUIProg)
    myGUIProg->OnUpdateDeleteopt(action);
}


void CLavaGUIView::OnInsertOpt()
{
  if (released)
    return;
  if (myGUIProg && myGUIProg->InsertNode) {
    myGUIProg->CmdExec.InsertIterItem(myGUIProg->InsertNode);
  }
}

void CLavaGUIView::OnUpdateInsertopt(QAction* action)
{
  if (myGUIProg) 
    myGUIProg->OnUpdateInsertopt(action);
}

void CLavaGUIView::OnTogglestate()
{
  if (LBaseData->inRuntime && GetDocument()->isObject) { // && !GetDocument()->IsEmbedded()) {
    if (*ResultDPtr)
      ToggleObjectCat(*ResultDPtr);
    if (*ResultDPtr)
      CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
    OnModified();
  }
}

void CLavaGUIView::OnUpdateTogglestate(QAction* action)
{
  action->setChecked(CurrentCategory);
  action->setEnabled(LBaseData->inRuntime && GetDocument()->isObject); // && !GetDocument()->IsEmbedded());
}


void CLavaGUIView::OnModified()
{
  GetDocument()->Modify(GetDocument()->isObject);
  if (LBaseData->inRuntime && GetDocument()->isObject /*&& !GetDocument()->IsEmbedded()*/)
    ((CLavaGUIFrame*)GetParentFrame())->SetModified(GetDocument()->GetTitle(), true);
  //GetDocument()->UpdateAllItems(0);
}


bool CLavaGUIView::OnKill()
{
  return true;
}

void CLavaGUIView::MessToStatusbar()
{
  if (LBaseData->inRuntime && GetDocument()->isObject) {
    if (StatusbarMess.isEmpty()) {
      StatusbarMess = "Lava object defined by lava component in file ";
      StatusbarMess += GetDocument()->IDTable.IDTab[0]->LinkName.c;
      if (GetDocument()->IDTable.IDTab[0]->LinkName != GetDocument()->PathName) {
        StatusbarMess += " -> ";
        StatusbarMess += GetDocument()->PathName.c;
      }
    }
    wxTheApp->m_appWindow->statusBar()->showMessage(StatusbarMess);
  }
}

void CLavaGUIView::OnActivateView(bool bActivate, wxView *deactiveView)
{
  if (myGUIProg) {
    if (bActivate) {
      active = true;
      if (LBaseData->inRuntime) {
        MessToStatusbar();
      }
      else
        GetDocument()->ResetError();
      if (myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget)
        if (!myGUIProg->focNode->data.FIP.widget->hasFocus())
          myGUIProg->focNode->data.FIP.widget->setFocus();
      clipboard_text_notEmpty = !QApplication::clipboard()->text().isEmpty();
      wxTheApp->updateButtonsMenus();
    }
    else {
      active = false;
      QString msg("");
      wxTheApp->m_appWindow->statusBar()->showMessage(msg);
      DisableActions();
    }
  }
}


void CLavaGUIView::OnChoosefont()
{
  if (myGUIProg) {
    myGUIProg->SetFont(&LBaseData->m_FormFont);
    myGUIProg->RedrawForm();
  }
}


void CLavaGUIView::setNewLabelFont()
{
  if (myGUIProg)
    myGUIProg->RedrawForm();
}


void CLavaGUIView::OnEditCopy()
{
  if (released)
    return;
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw ) {
      if (myGUIProg->editNode->data.BasicFlags.Contains(Text))
        ((CMultiLineEdit*)focw)->copy();
      else
        ((CTEdit*)focw)->copy();
      clipboard_text_notEmpty = !QApplication::clipboard()->text().isEmpty();
    }
  }
}


void CLavaGUIView::OnEditCut()
{
  if (released)
    return;
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw ) {
      if (myGUIProg->editNode->data.BasicFlags.Contains(Text))
        ((CMultiLineEdit*)focw)->cut();
      else
        ((CTEdit*)focw)->cut();
      clipboard_text_notEmpty = !QApplication::clipboard()->text().isEmpty();
    }
  }
}

void CLavaGUIView::OnEditPaste()
{
  if (released)
    return;
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw )
      if (myGUIProg->editNode->data.BasicFlags.Contains(Text))
        ((CMultiLineEdit*)focw)->paste();
      else
        ((CTEdit*)focw)->paste();
//      ((CTEdit*)focw)->paste();
  }
}


void CLavaGUIView::OnUpdateEditPaste(QAction* action)
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
                   && (myGUIProg->editNode == myGUIProg->focNode)
                   && ((myGUIProg->editNode->data.BasicFlags.Contains(Text)
                     && !((CMultiLineEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && clipboard_text_notEmpty
                   || !myGUIProg->editNode->data.BasicFlags.Contains(Text)
                     && !((CTEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && clipboard_text_notEmpty)));
}

void CLavaGUIView::OnUpdateEditCut(QAction* action)
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
                   && (myGUIProg->editNode == myGUIProg->focNode)
                   && (myGUIProg->editNode->data.BasicFlags.Contains(Text)
                     && !((CMultiLineEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && ((CMultiLineEdit*)myGUIProg->editNode->data.FIP.widget)->textCursor().hasSelection()
                   || !myGUIProg->editNode->data.BasicFlags.Contains(Text)
                     && !((CTEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && ((CTEdit*)myGUIProg->editNode->data.FIP.widget)->hasSelectedText())
                   );
}

void CLavaGUIView::OnUpdateEditCopy(QAction* action)
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
         && (myGUIProg->editNode == myGUIProg->focNode)
         && (myGUIProg->editNode->data.BasicFlags.Contains(Text)
           && ((CMultiLineEdit*)myGUIProg->editNode->data.FIP.widget)->textCursor().hasSelection()
         || !myGUIProg->editNode->data.BasicFlags.Contains(Text)
           && ((CTEdit*)myGUIProg->editNode->data.FIP.widget)->hasSelectedText())

                     );
}


