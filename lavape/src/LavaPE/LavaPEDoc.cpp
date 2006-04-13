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

#include "LavaBaseStringInit.h"
#include "LavaPEStringInit.h"
#include "LavaPEDoc.h"
#include "ExecView.h"
#include "LavaGUIView.h"
#include "LavaPEWizard.h"
#include "Bars.h"
#include "VTView.h"
#include "Convert.h"
#include "LavaPEFrames.h"
#include "SynIO.h"
#include "Boxes.h"
#include "docview.h"
#include "qmessagebox.h"
#include "qpixmapcache.h"
#include "qfileinfo.h"
#include "q3process.h"
#include "q3ptrlist.h"
//Added by qt3to4:
#include <QPixmap>

#pragma hdrstop


CLavaPEDoc::CLavaPEDoc()
{
  hasHint = false;
  drawView = false;
  nErrors = 0;
  nPlaceholders = 0;
  hasErrorsInTree = false;
  changeInUpdate = false;
  isReadOnly = false;
  openInTotalCheck = false;
  checkedInTotalCheck = false;
}

CLavaPEDoc::~CLavaPEDoc()
{
}



bool CLavaPEDoc::AddVBase(LavaDECL* classDECL, LavaDECL* conDECL )
{
  LavaDECL *IFace, *ElFunc;
  bool elOk, ok = true;
  CHETVElem *El;

  CHETID *cheID = (CHETID*)classDECL->Supports.first;
  while (cheID) { 
    IFace = IDTable.GetDECL(cheID->data, classDECL->inINCL);
    if (IFace) {
      if (IFace->DeclType == VirtualType) {
        IFace = IDTable.GetFinalBasicType(cheID->data, classDECL->inINCL, conDECL);
        if (!IFace)
          return false;
        if (IFace->VElems.UpdateNo <= UpdateNo) {
          ResetVElems(IFace);
          elOk = MakeVElems(IFace);
          ok = ok && elOk;
        }
        elOk = AddVElems(conDECL, IFace);
        ok = ok && elOk;
        if (IFace->fromBType == NonBasic) {
          //default initializer vorhanden?
          ElFunc = 0;
          for (El = (CHETVElem*)IFace->VElems.VElems.first;
               El && (!ElFunc || !ElFunc->TypeFlags.Contains(defaultInitializer));
               El = (CHETVElem*)El->successor) {
            if (IFace == IDTable.GetDECL(El->data.VTClss)) {
              ElFunc = IDTable.GetDECL(El->data.VTEl);
              if (ElFunc && (ElFunc->DeclType != Function))
                ElFunc = 0;
            }
            else
              ElFunc = 0;
          }
          if (!ElFunc || !ElFunc->TypeFlags.Contains(defaultInitializer)) 
            new CLavaError(&conDECL->DECLError1, &ERR_VBaseWithoutDefaultIni);
        }
            
      }
      elOk = AddVBase(IFace, conDECL);
      ok = ok && elOk;
      if (!ok) {// there is an error in a base class
        new CLavaError(&classDECL->DECLError1, &ERR_InVTofBaseIF);
        classDECL->WorkFlags.INCL(recalcVT);
      }
    }
    cheID = (CHETID*)cheID->successor;
  }
  return ok;
}


bool CLavaPEDoc::AddVElems(LavaDECL *decl, LavaDECL* baseDECL)
{
  bool ok = true, allOK = true;
  QString cstr;
  CHETVElem *El, *lastEl, *ElC, *addElem;
  CHETID *cheID;
  TID addID, addID2, IDClss;

  addElem = (CHETVElem*)baseDECL->VElems.VElems.first;
  while (addElem) {
    for (El = (CHETVElem*)decl->VElems.VElems.first;
         El && (El->data.VTClss != addElem->data.VTClss);
         El = (CHETVElem*)El->successor);
    if (!El) {
      //it is from a new class, add all entries from this class
      IDClss = addElem->data.VTClss;
      while (addElem && (addElem->data.VTClss == IDClss)) {
        El = new CHETVElem;
        El->data.VTClss = addElem->data.VTClss;
        El->data.VTEl = addElem->data.VTEl; 
        El->data.VTBaseEl = addElem->data.VTBaseEl; 
        El->data.op = addElem->data.op;
        El->data.TypeFlags = addElem->data.TypeFlags;
        El->data.Ambgs = addElem->data.Ambgs;
        El->data.ok = addElem->data.ok;
        decl->VElems.VElems.Append(El);
        addElem = (CHETVElem*)addElem->successor;
      }
    }
    else {
      ElC = El;
      for (lastEl = El;
           lastEl && (lastEl->data.VTClss == addElem->data.VTClss);
           lastEl = (CHETVElem*)lastEl->successor);
      //is it or is an extension allready in the table?
      while (El && !IDTable.IsAn(El->data.VTEl, 0, addElem->data.VTEl, 0))
        if (El == lastEl)
          El = 0;
        else
          El = (CHETVElem*)El->successor;
      if (!El) {
        El = ElC;
        El = ElC;
        //is it an extension of an element in the table?
        while (El && !IDTable.IsAnc(addElem->data.VTEl, 0, El->data.VTEl, 0))
          if (El == lastEl)
            El = 0;
          else
            El = (CHETVElem*)El->successor;
        if (El) {
          El->data.VTEl = addElem->data.VTEl; 
          El->data.VTBaseEl = addElem->data.VTBaseEl; 
          El->data.TypeFlags = addElem->data.TypeFlags;
          El->data.op = addElem->data.op;
          El->data.ok = addElem->data.ok;
        }
        else {
          //find the position in the table
          El = IDTable.FindSamePosInVT(addElem, ElC, lastEl);
          if (El) { //same pos in VT
            if (!IDTable.Overrides(El->data.VTEl, 0, addElem->data.VTEl, 0,decl)) {
              if (IDTable.Overrides(addElem->data.VTEl, 0, El->data.VTEl, 0, decl)) {
                El->data.VTEl = addElem->data.VTEl; 
                El->data.VTBaseEl = addElem->data.VTBaseEl; 
                El->data.TypeFlags = addElem->data.TypeFlags;
                El->data.op = addElem->data.op;
                El->data.ok = addElem->data.ok;
              }
              else {
                if (El->data.Ambgs.first)
                  for (cheID = (CHETID*)El->data.Ambgs.first;
                       cheID && (cheID->data != addElem->data.VTEl);
                       cheID = (CHETID*)cheID->successor);
                else
                  cheID = 0;
                if (!cheID) {
                  cheID = new CHETID;
                  cheID->data = addElem->data.VTEl; 
                  El->data.Ambgs.Append(cheID);
                }
                El->data.ok = false;
              }
            }//same pos in VT
            //else El overrides addElem
          }
          else {
            //the base class IFace has an error in the table
            new CLavaError(&baseDECL->DECLError2, &ERR_InVT);
            ok = false;
          }
        }
      }
      addElem = (CHETVElem*)addElem->successor;
    }
    allOK = allOK && ok;
  }
  return allOK;
}


void CLavaPEDoc::AutoCorr(LavaDECL* decl)
{
  LavaDECL *newDECL;
  QString cstr;
  CHE *cheEl, *cheIOEl, *che = (CHE*)decl->DECLError1.first;
  CHETID *cheTID, *ncheTID;
  bool lfuncImpl;
  while (che) {
    if (((CLavaError*)che->data)->showAutoCorrBox) {
      ((CLavaError*)che->data)->showAutoCorrBox = false;
      if (((CLavaError*)che->data)->IDS == &ERR_NoOverridden) {
        newDECL = NewLavaDECL();
        *newDECL = *decl;
        CCorrOverBox * box = new CCorrOverBox(newDECL, this, MainView);
        if (box->OnInitDialog() == BoxContinue) {
          if (box->exec() != QDialog::Accepted) {
            delete newDECL;
            return; 
          }
        }
        else {
          delete newDECL;
          return;
        }
      }
      else {
        lfuncImpl = (((CLavaError*)che->data)->IDS == &ERR_MissingItfFuncDecl)
                     || (((CLavaError*)che->data)->IDS == &ERR_NoSetGetMember)
                     || (((CLavaError*)che->data)->IDS == &ERR_NoImplForAbstract)
                    || (((CLavaError*)che->data)->IDS == &ERR_ImplOfAbstract);
        if (AutoCorrBox(((CLavaError*)che->data)->IDS) != QDialog::Accepted) {
          if (lfuncImpl)
            decl->WorkFlags.INCL(allowDEL);
          return;
        }
        newDECL = NewLavaDECL();
        *newDECL = *decl;
        if (lfuncImpl) {
          newDECL->SecondTFlags.EXCL(funcImpl);      
          newDECL->TypeFlags.EXCL(isProtected);
          newDECL->TypeFlags.EXCL(isPropGet);   
          newDECL->TypeFlags.EXCL(isPropSet);
          newDECL->TypeFlags.EXCL(isInitializer);
          newDECL->TypeFlags.EXCL(defaultInitializer);
          newDECL->Supports.Destroy();
          if (newDECL->op != OP_noOp) {
            newDECL->LocalName = DString("leaved_from_operator_") + LBaseData->OpFuncNames[newDECL->op];
            newDECL->op = OP_noOp;
          }
          cheIOEl = (CHE*)newDECL->NestedDecls.first;
          while (cheIOEl) {
            ((LavaDECL*)cheIOEl->data)->SecondTFlags.EXCL(funcImpl);     
            ((LavaDECL*)cheIOEl->data)->TypeFlags.EXCL(isPropGet);    
            ((LavaDECL*)cheIOEl->data)->TypeFlags.EXCL(isPropSet);
            ((LavaDECL*)cheIOEl->data)->Supports.Destroy();
            cheIOEl = (CHE*)cheIOEl->successor;
          }
        }
        else {
          if (((CLavaError*)che->data)->IDS == &ERR_OverriddenStatic) {
            newDECL->Supports.Destroy();
            newDECL->SecondTFlags.EXCL(overrides);
          }
          else if (((CLavaError*)che->data)->IDS == &ERR_NoAbstract) 
            newDECL->TypeFlags.EXCL(isAbstract);
          else {
            cheTID = (CHETID*)newDECL->Supports.first;
            while (cheTID) {
              if (!IDTable.GetDECL(cheTID->data, newDECL->inINCL)) {
                ncheTID = (CHETID*)cheTID->successor;
                if (!ncheTID && (decl->DeclType == Interface)
                    && (cheTID == (CHETID*)newDECL->Supports.first)) {
                  if (isStd)
                    cheTID->data.nINCL = 0;
                  else
                    cheTID->data.nINCL = 1;
                  cheTID->data.nID = IDTable.BasicTypesID[B_Object];
                  if (cheTID->data.nID < 0)
                    cheTID = 0;
                }
                else {
                  newDECL->Supports.Uncouple(cheTID);
                  delete cheTID;
                  cheTID = ncheTID;
                }
              }
              else
                cheTID = (CHETID*)cheTID->successor;
            }
            cheTID = (CHETID*)newDECL->Inherits.first;
            while (cheTID) {
              if (!IDTable.GetDECL(cheTID->data, newDECL->inINCL)) {
                ncheTID = (CHETID*)cheTID->successor;
                newDECL->Inherits.Uncouple(cheTID);
                delete cheTID;
                cheTID = ncheTID;
              }
              else
                cheTID = (CHETID*)cheTID->successor;
            }
          }
        }
      }
      for (cheEl = (CHE*)decl->ParentDECL->NestedDecls.first;
           cheEl && ((LavaDECL*)cheEl->data != decl);
           cheEl = (CHE*)cheEl->successor);
      DString *name = new DString(decl->FullName);  
      CLavaPEHint* hint = new CLavaPEHint(CPECommand_Change, this, (const unsigned long)3, (DWORD)newDECL, (DWORD)name, 0, (DWORD)&cheEl->data);
      UndoMem.AddToMem(hint);
      UpdateDoc(0, FALSE, hint);
      return;
    }
    if (che == decl->DECLError1.last)
      che = (CHE*)decl->DECLError2.first;
    else
      che = (CHE*)che->successor;
  }
}


int CLavaPEDoc::AutoCorrBox(QString* errID)
{
  QString cstr = *errID;
  cstr += "\r\n";
  if (errID == &ERR_NoOverridden) 
    cstr += "Correct the declaration what it overrides? ";
  else if ((errID == &ERR_MissingItfFuncDecl) || (errID == &ERR_NoSetGetMember) || (errID == &ERR_ImplOfAbstract)
       || (errID == &ERR_NoImplForAbstract))
    cstr += "Change to private function?  ";
  else if (errID == &ERR_NoAbstract)
    cstr += "Make the function non abstract?  ";
  else if (errID == &ERR_OverriddenStatic)
    cstr += "Remove this erroneous reference?  ";
  else if (errID == &ERR_CleanSupports)
    cstr += "Remove this redundant entry?  ";
  else if (errID == &ERR_OverriddenIOType)
    cstr += "Set the right types?  ";
  else if (errID == &ERR_NoOverridden)
    cstr += "Set the right types?  ";
  else
    cstr += "Remove this undefined reference?  ";
  return QMessageBox::question(0,qApp->name(),cstr,QMessageBox::Ok,QMessageBox::Cancel,0);
  //return AfxMessageBox(cstr, MB_OKCANCEL+MB_ICONINFORMATION);
}


bool CLavaPEDoc::CheckForm(LavaDECL* formDECL, int checkLevel)
{
  LavaDECL *classElDecl, *formElDecl, *fclassDECL, *basefDECL, *baseClassDECL, *classDECL;
  CHE *cheformEl, *cheTextEl, *checlassEl;
  CHETID *cheS, *cheF, *cheID;
  TID inID, fsupID;
  CHAINX chain;
  SynFlags typeFlags;
  bool found, changed = false;
  QString* err;

  classDECL = CheckGetFinalMType(formDECL);
  for (; classDECL && classDECL->DeclType == VirtualType;
         classDECL = IDTable.GetDECL(classDECL->RefID, classDECL->inINCL));
  if (!classDECL) {
    new CLavaError(&formDECL->DECLError1, &ERR_NoIFforForm);
    return false;
  }
  formDECL->Supports.Destroy();
  cheS = (CHETID*)formDECL->ParentDECL->Supports.first;
  fclassDECL = IDTable.GetDECL(cheS->data, formDECL->inINCL);
  if (!fclassDECL) {
    new CLavaError(&formDECL->DECLError1, &ERR_NoExtForm);
    return false;
  }
  cheS = (CHETID*)fclassDECL->Supports.first;
  while (cheS) { //!
    basefDECL = IDTable.GetFinalDef(cheS->data, fclassDECL->inINCL);
    if (basefDECL && basefDECL->TypeFlags.Contains(isGUI)) {
      baseClassDECL = IDTable.GetDECL(basefDECL->RefID, basefDECL->inINCL);
      if (!baseClassDECL) {
        new CLavaError(&formDECL->DECLError1, &ERR_NoBaseFormIF);
        return false;
      }

      if (IDTable.IsAnc(formDECL->RefID, 0, basefDECL->RefID, basefDECL->inINCL)) {
        cheF = new CHETID;
        cheF->data.nID = basefDECL->OwnID;
        cheF->data.nINCL = basefDECL->inINCL;
        formDECL->Supports.Append(cheF);
      }
    }
    cheS = (CHETID*)cheS->successor;
  }

  /*
  ChainAny idChain;
  idChain.first = formDECL->Supports.first;
  idChain.last = formDECL->Supports.last;
  formDECL->Supports.first = 0;
  formDECL->Supports.last = 0;
  CHETID *cheIDclass = (CHETID*)classDECL->Supports.first;
  CHETID *cheIDform;
  TID fID = TID(formDECL->ParentDECL->OwnID, formDECL->inINCL);
  while (cheIDclass) {
    cheIDform = (CHETID*)idChain.first;
    found = false;
    while (cheIDform) {
      if (cheIDform->data == cheIDclass->data) 
      found = IDTable.EQEQ(cheIDform->data, formDECL->inINCL, cheIDclass->data, classDECL->inINCL);
      if (!found) {
        formElDecl = IDTable.GetDECL(cheIDform->data, formDECL->inINCL);
        found = formElDecl
                && (formElDecl->DeclType == FormDef)
                && IDTable.EQEQ(formElDecl->RefID, formElDecl->inINCL, cheIDclass->data, classDECL->inINCL)
                && IDTable.IsAn(fID, 0, ((CHETID*)formElDecl->ParentDECL->Supports.first)->data, formElDecl->inINCL);
      }
      if (found) {
        changed = changed || (idChain.first != cheIDform);
        cheIDform = (CHETID*)idChain.Uncouple(cheIDform);
        formDECL->Supports.Append(cheIDform);
        cheIDform = 0;
      }
      else
        cheIDform = (CHETID*)cheIDform->successor;
    }
    if (!found) {
      cheIDform = new CHETID;
      cheIDform->data = cheIDclass->data;
      formDECL->Supports.Append(cheIDform);
      changed = true;
    }
    cheIDclass = (CHETID*)cheIDclass->successor;
  }
  changed = changed || (idChain.first != 0);
  idChain.Destroy();
*/

  chain.first = formDECL->NestedDecls.first;
  chain.last = formDECL->NestedDecls.last;
  formDECL->NestedDecls.first = 0;
  formDECL->NestedDecls.last = 0;
  if ((classDECL->DeclDescType == EnumType) && (checkLevel >= CHLV_inUpdateLow))
    if (formDECL->DeclDescType != EnumType) {
      formDECL->DeclDescType = EnumType;
      formDECL->EnumDesc = classDECL->EnumDesc;
      ((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm(formDECL, classDECL, mySynDef);
    }
    else 
      changed = CheckMenu(formDECL, classDECL);
  checlassEl = (CHE*)classDECL->NestedDecls.first;
  if (checlassEl)
    classElDecl = (LavaDECL*)checlassEl->data;
  while (checlassEl) {
    if (classElDecl->TypeFlags.Contains(constituent)
       && (     (classElDecl->DeclType == Attr)
             || ((classElDecl->DeclType == VirtualType)
                    && (   classElDecl->SecondTFlags.Contains(isSet)
                        || classElDecl->SecondTFlags.Contains(isArray)
                       ))
          )  ) {
      inID = TID(classElDecl->OwnID, classElDecl->inINCL); 
      cheformEl = (CHE*)chain.first;
      found = false;
      while (cheformEl && !found) {
        formElDecl = (LavaDECL*)cheformEl->data;
        if (formElDecl->Supports.first) {
          fsupID = ((CHETID*)formElDecl->Supports.first)->data;
          fsupID.nINCL = IDTable.IDTab[formDECL->inINCL]->nINCLTrans[fsupID.nINCL].nINCL;
          found = (fsupID == inID);
        }
        if (!found) {
          if (formElDecl->DeclType == FormText) {
            cheTextEl = cheformEl;
            cheformEl = (CHE*)cheformEl->successor;
            cheTextEl = (CHE*)chain.Uncouple(cheTextEl);
            formDECL->NestedDecls.Append(cheTextEl);
          }
          else
            cheformEl = (CHE*)cheformEl->successor;
        }
      }
      if (cheformEl) {
        changed = changed || (chain.first != cheformEl);
        cheformEl = (CHE*)chain.Uncouple(cheformEl);
        formDECL->NestedDecls.Append(cheformEl);
        typeFlags = classElDecl->TypeFlags;
//        typeFlags.EXCL(Overrides);
        if (typeFlags != formElDecl->TypeFlags) {
          formElDecl->TypeFlags = typeFlags;
          changed = true;
        }
//      if (formElDecl->DeclDescType == NamedType) {
          err = CheckFormEl(formElDecl, classElDecl);
          if (err) {
            new CLavaError(&formElDecl->DECLError2, err);
            new CLavaError(&formDECL->DECLError1, err);
          }
//      }
      }
      else {//attr noch nicht in form
        if (!formDECL->inINCL) {
          changed = true;
          UpdateNo++;
          formElDecl = NewLavaDECL();
          *formElDecl = *classElDecl;
          formElDecl->DECLComment.Destroy();
          formElDecl->RefID.nINCL = IDTable.IDTab[classElDecl->inINCL]->nINCLTrans[classElDecl->RefID.nINCL].nINCL;
          cheformEl = NewCHE(formElDecl);
          formDECL->NestedDecls.Append(cheformEl);
          formElDecl->ParentDECL = formDECL;
          if (checkLevel >= CHLV_inUpdateLow) {
            cheID = new CHETID;
            cheID->data = TID(classElDecl->OwnID, classElDecl->inINCL);
            formElDecl->SecondTFlags.EXCL(overrides);
            formElDecl->WorkFlags.EXCL(selAfter);
            formElDecl->Supports.Destroy();
            formElDecl->Supports.Append(cheID);  //implements
            formElDecl->inINCL = 0;
            formElDecl->OwnID = -1;
            ((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm(formElDecl, classElDecl, mySynDef);
            if (checkLevel > CHLV_inUpdateLow) {
              UpdateNo++;
              IDTable.NewID((LavaDECL**)&cheformEl->data);
              formElDecl->WorkFlags.INCL(newTreeNode);
            }
            if (checkLevel > CHLV_inUpdateHigh) 
              modified = true;
          }
        }

      }
    }
    checlassEl = (CHE*)checlassEl->successor;
    if (checlassEl) 
      classElDecl = (LavaDECL*)checlassEl->data;

  }
  cheformEl = (CHE*)chain.first;
  while (cheformEl) {
    formElDecl = (LavaDECL*)cheformEl->data;
    if (formElDecl->DeclType == FormText) {
      cheTextEl = cheformEl;
      cheformEl = (CHE*)cheformEl->successor;
      cheTextEl = (CHE*)chain.Uncouple(cheTextEl);
      formDECL->NestedDecls.Append(cheTextEl);
    }
    else
      cheformEl = (CHE*)cheformEl->successor;
  }
  changed = changed || (chain.first != 0);
  cheformEl = (CHE*)chain.first;
  while (cheformEl && ((((LavaDECL*)cheformEl->data)->DeclType == Attr)
                       || (((LavaDECL*)cheformEl->data)->DeclType == VirtualType))) {
    IDTable.DeleteID(((LavaDECL*)cheformEl->data)->OwnID);
    changed = true;
    cheformEl = (CHE*)cheformEl->successor;
  }
//  chain.Destroy();
  return changed;
}


bool CLavaPEDoc::CheckFuncImpl(LavaDECL* funcDECL, int checkLevel, bool& changed)
{
  CHE *cheIOEl, *checlassIOEl, *che, *elFound;
  CHETID *cheid, *cheID;
  LavaDECL *IOEl, *oldIOEl, *classIOEl, *classFuncDECL =0;
  CHAINX chain;

  cheID = (CHETID*)funcDECL->Supports.first; //implements
  if (cheID) 
    classFuncDECL = IDTable.GetDECL(cheID->data, funcDECL->inINCL);
  if (!classFuncDECL)
    return false;
  funcDECL->TypeFlags.EXCL(isAbstract);
  funcDECL->TypeFlags.EXCL(isNative);
  SynFlags classTypeFlags = classFuncDECL->TypeFlags;
  SynFlags typeFlags = funcDECL->TypeFlags;
  if (typeFlags != classTypeFlags) {
    changed = true;
    funcDECL->TypeFlags = classTypeFlags;
    funcDECL->SecondTFlags.INCL(funcImpl);
  }
  if (checkLevel > CHLV_inUpdateLow) {
    chain.first = funcDECL->NestedDecls.first;
    chain.last = funcDECL->NestedDecls.last;
    funcDECL->NestedDecls.first = 0;
    funcDECL->NestedDecls.last = 0;
    funcDECL->NestedDecls = classFuncDECL->NestedDecls;
    cheIOEl = (CHE*)funcDECL->NestedDecls.first;
    while (cheIOEl) {
      if (((LavaDECL*)cheIOEl->data)->DeclDescType == ExecDesc) {
        che = (CHE*)cheIOEl->successor;
        funcDECL->NestedDecls.Delete(cheIOEl);
        cheIOEl = che;
      }
      else
        cheIOEl = (CHE*)cheIOEl->successor;
    }
  }
  cheIOEl = (CHE*)funcDECL->NestedDecls.first;
  checlassIOEl = (CHE*)classFuncDECL->NestedDecls.first;
  while (cheIOEl && checlassIOEl
         && (((LavaDECL*)checlassIOEl->data)->DeclDescType != ExecDesc)
         && (((LavaDECL*)cheIOEl->data)->DeclDescType != ExecDesc) ) {
    IOEl = (LavaDECL*)cheIOEl->data;
    classIOEl = (LavaDECL*)checlassIOEl->data;
    cheID = new CHETID;
    cheID->data = TID(classIOEl->OwnID, classIOEl->inINCL);
    IOEl->Supports.Destroy();
    IOEl->Supports.Append(cheID);  //implements
    IOEl->RefID.nINCL = IDTable.IDTab[IOEl->inINCL]->nINCLTrans[IOEl->RefID.nINCL].nINCL;
    IOEl->inINCL = 0;
    IOEl->TypeFlags = classIOEl->TypeFlags;
    IOEl->SecondTFlags.INCL(funcImpl);
    IOEl->SecondTFlags.EXCL(overrides);
    IOEl->WorkFlags.EXCL(selAfter);
    if (checkLevel > CHLV_inUpdateLow) {
      IOEl->ParentDECL = funcDECL;
      che = (CHE*)chain.first;
      elFound = 0;
      while (che && !elFound) {
        oldIOEl = (LavaDECL*)che->data;
        if ((oldIOEl->DeclType == IAttr) || (oldIOEl->DeclType == OAttr)) {
          cheid = (CHETID*)oldIOEl->Supports.first;
          if (cheid && (IOEl->OwnID == cheid->data.nID)) 
            elFound = che;
          else {
            che = (CHE*)che->successor;
            changed = true;
          }
        }
        else {
          che = (CHE*)che->successor;
          changed = true;
        }
      }
      if (elFound) {
        IOEl->OwnID = ((LavaDECL*)elFound->data)->OwnID;
        *((LavaDECL*)elFound->data) = *IOEl;
        IOEl = (LavaDECL*)elFound->data;
        elFound = (CHE*)chain.Uncouple(elFound);
        funcDECL->NestedDecls.Insert(cheIOEl->predecessor, elFound);
        che = (CHE*)funcDECL->NestedDecls.Uncouple(cheIOEl);
        delete che;
        cheIOEl = elFound;
        changed = changed || (IOEl->RefID.nID != classIOEl->RefID.nID);
        typeFlags = classIOEl->TypeFlags;
//        typeFlags.INCL(isFuncImpl);
//        typeFlags.EXCL(Overrides);
        changed = (IOEl->TypeFlags == typeFlags); //true; 20.05.01
        IOEl->TypeFlags = typeFlags;
        //IDTable.Change((LavaDECL**)&cheIOEl->data);
        //che = (CHE*)chain.Uncouple(elFound);
        //delete che;
      }
      else {
        changed = true;
        IDTable.NewID((LavaDECL**)&cheIOEl->data);
        ((LavaDECL*)cheIOEl->data)->WorkFlags.INCL(newTreeNode);
      }

    }
    cheIOEl = (CHE*)cheIOEl->successor;
    checlassIOEl = (CHE*)checlassIOEl->successor;
  }
  che = (CHE*)chain.first;
  while (che && ((((LavaDECL*)che->data)->DeclType == IAttr)
    || (((LavaDECL*)che->data)->DeclType == OAttr))) {
    IDTable.DeleteID(((LavaDECL*)che->data)->OwnID);
    changed = true;
    che = (CHE*)che->successor;
  }
  while (che) {
    cheIOEl = che;
    che = (CHE*)che->successor;
    cheIOEl = (CHE*)chain.Uncouple(cheIOEl);
    funcDECL->NestedDecls.Append(cheIOEl);
  }
  return true;
}

bool CLavaPEDoc::CheckImpl(LavaDECL* implDECL, int checkLevel)
{
  CHE *cheImplEl, *cheIOEl, *cheI=0, *che, *afterElem = 0;
  TID fID, supID;
  bool toImpl, changed = false, fchanged, found, hasForm = false;
  LavaDECL *implElDecl, *classElDecl, *classDecl, *formDecl;

  CHETID* cheIDI = (CHETID*)implDECL->Supports.first;
  if (!cheIDI)
    return false;
  classDecl = IDTable.GetDECL(cheIDI->data, implDECL->inINCL);
  if (!classDecl) {
    new CLavaError(&implDECL->DECLError1, &ERR_NoImplClass);
    return false;
  }
  if (classDecl->LocalName != implDECL->LocalName) {
    changed = true;
    implDECL->LocalName = classDecl->LocalName;
  }
  SynFlags typeFlag = implDECL->TypeFlags;
  if (classDecl->SecondTFlags.Contains(isChain))
    implDECL->SecondTFlags.INCL(isChain);
  else
    implDECL->SecondTFlags.EXCL(isChain);
  if (classDecl->SecondTFlags.Contains(isEnum))
    implDECL->SecondTFlags.INCL(isEnum);
  else
    implDECL->SecondTFlags.EXCL(isEnum);
  if (classDecl->TypeFlags.Contains(isAbstract))
    implDECL->TypeFlags.INCL(isAbstract);
  else
    implDECL->TypeFlags.EXCL(isAbstract);
  if (classDecl->SecondTFlags.Contains(isSet))
    implDECL->SecondTFlags.INCL(isSet);
  else
    implDECL->SecondTFlags.EXCL(isSet);
  if (classDecl->SecondTFlags.Contains(isArray))
    implDECL->SecondTFlags.INCL(isArray);
  else
    implDECL->SecondTFlags.EXCL(isArray);
  if (classDecl->SecondTFlags.Contains(isException))
    implDECL->SecondTFlags.INCL(isException);
  else
    implDECL->SecondTFlags.EXCL(isException);
  if (classDecl->TypeFlags.Contains(isGUI))
    implDECL->TypeFlags.INCL(isGUI);
  else
    implDECL->TypeFlags.EXCL(isGUI);
  changed = changed || (typeFlag != implDECL->TypeFlags);

  cheI = (CHE*)classDecl->NestedDecls.first;
  while (cheI) { 
    classElDecl = (LavaDECL*)cheI->data;
    toImpl = classElDecl && (classElDecl->DeclType == Function)
             && !classElDecl->TypeFlags.Contains(isAbstract)
             && !classElDecl->TypeFlags.Contains(isNative);
    if (toImpl) {
      fID = TID(classElDecl->OwnID, classElDecl->inINCL);
      cheImplEl = (CHE*)implDECL->NestedDecls.first;
      found = false;
      if (cheImplEl)
        implElDecl = (LavaDECL*)cheImplEl->data;
      while (cheImplEl && !found) {
        if ((implElDecl->DeclType == Function) 
             && implElDecl->Supports.first
             && implElDecl->SecondTFlags.Contains(funcImpl)) {
          supID = ((CHETID*)implElDecl->Supports.first)->data;
          supID.nINCL = IDTable.IDTab[implDECL->inINCL]->nINCLTrans[supID.nINCL].nINCL;
          found = (supID == fID);
        }
        if (!found) {
          cheImplEl = (CHE*)cheImplEl->successor;
          if (cheImplEl)
            implElDecl = (LavaDECL*)cheImplEl->data;
        }
      }
      if (!cheImplEl) { //function not found, then make it
        changed = true;
        UpdateNo++;
        implElDecl = NewLavaDECL();
        *implElDecl = *classElDecl;
        cheIOEl = (CHE*)implElDecl->NestedDecls.first;
        while (cheIOEl) {
          if ((((LavaDECL*)cheIOEl->data)->DeclType == Require) || (((LavaDECL*)cheIOEl->data)->DeclType == Ensure)) {     
            che = (CHE*)cheIOEl->successor;
            implElDecl->NestedDecls.Delete(cheIOEl);
            cheIOEl = che;
          }
          else
            cheIOEl = (CHE*)cheIOEl->successor;
        }

        implElDecl->SecondTFlags.INCL(funcImpl);
        implElDecl->DECLComment.Destroy();
        CHETID* cheID = new CHETID;
        cheID->data = fID;
        implElDecl->Supports.Destroy();
        implElDecl->Supports.Append(cheID);  //implements
        implElDecl->inINCL = 0; //must stay before CheckFuncImpl
        CheckFuncImpl(implElDecl, CHLV_inUpdateLow, fchanged);
//        implElDecl->inINCL = 0;
        implElDecl->OwnID = -1;
        implElDecl->RefID.nINCL = IDTable.IDTab[classElDecl->inINCL]->nINCLTrans[classElDecl->RefID.nINCL].nINCL;
        implElDecl->WorkFlags.INCL(checkmark);
        implElDecl->WorkFlags.EXCL(selAfter);
        implElDecl->SecondTFlags.EXCL(overrides);
        cheImplEl = NewCHE(implElDecl);
        if (implDECL->NestedDecls.last && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclDescType == ExecDesc)
          implDECL->NestedDecls.Insert(implDECL->NestedDecls.last->predecessor, cheImplEl);
        else
          implDECL->NestedDecls.Append(cheImplEl);
        GetExecDECL(implElDecl,ExecDef);
        implElDecl->ParentDECL = implDECL;
        if (checkLevel > CHLV_inUpdateLow) {
          UpdateNo++;
          IDTable.NewID((LavaDECL**)&cheImplEl->data);
          implElDecl->WorkFlags.INCL(newTreeNode);
        }
        if (checkLevel > CHLV_inUpdateHigh) 
          modified = true;
        if (checkLevel == CHLV_inUpdateHigh)
          implDECL->WorkFlags.INCL(newTreeNode);
      }
      else  {  
        fchanged = false;
        if (CheckFuncImpl((LavaDECL*)cheImplEl->data, checkLevel, fchanged))
          implElDecl->WorkFlags.INCL(checkmark);
        changed = changed || fchanged;
      }
    }
    cheI = (CHE*)cheI->successor;
  }
  bool sgs = MakeSetAndGets(implDECL, classDecl, checkLevel);
    //set and get functions
  changed = changed || sgs;
  //fchanged = checkImpls(implDECL, classDecl, checkLevel);

  changed = changed || fchanged;

  cheImplEl = (CHE*)implDECL->NestedDecls.first;
  while (cheImplEl) {
    implElDecl = (LavaDECL*)cheImplEl->data;
    if ((implElDecl->DeclType != Function)
      || implElDecl->WorkFlags.Contains(checkmark) ) {
      implElDecl->WorkFlags.EXCL(checkmark);
    }
    else {
      if (checkLevel == CHLV_fit) {
        changed = true;
        UpdateNo++;
        /*
        if (implElDecl->SecondTFlags.Contains(funcImpl) ) {
          if (implElDecl->TypeFlags.Contains(isPropGet) || implElDecl->TypeFlags.Contains(isPropSet)) {
            classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
            if (classElDecl && !classElDecl->TypeFlags.Contains(hasSetGet)) 
              new CLavaError(&implElDecl->DECLError2, &ERR_NoSetGetMember);
            else
              new CLavaError(&implElDecl->DECLError2, &ERR_MissingItfFuncDecl);
          }
          else {
            classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
            if (classElDecl) 
              new CLavaError(&implElDecl->DECLError2, &ERR_NoImplForAbstract);
            else
              new CLavaError(&implElDecl->DECLError2, &ERR_MissingItfFuncDecl);
          }
        }
        */
        implElDecl->SecondTFlags.EXCL(funcImpl);     
        implElDecl->TypeFlags.EXCL(isProtected);
        implElDecl->TypeFlags.EXCL(isPropGet);    
        implElDecl->TypeFlags.EXCL(isPropSet);
        implElDecl->TypeFlags.EXCL(isInitializer);
        implElDecl->TypeFlags.EXCL(defaultInitializer);
        implElDecl->Supports.Destroy();
        if (implElDecl->op != OP_noOp) {
          implElDecl->LocalName = DString("leaved_from_operator_") + LBaseData->OpFuncNames[((LavaDECL*)cheImplEl->data)->op];
          implElDecl->op = OP_noOp;
        }
        cheIOEl = (CHE*)((LavaDECL*)cheImplEl->data)->NestedDecls.first;
        while (cheIOEl) {
          ((LavaDECL*)cheIOEl->data)->SecondTFlags.EXCL(funcImpl);     
          ((LavaDECL*)cheIOEl->data)->TypeFlags.EXCL(isPropGet);    
          ((LavaDECL*)cheIOEl->data)->TypeFlags.EXCL(isPropSet);
          ((LavaDECL*)cheIOEl->data)->Supports.Destroy();
          cheIOEl = (CHE*)cheIOEl->successor;
        }
      }
      else
        if (implElDecl->SecondTFlags.Contains(funcImpl) ) {
          if (implElDecl->TypeFlags.Contains(isPropGet) || implElDecl->TypeFlags.Contains(isPropSet)) {
            classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
            if (classElDecl && !classElDecl->TypeFlags.Contains(hasSetGet)) 
              new CLavaError(&implElDecl->DECLError2, &ERR_NoSetGetMember);
            else
              if (checkLevel == CHLV_showError) 
                new CLavaError(&implElDecl->DECLError2, &ERR_MissingItfFuncDecl);
          }
          else {
            classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
            if (classElDecl) {
              if (checkLevel == CHLV_showError) 
                new CLavaError(&implElDecl->DECLError2, &ERR_NoImplForAbstract);
            }
            else
              if (checkLevel == CHLV_showError) 
                new CLavaError(&implElDecl->DECLError2, &ERR_MissingItfFuncDecl);
          }
        }
 
        /*
        if (implElDecl->TypeFlags.Contains(isPropGet) || implElDecl->TypeFlags.Contains(isPropSet)) {
          classElDecl = IDTable.GetDECL(((CHETID*)implElDecl->Supports.first)->data, implElDecl->inINCL);
          if (classElDecl) 
              new CLavaError(&implElDecl->DECLError1, &ERR_NoSetGetMember);
        }
        else
          if (checkLevel == CHLV_showError) 
            new CLavaError(&implElDecl->DECLError1, &ERR_MissingFuncDecl);
        */
    }
    cheImplEl = (CHE*)cheImplEl->successor;
  }
  if (implDECL->TypeFlags.Contains(isGUI)) {
    cheImplEl = (CHE*)implDECL->NestedDecls.first;
    while (cheImplEl && !hasForm) {
      if (((LavaDECL*)cheImplEl->data)->DeclType == VirtualType)
        afterElem = cheImplEl;
      else
        hasForm = ((LavaDECL*)cheImplEl->data)->DeclType == FormDef;
      cheImplEl = (CHE*)cheImplEl->successor;
    }
    if (!hasForm) {
      if (checkLevel >= CHLV_inUpdateLow) {
        changed = true;
        UpdateNo++;
        formDecl = NewLavaDECL();
        if (IDTable.GetDECL(implDECL->RefID, implDECL->inINCL)) {
          formDecl->DeclType = FormDef;
          formDecl->ParentDECL = implDECL;
          formDecl->LocalName = implDECL->LocalName + DString("_UI");
          formDecl->FullName = implDECL->FullName + pkt + formDecl->LocalName;
          formDecl->RefID = implDECL->RefID;
          CheckForm(formDecl, CHLV_inUpdateLow);
          che = NewCHE(formDecl);
          implDECL->NestedDecls.Insert(afterElem, che);
        }
        else {
          if (checkLevel == CHLV_showError) 
            new CLavaError(&implDECL->DECLError1, &ERR_NoExtForm);
        }
      }
      else {
        if (checkLevel == CHLV_showError) 
          new CLavaError(&implDECL->DECLError1, &ERR_NoExtForm);
      }
    }
  }
  
  return changed;
}


bool CLavaPEDoc::CheckMenu(LavaDECL* formDECL, LavaDECL* classDECL)
{
  bool changed = false;
  LavaDECL* inElClass = &((TEnumDescription*)classDECL->EnumDesc.ptr)->EnumField;
  LavaDECL* inElForm = &((TEnumDescription*)formDECL->EnumDesc.ptr)->EnumField;
  LavaDECL* menuTree = &((TEnumDescription*)formDECL->EnumDesc.ptr)->MenuTree;
  CHE* inDefEl = (CHE*)menuTree->NestedDecls.first;
  CHEEnumSelId* enumselClass = (CHEEnumSelId*)inElClass->Items.first;
  CHEEnumSelId* enumselForm = (CHEEnumSelId*)inElForm->Items.first;
  CHE* inDefEl0 = 0, *ininDefEl;
  CHEEnumSelId* enumselForm0 = 0;
  LavaDECL* Decl, *inDecl;
  bool newItem = false;

  while (enumselClass) { 
    if (enumselForm && (enumselForm->data.Id == enumselClass->data.Id)) {
      while (inDefEl && !((LavaDECL*)inDefEl->data)->LocalName.l) {
        inDefEl0 = inDefEl;
        inDefEl = (CHE*)inDefEl->successor;
      }
      if (inDefEl
        && (((LavaDECL*)inDefEl->data)->LocalName == enumselClass->data.Id)) {
        inDefEl0 = inDefEl;
        inDefEl = (CHE*)inDefEl->successor;
      }
    }
    else {
      changed = true;
      if (enumselForm) {
        for ( ;enumselForm && (enumselForm->data.Id != enumselClass->data.Id);
               enumselForm = (CHEEnumSelId*)enumselForm->successor);
        if (enumselForm) {
          enumselForm = (CHEEnumSelId*)inElForm->Items.Uncouple(enumselForm);
          inElForm->Items.Insert(enumselForm0, enumselForm);
          if (inDefEl) {
            inDefEl0 = (CHE*)inDefEl->predecessor;
            for ( ;inDefEl && (((LavaDECL*)inDefEl->data)->LocalName != enumselClass->data.Id);
                   inDefEl = (CHE*)inDefEl->successor);
            if (inDefEl) {
              inDefEl = (CHE*)menuTree->NestedDecls.Uncouple(inDefEl);
              menuTree->NestedDecls.Insert(inDefEl0, inDefEl);
            }
            if (inDefEl) {
              inDefEl0 = inDefEl;
              inDefEl = (CHE*)inDefEl->successor;
            }
            else
              inDefEl = (CHE*)inDefEl0->successor;
          } 
        }
        else
          newItem = true;
      }
      else
        newItem = true;
    }//else
    if (newItem) { //new item
      newItem = false;
      enumselForm = new CHEEnumSelId;
      inElForm->Items.Insert(enumselForm0, enumselForm);
      enumselForm->data.Id = enumselClass->data.Id;
      enumselForm->data.SelectionCode = enumselClass->data.Id;
      Decl = NewLavaDECL();
      Decl->Annotation.ptr = new TAnnotation;
      Decl->Annotation.ptr->FA.ptr = new TAnnotation;
      Decl->LocalName = enumselClass->data.Id;
      Decl->DeclType = Attr;
      Decl->DeclDescType = Undefined;
      inDefEl = NewCHE(Decl);
      menuTree->NestedDecls.Insert(inDefEl0, inDefEl);
      inDecl = NewLavaDECL();
      inDecl->Annotation.ptr = new TAnnotation;
      inDecl->Annotation.ptr->FA.ptr = new TAnnotation;
      inDecl->DeclDescType = LiteralString;
      inDecl->LitStr = enumselClass->data.Id;
      ininDefEl = NewCHE(inDecl);
      Decl->Annotation.ptr->Prefixes.Append(ininDefEl);
      LavaDECL* inDecl = NewLavaDECL();
      inDecl->Annotation.ptr = new TAnnotation;
      inDecl->Annotation.ptr->FA.ptr = new TAnnotation;
      inDecl->DeclDescType = LiteralString;
      inDecl->LitStr = enumselClass->data.Id;
      ininDefEl = NewCHE(inDecl);
      Decl->Annotation.ptr->Prefixes.Append(ininDefEl);
      inDefEl0 = inDefEl;
      inDefEl = (CHE*)inDefEl->successor;
    }
    enumselForm0 = enumselForm;
    enumselForm = (CHEEnumSelId*)enumselForm->successor;
    enumselClass = (CHEEnumSelId*)enumselClass->successor;
  }//while

  ChainAnyElem* pred;
  while (inDefEl) {
    if (((LavaDECL*)inDefEl->data)->LocalName.l) {
      changed = true;
      pred = inDefEl->predecessor;
      ((TEnumDescription*)formDECL->EnumDesc.ptr)->MenuTree.NestedDecls.Remove(pred);
      if (pred)
        inDefEl = (CHE*)pred->successor;
      else
        inDefEl = (CHE*)((TEnumDescription*)formDECL->EnumDesc.ptr)->MenuTree.NestedDecls.first;
    }
    else
      inDefEl = (CHE*)inDefEl->successor;
  }
  if (enumselForm) {
    pred = enumselForm->predecessor;
    while (enumselForm) {
      changed = true;
      inElForm->Items.Remove(pred);
      if (pred)
        enumselForm = (CHEEnumSelId*)pred->successor;
      else
        enumselForm = (CHEEnumSelId*)inElForm->Items.first;
    }
  }
  if (changed) {
    LavaDECL* inEl = (LavaDECL*)formDECL->Annotation.ptr->MenuDECL.ptr;
    if (inEl) {
      ((TAnnotation*)formDECL->Annotation.ptr->FA.ptr)->BasicFlags
          = ((TAnnotation*)inEl->Annotation.ptr->FA.ptr)->BasicFlags;
      formDECL->Annotation.ptr->MenuDECL.Destroy();
    }
  }
  return changed;
}


bool CLavaPEDoc::CheckOverInOut(LavaDECL* funcDECL, int checkLevel)
{
  SynFlags typeFlags;
  CHE *cheOverIO, *cheIO, *chenext;
  LavaDECL *IODECL, *OverFunc;
  CHETID *cheID, *cheOverID;
  CHAINX chain;
  
  if (!funcDECL->SecondTFlags.Contains(overrides))
    return false;
  if (!funcDECL->Supports.first) {
    if (checkLevel == CHLV_fit) {
      funcDECL->SecondTFlags.EXCL(overrides);
      UpdateNo++;
      return true;
    }
    else
      return false;
  }
  cheID = (CHETID*)funcDECL->Supports.first;
  OverFunc = IDTable.GetDECL(cheID->data, funcDECL->inINCL);
  while (!OverFunc && cheID) {
    cheID = (CHETID*)cheID->successor;
    if (cheID)
      OverFunc = IDTable.GetDECL(cheID->data, funcDECL->inINCL);
  }
  if (!OverFunc) {
    if (checkLevel == CHLV_fit) {
      funcDECL->Supports.Destroy();
      funcDECL->SecondTFlags.EXCL(overrides);
      UpdateNo++;
      return true;
    }
    else
      return false;
  }
  bool catErr, changed = false, found;
  /*
  if (funcDECL->TypeFlags.Contains(inheritsBody))
    if (OverFunc->TypeFlags.Contains(isNative))
      funcDECL->TypeFlags.INCL(isNative);
    else
      funcDECL->TypeFlags.EXCL(isNative);
  */
  if (!funcDECL->ParentDECL->TypeFlags.Contains(isAbstract)) 
    funcDECL->TypeFlags.EXCL(isAbstract);
  if (funcDECL->DeclType == Attr) {
    if (!OverFunc->TypeFlags.Contains(isConst) && funcDECL->TypeFlags.Contains(isConst)) {
      funcDECL->TypeFlags.EXCL(isConst);
      changed = true;
    }
  }
  else
    if (OverFunc->TypeFlags.Contains(isConst) && !funcDECL->TypeFlags.Contains(isConst)) {
      funcDECL->TypeFlags.INCL(isConst);
      changed = true;
    }
  if (!OverFunc->TypeFlags.Contains(isProtected) && funcDECL->TypeFlags.Contains(isProtected)) {
    funcDECL->TypeFlags.EXCL(isProtected);
    changed = true;
  }
  if (funcDECL->DeclType == Attr)
    return changed;

  cheOverIO = (CHE*)OverFunc->NestedDecls.first;
  if (checkLevel > CHLV_noCheck) {
    chain.first = funcDECL->NestedDecls.first;
    chain.last = funcDECL->NestedDecls.last;
    funcDECL->NestedDecls.first = 0;
    funcDECL->NestedDecls.last = 0;
    while (cheOverIO && (((LavaDECL*)cheOverIO->data)->DeclDescType != ExecDesc)) {
      cheID = 0;
      cheIO = 0;
      found = false;
      cheIO = (CHE*)chain.first;
      while (cheIO && !found && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
        for (cheID = (CHETID*)((LavaDECL*)cheIO->data)->Supports.first;
          cheID && (cheID->data != TID(((LavaDECL*)cheOverIO->data)->OwnID, ((LavaDECL*)cheOverIO->data)->inINCL));
          cheID = (CHETID*)cheID->successor);
        found = cheID != 0;
        if (!found)
          cheIO = (CHE*)cheIO->successor;
      }
      if (found) 
        cheIO = (CHE*)chain.Uncouple(cheIO);
      else {
        UpdateNo++;
        IODECL = NewLavaDECL();
        *IODECL = *(LavaDECL*)cheOverIO->data;
        IODECL->DECLComment.Destroy();
        IODECL->Supports.Destroy();
        IODECL->inINCL = 0;
        IODECL->OwnID = -1;
        if (IODECL->RefID.nID >= 0)
          IODECL->RefID.nINCL = IDTable.IDTab[OverFunc->inINCL]->nINCLTrans[IODECL->RefID.nINCL].nINCL;
        IODECL->ParentDECL = funcDECL;
        IODECL->SecondTFlags.INCL(overrides);
        IODECL->WorkFlags.EXCL(selAfter);
        cheIO = NewCHE(IODECL);
        if (checkLevel > CHLV_inUpdateLow) {
          IDTable.NewID((LavaDECL**)&cheIO->data);
          IODECL->WorkFlags.INCL(newTreeNode);
        }
        if (checkLevel > CHLV_inUpdateHigh) 
          modified = true;
        changed = true;
      }
      if (checkLevel > CHLV_inUpdateLow) {
        typeFlags = ((LavaDECL*)cheIO->data)->TypeFlags;
        GetCategoryFlags((LavaDECL*)cheIO->data, catErr);
        changed = changed || (typeFlags != ((LavaDECL*)cheIO->data)->TypeFlags);
      }
      if (cheIO)
        funcDECL->NestedDecls.Append(cheIO);
      cheOverIO = (CHE*)cheOverIO->successor;
    }
    cheIO = (CHE*)chain.first;
    while (cheIO) {
      if (((LavaDECL*)cheIO->data)->DeclDescType == ExecDesc) {
        chenext =(CHE*)cheIO->successor;
        cheIO = (CHE*)chain.Uncouple(cheIO);
        funcDECL->NestedDecls.Append(cheIO);
        cheIO = chenext;
      }
      else
        cheIO = (CHE*)cheIO->successor;
    }
   if (checkLevel > CHLV_inUpdateLow) {
      cheIO = (CHE*)chain.first;
      while (cheIO) {
        UpdateNo++;
        IDTable.DeleteID(((LavaDECL*)cheIO->data)->OwnID);
        changed = true;
        cheIO = (CHE*)cheIO->successor;
      }
    }
  }
  else {
    cheIO = (CHE*)funcDECL->NestedDecls.first;
    while (cheIO) {
      UpdateNo++;
      changed = true;
      ((LavaDECL*)cheIO->data)->Supports.Destroy();
      cheIO = (CHE*)cheIO->successor;
    }
  }
  cheOverID = (CHETID*)funcDECL->Supports.first;
  while (cheOverID) {
    OverFunc = IDTable.GetDECL(cheOverID->data, funcDECL->inINCL);
    if (OverFunc) {
      cheOverIO = (CHE*)OverFunc->NestedDecls.first;
      cheIO = (CHE*)funcDECL->NestedDecls.first;
      while (cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
        for (cheID = (CHETID*)((LavaDECL*)cheIO->data)->Supports.first;
          cheID && (cheID->data != TID(((LavaDECL*)cheOverIO->data)->OwnID, ((LavaDECL*)cheOverIO->data)->inINCL));
          cheID = (CHETID*)cheID->successor);
        if (!cheID) {
          cheID = new CHETID;
          cheID->data = TID(((LavaDECL*)cheOverIO->data)->OwnID, ((LavaDECL*)cheOverIO->data)->inINCL);
          ((LavaDECL*)cheIO->data)->Supports.Append(cheID);
          UpdateNo++;
          changed = true;
        }
        cheIO = (CHE*)cheIO->successor;
        cheOverIO = (CHE*)cheOverIO->successor;
      }
    }
    cheOverID = (CHETID*)cheOverID->successor;
  }
  cheIO = (CHE*)funcDECL->NestedDecls.first;
  while (cheIO && (((LavaDECL*)cheIO->data)->DeclDescType != ExecDesc)) {
    cheID = (CHETID*)((LavaDECL*)cheIO->data)->Supports.first;
    while (cheID) {
      IODECL = IDTable.GetDECL(cheID->data, funcDECL->inINCL);
      if (IODECL) {
        for (cheOverID = (CHETID*)funcDECL->Supports.first;
            cheOverID && (cheOverID->data != TID(IODECL->ParentDECL->OwnID, IODECL->inINCL));
            cheOverID = (CHETID*)cheOverID->successor);
      }
      else
        cheOverID = 0;
      if (!cheOverID) {
        cheOverID = (CHETID*)cheID->successor;
        ((LavaDECL*)cheIO->data)->Supports.Remove(cheID->predecessor);
        cheID = cheOverID;
        changed = true;
      }
      else
        cheID = (CHETID*)cheID->successor;
    }
    cheIO = (CHE*)cheIO->successor;
  }


        /*
        ((LavaDECL*)cheIO->data)->Supports.Destroy();
        while (cheOverIO 
          && ( ((LavaDECL*)cheOverIO->data)->LocalName != ((LavaDECL*)cheIO->data)->LocalName)) {
          cheOverIO = (CHE*)cheOverIO->successor;
          changed = true;
        }
        if (cheOverIO) {
          cheID = new CHETID;
          cheID->data = TID(((LavaDECL*)cheOverIO->data)->OwnID, ((LavaDECL*)cheOverIO->data)->inINCL);
          ((LavaDECL*)cheIO->data)->Supports.Append(cheID);
          UpdateNo++;
        }
        cheIO = (CHE*)cheIO->successor;
      }
    }
    cheOverID = (CHETID*)cheOverID->successor;
  }
  */
  return changed;
}

bool CLavaPEDoc::CollectP(const TIDs& paramIDs, LavaDECL* collectDECL)
{
  TIDs refIDs, baseParamIDs;
  CHETID *cheID, *cheIDbase, *cheIDVal;
  LavaDECL *ElDECL, *bDECL;

  cheID = (CHETID*)paramIDs.first;
  while (cheID) {
    ElDECL = IDTable.GetDECL(cheID->data);
    if (ElDECL) {
      ElDECL->WorkFlags.INCL(checkmark);
      ElDECL->ParentDECL->WorkFlags.INCL(checkmark);
      cheIDVal = new CHETID;
      cheIDVal->data =  TID(ElDECL->RefID.nID, IDTable.IDTab[ElDECL->inINCL]->nINCLTrans[ElDECL->RefID.nINCL].nINCL);
      refIDs.Append(cheIDVal);
      if (ElDECL->Supports.first) {
        cheIDbase = new CHETID;
        cheIDbase->data = ((CHETID*)ElDECL->Supports.first)->data;
        cheIDbase->data.nINCL = IDTable.IDTab[ElDECL->inINCL]->nINCLTrans[cheIDbase->data.nINCL].nINCL;
        baseParamIDs.Append(cheIDbase);
      }
    }
    cheID = (CHETID*)cheID->successor;
  }
  if (baseParamIDs.first)
    CollectP(baseParamIDs, 0);
  collectPattern(ElDECL->ParentDECL, paramIDs, refIDs);
  if (baseParamIDs.first) {
    bDECL = IDTable.GetDECL(((CHETID*)baseParamIDs.first)->data);
    if (bDECL)
      bDECL->ParentDECL->ResetCheckmarks();
  }
  if (collectDECL) {
//    RmDuplicates(collectDECL->ParentDECL);
    *collectDECL = *ElDECL->ParentDECL;
    collectDECL->DeclType = PatternDef;
    ShrinkCollectDECL(collectDECL);
    collectDECL->ResetCheckmarks();
    //paramDECL->ParentDECL->ResetCheckmarks();
    return collectDECL->NestedDecls.first != 0;
  }
  else 
    return true;
}

bool CLavaPEDoc::CollectPattern(LavaDECL *paramDECL, LavaDECL* collectDECL)
{
  TIDs paramIDs;
  CHETID  *cheID;
  CHE* cheEl;
  LavaDECL* ElDECL;

  if (paramDECL->DeclType == PatternDef) {
    cheEl = (CHE*)paramDECL->NestedDecls.first;
    while (cheEl) {
      ElDECL = (LavaDECL*)cheEl->data;
      cheID = new CHETID;
      cheID->data = TID(ElDECL->OwnID, ElDECL->inINCL);
      paramIDs.Append(cheID);
      cheEl = (CHE*)cheEl->successor;
    }
  }
  else {
    paramDECL->WorkFlags.INCL(checkmark);
    paramDECL->ParentDECL->WorkFlags.INCL(checkmark);
    cheID = new CHETID;
    cheID->data = TID(paramDECL->OwnID, paramDECL->inINCL);
    paramIDs.Append(cheID);
  }
  return CollectP(paramIDs, collectDECL);
}


bool CLavaPEDoc::collectPattern(LavaDECL *decl, const TIDs& paramIDs, const TIDs& refIDs)
{
  LavaDECL* elDECL /*, *baseElDECL -siehe unten*/;
  CHE *che = (CHE*)decl->NestedDecls.first;
  CHETID* cheID;
  TID id;
  bool inp, inPC = false;

  while (che) {
    elDECL = (LavaDECL*)che->data;
    inp = false;
    if (elDECL->DeclType == Interface) {
      //is this interface value of a virtual type in the collection of virtual types to be ovverridden 
      id =  TID(elDECL->OwnID, elDECL->inINCL);
      for (cheID = (CHETID*)refIDs.first; !inp && (cheID != 0); cheID = (CHETID*)cheID->successor)
        if (cheID->data == id)
          inp = true;
      if (collectPattern(elDECL, paramIDs, refIDs))
        inp = true;
    }
    else {
      if (elDECL->DeclType == Function) {
        if (elDECL->TypeFlags.Contains(forceOverride)) {
          elDECL->WorkFlags.INCL(checkmark);
          elDECL->Inherits.Destroy();
          inp = true;
        }
        else
          inp = collectPattern(elDECL, paramIDs, refIDs);
      }
    }
    if ((elDECL->DeclType == Interface) || (elDECL->DeclType == VirtualType)) {
    /*
    // wozu war das? -- probeweise wieder eingeklammert!!
      if (!inp && elDECL->Supports.first) { //!
        baseElDECL = IDTable.GetFinalDef(((CHETID*)elDECL->Supports.first)->data, elDECL->inINCL);
        inp = (baseElDECL && baseElDECL->WorkFlags.Contains(checkmark));
      }
    */
      if (inp) 
        elDECL->WorkFlags.INCL(checkmark);
    }
    inPC = inPC || inp;
    che = (CHE*)che->successor;
  }
  return inPC;
}

void CLavaPEDoc::ConcernExecs(CLavaPEHint* hint)
{
  TDeclType defType = ((LavaDECL*)hint->CommandData1)->DeclType;
  if ((hint->com != CPECommand_Insert) || ((defType == IAttr) || (defType == OAttr))) {
    CLavaBaseView* view;
    POSITION pos = GetFirstViewPos();
    CheckData chd;
    chd.concernExecs = true;
    while (pos) {
      view = (CLavaBaseView*)GetNextView(pos);
      if (view->inherits("CExecView")) {
        chd.document = this;
        chd.myDECL = ((CExecView*)view)->myDECL;
        chd.execView = view;
        chd.hint = hint;
        ((SynObject*)chd.myDECL->Exec.ptr)->Check(chd);
      }
    }
  }
}

void CLavaPEDoc::ConcernForms(CLavaPEHint* hint)
{
  LavaDECL *hintDECL = (LavaDECL*)hint->CommandData1;
  bool getFromMem = ((hintDECL->DeclType == DragFeature) 
     || (hintDECL->DeclType == DragFeatureF) 
     || (hintDECL->DeclType == DragParam) 
     || (hintDECL->DeclType == PatternDef) 
     || (hintDECL->DeclType == DragIO)
     || (hintDECL->DeclType ==  DragDef));
  CLavaPEHint* actHint, * lastHint;
  int pos = -1;
  if (getFromMem)
    lastHint = UndoMem.GetLastHint();
  else
    lastHint = hint;
  do {
    if (getFromMem)
      actHint = UndoMem.DoFromMem(pos);
    else
      actHint = hint;
    hintDECL = (LavaDECL*)actHint->CommandData1;
    if ( ( hintDECL->DeclType == Attr) && ( hintDECL->DeclDescType != Undefined)
         || ( hintDECL->DeclType == VirtualType)
            && (hintDECL->SecondTFlags.Contains(isSet) || hintDECL->SecondTFlags.Contains(isArray) )
         || ( hintDECL->DeclType == Interface) && (actHint->com != CPECommand_Insert) ) {
      CExecForms *fcf = new CExecForms(mySynDef, actHint);
      delete fcf;
    }
  } while (actHint != lastHint);
}


void CLavaPEDoc::ConcernImpls(CLavaPEHint* hint, LavaDECL* ppDECL, bool )
{  //extensions and implementations of the changed interface
  TID ifaceID, funcID;
  CExecOverrides *fcfo = 0;
  CExecImpls *fcf = 0;
  CLavaPEHint *actHint, *lastHint;
  LavaDECL *dragdropParent, *pppDECL, *hintDECL;
  bool execO, getFromMem;
  int pos = -1;

  ifaceID = TID(-1, 0);
  funcID = TID(-1, 0);
  hintDECL = (LavaDECL*)hint->CommandData1;
  getFromMem = ((hintDECL->DeclType == DragFeature) 
     || (hintDECL->DeclType == DragFeatureF) 
     || (hintDECL->DeclType == DragParam) 
     || (hintDECL->DeclType == PatternDef) 
     || (hintDECL->DeclType == DragIO)
     || (hintDECL->DeclType ==  DragDef));
//    get the hints from UndoMem
  if (getFromMem)
    lastHint = UndoMem.GetLastHint();
  else
    lastHint = hint;
  do {
    if (getFromMem)
      actHint = UndoMem.DoFromMem(pos);
    else
      actHint = hint;
    hintDECL = (LavaDECL*)actHint->CommandData1;
      
    execO = (actHint->com != CPECommand_Delete)
             || (hintDECL->DeclType != Interface)
                && (hintDECL->DeclType != Function) 
                && (hintDECL->DeclType != Attr) 
             || (actHint->CommandData5 == 0); //not from drag
    if (ppDECL &&
       (  ( (hintDECL->DeclType == Function)  || (hintDECL->DeclType == Attr))
           && (ppDECL->DeclType == Interface) 
       || (hintDECL->DeclType == VirtualType)
       || (hintDECL->DeclType == Attr) && hintDECL->TypeFlags.Contains(hasSetGet)
                                      && (ppDECL->DeclType == Impl) ) ) {
      dragdropParent = (LavaDECL*)actHint->CommandData5;
      if (!dragdropParent || (hintDECL->ParentDECL != dragdropParent)) {
        funcID.nID = hintDECL->OwnID;
        ifaceID.nID = ppDECL->OwnID;
        fcf = new CExecImpls(mySynDef, ifaceID, funcID, actHint);
        if (execO) 
          fcfo = new CExecOverrides(mySynDef, ifaceID, funcID, actHint);
      }
    }
    else {
      if (ppDECL && ((hintDECL->DeclType == IAttr) || (hintDECL->DeclType == OAttr))
          && ( ppDECL->DeclType == Function)  && ppDECL->ParentDECL) {
        pppDECL = ppDECL->ParentDECL;
        if (pppDECL->DeclType == Interface) { 
          funcID.nID = ppDECL->OwnID;
          ifaceID.nID = pppDECL->OwnID;
//          fcf = new CExecImpls(mySynDef, ifaceID, funcID, actHint);
          if (execO) 
            fcfo = new CExecOverrides(mySynDef, ifaceID, funcID, actHint);
          fcf = new CExecImpls(mySynDef, ifaceID, funcID, actHint);//hier hin 10.11.2000
          //Erinnerung: erst Overrides dann Impls, weil sonst die Implementation einer
          //überschreibenden Funktion nicht angepasst wird
        }
      }
      else
        if ((actHint->com != CPECommand_Insert)
          && (hintDECL->DeclType == Interface) ) { 
          ifaceID.nID = hintDECL->OwnID;
          fcf = new CExecImpls(mySynDef, ifaceID, funcID, actHint);
          if (execO) 
            fcfo = new CExecOverrides(mySynDef, ifaceID, funcID, actHint);
        }
    }
    if (fcf)
      delete fcf;
    if (fcfo)
      delete fcfo;
    fcf = 0;
    fcfo =0;
  } while (actHint != lastHint);
}

void CLavaPEDoc::DownFind(LavaDECL* decl, CFindData& fw) 
{
  CHE *inCheEl;
  CHETID *che;
  CSearchData sData;
  CHEEnumSelId *enumsel;
  LavaDECL *fDecl;
  TID mainRefTid, secondID;

  mainRefTid = fw.refTid;
  if (fw.index) { //by name
    if (decl->LocalName.l && decl->LocalName.StringMatch(fw.searchName, fw.FindRefFlags.Contains(matchCase), fw.FindRefFlags.Contains(wholeWord)))
      SetNameText(decl, fw);
    if (decl->DeclDescType == EnumType) {
      for (enumsel = (CHEEnumSelId*)((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Items.first;
           enumsel;
           enumsel = (CHEEnumSelId*)enumsel->successor)
        if (enumsel->data.Id.StringMatch(fw.searchName, fw.FindRefFlags.Contains(matchCase), fw.FindRefFlags.Contains(wholeWord))) {
          fw.enumID = enumsel->data.Id;
          SetNameText(decl, fw);
          fw.enumID.Reset(0);
        }
    }
  }
  else { //find references
    if (!fw.refTid.nINCL && (fw.refTid.nID == decl->OwnID))
      if (fw.enumID.l) {
        fw.refCase = 5;
        SetFindText(decl, fw);
      }
      else {
        fw.refCase = 4;
        SetFindText(decl, fw);
      }
    if (!fw.enumID.l) {
      if (fw.FindRefFlags.Contains(derivRefs)) {
        for (che = (CHETID*)decl->Supports.first; che; che = (CHETID*)che->successor) 
          if (che->data == fw.refTid) {
            fw.refCase = 1;
            SetFindText(decl, fw);
          }
      }
      if (fw.FindRefFlags.Contains(derivRefs) && fw.FindRefFlags.Contains(readRefs) && fw.FindRefFlags.Contains(writeRefs)) {
        if ((decl->DeclType != FormDef) && (decl->RefID == fw.refTid)) {
          fw.refCase = 0;
          SetFindText(decl, fw);
        }
        else { //check UI-references
          if ((decl->DeclType == Attr) && (decl->ParentDECL->DeclType == FormDef)) {
            if (secondID.nID < 0)
              secondID = FindUIReference(decl, fw);
            if ((secondID.nID > 0) && (secondID == decl->RefID)) {
              fw.refTid = secondID;
              fw.refCase = 0;
              SetFindText(decl, fw);
              fw.refTid = mainRefTid;
            }
            else {
              
              fDecl = IDTable.GetDECL(decl->RefID, decl->inINCL);
              if (fDecl->DeclType == FormDef) {
                fDecl = IDTable.GetDECL(((CHETID*)fDecl->ParentDECL->Supports.first)->data,fDecl->inINCL);
                if (TID(fDecl->OwnID, fDecl->inINCL) == fw.refTid) {
                  fw.refCase = 0;
                  secondID = decl->RefID;
                  fw.refTid = secondID;
                  SetFindText(decl, fw);
                  fw.refTid = mainRefTid;
                }
              }
            }
          }
        }
        for (che = (CHETID*)decl->Inherits.first; che; che = (CHETID*)che->successor) 
          if (che->data == fw.refTid) {
            fw.refCase = 2;
            SetFindText(decl, fw); 
          }
        if ((decl->DeclType == VirtualType) && (decl->ParentDECL->DeclType == FormDef)
            && decl->Annotation.ptr
            && decl->Annotation.ptr->IterOrig.ptr
            && ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
          LavaDECL* CHEEl = ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
          if (CHEEl->RefID == fw.refTid) {
            fw.refCase = 3;
            SetFindText(decl, fw); 
          }
          else {
            if (secondID.nID < 0)
              secondID = FindUIReference(decl, fw);
            if ((secondID.nID > 0) && (secondID == CHEEl->RefID)) {
              fw.refTid = secondID;
              fw.refCase = 3;
              SetFindText(decl, fw);
              fw.refTid = mainRefTid;
            }

          }
        }
      }
    }
  }
  if ((decl->DeclDescType == StructDesc) || (decl->DeclDescType == EnumType)) {
    inCheEl = (CHE*)decl->NestedDecls.first;
    while (inCheEl) {
      if (((LavaDECL*)inCheEl->data)->DeclDescType != ExecDesc) 
          DownFind((LavaDECL*)inCheEl->data, fw); //absFileName, refTid, enumID, fw);
      else {
        if ( fw.index || fw.FindRefFlags.Contains(readRefs) 
              || fw.FindRefFlags.Contains(writeRefs)) { //Execs
          sData.execDECL = (LavaDECL*)inCheEl->data;
          sData.doc = this;
          sData.findRefs = fw;
          sData.finished = false;
          ((SynObjectBase*)((LavaDECL*)inCheEl->data)->Exec.ptr)->MakeTable((address)&IDTable, ((LavaDECL*)inCheEl->data)->inINCL, (SynObjectBase*)inCheEl->data, onSearch, 0,0, (address)&sData);
        }
      }
      inCheEl = (CHE*)inCheEl->successor;
    }
  }
}

bool CLavaPEDoc::ErrorPageVisible()
{
  return  !((CLavaMainFrame*)wxTheApp->m_appWindow)->UtilitiesHidden
          && (((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->ActTab == tabError);
}

void CLavaPEDoc::SetCom8()
{
  UndoMem.SetCom8();
}


void CLavaPEDoc::ExecViewPrivToPub(LavaDECL* func, int delID)
{
  //1.PrivToPub: func is the new public func impl, func->Supports.first contains id of the old private func impl
  //2.PubToPriv: func is the new private func, func->RefID contains id of the old public func impl
  //             
  POSITION pos = GetFirstViewPos();
  bool active=false;
  wxView *view;
  TID oldID = TID(delID,0);
  while (pos) {
    view = (CLavaBaseView*)GetNextView(pos);
    active = view->inherits("CExecView") && (((CExecView*)view)->myID == oldID);
    if (active)
      ((CExecView*)view)->myID = TID(func->OwnID,0);
  }
  ViewPosRelease(pos);
}

wxDocument* CLavaPEDoc::FindOpenDoc(const DString& fn)
{
  POSITION pos; 
  wxDocManager* mana = wxDocManager::GetDocumentManager();
  pos = mana->GetFirstDocPos();
  CLavaPEDoc* doc;
  DString absName;
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    if (doc != this) {
      absName  = doc->IDTable.IDTab[0]->FileName;
      AbsPathName(absName, doc->IDTable.DocDir);
      if (SameFile(absName, fn)) {
        mana->DocPosRelease(pos);
        return doc;
      }
    }
  }
  return 0;
}

void CLavaPEDoc::FindReferences(DString& allNames, CFindData& fw)
{
  unsigned pos, pp;
//  DString absFileName;
  bool found;
//  TID refID;
  int ii;
  
  //refID.nID = id.nID;
  TID id = fw.refTid;
  CHESimpleSyntax *simpleSyntax = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
  while (simpleSyntax) {
    if (simpleSyntax->data.TopDef.ptr) {
      found = false;
      fw.fname = IDTable.IDTab[simpleSyntax->data.nINCL]->FileName;
      AbsPathName(fw.fname, IDTable.DocDir);
      for (ii = 0; (ii < IDTable.IDTab[simpleSyntax->data.nINCL]->maxTrans)
                   && (!IDTable.IDTab[simpleSyntax->data.nINCL]->nINCLTrans[ii].isValid
                       || (IDTable.IDTab[simpleSyntax->data.nINCL]->nINCLTrans[ii].nINCL != id.nINCL)); ii++);
      fw.refTid.nINCL = ii;
      pp = 0;
      while ((pp < allNames.l) && !found) {
        if (allNames.Contains (fw.fname,pp,pos)) {
          found = (allNames[pos-1] == ',') && (allNames[pos+fw.fname.l] == ',');
          if (!found)
            pp = pos + fw.fname.l;
        }
        else
          pp = allNames.l;
      }
      if (!found) {
        allNames = allNames + fw.fname + komma;
        DownFind(simpleSyntax->data.TopDef.ptr, fw);//absFileName, refID, enumID, fw);
      }
    }
    if (fw.FWhere == findInIncl)
      simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor;
    else
      simpleSyntax = 0;
  }
}

TID CLavaPEDoc::FindUIReference(LavaDECL* decl, CFindData& fw) 
{
  TID secondID;
  LavaDECL* fDecl = IDTable.GetDECL(decl->RefID, decl->inINCL);
  if (fDecl->DeclType == FormDef) {
    if (TID(fDecl->ParentDECL->OwnID, fDecl->inINCL) == fw.refTid) 
      return decl->RefID;
    fDecl = IDTable.GetDECL(((CHETID*)fDecl->ParentDECL->Supports.first)->data,fDecl->inINCL);
    if (TID(fDecl->OwnID, fDecl->inINCL) == fw.refTid) {
      return decl->RefID;
    }
  }
  return secondID;
}

CHE* CLavaPEDoc::GetExecChe(LavaDECL* parentDecl,TDeclType type, bool makeIt)
{
  LavaDECL *cDECL = 0;
  CHE *che, *afterChe = (CHE*)parentDecl->NestedDecls.last;
  if (afterChe) {
    cDECL = (LavaDECL*)afterChe->data;
    if (cDECL->DeclType != type) {
      if ((cDECL->DeclType == ExecDef)
        || (cDECL->DeclType == Ensure) && (type == Require)) {
        afterChe = (CHE*)afterChe->predecessor; 
        if (afterChe) {
          cDECL = (LavaDECL*)afterChe->data;
          if (cDECL->DeclType != type) {
            if (cDECL->DeclType == Ensure) {
              afterChe = (CHE*)afterChe->predecessor; 
              if (afterChe) {
                cDECL = (LavaDECL*)afterChe->data;
                if (cDECL->DeclType != type)
                  cDECL = 0;
              }
              else
                cDECL = 0;
            }
            else
              cDECL = 0;
          }//else ok
        }
        else
          cDECL = 0;
      }
      else
        cDECL = 0;
    }//else ok
  }
  if (!cDECL) {
    if (makeIt) {
      cDECL = NewLavaDECL();
      cDECL->DeclType = type;
      cDECL->DeclDescType = ExecDesc;
      cDECL->FullName = parentDecl->FullName;
      cDECL->ParentDECL = parentDecl;
      che = NewCHE(cDECL);
      parentDecl->NestedDecls.Insert(afterChe, che);
      ((CLavaPEApp*)wxTheApp)->ExecUpdate.MakeExec(cDECL);
      if (parentDecl->OwnID != -1)
        ((SynObjectBase*)cDECL->Exec.ptr)->MakeTable((address)&IDTable, parentDecl->inINCL, (SynObjectBase*)cDECL, onNewID);
      return che;
    }
    else
      return 0;
  }
  else
    return afterChe;
}

/*
DString CLavaPEDoc::GetIOLabel(TAnnotation *anno)
{
  QString cstr;
  int ids;
  if (anno && anno->IoSigFlags.Contains(DONTPUT))
    ids = IDS_DONTPUT;
  else
    if (anno && (anno->Emphasis == NoEcho))
      if (anno->IoSigFlags.Contains(UnprotectedUser) && anno->IoSigFlags.Contains(UnprotectedProg))
        ids = IDS_NoEchoIO;
      else
        if (anno->IoSigFlags.Contains(UnprotectedUser))
          ids = IDS_NoEchoI;
        else
          if (anno->IoSigFlags.Contains(UnprotectedProg))
            ids = IDS_NoEchoO;
          else
            ids = IDS_NoEchoDefault;
    else
      if (anno && anno->IoSigFlags.Contains(UnprotectedUser) && anno->IoSigFlags.Contains(UnprotectedProg))
        ids = IDS_EchoIO;
      else
        if (anno && anno->IoSigFlags.Contains(UnprotectedUser))
          ids = IDS_EchoI;
        else
          if (anno && anno->IoSigFlags.Contains(UnprotectedProg))
            ids = IDS_EchoO;
          else
            ids = IDS_EchoDefault;
  cstr.LoadString(ids);
  return DString(cstr);
}
*/

bool CLavaPEDoc::GetOperatorID(LavaDECL* decl, TOperator op, TID& ElID)
{
//  ResetVElems(decl);
  if (!decl)
    return 0;
  MakeVElems(decl);
  return getOperatorID(decl, op, ElID);
}

bool CLavaPEDoc::getOperatorID(LavaDECL* decl, TOperator op, TID& ElID)
{
  //ResetVElems(decl); call Reset and Make before calling GetOperatorID 
  //MakeVElems(decl);
  CHETVElem *El;
  for (El = (CHETVElem*)decl->VElems.VElems.first;
       El && (El->data.op != op);
       El = (CHETVElem*)El->successor);
  if (El) {
    ElID = El->data.VTEl;
    return true;
  }
  return false;
}

DString CLavaPEDoc::GetTypeLabel(LavaDECL* elDef, bool goDown)
{ 
  DString lab;
  QString cstr;
  LavaDECL* decl;
  lab.Reset(0);
  if (elDef->DeclType == FormDef) {
    lab = DString("Form");
    if (goDown) {
      decl = CheckGetFinalMType(elDef);
      if (decl)
        lab = lab + DString(" ") + decl->FullName;
      else
        lab = lab + DString(" ??");
    }
    return lab;
  }

  switch (elDef->DeclDescType) {
    case StructDesc:
      break;
    case EnumType:
//      lab = DString("Enumeration");
      break;
    case NamedType:
    case BasicType:
      if (elDef->DeclDescType == BasicType)
        lab += ((CLavaPEApp*)wxTheApp)->LBaseData.BasicNames[elDef->BType];
      else {
//        decl = IDTable.GetDECL(elDef->RefID, elDef->inINCL);
        decl = CheckGetFinalMType(elDef);
        if (decl)
          if (decl->DeclType == VirtualType)
            lab = lab + lthen + decl->LocalName + grthen; 
          else
            if (decl->DeclType == FormDef)
              lab += decl->ParentDECL->FullName;
            else
              lab += decl->FullName;
        else {
          lab += DString("??");
          new CLavaError(&elDef->DECLError1, &ERR_NoRefType);
        }
      }
      break; 
    case LiteralString:
      lab = DString("Literal");
      break;
    case ExecDesc:
      return DString("Invariant");
    default: 
      lab.Reset(0);
  }
  return lab;
}

bool CLavaPEDoc::IsSpecialOf(LavaDECL* decl, const TIDs& basicIDs, bool cDeriv)
{  //c-derivations only
  TID id;
  bool ok;
  CHETID* che;
  CContext con;
  LavaDECL *idecl, *baseDECL;

  id = TID(decl->OwnID, decl->inINCL);
  che = (CHETID*)basicIDs.first;
  ok = true;
  while (che && ok) {
    ok = IDTable.IsAn(id, 0, che->data, 0);
    if (ok && cDeriv) {
      baseDECL = IDTable.GetDECL(che->data);
      IDTable.GetPattern(baseDECL, con);
      if (baseDECL && con.oContext)
        ok = decl == baseDECL;
      //else
      if (!ok) 
        ok = IsCDerivation(decl, baseDECL);     
    }
    che = (CHETID*)che->successor;
  }
  if (decl->DeclType == VirtualType) {
    idecl = decl;
    while (idecl && (idecl->DeclType == VirtualType)
           && !idecl->TypeFlags.Contains(isAbstract)) {
      idecl = IDTable.GetDECL(idecl->RefID, idecl->inINCL);
      if (!idecl)
        return false;
      id = TID(idecl->OwnID, idecl->inINCL);
      che = (CHETID*)basicIDs.first;
      ok = true;
      while (che && ok) {
        ok = IDTable.IsAn(id, 0, che->data, 0);
        if (ok && cDeriv) {
          baseDECL = IDTable.GetDECL(che->data);
          IDTable.GetPattern(baseDECL, con);
          if (con.oContext)
            ok = decl == baseDECL;
          else 
            ok = IsCDerivation(decl, baseDECL);
        }
        che = (CHETID*)che->successor;
      }
    } 
    return false;
  }
  else
    return ok;
}

bool CLavaPEDoc::MakeFormVT(LavaDECL *decl, CheckData* )
{
  if (decl->VElems.UpdateNo > UpdateNo)
    return true;
  decl->VElems.UpdateNo = UpdateNo+1;
  ResetVElems(decl);
  LavaDECL **PbaseFDECL, *guibaseDECL, *classDECL;
  bool ok=true;
  CHETID* cheID;
  CHETVElem *El, *Elbase;
  CheckForm(decl, CHLV_inUpdateLow);
  if (decl->DeclType == FormDef) {  
    cheID = (CHETID*)decl->Supports.first;
    while (cheID) {
      guibaseDECL = IDTable.GetDECL(cheID->data, decl->inINCL);
      if (!guibaseDECL)
        return false;
      if (guibaseDECL->TypeFlags.Contains(isGUI)) {
        PbaseFDECL = GetFormpDECL(guibaseDECL);
        if (PbaseFDECL && *PbaseFDECL) {
          ok = MakeFormVT(*PbaseFDECL);
          Elbase = (CHETVElem*)(*PbaseFDECL)->VElems.VElems.first;
          while (Elbase) {
            for (El = (CHETVElem*)decl->VElems.VElems.first;
                 El && (El->data.VTEl != Elbase->data.VTEl);
                 El = (CHETVElem*)El->successor);
            if (El) {
              if (El->data.VTClss != Elbase->data.VTClss)
                El->data.ok = false;
            }
            else {
              El = new CHETVElem;
              El->data.VTClss = Elbase->data.VTClss;
              El->data.VTEl = Elbase->data.VTEl; 
              El->data.TypeFlags = Elbase->data.TypeFlags;
              El->data.ok = Elbase->data.ok;
              decl->VElems.VElems.Append(El);
            }
            Elbase = (CHETVElem*)Elbase->successor;
          }
        }
      }
      cheID = (CHETID*)cheID->successor;
    }
  }

  classDECL = IDTable.GetDECL(decl->RefID, decl->inINCL);
  if (classDECL) {
    El = new CHETVElem;
    El->data.VTClss = TID( decl->OwnID, decl->inINCL);
    El->data.VTEl = TID( classDECL->OwnID, classDECL->inINCL); 
    El->data.TypeFlags = classDECL->TypeFlags;
    decl->VElems.VElems.Append(El);
  }
  else
    return false;
  for (El = (CHETVElem*)decl->VElems.VElems.first;
       El && El->data.ok;
       El = (CHETVElem*)El->successor);
  return  (El == 0) && ok;
}

int CLavaPEDoc::MakeFunc(LavaDECL* idecl, bool otherDoc, QWidget* parent) 
{
  SynFlags first = (const unsigned long)1;
  LavaDECL *funcDecl;
  CLavaPEHint *hint;
  DWORD d4;
  DString *str2 = 0;
  int pos;
  TIDType type;
  bool b=false;

  funcDecl = NewLavaDECL();
  funcDecl->TreeFlags.INCL(hasEmptyOpt);
  funcDecl->TreeFlags.INCL(isExpanded);
  funcDecl->DeclType = Function;
  funcDecl->ParentDECL = idecl;
  funcDecl->FullName = idecl->FullName;
  funcDecl->DeclDescType = StructDesc;
  funcDecl->ParentDECL->TreeFlags.INCL(MemsExpanded);
  
  if (CallBox(funcDecl, 0, this, true, b, parent) != QDialog::Accepted) {
    delete funcDecl;
    return -1;
  }
  d4 = IDTable.GetVar(TID(idecl->OwnID, 0), type);
  pos = idecl->GetAppendPos(Function);
  str2 = new DString(idecl->FullName);
  hint = new CLavaPEHint(CPECommand_Insert, this, first, (DWORD) funcDecl, (DWORD)str2, (DWORD)pos, d4);
  UndoMem.AddToMem(hint);
  UpdateDoc(0, FALSE, hint);
  if (d4 && (idecl->DeclType == Interface)) 
    ConcernImpls(hint, *(LavaDECL**)d4);
  ConcernExecs(hint);
  if (otherDoc)
    SetLastHint();
  return funcDecl->OwnID;
}


LavaDECL* CLavaPEDoc::MakeGUI(LavaDECL* relDECL, LavaDECL** pparent, int& pos, LavaDECL* posdecl)

//Called from makeGUI-toolbutton handler:
//                 makes GUI interface and implementation from relDECL,
//                 which is an interface if called from main tree  or from its own
//                 and is an attribute or virtual type if called from a GUIView or from its own,
//always returns the GUIinterface.
{
  LavaDECL *GUIinterface=0, *GUIimpl=0, *interdecl, *attrdecl,
             *newAttrdecl, *FormDecl, *posDECL=0;
  QString valNewName;
  DString *name;
  int startpos = pos, implPos;
  CLavaPEHint *hint;
  CHE *elChe;
  CHETID *cheID, *cheFID;
  SynFlags firstlast;

  if (posdecl)
    posDECL = posdecl;
  if (!startpos)
    firstlast.INCL(firstHint);
  if (relDECL->DeclType == Interface) {
    if (relDECL->TypeFlags.Contains(isGUI))
      return relDECL;
    if (isStd || (relDECL->inINCL == 1))
      return 0;
    interdecl = relDECL;
    if (!posdecl)
      posDECL = interdecl;
    attrdecl = 0;
  }
  else {
    attrdecl = relDECL;
    interdecl = IDTable.GetDECL(relDECL->RefID, relDECL->inINCL);
    if (!posdecl)
      posDECL = attrdecl->ParentDECL;
    if (startpos && (interdecl->DeclType == FormDef))
      return interdecl;
    if (!startpos && ((CLavaPEApp*)wxTheApp)->Browser.findAnyForm(relDECL, relDECL->RefID, mySynDef)) {
      GUIimpl = IDTable.GetDECL(relDECL->RefID, relDECL->inINCL)->ParentDECL;
      GUIinterface = IDTable.GetDECL(((CHETID*)GUIimpl->Supports.first)->data);
      return GUIinterface;
    }
    if (interdecl->DeclType == FormDef)
      interdecl = IDTable.GetDECL(interdecl->RefID, interdecl->inINCL);
    if (isStd || (interdecl->inINCL == 1))
      return 0;
  }
  GUIinterface = NewLavaDECL();
  GUIinterface->DeclType = Interface;
  GUIinterface->DeclDescType = StructDesc;
  GUIimpl = NewLavaDECL();
  GUIimpl->DeclType = Impl;
  GUIimpl->DeclDescType = StructDesc;
  GUIinterface->RefID = TID(interdecl->OwnID, interdecl->inINCL);
  GUIinterface->ParentDECL = posDECL->ParentDECL;
  if (!startpos) {
    elChe = (CHE*)posDECL->ParentDECL->NestedDecls.first;
    pos = 2;
    while (elChe && (posDECL != (LavaDECL*)elChe->data)) {
      if (((LavaDECL*)elChe->data)->DeclType != VirtualType)
        pos++;
      elChe = (CHE*) elChe->successor;
    }
  }
  GUIimpl->RefID = GUIinterface->RefID;
  valNewName = QString("GUI_of_") + QString(interdecl->LocalName.c);
  while (CheckNewName(valNewName, GUIinterface, this)) 
    valNewName.insert(6,'_');
  GUIimpl->ParentDECL = GUIinterface->ParentDECL;
  GUIimpl->FullName = GUIinterface->FullName;
  GUIimpl->LocalName = GUIinterface->LocalName;
  GUIinterface->ParentDECL->TreeFlags.INCL(DefsExpanded);
  MakeGUIFuncs(GUIinterface);
 
  name = new DString(GUIinterface->FullName);
  hint = new CLavaPEHint(CPECommand_Insert, this, firstlast, (DWORD) GUIinterface,
    (DWORD)name, (DWORD)pos, (DWORD)pparent);
  UndoMem.AddToMem(hint);
  UpdateDoc(0, false, hint);
  pos++;
  firstlast.EXCL(firstHint);
  cheID = new CHETID;
  cheID->data = TID(GUIinterface->OwnID, GUIinterface->inINCL);
  GUIimpl->Supports.Append(cheID);
  GUIimpl->TypeFlags.INCL(isGUI);
  for (cheID = (CHETID*)interdecl->Supports.first;
       cheID; cheID = (CHETID*)cheID->successor) {
    FormDecl = IDTable.GetDECL(cheID->data, interdecl->inINCL);
    if (!FormDecl->TypeFlags.Contains(isGUI)) {
      CFindLikeForm *fLike = new CFindLikeForm(mySynDef, cheID->data, interdecl->inINCL, GUIinterface);
      if (fLike->pdecl && *fLike->pdecl) 
        FormDecl = IDTable.GetDECL(((CHETID*)(*fLike->pdecl)->ParentDECL->Supports.first)->data,(*fLike->pdecl)->inINCL);
      else {
        FormDecl = MakeGUI(FormDecl, pparent, pos, posDECL);
      }
      if (FormDecl) {
        cheFID = new CHETID;
        cheFID->data = TID(FormDecl->OwnID, FormDecl->inINCL);
        GUIinterface->Supports.Append(cheFID);
      }
    }
  }
  CheckImpl(GUIimpl, CHLV_inUpdateLow);
  for (elChe = (CHE*)GUIimpl->NestedDecls.first;
       elChe && (((LavaDECL*)elChe->data)->DeclType != FormDef);
       elChe = (CHE*)elChe->successor);
  if (elChe) {
    FormDecl = (LavaDECL*)elChe->data;
    for (elChe = (CHE*)FormDecl->NestedDecls.first;
         elChe ;
         elChe = (CHE*)elChe->successor) {
      if ((((LavaDECL*)elChe->data)->DeclType == Attr) || (((LavaDECL*)elChe->data)->DeclType == VirtualType)) {
        if (IDTable.GetDECL(((LavaDECL*)elChe->data)->RefID)->DeclType != FormDef)
          if (!((CLavaPEApp*)wxTheApp)->Browser.findAnyForm((LavaDECL*)elChe->data, ((LavaDECL*)elChe->data)->RefID, mySynDef))
            MakeGUI((LavaDECL*)elChe->data, pparent, pos, posDECL);
    
      }
    }
  }
  else
    return 0;
  name = new DString(GUIimpl->FullName);
  elChe = (CHE*)posDECL->ParentDECL->NestedDecls.first;
  implPos = 2;
  while (elChe && (GUIinterface != (LavaDECL*)elChe->data)) {
    if (((LavaDECL*)elChe->data)->DeclType != VirtualType)
      implPos++;
    elChe = (CHE*) elChe->successor;
  }
  hint = new CLavaPEHint(CPECommand_Insert, this, firstlast, (DWORD) GUIimpl,
    (DWORD)name, (DWORD)implPos, (DWORD)pparent);
  UndoMem.AddToMem(hint);
  UpdateDoc(0, false, hint);
  pos++;

  if (attrdecl) {
    if (!startpos) {
      newAttrdecl = NewLavaDECL();
      *newAttrdecl = *attrdecl;
      newAttrdecl->RefID = TID(FormDecl->OwnID, 0);
     // hier hint für geändertes attr
      for (elChe = (CHE*)attrdecl->ParentDECL->NestedDecls.first;
           elChe && ((LavaDECL*)elChe->data != attrdecl);
           elChe = (CHE*)elChe->successor);
      name = new DString(newAttrdecl->FullName);  
      hint = new CLavaPEHint(CPECommand_Change, this, firstlast,
               (DWORD)newAttrdecl, (DWORD)name, 0, (DWORD)&elChe->data);
      UndoMem.AddToMem(hint);
      UpdateDoc(0, false, hint);
    }
    else
      attrdecl->RefID = TID(FormDecl->OwnID, 0);
  }
  if (!startpos)
    SetLastHint();
  return GUIinterface;
}


void CLavaPEDoc::MakeGUIFuncs(LavaDECL* guiInterface)
{
  LavaDECL *interdecl, *fdecl, *IOdecl;
  CHE *elChe;

  interdecl = IDTable.GetDECL(guiInterface->RefID);
  if (interdecl->TypeFlags.Contains(thisComponent))
    guiInterface->TypeFlags.INCL(thisCompoForm);

  fdecl = NewLavaDECL();
  fdecl->DeclType = Function;
  fdecl->DeclDescType = StructDesc;
  fdecl->ParentDECL = guiInterface;
  fdecl->LocalName = DString("FillOut");
  fdecl->FullName = guiInterface->FullName + fdecl->LocalName;
  fdecl->TypeFlags += SET(isGUI, isNative, isConst,-1);
  elChe = NewCHE(fdecl);
  guiInterface->NestedDecls.Append(elChe);

  IOdecl = NewLavaDECL();
  IOdecl->DeclType = IAttr;
  IOdecl->DeclDescType = NamedType;
  IOdecl->ParentDECL = fdecl;
  IOdecl->LocalName = DString("UI_in");
  IOdecl->FullName = fdecl->FullName + IOdecl->LocalName;
  IOdecl->RefID = guiInterface->RefID;
  IOdecl->TypeFlags += SET(isGUI, trueObjCat, isOptional,-1);
  elChe = NewCHE(IOdecl);
  fdecl->NestedDecls.Append(elChe);

  IOdecl = NewLavaDECL();
  IOdecl->DeclType = OAttr;
  IOdecl->DeclDescType = NamedType;
  IOdecl->ParentDECL = fdecl;
  IOdecl->LocalName = DString("UI_out");
  IOdecl->FullName = fdecl->FullName + IOdecl->LocalName;
  IOdecl->RefID = guiInterface->RefID;
  IOdecl->TypeFlags += SET(isGUI, trueObjCat,-1);
  elChe = NewCHE(IOdecl);
  fdecl->NestedDecls.Append(elChe);

  fdecl = NewLavaDECL();
  fdecl->DeclType = Function;
  fdecl->DeclDescType = StructDesc;
  fdecl->ParentDECL = guiInterface;
  fdecl->LocalName = DString("Edit");
  fdecl->FullName = guiInterface->FullName + fdecl->LocalName;
  fdecl->TypeFlags += SET(isGUI, isGUIEdit, isNative,-1);
  elChe = NewCHE(fdecl);
  guiInterface->NestedDecls.Append(elChe);

  IOdecl = NewLavaDECL();
  IOdecl->DeclType = IAttr;
  IOdecl->DeclDescType = NamedType;
  IOdecl->ParentDECL = fdecl;
  IOdecl->LocalName = DString("UI_in");
  IOdecl->FullName = fdecl->FullName + IOdecl->LocalName;
  IOdecl->RefID = guiInterface->RefID;
  IOdecl->TypeFlags += SET(isGUI, stateObject, trueObjCat,-1);
  elChe = NewCHE(IOdecl);
  fdecl->NestedDecls.Append(elChe);

  guiInterface->TypeFlags.INCL(isGUI);
}

void CLavaPEDoc::MakeIniFunc(LavaDECL* ifDECL)
{
  LavaDECL* fdecl = NewLavaDECL();
  fdecl->DeclType = Function;
  fdecl->DeclDescType = StructDesc;
  fdecl->ParentDECL = ifDECL;
  fdecl->LocalName = DString("ini"); //myDECL->LocalName;
  fdecl->FullName = ifDECL->FullName + fdecl->LocalName;
  fdecl->TypeFlags += SET(isInitializer, defaultInitializer,/*isConst,*/-1);
  CHE* che = NewCHE(fdecl);
  ifDECL->NestedDecls.Prepend(che);
}


LavaDECL* CLavaPEDoc::MakeOneSetGet(TypeFlag setgetflag, LavaDECL* implDECL,
                                        LavaDECL* propDecl, int checkLevel)
{
  LavaDECL *setGet, *IOEl, *returnDECL=0;
  CHE *cheSetGet, *cheIOEl;
  TID fID, supID;
  bool found = false;

  cheSetGet = (CHE*)implDECL->NestedDecls.first;
  fID = TID(propDecl->OwnID, propDecl->inINCL);
  if (cheSetGet)
    setGet = (LavaDECL*)cheSetGet->data;
  while (cheSetGet && !found) {
    if ((setGet->DeclType == Function)  
        && setGet->TypeFlags.Contains(setgetflag)
        && setGet->Supports.first) {
      supID = ((CHETID*)setGet->Supports.first)->data;
      supID.nINCL = IDTable.IDTab[implDECL->inINCL]->nINCLTrans[supID.nINCL].nINCL;
      found = (supID == fID);
    }
    if (!found) {
      cheSetGet = (CHE*)cheSetGet->successor;
      if (cheSetGet)
        setGet = (LavaDECL*)cheSetGet->data;
    }
  }
  if (!cheSetGet) { //function not found, then make it
    setGet = NewLavaDECL();
    setGet->DeclType = Function; // = *ifaceElDecl;
    setGet->DeclDescType = StructDesc;
    setGet->SecondTFlags.INCL(funcImpl);
    setGet->TypeFlags.INCL(setgetflag);
//    setGet->TypeFlags.INCL(isVirtual);
    CHETID* cheID = new CHETID;
    cheID->data = fID;
    setGet->Supports.Destroy();
    setGet->Supports.Append(cheID);  //implements get property
    setGet->inINCL = 0;
    returnDECL = setGet;
    IOEl = NewLavaDECL();
    IOEl->SecondTFlags.INCL(funcImpl);
    if (setgetflag == isPropGet)
      IOEl->DeclType = OAttr;
    else
      IOEl->DeclType = IAttr;
    IOEl->DeclDescType = NamedType;
    IOEl->inINCL = 0;
    cheIOEl = NewCHE(IOEl);
    setGet->NestedDecls.Append(cheIOEl);
    if (checkLevel == CHLV_inUpdateHigh)
      implDECL->WorkFlags.INCL(newTreeNode);;
  }
  if (setgetflag == isPropGet) {
    setGet->LocalName = DString("Get_") + propDecl->LocalName;
    setGet->TypeFlags.INCL(isConst);
  }
  else
    setGet->LocalName = DString("Set_") + propDecl->LocalName;
  cheIOEl = (CHE*)setGet->NestedDecls.first;
  IOEl = (LavaDECL*)cheIOEl->data;
  IOEl->RefID = TID(propDecl->RefID.nID, IDTable.IDTab[propDecl->inINCL]->nINCLTrans[propDecl->RefID.nINCL].nINCL);
  IOEl->LocalName = propDecl->LocalName;
  if (propDecl->TypeFlags.Contains(trueObjCat))
    IOEl->TypeFlags.INCL(trueObjCat);
  else
    IOEl->TypeFlags.EXCL(trueObjCat);
  if (propDecl->TypeFlags.Contains(definesObjCat))
    IOEl->TypeFlags.INCL(definesObjCat);
  else
    IOEl->TypeFlags.EXCL(definesObjCat);
  if (propDecl->TypeFlags.Contains(stateObject))
    IOEl->TypeFlags.INCL(stateObject);
  else
    IOEl->TypeFlags.EXCL(stateObject);
                                 
  setGet->WorkFlags.INCL(checkmark);
  return returnDECL;
}

void CLavaPEDoc::MakeOperator(LavaDECL* opDecl)
{
  if (opDecl->op == OP_noOp)
    return;
  opDecl->LocalName = LBaseData->OperatorNames[opDecl->op];
  opDecl->FullName = opDecl->FullName + opDecl->LocalName;
  TID parentID(opDecl->ParentDECL->OwnID, opDecl->ParentDECL->inINCL);
  CHE *cheArg1=0, *cheArg2=0, *cheVal=0, *che;
  LavaDECL *declArg1, *declArg2, *declVal;
  while (opDecl->NestedDecls.first) {
    che = (CHE*)opDecl->NestedDecls.Uncouple(opDecl->NestedDecls.first);
    if (((LavaDECL*)che->data)->DeclType == IAttr) 
      if (!cheArg1)
        cheArg1 = che;
      else
        if (!cheArg2)
          cheArg2 = che;
        else
          delete che;
    else
      if (((LavaDECL*)che->data)->DeclType == OAttr) {
        cheVal = che;
        opDecl->NestedDecls.Destroy();
      }
      else
        delete che;
  }
  opDecl->NestedDecls.Destroy();
  if (opDecl->op != OP_arraySet)
    delete cheArg2;
  if ((opDecl->op == OP_equal)
     || (opDecl->op == OP_notequal)
     || (opDecl->op == OP_lessthen)
     || (opDecl->op == OP_greaterthen)
     || (opDecl->op == OP_lessequal)
     || (opDecl->op == OP_greaterequal)
     || (opDecl->op == OP_arraySet)) {
    if (cheVal)
      delete cheVal;
  }
  else {
    if (cheVal)
      declVal = (LavaDECL*)cheVal->data;
    else {
      declVal = NewLavaDECL();
      cheVal = NewCHE(declVal);
      declVal->ParentDECL = opDecl;
      declVal->DeclType = OAttr;
      declVal->LocalName = DString("result");
      declVal->TypeFlags.INCL(trueObjCat);
    }
    opDecl->NestedDecls.Append(cheVal);
    if (opDecl->op == OP_fis) {
      if (isStd)
        declVal->RefID = TID (IDTable.BasicTypesID[(int)Integer], 0);
      else
        declVal->RefID = TID (IDTable.BasicTypesID[(int)Integer], 1);
      declVal->BType = Integer;
      declVal->DeclDescType = BasicType;
    }
    else {
      declVal->RefID = parentID;
      declVal->BType = opDecl->ParentDECL->fromBType;
      if (opDecl->ParentDECL->fromBType == NonBasic)
        declVal->DeclDescType = NamedType;
      else 
        declVal->DeclDescType = BasicType;
    }
  }
  switch (opDecl->op) {
  case OP_equal:
  case OP_notequal:
  case OP_lessthen:
  case OP_greaterthen:
  case OP_lessequal:
  case OP_greaterequal:
  case OP_plus:
  case OP_mult:
  case OP_div:
  case OP_mod:
  case OP_bwAnd:
  case OP_bwOr:
  case OP_bwXor:
  case OP_lshift:
  case OP_rshift:
    if (cheArg1)
      declArg1 = (LavaDECL*)cheArg1->data;
    else {
      declArg1 = NewLavaDECL();
      cheArg1 = NewCHE(declArg1);
      declArg1->ParentDECL = opDecl;
      declArg1->DeclType = IAttr;
      declArg1->LocalName = DString("op1");
      declArg1->TypeFlags.INCL(trueObjCat);
      if ((opDecl->op == OP_lshift) || (opDecl->op == OP_rshift)) {
        declArg1->DeclDescType = BasicType;
        declArg1->BType = Integer;
        declArg1->RefID.nID = IDTable.BasicTypesID[Integer];
        if (isStd)
          declArg1->RefID.nINCL = 0;
        else
          declArg1->RefID.nINCL = 1;
      }
      else {
        declArg1->RefID = parentID;
        declArg1->BType = opDecl->ParentDECL->fromBType;
        if (opDecl->ParentDECL->fromBType == NonBasic)
          declArg1->DeclDescType = NamedType;
        else 
          declArg1->DeclDescType = BasicType;
      }
    }
    opDecl->NestedDecls.Prepend(cheArg1);
    break;
  case OP_minus:
  case OP_invert:
//    declVal->RefID = parentID;
  case OP_fis:
    if (cheArg1)
      delete cheArg1;
    break;
  case OP_arrayGet:
    if (cheArg1)
      declArg1 = (LavaDECL*)cheArg1->data;
    else {
      declArg1 = NewLavaDECL();
      cheArg1 = NewCHE(declArg1);
    }
    declArg1->ParentDECL = opDecl;
    declArg1->DeclType = IAttr;
    declArg1->DeclDescType = BasicType;
    declArg1->BType = Integer;
    declArg1->TypeFlags.INCL(trueObjCat);
    declArg1->RefID.nID = IDTable.BasicTypesID[Integer];
    if (isStd)
      declArg1->RefID.nINCL = 0;
    else
      declArg1->RefID.nINCL = 1;
    declArg1->LocalName = DString("op1");
    opDecl->NestedDecls.Prepend(cheArg1);
    declVal->DeclDescType = NamedType;
    declVal->BType = NonBasic;
    declVal->TypeFlags.EXCL(trueObjCat);
    IDTable.GetParamID(opDecl->ParentDECL, declVal->RefID, isArray);

    break;
  case OP_arraySet:
    if (cheArg1)
      declArg1 = (LavaDECL*)cheArg1->data;
    else {
      declArg1 = NewLavaDECL();
      cheArg1 = NewCHE(declArg1);
    }
    declArg1->ParentDECL = opDecl;
    declArg1->DeclType = IAttr;
    declArg1->DeclDescType = BasicType;
    declArg1->BType = Integer;
    declArg1->TypeFlags.INCL(trueObjCat);
    declArg1->RefID.nID = IDTable.BasicTypesID[Integer];
    if (isStd)
      declArg1->RefID.nINCL = 0;
    else
      declArg1->RefID.nINCL = 1;
    declArg1->LocalName = DString("op1");
    opDecl->NestedDecls.Prepend(cheArg1);
    if (cheArg2)
      declArg2 = (LavaDECL*)cheArg2->data;
    else {
      declArg2 = NewLavaDECL();
      cheArg2 = NewCHE(declArg2);
      declArg2->ParentDECL = opDecl;
      declArg2->DeclType = IAttr;
      declArg2->TypeFlags.INCL(trueObjCat);
      declArg2->LocalName = DString("op2");
    }
    declArg2->DeclDescType = NamedType;
    declArg2->BType = NonBasic;
    IDTable.GetParamID(opDecl->ParentDECL, declArg2->RefID, isArray);
    opDecl->NestedDecls.Append(cheArg2);

    break;
  }
}

LavaDECL* CLavaPEDoc::MakeSet(const TID& setID, LavaDECL* decl)
{
  LavaDECL *setDECL, *basicDECL, *basePar, *paramEl, *refDECL;
  CHETID* cheS;
  CHE* che;
  bool catErr;

  if (decl)
    setDECL = decl;
  else
    setDECL = NewLavaDECL();
  setDECL->SecondTFlags.INCL(isSet);
  setDECL->DeclType = Interface;
  setDECL->DeclDescType = StructDesc;
  setDECL->inINCL = 0;
  setDECL->fromBType = NonBasic;
  setDECL->Supports.Destroy();
  cheS = new CHETID;
  cheS->data.nID = IDTable.BasicTypesID[B_Set];
  if (isStd)
    cheS->data.nINCL = 0;
  else
    cheS->data.nINCL = 1;
  basicDECL = IDTable.GetDECL(cheS->data);
  if (!basicDECL)
    return 0;
  setDECL->Supports.Append(cheS);
  basePar = (LavaDECL*)((CHE*)basicDECL->NestedDecls.first)->data;
  paramEl = NewLavaDECL();
  *paramEl = *basePar;
  paramEl->DECLComment.Destroy();
  che = NewCHE(paramEl);
  setDECL->NestedDecls.Append(che);
  paramEl->ParentDECL = setDECL;
  paramEl->RefID = setID;
  paramEl->TypeFlags.EXCL(isAbstract);
  paramEl->TypeFlags.INCL(constituent);
  paramEl->SecondTFlags.INCL(overrides);
  paramEl->WorkFlags.EXCL(selAfter);
  paramEl->inINCL = 0;
  refDECL = IDTable.GetDECL(setID);
  if (!refDECL)
    return 0;
  if (refDECL->fromBType == NonBasic)
    paramEl->DeclDescType = NamedType;
  else {
    paramEl->DeclDescType = BasicType;
    paramEl->BType = refDECL->fromBType;
  }
  cheS = new CHETID;
  cheS->data.nID = paramEl->OwnID;
  if (isStd)
    cheS->data.nINCL = 0;
  else
    cheS->data.nINCL = 1;
  paramEl->Supports.Destroy();
  paramEl->Supports.Append(cheS);
  GetCategoryFlags(paramEl, catErr);
  MakeIniFunc(setDECL);
  return setDECL;
}

bool CLavaPEDoc::MakeSetAndGets(LavaDECL* implDECL, LavaDECL* classDecl, int checkLevel)
{
  LavaDECL *ifaceElDecl, *newimplElDecl;
  CHE *cheImplEl, *cheI=0, *cheImpl=0;
  bool toImpl, changed = false;
  CHETVElem *El = (CHETVElem*)classDecl->VElems.VElems.first;
  cheImpl = (CHE*)implDECL->NestedDecls.first;
  if (!El) {
    cheI = (CHE*)classDecl->NestedDecls.first;
    if (!cheI) {
      cheI = cheImpl;
      cheImpl = 0;
    }
  }
  while (El || cheI) { 
    if (El) {
      ifaceElDecl = IDTable.GetDECL(El->data.VTEl);
      toImpl = ifaceElDecl && (ifaceElDecl->DeclType == Attr) 
               && ifaceElDecl->TypeFlags.Contains(isAbstract)
               && ifaceElDecl->TypeFlags.Contains(hasSetGet);
    }
    else {
      ifaceElDecl = (LavaDECL*)cheI->data;
      toImpl = ifaceElDecl && (ifaceElDecl->DeclType == Attr)
               && ifaceElDecl->TypeFlags.Contains(hasSetGet)
               && !ifaceElDecl->TypeFlags.Contains(isAbstract);
//               && !ifaceElDecl->TypeFlags.Contains(inheritsBody);
    }
    if (toImpl) {
      newimplElDecl = MakeOneSetGet(isPropGet, implDECL, ifaceElDecl, checkLevel);
      if (newimplElDecl) {
        cheImplEl = NewCHE(newimplElDecl);
        if (implDECL->NestedDecls.last && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclDescType == ExecDesc)
          implDECL->NestedDecls.Insert(implDECL->NestedDecls.last->predecessor, cheImplEl);
        else
          implDECL->NestedDecls.Append(cheImplEl);
        if (checkLevel > CHLV_inUpdateLow) {
          IDTable.NewID((LavaDECL**)&cheImplEl->data);
          newimplElDecl->ParentDECL = implDECL;
          newimplElDecl->WorkFlags.INCL(newTreeNode);
        }
        if (checkLevel > CHLV_inUpdateHigh) 
          modified = true;
        changed = true;
      }
      newimplElDecl = MakeOneSetGet(isPropSet, implDECL, ifaceElDecl, checkLevel);
      if (newimplElDecl) {
        cheImplEl = NewCHE(newimplElDecl);
        if (implDECL->NestedDecls.last && ((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data)->DeclDescType == ExecDesc)
          implDECL->NestedDecls.Insert(implDECL->NestedDecls.last->predecessor, cheImplEl);
        else
          implDECL->NestedDecls.Append(cheImplEl);
        newimplElDecl->ParentDECL = implDECL;
        if (checkLevel > CHLV_inUpdateLow) {
          UpdateNo++;
          IDTable.NewID((LavaDECL**)&cheImplEl->data);
          newimplElDecl->WorkFlags.INCL(newTreeNode);
        }
        if (checkLevel > CHLV_inUpdateHigh) 
          modified = true;
        changed = true;
      }
    }
    if (El) {
      El = (CHETVElem*)El->successor;
      if (!El) {
        cheI = (CHE*)classDecl->NestedDecls.first;
        if (!cheI) {
          cheI = cheImpl;
          cheImpl = 0;
        }
      }
    }
    else {
      cheI = (CHE*)cheI->successor;
      if (!cheI) {
        cheI = cheImpl;
        cheImpl = 0;
      }
    }

  }//all properties
  return changed;
}

bool CLavaPEDoc::MakeVElems(LavaDECL *classDECL, CheckData* pckd)
{
  bool isNSp, isCreatable, elOk, allOk = true, GUInew = true, GUInewE = true, hasEnum = false;
  QString cstr;
  CHETVElem *El;
  LavaDECL *elDecl, *IFace, *elBase;
  CHE *cheDecl;
  CHETID *cheID;
  TID declID = TID(classDECL->OwnID, classDECL->inINCL);

  if (!classDECL || (classDECL->DeclType == VirtualType))
    return false;
  classDECL->DECLError2.Destroy();
  cheID = (CHETID*)classDECL->Supports.first;

  if (classDECL->VElems.UpdateNo > UpdateNo)
    return true;
  classDECL->VElems.UpdateNo = UpdateNo+1;
  ResetVElems(classDECL);
  while (cheID) { //!!
    IFace = IDTable.GetDECL(cheID->data, classDECL->inINCL);
    if (IFace) {
      if (IFace->DeclType == VirtualType) 
        IFace = IDTable.GetFinalBasicType(cheID->data, classDECL->inINCL, classDECL);
      if (IFace) {
        if (IFace->VElems.UpdateNo <= UpdateNo) {
          ResetVElems(IFace);
          elOk = MakeVElems(IFace);
          allOk = allOk && elOk;
        }
        elOk = AddVElems(classDECL, IFace);
        allOk = allOk && elOk;
        if (!allOk) { // there is an error in a base class
          new CLavaError(&classDECL->DECLError1, &ERR_InVTofBaseIF);
          classDECL->WorkFlags.INCL(recalcVT);
        }
      }
      else {
        allOk = false;
        new CLavaError(&classDECL->DECLError1, &ERR_NoBaseIF);
      }
    }
    cheID = (CHETID*)cheID->successor;
  }
  elOk = AddVBase(classDECL, classDECL);
  if (!elOk) {// there is an error in a base class
    new CLavaError(&classDECL->DECLError2, &ERR_InVTofBaseIF);
    classDECL->WorkFlags.INCL(recalcVT);
  }
  allOk = allOk && elOk;
  if ((classDECL->DeclType != Interface) 
       && (classDECL->DeclType != Package))
    return allOk;

  cheDecl = (CHE*)classDECL->NestedDecls.first;
  isNSp = classDECL->DeclType == Package;
  while (cheDecl) {
    elDecl = (LavaDECL*)cheDecl->data;
    if ( ( ( elDecl->DeclType == VirtualType)
         || ( elDecl->DeclType == Function)
         || ( elDecl->DeclType == Attr)
         )
        && elDecl->Supports.first) {
      //check and replace the extensions and GUI-Show-function
      for (El = (CHETVElem*)classDECL->VElems.VElems.first;
           El && !IDTable.Overrides(TID(elDecl->OwnID, 0), elDecl->inINCL, El->data.VTEl, 0);
           El = (CHETVElem*)El->successor);
      if (El) {
        cheID = (CHETID*)elDecl->Supports.first;
        if (!IDTable.EQEQ(El->data.VTEl, 0, cheID->data, elDecl->inINCL) || El->data.Ambgs.first) {
          if (IDTable.IsAnc(El->data.VTEl,0,TID(elDecl->OwnID, elDecl->inINCL),0)) {
            for (elBase = IDTable.GetDECL(El->data.VTEl);
                 elBase->Supports.first &&
                 !IDTable.EQEQ(TID(elDecl->OwnID,elDecl->inINCL),0,((CHETID*)elBase->Supports.first)->data,elBase->inINCL);
                 elBase = IDTable.GetDECL(((CHETID*)elBase->Supports.first)->data,elBase->inINCL) );
            if (elBase->Supports.first) {
              elBase->Supports.Destroy();
              elBase->Supports = elDecl->Supports;
            }
          }
          elDecl->Supports.Destroy();
          elDecl->Supports = El->data.Ambgs;
          cheID = new CHETID;
          cheID->data = El->data.VTEl;
          elDecl->Supports.Prepend(cheID);
          if ((elDecl->DeclType == VirtualType) && (elDecl->RefID.nID >= 0))
            IDTable.CheckValOfVirtual(elDecl);
        }
        if (El->data.VTBaseEl.nID == -1)
          El->data.VTBaseEl = El->data.VTEl;
        El->data.VTEl = TID(elDecl->OwnID, elDecl->inINCL);
        El->data.TypeFlags = elDecl->TypeFlags;
        El->data.op = elDecl->op;
        El->data.Ambgs.Destroy();
        El->data.ok = true;
      }
      else { //the overridden object was not found
        El = new CHETVElem;
        El->data.VTClss = declID;
        El->data.VTEl = TID(elDecl->OwnID, elDecl->inINCL);
        El->data.VTBaseEl = El->data.VTEl;
        El->data.TypeFlags = elDecl->TypeFlags;
        El->data.op = ((LavaDECL*)cheDecl->data)->op;
        El->data.ok = false;
        classDECL->VElems.VElems.Append(El);
      }
    }
    else 
      if (( elDecl->DeclType == Function) && elDecl->TypeFlags.Contains(isGUI)) {
        for (El = (CHETVElem*)classDECL->VElems.VElems.first;
             El && !El->data.TypeFlags.Contains(isGUI); 
             El = (CHETVElem*)El->successor);
        if (El) {
          if (El->data.TypeFlags.Contains(isGUIEdit))
            GUInewE = false;
          else
            GUInew = false;
          El->data.VTEl = TID(elDecl->OwnID, elDecl->inINCL);
          El->data.VTBaseEl = El->data.VTEl;
          El->data.op = elDecl->op;
          El->data.Ambgs.Destroy();
          El->data.TypeFlags = elDecl->TypeFlags;
          El->data.ok = true;
        }
      }
    if (isNSp && (elDecl->DeclType != VirtualType))
      cheDecl = 0;
    else
      cheDecl = (CHE*)cheDecl->successor;
  }
  cheDecl = (CHE*)classDECL->NestedDecls.first;
  while (cheDecl) {
    elDecl = (LavaDECL*)cheDecl->data;
    if ( ( (elDecl->DeclType == VirtualType)
        || ( elDecl->DeclType == Function) && !elDecl->TypeFlags.Contains(isStatic)
        || ( elDecl->DeclType == Attr))
        && !elDecl->Supports.first
        && (GUInew || !elDecl->TypeFlags.Contains(isGUI))
        && (GUInewE || !elDecl->TypeFlags.Contains(isGUIEdit))) {
      El = new CHETVElem;
      El->data.VTClss = declID;
      El->data.VTEl = TID(elDecl->OwnID, elDecl->inINCL);
      El->data.VTBaseEl = El->data.VTEl;
      El->data.TypeFlags = elDecl->TypeFlags;
      El->data.op = elDecl->op;
      El->data.ok = true;
      classDECL->VElems.VElems.Append(El);
    }
    if (isNSp && (elDecl->DeclType != VirtualType))
      cheDecl = 0;
    else
      cheDecl = (CHE*)cheDecl->successor;
  }
  if (allOk) {
    elOk = true;
    declID = TID(-1,0);
    isCreatable = !classDECL->TypeFlags.Contains(isAbstract) && (classDECL->DeclType != Package);
    for (El = (CHETVElem*)classDECL->VElems.VElems.first;
         El && El->data.ok;
         El = (CHETVElem*)El->successor) {
      elDecl = IDTable.GetDECL(El->data.VTEl);
      if (isCreatable) {
        if (elDecl->TypeFlags.Contains(isAbstract)) {
          new CLavaError(&classDECL->DECLError1, &ERR_AbstractInherited, &elDecl->FullName);
          classDECL->WorkFlags.INCL(recalcVT);
          elOk = false;
        }
      }
      if (elDecl->TypeFlags.Contains(forceOverride)
          && (elDecl->ParentDECL != classDECL)) {
        new CLavaError(&classDECL->DECLError1, &ERR_ForceOver, &elDecl->FullName);
        classDECL->WorkFlags.INCL(recalcVT);
        El->data.ok = false;
        elOk = false;
      }
      if (declID != El->data.VTClss) {
        declID = El->data.VTClss;
        IFace = IDTable.GetDECL(declID);
        if (IFace->DeclDescType == EnumType)
          if (hasEnum && !classDECL->inINCL) {
            new CLavaError(&classDECL->DECLError1, &ERR_OneEnum, &classDECL->FullName);
            classDECL->WorkFlags.INCL(recalcVT);
            elOk = false;
          }
          else
            hasEnum = true;
      }
    }
    allOk = El == 0;
  }
  if (!allOk) {
    new CLavaError(&classDECL->DECLError1, &ERR_InVT);
    classDECL->WorkFlags.INCL(recalcVT);
  }
  return allOk && elOk;
}

void CLavaPEDoc::Modify(bool bModified)
{
  wxDocument::Modify (bModified); 
  if (MainView)
    ((CTreeFrame*)MainView->GetParentFrame())->SetModified(bModified);
}

//check the own document
void CLavaPEDoc::OnCheck() 
{
  CExecChecks* ch = new CExecChecks(this);
  delete ch;
  ShowErrorBox(false);
}

bool CLavaPEDoc::OnCloseDocument() 
{
  if (!((wxApp*)qApp)->appExit ) {
    if (debugOn && ((CLavaPEApp*)wxTheApp)->debugThread.running()) {
      ((CLavaPEApp*)wxTheApp)->debugThread.cleanBrkPoints(this);
      if (((CLavaPEApp*)wxTheApp)->debugThread.dbgReceived.lastReceived) 
        ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->removeExecStackPos((DbgStopData*)((CLavaPEApp*)wxTheApp)->debugThread.dbgReceived.lastReceived->DbgData.ptr, this);
      if (((CLavaPEApp*)wxTheApp)->debugThread.dbgReceived.newReceived) 
        ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->removeExecStackPos((DbgStopData*)((CLavaPEApp*)wxTheApp)->debugThread.dbgReceived.newReceived->DbgData.ptr, this);
      delete ((CLavaPEApp*)wxTheApp)->debugThread.workSocket;
      ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->setDebugData(0, this);
      ((CLavaPEApp*)wxTheApp)->debugThread.myDoc = 0;
      ((CLavaPEApp*)wxTheApp)->debugThread.pContExecEvent->release();
    }
    else
      ((CLavaPEApp*)wxTheApp)->debugThread.cleanBrkPoints(this);
    if (((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->stopDoc == this)
      ((CLavaMainFrame*)((CLavaPEApp*)wxTheApp)->m_appWindow)->m_UtilityView->stopDoc = 0;
  }
  return CPEBaseDoc::OnCloseDocument();
}

void CLavaPEDoc::OnCloseLastExecView()
{
	if (wxTheApp->deletingMainFrame) return;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_5->hide();
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_6->hide();
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_7->hide();
}

bool CLavaPEDoc::OnEmptyDoc(const QString& Name)
{
  bool bb;
  QString stdLava=StdLavaLog;

  mySynDef = new SynDef;
  SynIO.InitSyntax(mySynDef, Name);
  CalcNames(Name);
  IDTable.mySynDef = mySynDef;
  LavaDECL *TopDECL = NewLavaDECL();
  if (((CLavaPEApp*)wxTheApp)->pLComTemplate == GetDocumentTemplate()) {
    TopDECL->DeclType = Component;
    isObject = true;
  }
  else
    TopDECL->DeclType = Package;
  TopDECL->DeclDescType = StructDesc; // = DefDesc;
  TopDECL->TreeFlags.INCL(hasEmptyOpt);
  TopDECL->TreeFlags.INCL(isExpanded);
  TopDECL->WorkFlags.INCL(selDefs);
  TopDECL->WorkFlags.INCL(selAfter);
  TopDECL->OwnID = 0;
  ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr = TopDECL;
  mySynDef->IDTable = (address)&IDTable;
  isStd = false;
  IDTable.MakeTable(mySynDef, isStd);
  return (IncludeSyntax(stdLava, bb) != 0);
}

bool CLavaPEDoc::OnNewDocument()
{
  if (!CLavaBaseDoc::OnNewDocument())
    return false;
  if (!mySynDef) {
    QString Name = GetTitle();
    return OnEmptyDoc(Name);
  }
  return true;
}

bool CLavaPEDoc::OnOpenDocument(const QString& filename) 
{
//  AfxDebugBreak();
  bool errEx;
  DString str0, str, newTopName, *toINCL = 0, fn(qPrintable(filename));
	QString fName(filename);
  int readResult;

  //LBaseData->lastFileOpen = QString(fn.c);
  isStd = SameFile(fn.c, StdLava.ascii());
  CalcNames(filename);
  readResult = ReadSynDef(filename, mySynDef);
  isReadOnly = readResult > 0;
  if (readResult < 0) {
    mySynDef = 0;
    return false;
  }
  wxDocManager::GetDocumentManager()->AddFileToHistory(fName);

  if (mySynDef->SynDefTree.first == mySynDef->SynDefTree.last) {
    if (!isStd) {
      str = DString("File '") + fn + " is not a valid lava file";
      critical(wxTheApp->m_appWindow,qApp->name(),str.c,QMessageBox::Ok,0,0);
    }
    changeNothing = !isStd || !LBaseData->stdUpdate;
  }
	else
		changeNothing = isReadOnly;
  Modify(false);
  isObject = (((CLavaPEApp*)wxTheApp)->pLComTemplate == GetDocumentTemplate());
  if (mySynDef) {
    mySynDef->IDTable = (address)&IDTable;
    hasIncludes = false;
    AddSyntax(mySynDef, filename, errEx);  //Add include files/patterns
    if (errEx)
      return false;
    UpdateOtherDocs(0, str0, 0, false); 
    //CExecChecks* ch = new CExecChecks(this, true);
    IDTable.SetImplIDs(false);
    //delete ch;
  }
  else {
    if (readResult < 0) {
      str = DString("Cannot read file '") + fn + DString(", corrupt lava program syntax");
      str += "\n  ";
      str += "\nPerhaps different lava versions?"; 
      str += "\n  ";
    }
    else 
      str = DString("File '") + fn + DString("' not found");
    QMessageBox::critical(wxTheApp->m_appWindow, qApp->name(), str.c,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  return true;
}//OnOpenDocument

void CLavaPEDoc::OnRunLava() 
{
	QString interpreterPath, lavaFile = GetFilename(), buf;

  if (IsModified()
    && !((CLavaPEApp*)wxTheApp)->DoSaveAll()
    && (QMessageBox::Cancel == QMessageBox::question(wxTheApp->m_appWindow,qApp->name(),ERR_SaveFailed,QMessageBox::Ok,QMessageBox::Cancel,0)))
    return;
  lavaFile = GetFilename();
  /*
  if (lavaFile.isEmpty()) {
    QMessageBox::question(wxTheApp->m_appWindow,qApp->name(),IDP_SaveFirst,QMessageBox::Ok,0,0);
    return;
  }
  */
#ifdef WIN32
  interpreterPath = ExeDir + "/Lava.exe";
#else
  interpreterPath = ExeDir + "/Lava";
#endif
	
	QStringList args;
	args << lavaFile;
	QProcess interpreter;

  if (!((CLavaPEApp*)qApp)->interpreter.startDetached(interpreterPath,args)) {
    QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),ERR_LavaStartFailed.arg(errno),QMessageBox::Ok,0,0);
		return;
	}
}


void CLavaPEDoc::OnDebugLava() 
{  
  if (IsModified()
    && !((CLavaPEApp*)wxTheApp)->DoSaveAll()
    && (QMessageBox::Cancel == QMessageBox::question(wxTheApp->m_appWindow,qApp->name(),ERR_SaveFailed, 
    QMessageBox::Ok,QMessageBox::Cancel,0)))
    return;
/*
  if (lavaFile.isEmpty()) {
    QMessageBox::question(wxTheApp->m_appWindow,qApp->name(),IDP_SaveFirst,QMessageBox::Ok,0,0);
    return;
  }
  */
	
  debugOn = true;
  changeNothing = true;

  ((CLavaPEApp*)qApp)->debugThread.myDoc = this;
  ((CLavaPEApp*)qApp)->debugThread.adjustBrkPnts();
  ((CLavaPEApp*)qApp)->debugThread.start();
}

void CLavaPEDoc::interpreterExited () {
  delete ((CLavaPEApp*)wxTheApp)->debugThread.workSocket;
  ((CLavaPEApp*)qApp)->debugThread.pContExecEvent->release();
}

//check all included documents
void CLavaPEDoc::OnTotalCheck() 
{
  int ii, nErrBoxShown = 0;
  DString absName, absName2, messStr, nstr;
  CLavaPEDoc *doc;
  wxDocManager* mana = wxDocManager::GetDocumentManager();
  POSITION pos = 0;

  CExecChecks* ch = new CExecChecks(this);
  delete ch;
  wxView *actView = mana->GetActiveView();
  ((CLavaPEApp*)wxTheApp)->inTotalCheck = true;
  if (/*nTreeErrors ||*/ nErrors || nPlaceholders) {
    ShowErrorBox(true);
    nErrBoxShown++;
  }
  for (ii = 1; ii < IDTable.freeINCL; ii++) {
    if (IDTable.IDTab[ii]->isValid) {
      absName  = IDTable.IDTab[ii]->FileName;
      AbsPathName(absName, IDTable.DocDir);
      absName2.Reset(0);
      pos = mana->GetFirstDocPos();
      while (pos && !SameFile(absName, absName2)) {
        doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
        absName2  = doc->IDTable.IDTab[0]->FileName;
        AbsPathName(absName2, doc->IDTable.DocDir);
      }
      if (SameFile(absName, absName2)) 
        doc->openInTotalCheck = false;
      else {
        doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(absName.c);
        if (doc)
          doc->openInTotalCheck = true;
      }
      if (doc && !doc->checkedInTotalCheck) {
        CExecChecks* ch = new CExecChecks(doc);
        doc->checkedInTotalCheck = true;
        delete ch;
        if (doc->nErrors || doc->nPlaceholders) {
          nErrBoxShown++;
          //if (!doc->openInTotalCheck) 
          //  doc->MainView->GetParentFrame()->Activate(true);
          doc->ShowErrorBox(true);
        }
      }
    }
  }
  messStr = DString("Total check finished: ");
  if (!nErrBoxShown)
    messStr += DString("No errors");
  else {
    messStr += DString("Errors in ");
    if (nErrBoxShown == 1)
      messStr += DString("one lava file");
    else {
      Convert.IntToString(nErrBoxShown, nstr);
      messStr += nstr;
      messStr += DString(" lava files");
    }
  }
  QMessageBox::critical(wxDocManager::GetDocumentManager()->GetActiveView(), qApp->name(), messStr.c,  QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);

  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    doc->checkedInTotalCheck = false;
    if (doc->openInTotalCheck)
      doc->OnCloseDocument();
  }
  ((CLavaPEApp*)wxTheApp)->inTotalCheck = false;
  mana->SetActiveView(actView, true);
}

void CLavaPEDoc::OnUpdateRunLava(QAction* action) 
{
  if (!mySynDef) 
    return;
  LavaDECL* topDECL = (LavaDECL*)((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr;
  CHE* che;
  for (che = (CHE*)topDECL->NestedDecls.first;
       che && (((LavaDECL*)che->data)->DeclType == VirtualType);
       che = (CHE*) che->successor);
  action->setEnabled(che && (((LavaDECL*)che->data)->DeclType == Initiator));
}

bool CLavaPEDoc::OpenExecView(LavaDECL* eDECL)
{
  bool active=false;
  CLavaBaseView *view;
  wxMDIChildFrame *execChild;
//  LavaDECL *eDECL = (LavaDECL*)execChe->data;
  POSITION pos = GetFirstViewPos();
  while (pos && !active) {
    view = (CLavaBaseView*)GetNextView(pos);
    active = view->inherits("CExecView") && (((CExecView*)view)->myDECL == eDECL);
  }
  ViewPosRelease(pos);
  if (active)
    execChild = view->GetParentFrame();
  else {
    ((CLavaPEApp*)wxTheApp)->LBaseData.actHint = new CLavaPEHint(CPECommand_OpenExecView, this, (const unsigned long)3, (DWORD) eDECL, (DWORD)MainView, (DWORD)wxTheApp->m_appWindow->statusBar(), (DWORD)((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView, 0); // (DWORD) pdecl);
    execChild = ((CLavaPEApp*)wxTheApp)->pExecTemplate->CreateChildFrame(this);
    active = (execChild !=0);
    execChild->InitialUpdate();
		((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_5->show();
		((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_6->show();
		((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_7->show();

    if (!eDECL->Exec.ptr)
      SetExecItemImage(eDECL, false, false);
  }
  if (active) 
    execChild->Activate(true);
  if (((CLavaPEApp*)wxTheApp)->LBaseData.actHint) {
    delete ((CLavaPEApp*)wxTheApp)->LBaseData.actHint; 
    ((CLavaPEApp*)wxTheApp)->LBaseData.actHint = 0; 
  }
  return true;
}

bool CLavaPEDoc::OpenGUIView(LavaDECL** pdecl)
{ 
  bool active=false;
  CLavaBaseView *view;
  wxMDIChildFrame *formChild;
  POSITION pos = GetFirstViewPos();
  while (pos && !active) {
    view = (CLavaBaseView*)GetNextView(pos);
    active = !view->inherits("CTreeView") && !view->inherits("CExecView")
         && (((CLavaPEView*)((CFormFrame*)view->GetParentFrame())->viewR)->myDECL == *pdecl);
  }
  ViewPosRelease(pos);
  if (active) 
    formChild = view->GetParentFrame();
  else {
    ((CLavaPEApp*)wxTheApp)->LBaseData.actHint = new CLavaPEHint(CPECommand_OpenFormView, this, (const unsigned long)3, (DWORD) *pdecl, (DWORD)MainView, 0, (DWORD)pdecl);
    formChild = ((CLavaPEApp*)wxTheApp)->pFormTemplate->CreateChildFrame(this);
    active = (formChild !=0);
    formChild->InitialUpdate();
    delete ((CLavaPEApp*)wxTheApp)->LBaseData.actHint; 
    ((CLavaPEApp*)wxTheApp)->LBaseData.actHint = 0; 

  }
  if (active) {
    formChild->Activate(true);
	 // if (wxDocManager::GetOpenDocCount() == 1)
	 //	  QApplication::postEvent((CMainFrame*)wxTheApp->m_appWindow,new CustomEvent(QEvent::User,0));
  }
  return true;
}//OpenGUIView

bool CLavaPEDoc::OpenVTView(LavaDECL** pdecl, unsigned long autoUpdate)
{ 
  POSITION pos = GetFirstViewPos();
  CLavaBaseView* view;
  bool active=FALSE; 
  //CRect rr;
  CLavaPEHint * hint;

  while (pos && !active) {
    view = (CLavaBaseView*)GetNextView(pos);
    active = view->inherits("CVTView") && (((CVTView*)view)->myDECL == *pdecl);
  }
  ViewPosRelease(pos);
  if (active) {
    if (!autoUpdate)
      ((CTreeFrame*)view->GetParentFrame())->CalcSplitters(true); //make it visible
  }
  else {
    hint = new CLavaPEHint(CPECommand_OpenSelView, this, (const unsigned long)3, (DWORD) *pdecl, (DWORD)MainView, autoUpdate,(DWORD)pdecl);
    if (autoUpdate)
      QApplication::postEvent(((CLavaPEView*)MainView)->myVTView, new QCustomEvent(IDU_LavaPE_SyncTree, (void*)hint));
    else {
      ((CLavaPEView*)MainView)->myVTView->OnUpdate(MainView, 0, hint);
      delete hint;
    }
  }
  return true;
}//openVTView

bool CLavaPEDoc::OpenWizardView(CLavaBaseView* formView, LavaDECL** pdecl/*, unsigned long autoUpdate*/)
{ 
//  POSITION pos = GetFirstViewPos();
  CLavaBaseView* view;
  bool active=FALSE; 
  //CRect rr;
  CLavaPEHint * hint;

  view = ((CFormFrame*)formView->GetParentFrame())->wizardView; 
  hint = new CLavaPEHint(CPECommand_OpenWizardView, this, (const unsigned long)3, (DWORD) *pdecl, (DWORD)MainView, 0 /*autoUpdate*/,(DWORD)pdecl);
  view->OnUpdate(formView,0,hint);
  delete hint;
  return true;
}//openWizardView

void CLavaPEDoc::ResetError()
{
  DString str0;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetComment(str0, true);
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->ResetError(); 
}


void CLavaPEDoc::ResetVElems(LavaDECL *classDECL)
{
  if (classDECL) {
    classDECL->VElems.VElems.Destroy();
    classDECL->DECLError2.Destroy();
  }
}


void CLavaPEDoc::SetExecFindText(CSearchData& sData)
{
  if (!sData.findRefs.fname.l) {
    sData.findRefs.fname = IDTable.IDTab[0]->FileName;
    AbsPathName(sData.findRefs.fname, IDTable.DocDir);
  }
  DString barText = sData.findRefs.fname + DString(":  ");
  barText += sData.execDECL->ParentDECL->FullName;
  //barText += DString(", ");
  if (sData.execDECL->DeclType == ExecDef)
    if ((sData.execDECL->ParentDECL->DeclType == Function)
      || (sData.execDECL->ParentDECL->DeclType == Initiator))
      barText += DString(":   ");
    else
      barText += DString(", Invariant: ");
  else if (sData.execDECL->DeclType == Ensure)
    barText += DString(", Ensure:   ");
  else 
    barText += DString(", Require:   ");
      
  barText += sData.constructNesting;
  //CFindData* data = new CFindData(1, sData.fileName, sData.execDECL->OwnID, sData.synObjectID, sData.refID);
  CFindData* data = new CFindData;
  *data = sData.findRefs;
  if (data->index) //means search by name
    data->index = 100 + (int)sData.execDECL->DeclType;
  else 
    data->index = (int)sData.execDECL->DeclType;
//  data->enumID = sData.enumID;
//  data->fname = sData.fileName;
//  data->refTid = sData.refID;
  data->refCase = sData.synObjectID;
  data->nID = sData.execDECL->ParentDECL->OwnID;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetFindText(barText, data);
}


CHE* CLavaPEDoc::SetExecChe(LavaDECL* parentDecl,LavaDECL* execDecl)
{
  LavaDECL *cDECL = 0;
  CHE *che, *afterChe = (CHE*)parentDecl->NestedDecls.last;
  if (afterChe) {
    cDECL = (LavaDECL*)afterChe->data;
    if (cDECL->DeclType != execDecl->DeclType) {
      if ((cDECL->DeclType == ExecDef)
        || (cDECL->DeclType == Ensure) && (execDecl->DeclType == Require)) {
        afterChe = (CHE*)afterChe->predecessor; 
        if (afterChe) {
          cDECL = (LavaDECL*)afterChe->data;
          if (cDECL->DeclType != execDecl->DeclType) {
            if (cDECL->DeclType == Ensure) {
              afterChe = (CHE*)afterChe->predecessor; 
              if (afterChe) {
                cDECL = (LavaDECL*)afterChe->data;
                if (cDECL->DeclType != execDecl->DeclType)
                  cDECL = 0;
              }
              else
                cDECL = 0;
            }
            else
              cDECL = 0;
          }//else ok
        }
        else
          cDECL = 0;
      }
      else
        cDECL = 0;
    }
  }
  if (!cDECL) {
    che = NewCHE(execDecl);
    parentDecl->NestedDecls.Insert(afterChe, che);
    return che;
  }
  else
    return 0;
}

void CLavaPEDoc::SetExecItemImage(LavaDECL* execDECL, bool empty, bool hasErrors)
{
  if (!MainView)
    return;
  CLavaPEView* view = (CLavaPEView*)MainView;
  int bm;
  bool hasCom;
  SynFlags flags;
  if ((execDECL->ParentDECL->DeclType == Interface) || (execDECL->ParentDECL->DeclType == Impl))
    flags.INCL(invariantPM);
  if (empty)
    flags.INCL(emptyPM);
  CTreeItem* item = view->BrowseTree(execDECL->ParentDECL, (CTreeItem*)view->Tree->RootItem);
  item = view->getSectionNode(item, execDECL->DeclType);
  bm = view->GetPixmap(true,true,execDECL->DeclType,flags);
  /*
  if (empty) 
    bm = ((CLavaPEApp*)wxTheApp)->LavaIcons[7];
  else
    bm = ((CLavaPEApp*)wxTheApp)->LavaIcons[8];
  */
  hasCom = execDECL && execDECL->DECLComment.ptr && execDECL->DECLComment.ptr->Comment.l;
  if (item) {
    item->setPixmapIndex(bm); 
    item->SetItemMask(hasErrors, hasCom);
//    item->repaint();
  }
}


void CLavaPEDoc::SetFindText(LavaDECL* inDecl, CFindData& fw)//const DString& absFileName, int refCase, const TID& refTid, const DString& enumID)
{
  DString barText = fw.fname + DString(":  ");
  barText += inDecl->FullName;
  barText += DString(":   ");
  CFindData* data = new CFindData;
  *data = fw;
  data->nID = inDecl->OwnID;
//  CFindData* data = new CFindData(0, absFileName, inDecl->OwnID, refCase, refTid);
  switch (data->refCase) {
  case 0:  //RefID
    switch (inDecl->DeclType) {
    case Attr:
      barText += DString("Type of member");
      break;
    case IAttr:
      barText += DString("Type of input");
      break;
    case OAttr:
      barText += DString("Type of output");
      break;
    case VirtualType:
      barText += DString("Real type of virtual type");
      break;
    case Interface:
      barText += DString("GUI service");
      break;
    case Impl:
    case FormDef:
      barText += DString("GUI service implementation");
      break;
    default: ;
    }
    break;
  case 1: //Supports
    switch (inDecl->DeclType) {
    case Interface:
    case Package:
      barText += DString("Extension");
      break;
    case VirtualType:
      barText += DString("Overriding virtual type");
      break;
    case Impl:
      barText += DString("Implementation");
      break;
    case Function:
      if (inDecl->SecondTFlags.Contains(funcImpl))
        barText += DString("Implementation of function");
      else
        barText += DString("Overriding function");
      break;
    case Attr:
      if (inDecl->ParentDECL->DeclType == FormDef)
        barText += DString("Associated member in form");
      else
        barText += DString("Overriding member in pattern");
      break;
    case IAttr:
      barText += DString("Input of overriding function");
      break;
    case OAttr:
      barText += DString("Output of overriding function");
      break;
    case CompObjSpec:
      barText += DString("Component object specification");
      break;
    default: ;
    }
    break;
  case 2:
    if (inDecl->DeclType == Function)
      barText += DString("Thrown type in function");
    else
      if ((inDecl->DeclType == Interface) || (inDecl->DeclType == CompObjSpec))
       barText += DString("Signaled type in interface");
    break;
  case 3:
    barText += DString("Form of set element type");
    break;
  case 5:
    //data->enumID = enumID;
  case 4:
    barText += DString("Own declaration");
    break;
  default: ;
  }
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetFindText(barText, data);
}

void CLavaPEDoc::SetLastHints(bool fromDragDrop, bool otherDocs)
{  //finish of drag and drop and change include file from InclView
  POSITION pos; 
  DString str0;
  CExecSetImpls *impls;
  CLavaPEDoc* doc;

  wxDocManager* mana = wxDocManager::GetDocumentManager(); 
  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    if (doc->hasHint) {
      doc->SetLastHint(false);
      if (otherDocs)
        doc->UpdateOtherDocs(this, str0, 0, false);
    }
  } 
  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    if (doc->hasHint || doc->drawView) {
      doc->hasHint = false;
      if (fromDragDrop) {
        doc->drawView = false;
        impls = new CExecSetImpls(doc->mySynDef);
        delete impls;
        ((CLavaPEView*)doc->MainView)->m_hitemDrag = 0;
      }
      doc->UpdateAllViews(NULL, 3, 0);
    }
  }
}


void CLavaPEDoc::SetNameText(LavaDECL* inDecl, CFindData& fw)//const DString& absFileName, const DString& enumID)
{
  //TID tid;
  bool putIt = false;
  DString barText = fw.fname + DString(":  ");
  if (!fw.enumID.l) {
    switch (inDecl->DeclType) {
    case Attr:
      barText += DString("Member ");
      putIt = true;
      break;
    case Function:
      if (!inDecl->TypeFlags.Contains(funcImpl)) {
        if (inDecl->TypeFlags.Contains(defaultInitializer))
          barText += DString("Default initializer ");
        else if (inDecl->TypeFlags.Contains(isInitializer))
          barText += DString("Initializer ");
        else
          barText += DString("Function ");
        putIt = true;
      }
      break;
    case IAttr:
      if (!inDecl->ParentDECL->TypeFlags.Contains(funcImpl)) {
        barText += DString("Function input ");
        putIt = true;
      }
      break;
    case OAttr:
      if (!inDecl->ParentDECL->TypeFlags.Contains(funcImpl)) {
        barText += DString("Function output ");
        putIt = true;
      }
      break;
    case VirtualType:
      barText += DString("Virtual type ");
      putIt = true;
      break;
    case Interface:
      if (inDecl->TypeFlags.Contains(isGUI))
        barText += DString("GUI service interface ");
      else
        barText += DString("Interface ");
      putIt = true;
      break;
    case CompObjSpec:
      barText += DString("Component object specification ");
      putIt = true;
      break;
    case Initiator:
      barText += DString("Main program ");
      putIt = true;
      break;
    case Package:
      barText += DString("Package ");
      putIt = true;
      break;
    default: ;
    }
  }
  else {
    barText += "Enumeration item ";
    barText += fw.enumID;
    barText += " in interface ";
    putIt = true;
  }
  if (putIt) {
    barText += inDecl->FullName;
  //  CFindData* data = new CFindData(2, absFileName, inDecl->OwnID, 0, tid);
    CFindData* data = new CFindData;
    *data = fw;
    data->nID = inDecl->OwnID;
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetFindText(barText, data);
  }
}

void CLavaPEDoc::SetPEError(const CHAINX& ErrChain, bool andShow)
{
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetErrorOnUtil(ErrChain);
  if (andShow) {
    ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetTab(tabError);
    //((CLavaMainFrame*)wxTheApp->m_appWindow)->RecalcLayout();
  }
}

void CLavaPEDoc::SetTreeItemImage(LavaDECL* errDECL, bool hasErr)
{ 
  CLavaPEView* view = (CLavaPEView*)MainView;
  CTreeItem* item = view->BrowseTree(errDECL, (CTreeItem*)view->Tree->RootItem);
  if (item) {
    item->SetItemMask(hasErr, errDECL->DECLComment.ptr);
//    item->repaint();
  }
}

void CLavaPEDoc::ShowErrorBox(bool inOpen)
{
  QString mess;
  DString nstr;
  int nerr = nErrors;// + nTreeErrors;
  if (((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate)
    return;
  if (inOpen)
    if (((CLavaPEApp*)wxTheApp)->inTotalCheck) {
      mess = QString("File ");
      mess += QString(GetFilename());
      mess += QString(" contains ");
    }
    else
      mess = QString("This Lava file contains ");
  if (nerr) {
    Convert.IntToString(nerr, nstr);
    mess += QString(nstr.c);
    if (nerr == 1)
      mess += QString(" error");
    else
      mess += QString(" errors");
  }
  else
    if (inOpen)
      mess += QString ("no errors");
    else
      mess = QString ("No errors");
  if (nPlaceholders) {
    Convert.IntToString(nPlaceholders, nstr);
    mess += QString(" and ") + QString(nstr.c);
    if (nPlaceholders == 1)
      mess += QString(" placeholder");
    else
      mess += QString(" placeholders");
  }
  mess += QString(pkt.c);
  if (nErrors + nPlaceholders)
    QMessageBox::critical(wxTheApp->m_appWindow,qApp->name(),mess,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  else
    QMessageBox::information(wxTheApp->m_appWindow,qApp->name(),mess,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
}

void CLavaPEDoc::ShrinkCollectDECL(LavaDECL* decl)
{
  CHE* chePre = 0;
  CHE* che = (CHE*)decl->NestedDecls.first;
  LavaDECL* elDECL;

  while (che) {
    elDECL = (LavaDECL*)che->data;
    if (!elDECL->WorkFlags.Contains(checkmark) 
        && (elDECL->DeclType != IAttr) && (elDECL->DeclType != OAttr)) {
      decl->NestedDecls.Uncouple(che);
      delete che;
      if (chePre)
        che = (CHE*)chePre->successor;
      else
        che = (CHE*)decl->NestedDecls.first;
    }        
    else {
      elDECL->WorkFlags.EXCL(checkmark);
      elDECL->Supports.Destroy();
      elDECL->DECLComment.Destroy();
      if (elDECL->DeclType == Interface) {
        elDECL->Items.Destroy();
        elDECL->DeclDescType = StructDesc;
      }
      ShrinkCollectDECL(elDECL);
      if (elDECL->DeclType == Interface)
        MakeIniFunc(elDECL);
      chePre = che;
      che = (CHE*)che->successor;
    }
  }
}

bool CLavaPEDoc::TrueReference(LavaDECL* decl, int refCase, const TID& refTid)
{
  CHETID *che;

  switch (refCase) {
  case 0:  //RefID
    if (decl->RefID == refTid)
      return true;
    break;
  case 1: //Supports
    for (che = (CHETID*)decl->Supports.first; che && (che->data != refTid); che = (CHETID*)che->successor);
    if (che)
      return true;
    break;
  case 2: //Inherits
    for (che = (CHETID*)decl->Inherits.first; che && (che->data != refTid); che = (CHETID*)che->successor);
    if (che)
      return true;
    break;
  case 3: //Chain element in form
    if ((decl->DeclType == VirtualType) && (decl->ParentDECL->DeclType == FormDef)
      && decl->Annotation.ptr
      && decl->Annotation.ptr->IterOrig.ptr
      && ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
      LavaDECL* CHEEl = ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
      if (CHEEl->RefID == refTid)
        return true;
    }
    break;
  case 4:  //Own id
  case 5:  //enumID in own id
    return (decl->OwnID == refTid.nID) && (decl->inINCL == refTid.nINCL);
  default: ;
  }
  return false;
}

void CLavaPEDoc::UpdateMoveInDocs(const DString& clipDocFn)
{
  CHESimpleSyntax* cheSyn;
  POSITION pos; 
  DString absDropFn, *str;//, str0, absName, absName2, dragFn;
  int dragIncl = -1, dropIncl = -1;
  LavaDECL* oldTopDECL, *newTopDECL;
  CLavaPEHint* hint;
  CLavaPEDoc* doc, *dragDoc=0;
  CLavaPEView* view;
  SynFlags firstLast;

  absDropFn  = IDTable.IDTab[0]->FileName;
  AbsPathName(absDropFn, IDTable.DocDir);
  firstLast.INCL(multiDocHint);
  this->hasHint = true;
  wxDocManager* mana = wxDocManager::GetDocumentManager();
  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    dragIncl = -1;
    dropIncl = -1;
    //dropFn = absName;
    //dragFn = clipDoc;
    if (absDropFn == clipDocFn)
      dragDoc = this;
    //RelPathName(dropFn, doc->IDTable.DocDir);
    //RelPathName(dragFn, doc->IDTable.DocDir);
    //absName2 = doc->IDTable.IDTab[0]->FileName;
    //AbsPathName(absName2, doc->IDTable.DocDir);
    if (!dragDoc && SameFile(doc->IDTable.IDTab[0]->FileName, doc->IDTable.DocDir, clipDocFn)) {
      dragDoc = doc;
      view = (CLavaPEView*)doc->DragView;
      if (view && view->pDeclDragP && view->CollectDECL) {
        /*
        CTreeItem* dragParent = view->Tree->GetParentItem(view->m_hitemDrag);
        TItemData* dd = (TItemData*)view->Tree->GetItemData(dragParent);
        DWORD d4 = ((TItemData*)view->Tree->GetItemData(dragParent))->synEl;
        */
        FIRSTLAST(doc, firstLast);
        DWORD d4 = (DWORD)view->pDeclDragP;
        DString* str2 = 0;
        if (view->CollectDECL->FullName.l)
          str2 = new DString(view->CollectDECL->FullName);
        hint = new CLavaPEHint(CPECommand_Delete, doc, firstLast, (DWORD)view->CollectDECL, (DWORD)str2, (DWORD)view->CollectPos, d4, 0);
        doc->UpdateDoc(view, FALSE, hint);
        view->pDeclDragP = 0;
        delete hint; //??
      }
    }
    cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
    while (cheSyn && ((dropIncl<0) || (dragIncl<0))) {
      if ( SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, absDropFn))//, doc->IDTable.DocDir))
        dropIncl = cheSyn->data.nINCL;
      if (SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, clipDocFn)) 
        dragIncl = cheSyn->data.nINCL;
      cheSyn = (CHESimpleSyntax*)cheSyn->successor;
    }
    if ((dragIncl >= 0) && (dropIncl >= 0)) {
      oldTopDECL = ((CHESimpleSyntax*)doc->mySynDef->SynDefTree.first)->data.TopDef.ptr;
      newTopDECL = NewLavaDECL();
      *newTopDECL = *oldTopDECL;
      //Change IDs
      doc->IDTable.StartClipIDs(dragIncl, dropIncl, &this->IDTable);
      IDTable.StartClipIDs(dragIncl, dropIncl, &this->IDTable);
      doc->IDTable.ChangeRefsToClipIDs(newTopDECL);
      if (doc->IDTable.hasRefToClipID) {
        ((CHESimpleSyntax*)doc->mySynDef->SynDefTree.first)->data.TopDef.ptr = newTopDECL;
        doc->IDTable.DownChange(&((CHESimpleSyntax*)doc->mySynDef->SynDefTree.first)->data.TopDef.ptr);
        str = new DString(newTopDECL->LocalName);
        FIRSTLAST(doc, firstLast);
        hint = new CLavaPEHint(CPECommand_Change, doc, firstLast, (DWORD)oldTopDECL, (DWORD)str, 0,  (DWORD)&((CHESimpleSyntax*)doc->mySynDef->SynDefTree.first)->data.TopDef.ptr);
        doc->UndoMem.AddToMem(hint);
        UpdateNo++;
      }
      else {
        doc->IDTable.DragINCL = 0;
        doc->IDTable.DropINCL = 0;
        delete newTopDECL;
      }
    }
  } 
}


void CLavaPEDoc::UpdateOtherDocs(wxDocument* skipOther, DString& inclFile, int newINCL, bool synDel, SynFlags flag)
{
  CHESimpleSyntax* cheSyn;
  CLavaPEDoc* doc;
  CLavaPEHint* hint;
  DString absDropFn, str, relInclFile; //,absName;
  CHAINX chain;
  CHE* che;
  POSITION pos; 
  bool isNew;
  CExecSetImpls *impls;
  wxDocManager* mana = wxDocManager::GetDocumentManager() ;

  absDropFn = IDTable.IDTab[0]->FileName;
  AbsPathName(absDropFn, IDTable.DocDir);

  pos = mana->GetFirstDocPos();
  NSTLavaDECL topDef;
  DString oldTopName;
  if (skipOther && !((CLavaPEView*)((CLavaPEDoc*)skipOther)->UndoMem.DrawTree))
    flag.INCL(noDrawHint);
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);
    //dropFn = absName;
    //RelPathName(dropFn, doc->IDTable.DocDir);
    if (inclFile.l) {
      relInclFile = inclFile;
      RelPathName(relInclFile, doc->IDTable.DocDir);
    }
    cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
    if (cheSyn)
      cheSyn = (CHESimpleSyntax*)cheSyn->successor;
    for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName,doc->IDTable.DocDir, absDropFn);//,doc->IDTable.DocDir);
           cheSyn = (CHESimpleSyntax*)cheSyn->successor);
    if (cheSyn) {
      if (inclFile.l) {
        if (synDel)
          doc->IDTable.RemoveFromInclTrans(cheSyn->data.nINCL, relInclFile);
        else {
          QString inclFile_q=inclFile.c;
          if (doc->IncludeSyntax(inclFile_q, isNew)) {
            if (isNew) {
              doc->Modify(true);
              impls = new CExecSetImpls(doc->mySynDef);
              delete impls;
              doc->UpdateNo++;
              if (skipOther)
                doc->drawView = true;
              else {
                hint = new CLavaPEHint(CPECommand_FromOtherDoc, doc, flag);
                doc->UpdateAllViews(NULL, CHLV_showError, hint);
                delete hint;
              }
            }
            else
              doc->IDTable.SetInclTransValid(cheSyn->data.nINCL, relInclFile, newINCL);
          }
        }
      }
      else {
        topDef.ptr = cheSyn->data.TopDef.ptr;
        cheSyn->data.TopDef.ptr = 0;
        cheSyn->data.TopDef = ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef;
        doc->IDTable.AddSimpleSyntax(mySynDef, IDTable.DocDir, true, cheSyn);
        if (cheSyn->data.LocalTopName.l) {
          oldTopName = cheSyn->data.TopDef.ptr->LocalName;
          doc->IDTable.SetAsName(cheSyn->data.nINCL, cheSyn->data.LocalTopName, oldTopName, cheSyn->data.TopDef.ptr);
        }
        doc->UpdateNo++;
        if (skipOther)
          doc->drawView = true;
        else {
          hint = new CLavaPEHint(CPECommand_FromOtherDoc, doc, flag);//(const unsigned long) 0);
          doc->changeInUpdate = false;
          impls = new CExecSetImpls(doc->mySynDef);
          delete impls;
          doc->UpdateAllViews(NULL, CHLV_showError, hint);
          if (doc->changeInUpdate) {
            che = NewCHE((AnyType*)doc);
            chain.Append(che);
            doc->changeInUpdate = false;
          }
          delete hint;
        }
        UpdateNo++;
        topDef.Destroy();
      }
    }
  }
  for (che = (CHE*)chain.first; che; che = (CHE*)che->successor) {
    ((CLavaPEDoc*)che->data)->UpdateOtherDocs(0, str, 0, false, flag);
    che->data = 0;
  }
}
