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

/**********************************************************************

               implementation of class CmdExecCLASS

 **********************************************************************/

#include "DIO.h"

#include "GUIProg.h"
#include "CmdExec.h"
#include "MakeGUI.h"
#include "BAdapter.h"
#include "qmessagebox.h"
#include "LavaBaseStringInit.h"
#include "TButton.h"
//#include "stdafx.h"

/*
bool CmdExecCLASS::okBType (TBasicType btype,int in)
{
  bool ok, b=false;
  CHEEnumSelId *items;
  unsigned j;
  DString str;

  if (GUIProg->MPTR == 0) return false;
  ok = true;
  if (GUIProg->MPTR->data.Atomic == false)
    ok = false;
  else {
    switch (GUIProg->MPTR->data.BType) {

    case Enumeration:
      ok = false;
      GUIProg->MPTR->data.D  = unsigned(in);
      j = 0;
      items = (CHEEnumSelId*)GUIProg->MPTR->data.FormSyntax->Items.first;
      while (items && !ok)
        if (GUIProg->MPTR->data.D == j) {
          if (items->data.SelectionCode.c
              && (items->data.SelectionCode.l == 1)
              && (items->data.SelectionCode.c[0] == ' '))   {
            GUIProg->MPTR->data.StringValue.Reset(1); 
             
            Conv.FieldLength(GUIProg->MPTR,0);
          }
          else {
            if (items->data.SelectionCode.c == 0) {

              GUIProg->MPTR->data.StringValue = STRING(" ");
            }
            else
              GUIProg->MPTR->data.StringValue = items->data.SelectionCode;
            Conv.FieldLength(GUIProg->MPTR,0);
          }
          ok = true;
        }
        else {
          items = (CHEEnumSelId*)items->successor;
          j++;
        }
      if (!ok)
        if (GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedProg)
            && !GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedUser))
          ok = Conv.DataLengthOK(GUIProg->MPTR);
      break;

    case Integer:
      j = 0;
      Conv.IntToString(in,str,j);
      GUIProg->MPTR->data.I  = in;
      GUIProg->MPTR->data.StringValue = str;
      Conv.FieldLength(GUIProg->MPTR, 0);
      if (GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedProg)
          && !GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedUser))
        ok = Conv.DataLengthOK(GUIProg->MPTR);
      break;

    case B_Bool:
      if (btype != B_Bool)
        ok = false;
      else {
        Conv.BoolToString(b,str,ok);
        if (GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedProg)
            && !GUIProg->MPTR->data.IoSigFlags.Contains(UnprotectedUser)
            && ok)
          ok = Conv.DataLengthOK(GUIProg->MPTR);
        if (ok)
          GUIProg->MPTR->data.B   = b;
        else ok = false;
      }
      break;

    
    case Float:
      if (btype != Float)
        ok = false;
      break;
    
    case Double:
      if (btype != Double)
        ok = false;
      break;
    case Identifier:
      if (btype != Identifier)
        ok = false;
      break;

    case Char:
      if (btype != Char)
        ok = false;
      break;

    default:
      ;
    }
  }
  return ok;
} // END OF okBType
*/

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
  CHEFormNode *optNode, *popupNode;
  LavaDECL *formSyn;

  optNode = fNode->data.FIP.up;
  formSyn = optNode->data.FormSyntax;
  optNode->data.SubTree.Destroy();
  if (LBaseData->inRuntime) {
    if (!((CGUIProg*)GUIProg)->LavaForm.AllocResultObj(formSyn, (LavaVariablePtr)optNode->data.ResultVarPtr))
      return;
    ((CGUIProg*)GUIProg)->OnModified();
  }
  ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
  ((CGUIProg*)GUIProg)->LavaForm.PartialForm(formSyn, optNode); 
  if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
    if (!LBaseData->inRuntime) 
      QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), "Empty optional element",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
  GUIProg->focNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (optNode, optNode);
  GUIProg->CurPTR = GUIProg->focNode;
  popupNode = InPopupShell(optNode);
  if (popupNode)
    ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
  else
    ((CGUIProg*)GUIProg)->RedrawForm();
  ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(GUIProg->focNode);     
}

void CmdExecCLASS::DeleteOptionalItem ( CHEFormNode* fNode)
{
  CHEFormNode *popupNode, *parNode = fNode->data.FIP.up;
  fNode = (CHEFormNode*)parNode->data.SubTree.Uncouple(fNode); 

  CHEFormNode* elliNode = 0;
  LavaDECL* formSyn = parNode->data.FormSyntax;
  ((CGUIProg*)GUIProg)->LavaForm.CreateEllipsis(elliNode,formSyn);
  parNode->data.SubTree.Insert(fNode->predecessor, elliNode);
  if (fNode->data.BasicFlags.Contains(Groupbox))
    elliNode->data.BasicFlags.INCL(Groupbox);
  elliNode->data.FIP.up = parNode;
  GUIProg->focNode = 0;
  GUIProg->CurPTR = elliNode;
  if (fNode->data.FIP.popupShell) {
    delete fNode->data.FIP.popupShell;
    fNode->data.FIP.popupShell = 0;
    fNode->data.FIP.poppedUp = false;
  }
  popupNode = InPopupShell(elliNode);
  if (popupNode)
    ((CGUIProg*)GUIProg)->MakeGUI.Popup(popupNode,true, true, true);
  else
    ((CGUIProg*)GUIProg)->RedrawForm();
  ((CGUIProg*)GUIProg)->MakeGUI.CursorOnField(elliNode);     
  ((CGUIProg*)GUIProg)->MakeGUI.VisibleDeleteButton(elliNode,false);
  if (LBaseData->inRuntime) {
  //to do: delete the object *fNode->data.ResultVarPtr
   *fNode->data.ResultVarPtr = 0;
    ((CGUIProg*)GUIProg)->OnModified();
  }
  delete fNode;
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
  CHEFormNode *popupNode, *beforeNode, *chainNode, *parNode, *insertedNode;
  LavaObjectPtr newStackFrame[4], handle = 0;
  LavaDECL *iterSyn, *formSyn;

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
      newStackFrame[0] = *(LavaVariablePtr)chainNode->data.ResultVarPtr; //chain object
      if (beforeNode->data.ResultVarPtr)
        newStackFrame[1] = (LavaObjectPtr)beforeNode->data.HandleObjPtr; //before handle
      else
        newStackFrame[1] =  0;
      newStackFrame[2] = *(LavaVariablePtr)insertedNode->data.ResultVarPtr; //new data
      newStackFrame[2] = newStackFrame[2] + newStackFrame[2][0][((CGUIProg*)GUIProg)->myDoc->GetSectionNumber(((CGUIProg*)GUIProg)->ckd, newStackFrame[2][0][0].classDECL, GUIProg->myDoc->DECLTab[B_Object])].sectionOffset;
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown)
        return;
      ((CGUIProg*)GUIProg)->ex = HSetInsertBefore(((CGUIProg*)GUIProg)->ckd, newStackFrame);
      ((SynFlags*)(newStackFrame[2]+1))->INCL(insertedItem);
      if (((CGUIProg*)GUIProg)->ex)
        return;
      handle = newStackFrame[3];
      DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,*(LavaVariablePtr)insertedNode->data.ResultVarPtr); //decr from AllocateObject after putting into chain
      ((CGUIProg*)GUIProg)->OnModified();
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
  ((CGUIProg*)GUIProg)->LavaForm.emptyInsertion = true;
  ((CGUIProg*)GUIProg)->LavaForm.PartialForm(formSyn, insertedNode); 
  GUIProg->focNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (insertedNode, insertedNode);
  if (((CGUIProg*)GUIProg)->LavaForm.emptyInsertion) {
    delete insertedNode;
    if (!LBaseData->inRuntime) 
      QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), "Empty chain element",  QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
  insertedNode->data.HandleObjPtr = (CSecTabBase**)handle;
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

void CmdExecCLASS::DeleteIterItem (CHEFormNode* fNode)
{
  CHEFormNode *popupNode, *delNode,  *chainNode;
  LavaObjectPtr newStackFrame[3];

  if (fNode->data.IterFlags.Contains(Ellipsis))
    delNode = (CHEFormNode*)fNode->predecessor;
  else
    if (fNode->data.FIP.up->data.IterFlags.Contains(Ellipsis))
      delNode = (CHEFormNode*)fNode->data.FIP.up->predecessor;
    else
      delNode = fNode;
  if (delNode) {
    delNode = (CHEFormNode*)delNode->data.FIP.up->data.SubTree.Uncouple(delNode); 
    if (LBaseData->inRuntime) {
      //to do: remove chain elem from object
      chainNode = delNode->data.FIP.up;
      while (chainNode && !chainNode->data.ResultVarPtr)
        chainNode = chainNode->data.FIP.up;
      if (chainNode) {
        if (GUIProg->fromFillIn) {
          newStackFrame[0] = *(LavaVariablePtr)chainNode->data.ResultVarPtr;
          newStackFrame[1] = (LavaObjectPtr)delNode->data.HandleObjPtr;
          if (!SetRemove(((CGUIProg*)GUIProg)->ckd, newStackFrame))
            return;
        }
        else
          ((SynFlags*)((LavaObjectPtr)delNode->data.HandleObjPtr+1))->INCL(deletedItem);
        ((CGUIProg*)GUIProg)->OnModified();
      }
      else {
        GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, delNode->data.FormSyntax, &ERR_ErrInForm,0);
        return;
      }
    }
    GUIProg->focNode = 0;
    GUIProg->newFocus = 0;
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
} // END OF DeleteIterItem

/*
bool CmdExecCLASS::ChangeAtomicObject(LavaVariablePtr resultVarPtr)
{
  LavaObjectPtr objectPtr;


  objectPtr = 0;
  if (!((SynFlags*)((*resultVarPtr) - (*resultVarPtr)[0][0].sectionOffset+1))->Contains(notFromSource)) {
    try {
#ifndef WIN32
      if (setjmp(contOnHWexception)) throw hwException;
#endif
      ((CGUIProg*)GUIProg)->ex = CopyObject(((CGUIProg*)GUIProg)->ckd, resultVarPtr, &objectPtr, false);
      if (((CGUIProg*)GUIProg)->ex)
        return false;
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown)
        return false;
    }
    catch (CRuntimeException* ex) {
      if (!ex->SetLavaException(((CGUIProg*)GUIProg)->ckd)) 
        throw;
      return false;
    }
    catch (CHWException* ex) {
      if (!ex->SetLavaException(((CGUIProg*)GUIProg)->ckd)) 
        throw;
      return false;
    }
    DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd, *resultVarPtr);
    *resultVarPtr = objectPtr;
    ((SynFlags*)(objectPtr - objectPtr[0][0].sectionOffset+1))->INCL(notFromSource);
  }
  return true;
}
*/

bool CmdExecCLASS::ConvertAndStore (CHEFormNode* trp)
{
  CHEEnumSelId *enumItem;
  //unsigned c, width, digits;
  unsigned j;
  int i;
//  bool b; 
  bool Done = true;
  DString strx;
  DString str0;
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
//            if (!ChangeAtomicObject(enumvarPtr))
//              return false;
            enumPtr = *enumvarPtr;
            enumPtr = CastEnumType(((CGUIProg*)GUIProg)->ckd, enumPtr);
            if (!enumPtr)
              return false;
            *(int*)(enumPtr + LSH) = j;
            pstr = (QString*)(enumPtr+LSH+1);
            if (*pstr) 
              *pstr = QString(enumItem->data.Id.c);
            else
              NewQString(pstr, enumItem->data.Id.c);
            //((CGUIProg*)GUIProg)->OnModified(); done below
          }
          Done = true;
        }
        if (!Done) {
          //msg.LoadString(&ERR_Odd_char);
          //AfxMessageBox(msg,MB_OK|MB_ICONERROR);
          QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), ERR_Odd_char,  QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
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
//          if (!ChangeAtomicObject((LavaVariablePtr)trp->data.ResultVarPtr))
//            return false;
          *(int*)(*(trp->data.ResultVarPtr)+LSH) = i;
        }
      }
      else {
        //msg.LoadString(&ERR_Odd_char);
        //AfxMessageBox(msg,MB_OK|MB_ICONERROR);
        QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), ERR_Odd_char,  QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        ((CGUIProg*)GUIProg)->MakeGUI.setFocus(GUIProg->ErrPos,trp);
      }
      break;

    case B_Bool:
      /*
      Done = trp->data.BasicFlags.Contains(Toggle);
      Conv.StringToBool(trp->data.StringValue,b,Done);
      if (Done) {
        trp->data.B = b;
        if (LBaseData->inRuntime)
          *(bool*)(*(trp->data.ResultVarPtr)+LSH) = b;
      }
      else {
        msg.LoadString(&ERR_Odd_char);
        AfxMessageBox(msg,MB_OK|MB_ICONERROR);
        ((CGUIProg*)GUIProg)->MakeGUI.setFocus(GUIProg->ErrPos,trp);
      }
      */
      break;

    case Float:
      trp->data.F = (float)atof(trp->data.StringValue.c);
      if (LBaseData->inRuntime) {
//        if (!ChangeAtomicObject((LavaVariablePtr)trp->data.ResultVarPtr))
//          return false;
        *(float*)(*(trp->data.ResultVarPtr)+LSH) = trp->data.F;
      }
      /*
      else {
        msg.LoadString(&ERR_Odd_char);
        AfxMessageBox(msg,MB_OK|MB_ICONERROR);
        ((CGUIProg*)GUIProg)->MakeGUI.setFocus(GUIProg->ErrPos,trp);
      }*/
      break;

    case Double:
      trp->data.Db = atof(trp->data.StringValue.c);
      if (LBaseData->inRuntime) {
        *(double*)(*(trp->data.ResultVarPtr)+LSH) = trp->data.Db;
//        if (!ChangeAtomicObject((LavaVariablePtr)trp->data.ResultVarPtr))
//          return false;
      }
      break;

    case Char:
      if (LBaseData->inRuntime) {
//        if (!ChangeAtomicObject((LavaVariablePtr)trp->data.ResultVarPtr))
//          return false;
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
        
//        if (!ChangeAtomicObject((LavaVariablePtr)trp->data.ResultVarPtr))
//          return false;
        pstr = (QString*)(*(trp->data.ResultVarPtr)+LSH);
        if (*pstr) 
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
  return Done && !((CGUIProg*)GUIProg)->ckd.exceptionThrown;
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
      /*
      if (!trp->data.StringValue.l || !trp->data.B)
        trp->data.StringValue = STRING("x");
      else
        trp->data.StringValue = STRING(" ");
        */
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
  return Done && !((CGUIProg*)GUIProg)->ckd.exceptionThrown;
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
}
