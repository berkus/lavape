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
#include "Lava.xpm"
#include "qsignalmapper.h"
#include "qstylefactory.h"
#include "q3dict.h"
//Added by qt3to4:
#include <QPixmap>
#include <QCustomEvent>
#include <QActionGroup>
#include <QMenu>


/////////////////////////////////////////////////////////////////////////////
// CMainFrame
/*
static UINT indicators[] =
{
  ID_SEPARATOR,           // Statusleistenanzeige
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
};
*/

CLavaMainFrame::CLavaMainFrame() : wxMainFrame(0, "LavaMainFrame")
{
  m_CentralWidget->setMinimumSize(500,300);

  setIconSize(QSize(16,16));
  setupUi(this); // populate this main frame

	makeStyle(LBaseData->m_style);

  QMenu *styleMenu = new QMenu(this);
  styleMenu->setCheckable( TRUE );
  viewMenu->insertItem( "Set st&yle" , styleMenu );
  QActionGroup *ag = new QActionGroup( this);
  ag->setExclusive( TRUE );
  QSignalMapper *styleMapper = new QSignalMapper( this );
  connect( styleMapper, SIGNAL( mapped(const QString&) ), this, SLOT( makeStyle(const QString&) ) );
  QStringList list = QStyleFactory::keys();
  list.sort();
  Q3Dict<int> stylesDict( 17, FALSE );
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString styleStr = *it;
		QString styleAccel = styleStr;
		if ( stylesDict[styleAccel.left(1)] ) {
			for ( int i = 0; i < styleAccel.length(); i++ ) {
				if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
					stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
					styleAccel = styleAccel.insert( i, '&' );
					break;
				}
			}
		}
		else {
			stylesDict.insert(styleAccel.left(1), (const int *)1);
			styleAccel = "&"+styleAccel;
 		}
//		QAction *a = new QAction( styleStr, QIcon(), styleAccel, 0, ag, 0, ag->isExclusive() );
		QAction *a = new QAction( styleStr,ag);
		connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );
		styleMapper->setMapping(a,a->text() );
		if (LBaseData->m_style == styleStr)
		  a->setOn(true);
  }
  ag->addTo(styleMenu);

	m_childFrameHistory->m_menu = windowMenu;
  wxDocManager::GetDocumentManager()->m_fileHistory->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  LBaseData->insActionPtr = insAction;
  LBaseData->delActionPtr = delAction;
//  LBaseData->okActionPtr = okAction;
  LBaseData->toggleCatActionPtr = toggleCategoryAction;
  LBaseData->updateCancelActionPtr = editUndoAction;
  LBaseData->editCopyActionPtr = editCopyAction;
  LBaseData->editCutActionPtr = editCutAction;
  LBaseData->editPasteActionPtr = editPasteAction;
//  connect( okAction, SIGNAL( activated() ), this, SLOT( okAction_triggered() ) );
  connect( toggleCategoryAction, SIGNAL( activated() ), this, SLOT( on_toggleCategoryAction_triggered() ) );
  connect( editUndoAction, SIGNAL( activated() ), this, SLOT( on_editUndoAction_triggered() ) );
  connect( insAction, SIGNAL( activated() ), this, SLOT( on_insAction_triggered() ) );
  connect( delAction, SIGNAL( activated() ), this, SLOT( on_delAction_triggered() ) );
  setIcon(QPixmap((const char**) Lava));
  lastTile = 0;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  if (style.isEmpty()) return;

  LBaseData->m_style = style;
  wxTheApp->saveSettings();
  if (style == "Windows")
    QApplication::setStyle(new MyWindowsStyle);
  else
	  QApplication::setStyle(style);
/*
	if(style == "Motif" || style == "MotifPlus") {
	  QPalette p( QColor( 192, 192, 192 ) );
	  qApp->setPalette( p, TRUE );
	  qApp->setFont( LBaseData->m_GlobalFont, TRUE );
	}
*/
	if (completelyCreated)
		repaint();
}

int MyWindowsStyle::pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const
{
  int px = QWindowsStyle::pixelMetric( pm, option, widget);
  
  switch( pm )
  {
    case PM_ToolBarItemMargin:
    case PM_ToolBarItemSpacing:
      px = 0; break;
    case PM_ToolBarIconSize:
      px = 16; break;
    default: break;
  }
  
  return px;
}
void CLavaMainFrame::UpdateUI()
{
  CLavaDoc* doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
  bool enable = doc && doc->isObject;
  preconditionsAction->setEnabled(!LBaseData->m_checkPostconditions);  
  preconditionsAction->setOn(LBaseData->m_checkPreconditions);  
  postconditionsAction->setOn(LBaseData->m_checkPostconditions);  
  invariantsAction->setOn(LBaseData->m_checkInvariants); 
  pmDumpAction->setOn(LBaseData->m_pmDumps);
  fileCloseAction->setEnabled(enable);
  fileSaveAction->setEnabled(enable);
  fileSaveAsAction->setEnabled(enable);
  fileSaveAllAction->setEnabled(enable);
}

void CLavaMainFrame::customEvent(QCustomEvent *ev){
	HistWindow *hw;
	DString title;
	
	if (ev->data()) {
		title = DString(((QWidget*)ev->data())->caption());
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
  wxView *view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view && view->inherits("CLavaGUIView"))
    ((CLavaGUIView*)view)->NoteLastModified();
  wxMainFrame::fileExit();
}

void CLavaMainFrame::on_tileVerticAction_triggered()
{
  wxMainFrame::on_tileVerticAction_triggered(menubar, lastTile);
}

void CLavaMainFrame::on_tileHorizAction_triggered()
{
  wxMainFrame::on_tileHorizAction_triggered(menubar, lastTile);
}

CLavaMainFrame::~CLavaMainFrame()
{

}

/*
void CLavaMainFrame::okAction_triggered()
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
    view->OnCancel();
}

void CLavaMainFrame::on_insAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsertOpt();
}

void CLavaMainFrame::on_delAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDeleteOpt();
}


void CLavaMainFrame::on_editCutAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditCut();
}

void CLavaMainFrame::on_editCutCopy_triggered()
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

