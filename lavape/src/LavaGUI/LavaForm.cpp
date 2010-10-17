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


#include "ASN1pp.h"
#include "CDPpp.h"
#include "Convert.h"
#include "BAdapter.h"
#include "DIO.h"
#include "Syntax.h"
#include "GUIProg.h"
#include "GUIProgBase.h"
#include "LavaForm.h"
#include "qmessagebox.h"
//Added by qt3to4:
#include <QPixmap>
#include "qpixmapcache.h"
#include "LavaBaseStringInit.h"
#include "LavaGUIView.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "mdiframes.h"

#pragma hdrstop


#define RETURNFALSE() {\
  if (!GUIProg->isView) {\
    if (((CGUIProg*)GUIProg)->ckd.exceptionThrown) \
      ((CGUIProg*)GUIProg)->ex = new CRuntimeException(check_ex, &((CGUIProg*)GUIProg)->ckd.exceptionMsg);\
    else\
      ((CGUIProg*)GUIProg)->ex = new CRuntimeException(memory_ex, &ERR_AllocObjectFailed);\
    ((LavaGUIDialog*)GUIProg->ViewWin)->OnCancel();\
  }\
  else {\
    CheckData* ckdPtr = new CheckData();\
    *ckdPtr = ((CGUIProg*)GUIProg)->ckd;\
    ((CGUIProg*)GUIProg)->ckd.synError = 0;\
    QApplication::postEvent(wxTheApp, new CustomEvent(UEV_DebugStop, (void*)ckdPtr));\
  }\
  return false;\
}

bool LavaFormCLASS::AllocResultObj(LavaDECL *syn, LavaVariablePtr resultObjPtr, bool emptyOpt)
{
//  LavaObjectPtr serv;
  LavaDECL *servDECL;
 
  bool newObj = !(*resultObjPtr) && !emptyOpt;
  if (!syn) {
    GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, syn, &ERR_ErrInForm,0);
    RETURNFALSE()  //return false;
  }
  if ((syn->DeclType == Attr) || (syn->DeclType == VirtualType) || (syn->DeclType == PatternDef)) {
    if (syn->RuntimeDECL->DeclType == FormDef) {
      if (!syn->RuntimeDECL->WorkFlags.Contains(runTimeOK))
        if (!GUIProg->myDoc->CheckForm(((CGUIProg*)GUIProg)->ckd, syn->RuntimeDECL))
          RETURNFALSE() //return false;
        if (newObj) {
          servDECL = syn->RuntimeDECL->ParentDECL->RuntimeDECL;
          if (!servDECL->WorkFlags.Contains(runTimeOK))
            if (!GUIProg->myDoc->CheckImpl(((CGUIProg*)GUIProg)->ckd, servDECL))
              RETURNFALSE()  //return false;
          *resultObjPtr = AllocateObject(((CGUIProg*)GUIProg)->ckd, syn->RuntimeDECL->RelatedDECL, syn->TypeFlags.Contains(isStateObjectY));
          if (!*resultObjPtr)
            RETURNFALSE() 
          if (GUIProg->myDoc->isObject
            && !CallDefaultInit(((CGUIProg*)GUIProg)->ckd, *resultObjPtr))
            RETURNFALSE() //return false;
        }
    }
    else
      if (newObj) {
        *resultObjPtr = AllocateObject(((CGUIProg*)GUIProg)->ckd, syn->RuntimeDECL, syn->TypeFlags.Contains(isStateObjectY));
        if (!*resultObjPtr) 
          RETURNFALSE() 
        if (GUIProg->myDoc->isObject
          && !CallDefaultInit(((CGUIProg*)GUIProg)->ckd, *resultObjPtr))
          RETURNFALSE()  //return false;
      }
  }
  else
    if (syn->DeclType == FormDef) {
      if (!syn->WorkFlags.Contains(runTimeOK))
        if (!GUIProg->myDoc->CheckForm(((CGUIProg*)GUIProg)->ckd, syn))
          RETURNFALSE()  //return false;
      if (newObj) {
        servDECL = syn->ParentDECL->RuntimeDECL;
        if (!servDECL->WorkFlags.Contains(runTimeOK))
          if (!GUIProg->myDoc->CheckImpl(((CGUIProg*)GUIProg)->ckd, servDECL))
            RETURNFALSE()  //return false;
        *resultObjPtr = AllocateObject(((CGUIProg*)GUIProg)->ckd, syn->RelatedDECL, false); //??
        if (!*resultObjPtr)
          RETURNFALSE() 
        if (GUIProg->myDoc->isObject
          && !CallDefaultInit(((CGUIProg*)GUIProg)->ckd, *resultObjPtr))
          RETURNFALSE()  //return false;
      }
    }
    else {
      *resultObjPtr = AllocateObject(((CGUIProg*)GUIProg)->ckd, syn, false); //??
      if (!*resultObjPtr) 
        RETURNFALSE() 
      if (GUIProg->myDoc->isObject
         && !CallDefaultInit(((CGUIProg*)GUIProg)->ckd, *resultObjPtr))
        RETURNFALSE()  //return false;
    }
  if (newObj) {
    ((SynFlags*)((*resultObjPtr)+1))->INCL(useDefaults);
//    ((SynFlags*)((*resultObjPtr)+1))->INCL(notFromSource);
    //((SynFlags*)((*resultObjPtr)+1))->INCL(finished);
  }
  return true;
}



bool LavaFormCLASS::AllocFNode (CHEFormNode *&formNode, 
                                LavaDECL *syn, 
                                LavaVariablePtr resultObjPtr)

{
  formNode = (CHEFormNode*)NewCHEFormNode();
  formNode->data.Atomic = false;
  formNode->data.BType = NonBasic;
  formNode->data.FormSyntax = syn;
  if (syn && syn->DeclType == FormText)
    formNode->data.ResultVarPtr = 0;
  else {
    formNode->data.ResultVarPtr = (CSecTabBase***)resultObjPtr;
    if (resultObjPtr)
      if (!AllocResultObj(syn, resultObjPtr, syn->TypeFlags.Contains(isOptional)))
        return false;
  }
  formNode->data.Annotation.ptr = 0;
  formNode->data.FIP.widget = 0;
  formNode->data.FIP.frameWidget = 0;
  formNode->data.FIP.popupShell = 0;
  formNode->data.FIP.borderWidth = 0;
  formNode->data.FIP.leng = 0;
  formNode->data.FIP.visible = 0;
  formNode->data.FIP.poppedUp = false;
  formNode->data.FIP.up = 0;
  QString dd = QString(((TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable)->DocDir.c);
  QString xf;
  if (syn && syn->Annotation.ptr) {
    CHETAnnoEx* ex = syn->Annotation.ptr->GetAnnoEx(anno_Pixmap);
    if (ex && ex->data.xpmFile.l) {
      QDir qdir(dd);//QString(((TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable)->DocDir.c));
      xf = QString(ex->data.xpmFile.c);
      QFileInfo qf(qdir, xf);//QString(ex->data.xpmFile.c));
      formNode->data.Pixmap = new QPixmap(qf.absoluteFilePath());
    }
    if (syn->Annotation.ptr->String1.l && formNode->data.LFont.fromString(syn->Annotation.ptr->String1.c))
      formNode->data.ownLFont = ownFont;
    if (syn->Annotation.ptr->String2.l && formNode->data.TFont.fromString(syn->Annotation.ptr->String2.c))
      formNode->data.ownTFont = ownFont;
    ex = syn->Annotation.ptr->GetAnnoEx(anno_Color);
    if (ex) {
      formNode->data.ColorBValid = ex->data.RgbBackValid;
      formNode->data.ColorFValid = ex->data.RgbForeValid;
      if (formNode->data.ColorBValid)
        formNode->data.ColorB = QColor(ex->data.RgbBackColor);
      if (formNode->data.ColorFValid)
        formNode->data.ColorF = QColor(ex->data.RgbForeColor);  
    }
    else {
      formNode->data.ColorBValid = false;
      formNode->data.ColorFValid = false;
    }
  }
  return true;
//  formNode->data.FIP.pred = 0;
//  formNode->data.FIP.fieldNode = 0;
} // END OF AllocFNode


TAnnotation *LavaFormCLASS::inheritAnnotation ( LavaDECL *DECLptr)

{
  if (!DECLptr)
    return 0;
  if (DECLptr->Annotation.ptr)
    return DECLptr->Annotation.ptr;
  if (DECLptr->DeclDescType == NamedType) {
    if (LBaseData->inRuntime)
      DECLptr = DECLptr->RuntimeDECL;
    else
      DECLptr = ((TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable)->GetDECL(DECLptr->RefID, DECLptr->inINCL);
    return inheritAnnotation(DECLptr);
  }
  else return 0;
} // END OF inheritAnnotation

void LavaFormCLASS::CreateEllipsis (CHEFormNode *&fNode, LavaDECL *DECLptr, bool allowHandler)

{
  DString insertText;
  CHEFormNode *insertButton, *deleteButton;

  if (DECLptr->TypeFlags.Contains(isPlaceholder)) 
    return;
  if (!fNode)
    AllocFNode(fNode, DECLptr, 0);
  fNode->data.IterFlags.INCL(Ellipsis);
  TAnnotation* iteranno = (TAnnotation*)currentIteration->Annotation.ptr->FA.ptr;
  TAnnotation* anno = (TAnnotation*)DECLptr->Annotation.ptr->FA.ptr;
  fNode->data.FormSyntax = DECLptr;
  fNode->data.Annotation.ptr = inheritAnnotation(DECLptr);
  fNode->data.allowOwnHandler = allowHandler;
  if (fNode->data.Annotation.ptr) {
    fNode->data.Annotation.ptr = (TAnnotation*)fNode->data.Annotation.ptr->FA.ptr;
//    fNode->data.Annotation.ptr->FrameSpacing = DECLptr->Annotation.ptr->FrameSpacing;
//    fNode->data.Annotation.ptr->TabPosition = DECLptr->Annotation.ptr->TabPosition;
  }
  if (anno->IterFlags.Contains(Optional)) {
    fNode->data.Atomic = true;
    emptyInsertion = false;
    fNode->data.IterFlags.INCL(Optional);
    insertText = ellipsis;
    fNode->data.IoSigFlags = SET(UnprotectedUser,-1);
  }
  else {
    if (!anno) {
      fNode->data.Atomic = true;
      emptyInsertion = false;
      insertText = ellipsis;
    }
    else if (!iteranno->String2.l) {
      fNode->data.Atomic = true;
      emptyInsertion = false;
      if (!iteranno->String1.l)
        insertText = ellipsis;
      else insertText = iteranno->String1;
    }
    else fNode->data.Atomic = false;
  }
  if (fNode->data.Atomic) {
    fNode->data.BType = VLString;
    /* must just be # HexByte... to avoid Destroy(delVar.H) */
    fNode->data.BasicFlags = fNode->data.BasicFlags+SET(Atomic,-1);
    fNode->data.IoSigFlags.INCL(UnprotectedUser);
    fNode->data.StringValue = insertText;
  }
  else {
    AllocFNode(insertButton,DECLptr, 0);
    insertButton->data.Atomic = true;
    insertButton->data.allowOwnHandler = allowHandler;
    emptyInsertion = false;
    fNode->data.SubTree.Append(insertButton);
    insertButton->data.FIP.up = fNode;
    insertButton->data.BType = VLString;
    insertButton->data.BasicFlags = SET(Atomic,-1);
    insertButton->data.IoSigFlags = SET(UnprotectedUser,-1);
    SetIoFlags(fNode, insertButton);
    insertButton->data.StringValue = iteranno->String1;
    AllocFNode(deleteButton,DECLptr, 0);
    fNode->data.SubTree.Append(deleteButton);
    deleteButton->data.Atomic = true;
    emptyInsertion = false;
//    deleteButton->data.FIP.pred = insertButton;
    deleteButton->data.FIP.up = fNode;
    deleteButton->data.BType = VLString;
    deleteButton->data.BasicFlags = SET(Atomic,-1);
    deleteButton->data.IoSigFlags = SET(UnprotectedUser,-1);
    SetIoFlags(fNode, deleteButton);
    deleteButton->data.allowOwnHandler = allowHandler;
    deleteButton->data.StringValue = iteranno->String2;
  }
} // END OF CreateEllipsis

void LavaFormCLASS::SetIoFlags(CHEFormNode* upNode, CHEFormNode* subNode)
{
  if (upNode->data.IoSigFlags.Contains(DONTPUT))
    subNode->data.IoSigFlags.INCL(DONTPUT);
  /*
  if (!upNode->data.IoSigFlags.Contains(UnprotectedUser))
    subNode->data.IoSigFlags.EXCL(UnprotectedUser);
  if (!upNode->data.IoSigFlags.Contains(UnprotectedProg))
    subNode->data.IoSigFlags.EXCL(UnprotectedProg);
  */
}

void LavaFormCLASS::MakeForm ( LavaDECL* formDECL, LavaVariablePtr resultPtr,
                                 CHEFormNode*& empForm)

{
  LavaDECL *DECLptr;
  TAnnotation *annoPtr;
  DString ignoreString;
  CHEFormNode *newFNode; //, *oldFNode
  TIDs *ClassChain = 0;

  DECLptr = NewLavaDECL();
  DECLptr->Annotation.ptr = NewTAnnotation();
  DECLptr->Annotation.ptr->BasicFlags = SET(Atomic,-1);
  DECLptr->Annotation.ptr->IoSigFlags = SET(UnprotectedProg,-1);
  DECLptr->Annotation.ptr->Length.FieldBoundRel = LG;
  DECLptr->Annotation.ptr->Length.Field = 1;
  DECLptr->Annotation.ptr->Length.DataBoundRel = GE;
  DECLptr->Annotation.ptr->Length.Data = 0;
  DECLptr->DeclDescType = BasicType;
  DECLptr->BType = VLString;
  AllocFNode(newFNode,DECLptr, 0); /* for prefix text of entire form */
  newFNode->data.BasicFlags = SET(Atomic,-1);
  newFNode->data.IoSigFlags = SET(UnprotectedProg,-1);
  newFNode->data.Atomic = true;
  newFNode->data.allowOwnHandler = true;
  emptyInsertion = false;
  newFNode->data.BType = VLString;
  empForm = newFNode;
/*
    newFNode->data.Atomic = false;
    CHE* DECLCHptrAny = (CHE*)formDECL->Annotation.ptr->Prefixes.first;
    exprList(0, DECLCHptrAny, 0, SET(Flag_INPUT,Flag_OUTPUT,-1), newFNode, true);
*/

  DECLptr = formDECL;
  emptyForm = true;
  if (!LBaseData->inRuntime)
    ClassChain = new TIDs;
  partialForm(DECLptr, DECLptr, resultPtr, SET(Flag_INPUT,Flag_OUTPUT,-1), newFNode, true, true, ClassChain);
  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
    return;
  if (ClassChain)
    delete ClassChain;
  emptyForm = false;
  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex)
    return ;
  empForm->successor = newFNode;
//  newFNode->data.FIP.pred = empForm;
//  oldFNode = newFNode;

  DECLptr = NewLavaDECL();
  annoPtr = NewTAnnotation();
  AllocFNode(newFNode,DECLptr, 0); /* for suffix text of entire form */
  empForm->successor->successor = newFNode;
//  newFNode->data.FIP.pred = oldFNode;
  DECLptr->Annotation.ptr = annoPtr;
  DECLptr->Annotation.ptr->BasicFlags = SET(Atomic,-1);
  DECLptr->Annotation.ptr->IoSigFlags = SET(UnprotectedProg,-1);
  DECLptr->Annotation.ptr->Length.FieldBoundRel = LG;
  DECLptr->Annotation.ptr->Length.Field = 1;
  DECLptr->Annotation.ptr->Length.DataBoundRel = GE;
  DECLptr->Annotation.ptr->Length.Data = 0;
  DECLptr->DeclDescType = BasicType;
  DECLptr->BType = VLString;
  annoPtr->Emphasis = Low;
  annoPtr->IndType = (TIndentation)DFTIndType;
  annoPtr->Indentation = 0;
  annoPtr->JustType = (TJustification)DFTJustification;
  annoPtr->Alignment = (TAlignment)DFTAlignment;
  annoPtr->Box = (TBox)DFTBox;
  annoPtr->Space = 0;
  annoPtr->TabPosition = -1;
  annoPtr->FrameSpacing = 1;
  newFNode->data.Annotation.ptr = annoPtr;
  newFNode->data.BasicFlags = SET(Atomic,-1);
  newFNode->data.IoSigFlags = SET(UnprotectedProg,-1);
  newFNode->data.Atomic = true;
  newFNode->data.allowOwnHandler = true;
  emptyInsertion = false;
  newFNode->data.BType = VLString;
  /*
    newFNode->data.Atomic = false;
    DECLCHptrAny = (CHE*)formDECL->Annotation.ptr->Suffixes.first;
    exprList(0, DECLCHptrAny, 0, SET(Flag_INPUT,Flag_OUTPUT,-1), newFNode, true);
  */
  return ;
} // END OF MakeForm


void LavaFormCLASS::PartialForm (LavaDECL* FormDecl, CHEFormNode *&fNode, bool allowHandler)

{
  TIDs *ClassChain = 0;
  CHEFormNode *newFNode;
  SynFlags defaultIOflags;
  CHE* che;
  if (!LBaseData->inRuntime)
    ClassChain = new TIDs;

  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
    return ;
  if (fNode) {
    defaultIOflags = fNode->data.IoSigFlags*SET(Flag_INPUT,Flag_OUTPUT,-1);
    fNode->data.Annotation.ptr = FormDecl->Annotation.ptr;
    fNode->data.BasicFlags = FormDecl->Annotation.ptr->BasicFlags;
    fNode->data.IterFlags = FormDecl->Annotation.ptr->IterFlags;
    fNode->data.FormSyntax = FormDecl;

    FormDecl->Annotation.ptr->BasicFlags.EXCL(Atomic);
    fNode->data.Atomic = false;
    che = (CHE*)FormDecl->Annotation.ptr->Prefixes.first;
    exprList(che, defaultIOflags, fNode, true);
  }

  partialForm(FormDecl, FormDecl, (LavaVariablePtr)fNode->data.ResultVarPtr, defaultIOflags, newFNode, true, allowHandler, ClassChain);
  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
    return;
  if (fNode) {
    if (FormDecl->Annotation.ptr->IterFlags.Contains(Optional)) {
      fNode->data.IterFlags.INCL(Optional);
      fNode->data.IterFlags.EXCL(NoEllipsis);
    }
    newFNode->data.FIP.up = fNode;
    SetIoFlags(fNode, newFNode);
    fNode->data.SubTree.Append(newFNode);
    che = (CHE*)FormDecl->Annotation.ptr->Suffixes.first;
    exprList(che, defaultIOflags, fNode, true);
  }

  if (ClassChain)
    delete ClassChain;
} // END OF PartialForm


bool LavaFormCLASS::setDefaultValue (CHEFormNode *resultFNode)

{
  LavaVariablePtr rPtr;
  LavaObjectPtr enumPtr, totalObj;
  LavaDECL *decl, *enumSyntax=0;
  TAnnotation *anno;
  CHEEnumSelId *enumSel=0;
  DString str(100);
  QString qstr, *pstr;
  int iVal;
  bool dftDefined;

  anno = resultFNode->data.Annotation.ptr;
  if (!resultFNode->data.Atomic)
    if (resultFNode->data.FormSyntax->DeclDescType == EnumType)
      if (resultFNode->data.FormSyntax->Annotation.ptr->BasicFlags.Contains(MenuFirst))
        resultFNode = (CHEFormNode*)resultFNode->data.SubTree.first->successor;
      else
        resultFNode = (CHEFormNode*)((CHEFormNode*)resultFNode->data.SubTree.first)->data.SubTree.first;
    else
      return true;

  if (resultFNode->data.IoSigFlags.Contains(DONTPUT))
    return true;

  rPtr = (LavaVariablePtr)resultFNode->data.ResultVarPtr;
  decl = resultFNode->data.FormSyntax;
  switch (resultFNode->data.BType) {
  case B_Bool:
    resultFNode->data.StringValue.Destroy();
    resultFNode->data.B = false;
    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr)
     totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      if (anno && (anno->BType == B_Bool)) {
        if (anno->IoSigFlags.Contains(trueValue)) {
          resultFNode->data.B = anno->B;
          resultFNode->data.FIP.leng = 1;
          if (resultFNode->data.BasicFlags.Contains(Toggle))
            if (resultFNode->data.B)
              resultFNode->data.StringValue = DString("x");
            else
              resultFNode->data.StringValue.Reset(1);
          else
            if (resultFNode->data.B)
              resultFNode->data.StringValue = DString("j");
            else
              resultFNode->data.StringValue = DString("n");
          if (rPtr)
            *(bool*)((*rPtr)+LSH) = resultFNode->data.B;
          resultFNode->data.IoSigFlags.INCL(trueValue);
        }
      }
    }
    else 
      if (rPtr) {
        resultFNode->data.B = *(bool*)((*rPtr)+LSH);
        resultFNode->data.FIP.leng = 1;
        if (resultFNode->data.BasicFlags.Contains(Toggle))
          if (resultFNode->data.B)
            resultFNode->data.StringValue = DString("x");
          else
            resultFNode->data.StringValue.Reset(1);
        else
          if (resultFNode->data.B)
            resultFNode->data.StringValue = DString("true");
          else
            resultFNode->data.StringValue = DString("false");
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    break;

  case Integer:
    resultFNode->data.StringValue.Destroy();
    resultFNode->data.I = 0;
    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr)
     totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      if (anno && (anno->BType == Integer)) {
        resultFNode->data.I = anno->I;
        if (((TAnnotation*)resultFNode->data.FormSyntax->Annotation.ptr->FA.ptr)->IoSigFlags.Contains(trueValue))
          Convert.IntToString(resultFNode->data.I, str);
        resultFNode->data.FIP.leng = str.l;
        resultFNode->data.StringValue = str;
        if (rPtr)
          *(int*)((*rPtr)+LSH) = resultFNode->data.I;
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    }
    else
      if (rPtr) {
        resultFNode->data.I = *(int*)((*rPtr)+LSH);
        Convert.IntToString(resultFNode->data.I, str);
        resultFNode->data.FIP.leng = str.l;
        resultFNode->data.StringValue = str;
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    break;
  case Float:
  case Double:
    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr)
      totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      if (anno  && anno->StringValue.c && anno->StringValue.l ) {
        resultFNode->data.StringValue = anno->StringValue;
        resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
        if (resultFNode->data.BType == Float) {
          resultFNode->data.F = (float)atof(resultFNode->data.StringValue.c);
          if (rPtr) 
            *(float*)((*rPtr)+LSH) = resultFNode->data.F;
        }
        else {
          resultFNode->data.Db = atof(resultFNode->data.StringValue.c);
          if (rPtr) 
            *(double*)((*rPtr)+LSH) = resultFNode->data.Db;
        }
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
      else
        resultFNode->data.IoSigFlags.EXCL(trueValue);
    }
    else 
      if (rPtr) {
        resultFNode->data.StringValue.Reset(100);
        if (resultFNode->data.BType == Float) {
          resultFNode->data.F = *(float*)((*rPtr)+LSH);
          qstr.setNum(resultFNode->data.F);
          qstr = qstr.rightJustified(anno->Length.Field,' ',true);
          resultFNode->data.StringValue = STRING(qPrintable(qstr));
      //    Conv.RealToString(resultFNode->data.F, 0, anno->Length.Field, resultFNode->data.StringValue, ok);
        }
        else {
          resultFNode->data.Db = *(double*)((*rPtr)+LSH);
          resultFNode->data.F = *(float*)((*rPtr)+LSH);
          qstr.setNum(resultFNode->data.Db);
          qstr = qstr.rightJustified(anno->Length.Field,' ',true);
          resultFNode->data.StringValue = STRING(qPrintable(qstr));
//          Conv.RealToString(resultFNode->data.Db, 0, anno->Length.Field, resultFNode->data.StringValue, ok);
        }
        resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
      else
        resultFNode->data.IoSigFlags.EXCL(trueValue);
    break;

  case VLString:
    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr)
      totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      if (anno && (anno->BType == VLString) && anno->StringValue.c ) {
        resultFNode->data.StringValue = anno->StringValue;
        resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
        if (rPtr) {
          pstr = (QString*)((*rPtr)+LSH);
          if (!pstr->isEmpty()) 
            *pstr = QString(resultFNode->data.StringValue.c);
          else
            NewQString(pstr, resultFNode->data.StringValue.c);
        }
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    }
    else 
      if (rPtr) {
        pstr = (QString*)((*rPtr)+LSH);
        if (!pstr->isEmpty()) {
          resultFNode->data.StringValue = STRING(qPrintable((*pstr)));
          resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
          resultFNode->data.IoSigFlags.INCL(trueValue);
        }
      }
    break;

  case Char:
    resultFNode->data.StringValue.Destroy();
    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr)
      totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      if (anno && ((anno->BType == VLString) || (anno->BType == Char))
           && anno->StringValue.c ) {
        resultFNode->data.StringValue = anno->StringValue;
        resultFNode->data.FIP.leng = 1;
        if (rPtr)
          *(char*)((*rPtr)+LSH) = resultFNode->data.StringValue.c[0];
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    }
    else {
      resultFNode->data.StringValue.Reset(1);
      if (rPtr) {
        resultFNode->data.StringValue = DString(*(char*)((*rPtr)+LSH), 1);
        resultFNode->data.FIP.leng = 1;
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    }

    break;

  case Enumeration:
    if (upNode) {
      resultFNode->data.StringValue.Destroy();
      rPtr = (LavaVariablePtr)upNode->data.ResultVarPtr;
      decl = upNode->data.FormSyntax;
    }
    else
      if (anno->BasicFlags.Contains(DefaultMenu)) {
        enumSyntax = GUIProg->myDoc->IDTable.GetDECL(resultFNode->data.FormSyntax->RefID, resultFNode->data.FormSyntax->inINCL);
        if (!enumSyntax)
          return true;
        enumSyntax = &((TEnumDescription*)enumSyntax->EnumDesc.ptr)->EnumField;
        resultFNode->data.EnumField = enumSyntax;
      }
      else
        enumSyntax = resultFNode->data.FormSyntax;

    if (rPtr && !(*rPtr))
      if (!AllocResultObj(decl, rPtr))
        return false;
    if (rPtr) {
      totalObj = *rPtr - (*rPtr)[0]->sectionOffset;
      enumPtr = CastEnumType(((CGUIProg*)GUIProg)->ckd, *rPtr);
      if (!enumPtr)
        return false;
    }
    iVal = 0;
    if (!LBaseData->inRuntime || rPtr && ((SynFlags*)(totalObj+1))->Contains(useDefaults)) {
      enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
      if (anno) {
        for ( ; enumSel && (enumSel->data.Id != anno->StringValue);
                enumSel = (CHEEnumSelId*)enumSel->successor) 
          iVal++;
      }
    }
    else 
      if (rPtr) {
        pstr = (QString*)(enumPtr+LSH+1);
        if (!pstr->isEmpty()) {
          iVal = *(int*)(enumPtr+LSH);
          resultFNode->data.StringValue = STRING(qPrintable(*pstr));
          resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
          resultFNode->data.IoSigFlags.INCL(trueValue);
          for (enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
               enumSel && (enumSel->data.Id != resultFNode->data.StringValue);
               enumSel = (CHEEnumSelId*)enumSel->successor);
          if (!enumSel) {
            iVal = 0;
            enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
          }
        }
        else {
          enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
        }  
      }

    if (upNode
        && (upNode->data.BasicFlags.Contains(OptionMenu)
            || upNode->data.BasicFlags.Contains(PopUpMenu))) {
      for ( ; enumSel &&  enumSel->data.SelectionCode == blank;
              enumSel = (CHEEnumSelId*)enumSel->successor) 
        iVal++;
    }
    dftDefined = false;
    if (enumSel) {
      resultFNode->data.D = iVal;
      if (anno->BasicFlags.Contains(DefaultMenu)) {
        resultFNode->data.StringValue = enumSel->data.Id;
        dftDefined = true;
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
      else
        resultFNode->data.StringValue = enumSel->data.SelectionCode;
      resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
    }
    if (!dftDefined && anno) {
      if (enumSel && (enumSel->data.SelectionCode.c)) {
        dftDefined = true;
        resultFNode->data.D = iVal;
        if (enumSel->data.SelectionCode.c[0] == ' ') {
          resultFNode->data.StringValue.Reset(1);
          resultFNode->data.FIP.leng = 0;
        }
        else {
          resultFNode->data.StringValue = enumSel->data.SelectionCode;
          resultFNode->data.FIP.leng = resultFNode->data.StringValue.l;
        }
        resultFNode->data.IoSigFlags.INCL(trueValue);
      }
    }
    if (!dftDefined) {
      iVal = 0;
      for (enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
           enumSel  && Compare(blank,enumSel->data.SelectionCode);
           enumSel = (CHEEnumSelId*)enumSel->successor) 
        iVal++;
      resultFNode->data.StringValue.Reset(1);
      resultFNode->data.D = iVal;
      resultFNode->data.FIP.leng = 1;
      if (enumSel)
        resultFNode->data.IoSigFlags.INCL(trueValue);
    }
    if (rPtr) {
      if (!enumSel) {
        enumSel = (CHEEnumSelId*)enumSyntax->Items.first;
        resultFNode->data.D = 0;
      }
      *(int*)(enumPtr+LSH) = resultFNode->data.D;
      pstr = (QString*)(enumPtr+LSH+1);
      if (!pstr->isEmpty()) 
        *pstr = QString(enumSel->data.Id.c);
      else
        NewQString(pstr, enumSel->data.Id.c);
    }
    break;
  default:
    ;
  }
  if (LBaseData->inRuntime && rPtr) 
    ((SynFlags*)(totalObj+1))->EXCL(useDefaults);
  return true;
} // END OF setDefaultValue

void LavaFormCLASS::partialForm (LavaDECL* parDECL, LavaDECL* FormDecl, /*pure input*/
                                 LavaVariablePtr resultVarPtr,
                                 SynFlags defaultIOflags,
                                 CHEFormNode *&resultFNode,
                                 bool nowField, bool allowHandler,
                                 TIDs *classChain
                                 )

{
  CHEFormNode *newFNode=0, *variant=0, *variantParent=0;
  LavaDECL *DECLptr, *finalVT;
  CHE *DECLCHptrAny;
  TEnumDescription *eDescPtr;
  DString ignoreString;
  unsigned oldLevel;
  SynFlags ioFlags;
  bool oldEmpty;
  TAnnotation * anno;

  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
    return ;
  anno = FormDecl->Annotation.ptr;
  if (anno) {
  }
  else {
    anno = NewTAnnotation();
    FormDecl->Annotation.ptr = anno;
  }
  level++;
  resultFNode = 0;
  if (nowField) {
    anno = (TAnnotation*)FormDecl->Annotation.ptr->FA.ptr;
    if (!anno) {
      anno = NewTAnnotation();
      FormDecl->Annotation.ptr->FA.ptr = anno;
    }
    if (FormDecl->TypeFlags.Contains(isOptional))
      anno->IterFlags.INCL(Optional);
    else
      anno->IterFlags.EXCL(Optional);
  }
  ioFlags = anno->IoSigFlags*SET(Flag_INPUT,Flag_OUTPUT,DONTPUT,-1);
  if (defaultIOflags.Contains(DONTPUT))
    ioFlags.INCL(DONTPUT); 
  if (ioFlags != SET())
    defaultIOflags = ioFlags;

  if (anno->IterFlags.Contains(Optional))
    if (emptyForm || (level > 1)) {
      AllocFNode(resultFNode, FormDecl, 0);
      resultFNode->data.ResultVarPtr = (CSecTabBase***)resultVarPtr; 
      resultFNode->data.Annotation.ptr = anno;
      resultFNode->data.BasicFlags = anno->BasicFlags-SET(Atomic,-1);
      resultFNode->data.IoSigFlags = anno->IoSigFlags+defaultIOflags;
      if (FormDecl->TypeFlags.Contains(isPlaceholder)) {
        resultFNode->data.IoSigFlags.INCL(DONTPUT);
        defaultIOflags.INCL(DONTPUT);
      }
      resultFNode->data.IterFlags = anno->IterFlags;
      resultFNode->data.allowOwnHandler = allowHandler;

      if (!defaultIOflags.Contains(Flag_INPUT) && LBaseData->inRuntime)
        resultFNode->data.IterFlags.INCL(FixedCount);
      if (!resultFNode->data.ResultVarPtr || !*resultFNode->data.ResultVarPtr) {
        if (!resultFNode->data.IterFlags.Contains(FixedCount)) {
          currentIteration = FormDecl;
          CreateEllipsis(resultFNode,FormDecl, allowHandler);
        }
        level--;
        return;
      }
    }

  finalVT = GetFinalVType(FormDecl);
  if (!nowField 
      || (finalVT->DeclDescType != NamedType)
      || anno->BasicFlags.Contains(DefaultMenu)
      || (FormDecl->DeclType == VirtualType)) {
    if (!AllocFNode(resultFNode, FormDecl, resultVarPtr))
      return;
    resultFNode->data.Annotation.ptr = anno;
    //resultFNode->data.WidgetName = FormDecl->WidgetName;
    resultFNode->data.BasicFlags = anno->BasicFlags;
    resultFNode->data.IoSigFlags = anno->IoSigFlags+defaultIOflags;
    resultFNode->data.allowOwnHandler = allowHandler;
    if (FormDecl->TypeFlags.Contains(isPlaceholder)) {
      resultFNode->data.IoSigFlags.INCL(DONTPUT);
      defaultIOflags.INCL(DONTPUT);
    }
    resultFNode->data.IterFlags = anno->IterFlags;
  }
  if (!nowField) {
    if ((FormDecl->DeclType != Attr) && (FormDecl->DeclType != FormText)
        && (FormDecl->DeclType != VirtualType) 
        /*hinzugefügt 11.09.01:*/ && (FormDecl->DeclType != PatternDef)) {
      level--;
      return;
    }
    anno->BasicFlags.EXCL(Atomic);
    resultFNode->data.Atomic = false;
    DECLCHptrAny = (CHE*)anno->Prefixes.first;
    exprList(DECLCHptrAny, defaultIOflags, resultFNode, true);
    if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex) {
      level--;
      return;
    }
    if (FormDecl->DeclDescType == Undefined) {
        level--;
        return;
    }
    if (((TAnnotation*)FormDecl->Annotation.ptr->FA.ptr)->IterFlags.Contains(Optional)
        && (emptyForm || (level > 1)) 
        && resultFNode->data.ResultVarPtr && *resultFNode->data.ResultVarPtr) {
      oldEmpty = emptyForm;
      oldLevel = level;
      emptyForm = false;
      level = 0;
      resultFNode->data.SubTree.Destroy();
      PartialForm(FormDecl, resultFNode, allowHandler);
      emptyForm = oldEmpty;
      level = oldLevel;
      level--;
      return;
    }

    else {
      partialForm(0, FormDecl, (LavaVariablePtr)resultFNode->data.ResultVarPtr, defaultIOflags, newFNode, true, allowHandler, classChain);
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex) {
        level--;
        return;
      }
      newFNode->data.FIP.up = resultFNode;
      SetIoFlags(resultFNode, newFNode);
      resultFNode->data.SubTree.Append(newFNode);
    }
    DECLCHptrAny = (CHE*)anno->Suffixes.first;
    exprList(DECLCHptrAny, defaultIOflags, resultFNode, true);
  }
  else { //nowField
    if (FormDecl->DeclType == VirtualType) {
      anno->BasicFlags.EXCL(Atomic);
      IterForm(resultFNode, FormDecl, defaultIOflags, allowHandler, classChain);
    }
    else {
      switch (finalVT->DeclDescType) {
        case BasicType:
          anno->BasicFlags.INCL(Atomic);
          resultFNode->data.Atomic = true;
          emptyInsertion = false;
          resultFNode->data.BType = finalVT->BType;
          switch (resultFNode->data.BType) {
            case B_Bool:
              resultFNode->data.B = false;
              break;
            case Enumeration:
              resultFNode->data.D = 0;
              break;
            case Integer:
              resultFNode->data.I = 0;
              break;
          }
          if (defaultIOflags.Contains(Flag_INPUT) || defaultIOflags.Contains(DONTPUT) ) {
            resultFNode->data.IoSigFlags.INCL(UnprotectedUser);
          }
          if (defaultIOflags.Contains(Flag_OUTPUT)) {
            resultFNode->data.IoSigFlags.INCL(UnprotectedProg);
          }
          if (resultFNode->data.BType != Enumeration)
            setDefaultValue(resultFNode);
          break;
        case LiteralString:
          anno->BasicFlags.INCL(Atomic);
          emptyInsertion = false;
          resultFNode->data.Atomic = true;
          resultFNode->data.BType = VLString;
          resultFNode->data.StringValue = FormDecl->LitStr;
          break;


        case NamedType:
          if (anno->BasicFlags.Contains(DefaultMenu)) {
            anno->BasicFlags.INCL(Atomic);
            resultFNode->data.Atomic = true;
            emptyInsertion = false;
            resultFNode->data.BType = Enumeration;
            resultFNode->data.D = 0;
            if (defaultIOflags.Contains(Flag_INPUT) || defaultIOflags.Contains(DONTPUT) ) 
              resultFNode->data.IoSigFlags.INCL(UnprotectedUser);
            if (defaultIOflags.Contains(Flag_OUTPUT)) 
              resultFNode->data.IoSigFlags.INCL(UnprotectedProg);
            upNode = 0;
            setDefaultValue(resultFNode);
          }
          else {
            anno->BasicFlags.EXCL(Atomic);
            if (LBaseData->inRuntime && (FormDecl->DeclType != PatternDef))
              DECLptr = FormDecl->RuntimeDECL;
            else
              DECLptr = ((TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable)->GetDECL(finalVT->RefID, finalVT->inINCL);
            if (DECLptr 
                && ((DECLptr->DeclType == FormDef)
                    || (DECLptr->DeclType == Interface) && (DECLptr->DeclDescType == EnumType))) {
              if (resultFNode) 
                partialForm(DECLptr, DECLptr, (LavaVariablePtr)resultFNode->data.ResultVarPtr, defaultIOflags, resultFNode, true, allowHandler, classChain);
              else
                partialForm(DECLptr, DECLptr, resultVarPtr, defaultIOflags, resultFNode, true, allowHandler, classChain);
              if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !resultFNode || ((CGUIProg*)GUIProg)->ex)
                break;
              if (anno->WidgetName.l)
                resultFNode->data.Annotation.ptr->WidgetName = anno->WidgetName;
              if (FormDecl->TypeFlags.Contains(isPlaceholder)) {
                resultFNode->data.IoSigFlags.INCL(DONTPUT);
                defaultIOflags.INCL(DONTPUT);
              }
              resultFNode->data.BasicFlags += anno->BasicFlags;
              resultFNode->data.IoSigFlags += anno->IoSigFlags+defaultIOflags;
              resultFNode->data.IterFlags += anno->IterFlags;
              //if (!FormDecl->Annotation.ptr) //&& (DECLptr->DeclDescType == DefDesc))
              //  FormDecl->Annotation.ptr = NewTAnnotation();
              //if (DECLptr->DeclDescType == DefDesc)
              if (DECLptr->DeclDescType != BasicType)
                anno->myDECL = FormDecl;
              //if (FormDecl->Annotation.ptr) 
                //if (!FormDecl->Annotation.ptr->IterFlags.Contains(Optional))
              resultFNode->data.Annotation.ptr = anno;
              upNode = 0; // cf. setDefaultValue, Enumeration: StringValue.Reset(0)
              setDefaultValue(resultFNode);
            }
          }
          break;

        case EnumType:
          anno->BasicFlags.EXCL(Atomic);
          resultFNode->data.Atomic = false;
          if (FormDecl->WorkFlags.Contains(selEnum)) {
            anno->BasicFlags.EXCL(Groupbox);
            upNode = resultFNode; // for option menu setDefaultValue
            eDescPtr = (TEnumDescription*)FormDecl->EnumDesc.ptr;
            partialForm(0, &eDescPtr->EnumField, 0, defaultIOflags, newFNode, true, allowHandler, 0);
            if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex)
              break;
            /*
            if (fieldNode) {
              resultFNode->data.FIP.fieldNode = fieldNode;
              fieldNode = 0;
            }
            */
            newFNode->data.FIP.up = resultFNode;
            newFNode->data.ResultVarPtr = resultFNode->data.ResultVarPtr;
            SetIoFlags(resultFNode, newFNode);
            resultFNode->data.SubTree.Append(newFNode);
            partialForm(0, &eDescPtr->MenuTree, 0, defaultIOflags, newFNode, true, allowHandler, 0);
            if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex)
              break;
            newFNode->data.FIP.up = resultFNode;
//            newFNode->data.FIP.pred = (CHEFormNode*)resultFNode->data.SubTree.last;
            SetIoFlags(resultFNode, newFNode);
            resultFNode->data.SubTree.Append(newFNode);
            break;
          }
          else {
            if (!FormDecl->Annotation.ptr->MenuDECL.ptr) {
              LavaDECL *inDecl = NewLavaDECL();
              FormDecl->Annotation.ptr->MenuDECL.ptr = inDecl;

              *inDecl = *FormDecl;
              inDecl->WorkFlags.INCL(runTimeOK);
              inDecl->ParentDECL = parDECL;
              inDecl->NestedDecls.Destroy();
              inDecl->Supports.Destroy();
              inDecl->SectionTabPtr = 0;
              inDecl->WorkFlags.INCL(selEnum);
              FormDecl->WorkFlags.EXCL(selEnum);
              anno->BasicFlags.EXCL(ButtonMenu);
              anno->BasicFlags.EXCL(OptionMenu);
              anno->BasicFlags.EXCL(PopUpMenu);
              resultFNode->data.BasicFlags.EXCL(ButtonMenu);
              resultFNode->data.BasicFlags.EXCL(OptionMenu);
              resultFNode->data.BasicFlags.EXCL(PopUpMenu);
            }
            else {
              ((LavaDECL*)FormDecl->Annotation.ptr->MenuDECL.ptr)->ParentDECL = parDECL;
              ((LavaDECL*)FormDecl->Annotation.ptr->MenuDECL.ptr)->WorkFlags.INCL(selEnum);
              ((LavaDECL*)FormDecl->Annotation.ptr->MenuDECL.ptr)->WorkFlags.INCL(runTimeOK);
            }

          }

        case StructDesc:
          anno->BasicFlags.EXCL(Atomic);
          resultFNode->data.Atomic = false;
          if (parDECL)
            memberList(parDECL, (LavaVariablePtr)resultFNode->data.ResultVarPtr, defaultIOflags,resultFNode, false, allowHandler, classChain);
          else {
            DECLCHptrAny = (CHE*)FormDecl->NestedDecls.first;
            exprList(DECLCHptrAny, defaultIOflags, resultFNode, false);
          }
          break;
        default: ;
      }
    }
  }
  level--;
} // END OF partialForm

bool LavaFormCLASS::IterForm(CHEFormNode* resultFNode, LavaDECL* FormDecl,
                             SynFlags& defaultIOflags, bool allowHandler, TIDs *classChain)
{
  LavaDECL *iterDECL;
  LavaObjectPtr handle, multiObj, elemObj, newStackFrame[SFH+3];
  LavaVariablePtr elDataVarPtr;
  CHEFormNode* chainNode, *newFNode;
  int arrayLen, ii, iterations=0;

  resultFNode->data.Atomic = false;
  if (FormDecl->TypeFlags.Contains(isPlaceholder)) {
    resultFNode->data.IoSigFlags.INCL(DONTPUT);
    defaultIOflags.INCL(DONTPUT);
  }
  if (!defaultIOflags.Contains(Flag_INPUT)) {
    resultFNode->data.IterFlags.INCL(FixedCount);
    resultFNode->data.IterFlags.INCL(NoEllipsis);
  }
  iterDECL = ((TIteration*)FormDecl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
  iterDECL->inINCL = FormDecl->inINCL;
  iterDECL->ParentDECL = FormDecl;
  if (LBaseData->inRuntime) {
    if (!iterDECL->RuntimeDECL) {
      iterDECL->RuntimeDECL = GetFinalIterType(iterDECL);
      iterDECL->RelatedDECL = FormDecl->RelatedDECL;
    }
    for (chainNode = resultFNode;
         chainNode && !chainNode->data.ResultVarPtr;
         chainNode = chainNode->data.FIP.up);
    if (chainNode && !chainNode->data.IoSigFlags.Contains(DONTPUT)) {
      multiObj = *(LavaVariablePtr)chainNode->data.ResultVarPtr;
      if (FormDecl->SecondTFlags.Contains(isSet)) {
        newStackFrame[0] = 0;
        newStackFrame[1] = 0;
        newStackFrame[2] = 0;
        newStackFrame[SFH] = multiObj;
        newStackFrame[SFH+1] = 0;
        if (!HSetFirst(((CGUIProg*)GUIProg)->ckd, newStackFrame))
        return false; 
        handle = newStackFrame[SFH+1];
        arrayLen = 0;
        iterations = 0;
      }
      else {
        arrayLen = HArrayGetLen(multiObj);
        if (!arrayLen) {
          arrayLen = FormDecl->Annotation.ptr->Length.DecPoint;
          HArrayMakeLen(multiObj, arrayLen);
        }
        iterations = arrayLen;
        handle = 0;
      }
      ii = 0;
      while (handle || (ii < arrayLen)) {
        if (!handle || !((SynFlags*)(handle+1))->Contains(deletedItem)) {
          if (FormDecl->SecondTFlags.Contains(isSet)) {
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = multiObj;
            newStackFrame[SFH+2] = (LavaObjectPtr)0;
            newStackFrame[SFH+1] = handle;
            if (!SetGet(((CGUIProg*)GUIProg)->ckd, newStackFrame))
              return false;
            elemObj = newStackFrame[SFH+2];
            if (elemObj)
              DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,elemObj); //nicht nochmal zählen
          }
          else {
            elemObj = HArrayGetEl(multiObj, ii);
            if (!elemObj) {
              elemObj = AllocateObject(((CGUIProg*)GUIProg)->ckd, iterDECL->RelatedDECL->RuntimeDECL, iterDECL->RelatedDECL->TypeFlags.Contains(isStateObjectY));
              if (elemObj) {
  //              ((SynFlags*)(elemObj+1))->INCL(notFromSource);
                ((SynFlags*)(elemObj+1))->INCL(useDefaults);
  //              CallDefaultInit(((CGUIProg*)GUIProg)->ckd, elemObj);
                HArraySetEl(((CGUIProg*)GUIProg)->ckd, multiObj, elemObj, ii);
                DEC_FWD_CNT(((CGUIProg*)GUIProg)->ckd,elemObj); //nicht nochmal zählen
              }
              else {
                if (!((CGUIProg*)GUIProg)->ckd.exceptionThrown)
                  if (!GUIProg->isView) {
                    ((CGUIProg*)GUIProg)->ex = new CRuntimeException(memory_ex, &ERR_AllocObjectFailed);
                    ((LavaGUIDialog*)GUIProg->ViewWin)->OnCancel();
                  }
                return false;
              }
              if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex) {
                if (!GUIProg->isView)
                  ((LavaGUIDialog*)GUIProg->ViewWin)->OnCancel();
               return false;
              }
            }
            ii++;
          }
          elemObj = elemObj - elemObj[0][0].sectionOffset;
          elDataVarPtr = NewLavaVarPtr(elemObj + elemObj[0][GUIProg->myDoc->GetSectionNumber(((CGUIProg*)GUIProg)->ckd, elemObj[0][0].classDECL, iterDECL->RelatedDECL->RuntimeDECL)].sectionOffset);
          partialForm(0, iterDECL, elDataVarPtr,  defaultIOflags, newFNode, false/*true, 11.09.01*/, false, classChain);
          if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex)
            break;
          newFNode->data.FIP.up = resultFNode;
          newFNode->data.IterFlags.INCL(IteratedItem);
          newFNode->data.IterFlags.INCL(Inserted);
          if (FormDecl->SecondTFlags.Contains(isArray))
            newFNode->data.IterFlags.INCL(FixedCount);
          resultFNode->data.SubTree.Append(newFNode);
          setDefaultValue(newFNode);
          if (FormDecl->SecondTFlags.Contains(isSet)) {
            newFNode->data.HandleObjPtr = (CSecTabBase**)handle;
            newStackFrame[0] = 0;
            newStackFrame[1] = 0;
            newStackFrame[2] = 0;
            newStackFrame[SFH] = multiObj;
            newStackFrame[SFH+1] = handle;
            newStackFrame[SFH+2] = 0;
            ((CGUIProg*)GUIProg)->ex = HSetSucc(((CGUIProg*)GUIProg)->ckd, newStackFrame);
            if (((CGUIProg*)GUIProg)->ex)
              return false;
            handle = newStackFrame[SFH+2];
            iterations++;
          }
        }
        else { //deletedItem: ignore
          newStackFrame[0] = 0;
          newStackFrame[1] = 0;
          newStackFrame[2] = 0;
          newStackFrame[SFH] = multiObj;
          newStackFrame[SFH+1] = handle;
          newStackFrame[SFH+2] = 0;
          ((CGUIProg*)GUIProg)->ex = HSetSucc(((CGUIProg*)GUIProg)->ckd, newStackFrame);
          if (((CGUIProg*)GUIProg)->ex)
            return false;
          handle = newStackFrame[SFH+2];
        }
      }//while
    }//if (chainNode &...
    else { //error
      if (!chainNode->data.IoSigFlags.Contains(DONTPUT)) {
        GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, iterDECL, &ERR_ErrInForm,0);
        return false;
      }
    } 
  }//inRuntime
  else {
    if (FormDecl->SecondTFlags.Contains(isArray)
      &&  FormDecl->Annotation.ptr->Length.DecPoint) {
      arrayLen = FormDecl->Annotation.ptr->Length.DecPoint;
      ii = 0;
      while (ii < arrayLen) {
        partialForm(0, iterDECL, 0,  defaultIOflags, newFNode, false/*true, 11.09.01*/, false, classChain);
        if (!newFNode)
          break;
        newFNode->data.FIP.up = resultFNode;
        newFNode->data.IterFlags.INCL(IteratedItem);
        newFNode->data.IterFlags.INCL(Inserted);
        if (FormDecl->SecondTFlags.Contains(isArray))
          newFNode->data.IterFlags.INCL(FixedCount);
        resultFNode->data.SubTree.Append(newFNode);
        setDefaultValue(newFNode);
        iterations++;
        ii++;
      }//while
    }//if (chainNode &...
  }
  if (!iterations 
       && (FormDecl->SecondTFlags.Contains(isSet) || !iterDECL->RuntimeDECL) 
       && !resultFNode->data.IoSigFlags.Contains(DONTPUT)
       && resultFNode->data.IoSigFlags.Contains(Flag_INPUT)
       && ( (((TIteration*)FormDecl->Annotation.ptr->IterOrig.ptr)->BoundType == EQ)
       || (((TIteration*)FormDecl->Annotation.ptr->IterOrig.ptr)->BoundType == GE))) {
    iterations = ((TIteration*)FormDecl->Annotation.ptr->IterOrig.ptr)->Bound;
    for (ii = 0; ii < iterations; ii++) {
      partialForm(0, iterDECL, 0, defaultIOflags, newFNode, true, false, classChain);
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || !newFNode || ((CGUIProg*)GUIProg)->ex)
        break;
      newFNode->data.IterFlags.INCL(IteratedItem);
      newFNode->data.IterFlags.INCL(Inserted);
      if (FormDecl->SecondTFlags.Contains(isArray))
        newFNode->data.IterFlags.INCL(FixedCount);
      newFNode->data.FIP.up = resultFNode;
      resultFNode->data.SubTree.Append(newFNode);
    }
  }

  if ((((TIteration*)FormDecl->Annotation.ptr->IterOrig.ptr)->BoundType != EQ)
      && !resultFNode->data.IterFlags.Contains(NoEllipsis)) {
    currentIteration = FormDecl;
    newFNode = 0;
    CreateEllipsis(newFNode, iterDECL, allowHandler);
    newFNode->data.IoSigFlags += defaultIOflags;
    newFNode->data.FIP.up = resultFNode;
    SetIoFlags(resultFNode, newFNode);
    resultFNode->data.SubTree.Append(newFNode);
  }
  return true;
}


void LavaFormCLASS::memberList (LavaDECL* parDECL,
                               LavaVariablePtr resultObjPtr,
                               SynFlags defaultIOflags,
                               CHEFormNode *resultFNode,
                               bool nowField,
                               bool allowHandler,
                               TIDs *classChain)

{
  CHEFormNode *newFNode;
  LavaDECL *formDECL, *classDECL, *memDECL, *decl;
  LavaVariablePtr resultMemVar=0;
  CHETVElem *classEl, *formEl;
  CHE* cheMem;
  TID classmemID, classID;
  CHETID *cheID;
  TIDTable* IDTable = (TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable;;
  QString msg;
  int sectionNumber;
  bool isForm;

  if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
    return ;
  classDECL = IDTable->GetDECL(parDECL->RefID, parDECL->inINCL); 
  if (!classDECL) {
    if (LBaseData->inRuntime) {
      GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, parDECL, &ERR_Broken_ref, 0);
      return;
    }
    else {
      msg = ERR_Broken_ref;
      msg = msg + QString(" in : ")  + QString(parDECL->FullName.c);
      QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), msg);
    }
    return;
  }
  if (classChain) {
    classID = TID(classDECL->OwnID, classDECL->inINCL);
    if (parDECL->TypeFlags.Contains(isOptional)) {
      cheID = new CHETID;
      cheID->data = classID;
      classChain->Append(cheID);
      cheID = new CHETID;
      cheID->data.nID = -1;
      classChain->Append(cheID);
    }
    else {
      for (cheID = (CHETID*)classChain->first; 
           cheID && (cheID->data != classID) && (cheID->data.nID != -1);
           cheID = (CHETID*)cheID->successor);
      if (cheID && (cheID->data.nID != -1)) {
        msg = ERR_ClassCircle;
        while (cheID) {
          decl = IDTable->GetDECL(cheID->data);
          msg += decl->FullName.c;
          cheID = (CHETID*)cheID->successor;
          msg += " -> ";
        }
        msg = msg + QString(classDECL->FullName.c);
        QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), msg);
        return;
      }
      else {
        cheID = new CHETID;
        cheID->data = classID;
        classChain->Append(cheID);
      }
    }
  }
  for (; classDECL && classDECL->DeclType == VirtualType;
         classDECL = IDTable->GetDECL(classDECL->RefID, classDECL->inINCL));
  if (!parDECL->VElems.VElems.first || !LBaseData->inRuntime)
    GUIProg->myDoc->MakeFormVT(parDECL, &((CGUIProg*)GUIProg)->ckd);
  if (!classDECL) 
    if (LBaseData->inRuntime) {
      GUIProg->myDoc->LavaError(((CGUIProg*)GUIProg)->ckd, true, parDECL, &ERR_Broken_ref, 0);
      return;
    }
    else {
      msg = ERR_NoIFforForm;
      msg = msg + QString(" : ")  + QString(parDECL->ParentDECL->FullName.c);
      QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), msg);
      return;
    }
  if (!classDECL->VElems.VElems.first || !LBaseData->inRuntime)
    if (!GUIProg->myDoc->MakeVElems(classDECL, &((CGUIProg*)GUIProg)->ckd))
      return;
  formEl = (CHETVElem*)parDECL->VElems.VElems.first;
  formDECL = IDTable->GetDECL(formEl->data.VTClss); //remember: TIDs in VElems are already adjusted
  if (formDECL != parDECL) { //form has inherited form from base class
    cheMem = (CHE*)parDECL->NestedDecls.first;
    if (cheMem && ( ((LavaDECL*)cheMem->data)->DeclDescType == LiteralString) 
    && ((LavaDECL*)cheMem->data)->Annotation.ptr->BasicFlags.Contains(beforeBaseType)) {
      partialForm(0, (LavaDECL*)cheMem->data, 0, defaultIOflags, newFNode, nowField, allowHandler, classChain);
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex) {
        ReduceClassChain(classChain);
        return;
      }
      if (newFNode) {
        newFNode->data.FIP.up = resultFNode;
        SetIoFlags(resultFNode, newFNode);
        resultFNode->data.SubTree.Append(newFNode);
      }
    }
  }
  while (formEl) {
    formDECL = IDTable->GetDECL(formEl->data.VTClss); //remember: TIDs in VElems are already adjusted
    if (formDECL && (formDECL->DeclDescType == EnumType)) {
      partialForm(0, (LavaDECL*)formDECL->Annotation.ptr->MenuDECL.ptr, (LavaVariablePtr)resultFNode->data.ResultVarPtr,  defaultIOflags, newFNode, true, allowHandler, 0);
      if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex) {
        ReduceClassChain(classChain);
        return;
      }
      if (newFNode) {
        newFNode->data.FIP.up = resultFNode;
//        newFNode->data.FIP.pred = (CHEFormNode*)resultFNode->data.SubTree.last;
        SetIoFlags(resultFNode, newFNode);
        resultFNode->data.SubTree.Append(newFNode);
      }
    }
    cheMem = (CHE*)formDECL->NestedDecls.first;
    if (cheMem && ((LavaDECL*)cheMem->data)->Annotation.ptr
      && (formDECL == parDECL) && ( ((LavaDECL*)cheMem->data)->DeclDescType == LiteralString) 
      && ((LavaDECL*)cheMem->data)->Annotation.ptr->BasicFlags.Contains(beforeBaseType))
      cheMem = (CHE*)cheMem->successor;
    while (cheMem) {
      memDECL = (LavaDECL*)cheMem->data;
      if ( memDECL->DeclDescType != LiteralString) {
        if ((memDECL->DeclDescType == BasicType)
          && ((memDECL->BType == B_Bool) || (memDECL->BType == Char) || (memDECL->BType == Integer) || (memDECL->BType == Float) || (memDECL->BType == Double) || (memDECL->BType == VLString)))
          isForm = true;
        else {
          formDECL = FinalFormDECL(memDECL);
          if (formDECL) {
            isForm = formDECL && ((formDECL->DeclType == FormDef)
                     || (formDECL->DeclDescType == BasicType)
                     || (formDECL->DeclDescType == EnumType));
            if (formDECL->DeclDescType == EnumType)
              if (formDECL->DeclType == FormDef) {
                memDECL->Annotation.ptr->BasicFlags.EXCL(DefaultMenu);
                ((TAnnotation*)memDECL->Annotation.ptr->FA.ptr)->BasicFlags.EXCL(DefaultMenu);
              }
              else {
                memDECL->Annotation.ptr->BasicFlags.INCL(DefaultMenu);
                ((TAnnotation*)memDECL->Annotation.ptr->FA.ptr)->BasicFlags.INCL(DefaultMenu);
              }
          }
        }
        classEl = 0;
        if (isForm) {
          //mem in class?
          classmemID = TID(((CHETID*)memDECL->Supports.first)->data.nID, 
                           IDTable->IDTab[memDECL->inINCL]->nINCLTrans[((CHETID*)memDECL->Supports.first)->data.nINCL].nINCL);
          for (classEl = (CHETVElem*)classDECL->VElems.VElems.first;
               classEl && (classEl->data.VTEl != classmemID);
               classEl = (CHETVElem*)classEl->successor);
        }
      }
      if ((memDECL->DeclDescType == LiteralString) || classEl) {
        if ( (memDECL->DeclDescType != LiteralString)  && LBaseData->inRuntime && memDECL->RelatedDECL) {
          if (memDECL->DeclType == Attr) {
            sectionNumber = GUIProg->myDoc->GetMemsSectionNumber(((CGUIProg*)GUIProg)->ckd, parDECL->RelatedDECL, memDECL->RelatedDECL);
            resultMemVar = GetMemberVarPtr(resultObjPtr, memDECL->RelatedDECL, sectionNumber);
          }
          else //virtual type of element type
            if (memDECL->SecondTFlags.Contains(isSet))
              resultMemVar = NewLavaVarPtr(CastSetType(((CGUIProg*)GUIProg)->ckd, *resultObjPtr));
            else
              if (memDECL->SecondTFlags.Contains(isArray))
                resultMemVar = NewLavaVarPtr(CastArrayType(((CGUIProg*)GUIProg)->ckd, *resultObjPtr));
        }
        partialForm(0, memDECL, resultMemVar, defaultIOflags, newFNode, nowField, allowHandler, classChain);
        if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex) {
          ReduceClassChain(classChain);
          return;
        }
        if (newFNode) {
          newFNode->data.FIP.up = resultFNode;
//          newFNode->data.FIP.pred = (CHEFormNode*)resultFNode->data.SubTree.last;
          SetIoFlags(resultFNode, newFNode);
          resultFNode->data.SubTree.Append(newFNode);
        }
      }
      cheMem = (CHE*)cheMem->successor;
    }
    formEl = (CHETVElem*)formEl->successor;
  }
  ReduceClassChain(classChain);
}

void LavaFormCLASS::ReduceClassChain(TIDs* classChain)
{
  if (!classChain)
    return;
  CHETID * che = (CHETID*)classChain->last;
  if (che) {
    if (che->data.nID == -1) 
      classChain->Cut(che->predecessor->predecessor);
    else
      classChain->Cut(che->predecessor);
  }
}

LavaDECL* LavaFormCLASS::GetFinalIterType(LavaDECL* decl)
{
  LavaDECL *iterType;
  TIDTable* IDTable = (TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable;
  for (iterType = decl;
       iterType && (iterType->DeclType != Interface) && (iterType->DeclType != FormDef);
       iterType = IDTable->GetDECL(iterType->RefID, iterType->inINCL));
  return iterType;
}

LavaDECL* LavaFormCLASS::FinalFormDECL(LavaDECL* decl)
{
  LavaDECL *formDECL;
  TIDTable* IDTable = (TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable;
  if (LBaseData->inRuntime) {
    formDECL = decl->RuntimeDECL;
    for ( ; formDECL && (formDECL->DeclType == VirtualType) && (formDECL->DeclDescType != BasicType);
          formDECL = formDECL->RuntimeDECL);
  }
  else {
    formDECL = IDTable->GetDECL(decl->RefID, decl->inINCL);
    for ( ; formDECL && (formDECL->DeclType == VirtualType) && (formDECL->DeclDescType != BasicType);
         formDECL = IDTable->GetDECL(formDECL->RefID, formDECL->inINCL));
  }
  return formDECL;
}


LavaDECL* LavaFormCLASS::GetFinalVType(LavaDECL* decl)
{
  if ((decl->DeclType != Attr) && (decl->DeclType != VirtualType)
      || (decl->DeclDescType != NamedType))
    return decl;
  LavaDECL *formDECL, *hdecl;
  TIDTable* IDTable = (TIDTable*)((CGUIProg*)GUIProg)->mySynDef->IDTable;
  if (LBaseData->inRuntime) 
    if (decl->DeclType == VirtualType)
      return decl->RuntimeDECL;
    else
      return decl;
  else {
    formDECL = decl;
    hdecl = IDTable->GetDECL(decl->RefID, decl->inINCL);
    while (hdecl && (hdecl->DeclType == VirtualType)) {
      formDECL = hdecl;
      hdecl = IDTable->GetDECL(hdecl->RefID, hdecl->inINCL);
    }
  }
  return formDECL;
}



void LavaFormCLASS::exprList (CHE *DECLCHptrAny,
                               SynFlags defaultIOflags,
                               CHEFormNode *resultFNode,
                               bool nowField)

{
  CHEFormNode *newFNode=0; 
  for (;DECLCHptrAny; DECLCHptrAny = (CHE*)DECLCHptrAny->successor) {
    partialForm(0, (LavaDECL*)DECLCHptrAny->data, 0, defaultIOflags, newFNode, nowField, false, 0);
    if (((CGUIProg*)GUIProg)->ckd.exceptionThrown || ((CGUIProg*)GUIProg)->ex)
      return;
    if (newFNode) {
      newFNode->data.FIP.up = resultFNode;
//      newFNode->data.FIP.pred = (CHEFormNode*)resultFNode->data.SubTree.last;
      SetIoFlags(resultFNode, newFNode);
      resultFNode->data.SubTree.Append(newFNode);
    }
  }
}


LavaVariablePtr LavaFormCLASS::GetMemberVarPtr(LavaVariablePtr objPtr, LavaDECL* fieldDECL, int sectionNumber)
{

  if (objPtr && *objPtr && **objPtr) 
      return (LavaVariablePtr)(*objPtr
              - ((CSectionDesc*)(**objPtr))->sectionOffset
              + ((CSectionDesc*)(**objPtr))[sectionNumber + fieldDECL->SectionInfo2].sectionOffset
              + fieldDECL->SectionInfo1 + LSH);
  else
    return objPtr;
}

CHEFormNode* LavaFormCLASS::BrowseForm(CHEFormNode *formNode, LavaDECL* formDECL)
{
  CHEFormNode *actFNode = formNode, *returnNode;
  while (actFNode 
    && ((actFNode->data.FormSyntax != formDECL) 
        || !actFNode->data.FIP.frameWidget)) {

    returnNode = BrowseForm((CHEFormNode*)actFNode->data.SubTree.first, formDECL);
    if (returnNode)
      return returnNode;
    actFNode = (CHEFormNode*)actFNode->successor;
  }
  return actFNode;
}

void LavaFormCLASS::DeleteForm (CHEFormNode *formNode)
{
  if (!formNode->data.FIP.up) {
    /* first deallocate the "artificial" mySynDef definition nodes
       corresponding to the 2 top level formNode nodes for prefix
       and suffix text; the formNode mySynDef of the form node must not be
       deleted since it is a "real" mySynDef definition node */

    delete formNode->data.FormSyntax;
    if (formNode->successor && formNode->successor->successor)
      delete ((CHEFormNode*)formNode->successor->successor)->data.FormSyntax;
  }
  //DeleteWindows(formNode);
  if (formNode->successor && formNode->successor->successor) {
    delete (CHEFormNode*)formNode->successor->successor;
    formNode->successor->successor = 0;
  }
  if (formNode->successor) {
    delete (CHEFormNode*)formNode->successor;
    formNode->successor = 0;
  }
  delete formNode;
} // END OF DeleteForm


void LavaFormCLASS::DeletePopups(CHEFormNode *actFNode)
{
  CHEFormNode *subNode, *nextNode;
  for (nextNode = actFNode; nextNode; nextNode = (CHEFormNode*)nextNode->successor) {
    if (nextNode->data.FIP.popupShell) {
      if (!LBaseData->inRuntime && !GUIProg->myDoc->deleting) {
        QPoint pp = nextNode->data.FIP.popupShell->parentWidget()->pos(); 
        nextNode->data.FormSyntax->SectionInfo1 = pp.y();
        nextNode->data.FormSyntax->SectionInfo2 = pp.x();
      }
      delete nextNode->data.FIP.popupShell;
      nextNode->data.FIP.popupShell = 0;
    }
    else
      for (subNode = (CHEFormNode*)nextNode->data.SubTree.first;
          subNode;
          subNode = (CHEFormNode*)subNode->successor)
        DeletePopups(subNode);
  }
}


void LavaFormCLASS::DeleteWindows(CHEFormNode *object_first, bool redraw, bool finalRelease)
{
  bool notMenu = true;
  QWidget* lastFrameDel =0;

  CHEFormNode *actFNode = object_first;
  if (!finalRelease && !redraw) 
    DeletePopups(actFNode);
  while (actFNode) {
    if (actFNode->data.FIP.frameWidget 
      && actFNode->data.IoSigFlags.Contains(firstUseOfFrame)) {
      lastFrameDel = (QWidget*)actFNode->data.FIP.frameWidget;
      //lastFrameDel->hide();
      if (lastFrameDel->parentWidget()->inherits("GUIVBox")) 
        delete actFNode->data.FIP.frameWidget;
      actFNode->data.FIP.frameWidget = 0;
    }
    actFNode = (CHEFormNode*)actFNode->successor;
  }
}

bool LavaFormCLASS::OnOK(CHEFormNode *object_first)
{
  if (!object_first)
    return true;
  CHEFormNode *enumFNode, *actFNode, *actFNode0;
  actFNode0 = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (object_first, ((CGUIProg*)GUIProg)->Root);
  actFNode = actFNode0;
  while (actFNode) {
    if (actFNode->data.IoSigFlags.Contains(UnprotectedUser)) {
      if (actFNode->data.Atomic || actFNode->data.BasicFlags.Contains(Atomic)) {
        if (!actFNode->data.IterFlags.Contains(Ellipsis)
          && !actFNode->data.IoSigFlags.Contains(trueValue)
          && !((CGUIProg*)GUIProg)->FrozenObject) {
          //show MessageBox and
          //SetFocus
          QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), ERR_EnterValue);
          if (((CGUIProg*)GUIProg)->MakeGUI.setFocus(1,actFNode)) 
            if (GUIProg->focNode) 
              ((CGUIProg*)GUIProg)->MakeGUI.SetPointer((QWidget*)GUIProg->focNode->data.FIP.widget, 1);
         return false;
        }
      }
      else {
        if (actFNode->data.FormSyntax->DeclDescType == EnumType) {
          if (actFNode->data.FormSyntax->Annotation.ptr->BasicFlags.Contains(MenuFirst))
            enumFNode = (CHEFormNode*)actFNode->data.SubTree.first->successor;
          else
            enumFNode = (CHEFormNode*)((CHEFormNode*)actFNode->data.SubTree.first)->data.SubTree.first;
          if (!enumFNode->data.IoSigFlags.Contains(trueValue)) {
            //show MessageBox and
            //SetFocus
            QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), ERR_SelectValue);
            if (((CGUIProg*)GUIProg)->MakeGUI.setFocus(1,actFNode)) 
              if (actFNode) 
                ((CGUIProg*)GUIProg)->MakeGUI.SetPointer((QWidget*)actFNode->data.FIP.widget, 1);
            return false;
          }
        }
        else
          if (!OnOK((CHEFormNode*)actFNode->data.SubTree.first))
            return false;
      }
    }
    actFNode = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected (actFNode, ((CGUIProg*)GUIProg)->Root);
    if (actFNode == actFNode0)
      return true;
  }
  return true;
}

bool LavaFormCLASS::IsIteration (CHEFormNode*& trp)
{
  if (trp && trp->data.FormSyntax)
    return trp->data.FormSyntax->SecondTFlags.Contains(isSet);
  return false;
} // END OF IsIteration

bool LavaFormCLASS::IsAdmissibleIteration (CHEFormNode* trp, bool& equal, unsigned count)
{
  CHEFormNode* trpn;
  unsigned bound;
  LavaDECL *iterDecl;

  equal = false;
  if (!trp || trp->data.IterFlags.Contains(FixedCount))
    return false;
  if (trp->data.IterFlags.Contains(Optional)) {
    equal = true;
    return true;
  }
  else {
    /*
    if (trp->data.Atomic)
      trpn = trp;
    else
      trpn = (CHEFormNode*)trp->data.SubTree.first;
    */
    bound = 0;
    trpn = trp;
    while (trpn && trpn->data.IterFlags.Contains(IteratedItem)) {
      if (!trpn->data.IterFlags.Contains(Ignore))
        bound++;
      trpn = (CHEFormNode*)trpn->successor;
    }
    trpn = (CHEFormNode*)trp->predecessor;
    while (trpn && trpn->data.IterFlags.Contains(IteratedItem)) {
      if (!trpn->data.IterFlags.Contains(Ignore))
        bound++;
      trpn = (CHEFormNode*)trpn->predecessor;
    }
  }
  if (trp->data.FormSyntax->SecondTFlags.Contains(isSet)) {
    if (!count)
      if (bound > 0)
        bound = bound-1;
      else {
        equal = false;
        return false;
      }
    else
      bound = bound+count;
    if (trp->data.FormSyntax->DeclType == VirtualType)
      iterDecl = trp->data.FormSyntax;
    else
      iterDecl = trp->data.FIP.up->data.FormSyntax;
    switch (((TIteration*)iterDecl->Annotation.ptr->IterOrig.ptr)->BoundType ) {
    case GE:
      if (bound >= ((TIteration*)iterDecl->Annotation.ptr->IterOrig.ptr)->Bound) {
        equal = true;
        return true;
      }
      break;
    case EQ:
      if (bound >=  ((TIteration*)iterDecl->Annotation.ptr->IterOrig.ptr)->Bound)  {
        equal = true;
        return true;
      }
      break;
    case LE:
      if (bound <= ((TIteration*)iterDecl->Annotation.ptr->IterOrig.ptr)->Bound)  {
        equal = true;
        return true;
      }
      break;
    case LG:
      break;
    }
    equal = false;
    return false;
  }
  equal = true;
  return true;
} // END OF IsAdmissibleIteration

LavaVariablePtr LavaFormCLASS::NewLavaVarPtr(LavaObjectPtr val)
{
  if (PtrCount == 1024) {
    actPtr[1024] = (LavaObjectPtr)new LavaObjectPtr[1025];
    actPtr = (LavaVariablePtr)actPtr[1024];
    actPtr[1024] = 0;
    PtrCount = 0;
  }
  else 
    PtrCount++;
  actPtr[PtrCount] = val;
  return &actPtr[PtrCount];
}

/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

void LavaFormCLASS::INIT (CGUIProgBase *guiPr)
{
  GUIProg = guiPr;
  Silent = false;
  emptyForm = false;
  level = 0;

  ellipsis = "...";
  blank = " ";
  blank2 = "  ";
  blank4 = "    ";
  questionMarks = "???";
  PtrCount = 0;
  actPtr = newPtr;
  newPtr[1024] = 0;

}

