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


#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "LavaPE.h"
#include "LavaAppBase.h"
#include "LavaPEView.h"
#include "LavaGUIView.h"
//#include "ExecView.h"
#include "VTView.h"
#include "InclView.h"
#include "LavaGUIFrame.h"
#include "LavaPEDoc.h"
#include "LavaPEFrames.h"
#include "LavaPEWizard.h"
#include "Bars.h"
#include "Resource.h"

#include "setup.h"
#include "docview.h"
#include "mdiframes.h"

#include "qpixmap.h"
#include "qpushbutton.h"
#include "qiconset.h"
#include "qmainwindow.h"
#include "qmenubar.h"
#include "qmessagebox.h"
#include "qcanvas.h"
#include "qcursor.h"
#include "qpopupmenu.h"
#include "qlistview.h"
#include "qvbox.h"
#include "qwhatsthis.h"
#include "qworkspace.h"
#include "qstatusbar.h"
#include "qaction.h"
#include "qsplitter.h"
#include "qsize.h"
#include "qfontinfo.h"
#include "qapplication.h"
#include "qstylefactory.h"
#include "qpalette.h"
#include "qaction.h"
#include "qsignalmapper.h"
#include "qdict.h"
#include "qevent.h"



CLavaMainFrame::CLavaMainFrame(QWidget* parent, const char* name, WFlags fl)
:CMainFrame(parent, name, fl)
{
  if (!LBaseData->m_style.isEmpty())
	  makeStyle(LBaseData->m_style);

  m_OutputBar = 0;
	lastTile = 0;
  QPopupMenu *style = new QPopupMenu(this);
  style->setCheckable( TRUE );
  viewMenu->insertItem( "Set st&yle" , style );
  QActionGroup *ag = new QActionGroup( this, 0 );
  ag->setExclusive( TRUE );
  QSignalMapper *styleMapper = new QSignalMapper( this );
  connect( styleMapper, SIGNAL( mapped( const QString& ) ), this, SLOT( makeStyle( const QString& ) ) );
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

  LBaseData->insActionPtr = insAction;
  LBaseData->delActionPtr = delAction;
  LBaseData->okActionPtr = 0;
  LBaseData->toggleCatActionPtr = 0;
  LBaseData->updateCancelActionPtr = 0;

  LBaseData->additionActionPtr = additionAction;
  LBaseData->bitAndActionPtr = bitAndAction;
  LBaseData->bitOrActionPtr = bitOrAction;
  LBaseData->bitXorActionPtr = bitXorAction;
  LBaseData->commentOutActionPtr = commentOutAction;
  LBaseData->conflictingAssigActionPtr = conflictingAssigAction;
  LBaseData->deleteActionPtr = deleteAction;
  LBaseData->divideActionPtr = divideAction;
  LBaseData->editCommentActionPtr = editCommentAction;
  LBaseData->editCopyActionPtr = editCopyAction;
  LBaseData->editCutActionPtr = editCutAction;
  LBaseData->editPasteActionPtr = editPasteAction;
  LBaseData->editSelItemActionPtr = editSelItemAction;
  LBaseData->editRedoActionPtr = editRedoAction;
  LBaseData->editUndoActionPtr = editUndoAction;
  LBaseData->equalActionPtr = equalAction;
  LBaseData->evaluateActionPtr = evaluateAction;
  LBaseData->falseActionPtr = falseAction;
  LBaseData->findRefsActionPtr = findRefsAction;
  LBaseData->functionCallActionPtr = functionCallAction;
  LBaseData->gotoDeclActionPtr = gotoDeclAction;
  LBaseData->gotoImplActionPtr = gotoImplAction;
  LBaseData->greaterEqualActionPtr = greaterEqualAction;
  LBaseData->greaterThanActionPtr = greaterThanAction;
  LBaseData->handleActionPtr = handleAction;
  LBaseData->insertActionPtr = insertAction;
  LBaseData->insertBeforeActionPtr = insertBeforeAction;
  LBaseData->intervalActionPtr = intervalAction;
  LBaseData->invertActionPtr = invertAction;
  LBaseData->leftShiftActionPtr = leftShiftAction;
  LBaseData->lessEqualActionPtr = lessEqualAction; 
  LBaseData->lessThanActionPtr = lessThanAction;
  LBaseData->modulusActionPtr = modulusAction;
  LBaseData->multiplicationActionPtr = multiplicationAction;
  LBaseData->newLineActionPtr = newLineAction;
  LBaseData->nextCommentActionPtr = nextCommentAction;
  LBaseData->nextErrorActionPtr = nextErrorAction;
  LBaseData->notEqualActionPtr = notEqualAction;
  LBaseData->nullActionPtr = nullAction;
  LBaseData->optLocalVarActionPtr = optLocalVarAction;
  LBaseData->ordActionPtr = ordAction;
  LBaseData->prevCommentActionPtr = prevCommentAction;
  LBaseData->prevErrorActionPtr = prevErrorAction;
  LBaseData->rightShiftActionPtr = rightShiftAction;
  LBaseData->showOptsActionPtr = showOptsAction;
  LBaseData->staticCallActionPtr = staticCallAction;
  LBaseData->toggleArrowsActionPtr = toggleArrowsAction;
  LBaseData->toggleCategoryActionPtr = toggleCategoryAction; 
  LBaseData->toggleCommentsActionPtr = toggleCommentsAction;
  LBaseData->toggleInputArrowsActionPtr = toggleInputArrowsAction;
  LBaseData->toggleSignActionPtr = toggleSignAction;
  LBaseData->toggleSubstTypeActionPtr = toggleSubstTypeAction;
  LBaseData->trueActionPtr = trueAction;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  LBaseData->m_style = style;
  wxTheApp->saveSettings();
	qApp->setStyle(style);
/*
	if(style == "Platinum") {
	  QPalette p( QColor( 239, 239, 239 ) );
	  qApp->setPalette( p, TRUE );
	  qApp->setFont( appFont, TRUE );
	} 
	else if(style == "Windows") {
	  qApp->setFont( appFont, TRUE );
	} 
	else if(style == "CDE") {
	  QPalette p( QColor( 75, 123, 130 ) );
	  p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
	  p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
	  p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
	  p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	  p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
	  p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
	  p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
	  p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
	  p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
	  p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
	  p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
	  p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
	  p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
	  qApp->setPalette( p, TRUE );
	  qApp->setFont( QFont( "times", appFont.pointSize() ), TRUE );
	} */
	if(style == "Motif" || style == "MotifPlus") {
	  QPalette p( QColor( 192, 192, 192 ) );
	  qApp->setPalette( p, TRUE );
	  qApp->setFont( appFont, TRUE );
	}

	bool isVisible = Toolbar_7->isVisible();
	delete Toolbar_7;
  Toolbar_7 = new QToolBar( QString(""), this, DockLeft );
  Toolbar_7->setLabel(tr("Keyword toolbar"));
  fillKwdToolbar(Toolbar_7);
	if (isVisible)
		Toolbar_7->show();
	else
		Toolbar_7->hide();

	if (completelyCreated)
		repaint();
}

CLavaMainFrame::~CLavaMainFrame()
{
  //delete m_OutputBar;
}

bool CLavaMainFrame::OnCreate()
{
	m_childFrameHistory->m_menu = windowMenu;
  wxDocManager::GetDocumentManager()->m_fileHistory->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  setIcon(QPixmap((const char**) Lava));
  QSplitter* split = new QSplitter(m_CentralWidget);
  split->setOrientation(Qt::Vertical);
  CreateWorkspace(split);
  m_OutputBar = new COutputBar(split);
  LoadFileHistory();
  m_OutputBar->hide();
  Toolbar_3->setNewLine(true);
  Toolbar_5->hide();
  Toolbar_5->setNewLine(true);
  Toolbar_5->hide();
  Toolbar_6->hide();
  Toolbar_7->hide();
  QWhatsThis::whatsThisButton(Toolbar_1);
  OutputBarHidden = true;

	completelyCreated = true;
	
  return true;
}

void CLavaMainFrame::UpdateUI()
{
  OnUpdateBarhammer(showUtilWindowAction);
  viewTB1Action->setOn(Toolbar_1->isVisible());  
  viewTB2Action->setOn(Toolbar_2->isVisible());  
  viewTB3Action->setOn(Toolbar_3->isVisible());  
  viewTB4Action->setOn(Toolbar_4->isVisible());  
  viewTB5Action->setOn(Toolbar_5->isVisible());
  viewTB6Action->setOn(Toolbar_6->isVisible());
  viewTB7Action->setOn(Toolbar_7->isVisible());
  CLavaPEDoc* doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
  bool enable = (doc != 0);
  fileCloseAction->setEnabled(enable);
  fileSaveAction->setEnabled(enable);
  fileSaveAsAction->setEnabled(enable);
	genLinkedHtmlAction->setEnabled(enable);
	genSingleHtmlAction->setEnabled(enable);
  findByNameAction->setEnabled(enable);
  checkAction->setEnabled(enable); 
  checkAllAction->setEnabled(enable);
  if (doc) 
    doc->OnUpdateRunLava(runAction);
  else
    runAction->setEnabled(false);
}

void CLavaMainFrame::newKwdToolbutton(QToolBar *tb, QPushButton *&pb, char *text, char *slotParm)
{
  QFont f;

  pb = new QPushButton(text,tb);
  connect(pb,SIGNAL(clicked()),this,slotParm);
  f = pb->font();
  f.setBold(true);
  pb->setFont(f);
  pb->setFlat(true);
  pb->setAutoDefault(false);
  pb->setMinimumHeight(pb->fontInfo().pointSize()+6);
  pb->setMaximumWidth(pb->fontMetrics().width("el. in set")+6);
}

/*
bool CLavaMainFrame::event(QEvent *ev)
{
  bool isVisible;

	if (ev->type() == QEvent::ApplicationPaletteChange && completelyCreated && Toolbar_7) {
	  isVisible = Toolbar_7->isVisible();
	  QWidget::event(ev);
	  delete Toolbar_7;
    Toolbar_7 = new QToolBar( QString(""), this, DockLeft );
    Toolbar_7->setLabel(tr("Keyword toolbar"));
    fillKwdToolbar(Toolbar_7);
	  if (isVisible)
		  Toolbar_7->show();
	  else
		  Toolbar_7->hide();
	  return true;
	}
	else {
		wxMainFrame::event(ev);
		return true;
	}
}
*/

void CLavaMainFrame::fillKwdToolbar(QToolBar *tb)
{
  if (LBaseData->m_style != "CDE") {
	  QColorGroup cgDis(tb->palette().disabled());
	  cgDis.setColor(QColorGroup::ButtonText,darkGray);
	  QColorGroup cgAct(tb->palette().active());
	  cgAct.setColor(QColorGroup::ButtonText,blue);
	  QPalette pal(cgAct,cgDis,tb->palette().inactive());
		tb->setPalette(pal);
	}

  newKwdToolbutton(tb,LBaseData->declareButton,"&declare",SLOT(declare()));
  newKwdToolbutton(tb,LBaseData->existsButton,"&exists",SLOT(exists()));
  newKwdToolbutton(tb,LBaseData->foreachButton,"&foreach",SLOT(foreach()));
  newKwdToolbutton(tb,LBaseData->selectButton,"se&lect",SLOT(select()));
  newKwdToolbutton(tb,LBaseData->elInSetButton,"el. in set",SLOT(elInSet()));

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->ifButton,"&if",SLOT(ifStm()));
  newKwdToolbutton(tb,LBaseData->ifxButton,"if-e&xpr",SLOT(ifExpr()));
  newKwdToolbutton(tb,LBaseData->switchButton,"s&witch",SLOT(switchStm()));
  newKwdToolbutton(tb,LBaseData->typeSwitchButton,"&type",SLOT(typeStm()));
  newKwdToolbutton(tb,LBaseData->andButton,"and / ;",SLOT(and_stm()));
  newKwdToolbutton(tb,LBaseData->orButton,"&or",SLOT(or_stm()));
  newKwdToolbutton(tb,LBaseData->xorButton,"xor",SLOT(xor_stm()));
  newKwdToolbutton(tb,LBaseData->notButton,"not",SLOT(not_stm()));
  newKwdToolbutton(tb,LBaseData->assertButton,"&assert",SLOT(assert_stm()));
  newKwdToolbutton(tb,LBaseData->tryButton,"try",SLOT(try_stm()));
  newKwdToolbutton(tb,LBaseData->succeedButton,"succeed",SLOT(succeed()));
  newKwdToolbutton(tb,LBaseData->failButton,"fail",SLOT(fail()));
  newKwdToolbutton(tb,LBaseData->runButton,"&run",SLOT(call()));;

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->setButton,"&set",SLOT(set()));
  newKwdToolbutton(tb,LBaseData->newButton,"&new",SLOT(newExpr()));
  newKwdToolbutton(tb,LBaseData->cloneButton,"clone",SLOT(clone()));
  newKwdToolbutton(tb,LBaseData->copyButton,"&copy",SLOT(copy()));
  newKwdToolbutton(tb,LBaseData->attachButton,"attach",SLOT(attach()));
  newKwdToolbutton(tb,LBaseData->qryItfButton,"qry itf",SLOT(qryItf()));
  newKwdToolbutton(tb,LBaseData->scaleButton,"scale",SLOT(scale()));
  newKwdToolbutton(tb,LBaseData->itemButton,"item",SLOT(item()));
  newKwdToolbutton(tb,LBaseData->callbackButton,"callback",SLOT(callback()));
}

/////////////////////////////////////////////////////////////////////////////
// handlers:

void CLavaMainFrame::fileNew()
{
  ((CLavaPEApp*)wxTheApp)->OnFileNew();
}

void CLavaMainFrame::newCompo()
{
  ((CLavaPEApp*)wxTheApp)->OnNewComponent();
}

void CLavaMainFrame::fileOpen()
{
  ((CLavaPEApp*)wxTheApp)->OnFileOpen();
}

void CLavaMainFrame::importComponent()
{
  ((CLavaPEApp*)wxTheApp)->OnImport();
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
  ((CLavaPEApp*)wxTheApp)->OnSaveAll();
}

void CLavaMainFrame::fileClose()
{
  wxDocManager::GetDocumentManager()->OnFileClose();
}

void CLavaMainFrame::editUndo()
{
  ((CLavaPEApp*)wxTheApp)->OnEditUndo();
}

void CLavaMainFrame::editRedo()
{
  ((CLavaPEApp*)wxTheApp)->OnEditRedo();
}

void CLavaMainFrame::returnToRef()
{
  ((CLavaPEApp*)wxTheApp)->OnPopcontext();
}

void CLavaMainFrame::findByName()
{
  ((CLavaPEApp*)wxTheApp)->OnFindByName();
}

void CLavaMainFrame::run()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnRunLava();
}

void CLavaMainFrame::check()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnCheck();
}

void CLavaMainFrame::checkAll()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnTotalCheck();
}

void CLavaMainFrame::helpContents()
{
	((CLavaPEApp*)wxTheApp)->HtmlHelp();
}

void CLavaMainFrame::editingLavaProgs()
{
	((CLavaPEApp*)wxTheApp)->EditingLavaProgs();
}

void CLavaMainFrame::learningLava()
{
	((CLavaPEApp*)wxTheApp)->LearningLava();
}

void CLavaMainFrame::helpAbout()
{
  ((CLavaPEApp*)wxTheApp)->OnAppAbout();
}

void CLavaMainFrame::filePrint(){}

void CLavaMainFrame::setTreeFont()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseTreeFont();
}

void CLavaMainFrame::setFormTextFont()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseFormFont(0);
}

void CLavaMainFrame::setFormLabelFont()
{
  LBaseData->useLabelFont = true;
  ((CLavaPEApp*)wxTheApp)->OnChooseFormFont(1);
}

/*
void CLavaMainFrame::setFormButtonFont()
{
  LBaseData->useButtonFont = true;
  ((CLavaPEApp*)wxTheApp)->OnChooseFormFont(2);
}
*/

void CLavaMainFrame::setExecFont()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseExecFont();
}

void CLavaMainFrame::setGlobalFont()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseGlobalFont();
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

void CLavaMainFrame::editFind()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFindreferences();
}

void CLavaMainFrame::expandSubtree()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnExpandAll();
}

void CLavaMainFrame::collapseSubtree()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCollapsAll();
}

void CLavaMainFrame::showOpts()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowOptionals();
}

void CLavaMainFrame::showAllOpts()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowAllEmptyOpt();
}

void CLavaMainFrame::hideEmptyOpts()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnRemoveAllEmptyOpt();
}

void CLavaMainFrame::nextComment()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNextComment();
}

void CLavaMainFrame::prevComment()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPrevComment();
}

void CLavaMainFrame::nextError()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNextError();
}

void CLavaMainFrame::prevError()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPrevError();
}
void CLavaMainFrame::newInclude()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewInclude();
}

void CLavaMainFrame::newPackage()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewNamespace();
}

void CLavaMainFrame::newInitiator()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewinitiator();
}

void CLavaMainFrame::new_Interface()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewclassintf();
}

void CLavaMainFrame::newImpl()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewclassimpl();
}

void CLavaMainFrame::newCOspec()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCSpec();
}

void CLavaMainFrame::newCOimpl()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnComponent();
}

void CLavaMainFrame::newSet()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewset();
}

void CLavaMainFrame::newEnum()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewenum();
}

void CLavaMainFrame::newVT()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewVirtualType();
}

void CLavaMainFrame::newFunc()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewfunction();
}

void CLavaMainFrame::newMemVar()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewmember();
}

void CLavaMainFrame::newEnumItem()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewEnumItem();
}

void CLavaMainFrame::editSelItem()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditSel();
}

void CLavaMainFrame::editComment()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnComment();
}

void CLavaMainFrame::gotoDec()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGotodef();
}

void CLavaMainFrame::gotoImpl()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGotoImpl();
}

void CLavaMainFrame::showOverridables()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowOverridables();
}

void CLavaMainFrame::openFormView()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowformview();
}

void CLavaMainFrame::insertText()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLitStr();
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


void CLavaMainFrame::deleteOp()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDelete();
}

void CLavaMainFrame::override()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOverride();
}

void CLavaMainFrame::genLinkedHtml()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    ((CLavaPEView*)((CPEBaseDoc*)view->GetDocument())->MainView)->OnGenHtml();
}

void CLavaMainFrame::genSingleHtml()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    ((CLavaPEView*)((CPEBaseDoc*)view->GetDocument())->MainView)->OnGenHtmlS();
}


//////////////////////////////////////////////////////////
// Exec handlers (tool buttons):

void CLavaMainFrame::toggleCategory(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleCategory();
}

void CLavaMainFrame::optLocalVar(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOptLocalVar();
}

void CLavaMainFrame::toggleSubstType(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleSubstitutable();
}

void CLavaMainFrame::insertBefore(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsertBefore();
}

void CLavaMainFrame::insert(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsert();
}

void CLavaMainFrame::toggleArrows(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleArrows();
}

void CLavaMainFrame::toggleInputArrows(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInputArrow();
}

void CLavaMainFrame::conflictingAssig(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnConflict();
}

void CLavaMainFrame::newLine(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewLine();
}

void CLavaMainFrame::commentOut(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIgnore();
}

void CLavaMainFrame::toggleComments(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowComments();
}

void CLavaMainFrame::lessThan(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLt();
}

void CLavaMainFrame::lessEqua(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLe();
}

void CLavaMainFrame::equal(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEq();
}

void CLavaMainFrame::notEqual(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNe();
}

void CLavaMainFrame::greaterEqual(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGe();
}

void CLavaMainFrame::greaterThan(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGt();
}

void CLavaMainFrame::functionCall(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFunctionCall();
}

void CLavaMainFrame::staticCall(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnStaticCall();
}

void CLavaMainFrame::trueHandler(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTrue();
}

void CLavaMainFrame::falseHandler(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFalse();
}

void CLavaMainFrame::evaluate(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEvaluate();
}

void CLavaMainFrame::null(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNull();
}

void CLavaMainFrame::interval(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInterval();
}

void CLavaMainFrame::toggleSign(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPlusMinus();
}

void CLavaMainFrame::addition(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPlus();
}

void CLavaMainFrame::multiplication(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnMult();
}

void CLavaMainFrame::divide(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDivide();
}

void CLavaMainFrame::modulus(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnModulus();
}

void CLavaMainFrame::leftShift(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLshift();
}

void CLavaMainFrame::rightShift(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnRshift();
}

void CLavaMainFrame::bitAnd(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitAnd();
}

void CLavaMainFrame::bitOr(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitOr();
}

void CLavaMainFrame::bitXor(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitXor();
}

void CLavaMainFrame::invert(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInvert();
}

void CLavaMainFrame::handle(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnHandle();
}

void CLavaMainFrame::ord(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOrd();
}

// Exec handlers (keyword push buttons):

void CLavaMainFrame::declare(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDeclare();
}

void CLavaMainFrame::exists(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnExists();
}

void CLavaMainFrame::foreach(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnForeach();
}

void CLavaMainFrame::select(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnSelect();
}

void CLavaMainFrame::elInSet(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIn();
}

void CLavaMainFrame::ifStm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIf();
}

void CLavaMainFrame::ifExpr(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIfExpr();
}

void CLavaMainFrame::switchStm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnSwitch();
}

void CLavaMainFrame::typeStm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTypeSwitch();
}

void CLavaMainFrame::and_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnAnd();
}

void CLavaMainFrame::or_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOr();
}

void CLavaMainFrame::xor_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnXor();
}

void CLavaMainFrame::not_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNot();
}

void CLavaMainFrame::assert_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnAssert();
}

void CLavaMainFrame::try_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTryStatement();
}

void CLavaMainFrame::succeed(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnSucceed();
}

void CLavaMainFrame::fail(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFail();
}

void CLavaMainFrame::call(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCall();
}

void CLavaMainFrame::set(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnAssign();
}

void CLavaMainFrame::newExpr(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCreateObject();
}

void CLavaMainFrame::clone(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnClone();
}

void CLavaMainFrame::copy(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCopy();
}

void CLavaMainFrame::attach(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnAttach();
}

void CLavaMainFrame::qryItf(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnQueryItf();
}

void CLavaMainFrame::scale(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnQua();
}

void CLavaMainFrame::item(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnItem();
}

void CLavaMainFrame::callback(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCallback();
}

// end of exec handlers
/////////////////////////////////////////////
void CLavaMainFrame::adjustToolbar_7 () {
  bool isVisible;

	isVisible = Toolbar_7->isVisible();
	delete Toolbar_7;
  Toolbar_7 = 0;
  Toolbar_7 = new QToolBar( QString(""), this, DockLeft );
	if (isVisible)
	  Toolbar_7->show();
	else
	  Toolbar_7->hide();
  Toolbar_7->setLabel(tr("Keyword toolbar"));
  fillKwdToolbar(Toolbar_7);
}


void CLavaMainFrame::cascade()
{
  Cascade();
  /*
  int ii;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();

  wxTheApp->isChMaximized = false;
  m_workspace->cascade();
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) 
      if (window->inherits("wxMDIChildFrame"))
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
  }
  */
}

void CLavaMainFrame::tileVertic()
{
  TileVertic(menubar, lastTile);
  /*
  int ii, cc = 0, x = 0, widthForEach, preferredWidth, actWidth, minHeight=0, allHeight;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();

	wxTheApp->isChMaximized = false;
	lastTile = 1;

  if (!windows.count() )
      return;
  cc = (int)windows.count();
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (((QMainWindow*)window)->isMinimized()) {
      cc--;
      minHeight = menubar->height();
    }
    else
      if (window->inherits("wxMDIChildFrame"))
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
  }
  if (!cc)
    return;
  if (cc > 3) {
    m_workspace->tile();
    return;
  }
  allHeight = m_workspace->height() - minHeight;
  widthForEach = m_workspace->width() / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredWidth = window->minimumWidth()+window->parentWidget()->baseSize().width();
      actWidth = QMAX(widthForEach, preferredWidth);
      window->parentWidget()->setGeometry( x, 0, actWidth, allHeight );
      x += actWidth;
      if (!((QMainWindow*)window)->isMaximized()) 
        window->showNormal();
    }
  }
  */
}
    
void CLavaMainFrame::tileHoriz()
{
  TileHoriz(menubar, lastTile);
  /*
  int ii, cc = 0, y = 0, heightForEach, preferredHeight, actHeight, minHeight=0;
  QWidget *window;
  QWidgetList windows = m_workspace->windowList();

	lastTile = 2;

  if (!windows.count() )
      return;
  cc = (int)windows.count();
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (((QMainWindow*)window)->isMinimized()) {
      cc--;
      minHeight = menubar->height();
    }
    else
      if (window->inherits("wxMDIChildFrame"))
        ((wxMDIChildFrame*)window)->oldWindowState = QEvent::ShowNormal;
  }
  if (!cc)
    return;
  if (cc > 3) {
    m_workspace->tile();
    return;
  }
  heightForEach = (m_workspace->height() - minHeight) / cc;
  for (ii = 0; ii < int(windows.count()); ++ii ) {
    window = windows.at(ii);
    if (!((QMainWindow*)window)->isMinimized()) {
      preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
      actHeight = QMAX(heightForEach, preferredHeight);
      window->parentWidget()->setGeometry( 0, y, m_workspace->width(), actHeight );
      y += actHeight;
      if (!((QMainWindow*)window)->isMaximized()) 
        window->showNormal();
    }
  }
  */
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

void CLavaMainFrame::showUtilWindow() 
{
  if (OutputBarHidden) {
    m_OutputBar->show();
    LastBarState = -1;
  }
  else {
    m_OutputBar->hide();
    LastBarState = (int)m_OutputBar->ActTab;
  }
//  RecalcLayout();
  OutputBarHidden = !OutputBarHidden;
}

void CLavaMainFrame::OnUpdateBarhammer(wxAction* action) 
{
  action->setOn(!OutputBarHidden);
}

void CLavaMainFrame::ShowBarTab(BarTabs tab)
{
  if (OutputBarHidden || (((COutputBar*)m_OutputBar)->ActTab != tab)) 
    ;//m_OutputBar->SetTab(tab);
  else {
    if ((LastBarState < 0) || (((COutputBar*)m_OutputBar)->ActTab == tab)) {
      m_OutputBar->hide();
      OutputBarHidden = TRUE;
    }
    else 
     ;// m_OutputBar->SetTab((BarTabs)LastBarState);
    LastBarState = (int)tab;
  }
//  RecalcLayout();
}

void CLavaMainFrame::viewTB1() 
{
  if (Toolbar_1->isVisible())
    Toolbar_1->hide();
  else
    Toolbar_1->show();
}

void CLavaMainFrame::viewTB2() 
{
  if (Toolbar_2->isVisible())
    Toolbar_2->hide();
  else
    Toolbar_2->show();
}


void CLavaMainFrame::viewTB3() 
{
  if (Toolbar_3->isVisible())
    Toolbar_3->hide();
  else
    Toolbar_3->show();
}


void CLavaMainFrame::viewTB4() 
{
  if (Toolbar_4->isVisible())
    Toolbar_4->hide();
  else
    Toolbar_4->show();
}


void CLavaMainFrame::viewTB5() 
{
  if (Toolbar_5->isVisible())
    Toolbar_5->hide();
  else
    Toolbar_5->show();
}



void CLavaMainFrame::viewTB6() 
{
  if (Toolbar_6->isVisible())
    Toolbar_6->hide();
  else
    Toolbar_6->show();
}


void CLavaMainFrame::viewTB7() 
{
  if (Toolbar_7->isVisible())
    Toolbar_7->hide();
  else
    Toolbar_7->show();
}


/*
void CLavaMainFrame::OnEditHints() 
{
  DString CHMname;
  CWnd* pWnd = GetTopLevelParent();
  
  CHMname = ExeDir + "\\LavaPE.chm>ContextWin";
  ::HtmlHelp(pWnd->m_hWnd, CHMname.c, HH_HELP_CONTEXT, ID_EDIT_HINTS);
}

void CLavaMainFrame::OnCheckRunHelp() 
{
  DString CHMname;
  CWnd* pWnd = GetTopLevelParent();
  
  CHMname = ExeDir + "\\LavaPE.chm>ContextWin";
  ::HtmlHelp(pWnd->m_hWnd, CHMname.c, HH_HELP_CONTEXT, ID_CHECK_RUN_HELP);
  
}

*/




CTreeFrame::CTreeFrame(QWidget* parent):wxMDIChildFrame(parent, "TreeFrame") 
{
  viewR = 0;
  showIt = !((CLavaPEApp*)wxTheApp)->inTotalCheck;
}


bool CTreeFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  setIcon(QPixmap((const char**) Lava));
  QSize sz;
  if (oldWindowState != QEvent::ShowMaximized) {
    sz = parentWidget()->size();
    resize(sz.width()*7/10, sz.height()*7/10);
  }
  splitter = new QSplitter(this);//vb);
  setCentralWidget(splitter);
  splitter->setOrientation(Qt::Horizontal);
  m_clientWindow = splitter;
  viewL = new CInclView(splitter, doc);
  viewM = new CLavaPEView(splitter, doc);
  viewR = new CVTView(splitter, doc);
  if (viewM->OnCreate() && viewL->OnCreate() && viewR->OnCreate()) {
    ((CInclView*)viewL)->myTree = (CTreeView*)viewM;
    ((CVTView*)viewR)->myMainView = (CLavaPEView*)viewM;
    ((CLavaPEView*)viewM)->myInclView = (CTreeView*)viewL;
    ((CLavaPEView*)viewM)->myVTView = (CTreeView*)viewR;
    doc->AddView(viewR);
    doc->AddView(viewL);
    doc->AddView(viewM);
    QValueList<int> list = splitter->sizes();
    QValueList<int>::Iterator it = list.begin();
    int totalW = *it;
    ++it;
    totalW += *it;
    ++it;
    totalW += *it;
    it = list.begin();
    *it = 10;
    ++it;
    *it = totalW-20;
    ++it;
    *it = 10;
    splitter->setSizes(list);
    lastActive = viewM;
		return true;
  }
  else {
    delete viewL;
    delete viewR;
    delete viewM;
    return false;
  }
}


void CTreeFrame::InitialUpdate()

{
  wxMDIChildFrame::InitialUpdate();
  wxDocManager::GetDocumentManager()->SetActiveView(viewM, true);
}

CTreeFrame::~CTreeFrame()
{
}

void CTreeFrame::closeEvent(QCloseEvent *e)
{
	if (viewM->GetDocument()->Close())
		QWidget::closeEvent(e);
}

void CTreeFrame::Activate(bool activate, bool windowMenuAction) 
{
  showIt = activate && (showIt || !((CLavaPEApp*)wxTheApp)->inTotalCheck);
  if (showIt)
    wxMDIChildFrame::Activate(showIt,windowMenuAction);
  else 
    hide(); 
}

void CTreeFrame::CalcSplitters(bool showVT, bool showINCL)
{
  QValueList<int> list = splitter->sizes();
  QValueList<int>::Iterator it = list.begin();
  int wl, wr, totalW = *it;
  ++it;
  totalW += *it;
  ++it;
  totalW += *it;
  it = list.begin();
  wl = *it;
  if (showINCL && (wl < 20))
    wl = totalW/3;
  it = list.begin();;
  *it = wl;
  ++it;
  ++it;
  wr = *it;
  if (showVT && (wr < 20)) 
    wr = totalW/3;
  *it = wr;
  it--;
  *it = totalW-wl-wr;
  it--;
  splitter->setSizes(list);
}

void CTreeFrame::SetModified(bool changed)
{
  if (deleting)
    return;
  CLavaPEDoc* doc = (CLavaPEDoc*)((CLavaPEView*)viewM)->GetDocument();
  QString title = doc->GetTitle();
  if (doc->isReadOnly)
    title += " (read only)";
  if (title.length() && (title.at(title.length()-1) == '*')) {
    if (!changed)
      title.remove(title.length()-1, 1);
  }
  else
    if (changed)
      title += "*";
  SetTitle(title);
} 


CFormFrame::CFormFrame(QWidget* parent):CLavaGUIFrame(parent) 
{
  viewL = 0;
  viewR = 0;
  widthPrev = 0;
}

CFormFrame::~CFormFrame()
{
  deleting = true;
}


bool CFormFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  QSize sz;
  DString title = CalcTitle( (LavaDECL*)((CLavaPEApp*)wxTheApp)->LBaseData.actHint->CommandData1, ((CLavaBaseDoc*)LBaseData->actHint->fromDoc)->IDTable.DocName);
  setCaption(title.c);
  setIcon(QPixmap((const char**) Lava));
  if (oldWindowState != QEvent::ShowMaximized) {
    sz = parentWidget()->size();
    resize(sz.width()*7/10, sz.height()*7/10); 
  }
  splitter = new QSplitter(this);
  setCentralWidget(splitter);
  splitter->setOrientation(Qt::Horizontal);
  m_clientWindow = splitter;
  viewL = new CLavaGUIView(splitter, doc);
  QSplitter* split = new QSplitter(splitter);
  split->setOrientation(Qt::Vertical);
  viewR = new CLavaPEView(split, doc);
  wizardView = new CWizardView(split, doc);
  ((CLavaGUIView*)viewL)->myTree = (CTreeView*)viewR;
  ((CLavaPEView*)viewR)->myFormView = (CLavaGUIView*)viewL;
  if (viewL->OnCreate() && viewR->OnCreate()) {
    doc->AddView(viewR);
    doc->AddView(viewL);
    doc->AddView(wizardView);
    QValueList<int> list = splitter->sizes();
    QValueList<int>::Iterator it = list.begin();
    int totalW = *it;
    ++it;
    totalW += *it;
    it = list.begin();
    *it = totalW/4 * 3;
    ++it;
    *it = totalW/4;
    splitter->setSizes(list);
    lastActive = viewR;
    return true;
  }
  else {
    delete viewL;
    delete viewR;
    delete wizardView;
    return false;
  }
}


void CFormFrame::InitialUpdate()
{
  wxMDIChildFrame::InitialUpdate();
  wxDocManager::GetDocumentManager()->SetActiveView(viewR, true);
}

void CFormFrame::CalcSplitters()
{
}

/*

/////////////////////////////////////////////////////////////////////////////
// CFormFrame message handlers

bool CFormFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  if (!m_Splitter.CreateStatic(this, 1, 2)) {
    return FALSE;
  }
  CRect rect;
  GetClientRect(&rect);
  widthPrev = rect.right;
  // add the first splitter pane - the default view in column 0
  if (!m_Splitter.CreateView(0, 0,  pContext->m_pNewViewClass, CSize(3*rect.right/4, 50), pContext))
    return FALSE;
  // add the second splitter pane - an input view in column 1
  if (!m_Splitter.CreateView(0, 1, RUNTIME_CLASS(CLavaPEView), CSize(rect.right/4, 50), pContext))
    return FALSE;
  // activate the input view
  viewL = (QScrollView*)m_Splitter.GetPane(0,0);
  viewR = (CLavaBaseView*)m_Splitter.GetPane(0,1);
  ((CLavaGUIView*)viewL)->myTree = (CTreeView*)viewR;
  ((CLavaPEView*)viewR)->myFormView = (QScrollView*)viewL;
  SetActiveView(viewR);
  return TRUE;
}

bool CFormFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
  dwStyle = dwStyle & (~FWS_ADDTOTITLE);
  DString title = CalcTitle( (LavaDECL*)((CLavaPEApp*)wxTheApp)->LBaseData.actHint->CommandData1, ((CLavaBaseDoc*)LBaseData->actHint->fromDoc)->IDTable.DocName);
  return CMDIChildWnd::Create(lpszClassName, title.c, dwStyle, rect, pParentWnd, pContext);
}


void CFormFrame::RecalcLayout(bool bNotify) 
{ 
  CMDIChildWnd::RecalcLayout(bNotify);
  CRect rect;
  int w0, w1, w0m, w1m;
  GetClientRect(&rect);
  //remeber: rect.right = 0 if frame is minimized
  if (viewL && viewR && rect.right && widthPrev && (rect.right != widthPrev)) {
    m_Splitter.GetColumnInfo(0, w0, w0m);
    m_Splitter.GetColumnInfo(1, w1, w1m);
    m_Splitter.SetColumnInfo(0, rect.right*w0/widthPrev, 20);
    m_Splitter.SetColumnInfo(1, rect.right*w1/widthPrev, 20);
    m_Splitter.RecalcLayout();
    widthPrev = rect.right;
  }
}

*/
