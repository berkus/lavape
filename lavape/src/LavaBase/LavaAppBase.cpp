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


#include "LavaBase_all.h"
#include "BASEMACROS.h"

#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "SylTraversal.h"
#include "LavaThread.h"
#include "SynIO.h"
//#include "mdiframes.h"

#include "qobject.h"
#include "qstring.h"
#include "qmessagebox.h"
#include <QProcess>
#include <QByteArray>
//Added by qt3to4:
#include <errno.h>

#include "qfiledialog.h"
#ifndef __GNUC__
#include <windows.h>
#include <shlobj.h>
#endif

#pragma hdrstop


//QAssistantClient *qacl=0;


CLavaPEHint::CLavaPEHint(CPECommand cc, wxDocument* fromdoc, SynFlags first,
                       void * commandData1, void * commandData2,
                       void * commandData3, void * commandData4,
                       void * commandData5, void * commandData6, void * commandData7, void * commandData8)
{
  setObjectName("LavaPEHint");
  com = cc;
  fromDoc = fromdoc;
  FirstLast = first;
  CommandData1 = commandData1;
  CommandData2 = commandData2;
  CommandData3 = commandData3;
  CommandData4 = commandData4;
  CommandData5 = commandData5;
  CommandData6 = commandData6;
  CommandData7 = commandData7;
  CommandData8 = commandData8;
}

CLavaPEHint::~CLavaPEHint()
{
  if (FirstLast.Contains(firstHint)
      && FirstLast.Contains(multiDocHint))
    LBaseData->MultiUndoMem.RemoveHint(this);
  if ((com == CPECommand_Change)
       || (com == CPECommand_Delete)
       || (com == CPECommand_Insert)
       || (com == CPECommand_Exclude)
       || (com == CPECommand_ChangeInclude)
       || (com == CPECommand_Include)) {
    if(CommandData2)
      delete (DString*)CommandData2;
  }
  else
    if (com == CPECommand_Exec)
      LBaseData->ExecUpdate->DeleteHint(this);
  if(CommandData7 && (com != CPECommand_Change))
    delete (DString*)CommandData7;

}

void CLavaPEHint::Destroy(bool inRedo) //call this function only from CleanUndoMem
{
  if (CommandData1)
    switch (com) {
      case CPECommand_Change:
        delete (LavaDECL*)CommandData1;
        break;
      case CPECommand_Insert:
        if (inRedo)
          delete (CHE*)CommandData9;
        break;
      case CPECommand_Delete:
        if (!inRedo)
          delete (CHE*)CommandData9;
        break;
      case CPECommand_Include:
        if (inRedo)
          delete (CHESimpleSyntax*)CommandData1;
        break;
      case CPECommand_ChangeInclude:
        if (inRedo)
          delete (CHESimpleSyntax*)CommandData1;
        break;
      case CPECommand_Exclude:
        if (!inRedo)
          delete (CHESimpleSyntax*)CommandData1;
        break;
      default: ;
    }
}

CUndoMem::CUndoMem()
{
  undoMemPosition = 0;
  redoMemPosition = 0;
  saveMemPosition = 0;
  DrawTree = false;
  wasFirst = false;
  max_undo = 128;
  UndoMemory = new CLavaPEHint* [max_undo];
  RedoMemory = new CLavaPEHint* [max_undo];
  int ii;
  for ( ii= 0; ii < max_undo; ii++) {
    UndoMemory[ii] = 0;
    RedoMemory[ii] = 0;
  }
}

void CUndoMem::SetLastHint()
{
  if (undoMemPosition > 0) {
    UndoMemory[undoMemPosition-1]->FirstLast.INCL(lastHint);
    if (UndoMemory[undoMemPosition-1]->FirstLast.Contains(multiDocHint))
      LBaseData->MultiUndoMem.SetLastHint(UndoMemory[undoMemPosition-1]);
  }
}

CLavaPEHint* CUndoMem::GetLastHint()
{
  if (undoMemPosition > 0)
    return UndoMemory[undoMemPosition-1];
  else
    return 0;
}

CLavaPEHint* CUndoMem::GetLastRedo()
{
  if (redoMemPosition > 0)
    return RedoMemory[redoMemPosition-1];
  else
    return 0;
}

void CUndoMem::AddToMem(CLavaPEHint* hint)
{
  while (redoMemPosition > 0) {
    redoMemPosition--;
    //    RedoMemory[redoMemPosition]->Destroy();
    delete RedoMemory[redoMemPosition];
  }
  redoMemPosition = 0;
  if (!hint->FirstLast.Contains(firstHint))
    if (!undoMemPosition || UndoMemory[undoMemPosition-1]->FirstLast.Contains(lastHint)) {
      hint->FirstLast.INCL(firstHint);
      DrawTree = false;
    }
  UndoMemory[undoMemPosition] = hint;
  undoMemPosition++;
  if (undoMemPosition >= max_undo) {
    CLavaPEHint** newMem = new CLavaPEHint* [max_undo+128];
    CLavaPEHint** newredoMem = new CLavaPEHint* [max_undo+128];
    int ii;
    for (ii = 0; ii < max_undo; ii++) {
      newMem[ii] = UndoMemory[ii];
      newredoMem[ii] = RedoMemory[ii];
    }
    max_undo += 128;
    UndoMemory = newMem;
    RedoMemory = newredoMem;
  }
  if (hint->FirstLast.Contains(multiDocHint)) {
    DrawTree = true;
    if (hint->FirstLast.Contains(firstHint))
      LBaseData->MultiUndoMem.AddToMem(hint);
    if (hint->FirstLast.Contains(lastHint))
      LBaseData->MultiUndoMem.SetLastHint(hint);
  }
  else {
    if (hint->FirstLast.Contains(firstHint))
      DrawTree = false;
    DrawTree = DrawTree || (hint->com == CPECommand_Insert)
               || (hint->com == CPECommand_Delete)
               || (hint->com == CPECommand_Move)
               || (hint->com == CPECommand_Change && hint->CommandData7);

  }
  ((CPEBaseDoc*)hint->fromDoc)->modified = false;
  OnUpdateEditUndo(LBaseData->editUndoActionPtr);
  OnUpdateEditRedo(LBaseData->editRedoActionPtr);
}


CLavaPEHint* CUndoMem::DoFromMem(int& pos)
{
  if (undoMemPosition <= 0)
    return NULL;
  if (pos < 0) {
    pos = undoMemPosition-1 ;
    while ((pos >= 0) && !UndoMemory[pos]->FirstLast.Contains(firstHint))
      pos = pos-1;
    Q_ASSERT (pos >=  0);
  }
  else
    if (pos == (undoMemPosition - 1))
      return NULL;  //finished
    else
      pos++;
  if (UndoMemory[pos]) {
    OnUpdateEditUndo(LBaseData->editUndoActionPtr);
    OnUpdateEditRedo(LBaseData->editRedoActionPtr);
    return UndoMemory[pos];
  }
  else
    return NULL;
}

CLavaPEHint* CUndoMem::UndoFromMem(int& pos)
{
  if (undoMemPosition > 0)  {
    if (pos < 0)  //the undo button has been clicked
      pos = undoMemPosition -1;
    else
      if (wasFirst)
        return NULL;
      else
        pos = pos -1;
    if (UndoMemory[pos]) {
      undoMemPosition = pos;
      wasFirst = UndoMemory[pos]->FirstLast.Contains(firstHint);
      RedoMemory[redoMemPosition] = UndoMemory[pos];
      redoMemPosition++;
      ((CPEBaseDoc*)UndoMemory[pos]->fromDoc)->modified = false;
      if (UndoMemory[pos]->FirstLast.Contains(lastHint))
        DrawTree = false;
      DrawTree = DrawTree || (UndoMemory[pos]->com == CPECommand_Insert)
                 || (UndoMemory[pos]->com == CPECommand_Delete)
                 || (UndoMemory[pos]->com == CPECommand_Move)
                 || ((UndoMemory[pos]->com == CPECommand_Change) && UndoMemory[pos]->CommandData7);
      OnUpdateEditUndo(LBaseData->editUndoActionPtr);
      OnUpdateEditRedo(LBaseData->editRedoActionPtr);
      return UndoMemory[pos];
    }
    else
      return NULL;
  }
  else
    return NULL;
}


CLavaPEHint* CUndoMem::RedoFromMem(int& pos)
{
  if (redoMemPosition <= 0)
    return NULL;
  if (pos < 0)
    pos = redoMemPosition-1 ;
  else {
    if ((pos == 0) || RedoMemory[pos-1]->FirstLast.Contains(firstHint))
      return NULL;  //finished
    pos = pos--;
    Q_ASSERT (pos >=  0);
  }
  if (RedoMemory[pos]) {
    UndoMemory[undoMemPosition] = RedoMemory[pos];
    redoMemPosition--;
    undoMemPosition++;
    ((CPEBaseDoc*)RedoMemory[pos]->fromDoc)->modified = false;
    if (RedoMemory[pos]->FirstLast.Contains(firstHint))
      DrawTree = false;
    DrawTree = DrawTree || (RedoMemory[pos]->com == CPECommand_Insert)
               || (RedoMemory[pos]->com == CPECommand_Delete)
               || (RedoMemory[pos]->com == CPECommand_Move)
               || (RedoMemory[pos]->com == CPECommand_Change && RedoMemory[pos]->CommandData7);
    OnUpdateEditUndo(LBaseData->editUndoActionPtr);
    OnUpdateEditRedo(LBaseData->editRedoActionPtr);
    return RedoMemory[pos];
  }
  else
    return NULL;
}

void  CUndoMem::CleanUndoMem()
{
  while (undoMemPosition > 0) {
    undoMemPosition--;
    UndoMemory[undoMemPosition]->Destroy(false);
    delete UndoMemory[undoMemPosition];
  }
  undoMemPosition = 0;
  while (redoMemPosition > 0) {
    redoMemPosition--;
    RedoMemory[redoMemPosition]->Destroy(true);
    delete RedoMemory[redoMemPosition];
  }
  redoMemPosition = 0;
  if (!wxTheApp->deletingMainFrame) {
    OnUpdateEditUndo(LBaseData->editUndoActionPtr);
    OnUpdateEditRedo(LBaseData->editRedoActionPtr);
  }
}

void CUndoMem::SetCom8()
{
  int posDel=-1, posIns=-1,
      pos = undoMemPosition-1;
  while ((pos >= 0) //&& !UndoMemory[pos]->FirstLast.Contains(firstHint)
     && ((posDel == -1) || (posIns == -1))) {
    if ((((LavaDECL*)UndoMemory[pos]->CommandData1)->DeclType == Function)
        && (((LavaDECL*)UndoMemory[pos]->CommandData1)->ParentDECL->DeclType == Impl)) {
      if (UndoMemory[pos]->com == CPECommand_Delete)
        posDel = pos;
      else if ((UndoMemory[pos]->com == CPECommand_Insert)
               || (UndoMemory[pos]->com == CPECommand_Move))
        posIns = pos;
    }
    if (UndoMemory[pos]->FirstLast.Contains(firstHint))
      pos = -1;
    else
      pos--;
  }
  while ((posDel >= 0) && (posIns >= 0)
      && (UndoMemory[posDel]->com == CPECommand_Delete)
      && (((LavaDECL*)UndoMemory[posDel]->CommandData1)->DeclType == Function)
      && (((LavaDECL*)UndoMemory[posDel]->CommandData1)->ParentDECL->DeclType == Impl)) {
    UndoMemory[posDel]->CommandData8 = (void *)((LavaDECL*)UndoMemory[posIns]->CommandData1)->OwnID;
    UndoMemory[posIns]->CommandData8 = (void *)((LavaDECL*)UndoMemory[posDel]->CommandData1)->OwnID;
    ((CPEBaseDoc*)UndoMemory[posDel]->fromDoc)->ExecViewPrivToPub((LavaDECL*)UndoMemory[posIns]->CommandData1,(long int)UndoMemory[posIns]->CommandData8);
    posDel--;
    posIns--;
  }
}

CUndoMem::~CUndoMem()
{
  delete [] UndoMemory;
  delete [] RedoMemory;
}

bool CUndoMem::DocModified()
{
  return (undoMemPosition != saveMemPosition);
}

void CUndoMem::SetSavePos()
{
  saveMemPosition = undoMemPosition;
}

void CUndoMem::OnEditUndo()
{
  if ((undoMemPosition > 0) && UndoMemory[undoMemPosition -1])
    if (UndoMemory[undoMemPosition -1]->FirstLast.Contains(multiDocHint))
      LBaseData->MultiUndoMem.Undo(UndoMemory[undoMemPosition -1]);
    else
      ((CPEBaseDoc*)UndoMemory[undoMemPosition-1]->fromDoc)->Undo();
  OnUpdateEditUndo(LBaseData->editUndoActionPtr);
  OnUpdateEditRedo(LBaseData->editRedoActionPtr);
}

void CUndoMem::OnUpdateEditUndo(QAction *action)
{
  action->setEnabled((undoMemPosition > 0) && !UndoMemory[undoMemPosition-1]->FirstLast.Contains(blockedHint));
}

bool CUndoMem::EnableUndo()
{
  return ((undoMemPosition > 0) && !UndoMemory[undoMemPosition-1]->FirstLast.Contains(blockedHint));
}

void CUndoMem::OnEditRedo()
{
  if ((redoMemPosition > 0) && RedoMemory[redoMemPosition -1])
    if (RedoMemory[redoMemPosition-1]->FirstLast.Contains(multiDocHint))
      LBaseData->MultiUndoMem.Redo(RedoMemory[redoMemPosition-1]);
    else
      ((CPEBaseDoc*)RedoMemory[redoMemPosition-1]->fromDoc)->Undo(TRUE);
  OnUpdateEditUndo(LBaseData->editUndoActionPtr);
  OnUpdateEditRedo(LBaseData->editRedoActionPtr);
}

void CUndoMem::OnUpdateEditRedo(QAction *action)
{
  action->setEnabled((redoMemPosition > 0) && !RedoMemory[redoMemPosition-1]->FirstLast.Contains(blockedHint));
}

void CMultiUndoMem::StartMultiDocUpdate()
{
  StartUpdate = true;
}

 void CMultiUndoMem::AddToMem(CLavaPEHint *hint)
{
  CMultiDocEntry *elem;
  CHE *che;
  if (hint->FirstLast.Contains(firstHint)) {
    elem = new CMultiDocEntry;
    elem->FirstHint = hint;
    elem->FileName = ((CLavaBaseDoc*)hint->fromDoc)->GetAbsSynFileName();
    if (StartUpdate) {
      StartUpdate = false;
      elem->StartUpdate = true;
    }
    che = new CHE(elem);
    MultiUndo.Append(che);
  }
}

 void CMultiUndoMem::SetLastHint(CLavaPEHint *hint)
 {
  CHE *che;
  if (hint->FirstLast.Contains(lastHint)) {
    for (che = (CHE*)MultiUndo.last;
         che && (((CMultiDocEntry*)che->data)->FirstHint->fromDoc != hint->fromDoc
             && !((CMultiDocEntry*)che->data)->StartUpdate);
         che = (CHE*)che->predecessor);
    if (che && ((CMultiDocEntry*)che->data)->FirstHint->fromDoc == hint->fromDoc)
      ((CMultiDocEntry*)che->data)->LastHint = hint;
  }
}

bool CMultiUndoMem::Undo(CLavaPEHint *withLastHint)
{
  CHE *cheHint, *che, *cheStart=0, *cheFin=0, *cheErr=0;
  int result = 0;
  CLavaPEHint *lHint;
  QString mess;
  DString str0;

  cheFin = (CHE*)MultiUndo.last;
  for (cheHint = (CHE*)MultiUndo.last;
       cheHint && (((CMultiDocEntry*)cheHint->data)->LastHint != withLastHint);
       cheHint = (CHE*)cheHint->predecessor)
    if (((CMultiDocEntry*)cheHint->data)->StartUpdate)
      cheFin = (CHE*)cheHint->predecessor;
  if (cheHint) {
    for (cheStart = cheHint;
         cheStart && !((CMultiDocEntry*)cheStart->data)->StartUpdate;
         cheStart = (CHE*)cheStart->predecessor);
    cheStart = (CHE*)cheStart->predecessor;
    for (che = cheFin; che && (che != cheStart) && (result != 2); che = (CHE*)che->predecessor) {
      lHint = ((CMultiDocEntry*)che->data)->LastHint;
      if (lHint) {
        if (((CPEBaseDoc*)lHint->fromDoc)->UndoMem.GetLastHint() != lHint) {
          cheErr = che;
          result = 1;  //not the last update
        }
      }
      else {
        cheErr = che;
        result = 2; //undo for ever impossible
      }
    }
    if (!result) {
      for (che = cheFin; che && (che != cheStart); che = (CHE*)che->predecessor)
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->Undo();
      for (che = cheFin; che && (che != cheStart); che = (CHE*)che->predecessor)
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateOtherDocs(0, str0, 0, false);
      lHint = new CLavaPEHint(CPECommand_FromOtherDoc, 0, (const unsigned long) 0);
      for (che = cheFin; che && (che != cheStart); che = (CHE*)che->predecessor) {
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateOtherDocs(0, str0, 0, false);
        lHint->fromDoc = ((CMultiDocEntry*)che->data)->LastHint->fromDoc;
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateAllViews(NULL, CHLV_showError, lHint);
      }
      delete lHint;
      return true;
    }
    else {
      mess = QString("You want to undo an update - from drag/drop-operation - where also other document(s) are involved:");
      mess += QString("\n   - but in document ");
      mess += QString(((CMultiDocEntry*)cheErr->data)->FileName.c);
      if (result == 1)
        mess += QString(" this is not the last update.");
      else
        mess += QString(" this update can never be undone.");
      mess += QString("\n    Therefore undo(redo) does not work.");
      if (result == 2) {
        mess += QString("\n\nWould you like to undo the update nevertheless in those documents where it is possible?\nPress Yes-button if you like, press No otherwise");
      if (QMessageBox::question(
        wxTheApp->m_appWindow,qApp->applicationName(),mess,
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No) == QMessageBox::Yes) {
        //if (QMessageBox() == QMessageBox::Yes) {//mess, MB_YESNO|MB_ICONQUESTION) == IDYES) {
          che = cheFin;
          while (che && (che != cheStart)) {
            if (!((CMultiDocEntry*)che->data)->LastHint) {
              if (((CMultiDocEntry*)che->data)->StartUpdate)
                ((CMultiDocEntry*)((CHE*)che->successor)->data)->StartUpdate = true;
              cheErr = che;
              che = (CHE*)che->predecessor;
              MultiUndo.Delete(cheErr);
            }
            else
              che = (CHE*)che->predecessor;
          }
          Undo(withLastHint);
        }
        else {
          for (che = cheFin; che && (che != cheStart); che = (CHE*)che->predecessor)
            if (((CMultiDocEntry*)che->data)->LastHint) {
              ((CMultiDocEntry*)che->data)->LastHint->FirstLast.INCL(blockedHint);
              ((CMultiDocEntry*)che->data)->FirstHint->FirstLast.INCL(blockedHint);
            }
        }
      }
      else
        critical(wxTheApp->m_appWindow,qApp->applicationName(),mess);
        //QMessageBox(mess);
    }
  }
  //program error
  return false;
}

bool CMultiUndoMem::Redo(CLavaPEHint *withFirstHint)
{
  CHE *cheHint, *che, *cheStart=0, *cheFin=0, *cheErr=0;
  CLavaPEHint *lHint;
  QString mess;
  DString str0;
  int result=0;

  for (cheHint = (CHE*)MultiUndo.last;
       cheHint && (((CMultiDocEntry*)cheHint->data)->FirstHint != withFirstHint);
       cheHint = (CHE*)cheHint->predecessor) {
    if (((CMultiDocEntry*)cheHint->data)->StartUpdate)
      cheFin = (CHE*)cheHint;
  }
  if (cheHint) {
    for (cheStart = cheHint;
         cheStart && !((CMultiDocEntry*)cheStart->data)->StartUpdate && !result;
         cheStart = (CHE*)cheStart->predecessor) {
    }
    for (che = cheStart; (che != cheFin) && (result != 2); che = (CHE*)che->successor) {
      lHint = ((CMultiDocEntry*)che->data)->FirstHint;
      if (lHint) {
        if (((CPEBaseDoc*)lHint->fromDoc)->UndoMem.GetLastRedo() != lHint) {
          cheErr = che;
          result = 1; //not the last undone update
        }
      }
      else {
        cheErr = che;
        result = 2; //redo for ever impossible
      }
    }
    if (!result) {
      for (che = cheStart; che != cheFin; che = (CHE*)che->successor)
         ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->Undo(true);
      for (che = cheStart; che != cheFin; che = (CHE*)che->successor)
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateOtherDocs(0, str0, 0, false);
      lHint = new CLavaPEHint(CPECommand_FromOtherDoc, 0, (const unsigned long) 0);
      for (che = cheStart; che != cheFin; che = (CHE*)che->successor) {
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateOtherDocs(0, str0, 0, false);
        lHint->fromDoc = ((CMultiDocEntry*)che->data)->LastHint->fromDoc;
        ((CPEBaseDoc*)((CMultiDocEntry*)che->data)->LastHint->fromDoc)->UpdateAllViews(NULL, CHLV_showError, lHint);
      }
      delete lHint;
      return true;
    }
    else {
      mess = QString("You want to redo an update - from drag/drop-operation - where also other document(s) are involved:");
      mess += QString("\n   - but in document ");
      mess += QString(((CMultiDocEntry*)cheErr->data)->FileName.c);
      if (result == 1)
        mess += QString(" this is not the last undone update.");
      else
        mess += QString(" this update can never be redone.");
      mess += QString("\n    Therefore redo does not work.");
      if (result == 2) {
        mess += QString("\n\nWould you like to redo the update nevertheless in those documents where it is possible?\nPress Yes-button if you like, press No otherwise");
      if (QMessageBox::question(
        wxTheApp->m_appWindow,qApp->applicationName(),mess,
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No) == QMessageBox::Yes) {
        //if (QMessageBox()) {//mess, MB_YESNO|MB_ICONQUESTION) == IDYES) {
          che = cheStart;
          while (che != cheFin) {
            if (!((CMultiDocEntry*)che->data)->LastHint) {
              if (((CMultiDocEntry*)che->data)->StartUpdate)
                ((CMultiDocEntry*)((CHE*)che->successor)->data)->StartUpdate = true;
              cheErr = che;
              che = (CHE*)che->successor;
              MultiUndo.Delete(cheErr);
            }
            else
              che = (CHE*)che->successor;
          }
          Redo(withFirstHint);
        }
        else {
          for (che = cheStart; che != cheFin; che = (CHE*)che->successor)
            if (((CMultiDocEntry*)che->data)->LastHint) {
              ((CMultiDocEntry*)che->data)->LastHint->FirstLast.INCL(blockedHint);
              ((CMultiDocEntry*)che->data)->FirstHint->FirstLast.INCL(blockedHint);
            }
        }
      }
      else
        critical(wxTheApp->m_appWindow,qApp->applicationName(),mess);
        //QMessageBox();//mess);
    }
  }
  //program error
  return false;
}

void CMultiUndoMem::RemoveHint(CLavaPEHint *fHint)
{
  CHE *che;
  for (che = (CHE*)MultiUndo.first; che; che = (CHE*)che->successor) {
    if (((CMultiDocEntry*)che->data)->FirstHint == fHint) {
      ((CMultiDocEntry*)che->data)->FirstHint = 0;
      ((CMultiDocEntry*)che->data)->LastHint = 0;
    }
  }
}

CBrowseContext::CBrowseContext(wxView* view, CHEFormNode* node)
{
  setObjectName("BrowseContext");
  prev = LBaseData->Browser->LastBrowseContext;
  fromView = view;
  synObjSel = 0;
  formNode = node;
  type = NoDef;
}


CBrowseContext::CBrowseContext(wxView* view, LavaDECL* decl, int t)
{
  setObjectName("BrowseContext");
  prev = LBaseData->Browser->LastBrowseContext;
  fromView = view;
  synObjSel = 0;
  formNode = 0;
  type = t;
  id = TID(decl->OwnID, decl->inINCL);
  if (decl->ParentDECL)
    parentID = TID(decl->ParentDECL->OwnID, decl->inINCL);
}


CBrowseContext::CBrowseContext(wxView* view, SynObjectBase* synObj)
{
  setObjectName("BrowseContext");
  prev = LBaseData->Browser->LastBrowseContext;
  fromView = view;
  synObjSel = synObj;
  formNode = 0;
  type = NoDef;
}

void CBrowseContext::RemoveView(wxView* view)
{
  CBrowseContext *del, *suc = 0, *act = LBaseData->Browser->LastBrowseContext;
  while (act) {
    if (act->fromView == view) {
      del = act;
      act = act->prev;
      if (suc)
        suc->prev = act;
      else
        LBaseData->Browser->LastBrowseContext = act;
      delete del;
    }
    else {
      suc = act;
      act = suc->prev;
    }
  }
}

void CBrowseContext::RemoveSynObj(SynObjectBase* synObj)
{
  CBrowseContext *del, *suc = 0, *act = LBaseData->Browser->LastBrowseContext;
  while (act) {
    if (act->synObjSel == synObj) {
      del = act;
      act = act->prev;
      if (suc)
        suc->prev = act;
      else
        LBaseData->Browser->LastBrowseContext = act;
      delete del;
    }
    else {
      suc = act;
      act = suc->prev;
    }
  }
}


void CBrowseContext::RemoveFormNode()
{
  CBrowseContext *del, *suc = 0, *act = LBaseData->Browser->LastBrowseContext;
  while (act) {
    if (act->formNode) {
      del = act;
      act = act->prev;
      if (suc)
        suc->prev = act;
      else
        LBaseData->Browser->LastBrowseContext = act;
      delete del;
    }
    else {
      suc = act;
      act = suc->prev;
    }
  }
}


bool CPEBaseBrowse::CanBrowse(LavaDECL* DECL)
{
  if (DECL &&  ((DECL->RefID.nID >= 0)
             || (DECL->Supports.first)
             || (DECL->Inherits.first)
             || (DECL->DeclDescType == NamedType)
     ))
    return TRUE;
  return FALSE;
}

LavaDECL* CPEBaseBrowse::BrowseDECL(wxDocument* doc, TID& id, DString* enumID, bool openExec)
{
  mySynDef = ((CPEBaseDoc*)doc)->mySynDef;
  LavaDECL* decl = 0;
  decl = ((TIDTable*)mySynDef->IDTable)->GetDECL(id);
  if (!decl || !GotoDECL(doc, decl, id, false, enumID, openExec))
    DestroyLastBrsContext();
  return decl; //decl in the calling document
}

bool CPEBaseBrowse::GotoImpl(wxDocument* fromDoc, TID id)
{
  LavaDECL* decl = ((CLavaBaseDoc*)fromDoc)->IDTable.GetDECL(id);
  return GotoImpl(fromDoc, decl);
}

void CPEBaseBrowse::DestroyLastBrsContext()
{
  if (LastBrowseContext) {
    CBrowseContext* last = LastBrowseContext;
    LastBrowseContext = last->prev;
    delete last;
  }
}


TBasicType CPEBaseBrowse::GetBasicType(SynDef *lavaCode, LavaDECL* decl)
{
  if (decl)
    switch (decl->DeclDescType) {
      case BasicType:
        return decl->BType;
      case EnumType:
        return Enumeration;
      case NamedType:
       return GetBasicType(lavaCode, ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL));
      case StructDesc:
      case Undefined:
        return NonBasic;
      default: ;
    }
  return NonBasic;

}


TDeclDescType CPEBaseBrowse::GetExprType(SynDef *lavaCode, LavaDECL *decl)
{
  if (decl)
    switch (decl->DeclDescType) {
      case BasicType:
      case StructDesc:
      case EnumType:
      case LiteralString:
      case ExecDesc:
      case Undefined:
        return decl->DeclDescType;
      case NamedType:
        return GetExprType(lavaCode, ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL));
      default: ;
    }
  return UnknownDDT;
}

TDeclType CPEBaseBrowse::GetCategory(SynDef *lavaCode, LavaDECL *decl, SynFlags& flags)
{
  if (decl) {
    flags = decl->TypeFlags;
    switch (decl->DeclType) {
//      case VirtualType:
      case Alias:
      case Attr:
      case IAttr:
      case OAttr:
        if ((decl->DeclDescType == BasicType) || (decl->DeclDescType == LiteralString) )
          return BasicDef;
        else
          if (decl->DeclDescType == NamedType) {
            LavaDECL* refDECL = ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL);
            return GetCategory(lavaCode, refDECL, flags);
          }
        break;
      default:
        return decl->DeclType;
    }
  }
  return UnDef;
}


CLavaBaseData *LBaseData;



void CLavaBaseData::Init(CPEBaseBrowse *browser, CBaseExecUpdate *execUpdate)

{
  LavaBaseStringInit();
  Browser = browser;
  ExecUpdate = execUpdate;
  BasicNames[NonBasic]     = "NonBasic";
  BasicNames[B_Object]     = "Object";
  BasicNames[Bitset]       = "Bitset";
  BasicNames[B_Bool]       = "Boolean";
  BasicNames[Char]         = "Char";
  BasicNames[Integer]      = "Integer";
  BasicNames[Float]        = "Float";
  BasicNames[Double]       = "Double";
  BasicNames[VLString]     = "String";
  BasicNames[Enumeration]  = "Enumeration";
  BasicNames[B_Set]        = "Set";
  BasicNames[B_Chain]      = "Chain";
  BasicNames[B_Che]        = "Handle";
  BasicNames[B_Array]      = "Array";
  BasicNames[ComponentObj] = "ComponentObj";
  BasicNames[B_Exception]  = "Exception";
  BasicNames[B_HWException]  = "HardwareException";
  BasicNames[B_RTException]  = "RuntimeException";
  BasicNames[B_GUI]          = "GUI";
  BasicNames[Identifier]   = "Identifier";

  OperatorNames [OP_equal ] =    DString("operator \"=\"");
  OperatorNames [OP_notequal ] = DString("operator \"=\"");
  OperatorNames [OP_lessthan ] = DString("operator \"<\"");
  OperatorNames [OP_greaterthan ] = DString("operator \">\"");
  OperatorNames [OP_lessequal] = DString("operator \"<=\"");
  OperatorNames [OP_greaterequal] = DString("operator \">=\"");
  OperatorNames [OP_plus ] =     DString("operator \"+\"");
  OperatorNames [OP_minus ] =    DString("operator \"-\"");
  OperatorNames [OP_mult ]  =    DString("operator \"*\"");
  OperatorNames [OP_div ]  =     DString("operator \"/\"");
  OperatorNames [OP_mod ]   =    DString("operator \"%\"");
  OperatorNames [OP_bwAnd ]  =   DString("operator \"&\"");
  OperatorNames [OP_bwOr ] =     DString("operator \"|\"");
  OperatorNames [OP_bwXor ] =    DString("operator \"||\"");
  OperatorNames [OP_invert ]   = DString("operator \"\"");
  OperatorNames [OP_lshift ]   = DString("operator \"<<\"");
  OperatorNames [OP_rshift ]   = DString("operator \">>\"");
  OperatorNames [OP_arrayGet ] = DString("operator \"<-[]\"");
  OperatorNames [OP_arraySet ] = DString("operator \"[]<-\"");
  OperatorNames [OP_fis ] =      DString("operator \"#\"");

  OpFuncNames [OP_equal ] =    DString("equal");
  OpFuncNames [OP_notequal ] = DString("not_equal");
  OpFuncNames [OP_lessthan ] = DString("less_then");
  OpFuncNames [OP_greaterthan ] = DString("greater_then");
  OpFuncNames [OP_lessequal] = DString("less_equal");
  OpFuncNames [OP_greaterequal] = DString("greater_equal");
  OpFuncNames [OP_plus ] =     DString("plus");
  OpFuncNames [OP_minus ] =    DString("minus");
  OpFuncNames [OP_mult ]  =    DString("mult");
  OpFuncNames [OP_div ]  =     DString("div");
  OpFuncNames [OP_mod ]   =    DString("mod");
  OpFuncNames [OP_bwAnd ]  =   DString("and");
  OpFuncNames [OP_bwOr ] =     DString("or");
  OpFuncNames [OP_bwXor ] =    DString("xor");
  OpFuncNames [OP_invert ]   = DString("invert");
  OpFuncNames [OP_lshift ]   = DString("lshift");
  OpFuncNames [OP_rshift ]   = DString("rshift");
  OpFuncNames [OP_arrayGet ] = DString("arrayGet");
  OpFuncNames [OP_arraySet ] = DString("arraySet");
  OpFuncNames [OP_fis ] =      DString("ord");


  maxb=((unsigned)Identifier);
  actHint = 0;
  inMultiDocUpdate = false;
  if (Browser)
    Browser->LastBrowseContext = 0;
  LBaseData = this;
}

CLavaBaseData::~CLavaBaseData()
{
}


bool CLavaBaseData::isIdentifier(const QString& ident)
{
  int len = ident.length();

  if (len && (ident[0].isLetter() || (ident[0] == '_'))) {
    int ii = 1;
    while (ii < len) {
      if (ident[ii].isLetterOrNumber() || (ident[ii] == '_'))
        ii += 1;
      else
        return FALSE;
    }
    return TRUE;
  }
  else
    return FALSE;
}


DString CLavaBaseData::calcRelName(const DString& qname, const DString& scopeName)
{
  int pPos=0, pos ;
  DString relName = qname;
  for (pos = 0;
      (pos < qname.l) && (pos < scopeName.l) && qname[pos] == scopeName[pos]; pos++)
    if (qname[pos] == ':')
      pPos = pos;
  if ((scopeName.l == pos) && (qname.l > pos) && (qname[pos] == ':'))
    pPos = pos+1;  //auf dem zweite Doppelpunkt
  if (pPos)
    relName.Delete(0, pPos+1);
  return relName;
}


void LavaEnd(wxDocument* fromDoc, bool doClose)
{
  CThreadList *thrL;
  CLavaThread *curThr;

	curThr = (CLavaThread*)QThread::currentThread();
  if (curThr == wxTheApp->mainThread) { // mainThread!
    if (((CLavaBaseDoc*)fromDoc)->throwError) {
      if (((CLavaBaseDoc*)fromDoc)->ThreadList) {
        thrL = ((CLavaBaseDoc*)fromDoc)->ThreadList;
        for (int i=0; i<thrL->size(); i++) {
          thrL->at(i)->resume();
        }
        return;
      }
      else
		    throw CUserException();
    }
    if (doClose ) {
      if (((CLavaBaseDoc*)fromDoc)->isObject ) {
        if (/*!((CLavaBaseDoc*)fromDoc)->IsEmbedded() &&*/ ((CLavaBaseDoc*)fromDoc)->OnSaveModified()) {
          ((CLavaBaseDoc*)fromDoc)->OnCloseDocument();
          delete fromDoc;
        }
        return;
      }
      else {
        wxView* tview;
        int pos;
        for (pos = 0; pos < fromDoc->m_documentViews.size(); pos++) {
          tview = fromDoc->m_documentViews[pos];
          delete tview->GetParentFrame(); //->DestroyWindow();
        }
      }
    }
  }
  else {
    bool err = ((CLavaBaseDoc*)fromDoc)->throwError;
    ((CLavaBaseDoc*)fromDoc)->throwError = false;
    CLavaPEHint* hint = new CLavaPEHint(CPECommand_LavaEnd, fromDoc, (const unsigned long)3,curThr);
		QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaEnd,(void*)hint));
    if (((CLavaBaseDoc*)fromDoc)->ThreadList) {
      thrL = ((CLavaBaseDoc*)fromDoc)->ThreadList;
      for (int i=0; i<thrL->size(); i++) {
				if ((thrL->at(i) != curThr))
          thrL->at(i)->resume();
      }
		}
		if (err)
			throw CUserException();
  }
}

QMessageBox::StandardButton critical(QWidget *parent, const QString &caption,
	  const QString &text,
                QMessageBox::StandardButtons buttons, QMessageBox::StandardButton dftButton) {
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();

  if (currentThread == wxTheApp->mainThread) // mainThread!
                return QMessageBox::critical(parent,caption,text,buttons,dftButton);

  CMsgBoxParams params(
                currentThread,0,parent,caption,text,buttons,dftButton);

	QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaMsgBox,&params));
  currentThread->suspend();
	return params.result;
}

QMessageBox::StandardButton information(QWidget *parent, const QString &caption,
		const QString& text,
                QMessageBox::StandardButtons buttons, QMessageBox::StandardButton dftButton) {
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();

  if (currentThread == wxTheApp->mainThread) // mainThread!
    return QMessageBox::information(parent,caption,text,buttons,dftButton);

  CMsgBoxParams params(
                currentThread,1,parent,caption,text,buttons,dftButton);

  currentThread->waitingForUI = true;
	QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaMsgBox,&params));
  currentThread->suspend();
	return params.result;
}

QMessageBox::StandardButton question(QWidget *parent, const QString &caption,
		const QString& text,
                QMessageBox::StandardButtons buttons, QMessageBox::StandardButton dftButton) {
  CLavaThread *currentThread = (CLavaThread*)QThread::currentThread();

	if (currentThread == wxTheApp->mainThread) // mainThread!
                return QMessageBox::question(parent,caption,text,buttons,dftButton);

  CMsgBoxParams params(
                currentThread,2,parent,caption,text,buttons,dftButton);

	QApplication::postEvent(wxTheApp, new CustomEvent(UEV_LavaMsgBox,&params));
  currentThread->suspend();
	return params.result;
}

QString L_GetOpenFileName(const QString& startFileName,
				      QWidget *parent,
				      const QString& caption,
				      const QString& filter,
				      const QString& filter2
            )
{
  QFileInfo qf;
  QString fileName, currentFilter, initialDir;
  QFileDialog *fd = new QFileDialog(parent);
	QStringList filters;

  qf = QFileInfo(startFileName);
  fileName = qf.fileName();
  QFileInfo qfresolved(ResolveLinks(qf));
  currentFilter = qfresolved.suffix();
  initialDir = qf.path();
  fd->setDirectory(qf.absolutePath());

	filters << filter;
  if (filter2 != QString::null)
    filters << filter2;
	fd->setNameFilters(filters);
  fd->setWindowTitle(caption);
  if (currentFilter.isEmpty())
    currentFilter = "lava";
  currentFilter = "*." + currentFilter;
  if (filter2.contains(currentFilter))
    currentFilter = filter2;
  else
    currentFilter = filter;
  fd->selectNameFilter(currentFilter);
  fd->setFileMode( QFileDialog::ExistingFile );
  fd->setViewMode( QFileDialog::List );
  if (LBaseData->inRuntime) {
   if (!fileName.contains(".lcom"))
     fd->selectFile(fileName);
  }
  else {
   if (!fileName.contains(".ldoc"))
     fd->selectFile(fileName);
  }
  fd->setResolveSymlinks(false);
  if (fd->exec() == QDialog::Accepted ) {
    QStringList selFiles=fd->selectedFiles();
    selFiles.replaceInStrings("\\", "/");
    fileName = selFiles.first();
    delete fd;
    return fileName;
  }
  else {
    delete fd;
    return 0;
  }
}

QStringList L_GetOpenFileNames(const QString& startFileName,
				      QWidget *parent,
				      const QString& caption,
				      const QString& filter,
				      const QString& filter2
            )
{
  QStringList resultNames;
  QFileInfo qf = QFileInfo(startFileName);
  QFileInfo qfresolved(ResolveLinks(qf));
  QString currentFilter = qfresolved.suffix();
  QString fileName, initialDir = qf.path();

  QFileDialog *fd = new QFileDialog(parent);
	QStringList filters;

	filters << filter;
  if (filter2 != QString::null)
    filters << filter2;
  fileName = qf.fileName();
  currentFilter = qfresolved.suffix();
  initialDir = qf.path();
  fd->setDirectory(qf.absolutePath());
	fd->setNameFilters(filters);
  fd->setWindowTitle(caption);
  if (currentFilter.isEmpty())
    currentFilter = "lava";
  currentFilter = "*." + currentFilter;
  if (filter2.contains(currentFilter))
    currentFilter = filter2;
  else
    currentFilter = filter;
  fd->selectNameFilter(currentFilter);
  if (LBaseData->inRuntime) {
   if (!fileName.contains(".lcom"))
     fd->selectFile(fileName);
  }
  else {
   if (!fileName.contains(".ldoc"))
     fd->selectFile(fileName);
  }
  fd->setFileMode( QFileDialog::ExistingFiles );
  fd->setViewMode( QFileDialog::List );
  if (fd->exec() == QDialog::Accepted ) {
    resultNames = fd->selectedFiles();
    delete fd;
    resultNames.replaceInStrings("\\", "/");
    return resultNames;
  }
  else {
    delete fd;
    return QStringList();
  }
}
