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

	m_childFrameHistory->m_menu = windowMenu;
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
  setWindowIcon(QPixmap((const char**) Lava));
  lastTile = 0;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  if (style.isEmpty()) return;

  LBaseData->m_style = style;
  wxTheApp->saveSettings();
  if (!style.isEmpty()) {
    if (style == "Windows")
      QApplication::setStyle(new MyWindowsStyle);
#ifdef WIN32
    else if (style == "WindowsXP")
      QApplication::setStyle(new MyWindowsXPStyle);
#endif
#ifdef Darwin
    else if (style == "Mac")
      QApplication::setStyle(new MyMacStyle);
#endif
    else if (style == "CDE")
      QApplication::setStyle(new MyCDEStyle);
    else if (style == "Motif")
      QApplication::setStyle(new MyMotifStyle);
    else if (style == "Plastique")
      QApplication::setStyle(new MyPlastiqueStyle);
    else if (style == "Cleanlooks")
      QApplication::setStyle(new MyCleanlooksStyle);
    else
	    QApplication::setStyle(style);
	  if (completelyCreated)
		  repaint();
  }
}

void CLavaMainFrame::UpdateUI()
{
  CLavaDoc* doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
  bool enable = doc && doc->isObject;
  preconditionsAction->setEnabled(!LBaseData->m_checkPostconditions);
  preconditionsAction->setChecked(LBaseData->m_checkPreconditions);
  postconditionsAction->setChecked(LBaseData->m_checkPostconditions);
  invariantsAction->setChecked(LBaseData->m_checkInvariants);
  pmDumpAction->setChecked(LBaseData->m_pmDumps);
  fileCloseAction->setEnabled(enable);
  fileSaveAction->setEnabled(enable);
  fileSaveAsAction->setEnabled(enable);
  fileSaveAllAction->setEnabled(enable);
  if (LBaseData->docModal && ((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)
    ((LavaGUIDialog*)((CLavaBaseDoc*)LBaseData->docModal)->ActLavaDialog)->UpdateUI();
}

void CLavaMainFrame::customEvent(QEvent *ev0){
	HistWindow *hw;
	DString title;
  CustomEvent *ev=(CustomEvent*)ev0;

	if (ev->data()) {
		title = DString(qPrintable(((QWidget*)ev->data())->windowTitle()));
		if (title.l) {
			if (title[title.l-1] == '*')
				title = title.Substr(0,title.l-1);
			hw = new HistWindow(title,(wxMDIChildFrame*)ev->data());
			m_childFrameHistory->AddToHistory(hw,this);
		}
	}
	else
		switch (lastTile) {
		case 0:
		case 1:
			on_tileVerticAction_triggered();
			break;
		case 2:
			on_tileHorizAction_triggered();
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
  OnFileExit();
}

void CLavaMainFrame::on_tileVerticAction_triggered()
{
  TileVertic(menubar, lastTile);
}

void CLavaMainFrame::on_tileHorizAction_triggered()
{
  TileHoriz(menubar, lastTile);
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

