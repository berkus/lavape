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


#include "LavaPE.h"
#include "LavaAppBase.h"
#include "LavaPEView.h"
#include "LavaGUIView.h"
#include "ExecView.h"
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
#include "qtooltip.h"
#include "qassistantclient.h"

typedef QMap<QString,QString> HelpTextMap;


CLavaMainFrame::CLavaMainFrame(QWidget* parent, const char* name, WFlags fl)
:CMainFrame(parent, name, fl)
{
  theActiveFrame = 0;

	makeStyle(LBaseData->m_style);

  m_UtilityView = 0;
	lastTile = 0;
  QPopupMenu *style = new QPopupMenu(this);
  style->setCheckable( TRUE );
  optionMenu->insertItem( "Set st&yle" , style );
  QActionGroup *ag = new QActionGroup( this, 0 );
  ag->setExclusive( TRUE );
  QSignalMapper *styleMapper = new QSignalMapper( this );
  connect( styleMapper, SIGNAL( mapped( const QString& ) ), this, SLOT( makeStyle( const QString& ) ) );
  QStringList list = QStyleFactory::keys();
  list.sort();
  QDict<int> stylesDict( 17, FALSE );
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString styleStr = *it;
//    qDebug("style=%s",styleStr.ascii());
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
//  LBaseData->okActionPtr = 0;
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

  LBaseData->DbgActionPtr = DbgAction;
  LBaseData->DbgStepNextActPtr = DbgStepNextAct;
  LBaseData->DbgStepNextFunctionActPtr = DbgStepNextFunctionAct;
  LBaseData->DbgStepintoActPtr = DbgStepintoAct;
  LBaseData->DbgStepoutActPtr = DbgStepoutAct;
  LBaseData->DbgRunToSelActPtr = DbgRunToSelAct;
  LBaseData->DbgBreakpointActPtr = DbgBreakpointAct;
  LBaseData->DbgClearBreakpointsActPtr = DbgClearBreakpointsAct;
  LBaseData->DbgStopActionPtr = DbgStopAction;
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  bool isVisible, firstTime=false;
  if (!style.isEmpty()) {
    LBaseData->m_style = style;
    wxTheApp->saveSettings();
	  qApp->setStyle(style);
	  if(style == "Motif" || style == "MotifPlus") {
	    QPalette p( QColor( 192, 192, 192 ) );
	    qApp->setPalette( p, TRUE );
	    qApp->setFont( LBaseData->m_GlobalFont, TRUE );
	  }
  }

  isVisible = Toolbar_7->isVisible();
  if (LBaseData->declareButton) {
    delete Toolbar_7;
    Toolbar_7 = new QToolBar( QString(""), this, DockLeft );
    Toolbar_7->setLabel(tr("Keyword toolbar"));
  }
  else
    firstTime = true;
  fillKwdToolbar(Toolbar_7);
	if (isVisible)
		Toolbar_7->show();
	else
		Toolbar_7->hide();

  if (!firstTime) {
    delete LBaseData->whatNextButton;
    delete LBaseData->myWhatsThisButton;
  }
  fillHelpToolbar(HelpToolbar);

  if (theActiveFrame && QString(theActiveFrame->name()) != "ExecFrame")
    CExecView::DisableKwdButtons();

	if (completelyCreated)
		repaint();
}

CLavaMainFrame::~CLavaMainFrame()
{
  //delete m_UtilityView;
}

bool CLavaMainFrame::OnCreate()
{
  ToolbarWhatsThis *tbw1, *tbw2, *tbw3, *tbw4, *tbw5, *tbw6;

  tbw1 = new ToolbarWhatsThis(Toolbar_1);
  fillHelpMap1(tbw1);
  tbw2 = new ToolbarWhatsThis(Toolbar_2);
  fillHelpMap2(tbw2);
  tbw3 = new ToolbarWhatsThis(Toolbar_3);
  fillHelpMap3(tbw3);
  tbw4 = new ToolbarWhatsThis(Toolbar_4);
  fillHelpMap4(tbw4);
  tbw5 = new ToolbarWhatsThis(Toolbar_5);
  fillHelpMap5(tbw5);
  tbw6 = new ToolbarWhatsThis(Toolbar_6);
  fillHelpMap6(tbw6);
	m_childFrameHistory->m_menu = windowMenu;
  wxDocManager::GetDocumentManager()->m_fileHistory->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  setIcon(QPixmap((const char**) Lava));
  QSplitter* split = new QSplitter(m_CentralWidget);
  split->setOrientation(Qt::Vertical);
  CreateWorkspace(split);
  m_UtilityView = new CUtilityView(split);
  LoadFileHistory();
  m_UtilityView->hide();
  HelpToolbar->setOffset(100000);
  Toolbar_3->setNewLine(true);
  Toolbar_5->hide();
  Toolbar_5->setNewLine(true);
  Toolbar_5->hide();
  Toolbar_6->hide();
  Toolbar_7->hide();
  UtilitiesHidden = true;

	completelyCreated = true;
	
  return true;
}

void CLavaMainFrame::UpdateUI()
{
  OnUpdateshowUtil(showUtilWindowAction);
  saveEveryChangeAction->setOn(LBaseData->m_saveEveryChange);  
  viewTB1Action->setOn(Toolbar_1->isVisible());  
  viewTB2Action->setOn(Toolbar_2->isVisible());
  viewWhatNextTBAction->setOn(HelpToolbar->isVisible());
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

void CLavaMainFrame::newKwdToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,QString tooltip,QString whatsThis)
{
  QFont f;

  pb = new QPushButton(QString(text),tb);
  connect(pb,SIGNAL(clicked()),this,slotParm);
  f = pb->font();
  f.setBold(true);
  pb->setFont(f);
  pb->setFlat(true);
  pb->setAutoDefault(false);
  pb->setMinimumHeight(pb->fontInfo().pointSize()+4);
  pb->setMaximumWidth(pb->fontMetrics().width("el. in set")+6);
  if (tooltip)
    QToolTip::add(pb,tooltip);
  if (whatsThis)
    new WhatsThis(whatsThis,pb);
  pb->show();
}

void CLavaMainFrame::newHelpToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,char *tooltip,char *whatsThis)
{
  QFont f;

  pb = new QPushButton(QString(text),tb);
  connect(pb,SIGNAL(clicked()),this,slotParm);
  f = pb->font();
  f.setBold(true);
  pb->setFont(f);
  pb->setPaletteForegroundColor(QColor(blue));
//  pb->setFlat(true);
  pb->setAutoDefault(false);
//  pb->setMinimumHeight(pb->fontInfo().pointSize()+6);
//  pb->setMaximumWidth(pb->fontMetrics().width("What's this?")+6);
  if (tooltip)
    QToolTip::add(pb,tooltip);
  if (whatsThis)
    new WhatsThis(whatsThis,pb);
  pb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  pb->show();
}

void CLavaMainFrame::fillHelpToolbar(QToolBar *tb)
{
  LBaseData->myWhatsThisButton = QWhatsThis::whatsThisButton(HelpToolbar);
  QWhatsThis::add(LBaseData->myWhatsThisButton,"<p>Drag the \"What's this?\" cursor to any user interface object"
    " and drop it there to see a <b>little popup info (but usually more than a tooltip)</b> on that object.</p>");
  newHelpToolbutton(tb,LBaseData->whatNextButton,"What next?",SLOT(whatNext_clicked()),
    "What can I do next at the current selection?",
    "<p>Provides online help which lists the most important operations "
    "that you can perform <b>at the current selection</b></p>");
}

void CLavaMainFrame::fillKwdToolbar(QToolBar *tb)
{
	QColorGroup cgDis(tb->palette().disabled());
	cgDis.setColor(QColorGroup::ButtonText,darkGray);
	QColorGroup cgAct(tb->palette().active());
	cgAct.setColor(QColorGroup::ButtonText,blue);
	QPalette pal(cgAct,cgDis,tb->palette().inactive());
	tb->setPalette(pal);

  newKwdToolbutton(tb,LBaseData->declareButton,"&declare",SLOT(declare()),
    QObject::tr("Declare local variables: \"d\""),
    QObject::tr("<p><a href=\"Declare.htm\">declare</a> local variables</p>"));
  newKwdToolbutton(tb,LBaseData->existsButton,"&exists",SLOT(exists()),
    QObject::tr("Existential quantifier: \"e\""),
    QObject::tr("<p><a href=\"Exists.htm\">Existential quantifier</a> ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->foreachButton,"&foreach",SLOT(foreach()),
    QObject::tr("Universal quantifier: \"f\""),
    QObject::tr("<p><a href=\"Foreach.htm\">Universal quantifier</a> ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->selectButton,"se&lect",SLOT(select()),
    QObject::tr("Select elements from set(s) and add derived new elements to a given set: \"l\""),
    QObject::tr("<p><a href=\"Select.htm\">Select</a> quantifier ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->elInSetButton,"el. in set",SLOT(elInSet()),
    QObject::tr("Test if element is contained in set"),
    QObject::tr("<p><a href=\"InSet.htm\">Element in set</a> test</p>"));

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->ifButton,"&if",SLOT(ifStm()),
    QObject::tr("If-then-else statement: \"i\""),
    QObject::tr("<p><a href=\"IfStm.htm\">if-then-elsif-else</a> statement with optional\nelsif and else branches</p>"));
  newKwdToolbutton(tb,LBaseData->ifxButton,"if-expr",SLOT(ifExpr()),
    QObject::tr("If-then-else expression: \"x\""),
    QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelsif and else branches</p>"));
  newKwdToolbutton(tb,LBaseData->switchButton,"s&witch",SLOT(switchStm()),
    QObject::tr("Switch statement: \"w\""),
    QObject::tr("<p><a href=\"Switch.htm\">switch</a> statement with optional else branch</p>"));
  newKwdToolbutton(tb,LBaseData->typeSwitchButton,"&type",SLOT(typeStm()),
    QObject::tr("Type switch statement: \"t\""),
    QObject::tr("<p><a href=\"TypeSwitch.htm\">type switch</a> statement with optional else branch</p>"));
  newKwdToolbutton(tb,LBaseData->andButton,"&and / ;",SLOT(and_stm()),
    QObject::tr("AND conjunction of statements: \"a\""),
    QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>"));
  newKwdToolbutton(tb,LBaseData->orButton,"&or",SLOT(or_stm()),
    QObject::tr("OR conjunction of statements: \"o\""),
    QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>"));
  newKwdToolbutton(tb,LBaseData->xorButton,"xor",SLOT(xor_stm()),
    QObject::tr("Exclusive OR of statements"),
    QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>"));
  newKwdToolbutton(tb,LBaseData->notButton,"not",SLOT(not_stm()),
    QObject::tr("Negation of a statement"),
    QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>"));
  newKwdToolbutton(tb,LBaseData->tryButton,"tr&y",SLOT(try_stm()),
    QObject::tr("Try a statement, catch exceptions: \"y\""),
    QObject::tr("<p><a href=\"Try.htm\">Try</a> a statement, catch exceptions</p>"));
  newKwdToolbutton(tb,LBaseData->assertButton,"assert",SLOT(assert_stm()),
    QObject::tr("Assertion"),
    QObject::tr("<p>An <a href=\"Assert.htm\">embedded assertion</a> is embedded anywhwere in executable code"
    " (in contrast to <a href=\"../DBC.htm\">attached assertions</a>)"
    " and throws a specific exception in case of violation</p>"));
  newKwdToolbutton(tb,LBaseData->succeedButton,"succeed",SLOT(succeed()),
    QObject::tr("Immediate affirmative/successful return"),
    QObject::tr("<p><a href=\"FailSucceed.htm\">Succeed</a>: immediate affirmative/successful return from an <a href=\"../EditExec.htm#exec\">exec</a></p>"));
  newKwdToolbutton(tb,LBaseData->failButton,"fail",SLOT(fail()),
    QObject::tr("Immediate negative/unsuccessful return, optionally throw exception"),
    QObject::tr("<p><a href=\"FailSucceed.htm\">Fail</a>: immediate negative/unsuccessful return from an <a href=\"../EditExec.htm#exec\">exec</a>,"
    " optionally throw an <a href=\"../ExceptionSamples.htm\">exception</a></p>"));

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->setButton,"&set",SLOT(set()),
    QObject::tr("Assignment statement: \"s\""),
    QObject::tr("<p><a href=\"Assign.htm\">Assignment statement</a></p>"));
  newKwdToolbutton(tb,LBaseData->newButton,"&new",SLOT(newExpr()),
    QObject::tr("Create a new object: \"n\""),
    QObject::tr("<p>Create a <a href=\"New.htm\">new</a> object</p>"));
  newKwdToolbutton(tb,LBaseData->oldButton,"old",SLOT(old()),
    QObject::tr("Old value (on entry to function)"),
    QObject::tr("<p><a href=\"Old.htm\">Old</a> value of a variable or expression (on entry to function)</p>"));
  newKwdToolbutton(tb,LBaseData->cloneButton,"clone",SLOT(clone()),
    QObject::tr("Clone an object"),
    QObject::tr("<p><a href=\"Clone.htm\">Clone</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object</p>"));
  newKwdToolbutton(tb,LBaseData->copyButton,"&copy",SLOT(copy()),
    QObject::tr("Copy an object onto another object: \"c\""),
    QObject::tr("<p><a href=\"Copy.htm\">Copy</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object onto another object</p>"));
  newKwdToolbutton(tb,LBaseData->attachButton,"attach",SLOT(attach()),
    QObject::tr("Attach a component object through an interface"),
    QObject::tr("<p><a href=\"Attach.htm\">Attach</a> an existing <b><i><font color=\"red\">Lava</font></i></b> component object through one of its interfaces</p>"));
  newKwdToolbutton(tb,LBaseData->qryItfButton,"qry itf",SLOT(qryItf()),
    QObject::tr("Query interface: get another interface from an already known one"),
    QObject::tr("<p><a href=\"QryItf.htm\">Query interface:</a> Make another interface of a component object accessible, starting from a known interface</p>"));
  newKwdToolbutton(tb,LBaseData->scaleButton,"scale",SLOT(scale()),
    QObject::tr("Add a scale (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters"),
    QObject::tr("<p>Add a <a href=\"Scale.htm\">scale</a> (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters</p>"));
  newKwdToolbutton(tb,LBaseData->itemButton,"item",SLOT(item()),
    QObject::tr("Get an enumeration item from its index"),
    QObject::tr("<p>Get an enumeration <a href=\"EnumItem.htm\">item</a> from its index</p>"));
  newKwdToolbutton(tb,LBaseData->connectButton,"connect",SLOT(connectSignal()),
    QObject::tr("Connect a signal to a handler (\"callback\")"),
    QObject::tr("<p>Connect a software signal to a <a href=\"../Callbacks.htm\">handler (\"callback\")</a></p>"));
  newKwdToolbutton(tb,LBaseData->disconnectButton,"disconn",SLOT(disconnect()),
    QObject::tr("Disconnect a handler (\"callback\") from a signal"),
    QObject::tr("<p>Disconnect a signal from a <a href=\"../Callbacks.htm\">handler (\"callback\")</a></p>"));
  newKwdToolbutton(tb,LBaseData->emitButton,"signal",SLOT(emitSignal()),
    QObject::tr("Emit a signal"),
    QObject::tr("<p>Emit a <a href=\"../Callbacks.htm\">software signal</a></p>"));
  newKwdToolbutton(tb,LBaseData->runButton,"&run",SLOT(call()),
    QObject::tr("Run a nested initiator"),
    QObject::tr("<p><a href=\"Run.htm\">Run</a> a nested <a href=\"../Packages.htm#initiator\">main program</a></p>"));
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
    view->OnFindReferences();
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
    view->OnNewPackage();
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
    view->OnNewInterface();
}

void CLavaMainFrame::newImpl()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewImpl();
}

void CLavaMainFrame::newCOspec()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewCSpec();
}

void CLavaMainFrame::newCOimpl()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewComponent();
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
    view->OnGotoDecl();
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
    view->OnShowGUIview();
}

void CLavaMainFrame::makeGUI()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnMakeGUI();
}

void CLavaMainFrame::insertText()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewLitStr();
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


void CLavaMainFrame::DbgStart()
{
  if (((CLavaPEDebugThread*)LBaseData->debugThread)->running()) {
    DbgMessage* mess = new DbgMessage(Dbg_Continue);
    QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
  }
  else {
    CLavaPEDoc* doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
    doc->OnDebugLava();
  }
}


void CLavaMainFrame::DbgClearBreakpoints()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ClearBrkPnts = true;
  ((CLavaPEDebugThread*)LBaseData->debugThread)->clearBrkPnts();
}

void CLavaMainFrame::DbgBreakpoint()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->DbgBreakpoint();
}

void CLavaMainFrame::DbgStepNext()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_Step;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::DbgStepNextFunction()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepFunc;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::DbgStepinto()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepInto;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::DbgStepout()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepOut;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::DbgStop()
{
  if (((CLavaPEDebugThread*)LBaseData->debugThread)->interpreterWaits) {
    DbgMessage* mess = new DbgMessage(Dbg_Exit);
    QApplication::postEvent(wxTheApp,new QCustomEvent(IDU_LavaDebugRq,(void*)mess));
  }
  else 
    if  (((CLavaPEDebugThread*)LBaseData->debugThread)->startedFromLava) 
      close_socket(((CLavaPEDebugThread*)LBaseData->debugThread)->workSocket);
    else 
      ((CLavaPEApp*)qApp)->interpreter.kill();
}

void CLavaMainFrame::DbgRunToSel()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->DbgRunToSel();

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

void CLavaMainFrame::old(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOld();
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

void CLavaMainFrame::connectSignal(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnConnect();
}

void CLavaMainFrame::disconnect(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDisconnect();
}

void CLavaMainFrame::emitSignal(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEmitSignal();
}

// end of exec handlers
/////////////////////////////////////////////
void CLavaMainFrame::adjustToolbar_7 () {
  bool isVisible=Toolbar_7->isVisible();

	delete Toolbar_7;
  Toolbar_7 = 0;
  Toolbar_7 = new QToolBar( QString(""), this, DockLeft );
	if (isVisible)
	  Toolbar_7->show();
	else
	  Toolbar_7->hide();
  Toolbar_7->setLabel(tr("Keyword toolbar"));
  fillKwdToolbar(Toolbar_7);

  delete LBaseData->whatNextButton;
  delete LBaseData->myWhatsThisButton;
  fillHelpToolbar(HelpToolbar);
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
    if ( !((CLavaPEApp*)wxTheApp)->inTotalCheck) { //to prevent crash if mdi-frame already closed
		  title = DString(((QWidget*)ev->data())->caption());
		  if (title.l) {
			  if (title[title.l-1] == '*')
				  title = title.Substr(0,title.l-1);
			  hw = new HistWindow(title,(wxMDIChildFrame*)ev->data());
			  m_childFrameHistory->AddToHistory(hw,this);
		  }
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
  if (UtilitiesHidden) {
    if ((m_UtilityView->ActTab == tabError) && !m_UtilityView->CommentEmpty && m_UtilityView->ErrorEmpty )
      m_UtilityView->SetTab(tabComment);
    else if ((m_UtilityView->ActTab == tabComment) && m_UtilityView->CommentEmpty && !m_UtilityView->ErrorEmpty)
      m_UtilityView->SetTab(tabError);
    m_UtilityView->show();
    LastUtilitiesState = -1;
  }
  else {
    m_UtilityView->hide();
    LastUtilitiesState = (int)m_UtilityView->ActTab;
  }
  UtilitiesHidden = !UtilitiesHidden;
}

void CLavaMainFrame::OnUpdateshowUtil(wxAction* action) 
{
  action->setOn(!UtilitiesHidden);
}

/*
void CLavaMainFrame::ShowUtilitiesTab(UtilityTabs tab)
{
  if (UtilitiesHidden || (((CUtilityView*)m_UtilityView)->ActTab != tab)) 
    ;//m_UtilityView->SetTab(tab);
  else {
    if ((LastUtilitiesState < 0) || (((CUtilityView*)m_UtilityView)->ActTab == tab)) {
      m_UtilityView->hide();
      UtilitiesHidden = TRUE;
    }
    else 
     ;// m_UtilityView->SetTab((UtilityTabs)LastUtilitiesState);
    LastUtilitiesState = (int)tab;
  }
//  RecalcLayout();
}
*/

void CLavaMainFrame::saveEveryChange(bool on) 
{
  if (on) {
    LBaseData->m_saveEveryChange = true;
    LBaseData->m_strSaveEveryChange = "true";
  }
  else {
    LBaseData->m_saveEveryChange = false;
    LBaseData->m_strSaveEveryChange = "false";
  }
  ((CLavaPEApp*)wxTheApp)->saveSettings();
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

void CLavaMainFrame::viewHelpTB() 
{
  if (HelpToolbar->isVisible())
    HelpToolbar->hide();
  else
    HelpToolbar->show();
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

void CLavaMainFrame::whatNext_clicked() 
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  QString fileName=ExeDir+"/../doc/html/whatNext/GlobalWhatNext.htm";
	QString path("");
	QStringList args;

  if (view)
    view->whatNext();
  else {
	  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	  
	  if (!qacl) {
		  qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		  qacl->setArguments(args);
	  }

	  qacl->showPage(fileName);
  }
}

void CLavaMainFrame::howTo_clicked() 
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  QString fileName=ExeDir+"/../doc/html/howTo/HowToContents.htm";
	QString path("");
	QStringList args;

  if (view)
    view->howTo();
  else {
	  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";
	  
	  if (!qacl) {
		  qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		  qacl->setArguments(args);
	  }

	  qacl->showPage(fileName);
  }
}


CTreeFrame::CTreeFrame(QWidget* parent):wxMDIChildFrame(parent, "TreeFrame") 
{
  viewR = 0;
  showIt = !((CLavaPEApp*)wxTheApp)->inTotalCheck;
}


bool CTreeFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  setIcon(QPixmap((const char**) Lava));
  QSize sz;
//  if (oldWindowState != QEvent::ShowMaximized) {
    sz = parentWidget()->size();
    resize(sz.width()*7/10, sz.height()*7/10);
//  }
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
    if (oldWindowState == QEvent::ShowMaximized) 
      showMaximized();
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
//  if (oldWindowState != QEvent::ShowMaximized) {
    sz = parentWidget()->size();
    resize(sz.width()*7/10, sz.height()*7/10); 
//  }
  myDoc = (CLavaBaseDoc*)doc;
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
    if (oldWindowState == QEvent::ShowMaximized) 
      showMaximized();
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

QString ToolbarWhatsThis::text(const QPoint &point) {
  QToolButton *button;

  button = (QToolButton*)toolbar->childAt(point.x(),point.y());//execView->sv->viewportToContents(point.x(),point.y(),xc,yc);
  if (!button)
    return QString::null;
  return helpTextMap[button->textLabel()];
}

void CLavaMainFrame::fillHelpMap1(ToolbarWhatsThis *tbw) {
}

void CLavaMainFrame::fillHelpMap2(ToolbarWhatsThis *tbw) {
}

void CLavaMainFrame::fillHelpMap3(ToolbarWhatsThis *tbw) {
  tbw->helpTextMap["include"] = QString("<p><a href=\"../Packages.htm#include\">Include</a>"
    " another <font color=\"red\"><b><i>Lava</i></b></font> file</p>");
  tbw->helpTextMap["package"] = QString("<p>Create a new <a href=\"../Packages.htm#packages\">package</a>"
    " (= group of declarations/implementations that belong closely together)</p>");
  tbw->helpTextMap["initiator"] = QString("<p>Create a new <a href=\"../Packages.htm#initiator\">main program</a>"
    " (= <font color=\"red\"><b><i>Lava</i></b></font> main program)</p>");
  tbw->helpTextMap["interface"] = QString("<p>Create a new <a href=\"../SepItfImpl.htm\">interface</a></p>"
    " (= the public part of a <font color=\"red\"><b><i>Lava</i></b></font> class)</p>");
  tbw->helpTextMap["implementation"] = QString("<p>Create a new <a href=\"../SepItfImpl.htm\">implementation</a></p>"
    " (= the private part of a <font color=\"red\"><b><i>Lava</i></b></font> class)</p>");
  tbw->helpTextMap["COS"] = QString("<p>Create a new <a href=\"../Components.htm\">component object</a> specification</p>");
  tbw->helpTextMap["COI"] = QString("<p>Create a new <a href=\"../Components.htm\">component object</a> implementation</p>");
  tbw->helpTextMap["set"] = QString("<p>Create a new finite <a href=\"SetChain.htm\">set</a> of <font color=\"red\"><b><i>Lava</i></b></font> objects</p>");
  tbw->helpTextMap["enumeration"] = QString("<p>Create a new <a href=\"Enumeration.htm\">enumeration type</a></p>");
  tbw->helpTextMap["enumItem"] = QString("<p>Add a new <a href=\"Enumeration.htm\">enumerated item</a> to the current enumeration</p>");
  tbw->helpTextMap["VT"] = QString("<p>Add a new <a href=\"../PatternsFrameworks.htm\">virtual type</a> to an interface or package</p>");
  tbw->helpTextMap["function"] = QString("<p>Add a new virtual or static <a href=\"../dialogs/FunctionBox.htm\">member function</a> to the current interface or implementation</p>");
  tbw->helpTextMap["memberVariable"] = QString("<p>Add a new <a href=\"../dialogs/MemVarBox.htm\">member variable</a> to the current interface or implementation</p>");
  tbw->helpTextMap["makeGUI"] = QString("<p>Generate a <a href=\"../EditForm.htm#GUI\">GUI service interface</a> from the selected interface</p>");
}

void CLavaMainFrame::fillHelpMap4(ToolbarWhatsThis *tbw) {
}

void CLavaMainFrame::fillHelpMap5(ToolbarWhatsThis *tbw) {
}

void CLavaMainFrame::fillHelpMap6(ToolbarWhatsThis *tbw) {
}
