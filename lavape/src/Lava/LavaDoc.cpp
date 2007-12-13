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


#include "Lava.h"

#include "ASN1.h"
#include "LavaDoc.h"
#include "MakeGUI.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "SynIDTable.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "BAdapter.h"
#include "LavaProgram.h"
#include "Convert.h"
#include "Constructs.h"
#include "qmessagebox.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qfiledialog.h"
#include "qdatastream.h"

#ifdef WIN32
#include "qt_windows.h"
#include "commdlg.h"
#include "winuser.h"
#endif

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CLavaDoc


CLavaDoc::CLavaDoc()
{
  ObjTab = 0;
  MaxTab = 0;
  ActTab = 0;
  Redraw = true;
  firstStore = true;
  newLdoc = false;
  numAllocObjects = 0;
}

bool CLavaDoc::DeleteContents() 
{
  CheckData ckd;
  LavaObjectPtr obj;

  ckd.document = this;
  if (RuntimeView && ((CLavaGUIView*)RuntimeView)->myGUIProg) {
    while (((CLavaGUIView*)RuntimeView)->myGUIProg->allocatedObjects.size()) {
      obj = ((CLavaGUIView*)RuntimeView)->myGUIProg->allocatedObjects.takeFirst();
      DEC_FWD_CNT(ckd,obj);
    }
  }
  return CLavaBaseDoc::DeleteContents(); 
}


bool CLavaDoc::OnCloseDocument()
{
  if (isObject && !ThreadList->isEmpty()) {
    if (((CLavaDebugger*)LBaseData->debugger)->isConnected)
      ((CLavaDebugger*)LBaseData->debugger)->stop(normalEnd);
    m_execThreadPtr->ldocEnd = true;
    m_execThreadPtr->resume();
  }
  return CLavaProgram::OnCloseDocument();
}


CLavaDoc::~CLavaDoc()
{
 if (numAllocObjects) {
   QMessageBox::critical(wxTheApp->m_appWindow, wxTheApp->applicationName(), QString("Memory leak: %1 orphaned Lava object(s)").arg(numAllocObjects),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  }
  //int sz = allocatedObjects.size();
  if (((CLavaApp*)wxTheApp)->debugger.startedFromLavaPE) 
    qApp->exit(0);
  else
    if ((!((wxApp*)qApp)->appExit) && debugOn && (this == ((CLavaApp*)wxTheApp)->debugger.myDoc)) {
      ((CLavaApp*)wxTheApp)->debugger.myDoc = 0;
      ((CLavaApp*)wxTheApp)->debugger.get_cid->Done = false;
      //((CLavaApp*)wxTheApp)->debugger.resume();
      //((CLavaApp*)wxTheApp)->debugger.wait();
    }
}

 
bool CLavaDoc::OnNewDocument()
{
  bool ok;
  isObject = true; 
  if (!CLavaProgram::OnNewDocument())
    return false;
  if (((CLavaApp*)wxTheApp)->SyntaxToOpen.l) {
    PathName = ((CLavaApp*)wxTheApp)->SyntaxToOpen;
    ((CLavaApp*)wxTheApp)->SyntaxToOpen.Reset(0);
    ok = OnEmptyObj(PathName, ((CLavaApp*)wxTheApp)->NameToStore);
    ((CLavaApp*)wxTheApp)->NameToStore.Reset(0);
    return ok;
  }
  else 
    return SelectLcom(true);
}

bool CLavaDoc::OnEmptyObj(const DString& lcomName, const DString& linkName)
{
  if (OnOpenProgram(lcomName.c, false, false, true)) {
    isObject = true;
    IDTable.IDTab[0]->LinkName = linkName;
    QString str = GetTitle();
    ObjectPathName = DString(qPrintable(str));
    Modify(true);
    newLdoc = true;
    return ExecuteLavaObject();
  }
  else
    return false;
}


bool CLavaDoc::SelectLcom(bool emptyDoc)
{
  QString iniFile, fileName = wxTheApp->GetLastFileOpen();
  QFileInfo qf = QFileInfo(fileName);
  DString linkName;
  DString dir;

#ifdef WIN32
  if (qf.suffix() == "lnk") {
    fileName.remove(".lnk");
    qf.setFile(fileName);
  }
#endif
  if (qf.suffix() == ((CLavaApp*)wxTheApp)->pLavaLcomTemplate->GetDefaultExtension())
    iniFile = qf.absoluteFilePath();
  else
    iniFile = ExeDir + ComponentLinkDir;
  QString filter = ("LavaCom file (*.lcom)");

  fileName = L_GetOpenFileName( iniFile,
				      ((CLavaApp*)wxTheApp)->m_appWindow,
				      "Lava (*.lcom) file defining the type of lava component object",
				      filter,
              "lcom"
              );
  if (!fileName.isEmpty()) {
    qf.setFile(fileName);
    fileName = qf.absoluteFilePath();
    qf.setFile(fileName);
    wxTheApp->SetLastFileOpen(fileName);
    QString fn = ResolveLinks(qf);
    PathName = DString(qPrintable(fn));
    if (qf.absolutePath() + "/" == ExeDir + ComponentLinkDir) 
      linkName = DString(qPrintable(qf.fileName()));
    else 
      linkName = DString(qPrintable(fileName));

  }
  if (PathName.l) {
    if (emptyDoc)
      return OnEmptyObj(PathName, linkName);
    else
      if (OnOpenProgram(PathName.c, false, true, true)) {
        if (!IDTable.IDTab[0]->LinkName.l)
          IDTable.IDTab[0]->LinkName = linkName; 
        return true;
      }
  }
  return false;
}


bool CLavaDoc::OnOpenDocument(const QString& fname) 
{
  CheckData ckd;
  QString emptyName, filename;
  DString linkName;

  if (fname.isEmpty()) 
    return OnNewDocument();
  QFileInfo qf = QFileInfo(fname);
  filename = qf.absoluteFilePath();
  wxDocManager::GetDocumentManager()->AddFileToHistory(filename);
  if (LBaseData->openForDebugging) {
    debugOn = true;
    openForDebugging = true;
    LBaseData->openForDebugging = false;
    ((CLavaDebugger*)LBaseData->debugger)->initData(this, &m_execThread);
    ((CLavaDebugger*)LBaseData->debugger)->dbgStopData->stopReason = Stop_Start;
    QApplication::postEvent(LBaseData->debugger, new CustomEvent(UEV_Start,0));
  }
  if (((CLavaApp*)wxTheApp)->pLavaLdocTemplate == GetDocumentTemplate()) {
    QFile fn(filename); 
    ObjectPathName = DString(qPrintable(filename));
    if( !fn.open(QIODevice::ReadOnly))
      return false;
    QDataStream ar(&fn);
    Serialize(ar);
    if (!throwError && (!ar.atEnd() || (fn.error() != QFile::NoError))) {
      QString err = fn.errorString();
      fn.unsetError();
      critical(wxTheApp->m_appWindow, qApp->applicationName(), err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      LObjectError(ckd, filename, emptyName, &ERR_ldocNotOpened);
      fn.close();
      return false;
    }
    fn.close();
    if (throwError)
      return false;
    isObject = true;
    if (!ObjectPathName.l) {
      emptyName = GetTitle();
      //((CLavaApp*)wxTheApp)->pLavaLdocTemplate->GetDocString(emptyName, CDocTemplate::docName);
      ObjectPathName = DString(qPrintable(emptyName));   
    }
    return ExecuteLavaObject();
  }
  else if (((CLavaApp*)wxTheApp)->pLavaLcomTemplate == GetDocumentTemplate()) {
    SetDocumentTemplate(((CLavaApp*)wxTheApp)->pLavaLdocTemplate);
    CLavaProgram::OnNewDocument();
    isObject = true;
    qf.setFile(filename);
    if (qf.absolutePath() + "/" == ExeDir + ComponentLinkDir) 
      linkName = DString(qPrintable(qf.fileName()));
    else 
      linkName = DString(qPrintable(filename));
    QString fn = ResolveLinks(qf);
    PathName = DString(qPrintable(fn));
    return OnEmptyObj(PathName, linkName);
  }
  else { //Lava task (*.lava)
    PathName = qPrintable(filename);
    return OnOpenProgram(filename, true, false, true);
  }
}

bool CLavaDoc::SaveAs()
{
  newLdoc = false;
  return CLavaProgram::SaveAs();
}


bool CLavaDoc::OnSaveDocument(const QString& lpszPathName) 
{
  CheckData ckd;
  CRuntimeException *ex;
  QString emptyName;

  if (newLdoc) {
    newLdoc = false;
    SaveAs();
  }
  ckd.document = this;
  if (isObject) {
    ObjectPathName = DString(qPrintable(lpszPathName));
    ((CLavaGUIView*)RuntimeView)->GetParentFrame()->setWindowTitle(GetTitle());
    if (DocObjects[2]) {
      if (RuntimeView) {
        inSaveProc = true;
        ((CLavaGUIView*)RuntimeView)->NoteLastModified();
        inSaveProc = false;
      }
      if (DocObjects[1]) {
        DFC(DocObjects[1]);
        DocObjects[1] = 0;
      }
      ex = CopyObject(ckd, &DocObjects[2], &DocObjects[1], ((SynFlags*)(DocObjects[2]+1))->Contains(stateObjFlag), DocObjects[0][0][0].classDECL->RelatedDECL);
      if (ex) {
        ex->SetLavaException(ckd);
        delete ex;
      }
      if (ckd.exceptionThrown)
        return false;
      ((CLavaGUIView*)RuntimeView)->NoteNewObj(DocObjects[1]);
    }
    if (lpszPathName.length()) {
      QFile file(lpszPathName); 
      if( !file.open(QIODevice::WriteOnly))
        return false;
      QDataStream ar(&file);
      Serialize(ar);
      if (file.error() != QFile::NoError) {
        QString err = file.errorString();
        file.unsetError();
        critical(wxTheApp->m_appWindow, qApp->applicationName(), err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        LObjectError(ckd, lpszPathName, emptyName, &ERR_ldocNotStored);
        file.close();
        return false;
      }
      file.close();
      Modify(false);
      ((CLavaGUIFrame*)RuntimeView->GetParentFrame())->SetModified(GetTitle(), false);
      return true;
    }
    else
      if (((CLavaApp*)wxTheApp)->pLavaLdocTemplate == GetDocumentTemplate())
        return wxDocument::OnSaveDocument(lpszPathName);
  }
  else
    return wxDocument::OnSaveDocument(lpszPathName);
  return true;
}


/////////////////////////////////////////////////////////////////////////////
// CLavaDoc Serialisierung
void CLavaDoc::customEvent(QEvent *ev0)
{
  CustomEvent *ev=(CustomEvent*)ev0;

	COpenObjectParms *oop = (COpenObjectParms*)ev->data();

  if (ev->type() == UEV_OpenObject) 
    oop->obj = OpenObject(*oop->ckdPtr,oop->urlObj);
  oop->thr->resume();
}

LavaObjectPtr CLavaDoc::OpenObject(CheckData& ckd, LavaObjectPtr urlObj) 
{ 
  LavaObjectPtr urlStr, object=0;
  QString *pfileName, noName, err;
  int secn;

  secn = GetSectionNumber(ckd, (*urlObj)->classDECL, DECLTab[VLString]);
  if (ckd.exceptionThrown)
    return 0;
  urlStr = (LavaObjectPtr)(urlObj - (*urlObj)->sectionOffset + (*urlObj)[secn].sectionOffset);
  pfileName = (QString*)(urlStr + LSH);
  if (pfileName->right(LBaseData->ldocFileExt.length()) == LBaseData->ldocFileExt) {
    //try {
      QFileInfo qf(*pfileName);
      if (qf.isRelative()) {
        qf.setFile(QDir(IDTable.DocDir.c), *pfileName);
        *pfileName = qf.absoluteFilePath();
#ifdef WIN32
        QString driveLetter = QString((*pfileName)[0].toUpper());
        (*pfileName).replace(0,1,driveLetter);
#endif
      }
      QFile file(*pfileName);
      if( !file.open(QIODevice::ReadOnly))
        return 0;
      QDataStream ar(&file);
      SerializeObj(ckd, ar, &object, *pfileName);
      bool e = ar.atEnd();
      if (!ar.atEnd() || (file.error() != QFile::NoError)) {
        if (!ckd.exceptionThrown) {
          err = file.fileName() + ": " + file.errorString();
          critical(wxTheApp->m_appWindow, qApp->applicationName(), err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          LObjectError(ckd, *pfileName, noName, &ERR_ldocNotOpened);
        }
        file.unsetError();
        file.close();
        return 0;
      }
      file.close();
    if (object) {
      if (!*(object-LOH)) *(LavaVariablePtr)(object-LOH) = (LavaObjectPtr)new RunTimeData;
      ((RunTimeData*)*(object-LOH))->urlObj = urlObj;
    }
    return object;
  }
  else
    return 0;
}

bool CLavaDoc::SaveObject(CheckData& ckd, LavaObjectPtr object) 
{
  LavaObjectPtr urlStr, urlObj;
  QString *pfileName, noName;
  int secn;

  if (object) {
    //try {
      object = object - object[0]->sectionOffset;
      urlObj = ((RunTimeData*)*(object-LOH))->urlObj;
      secn = GetSectionNumber(ckd, (*urlObj)->classDECL, DECLTab[VLString]);
      if (ckd.exceptionThrown)
        return false;
      urlStr = (LavaObjectPtr)(urlObj - (*urlObj)->sectionOffset + (*urlObj)[secn].sectionOffset);
      pfileName = (QString*)(urlStr + LSH);
      QFileInfo qf(*pfileName);
      if (qf.isRelative())
        qf.setFile(QString(IDTable.DocDir.c), *pfileName);
      QFile file(qf.absoluteFilePath());
      if( !file.open(QIODevice::WriteOnly)) {
        QString str = QString("File '") + *pfileName + QString("' couldn't be opened for writing");
        QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),str ,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
      }
      QDataStream ar(&file);
      file.unsetError();
      SerializeObj(ckd, ar, &object, qf.absoluteFilePath());
      QFile::FileError fErr = file.error();
      //if (ar.atEnd() || (fErr != QFile::NoError)) {
      if (fErr != QFile::NoError) {
        QString err = file.errorString();
        file.unsetError();
        critical(wxTheApp->m_appWindow, qApp->applicationName(), err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        LObjectError(ckd, *pfileName, noName, &ERR_ldocNotOpened);
        file.close();
        return false;
      }
      file.close();
    return !ckd.exceptionThrown;
  }
  else
    return false;
}


void CLavaDoc::Serialize(QDataStream& ar)
{
  ASN1tofromAr* cid = new ASN1tofromAr(&ar, ObjectPathName);
  CheckData ckd;
  ckd.document = this;
  if (ar.device()->isWritable()) 
    Store(ckd, cid, DocObjects[1]);
  else 
    Load(ckd, cid, &DocObjects[1]);
  delete cid;
}

void CLavaDoc::SerializeObj(CheckData& ckd, QDataStream& ar, LavaVariablePtr pObject, const QString& fileName)
{
  ASN1tofromAr* cid = new ASN1tofromAr(&ar, fileName);
  if (ar.device()->isWritable()) 
    Store(ckd, cid, *pObject);
  else 
    Load(ckd, cid, pObject);
  delete cid;
}


bool CLavaDoc::Store(CheckData& ckd, ASN1tofromAr* cid, LavaObjectPtr object)
{
  LavaObjectPtr sectionPtr, newStackFrame[SFH+2];
  LavaVariablePtr memPtr;
  LavaDECL *classDECL, *secClassDECL;
  int iStore, iCast, iTab, iSect, ll, lmem, llast, objINCL, ii, *reversTab=0;
  TAdapterFunc* funcAdapter;
  QString dPN;
  DString synName, linkName;
  
  newStackFrame[0] = 0;
  newStackFrame[1] = 0;
  newStackFrame[2] = 0;
  //try {
    if (object) {
      objINCL = (*object)->classDECL->inINCL;
      cid->PUTint((int)RELEASE); //(*cid->Ar) << RELEASE;
      dPN = QString(IDTable.IDTab[objINCL]->LinkName.c);
      cid->PUTCString(&dPN);  //(*cid->Ar) << dPN;
      ObjTab = new LavaObjectPtr [1000];
      MaxTab = 1000;
      ObjTab[0] = 0;
      ObjTab[1] = object;
      if (objINCL) {
        reversTab = new int [IDTable.maxINCL];//.IDTab[0]->maxTrans];
        for (ii = 0;
             ii < IDTable.IDTab[0]->maxTrans;
             ii++)
          reversTab[ii] = -1;
        for (ii = 0;
             ii < IDTable.IDTab[objINCL]->maxTrans;
             ii++)
               if (IDTable.IDTab[objINCL]->nINCLTrans[ii].isValid)
                 reversTab[IDTable.IDTab[objINCL]->nINCLTrans[ii].nINCL] = ii;

      }
      iStore = 1;
      ActTab = 2;
      for (iTab = 1; iTab < ActTab; iTab++) {
        classDECL = ObjTab[iTab][0]->classDECL;
        cid->PUTint(ObjTab[iTab][0]->implDECL->OwnID); //(*cid->Ar) << ObjTab[iTab][0]->implDECL->OwnID;
        if (objINCL)
          cid->PUTint(reversTab[ObjTab[iTab][0]->implDECL->inINCL]); //(*cid->Ar) << reversTab[ObjTab[iTab][0]->implDECL->inINCL];
        else
          cid->PUTint(ObjTab[iTab][0]->implDECL->inINCL); //(*cid->Ar) << ObjTab[iTab][0]->implDECL->inINCL;
        for (iSect = 0; iSect < ObjTab[iTab][0]->nSections; iSect++) {
          if (((CSectionDesc*)classDECL->SectionTabPtr)[iSect].SectionFlags.Contains(SectPrimary)) {
            secClassDECL = ((CSectionDesc*)classDECL->SectionTabPtr)[iSect].classDECL;
            sectionPtr = ObjTab[iTab] - (*ObjTab[iTab])->sectionOffset + (*ObjTab[iTab])[iSect].sectionOffset;
            if (isObject && firstStore && (iTab == 1))
              ((SynFlags*)(sectionPtr+1))->INCL(stateObjFlag);
            CDPSynFlags(PUT, cid, (address)(sectionPtr+1),false); //(*cid->Ar) << *(DWORD*)(sectionPtr+1); //section flags
            lmem = LSH;
            if ((*sectionPtr)->classDECL->TypeFlags.Contains(isNative)) { //native section content
              funcAdapter = GetAdapterTable(ckd, (*sectionPtr)->classDECL,0);
              if (!funcAdapter)
                return false;
              if (funcAdapter[3]) { //section has serialize function
                newStackFrame[SFH] = sectionPtr;
                newStackFrame[SFH+1] = (LavaObjectPtr)cid;
                funcAdapter[3](ckd, newStackFrame);
              }
              else {        
                if (secClassDECL->inINCL == 1) {
                  if (secClassDECL->fromBType == B_Set)
                    StoreChain(cid, sectionPtr);
                  else if (secClassDECL->fromBType == B_Array)
                    StoreArray(cid, sectionPtr);
                }
                else {
                  llast = (int)(unsigned)funcAdapter[0] + LSH;//((unsigned)(funcAdapter[0])+3)/4+2;
                  for (ll = LSH; ll < llast; ll++)
                    cid->PUTint(*(long int*)(sectionPtr+ll)); //(*cid->Ar) << *(DWORD*)(sectionPtr + ll);
                }
              }
              lmem = (int)(unsigned)funcAdapter[0] + LSH;
            }
            //else {  //(##########)
              llast = secClassDECL->SectionInfo2 + LSH;
              for (/*ll = LSH*/; lmem < llast; lmem++) {
                memPtr = (LavaVariablePtr)(sectionPtr + lmem);
                if (*memPtr) {
                  iCast = (*memPtr)[0][0].sectionOffset;
                  for (iStore = 1;
                       (iStore < ActTab) && (ObjTab[iStore] != (*memPtr) - iCast);
                       iStore++);
                  if (iStore == ActTab) { //not yet contained
                    if (ActTab == MaxTab) 
                      FullTab();
                    ObjTab[ActTab] = (*memPtr) - iCast;
                    ActTab++;
                  }
                }
                else {
                  iStore = 0; //store 0 if member=0
                  iCast = 0;
                }
                long int ics = (iCast << 16) | iStore;
                cid->PUTint(ics); //store number of member object and section offset
              }//for (ll...
            //}//section not native
          }//primary
        }//for (iSect...
      }//for (iTab...
      delete [] ObjTab;
      ObjTab = 0;
      MaxTab = 0;
      ActTab = 0;
    }
  firstStore = false;
  cid->PUTint(-1); //(*cid->Ar) << -1; //nID = -1 indicates end of objects
  if (objINCL) 
    delete [] reversTab;
  return true;
}

void CLavaDoc::FullTab()
{
  int nii;
  LavaObjectPtr *objTab;

  objTab = new LavaObjectPtr [MaxTab + 1000];
  for (nii = 0; nii < MaxTab; nii++)
    objTab[nii] = ObjTab[nii];
  delete [] ObjTab;
  ObjTab = objTab;
  MaxTab += 1000;
}

void CLavaDoc::StoreChain(ASN1tofromAr* cid, LavaObjectPtr object)
{
  int iStore, iCast;
  long int ics;
  CHE* che;
  for (che = (CHE*)((CHAINX*)(object+LSH))->first; che; che = (CHE*)che->successor) {
    if (che->data) {
      iCast = ((LavaObjectPtr)che->data)[0][0].sectionOffset;
      for (iStore = 1;
        (iStore < ActTab) && ObjTab[iStore] != (LavaObjectPtr)che->data - iCast;
        iStore++);
      if (iStore == ActTab) { //not yet contained
        if (ActTab == MaxTab) 
          FullTab();
        ObjTab[ActTab] = (LavaObjectPtr)che->data - iCast;
        ActTab++;
      }
    }
    else {
      iStore = 0; //store 0 if data = 0
      iCast = 0;
    }
    ics = (iCast << 16) | iStore;
    cid->PUTint(ics /*MAKELONG(iStore, iCast)*/); //(*cid->Ar) << MAKELONG(iStore, iCast); //store number of data object and section offset
  }
  ics = -1;
  cid->PUTint(ics /*MAKELONG(-1, 0)*/); //(*cid->Ar) << MAKELONG(-1, 0); //store -1 for end of chain
}

void CLavaDoc::StoreArray(ASN1tofromAr* cid, LavaObjectPtr object)
{
  int iStore, iCast, arrayLen, ii;
  LavaVariablePtr array = *(LavaVariablePtr*)(object+LSH+1);
  arrayLen = *(int*)(object+LSH);
  cid->PUTint(arrayLen); //(*cid->Ar) << arrayLen;
  for (ii = 0; ii < arrayLen; ii++) {
    if (array[ii]) {
      iCast = array[ii][0]->sectionOffset;
      for (iStore = 1;
          (iStore < ActTab) && (ObjTab[iStore] != array[ii]-iCast);
        iStore++);
      if (iStore == ActTab) { //not yet contained
        if (ActTab == MaxTab) 
          FullTab();
        ObjTab[ActTab] = array[ii] - iCast;
        ActTab++;
      }
    }
    else {
      iStore = 0; //store 0 if data = 0
      iCast = 0;
    }
    long int ics = (iCast << 16) | iStore;
    cid->PUTint(ics /*MAKELONG(iStore, iCast)*/); //(*cid->Ar) << MAKELONG(iStore, iCast); //store number of data object and section offset
  }
}

QString* CLavaDoc::LoadArray1(ASN1tofromAr* cid, LavaObjectPtr object)
{
  int arrayLen, ii;
  long int lon;

  cid->GETint(arrayLen); //(*cid->Ar) >> arrayLen;
  HArrayMakeLen(object, arrayLen);
  for (ii = 0; ii < arrayLen; ii++) {
    cid->GETint(lon); //(*cid->Ar) >> lon;
    (*(LavaVariablePtr*)(object+LSH+1))[ii] = (LavaObjectPtr)lon;
  }
  return 0;
}


void CLavaDoc::LoadArray2(CheckData& ckd, LavaObjectPtr object)
{
  int ii, iTab, arrayLen = *(int*)(object+LSH);
  LavaVariablePtr arPtr = *(LavaVariablePtr*)(object+LSH+1);
  for (ii = 0; ii < arrayLen; ii++)
    if ((*(LavaVariablePtr*)(object+LSH+1))[ii]) {
      iTab = (int)(signed short)LOWORD(arPtr[ii]);
      arPtr[ii] = (ObjTab[iTab] + HIWORD(arPtr[ii]));
      if (iTab)
        INC_FWD_CNT(ckd, ObjTab[iTab]);
    }
}


QString* CLavaDoc::LoadChain1(CheckData& ckd, ASN1tofromAr* cid, LavaObjectPtr object)
{
  int iStore;
  long lon;

  for (cid->GETint(lon); //(*cid->Ar) >> lon;
       (signed short)LOWORD(lon) >= 0;
       cid->GETint(lon)) { //(*cid->Ar) >> lon) {
    iStore = (int)(signed short)LOWORD(lon);
    if (iStore) {
      LavaObjectPtr che  = AllocateObject(ckd, DECLTab[B_Che], false);
      if (!che)
        return &ERR_CorruptObject;
      *(LavaVariablePtr)(che+LSH) = object;
      ((CHE*)(che+LSH+1))->data = (DObject*)lon;
      ((CHAINX*)(object+LSH))->Append((CHE*)(che+LSH+1));  
      //SetAdd(ckd, callStack);
    }
  }
  return 0;
}

void CLavaDoc::LoadChain2(CheckData& ckd, LavaObjectPtr object)
{
  CHE* che;
  int iTab;
  for (che = (CHE*)((CHAINX*)(object+LSH))->first; che; che = (CHE*)che->successor)
    if (che->data) {
      iTab = (int)(signed short)LOWORD(che->data);
      che->data = (DObject*)(ObjTab[iTab] + HIWORD(che->data));
      if (iTab)
        INC_FWD_CNT(ckd, ObjTab[iTab]);
    }
}

bool CLavaDoc::Load(CheckData& ckd, ASN1tofromAr* cid, LavaVariablePtr pObject)
{
  LavaObjectPtr sectionPtr, objPtr, newStackFrame[SFH+2], oCast;
  LavaVariablePtr memPtr;
  LavaDECL *classDECL, *implDECL, *secClassDECL;
  CHESimpleSyntax *cheSyn;
  int iTab, iSect, ll, lmem, llast, objINCL, iEnum, iObj;
  long lon;
  TID id;
  SynFlags secFlag;
  TAdapterFunc* funcAdapter;
  QString dPN("???"), msg, *err=0, fn, dirStr;
  DString synName, linkName, enumStr;
  bool lcomPur;
  TEnumDescription *enumDesc=0;
  CHEEnumSelId *cheItem;

  newStackFrame[0] = 0;
  newStackFrame[1] = 0;
  newStackFrame[2] = 0;
  isObject = ((CLavaApp*)wxTheApp)->pLavaLdocTemplate == GetDocumentTemplate();
  if (isObject)
    firstStore = false;
  //try {
    if (pObject) {
      cid->GETint(cid->Release); //(*cid->Ar) >> cid->Release;
      if (cid->Skip()) {
        LObjectError(ckd, cid->FileName, dPN, &ERR_lcomNotOpened);
        return false;
      }
      cid->GETCString(&dPN); //(*cid->Ar) >> dPN;
      if (cid->Skip()) {
        LObjectError(ckd, cid->FileName, dPN, &ERR_lcomNotOpened);
        return false;
      }
      linkName = DString(qPrintable(dPN));
      QFileInfo qf(dPN);
      lcomPur = !qf.isRelative();
      if (!lcomPur) 
        dPN = ExeDir + ComponentLinkDir + dPN;
#ifdef WIN32
      QString driveLetter = QString(dPN[0].toUpper());
      dPN.replace(0,1,driveLetter);
#endif
      qf.setFile(dPN);
      QString fn = ResolveLinks(qf);
#ifdef WIN32
      driveLetter = QString(fn[0].toUpper());
      fn.replace(0,1,driveLetter);
#endif
      synName = DString(qPrintable(fn));
      if (!synName.l) {
        LObjectError(ckd, cid->FileName, dPN, &ERR_lcomNotOpened);
        return false;
      }
      if (isObject) {
        if (!OnOpenProgram(fn, false, lcomPur, false)) {
          LObjectError(ckd, cid->FileName, synName.c, &ERR_lcomNotOpened);
          return false;
        }
        if (!IDTable.IDTab[0]->LinkName.l)
          IDTable.IDTab[0]->LinkName = linkName; 
        InitBAdapter();
        objINCL = 0;
      }
      else {
        cheSyn = AttachSyntax(ckd, fn);
        if (!cheSyn) {
          LObjectError(ckd, cid->FileName, synName.c, &ERR_lcomNotOpened);
          return false;
        }
        objINCL = cheSyn->data.nINCL;
        IDTable.IDTab[objINCL]->LinkName = linkName; //always after the call of AttachSyntax
      }
      ObjTab = new LavaObjectPtr [1000];
      MaxTab = 1000;
      ObjTab[0] = 0;
      ActTab = 1;
      cid->GETint(id.nID); //(*cid->Ar) >> id.nID;
      while (id.nID != -1) {// all objects
        cid->GETint(id.nINCL); //(*cid->Ar) >> id.nINCL;
        CDPSynFlags(GET, cid, (address)&secFlag,false); //(*cid->Ar) >> secFlag.bits;
        implDECL = IDTable.GetDECL(id, objINCL);
        if (!implDECL) {
          ObjectPathName = synName;
          *pObject = 0;
          if (classDECL)
            LObjectError(ckd, cid->FileName, dPN, &ERR_CorruptObject, 1, &classDECL->FullName, &id);
          else
            LObjectError(ckd, cid->FileName, dPN, &ERR_CorruptObject);
          return false;
        }
        if (implDECL->DeclType == Impl)
          classDECL = IDTable.GetDECL(((CHETID*)implDECL->Supports.first)->data, implDECL->inINCL);
        else
          classDECL = implDECL;
        if (!classDECL) {
          *pObject = 0;
          LObjectError(ckd, cid->FileName, dPN, &ERR_CorruptObject, 2, &implDECL->FullName, &((CHETID*)implDECL->Supports.first)->data);
          return false;
        }
        objPtr = AllocateObject(ckd, classDECL, secFlag.Contains(stateObjFlag));
        if (!objPtr) {
          if (!ckd.exceptionThrown)
            LObjectError(ckd, cid->FileName, dPN, &ERR_CorruptObject, 2, &implDECL->FullName, &((CHETID*)implDECL->Supports.first)->data);
          return false;
        }
        ObjTab[ActTab] = objPtr;
        if (ActTab == MaxTab)
          FullTab();
        ObjTab[ActTab] = objPtr;
        if (ActTab == 1) {
          *pObject = objPtr;
          oCast = objPtr - (objPtr[0])[0].sectionOffset;
          INC_FWD_CNT(ckd, oCast);
        }
        for (iSect = 0; iSect < ObjTab[ActTab][0]->nSections; iSect++) {
          if (((CSectionDesc*)classDECL->SectionTabPtr)[iSect].SectionFlags.Contains(SectPrimary)) {
            secClassDECL = ((CSectionDesc*)classDECL->SectionTabPtr)[iSect].classDECL;
            sectionPtr = objPtr - (objPtr[0])[0].sectionOffset + (objPtr[0])[iSect].sectionOffset;
            if (iSect)
              CDPSynFlags(GET, cid, (address)(sectionPtr+1),false); //(*cid->Ar) >> *(DWORD*)(sectionPtr+1);
            else
               *(sectionPtr+1) = (CSectionDesc*)secFlag.bits;
            if ((secClassDECL->DeclDescType == EnumType) && !enumDesc)
              enumDesc = (TEnumDescription*)secClassDECL->EnumDesc.ptr;
            lmem = LSH;
            if ((*sectionPtr)->classDECL->TypeFlags.Contains(isNative)) { //native section content
              funcAdapter = GetAdapterTable(ckd, (*sectionPtr)->classDECL,0);
              if (!funcAdapter)
                return false;
              if (funcAdapter[3]) { //section has serialize function
                newStackFrame[SFH] = sectionPtr;
                newStackFrame[SFH+1] = (LavaObjectPtr)cid;
                funcAdapter[3](ckd, newStackFrame);
                if ((secClassDECL->inINCL == 1) && (secClassDECL->fromBType == Enumeration)
                  && enumDesc) {
                  iEnum = 0;
                  enumStr = DString(qPrintable((*(QString*)(sectionPtr+LSH+1))));
                  for (cheItem = (CHEEnumSelId*)enumDesc->EnumField.Items.first;
                       cheItem && (cheItem->data.Id != enumStr);
                       cheItem = (CHEEnumSelId*)cheItem->successor) 
                    iEnum++;
                  *(int*)(sectionPtr+LSH) = iEnum;
                  enumDesc = 0;
                }
              }
              else {        
                if ((secClassDECL->inINCL == 1)
                  && (secClassDECL->fromBType == B_Set)) {
                  if (err = LoadChain1(ckd, cid, sectionPtr)) {
                    *pObject = 0;
                    LObjectError(ckd, cid->FileName, dPN, err, 3, &ObjTab[ActTab][0]->classDECL->FullName);
                    delete err;
                    return false;
                  }
                }
                else if ((secClassDECL->inINCL == 1)
                  && (secClassDECL->fromBType == B_Array)) {
                  if (err = LoadArray1(cid, sectionPtr)) {
                    *pObject = 0;
                    LObjectError(ckd, cid->FileName, dPN, err, 4, &ObjTab[ActTab][0]->classDECL->FullName);
                    delete err;
                    return false;
                  }
                }
                else {
                  llast = (int)(unsigned)funcAdapter[0] + LSH;
                  for (ll = LSH; ll < llast; ll++)
                    cid->GETint(*(long int*)(sectionPtr + ll)); 
                }
              }
              lmem = (int)(unsigned)funcAdapter[0] + LSH;
            }
            llast = secClassDECL->SectionInfo2 + LSH;
            for (/*ll = LSH*/; lmem < llast; lmem++) 
              cid->GETint(*(long int*)(sectionPtr + lmem)); 
          }//primary
        }//for (iSect...
        ActTab++;
        cid->GETint(id.nID); 
      }//while
      for (iTab = 1; iTab < ActTab; iTab++) {
        classDECL = ObjTab[iTab][0]->classDECL;
        objPtr = ObjTab[iTab];
        for (iSect = 0; iSect < ObjTab[iTab][0]->nSections; iSect++) {
          if (((CSectionDesc*)classDECL->SectionTabPtr)[iSect].SectionFlags.Contains(SectPrimary)) {
            secClassDECL = ((CSectionDesc*)classDECL->SectionTabPtr)[iSect].classDECL;
            sectionPtr = objPtr - (objPtr[0])[0].sectionOffset + (objPtr[0])[iSect].sectionOffset;
            lmem = LSH;
            if ((*sectionPtr)->classDECL->TypeFlags.Contains(isNative)) {
              if ((secClassDECL->inINCL == 1)
                   && (secClassDECL->fromBType == B_Set))
                LoadChain2(ckd, sectionPtr);
              else if ((secClassDECL->inINCL == 1)
                 && (secClassDECL->fromBType == B_Array))
                LoadArray2(ckd, sectionPtr);
              funcAdapter = GetAdapterTable(ckd, secClassDECL,0);
              if (funcAdapter)
                lmem = (int)(unsigned)funcAdapter[0] + LSH;
            }
            llast = secClassDECL->SectionInfo2 + LSH;
            for (/*ll = LSH*/; lmem < llast; lmem++) {
              memPtr = (LavaVariablePtr)(sectionPtr + lmem);
              lon = *(long*)(LavaVariablePtr)(sectionPtr + lmem);
              iObj = (int)(signed short)LOWORD(lon);
              *memPtr = ObjTab[iObj] + HIWORD(lon);
              if (iObj) //d.h. object!= 0
                INC_FWD_CNT(ckd, ObjTab[iObj]);
            }//for (lmem...
          }//primary
        }//for (iSect...
      }
      for (iTab = 1; iTab < ActTab; iTab++)
        DEC_FWD_CNT(ckd, ObjTab[iTab]);
      delete [] ObjTab;
      ObjTab = 0;
      MaxTab = 0;
      ActTab = 0;
    }
  //}//try
	if (err)
		delete err;
  return true;
}


bool CLavaDoc::IsModified() 
{
  if (isObject ) {
    if (m_documentViews.size()) {
	    CLavaGUIView *pView = (CLavaGUIView *)m_documentViews[0];
      pView->NoteLastModified();
    }
  }
  return wxDocument::IsModified();
}


void CLavaDoc::LObjectError(CheckData& ckd, const QString& ldocName, const QString& lcomName, QString* nresourceID, int moreText, const DString* text, const TID* id)
{
  QString msg, errText;
  DString strID, strINCL, ref;
  int code;
  if (!ckd.exceptionThrown) {
    if (nresourceID) {
      errText = QString(*nresourceID);
      msg += "\nLava Object: " + ldocName;
      if (lcomName.length())
        msg += "\nLava Component Program: " + lcomName;
      msg += ":\n" + errText;
      if (id) {
        Convert.IntToString(id->nINCL, strINCL);
        Convert.IntToString(id->nID, strID);
        msg += "\nBroken reference: (";
        msg += strID.c;
        msg = msg + QString(",") + QString(strINCL.c) + QString(")");
      }
      if (moreText == 1) {
        msg += "\nType of object (implementation) not found. Type of last readable object has name ";
        msg += QString(text->c);
      }
      else if (moreText == 2) {
        msg += "\nInterface declaration of implementation with name ";
        msg += QString(text->c);
        msg += " not found.";
      }
      else if (moreText == 3) {
        msg += "\nLoading chain object in class with name ";
        msg += QString(text->c);
        msg += " failed.";
      }
      else if (moreText == 4) {
        msg += "\nLoading array object in class with name ";
        msg += QString(text->c);
        msg += " failed.";
      }
      if ((nresourceID == &ERR_ldocNotOpened) 
          ||  (nresourceID == &ERR_lcomNotOpened)) {
        code = ldocNotOpened_ex;
        throwError = true;
      }
      else if (nresourceID == &ERR_CorruptObject) {
        code = CorruptObject_ex;
        throwError = true;
      }
      else if (nresourceID == &ERR_ldocNotStored) 
        code = ldocNotStored_ex;
      else
        code = CorruptObject_ex;
      if (isObject && throwError)
        critical(wxTheApp->m_appWindow, qApp->applicationName(),msg,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      else
        if (!SetLavaException(ckd, code, msg))
          throw CRuntimeException(code, &msg);
    }
  }
}

bool CLavaDoc::ExecuteLavaObject()
{
  CheckData ckd;
  LavaDECL *topDECL, *showIntfDecl = 0, *classDECL;
  CHE *che, *cheCompo;
  CRuntimeException *ex;

  ckd.document = this;
  InitBAdapter();
  topDECL = (LavaDECL*)((CHESimpleSyntax*)mySynDef->SynDefTree.first)->data.TopDef.ptr;
  if (topDECL) {
    for (cheCompo = (CHE*)topDECL->NestedDecls.first;
         cheCompo && (((LavaDECL*)cheCompo->data)->DeclType != CompObj); 
         cheCompo = (CHE*)cheCompo->successor);
    if (cheCompo && CheckCompObj((LavaDECL*)cheCompo->data)) {
      for (che = (CHE*)((LavaDECL*)cheCompo->data)->NestedDecls.first;
           che && !((LavaDECL*)che->data)->TypeFlags.Contains(thisComponent); 
           che = (CHE*)che->successor);
      if (che) {
        classDECL = (LavaDECL*)che->data;
        for (che = (CHE*)((LavaDECL*)cheCompo->data)->NestedDecls.first;
             che && ((((LavaDECL*)che->data)->DeclType != Interface)
                    || !((LavaDECL*)che->data)->SecondTFlags.Contains(isGUI)
                    || !IDTable.EQEQ(((LavaDECL*)che->data)->RefID, classDECL->inINCL, TID(classDECL->OwnID, classDECL->inINCL), 0)); 
             che = (CHE*)che->successor);
        if (che)
          showIntfDecl = (LavaDECL*)che->data;
      }
    }
    if (showIntfDecl)
      if (!CheckImpl(ckd, showIntfDecl))
        showIntfDecl = 0;
  }
  if (showIntfDecl) {
    if (!DocObjects[0]) {
      DocObjects[0] = AllocateObject(ckd, showIntfDecl, true);
      if (!DocObjects[0]) {
        if (!ckd.exceptionThrown)
          SetLavaException(ckd, memory_ex, ERR_AllocObjectFailed);
        HCatch(ckd);
        return false;
      }
      CallDefaultInit(ckd, DocObjects[0]);
      if (ckd.exceptionThrown) {
        HCatch(ckd);
        return false;
      }
    }
    if (DocObjects[2]) {
      DFC( DocObjects[2]);
      DocObjects[2] = 0;
    }
    try {
#ifndef WIN32
      if (setjmp(contOnHWexception)) throw hwException;
#endif
      if (DocObjects[1]) {
        ex = CopyObject(ckd, &DocObjects[1], &DocObjects[2], ((SynFlags*)(DocObjects[1]+1))->Contains(stateObjFlag), classDECL);
        if (ex) {
          ex->SetLavaException(ckd);
          delete ex;
        }
        if (ckd.exceptionThrown) {
          HCatch(ckd);
          return false;
        }
      }
      else {
        DocObjects[2] = AllocateObject(ckd, classDECL, true);
        if (!DocObjects[2]) {
          if (!ckd.exceptionThrown)
            SetLavaException(ckd, memory_ex, ERR_AllocObjectFailed);
          HCatch(ckd);
          return false;
        }
        //CallDefaultInit(ckd, DocObjects[2]);
        if (ckd.exceptionThrown) {
          HCatch(ckd);
          return false;
        }
      }
    }
    catch (CRuntimeException exl) {
      if (exl.SetLavaException(ckd)) 
        HCatch(ckd);
      else
       throw;
      return false;
    }
    catch (CHWException exl) {
      if (exl.SetLavaException(ckd)) 
        HCatch(ckd);
      else
       throw;
      return false;
    }
    return true;
  }
  else {
    critical(wxTheApp->m_appWindow, qApp->applicationName(),"No executable Lava-GUI-service",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
}
