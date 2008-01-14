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
#include "LavaGUIPopup.h"
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"

#include "TEdit.h"
#include "mdiframes.h"

#include <QScrollBar>
#include <QAction>

//Added by qt3to4:

#pragma hdrstop


CGUIProg::CGUIProg() : CGUIProgBase()
{
  newFocus = 0;
  Root = 0;
  MPTR = 0;
  butNode = 0;
  editNode = 0;
  focNode = 0;
  ActNode = 0;
  CurPTR = 0;
  FocusPopup = 0;
  popUps = 0;
  CurPos = 0;
  refresh = true;
  Warning = 0;
  InCommandAgent = false;
  allowIncompleteForms = false;
  ErrorCode = NONE;
  ErrPos = 0;
  FrozenObject = 0;
  fromFillIn = 0;
  Redraw = false;
  ServicePtr = 0;
  inNewHandler = false;
}

void CGUIProg::Create(CLavaBaseDoc* doc, QWidget* view)
{
  Root = 0;
  myDoc = doc;
  ckd.document = myDoc;
  ex = 0;
  mySynDef = doc->mySynDef;
  Conv.INIT(this);
  CmdExec.INIT(this);
  LavaForm.INIT(this);
  MakeGUI.INIT(this);
  TreeSrch.INIT(this);
  ViewWin = view;
  isView = view->inherits("CLavaGUIView");
  if (view->inherits("CLavaGUIView"))
    scrView = ((CLavaGUIView*)view)->scrollView();
  else
    scrView = ((LavaGUIDialog*)view)->scrollView();
  CGUIProgBase::Create();
  delActionPtr = new QAction(/*LBaseData->delActionPtr->icon(),*/ LBaseData->delActionPtr->text(),LBaseData->delActionPtr->parent());
  insActionPtr = new QAction(/*LBaseData->insActionPtr->icon(),*/ LBaseData->insActionPtr->text(),LBaseData->insActionPtr->parent());
  newHandlerActionPtr = new QAction(QString("New handler"), LBaseData->insActionPtr->parent());
  attachHandlerActionPtr = new QAction(QString("Attach existing handler"), LBaseData->insActionPtr->parent());
}

void CGUIProg::OnUpdate( LavaDECL* decl, LavaVariablePtr resultPtr)
{
  newFocus = 0;
  FormName = decl->FullName;  // forS in RequestEval
  if (Root) {
    LavaForm.DeleteWindows(Root, false);
    LavaForm.DeleteForm(Root);
  }
  Root = 0;
  MPTR = 0;
  editNode = 0;
  butNode = 0;
  focNode = 0;
  ActNode = 0;
  CurPTR = 0;
  FocusPopup = 0;
  popUps = 0;
  CurPos = 0;
  refresh = true;
  Warning = 0;
  InCommandAgent = false;
  allowIncompleteForms = false;
  ErrorCode = NONE;
  ErrPos = 0;
  if (myDoc->GetFormpDECL(decl))
    LavaForm.MakeForm (*myDoc->GetFormpDECL(decl), resultPtr, Root);
}

void CGUIProg::NoteLastModified()
{
  if (focNode) {
    QWidget* focw = focNode->data.FIP.widget;
    if (focw  && (focw->inherits("CTEdit") || focw->inherits("CMultiLineEdit"))) {
      ((CTEdit*)focw)->clearFocus();
      focNode = 0;
    }
  }
}

void CGUIProg::SyncTree(CHEFormNode *node, bool noSync)
{
  CHEFormNode *upNode;
  HandlerInfos *handlerChain;
  CHEHandlerInfo *chehand, *cheCop;

  
  if (LBaseData->inRuntime)
    return;
  handlerChain = new HandlerInfos;
  *handlerChain = node->data.myHandler;
  upNode = node->data.FIP.up;
  while (upNode) {
    if (upNode->data.myHandler.last && (!handlerChain->last
        || (((CHEHandlerInfo*)upNode->data.myHandler.last)->data.HandlerID != ((CHEHandlerInfo*)handlerChain->last)->data.HandlerID))) {
      chehand = (CHEHandlerInfo*)upNode->data.myHandler.first;
      while (chehand) {
        cheCop = new CHEHandlerInfo;
        cheCop->data = chehand->data;
        handlerChain->Append(cheCop);
        chehand = (CHEHandlerInfo*)chehand->successor;
      }
    }
    upNode = upNode->data.FIP.up;
  }
  if (!inSynchTree && !noSync && !((CLavaGUIFrame*)((CLavaGUIView*)ViewWin)->GetParentFrame())->onClose)
    if (!inSyncForm) {
      if (((CLavaGUIView*)ViewWin)->LastBrowseNode)
        ((CLavaGUIView*)ViewWin)->resetLastBrowseNode();
      for (upNode = node; upNode && !upNode->data.FormSyntax->ParentDECL;upNode = upNode->data.FIP.up);
      for (;
        upNode && upNode->data.FormSyntax->ParentDECL && (upNode->data.FormSyntax->ParentDECL != myDECL)&& (upNode->data.FormSyntax->ParentDECL->ParentDECL != myDECL);
        upNode = upNode->data.FIP.up);
      if (upNode) {
        inSynchTree = true;
        CustomEvent syncev(UEV_LavaPE_SyncTree,(void*)new CSyncData(upNode->data.FormSyntax, handlerChain));
        if (!QApplication::sendEvent(((CLavaGUIView*)ViewWin)->myTree, &syncev ))
          inSynchTree = false;
      }
      else
        delete handlerChain;
    }
    else {
      QApplication::postEvent(((CLavaGUIView*)ViewWin)->myTree,new CustomEvent(UEV_LavaPE_CalledView));
      delete handlerChain;
    }
  else
    if ((!inSyncForm || noSync && handlerChain->first) && !((CLavaGUIFrame*)((CLavaGUIView*)ViewWin)->GetParentFrame())->onClose) {
      CustomEvent* syncev1 = new CustomEvent(UEV_LavaPE_SyncTree,(void*)new CSyncData(0, handlerChain));
      QApplication::postEvent(((CLavaGUIView*)ViewWin)->myTree, syncev1);
    }
    else
      delete handlerChain;
}


void CGUIProg::OnModified()
{
  if (isView)
    ((CLavaGUIView*)ViewWin)->OnModified();
}

void CGUIProg::RedrawForm()
{
  //QPoint vpoint, tPoint;
  int sx, sy;
  if (Root) {
    sx = ((GUIScrollView*)scrView)->horizontalScrollBar()->value();//contentsX();
    sy = ((GUIScrollView*)scrView)->verticalScrollBar()->value();//contentsY();
    refresh = false;
    LavaForm.DeleteWindows(Root, true);
  }
  refresh = true;
  MakeGUI.DisplayScreen(false);
  //((GUIScrollView*)scrollView)->setContentsPos(sx, sy);
  ((GUIScrollView*)scrView)->horizontalScrollBar()->setValue(sx);
  ((GUIScrollView*)scrView)->verticalScrollBar()->setValue(sy);
  //((GUIScrollView*)scrollView)->updateContents();
  ((GUIScrollView*)scrView)->viewport()->update();
}

CGUIProg::~CGUIProg()
{
  if (Root)
    LavaForm.DeleteForm(Root);
}

void CGUIProg::OnTab(bool back, QWidget* win)
{
  CHEFormNode *frmPtr, *trp = 0;
  QWidget* parentWin;

  if (!butNode)
    MPTR = focNode;
  else
    MPTR = butNode;
  for (parentWin = win->parentWidget(); !parentWin->inherits("GUIVBox"); parentWin = parentWin->parentWidget());
  if (((GUIScrollView*)scrView)->qvbox == parentWin)
    frmPtr = Root;
  else
    frmPtr = ((CLavaGUIPopup*)parentWin)->myFormNode;
  while (!trp) {
    if (back) {
      trp = TreeSrch.PrevUnprotected(MPTR);
      if (!trp )
        trp = TreeSrch.LastUnprotected(Root);
    }
    else {
      trp = TreeSrch.NextUnprotected(MPTR, frmPtr);
      if (!trp )
        trp = TreeSrch.NthUnprotected(1);
    }
    if (MPTR == trp)
      return;
    if (MakeGUI.setFocus(1,trp)) {
      if (focNode) {
        if (!focNode->data.Atomic
            &&  focNode->data.IterFlags.Contains(Ellipsis))
          MakeGUI.SetPointer((QWidget*)((CHEFormNode*)focNode->data.SubTree.first)->data.FIP.widget,1);
        else
          MakeGUI.SetPointer((QWidget*)focNode->data.FIP.widget, 1);
        /*
        if (!inSyncForm)
          SyncTree(focNode);
        else
          inSyncForm = false;
        */
      }
    }
    else {
      MPTR = trp;
      trp = 0;
    }
  }
}


void CGUIProg::OnSetFocus(CHEFormNode* trp0)
{
  bool polset;
  MPTR = trp0;
  if (MPTR) {
    MakeGUI.setFocus(2, MPTR);
    CHEFormNode* trp = focNode;
    /* trp : neuer / MPTR : alter Focus */
    if (trp->data.BasicFlags.Contains(Toggle)) {
      MPTR = focNode;
      CmdExec.Toggl(MPTR);

      if (trp->data.IoSigFlags.Contains(Enter)
            || trp->data.IoSigFlags.Contains(EnterAsIs))
         polset = true;
      if (trp->data.IoSigFlags.Contains(EnterAsIs))
        allowIncompleteForms = true;
      }
    }
}

bool CGUIProg::OnUpdateInsertopt(QAction* action)
{
 QWidget* insertWindow = 0;
 InsertNode = ActNode;
 while (InsertNode && (insertWindow != ViewWin)) {// && (insertWindow != frm)) {
    insertWindow = InsertNode->data.FIP.frameWidget;
    if (InsertNode->data.IterFlags.Contains(IteratedItem)
        && !InsertNode->data.IterFlags.Contains(FixedCount)) {
      action->setEnabled(true);
      return true;
    }
    InsertNode = InsertNode->data.FIP.up;
  }
  action->setEnabled(false);
  InsertNode = 0;
  return false;
}

bool CGUIProg::OnUpdateDeleteopt(QAction* action)
{
  QWidget* delWindow = 0;
  DelNode = ActNode;
  while (DelNode && (delWindow != ViewWin)) {
    delWindow = DelNode->data.FIP.frameWidget;
    if (DelNode->data.IterFlags.Contains(Optional)
        || DelNode->data.IterFlags.Contains(IteratedItem)
           && !DelNode->data.IterFlags.Contains(FixedCount)) {
      action->setEnabled(true);
      return true;
    }
    DelNode = DelNode->data.FIP.up;
  }
  DelNode = 0;
  action->setEnabled(false);
  return false;
}

void CGUIProg::OnUpdateNewHandler(QAction* action)
{
  action->setEnabled(!LBaseData->inRuntime && ActNode
                     && ActNode->data.myHandlerNode);
}

void CGUIProg::OnUpdateAttachHandler(QAction* action)
{
  action->setEnabled(!LBaseData->inRuntime && ActNode
                     && ActNode->data.myHandlerNode);
}

void CGUIProg::ExecuteChainAction(QAction* action)
{
  if (action == insActionPtr) {
    if (InsertNode)
      CmdExec.InsertIterItem(InsertNode);
  }
  else if (action == delActionPtr) {
    if (DelNode) {
      if (DelNode->data.IterFlags.Contains(Optional))
       CmdExec.DeleteOptionalItem(DelNode);
      else
        CmdExec.DeleteIterItem(DelNode);
      DelNode = 0;
    }
  }
}

void CGUIProg::OnNewHandler()
{
  LavaDECL* decl;
  CHETIDs* cheTIDs;
  if (ActNode && ActNode->data.myHandlerNode) {
    decl = NewLavaDECL();
    decl->DeclType = Function;
    decl->SecondTFlags.INCL(isHandler);
    decl->ParentDECL = myDECL;
	if (ActNode->data.IterFlags.Contains(isOptional)
		&& (ActNode->data.FIP.widget->inherits("CPushButton")
		&& (((CPushButton*)ActNode->data.FIP.widget)->DISCOButtonType == isElli)
		  || ActNode->data.myHandlerNode->data.FIP.widget->inherits("CFormWid")
		  || ActNode->data.myHandlerNode->data.FIP.widget->inherits("CFormWid"))
		)
	  decl->GUISignaltype = Ev_OptInsert;
	else
	  decl->GUISignaltype = 0;
    cheTIDs = new CHETIDs();
    cheTIDs->data = ActNode->data.myHandlerNode->data.myName;
    decl->HandlerClients.Append(cheTIDs);
    inNewHandler = true;
    QApplication::postEvent(((CLavaGUIView*)ViewWin)->myTree, new CustomEvent(UEV_NewHandler, (void*)decl));
  }
}

void CGUIProg::OnAttachHandler()
{
  if (ActNode && ActNode->data.myHandlerNode) {
    CAttachData* data = new CAttachData(myDECL, ActNode->data.myHandlerNode->data.myName);
    inNewHandler = true;
    QApplication::postEvent(((CLavaGUIView*)ViewWin)->myTree,new CustomEvent(UEV_AttachHandler, (void*)data));
  }
}
