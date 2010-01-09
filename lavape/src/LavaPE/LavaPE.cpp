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


#include "MACROS.h"
#include "LavaPE.h"

#include <stdlib.h>
#include <errno.h>

//#include <prelude.h>
#include <QTcpSocket>
#include "LavaBaseStringInit.h"
#include "LavaPE_all.h"
#include "LavaPEFrames.h"
#include "LavaPEDoc.h"
#include "LavaPEView.h"
#include "LavaPEWizard.h"
#include "ExecFrame.h"
#include "FindRefsBox.h"
#include "SylTraversal.h"
#include "ExecView.h"
#include "VTView.h"
#include "SynIO.h"
#include "LavaPEStringInit.h"
#include "SYSTEM.h"
#include "ASN1File.h"
#include "wx_obj.h"
#include "CmdExec.h"

#include "qdir.h"
#include "qfont.h"
#include "qfiledialog.h"
#include "qfontdialog.h"
#include "qsettings.h"
#include "qpixmapcache.h"
#include "qmessagebox.h"
#include "qprocess.h"
#include "qglobal.h"
//Added by qt3to4:
#include <QPixmap>
#include <QEvent>

#include "AboutBox.h"
//#include "qtest.h"

#pragma hdrstop

#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#include <process.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif

static QString szSaveEveryChange = "SaveEveryChange";
static QString szExecFont = "ExecFont";
static QString szFormFont = "FormFont";
static QString szFormLabelFont = "FormLabelFont";
static QString szTreeFont = "TreeFont";
static QString szGlobalFont = "GlobalFont";
static QString favoriteBrowser = "MyBrowser";
static QString gui_style = "Style";
static QString lfOpen = "LastFileOpenPE";

static const char slash='/';



/////////////////////////////////////////////////////////////////////////////
// CLavaPEApp
int main(int argc, char ** argv ) {
  CLavaPEApp app(argc,argv);
//  bool ok; QFontDialog::getFont(&ok); app.exit();
  app.OnAppCreate(); // pixmaps may be created only after the app object

  ExeDir = wxTheApp->applicationDirPath();
#ifdef WIN32
  QString driveLetter = QString(ExeDir[0].toUpper());
  ExeDir.replace(0,1,driveLetter);
#else
//  QString ldpath=QString("LD_LIBRARY_PATH=")+ExeDir+"/../lib";
//  setenv("LD_LIBRARY_PATH","../lib",1);
#endif
#ifdef __Darwin
  ExeDir = ExeDir + "../../../..";
#endif
  QFileInfo qf = QFileInfo(ExeDir);
  ExeDir = ResolveLinks(qf);
  QDir::setCurrent(ExeDir);
  StdLavaLog = ExeDir + "/std.lava";
  qf = QFileInfo(StdLavaLog);
  StdLava = ResolveLinks(qf);

  app.m_appWindow = new CLavaMainFrame();
#ifdef _DEBUG
  //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  //_CrtSetBreakAlloc(206362);
#endif

  //QTest::qSleep(20000);

  if (app.m_appWindow->OnCreate())
    app.m_appWindow->showMaximized();
  else
    return 1;

  if (!app.cmdLineEvaluated && argc > 1) {
    app.cmdLineEvaluated = true;
    app.OpenDocumentFile(argv[1]);
  }

  int res = app.exec();
  return res;
}

int enumBM = 10;
int enumselBM = 12;
int overRidesBM = 21;
int lavafileBM = 25;
int xcomBM = 31;
int xerrBM = 32;
int xerrcomBM = 33;


CLavaPEApp::CLavaPEApp(int &argc, char ** argv )
:wxApp(argc, argv )
 {
  bool b1=false;
  QString lfo;

  LBaseData.stdUpdate = 0;
  //stop here and set stdUpdate = 1 to allow updates in std.lava

  SetVendorName("LavaPE-Project");
  SetAppName("LavaPE");
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

#ifdef WIN32
  QApplication::setFont(QFont("Helvetica", 10));
#else
  QApplication::setFont(QFont("Adobe Helvetica", 12));
#endif

  Browser.LastBrowseContext = 0;
  LBaseData.Init(&Browser, &ExecUpdate);
  SynIO.INIT();
  InitGlobalStrings();
  LavaPEStringInit();
//  qt_use_native_dialogs = false;

  inTotalCheck = false;;
  LBaseData.theApp = this;
  LBaseData.inRuntime = false;
  LBaseData.debugger = &debugger;
  //clipboard()->clear();


  settings.beginGroup("generalSettings");
  LBaseData.m_strSaveEveryChange = settings.value(szSaveEveryChange,QVariant("true")).toString();
  if (LBaseData.m_strSaveEveryChange == "true")
    LBaseData.m_saveEveryChange = true;
  else
    LBaseData.m_saveEveryChange = false;
  settings.endGroup();

  settings.beginGroup("fontSettings");

  LBaseData.m_lfDefExecFont = settings.value(szExecFont,QVariant(QApplication::font().toString())).toString();
  LBaseData.m_ExecFont.fromString(LBaseData.m_lfDefExecFont);

  LBaseData.m_lfDefFormFont = settings.value(szFormFont,QVariant(QApplication::font().toString())).toString();
  LBaseData.m_FormFont.fromString(LBaseData.m_lfDefFormFont);

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
/*
  LBaseData.m_lfDefFormLabelFont = settings.value(szFormLabelFont,QVariant(QApplication::font().toString())).toString();
  LBaseData.m_FormLabelFont.fromString(LBaseData.m_lfDefFormLabelFont);
*/
  LBaseData.m_lfDefTreeFont = settings.value(szTreeFont,QVariant(QApplication::font().toString())).toString();
  LBaseData.m_TreeFont.fromString(LBaseData.m_lfDefTreeFont);

  LBaseData.m_lfDefGlobalFont = settings.value(szGlobalFont,QVariant(QApplication::font().toString())).toString();
  LBaseData.m_GlobalFont.fromString(LBaseData.m_lfDefGlobalFont);

  setFont(LBaseData.m_GlobalFont,false);

  settings.endGroup();

  settings.beginGroup("otherSettings");
  LBaseData.m_myWebBrowser = settings.value(favoriteBrowser).toString();
  LBaseData.m_style = settings.value(gui_style).toString();
  lfo=QString(settings.value(lfOpen).toString());
  wxTheApp->lastFileOpen = lfo;
  settings.endGroup();
#ifndef WIN32
  if (LBaseData.m_myWebBrowser.isEmpty())
#endif
  LBaseData.m_myWebBrowser = InitWebBrowser();

  pLavaTemplate = new wxDocTemplate(m_docManager,
    "LavaPE", "*.lava","", "lava", "Lava Doc", "Lava Frame", "Lava Tree View",
    &CLavaPEDoc::CreateObject, &CTreeFrame::CreateObject, &CLavaPEView::CreateObject);
  m_docManager->AssociateTemplate(pLavaTemplate);

  pExecTemplate = new wxDocTemplate(m_docManager,
    "LavaPE", "*.lava","", "lava", "Lava Doc", "Lava Frame", "Lava Tree View",
    &CLavaPEDoc::CreateObject, &CExecFrame::CreateObject, &CExecView::CreateObject);
  m_docManager->AssociateTemplate(pExecTemplate);

  pLComTemplate = new wxDocTemplate(m_docManager,
    "LavaPE", "*.lcom","", "lcom", "Lava Compo", "Lava Frame", "Lava Tree View",
    &CLavaPEDoc::CreateObject, &CTreeFrame::CreateObject, &CLavaPEView::CreateObject);
  m_docManager->AssociateTemplate(pLComTemplate);

  pFormTemplate = new wxDocTemplate(m_docManager,
    "LavaPE", "*.lava","", "lava", "Lava Doc", "Lava Frame", "Lava GUI View",
    &CLavaPEDoc::CreateObject, &CFormFrame::CreateObject, &CLavaGUIView::CreateObject);//&CLavaGUIView::CreateObject);
  m_docManager->AssociateTemplate(pFormTemplate);

  Tokens_INIT();
  connect(this,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(onFocusChanged(QWidget*,QWidget*)));
}

void CLavaPEApp::OnAppCreate() { // pixmaps may be created only after the app object
  LavaPixmaps[ 0] = new QPixmap((const char**)PX_basicatt);;
  LavaPixmaps[ 1] = new QPixmap((const char**)PX_BasicType);
  LavaPixmaps[ 2] = new QPixmap((const char**)PX_paramatt);
  LavaPixmaps[ 3] = new QPixmap((const char**)PX_patpara);
  LavaPixmaps[ 4] = new QPixmap((const char**)PX_classimpl);
  LavaPixmaps[ 5] = new QPixmap((const char**)PX_classAttr);
  LavaPixmaps[ 6] = new QPixmap((const char**)PX_classint);
  LavaPixmaps[ 7] = new QPixmap((const char**)PX_execEmpty);
  LavaPixmaps[ 8] = new QPixmap((const char**)PX_exec);
  LavaPixmaps[ 9] = new QPixmap((const char**)PX_definitions);//all types
  LavaPixmaps[10] = new QPixmap((const char**)PX_enum);
  LavaPixmaps[11] = new QPixmap((const char**)PX_enumattr);
  LavaPixmaps[12] = new QPixmap((const char**)PX_enumsel);
  LavaPixmaps[13] = new QPixmap((const char**)PX_form);
  LavaPixmaps[14] = new QPixmap((const char**)PX_formAt);
  LavaPixmaps[15] = new QPixmap((const char**)PX_function);
  LavaPixmaps[16] = new QPixmap((const char**)PX_initiato);
  LavaPixmaps[17] = new QPixmap((const char**)PX_inputs);
  LavaPixmaps[18] = new QPixmap((const char**)PX_literal);
  LavaPixmaps[19] = new QPixmap((const char**)PX_members);
  LavaPixmaps[20] = new QPixmap((const char**)PX_outputs);
  LavaPixmaps[21] = new QPixmap((const char**)PX_overwrit);
  LavaPixmaps[22] = new QPixmap((const char**)PX_package);
  LavaPixmaps[23] = new QPixmap((const char**)PX_cospec);
  LavaPixmaps[24] = new QPixmap((const char**)PX_coimpl);
  LavaPixmaps[25] = new QPixmap((const char**)PX_lavafile);
  LavaPixmaps[26] = new QPixmap((const char**)PX_undefine);
  LavaPixmaps[27] = new QPixmap((const char**)PX_params);
  LavaPixmaps[28] = new QPixmap((const char**)PX_settyp);
  LavaPixmaps[29] = new QPixmap((const char**)PX_setatt);
  LavaPixmaps[30] = new QPixmap((const char**)PX_formimpl);
  LavaPixmaps[31] = new QPixmap((const char**)PX_xcommask);
  LavaPixmaps[32] = new QPixmap((const char**)PX_xerrmask);
  LavaPixmaps[33] = new QPixmap((const char**)PX_xerrcommask);
  LavaPixmaps[34] = new QPixmap((const char**)PX_ensure);
  LavaPixmaps[35] = new QPixmap((const char**)PX_ensureDis);
  LavaPixmaps[36] = new QPixmap((const char**)PX_invariant);
  LavaPixmaps[37] = new QPixmap((const char**)PX_invariantDis);
  LavaPixmaps[38] = new QPixmap((const char**)PX_require);
  LavaPixmaps[39] = new QPixmap((const char**)PX_requireDis);
  LavaPixmaps[40] = 0;

  LavaIcons[ 0] = new QIcon(*LavaPixmaps[0]);
  LavaIcons[ 1] = new QIcon(*LavaPixmaps[1]);
  LavaIcons[ 2] = new QIcon(*LavaPixmaps[2]);
  LavaIcons[ 3] = new QIcon(*LavaPixmaps[3]);
  LavaIcons[ 4] = new QIcon(*LavaPixmaps[4]);
  LavaIcons[ 5] = new QIcon(*LavaPixmaps[5]);
  LavaIcons[ 6] = new QIcon(*LavaPixmaps[6]);
  LavaIcons[ 7] = new QIcon(*LavaPixmaps[7]);
  LavaIcons[ 8] = new QIcon(*LavaPixmaps[8]);
  LavaIcons[ 9] = new QIcon(*LavaPixmaps[9]);//all types
  LavaIcons[10] = new QIcon(*LavaPixmaps[10]);
  LavaIcons[11] = new QIcon(*LavaPixmaps[11]);
  LavaIcons[12] = new QIcon(*LavaPixmaps[12]);
  LavaIcons[13] = new QIcon(*LavaPixmaps[13]);
  LavaIcons[14] = new QIcon(*LavaPixmaps[14]);
  LavaIcons[15] = new QIcon(*LavaPixmaps[15]);
  LavaIcons[16] = new QIcon(*LavaPixmaps[16]);
  LavaIcons[17] = new QIcon(*LavaPixmaps[17]);
  LavaIcons[18] = new QIcon(*LavaPixmaps[18]);
  LavaIcons[19] = new QIcon(*LavaPixmaps[19]);
  LavaIcons[20] = new QIcon(*LavaPixmaps[20]);
  LavaIcons[21] = new QIcon(*LavaPixmaps[21]);
  LavaIcons[22] = new QIcon(*LavaPixmaps[22]);
  LavaIcons[23] = new QIcon(*LavaPixmaps[23]);
  LavaIcons[24] = new QIcon(*LavaPixmaps[24]);
  LavaIcons[25] = new QIcon(*LavaPixmaps[25]);
  LavaIcons[26] = new QIcon(*LavaPixmaps[26]);
  LavaIcons[27] = new QIcon(*LavaPixmaps[27]);
  LavaIcons[28] = new QIcon(*LavaPixmaps[28]);
  LavaIcons[29] = new QIcon(*LavaPixmaps[29]);
  LavaIcons[30] = new QIcon(*LavaPixmaps[30]);
  LavaIcons[31] = new QIcon(*LavaPixmaps[31]);
  LavaIcons[32] = new QIcon(*LavaPixmaps[32]);
  LavaIcons[33] = new QIcon(*LavaPixmaps[33]);
  LavaIcons[34] = new QIcon(*LavaPixmaps[34]);
  LavaIcons[35] = new QIcon(*LavaPixmaps[35]);
  LavaIcons[36] = new QIcon(*LavaPixmaps[36]);
  LavaIcons[37] = new QIcon(*LavaPixmaps[37]);
  LavaIcons[38] = new QIcon(*LavaPixmaps[38]);
  LavaIcons[39] = new QIcon(*LavaPixmaps[39]);
  LavaIcons[40] = 0;
}

void CLavaPEApp::onFocusChanged(QWidget *old, QWidget *now) {
  QWidget *parent=now;

  while (parent && !parent->inherits("wxView") && !parent->inherits("CComboBar"))
    parent = parent->parentWidget();
  if (!parent)
    return;
  if (parent->inherits("CComboBar"))
    ((CComboBar*)parent)->myExecView->Activate(false);
  else if (parent->inherits("wxView"))
    ((wxView*)parent)->Activate(false);
}

bool CLavaPEApp::event(QEvent *e)
{
  wxView *actView;
  CLavaPEDoc *doc;
  switch (e->type()) {
  case UEV_LavaDebug:
    ((CLavaMainFrame*)m_appWindow)->m_UtilityView->setDebugData((DbgMessages*)((CustomEvent*)e)->data(), debugger.myDoc);
    m_appWindow->activateWindow();
    m_appWindow->raise();
    return true;
  case UEV_LavaDebugRq:
    if (!debugger.isConnected)
      return true;
    if (debugger.dbgRequest) {
      delete debugger.dbgRequest;
      debugger.dbgRequest = 0;
    }
    debugger.dbgRequest = (DbgMessage*)((CustomEvent*)e)->data();
    debugger.send();
    return true;
  case UEV_LavaDebugW:
    ((CLavaMainFrame*)m_appWindow)->DbgBreakpointAct->setEnabled(true);
    ((CLavaMainFrame*)m_appWindow)->DbgClearBreakpointsAct->setEnabled(true);
    return true;
  case UEV_LavaPE_CloseDoc:
    doc = (CLavaPEDoc*)((CustomEvent*)e)->data();
    doc->OnCloseDocument();
    delete doc;
    return true;
  case UEV_LavaGUIEvent:
    actView = wxDocManager::GetDocumentManager()->GetActiveView();
    if ( actView->inherits("CLavaGUIView") )
      ((CLavaGUIView*)actView)->myGUIProg->CmdExec.GUIEvent( e);
    return true;
   default:
    return wxApp::event(e);
  }
}

QString CLavaPEApp::InitWebBrowser () {
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

void CLavaPEApp::OnAppAbout()
{
  CAboutBox aboutDlg(m_appWindow,"About LavaPE",true);
  aboutDlg.setWindowFlags(aboutDlg.windowFlags() ^ Qt::WindowContextHelpButtonHint);
  aboutDlg.exec();
}

void CLavaPEApp::OnEditUndo()
{
  CLavaPEDoc* doc = (CLavaPEDoc*)m_docManager->GetActiveDocument();
  if (doc)
    doc->UndoMem.OnEditUndo();
}



void CLavaPEApp::OnFileNew()
{
  pLavaTemplate->CreateDocument(NULL, wxDOC_NEW);
}


void CLavaPEApp::OnNewComponent()
{
  pLComTemplate->CreateDocument(NULL, wxDOC_NEW);
}


void CLavaPEApp::OnPopcontext()
{
  CLavaPEDoc* doc;
  LavaDECL *decl;
  CTreeItem* item;
  CBrowseContext * back = ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext;
  if (!back)
    return;
  ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = back->prev;
  wxView *backView = back->fromView;
  backView->Activate(false);

  if (backView->inherits("CExecView"))
    ((CExecView*)backView)->Select((SynObject*)back->synObjSel);
  else if (backView->inherits("CLavaGUIView"))
    back->formNode->data.FIP.widget->setFocus();
  else {
    if (back->id.nID != -1) {
      if (backView->inherits("CLavaPEView")) {
        doc = ((CLavaPEView*)backView)->GetDocument();
        decl = doc->IDTable.GetDECL(back->id);
        if (!decl)
        decl = doc->IDTable.GetDECL(back->parentID);
        if (decl)
          item = ((CLavaPEView*)backView)->BrowseTree(decl, (CTreeItem*)((CLavaPEView*)backView)->Tree->RootItem);
        else
          item = 0;
      }
      else
        if (backView->inherits("CVTView"))
          item =((CVTView*)backView)->BrowseTree(back->id, 0, back->type);
        else
          item = 0;
      if (item)
        ((CTreeView*)backView)->Tree->setCurAndSel(item);
    }
  }
  delete back;
}

void CLavaPEApp::OnEditRedo()
{
  CLavaPEDoc* doc = (CLavaPEDoc*)m_docManager->GetActiveDocument();
  if (doc)
    doc->UndoMem.OnEditRedo();
}


void CLavaPEApp::OnChooseTreeFont()
{
  CLavaPEDoc* doc;
  int posD, posV;
  wxView *view;
  QFont lf;
  wxDocManager *dm;
  bool ok;

  lf = LBaseData.m_ExecFont;

  lf = QFontDialog::getFont(&ok/*,LBaseData.m_TreeFont*/,m_appWindow);
  if (ok) {
    LBaseData.m_TreeFont = lf;
    LBaseData.m_lfDefTreeFont = lf.toString();

    dm = wxDocManager::GetDocumentManager();
    //posD = dm->GetFirstDocPos();
    //while (posD) {
    for (posD = 0; posD < dm->m_docs.size(); posD++) {
      doc = (CLavaPEDoc*)dm->m_docs[posD];
      //posV = doc->GetFirstViewPos();
      //while (posV) {
      for (posV = 0; posV < doc->m_documentViews.size(); posV++) {
        view = doc->m_documentViews[posV];
        if (view->inherits("CLavaPEView")
            || view->inherits("CVTView")
            || view->inherits("CWizardView")
            || view->inherits("CInclView"))
          if (view->inherits("CWizardView"))
            ((CWizardView*)view)->SetFont(LBaseData.m_TreeFont);
          else
            view->setFont(LBaseData.m_TreeFont);
      }
    }
    saveSettings();
  }
}

void CLavaPEApp::OnChooseExecFont()
{
  CLavaPEDoc* doc;
  int posD, posV;
  wxView *view;
  QFont lf;
  wxDocManager *dm;
  bool ok;

  lf = LBaseData.m_ExecFont;

  lf = QFontDialog::getFont(&ok,LBaseData.m_ExecFont,m_appWindow);
  if (ok) {
    LBaseData.m_ExecFont = lf;
    LBaseData.m_lfDefExecFont = lf.toString();

    dm = wxDocManager::GetDocumentManager();
    for (posD = 0; posD < dm->m_docs.size(); posD++) {
      doc = (CLavaPEDoc*)dm->m_docs[posD];
      for (posV = 0; posV < doc->m_documentViews.size(); posV++) {
        view = doc->m_documentViews[posV];
        if (view->inherits("CExecView"))
          ((CExecView*)view)->sv->setFont(LBaseData.m_ExecFont);
      }
    }
    saveSettings();
  }
}


void CLavaPEApp::OnChooseFormFont(int font_case)
{
  CLavaPEDoc* doc;
  int posD, posV;
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
      /*
      if (!LBaseData.useButtonFont) {
        LBaseData.m_FormButtonFont = lf;
        LBaseData.m_lfDefFormButtonFont = LBaseData.m_lfDefFormFont;
      }*/

      dm = wxDocManager::GetDocumentManager();
      /*
      posD = dm->GetFirstDocPos();
      while (posD) {
        doc = (CLavaPEDoc*)dm->GetNextDoc(posD);
        posV = doc->GetFirstViewPos();
        while (posV) {
          view = doc->GetNextView(posV);*/
      for (posD = 0; posD < dm->m_docs.size(); posD++) {
        doc = (CLavaPEDoc*)dm->m_docs[posD];
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
      //posD = dm->GetFirstDocPos();
      //while (posD) {
      //  doc = (CLavaPEDoc*)dm->GetNextDoc(posD);
        //posV = doc->GetFirstViewPos();
        //while (posV) {
          //((view = doc->GetNextView(posV);
      for (posD = 0; posD < dm->m_docs.size(); posD++) {
        doc = (CLavaPEDoc*)dm->m_docs[posD];
        for (posV = 0; posV < doc->m_documentViews.size(); posV++) {
          view = doc->m_documentViews[posV];
          if (view->inherits("CLavaGUIView"))
            ((CLavaGUIView*)view)->setNewLabelFont();//LBaseData.m_FormLabelFont);
        }
      }
      saveSettings();
    }
  }
  /*
  else {
    lf = LBaseData.m_FormButtonFont;

    lf = QFontDialog::getFont(&ok,LBaseData.m_FormButtonFont,m_appWindow);
    if (ok) {
      LBaseData.m_FormButtonFont = lf;
      LBaseData.m_lfDefFormButtonFont = lf.toString();

      dm = wxDocManager::GetDocumentManager();
      posD = dm->GetFirstDocPos();
      while (posD) {
        doc = (CLavaPEDoc*)dm->GetNextDoc(posD);
        posV = doc->GetFirstViewPos();
        while (posV) {
          view = doc->GetNextView(posV);
          if (view->inherits("CLavaGUIView"))
            ((CLavaGUIView*)view)->setNewButtonFont();//LBaseData.m_FormLabelFont);
        }
      }
      saveSettings();
    }
  }
  */
}


void CLavaPEApp::OnChooseGlobalFont()
{
  QFont lf;
  bool ok;

  lf = QFontDialog::getFont(&ok,LBaseData.m_GlobalFont,m_appWindow);
  if (ok) {
    LBaseData.m_GlobalFont = lf;
    LBaseData.m_lfDefGlobalFont = lf.toString();
    setFont(LBaseData.m_GlobalFont);
    saveSettings();
  }
}


void CLavaPEApp::OnFileOpen()
{
  QString fileName = wxGetOpenFileName(m_appWindow, wxTheApp->GetLastFileOpen(), "Select a file to open",  "Lava file (*.lava)", "LavaCom file (*.lcom)");
  if (fileName.isEmpty())
    return;
#ifdef WIN32
  QString driveLetter = QString(fileName[0].toUpper());
  fileName.replace(0,1,driveLetter);
#endif
  OpenDocumentFile(fileName);
}

void CLavaPEApp::OpenDocumentFile(const QString& lpszFileName)
{
  QString name;
  QDir cwd;
  name = cwd.absoluteFilePath(lpszFileName);
  name = cwd.cleanPath(name);
  if (!name.contains('.'))
    return;
  wxTheApp->SetLastFileOpen(name);
#ifdef WIN32
  QString driveLetter = QString(name[0].toUpper());
  name.replace(0,1,driveLetter);
#endif
  ((CLavaPEApp*)qApp)->debugger.myDoc = (CLavaBaseDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
  if (argc > 2) {
    ((CLavaPEApp*)qApp)->debugger.remoteIPAddress = argv[2];
    ((CLavaPEApp*)qApp)->debugger.remotePort = (quint16)QString(argv[3]).toUShort();
    ((CLavaPEApp*)qApp)->debugger.start();
  }
}

void CLavaPEApp::HtmlHelp()
{
  QString page("html/FAQ.htm");
  assistant->ShowPage(page);
}

void CLavaPEApp::EditingLavaProgs()
{
  wxTheApp->assistant->ShowPage("html/BasicEditing.htm");
}

void CLavaPEApp::LearningLava()
{
  wxTheApp->assistant->ShowPage("html/LavaBySamples.htm");
}

void CLavaPEApp::OnSaveAll()
{
  DoSaveAll();
}


void CLavaPEApp::OnImport()
{
  QMessageBox::critical(wxDocManager::GetDocumentManager()->GetActiveView(), applicationName(), "Not yet implemented!",QMessageBox::Ok| QMessageBox::Default,Qt::NoButton);
}

void CLavaPEApp::OnFindByName()
{
  CFindData fw;
  fw.index = 2;
  Browser.OnFindRefs(0, 0, fw);
}

void CLavaPEApp::UpdateUI()
{
  OnUpdateEditUndo(((CLavaMainFrame*)m_appWindow)->editUndoAction);
  OnUpdateEditRedo(((CLavaMainFrame*)m_appWindow)->editRedoAction);
  OnUpdatePopcontext(((CLavaMainFrame*)m_appWindow)->returnToRefAction);
  OnUpdateSaveAll(((CLavaMainFrame*)m_appWindow)->fileSaveAllAction);
}

void CLavaPEApp::OnUpdateEditUndo(QAction* action)
{
  CLavaPEDoc* doc = (CLavaPEDoc*)m_docManager->GetActiveDocument();
  if (doc)
    doc->UndoMem.OnUpdateEditUndo(action);
  else
    action->setEnabled(false);
}


void CLavaPEApp::OnUpdateEditRedo(QAction* action)
{
  CLavaPEDoc* doc = (CLavaPEDoc*)m_docManager->GetActiveDocument();
  if (doc)
    doc->UndoMem.OnUpdateEditRedo(action);
  else
    action->setEnabled(false);
}

void CLavaPEApp::OnUpdatePopcontext(QAction* action)
{
  action->setEnabled((Browser.LastBrowseContext != 0) && wxDocManager::GetDocumentManager()->GetOpenDocCount());
}

void CLavaPEApp::OnUpdateSaveAll(QAction* action)
{
  action->setEnabled(wxDocManager::GetDocumentManager()->GetOpenDocCount());
}

CLavaPEApp::~CLavaPEApp()
{
  OnAppExit();
  FindList.Destroy();

  for (int i = 0; LavaIcons[i] != 0; i++)
    delete LavaIcons[i];
  for (int i = 0; LavaPixmaps[i] != 0; i++)
    delete LavaPixmaps[i];
}

void CLavaPEApp::saveSettings()
{
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

//  settings.setPath(wxTheApp->GetVendorName(),wxTheApp->GetAppName(),QSettings::UserScope);
//  settings.beginGroup(GetSettingsPath());

  settings.beginGroup("generalSettings");
  settings.setValue(szSaveEveryChange,LBaseData.m_strSaveEveryChange);
  settings.endGroup();

  settings.beginGroup("fontSettings");
  settings.setValue(szExecFont,LBaseData.m_lfDefExecFont);
  settings.setValue(szFormFont,LBaseData.m_lfDefFormFont);
  if (LBaseData.useLabelFont)
    settings.setValue(szFormLabelFont,LBaseData.m_lfDefFormLabelFont);
  settings.setValue(szTreeFont,LBaseData.m_lfDefTreeFont);
  settings.setValue(szGlobalFont,LBaseData.m_lfDefGlobalFont);
  settings.endGroup();

  settings.beginGroup("otherSettings");
  settings.setValue(favoriteBrowser,LBaseData.m_myWebBrowser);
  settings.setValue(gui_style,LBaseData.m_style);
  settings.setValue(lfOpen,wxTheApp->GetLastFileOpen());
  settings.endGroup();
  settings.sync();
}

int CLavaPEApp::OnAppExit()
{
  saveSettings();
  //delete [] TOKENSTR;
  hashTable.clear();
  SynIO.EXIT();
  delete classDict;

  return 0;
}

void CLavaPEApp::OnCloseLastExecView()
{
	if (deletingMainFrame)
    return;
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_5->hide();
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_6->hide();
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->Toolbar_7->hide();
}

int CLavaPEBrowse::findAnyForm(LavaDECL * decl, TID& refID, SynDef *lavaCode)
{
  LavaDECL *defPDECL = ((TIDTable*)lavaCode->IDTable)->GetDECL(refID, decl->inINCL);
  LavaDECL *defFDECL = 0;
  TBasicType BType;
  TID id;
  int rr = 0;
  if (defPDECL && ((defPDECL->DeclType == Interface))) {
    if (defPDECL->SecondTFlags.Contains(isGUI))
      return 1;
    id = TID(defPDECL->OwnID,0);
    //use any form if available
    CFindLikeForm *fLike = new CFindLikeForm(lavaCode, id, defPDECL->inINCL, decl);
    if (fLike->pdecl && *fLike->pdecl)
      defFDECL = *fLike->pdecl;
    delete fLike;
    if (defFDECL
        && ((TIDTable*)lavaCode->IDTable)->EQEQ(defFDECL->RefID, defFDECL->inINCL, id, defPDECL->inINCL)) {
      refID = TID(defFDECL->OwnID, defFDECL->inINCL);
      rr = 1;
    }
    else {
      refID = TID(defPDECL->OwnID, defPDECL->inINCL);
      if (defPDECL->Annotation.ptr && defPDECL->Annotation.ptr->FA.ptr) {
        decl->Annotation.ptr->BasicFlags += ((TAnnotation*)defPDECL->Annotation.ptr->FA.ptr)->BasicFlags;
        decl->Annotation.ptr->IoSigFlags += ((TAnnotation*)defPDECL->Annotation.ptr->FA.ptr)->IoSigFlags;
        decl->Annotation.ptr->IterFlags += ((TAnnotation*)defPDECL->Annotation.ptr->FA.ptr)->IterFlags;
      }
      BType = GetBasicType(lavaCode, decl);
      switch (BType ) {
        case NonBasic:
          break;
        case Enumeration:
          rr = 0;
          break;
        case B_Bool:
          decl->Annotation.ptr->BasicFlags.INCL(Toggle);
          rr = -1;
          break;
        case VLString:
          rr = -1;
          if (decl->Annotation.ptr->Length.Field == 1)
            decl->Annotation.ptr->Length.Field = 7;
          break;
        case Integer:
          rr = -1;
          if (decl->Annotation.ptr->Length.Field == 1)
            decl->Annotation.ptr->Length.Field = 3;
          break;

        default:
          rr = -1;;
      }
    }
  }
  return rr;
}

int CLavaPEBrowse::HasDefaultForm(LavaDECL* decl, LavaDECL* patDECL, SynDef *lavaCode)
{
  CHE *inCheEl, *toRemove, *LFirst;
  int rr = 0, nrr;
  LavaDECL *inDecl, *oldInDecl, *finalDecl, *declGet, *elDECL;
  bool newForm = false;
  CHETID * cheSup;
  TAnnotation* anno;

  if (!decl->Annotation.ptr) {
    decl->Annotation.ptr = new TAnnotation;
    newForm = true;
  }
  if (!decl->Annotation.ptr->FA.ptr) {
    decl->Annotation.ptr->FA.ptr = new TAnnotation;
    newForm = true;
  }
  LFirst = (CHE*)decl->Annotation.ptr->Suffixes.first;
  if (!LFirst)
    LFirst = (CHE*)decl->Annotation.ptr->Prefixes.first;
  if (newForm && !LFirst && decl->LocalName.l) {
    inDecl = NewLavaDECL();
    inDecl->Annotation.ptr = new TAnnotation;
    inDecl->DeclDescType = LiteralString;
    inDecl->LitStr = decl->LocalName;
    LFirst = NewCHE(inDecl);
    if ((decl->DeclDescType == BasicType) &&  (decl->BType == B_Bool)) {
      inDecl->Annotation.ptr->BasicFlags.INCL(Toggle);
      decl->Annotation.ptr->Suffixes.Append(LFirst);
    }
    else
      decl->Annotation.ptr->Prefixes.Append(LFirst);
    decl->Annotation.ptr->FrameSpacing = 1;
  }
  anno = (TAnnotation*)decl->Annotation.ptr->FA.ptr;
  if (LFirst && ((decl->DeclDescType != BasicType) ||  (decl->BType != B_Bool))) {
    anno->BasicFlags.EXCL(Toggle);
    ((LavaDECL*)LFirst->data)->Annotation.ptr->BasicFlags.EXCL(Toggle);
  }
  if ((decl->DeclType == VirtualType)
      && (decl->SecondTFlags.Contains(isSet) || decl->SecondTFlags.Contains(isArray))) {
    for (finalDecl = decl;
         finalDecl && (finalDecl->DeclDescType != BasicType)
                   && ((finalDecl->DeclType == VirtualType) || (finalDecl->DeclType == PatternDef));
         finalDecl = ((TIDTable*)lavaCode->IDTable)->GetDECL(finalDecl->RefID, finalDecl->inINCL));
    if (finalDecl && (finalDecl->DeclDescType == BasicType)) {
      decl->DeclDescType = BasicType;
      decl->BType = finalDecl->BType;
      decl->RefID = TID(finalDecl->RefID.nID, ((TIDTable*)lavaCode->IDTable)->IDTab[finalDecl->inINCL]->nINCLTrans[finalDecl->RefID.nINCL].nINCL);
    }
    if (!decl->Annotation.ptr->IterOrig.ptr) {
      decl->Annotation.ptr->IterOrig.ptr = new TIteration;
      if (decl->SecondTFlags.Contains(isSet)) {
        ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->BoundType = GE;
        ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->Bound = 0;
      }
      else { //array
        ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->BoundType = EQ;
        ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->Bound = 1;
        ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->IterFlags.INCL(FixedCount);
      }
    }
    if (!((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr) {
      inDecl = NewLavaDECL();
      ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr = inDecl;
      inDecl->DeclDescType = decl->DeclDescType;
      inDecl->BType = decl->BType;
      inDecl->TreeFlags = decl->TreeFlags;
      inDecl->SecondTFlags = decl->SecondTFlags;
      inDecl->DeclType = PatternDef; //not VirtualType
      inDecl->RefID = TID(decl->RefID.nID, ((TIDTable*)lavaCode->IDTable)->IDTab[decl->inINCL]->nINCLTrans[decl->RefID.nINCL].nINCL);
    }
    else {
      inDecl = ((TIteration*)decl->Annotation.ptr->IterOrig.ptr)->IteratedExpr.ptr;
      inDecl->inINCL = decl->inINCL;
    }
    oldInDecl = inDecl;
    rr = HasDefaultForm(inDecl, patDECL, lavaCode);
    decl->RefID = TID(inDecl->RefID.nID, ((TIDTable*)lavaCode->IDTable)->IDTab[inDecl->inINCL]->nINCLTrans[inDecl->RefID.nINCL].nINCL);
    if (!inDecl->Annotation.ptr) {
      inDecl->Annotation.ptr = new TAnnotation;
      inDecl->Annotation.ptr->FrameSpacing = 1;
    }
  }
  else {
    if (decl->DeclDescType == NamedType) {
      for (finalDecl = ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL);
           finalDecl && (finalDecl->DeclDescType != BasicType)
                     && ((finalDecl->DeclType == VirtualType) || (finalDecl->DeclType == PatternDef));
           finalDecl = ((TIDTable*)lavaCode->IDTable)->GetDECL(finalDecl->RefID, finalDecl->inINCL));
      if (finalDecl && (finalDecl->DeclDescType == BasicType)) {
        decl->DeclDescType = BasicType;
        decl->BType = finalDecl->BType;
        decl->RefID = TID(finalDecl->RefID.nID, ((TIDTable*)lavaCode->IDTable)->IDTab[finalDecl->inINCL]->nINCLTrans[finalDecl->RefID.nINCL].nINCL);
      }
    }
    switch ( decl->DeclDescType) {
      case BasicType:
        switch (decl->BType ) {
          case B_Bool:
            anno->BasicFlags.INCL(Toggle);
            break;
          case Float:
          case Double:
            if (anno->Length.Field == 1)
              anno->Length.Field = 5;
            break;
          case VLString:
            if (anno->Length.Field == 1)
              anno->Length.Field = 7;
            break;
          case Integer:
            if (anno->Length.Field == 1)
              anno->Length.Field = 3;
            break;
          default: ;
        };
        rr = -1;
        break;
     case EnumType:
     case StructDesc:
        for (cheSup = (CHETID*)decl->Supports.first; cheSup; cheSup = (CHETID*)cheSup->successor) {
          declGet = ((TIDTable*)lavaCode->IDTable)->GetDECL(cheSup->data, decl->inINCL);
          if (declGet)
            rr = findAnyForm(decl, cheSup->data, lavaCode);
        }
        if (decl->DeclDescType == EnumType) {
          if (!SynIO.fromOldStyle)
            makeDefaultMenu(decl);
          rr = 1;
        }
        inCheEl = (CHE*)decl->NestedDecls.first;
        while (inCheEl) {
          elDECL = (LavaDECL*)inCheEl->data;
          if ( elDECL->DeclType == Attr) {
            if (elDECL->OwnID >= 0) {
              cheSup = new CHETID;
              cheSup->data = TID(elDECL->OwnID, patDECL->inINCL);
              elDECL->Supports.Prepend(cheSup);
              elDECL->OwnID = -1;
            }
            nrr = HasDefaultForm(elDECL, patDECL, lavaCode);
            if ((nrr != 0) && (rr < 1))
              rr = nrr;
          }
          else {
            if ((elDECL->DeclType == VirtualType)
                && (elDECL->SecondTFlags.Contains(isSet) || elDECL->SecondTFlags.Contains(isArray))) {
              rr = HasDefaultForm(elDECL, patDECL, lavaCode);
            }//pattern
            ((LavaDECL*)inCheEl->data)->OwnID = -1;
          }//else
          inCheEl = (CHE*)inCheEl->successor;
        }//while
        //at least remove other declarations
        inCheEl = (CHE*)decl->NestedDecls.first;
        while (inCheEl) {
          elDECL = (LavaDECL*)inCheEl->data;
          if ( (elDECL->DeclType != Attr)
            && ((elDECL->DeclType != VirtualType)
                || !elDECL->SecondTFlags.Contains(isSet) && !elDECL->SecondTFlags.Contains(isArray))) {
            toRemove = inCheEl;
            inCheEl = (CHE*)inCheEl->successor;
            toRemove = (CHE*)decl->NestedDecls.Uncouple(toRemove);
            delete toRemove;
          }
          else
            inCheEl = (CHE*)inCheEl->successor;
        }
        if (rr != 0)
          rr = 1;
        break;

      case NamedType:
        declGet = ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL);
        if (declGet)
          if (declGet->DeclDescType == EnumType) {
            rr = 1;
          }
          else {
            rr = findAnyForm(decl, decl->RefID, lavaCode);
            if ((rr > 0) && (decl->DeclType == Attr)) {
              declGet = ((TIDTable*)lavaCode->IDTable)->GetDECL(decl->RefID, decl->inINCL);
              if (declGet && (declGet->DeclType == FormDef))
                ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->BasicFlags.INCL(Groupbox);
            }
          }
        break;
      default:
        ;
    }
  }
  return rr;
}


void CLavaPEBrowse::makeDefaultMenu(LavaDECL* decl)
{
  CHE *inDefEl;
  CHE *ininDefEl;
  LavaDECL *inEl, *inCheDECL;
  CHEEnumSelId * enumsel;
  unsigned ml = 0;

  TAnnotation* anno = (TAnnotation*)decl->Annotation.ptr->FA.ptr;
  anno->BasicFlags.EXCL(ButtonMenu);
  anno->BasicFlags.EXCL(PopUpMenu);
  anno->BasicFlags.INCL(OptionMenu);
  TAnnotation* enumAnno = ((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Annotation.ptr;
  if (!enumAnno) {
    enumAnno = new TAnnotation;
    ((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Annotation.ptr = enumAnno;
  }
  if (!enumAnno->FA.ptr)
    enumAnno->FA.ptr = new TAnnotation;
  enumAnno = (TAnnotation*)enumAnno->FA.ptr;
  enumAnno->IoSigFlags.INCL(Enter);
  inEl = &((TEnumDescription*)decl->EnumDesc.ptr)->EnumField;
  ((TEnumDescription*)decl->EnumDesc.ptr)->MenuTree.DeclType = NoDef;
  ((TEnumDescription*)decl->EnumDesc.ptr)->MenuTree.DeclDescType = StructDesc;
  ((TEnumDescription*)decl->EnumDesc.ptr)->MenuTree.NestedDecls.Destroy();
  enumsel = (CHEEnumSelId*)inEl->Items.first;
  while (enumsel) {
    enumsel->data.SelectionCode = enumsel->data.Id;
    ml = lmax ( ml, enumsel->data.SelectionCode.l);
    inCheDECL = NewLavaDECL();
    inCheDECL->Annotation.ptr = new TAnnotation;
    inCheDECL->Annotation.ptr->FA.ptr = new TAnnotation;
    inCheDECL->LocalName = enumsel->data.Id; // = DefDesc;
    inCheDECL->DeclDescType = Undefined; // = DefDesc;
    inCheDECL->DeclType = Attr;
    ((TAnnotation*)inCheDECL->Annotation.ptr->FA.ptr)->BasicFlags.EXCL(BlankSelCode);
    inDefEl = NewCHE(inCheDECL);
    ((TEnumDescription*)decl->EnumDesc.ptr)->MenuTree.NestedDecls.Append(inDefEl);
    inCheDECL = NewLavaDECL();
    inCheDECL->Annotation.ptr = new TAnnotation;
    inCheDECL->Annotation.ptr->FA.ptr = new TAnnotation;
    inCheDECL->DeclDescType = LiteralString;
    inCheDECL->LitStr = enumsel->data.SelectionCode;
    ininDefEl = NewCHE(inCheDECL);
    /*((TAnnotation*)*/((LavaDECL*)inDefEl->data)->Annotation.ptr/*->FA.ptr)*/->Prefixes.Append(ininDefEl);
    inCheDECL = NewLavaDECL();
    inCheDECL->Annotation.ptr = new TAnnotation;
    inCheDECL->Annotation.ptr->FA.ptr = new TAnnotation;
    inCheDECL->DeclDescType = LiteralString;
    inCheDECL->LitStr = enumsel->data.Id;
    ((TAnnotation*)inCheDECL->Annotation.ptr->FA.ptr)->BasicFlags.EXCL(MenuText);
    ininDefEl = NewCHE(inCheDECL);
    /*((TAnnotation*)*/((LavaDECL*)inDefEl->data)->Annotation.ptr/*->FA.ptr)*/->Prefixes.Append(ininDefEl);
    enumsel = (CHEEnumSelId*)enumsel->successor;
  }//while
  enumAnno->Length.Field = ml;

}//makeDefaultMenu


bool CLavaPEBrowse::GotoDECL(wxDocument* fromDoc, LavaDECL* decl, TID id, bool sendMess, DString* enumID, bool openExec, TDeclType execType)
{
  CLavaPEDoc *doc;
  LavaDECL *declsel = 0, *formDECL = 0, *execDecl;
  bool popUp = false, activateMainView = false;
  CLavaBaseView *view;
  CTreeItem *item, *itemOld;
  int pos;

  mySynDef = ((CLavaPEDoc*)fromDoc)->mySynDef;
  if (id.nINCL > 0) {
    DString fn = ((CLavaPEDoc*)fromDoc)->IDTable.GetRelSynFileName(id);
    AbsPathName(fn, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
    doc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(fn.c);
    if (doc)
      declsel = doc->IDTable.GetDECL(0, id.nID);
  }
  else {
    doc = (CLavaPEDoc*)fromDoc;
    declsel = decl;
  }
  if (declsel) {
    //used only after GotoImpl means always the ExecDef
    if (openExec) {
      execDecl = doc->GetExecDECL(declsel, execType, false,false);
      return doc->OpenExecView(execDecl);
    }
    else {
      //pos = doc->GetFirstViewPos();
      view = (CLavaBaseView*)doc->m_documentViews[0]; //GetNextView(pos); //find the appropriate view
      if (declsel->DeclType == FormDef)
        formDECL = declsel->ParentDECL;
      else
        if (declsel->ParentDECL->DeclType == FormDef)
          formDECL = declsel->ParentDECL->ParentDECL;
      for (pos = 0; (pos < doc->m_documentViews.size())
                   && ( !doc->m_documentViews[pos]->inherits("CLavaPEView")
                       || !((CLavaPEView*)doc->m_documentViews[pos])->myInclView && !formDECL
                       || ((CLavaPEView*)doc->m_documentViews[pos])->myInclView && formDECL
                       || !(!formDECL && declsel->isInSubTree(((CLavaPEView*)doc->m_documentViews[pos])->myDECL)
                            || (formDECL == ((CLavaPEView*)doc->m_documentViews[pos])->myDECL)) );
           pos++);

      if (pos < doc->m_documentViews.size())
        view = (CLavaBaseView*)doc->m_documentViews[pos];
      else
        view = doc->MainView; //find the documents main view
      if (view) {
        popUp = (view == doc->MainView) && formDECL;
        if (!popUp)
          view->Activate(false);
        if (popUp)
          item = ((CLavaPEView*)view)->BrowseTree(formDECL, (CTreeItem*)((CLavaPEView*)view)->Tree->RootItem, enumID);
        else
          item = ((CLavaPEView*)view)->BrowseTree(declsel, (CTreeItem*)((CLavaPEView*)view)->Tree->RootItem, enumID);
        if (item) {
          itemOld = (CTreeItem*)((CLavaPEView*)view)->Tree->currentItem();
          if (itemOld)
            itemOld->setSelected(false);
          ((CLavaPEView*)view)->Tree->setCurAndSel(item);
          if (((CLavaPEView*)view)->myFormView && (item != ((CLavaPEView*)view)->Tree->RootItem))
            ((CLavaGUIView*)((CLavaPEView*)view)->myFormView)->SyncForm(declsel);
          if (popUp)
            if (formDECL) {
              ((CLavaPEView*)view)->OnShowSpecialView(FormDef);
              if (declsel->DeclType != FormDef)
                GotoDECL(fromDoc, decl, id, sendMess, enumID);
            }
            else {
              ((CLavaPEView*)view)->ExpandItem(item);
              ((CLavaPEView*)view)->Tree->scrollToItem(item, QAbstractItemView::EnsureVisible );//ensureItemVisible(item);
            }
          else
            ((CLavaPEView*)view)->Tree->scrollToItem(item, QAbstractItemView::EnsureVisible );//ensureItemVisible(item);
        }
        return true;
      }
      else {
        if (sendMess)
          QMessageBox::critical(((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView, qApp->applicationName(),IDP_RefNotFound,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        else
          QMessageBox::critical(wxDocManager::GetDocumentManager()->GetActiveView(), qApp->applicationName(),IDP_NoDefFound,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        return false;
      }
    }
  }
  else {
    if (sendMess)
      QMessageBox::critical(((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView, qApp->applicationName(),IDP_RefNotFound,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    else
      QMessageBox::critical(wxDocManager::GetDocumentManager()->GetActiveView(), qApp->applicationName(),IDP_NoDefFound,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    return false;
  }
}

bool CLavaPEBrowse::OnFindRefs(wxDocument* fromDoc, LavaDECL* decl, CFindData& fw)
{
  CHESimpleSyntax* cheSyn;
  CLavaPEDoc* doc;
  DString  fromAbsName; //fromRelName, relInclFile;
  int pos;
  TID fromID;
  DString allNames = komma;

  if (fw.index) {//by name
    fw.FWhere = findInIncl;
    CFindByNameBox* box = new CFindByNameBox(&fw, wxTheApp->m_appWindow);
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    if ((box->exec() != QDialog::Accepted) || !fw.searchName.l) {
      delete box;
      return false;
    }
    delete box;
  }
  else {
    CFindRefsBox* box = new CFindRefsBox(decl, (fw.FWhere == findInThisView), &fw, wxTheApp->m_appWindow);
    box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    if (box->exec() != QDialog::Accepted) {
      delete box;
      return false;
    }
    delete box;
  }
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->DeleteAllPageItems(tabFind);
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->SetTab(tabFind);
  if (!fw.index) {
    if (!decl || (fw.FWhere == findInThisView))
      return true;
    fw.refTid.nID = decl->OwnID;
    fw.refTid.nINCL = decl->inINCL;
  }
  if (!fw.index && (fw.FWhere == findInThisDoc)) {
    //((CLavaPEDoc*)fromDoc)->FindReferences(fromID, allNames, enumID, fw);
    ((CLavaPEDoc*)fromDoc)->FindReferences(allNames, fw);
    return true;
  }
  if (!fw.index) {
    fromAbsName = ((CLavaPEDoc*)fromDoc)->IDTable.IDTab[decl->inINCL]->FileName;
    AbsPathName(fromAbsName, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
  }
  wxDocManager* mana = wxDocManager::GetDocumentManager();
  /*
  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);*/
  for (pos = 0; pos < mana->m_docs.size(); pos++) {
    doc = (CLavaPEDoc*)mana->m_docs[pos];
    if (fw.index)
      doc->FindReferences(allNames, fw);
    else {
      if  (doc == (CLavaPEDoc*)fromDoc) {
        fw.refTid.nINCL = decl->inINCL;
        //doc->FindReferences(fromID, allNames, enumID, fw);
        doc->FindReferences(allNames, fw);
      }
      else {
        //fromRelName = fromAbsName;
        //RelPathName(fromRelName, doc->IDTable.DocDir);
        cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
        if (cheSyn) {
          cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is fromDoc included?
          for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, fromAbsName);//fromRelName, doc->IDTable.DocDir);
                 cheSyn = (CHESimpleSyntax*)cheSyn->successor);
          if (cheSyn) {
            fw.refTid.nINCL = cheSyn->data.nINCL;
            //doc->FindReferences(fromID, allNames, enumID, fw);
            doc->FindReferences(allNames, fw);
          }
        }
      }
    }
  }
  return true;
}

bool CLavaPEBrowse::FindImpl(wxDocument* fromDoc, LavaDECL* decl, wxDocument*& implDoc, LavaDECL**& p_implDecl/*in implDoc*/)
{
  CHESimpleSyntax* cheSyn;
  CLavaPEDoc* doc;
  DString implAbsName, fromAbsName; //fromRelName, relInclFile;
  int pos;
  TID fromID, toID;
  int funcnID;
  TIDType type;

  if ((decl->DeclType == Attr) || (decl->DeclType == Function)) {
    fromID.nID = decl->ParentDECL->OwnID; //interface ID
    funcnID = decl->OwnID;
  }
  else {
    fromID.nID = decl->OwnID;
    funcnID = 0;
  }
  //implementation and interface in the same document or in included docs?
  fromID.nINCL = decl->inINCL;

  toID = ((CLavaPEDoc*)fromDoc)->IDTable.FindImpl(fromID, funcnID);
  if (toID.nID >= 0) {
    if (toID.nINCL) {
      implAbsName = ((CLavaPEDoc*)fromDoc)->IDTable.IDTab[toID.nINCL]->FileName;
      AbsPathName(implAbsName, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
      implDoc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(implAbsName.c);
    }
    else
      implDoc = fromDoc;
    p_implDecl = (LavaDECL**)((CLavaPEDoc*)implDoc)->IDTable.GetVar(TID(toID.nID, 0), type);
    return true;
  }
  else {
    fromAbsName = ((CLavaPEDoc*)fromDoc)->IDTable.IDTab[decl->inINCL]->FileName;
    AbsPathName(fromAbsName, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
    //implementation and interface in different documents?
    wxDocManager* mana = wxDocManager::GetDocumentManager();
    /*
    pos = mana->GetFirstDocPos();
    while (pos) {
      doc = (CLavaPEDoc*)mana->GetNextDoc(pos);*/
    for (pos = 0; pos < mana->m_docs.size(); pos++) {
      doc = (CLavaPEDoc*)mana->m_docs[pos];
      if (doc != fromDoc) {
        //fromRelName = fromAbsName;
        //RelPathName(fromRelName, doc->IDTable.DocDir);
        cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
        if (cheSyn)
          cheSyn = (CHESimpleSyntax*)cheSyn->successor;
        for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, fromAbsName);//fromRelName, doc->IDTable.DocDir);
               cheSyn = (CHESimpleSyntax*)cheSyn->successor);
        if (cheSyn) {
          //the fromDoc is included
          fromID.nINCL = cheSyn->data.nINCL;
          toID = doc->IDTable.FindImpl(fromID, funcnID);
          if (toID.nID >= 0) {
            if (toID.nINCL) {
              implAbsName = ((CLavaPEDoc*)doc)->IDTable.IDTab[toID.nINCL]->FileName;
              AbsPathName(implAbsName, ((CLavaPEDoc*)doc)->IDTable.DocDir);
              implDoc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(implAbsName.c);
            }
            else
              implDoc = doc;
            p_implDecl = (LavaDECL**)((CLavaPEDoc*)implDoc)->IDTable.GetVar(TID(toID.nID, 0), type);
            return true;
          }
        }//cheSyn
      }
    }//while (pos)
  }
  return false;
}

bool CLavaPEBrowse::GotoImpl(wxDocument* fromDoc, LavaDECL* decl)
{
  CHESimpleSyntax* cheSyn;
  CLavaPEDoc* doc;
  DString fromAbsName; //,fromRelName, relInclFile;
  int pos;
  TID fromID, toID;
  int funcnID;
  LavaDECL *fromDecl, *implDECL;

  if (decl->DeclType == Initiator) {
    if (decl->inINCL > 0) {
      fromAbsName = ((CLavaPEDoc*)fromDoc)->IDTable.IDTab[decl->inINCL]->FileName;
      AbsPathName(fromAbsName, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
      doc = (CLavaPEDoc*)wxTheApp->m_docManager->FindOpenDocument(fromAbsName.c);
    }
    else
      doc = (CLavaPEDoc*)fromDoc;
    if (doc) {
      implDECL = doc->IDTable.GetDECL(0,decl->OwnID);
      if (implDECL) {
        ((CLavaPEDoc*)doc)->OpenExecView((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data);
        return true;
      }
    }
    else
      return false;
  }

  if ((decl->DeclType == Attr) || (decl->DeclType == Function)) {
    fromID.nID = decl->ParentDECL->OwnID;
    funcnID = decl->OwnID;
  }
  else {
    fromID.nID = decl->OwnID;
    funcnID = 0;
  }
  //implementation and interface in the same document?
  fromID.nINCL = decl->inINCL;

  toID = ((CLavaPEDoc*)fromDoc)->IDTable.FindImpl(fromID, funcnID);
  if (toID.nID >= 0) {
    fromDecl = ((CLavaPEDoc*)fromDoc)->IDTable.GetDECL(fromID);
    if (funcnID > 0) {
      implDECL = ((CLavaPEDoc*)fromDoc)->IDTable.GetDECL(toID);
      if (implDECL->TypeFlags.Contains(isPropSet) || implDECL->TypeFlags.Contains(isPropGet))
        implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(fromDoc, toID);
      else
        if (toID.nINCL)
          implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(fromDoc, toID, 0, true);
        else
          ((CLavaPEDoc*)fromDoc)->OpenExecView((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data);
    }
    else
      implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(fromDoc, toID);
    return true;
  }
  fromAbsName = ((CLavaPEDoc*)fromDoc)->IDTable.IDTab[decl->inINCL]->FileName;
  AbsPathName(fromAbsName, ((CLavaPEDoc*)fromDoc)->IDTable.DocDir);
  //implementation and interface in different documents?
  wxDocManager* mana = wxDocManager::GetDocumentManager();
  /*
  pos = mana->GetFirstDocPos();
  while (pos) {
    doc = (CLavaPEDoc*)mana->GetNextDoc(pos);*/
  for (pos = 0; pos < mana->m_docs.size(); pos++) {
    doc = (CLavaPEDoc*)mana->m_docs[pos];
    if (doc != fromDoc) {
      //fromRelName = fromAbsName;
      //RelPathName(fromRelName, doc->IDTable.DocDir);
      cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
      if (cheSyn)
        cheSyn = (CHESimpleSyntax*)cheSyn->successor;
      for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, fromAbsName);//fromRelName, doc->IDTable.DocDir);
             cheSyn = (CHESimpleSyntax*)cheSyn->successor);
      if (cheSyn) {
        //the fromDoc is included
        fromID.nINCL = cheSyn->data.nINCL;
        toID = doc->IDTable.FindImpl(fromID, funcnID);
        if (toID.nID >= 0) {
          fromDecl = doc->IDTable.GetDECL(fromID);
          if (funcnID > 0) {
            implDECL = doc->IDTable.GetDECL(toID);
            if (implDECL->TypeFlags.Contains(isPropGet) || implDECL->TypeFlags.Contains(isPropGet))
              implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(doc, toID);
            else
              if (toID.nINCL)
                implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(doc, toID, 0, true);
              else
                doc->OpenExecView((LavaDECL*)((CHE*)implDECL->NestedDecls.last)->data);
          }
          else
            implDECL = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(doc, toID);
          return true;
        }
      }
    }
  }
  return false;
}

