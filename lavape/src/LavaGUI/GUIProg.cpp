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


#include "GUIProg.h"
#include "LavaGUIPopup.h"
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"

#include "TEdit.h"
#include "mdiframes.h"
//#include "stdafx.h"



CGUIProg::CGUIProg() : CGUIProgBase()
{
  newFocus = 0;
  Root = 0;
  MPTR = 0;
  butNode = 0;
  editNode = 0;
  focNode = 0;
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
}

void CGUIProg::Create(CLavaBaseDoc* doc, wxView* view)
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
  CGUIProgBase::Create();
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

void CGUIProg::SyncTree(CHEFormNode *node)
{
  CHEFormNode *upNode;
  if (!LBaseData->inRuntime && !inSynchTree && !((CLavaGUIFrame*)((CLavaGUIView*)ViewWin)->GetParentFrame())->onClose)
    if (!inSyncForm) {
      if (((CLavaGUIView*)ViewWin)->LastBrowseNode) 
        ((CLavaGUIView*)ViewWin)->resetLastBrowseNode();
      for (upNode = node; upNode && !upNode->data.FormSyntax->ParentDECL;upNode = upNode->data.FIP.up);
      for (;
        upNode && upNode->data.FormSyntax->ParentDECL && (upNode->data.FormSyntax->ParentDECL->ParentDECL != ((CLavaGUIView*)ViewWin)->myDECL);
        upNode = upNode->data.FIP.up);
      if (upNode) {
        inSynchTree = true;
        QCustomEvent syncev(IDU_LavaPE_SyncTree, (void*)upNode->data.FormSyntax);
        if (!QApplication::sendEvent(((CLavaGUIView*)ViewWin)->myTree, &syncev ))
          inSynchTree = false;
      }
    }
    else
      QApplication::postEvent(((CLavaGUIView*)ViewWin)->myTree,new QCustomEvent(IDU_LavaPE_CalledView));
}


void CGUIProg::OnModified()
{
  ((CLavaGUIView*)ViewWin)->OnModified();
}  

void CGUIProg::RedrawForm()
{
  //QPoint vpoint, tPoint;
  int sx, sy;
  if (Root) {
    sx = ((CLavaGUIView*)ViewWin)->scrollView()->contentsX();
    sy = ((CLavaGUIView*)ViewWin)->scrollView()->contentsY();
    refresh = false;
    LavaForm.DeleteWindows(Root, true);
  }
  refresh = true;
  MakeGUI.DisplayScreen(false);
  ((CLavaGUIView*)ViewWin)->scrollView()->setContentsPos(sx, sy);
  ((CLavaGUIView*)ViewWin)->scrollView()->updateContents();
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
  if (((CLavaGUIView*)ViewWin)->qvbox == parentWin)
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
