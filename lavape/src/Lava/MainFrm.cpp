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


#ifdef __GNUC__
#pragma implementation
#endif


#include "Lava.h"
#include "MainFrm.h"
#include "LavaDoc.h"
#include "LavaGUIView.h"
#include "LavaGUIFrame.h"
#include "Lava.xpm"
#include "qsignalmapper.h"
#include "qstylefactory.h"
#include "qdict.h"
//#include "stdafx.h"


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

CLavaMainFrame::CLavaMainFrame() : CMainFrame(0, "LavaMainFrame")
{
	makeStyle(LBaseData->m_style);

  QPopupMenu *style = new QPopupMenu(this);
  style->setCheckable( TRUE );
  viewMenu->insertItem( "Set st&yle" , style );
  QActionGroup *ag = new QActionGroup( this, 0 );
  ag->setExclusive( TRUE );
  QSignalMapper *styleMapper = new QSignalMapper( this );
  connect( styleMapper, SIGNAL( mapped(const QString&) ), this, SLOT( makeStyle(const QString&) ) );
  QStringList list = QStyleFactory::keys();
  list.sort();
  QDict<int> stylesDict( 17, FALSE );
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString styleStr = *it;
		QString styleAccel = styleStr;
		if ( stylesDict[styleAccel.left(1)] ) {
			for ( uint i = 0; i < styleAccel.length(); i++ ) {
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
		QAction *a = new QAction( styleStr, QIconSet(), styleAccel, 0, ag, 0, ag->isExclusive() );
		connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );
		styleMapper->setMapping(a,a->text() );
		if (LBaseData->m_style == styleStr)
		  a->setOn(true);
  }
  ag->addTo(style);

	m_childFrameHistory->m_menu = windowMenu;
  wxDocManager::GetDocumentManager()->m_fileHistory->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  LBaseData->insActionPtr = insAction;
  LBaseData->delActionPtr = delAction;
  LBaseData->okActionPtr = okAction;
  LBaseData->toggleCatActionPtr = toggleCategoryAction;
  LBaseData->updateCancelActionPtr = editUndoAction;
  LBaseData->editCopyActionPtr = editCopyAction;
  LBaseData->editCutActionPtr = editCutAction;
  LBaseData->editPasteActionPtr = editPasteAction;
  connect( okAction, SIGNAL( activated() ), this, SLOT( okAction_activated() ) );
  connect( toggleCategoryAction, SIGNAL( activated() ), this, SLOT( toggleCategoryAction_activated() ) );
  connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndoAction_activated() ) );
  connect( insAction, SIGNAL( activated() ), this, SLOT( insAction_activated() ) );
  connect( delAction, SIGNAL( activated() ), this, SLOT( delAction_activated() ) );
  setIcon(QPixmap((const char**) Lava));
  lastTile = 0;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  if (style.isEmpty()) return;

  LBaseData->m_style = style;
  wxTheApp->saveSettings();
	qApp->setStyle(style);
	if(style == "Motif" || style == "MotifPlus") {
	  QPalette p( QColor( 192, 192, 192 ) );
	  qApp->setPalette( p, TRUE );
	  qApp->setFont( appFont, TRUE );
	}

	if (completelyCreated)
		repaint();
}

void CLavaMainFrame::UpdateUI()
{
  CLavaDoc* doc = (CLavaDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
  bool enable = doc && doc->isObject;
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
			tileVertic();
			break;
		case 2:
			tileHoriz();
		}
}

void CLavaMainFrame::fileNew()
{
  ((CLavaApp*)wxTheApp)->OnFileNew();
}

void CLavaMainFrame::fileOpen()
{
  ((CLavaApp*)wxTheApp)->OnFileOpen();
}


void CLavaMainFrame::fileSave()
{
  wxDocManager::GetDocumentManager()->OnFileSave();
}

void CLavaMainFrame::fileSaveAs()
{
  wxDocManager::GetDocumentManager()->OnFileSaveAs();
}

void CLavaMainFrame::fileSaveAll()
{
  ((CLavaApp*)wxTheApp)->OnSaveAll();
}

void CLavaMainFrame::fileClose()
{
  wxDocManager::GetDocumentManager()->OnFileClose();
}

void CLavaMainFrame::helpAbout()
{
  ((CLavaApp*)wxTheApp)->OnAppAbout();
}


void CLavaMainFrame::fileExit()
{
  wxView *view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view && view->inherits("CLavaGUIView"))
    ((CLavaGUIView*)view)->NoteLastModified();
  CMainFrame::fileExit();
}

void CLavaMainFrame::tileVertic()
{
  TileVertic(menubar, lastTile);
}

void CLavaMainFrame::tileHoriz()
{
  TileHoriz(menubar, lastTile);
}
void CLavaMainFrame::cascade()
{
  Cascade();
}

CLavaMainFrame::~CLavaMainFrame()
{

}


void CLavaMainFrame::okAction_activated()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOK();
}

void CLavaMainFrame::toggleCategoryAction_activated()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTogglestate();
}

void CLavaMainFrame::editUndoAction_activated()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCancel();
}

void CLavaMainFrame::insAction_activated()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsertOpt();
}

void CLavaMainFrame::delAction_activated()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDeleteOpt();
}


void CLavaMainFrame::editCut()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditCut();
}

void CLavaMainFrame::editCopy()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditCopy();
}

void CLavaMainFrame::editPaste()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditPaste();
}

void CLavaMainFrame::setFormTextFont()
{
  ((CLavaApp*)wxTheApp)->OnChooseFormFont(0);
}

void CLavaMainFrame::setFormLabelFont()
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

void CLavaMainFrame::setGlobalFont()
{
  ((CLavaApp*)wxTheApp)->OnChooseGlobalFont();
}

void CLavaMainFrame::helpContents()
{
	((CLavaApp*)wxTheApp)->HtmlHelp();
}

void CLavaMainFrame::editingLavaProgs()
{
	((CLavaApp*)wxTheApp)->EditingLavaProgs();
}

void CLavaMainFrame::learningLava()
{
	((CLavaApp*)wxTheApp)->LearningLava();
}

