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
//#include "ASN1File.h"
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
#include "qclipboard.h"
#include "qfiledialog.h"
#include "qfontdialog.h"
#include "qmessagebox.h"
#include <signal.h>
#include "qassistantclient.h"
#include "qprocess.h"
#include <stdlib.h>

#ifndef WIN32
#include <locale.h>
#endif


static QString szCheckPreconditions = "/CheckPreconditions";
static QString szCheckPostconditions = "/CheckPostconditions";
static QString szCheckInvariants = "/CheckInvariants";
static QString szPmDumps = "/PmDumps";
static QString szFormFont = "/FormFont";
static QString szFormLabelFont = "/FormLabelFont";
static QString szGlobalFont = "/GlobalFont";
static QString favoriteBrowser = "/MyBrowser";
static QString gui_style = "/Style";

static char slash='/';

/////////////////////////////////////////////////////////////////////////////
// CLavaApp


int main( int argc, char ** argv ) {
	CLavaApp ap(argc,argv);
  QString componentPath;

//  DebugBreak();
  if (sock_init())
    qDebug("sock_init failed");

  componentPath = getenv("PATH");
#ifdef WIN32
  if (componentPath.isEmpty())
    componentPath = ExeDir + "\\Components";
  else
    componentPath = componentPath + ";" + ExeDir + "\\Components";
  _putenv((QString("PATH=")+componentPath).latin1());
#else
  if (componentPath.isEmpty())
    componentPath = ExeDir + "/Components";
  else
    componentPath = componentPath + ";" + ExeDir + "/Components";
  putenv((char *)(QString("PATH=")+componentPath).latin1());
//  setenv("PATH",componentPath.latin1(),1);
#endif

  ap.m_appWindow = new CLavaMainFrame;
  if (ap.m_appWindow->OnCreate()) {
    ap.m_appWindow->resize(350,300);
    ap.m_appWindow->show();
		//ap.m_appWindow->showMaximized();
  }
	else
		return 1;
	
	threadStg()->setLocalData(new CThreadData(0)); 

	int res = ap.exec();
  
  if (allocatedObjects) {
    qDebug("\n\nMemory leak: %x orphaned Lava object(s)\n\n",allocatedObjects);
    QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), QString("Memory leak: %1 orphaned Lava object(s)").arg(allocatedObjects),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  }
	return res;
}


/////////////////////////////////////////////////////////////////////////////
// CLavaApp Konstruktion

CLavaApp::CLavaApp(int argc, char ** argv )
:wxApp(argc, argv )
{
  bool ok;
#ifndef WIN32
  int rc;
  sigset_t sigs;
#endif
  SynIO.INIT();
  InitGlobalStrings();
  qt_use_native_dialogs = false;

  SetVendorName("Fraunhofer-SIT");
  SetAppName("LavaPE");
  QSettings settings(QSettings::Native);
  settings.setPath(wxTheApp->GetVendorName(),wxTheApp->GetAppName(),QSettings::User);
//  settings.beginGroup(GetSettingsPath());
  LBaseData.theApp = this;
  LBaseData.inRuntime = true;
  clipboard()->clear();
  LBaseData.actHint = 0;
  LBaseData.Init(0, 0);
  LBaseData.debugThread = &debugThread;

  settings.beginGroup("/generalSettings");
  LBaseData.m_strCheckPreconditions = settings.readEntry(szCheckPreconditions, 0, &ok);
  if (ok)
    if (LBaseData.m_strCheckPreconditions == "true")
      LBaseData.m_checkPreconditions = true;
    else
      LBaseData.m_checkPreconditions = false;
  else {
    LBaseData.m_checkPreconditions = true;
    LBaseData.m_strCheckPreconditions = "true";
  }
  LBaseData.m_strCheckPostconditions = settings.readEntry(szCheckPostconditions, 0, &ok);
  if (ok)
    if (LBaseData.m_strCheckPostconditions == "true")
      LBaseData.m_checkPostconditions = true;
    else
      LBaseData.m_checkPostconditions = false;
  else {
    LBaseData.m_checkPostconditions = false;
    LBaseData.m_strCheckPostconditions = "false";
  }
  LBaseData.m_strCheckInvariants = settings.readEntry(szCheckInvariants, 0, &ok);
  if (ok)
    if (LBaseData.m_strCheckInvariants == "true")
      LBaseData.m_checkInvariants = true;
    else
      LBaseData.m_checkInvariants = false;
  else {
    LBaseData.m_checkInvariants = false;
    LBaseData.m_strCheckInvariants = "false";
  }
  LBaseData.m_strPmDumps = settings.readEntry(szPmDumps, 0, &ok);
  if (ok)
    if (LBaseData.m_strPmDumps == "true")
      LBaseData.m_pmDumps = true;
    else
      LBaseData.m_pmDumps = false;
  else {
    LBaseData.m_pmDumps = true;
    LBaseData.m_strPmDumps = "true";
  }
  settings.endGroup();

  settings.beginGroup("/fontSettings");

  LBaseData.m_lfDefFormFont = settings.readEntry(szFormFont, 0, &ok);
  if (ok)
    LBaseData.m_FormFont.fromString(LBaseData.m_lfDefFormFont);
  else {
    LBaseData.m_FormFont = QApplication::font();
    LBaseData.m_lfDefFormFont = LBaseData.m_FormFont.toString();
  }

  LBaseData.m_lfDefFormLabelFont = settings.readEntry(szFormLabelFont, 0, &ok);
  if (ok) {
    LBaseData.m_FormLabelFont.fromString(LBaseData.m_lfDefFormLabelFont);
    LBaseData.useLabelFont = true;
  }
  else {
    LBaseData.m_FormLabelFont = LBaseData.m_FormFont;//QApplication::font();
    LBaseData.m_lfDefFormLabelFont = LBaseData.m_lfDefFormFont;//LBaseData.m_FormFont.toString();
    LBaseData.useLabelFont = false;
  }

  LBaseData.m_lfDefGlobalFont = settings.readEntry(szGlobalFont, 0, &ok);
  if (ok)
    LBaseData.m_GlobalFont.fromString(LBaseData.m_lfDefGlobalFont);
  else {
    LBaseData.m_GlobalFont = QApplication::font();
    LBaseData.m_lfDefGlobalFont = LBaseData.m_GlobalFont.toString();
  }
  setFont(LBaseData.m_GlobalFont,false);

  settings.endGroup();

  settings.beginGroup("/otherSettings");
  LBaseData.m_myWebBrowser = settings.readEntry(favoriteBrowser, 0, &ok);
  LBaseData.m_style = settings.readEntry(gui_style, 0, &ok);
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
  
  ExeDir = applicationDirPath();
#ifdef WIN32
  QString driveLetter = QString(ExeDir[0].upper());
  ExeDir.replace(0,1,driveLetter);
#endif
  StdLavaLog = ExeDir + "/std.lava";
  QFileInfo qf = QFileInfo(StdLavaLog);
	StdLava = ResolveLinks(qf);
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

  rc = RegOpenKeyEx(HKEY_CLASSES_ROOT,".htm",NULL,KEY_QUERY_VALUE,&hkey);
  rc = RegQueryValueEx(hkey,NULL,NULL,&valType,data,&dataSize);
  rc = RegOpenKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)&data[0],0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"shell",0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"open",0,KEY_QUERY_VALUE,&hkey);
  rc = RegOpenKeyEx(hkey,"command",0,KEY_QUERY_VALUE,&hkey);
  dataSize = 100;
  rc = RegQueryValueEx(hkey,0,0,&valType,&data[0],&dataSize);
  str = QString(datap);
  if (str[0] == '"') {
    pos = str.find('\"',1);
    prog = str.mid(1,pos-1);
  }
  else {
    if (pos = str.find(' ',1))
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
	CLavaDoc *doc;
	CMsgBoxParams *mbp;
	CLavaPEHint *pHint;
	CLavaThread *thr;
  DumpEventData* dumpdata;
  int result;
  CheckData ckd;

	switch (e->type()) {
  case IDU_LavaStart:
    thr = new CLavaThread(ExecuteLava,(CLavaDoc*)((QCustomEvent*)e)->data());
    thr->start();
    break;
	case IDU_LavaEnd:
	  pHint = (CLavaPEHint*)((QCustomEvent*)e)->data();
		doc = (CLavaDoc*)pHint->fromDoc;
/*		thr = (CLavaThread*)pHint->CommandData1;
    if (thr) {
		  thr->wait();
		 // delete thr;
    }*/
		delete (CLavaPEHint*)pHint;
    if (doc)
		  doc->OnCloseDocument();
    return true;
	case IDU_LavaMsgBox:
    mainWidget()->setActiveWindow();
    mainWidget()->raise();
		mbp = (CMsgBoxParams*)((QCustomEvent*)e)->data();
		switch (mbp->funcSpec) {
		case 0:
			mbp->result =	QMessageBox::critical(
				mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
			(*mbp->thr->pContExecEvent)--;
			break;
		case 1:
			mbp->result =	QMessageBox::information(
				mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
			(*mbp->thr->pContExecEvent)--;
			break;
		case 2:
			mbp->result =	QMessageBox::question(
				mbp->parent,*mbp->caption,*mbp->text,mbp->button0,mbp->button1,mbp->button2);
			(*mbp->thr->pContExecEvent)--;
			break;
		}
    break;
	case IDU_LavaShow:
    pHint = (CLavaPEHint*)((QCustomEvent*)e)->data();
    ((CLavaDoc*)pHint->fromDoc)->LavaDialog = new LavaGUIDialog(((wxMainFrame*)mainWidget()), pHint); 
    result = ((QDialog*)((CLavaDoc*)pHint->fromDoc)->LavaDialog)->exec();
    delete ((CLavaDoc*)pHint->fromDoc)->LavaDialog;
    ((CLavaDoc*)pHint->fromDoc)->LavaDialog = 0;
    if (pHint->CommandData5) {
      thr = (CLavaThread*)((CLavaPEHint*)((QCustomEvent*)e)->data())->CommandData5;
      if (result == QDialog::Rejected) {
        if (!thr->pContExecEvent->ex)
          ckd.document = (CLavaBaseDoc*)pHint->fromDoc;
          thr->pContExecEvent->ex = new CRuntimeException(RunTimeException_ex, &ERR_CanceledForm);
          //SetLavaException(ckd, CanceledForm_ex, ERR_CanceledForm);
      }
      (*thr->pContExecEvent)--;
    }
    delete (CLavaPEHint*)((QCustomEvent*)e)->data();
		break;
  case IDU_LavaDump:
    dumpdata = (DumpEventData*)((QCustomEvent*)e)->data();
    dumpdata->doc->DumpFrame = new LavaDumpFrame(((wxMainFrame*)mainWidget()), dumpdata); 
    ((QDialog*)dumpdata->doc->DumpFrame)->exec();
    delete dumpdata->doc->DumpFrame;
    dumpdata->doc->DumpFrame = 0;
    (*((DumpEventData*)((QCustomEvent*)e)->data())->currentThread->pContExecEvent)--;
    delete (DumpEventData*)((QCustomEvent*)e)->data();
		break;
	default:
		wxApp::event(e);
	}
	return QApplication::event(e);
}



QString lavaFileDialog(const QString& startFileName, QWidget* parent, const QString& caption, bool existing)
{
    
  return L_GetOpenFileName(startFileName, parent, caption,
          "Lava file (*.lava)", "lava", "Lava object file (*.ldoc)", "ldoc");


}

void CLavaApp::OnFileOpen() 
{
  QString fileName = lavaFileDialog(LBaseData.lastFileOpen, m_appWindow, "Select a file to open", true);
  if (fileName.isEmpty())
    return;
#ifdef WIN32
  QString driveLetter = QString(fileName[0].upper());
  fileName.replace(0,1,driveLetter);
#endif
  OpenDocumentFile(fileName);
}

void CLavaApp::OpenDocumentFile(const QString& lpszFileName) 
{
  QString name;
  QDir cwd;
  CLavaDoc* doc;

	name = cwd.absFilePath(lpszFileName);
	name = cwd.cleanDirPath(name);
	if (!name.contains('.'))
		return;
  LBaseData.lastFileOpen = name;
  if (argc() > 2) {
    debugThread.remoteIPAddress = QString(argv()[2]);
    debugThread.remotePort = QString(argv()[3]);

    //QMessageBox::critical(/*qApp->*/mainWidget(),qApp->name(),"Lava Interpreter: Debug support not yet fully implemented" ,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    debugThread.debugOn = true;
    LBaseData.debugOn = true;
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
    doc->startedFromLavaPE = true;
    ((CLavaMainFrame*)m_appWindow)->fileOpenAction->setEnabled(false);
    ((CLavaMainFrame*)m_appWindow)->fileNewAction->setEnabled(false);
    debugThread.initData(doc);
    debugThread.start();
  }
  else {
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->CreateDocument(name,wxDOC_SILENT);
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
  POSITION pos = wxDocManager::GetDocumentManager()->GetFirstDocPos();
  while (pos) {
    doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->GetNextDoc(pos);
    if ((((CLavaApp*)wxTheApp)->pLavaLdocTemplate == doc->GetDocumentTemplate())
        && doc->IsModified())
      ret = ret && doc->Save();
  }
  return ret;
}

void CLavaApp::OnAppAbout()
{
  CAboutBox aboutDlg(mainWidget(),"About Lava",true);
  aboutDlg.exec();
}

void CLavaApp::saveSettings()
{
  QSettings settings(QSettings::Native);

  settings.setPath(wxTheApp->GetVendorName(),wxTheApp->GetAppName(),QSettings::User);
//  settings.beginGroup(GetSettingsPath());

  settings.beginGroup("/generalSettings");
  settings.writeEntry(szCheckPreconditions,LBaseData.m_strCheckPreconditions);
  settings.writeEntry(szCheckPostconditions,LBaseData.m_strCheckPostconditions);
  settings.writeEntry(szCheckInvariants,LBaseData.m_strCheckInvariants);
  settings.writeEntry(szPmDumps,LBaseData.m_strPmDumps);
  settings.endGroup();

  settings.beginGroup("/fontSettings");
  settings.writeEntry(szFormFont,LBaseData.m_lfDefFormFont);
  if (LBaseData.useLabelFont)
    settings.writeEntry(szFormLabelFont,LBaseData.m_lfDefFormLabelFont);
  settings.writeEntry(szGlobalFont,LBaseData.m_lfDefGlobalFont);
  settings.endGroup();

  settings.beginGroup("/otherSettings");
  settings.writeEntry(favoriteBrowser,LBaseData.m_myWebBrowser);
  settings.writeEntry(gui_style,LBaseData.m_style);
  settings.endGroup();
}

int CLavaApp::ExitInstance() 
{
  saveSettings();
  delete [] TOKENSTR;
  return 0;
}


void CLavaApp::OnChooseFormFont(int font_case) 
{
  CLavaDoc* doc;
  POSITION posD, posV;
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
      posD = dm->GetFirstDocPos();
      while (posD) {
        doc = (CLavaDoc*)dm->GetNextDoc(posD);
        posV = doc->GetFirstViewPos();
        while (posV) {
          view = doc->GetNextView(posV);
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
      posD = dm->GetFirstDocPos();
      while (posD) {
        doc = (CLavaDoc*)dm->GetNextDoc(posD);
        posV = doc->GetFirstViewPos();
        while (posV) {
          view = doc->GetNextView(posV);
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
	  setFont(LBaseData.m_GlobalFont,true);
    saveSettings();
  }
}


void CLavaApp::HtmlHelp()
{
	QString path(ExeDir);
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
	QString path(ExeDir);
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
