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

#include "cmainframe.h"

#include "qpixmap.h"
#include "qpushbutton.h"
#include "qicon.h"
#include "qmainwindow.h"
#include "qmenubar.h"
#include "qmessagebox.h"
//#include "q3canvas.h"
#include "qcursor.h"
#include <QMenu>
#include "qtreewidget.h"
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
#include "qhash.h"
#include "qevent.h"
#include "qtooltip.h"
#include "QtAssistant/qassistantclient.h"
//Added by qt3to4:
#include <QActionGroup>
#include <QCloseEvent>
//#include <Q3ValueList>

#pragma hdrstop


typedef QMap<QString,QString> HelpTextMap;


CLavaMainFrame::CLavaMainFrame() : CMainFrame()
{
  setObjectName("LavaMainFrame");
  setupUi(this);

  // workaround because insertToolBarBreak is broken:
  removeToolBar(Toolbar_1);
  removeToolBar(Toolbar_2);
  removeToolBar(HelpToolbar);
  removeToolBar(Toolbar_3);
  removeToolBar(Toolbar_4);
  removeToolBar(ToolbarDbg);
  removeToolBar(Toolbar_5);
  removeToolBar(Toolbar_6);

  addToolBar(Toolbar_1);
  Toolbar_1->show();
  addToolBar(Toolbar_2);
  Toolbar_2->show();
  addToolBar(HelpToolbar);
  HelpToolbar->show();
  addToolBarBreak();
  addToolBar(Toolbar_3);
  Toolbar_3->show();
  addToolBar(Toolbar_4);
  Toolbar_4->show();
  addToolBar(ToolbarDbg);
  ToolbarDbg->show();
  addToolBarBreak();
  addToolBar(Toolbar_5);
  Toolbar_5->show();
  addToolBar(Toolbar_6);
  Toolbar_6->show();

  installToolButtonEvtFilters(Toolbar_1);
  installToolButtonEvtFilters(Toolbar_2);
  installToolButtonEvtFilters(HelpToolbar);
  installToolButtonEvtFilters(Toolbar_3);
  installToolButtonEvtFilters(Toolbar_4);
  installToolButtonEvtFilters(ToolbarDbg);
  installToolButtonEvtFilters(Toolbar_5);
  installToolButtonEvtFilters(Toolbar_6);

  theActiveFrame = 0;

  makeStyle(LBaseData->m_style);

  m_UtilityView = 0;
  splitHoriz = true;
  QMenu *styleMenu = optionMenu->addMenu("Set st&yle");
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
		QObject::connect( a,SIGNAL( triggered() ), styleMapper, SLOT(map()) );
		styleMapper->setMapping(a,a->text() );
		if (LBaseData->m_style == styleStr)
		  a->setChecked(true);
  }
  styleMenu->addActions(ag->actions());

  LBaseData->insActionPtr = insAction;
  LBaseData->delActionPtr = delAction;
  LBaseData->newFuncActionPtr = newFuncAction;
  LBaseData->toggleCatActionPtr = 0;
  LBaseData->updateResetActionPtr = 0;

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
  LBaseData->insertActionPtr = execInsAction;
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
  LBaseData->toggleClosedActionPtr = toggleClosedAction;
  LBaseData->parmNameActionPtr = parmNameAction;
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

void CLavaMainFrame::installToolButtonEvtFilters(QToolBar *tb) {
  QList<QToolButton*> buttonList = tb->findChildren<QToolButton*>();
  for (int i=0; i<buttonList.size(); i++)
    buttonList.at(i)->installEventFilter(this);
}

void CLavaMainFrame::makeStyle(const QString &style)
{
  bool isVisible, firstTime=false;

  if (!style.isEmpty()) {
    LBaseData->m_style = style;
    wxTheApp->saveSettings();
    if (style == "Windows")
      QApplication::setStyle(new MyWindowsStyle);
#ifdef WIN32
    else if (style == "WindowsXP")
      QApplication::setStyle(new MyWindowsXPStyle);
    else if (style == "WindowsVista")
      QApplication::setStyle(new MyWindowsVistaStyle);
#endif
#ifdef __Darwin
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
  }
  else {
#ifdef __Darwin
    LBaseData->m_style = "Mac";
    wxTheApp->saveSettings();
    QApplication::setStyle(new MyMacStyle);
#else
    LBaseData->m_style = "Windows";
    wxTheApp->saveSettings();
    QApplication::setStyle(new MyWindowsStyle);
#endif
  }

  isVisible = Toolbar_7->isVisible();
  if (!LBaseData->declareButton) {
    firstTime = true;
    fillKwdToolbar(Toolbar_7);
  }
  Toolbar_7->setAllowedAreas(Qt::RightToolBarArea|Qt::LeftToolBarArea);
  int w = Toolbar_7->width();
  Toolbar_7->setContentsMargins(0,0,0,0);

  if (!firstTime) {
    delete LBaseData->myWhatsThisAction;
  }
  fillHelpToolbar(HelpToolbar);

  wxTheApp->updateButtonsMenus();
  //if (theActiveFrame && QString(theActiveFrame->objectName()) != "ExecFrame")
  //  CExecView::DisableKwdButtons();
}

CLavaMainFrame::~CLavaMainFrame()
{
}

bool CLavaMainFrame::OnCreate()
{
  wxHistory *fileHist = wxDocManager::GetDocumentManager()->m_fileHistory;
  DString fn;

  //m_childFrameHistory->m_menu = windowMenu;
  fileHist->m_menu = ((CLavaMainFrame*)wxTheApp->m_appWindow)->fileMenu;
  LoadFileHistory();
  for (int i = 0; i < fileHist->m_historyN; i++)
  {
    fn = *fileHist->m_history[i];
    if (fn.Substr(fn.l-5,5) == DString(".ldoc"))
      fileHist->m_actions[i]->setEnabled(false);
  }
  setWindowIcon(QPixmap((const char**) Lava));
  QSplitter* split = new QSplitter(this);
  m_CentralWidget = split;
  setCentralWidget(m_CentralWidget);
  split->setOrientation(Qt::Vertical);
  CreateWorkspace(split);
  split->addWidget(m_ClientArea);
  m_UtilityView = new CUtilityView(0);
  split->addWidget(m_UtilityView);
  QList<int> list=split->sizes();
  list.replace(0,700);//int(totalW * 0.7));
  list.replace(1,300);//int(totalW * 0.3));
  split->setSizes(list);
  m_UtilityView->hide();
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
  saveEveryChangeAction->setChecked(LBaseData->m_saveEveryChange);
  viewTB1Action->setChecked(Toolbar_1->isVisible());
  viewTB2Action->setChecked(Toolbar_2->isVisible());
  viewHelpTBAction->setChecked(HelpToolbar->isVisible());
  viewTB3Action->setChecked(Toolbar_3->isVisible());
  viewTB4Action->setChecked(Toolbar_4->isVisible());
  viewTB5Action->setChecked(Toolbar_5->isVisible());
  viewTB6Action->setChecked(Toolbar_6->isVisible());
  viewTB7Action->setChecked(Toolbar_7->isVisible());
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

void CLavaMainFrame::newKwdToolbutton(QToolBar *tb,QToolButton *&pb,const char *text,const char *slotParm,QString tooltip,QString whatsThis)
{
  QFont f;

  pb = new QToolButton(tb);
  pb->setText(QString(text));

  tb->addWidget(pb);
  connect(pb,SIGNAL(clicked()),slotParm);
  f = pb->font();
  f.setBold(true);
  pb->setFont(f);

  pb->setToolButtonStyle(Qt::ToolButtonTextOnly);
  pb->setMinimumHeight(pb->fontInfo().pixelSize());
  pb->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
  if (!tooltip.isEmpty()) {
    pb->setToolTip(tooltip);
    pb->setStatusTip(tooltip);
  }
  if (!whatsThis.isEmpty())
    pb->setWhatsThis(whatsThis);
  pb->installEventFilter(this);
}

void CLavaMainFrame::newHelpToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,char *tooltip,char *whatsThis)
{
  QFont f;

  pb = new QPushButton(QString(text),tb);
  connect(pb,SIGNAL(clicked()),slotParm);
  f = pb->font();
  f.setBold(true);
  pb->setFont(f);
  QPalette palette=pb->palette();
  palette.setColor(QPalette::WindowText,Qt::blue);
  pb->setPalette(palette);
  pb->setAutoDefault(false);
  if (tooltip)
    pb->setToolTip(tooltip);
  pb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  pb->show();
}

void CLavaMainFrame::fillHelpToolbar(QToolBar *tb)
{
  /*
  newHelpToolbutton(tb,LBaseData->whatNextButton,"What next?",SLOT(on_whatNext_clicked()),
    "What can I do next at the current selection?",
    "<p>Provides online help which lists the most important operations "
    "that you can carry out <b>at the current selection</b></p>");
*/
  LBaseData->myWhatsThisAction = QWhatsThis::createAction(HelpToolbar);
  HelpToolbar->addAction(LBaseData->myWhatsThisAction);
  LBaseData->myWhatsThisAction->setWhatsThis("<p>Drag the \"What's this?\" cursor to any user interface object"
    " and drop it there to see a <b>little popup info (but usually more than a tooltip)</b> on that object.</p>");
  LBaseData->myWhatsThisAction->setToolTip("Enter \"WhatsThis help\" mode");
}

void CLavaMainFrame::fillKwdToolbar(QToolBar *tb)
{
  QPalette palette=tb->palette();
  palette.setColor(QPalette::Active,QPalette::ButtonText,Qt::blue);
  tb->setPalette(palette);

  newKwdToolbutton(tb,LBaseData->declareButton,"&declare",SLOT(declare()),
    QObject::tr("Declare local variables: \"d\""),
    QObject::tr("<p><a href=\"Declare.htm\">declare</a> local variables</p>"));
  newKwdToolbutton(tb,LBaseData->existsButton,"&exists",SLOT(exists()),
    QObject::tr("Existential quantifier: \"e\""),
    QObject::tr("<p><a href=\"Exists.htm\">Existential quantifier</a> ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->foreachButton,"&foreach",SLOT(foreachStm()),
    QObject::tr("Universal quantifier: \"f\""),
    QObject::tr("<p><a href=\"Foreach.htm\">Universal quantifier</a> ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->selectButton,"se&lect",SLOT(select()),
    QObject::tr("Select elements from set(s) and add derived new elements to a given set: \"l\""),
    QObject::tr("<p><a href=\"Select.htm\">Select</a> quantifier ranging \nover a finite set</p>"));
  newKwdToolbutton(tb,LBaseData->elInSetButton,"el. in set",SLOT(elInSet()),
    QObject::tr("Test if element is contained in set"),
    QObject::tr("<p><a href=\"InSet.htm\">Element in set</a> test</p>"));

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->setButton,"&set",SLOT(set()),
    QObject::tr("Assignment statement: \"s\""),
    QObject::tr("<p><a href=\"Assign.htm\">Assignment statement</a></p>"));
  newKwdToolbutton(tb,LBaseData->copyButton,"&copy",SLOT(copy()),
    QObject::tr("Copy an object onto another object: \"c\""),
    QObject::tr("<p><a href=\"Copy.htm\">Copy</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object onto another object</p>"));
  newKwdToolbutton(tb,LBaseData->ifButton,"&if",SLOT(ifStm()),
    QObject::tr("If-then-else statement: \"i\""),
    QObject::tr("<p><a href=\"IfStm.htm\">if-then-elsif-else</a> statement with optional\nelif and else branches</p>"));
  newKwdToolbutton(tb,LBaseData->ifdefButton,"&ifdef",SLOT(ifdefStm()),
    QObject::tr("Ifdef statement"),
    QObject::tr("<p><a href=\"IfdefElse.htm\">ifdef</a> statement: if variable != null then ...</p>"));
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
  newKwdToolbutton(tb,LBaseData->assertButton,"assert",SLOT(assert_stm()),
    QObject::tr("Assertion"),
    QObject::tr("<p>An <a href=\"Assert.htm\">embedded assertion</a> is embedded anywhwere in executable code"
    " (in contrast to <a href=\"../DBC.htm\">attached assertions</a>)"
    " and throws a specific exception in case of violation</p>"));
  newKwdToolbutton(tb,LBaseData->ignoreButton,"ignore",SLOT(ignore_stm()),
    QObject::tr("Consciously ignore certain mandatory input parameters (must be the first statement!)"),
    QObject::tr("<p>Consciously ignore certain mandatory input parameters "
    "(which would have to be used otherwise!). </p>"
    "\"ignore\" must be the first statement after the function header"));
  newKwdToolbutton(tb,LBaseData->tryButton,"tr&y",SLOT(try_stm()),
    QObject::tr("Try a statement, catch exceptions: \"y\""),
    QObject::tr("<p><a href=\"Try.htm\">Try</a> a statement, catch exceptions</p>"));
  newKwdToolbutton(tb,LBaseData->failButton,"fail",SLOT(throwEx()),
    QObject::tr("Fail or even throw an exception"),
    QObject::tr("<p><a href=\"FailSucceed.htm\">Throw</a>: immediate negative/unsuccessful return from an <a href=\"../EditExec.htm#\">exec</a>,"
    " optionally throw an <a href=\"../ExceptionSamples.htm\">exception</a></p>"));
  newKwdToolbutton(tb,LBaseData->succeedButton,"succeed",SLOT(succeed()),
    QObject::tr("Immediate affirmative/successful return"),
    QObject::tr("<p><a href=\"FailSucceed.htm\">Succeed</a>: immediate affirmative/successful return from an <a href=\"../EditExec.htm\">exec</a></p>"));
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

  tb->addSeparator();

  newKwdToolbutton(tb,LBaseData->newButton,"&new",SLOT(newExpr()),
    QObject::tr("Create a new object: \"n\""),
    QObject::tr("<p>Create a <a href=\"New.htm\">new</a> object</p>"));
  newKwdToolbutton(tb,LBaseData->oldButton,"old",SLOT(old()),
    QObject::tr("Old value (on entry to function)"),
    QObject::tr("<p><a href=\"Old.htm\">Old</a> value of a variable or expression (on entry to function)</p>"));
  newKwdToolbutton(tb,LBaseData->cloneButton,"clone",SLOT(clone()),
    QObject::tr("Clone an object"),
    QObject::tr("<p><a href=\"Clone.htm\">Clone</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object</p>"));
  newKwdToolbutton(tb,LBaseData->elsexButton,"else-expr",SLOT(elseExpr()),
    QObject::tr("else expression"),
    QObject::tr("<p>\"x <a href=\"IfdefElse.htm\">else</a> y\" yields x if x != null, else y</p>"));
  newKwdToolbutton(tb,LBaseData->ifxButton,"if-expr",SLOT(ifExpr()),
    QObject::tr("If-then-else expression: \"x\""),
    QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelif and else branches</p>"));
  newKwdToolbutton(tb,LBaseData->scaleButton,"scale",SLOT(scale()),
    QObject::tr("Add a scale (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters"),
    QObject::tr("<p>Add a <a href=\"Scale.htm\">scale</a> (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters</p>"));
  newKwdToolbutton(tb,LBaseData->itemButton,"item",SLOT(getEnumItem()),
    QObject::tr("Get an enumeration item from its index"),
    QObject::tr("<p>Get an enumeration <a href=\"EnumItem.htm\">item</a> from its index</p>"));
  newKwdToolbutton(tb,LBaseData->attachButton,"attach",SLOT(attach()),
    QObject::tr("Attach a component object through an interface"),
    QObject::tr("<p><a href=\"Attach.htm\">Attach</a> an existing <b><i><font color=\"red\">Lava</font></i></b> component object through one of its interfaces</p>"));
  newKwdToolbutton(tb,LBaseData->qryItfButton,"qry itf",SLOT(qryItf()),
    QObject::tr("Query interface: get another interface from an already known one"),
    QObject::tr("<p><a href=\"QryItf.htm\">Query interface:</a> Make another interface of a component object accessible, starting from a known interface</p>"));
}

/////////////////////////////////////////////////////////////////////////////
// handlers:

void CLavaMainFrame::on_fileNewAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnFileNew();
}

void CLavaMainFrame::on_fileOpenAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnFileOpen();
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
  ((CLavaPEApp*)wxTheApp)->OnSaveAll();
}

void CLavaMainFrame::on_fileCloseAction_triggered()
{
  wxDocManager::GetDocumentManager()->OnFileClose();
}

void CLavaMainFrame::on_editUndoAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnEditUndo();
}

void CLavaMainFrame::on_editRedoAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnEditRedo();
}


void CLavaMainFrame::on_newCompoAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnNewComponent();
}

void CLavaMainFrame::on_importComponentAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnImport();
}

void CLavaMainFrame::on_returnToRefAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnPopcontext();
}

void CLavaMainFrame::on_findByNameAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnFindByName();
}

void CLavaMainFrame::on_runAction_triggered()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnRunLava();
}

void CLavaMainFrame::on_checkAction_triggered()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnCheck();
}

void CLavaMainFrame::on_checkAllAction_triggered()
{
  ((CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument())->OnTotalCheck();
}

void CLavaMainFrame::on_helpContentsAction_triggered()
{
	((CLavaPEApp*)wxTheApp)->HtmlHelp();
}

void CLavaMainFrame::on_editingLavaProgsAction_triggered()
{
	((CLavaPEApp*)wxTheApp)->EditingLavaProgs();
}

void CLavaMainFrame::on_learningLavaAction_triggered()
{
	((CLavaPEApp*)wxTheApp)->LearningLava();
}

void CLavaMainFrame::on_helpAboutAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnAppAbout();
}

void CLavaMainFrame::filePrint(){}

void CLavaMainFrame::on_setTreeFontAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseTreeFont();
}

void CLavaMainFrame::on_setFormTextFontAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseFormFont(0);
}

void CLavaMainFrame::on_setFormLabelFontAction_triggered()
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

void CLavaMainFrame::on_setExecFontAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseExecFont();
}

void CLavaMainFrame::on_setGlobalFontAction_triggered()
{
  ((CLavaPEApp*)wxTheApp)->OnChooseGlobalFont();
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

void CLavaMainFrame::on_findRefsAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFindReferences();
}

void CLavaMainFrame::on_expandAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view) {
    wxTheApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    view->OnExpandAll();
    wxTheApp->restoreOverrideCursor();
  }
}

void CLavaMainFrame::on_collapseAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnCollapseAll();
}

void CLavaMainFrame::on_showOptsAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowOptionals();
}

void CLavaMainFrame::on_showAllOptsAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view) {
    wxTheApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    view->OnShowAllEmptyOpt();
    wxTheApp->restoreOverrideCursor();
  }
}

void CLavaMainFrame::on_hideEmptyOptsAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view) {
    wxTheApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    view->OnRemoveAllEmptyOpt();
    wxTheApp->restoreOverrideCursor();
  }
}

void CLavaMainFrame::on_nextCommentAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNextComment();
}

void CLavaMainFrame::on_prevCommentAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPrevComment();
}

void CLavaMainFrame::on_nextErrorAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNextError();
}

void CLavaMainFrame::on_prevErrorAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPrevError();
}
void CLavaMainFrame::on_newIncludeAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewInclude();
}

void CLavaMainFrame::on_newPackageAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewPackage();
}

void CLavaMainFrame::on_newInitiatorAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewinitiator();
}

void CLavaMainFrame::on_newInterfaceAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewInterface();
}

void CLavaMainFrame::on_newImplAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewImpl();
}

void CLavaMainFrame::on_newCOspecAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewCSpec();
}

void CLavaMainFrame::on_newCOimplAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewComponent();
}

void CLavaMainFrame::on_newSetAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewset();
}

void CLavaMainFrame::on_newEnumAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewenum();
}

void CLavaMainFrame::on_newVTAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewVirtualType();
}

void CLavaMainFrame::on_newFuncAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewfunction();
}

void CLavaMainFrame::on_newMemVarAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewmember();
}

void CLavaMainFrame::on_newEnumItemAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewEnumItem();
}

void CLavaMainFrame::on_editSelItemAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEditSel();
}

void CLavaMainFrame::on_editCommentAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnComment();
}

void CLavaMainFrame::on_gotoDeclAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGotoDecl();
}

void CLavaMainFrame::on_gotoImplAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGotoImpl();
}

void CLavaMainFrame::on_showOverridablesAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowOverridables();
}

void CLavaMainFrame::on_openFormViewAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowGUIview();
}

void CLavaMainFrame::on_makeGUIAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnMakeGUI();
}

void CLavaMainFrame::on_insertTextAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewLitStr();
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


void CLavaMainFrame::on_deleteAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDelete();
}

void CLavaMainFrame::on_overrideAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOverride();
}


void CLavaMainFrame::on_DbgAction_triggered()
{
  if (((CLavaPEDebugger*)LBaseData->debugger)->isConnected) {
    DbgMessage* mess = new DbgMessage(Dbg_Continue);
    QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
  }
  else {
    CLavaPEDoc* doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->GetActiveDocument();
    doc->OnDebugLava();
  }
}


void CLavaMainFrame::on_DbgClearBreakpointsAct_triggered()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ClearBrkPnts = true;
  ((CLavaPEDebugger*)LBaseData->debugger)->clearBrkPnts();
}

void CLavaMainFrame::on_DbgBreakpointAct_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->on_DbgBreakpointAct_triggered();
}

void CLavaMainFrame::on_DbgStepNextAct_triggered()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_Step;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::on_DbgStepNextFunctionAct_triggered()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepFunc;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::on_DbgStepintoAct_triggered()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepInto;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::on_DbgStepoutAct_triggered()
{
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_StepOut;
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
}

void CLavaMainFrame::on_DbgStopAction_triggered()
{
  ((CLavaPEDebugger*)LBaseData->debugger)->stop(otherError);
}

void CLavaMainFrame::on_DbgRunToSelAct_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->on_DbgRunToSelAct_triggered();

}


void CLavaMainFrame::on_genLinkedHtmlAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    ((CLavaPEView*)((CPEBaseDoc*)view->GetDocument())->MainView)->OnGenHtml();
}

void CLavaMainFrame::on_genSingleHtmlAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    ((CLavaPEView*)((CPEBaseDoc*)view->GetDocument())->MainView)->OnGenHtmlS();
}


//////////////////////////////////////////////////////////
// Exec handlers (tool buttons):

void CLavaMainFrame::on_toggleCategoryAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleCategory();
}

void CLavaMainFrame::on_optLocalVarAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnOptLocalVar();
}

void CLavaMainFrame::on_toggleSubstTypeAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleSubstitutable();
}

void CLavaMainFrame::on_toggleClosedAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleClosed();
}

void CLavaMainFrame::on_parmNameAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleParmNames();
}

void CLavaMainFrame::on_insertBeforeAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsertBefore();
}

void CLavaMainFrame::on_execInsAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInsert();
}

void CLavaMainFrame::on_toggleArrowsAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnToggleArrows();
}

void CLavaMainFrame::on_toggleInputArrowsAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInputArrow();
}

void CLavaMainFrame::on_conflictingAssigAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnConflict();
}

void CLavaMainFrame::on_newLineAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNewLine();
}

void CLavaMainFrame::on_commentOutAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIgnore();
}

void CLavaMainFrame::on_toggleCommentsAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnShowComments();
}

void CLavaMainFrame::on_lessThanAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLt();
}

void CLavaMainFrame::on_lessEqualAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLe();
}

void CLavaMainFrame::on_equalAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEq();
}

void CLavaMainFrame::on_notEqualAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNe();
}

void CLavaMainFrame::on_greaterEqualAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGe();
}

void CLavaMainFrame::on_greaterThanAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnGt();
}

void CLavaMainFrame::on_functionCallAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFunctionCall();
}

void CLavaMainFrame::on_staticCallAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnStaticCall();
}

void CLavaMainFrame::on_trueAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnTrue();
}

void CLavaMainFrame::on_falseAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnFalse();
}

void CLavaMainFrame::on_evaluateAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnEvaluate();
}

void CLavaMainFrame::on_nullAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnNull();
}

void CLavaMainFrame::on_intervalAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInterval();
}

void CLavaMainFrame::on_toggleSignAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPlusMinus();
}

void CLavaMainFrame::on_additionAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnPlus();
}

void CLavaMainFrame::on_multiplicationAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnMult();
}

void CLavaMainFrame::on_divideAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnDivide();
}

void CLavaMainFrame::on_modulusAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnModulus();
}

void CLavaMainFrame::on_leftShiftAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnLshift();
}

void CLavaMainFrame::on_rightShiftAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnRshift();
}

void CLavaMainFrame::on_bitAndAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitAnd();
}

void CLavaMainFrame::on_bitOrAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitOr();
}

void CLavaMainFrame::on_bitXorAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnBitXor();
}

void CLavaMainFrame::on_invertAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnInvert();
}

void CLavaMainFrame::on_handleAction_triggered(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnHandle();
}

void CLavaMainFrame::on_ordAction_triggered(){
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

void CLavaMainFrame::foreachStm(){
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

void CLavaMainFrame::ifdefStm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIfdef();
}

void CLavaMainFrame::ifExpr(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIfExpr();
}

void CLavaMainFrame::elseExpr(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnElseExpr();
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

void CLavaMainFrame::ignore_stm(){
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  if (view)
    view->OnIgnoreStm();
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

void CLavaMainFrame::throwEx(){
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

void CLavaMainFrame::getEnumItem(){
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


void CLavaMainFrame::on_splitVerticAction_triggered()
{
  if (!splitHoriz) {
		splitVerticAction->setChecked(true);
    equalize();
    return;
  }
  splitHoriz = !splitHoriz;
	switch (splitHoriz) {
	case true:
    m_ClientArea ->setOrientation(Qt::Horizontal);
		splitHorizAction->setChecked(true);
		splitVerticAction->setChecked(false);
		break;
	case false:
    m_ClientArea ->setOrientation(Qt::Vertical);
		splitHorizAction->setChecked(false);
		splitVerticAction->setChecked(true);
	}
}

void CLavaMainFrame::on_splitHorizAction_triggered()
{
  if (splitHoriz) {
		splitHorizAction->setChecked(true);
    equalize();
    return;
  }
  splitHoriz = !splitHoriz;
	switch (splitHoriz) {
	case true:
    m_ClientArea ->setOrientation(Qt::Horizontal);
		splitHorizAction->setChecked(true);
		splitVerticAction->setChecked(false);
		break;
	case false:
    m_ClientArea ->setOrientation(Qt::Vertical);
		splitHorizAction->setChecked(false);
		splitVerticAction->setChecked(true);
	}
}

void CLavaMainFrame::customEvent(QEvent *ev0){
	HistWindow *hw;
	DString title;
  CustomEvent *ev=(CustomEvent*)ev0;

  if (ev->data()) {
    if ( !((CLavaPEApp*)wxTheApp)->inTotalCheck) { //to prevent crash if mdi-frame already closed
		  title = DString(qPrintable(((QWidget*)ev->data())->windowTitle()));
		  if (title.l) {
			  if (title[title.l-1] == '*')
				  title = title.Substr(0,title.l-1);
			  hw = new HistWindow(title,(wxChildFrame*)ev->data());
			  m_childFrameHistory->AddToHistory(hw,this);
		  }
    }
  }
	else
    splitHoriz = !splitHoriz;
		switch (splitHoriz) {
		case true:
			splitHorizAction->setChecked(true);
			splitVerticAction->setChecked(false);
			break;
		case false:
			splitHorizAction->setChecked(false);
			splitVerticAction->setChecked(true);
		}
}

bool CLavaMainFrame::eventFilter(QObject *obj,QEvent *ev) {
  QWhatsThisClickedEvent *wtcEv;

  if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    ShowPage(QString("whatsThis/")+wtcEv->href());
    return true;
  }
  else
    return false;
}

void CLavaMainFrame::on_showUtilWindowAction_triggered()
{
  //bool updEnabled;

  if (UtilitiesHidden) {
    if ((m_UtilityView->ActTab == tabError) && !m_UtilityView->CommentEmpty && m_UtilityView->ErrorEmpty )
      m_UtilityView->SetTab(tabComment);
    else if ((m_UtilityView->ActTab == tabComment) && m_UtilityView->CommentEmpty && !m_UtilityView->ErrorEmpty)
      m_UtilityView->SetTab(tabError);
    m_UtilityView->setCurrentIndex(m_UtilityView->ActTab);
    m_UtilityView->update();
    m_UtilityView->show();
    LastUtilitiesState = -1;
    UtilitiesHidden = false;//!UtilitiesHidden;
  }
  else {
    m_UtilityView->hide();
    LastUtilitiesState = (int)m_UtilityView->ActTab;
    //if (/*wxTheApp->isChMaximized && */wxTheApp->activeView()->inherits("CExecView")) {
    //  CExecView *ev = (CExecView*)wxTheApp->activeView();
    //  ev->redCtl->update();
      //ev->redCtl->setUpdatesEnabled(false);
      //ev->redCtl->setUpdatesEnabled(true);
    //}
    wxTheApp->updateButtonsMenus();
    UtilitiesHidden = true;
  }
}

void CLavaMainFrame::OnUpdateshowUtil(QAction* action)
{
  action->setChecked(!UtilitiesHidden);
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

void CLavaMainFrame::on_saveEveryChangeAction_triggered(bool on)
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

void CLavaMainFrame::on_viewTB1Action_triggered()
{
  if (Toolbar_1->isVisible())
    Toolbar_1->hide();
  else
    Toolbar_1->show();
}

void CLavaMainFrame::on_viewTB2Action_triggered()
{
  if (Toolbar_2->isVisible())
    Toolbar_2->hide();
  else
    Toolbar_2->show();
}

void CLavaMainFrame::on_viewHelpTBAction_triggered()
{
  if (HelpToolbar->isVisible())
    HelpToolbar->hide();
  else
    HelpToolbar->show();
}
void CLavaMainFrame::on_viewDebugTBAction_triggered()
{
  if (ToolbarDbg->isVisible())
    ToolbarDbg->hide();
  else
    ToolbarDbg->show();
}


void CLavaMainFrame::on_viewTB3Action_triggered()
{
  if (Toolbar_3->isVisible())
    Toolbar_3->hide();
  else
    Toolbar_3->show();
}


void CLavaMainFrame::on_viewTB4Action_triggered()
{
  if (Toolbar_4->isVisible())
    Toolbar_4->hide();
  else
    Toolbar_4->show();
}


void CLavaMainFrame::on_viewTB5Action_triggered()
{
  if (Toolbar_5->isVisible())
    Toolbar_5->hide();
  else
    Toolbar_5->show();
}



void CLavaMainFrame::on_viewTB6Action_triggered()
{
  if (Toolbar_6->isVisible())
    Toolbar_6->hide();
  else
    Toolbar_6->show();
}


void CLavaMainFrame::on_viewTB7Action_triggered()
{
  if (Toolbar_7->isVisible())
    Toolbar_7->hide();
  else
    Toolbar_7->show();
}

void CLavaMainFrame::on_whatNextAction_triggered()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  QString fileName=ExeDir+"/../doc/html/whatNext/GlobalWhatNext.htm";
  QString path(QASSISTANT);
  QStringList args;

  if (view)
    view->on_whatNextAction_triggered();
  else {
	  args << "-profile" << ExeDir + "/../doc/LavaPE.adp";

	  if (!qacl) {
		  qacl = new QAssistantClient(path,wxTheApp->m_appWindow);
		  qacl->setArguments(args);
	  }

	  qacl->showPage(fileName);
  }
}
/*
void CLavaMainFrame::on_howTo_clicked()
{
  CLavaBaseView* view = (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView();
  QString fileName=ExeDir+"/../doc/html/howTo/HowToContents.htm";
	QString path(ExeDir);
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
*/

CTreeFrame::CTreeFrame(QWidget* parent):wxChildFrame(parent)
{
  viewR = 0;
  showIt = !((CLavaPEApp*)wxTheApp)->inTotalCheck;
  if (!showIt) {
    ((wxTabWidget*)parent)->removeTab(((wxTabWidget*)parent)->indexOf(this));
    m_tabWidget = 0;
    hide();
  }
}

bool CTreeFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  setWindowIcon(QPixmap((const char**) Lava));
  QSize sz;
  int totalW=0;

  splitter = new QSplitter(this);//vb);
  splitter->setHandleWidth(3);
  layout->addWidget(splitter);
  layout->setMargin(0);
  splitter->setOrientation(Qt::Horizontal);
  m_clientWindow = splitter;
  viewL = new CInclView(this, doc);
  viewM = new CLavaPEView(this, doc);
  viewR = new CVTView(this, doc);
  ((CTreeView*)viewL)->Tree->header()->setResizeMode(QHeaderView::ResizeToContents);
  ((CTreeView*)viewL)->Tree->header()->setStretchLastSection(false);
  ((CTreeView*)viewM)->Tree->header()->setResizeMode(QHeaderView::ResizeToContents);
  ((CTreeView*)viewM)->Tree->header()->setStretchLastSection(false);
  ((CTreeView*)viewR)->Tree->header()->setResizeMode(QHeaderView::ResizeToContents);
  ((CTreeView*)viewR)->Tree->header()->setStretchLastSection(false);
  splitter->addWidget(viewL);
  splitter->addWidget(viewM);
  splitter->addWidget(viewR);
  viewL->setWhatsThis(QString(tr("No specific help available here")));
  viewM->setWhatsThis(QString(tr("No specific help available here")));
  viewR->setWhatsThis(QString(tr("No specific help available here")));
  if (viewM->OnCreate() && viewL->OnCreate() && viewR->OnCreate()) {
    doc->AddChildFrame(this);
    m_document = doc;
    //wxChildFrame::OnCreate(temp,doc);
    ((CInclView*)viewL)->myTree = (CTreeView*)viewM;
    ((CVTView*)viewR)->myMainView = (CLavaPEView*)viewM;
    ((CLavaPEView*)viewM)->myInclView = (CTreeView*)viewL;
    ((CLavaPEView*)viewM)->myVTView = (CTreeView*)viewR;
    doc->AddView(viewR);
    doc->AddView(viewL);
    doc->AddView(viewM);


    QList<int> list=splitter->sizes();
    splitter->resize(300,200);
    totalW = 600;
    list.replace(0,int(totalW * 0.1));
    list.replace(1,int(totalW * 0.8));
    list.replace(2,totalW - list.at(0) - list.at(1));
    splitter->setSizes(list);
    lastActive = viewM;
    //if  ((oldWindowState == Qt::WindowMaximized) && showIt)
    if  (wxTheApp->isChMaximized && showIt)
      showMaximized();
		return true;
  }
  else {
    deleteLater();
    return false;
  }
}


void CTreeFrame::InitialUpdate()

{
  if (showIt) {
    wxChildFrame::InitialUpdate();
    wxDocManager::GetDocumentManager()->SetActiveView(viewM, true);
  }
}

CTreeFrame::~CTreeFrame()
{
}

void CTreeFrame::closeEvent(QCloseEvent *e)
{
  QApplication::postEvent(viewM->GetDocument(),new CustomEvent(UEV_Close,(void*)this));
}

void CTreeFrame::Activate(bool activate, bool windowMenuAction)
{
  showIt = activate && (showIt || !((CLavaPEApp*)wxTheApp)->inTotalCheck);
  if (showIt)
    wxChildFrame::Activate(showIt,windowMenuAction);
  else
    hide();
}

void CTreeFrame::CalcSplitters(bool showVT, bool showINCL)
{
  int i, wl, wr, totalW=0, r=10;
    QList<int> list=splitter->sizes();

    for (i=0; i<3; i++)
      totalW += list.at(i);
    wl = list.at(0);
    wr = list.at(2);
    if (showINCL && (wl < 20))
      list.replace(0,int(totalW * 0.3));
    if (showVT && (wr < 20))
      list.replace(2,int(totalW * 0.3));
    list.replace(1,totalW - list.at(0) - list.at(2));
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
void CFormFrame::closeEvent(QCloseEvent *ev)
{
  ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_UtilityView->DeleteAllPageItems(tabHandler);
  CLavaGUIFrame::closeEvent(ev);
}


CFormFrame::~CFormFrame()
{
  deleting = true;
}


bool CFormFrame::OnCreate(wxDocTemplate *temp, wxDocument *doc)
{
  QSize sz;
  //if (!wxChildFrame::OnCreate(temp, doc))
  //  return false;
  doc->AddChildFrame(this);
  m_document = doc;
  DString title = CalcTitle( (LavaDECL*)((CLavaPEApp*)wxTheApp)->LBaseData.actHint->CommandData1, ((CLavaBaseDoc*)LBaseData->actHint->fromDoc)->IDTable.DocName);
  QString str(title.c);
  SetTitle(str);
  //setWindowIcon(QPixmap((const char**) Lava));
  myDoc = (CLavaBaseDoc*)doc;

  splitter = new QSplitter(this);
  splitter->setHandleWidth(3);
  layout->addWidget(splitter);
  layout->setMargin(0);
  splitter->setOrientation(Qt::Horizontal);
  m_clientWindow = splitter;

  viewL = new CLavaGUIView(this, doc);
  QSplitter* split = new QSplitter(this);
  splitter->addWidget(viewL);
  splitter->addWidget(split);
  split->setOrientation(Qt::Vertical);
  viewR = new CLavaPEView(split, doc);
  wizardView = new CWizardView(split, doc);
  split->addWidget(viewR);
  split->addWidget(wizardView);
  ((CLavaGUIView*)viewL)->myTree = (CTreeView*)viewR;
  ((CLavaPEView*)viewR)->myFormView = (CLavaGUIView*)viewL;
  if (viewL->OnCreate() && viewR->OnCreate()) {
    doc->AddView(viewR);
    doc->AddView(viewL);
    doc->AddView(wizardView);
    QList<int> list=splitter->sizes();
    int totalW = 500;
    list.replace(0,int(totalW * 0.7));
    list.replace(1,int(totalW * 0.3));
    splitter->setSizes(list);
    lastActive = viewR;
    QList<int> listH=split->sizes();
    int totalH = 300;
    listH.replace(0,int(totalH * 0.3));
    listH.replace(1,int(totalH * 0.7));
    split->setSizes(listH);
    //if (oldWindowState == Qt::WindowMaximized)
    if (wxTheApp->isChMaximized)
      showMaximized();
    else
      show();
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
  wxChildFrame::InitialUpdate();
  wxDocManager::GetDocumentManager()->SetActiveView(viewR, true);
}

void CFormFrame::CalcSplitters()
{
}
