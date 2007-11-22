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


#include "DIO.h"

#include "GUIProg.h"
#include "CmdExec.h"
#include "MakeGUI.h"
#include "BAdapter.h"
#include "LavaBaseDoc.h"
#include "qmessagebox.h"
#include "LavaBaseStringInit.h"
#include "TButton.h"
#include "mdiframes.h"

#pragma hdrstop


CHEFormNode* CmdExecCLASS::InPopupShell(CHEFormNode* node)
{
  CHEFormNode* upNode;
  for (upNode = node; upNode && !upNode->data.BasicFlags.Contains(PopUp); upNode = upNode->data.FIP.up);
  return upNode;
}

CHEFormNode* CmdExecCLASS::GetOptNode(CHEFormNode* fNode)
{
  return fNode->data.FIP.up;
}

void CmdExecCLASS::InsertOptionalItem (CHEFormNode* fNode)
{
  //LavaDECL *formSyn;

  optNode = fNode->data.FIP.up;
  formSyn = optNode->data.FormSyntax;
  LastEvent = ID_INSERTOPT;
  if (LBaseData->inRuntime) {
    if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(formSyn, (LavaVariablePtr)optNode->data.ResultVarPtr))
      return;
    if (optNode->data.myHandler.first) {
      if (OptHandlerCall(optNode, Ev_OptInsert))
        ((CGUIProg*)GUIProg)->OnModified();
      else
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
  }
  else
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
  /*
  GUIProg->focNode = 0;
  optNode->data.SubTree.Destroy();
  ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
  ((CGUIProg*)GUIProg)->LavaForm.PartialForm(formSyn, optNode, optNode->data.allowOwnHandler); 
  if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
    if (!LBaseData->inRuntime) 
      QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), "Empty optional element",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    return;
  }
  optNode->data.IterFlags.INCL(Optional);
  LastEvent = ID_INSERTOPT;
  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));*/

}

void CmdExecCLASS::DeleteOptionalItem ( CHEFormNode* fNode)
{ 
  //LavaDECL* formSyn;

  optNode = fNode;
  parNode = optNode->data.FIP.up;
  LastEvent = ID_DELETEOPT;
  if (LBaseData->inRuntime) {
    if (parNode->data.myHandler.first) {
      if (OptHandlerCall(parNode, Ev_OptDelete))
        ((CGUIProg*)GUIProg)->OnModified();
      else
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
  }
  else
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
  /*
  delNode = (CHEFormNode*)parNode->data.SubTree.Uncouple(fNode); 
  elliNode = 0;
  formSyn = parNode->data.FormSyntax;
  ((CGUIProg*)GUIProg)->LavaForm.CreateEllipsis(elliNode,formSyn, parNode->data.allowOwnHandler);
  parNode->data.SubTree.Insert(delNode->predecessor, elliNode);
  if (delNode->data.BasicFlags.Contains(Groupbox))
    elliNode->data.BasicFlags.INCL(Groupbox);
  elliNode->data.FIP.up = parNode;
  parNode->data.IterFlags.EXCL(Optional);
  LastEvent = ID_DELETEOPT;
  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));*/
}

CHEFormNode* CmdExecCLASS::GetIterNode(CHEFormNode* fNode)
{
  CHEFormNode *parNode;
  parNode = fNode->data.FIP.up;
  if (fNode->data.IterFlags.Contains(Ellipsis))
    return fNode;
  else
    if (parNode->data.IterFlags.Contains(Ellipsis)) {
      return parNode;
    }
    else
      return fNode;
}

void CmdExecCLASS::InsertIterItem (CHEFormNode* fNode)
{
  //CHEFormNode *chainNode;
  //LavaObjectPtr HandlerStack[SFH+7], handle = 0;
  LavaDECL *iterSyn;//, *formSyn;

  parNode = fNode->data.FIP.up;
  if (fNode->data.IterFlags.Contains(Ellipsis))
    beforeNode = fNode;
  else
    if (parNode->data.IterFlags.Contains(Ellipsis)) {
      beforeNode = parNode;
      parNode = beforeNode->data.FIP.up;
    }
    else
      beforeNode = fNode;
  ((CGUIProg*)GUIProg)->LavaForm.AllocFNode(insertedNode, 0,0);
  insertedNode->data.IoSigFlags = parNode->data.IoSigFlags;
  formSyn = beforeNode->data.FormSyntax;
  if (LBaseData->inRuntime) {
    chainNode = fNode->data.FIP.up;
    while (chainNode && !chainNode->data.ResultVarPtr)
      chainNode = chainNode->data.FIP.up;
    if (chainNode) {
      insertedNode->data.ResultVarPtr = (CSecTabBase ***)((CGUIProg*)GUIProg)->LavaForm.NewLavaVarPtr(0);
      if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(formSyn, (LavaVariablePtr)insertedNode->data.ResultVarPtr))
        return;
      StackFrame[0] = 0;
      StackFrame[1] = 0;
      StackFrame[2] = 0; //chain object
      StackFrame[SFH] = *(LavaVariablePtr)chainNode->data.ResultVarPtr; //chain object
      if (beforeNode->data.ResultVarPtr)
        StackFrame[SFH+1] = (LavaObjectPtr)beforeNode->data.HandleObjPtr; //before handle
      else
        StackFrame[SFH+1] =  0;
      StackFrame[SFH+2] = *(LavaVariablePtr)insertedNode->data.ResultVarPtr; //new data
      StackFrame[SFH+3] = 0;
      StackFrame[SFH+4] = 0;
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
        return;
    }
    else {
      //error
      GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, formSyn, &ERR_ErrInForm,0);
      return;
    }
  }
  if (formSyn->DeclType != VirtualType)
    formSyn = parNode->data.FormSyntax;
  if (formSyn->Annotation.ptr && formSyn->Annotation.ptr->IterOrig.ptr) {
    iterSyn = ((TIteration*)formSyn->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
    if (iterSyn) {
      iterSyn->inINCL = formSyn->inINCL;
      formSyn = iterSyn;
    }
  }
  LastEvent = IDM_ITER_INSERT;
  if (LBaseData->inRuntime) {
    if (chainNode->data.myHandler.first) {
      if (!ChainHandlerCall(chainNode, StackFrame, Ev_ChainInsert))
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
    else
      QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
  }
  else
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
}

void CmdExecCLASS::DeleteIterItem (CHEFormNode* fNode)
{
  //CHEFormNode *chainNode;
  //LavaObjectPtr StackFrame[SFH+3];

  if (fNode->data.IterFlags.Contains(Ellipsis))
    delNode = (CHEFormNode*)fNode->predecessor;
  else
    if (fNode->data.FIP.up->data.IterFlags.Contains(Ellipsis))
      delNode = (CHEFormNode*)fNode->data.FIP.up->predecessor;
    else
      delNode = fNode;
  if (delNode) {
    LastEvent = IDM_ITER_DEL;
    if (LBaseData->inRuntime) {
      //to do: remove chain elem from object
      chainNode = delNode->data.FIP.up;
      while (chainNode && !chainNode->data.ResultVarPtr)
        chainNode = chainNode->data.FIP.up;
      if (chainNode) {
        StackFrame[0] = 0;
        StackFrame[1] = 0;
        StackFrame[2] = 0;
        StackFrame[SFH] = *(LavaVariablePtr)chainNode->data.ResultVarPtr;
        StackFrame[SFH+1] = (LavaObjectPtr)delNode->data.HandleObjPtr;
        StackFrame[SFH+2] = 0;
        if (chainNode->data.myHandler.first) {
          if (!ChainHandlerCall(chainNode, StackFrame, Ev_ChainDelete ))
            QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
        }
        else
          QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
      }
      else {
        GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, delNode->data.FormSyntax, &ERR_ErrInForm,0);
        return;
      }
    }//inRuntime
    else 
      QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
  }
}



/*
        if (!chainNode->data.myHandler.first
            || ChainHandlerCall(chainNode, StackFrame, Ev_ChainDelete )) {
          if (GUIProg->fromFillIn) {
            StackFrame[SFH] = *(LavaVariablePtr)chainNode->data.ResultVarPtr;
            StackFrame[SFH+1] = (LavaObjectPtr)delNode->data.HandleObjPtr;
            if (!SetRemove(((CGUIProg*)GUIProg)->ckd, StackFrame))
              return;
          }
          else
            ((SynFlags*)((LavaObjectPtr)delNode->data.HandleObjPtr+1))->INCL(deletedItem);
          delNode = (CHEFormNode*)delNode->data.FIP.up->data.SubTree.Uncouple(delNode); 
          ((CGUIProg*)GUIProg)->OnModified();
        }
        else
          delNode = 0;
      }
      else {
        GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, delNode->data.FormSyntax, &ERR_ErrInForm,0);
        return;
      }
    }//inRuntime
    else {
        delNode = (CHEFormNode*)delNode->data.FIP.up->data.SubTree.Uncouple(delNode);
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
  }
  LastEvent = IDM_ITER_DEL;
  if (delNode) 
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
}*/ // END OF DeleteIterItem


bool CmdExecCLASS::GUIEvent(QEvent* ev)
{
  CHEFormNode *popupNode;
  bool doIt;

  if (ev->type() == UEV_LavaGUIEvent) {

    if (LastEvent == IDM_ITER_INSERT) {

      if (LBaseData->inRuntime) {
        if (HandlerStack) {
          doIt = *(bool*)(HandlerStack[SFH+4]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+4]);
          if (!doIt) {
            if (HandlerStack[SFH+5])
              DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+5]);
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
            *insertedNode->data.ResultVarPtr = 0;
            delete insertedNode;
            insertedNode = 0;
 	          delete [] HandlerStack;
            HandlerStack = 0;
            return true;
          }
          if (HandlerStack[SFH+5]) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
            StackFrame[SFH+2] = HandlerStack[SFH+5];
            *insertedNode->data.ResultVarPtr = (CSecTabBase**)StackFrame[SFH+2];
          }
          delete [] HandlerStack;
          HandlerStack = 0;
        }
        StackFrame[SFH+2] = StackFrame[SFH+2] + StackFrame[SFH+2][0][((CGUIProg*)GUIProg)->myDoc->GetSectionNumber(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2][0][0].classDECL, GUIProg->myDoc->DECLTab[B_Object])].sectionOffset;
      }
      ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
      ((CGUIProg*)GUIProg)->LavaForm.PartialForm(formSyn, insertedNode, false); 
      if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
        delete insertedNode;
        insertedNode = 0;
        if (!LBaseData->inRuntime) 
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), "Empty chain element",  QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        return true;
      }
      if (LBaseData->inRuntime) {
        ((CGUIProg*)GUIProg)->ex = HSetInsertBefore(((CGUIProg*)GUIProg)->ckd, StackFrame);
        ((SynFlags*)(StackFrame[SFH+2]+1))->INCL(insertedItem);
        if (((CGUIProg*)GUIProg)->ex)
          return true;
        handle = StackFrame[SFH+3];
        DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,*(LavaVariablePtr)insertedNode->data.ResultVarPtr); //decr from AllocateObject after putting into chain
        ((CGUIProg*)GUIProg)->OnModified();
        insertedNode->data.HandleObjPtr = (CSecTabBase**)handle;
      }//inRunTime
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (insertedNode, insertedNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      parNode->data.SubTree.Insert(beforeNode->predecessor, insertedNode);
      insertedNode->data.FIP.up = parNode; 
      insertedNode->data.IterFlags.INCL(IteratedItem);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(insertedNode);
      if (popupNode) {
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
        if (!GUIProg->focNode)
          popupNode->data.FIP.popupShell->raise();
      }
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      if (GUIProg->focNode)
        ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->focNode);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(insertedNode,false);
    }

    else if (LastEvent == IDM_ITER_DEL) {

      if (LBaseData->inRuntime) {
        if (HandlerStack) {
          doIt = *(bool*)(HandlerStack[SFH+3]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+3]);
          delete [] HandlerStack;
          HandlerStack = 0;
          if (!doIt) 
            return true;
        }
        if (GUIProg->fromFillIn) {
          StackFrame[SFH] = *(LavaVariablePtr)chainNode->data.ResultVarPtr;
          StackFrame[SFH+1] = (LavaObjectPtr)delNode->data.HandleObjPtr;
          if (!SetRemove(((CGUIProg*)GUIProg)->ckd, StackFrame))
            return true;
        }
        else
          ((SynFlags*)((LavaObjectPtr)delNode->data.HandleObjPtr+1))->INCL(deletedItem);
      }
      delNode = (CHEFormNode*)delNode->data.FIP.up->data.SubTree.Uncouple(delNode); 
      ((CGUIProg*)GUIProg)->OnModified();
      GUIProg->setFocNode(0);
      GUIProg->newFocus = 0;
      GUIProg->ActNode = 0;
      if (delNode->successor)
        GUIProg->CurPTR = (CHEFormNode*)delNode->successor;
      else
        GUIProg->CurPTR = (CHEFormNode*)delNode->predecessor;
      popupNode = InPopupShell(GUIProg->CurPTR);
      if (popupNode) {
        delNode->successor = 0;
        ((CGUIProg*)GUIProg)->LavaForm.DeleteWindows(delNode, false);
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      }
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->CurPTR);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(GUIProg->CurPTR,false);
      delete delNode; 
    }

    else if (LastEvent == ID_INSERTOPT) {

      if (LBaseData->inRuntime) {
        if (HandlerStack) {
          doIt = *(bool*)(HandlerStack[SFH+2]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+2]);
          if (!doIt) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+1]);
            *optNode->data.ResultVarPtr = 0;
            if (HandlerStack[SFH+3])
              DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+3]);
 	          delete [] HandlerStack;
            HandlerStack = 0;
            return true;
          }
          if (HandlerStack[SFH+3]) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+1]);
            *optNode->data.ResultVarPtr = (CSecTabBase**)HandlerStack[SFH+3];
          }
 	        delete [] HandlerStack;
          HandlerStack = 0;
       }
      }
      ((CGUIProg*)GUIProg)->OnModified();
      GUIProg->focNode = 0;
      optNode->data.SubTree.Destroy();
      ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
      ((CGUIProg*)GUIProg)->LavaForm.PartialForm(formSyn, optNode, optNode->data.allowOwnHandler); 
      if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
        if (!LBaseData->inRuntime) 
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), "Empty optional element",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        return true;
      }
      optNode->data.IterFlags.INCL(Optional);
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (optNode, optNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(optNode);
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (optNode, optNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(optNode);
      if (popupNode)
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->focNode);
    }

    else if (LastEvent == ID_DELETEOPT) {

      if (LBaseData->inRuntime) {
        if (HandlerStack) {
          doIt = *(bool*)(HandlerStack[SFH+2]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+2]);
 	        delete [] HandlerStack;
          if (!doIt)
            return true;
        }
      }
      delNode = (CHEFormNode*)parNode->data.SubTree.Uncouple(optNode); 
      elliNode = 0;
      formSyn = parNode->data.FormSyntax;
      ((CGUIProg*)GUIProg)->LavaForm.CreateEllipsis(elliNode,formSyn, parNode->data.allowOwnHandler);
      parNode->data.SubTree.Insert(delNode->predecessor, elliNode);
      if (delNode->data.BasicFlags.Contains(Groupbox))
        elliNode->data.BasicFlags.INCL(Groupbox);
      elliNode->data.FIP.up = parNode;
      parNode->data.IterFlags.EXCL(Optional);
      GUIProg->ActNode = 0;
      GUIProg->setFocNode(0);
      GUIProg->CurPTR = elliNode;
      if (delNode->data.FIP.popupShell) {
        delete delNode->data.FIP.popupShell;
        delNode->data.FIP.popupShell = 0;
        delNode->data.FIP.poppedUp = false;
      }
      popupNode = InPopupShell(elliNode);
      if (popupNode)
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(elliNode);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(elliNode,false);
      if (LBaseData->inRuntime) {
        DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,*(LavaVariablePtr)delNode->data.ResultVarPtr);
        *delNode->data.ResultVarPtr = 0;
        ((CGUIProg*)GUIProg)->OnModified();
      } 
      delete delNode;
    }
    else if (LastEvent == ID_EDITVALUE) {

      if (LBaseData->inRuntime) {
        if (HandlerStack) {
          editNode->data.ResultVarPtr = (CSecTabBase***)editPtr;
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, *editPtr);
          if (HandlerStack[SFH+3]) {
            *editNode->data.ResultVarPtr = (CSecTabBase**)HandlerStack[SFH+3];
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+2]);
          }
          else {
            *editNode->data.ResultVarPtr = (CSecTabBase**)HandlerStack[SFH+2];
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+3]);
          }
          ((CGUIProg*)GUIProg)->LavaForm.setDefaultValue(editNode);
	        delete [] HandlerStack;
          HandlerStack = 0;
          
          editNode->data.IoSigFlags.INCL(trueValue);
          if (editNode->data.FIP.widget->inherits("QLineEdit"))
            ((CTEdit*)editNode->data.FIP.widget)->setText(editNode->data.StringValue.c);
          else
            ((CMultiLineEdit*)editNode->data.FIP.widget)->setPlainText(editNode->data.StringValue.c);
          editNode->data.FIP.widget->setFocus();
        }
      }
    }
  }
  return true;
}

CHEHandlerInfo* CmdExecCLASS::GetHandler(CHEFormNode* fNode, int eventType)
{
  LavaObjectPtr obj = 0;
  CHEHandlerInfo *cheHandler;
  LavaDECL *handlerDECL, *classDECL;

  for (cheHandler = (CHEHandlerInfo*)fNode->data.myHandler.last;
    cheHandler && (GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID)->GUISignaltype != eventType);
    cheHandler = (CHEHandlerInfo*)cheHandler->predecessor
  );
  if (!cheHandler)
    return 0;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  if (!cheHandler->data.HandlerNode->data.GUIService) {
    if (handlerDECL->ParentDECL == GUIProg->myDECL)
      cheHandler->data.HandlerNode->data.GUIService = *((CSecTabBase***)GUIProg->ServicePtr);
    else {
      classDECL = handlerDECL->ParentDECL;
      if (classDECL->DeclType == Impl)
        classDECL = GUIProg->myDoc->IDTable.GetDECL(((CHETID*)classDECL->Supports.first)->data, classDECL->inINCL);
      obj = AllocateObject(GUIProg->ckd, classDECL, true);
      if (obj && CallDefaultInit(((CGUIProg*)GUIProg)->ckd, obj)) {
        GUIProg->allocatedObjects.append(obj);
        cheHandler->data.HandlerNode->data.GUIService = (CSecTabBase**)obj;
      }
      else {
        if (obj)
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, obj);
        if (!((CGUIProg*)GUIProg)->ckd.exceptionThrown)
          ((CGUIProg*)GUIProg)->ex = new CRuntimeException(memory_ex, &ERR_AllocObjectFailed);
        return 0;
      }
    }
  }
  return cheHandler;
}

bool CmdExecCLASS::OptHandlerCall(CHEFormNode* optNode, int eventType)
{
  bool ok;
  CVFuncDesc *fDesc;
  CSectionDesc *funcSect;
  int fsizeBytes, fsize;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(optNode, eventType);
  if (!cheHandler)
    return false;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  fsize = fDesc->stackFrameSize;
  fsizeBytes = fsize<<2;
  LavaVariablePtr rPtr = (LavaVariablePtr)optNode->data.ResultVarPtr;
	HandlerStack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii < fsize; ii++)
    HandlerStack[ii] = 0;
  HandlerStack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  HandlerStack[SFH+1] = *(LavaVariablePtr)optNode->data.ResultVarPtr;
  if (fDesc->isNative)
    ok = (*fDesc->funcPtr)(((CGUIProg*)GUIProg)->ckd, HandlerStack);
  else
    ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ((CGUIProg*)GUIProg)->ckd, HandlerStack);
  if (!ok) 
    ((CGUIProg*)GUIProg)->ckd.document->LavaError(((CGUIProg*)GUIProg)->ckd, true, ((LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0]->classDECL, &ERR_RunTimeException,0);

  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));//entfällt, wenn thread gestartet
  return true;

  /* else {
    ok = *(bool*)(HandlerStack[SFH+2]+LSH);
    DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+2]);
    if (eventType == Ev_OptInsert) {
      DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+1]);
      if (ok && HandlerStack[SFH+3]) 
        *optNode->data.ResultVarPtr = (CSecTabBase**)HandlerStack[SFH+3];
      else
        ok = false;
    }
  }
	  delete [] HandlerStack;

    /*
  if (eventType == Ev_OptInsert) {
    DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+1]);
    optNode->data.ResultVarPtr = (CSecTabBase***)&StackFrame[SFH+3];
  }
  return ok;*/
}

bool CmdExecCLASS::ChainHandlerCall(CHEFormNode* chainNode, LavaVariablePtr StackFrame, int eventType)
{
  bool ok;
  CVFuncDesc *fDesc;
  CSectionDesc *funcSect;
  int fsizeBytes, fsize;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(chainNode, eventType);
  if (!cheHandler)
    return false;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  fsize = fDesc->stackFrameSize;
  fsizeBytes = fsize<<2;
	HandlerStack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii<fsize; ii++)
    HandlerStack[ii] = 0;
  HandlerStack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  HandlerStack[SFH+1] = StackFrame[SFH]; //chain
  HandlerStack[SFH+1] = (LavaObjectPtr)((HandlerStack[SFH+1])-(*(HandlerStack[SFH+1]))->sectionOffset);
  HandlerStack[SFH+2] = StackFrame[SFH+1]; //handle
  if (eventType == Ev_ChainInsert) {
    HandlerStack[SFH+3] = StackFrame[SFH+2];//newElem
  }
  if (fDesc->isNative)
    ok = (*fDesc->funcPtr)(((CGUIProg*)GUIProg)->ckd, HandlerStack);
  else
    ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ((CGUIProg*)GUIProg)->ckd, HandlerStack);
  if (!ok) 
    ((CGUIProg*)GUIProg)->ckd.document->LavaError(((CGUIProg*)GUIProg)->ckd, true, ((LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0]->classDECL, &ERR_RunTimeException,0);
  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));//entfällt, wenn thread gestartet
  /*else {
    if (eventType == Ev_ChainInsert) {
      ok = *(bool*)(HandlerStack[SFH+4]+LSH);
      DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+4]);
      if (ok && HandlerStack[SFH+5]) {
        DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
        StackFrame[SFH+2] = HandlerStack[SFH+5];
      }
    }
    else {
      ok = *(bool*)(HandlerStack[SFH+3]+LSH);
      DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, HandlerStack[SFH+3]);
    }
  }

	delete [] HandlerStack;
  HandlerStack = 0;
  */
  return true;
}

bool  CmdExecCLASS::EditHandlerCall(CHEFormNode* fNode, STRING newStr)
{
  CVFuncDesc *fDesc;
  CSectionDesc *funcSect;
  bool ok;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(fNode, Ev_ValueChanged);
  if (!cheHandler)
    return false;
  editNode = fNode;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  LastEvent = ID_EDITVALUE;
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  int fsize = fDesc->stackFrameSize;
  int fsizeBytes = fsize<<2;
  editPtr = (LavaVariablePtr)editNode->data.ResultVarPtr;

	HandlerStack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii < fsize; ii++)
    HandlerStack[ii] = 0;
  HandlerStack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  HandlerStack[SFH+1] = *(LavaVariablePtr)editNode->data.ResultVarPtr; //old value
  if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(editNode->data.FormSyntax, &HandlerStack[SFH+2])) {
	  delete [] HandlerStack;
    HandlerStack = 0;
    return false;
  }
  editNode->data.ResultVarPtr = (CSecTabBase***)&HandlerStack[SFH+2];
  editNode->data.StringValue = newStr;
  ConvertAndStore(editNode);
  if (fDesc->isNative)
    ok = (*fDesc->funcPtr)(((CGUIProg*)GUIProg)->ckd, HandlerStack);
  else
    ok = fDesc->Execute((SynObjectBase*)fDesc->funcExec->Exec.ptr, ((CGUIProg*)GUIProg)->ckd, HandlerStack);
  if (!ok) {
    ((CGUIProg*)GUIProg)->ckd.document->LavaError(((CGUIProg*)GUIProg)->ckd, true, ((LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0]->classDECL, &ERR_RunTimeException,0);

	  delete [] HandlerStack;
    HandlerStack = 0;
    return false;
  }
  QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));//entfällt, wenn thread gestartet
  return true;
  
  /*fNode->data.ResultVarPtr = (CSecTabBase***)rPtr;
  DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, *rPtr);
  if (StackFrame[SFH+3]) {
    *fNode->data.ResultVarPtr = (CSecTabBase**)StackFrame[SFH+3];
    DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
  }
  else {
    *fNode->data.ResultVarPtr = (CSecTabBase**)StackFrame[SFH+2];
    DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+3]);
  }
  ((CGUIProg*)GUIProg)->LavaForm.setDefaultValue(fNode);


	  delete [] HandlerStack;
    HandlerStack = 0;
    return true;*/
}

bool CmdExecCLASS::ConvertAndStore (CHEFormNode* trp)
{
  CHEEnumSelId *enumItem;
  unsigned j;
  int i;
  bool Done = true;
  DString strx, str0;
  QString *pstr, msg;
  LavaVariablePtr enumvarPtr;
  LavaObjectPtr enumPtr;

  if (trp->data.Atomic) {
    GUIProg->ErrorCode = NONE;

    switch (trp->data.BType) {

    case Enumeration:
      if (trp->data.BasicFlags.Contains(Toggle)
          && trp->data.BasicFlags.Contains(BitmapLabel)) 
        Done = true;
      else {
        if (!trp->data.StringValue.c || !trp->data.StringValue.l) 
          strx = " ";
        else
          strx = trp->data.StringValue;
        Done = false;
        j = 0;
        if (trp->data.EnumField)
          for (enumItem = (CHEEnumSelId*)trp->data.EnumField->Items.first;
               enumItem && (enumItem->data.Id != trp->data.StringValue);
               enumItem = (CHEEnumSelId*)enumItem->successor)
            j++;
        else
          for (enumItem = (CHEEnumSelId*)trp->data.FormSyntax->Items.first;
               enumItem && (enumItem->data.SelectionCode != strx);
               enumItem = (CHEEnumSelId*)enumItem->successor)
            j++;
        if (enumItem) {
          trp->data.D = j;
          if (LBaseData->inRuntime) {
            if (trp->data.EnumField)
              enumvarPtr = (LavaVariablePtr)(trp->data.ResultVarPtr);
            else
              enumvarPtr = (LavaVariablePtr)(trp->data.FIP.up->data.ResultVarPtr);
            if (!(*enumvarPtr))
              return false;
            enumPtr = *enumvarPtr;
            enumPtr = CastEnumType(((CGUIProg*)GUIProg)->ckd, enumPtr);
            if (!enumPtr)
              return false;
            *(int*)(enumPtr + LSH) = j;
            pstr = (QString*)(enumPtr+LSH+1);
            if (!pstr->isEmpty()) 
              *pstr = QString(enumItem->data.Id.c);
            else
              NewQString(pstr, enumItem->data.Id.c);
          }
          Done = true;
        }
        if (!Done) {
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), ERR_Odd_char,  QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
          if (Conv.DataLengthOK(trp)) {
            if (trp->data.StringValue.c)
              GUIProg->ErrPos = trp->data.StringValue.l;
            ((CGUIProg*)GUIProg)->MakeGUI.setFocus(GUIProg->ErrPos,trp);
          }
          else 
            return false;
        }
      }
      break;
 
    case Integer:
      Conv.StringToInt(trp->data.StringValue,i,Done);
      if (Done) {
        trp->data.I = i;
        if (LBaseData->inRuntime) {
          *(int*)(*(trp->data.ResultVarPtr)+LSH) = i;
        }
      }
      else {
        QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), ERR_Odd_char,  QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        ((CGUIProg*)GUIProg)->MakeGUI.setFocus(GUIProg->ErrPos,trp);
      }
      break;

    case B_Bool:
     break;

    case Float:
      trp->data.F = (float)atof(trp->data.StringValue.c);
      if (LBaseData->inRuntime) {
        *(float*)(*(trp->data.ResultVarPtr)+LSH) = trp->data.F;
      }
      break;

    case Double:
      trp->data.Db = atof(trp->data.StringValue.c);
      if (LBaseData->inRuntime) {
        *(double*)(*(trp->data.ResultVarPtr)+LSH) = trp->data.Db;
      }
      break;

    case Char:
      if (LBaseData->inRuntime) {
        pstr = (QString*)(*(trp->data.ResultVarPtr)+LSH);
        *(char*)(*(trp->data.ResultVarPtr)+LSH) = trp->data.StringValue[0];
      }
      Done = true;
      break;

    case Identifier:
      Conv.ConvToId(trp->data.StringValue,Done);
      break;
    case VLString:
      if (LBaseData->inRuntime) {
        pstr = (QString*)(*(trp->data.ResultVarPtr)+LSH);
        if (!pstr->isEmpty()) 
          *pstr = QString(trp->data.StringValue.c);
        else
          NewQString(pstr, trp->data.StringValue.c);
      }
      Done = true;
      break;
     default: ;
    }
  }
  if (LBaseData->inRuntime) 
    ((CGUIProg*)GUIProg)->OnModified();
  return Done && !((CGUIProg*)GUIProg)->ckd.exceptionThrown && !((CGUIProg*)GUIProg)->ex;
} // END OF ConvertAndStore



bool CmdExecCLASS::Toggl (CHEFormNode* trp)
{
  CHEEnumSelId *enumItem;
  unsigned j, tog;
  bool Done = false;

  GUIProg->MPTR = trp;
  if (!trp)
    return false;
  if (trp->data.Atomic) {
    switch (trp->data.BType) {
    case B_Bool:
      break;
    case Enumeration:
      tog = trp->data.D;
      j = 0;
      enumItem = (CHEEnumSelId*)trp->data.FormSyntax->Items.first;

      /* find current item: */
      while (tog != j) {
        enumItem = (CHEEnumSelId*)enumItem->successor;
        j++;
      }

      if (!trp->data.StringValue.c || !enumItem->data.SelectionCode.c) 
        /* find first successor item with SelectionCode # NIL */
        do {
          enumItem = (CHEEnumSelId*)enumItem->successor;
          j++;
          if (!enumItem) {
            enumItem = (CHEEnumSelId*)trp->data.FormSyntax->Items.first;
            j = 0;
          }
        }
        while (!enumItem->data.SelectionCode.c);
      trp->data.D = j;

      if (trp->data.BasicFlags.Contains(BitmapLabel))
        ((CToggleButton*)trp->data.FIP.widget)->setText(enumItem->data.SelectionCode.c);
      else {
        trp->data.StringValue = enumItem->data.SelectionCode;
      }
      break;
    } /* CASE BType */
    Done = ConvertAndStore(trp);
    if (!Done) {

    }
  } /* IF Atomic = true */
  return Done && !((CGUIProg*)GUIProg)->ckd.exceptionThrown && !((CGUIProg*)GUIProg)->ex;
} // END OF Toggl


void CmdExecCLASS::HandlePopup(CHEFormNode* trp)
{
  if (!trp->data.IoSigFlags.Contains(Signature)) {
    ((CGUIProg*)GUIProg)->MakeGUI.Popup(trp,true,true);
    return;
  }
}


void CmdExecCLASS::HandleButtonClick(CHEFormNode* button, CHEFormNode* trp)
{
  //add code from old CommandHandler
   bool done, polset;
   CHEFormNode* popupNode;

   SelectButton(button,trp);
   if (button->data.BasicFlags.Contains(Popdown)) {
     popupNode = button;
     while (popupNode && !popupNode->data.BasicFlags.Contains(PopUp))
       popupNode = popupNode->data.FIP.up;
     if (popupNode) {
       done = false;
       ((CGUIProg*)GUIProg)->MakeGUI.PopDown(popupNode,popupNode,done);
     }
   }
   if (trp->data.IoSigFlags.Contains(Enter) || trp->data.IoSigFlags.Contains(EnterAsIs))
     polset = true;
   if (trp->data.IoSigFlags.Contains(EnterAsIs))
     GUIProg->allowIncompleteForms = true;
}


void CmdExecCLASS::SelectButton (CHEFormNode* button, CHEFormNode*& trp)
{
  DString str;
  GUIProg->ErrorCode = NONE;
  if (button && (button->predecessor)) 
    str = ((CHEFormNode*)button->predecessor)->data.StringValue;
  if (trp) {
    switch (trp->data.BType) {
      case Enumeration:
        trp->data.StringValue = str;
        ConvertAndStore(trp);
        break;
      default: ;
    }
    if (trp->data.FIP.widget && trp->data.FIP.up->data.BasicFlags.Contains(PopUpMenu)
        && (!trp->data.FIP.up->data.Annotation.ptr /*|| !trp->data.FIP.up->data.Annotation.ptr->SigDef.SigID.c*/)) {
      ((CPushButton*)trp->data.FIP.widget)->setText(str.c);
    }
  }
} // END OF SelectButton

void CmdExecCLASS::INIT (CGUIProgBase *guiPr)
{
  GUIProg = guiPr;
  handle = 0;
  HandlerStack = 0;
}
