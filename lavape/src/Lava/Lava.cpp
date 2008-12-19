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


//#include "prelude.h"
//#include "sflsock.h"
#include "Lava.h"
#include "SYSTEM.h"
#include "CDPpp.h"
#include "AboutBox.h"
#include "LavaAppBase.h"
#include "LavaThread.h"
#include "LavaBaseDoc.h"
#include "DumpView.h"
#include "LavaProgram.h"
#include "MainFrm.h"
#include "LavaGUIFrame.h"
#include "LavaBaseStringInit.h"
#include "LavaDoc.h"
#include "LavaGUIView.h"
#include "Tokens.h"
#include "wx_obj.h"

#include "qclipboard.h"
#include "qfiledialog.h"
#include "qfontdialog.h"
#include "qmessagebox.h"
#include <signal.h>
#include "QtAssistant/qassistantclient.h"
#include "qprocess.h"
#include <stdlib.h>
#include <QTextStream>
#include <QtTest/QtTest>

#ifndef WIN32
#include <locale.h>
#endif

#pragma hdrstop


static QString szCheckPreconditions = "CheckPreconditions";
static QString szCheckPostconditions = "CheckPostconditions";
static QString szCheckInvariants = "CheckInvariants";
static QString szPmDumps = "PmDumps";
static QString szFormFont = "FormFont";
static QString szFormLabelFont = "FormLabelFont";
static QString szGlobalFont = "GlobalFont";
static QString favoriteBrowser = "MyBrowser";
static QString gui_style = "Style";
static QString lfOpen = "LastFileOpen";

static const char slash='/';

/////////////////////////////////////////////////////////////////////////////
// CLavaApp


int main( int argc, char ** argv ) {
  CLavaApp app(argc,argv);
  
  ExeDir = app.applicationDirPath();
#ifdef WIN32
  QString driveLetter = QString(ExeDir[0].toUpper());
  ExeDir.replace(0,1,driveLetter);
#else
//  QString ldpath=QString("LD_LIBRARY_PATH=")+ExeDir+"/../lib";
  setenv("LD_LIBRARY_PATH","../lib",1);
#endif
  QDir::setCurrent(ExeDir);
  
  StdLavaLog = ExeDir + "/std.lava";
  QFileInfo qf = QFileInfo(StdLavaLog);
  StdLava = ResolveLinks(qf);

  QString componentPath;
  QByteArray myPath;

#ifdef _DEBUG
  //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  //_CrtSetBreakAlloc(113357);
#endif

  //QTest::qSleep(20000);

  app.m_appWindow = new CLavaMainFrame;

  if (app.m_appWindow->OnCreate()) {
    app.m_appWindow->show();
  }
  else
    return 1;

  //app.updateButtonsMenus();

  int res = app.exec();

  return res;
}


/////////////////////////////////////////////////////////////////////////////
// CLavaApp Konstruktion

CLavaApp::CLavaApp(int &argc, char ** argv )
:wxApp(argc, argv )
{
#ifndef WIN32
  int rc;
  sigset_t sigs;
#endif
  SynIO.INIT();
  InitGlobalStrings();
//  qt_use_native_dialogs = false;

  SetVendorName("LavaPE-Project");
  SetAppName("LavaPE");
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());
//  settings.setPath(wxTheApp->GetVendorName(),wxTheApp->GetAppName(),QSettings::User);
//  settings.beginGroup(GetSettingsPath());
  LBaseData.theApp = this;
  LBaseData.inRuntime = true;
  LBaseData.openForDebugging = false;
  //clipboard()->clear();
  LBaseData.actHint = 0;
  LBaseData.Init(0, 0);
  LBaseData.debugger = &debugger;
  settings.beginGroup("generalSettings");
  LBaseData.m_strCheckPreconditions = settings.value(szCheckPreconditions,QVariant("true")).toString();
  if (LBaseData.m_strCheckPreconditions == "true")
    LBaseData.m_checkPreconditions = true;
  else
    LBaseData.m_checkPreconditions = false;
  LBaseData.m_strCheckPostconditions = settings.value(szCheckPostconditions,QVariant("false")).toString();
  if (LBaseData.m_strCheckPostconditions == "true")
    LBaseData.m_checkPostconditions = true;
  else
    LBaseData.m_checkPostconditions = false;
  LBaseData.m_strCheckInvariants = settings.value(szCheckInvariants,QVariant("false")).toString();
  if (LBaseData.m_strCheckInvariants == "true")
    LBaseData.m_checkInvariants = true;
  else
    LBaseData.m_checkInvariants = false;
  LBaseData.m_strPmDumps = settings.value(szPmDumps,QVariant("true")).toString();
  if (LBaseData.m_strPmDumps == "true")
    LBaseData.m_pmDumps = true;
  else
    LBaseData.m_pmDumps = false;
  settings.endGroup();

  settings.beginGroup("fontSettings");

  LBaseData.m_lfDefFormFont = settings.value(szFormFont).toString();
  if (!LBaseData.m_lfDefFormFont.isEmpty())
    LBaseData.m_FormFont.fromString(LBaseData.m_lfDefFormFont);
  else {
    LBaseData.m_FormFont = QApplication::font();
    LBaseData.m_lfDefFormFont = LBaseData.m_FormFont.toString();
  }

  LBaseData.m_lfDefFormLabelFont = settings.value(szFormLabelFont).toString();
  if (!LBaseData.m_lfDefFormLabelFont.isEmpty()) {
    LBaseData.m_FormLabelFont.fromString(LBaseData.m_lfDefFormLabelFont);
    LBaseData.useLabelFont = true;
  }
  else {
    LBaseData.m_FormLabelFont = LBaseData.m_FormFont;
    LBaseData.m_lfDefFormLabelFont = LBaseData.m_lfDefFormFont;
    LBaseData.useLabelFont = false;
  }

  LBaseData.m_lfDefGlobalFont = settings.value(szGlobalFont).toString();
  if (!LBaseData.m_lfDefGlobalFont.isEmpty())
    LBaseData.m_GlobalFont.fromString(LBaseData.m_lfDefGlobalFont);
  else {
    LBaseData.m_GlobalFont = QApplication::font();
    LBaseData.m_lfDefGlobalFont = LBaseData.m_GlobalFont.toString();
  }
  setFont(LBaseData.m_GlobalFont,false);

  settings.endGroup();

  settings.beginGroup("otherSettings");
  LBaseData.m_myWebBrowser = settings.value(favoriteBrowser).toString();
  LBaseData.m_style = settings.value(gui_style).toString();
  QString lfo = QString(settings.value(lfOpen).toString());
  wxTheApp->lastFileOpen = lfo;
  settings.endGroup();
#ifndef WIN32
  if (LBaseData.m_myWebBrowser.isEmpty())
#endif
    LBaseData.m_myWebBrowser = InitWebBrowser();

  pLavaTaskTemplate = new wxDocTemplate(m_docManager,
    "Lava", "*.lava","", "lava", "Lava Doc", "Lava Frame", "Lava GUI View",
    &CLavaDoc::CreateObject, &CLavaGUIFrame::CreateObject, &CLavaGUIView::CreateObject);
  m_docManager->AssociateTemplate(pLavaTaskTemplate);

  pLavaLdocTemplate = new wxDocTemplate(m_docManager,
    "Lava Object", "*.ldoc","", "ldoc", "Lava Object", "Lava Frame", "Lava GUI View",
    &CLavaDoc::CreateObject, &CLavaGUIFrame::CreateObject, &CLavaGUIView::CreateObject);
  m_docManager->AssociateTemplate(pLavaLdocTemplate);
  LBaseData.ldocFileExt = "." + pLavaLdocTemplate->GetDefaultExtension();

  pLavaLcomTemplate = new wxDocTemplate(m_docManager,
    "Lava Component", "*.lcom","", "lcom", "Lava Component", "Lava Frame", "Lava GUI View",
    &CLavaDoc::CreateObject, &CLavaGUIFrame::CreateObject, &CLavaGUIView::CreateObject);
  m_docManager->AssociateTemplate(pLavaLcomTemplate);
  Tokens_INIT();
}


QString CLavaApp::InitWebBrowser () {
  QString prog;

#ifdef WIN32

  QString str, arg1;
  HKEY hkey=0;
  DWORD valType=0, dataSize=100;
  BYTE data[100];
  char *datap=(char*)&data[0];
  LONG rc=0;
  unsigned pos=0;

  rc = RegOpenKeyEx(HKEY_CLASSES_ROOT,".htm",0,KEY_QUERY_VALUE,&hkey);
  rc = RegQueryValueEx(hkey,NULL,NULL,&valType,data,&dataSize);
  rc = RegOpenKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)&data[0],0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"shell",0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"open",0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"command",0,KEY_QUERY_VALUE,&hkey);
  dataSize = 100;
  rc = RegQueryValueEx(hkey,0,0,&valType,&data[0],&dataSize);
  str = QString(datap);
  if (str[0] == '"') {
    pos = str.indexOf('\"',1);
    prog = str.mid(1,pos-1);
  }
  else {
    if (pos = str.indexOf(' ',1))
      prog = str.mid(0,pos);
    else
      prog = str;
  }
#else
        prog = "firefox";
#endif
        return prog;
}

bool CLavaApp::event(QEvent *e)
{
  CLavaProgram *doc;
  wxView *actView;
  CMsgBoxParams *mbp;
  CLavaPEHint *pHint;
  CLavaThread *thr;
  DumpEventData* dumpdata;
  int result;
  CheckData ckd;

  switch (e->type()) {
  case UEV_LavaStart:
    doc = (CLavaProgram*)((CustomEvent*)e)->data();
    doc->m_execThread.start();
    return true;
  case UEV_LavaEnd:
    pHint = (CLavaPEHint*)((CustomEvent*)e)->data();
    doc = (CLavaProgram*)pHint->fromDoc;
    delete (CLavaPEHint*)pHint;
    if (((CLavaDebugger*)LBaseData.debugger)->isConnected && doc->debugOn)
      ((CLavaDebugger*)LBaseData.debugger)->stop(normalEnd);
    if (doc && (!doc->isObject || !doc->m_execThread.ldocEnd)) {
      doc->m_execThread.wait();
      doc->OnCloseDocument();
      delete doc;
    }
    if (debugger.startedFromLavaPE || wxTheApp->appExit)
      wxTheApp->exit(0);
    return true;
  case UEV_LavaMsgBox:
    m_appWindow->activateWindow();
    m_appWindow->raise();
    mbp = (CMsgBoxParams*)((CustomEvent*)e)->data();
    thr = mbp->thr;
    switch (mbp->funcSpec) {
    case 0:
      result = QMessageBox::critical(
        mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
      if (result != QMessageBox::NoButton) {
        mbp->result = result;
        mbp->thr->waitingForUI = false;
        mbp->thr->resume();
      }
      return true;
    case 1:
      result =   QMessageBox::information(
        mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
      if (result != QMessageBox::NoButton) {
        mbp->result = result;
        mbp->thr->waitingForUI = false;
        mbp->thr->resume();
      }
      return true;
    case 2:
      result =   QMessageBox::question(
        mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
      if (result != QMessageBox::NoButton) {
        mbp->result = result;
        mbp->thr->waitingForUI = false;
        mbp->thr->resume();
      }
      return true;
    }
    return true;
  case UEV_LavaShow:
    pHint = (CLavaPEHint*)((CustomEvent*)e)->data();
    LBaseData.docModal = pHint->fromDoc;
    ((CLavaDoc*)LBaseData.docModal)->ActLavaDialog = new LavaGUIDialog(m_appWindow, pHint);
     if (pHint->CommandData5)
      thr = (CLavaThread*)((CLavaPEHint*)((CustomEvent*)e)->data())->CommandData5;
    else
      thr = 0;
    if (!thr || !thr->mySemaphore.lastException && !thr->mySemaphore.ex) 
      result = ((QDialog*)((CLavaDoc*)LBaseData.docModal)->ActLavaDialog)->exec();
    else
      result = QDialog::Rejected;
    if (LBaseData.docModal) {
      delete ((CLavaDoc*)LBaseData.docModal)->ActLavaDialog;
      ((CLavaDoc*)LBaseData.docModal)->ActLavaDialog = 0;
      LBaseData.docModal = 0;
      if (thr) {
        if (result == QDialog::Rejected) {
          if (!thr->mySemaphore.ex) {
            ckd.document = (CLavaBaseDoc*)pHint->fromDoc;
            thr->mySemaphore.ex = new CRuntimeException(RunTimeException_ex, &ERR_CanceledForm);
          }
        }
        thr->waitingForUI = false;
        thr->resume();
      }
    }
    delete (CLavaPEHint*)((CustomEvent*)e)->data();
    return true;
  case UEV_LavaDump:
    dumpdata = (DumpEventData*)((CustomEvent*)e)->data();
    dumpdata->doc->DumpFrame = new LavaDumpFrame(m_appWindow, dumpdata);
    ((QDialog*)dumpdata->doc->DumpFrame)->exec();
    delete dumpdata->doc->DumpFrame;
    dumpdata->doc->DumpFrame = 0;
    ((DumpEventData*)((CustomEvent*)e)->data())->currentThread->waitingForUI = false;
    ((DumpEventData*)((CustomEvent*)e)->data())->currentThread->resume();
    delete (DumpEventData*)((CustomEvent*)e)->data();
    return true;
  case UEV_PMDumpOff:
    ((CMainFrame*)m_appWindow)->pmDumpAction->setChecked(false);
    return true;
  case UEV_LavaGUIEvent: 
    if ( LBaseData.docModal 
      && ((CLavaBaseDoc*)LBaseData.docModal)->ActLavaDialog ) 
      ((LavaGUIDialog*)((CLavaBaseDoc*)LBaseData.docModal)->ActLavaDialog)->myGUIProg->CmdExec.GUIEvent( e);
    else {
      actView = wxDocManager::GetDocumentManager()->GetActiveView();
      if (actView && actView->inherits("CLavaGUIView") ) 
        ((CLavaGUIView*)actView)->myGUIProg->CmdExec.GUIEvent( e);
    }
    return true;
  case UEV_DebugStop:
    DebugStop(*(CheckData*)((CustomEvent*)e)->data(),0,0,QString("Object allocation failed because error detected in syntax:\n")+(QString)((CheckData*)((CustomEvent*)e)->data())->exceptionMsg, Stop_Exception,0,0);
    doc = (CLavaProgram*)((CheckData*)((CustomEvent*)e)->data())->document;
    delete (CheckData*)((CustomEvent*)e)->data();
    doc->OnCloseDocument();
    delete doc;
    return true;
  default:
    return wxApp::event(e);
  }

  //return QApplication::event(e);
}


/*
QString lavaFileDialog(const QString& startFileName, QWidget* parent, const QString& caption)
{

  return wxTheApp->wxGetOpenFileName(parent, startFileName, caption,
          "Lava file (*.lava)", "Lava object file (*.ldoc)");


}*/

void CLavaApp::OnFileOpen()
{
  //QString fileName = lavaFileDialog(GetLastFileOpen(), m_appWindow, "Select a file to open");
  QString fileName = wxGetOpenFileName(m_appWindow, GetLastFileOpen(), "Select a file to open",  "Lava file (*.lava)", "Lava object file (*.ldoc)");
  if (fileName.isEmpty())
    return;
#ifdef WIN32
  QString driveLetter = QString(fileName[0].toUpper());
  fileName.replace(0,1,driveLetter);
#endif
  OpenDocumentFile(fileName);
}

void CLavaApp::OnFileDebug()
{
  //QString fileName = lavaFileDialog(wxTheApp->GetLastFileOpen(), m_appWindow, "Select a file to open for debugging");
  QString fileName = wxGetOpenFileName(m_appWindow, GetLastFileOpen(), "Select a file to open for debugging",  "Lava file (*.lava)", "Lava object file (*.ldoc)");
  if (fileName.isEmpty())
    return;
  LBaseData.openForDebugging = true;
#ifdef WIN32
  QString driveLetter = QString(fileName[0].toUpper());
  fileName.replace(0,1,driveLetter);
#endif
  OpenDocumentFile(fileName);
}

void CLavaApp::OpenDocumentFile(const QString& lpszFileName)
{
  QString name, port;
  QDir cwd;
  CLavaDoc* doc;

  name = cwd.absoluteFilePath(lpszFileName);
  name = QDir::cleanPath(name);
  if (!name.contains('.'))
    return;
  wxTheApp->SetLastFileOpen(name);
  if (argc <= 2) {
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
  }
  else if (argc == 3) {
    debugger.startedFromLavaPE = true;
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
  }
  else if (argc > 3) {
    debugger.remoteIPAddress = argv[2];
    port = QString(argv[3]);
    debugger.remotePort = (quint16)port.toUShort();

    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
    doc->debugOn = true;
    ((CLavaMainFrame*)m_appWindow)->fileOpenAction->setEnabled(false);
    ((CLavaMainFrame*)m_appWindow)->fileNewAction->setEnabled(false);
    debugger.startedFromLavaPE = true;
  }
}

void CLavaApp::OnFileNew()
{
  SyntaxToOpen.Reset(0);
  pLavaLdocTemplate->CreateDocument(NULL, wxDOC_NEW);
}


void CLavaApp::OnSaveAll()
{
  DoSaveAll();
}

bool CLavaApp::DoSaveAll()
{
  bool ret = true;
  CLavaDoc* doc;
  int pos;
  for (pos = 0; pos < wxDocManager::GetDocumentManager()->m_docs.size(); pos++) {
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->m_docs[pos];
    if ((((CLavaApp*)wxTheApp)->pLavaLdocTemplate == doc->GetDocumentTemplate())
        && doc->IsModified())
      ret = ret && doc->Save();
  }
  return ret;
}

void CLavaApp::OnAppAbout()
{
  CAboutBox aboutDlg(m_appWindow,"About Lava",true);
  aboutDlg.setWindowFlags(aboutDlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
  aboutDlg.exec();
}

void CLavaApp::saveSettings()
{
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

//  settings.setPath(wxTheApp->GetVendorName(),wxTheApp->GetAppName(),QSettings::User);
//  settings.beginGroup(GetSettingsPath());

  settings.beginGroup("generalSettings");
  settings.setValue(szCheckPreconditions,LBaseData.m_strCheckPreconditions);
  settings.setValue(szCheckPostconditions,LBaseData.m_strCheckPostconditions);
  settings.setValue(szCheckInvariants,LBaseData.m_strCheckInvariants);
  settings.setValue(szPmDumps,LBaseData.m_strPmDumps);
  settings.endGroup();

  settings.beginGroup("fontSettings");
  settings.setValue(szFormFont,LBaseData.m_lfDefFormFont);
  if (LBaseData.useLabelFont)
    settings.setValue(szFormLabelFont,LBaseData.m_lfDefFormLabelFont);
  settings.setValue(szGlobalFont,LBaseData.m_lfDefGlobalFont);
  settings.endGroup();

  settings.beginGroup("otherSettings");
  settings.setValue(favoriteBrowser,LBaseData.m_myWebBrowser);
  settings.setValue(gui_style,LBaseData.m_style);
  settings.setValue(lfOpen,wxTheApp->GetLastFileOpen());
  settings.endGroup();
}

int CLavaApp::OnAppExit()
{
  wxView *view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view && view->inherits("CLavaGUIView"))
    ((CLavaGUIView*)view)->NoteLastModified();
  saveSettings();
  //delete [] TOKENSTR;
  hashTable.clear();
  SynIO.EXIT();
  delete classDict;

  //if (debugger.workSocket && debugger.workSocket->state() != QAbstractSocket::UnconnectedState)
  //  debugger.workSocket->abort();
  return 0;
}


void CLavaApp::OnChooseFormFont(int font_case)
{
  CLavaDoc* doc;
  int posV, posD;
  wxView *view;
  QFont lf;
  wxDocManager *dm;
  bool ok;

  if (!font_case) {
    lf = LBaseData.m_FormFont;

    lf = QFontDialog::getFont(&ok,LBaseData.m_FormFont,m_appWindow);
    if (ok) {
      LBaseData.m_FormFont = lf;
      LBaseData.m_lfDefFormFont = lf.toString();
      if (!LBaseData.useLabelFont) {
        LBaseData.m_FormLabelFont = lf;
        LBaseData.m_lfDefFormLabelFont = LBaseData.m_lfDefFormFont;
      }
      dm = wxDocManager::GetDocumentManager();
      for (posD = 0; posD < dm->m_docs.size(); posD++) {
        doc = (CLavaDoc*)dm->m_docs[posD];
        for (posV = 0; posV < doc->m_documentViews.size(); posV++) {
          view = doc->m_documentViews[posV];
          if (view->inherits("CLavaGUIView")) {
            view->setFont(LBaseData.m_FormFont);
            ((CLavaGUIView*)view)->OnChoosefont();
          }
        }
      }
      saveSettings();
    }
  }
  else {
    lf = LBaseData.m_FormLabelFont;

    lf = QFontDialog::getFont(&ok,LBaseData.m_FormLabelFont,m_appWindow);
    if (ok) {
      LBaseData.m_FormLabelFont = lf;
      LBaseData.m_lfDefFormLabelFont = lf.toString();

      dm = wxDocManager::GetDocumentManager();
      for (posD = 0; posD < dm->m_docs.size(); posD++) {
        doc = (CLavaDoc*)dm->m_docs[posD];
        for (posV = 0; (posV < doc->m_documentViews.size()); posV++) {
          view = doc->m_documentViews[posV]; //GetNextView(pos);
          if (view->inherits("CLavaGUIView"))
            ((CLavaGUIView*)view)->setNewLabelFont();
        }
      }
      saveSettings();
    }
  }

}


void CLavaApp::OnChooseGlobalFont()
{
  QFont lf;
  bool ok;

  lf = LBaseData.m_GlobalFont;

  lf = QFontDialog::getFont(&ok,LBaseData.m_GlobalFont,m_appWindow);
  if (ok) {
    LBaseData.m_GlobalFont = lf;
    LBaseData.m_lfDefGlobalFont = lf.toString();
    setFont(LBaseData.m_GlobalFont);
    saveSettings();
  }
}


void CLavaApp::HtmlHelp()
{
  QString path(QASSISTANT);
  QStringList args;
  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
  if (!qacl) {
          qacl = new QAssistantClient(path,m_appWindow);
          qacl->setArguments(args);
  }

  qacl->showPage(ExeDir + "/../doc/html/FAQ.htm");
}

void CLavaApp::EditingLavaProgs()
{
  QString path(QASSISTANT);
  QStringList args;
  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";

  if (!qacl) {
          qacl = new QAssistantClient(path,m_appWindow);
          qacl->setArguments(args);
  }

  qacl->showPage(ExeDir + "/../doc/html/BasicEditing.htm");
}

void CLavaApp::LearningLava()
{
  QString path(ExeDir);
  QStringList args;
  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";

  if (!qacl) {
          qacl = new QAssistantClient(path,m_appWindow);
          qacl->setArguments(args);
  }

  qacl->showPage(ExeDir + "/../doc/html/LavaBySamples.htm");
}
