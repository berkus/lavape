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
#include "MainFrm.h"
#include "LavaDoc.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "qsignalmapper.h"
#include "qstylefactory.h"
#include "qhash.h"
//Added by qt3to4:
#include <QPixmap>
#include <QActionGroup>
#include <QMenu>

#pragma hdrstop
#include "Lava.xpm"


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

CLavaMainFrame::CLavaMainFrame() : wxMainFrame()
{
  LBaseData->docModal = 0;
  setObjectName("LavaMainFrame");
  setupUi(this); // populate this main frame

	makeStyle(LBaseData->m_style);

  QMenu *styleMenu = viewMenu->addMenu("Set st&yle");
  QActionGroup *ag = new QActionGroup( this);
  ag->setExclusive( TRUE );
  QSignalMapper *styleMapper = new QSignalMapper( this );
  connect( styleMapper,SIGNAL( mapped(const QString&) ),SLOT( makeStyle(const QString&) ) );

  QStringList list = QStyleFactory::keys();
  list.sort();
  QHash<QString,int*> stylesDict;
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString styleStr = *it;
		QAction *a = new QAction( styleStr,ag);
    a->setCheckable(true);
    QObject::connect(a,SIGNAL(triggered()),styleMapper,SLOT(map()));
		styleMapper->setMapping(a,a->text() );
		if (LBaseData->m_style == styleStr)
		  a->setChecked(true);
  }
  styleMenu->addActions(ag->actions());

	//m_childFrameHistory->m_menu = windowMenu;
  wxDocManager::GetDocumentManager()->m_fileHistory->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  LBaseData->insActionPtr = insAction;
  LBaseData->delActionPtr = delAction;
//  LBaseData->okActionPtr = okAction;
  LBaseData->toggleCatActionPtr = toggleCategoryAction;
  LBaseData->updateResetActionPtr = editUndoAction;
  LBaseData->editCopyActionPtr = editCopyAction;
  LBaseData->editCutActionPtr = editCutAction;
  LBaseData->editPasteActionPtr = editPasteAction;
  //connect( toggleCategoryAction,SIGNAL( triggered() ),SLOT( on_toggleCategoryAction_triggered() ) );
  //connect( editUndoAction,SIGNAL( triggered() ),SLOT( on_editUndoAction_triggered() ) );
  //connect( insAction,SIGNAL( triggered() ),SLOT( on_insAction_triggered() ) );
  //connect( delAction,SIGNAL( triggered() ),SLOT( on_delAction_triggered() ) );
  //setWindowIcon(QPixmap((const char**) Lava));
  lastTile = 0;

  installToolButtonEvtFilters(Toolbar_1);
  LBaseData->myWhatsThisAction = QWhatsThis::createAction(Toolbar_1);
  Toolbar_1->addAction(LBaseData->myWhatsThisAction);
  LBaseData->myWhatsThisAction->setWhatsThis("<p>Drag the \"What's this?\" cursor to any user interface object"
    " and drop it there to see a <b>little popup info (but usually more than a tooltip)</b> on that object.</p>");
  LBaseData->myWhatsThisAction->setToolTip("Enter \"WhatsThis help\" mode");
  LBaseData->myWhatsThisAction->setStatusTip("Enter \"WhatsThis help\" mode");
}

bool CLavaMainFrame::OnCreate() {
  wxHistory *fileHist = wxDocManager::GetDocumentManager()->m_fileHistory;
  DString fn;

  wxMainFrame::OnCreate();
  m_ClientArea = (QSplitter*)m_CentralWidget;

  for (int i = 0; i < fileHist->m_historyN; i++)
  {
    fn = *fileHist->m_history[i];
    if (fn.Substr(fn.l-5,5) == DString(".lcom"))
      fileHist->m_actions[i]->setEnabled(false);
  }
  return true;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  if (!style.isEmpty()) {
    LBaseData->m_style = style;
    wxTheApp->saveSettings();

#if !defined(QT_NO_STYLE_CDE)
    if (style == "CDE")
      QApplication::setStyle(new MyCDEStyle);
    else
#endif
#if !defined(QT_NO_STYLE_WINDOWS)
    if (style == "Windows")
      QApplication::setStyle(new MyWindowsStyle);
    else
#endif
#if defined(WIN32) && !defined(QT_NO_STYLE_WINDOWSXP)
    if (style == "WindowsXP")
      QApplication::setStyle(new MyWindowsXPStyle);
    else
#endif
#if defined(WIN32) && !defined(QT_NO_STYLE_WINDOWSVISTA)
    if (style == "WindowsVista")
      QApplication::setStyle(new MyWindowsVistaStyle);
    else
#endif
#if !defined(QT_NO_STYLE_CLEANLOOKS)
    if (style == "Cleanlooks")
      QApplication::setStyle(new MyCleanlooksStyle);
    else
#endif
#if !defined(QT_NO_STYLE_PLASTIQUE)
    if (style == "Plastique")
      QApplication::setStyle(new MyPlastiqueStyle);
    else
#endif
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
    if (style.startsWith(QLatin1String("macintosh")))
      QApplication::setStyle(new MyMacStyle);
    else
#endif
#if !defined(QT_NO_STYLE_GTK)
    if (style == "GTK+")
      QApplication::setStyle(new MyGtkStyle);
    else
#endif
#if !defined(QT_NO_STYLE_MOTIF)
    if (style == "Motif")
      QApplication::setStyle(new MyMotifStyle);
    else
#endif
    {}
  }

  if (completelyCreated)
	  repaint();
}

void CLavaMainFrame::UpdateUI()
{
  wxHistory *fileHist = wxDocManager::GetDocumentManager()->m_fileHistory;
  DString fn;

  CLavaDoc* doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
  bool enable = doc && doc->isObject;
  preconditionsAction->setEnabled(!LBaseData->m_checkPostconditions);
  preconditionsAction->setChecked(LBaseData->m_checkPreconditions);
  postconditionsAction->setChecked(LBaseData->m_checkPostconditions);
  invariantsAction->setChecked(LBaseData->m_checkInvariants);
  pmDumpAction->setChecked(LBaseData->m_pmDumps);
  fileNewAction->setEnabled(!LBaseData->debugger->isConnected);
  fileOpenAction->setEnabled(!LBaseData->debugger->isConnected);
  debugAction->setEnabled(!LBaseData->debugger->isConnected);
  //fileExitAction->setEnabled(!LBaseData->debugger->isConnected);
  fileCloseAction->setEnabled(enable);
  fileSaveAction->setEnabled(enable);
  fileSaveAsAction->setEnabled(enable);
  fileSaveAllAction->setEnabled(enable);
  if (LBaseData->docModal && ((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)
    ((LavaGUIDialog*)((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)->UpdateUI();

  for (int i = 0; i < fileHist->m_historyN; i++)
  {
    fn = *fileHist->m_history[i];
    fileHist->m_actions[i]->setEnabled(!LBaseData->debugger->isConnected && fn.Substr(fn.l-5,5) != DString(".lcom"));
  }
}

void CLavaMainFrame::on_fileNewAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnFileNew();
}

void CLavaMainFrame::on_fileOpenAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnFileOpen();
}

void CLavaMainFrame::on_debugAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnFileDebug();
}


void CLavaMainFrame::on_fileSaveAction_triggered()
{
  wxDocManager::GetDocumentManager()->OnFileSave();
}

void CLavaMainFrame::on_fileSaveAsAction_triggered()
{
  wxDocManager::GetDocumentManager()->OnFileSaveAs();
}

void CLavaMainFrame::on_fileSaveAllAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnSaveAll();
}

void CLavaMainFrame::on_fileCloseAction_triggered()
{
  wxDocManager::GetDocumentManager()->OnFileClose();
}

void CLavaMainFrame::on_helpAboutAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnAppAbout();
}


void CLavaMainFrame::on_fileExitAction_triggered()
{
  if (((CLavaDebugger*)LBaseData->debugger)->isConnected) {
    wxTheApp->appExit = true;
    ((CLavaDebugger*)LBaseData->debugger)->stop(disconnected);
    return;
  }
  wxMainFrame::OnFileExit();
}

void CLavaMainFrame::OnFileExit() {
  if (((CLavaDebugger*)LBaseData->debugger)->isConnected) {
    wxTheApp->appExit = true;
    ((CLavaDebugger*)LBaseData->debugger)->stop(disconnected);
    return;
  }
  wxMainFrame::OnFileExit();
}

CLavaMainFrame::~CLavaMainFrame()
{

}

/*
void CLavaMainFrame::on_okAction_triggered()
{
  if (((CLavaBaseDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->DumpFrame)
    ((LavaDumpFrame*)((CLavaBaseDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->DumpFrame)->OnOK();
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOK();
}
*/
void CLavaMainFrame::on_toggleCategoryAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTogglestate();
}

void CLavaMainFrame::on_editUndoAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnReset();
}

void CLavaMainFrame::on_insAction_triggered()
{
  if (LBaseData->docModal && ((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)
    ((LavaGUIDialog*)((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)->OnInsertOpt();
  else {
    CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
    if (view)
      view->OnInsertOpt();
  }
}

void CLavaMainFrame::on_delAction_triggered()
{
  if (LBaseData->docModal && ((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)
    ((LavaGUIDialog*)((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)->OnDeleteOpt();
  else {
    CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
    if (view)
      view->OnDeleteOpt();
  }

}


void CLavaMainFrame::on_editCutAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditCut();
}

void CLavaMainFrame::on_editCopyAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditCopy();
}

void CLavaMainFrame::on_editPasteAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditPaste();
}

void CLavaMainFrame::on_setFormTextFontAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnChooseFormFont(0);
}

void CLavaMainFrame::on_setFormLabelFontAction_triggered()
{
  LBaseData->useLabelFont = true;
  ((CLavaApp*)wxTheApp)->OnChooseFormFont(1);
}

/*
void CLavaMainFrame::setFormButtonFont()
{
  LBaseData->useButtonFont = true;
  ((CLavaApp*)wxTheApp)->OnChooseFormFont(2);
}
*/

void CLavaMainFrame::on_setGlobalFontAction_triggered()
{
  ((CLavaApp*)wxTheApp)->OnChooseGlobalFont();
}

void CLavaMainFrame::on_helpContentsAction_triggered()
{
	((CLavaApp*)wxTheApp)->HtmlHelp();
}

void CLavaMainFrame::on_editingLavaProgsAction_triggered()
{
	((CLavaApp*)wxTheApp)->EditingLavaProgs();
}

void CLavaMainFrame::on_learningLavaAction_triggered()
{
	((CLavaApp*)wxTheApp)->LearningLava();
}

void CLavaMainFrame::on_preconditionsAction_triggered(bool on)
{
  if (on) {
    LBaseData->m_checkPreconditions = true;
    LBaseData->m_strCheckPreconditions = "true";
  }
  else {
    LBaseData->m_checkPreconditions = false;
    LBaseData->m_strCheckPreconditions = "false";
  }
  ((CLavaApp*)wxTheApp)->saveSettings();
}

void CLavaMainFrame::on_postconditionsAction_triggered(bool on)
{
  if (on) {
    LBaseData->m_checkPostconditions = true;
    LBaseData->m_strCheckPostconditions = "true";
    LBaseData->m_checkPreconditions = true;
    LBaseData->m_strCheckPreconditions = "true";
  }
  else {
    LBaseData->m_checkPostconditions = false;
    LBaseData->m_strCheckPostconditions = "false";
  }
  ((CLavaApp*)wxTheApp)->saveSettings();
}

void CLavaMainFrame::on_invariantsAction_triggered(bool on)
{
  if (on) {
    LBaseData->m_checkInvariants = true;
    LBaseData->m_strCheckInvariants = "true";
  }
  else {
    LBaseData->m_checkInvariants = false;
    LBaseData->m_strCheckInvariants = "false";
  }
  ((CLavaApp*)wxTheApp)->saveSettings();
}

void CLavaMainFrame::on_pmDumpAction_triggered(bool on)
{
  if (on) {
    LBaseData->m_pmDumps = true;
    LBaseData->m_strPmDumps = "true";
  }
  else {
    LBaseData->m_pmDumps = false;
    LBaseData->m_strPmDumps = "false";
  }
  ((CLavaApp*)wxTheApp)->saveSettings();
}

void CLavaMainFrame::installToolButtonEvtFilters(QToolBar *tb) {
  QList<QToolButton*> buttonList = tb->findChildren<QToolButton*>();
  for (int i=0; i<buttonList.size(); i++)
    buttonList.at(i)->installEventFilter(this);
}

bool CLavaMainFrame::eventFilter(QObject *obj,QEvent *ev) {
  QWhatsThisClickedEvent *wtcEv;

  if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    wxTheApp->assistant->ShowPage(QString("html/whatsThis/")+wtcEv->href());
    return true;
  }
  else
    return false;
}
