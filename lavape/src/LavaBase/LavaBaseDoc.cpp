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

#include "MachDep.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "BAdapter.h"
#include "SylTraversal.h"
#include "SynIO.h"
#include "Convert.h"
#include "docview.h"

#include "qmessagebox.h"
#include "qobject.h"
#include "qstring.h"
#include "qdir.h"

#pragma hdrstop

DebugStep nextDebugStep=noStep;

unsigned stepOutStackDepth, currentStackDepth;


#ifndef WIN32
jmp_buf contOnHWexception;
CHWException hwException(0,0);
#endif

      
void ASN1tofromAr::getChar (unsigned char& c)   
{
  if (Ar->atEnd())
    SetSkip(true);
  Ar->readRawData((char*)&c,1);
}
 
void ASN1tofromAr::putChar (const unsigned char& c)
{
  Ar->writeRawData((char*)&c,1);
  /*
  if (actPos >= (int)Ar->size()) {
    actLen += 10000;
    Ar->resize(10000);
  }
  (*Ar)[actPos] = c;
  actPos++;
  */
}
 
static const unsigned PrintableString = 4; /* actually 19 */


void ASN1tofromAr::GETCString (QString* s)
{
  bool eoc;
  unsigned i, unusedBits;
  DString ds;

  if (skip) return;
  ds.Reset(0);
  getString(Universal,PrintableString,ds,eoc,unusedBits);
  ds[ds.l] = '\0';
  if (ds.l > 0) {
    for (i = 0; i < ds.l; i++)
      ds[i] = (char)MachDep.FromASCII[(unsigned char)ds[i]];
    NewQString(s, ds.c);
    //for (i = 0; i < ds.l; i++)
      //s->SetAt(i, (char)MachDep.FromASCII[(unsigned char)ds[i]]);
  }
  else
    NewQString(s, "");
}


void ASN1tofromAr::PUTCString (QString* s)
{
  Header header;
  unsigned char outChar;
  unsigned i;

  if (skip)
    return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = PrintableString;
  // PrintableString 
  header.LengthForm = DefiniteLength;
  if (s->length())
    header.Len = s->length();
  else
    header.Len = 0;
  PutHeader(header);
  if (header.Len > 0)
    for (i = 0; i <= header.Len-1; i++) {
      outChar = MachDep.ToASCII[s->at(i).toLatin1()];
      putChar(outChar);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
 
CLavaBaseDoc::CLavaBaseDoc()
{
  mySynDef=NULL;
  hasIncludes = false;
  throwError = false;
  isObject = false;
  debugOn = false;
  RuntimeView = 0;
  inSaveProc = false;
  DocObjects[0] = 0;
  DocObjects[1] = 0;
  DocObjects[2] = 0;
  ThreadList = 0;
  DumpFrame=0;
  ActLavaDialog = 0;
  openLavaPEforDebugging = false;
}

CLavaBaseDoc::~CLavaBaseDoc()
{
  if (ThreadList)
    delete ThreadList;
  DeleteContents();
  mySynDef = 0;
}

void CLavaBaseDoc::CalcNames(const QString& FileName)
{
  int il;

  IDTable.DocName = qPrintable(FileName);
  IDTable.FileExtension = qPrintable(FileName);
  IDTable.mySynDef = mySynDef;
  for (il = IDTable.DocName.l-1; il && (IDTable.DocName[il] != '.'); il--);
  if (il) {
    IDTable.FileExtension.Delete(0, il);
    IDTable.DocName.Delete(il, IDTable.DocName.l-il);
  }
  for (il = IDTable.DocName.l-1; il && (IDTable.DocName[il] != '\\') && (IDTable.DocName[il] != '/'); il--);
  if (il) {
    IDTable.DocName.Delete(0, il+1);
    IDTable.DocDir = qPrintable(FileName);
    IDTable.DocDir.Delete(il+1, IDTable.DocDir.l);
  }
  else
    IDTable.DocDir.Reset(0);
}


int CLavaBaseDoc::ReadSynDef(const QString& fn, SynDef* &sntx, ASN1* cid)
{
  //is the sntx open?
  DString docDir, stdName;
  int rr = SynIO.ReadSynDef(fn, sntx, cid);
  if ((rr < 0) || !sntx) 
    return -1;

  CHESimpleSyntax *cheSyn;
  QFileInfo fi;
  QString qfn;
  cheSyn = (CHESimpleSyntax*)sntx->SynDefTree.first;
  if (!cheSyn->data.UsersName.l)
    cheSyn->data.UsersName = DString(qPrintable(fn));
  RelPathName(cheSyn->data.UsersName, IDTable.DocDir);
  cheSyn->data.SyntaxName = DString(qPrintable(fn));
  docDir = qPrintable(fn);
  CalcDirName(docDir);
  RelPathName(cheSyn->data.SyntaxName, docDir);
  cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  QDir dir(docDir.c);
  while (cheSyn) {
    cheSyn->data.SyntaxName = cheSyn->data.UsersName;
    fi.setFile(dir, cheSyn->data.SyntaxName.c);
    qfn = ResolveLinks(fi);
    cheSyn->data.SyntaxName = DString(qPrintable(qfn));
    RelPathName(cheSyn->data.SyntaxName, docDir);
    cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  }
  if (SameFile(qPrintable(fn), qPrintable(StdLava)))
    ((CHESimpleSyntax*)sntx->SynDefTree.first)->data.SyntaxName = DString("std.lava");
  else {
    if (sntx->SynDefTree.first->successor) {
      //stdName = ((CHESimpleSyntax*)sntx->SynDefTree.first->successor)->data.SyntaxName;
      //if (!SameFile(stdName, docDir, DString(StdLava))) {
        stdName = DString(qPrintable(StdLavaLog));
        RelPathName(stdName, docDir);
        ((CHESimpleSyntax*)sntx->SynDefTree.first->successor)->data.SyntaxName = stdName;
        ((CHESimpleSyntax*)sntx->SynDefTree.first->successor)->data.UsersName = stdName;
      //}
    }
  }
  return 0;
}


DString CLavaBaseDoc::GetAbsSynFileName()
{
  return IDTable.DocDir + IDTable.IDTab[0]->FileName;
}

CHESimpleSyntax* CLavaBaseDoc::AttachSyntax(CheckData& ckd, QString& fn)
{  //Add a new include at runtime
  bool isNew;
  QString* errCode;
  LavaDECL *errDECL;
  CHESimpleSyntax* syn;
  //DString fn = DString(*lpszPathName);

  IDTable.lastImpl = 0;
  syn = IncludeSyntax(fn, isNew);
  if (errCode = IDTable.SetImplDECLs(errDECL)) {
    LavaError(ckd, true, errDECL, errCode, 0);
    syn = 0;
  }
  return syn;
}

CHESimpleSyntax* CLavaBaseDoc::IncludeSyntax(const QString& fn, bool& isNew, int hint)
{  //Add a new include 
  SynDef *isyntax = 0;
  CHESimpleSyntax* cheSyn;
  QString str/* = fn*/;
  bool errEx;
  //RelPathName(str, IDTable.DocDir);
  //is mySynDef really new?
  isNew = false;
  for (cheSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
       cheSyn && !SameFile(cheSyn->data.SyntaxName, IDTable.DocDir, qPrintable(fn) /*str, IDTable.DocDir*/);
       cheSyn = (CHESimpleSyntax*)cheSyn->successor);
  if (!cheSyn || !cheSyn->data.TopDef.ptr) {
    isNew = true;
    if ((ReadSynDef(fn, isyntax) < 0) || !isyntax) {
      if (!isyntax) {
        str = QString("File '") + fn + "' not found";
        critical(wxTheApp->m_appWindow,qApp->applicationName(),str);
      }
      return 0;
    }
    if (hint && (isyntax->SynDefTree.first == isyntax->SynDefTree.last)) {
      str = QString("File '") + fn + " is not a valid lava file";
      critical(wxTheApp->m_appWindow,qApp->applicationName(),str);
      SynIO.DeleteSynDef(isyntax);
      return 0;
    }
    DString docDir = qPrintable(fn);
    CalcDirName(docDir);
//    str = fn;
    DString dstr=qPrintable(fn);
    RelPathName(dstr, docDir);
    ((CHESimpleSyntax*)isyntax->SynDefTree.first)->data.SyntaxName = dstr;
    hasIncludes = TRUE;
    cheSyn = AddSyntax(isyntax, fn, errEx, hint);
    SynIO.DeleteSynDef(isyntax);
    if (!LBaseData->inRuntime)
      IDTable.SetImplIDs(false);
  }
  return cheSyn;
}


CHESimpleSyntax* CLavaBaseDoc::AddSyntax(SynDef *syntaxIncl, const QString& fn, bool& errEx, int hint)
{
  //==========>>>>>fn is not a link!
  SynDef *isyntax=0;
  errEx = false;
  CHESimpleSyntax *cheSyn, *plusSyn = 0, *nplusSyn = 0;
  if (!syntaxIncl) 
    return 0;
  CHESimpleSyntax* cheSynIncl = (CHESimpleSyntax*)syntaxIncl->SynDefTree.first;
  if (!cheSynIncl) 
    return 0;
  int in = 0;
  bool inherited = (syntaxIncl != mySynDef);
  DString docDir = qPrintable(fn);
  CalcDirName(docDir);
  if (!inherited) {
    cheSynIncl->data.SyntaxName = IDTable.DocName + IDTable.FileExtension;
    IDTable.MakeTable(mySynDef, isStd);
  }
  else {//not the mySynDef of this document?
    //has the new mySynDef allready an entry or dummy entry?
    DString relFn = qPrintable(fn);
    RelPathName(relFn, IDTable.DocDir);
    for (plusSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
         plusSyn && !SameFile(plusSyn->data.SyntaxName, IDTable.DocDir, qPrintable(fn));//relFn, IDTable.DocDir);
         plusSyn = (CHESimpleSyntax*)plusSyn->successor);
    if (!plusSyn) {
      plusSyn = new CHESimpleSyntax;
      plusSyn->data.UsersName = relFn;
      mySynDef->SynDefTree.Append(plusSyn);
      if (hint == 1)
        IncludeHint(fn, plusSyn);
      else
        if (hint > 1)
          hint = 1;
      plusSyn->data.nINCL = IDTable.AddSimpleSyntax(syntaxIncl, docDir);
    }
    else {
      plusSyn->data.SyntaxName = relFn;
      if (hint || !plusSyn->data.TopDef.ptr)
        plusSyn->data.nINCL = IDTable.AddSimpleSyntax(syntaxIncl, docDir);
    }
    if (!plusSyn->data.TopDef.ptr) {
      plusSyn->data.TopDef.ptr = cheSynIncl->data.TopDef.ptr;
      plusSyn->data.SyntaxName = relFn;
      cheSynIncl->data.TopDef.ptr = 0;
      TID id = TID(((LavaDECL*)plusSyn->data.TopDef.ptr)->OwnID, plusSyn->data.nINCL);
      IDTable.SetVar(id, (DWORD)&plusSyn->data.TopDef.ptr);
      DString oldTopName = ((LavaDECL*)plusSyn->data.TopDef.ptr)->LocalName;
      if (plusSyn->data.LocalTopName.l)
        ((LavaDECL*)plusSyn->data.TopDef.ptr)->LocalName = plusSyn->data.LocalTopName;
      if (plusSyn->data.LocalTopName.l)
        IDTable.SetAsName(plusSyn->data.nINCL, plusSyn->data.LocalTopName, oldTopName, plusSyn->data.TopDef.ptr);
      if (hint == 3) {
        for (nplusSyn = ((CHESimpleSyntax*)plusSyn->successor);
             nplusSyn && !SameFile(nplusSyn->data.SyntaxName, IDTable.DocDir, qPrintable(fn)); //relFn, IDTable.DocDir);
             nplusSyn = (CHESimpleSyntax*)nplusSyn->successor);
        if (nplusSyn) 
          DelSyntax(nplusSyn);
      }
    }
  }
  cheSynIncl = (CHESimpleSyntax*)cheSynIncl->successor;
  //Add the includes of the new mySynDef
  DString fullfn;
  QString fullfn_q;
  int rVal;

  while (cheSynIncl) {
    cheSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first;
    while (cheSyn &&
      !SameFile(cheSyn->data.SyntaxName, IDTable.DocDir, cheSynIncl->data.SyntaxName, docDir))
      cheSyn = (CHESimpleSyntax*)cheSyn->successor;
    if (!cheSyn || !cheSyn->data.TopDef.ptr && !cheSyn->data.inWork) {
      fullfn = cheSynIncl->data.SyntaxName;
      AbsPathName(fullfn, docDir);
      fullfn_q = fullfn.c;
      rVal = ReadSynDef(fullfn_q, isyntax);
      if (cheSyn)
        cheSyn->data.inWork = true;
      hasIncludes = TRUE;
      if (!isyntax) {
        if (!LBaseData->inMultiDocUpdate) {
          DString str = DString("Include-file '") + fullfn/*cheSynIncl->data.SyntaxName*/ + DString("' not found");
          //QMessageBox();//str.c, MB_OK+MB_ICONSTOP);
          critical(wxTheApp->m_appWindow,qApp->applicationName(),str.c);
        }
        cheSynIncl->data.notFound = true;
        if (cheSyn)
          cheSyn->data.notFound = true;
        if (LBaseData->inRuntime || cheSyn && (cheSyn->data.nINCL == 1)) {
          errEx = true;
          return 0;
        }
      }
      else if (rVal < 0)
        return 0;
      else {
        nplusSyn = AddSyntax(isyntax, fullfn_q, errEx, hint);
        if (errEx)
          return 0;
        SynIO.DeleteSynDef(isyntax);
      }
    }
    cheSynIncl = (CHESimpleSyntax*)cheSynIncl->successor;
  }
  return plusSyn;
}


bool CLavaBaseDoc::CheckCompObj(LavaDECL* compoDECL)
{
  LavaDECL *specDecl, *elDecl;
  CHE *cheEl;
  bool ok1, okAll = false;
  CHETID *cheID;
  
  if (!compoDECL)
    return false;
  cheID = (CHETID*)compoDECL->Supports.first;
  if (!cheID)
    return false;
  specDecl = IDTable.GetDECL(cheID->data, compoDECL->inINCL);
  if (!specDecl) {
    new CLavaError(&compoDECL->DECLError1, &ERR_NoCompoSpec);
    return false;
  }
  cheEl = (CHE*)compoDECL->NestedDecls.first;
  while (cheEl && !okAll) {
    elDecl = (LavaDECL*)cheEl->data;
    if (elDecl->DeclType == Interface && elDecl->TypeFlags.Contains(isComponent)) {
      cheID = (CHETID*)specDecl->Supports.first;
      ok1 = true;
      while (cheID && ok1) {
        ok1 = IDTable.IsA(TID(elDecl->OwnID, elDecl->inINCL),0, cheID->data, specDecl->inINCL);
        cheID = (CHETID*)cheID->successor;
      }
      if (ok1 && elDecl->TypeFlags.Contains(isComponent)) {
        elDecl->TypeFlags.INCL(thisComponent);
        if (LBaseData->inRuntime )
          compoDECL->RelatedDECL = elDecl;
      }
      else
        elDecl->TypeFlags.EXCL(thisComponent);
      okAll = ok1;
    }
    cheEl = (CHE*)cheEl->successor;
  }
  return okAll;
}

TID CLavaBaseDoc::GetMappedVType(TID paramID, CContext &context, CheckData* pckd )
{
  //returns the TID of the mapped paramID and the new calculated Context
  CHETVElem* El;
  LavaDECL *decl;
  CContext con;
  if (context.iContext) {
    decl = context.iContext;
    switch (decl->DeclType) {
      case IAttr:
      case OAttr:
        decl = decl->ParentDECL->ParentDECL;
        break;
      case Attr:
      case Function:
      case VirtualType:
        decl = decl->ParentDECL;
        break;
      default: ;
    }
    if (decl->DeclType == Impl) 
      decl = IDTable.GetDECL(((CHETID*)decl->Supports.first)->data, decl->inINCL);
  }
  else 
    decl = context.oContext;
  if (!decl) {
    decl = IDTable.GetDECL(paramID);
    if (decl) {
      decl = decl->ParentDECL;
      NextContext(decl, context);
    }
  }
  IDTable.GetPattern(decl, con);
  if (!con.oContext || IDTable.IsA(context.oContext, TID(con.oContext->OwnID,con.oContext->inINCL), 0))
    con.oContext = context.oContext;
  if (con.iContext) {
    if (!MakeVElems(con.iContext, pckd))
      return TID(-1, 0);
    for (El = (CHETVElem*)con.iContext->VElems.VElems.first; El; El = (CHETVElem*)El->successor) {
      if (IDTable.IsA(El->data.VTEl, 0, paramID, 0)) {
        NextContext(con.iContext, context);
        return El->data.VTEl;
      }
    }
  }
  if (con.oContext) {
    if (!MakeVElems(con.oContext, pckd))
      return TID(-1, 0);
    for (El = (CHETVElem*)con.oContext->VElems.VElems.first; El; El = (CHETVElem*)El->successor) {
      if (IDTable.IsA(El->data.VTEl, 0, paramID, 0)) {
        NextContext(con.oContext, context);
        return El->data.VTEl;
      }
    }
  }
  return TID(-1, 0);
}


LavaDECL* CLavaBaseDoc::GetTypeAndContext(LavaDECL *decl, CContext &context)
{
  //returns the (not mapped) value of the virtual type
  //and the (static LPC) iContext and oContext of the virtual type
  TID tidObject=TID(IDTable.BasicTypesID[B_Object],isStd?0:1);

  if (!decl || decl == (LavaDECL*)-1)
    return 0;
  NextContext(decl, context);
	if (decl->TypeFlags.Contains(substitutable))
		context.ContextFlags = SET(multiContext,-1);
  if (decl->DeclType == VirtualType) {
    if (decl->RefID.nID >= 0)
      decl = IDTable.GetDECL(decl->RefID, decl->inINCL);
    else
      decl = IDTable.GetDECL(tidObject);
//      decl = 0;
    NextContext(decl, context); //18.05.01 ???neue Probleme
  }
  return decl;
}

bool CLavaBaseDoc::MemberTypeContext(LavaDECL *memDECL, CContext &context, CheckData* pckd)
{  
  // remember: a (multiform-) type of a member of some class,
  //  which is interpreted static in the class, must be interpreted static in any
  //  derivation of this class, even if the type is now in the context of the
  //  derivation.
  //
  // context is the context of some class, which has inherited a member memDECL
  // from a base class 

  if (!context.oContext)
    return true;
  CContext conLow, conLowFin;
  LavaDECL  *typeDECL, *pred = 0, *bMemDECL = memDECL;
  while (bMemDECL && bMemDECL->Supports.first && (bMemDECL->RefID.nID >= 0)) {
    pred = bMemDECL;
    bMemDECL = IDTable.GetDECL(((CHETID*)bMemDECL->Supports.first)->data, bMemDECL->inINCL);
  }
  if (bMemDECL && (bMemDECL->RefID.nID < 0))
    bMemDECL = pred;
  if (bMemDECL) {
    IDTable.GetPattern(bMemDECL, conLow);
    conLowFin = conLow;
    typeDECL = GetFinalMTypeAndContext(bMemDECL->RefID, bMemDECL->inINCL, conLowFin, pckd);
    if (pckd && pckd->exceptionThrown)
      return false;
    if (typeDECL && conLowFin.ContextFlags.Contains(staticContext)) 
      context = conLow;
  }
  return true;
}

void CLavaBaseDoc::NextContext(LavaDECL *decl, CContext &context)
{  //the result context is the LPC of decl, if inputs context.iContext=0 and context.oContext=0
  LavaDECL* declP = 0;
  CContext con;
	if (context.ContextFlags.Contains(undefContext))
		return;
  if (decl) {
    declP = decl;
    switch (declP->DeclType) {
      case IAttr:
      case OAttr:
        declP = declP->ParentDECL->ParentDECL;
        break;
      case Attr:
      case Function:
      case VirtualType:
        declP = declP->ParentDECL;
        break;
      default: ;
    }
    if (declP->DeclType == Impl) 
      declP = IDTable.GetDECL(((CHETID*)declP->Supports.first)->data, declP->inINCL);
  } 
  IDTable.GetPattern(declP, con);
  if (!con.iContext && !con.oContext) {
    context.iContext = 0;
    context.oContext = 0;
    context.ContextFlags = SET(staticContext,-1);
  }
  else {
    if (context.ContextFlags.Contains(multiContext)) {
      context.ContextFlags.EXCL(multiContext);
      context.ContextFlags.INCL(undefContext);
      return;
    }
    if (con.oContext && !con.iContext && context.iContext
        && IDTable.IsA(context.iContext, TID(con.oContext->OwnID, con.oContext->inINCL), 0))
      return;
    else {
      if (!con.iContext || !context.iContext
          || !IDTable.IsA(context.iContext, TID(con.iContext->OwnID, con.iContext->inINCL), 0)) {
        context.iContext = con.iContext;
        context.ContextFlags.EXCL(selfiContext);
      }
      if (!con.oContext || !context.oContext
        || !IDTable.IsA(context.oContext, TID(con.oContext->OwnID, con.oContext->inINCL), 0)) {
        context.oContext = con.oContext;
        context.ContextFlags.INCL(staticContext);
        context.ContextFlags.EXCL(selfoContext);
        context.ContextFlags.EXCL(multiContext);
        context.ContextFlags.EXCL(undefContext);
      }
    }
  }
}



LavaDECL* CLavaBaseDoc::GetFinalMVType(const TID& id, int inINCL, CContext &context, Category &cat, CheckData* pckd)
{
  LavaDECL *decl = IDTable.GetDECL(id, inINCL);
  if (!decl)
    return 0;
  return GetFinalMVType(decl, context, cat, pckd);
}


LavaDECL* CLavaBaseDoc::GetFinalMVType(LavaDECL *decl, CContext &context, Category &cat, CheckData* pckd)
{
  //returns the (final) mapped final virtual type
  //and the new calculated oContext
  
  LavaDECL *declVal, *declMapped, *startDecl=0; 
  TID pID;
  bool inContext = true;

  if (!decl)
    return 0;
  declVal = decl;
  declMapped = decl;
  cat = unknownCat;
  if (decl->DeclType != VirtualType) {
    NextContext(decl, context);
    return decl;
  }
  else {
    if (decl->TypeFlags.Contains(definesObjCat)
    && decl->TypeFlags.Contains(definiteCat))
      if (decl->TypeFlags.Contains(isStateObjectY))
        cat = stateObjectCat;
      else if (decl->TypeFlags.Contains(isAnyCatY))
        cat = anyCat;
      else
        cat = valueObjectCat;
  }
  while  (declVal   &&  (declVal->DeclType == VirtualType)) {
    if (inContext) {
      pID = GetMappedVType(TID(declVal->OwnID, declVal->inINCL), context, pckd);
      inContext = (pID.nID >= 0);
      if (inContext) {
        declMapped = IDTable.GetDECL(pID);
        if (!declMapped || (declMapped == startDecl))
          return 0;
        if (!startDecl)
          startDecl = declMapped;
      }
    }
    else
      declMapped = declVal;
    if (declMapped->RefID.nID >= 0) {
      if ((cat == unknownCat)
           && declMapped->TypeFlags.Contains(definesObjCat)
           && declMapped->TypeFlags.Contains(definiteCat))
        if (declMapped->TypeFlags.Contains(isStateObjectY))
          cat = stateObjectCat;
        else if (declMapped->TypeFlags.Contains(isAnyCatY))
          cat = anyCat;
        else
          cat = valueObjectCat;
      declVal = IDTable.GetDECL(declMapped->RefID, declMapped->inINCL);
      if (!declVal && (declVal != startDecl))
        return 0;
      if (!inContext) 
        NextContext(declVal, context);
    }
    else
      return declMapped; //abstract virtual type
    if (declMapped->TypeFlags.Contains(substitutable)) {
      context.ContextFlags = SET(multiContext,-1);
      inContext = true;
    }
  }
  return declMapped;
}

LavaDECL* CLavaBaseDoc::GetFinalMTypeAndContext(const TID& id, int inINCL, CContext &context, CheckData* pckd)
{
  //returns the final value of the (final) mapped final virtual value
  //and the new calculated oContext

  LavaDECL *decl = IDTable.GetDECL(id, inINCL);
  TID tidObject=TID(IDTable.BasicTypesID[B_Object],isStd?0:1);
  Category cat;

  if (!decl)
    return 0;
  if  (decl->DeclType == VirtualType) {
    decl = GetFinalMVType(id, inINCL, context, cat, pckd);
    if (decl && (decl->RefID.nID >= 0)) {
      if (decl->TypeFlags.Contains(substitutable))
        context.ContextFlags = SET(multiContext,-1);
      return IDTable.GetDECL(decl->RefID, decl->inINCL);
    }
    else
      return IDTable.GetDECL(tidObject);
//      return 0;
  }
  else {
    NextContext(decl, context);
    return decl;
  }
}



LavaDECL* CLavaBaseDoc::GetType(LavaDECL *decl)
{
  //returns the (not mapped) value of the virtual type
  if (decl && (decl->DeclType == VirtualType))
    return IDTable.GetDECL(decl->RefID.nINCL, decl->RefID.nID, decl->inINCL);
  else
    return decl;
}


LavaDECL* CLavaBaseDoc::CheckGetFinalMType(LavaDECL* inDECL, LavaDECL* itypeDECL, CheckData* pckd ) 
{
  //returns the final value of the (static final) mapped final virtual value
  LavaDECL *typeDECL, *oc=0;
  CContext context, con;
  if (!inDECL)
    return 0;
  if (itypeDECL)
    typeDECL = itypeDECL;
  else
    typeDECL = IDTable.GetDECL(inDECL->RefID, inDECL->inINCL);
  if (typeDECL && (typeDECL->ParentDECL == inDECL))
    inDECL = typeDECL;
  IDTable.GetPattern(inDECL, context);
  con = context; //19.12.2000, weil es VTs au�erhalb des Contextes zulie�!
  if (typeDECL && (typeDECL->DeclType == VirtualType)) {
    TID pID = GetMappedVType(TID(typeDECL->OwnID, typeDECL->inINCL), con, pckd);
    if ((pID.nID >= 0) && (con.oContext == context.oContext)) {
      typeDECL = IDTable.GetDECL(pID, 0);
      if (typeDECL && !itypeDECL) 
        inDECL->RefID = pID;
      return typeDECL;
    }
    else
      return 0;
  }
  else
    return typeDECL;
}


QString* CLavaBaseDoc::CheckScope(LavaDECL* elDef)
{
  LavaDECL *intfDECL, *parDECL, *decl;
  CHETID* che;
  if (!elDef)
    return 0;
  if ((elDef->DeclType == VirtualType)
      || (elDef->DeclType == Interface) && elDef->SecondTFlags.Contains(isGUI)
      || (elDef->DeclType == Attr)
      || (elDef->DeclType == IAttr)
      || (elDef->DeclType == OAttr)) {
    parDECL = elDef->ParentDECL;
    intfDECL = parDECL;
    if (parDECL->DeclType == Function) 
      parDECL = parDECL->ParentDECL;
    if (parDECL->DeclType == Impl) 
      intfDECL = IDTable.GetDECL(((CHETID*)parDECL->Supports.first)->data, elDef->inINCL);
    decl = IDTable.GetDECL(elDef->RefID, elDef->inINCL);
    if (decl && (decl->DeclType == Interface)) {
      if (decl && !decl->usableIn(parDECL))
        return &ERR_InvisibleType;
    }
    else //virtual type: use interface
      if (decl && !decl->usableIn(intfDECL))
        return &ERR_InvisibleType;
  }
  if ((elDef->DeclType == VirtualType)
      || (elDef->DeclType == Interface)
      || (elDef->DeclType == Impl)) {
    for (che = (CHETID*)elDef->Supports.first; che; che = (CHETID*)che->successor) {
      decl = IDTable.GetDECL(che->data, elDef->inINCL);
      if (decl && !decl->usableIn(elDef))
        return &ERR_InvisibleBasictype;
    }
  }
  for (decl = IDTable.GetDECL(elDef->RefID, elDef->inINCL);
       decl   &&  (decl->DeclType == VirtualType) && (elDef != decl);
       decl = IDTable.GetDECL(decl->RefID, decl->inINCL));
  if (elDef == decl)
    return &ERR_VirtualCircle;
  return 0;
}

bool CLavaBaseDoc::OverriddenMatch(LavaDECL *decl, bool setName, CheckData* pckd)
{
  //Value (=RefID) of VirtualType, Attr, IAttr or OAttr match 
  //with the value of the overridden VirtualType, Attr IAttr or OAttr
  LavaDECL *iDecl, *cDecl, *nameDecl, *ivalDECL, *cvalDECL;
  TID ivalID, cvalID;
  bool ok = true;
  if (!decl)
    return true;
  bool vt = (decl->DeclType == VirtualType);
  CContext con, icon;
  cDecl = decl;
  if (vt) {
    cvalDECL = decl;
    while (cvalDECL && (cvalDECL->DeclType == VirtualType)
           && !cvalDECL->TypeFlags.Contains(isAbstract)) {
      cDecl = cvalDECL;
      cvalDECL = IDTable.GetDECL(cDecl->RefID, cDecl->inINCL);
    }
  }
  else
    cvalDECL = IDTable.GetDECL(cDecl->RefID, cDecl->inINCL);
  IDTable.GetPattern(cDecl, con);
  if (!cDecl->TypeFlags.Contains(isAbstract) && (cDecl->RefID.nID >= 0)) {
    if (!cvalDECL)
      return true;
    CHETID* che = (CHETID*)decl->Supports.first;
    while (che && ok) {
      iDecl = IDTable.GetDECL(che->data, decl->inINCL);//!
      nameDecl = iDecl;
      if (iDecl)
        if ( vt) {
          ivalDECL = iDecl;
          while (ivalDECL && (ivalDECL->DeclType == VirtualType)
                 && !ivalDECL->TypeFlags.Contains(isAbstract)) {
            iDecl = ivalDECL;
            ivalDECL = IDTable.GetDECL(iDecl->RefID, iDecl->inINCL);
          }
        }
        else
          ivalDECL = IDTable.GetDECL(iDecl->RefID, iDecl->inINCL);
      if (iDecl) {
        if (setName && !decl->SecondTFlags.Contains(enableName)) {
          decl->LocalName = nameDecl->LocalName;
          setName = false;
        }
        if (!iDecl->TypeFlags.Contains(isAbstract) && (iDecl->RefID.nID >= 0)) {
          if (!ivalDECL)
            return true;
          if ((decl->DeclType != VirtualType) && (cvalDECL->DeclType != VirtualType))
            return ivalDECL == cvalDECL;

          ok = IDTable.IsA(cDecl->RefID, cDecl->inINCL, iDecl->RefID, iDecl->inINCL);
          //C-derivation or part of a pattern and its pattern derivation
          if (ok) {
            IDTable.GetPattern(iDecl, icon);
            if (!IsCDerivation(cvalDECL, ivalDECL, pckd) && (!pckd || !pckd->exceptionThrown)) {
              ivalID = TID(ivalDECL->OwnID,ivalDECL->inINCL);
              cvalID = TID(cvalDECL->OwnID,cvalDECL->inINCL);
              if ((icon.oContext) && (ivalID == GetMappedVType(ivalID, icon,0)) && (cvalID == GetMappedVType(cvalID, con,0))) {
                //ivalDECL->isInSubTree(icon.oContext) && cvalDECL->isInSubTree(con.oContext)) {
                if (!icon.iContext && con.iContext)
                  ok = IDTable.IsA(con.iContext, TID(icon.oContext->OwnID, icon.oContext->inINCL), 0); 
                else
                  if (icon.iContext)
                    ok = con.iContext && IDTable.IsA(con.iContext, TID(icon.iContext->OwnID, icon.iContext->inINCL),0);
                  else
                    ok = con.oContext && IDTable.IsA(con.oContext, TID(icon.oContext->OwnID, icon.oContext->inINCL),0);
              }
              else
                return false;
            }
            else
              if (pckd && pckd->exceptionThrown)
                return false;
          }
        }

      }
      che = (CHETID*)che->successor;
    }
  }
  return ok;
}

QString* CLavaBaseDoc::ExtensionAllowed(LavaDECL* decl, LavaDECL* baseDECL, CheckData* pckd)
{ //note: do not use this function in Boxes.cpp
  LavaDECL *valDECL, *decl2;
  CContext con;
  bool sameContext;
  CHETID *cheTID;

  if (!decl)
    return 0;
  if (!baseDECL->usableIn(decl))
    return &ERR_InvisibleBasictype;
  IDTable.GetPattern(baseDECL, con);
  if (!con.oContext || (con.oContext == baseDECL))
    return 0;
  if (baseDECL->DeclType == VirtualType) {
    if (!IDTable.IsA(decl/*->ParentDECL*/, TID(baseDECL->ParentDECL->OwnID, baseDECL->inINCL), 0))
      return &ERR_IllegalVTExtension;
    if (baseDECL->TypeFlags.Contains(isAbstract))
       if (IDTable.lowerOContext(decl, baseDECL, sameContext)/* && sameContext*/)
         return 0;
       else
         return &ERR_OverInOtherCon;
    valDECL = GetType(baseDECL);
    if (!valDECL)
      return &ERR_NoBaseIF;
    if  (IDTable.IsA(valDECL, TID(decl->OwnID, decl->inINCL), 0))
      return &ERR_IllegalExtension;
    if (TID(valDECL->OwnID,valDECL->inINCL) != TID(IDTable.BasicTypesID[B_Object],isStd?0:1)) {
      decl2 = NewLavaDECL();

      *decl2 = *decl;
      decl2->SectionTabPtr = 0; // to avoid being destroyed in ~LavaDECL
      decl2->runTimeData = 0;

      for (cheTID = (CHETID*)decl2->Supports.first; cheTID && !IDTable.EQEQ(cheTID->data,decl2->inINCL,TID(baseDECL->OwnID, baseDECL->inINCL),0);
        cheTID = (CHETID*)cheTID->successor);
      if (cheTID)
        decl2->Supports.Remove(cheTID->predecessor);
      if  (IDTable.IsA(decl2,TID(baseDECL->OwnID, baseDECL->inINCL), 0)) {
        delete decl2;
        return &ERR_IllegalExtension;
      }
      delete decl2;
    }
    if (IDTable.lowerOContext(decl, baseDECL, sameContext)
                /*&& sameContext*/ && (CheckGetFinalMType(decl, baseDECL) == baseDECL)) {
      //IDTable.GetPattern(decl, con);
      //((if ((baseDECL->ParentDECL == con.iContext) || (baseDECL->ParentDECL == con.oContext)) {
      //if (baseDECL->RefID.nID == -1)
      //  return 0;
      //if (!valDECL)
      //  return 0;
      //else {
      //  return 0;
      //}
      return 0;
    }
    else
      return &ERR_OverInOtherCon;
  }
  if (!IDTable.lowerOContext(decl, baseDECL, sameContext))
    return &ERR_OverInOtherCon;
  if (!sameContext && IDTable.isValOfVirtual(decl, baseDECL))
    return 0;
  if (sameContext)
    if (/*IsCDerivation(decl, baseDECL, pckd) || !IDTable.isValOfVirtual(decl) &&*/ !IDTable.isValOfVirtual(baseDECL))
      return 0;
    else 
      return &ERR_OverInSameCon;
  return &ERR_OverInOtherCon;
}


QString* CLavaBaseDoc::TypeForMem(LavaDECL* memdecl, LavaDECL* typeDECL, CheckData* pckd)
{
  //note: this function can be called with the ParentDECL instead of memdecl
  CContext con, context;
  LavaDECL* par;
  bool sameContext;
  if (!memdecl || !typeDECL)
    return 0;
  if (!typeDECL->usableIn(memdecl))
    return &ERR_InvisibleType;
/*
  if (memdecl->TypeFlags.Contains(substitutable)
      && !IDTable.otherOContext(memdecl, typeDECL))
    return &ERR_SubstInContext;
*/
  IDTable.GetPattern(typeDECL, con);
  if (!con.oContext)
    return 0;
  if (con.oContext == typeDECL)
    //if (IDTable.isValOfVirtual(typeDECL) && memdecl->isInSubTree(typeDECL))
    //  return &ERR_UseVT;
    //else
      return 0;
  if ((memdecl->DeclType == Attr)
    || (memdecl->DeclType == IAttr)
    || (memdecl->DeclType == OAttr)
    || (memdecl->DeclType == Function)
    || (memdecl->DeclType == Initiator)) {
//    if (memdecl->TypeFlags.Contains(substitutable) && !IDTable.otherOContext(memdecl, typeDECL))
//      return &ERR_SubstInContext;
    par = memdecl->ParentDECL;
    if ((memdecl->DeclType == IAttr)
        || (memdecl->DeclType == OAttr))
      par = par->ParentDECL;
  }
  else
    par = memdecl;
  if (typeDECL->DeclType == VirtualType) {
    TID pID = TID(typeDECL->OwnID, typeDECL->inINCL);
    IDTable.GetPattern(memdecl, context);
    if ( IDTable.lowerOContext(par, typeDECL, sameContext) 
             && (sameContext || (pID == GetMappedVType(pID, context, pckd))))
       if ((typeDECL->RefID.nID < 0) 
            ||  IDTable.GetDECL(typeDECL->RefID, typeDECL->inINCL))
         return 0;
       else
         return &ERR_Broken_ref; //broken reference
    else
      return &ERR_VTOutOfScope;
  }
  else {
    if (memdecl->SecondTFlags.Contains(isGUI))
      return 0;
    if (IDTable.lowerOContext(par, typeDECL, sameContext))
      //if (IDTable.isValOfVirtual(typeDECL))
      //  return &ERR_UseVT;
      //else
        return 0;
    else
      if (IDTable.otherOContext(par, typeDECL))
        return 0;
      else
        return &ERR_WrongContext;
  }
}


QString* CLavaBaseDoc::TestValOfVirtual(LavaDECL* vdecl, LavaDECL* valDECL)
{
  //note: this function can be called with the virtual type-DECL or the ParentDECL of the virtual type instead
  CContext con;
  bool sameContext;
  LavaDECL *par, *val;

  if (!vdecl || !valDECL)
    return 0;
  if (valDECL->DeclType == FormDef)
    val = IDTable.GetDECL(((CHETID*)valDECL->ParentDECL->Supports.first)->data, valDECL->inINCL);
  else
    val = valDECL;
  IDTable.GetPattern(val, con);
  if (!val->usableIn(vdecl))
    return &ERR_InvisibleType;
  if (vdecl->DeclType == VirtualType) {
    par = vdecl->ParentDECL;
  }
  else
    par = vdecl;
  if (par->DeclType == FormDef)
    par = IDTable.GetDECL(((CHETID*)par->ParentDECL->Supports.first)->data, par->inINCL);

  if (val->DeclType == VirtualType)
    if (IDTable.lowerOContext(par, val, sameContext) && (CheckGetFinalMType(par, val) == val)) //sameContext)
      return 0;
    else
      return &ERR_WrongContext;
  else  {
    if (!con.oContext || (con.oContext == val))
      return 0;
    if (IDTable.lowerOContext(par, val, sameContext) )
      if (IDTable.isValOfOtherVirtual(val, vdecl))
        return &ERR_DoubleVal;
      else
        return 0;
    else
      return &ERR_WrongContext;
  }
}


bool CLavaBaseDoc::IsCDerivation(LavaDECL *decl, LavaDECL *baseDecl, CheckData* pckd)
{
  LavaDECL* elDECL;
  CHETVElem *ElBase, *El;

  if (!decl || !baseDecl || (decl == baseDecl))
    return true;
  if (!MakeVElems(decl, pckd))
    return false;
  TID baseID = TID(baseDecl->OwnID, baseDecl->inINCL);
  for (El = (CHETVElem*)decl->VElems.VElems.first;
       El && (El->data.VTClss != baseID);
       El = (CHETVElem*)El->successor);
  if (!El) //if the baseDecl has no virtual type, no member and no function
    return IDTable.IsA(decl, TID(baseDecl->OwnID, baseDecl->inINCL), 0);
  for (ElBase = (CHETVElem*)baseDecl->VElems.VElems.first; ElBase; ElBase = (CHETVElem*)ElBase->successor) {
    elDECL = IDTable.GetDECL(ElBase->data.VTEl);
    if (elDECL && (elDECL->DeclType == VirtualType)) {
      for (El = (CHETVElem*)decl->VElems.VElems.first;
           El && (ElBase->data.VTEl != El->data.VTEl);
           El = (CHETVElem*)El->successor);
      if (!El)
        return false;
    }
  }
  return true;
}


QString* CLavaBaseDoc::CommonContext(LavaDECL* paramDECL)
{  
  CContext con, con1;
  CHETVElem *El;
  LavaDECL* elDECL;

  IDTable.GetPattern(paramDECL, con);
  if (con.iContext)
    IDTable.GetPattern(con.oContext, con1);
  else
    return 0;
  if (con1.iContext)
    return &ERR_NestedVTs;
  MakeVElems(con.oContext, 0);
  for (El = (CHETVElem*)con.oContext->VElems.VElems.first;
       El ;
       El = (CHETVElem*)El->successor) {
    elDECL = IDTable.GetDECL(El->data.VTEl);
    if (elDECL && (elDECL->DeclType == VirtualType) && IDTable.Overrides(elDECL, paramDECL))
      return &ERR_CommonContext;
  }
  return 0;
}

QString* CLavaBaseDoc::okPattern(LavaDECL* decl)
{//Interface and Package only
  CContext con;
  CHETVElem *Eli, *Elo;
  LavaDECL* elDECLi, *elDECLo;

  if (!decl)
    return 0;
  IDTable.GetPattern(decl, con);
  if (!con.iContext && !con.oContext)
    return 0;
  MakeVElems(decl, 0);
  MakeVElems(con.oContext, 0);
  if (con.iContext && (decl == con.iContext)) {
    for (Eli = (CHETVElem*)decl->VElems.VElems.first;
         Eli ;
         Eli = (CHETVElem*)Eli->successor) {
      elDECLi = IDTable.GetDECL(Eli->data.VTEl);
      if (elDECLi && (elDECLi->DeclType == VirtualType)) 
        for (Elo = (CHETVElem*)con.oContext->VElems.VElems.first;
             Elo ;
             Elo = (CHETVElem*)Elo->successor) {
          elDECLo = IDTable.GetDECL(Elo->data.VTEl);
          if (elDECLo && (elDECLo->DeclType == VirtualType) && (Eli->data.VTClss == Elo->data.VTClss))
            return &ERR_CommonContext;
        }
    }
  }
  return 0;
}

SynFlags CLavaBaseDoc::GetCategoryFlags(LavaDECL* memDECL, bool& catErr)
//return true: category of Attr, IAttr or OAttr
//             is defined by the referenced virtual type
{
  bool basedefsCat = false;
  bool refdefsCat = false;
  bool basetrueCat = false;
  bool reftrueCat = false;
  bool isStateOb = false;
  bool isAnyCat = false;
  bool baseabstract = false;
  bool baseSubst = false;
  CHETID* che;
  LavaDECL* decl=0;

  catErr = false;
  SynFlags inheritedFlags;
  if (!memDECL)
    return inheritedFlags;
  if (memDECL->SecondTFlags.Contains(overrides)) {
    for (che = (CHETID*)memDECL->Supports.first; che; che = (CHETID*)che->successor) {
      decl = IDTable.GetDECL(che->data, memDECL->inINCL);
      if (decl) {
        basedefsCat = basedefsCat || decl->TypeFlags.Contains(definesObjCat);
        basetrueCat = basetrueCat || decl->TypeFlags.Contains(definiteCat);
        isStateOb = isStateOb || decl->TypeFlags.Contains(isStateObjectY);
        isAnyCat = isAnyCat || decl->TypeFlags.Contains(isAnyCatY);
        baseabstract = baseabstract || decl->TypeFlags.Contains(isAbstract);
        baseSubst = baseSubst || decl->TypeFlags.Contains(substitutable);
      }
    }
    if (basedefsCat)
      inheritedFlags.INCL(definesObjCat);
    if (baseabstract)
      inheritedFlags.INCL(isAbstract);
    if (basetrueCat) {
      inheritedFlags.INCL(definiteCat);
      if (isStateOb)
        inheritedFlags.INCL(isStateObjectY);
      if (isAnyCat)
        inheritedFlags.INCL(isAnyCatY);
    }
    if (decl && (memDECL->DeclType == Attr)) { 
      if (decl->TypeFlags.Contains(constituent)) 
        memDECL->TypeFlags.INCL(constituent);
      else 
        memDECL->TypeFlags.EXCL(constituent);
      if (decl->TypeFlags.Contains(acquaintance))
        memDECL->TypeFlags.INCL(acquaintance);
      else 
        memDECL->TypeFlags.EXCL(acquaintance);
      if (decl->TypeFlags.Contains(copyOnAccess))
        memDECL->TypeFlags.INCL(copyOnAccess);
      else 
        memDECL->TypeFlags.EXCL(copyOnAccess);
      if (decl->TypeFlags.Contains(consumable))
        memDECL->TypeFlags.INCL(consumable);
      else 
        memDECL->TypeFlags.EXCL(consumable);
      if (decl->TypeFlags.Contains(hasSetGet)) 
        memDECL->TypeFlags.INCL(hasSetGet);
      else 
        memDECL->TypeFlags.EXCL(hasSetGet);   
      if (baseSubst)
        memDECL->TypeFlags.INCL(substitutable);
      else
        if (!baseabstract)
          memDECL->TypeFlags.EXCL(substitutable);
    }
    else
      if (memDECL->DeclType == VirtualType) {
        if (baseSubst)
          memDECL->TypeFlags.INCL(substitutable);
        else
          if (!baseabstract)
            memDECL->TypeFlags.EXCL(substitutable);
        if (!basetrueCat) {
          isStateOb = memDECL->TypeFlags.Contains(isStateObjectY);
          isAnyCat = memDECL->TypeFlags.Contains(isAnyCatY);
        }
      }
  }
  LavaDECL* refDECL = IDTable.GetDECL(memDECL->RefID, memDECL->inINCL);
  refdefsCat = (refDECL && (refDECL->DeclType == VirtualType)
                        && refDECL->TypeFlags.Contains(definesObjCat));
  if (refdefsCat) {
    inheritedFlags.INCL(definesObjCat);
    reftrueCat = !refDECL->TypeFlags.Contains(isAbstract);
    if (reftrueCat) {
      catErr = basetrueCat && (isStateOb != refDECL->TypeFlags.Contains(isStateObjectY));
      if (!catErr) 
        isStateOb = refDECL->TypeFlags.Contains(isStateObjectY);
      inheritedFlags.INCL(definiteCat);
    }
  }
  else {
    if (!basetrueCat && (memDECL->DeclType != VirtualType)) {
      isStateOb = memDECL->TypeFlags.Contains(isStateObjectY);
      isAnyCat = memDECL->TypeFlags.Contains(isAnyCatY);
    }
  }
  /* alt:
  if (basedefsCat || refdefsCat) 
    memDECL->TypeFlags.INCL(definesObjCat);
  else 
    if (memDECL->DeclType != VirtualType)
      memDECL->TypeFlags.EXCL(definesObjCat);
  */
  //neu:
  if (memDECL->DeclType == VirtualType)
    if (basedefsCat || refdefsCat) 
      memDECL->TypeFlags.INCL(definesObjCat);
//    else   //dies mu� so (weg-)bleiben, sonst verliert der VT seine definesObjCat-Eigenschaft
//      memDECL->TypeFlags.EXCL(definesObjCat);
  if (basetrueCat || reftrueCat) 
    memDECL->TypeFlags.INCL(definiteCat);
  else 
    if (memDECL->DeclType == VirtualType) {
      if (memDECL->TypeFlags.Contains(definesObjCat)
        && !memDECL->TypeFlags.Contains(isAbstract)) {
        memDECL->TypeFlags.INCL(definiteCat);
        isStateOb = memDECL->TypeFlags.Contains(isStateObjectY);
        isAnyCat = memDECL->TypeFlags.Contains(isAnyCatY);
      }
      else
        memDECL->TypeFlags.EXCL(definiteCat);
    }
    else
      if (refDECL && memDECL->TypeFlags.Contains(definesObjCat)
          && !refDECL->TypeFlags.Contains(definiteCat))
         memDECL->TypeFlags.EXCL(definiteCat);
      else
         memDECL->TypeFlags.INCL(definiteCat);
  if (memDECL->TypeFlags.Contains(definiteCat)) {
    if (isStateOb) {
      memDECL->TypeFlags.INCL(isStateObjectY);
      memDECL->TypeFlags.EXCL(isAnyCatY);
    }
    else if (isAnyCat) {
      memDECL->TypeFlags.INCL(isAnyCatY);
      memDECL->TypeFlags.EXCL(isStateObjectY);
    }
    else {
      memDECL->TypeFlags.EXCL(isStateObjectY);
      memDECL->TypeFlags.EXCL(isAnyCatY);
    }
  }
  return inheritedFlags;
}

LavaDECL** CLavaBaseDoc::GetFormpDECL(LavaDECL* decl)
{
  LavaDECL *implDECL = 0, **pFormDECL;
  CHE* che;
  if (decl) {
    if (decl->DeclType == Impl) 
      implDECL = decl;
    else {
      CFindLikeForm *like = new CFindLikeForm(mySynDef, decl->RefID, decl->inINCL, decl, decl);
      pFormDECL = like->pdecl;
      delete like;
      return pFormDECL;
    }
    if (implDECL) {
      for (che = (CHE*)implDECL->NestedDecls.first;
           che && (((LavaDECL*)che->data)->DeclType != FormDef);
           che = (CHE*)che->successor);
      if (che)
        return (LavaDECL**)&che->data;
    }
  }
  return 0;
}

TID CLavaBaseDoc::GetGUIDataTypeID(LavaDECL* GUIclass)
{
  TID dataID;
  LavaDECL *paramDECL;

  IDTable.GetParamID(GUIclass, dataID, isGUI);
  if (dataID.nID < 0) {
    dataID.nID = GUIclass->RefID.nID; 
    dataID.nINCL = IDTable.IDTab[GUIclass->inINCL]->nINCLTrans[GUIclass->RefID.nINCL].nINCL;
  }
  else {
    paramDECL = IDTable.GetDECL(dataID,0);
    dataID.nID = paramDECL->RefID.nID;
    dataID.nINCL = IDTable.IDTab[paramDECL->inINCL]->nINCLTrans[paramDECL->RefID.nINCL].nINCL;
  }
  return dataID;
}

LavaDECL* CLavaBaseDoc::FindInSupports(const DString& name, LavaDECL *self, LavaDECL * decl, bool down, bool onTop)
{
  LavaDECL *topDECL, *inDECL, *returnDECL = 0;
  CHESimpleSyntax* cheSyn;
  TID selfID, elID;
  if (!decl || decl && (decl->DeclDescType != StructDesc)&& (decl->DeclDescType != EnumType))
    return 0;
  if (self && ((self->DeclType == Impl)
               || (self->DeclType == CompObj)))
    return 0;
  if (self) 
    selfID = TID(self->OwnID, self->inINCL);
  CHE* che = (CHE*)decl->NestedDecls.first;
  if (che) 
    elID = TID(((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL);
  while (che 
        && (((LavaDECL*)che->data)->DeclDescType != ExecDesc)
        && ((che->data == self) 
            || (((LavaDECL*)che->data)->LocalName != name)
            || !onTop && ((LavaDECL*)che->data)->TypeFlags.Contains(isInitializer) && self->TypeFlags.Contains(isInitializer)
            || (((LavaDECL*)che->data)->DeclType == Impl)
            || (((LavaDECL*)che->data)->DeclType == CompObj)
            || down && (((LavaDECL*)che->data)->DeclType != VirtualType)
                    && (((LavaDECL*)che->data)->DeclType != Function)
                    && (((LavaDECL*)che->data)->DeclType != Attr)
            || IDTable.IsA(selfID, 0, elID, 0))) {
    che = (CHE*)che->successor;
    if (che) {
      elID = TID(((LavaDECL*)che->data)->OwnID, ((LavaDECL*)che->data)->inINCL);
    }
  }
  if (che && (((LavaDECL*)che->data)->DeclDescType != ExecDesc))
    return (LavaDECL*)che->data;
  else {
    if (down || self && ((self->DeclType == Attr) || (self->DeclType == Function))) {
      CHETID *cheSup = (CHETID*)decl->Supports.first;
      if (cheSup) {
        TID pID;
        while (cheSup && !returnDECL) {
          //pID = cheSup->data;
          //inDECL = GetFinalDef(pID, decl->inINCL, self);
          inDECL = IDTable.GetDECL(cheSup->data, decl->inINCL);
          if (inDECL)  
            returnDECL = FindInSupports(name, self, inDECL, true, false);
          if (returnDECL)
            return returnDECL;
          cheSup = (CHETID*)cheSup->successor;
        }
        if (returnDECL)
          return returnDECL;
      }
    }
  }
  if ((decl == ((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr)
    && !decl->LocalName.l) {
    for (cheSyn = (CHESimpleSyntax*)mySynDef->SynDefTree.first->successor; cheSyn; cheSyn = (CHESimpleSyntax*)cheSyn->successor) {
      topDECL = cheSyn->data.TopDef.ptr;
      if (!topDECL->LocalName.l) {
        for (che = (CHE*)topDECL->NestedDecls.first;
             che && (((LavaDECL*)che->data)->DeclDescType != ExecDesc)
                 && ( ((LavaDECL*)che->data)->LocalName != name);
             che = (CHE*)che->successor);
        if (che && (((LavaDECL*)che->data)->DeclDescType != ExecDesc)
            && (((LavaDECL*)che->data)->DeclType != Impl)
            && (((LavaDECL*)che->data)->DeclType != CompObj))
          return (LavaDECL*)che->data;
      }
    }
  }
  return 0;
}  


int CLavaBaseDoc::CheckHandlerIO(LavaDECL* funcDECL, LavaDECL* ClientMem)
{
  TID refID, clientTypeID;
  LavaDECL *memDECL, *clientType=0, *memsClientType=0, *decl;
  CHETIDs *cheTIDs;
  CHE *cheIO1=0, *cheIO2=0, *cheIO3=0, *cheIO4=0, *cheIO5=0;

  if (ClientMem) {
    memsClientType = IDTable.GetDECL(ClientMem->RefID, ClientMem->inINCL);
    if (memsClientType->TypeFlags.Contains(isGUI) || memsClientType->DeclType == FormDef)
      memsClientType = IDTable.GetDECL(memsClientType->RefID, memsClientType->inINCL);
  }
  cheTIDs = (CHETIDs*)funcDECL->HandlerClients.first;
  if (!cheTIDs)
    return -1;
  if (cheTIDs->data.last) {
    while (cheTIDs) {
      memDECL = IDTable.GetDECL(((CHETID*)cheTIDs->data.last)->data, funcDECL->inINCL);
      if (!memDECL)
        return -1;
      decl = IDTable.GetDECL(memDECL->RefID, memDECL->inINCL);
      if (!decl)
        return -1;
      if (decl->SecondTFlags.Contains(isGUI) || decl->DeclType == FormDef)
        decl = IDTable.GetDECL(decl->RefID, decl->inINCL);
      if (clientType) {
        if (clientType != decl)
          return -1;
      }
      else
        clientType = decl;
      cheTIDs = (CHETIDs*)cheTIDs->successor;
    }
  }
  else
    clientType = IDTable.GetDECL(funcDECL->ParentDECL->RefID, funcDECL->inINCL);

  if (!clientType || (memsClientType && (memsClientType != clientType)))
    return -1;
  clientTypeID = TID(clientType->OwnID, clientType->inINCL);
  cheIO1 = (CHE*)funcDECL->NestedDecls.first;
  if (!cheIO1)
    return -1;
  if (((LavaDECL*)cheIO1->data)->DeclType != IAttr)
    return -1;
  cheIO2 = (CHE*)cheIO1->successor;
  if (!cheIO2)
    return -1;
  cheIO3 = (CHE*)cheIO2->successor;
  if (cheIO3)
    cheIO4 = (CHE*)cheIO3->successor;
  switch (funcDECL->GUISignaltype) {
    case Ev_ValueChanged:
      if (clientType && (clientType->fromBType != B_Bool)
                     && (clientType->fromBType != Char)
                     && (clientType->fromBType != Integer)
                     && (clientType->fromBType != Float)
                     && (clientType->fromBType != Double)
                     && (clientType->fromBType != VLString)
                     && ((clientType->fromBType != NonBasic) || !clientType->SecondTFlags.Contains(isEnum)))
        return -1;
      if (((LavaDECL*)cheIO2->data)->DeclType != IAttr)
        return -1;
      if (!cheIO3 || (((LavaDECL*)cheIO3->data)->DeclType != OAttr))
        return -1;
      if (cheIO4  && (((LavaDECL*)cheIO4->data)->DeclType != ExecDef))
          return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO1->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO2->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO3->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      break;

    case Ev_ChainInsert:
      if (clientType && !clientType->SecondTFlags.Contains(isSet))
        return -1;
      if (((LavaDECL*)cheIO2->data)->DeclType != IAttr)
        return -1;
      if (!cheIO3 || (((LavaDECL*)cheIO3->data)->DeclType != IAttr))
        return -1;
      if (!cheIO4 || (((LavaDECL*)cheIO4->data)->DeclType != OAttr))
        return -1;
      cheIO5 = (CHE*)cheIO4->successor;
      if (!cheIO5 || (((LavaDECL*)cheIO5->data)->DeclType != OAttr))
        return -1;
      if (cheIO5->successor 
          && (((LavaDECL*)((CHE*)cheIO5->successor)->data)->DeclType != ExecDef))
          return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO1->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      if (((LavaDECL*)cheIO2->data)->BType != B_Che)
        return -1;
      IDTable.GetParamRefID(clientType, refID, isSet);
      if (!IDTable.EQEQ(((LavaDECL*)cheIO3->data)->RefID, funcDECL->inINCL, refID, 0) )
        return -1;
      if (((LavaDECL*)cheIO4->data)->BType != B_Bool)
        return -1;
      if (((LavaDECL*)cheIO5->data)->RefID != ((LavaDECL*)cheIO3->data)->RefID)
        return -1;
      break;
    case Ev_ChainDelete:
      if (clientType && !clientType->SecondTFlags.Contains(isSet))
        return -1;
      if (((LavaDECL*)cheIO2->data)->DeclType != IAttr)
        return -1;
      if (!cheIO3 || (((LavaDECL*)cheIO3->data)->DeclType != OAttr))
        return -1;
      if (cheIO4  && (((LavaDECL*)cheIO4->data)->DeclType != ExecDef))
          return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO1->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;    
      if (((LavaDECL*)cheIO2->data)->BType != B_Che)
        return -1;
      if (((LavaDECL*)cheIO3->data)->BType != B_Bool)
        return -1;
      break;
    case Ev_OptInsert:
      if (ClientMem && !ClientMem->TypeFlags.Contains(isOptional))
        return -1;
      if (((LavaDECL*)cheIO2->data)->DeclType != OAttr)
        return -1;
      if (!cheIO3 || (((LavaDECL*)cheIO3->data)->DeclType != OAttr))
        return -1;
      if (cheIO4  && (((LavaDECL*)cheIO4->data)->DeclType != ExecDef))
        return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO1->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      if (((LavaDECL*)cheIO2->data)->BType != B_Bool)
        return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO3->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      break;
    case Ev_OptDelete:
      if (ClientMem && !ClientMem->TypeFlags.Contains(isOptional))
        return -1;
      if (((LavaDECL*)cheIO2->data)->DeclType != OAttr)
        return -1;
      if (cheIO3  && (((LavaDECL*)cheIO3->data)->DeclType != ExecDef))
        return -1;
      if (!IDTable.EQEQ(((LavaDECL*)cheIO1->data)->RefID, funcDECL->inINCL, clientTypeID, 0))
        return -1;
      if (((LavaDECL*)cheIO2->data)->BType != B_Bool)
        return -1;
      break;
    default: return -1;

  }
  return funcDECL->GUISignaltype;
}

/////////////////////////////////////////////////////////////////////////////
// CLavaBaseDoc diagnostics


bool CLavaBaseDoc::DeleteContents() 
{
  CheckData ckd;
  ckd.document = this;

  if (DocObjects[0])
    DEC_FWD_CNT(ckd,DocObjects[0]);
  if (DocObjects[1])
    DEC_FWD_CNT(ckd,DocObjects[1]);
  if (DocObjects[2])
    DEC_FWD_CNT(ckd,DocObjects[2]);

  DocObjects[0] = 0;
  DocObjects[1] = 0;
  DocObjects[2] = 0;
  if (mySynDef)
    SynIO.DeleteSynDef(mySynDef); 
  IDTable.DestroyTable(); 
  return true;
}


bool isPartOf(const DString& name, const DString& ancestorName)
{
  unsigned loc = 1000;
  if ( ( (name[ancestorName.l] == ':') || (name[ancestorName.l] == '.') || (name.l == ancestorName.l))
      && name.Contains(ancestorName, 0, loc))
    return (loc == 0);
  else
    return false;
}

bool CLavaBaseDoc::AllowThrowType(LavaDECL* decl, TID throwID, int inINCL)
{
  if (!decl->SecondTFlags.Contains(overrides))
    return true;
  CHETID *cheS, *cheI;
  bool ok = true;
  LavaDECL *baseDECL;

  cheS = (CHETID*)decl->Supports.first;
  while (cheS && ok) {
    baseDECL = IDTable.GetDECL(cheS->data, decl->inINCL);
    ok = false;
    if (baseDECL) {
      cheI = (CHETID*)baseDECL->Inherits.first;
      while (cheI && !ok) {
        ok = IDTable.IsA(throwID, inINCL, cheI->data, baseDECL->inINCL);
        cheI = (CHETID*)cheI->successor;
      }
      cheS = (CHETID*)cheS->successor;
    }
  }
  return ok;
}

QString* CLavaBaseDoc::CheckException(LavaDECL *funcDecl, CheckData* pckd)
{
  LavaDECL *decl;
  bool sameContext;
  CHETID *cheEx = (CHETID*)funcDecl->Inherits.first;
  while (cheEx) {
    decl = IDTable.GetDECL(cheEx->data, funcDecl->inINCL);
    if (!decl || (decl->DeclType == Interface) && !decl->SecondTFlags.Contains(isException))
      return &ERR_NoException;
    else
    if ((decl->DeclType == VirtualType) 
         && IDTable.lowerOContext(funcDecl->ParentDECL, decl, sameContext)
         && (CheckGetFinalMType(funcDecl->ParentDECL, decl, pckd) == decl)) {
      decl = IDTable.GetDECL(decl->RefID, decl->inINCL);
      if (!decl)  
        return &ERR_MissingExceptionDef;
      else if (!decl->SecondTFlags.Contains(isException))
        return &ERR_NoException;
    }
    if (funcDecl->SecondTFlags.Contains(overrides)
      && !AllowThrowType(funcDecl, cheEx->data, funcDecl->inINCL))
      return &ERR_OverThrow;
    cheEx = (CHETID*)cheEx->successor;
  }
  return 0;
}

QString* CLavaBaseDoc::CheckFormEl(LavaDECL *formEl, LavaDECL *classEl)
{
  LavaDECL *formType, *classType;
  formType = IDTable.GetDECL(formEl->RefID, formEl->inINCL);
  for (;formType && (formType->DeclType == VirtualType);
        formType = IDTable.GetDECL(formType->RefID, formEl->inINCL));
  if (!formType)
    return &ERR_Broken_ref;
  if (!formType->usableIn(formEl)) 
    return &ERR_InvisibleType;
  classType = IDTable.GetDECL(classEl->RefID, classEl->inINCL);
  for (;classType && (classType->DeclType == VirtualType); classType = IDTable.GetDECL(classType->RefID, classType->inINCL));
  if (classType != formType) {
    if (classType && classType->TypeFlags.Contains(isAbstract)) 
      return &ERR_NoAbstractForm;
    if ((formType->DeclType != FormDef)
      || (IDTable.GetDECL(formType->RefID, formType->inINCL) != classType))
      return &ERR_CorruptForm3;
  }
  return 0;
}


CSectionDesc::CSectionDesc()
{
  sectionOffset = -1;     
  funcDesc = 0;
  attrDesc = 0;
  vtypeDesc = 0;
  implDECL = 0;          
  classDECL = 0; 
  nSections = 0;
  innerContext = 0; 
  outerContext = 0; 
  adapterTab = 0;
}

void CSectionDesc::Destroy()
{
  if (!sectionOffset) {
    if (funcDesc)
      delete [] funcDesc;
    if (attrDesc)
      delete [] attrDesc;
    if (vtypeDesc)
      delete [] vtypeDesc;
 }
}

void CalcDirName(DString& dirName)
{
  
  int il = dirName.l-1;
  while (il && (dirName[il] != '\\') && (dirName[il] != '/'))
    il--;
  if (il)
    dirName.Delete(il+1, dirName.l);
  else
    dirName.Reset(0);
}

bool SameFile(const DString& relName1, const DString& dirName1, const DString& relName2, const DString& dirName2)
{
  DString abs1 = relName1;
  AbsPathName(abs1, dirName1);
  DString abs2 = relName2;
  AbsPathName(abs2, dirName2);
  return SameFile(abs1, abs2);
}

bool SameFile(const DString& relName1, const DString& dirName1, const DString& absName2)
{
  DString abs1 = relName1;
  AbsPathName(abs1, dirName1);
  return SameFile(abs1, absName2);
}

bool SameFile(const DString& absName1, const DString& absName2)
{
  
  return absName1 == absName2;
  /*
  if (absName1.l && absName2.l) {
    QString n1 = absName1.c, n2 = absName2.c;
    QFileInfo f1(n1), f2(n2);
    if (f1.isSymLink()) {
      n1 = ResolveLinks(f1);
      f1.setFile(n1);
    }
    if (f2.isSymLink()) {
      n2 = ResolveLinks(f2);
      f2.setFile(n2);
    }
    QString p1 = f1.dir().canonicalPath();
    QString p2 = f2.dir().canonicalPath();
    return (p1 == p2) && (f1.fileName() == f2.fileName()); //!strcasecmp(str1.c,str2.c);
  }
  else
    return !absName1.l && !absName2.l;
    */
}

bool SameDir(const DString& dir1, const DString& dir2)
{
  if (dir1.l && dir2.l) {
    QDir d1(dir1.c), d2(dir2.c);
    QString p1 = d1.canonicalPath();
    QString p2 = d2.canonicalPath();
    return (p1 == p2); 
  }
  else
    return !dir1.l && !dir2.l;
}

void RelPathName(DString& fn, const DString& dirName)
{
  if (dirName.l) {
    int bpos = 0;
    int pos = 0;
    bool rel = false;
    if ((fn[1] == ':') && (tolower(fn[0]) == tolower(dirName[0]))
					|| (fn[0] == '/')) {
			if (fn[0] == '/') {
				fn.Delete(0, 1);
				bpos = 1;
			}
			else {
				fn.Delete(0, 3);
				bpos = 3;
			}
      rel = true;
      while ((pos < fn.l) && (tolower(fn[pos]) == tolower(dirName[bpos]))) {
        if ((fn.c[pos] == '\\') || (fn.c[pos] == '/')) {
          fn.Delete(0, pos+1);
          pos = 0;
        }
        else 
          pos++;
        bpos++;
      }
      if (rel) {
        while (bpos < dirName.l) {
          if ((dirName[bpos] == '\\') || (dirName[bpos] == '/')) 
            fn.Insert(pktpktsl, 0);
          bpos++;
        }
      }
    }
    if ((fn[0] != '\\') && (fn[0] != '/') && (fn[0] != '.') && (fn[1] != ':')) {
      for (pos = 0; (fn[pos] != '\\') && (fn[pos] != '/') && (pos < fn.l); pos++);
      if (pos < fn.l)
        fn.Insert(pktsl, 0);
    }
  }
}


void AbsPathName(DString& fn, const DString& dirName)
{
  if (dirName.l && ((fn[0] == '.') || (fn[0] != '\\') && (fn[0] != '/') && (fn[1] != ':'))) {
    unsigned pos;
    int ll;
    DString nfn = dirName;
    if (fn.Contains(pktsl, 0 , pos) && (pos == 0))
      fn.Delete(0, 2);
    while (fn.Contains(pktpktsl, 0 , pos) && (pos == 0)) {
      fn.Delete(0, 3);
      for (ll = nfn.l-2; (ll >= 0) && (nfn[ll] != '\\') && (nfn[ll] != '/'); ll--);
      if (ll >= 0)
        nfn.Delete(ll+1, nfn.l-ll-1);
    }
    fn = nfn + fn;
  }
  if (fn[0] == '\\'/* || fn[0] == '/'*/) {
    fn.Insert(DString("x:"), 0);
    fn[0] = dirName[0];
  }
}


LavaDECL* CLavaBaseDoc::GetExecDECL(LavaDECL* parentDecl,TDeclType type, bool makeIt, bool makeExec)
{
  LavaDECL *cDECL = 0;
  if (!parentDecl)
    return 0;
  CHE *afterChe = (CHE*)parentDecl->NestedDecls.last;
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
  if (!makeIt)
    return cDECL;
  if (!cDECL) {
    cDECL = NewLavaDECL();
    cDECL->DeclType = type;
    cDECL->DeclDescType = ExecDesc;
    cDECL->FullName = parentDecl->FullName;
    cDECL->ParentDECL = parentDecl;
    CHE* che = NewCHE(cDECL);
    parentDecl->NestedDecls.Insert(afterChe, che);
    if (makeExec) {
      LBaseData->ExecUpdate->MakeExec(cDECL);
      if (parentDecl->OwnID != -1)
        ((SynObjectBase*)cDECL->Exec.ptr)->MakeTable((address)&IDTable, parentDecl->inINCL, (SynObjectBase*)cDECL, onNewID);
    }
  }
  return cDECL;
}

