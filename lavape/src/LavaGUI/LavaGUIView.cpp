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


#ifdef __GNUC__
#pragma implementation
#endif


#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "SylTraversal.h"
#include "LavaAppBase.h"
#include "FormWid.h"
#include "TEdit.h"
#include "qpainter.h"
#include "qmessagebox.h"
#include "qclipboard.h"
#include "LavaBaseStringInit.h"
#include "qstatusbar.h"



/////////////////////////////////////////////////////////////////////////////
// CLavaGUIView

GUIScrollView::GUIScrollView(QWidget *parent, bool fromPopup) : QScrollView(parent, "GUIScrollView")
{
  setFocusPolicy(QWidget::StrongFocus);
  qvbox = new GUIVBox(0, fromPopup, this);//myScrv->viewport());
  addChild(qvbox);
  setResizePolicy(QScrollView::AutoOne);
  MaxBottomRight = QRect(0,0,0,0);
  
}


void GUIScrollView::viewportResizeEvent(QResizeEvent* ev)
{  
  QSize sz = MaxBottomRight.size();
  if (sz.width() < ev->size().width() )
    sz.setWidth(ev->size().width());
  if (sz.height() < ev->size().height() )
    sz.setHeight(ev->size().height());
  qvbox->resize(sz);
  resizeContents(sz.width(),sz.height());

  QScrollView::viewportResizeEvent(ev);
}


CLavaGUIView::CLavaGUIView(QWidget *parent,wxDocument *doc)
   : CLavaBaseView(parent,doc,"LavaGUIView")
{ 
  myScrv = new GUIScrollView(this, false);
  qvbox = myScrv->qvbox; 
  myDECL = 0;
  myGUIProg = 0;
	//m_pMemDC = 0;
	//m_pBmp = 0;
	//m_pOldBmp = 0;
  ServicePtr = 0;
  IniDataPtr = 0;
  ResultDPtr = 0;
  CurrentCategory = false;
//  myThread = 0;
  mainTree = 0;
  myTree = 0;
  //BitmapSize = QSize(0,0);
  LastBrowseNode = 0;
  inUpdate = true;
  StatusbarMess = "";
}


CLavaGUIView::~CLavaGUIView()
{
  if (myGUIProg) {
    if (!wxTheApp->apExit)
      myGUIProg->LavaForm.DeletePopups(myGUIProg->Root);
    delete myGUIProg;   
  }
  if (!wxTheApp->apExit) {
    QString msg("");
    wxTheApp->m_appWindow->statusBar()->message(msg);
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
  if (LBaseData->okActionPtr) {
	  OnUpdateTogglestate(LBaseData->toggleCatActionPtr);
    OnUpdateCancel(LBaseData->updateCancelActionPtr);
    OnUpdateOk(LBaseData->okActionPtr);
  }
  if (!LBaseData->inRuntime) 
    OnUpdateGotodef(LBaseData->gotoDeclActionPtr);
}

void CLavaGUIView::DisableActions()
{
  LBaseData->insActionPtr->setEnabled(false);
  LBaseData->delActionPtr->setEnabled(false);
  LBaseData->editCutActionPtr->setEnabled(false);
  LBaseData->editCopyActionPtr->setEnabled(false);
  LBaseData->editPasteActionPtr->setEnabled(false);
  if (LBaseData->okActionPtr) {
	  LBaseData->toggleCatActionPtr->setEnabled(false);
    LBaseData->updateCancelActionPtr->setEnabled(false);
    LBaseData->okActionPtr->setEnabled(false);
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
      if (LBaseData->inRuntime) { //lava task only
        myDECL = (*(LavaObjectPtr)pHint->CommandData1)->implDECL;
        //if (!GetDocument()->IsEmbedded())
        ((CLavaGUIFrame*)GetParentFrame())->NewTitle(myDECL, GetDocument()->IDTable.DocName);
        myID = TID(myDECL->OwnID, 0);
        ServicePtr = (LavaVariablePtr)pHint->CommandData1;
        IniDataPtr = (LavaVariablePtr)pHint->CommandData2;
        ResultDPtr = (LavaVariablePtr)pHint->CommandData3;
        if (*ResultDPtr) 
          CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
        myGUIProg->FrozenObject = (int)pHint->CommandData4;
        myThread = (CLavaThread*)pHint->CommandData5;
        myGUIProg->fromFillIn = (int)pHint->CommandData6;
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        //if (!GetDocument()->IsEmbedded())
          wxTheApp->mainWidget()->showMaximized();
      }
      else {
        myDECL = (LavaDECL*)pHint->CommandData1;
        for ( ; myDECL && (myDECL->DeclType == VirtualType); myDECL = GetDocument()->IDTable.GetDECL(myDECL->RefID, myDECL->inINCL));
        myID = TID(myDECL->OwnID, myDECL->inINCL);
        mainTree = (CLavaBaseView*)pHint->CommandData2; //LavaPE only
        LastBrowseNode = 0;
        myGUIProg->selDECL = myDECL;
        //myGUIProg->OnUpdate(myDECL, (LavaVariablePtr)pHint->CommandData3);
        OnUpdate(this, 0, pHint);
      }
    }
    else { //show Lava object autonom
      if (LBaseData->inRuntime && GetDocument()->isObject && GetDocument()->DocObjects[2]) {
        ServicePtr = &GetDocument()->DocObjects[0];
        IniDataPtr = &GetDocument()->DocObjects[1];
        ResultDPtr = &GetDocument()->DocObjects[2];
        if (*ResultDPtr)
          CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
        myGUIProg->FrozenObject = 0;
        myGUIProg->fromFillIn = 1;
        myDECL = (*ServicePtr)[0][0].implDECL;
        //if (!GetDocument()->IsEmbedded())
          ((CLavaGUIFrame*)GetParentFrame())->setCaption(GetDocument()->GetTitle());
        myID = TID(myDECL->OwnID, myDECL->inINCL);
        LastBrowseNode = 0;
        MessToStatusbar();
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        myGUIProg->MakeGUI.DisplayScreen(false);
      }
    }
  }
  else {
    if (LBaseData->inRuntime && GetDocument()->isObject) {
      GetDocument()->SelectLcom(true);
      myGUIProg = new CGUIProg;
      MessToStatusbar();
      myGUIProg->Create(GetDocument(), this);
      ServicePtr = &GetDocument()->DocObjects[0];
      IniDataPtr = &GetDocument()->DocObjects[1];
      ResultDPtr = &GetDocument()->DocObjects[2];
      if (*ResultDPtr)
        CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
      myGUIProg->FrozenObject = 0;
      myGUIProg->fromFillIn = 1;
      myDECL = (*ServicePtr)[0][0].implDECL;
      //if (!GetDocument()->IsEmbedded())
        ((CLavaGUIFrame*)GetParentFrame())->setCaption(GetDocument()->GetTitle());
      myID = TID(myDECL->OwnID, myDECL->inINCL);
      LastBrowseNode = 0;
      myGUIProg->OnUpdate( myDECL, ResultDPtr);
      myGUIProg->MakeGUI.DisplayScreen(false);
    }
  } 
}

void CLavaGUIView::OnUpdate(wxView* , unsigned undoRedoCheck, QObject* pHint) 
{ 
  inUpdate = true;
  CLavaPEHint* Hint = (CLavaPEHint*)pHint;
  if (Hint //&& Hint->CommandData1 
        && ((Hint->com == CPECommand_Delete)
        ||  (Hint->com == CPECommand_Insert)
        ||  (Hint->com == CPECommand_Change)
        ||  (Hint->com == CPECommand_FromOtherDoc))
      || (undoRedoCheck == 3) || (undoRedoCheck > CHLV_showError)) {
    if ((undoRedoCheck == 1) && Hint && Hint->FirstLast.Contains(firstHint)
        || (undoRedoCheck != 1) && Hint && Hint->FirstLast.Contains(lastHint)
        || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_showError)) {
      myGUIProg->newFocus = 0;
      myDECL = GetDocument()->IDTable.GetDECL(myID);
      if (!myDECL)
        return;
      //if (!GetDocument()->IsEmbedded())
        ((CLavaGUIFrame*)GetParentFrame())->NewTitle(myDECL, GetDocument()->IDTable.DocName);
       LastBrowseNode = 0;
       myGUIProg->OnUpdate(myDECL, 0);
       myGUIProg->MakeGUI.DisplayScreen(false);
    }
  }
  else
    if (Hint && (Hint->com == CPECommand_OpenFormView)) {
      if (LBaseData->inRuntime) {
        ServicePtr = (LavaVariablePtr)Hint->CommandData1;
        IniDataPtr = (LavaVariablePtr)Hint->CommandData2;
        ResultDPtr = (LavaVariablePtr)Hint->CommandData3;
        if (*ResultDPtr)
          CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
        myDECL = (*ServicePtr)[0]->implDECL;
        myGUIProg->FrozenObject = (int)Hint->CommandData4;
        myThread = (CLavaThread*)Hint->CommandData5;
        myGUIProg->fromFillIn = (int)Hint->CommandData6;
        //if (!GetDocument()->IsEmbedded())
          ((CLavaGUIFrame*)GetParentFrame())->NewTitle(myDECL, GetDocument()->IDTable.DocName);
      }
      else {
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
  if (ev->type() == IDU_LavaPE_CalledView) {
    wxDocManager::GetDocumentManager()->SetActiveView(this);
    if (myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget)
      myGUIProg->focNode->data.FIP.widget->setFocus();
    //myGUIProg->MakeGUI.CursorOnField(myGUIProg->focNode); 
    myGUIProg->inSynchTree = false;
    return true;
  }
  else if (ev->type() == IDU_LavaPE_SyncForm) {
    SyncForm((LavaDECL*)((QCustomEvent*)ev)->data());
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
  if (LastBrowseNode && LastBrowseNode->data.FIP.widget) {
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShape(QFrame::NoFrame);
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShadow(QFrame::Plain);
    ((QFrame*)LastBrowseNode->data.FIP.widget)->setPaletteForegroundColor(((CFormWid*)LastBrowseNode->data.FIP.widget)->ForegroundColor);
    ((QFrame*)LastBrowseNode->data.FIP.widget)->repaint();
    LastBrowseNode = 0;
  }

}

void CLavaGUIView::SyncForm(LavaDECL* selDECL)
{
  if (!inUpdate && !myGUIProg->inSynchTree) {
    /*
    if (myGUIProg->FocusPopup) 
      setFocus();
      */
    //if (LastBrowseNode && LastBrowseNode->data.IoSigFlags.Contains(setViewBorder)) {
    //  LastBrowseNode->data.IoSigFlags.EXCL(setViewBorder);
    //}
    resetLastBrowseNode();
    if (scrollView()->MaxBottomRight.bottom()) {
      LastBrowseNode = myGUIProg->LavaForm.BrowseForm(myGUIProg->Root, selDECL);
      if (LastBrowseNode) {
        if (LastBrowseNode->data.FIP.widget && LastBrowseNode->data.FIP.widget->inherits("CFormWid")) {
          //myGUIProg->focNode = myGUIProg->TreeSrch.NextUnprotected (LastBrowseNode, LastBrowseNode);
          ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShape(QFrame::Box );
          ((QFrame*)LastBrowseNode->data.FIP.widget)->setFrameShadow(QFrame::Plain);
     ((QFrame*)LastBrowseNode->data.FIP.widget)->setPaletteForegroundColor(QColor("#FF0000"));
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


void CLavaGUIView::OnUpdateGotodef(wxAction* action)
{
  action->setEnabled(myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget
                    && myGUIProg->focNode->data.FIP.widget->hasFocus());
}

void CLavaGUIView::OnGotodef()
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
  NoteLastModified();
  if (LBaseData->inRuntime && !GetDocument()->isObject)// && !GetDocument()->IsEmbedded())
    if (myGUIProg && myGUIProg->LavaForm.OnOK( myGUIProg->Root))
      if (myThread && myThread->pContExecEvent) {
        myThread->pContExecEvent->lastException = myGUIProg->ckd.lastException;
        myThread->pContExecEvent->ex = myGUIProg->ex;
        myGUIProg->ckd.lastException = 0;
        myGUIProg->ckd.exceptionThrown = false;
        (*myThread->pContExecEvent)--; // release semaphore, -> client thread resumes
      }
}

void CLavaGUIView::OnUpdateOk(wxAction* action) 
{
  action->setEnabled(LBaseData->inRuntime && !GetDocument()->isObject);// && !GetDocument()->IsEmbedded());
}

void CLavaGUIView::OnCancel()
{
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
        catch (CRuntimeException* ex) {
          if (!ex->SetLavaException(myGUIProg->ckd)) 
            throw;
          ok = false;
        }
        catch (CHWException* ex) {
          if (!ex->SetLavaException(myGUIProg->ckd)) 
            throw;
          ok = false;
        }
        ok = ok && !myGUIProg->ckd.exceptionThrown && !myGUIProg->ex;
      }
      if (ok && *ResultDPtr) {
        CurrentCategory = ((SynFlags*)((*ResultDPtr)+1))->Contains(stateObjFlag);
        LastBrowseNode = 0;
        myGUIProg->OnUpdate( myDECL, ResultDPtr);
        myGUIProg->MakeGUI.DisplayScreen(false);
        return;
      }
    }
  }
  OnOK(); //return to calling program if exceptions appear
}


void CLavaGUIView::OnUpdateCancel(wxAction* action) 
{
  action->setEnabled(LBaseData->inRuntime); // && !GetDocument()->IsEmbedded());
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
  if (myGUIProg && myGUIProg->DelNode) {
    if (myGUIProg->DelNode->data.IterFlags.Contains(Optional))
      myGUIProg->CmdExec.DeleteOptionalItem(myGUIProg->DelNode);  
    else
      myGUIProg->CmdExec.DeleteIterItem(myGUIProg->DelNode);  
    myGUIProg->DelNode = 0;
  }
}


void CLavaGUIView::OnUpdateDeleteopt(wxAction* action) 
{
  if (myGUIProg && myGUIProg->focNode && myGUIProg->focNode->data.FIP.frameWidget
      && myGUIProg->focNode->data.FIP.widget
      && !myGUIProg->focNode->data.IoSigFlags.Contains(DONTPUT)
      && myGUIProg->focNode->data.IoSigFlags.Contains(Flag_INPUT)) {
    //CFrameWnd *frm = myGUIProg->focNode->data.FIP.widget->GetParentFrame();
    QWidget* delWindow = myGUIProg->focNode->data.FIP.frameWidget;
    myGUIProg->DelNode = myGUIProg->focNode;
    while (myGUIProg->DelNode && (delWindow != this)) {// && (delWindow != frm)) {
      delWindow = myGUIProg->DelNode->data.FIP.frameWidget;
      if (myGUIProg->DelNode->data.IterFlags.Contains(Optional)
          || myGUIProg->DelNode->data.IterFlags.Contains(IteratedItem)
             && !myGUIProg->DelNode->data.IterFlags.Contains(FixedCount)) {
        action->setEnabled(true);
        return;
      }
      myGUIProg->DelNode = myGUIProg->DelNode->data.FIP.up;
    }
  }
  if (myGUIProg)
    myGUIProg->DelNode = 0;
  action->setEnabled(false);
}


void CLavaGUIView::OnInsertOpt() 
{
  if (myGUIProg && myGUIProg->InsertNode) {
    myGUIProg->CmdExec.InsertIterItem(myGUIProg->InsertNode);
  }
}


void CLavaGUIView::OnUpdateInsertopt(wxAction* action) 
{
  if (myGUIProg) {
    if (myGUIProg->focNode && myGUIProg->focNode->data.FIP.frameWidget
      && myGUIProg->focNode->data.FIP.widget
        && !myGUIProg->focNode->data.IoSigFlags.Contains(DONTPUT)
        && myGUIProg->focNode->data.IoSigFlags.Contains(Flag_INPUT)) {
      //CFrameWnd *frm = myGUIProg->focNode->data.FIP.widget->GetParentFrame();
      QWidget* insertWindow = myGUIProg->focNode->data.FIP.frameWidget;
      myGUIProg->InsertNode = myGUIProg->focNode;
      while (myGUIProg->InsertNode && (insertWindow != this)) {// && (insertWindow != frm)) {
        insertWindow = myGUIProg->InsertNode->data.FIP.frameWidget;
        if (myGUIProg->InsertNode->data.IterFlags.Contains(IteratedItem)
            && !myGUIProg->InsertNode->data.IterFlags.Contains(FixedCount)) {
          action->setEnabled(true);
          return;
        }
        myGUIProg->InsertNode = myGUIProg->InsertNode->data.FIP.up;
      }
    }
    myGUIProg->InsertNode = 0;
  }
  action->setEnabled(false);
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

void CLavaGUIView::OnUpdateTogglestate(wxAction* action) 
{
  action->setOn(CurrentCategory);
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
  if (myGUIProg && LBaseData->inRuntime && !GetDocument()->isObject) {
    if (ResultDPtr && *ResultDPtr) {
      if (QMessageBox::question(
            qApp->mainWidget(),qApp->name(),"Do you really want to abort this Lava program?",
            QMessageBox::Yes,
            QMessageBox::No) == QMessageBox::Yes) {
        /*
        DEC_FWD_CNT(myGUIProg->ckd,*ResultDPtr);
        *ResultDPtr = 0;
        */
        if (myThread && myThread->pContExecEvent) {
          if (!myGUIProg->ckd.lastException)
            SetLavaException(myGUIProg->ckd, CanceledForm_ex, ERR_CanceledForm);
          myThread->pContExecEvent->lastException = myGUIProg->ckd.lastException;
          myGUIProg->ckd.lastException = 0;
          myGUIProg->ckd.exceptionThrown = false;
          (*myThread->pContExecEvent)--;
        }
      }
      return false;
    }
  }
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
    wxTheApp->m_appWindow->statusBar()->message(StatusbarMess);
  }
}

void CLavaGUIView::OnActivateView(bool bActivate, wxView *deactiveView) 
{
  if (myGUIProg) {
    if (bActivate) {
      if (LBaseData->inRuntime) {
        MessToStatusbar();
      }
      else
        GetDocument()->ResetError();
      if (myGUIProg->focNode && myGUIProg->focNode->data.FIP.widget)
        if (!myGUIProg->focNode->data.FIP.widget->hasFocus())
          myGUIProg->focNode->data.FIP.widget->setFocus();
    }
    else {
      QString msg("");
      wxTheApp->m_appWindow->statusBar()->message(msg);
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

/*
void CLavaGUIView::setNewButtonFont()
{
  if (myGUIProg) 
    myGUIProg->RedrawForm();
}
*/

void CLavaGUIView::OnEditCopy()
{
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw )
      ((CTEdit*)focw)->copy();
  }
}


void CLavaGUIView::OnEditCut()
{
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw )
      ((CTEdit*)focw)->cut();
  }
}

void CLavaGUIView::OnEditPaste()
{
  if (myGUIProg && myGUIProg->editNode && (myGUIProg->editNode == myGUIProg->focNode)) {
    QWidget* focw = myGUIProg->focNode->data.FIP.widget;
    if (focw )
      ((CTEdit*)focw)->paste();
  }
}


void CLavaGUIView::OnUpdateEditPaste(wxAction* action) 
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
                     && (myGUIProg->editNode == myGUIProg->focNode)
                     && !((CTEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && !QApplication::clipboard()->text().isEmpty());
}

void CLavaGUIView::OnUpdateEditCut(wxAction* action) 
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
                     && (myGUIProg->editNode == myGUIProg->focNode)
                     && !((CTEdit*)myGUIProg->editNode->data.FIP.widget)->isReadOnly()
                     && ((CTEdit*)myGUIProg->editNode->data.FIP.widget)->hasSelectedText()
                     );
}

void CLavaGUIView::OnUpdateEditCopy(wxAction* action) 
{
  action->setEnabled (myGUIProg && myGUIProg->editNode
                     && (myGUIProg->editNode == myGUIProg->focNode)
                     && ((CTEdit*)myGUIProg->editNode->data.FIP.widget)->hasSelectedText()
                     );
}
  


/*
void CLavaGUIView::OnDestroy() 
{
  GetDocument()->RuntimeView = 0;
  if (myGUIProg)
    delete myGUIProg; 
}*/

/*
void CLavaGUIView::OnDraw() 
{
  if (myGUIProg) { 
    if (GetDocument()->Redraw) {
      myGUIProg->RedrawForm();
      GetDocument()->Redraw = false;
    }
    else
      myGUIProg->MakeGUI.DisplayScreen(false);
    myGUIProg->MakeGUI.CursorOnField(myGUIProg->focNode); 
    //if (myGUIProg->newFocus && myGUIProg->newFocus->m_hWnd)
    //  myGUIProg->newFocus->SetFocus();
  }
  if (myGUIProg->inSyncForm) {
    myGUIProg->inSyncForm = false;
//    myTree->SetFocus();
  }
//  inUpdate = false;
}

*/

/*

void CLavaGUIView::GetActViewSize(QSize& size) 
{
  QRect rect;
  GetClientRect(&rect);
  size = rect.Size();
}

bool CLavaGUIView::OnSetCursor(QWidget* pWnd, UINT nHitTest, UINT message) 
{
  if (!LBaseData->inRuntime)
    SetCursor(LBaseData->ArrowCurser);
  return false;
}


void CLavaGUIView::SetScreenshot()
{
  if (!GetDocument()->isObject || !GetDocument()->IsEmbedded())
    return;
	QSize size, sizeClient;

  if (m_pMemDC != NULL) { //replace old picture
		m_pMemDC->SelectObject(m_pOldBmp); // select out the bitmap
		delete m_pBmp; //??
		m_pBmp = 0; //??
 
		delete m_pMemDC;
		m_pMemDC = NULL;			   
	}

	CDC *dc = GetDC();
  QPoint point =  GetScrollPosition();
  GetActViewSize(BitmapSize);
	m_pMemDC = new CDC;
  if (!m_pMemDC->CreateCompatibleDC(dc))
    GetDocument()->LavaError(myGUIProg->ckd, true, 0, &ERR_NoPicture, 0);
	m_pBmp = new CBitmap;
  if (!m_pBmp->CreateDiscardableBitmap(dc, BitmapSize.cx, BitmapSize.cy)) {
    GetDocument()->LavaError(myGUIProg->ckd, true, 0, &ERR_NoPicture, 0);
    return;
  }

	m_pOldBmp = m_pMemDC->SelectObject(m_pBmp);
  if (m_pOldBmp == NULL) {
    GetDocument()->LavaError(myGUIProg->ckd, true, 0, &ERR_NoPicture, 0);
    return;
  }
	
	// Get the screen picture into the memory DC
//  if (!m_pMemDC->StretchBlt(0, 0, sizeClient.cx, sizeClient.cy, dc, 0, 0, size.cx, size.cy, SRCCOPY ))
  if (!m_pMemDC->BitBlt(0, 0,  BitmapSize.cx, BitmapSize.cy, dc,0, 0, SRCCOPY)) {
    GetDocument()->LavaError(myGUIProg->ckd, true, 0, &ERR_NoPicture, 0);
    return;
  }

	//Notify the server item that a change has been made that
	// needs to be reflected in the metafile
	((COleServerItem*)(GetDocument()->GetEmbeddedItem()))->NotifyChanged();
}
*/
