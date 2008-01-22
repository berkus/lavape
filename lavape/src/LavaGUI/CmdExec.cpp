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
  H_OptNode = fNode->data.FIP.up;
  H_FormSyn = H_OptNode->data.FormSyntax;
  LastEvent = ID_INSERTOPT;
  if (LBaseData->inRuntime) {
    if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(H_FormSyn, (LavaVariablePtr)H_OptNode->data.ResultVarPtr))
      return;
    if (H_OptNode->data.myHandler.first) {
      if (OptHandlerCall(H_OptNode, Ev_OptInsert))
        ((CGUIProg*)GUIProg)->OnModified();
      else
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
  }
  else
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
}

void CmdExecCLASS::DeleteOptionalItem ( CHEFormNode* fNode)
{ 
  H_OptNode = fNode;
  H_ParNode = H_OptNode->data.FIP.up;

  LastEvent = ID_DELETEOPT;
  if (LBaseData->inRuntime) {
    if (H_ParNode->data.myHandler.first) {
      if (OptHandlerCall(H_ParNode, Ev_OptDelete))
        ((CGUIProg*)GUIProg)->OnModified();
      else
        QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
    }
  }
  else
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
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
  LavaDECL *iterSyn;

  H_ParNode = fNode->data.FIP.up;
  if (fNode->data.IterFlags.Contains(Ellipsis))
    H_BeforeNode = fNode;
  else
    if (H_ParNode->data.IterFlags.Contains(Ellipsis)) {
      H_BeforeNode = H_ParNode;
      H_ParNode = H_BeforeNode->data.FIP.up;
    }
    else
      H_BeforeNode = fNode;
  ((CGUIProg*)GUIProg)->LavaForm.AllocFNode(H_InsertedNode, 0,0);
  H_InsertedNode->data.IoSigFlags = H_ParNode->data.IoSigFlags;
  H_FormSyn = H_BeforeNode->data.FormSyntax;
  if (LBaseData->inRuntime) {
    H_ChainNode = fNode->data.FIP.up;
    while (H_ChainNode && !H_ChainNode->data.ResultVarPtr)
      H_ChainNode = H_ChainNode->data.FIP.up;
    if (H_ChainNode) {
      H_InsertedNode->data.ResultVarPtr = (CSecTabBase ***)((CGUIProg*)GUIProg)->LavaForm.NewLavaVarPtr(0);
      if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(H_FormSyn, (LavaVariablePtr)H_InsertedNode->data.ResultVarPtr))
        return;
      StackFrame[0] = 0;
      StackFrame[1] = 0;
      StackFrame[2] = 0; //chain object
      StackFrame[SFH] = *(LavaVariablePtr)H_ChainNode->data.ResultVarPtr; //chain object
      if (H_BeforeNode->data.ResultVarPtr)
        StackFrame[SFH+1] = (LavaObjectPtr)H_BeforeNode->data.HandleObjPtr; //before handle
      else
        StackFrame[SFH+1] =  0;
      StackFrame[SFH+2] = *(LavaVariablePtr)H_InsertedNode->data.ResultVarPtr; //new data
      StackFrame[SFH+3] = 0;
      StackFrame[SFH+4] = 0;
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
        return;
    }
    else {
      //error
      GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, H_FormSyn, &ERR_ErrInForm,0);
      return;
    }
  }
  if (H_FormSyn->DeclType != VirtualType)
    H_FormSyn = H_ParNode->data.FormSyntax;
  if (H_FormSyn->Annotation.ptr && H_FormSyn->Annotation.ptr->IterOrig.ptr) {
    iterSyn = ((TIteration*)H_FormSyn->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
    if (iterSyn) {
      iterSyn->inINCL = H_FormSyn->inINCL;
      H_FormSyn = iterSyn;
    }
  }
  LastEvent = IDM_ITER_INSERT;
  if (LBaseData->inRuntime) {
    if (H_ChainNode->data.myHandler.first) {
      if (!ChainHandlerCall(StackFrame, Ev_ChainInsert))
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
  if (fNode->data.IterFlags.Contains(Ellipsis))
    H_DelNode = (CHEFormNode*)fNode->predecessor;
  else
    if (fNode->data.FIP.up->data.IterFlags.Contains(Ellipsis))
      H_DelNode = (CHEFormNode*)fNode->data.FIP.up->predecessor;
    else
      H_DelNode = fNode;
  if (H_DelNode) {
    LastEvent = IDM_ITER_DEL;
    if (LBaseData->inRuntime) {
      //to do: remove chain elem from object
      H_ChainNode = H_DelNode->data.FIP.up;
      while (H_ChainNode && !H_ChainNode->data.ResultVarPtr)
        H_ChainNode = H_ChainNode->data.FIP.up;
      if (H_ChainNode) {
        StackFrame[0] = 0;
        StackFrame[1] = 0;
        StackFrame[2] = 0;
        StackFrame[SFH] = *(LavaVariablePtr)H_ChainNode->data.ResultVarPtr;
        StackFrame[SFH+1] = (LavaObjectPtr)H_DelNode->data.HandleObjPtr;
        StackFrame[SFH+2] = 0;
        if (H_ChainNode->data.myHandler.first) {
          if (!ChainHandlerCall(StackFrame, Ev_ChainDelete ))
            QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
        }
        else
          QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
      }
      else {
        GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, H_DelNode->data.FormSyntax, &ERR_ErrInForm,0);
        return;
      }
    }//inRuntime
    else 
      QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaGUIEvent));
  }
}


bool CmdExecCLASS::GUIEvent(QEvent* ev)
{
  CHEFormNode *popupNode;
  LavaObjectPtr handle;
  LavaObjectPtr enumPtr;
  CRadioButton *radioButton;
  bool doIt, toggle = false;
  int ii;

  if (ev->type() == UEV_LavaGUIEvent) {

    if (LastEvent == IDM_ITER_INSERT) {
      GUIProg->ViewWin->setEnabled(true);

      if (LBaseData->inRuntime) {
        if (Handler_Stack) {
          doIt = *(bool*)(Handler_Stack[SFH+4]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+4]);
          if (!doIt) {
            if (Handler_Stack[SFH+5])
              DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+5]);
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
            *H_InsertedNode->data.ResultVarPtr = 0;
            delete H_InsertedNode;
            H_InsertedNode = 0;
 	          delete [] Handler_Stack;
            Handler_Stack = 0;
            return true;
          }
          if (Handler_Stack[SFH+5]) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2]);
            StackFrame[SFH+2] = Handler_Stack[SFH+5];
            *H_InsertedNode->data.ResultVarPtr = (CSecTabBase**)StackFrame[SFH+2];
          }
          delete [] Handler_Stack;
          Handler_Stack = 0;
        }
        StackFrame[SFH+2] = StackFrame[SFH+2] + StackFrame[SFH+2][0][((CGUIProg*)GUIProg)->myDoc->GetSectionNumber(((CGUIProg*)GUIProg)->ckd, StackFrame[SFH+2][0][0].classDECL, GUIProg->myDoc->DECLTab[B_Object])].sectionOffset;
      }
      ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
      ((CGUIProg*)GUIProg)->LavaForm.PartialForm(H_FormSyn, H_InsertedNode, false); 
      if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
        delete H_InsertedNode;
        H_InsertedNode = 0;
        if (!LBaseData->inRuntime) 
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), "Empty chain element",  QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        return true;
      }
      if (LBaseData->inRuntime) {
        ((CGUIProg*)GUIProg)->ex = HSetInsertBefore(((CGUIProg*)GUIProg)->ckd, StackFrame);
        ((SynFlags*)(StackFrame[SFH+2]+1))->INCL(insertedItem);
        if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
          return true;
        handle = StackFrame[SFH+3];
        DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,*(LavaVariablePtr)H_InsertedNode->data.ResultVarPtr); //decr from AllocateObject after putting into chain
        ((CGUIProg*)GUIProg)->OnModified();
        H_InsertedNode->data.HandleObjPtr = (CSecTabBase**)handle;
      }//inRunTime
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (H_InsertedNode, H_InsertedNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      H_ParNode->data.SubTree.Insert(H_BeforeNode->predecessor, H_InsertedNode);
      H_InsertedNode->data.FIP.up = H_ParNode; 
      H_InsertedNode->data.IterFlags.INCL(IteratedItem);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(H_InsertedNode);
      if (popupNode) {
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
        if (!GUIProg->focNode)
          popupNode->data.FIP.popupShell->raise();
      }
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      if (GUIProg->focNode)
        ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->focNode);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(H_InsertedNode,false);
    }

    else if (LastEvent == IDM_ITER_DEL) {
      GUIProg->ViewWin->setEnabled(true);

      if (LBaseData->inRuntime) {
        if (Handler_Stack) {
          doIt = *(bool*)(Handler_Stack[SFH+3]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+3]);
          delete [] Handler_Stack;
          Handler_Stack = 0;
          if (!doIt) 
            return true;
        }
        if (GUIProg->fromFillIn) {
          StackFrame[SFH] = *(LavaVariablePtr)H_ChainNode->data.ResultVarPtr;
          StackFrame[SFH+1] = (LavaObjectPtr)H_DelNode->data.HandleObjPtr;
          if (!SetRemove(((CGUIProg*)GUIProg)->ckd, StackFrame))
            return true;
        }
        else
          ((SynFlags*)((LavaObjectPtr)H_DelNode->data.HandleObjPtr+1))->INCL(deletedItem);
      }
      H_DelNode = (CHEFormNode*)H_DelNode->data.FIP.up->data.SubTree.Uncouple(H_DelNode); 
      ((CGUIProg*)GUIProg)->OnModified();
      GUIProg->setFocNode(0);
      GUIProg->newFocus = 0;
      GUIProg->ActNode = 0;
      if (H_DelNode->successor)
        GUIProg->CurPTR = (CHEFormNode*)H_DelNode->successor;
      else
        GUIProg->CurPTR = (CHEFormNode*)H_DelNode->predecessor;
      popupNode = InPopupShell(GUIProg->CurPTR);
      if (popupNode) {
        H_DelNode->successor = 0;
        ((CGUIProg*)GUIProg)->LavaForm.DeleteWindows(H_DelNode, false);
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      }
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->CurPTR);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(GUIProg->CurPTR,false);
      delete H_DelNode; 
    }

    else if (LastEvent == ID_INSERTOPT) {
      GUIProg->ViewWin->setEnabled(true);

      if (LBaseData->inRuntime) {
        if (Handler_Stack) {
          doIt = *(bool*)(Handler_Stack[SFH+2]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+2]);
          if (!doIt) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+1]);
            *H_OptNode->data.ResultVarPtr = 0;
            if (Handler_Stack[SFH+3])
              DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+3]);
 	          delete [] Handler_Stack;
            Handler_Stack = 0;
            return true;
          }
          if (Handler_Stack[SFH+3]) {
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+1]);
            *H_OptNode->data.ResultVarPtr = (CSecTabBase**)Handler_Stack[SFH+3];
          }
 	        delete [] Handler_Stack;
          Handler_Stack = 0;
       }
      }
      ((CGUIProg*)GUIProg)->OnModified();
      GUIProg->focNode = 0;
      H_OptNode->data.SubTree.Destroy();
      ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
      ((CGUIProg*)GUIProg)->LavaForm.PartialForm(H_FormSyn, H_OptNode, H_OptNode->data.allowOwnHandler); 
       if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
        return true;
     if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
        if (!LBaseData->inRuntime) 
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), "Empty optional element",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        return true;
      }
      H_OptNode->data.IterFlags.INCL(Optional);
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (H_OptNode, H_OptNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(H_OptNode);
      GUIProg->ActNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (H_OptNode, H_OptNode);
      GUIProg->setFocNode(GUIProg->ActNode);
      GUIProg->CurPTR = GUIProg->focNode;
      popupNode = InPopupShell(H_OptNode);
      if (popupNode)
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->focNode);
    }

    else if (LastEvent == ID_DELETEOPT) {
      GUIProg->ViewWin->setEnabled(true);

      if (LBaseData->inRuntime) {
        if (Handler_Stack) {
          doIt = *(bool*)(Handler_Stack[SFH+2]+LSH);
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+2]);
 	        delete [] Handler_Stack;
          if (!doIt)
            return true;
        }
      }
      H_DelNode = (CHEFormNode*)H_ParNode->data.SubTree.Uncouple(H_OptNode); 
      H_ElliNode = 0;
      H_FormSyn = H_ParNode->data.FormSyntax;
      ((CGUIProg*)GUIProg)->LavaForm.CreateEllipsis(H_ElliNode,H_FormSyn, H_ParNode->data.allowOwnHandler);
      H_ParNode->data.SubTree.Insert(H_DelNode->predecessor, H_ElliNode);
      if (H_DelNode->data.BasicFlags.Contains(Groupbox))
        H_ElliNode->data.BasicFlags.INCL(Groupbox);
      H_ElliNode->data.FIP.up = H_ParNode;
      H_ParNode->data.IterFlags.EXCL(Optional);
      GUIProg->ActNode = 0;
      GUIProg->setFocNode(0);
      GUIProg->CurPTR = H_ElliNode;
      if (H_DelNode->data.FIP.popupShell) {
        delete H_DelNode->data.FIP.popupShell;
        H_DelNode->data.FIP.popupShell = 0;
        H_DelNode->data.FIP.poppedUp = false;
      }
      popupNode = InPopupShell(H_ElliNode);
      if (popupNode)
        ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
      else
        ((CGUIProg*)GUIProg)->RedrawForm();
      ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(H_ElliNode);     
      ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(H_ElliNode,false);
      if (LBaseData->inRuntime) {
        DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,*(LavaVariablePtr)H_DelNode->data.ResultVarPtr);
        *H_DelNode->data.ResultVarPtr = 0;
        ((CGUIProg*)GUIProg)->OnModified();
      } 
      delete H_DelNode;
    }
    else if (LastEvent == ID_EDITVALUE) {
      GUIProg->ViewWin->setEnabled(true);

      if (LBaseData->inRuntime) {
        if (Handler_Stack) {
          H_EditNode->data.ResultVarPtr = (CSecTabBase***)H_EditPtr;
          if (H_ButtonNode) {
            toggle = H_ButtonNode->data.BType == B_Bool;
            if (!toggle && !H_EditNode->data.EnumField)
              H_EditNode->data.FIP.up->data.ResultVarPtr = (CSecTabBase***)H_EditPtr;
          }
          DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, *H_EditPtr);
          if (Handler_Stack[SFH+3]) {
            *H_EditNode->data.ResultVarPtr = (CSecTabBase**)Handler_Stack[SFH+3];
            DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, Handler_Stack[SFH+2]);
          }
          else {
            *H_EditNode->data.ResultVarPtr = (CSecTabBase**)Handler_Stack[SFH+2];
          }
          if (!H_ButtonNode || toggle)
            ((CGUIProg*)GUIProg)->LavaForm.setDefaultValue(H_EditNode);
	        delete [] Handler_Stack;
          Handler_Stack = 0;
          
          H_EditNode->data.IoSigFlags.INCL(trueValue);
          
          if (H_EditNode->data.FIP.widget && H_EditNode->data.FIP.widget->inherits("QLineEdit")) {
            ((CTEdit*)H_EditNode->data.FIP.widget)->setText(H_EditNode->data.StringValue.c);
            H_EditNode->data.FIP.widget->setFocus();          
          }
          else if (H_EditNode->data.FIP.widget && H_EditNode->data.FIP.widget->inherits("CMultiLineEdit")) {
            ((CMultiLineEdit*)H_EditNode->data.FIP.widget)->setPlainText(H_EditNode->data.StringValue.c);
            H_EditNode->data.FIP.widget->setFocus();          
          }
          else if (H_EditNode->data.FIP.widget && H_EditNode->data.FIP.widget->inherits("CTComboBox")) {
            enumPtr = CastEnumType(((CGUIProg*)GUIProg)->ckd, *((LavaVariablePtr)H_EditNode->data.ResultVarPtr));
            ((CTComboBox*)H_EditNode->data.FIP.widget)->setCurrentIndex(*(int*)(enumPtr+LSH));
            H_EditNode->data.FIP.widget->setFocus();          
          }
          else if (H_ButtonNode && H_ButtonNode->data.FIP.widget->inherits("CRadioButton")) {
            radioButton = (CRadioButton*)H_ButtonNode->data.FIP.widget;
            for (ii = 0; ii < ((CFormWid*)radioButton->Radio)->nRadio; ii++) {
              ((CFormWid*)radioButton->Radio)->Radio[ii]->setChecked((ii == H_EditNode->data.D));
            }
          }
          else if (H_ButtonNode && toggle)
            ((CToggleButton*)H_ButtonNode->data.FIP.widget)->setChecked(H_ButtonNode->data.B);
        }
      }
    }
    H_ButtonNode = 0;
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
        if (GUIProg->isView) {
        }
        else
          ((LavaGUIDialog*)GUIProg->ViewWin)->OnCancel();
        return 0;
      }
    }
  }
  return cheHandler;
}

bool CmdExecCLASS::OptHandlerCall(CHEFormNode* optNode, int eventType)
{
  CSectionDesc *funcSect;
  int fsizeBytes, fsize;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(optNode, eventType);
  if (!cheHandler)
    return false;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  Handler_fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  fsize = Handler_fDesc->stackFrameSize;
  fsizeBytes = fsize<<2;
  LavaVariablePtr rPtr = (LavaVariablePtr)optNode->data.ResultVarPtr;
	Handler_Stack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii < fsize; ii++)
    Handler_Stack[ii] = 0;
  Handler_Stack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  Handler_Stack[SFH+1] = *(LavaVariablePtr)optNode->data.ResultVarPtr;
  ContinueExecThread();
  return true;
}

bool CmdExecCLASS::ChainHandlerCall(LavaVariablePtr StackFrame, int eventType)
{
  CSectionDesc *funcSect;
  int fsizeBytes, fsize;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(H_ChainNode, eventType);
  if (!cheHandler)
    return false;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  Handler_fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  fsize = Handler_fDesc->stackFrameSize;
  fsizeBytes = fsize<<2;
	Handler_Stack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii<fsize; ii++)
    Handler_Stack[ii] = 0;
  Handler_Stack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  Handler_Stack[SFH+1] = StackFrame[SFH]; //chain
  Handler_Stack[SFH+1] = (LavaObjectPtr)((Handler_Stack[SFH+1])-(*(Handler_Stack[SFH+1]))->sectionOffset);
  Handler_Stack[SFH+2] = StackFrame[SFH+1]; //handle
  if (eventType == Ev_ChainInsert) 
    Handler_Stack[SFH+3] = StackFrame[SFH+2];//newElem
  ContinueExecThread();
  return true;
}

bool  CmdExecCLASS::EditHandlerCall(CHEFormNode* fNode, STRING newStr)
{
  CSectionDesc *funcSect;
  //bool ok;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(fNode, Ev_ValueChanged);
  if (!cheHandler)
    return false;
  H_EditNode = fNode;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  LastEvent = ID_EDITVALUE;
  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  Handler_fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  int fsize = Handler_fDesc->stackFrameSize;
  int fsizeBytes = fsize<<2;
  H_EditPtr = (LavaVariablePtr)H_EditNode->data.ResultVarPtr;

	Handler_Stack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii < fsize; ii++)
    Handler_Stack[ii] = 0;
  Handler_Stack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  Handler_Stack[SFH+1] = *(LavaVariablePtr)H_EditNode->data.ResultVarPtr; //old value
  if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(H_EditNode->data.FormSyntax, &Handler_Stack[SFH+2])) {
	  delete [] Handler_Stack;
    Handler_Stack = 0;
    return false;
  }
  H_EditNode->data.ResultVarPtr = (CSecTabBase***)&Handler_Stack[SFH+2];
  H_EditNode->data.StringValue = newStr;
  ConvertAndStore(H_EditNode);
  ContinueExecThread();
  return true;
}

bool CmdExecCLASS::ButtonHandlerCall(CHEFormNode* buttonNode, int enumNum)
{
  CSectionDesc *funcSect;
  bool toggle = false;
  LavaDECL* handlerDECL;

  CHEHandlerInfo* cheHandler = GetHandler(buttonNode, Ev_ValueChanged);
  if (!cheHandler)
    return false;
  H_ButtonNode = buttonNode;
  if (buttonNode->data.FIP.widget->inherits("CToggleButton")) {
    H_EditNode = buttonNode;
    toggle = true;
    ((CToggleButton*)buttonNode->data.FIP.widget)->setChecked(!((CToggleButton*)buttonNode->data.FIP.widget)->isChecked());
  }
  if (buttonNode->data.FIP.widget->inherits("CPushButton"))
    H_EditNode = ((CPushButton*)buttonNode->data.FIP.widget)->EnumNode;
  if (buttonNode->data.FIP.widget->inherits("CRadioButton"))
    H_EditNode = ((CRadioButton*)buttonNode->data.FIP.widget)->EnumNode;
  handlerDECL = GUIProg->myDoc->IDTable.GetDECL(cheHandler->data.HandlerID);
  LastEvent = ID_EDITVALUE;
  if (H_EditNode->data.EnumField || toggle)
    H_EditPtr = (LavaVariablePtr)(H_EditNode->data.ResultVarPtr);
  else
    H_EditPtr = (LavaVariablePtr)(H_EditNode->data.FIP.up->data.ResultVarPtr);

  funcSect = &(*(LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService)[0];
  Handler_fDesc = &funcSect->funcDesc[handlerDECL->SectionInfo1];
  int fsize = Handler_fDesc->stackFrameSize;
  int fsizeBytes = fsize<<2;
	Handler_Stack = new LavaObjectPtr[fsize];
  for (int ii = 0; ii < fsize; ii++)
    Handler_Stack[ii] = 0;
  Handler_Stack[SFH] = (LavaObjectPtr)cheHandler->data.HandlerNode->data.GUIService;
  Handler_Stack[SFH+1] = *(LavaVariablePtr)H_EditNode->data.ResultVarPtr; //old value
  if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj((*((LavaVariablePtr)H_EditNode->data.ResultVarPtr))[0]->classDECL, &Handler_Stack[SFH+2])) {
	  delete [] Handler_Stack;
    Handler_Stack = 0;
    Handler_fDesc = 0;
    return false;
  }
  H_EditNode->data.ResultVarPtr = (CSecTabBase***)&Handler_Stack[SFH+2];
  if (toggle) 
    H_EditNode->data.B = ((CToggleButton*)H_ButtonNode->data.FIP.widget)->isChecked();
  else {
    H_EditNode->data.StringValue = H_ButtonNode->data.StringValue;
    if (!H_EditNode->data.EnumField)
      H_EditNode->data.FIP.up->data.ResultVarPtr = (CSecTabBase***)&Handler_Stack[SFH+2];
    if (enumNum >= 0)
      H_EditNode->data.D = enumNum;
    ConvertAndStore(H_EditNode);
  }
  ContinueExecThread();
  return true;
}

void CmdExecCLASS::ContinueExecThread()
{
  if (!GUIProg->isView) {
    ((LavaGUIDialog*)GUIProg->ViewWin)->myThread->waitingForUI = false;
    ((LavaGUIDialog*)GUIProg->ViewWin)->myThread->handler_Call = true;
    ((LavaGUIDialog*)GUIProg->ViewWin)->myThread->waitingForUI = false;
    ((LavaGUIDialog*)GUIProg->ViewWin)->myThread->resume();
  }
  
  else {
    GUIProg->myDoc->m_execThreadPtr->waitingForUI = false;
    if (GUIProg->myDoc->ThreadList->isEmpty())
      QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaStart, (void*)GUIProg->myDoc));
    else
      GUIProg->myDoc->m_execThreadPtr->resume();
  }
  GUIProg->ViewWin->setEnabled(false);
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
  //handle = 0;
  Handler_Stack = 0;
  H_ButtonNode = 0;
}
